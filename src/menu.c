#include <time.h>
#include <stdlib.h> 
#include <stdio.h>

#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"

enum option{start, speed, score, quit};

struct cursor {
    char* text;
    char* blank;
    int textlen;
    int loc;
};

const char* text[] = {
    "Start Game",
    "Speed: Fast (to be implemented)",
    "High Scores (to be implemented)",
    "Quit"
};

void draw_cursor (wloc pa, struct cursor c) {
     c.loc % 2 ? attron(COLOR_PAIR(2)) : attron(COLOR_PAIR(1));
     mvaddstr(pa.y + c.loc * 2, pa.x - c.textlen - 1, c.text);
     mvaddstr(pa.y + c.loc * 2 - 2, pa.x - c.textlen - 1, c.blank);
     mvaddstr(pa.y + c.loc * 2 + 2, pa.x - c.textlen - 1, c.blank);
     refresh();
     attroff(COLOR_PAIR);
}

void draw_menu(wloc pa) {
     for (int i = 0; i < 4; i++) {
        i % 2 ? attron(COLOR_PAIR(1)) : attron(COLOR_PAIR(2));
        mvaddstr(pa.y + 2 * i, pa.x, text[i]);
     }
     attroff(COLOR_PAIR);
}

int show_menu(wloc pa, WINDOW* dummy) {
    enum option selected = start;
    struct timespec w;
    w.tv_sec = 2;
    w.tv_nsec = 0;
    struct cursor c = {"->", "  ", 2, 0};
    attron(A_BOLD);
    draw_menu(pa);
    draw_cursor(pa, c);
    char input;
    
    for (;;) {
        draw_menu(pa);
        draw_cursor(pa, c);
        input = wgetch(dummy); 
        if (input == 'w' && c.loc > 0) {
            c.loc--;  
            draw_cursor(pa, c);
        } else if (input == 's' && c.loc < 3) {
            c.loc++; 
            draw_cursor(pa, c);
        } else if (input == ' ' && c.loc == 0) {
            play_game(pa, dummy);
            nanosleep(&w, NULL);
            clear();
            refresh();
            wtimeout(dummy, 0);
            while (wgetch(dummy) != ERR);
            wtimeout(dummy, -1);
        } else if (input == ' ' && c.loc == 3) {
            break;
        }
    }
    return 0;
}
