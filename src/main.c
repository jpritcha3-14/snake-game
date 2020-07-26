#include <ncurses.h>
#include <stdlib.h>

#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"
#include "menu.h"

int main() {
    initscr();
    noecho();
    start_color();
    // Dummy window to for input, avoids extra refresh on wgetch call
    WINDOW* dummy = newwin(1, 1, 1000, 1000); 
    keypad(dummy, true);
    curs_set(0);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    wloc* la = get_logo_area(stdscr);
    wloc* ma = get_menu_area(stdscr, la);
    wloc* hsa = get_hs_area(stdscr);

    while(show_menu(ma, la, hsa, dummy));

    free(hsa);
    free(ma);
    free(la);

    endwin();
    return 0;
}
