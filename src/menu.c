#include <time.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"

enum option {start, speed, score, quit};

struct cursor {
    char* text;
    char* blank;
    int textlen;
    enum option option;
};

const char* speedtext[] = {
    "Slow",
    "Normal",
    "Fast"
};

const char* optiontext[] = {
    "Start Game",
    "Speed:",
    "High Scores (to be implemented)",
    "Quit"
};

const char* eraseoption = "          ";

void draw_cursor (wloc pa, struct cursor c) {
     c.option % 2 ? attron(COLOR_PAIR(2)) : attron(COLOR_PAIR(1));
     mvaddstr(pa.y + c.option * 2, pa.x - c.textlen - 1, c.text);
     mvaddstr(pa.y + c.option * 2 - 2, pa.x - c.textlen - 1, c.blank);
     mvaddstr(pa.y + c.option * 2 + 2, pa.x - c.textlen - 1, c.blank);
     refresh();
     attroff(COLOR_PAIR);
}

void draw_option (wloc pa, enum option op, int choice) {
     mvaddstr(pa.y + op * 2, pa.x + strlen(optiontext[op]) + 1, eraseoption);
     if (op == speed) {
        attron(COLOR_PAIR(1));
        mvaddstr(pa.y + op * 2, pa.x + strlen(optiontext[op]) + 1, speedtext[choice]);
     }
     refresh();
     attroff(COLOR_PAIR);
}

void draw_menu(wloc pa) {
     for (enum option i = start; i <= quit; i++) {
        i % 2 ? attron(COLOR_PAIR(1)) : attron(COLOR_PAIR(2));
        mvaddstr(pa.y + 2 * i, pa.x, optiontext[i]);
     }
     attroff(COLOR_PAIR);
}

int show_menu(wloc pa, WINDOW* dummy) {
    struct timespec w;
    w.tv_sec = 1;
    w.tv_nsec = 0;
    struct cursor c = {"--<", "   ", 3, start};
    enum speed s = normal;
    attron(A_BOLD);
    draw_menu(pa);
    draw_cursor(pa, c);
    draw_option(pa, 1, (int)s);
    char input;
    
    for (;;) {
        draw_menu(pa);
        draw_cursor(pa, c);
        draw_option(pa, 1, (int)s);
        input = wgetch(dummy); 
        if (input == 'w' && c.option > start) {
            c.option--;  
            draw_cursor(pa, c);
        } else if (input == 's' && c.option < quit) {
            c.option++; 
            draw_cursor(pa, c);
        } else if (c.option == speed) {
            if (input == 'd' && s < fast) s++; draw_option(pa, c.option, (int)s);
            if (input == 'a' && s > slow) s--; draw_option(pa, c.option, (int)s);
        } else if (input == ' ') {
            if (c.option == start) {
                play_game(pa, dummy, s);
                nanosleep(&w, NULL);
                clear();
                refresh();
                wtimeout(dummy, 0);
                while (wgetch(dummy) != ERR);
                wtimeout(dummy, -1);
            } else if (c.option == quit) break;
        }
    }
    return 0;
}
