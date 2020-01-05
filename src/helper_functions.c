#include <stdlib.h>

#include "structs.h"
#include "helper_functions.h"

// free all memory from the snake data structure
void freesnake(struct linked_list* snake) {
    if (snake->length > 1) {
        struct node* cur = snake->tail;
        for (int i = 0; i < snake->length - 2; i++) {
            free(cur->position);
            cur = cur->prev;
            free(cur->next);
        }
        free(cur->position);
        free(cur);
    }
    free(snake->head->position);
    free(snake->head);
    free(snake);
}

// 2D to 1D coordinate transformation
int toarrayidx(int row, int col, int cols) {
    int translatedcol = (col - 2) / 2;
    int translatedrow = row - 1;
    return translatedrow * cols + translatedcol; 
}

// 1D to 2D coordinate transformation
loc tolocation(int idx, int cols) {    
    int translatedcol = idx % cols;
    int translatedrow = idx / cols;
    loc loconscreen;
    loconscreen.row = translatedrow + 1;
    loconscreen.col = (translatedcol * 2) + 2;
    return loconscreen;
}

// Update valid locations for apple placement
void updatevalidpos(int* valid, const loc* toremove, const loc* toadd, int cols) {
    valid[toarrayidx(toremove->row, toremove->col, cols)] = 0;
    if (toadd) {
        valid[toarrayidx(toadd->row, toadd->col, cols)] = 1;
    }
}

// Return a valid location to place the next apple
loc placeapple(const int* valid, int rows, int cols) {
    int len = rows * cols;
    int apple = rand() % len;
    while (!valid[apple]) {
        apple = (apple + 1) % len; 
    } 
    return tolocation(apple, cols);
}

wloc* get_logo_area(WINDOW* w) {
    wloc* logo_area = malloc(sizeof(wloc)); 
    if (logo_area == NULL) {
        endwin(); 
        printf("Malloc failed (logo)\n");
        exit(1);
    }
    int wrows, wcols;
    getmaxyx(w, wrows, wcols);
    // Terminal too small for logo
    if (wrows < 22 || wcols < 60) {
        free(logo_area);
        return NULL;
    }
    logo_area->x = (wcols - 60) / 2;
    logo_area->y = (wrows - 22) / 2;
    logo_area->rows = 11;
    logo_area->cols = 60;
    return logo_area;
}


// Determine where to place the menu 
// based on the size of the terminal
wloc* get_menu_area(WINDOW* w, wloc* la) {
    wloc* menu_area = malloc(sizeof(wloc)); 
    int wrows, wcols;
    getmaxyx(w, wrows, wcols);
    if (wrows < 11 || wcols < 25 || menu_area == NULL) {
        endwin();
        printf("Terminal too small\n");
        exit(1);
    }
    menu_area->x = (wcols / 2) - 7;
    menu_area->y = la != NULL ? la->y + 12 : (wrows / 2) - 5;
    menu_area->cols = (wcols / 2);
    menu_area->rows = (wrows / 2);
    return menu_area;
}

// Determine the location of the high score window
wloc* get_hs_area(WINDOW* w) {
    wloc* hs_area = malloc(sizeof(wloc));
    int wrows, wcols;
    getmaxyx(w, wrows, wcols);
    hs_area->x = (wcols / 2) - 13;
    hs_area->y = (wrows / 2) - 5;
    hs_area->cols = (wcols / 2);
    hs_area->rows = (wrows / 2);
    return hs_area;
}

// Draw a border around the paly area
void draw_border(WINDOW* pa, char c) {
    int rows, cols;
    getmaxyx(pa, rows, cols);
    wmove(pa, 0, 0);
    wattron(pa, A_BOLD);

    for (int i = 0; i < cols; i++) {
        waddch(pa, c); 
    }
    for (int i = 1; i < rows; i++) {
        mvwaddch(pa, i, 0, c);
        mvwaddch(pa, i, cols - 1, c);
    }
    wmove(pa, rows - 1, 0);
    for (int i = 0; i < cols - 1; i++) {
        waddch(pa, c); 
    }
    attroff(A_BOLD);
    wrefresh(pa);
}

// Determine if a given location is in the play area
bool in_bounds(loc l, int rows, int cols) {
    return (l.row > 0 && l.row < rows - 1
            && l.col > 0 && l.col < cols - 1);
}
