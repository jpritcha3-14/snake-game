#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

enum direction{North, South, East, West};

struct loc {
    int row;
    int col;
};

typedef struct loc loc;

struct wloc {
    int y;
    int x;
    int cols;
    int rows;
};

typedef struct wloc wloc;
    
wloc get_play_area(WINDOW* w) {
    wloc play_area; 
    int wrows, wcols;
    getmaxyx(w, wrows, wcols);
    play_area.x = (wcols / 4);
    play_area.y = (wrows / 4);
    play_area.cols = (wcols / 2);
    play_area.cols += (wcols / 2) % 2 ? 1 : 0;
    play_area.rows = (wrows / 2);
    return play_area;
}

void draw_border(wloc pa, char c) {
    move(pa.y, pa.x);
    attron(A_BOLD);
    for (int i = pa.x; i <= pa.x + pa.cols; i++) {
        addch(c); 
    }
    for (int i = pa.y + 1; i < pa.y + pa.rows; i++) {
        mvaddch(i, pa.x, c);
        mvaddch(i, pa.x + pa.cols, c);
    }
    move(pa.y + pa.rows, pa.x);
    for (int i = pa.x; i <= pa.x + pa.cols; i++) {
        addch(c); 
    }
    attroff(A_BOLD);
    refresh();
    getch();
}

bool in_bounds(loc l, wloc w) {
    return (l.row > w.y && l.row < w.y + w.rows
            && l.col > w.x && l.col < w.x + w.cols);
}

void play_game(wloc pa) {
    char ch = 'd';
    char x = 'd';
    char nxt = 'd';
    char head = '>';
    char body = '*';
    int slen = 10;

    loc curloc;
    curloc.row = pa.y + 1;
    curloc.col = pa.x + 2;
    enum direction dir = East;
    struct timespec cur;
    cur.tv_sec = 0;
    cur.tv_nsec = 100000000;
    timeout(0);
    chtype *next_space = malloc(2 * sizeof(chtype));

    move(curloc.row, curloc.col);
    attron(COLOR_PAIR(1));
    attron(A_BOLD);

    for(;;) {
        // Read everything out of the character buffer, set x to last character;
        do {
            x = nxt;
            nxt = getch();
        } while (nxt != ERR);

        // Repeat last character if no input, or not wasd
        ch = (x == ERR 
          || (x != 'a' && x != 'w' && x != 'd' && x != 's')) ? ch : x;

        // Update head position
        if (ch == 'a' && dir != East) {
            dir = West;
            head = '<';        
        } else if (ch == 'w' && dir != South) {
            dir = North;
            head = '^';        
        } else if (ch == 'd' && dir != West) {
            dir = East;
            head = '>';        
        } else if (ch == 's' && dir != North) {
            dir = South;
            head = 'v';        
        }

        // place body where head was
        mvaddch(curloc.row, curloc.col, body);
        refresh();

        if (dir == West) {
            curloc.col -= 2;
        } else if (dir == North) {
            curloc.row -= 1;
        } else if (dir == East) {
            curloc.col += 2;
        } else {
            curloc.row += 1;
        }
        
        // get where head will be moved
        mvinchnstr(curloc.row, curloc.col, next_space, 1);
        // cast to unsigned char and strip off chtype formatting to compare characters
        if ((unsigned char)next_space[0] == ('*' & A_CHARTEXT) || !in_bounds(curloc, pa)) {
            mvaddch(curloc.row, curloc.col, head);
            refresh();
            free(next_space);
            return;
        }

        // place head
        mvaddch(curloc.row, curloc.col, head);
        refresh();
        nanosleep(&cur, NULL);
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_RED);
    keypad(stdscr, TRUE);
    curs_set(0);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    wloc pa = get_play_area(stdscr);
    draw_border(pa, '!');
    play_game(pa);
    endwin();
    return 0;
}
