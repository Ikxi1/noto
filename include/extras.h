#ifndef EXTRAS_H
#define EXTRAS_H
#include <signal.h>
#include <stdint.h>


typedef struct double_link_list{
    struct double_link_list *previous;
    struct double_link_list *next;
    uint8 line_length;
    char line[];
} DoubleLinkList;

typedef struct cursor{
    DoubleLinkList *text_row;
    uint8 text_col;
    uint8 screen_row;
    uint8 screen_col;
} Cursor;

extern const char scancode_lut[];

extern volatile sig_atomic_t running;

// Move the cursor to the end of the line, when moving up or down
// and the cursor is on now on a shorter line
void move_cursor_eol(Cursor cursor);

void handle_sigint(int sig);
void handle_sigterm(int sig);

void free_memory(DoubleLinkList *head);

#endif
