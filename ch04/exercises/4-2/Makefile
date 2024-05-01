INCLUDES := ../../../lib
OBJS := error_functions.o

ALL: main create-file-with-holes

main: main.c $(OBJS)
	gcc -o $@ main.c $(addprefix $(INCLUDES)/build/, $(OBJS)) -I$(INCLUDES)

$(OBJS): %.o: $(addprefix $(INCLUDES)/,%.c)
	cd $(INCLUDES) && make $@

create-file-with-holes: create-file-with-holes.c
	gcc -o $@ $^

.PHONY: clean
clean:
	rm -f main create-file-with-holes
	cd $(INCLUDES) && make clean
