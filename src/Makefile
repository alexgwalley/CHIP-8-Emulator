main: main.o graphics.o input.o
	gcc main.c graphics.c input.c -o main -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2

main.o: main.c
	gcc -c main.c

graphics.o: graphics.c graphics.h
	gcc -c graphics.c -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2

input.o: input.c input.h
	gcc -c input.c -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2