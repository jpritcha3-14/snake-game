#include <time.h>
#include <stdlib.h> 
#include <stdio.h>

#include "structs.h"
#include "helper_functions.h"
#include "game_loop.h"

void play_game(wloc pa, WINDOW* dummy) {
    clear();
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
    cur.tv_nsec = 100000000;
    chtype *next_space = malloc(2 * sizeof(chtype));

    // Initialize snake
    struct linked_list* snake = malloc(sizeof(struct linked_list));
    snake->head = malloc(sizeof(struct node));
    snake->head->position = malloc(sizeof(loc));
    snake->head->position->row = pa.y + 1;
    snake->head->position->col = pa.x + 2;
    snake->length = 1;

    // Intialize apple position array and place first apple
    bool* validapplepositions = malloc(pa.rows * pa.cols * sizeof(bool));
    validapplepositions[0] = false;
    for (int i = 1; i < pa.rows * pa.cols; i++) validapplepositions[i] = true;
    loc apple = placeapple(validapplepositions, pa);

    // Draw snake, apple, and score
    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvaddch(apple.row, apple.col, 'a');
    mvaddch(pa.y-1, pa.x+7, '0');
    attron(COLOR_PAIR(1));
    mvaddstr(pa.y-1, pa.x, "SCORE:");
    mvaddch(snake->head->position->row, snake->head->position->col, head);
    refresh();

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
        } else if (snake->length == 2) {
            previous.row = snake->tail->position->row;
            previous.col = snake->tail->position->col;
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

        // Draw a new body segment where head was
        mvaddch(snake->head->position->row, 
                snake->head->position->col, 
                dir == North || dir == South ? ':' : '-');

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
        mvinchnstr(snake->head->position->row, snake->head->position->col, next_space, 1);


        // Grow snake body, update score, and place a new apple when 
        // the snake head reaches the apple on screen.
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
            // Remove the last body segment if there was no apple collision
            mvaddch(previous.row, previous.col, ' ');
            mvinchnstr(snake->head->position->row, snake->head->position->col, next_space, 1);
            updatevalidpos(validapplepositions, pa, snake->head->position, &previous);
        }

        // Check for collision with self or play area boundary
        // Draw final frame and free memory before returning 
        if ((unsigned char)next_space[0] == ('-' & A_CHARTEXT) 
            || (unsigned char)next_space[0] == (':' & A_CHARTEXT) 
            || !in_bounds(*snake->head->position, pa)) {
            mvaddch(previous.row, previous.col, ' ');
            attron(COLOR_PAIR(2));
            mvaddch(snake->head->position->row, snake->head->position->col, 'x');
            refresh();
            free(next_space);
            free(validapplepositions);
            freesnake(snake);
            wtimeout(dummy, -1);
            attroff(COLOR_PAIR(2));
            return;
        }

        //Place head in new location, refresh the screen, and sleep for an interval 
        mvaddch(snake->head->position->row, snake->head->position->col, head);
        refresh();
        nanosleep(&cur, NULL);
    }
}
