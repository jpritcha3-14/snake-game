#ifndef HELPER_FUNCTIONS_HEADER
#define HELPER_FUNCTIONS_HEADER
#include <ncurses.h>

void freesnake(struct linked_list* snake);
int toarrayidx(int row, int col, wloc pa); 
loc tolocation(int idx, wloc pa);    
void updatevalidpos(bool* valid, wloc pa, loc* toremove, loc* toadd); 
loc placeapple(const bool* valid, wloc pa); 
wloc get_play_area(WINDOW* w);
void draw_border(wloc pa, char c);
bool in_bounds(loc l, wloc w);

#endif
