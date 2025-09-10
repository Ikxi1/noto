#include <extras.h>

void move_cursor_eol(Cursor cursor) {
    while (cursor.text_col >= cursor.text_row->line_length) {
        cursor.text_col--;
        cursor.screen_col--;
    }
}