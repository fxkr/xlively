CC=gcc
CFLAGS=-std=c99 -Wall
LIBS=-lm -lX11 -lpthread -lpopt

DEPS=
OBJ=main.o config.o model.o view.o ctrl.o

%.o: %.c %.h $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

xlively: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ core xlively

