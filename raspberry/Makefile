CC=gcc
CFLAGS=-I. -fPIC
DEPS = wiringPi.h
OBJ = src/walle.o
LIBS=-lwiringPi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -shared -o src/libwalle.so $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o main *.so