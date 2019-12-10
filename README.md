# Snake
The [classic game](https://en.wikipedia.org/wiki/Snake_(video_game_genre)) implemented in C using the ncurses library to draw the game in a terminal session.

![demo gif](https://github.com/jpritcha3-14/snake-game/blob/master/demo.gif)

## Install
The game should work on most any Unix-like system with ncurses available, just make sure you have ncurses installed.  To install ncurses on Debian/Ubuntu use: ```sudo apt-get install libncurses5-dev libncursesw5-dev```

Then use the makefile to compile, or use gcc directly:

```gcc -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c -o snake -lncurses```

## Motivation
This is just a small project to get my feet wet with C again after writing a lot of Python over the past few years.  It's been a great way to refamiliarize myself with pointer manipulation and memory allocation, learn ncurses, and gain some practice manipulating structs and higher level data structures.  I would highly reccomend this kind of project to anyone wanting to improve their C skills :)

## Future Updates
- Refactor some of the lower level structures and functions to make the code more concise and readable.
- Rewrite the play area as a curses window structure to make indexing cleaner.
- Add a menu system with options for snake speed and play area size.
- Add high scores for each option combination.
