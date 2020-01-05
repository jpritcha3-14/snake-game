#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <sqlite3.h>

struct rowscore {
    int row;
    int score;
};

int get_num_entries(const char* table);
void append_high_score(const char* table, const char* name, const int score); 
void update_high_score(const char* table, const int, const char* name, const int score); 
struct rowscore* get_min_row_and_score(const char* table);

void show_high_scores(WINDOW* hsw, WINDOW* dummy, const char* size, const char* speed) {
    char table[32];
    char title[32];
    wattron(hsw, COLOR_PAIR(1));
    sprintf(table, "%s%s", speed, size);
    sprintf(title, "High Scores: %s/%s", speed, size);
    mvwaddstr(hsw, 0, 0, title);

    wattron(hsw, COLOR_PAIR(2));
    mvwaddstr(hsw, 2, 3, "Name");
    mvwaddstr(hsw, 2, 15, "Score");


    sqlite3 *db;
    sqlite3_stmt *res;
    sqlite3_stmt *table_res;
    int row = 3;

    // Open db connection
    int rc = sqlite3_open("high_scores.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db)); 
        sqlite3_close(db);
    }

    // Create table if it doesn't exit 
    char table_sql[128];
    sprintf(table_sql, "CREATE TABLE IF NOT EXISTS %s(name TEXT, score INTEGER);", table);
    rc = sqlite3_prepare_v2(db, table_sql, -1, &table_res, 0);
    sqlite3_step(table_res);
    sqlite3_finalize(table_res);

    char sql[128]; 
    sprintf(sql, "SELECT name, score FROM %s ORDER BY score DESC;", table);

    rc = sqlite3_prepare_v2(db, sql, -1, &res, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute count prepare: %s\n", sqlite3_errmsg(db)); 
        return;
    }

    // Print all returned rows
    char buff[10];
    while(sqlite3_step(res) == SQLITE_ROW) {
        wattron(hsw, COLOR_PAIR((row + 1) % 2 + 1));
        strcpy(buff, "");
        sprintf(buff, "%d", sqlite3_column_int(res, 1));
        mvwaddstr(hsw, row, 3, (const char*)sqlite3_column_text(res, 0));
        mvwaddstr(hsw, row, 15, buff);
        row += 1;
    }
    
    sqlite3_finalize(res);
    sqlite3_close(db);

    wrefresh(hsw);
    wgetch(dummy); // Wait for user input to return
}

void add_high_score(WINDOW* hsw, int score, const char* size, const char* speed) {
    char table[32];
    sprintf(table, "%s%s", speed, size);
    int entries = get_num_entries(table);
    struct rowscore* rs = get_min_row_and_score(table);
    if (entries < 10 || score > rs->score) {
        echo(); 
        curs_set(2);
        char hs_msg[32];
        sprintf(hs_msg, "High Score: %d", score);
        char msg[] = "Enter your name";
        char name[11];
        wattron(hsw, COLOR_PAIR(1));
        mvwprintw(hsw, 0, 0, hs_msg);
        wattron(hsw, COLOR_PAIR(2));
        mvwprintw(hsw, 2, 0, msg);
        wattroff(hsw, COLOR_PAIR);
        wmove(hsw, 3, 0);
        wgetnstr(hsw, name, 10);
        if (entries < 10) {
            append_high_score(table, name, score);
        } else {
            update_high_score(table, rs->row, name, score);
        }
        curs_set(0);
        noecho();
    }
    free(rs);
}

