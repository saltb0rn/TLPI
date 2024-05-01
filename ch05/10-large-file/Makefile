INCLUDES := ../../lib
OBJS := error_functions.o

main: main.c $(OBJS)
	gcc -o $@ main.c $(addprefix $(INCLUDES)/build/, $(OBJS)) -I$(INCLUDES)

main-64: main-without-64.c $(OBJS)
	gcc -D_FILE_OFFSET_BITS=64 -o $@ main-without-64.c $(addprefix $(INCLUDES)/build/, $(OBJS)) -I$(INCLUDES)

$(OBJS): $(addprefix $(INCLUDES)/, %.o): $(addprefix $(INCLUDES)/, %.c)
	cd $(INCLUDES) && make $^

.PHONY: clean
clean:
	rm -f main
	rm -f main-64

