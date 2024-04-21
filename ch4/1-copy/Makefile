INCLUDES := ../../lib
OBJS := error_functions.o

main: copy.c $(OBJS)
	gcc -o $@ copy.c $(addprefix $(INCLUDES)/build/, $(OBJS)) -I$(INCLUDES)

$(OBJS): $(addprefix $(INCLUDES)/, %.o): $(addprefix $(INCLUDES)/, %.c)
	cd $(INCLUDES) && make $^

.PHONY: clean
clean: main
	rm -f main
	cd $(INCLUDES) && make clean
