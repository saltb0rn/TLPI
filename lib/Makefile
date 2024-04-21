BUILD_DIR := ./build

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

ALL: $(OBJS)
$(OBJS): %.o: %.c
	mkdir -p $(BUILD_DIR)
	gcc -c -o $(BUILD_DIR)/$@ $^

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
