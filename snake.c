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
    struct node* prev;
    loc* position;
};

struct linked_list {
    struct node* head;
    struct node* tail;  
    int length;
};

loc placeapple(bool* valid, wloc pa) {
    int cols = ((pa.cols - 3) / 2);
    int rows = (pa.rows - 2);
    int len = rows * cols; 
    int finalrow, finalcol;
    loc translated;

    int guess = rand() % len;
    while (valid[guess] == false) {
        guess = (guess + 1) % len; 
    } 
    
    finalrow = guess / cols; 
    finalcol = guess % cols;
    translated.row = pa.y + 1 + finalrow;
    translated.col = pa.x + 2 + (finalcol * 2);
    return translated;
}

void updatevalidpos(bool* valid, const wloc* pa, const loc* toadd, const loc* toremove) {
    int condensedcol = ((toadd->col - pa->x) / 2) - 1;
    int condensedrow = (toadd->row - pa->y) - 1;
    int idx = (condensedrow * pa->cols) + condensedcol;
    valid[idx] = false;
    if (toremove) {
        condensedcol = ((toremove->col - pa->x) / 2) - 1;
        condensedrow = (toremove->row - pa->y) - 1;
        idx = (condensedrow * pa->cols) + condensedcol;
        valid[idx] = true;
    }
}

    
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

    loc previous;
    char head = '>';
    char ch = 'd';
    char x;
    char nxt;
    char body;
    struct node* tempnodeptr;


    int totalpositions = ((pa.cols - 3) / 2) * (pa.rows - 2);
    bool* validapplepos = malloc(totalpositions * sizeof(bool));
    validapplepos[0] = false;
    for (int i = 0; i < totalpositions; i++) validapplepos[i] = true;
    loc apple = placeapple(validapplepos, pa);

    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvaddch(apple.row, apple.col, 'a');
    attroff(COLOR_PAIR(2));
    
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

        // Update head direction and representation 
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
            // this line can seg fault?
            previous.row = snake->tail->position->row;
            //
            previous.col = snake->tail->position->col;
            mvaddch(snake->head->position->row, snake->head->position->col, 
                    dir == North || dir == South ? ':' : '-');
            snake->tail->position->row = snake->head->position->row;
            snake->tail->position->col = snake->head->position->col;
            snake->tail->next = snake->head->next;
            // this line causing seg fault
            snake->head->next->prev = snake->tail;
            //
            tempnodeptr = snake->tail->prev; 
            snake->tail->prev = snake->head;
            snake->tail = tempnodeptr; 
            snake->head->next = snake->head->next->prev; 
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

        // cast to unsigned char and strip off chtype formatting to compare characters
        if ((unsigned char)next_space[0] == ('-' & A_CHARTEXT) 
            || (unsigned char)next_space[0] == (':' & A_CHARTEXT) 
            || !in_bounds(*snake->head->position, pa)) {
            mvaddch(snake->head->position->row, snake->head->position->col, head);
            mvaddch(previous.row, previous.col, ' ');
            refresh();
            return;
        }
        if ((unsigned char)next_space[0] == ('a' & A_CHARTEXT)) {
            if (snake->length == 1) {
                mvaddch(previous.row, previous.col, (dir == North || dir == South) ? ':' : '-');
                snake->tail = malloc(sizeof(struct node));
                snake->tail->prev = snake->head;
                snake->head->next = snake->tail;
            } else {
                snake->tail->next = malloc(sizeof(struct node));
                snake->tail->next->prev = snake->tail;
                snake->tail = snake->tail->next;
            }
            snake->tail->position = malloc(sizeof(loc));
            snake->tail->position->row = previous.row;
            snake->tail->position->col = previous.col;  
            snake->length += 1;

            updatevalidpos(validapplepos, &pa, snake->head->position, NULL);
            apple = placeapple(validapplepos, pa);
            attron(COLOR_PAIR(2));
            mvaddch(apple.row, apple.col, 'a');
            attron(COLOR_PAIR(1));
        } else {
            mvaddch(previous.row, previous.col, ' ');
            updatevalidpos(validapplepos, &pa, snake->head->position, &previous);
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
