#include <unistd.h>
#include <signal.h>

#ifdef DEBUG
#  include <stdio.h>
#  include <stdarg.h>
#  include <string.h>
#endif

#ifndef MAX_FREE_BLK
#  define MAX_FREE_BLK (128 * 1024)
#endif

#define MAX_DEBUG_STR (1024)
#define HEADER_SIZE   (sizeof(size_t))
#define POINTER_SIZE  (sizeof(void *))

static char *_free_list = NULL;

#ifdef DEBUG
static void
debug(const char *format, ...) {
  va_list args;
  va_start(args, format);

  char prefixed_format[MAX_DEBUG_STR];
  snprintf(prefixed_format, MAX_DEBUG_STR, "[malloc] %s\n", format);

  vfprintf(stderr, prefixed_format, args);
  va_end(args);
}
#else
static void
debug(__attribute__((unused)) const char *format, ...) {}
#endif

/*
  add size information at the start of a block.

  void *ptr is the meta

 */
static void
write_size(void *base_ptr, size_t size) {
  size_t *sizep = base_ptr;
  *sizep = size;
}

/* reads the size of a block, which is located as metadata in its first bytes */
static size_t
read_size(void *base_ptr) {
  size_t *sizep = base_ptr;
  return *sizep;
}

/* Calculate and return the base address of a block given by the user to
 * the `free` function. The block metadata is "hidden" from the user in calls
 * to `malloc`, and must be recovered here*/
static void *
get_base_address(void *ptr) {
  char *sizep = ptr;
  return (sizep - HEADER_SIZE);
}

static void
set_previous_free_block(void *base_ptr, void *previous_ptr) {
  /* the previous block pointer is located right after the size information */
  char *p = base_ptr;
  void **previous_block_info;

  p += HEADER_SIZE;
  previous_block_info = (void **) p;

  *previous_block_info = previous_ptr;
}

static void *
get_previous_free_block(void *base_ptr) {
  char *p = base_ptr;
  void **previous_block_info;

  p += HEADER_SIZE;
  previous_block_info = (void **)p;

  return *previous_block_info;
}

static void
set_next_free_block(void *base_ptr, void *next_ptr) {
  char *p = base_ptr;
  void **next_block_info;

  p += 2 * HEADER_SIZE;
  next_block_info = (void **) p;

  *next_block_info = next_ptr;
}

static void *
get_next_free_block(void *base_ptr) {
  char *p = base_ptr;
  void **next_block_info;

  p += 2 * HEADER_SIZE;
  next_block_info = (void **) p;

  return *next_block_info;
}

static void
print_free_list() {
  #ifdef DEBUG
  void *p = _free_list;
  char list[MAX_DEBUG_STR], node[MAX_DEBUG_STR];

  snprintf(list, MAX_DEBUG_STR, "FL:");

  while (p) {
    strncat(list, " -> ", MAX_DEBUG_STR);
    snprintf(node, MAX_DEBUG_STR, "[%p S=%ld P=%p N=%p]",
             p, read_size(p), get_previous_free_block(p), get_next_free_block(p));
    strncat(list, node, MAX_DEBUG_STR);

    p = get_next_free_block(p);
  }

  debug(list);
  #endif
}

/* Attempt to slice a block of memory pointed `by base_ptr`, returning a
 * block of `size` bytes. The memory block is required to be larger than
 * requested size. All links in the free list are maintained and the block
 * of memory returned already skips size information */
static void *
slice(void *base_ptr, size_t size) {
  void *previous_ptr = get_previous_free_block(base_ptr),
    *next_ptr = get_next_free_block(base_ptr);

  size_t original_size = read_size(base_ptr);
  if (original_size <= size) {
    return NULL;
  }

  char *p = base_ptr;
  char *base = base_ptr;
  p += HEADER_SIZE + size;
  if (previous_ptr) {
    set_next_free_block(previous_ptr, p);
  } else {
    _free_list = p;
  }

  if (next_ptr) {
    set_previous_free_block(next_ptr, p);
  }

  write_size(base_ptr, size);
  write_size(p, original_size - size - HEADER_SIZE);
  set_previous_free_block(p, previous_ptr);
  set_next_free_block(p, next_ptr);

  return (base + HEADER_SIZE);
}

/* the last free block is the one whose end is adjacent to the current program
 * break. When expanding the program break, it is useful to take into account
 * what is the condition of this block */
