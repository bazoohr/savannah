CC = gcc
CFLAGS += -Wall

OBJ = create_initrd.o

create_initrd: $(OBJ)
	$(CC) -o $@ $(OBJ)

.PHONY: clean
.PHONY: distclean
clean:
	rm -f create_initrd *.d *.o
distclean:
	rm -f create_initrd *.d *.o initrd.img
