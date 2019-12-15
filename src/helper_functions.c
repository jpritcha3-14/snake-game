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
int toarrayidx(int row, int col, wloc pa) {
    int translatedcol = (col - pa.x - 2) / 2;
    int translatedrow = row - pa.y - 1;
    return translatedrow * ((pa.cols - 1) / 2) + translatedcol; 
}

// 1D to 2D coordinate transformation
loc tolocation(int idx, wloc pa) {    
    int translatedcol = idx % pa.usefulcols;
    int translatedrow = idx / pa.usefulcols;
    loc loconscreen;
    loconscreen.row = translatedrow + pa.y + 1;
    loconscreen.col = (translatedcol * 2) + pa.x + 2;
    return loconscreen;
}

// Update valid locations for apple placement
void updatevalidpos(int* valid, wloc pa, const loc* toremove, const loc* toadd) {
    valid[toarrayidx(toremove->row, toremove->col, pa)] = 0;
    if (toadd) {
        valid[toarrayidx(toadd->row, toadd->col, pa)] = 1;
    }
}

// Return a valid location to place the next apple
loc placeapple(const int* valid, wloc pa) {
    int len = pa.rows * pa.usefulcols;
    int apple = rand() % len;
    while (!valid[apple]) {
        apple = (apple + 1) % len; 
    } 
    return tolocation(apple, pa);
}

// Determine the size of the play area 
// based on the size of the terminal display
wloc* get_menu_area(WINDOW* w) {
    wloc* menu_area = malloc(sizeof(wloc)); 
    int wrows, wcols;
    getmaxyx(w, wrows, wcols);
    if (wrows < 10 || wcols < 20 || menu_area == NULL) {
        return NULL;
    }
    menu_area->x = (wcols / 2) - 10;
    menu_area->y = (wrows / 2) - 5;
    menu_area->cols = (wcols / 2);
    menu_area->rows = (wrows / 2);
    return menu_area;
}

// Draw a border around the paly area
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

// Determine if a given location is in the play area
bool in_bounds(loc l, wloc w) {
    return (l.row > w.y && l.row < w.y + w.rows + 1
            && l.col > w.x && l.col < w.x + w.cols + 1);
}