int get_num_entries(const char* table) {
    sqlite3 *db;
    sqlite3_stmt *table_res;
    sqlite3_stmt *count_res;

    // Open db connection
    int rc = sqlite3_open("high_scores.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db)); 
        sqlite3_close(db);
        return -1;
    }

    // Create table if it doesn't exit 
    char table_sql[128];
    sprintf(table_sql, "CREATE TABLE IF NOT EXISTS %s(name TEXT, score INTEGER);", table);
    rc = sqlite3_prepare_v2(db, table_sql, -1, &table_res, 0);
    sqlite3_step(table_res);
    sqlite3_finalize(table_res);

    // Get number of values in table 
    char* count_sql = malloc(sizeof(char)*1024);
    int entry_count = 0;
    sprintf(count_sql, "SELECT COUNT(*) FROM %s;", table);
    rc = sqlite3_prepare_v2(db, count_sql, -1, &count_res, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute count prepare: %s\n", sqlite3_errmsg(db)); 
        return 1;
    }
    rc = sqlite3_step(count_res);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Failed to execute count step: %s\n", sqlite3_errmsg(db)); 
        return 1;
    }
    entry_count = sqlite3_column_int(count_res, 0); 
    sqlite3_finalize(count_res);
    return entry_count;
}

void append_high_score(const char* table, const char* name, const int score) {

    sqlite3 *db;
    sqlite3_stmt *res;
    
    // Open db connection
    int rc = sqlite3_open("high_scores.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db)); 
        sqlite3_close(db);
        return;
    }
    
    // Insert values into table
    char sql[128];
    sprintf(sql, "INSERT INTO %s(name, score) VALUES(?, ?);", table);

    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) {
        if (sqlite3_bind_text(res, 1, name, strlen(name), SQLITE_STATIC) != SQLITE_OK) {
            fprintf(stderr, "Problem with text bind: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(res);
            sqlite3_close(db);
            return;
        }
        if (sqlite3_bind_int(res, 2, score) != SQLITE_OK) {
            fprintf(stderr, "Problem with int bind: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(res);
            sqlite3_close(db);
            return;
        }
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db)); 
        sqlite3_finalize(res);
        sqlite3_close(db);
        return;
    }

    sqlite3_step(res);
    sqlite3_finalize(res);
    sqlite3_close(db);
}


void update_high_score(const char* table, const int lowrow, const char* name, const int score) {
    sqlite3 *db;
    sqlite3_stmt *res;
    
    // Open db connection
    int rc = sqlite3_open("high_scores.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db)); 
        sqlite3_close(db);
        return;
    }

    // Update lowest value in table 
    char sql[128];
    sprintf(sql, "UPDATE %s SET name=?, score=? WHERE rowid=?;", table);

    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) {
        if (sqlite3_bind_text(res, 1, name, strlen(name), SQLITE_STATIC) != SQLITE_OK) {
            fprintf(stderr, "Problem with text bind: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(res);
            sqlite3_close(db);
            return;
        }
        if (sqlite3_bind_int(res, 2, score) != SQLITE_OK) {
            fprintf(stderr, "Problem with int bind: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(res);
            sqlite3_close(db);
            return;
        }
        if (sqlite3_bind_int(res, 3, lowrow) != SQLITE_OK) {
            fprintf(stderr, "Problem with int bind: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(res);
            sqlite3_close(db);
            return;
        }
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db)); 
        sqlite3_finalize(res);
        sqlite3_close(db);
        return;
    }

    sqlite3_step(res);
    sqlite3_finalize(res);
    sqlite3_close(db);
}

struct rowscore* get_min_row_and_score(const char* table) {
    sqlite3 *db;
    sqlite3_stmt *res;
    
    // Open db connection
    int rc = sqlite3_open("high_scores.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db)); 
        sqlite3_close(db);
        return NULL;
    }

    // Get lowest score and its rowid
    char sql[128];
    sprintf(sql, "SELECT rowid, score FROM %s ORDER BY score ASC LIMIT 1", table);
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    struct rowscore* rs = malloc(sizeof(struct rowscore));
    
    sqlite3_step(res);
    rs->row = sqlite3_column_int(res, 0);
    rs->score = sqlite3_column_int(res, 1);
    sqlite3_finalize(res);
    sqlite3_close(db);
    return rs;
}
