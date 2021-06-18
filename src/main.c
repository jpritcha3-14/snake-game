#include <ncurses.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"
#include "high_scores.h"
#include "menu.h"

#if !defined(HSPATH)
const char DBPATH[] = "/usr/local/share/snake-game/assets/high_scores.db";
#else
const char DBPATH[] = HSPATH;
#endif

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

    // Create table and change permissions
    const char hs[] = "high_scores.db";
    create_db(hs);
    chmod(DBPATH, 0666);

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
