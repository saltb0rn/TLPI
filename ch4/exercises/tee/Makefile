INCLUDES := ../../../lib
OBJS := error_functions.o

main: main.c $(OBJS)
	gcc -o $@ main.c $(addprefix $(INCLUDES)/build/, $(OBJS)) -I$(INCLUDES)

$(OBJS): $(addprefix $(INCLUDES)/, %.o): $(addprefix $(INCLUDES)/, %.c)
	cd $(INCLUDES) && make $^

.PHONY: clean
clean:
	rm -f main
