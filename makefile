snake : src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c
	mkdir -p /usr/local/share/snake-game/assets
	cp ./assets/logo.txt /usr/local/share/snake-game/assets
	gcc -std=gnu11 -DABSPATH="TRUE" -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c -o /usr/local/bin/snake -lncurses -lsqlite3
	chmod -R o+rw /usr/local/share/snake-game/assets
	chmod o+x /usr/local/bin/snake

here : src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c
	gcc -std=gnu11 -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c -o snake -lncurses -lsqlite3

snaketest : src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c
	gcc -std=gnu11 -ggdb -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c -o snaketest -lncurses -lsqlite3

gdb : snaketest
	gdb snaketest

clean : 
	rm -rf /usr/local/share/snake-game
	rm /usr/local/bin/snake

clean-here :
	rm snake
	rm assets/high_scores.db
