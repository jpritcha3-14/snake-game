#include <time.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"

enum option {start, speed, size, score, quit};
enum size {small, med, large};

const struct sizes {
    loc small;
    loc med;
    loc large;
} sizes = {{10, 25}, {15, 50}, {20, 75}};

struct cursor {
    char* text;
    char* blank;
    int textlen;
    enum option option;
};

const char* sizetext[] = {
    "Small",
    "Medium",
    "Large"
};

const char* speedtext[] = {
    "Slow",
    "Normal",
    "Fast"
};

const char* optiontext[] = {
    "Start Game",
    "Speed:",
    "Size:",
    "High Scores TBA",
    "Quit"
};

const char* eraseoption = "          ";

void draw_cursor (wloc* ma, struct cursor c) {
     c.option % 2 ? attron(COLOR_PAIR(2)) : attron(COLOR_PAIR(1));
     mvaddstr(ma->y + c.option * 2, ma->x - c.textlen - 1, c.text);
     mvaddstr(ma->y + c.option * 2 - 2, ma->x - c.textlen - 1, c.blank);
     mvaddstr(ma->y + c.option * 2 + 2, ma->x - c.textlen - 1, c.blank);
     refresh();
     attroff(COLOR_PAIR);
}

void draw_option (wloc* ma, enum option op, int speedchoice, int sizechoice) {
     mvaddstr(ma->y + op * 2, ma->x + strlen(optiontext[op]) + 1, eraseoption);
     if (op == speed) {
        attron(COLOR_PAIR(1));
        mvaddstr(ma->y + op * 2, ma->x + strlen(optiontext[op]) + 1, speedtext[speedchoice]);
     }
     if (op == size) {
        attron(COLOR_PAIR(2));
        mvaddstr(ma->y + op * 2, ma->x + strlen(optiontext[op]) + 1, sizetext[sizechoice]);
     }
     refresh();
     attroff(COLOR_PAIR);
}

void draw_menu(wloc* ma) {
     for (enum option i = start; i <= quit; i++) {
        i % 2 ? attron(COLOR_PAIR(1)) : attron(COLOR_PAIR(2));
        mvaddstr(ma->y + 2 * i, ma->x, optiontext[i]);
     }
     attroff(COLOR_PAIR);
}

enum option get_max_size() {
    int r, c;
    getmaxyx(stdscr, r, c);
    if (r < sizes.med.row || c < sizes.med.col) {
        return 0;  
    } else if (r < sizes.large.row || c < sizes.large.col) {
        return 1;
    } else {
        return 2; 
    }
}

int show_menu(wloc* ma, WINDOW* dummy) {
    struct timespec w;
    w.tv_sec = 1;
    w.tv_nsec = 0;
    struct cursor c = {"--<", "   ", 3, start};
    enum speed s = normal;
    enum size sz = small;
    enum size max_size = get_max_size();
    attron(A_BOLD);
    draw_menu(ma);
    draw_cursor(ma, c);
    draw_option(ma, speed, (int)s, (int)sz);
    draw_option(ma, size, (int)s, (int)sz);

    char input;
    
    for (;;) {
        draw_menu(ma);
        draw_cursor(ma, c);
        draw_option(ma, speed, (int)s, (int)sz);
        draw_option(ma, size, (int)s, (int)sz);
        input = wgetch(dummy); 
        if (input == 'w' && c.option > start) {
            c.option--;  
            draw_cursor(ma, c);
        } else if (input == 's' && c.option < quit) {
            c.option++; 
            draw_cursor(ma, c);
        } else if (c.option == speed) {
            if (input == 'd' && s < fast) s++; draw_option(ma, c.option, (int)s, (int)sz);
            if (input == 'a' && s > slow) s--; draw_option(ma, c.option, (int)s, (int)sz);
        } else if (c.option == size) {
            if (input == 'd' && sz < max_size) sz++; draw_option(ma, c.option, (int)s, (int)sz);
            if (input == 'a' && sz > small) sz--; draw_option(ma, c.option, (int)s, (int)sz);
        } else if (input == ' ') {
            if (c.option == start) {
                //play_game(ma, dummy, s);
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
