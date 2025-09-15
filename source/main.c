#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>


#define NCURSES_WIDECHAR 1
#include <ncurses.h>

#include <pokkenizer.h>
#include <extras.h>


void free_memory(DoubleLinkList *head) {
    DoubleLinkList *next = head;
    while (next->next != NULL) {
        DoubleLinkList *now = next;
        next = next->next;
        free(now);
    }
}


int main(const int argc, char **argv) {
    // TOKENIZING THE FILE AND CREATE A DOUBLE LINKED LIST
    if (argc < 2) {printf("Provide a file name."); return 1;}
    /*
    Customized Pokkenizer, the persistent delimiters stays with the token.
    It splits after the delimiter.
    Tabs get converted to spaces.
    */
    char *token_buffer = get_pokke_buffer(argv[1], "", "\n");
    size_t line_length = strlen_asm(token_buffer);
    if (line_length > 200) {
        printf("Crossed the line length limit at line 0.");
        return 2;
    }
    // currently copying the null-terminator into the strings, necessary?
    DoubleLinkList *head = calloc(1, sizeof(DoubleLinkList) + 200);
    head->line_length = line_length - 1; // save without the null-terminator
    DoubleLinkList *previous = head;
    DoubleLinkList *next;
    memcpy(previous->line, token_buffer, line_length);
    token_buffer += line_length;

    int line_counter = 1;
    while (*token_buffer != '\0') {
        line_counter++;
        if (*token_buffer == 10) {
            // handle new line
            if (*(token_buffer+2) != 10) {
                line_counter--;
                if (*(token_buffer+2) == 0) line_counter++; // handle EOF
            }
        }
        line_length = strlen_asm(token_buffer);
        if (line_length > 200) {
            printf("Crossed the line length limit at line %d.", line_counter);
            return 2;
        }

        next = calloc(1, sizeof(DoubleLinkList) + 200);
        memcpy(next->line, token_buffer, line_length);
        next->line_length = line_length - 1; // save without the null-terminator
        previous->next = next;
        next->previous = previous;
        previous = next;
        token_buffer += line_length;
    }
    moommap();

    // SETTING UP NCURSES
    uint16_t row, col; // store number of rows and columns of the terminal
    uint8_t current_col = 0; // for left-right scrolling
    // for adjusting cursor position when moving to a short line
    // unused for now
    // uint8_t col_max = 0;
    initscr();
    curs_set(0); // trying to disable ncurses cursor, but doesn't work
    raw();
    noecho();
    keypad(stdscr, TRUE);
    nonl(); // no new line, Enter key is handled differently
    clear();
    #define ctrl(x)  ((x) & 0x1f) // macro to get value for Ctrl + <key>
    int key;
    DoubleLinkList *top;
    top = head;
    Cursor cursor = {top, 0, 0, 0};
    uint8_t running = 1;

    // PROGRAM LOOP
    while (running == 1) {

        getmaxyx(stdscr, row, col); // get rows and columns, make it resizable

        next = top;
        for (int i = 0; i < row-1;) { // printing the lines
            if (next->line_length >= current_col) // is scrolled to the right?
                mvprintw(i, 0, "%s", next->line + current_col);
            else mvprintw(i, 0, "%s", next->line + next->line_length);

            if (next == cursor.text_row) { // cursor printing
                // cursor in a line, including \n char
                if (cursor.text_col < cursor.text_row->line_length) {
                    for (int j = current_col; j < cursor.text_col; ++j) {
                        mvprintw(i, j, "%c", next->line[j]);
                    }
                    char c = next->line[cursor.text_col];
                    if (c != 10) mvaddch(i, cursor.text_col, A_STANDOUT | c);
                    else         mvaddch(i, cursor.text_col, A_STANDOUT | 32);
                    for (int j = cursor.text_col+1; j < next->line_length; ++j) {
                        mvprintw(i, j, "%c", next->line[j]);
                    }
                }
                // cursor jumping to shorter line, adjust cursor pos
                else {
                    // raise(SIGTRAP);
                    mvprintw(i, 0, "%s", next->line);
                    line_length = next->line_length-1;
                    mvaddch(i, line_length, A_STANDOUT | 32);
                    cursor.text_col = line_length;
                    cursor.screen_col = line_length;
                }
            }
            next = next->next;
            if (next == 0) break;
            ++i;
        }
        refresh();


        // keys
        key = wgetch(stdscr);
        for (int i = 0; i < col-1;) { // clear status line
            mvprintw(row-1, i++, " ");
        }
        switch (key) {
            case ctrl('q'): // quit
                running = 0;

            case 258: // arrow_down
                if (cursor.text_row->next != 0) { // check EOF
                    mvprintw(row-1, 0, "arrow_down");
                    if (cursor.screen_row < row - 2) {
                        cursor.text_row = cursor.text_row->next;
                        cursor.screen_row++;
                    }
                    else if (top->next != 0) {
                        top = top->next;
                        cursor.text_row = cursor.text_row->next;
                    }
                    else mvprintw(row-1, 0, "arrow_down what just happened");
                    // if (cursor.text_col >= cursor.text_row->line_length)
                    //     move_cursor_eol(cursor);
                    break;
                }
                mvprintw(row-1, 0, "arrow_down reached end of file");
                break;

            case 259: // arrow_up
                if (cursor.text_row->previous != 0) { // check BOF
                    mvprintw(row-1, 0, "arrow_up");
                    if (cursor.screen_row != 0) {
                        cursor.text_row = cursor.text_row->previous;
                        cursor.screen_row--;
                    }
                    else if (top->previous != 0) {
                        top = top->previous;
                        cursor.text_row = cursor.text_row->previous;
                    }
                    else mvprintw(row-1, 0, "arrow_up what just happened");
                    // if (cursor.text_col >= cursor.text_row->line_length)
                    //     move_cursor_eol(cursor);
                    break;
                }
                mvprintw(row-1, 0, "arrow_up reached beginning of file");
                break;

            case 260: // arrow_left
                if (cursor.text_col > 0) {
                    mvprintw(row-1, 0, "arrow_left");
                    cursor.text_col--;
                    // col_max--;
                    if (cursor.screen_col > 0) cursor.screen_col--;
                    else current_col--;
                    break;
                }
                mvprintw(row-1, 0, "arrow_left reached beginning of line");
                break;

            case 261: // arrow_right
                if (cursor.text_col < cursor.text_row->line_length - 1) { // -1 to account for line_length 1 starting index 0, it's weird
                    mvprintw(row-1, 0, "arrow_right");
                    cursor.text_col++;
                    // col_max++;
                    if (cursor.screen_col >= col) current_col++;
                    else cursor.screen_col++;
                    break;
                }
                mvprintw(row-1, 0, "arrow_right reached end of line");
                break;

            case 338: // page down
                mvprintw(row-1, 0, "page_down WILL LEAD TO UNDEFINED BEHAVIOUR");
                for (int i = 0; i < row -3;) {
                    if (top->next != 0) top = top->next;
                    else if (top->next == 0) break;
                    ++i;
                }
                break;

            case 339: // page up
                mvprintw(row-1, 0, "page_up WILL LEAD TO UNDEFINED BEHAVIOUR");
                for (int i = 0; i < row -3;) {
                    if (top->previous != 0) top = top->previous;
                    else if (top->previous == 0) break;
                    ++i;
                }
                break;

            case 262: // POS 1
                break;

            case 360: // END
                break;

            case 555: // alt left_arrow
                mvprintw(row-1, 0, "alt left_arrow");
                if (current_col > 0) {
                    current_col--;
                    cursor.text_col--;
                    break;
                }
                mvprintw(row-1, 0 ,"alt left_arrow reached beginning of line");
                break;

            case 570: // alt right_arrow
                mvprintw(row-1, 0, "alt right_arrow");
                if (current_col < 200) {
                    current_col++;
                    cursor.text_col++;
                    break;
                }
                mvprintw(row-1, 0 ,"alt right_arrow reached end of line");
                break;

            case ctrl('c'): // unhandled
                mvprintw(row-1, 0, "ctrl c");
                break;

            case ctrl('j'): // unhandled
                mvprintw(row-1, 0, "ctrl j");
                break;

            case ctrl('f'): // unhandled
                mvprintw(row-1, 0, "ctrl f");
                break;

            case KEY_ENTER: // idk, this is never called
                mvprintw(row-1, 0, "enter: %d", key);
                break;

            default:
                char c = scancode_lut[key];
                if (c != 0) {
                    // add characters to line
                    char buf = cursor.text_row->line[cursor.text_col+1];
                    for (int i = cursor.text_col; i >= cursor.text_row->line_length;) {
                        //if (i+1 >= 200) {
                        //    mvprintw(row-1, 0, "reached line length limit");
                        //    break;
                        //}
                        //buf[0] = cursor.text_row->line[i+1];
                        cursor.text_row->line[i+1] = cursor.text_row->line[i];
                        i++;
                    }
                    cursor.text_row->line[cursor.text_col] = c;
                    cursor.text_row->line_length++;
                    mvaddnstr(row-1, 0, &c, 1);
                }
                else mvprintw(row-1, 0, "key: %d", key);
                break;
        }
    }

    endwin();
    free_memory(head);
    printf("otsu\n");
    return 0;
}
