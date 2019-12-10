#include <ncurses.h>
#include <stdlib.h>

#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"

int main() {
    initscr();
    noecho();
    start_color();
    // Dummy window to for input, avoids extra refresh on wgetch call
    WINDOW* dummy = malloc(sizeof(WINDOW));
    dummy = newwin(1, 1, 1000, 1000); 
    keypad(dummy, true);
    curs_set(0);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    wloc pa = get_play_area(stdscr);
    draw_border(pa, '!');
    play_game(pa, dummy);
    
    endwin();
    return 0;
}
