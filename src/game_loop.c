#include <time.h>
#include <stdlib.h> 
#include <stdio.h>

#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"

int play_game(WINDOW* pa, WINDOW* sa, WINDOW* dummy, enum speed s) {
    int actualrows, actualcols;
    int rows, cols;
    int finalscore = -1;
    getmaxyx(pa, actualrows, actualcols);
    rows = actualrows - 2;
    cols = (actualcols - 3) / 2;

    draw_border(pa, '!');

    loc previous;
    char head = '<';
    char ch = 'd';
    char key;
    char prevkey;
    char nextkey = ERR;
    char nxt;
    char scorebuffer[6];
    struct node* tempfrontbody;
    enum direction dir = East;
    struct timespec cur;
    cur.tv_sec = 0;
    switch (s) {
        case slow:
           cur.tv_nsec = 400000000;
           break; 
        case normal:
           cur.tv_nsec = 200000000;
           break; 
        case fast:
           cur.tv_nsec = 100000000;
           break; 
    }
    chtype *next_space = malloc(2 * sizeof(chtype));

    // Initialize snake
    struct linked_list* snake = malloc(sizeof(struct linked_list));
    snake->head = malloc(sizeof(struct node));
    snake->head->position = malloc(sizeof(loc));
    snake->head->position->row = 1;
    snake->head->position->col = 2;
    snake->length = 1;

    // Intialize apple position array and place first apple
    int* validapplepositions = malloc(rows * cols * sizeof(int));
    validapplepositions[0] = 0;
    for (int i = 1; i < rows * cols; i++) validapplepositions[i] = 1;
    loc apple = placeapple(validapplepositions, rows, cols);

    // Draw snake, apple, and score
    wattron(pa, COLOR_PAIR(2));
    wattron(pa, A_BOLD);
    mvwaddch(pa, apple.row, apple.col, 'a');
    wattron(pa, COLOR_PAIR(1));
    mvwaddch(pa, snake->head->position->row, snake->head->position->col, head);

    wattron(sa, A_BOLD);
    wattron(sa, COLOR_PAIR(1));
    mvwaddstr(sa, 0, 0, "SCORE:");
    wattron(sa, COLOR_PAIR(2));
    mvwaddch(sa, 0, 7, '0');

    wrefresh(pa);
    wrefresh(sa);

    // Block until first key pressed, then set input to non-blocking
    nxt = wgetch(dummy);
    wtimeout(dummy, 0);
    
    for(;;) {

        // Read and validate inputs. If 2+ inputs recived at once,
        // use the second to last in the current iteration, and
        // the last ('nextkey') in the next iteration. This allows
        // for fast inputs on tight turns to be processed correctly.
        prevkey = ERR;
        key = ERR;
        if (nextkey != ERR) {
            ch = nextkey;
            nextkey = ERR;
            fflush(stdin); // clear buffer 
            //can also be implemented as: while (wgetch(dummy) != ERR);
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
                
        // Update the snake head direction and representation 
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

        // Update the body linked list.  The body is represented
        // as a circular linked list.  As the snake moves, the 
        // tail moved to the front of the body.
        if (snake->length == 1) {
            previous.row = snake->head->position->row;
            previous.col = snake->head->position->col;
        } else {
            previous.row = snake->tail->position->row;
            previous.col = snake->tail->position->col;
            snake->tail->position->row = snake->head->position->row;
            snake->tail->position->col = snake->head->position->col;
            snake->tail = snake->tail->prev;
            mvwaddch(pa, snake->head->position->row, snake->head->position->col, 
                    dir == North || dir == South ? ':' : '-');
        }

        // Move the head to new position
        if (dir == West) {
            snake->head->position->col -= 2;
        } else if (dir == North) {
            snake->head->position->row -= 1;
        } else if (dir == East) {
            snake->head->position->col += 2;
        } else {
            snake->head->position->row += 1;
        }

        // Get character present on screen in new head location
        mvwinchnstr(pa, snake->head->position->row, snake->head->position->col, next_space, 1);

        // Grow snake body, update score, and place a new apple when 
        // the snake head reaches the apple on screen.
        if ((unsigned char)next_space[0] == ('a' & A_CHARTEXT)) {
            if (snake->length == 1) {
                mvwaddch(pa, previous.row, previous.col, (dir == North || dir == South) ? ':' : '-');
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

            // Check for win condition
            if (snake->length == rows * cols) {
                wattron(pa, COLOR_PAIR(2));
                mvwaddch(pa, snake->head->position->row, snake->head->position->col, 'W');
                wrefresh(pa);
                finalscore = snake->length - 1;
                free(next_space);
                free(validapplepositions);
                freesnake(snake);
                wtimeout(dummy, -1);
                wattroff(pa, COLOR_PAIR(2));
                return finalscore;
            }

            sprintf(scorebuffer, "%d", snake->length - 1);
            updatevalidpos(validapplepositions, snake->head->position, NULL, cols);
            apple = placeapple(validapplepositions, rows, cols);
            wattron(pa, COLOR_PAIR(2));
            mvwaddch(pa, apple.row, apple.col, 'a');
            mvwaddstr(sa, 0, 7, scorebuffer);
            wrefresh(sa);
            wattron(pa, COLOR_PAIR(1));
        } else {
            // Remove the last body segment if there was no apple collision
            mvwaddch(pa, previous.row, previous.col, ' ');
            mvwinchnstr(pa, snake->head->position->row, snake->head->position->col, next_space, 1);

            // The following is a VERY important bounds check!  The head can only 
            // be out of bounds on the last game loop iteration before the game 
            // ends, but this can (in certain circumstances) write to a memory 
            // location outside of the bounds pointed to by validapplepositions.
            // This corrupts the heap and crashes the process when we attempt to 
            // free memory in the following conditional.  This is an excellent 
            // example of a subtle bug that can make C debugging so difficult!  
            // It took me several hours of head scratching to find.
            if (in_bounds(*snake->head->position, actualrows, actualcols)) { 
                updatevalidpos(validapplepositions, snake->head->position, &previous, cols);
            }
        }

        // Check for collision with self or play area boundary. Draw the final 
        // frame and free memory before returning to the menu.
        if ((unsigned char)next_space[0] == ('-' & A_CHARTEXT) 
            || (unsigned char)next_space[0] == (':' & A_CHARTEXT) 
            || !in_bounds(*snake->head->position, actualrows, actualcols)) {
            mvwaddch(pa, previous.row, previous.col, ' ');
            wattron(pa, COLOR_PAIR(2));
            mvwaddch(pa, snake->head->position->row, snake->head->position->col, 'x');
            wrefresh(pa);
            finalscore = snake->length - 1;
            free(next_space);
            free(validapplepositions);
            freesnake(snake);
            wtimeout(dummy, -1);
            wattroff(pa, COLOR_PAIR(2));
            return finalscore;
        }

        // Place head in new location, refresh the screen, and sleep for an interval 
        wattron(pa, COLOR_PAIR(1));
        mvwaddch(pa, snake->head->position->row, snake->head->position->col, head);
        wrefresh(pa);
        nanosleep(&cur, NULL);
    }
return finalscore; 
}
