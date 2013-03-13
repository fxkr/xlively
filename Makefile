CC=gcc
CFLAGS=-std=c99 -Wall
LIBS=-lm -lX11 -lpthread

DEPS=
OBJ=main.o model.o view.o ctrl.o

%.o: %.c %.h $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

xlively: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ core xlively

