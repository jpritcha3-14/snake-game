snake : src/game_loop.c src/helper_functions.c src/main.c 
	gcc -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c -o snake -lncurses

gdb : snake
	gdb snake

clean : 
	rm snake

