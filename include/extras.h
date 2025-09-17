#ifndef EXTRAS_H
#define EXTRAS_H
#include <wchar.h>
#include <stddef.h>
#include <stdint.h>


typedef struct double_link_list{
    struct double_link_list *previous;
    struct double_link_list *next;
    uint8 line_length;
    wchar_t line[];
} DoubleLinkList;

typedef struct cursor{
    DoubleLinkList *text_row;
    uint8 text_col;
    uint8 screen_row;
    uint8 screen_col;
} Cursor;

extern const wchar_t scancode_lut[];

// Move the cursor to the end of the line, when moving up or down
// and the cursor is on now on a shorter line
void move_cursor_eol(Cursor cursor);

#endif
