main: main.o graphics.o 
	gcc main.c graphics.c -o main -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2

main.o: main.c
	gcc -c main.c

graphics.o: graphics.c graphics.h
	gcc -c graphics.c
