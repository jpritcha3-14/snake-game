snake : src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c
	gcc -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c -o snake -lncurses -lsqlite3

snaketest : src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c
	gcc -ggdb -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c -o snaketest -lncurses -lsqlite3

gdb : snaketest
	gdb snaketest

clean : 
	rm snake

