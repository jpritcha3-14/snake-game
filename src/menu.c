#include <time.h>
#include <stdlib.h> 
#include <stdio.h>

#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"

enum option{start, speed, score, quit};

int show_menu(wloc pa, WINDOW* dummy) {
    enum option selected = start;
    char* start = "Start!";
    char* quit = "Press q to quit";

    struct timespec w;
    w.tv_sec = 2;
    w.tv_nsec = 0;

    attron(A_BOLD);
    mvaddstr(pa.y, pa.x, start);
    mvaddstr(pa.y + 2, pa.x, quit);
    refresh();
    
    while (wgetch(dummy) != 'q') {
        play_game(pa, dummy);
        nanosleep(&w, NULL);
        clear();
        mvaddstr(pa.y, pa.x, start);
        mvaddstr(pa.y + 2, pa.x, quit);
        refresh();
        wtimeout(dummy, 0);
        while (wgetch(dummy) != ERR);
        wtimeout(dummy, -1);
    }

    return 0;
}
