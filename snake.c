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
    int usefulcols;
};

typedef struct wloc wloc;

struct node {
    struct node* next;
    struct node* prev;
    loc* position;
};

struct linked_list {
    struct node* head;
    struct node* tail;  
    int length;
};

int toarrayidx(int row, int col, wloc pa) {
    int translatedcol = (col - pa.x - 2) / 2;
    int translatedrow = row - pa.y - 1;
    return translatedrow * ((pa.cols - 1) / 2) + translatedcol; 
}

loc tolocation(int idx, wloc pa) {    
    int translatedcol = idx % pa.usefulcols;
    int translatedrow = idx / pa.usefulcols;
    loc loconscreen;
    loconscreen.row = translatedrow + pa.y + 1;
    loconscreen.col = (translatedcol * 2) + pa.x + 2;
    return loconscreen;
}

void updatevalidpos(bool* valid, wloc pa, loc* toremove, loc* toadd) {
    valid[toarrayidx(toremove->row, toremove->col, pa)] = false;
    if (toadd) {
        valid[toarrayidx(toadd->row, toadd->col, pa)] = true;
    }
}

loc placeapple(const bool* valid, wloc pa) {
    int len = pa.rows * pa.usefulcols;
    int apple = rand() % len;
    while (!valid[apple]) {
        apple = (apple + 1) % len; 
    } 
    return tolocation(apple, pa);
}

wloc get_play_area(WINDOW* w) {
    wloc play_area; 
    int wrows, wcols;
    getmaxyx(w, wrows, wcols);
    play_area.x = (wcols / 4);
    play_area.y = (wrows / 4);
    play_area.cols = (wcols / 2);
    play_area.cols += (wcols / 2) % 2 ? 0 : 1;
    play_area.usefulcols = (play_area.cols - 1) / 2; 
    play_area.rows = (wrows / 2);
    return play_area;
}

void draw_border(wloc pa, char c) {
    move(pa.y, pa.x);
    attron(A_BOLD);
    for (int i = pa.x; i <= pa.x + pa.cols + 1; i++) {
        addch(c); 
    }
    for (int i = pa.y + 1; i <= pa.y + pa.rows; i++) {
        mvaddch(i, pa.x, c);
        mvaddch(i, pa.x + pa.cols + 1, c);
    }
    move(pa.y + pa.rows + 1, pa.x);
    for (int i = pa.x; i <= pa.x + pa.cols + 1; i++) {
        addch(c); 
    }
    attroff(A_BOLD);
    refresh();
}

bool in_bounds(loc l, wloc w) {
    return (l.row > w.y && l.row < w.y + w.rows + 1
            && l.col > w.x && l.col < w.x + w.cols + 1);
}

