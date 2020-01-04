#ifndef HIGH_SCORES_HEADER
#define HIGH_SCORES_HEADER 
#include <ncurses.h>

void show_high_scores(WINDOW* hsw, WINDOW* dummy, const char* size, const char* speed);
void add_high_score(WINDOW* hsw, int score, const char* size, const char* speed);

#endif