static void *
last_free_block() {
  void *curr, *prev;
  curr = _free_list;

  while (curr) {
    prev = curr;
    curr = get_next_free_block(prev);
  }

  return prev;
}

/* returns the address of the last position of the given memory block */
static void *
end_address(void *base_ptr) {
  char *p = base_ptr;
  return (p + HEADER_SIZE + read_size(base_ptr));
}

/* checkes if the two given memory block pointers are continuous */
static int
continuous(void *base_ptr1, void *base_ptr2) {
  return end_address(base_ptr1) == base_ptr2;
}

/* coalesces two memory blocks (assumed to be continuous) and updates related
 * metadata */
static void
coalesce(void *base_ptr1, void *base_ptr2) {
  size_t new_size = read_size(base_ptr1) + HEADER_SIZE + read_size(base_ptr2);
  write_size(base_ptr1, new_size);
}

/* checks if the last free block in the free memory list is larger than the
 * allowed `MAX_FREE_BLK`, in which case memory is given back to the system,
 * reducing the process' memory footprint */
static void
check_footprint() {
  void *last = last_free_block(),
    *prev = get_previous_free_block(last);

  size_t last_size = read_size(last);

  if (last_size >= MAX_FREE_BLK) {
    if (prev)
      set_next_free_block(prev, NULL);
    sbrk(-1 * (last_size + HEADER_SIZE));
  }
}

void*
_malloc(size_t size) {
  char *prev_breakp;

  /* SUSv3 allows an implementation to return either NULL or a small
   * memory block in this situation. We follow the latter, which is
   * the behavior implemented to Linux */
  if (0 == size)
    size = 1;

  debug("Malloc request of size %ld", (long) size);

  if (!_free_list) {
    debug("No free list found, creating one of size %ld", (long) 2 * size);

    /* if there is no free list (i.e., the first call of the function
     * in the process file), we allocate twice as much memory as
     * requested as an attempt to avoid further system calls */
    prev_breakp = sbrk(2 * size + HEADER_SIZE);
    if ((void *)-1 == prev_breakp)
      return NULL;

    _free_list = prev_breakp;
    write_size(_free_list, 2 * size);
    set_previous_free_block(_free_list, NULL);
    set_next_free_block(_free_list, NULL);

  }

  print_free_list();

  /* look for a suitable free block of memory to be used. Note that, for
   * simplicity sake, the search uses a first-fit fashion algorithm, returning
   * the first block for memory large enough to fulfil the request. */
  char *p = _free_list;
  while (p) {
    if (read_size(p) > size + HEADER_SIZE) {
      return slice(p, size);
    }
    p = get_next_free_block(p);
  }

  /* if we go to this point, it means there was no large enough memory blocks
   * that could fulfil the request. In this case, we move the program break,
   * increasing the memory footprint of the process */
  void *last = last_free_block();
  size_t last_size = read_size(last),
    break_increase = 2 * size + HEADER_SIZE;

  debug("No large enough free block, expanding program break by %ld bytes.", (long) break_increase);
  if (sbrk(break_increase) == (void *) -1) {
    debug("Fail to increase program break");
    return NULL;
  }

  write_size(last, last_size + break_increase);
  /* size doesn't equal (last_size + break_increase) */
  return slice(last, size);
  /* call slice here is to create links to prev and next free block */
}

