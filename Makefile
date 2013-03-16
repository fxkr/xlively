CC=gcc
CFLAGS=-std=c99 -Wall -O2 -finline-functions
LIBS=-lm -lX11 -lpthread -lpopt

DEPS=
OBJ=config.o model.o view.o ctrl.o

%.o: %.c %.h $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

xlively: main.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

benchmark: benchmark.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ core xlively benchmark

