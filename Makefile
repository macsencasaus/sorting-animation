CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb `pkg-config --cflags sdl2`
LIBS=`pkg-config --libs sdl2` -lm

sa: main.c
	$(CC) $(CFLAGS) -o sa ./main.c $(LIBS)

clean:
	rm sa
