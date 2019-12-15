#ifndef HELPER_FUNCTIONS_HEADER
#define HELPER_FUNCTIONS_HEADER
#include <ncurses.h>

void freesnake(struct linked_list* snake);
int toarrayidx(int row, int col, wloc pa); 
loc tolocation(int idx, wloc pa);    
void updatevalidpos(int* valid, wloc pa, const loc* toremove, const loc* toadd); 
loc placeapple(const int* valid, wloc pa); 
wloc* get_menu_area(WINDOW* w);
void draw_border(wloc pa, char c);
bool in_bounds(loc l, wloc w);

#endif
