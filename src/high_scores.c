#include <ncurses.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

void show_high_scores(WINDOW* hsw, WINDOW* dummy, const char* size, const char* speed) {
    char* title = malloc(sizeof(char) * (strlen("High Scores: ") + strlen(size) + strlen(speed) + 10));
    strcat(title, "High Scores: ");
    strcat(title, size);
    strcat(title, "/");
    strcat(title, speed);
    mvwaddstr(hsw, 0, 0, title); 
    wrefresh(hsw);
    free(title);
    wgetch(dummy);
}