void
_free(void *ptr) {
  /* SUSv3 allows the pointer given to `free` to NULL, in which case
   * nothing should be done */
  if (!ptr)
    return;

  /* if the free list was not created yet, this means that memory block
   * passed to this function was not obtained through malloc, and indicates
   * memory corruption. We indicate that by sending the current process
   * a SIGSEGV signal */
  if (!_free_list) {
    debug("Memory block not allocated by malloc");
    kill(getgid(), SIGSEGV);
  }

  void *base_address = get_base_address(ptr), *p;
  size_t blk_size = read_size(base_address);

  debug("Free request for block of the size %ld", (long) blk_size);
  print_free_list();

  /* we must find where to insert the given memory block in the existing
  * free list, keeping in mind that the list should be in the same ordered
  * that the actual memory is (virtual memory-wise, of course). */
  void *curr = _free_list, *prev = NULL;
  while (curr && curr < base_address) {
    /* keep in mind that the memory block to be freed has not informations
     * about previous and next free blocks, you cannot find the memory block
     * through the previous or next free blocks, the only way to locate the
     * our memory block is to compare the addresses of the neighboring blocks. */
    prev = curr;
    curr = get_next_free_block(prev);
  }

  /* here, we either got to the end of the free list, or found out the
   * blocks between which the new block should be inserted */
  if (prev) {
    if (curr) {
      /* prev -> base_address -> curr */
      debug("Freed block being inserted in the middle of the free list");

      if (continuous(prev, base_address)) {
        /* base_address extends the prev to be larger, maybe base_address and curr are also continuous,
         * the relation between prev and curr is untouched */
        /*  |<------|
         *          |<--------------|
         *  prev -> base_address -> curr
         *          |-------------->|
         *  |------>|
         *
         *  ==>
         *
         *  |<----------------------|
         * (prev + base_address) -> curr
         *  |---------------------->|
         */
        coalesce(prev, base_address);
      } else if (continuous(base_address, curr)) {
        /* |<------|               |<------|
         *         |<--------------|
         * prev -> base_address -> curr -> next_of_curr
         *         |-------------->|
         * |------>|               |------>|
         *
         *  ==>
         *
         * |<-------|
         *          |<----------------------|
         * prev -> (base_address + curr) -> next_of_curr
         *          |---------------------->|
         * |------->|
         */
        coalesce(base_address, curr);

        set_next_free_block(prev, base_address);
        set_previous_free_block(base_address, prev);
        set_next_free_block(base_address, get_next_free_block(curr));

        /* next free block of curr may be NULL, that is curr may be the last free block */
        if (get_next_free_block(curr)) {
          set_previous_free_block(get_next_free_block(curr), base_address);
        }

        check_footprint();
      } else {
        set_next_free_block(prev, base_address);
        set_previous_free_block(curr, base_address);

        set_next_free_block(base_address, prev);
        set_previous_free_block(base_address, curr);
      }
    } else {
      /* prev -> base_address */
      debug("Appending freed block at the end of the list");
      if (continuous(prev, base_address)) {
        coalesce(prev, base_address);
        check_footprint();
      } else {
        set_next_free_block(prev, base_address);
        set_previous_free_block(base_address, prev);
        set_next_free_block(base_address, NULL);
      }
    }
  } else {
    /* free_list = base_address -> curr */
    debug("Freed block should be the new free list head pointer");
    if (continuous(base_address, curr)) {
      debug("actually joined with current head");
      coalesce(base_address, curr);

      p = get_next_free_block(curr);
      if (p)
        set_previous_free_block(p, base_address);

      set_previous_free_block(base_address, NULL);
      set_next_free_block(base_address, p);

      check_footprint();
    } else {
      set_previous_free_block(curr, base_address);
      set_next_free_block(base_address, curr);
      set_previous_free_block(base_address, NULL);
    }

    _free_list = base_address;
  }
}

#include "tlpi_hdr.h"
#define MAX_ALLOCS 1000000

void simple_free_test() {
        /* do some mallocs, free it all, do the mallocs again (sbrk should not move) */
        int i, j, len;
        char * bufs[100];
        void * cur_brk;
        for (i = 0; i < 100; i++) {
                len = 100 + i;
                bufs[i] = _malloc(len);
                for (j = 0; j < len - 1; j++) {
                        bufs[i][j] = '0' + (i % 10);
                }
                bufs[i][len - 1] = '\0';
        }
        for (i = 0; i < 100; i++) {
                printf("%02d [0x%08lX]: %s\n", i, (unsigned long)bufs[i], (char *)bufs[i]);
        }
        cur_brk = sbrk(0);
        for (i = 0; i < 100; i++) {
                _free(bufs[i]);
        }
        for (i = 0; i < 100; i++) {
                len = 100 + i;
                bufs[i] = _malloc(len);
                for (j = 0; j < len - 1; j++) {
                        bufs[i][j] = '0' + (i % 10);
                }
                bufs[i][len - 1] = '\0';
        }
        printf("== sbrk(0): bfr 0x%08lX, after 0x%08lX ==\n",
                        (unsigned long)cur_brk,
                        (unsigned long)sbrk(0));
        for (i = 0; i < 100; i++) {
                printf("%02d [0x%08lX]: %s\n", i, (unsigned long)bufs[i], (char *)bufs[i]);
        }

}

int
main(int argc, char* argv[]) {
  /* test case here */
  simple_free_test();

  return 0;
}
