snakegame : src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c
	mkdir -p build/assets
	chmod o+w build/assets
	gcc -std=gnu11 -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c -o build/snake -lncurses -lsqlite3
	chmod go+x build/snake
	cp assets/logo.txt build/assets

install: build/snake build/assets
	mkdir -p /usr/local/share/snake-game
	cp -pr build/assets /usr/local/share/snake-game
	cp -p build/snake /usr/local/bin

snaketest : src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c
	gcc -std=gnu11 -ggdb -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c src/high_scores.c -o snaketest -lncurses -lsqlite3

gdb : snaketest
	gdb snaketest

clean : 
	rm -rf /usr/local/share/snake-game
	rm /usr/local/bin/snake

clean-test :
	rm snaketest
	rm assets/high_scores.db
