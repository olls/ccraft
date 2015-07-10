all:
	gcc main.c loader.c util/common.c -std=c11 -lSDL2 -g && timeout -m 200000 ./a.out
