snake : src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c
	gcc -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c -o snake -lncurses

snaketest : src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c
	gcc -ggdb -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c -o snaketest -lncurses

gdb : snake
	gdb snake

clean : 
	rm snake

