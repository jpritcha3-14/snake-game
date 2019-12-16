# Snake
The [classic game](https://en.wikipedia.org/wiki/Snake_(video_game_genre)) implemented in C using the ncurses library to draw the game in a terminal session. Grow as long as possible by eating apples that appear!

![menu png](https://github.com/jpritcha3-14/snake-game/blob/master/menu.png) ![demo gif](https://github.com/jpritcha3-14/snake-game/blob/master/demo.gif)

## Controls
#### Menu
- w/s: Move cursor
- a/d: Change setting (speed, size)
- space: Select

#### Game
- w/a/s/d: Change the direction of the snake. 
- Direction change inputs are buffered to allow for easy execution of double turns.

## Install
The game should work on most any Unix-like system with ncurses available, just make sure you have ncurses installed.  To install ncurses on Debian/Ubuntu use: ```sudo apt-get install libncurses5-dev libncursesw5-dev```

Then use the makefile to compile, or use gcc directly:
  
```gcc -Wall -Iheaders src/game_loop.c src/helper_functions.c src/main.c src/menu.c -o snake -lncurses```

## Motivation
This is just a small project to get my feet wet with C again after writing a lot of Python over the past few years.  It's been a great way to refamiliarize myself with pointer manipulation and memory allocation, learn ncurses, practice manipulating structs and higher level data structures with pointers, and use gdb to help squash subtle memory bugs.  I would highly reccomend this kind of project to anyone wanting to improve their C skills :)

## Future Updates
- Add high scores for each option combination.
- Refactor for conciseness and readability.
- ~~Rewrite the play area as a curses window structure to make indexing cleaner.~~
- ~~Add menu option to select play area size.~~
- ~~Add menu option to select play speed.~~
