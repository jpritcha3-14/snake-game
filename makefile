snake : src/game_loop.c src/helper_functions.c src/main.c src/menu.c
	gcc -ggdb -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c -o snake -lncurses

gdb : snake
	gdb snake

clean : 
	rm snake

