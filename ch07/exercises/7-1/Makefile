INCLUDES := ../../../lib
OBJS := error_functions.o get_num.o
CFLAGS := -fsanitize=address

main: main.c $(OBJS)
	gcc $(CFLAGS) -o $@ main.c $(addprefix $(INCLUDES)/build/, $(OBJS)) -I$(INCLUDES)

$(OBJS): %.o: $(addprefix $(INCLUDES)/, %.c)
	cd $(INCLUDES) && make $@

.PHONY: clean

clean:
	rm -f main
	cd $(INCLUDES) && make clean
