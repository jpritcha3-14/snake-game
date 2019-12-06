snake : snake.c
	gcc -ggdb snake.c -o snake -lncurses

gdb : snake
	gdb snake

clean : 
	rm snake

