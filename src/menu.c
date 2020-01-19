#include <time.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

#include "high_scores.h"
#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"

enum option {start, speed, size, score, quit};
enum size {small, med, large};
const loc sizes[] = {{10, 25}, {15, 51}, {20, 75}};

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
    "High Scores",
    "Quit"
};

const char* eraseoption = "          ";

void draw_cursor (wloc* ma, struct cursor c) {
     attron(A_BOLD);
     c.option % 2 ? attron(COLOR_PAIR(2)) : attron(COLOR_PAIR(1));
     mvaddstr(ma->y + c.option * 2, ma->x - c.textlen - 1, c.text);
     mvaddstr(ma->y + c.option * 2 - (c.option > 0 ? 2 : -2), ma->x - c.textlen - 1, c.blank);
     mvaddstr(ma->y + c.option * 2 + 2, ma->x - c.textlen - 1, c.blank);
     refresh();
     attroff(COLOR_PAIR);
}

void draw_option (wloc* ma, enum option op, int speedchoice, int sizechoice) {
     attron(A_BOLD);
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

void draw_logo(wloc* la) {
    if (la != NULL) {
        attron(COLOR_PAIR(1));
        FILE* fp;
        char buff[255];

        fp = fopen("./assets/logo.txt", "r");
        int i = la->y;
        while (fgets(buff, 255, fp)) {
            mvaddstr(i++, la->x, buff);
        }
        fclose(fp);
    }
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
    if (r < sizes[med].row + 1 || c < sizes[med].col) {
        return 0;  
    } else if (r < sizes[large].row + 1 || c < sizes[large].col) {
        return 1;
    } else {
        return 2; 
    }
}

// Return game area subwindow centered in terminal
WINDOW* getgamewindow(enum size sz) {
    int rows, cols;
    int row, col;
    getmaxyx(stdscr, rows, cols);
    row = rows / 2 - (sizes[sz].row / 2);
    row = row <= 0 ? 1 : row;
    col = cols / 2 - (sizes[sz].col / 2);
    col = col < 0 ? 0 : col;
    return subwin(stdscr, sizes[sz].row, sizes[sz].col, row, col);
}

WINDOW* getscorewindow(WINDOW* pw) {
    int y, x;
    wmove(pw, 0, 0);
    getparyx(pw, y, x);
    return subwin(stdscr, 1, 25, y - 1, x);
}

WINDOW* gethighscorewindow(wloc* hsa) {
    return subwin(stdscr, 13, 26, hsa->y, hsa->x);
}

int show_menu(wloc* ma, wloc* la, wloc* hsa, WINDOW* dummy) {
    char input;
    int finalscore = 0;
    struct timespec w;
    w.tv_sec = 1;
    w.tv_nsec = 0;
    struct cursor c = {"--<", "   ", 3, start};
    enum speed s = normal;
    enum size max_size = get_max_size();
    enum size sz = max_size;
    
    for (;;) {
        attron(A_BOLD);
        draw_logo(la);
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
            if (input == 'd' && s < fast) s++;
                draw_option(ma, c.option, (int)s, (int)sz);
            if (input == 'a' && s > slow) s--;
                draw_option(ma, c.option, (int)s, (int)sz);
        } else if (c.option == size) {
            if (input == 'd' && sz < max_size) sz++;
                draw_option(ma, c.option, (int)s, (int)sz);
            if (input == 'a' && sz > small) sz--;
                draw_option(ma, c.option, (int)s, (int)sz);
        } else if (input == ' ') { 
            if (c.option == start) {
                WINDOW* pw = getgamewindow(sz);
                WINDOW* sw = getscorewindow(pw);
                WINDOW* hsw = gethighscorewindow(hsa);
                clear();
                refresh();
                finalscore = play_game(pw, sw, dummy, s);
                nanosleep(&w, NULL);
                delwin(pw);
                delwin(sw);
                clear();
                refresh();
                wtimeout(dummy, 0);
                while (wgetch(dummy) != ERR);
                wtimeout(dummy, -1);
                add_high_score(hsw, finalscore, sizetext[sz], speedtext[s]);
                delwin(hsw);
                clear();
                refresh();
            } else if (c.option == score) {
                WINDOW* hsw = gethighscorewindow(hsa);
                clear();
                refresh();
                show_high_scores(hsw, dummy, sizetext[sz], speedtext[s]);
                delwin(hsw);
                clear();
                refresh();
            } else if (c.option == quit) break;
        }
    }
    return 0;
}
