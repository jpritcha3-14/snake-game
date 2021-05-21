# Snake
The [classic game](https://en.wikipedia.org/wiki/Snake_(video_game_genre)) implemented in C using the ncurses library to draw the game in a terminal session. Grow as long as possible by eating apples that appear.  Select form 9 different combinations of speed (Slow, Normal, Fast) and play area size (Small, Medium, Large).  The top 10 high scores are kept for each play area and size combination.

![menu png](https://github.com/jpritcha3-14/snake-game/blob/master/assets/menu.png) ![demo gif](https://github.com/jpritcha3-14/snake-game/blob/master/assets/demo.gif)

## Install
The game requires ncurses and sqlite3 which are available on most any Unix-like system.  To install them on a Debian based distro use: ```sudo apt-get install libncurses5-dev libncursesw5-dev sqlite3 libsqlite3-dev```

Then use the makefile to compile:

To place make the binary available to all users:
```
sudo make
```

To the binary in the current snake-game directory:
```
make here
```

## Uninstall
```
sudo make clean
```
or

```
make clean-here
```

## Controls
#### Menu
- w/s: Move cursor
- a/d: Change setting (speed, size)
- space: Select

#### Game
- w/a/s/d: Change the direction of the snake. 
- Direction change inputs are buffered to allow for easy execution of double turns.

#### High Scores
- Enter a name of up to 10 characters and then press enter to save the high score and return to the Menu.
- High scores for different play settings can be viewed by selecting the settings in the Menu, and then selecting the High Score option.
- Press any key to return to the Menu when viewing high scores.

## Motivation
This is just a small project to get my feet wet with C again after writing a lot of Python over the past few years.  It's been a great way to refamiliarize myself with pointer manipulation and memory allocation, learn ncurses, practice manipulating structs and higher level data structures with pointers, and use gdb to help squash subtle memory bugs.  I would highly reccomend this kind of project to anyone wanting to improve their C skills :)

## Future Updates
- Refactor for conciseness, readability, and correctness.
- ~~Add high scores for each option combination.~~
- ~~Rewrite the play area as a curses window structure to make indexing cleaner.~~
- ~~Add menu option to select play area size.~~
- ~~Add menu option to select play speed.~~
