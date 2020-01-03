#ifndef HELPER_FUNCTIONS_HEADER
#define HELPER_FUNCTIONS_HEADER
#include <ncurses.h>

void freesnake(struct linked_list* snake);
int toarrayidx(int row, int col, int cols); 
loc tolocation(int idx, int cols);    
void updatevalidpos(int* valid, const loc* toremove, const loc* toadd, int cols); 
loc placeapple(const int* valid, int rows, int cols); 
wloc* get_logo_area(WINDOW* w);
wloc* get_hs_area(WINDOW* w);
wloc* get_menu_area(WINDOW* w, wloc* la);
void draw_border(WINDOW* pa, char c);
bool in_bounds(loc l, int rows, int cols);

#endif
