#ifndef EXTRAS_H
#define EXTRAS_H
#include <stddef.h>
#include <stdint.h>


typedef struct double_link_list{
    struct double_link_list *previous;
    struct double_link_list *next;
    uint8_t line_length;
    char line[];
} DoubleLinkList;

typedef struct cursor{
    DoubleLinkList *text_row;
    uint8_t text_col;
    uint8_t screen_row;
    uint8_t screen_col;
} Cursor;

extern const char scancode_lut[];

// Move the cursor to the end of the line, when moving up or down
// and the cursor is on now on a shorter line
void move_cursor_eol(Cursor cursor);

#endif
