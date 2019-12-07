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

struct node {
    struct node* next;
    loc* position;
};

struct linked_list {
    struct node* head;
    struct node* tail;  
    int length;
};
    
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
}

bool in_bounds(loc l, wloc w) {
    return (l.row > w.y && l.row < w.y + w.rows
            && l.col > w.x && l.col < w.x + w.cols);
}

void play_game(wloc pa) {
    char head = '>';
    char ch = 'd';
    char x;
    char nxt;
    char body;
    struct linked_list* snake = malloc(sizeof(struct linked_list));
    snake->head = malloc(sizeof(struct node));
    snake->head->position = malloc(sizeof(loc));
    snake->head->position->row = pa.y + 1;
    snake->head->position->col = pa.x + 2;
    snake->length = 1;

    enum direction dir = East;
    struct timespec cur;
    cur.tv_sec = 0;
    cur.tv_nsec = 100000000;
    chtype *next_space = malloc(2 * sizeof(chtype));

    attron(COLOR_PAIR(1));
    attron(A_BOLD);
    move(snake->head->position->row, snake->head->position->col);
    mvaddch(snake->head->position->row, snake->head->position->col, head);
    refresh();
    nxt = getch();
    timeout(0);
    
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
        mvaddch(snake->head->position->row, 
                snake->head->position->col, 
                dir == North || dir == South ? ':' : '-');

        refresh();

        if (dir == West) {
            snake->head->position->col -= 2;
        } else if (dir == North) {
            snake->head->position->row -= 1;
        } else if (dir == East) {
            snake->head->position->col += 2;
        } else {
            snake->head->position->row += 1;
        }

        
        // get where head will be moved
        mvinchnstr(snake->head->position->row, snake->head->position->col, next_space, 1);
        // cast to unsigned char and strip off chtype formatting to compare characters
        if ((unsigned char)next_space[0] == ('-' & A_CHARTEXT) 
            || (unsigned char)next_space[0] == (':' & A_CHARTEXT) 
            || !in_bounds(*snake->head->position, pa)) {
            mvaddch(snake->head->position->row, snake->head->position->col, head);
            refresh();
            free(next_space);
            return;
        }

        // place head
        mvaddch(snake->head->position->row, snake->head->position->col, head);
        refresh();
        nanosleep(&cur, NULL);
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    start_color();
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