void play_game(wloc pa, WINDOW* dummy) {

    keypad(dummy, TRUE);
    loc previous;
    char head = '<';
    char ch = 'd';
    char key;
    char prevkey;
    char nextkey = ERR;
    char nxt;
    char body;
    char scorebuffer[6];
    struct node* tempfrontbody;
    struct node* temppenultbody;

    struct linked_list* snake = malloc(sizeof(struct linked_list));
    snake->head = malloc(sizeof(struct node));
    snake->head->position = malloc(sizeof(loc));
    snake->head->position->row = pa.y + 1;
    snake->head->position->col = pa.x + 2;
    snake->length = 1;

    bool* validapplepositions = malloc(pa.rows * pa.cols * sizeof(bool));
    validapplepositions[0] = false;
    for (int i = 1; i < pa.rows * pa.cols; i++) validapplepositions[i] = true;

    loc apple = placeapple(validapplepositions, pa);
    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvaddch(apple.row, apple.col, 'a');
    attroff(COLOR_PAIR(2));

    enum direction dir = East;
    struct timespec cur;
    cur.tv_sec = 0;
    cur.tv_nsec = 100000000;
    chtype *next_space = malloc(2 * sizeof(chtype));

    attron(COLOR_PAIR(2));
    mvaddch(pa.y-1, pa.x+7, '0');
    attron(COLOR_PAIR(1));
    mvaddstr(pa.y-1, pa.x, "SCORE:");
    mvaddch(snake->head->position->row, snake->head->position->col, head);
    refresh();
    nxt = wgetch(dummy);
    wtimeout(dummy, 0);
    
    for(;;) {
        // Read everything out of the character buffer, set x to last character;
        prevkey = ERR;
        key = ERR;
        if (nextkey != ERR) {
            ch = nextkey;
            nextkey = ERR;
            while (wgetch(dummy) != ERR); // clear buffer
        } else {
            do {
                prevkey = key;
                key = nxt;
                nxt = wgetch(dummy);
            } while (nxt != ERR);
    
            if (key != ERR && prevkey != ERR && key != prevkey
                && (prevkey == 'a' || prevkey == 'w' || prevkey == 'd' || prevkey == 's') 
                && (key == 'a' || key == 'w' || key == 'd' || key == 's')) {
                ch = prevkey;
                nextkey = key; 
            } else if (key != ERR
                && (key == 'a' || key == 'w' || key == 'd' || key == 's')) {
                ch = key;
            }
        }
                
        // Update head direction and representation 
        if (ch == 'a' && dir != East) {
            dir = West;
            head = '>';
        } else if (ch == 'w' && dir != South) {
            dir = North;
            head = 'v';
        } else if (ch == 'd' && dir != West) {
            dir = East;
            head = '<';
        } else if (ch == 's' && dir != North) {
            dir = South;
            head = '^';
        }

        // update the body linked list
        if (snake->length == 1) {
            previous.row = snake->head->position->row;
            previous.col = snake->head->position->col;
        } else if (snake->length == 2) {
            previous.row = snake->tail->position->row;
            previous.col = snake->tail->position->col;
            mvaddch(snake->head->position->row, snake->head->position->col,
                    dir == North || dir == South ? ':' : '-');
            snake->tail->position->row = snake->head->position->row;
            snake->tail->position->col = snake->head->position->col;
        } else {
            previous.row = snake->tail->position->row;
            previous.col = snake->tail->position->col;
            mvaddch(snake->head->position->row, snake->head->position->col, 
                    dir == North || dir == South ? ':' : '-');
            snake->tail->position->row = snake->head->position->row;
            snake->tail->position->col = snake->head->position->col;

            snake->tail = snake->tail->prev;
        }

        // draw new body segment where head was
        mvaddch(snake->head->position->row, 
                snake->head->position->col, 
                dir == North || dir == South ? ':' : '-');

        // move head to new position
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


        if ((unsigned char)next_space[0] == ('a' & A_CHARTEXT)) {
            if (snake->length == 1) {
                mvaddch(previous.row, previous.col, (dir == North || dir == South) ? ':' : '-');
                snake->tail = malloc(sizeof(struct node));
                snake->tail->next = snake->tail;
                snake->tail->prev = snake->tail;
            } else {
                tempfrontbody = snake->tail->next;
                snake->tail->next = malloc(sizeof(struct node));
                snake->tail->next->prev = snake->tail;
                snake->tail->next->next = tempfrontbody;
                tempfrontbody->prev = snake->tail->next;
                snake->tail = snake->tail->next;
            }
            snake->tail->position = malloc(sizeof(loc));
            snake->tail->position->row = previous.row;
            snake->tail->position->col = previous.col;  
            snake->length += 1;
            sprintf(scorebuffer, "%d", snake->length - 1);

            updatevalidpos(validapplepositions, pa, snake->head->position, NULL);
            apple = placeapple(validapplepositions, pa);
            attron(COLOR_PAIR(2));
            mvaddch(apple.row, apple.col, 'a');
            mvaddstr(pa.y - 1, pa.x + 7, scorebuffer);
            attron(COLOR_PAIR(1));
        } else {
            mvaddch(previous.row, previous.col, ' ');
            mvinchnstr(snake->head->position->row, snake->head->position->col, next_space, 1);
            updatevalidpos(validapplepositions, pa, snake->head->position, &previous);
        }

        // cast to unsigned char and strip off chtype formatting to compare characters
        if ((unsigned char)next_space[0] == ('-' & A_CHARTEXT) 
            || (unsigned char)next_space[0] == (':' & A_CHARTEXT) 
            || !in_bounds(*snake->head->position, pa)) {
            mvaddch(previous.row, previous.col, ' ');
            mvaddch(snake->head->position->row, snake->head->position->col, head);
            refresh();
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
    WINDOW* dummy = malloc(sizeof(WINDOW));
    dummy = newwin(0, 0, 1000, 1000);
    nodelay(dummy, true);
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
