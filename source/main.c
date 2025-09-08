#include <pokkenizer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>


typedef struct double_link_list{
    struct double_link_list *previous;
    struct double_link_list *next;
    uint8_t line_length;
    char line[];
} DoubleLinkList;


typedef struct cursor{
    DoubleLinkList *row;
    uint8_t col;
} Cursor;


void free_memory(DoubleLinkList *head) {
    DoubleLinkList *next = head;
    while (next->next != NULL) {
        DoubleLinkList *now = next;
        next = next->next;
        free(now);
    }
}


int main(const int argc, char **argv) {
    if (argc < 2) {printf("Provide a file name."); return 1;}
    /*
    Customized Pokkenizer, the persistent delimiters stays with the token.
    It splits after the delimiter.
    Tabs get converted to spaces.
    */
    char *token_buffer = get_pokke_buffer(argv[1], "", "\n");
    size_t line_length = strlen_asm(token_buffer);
    if (line_length > 200) {printf("Crossed the line length limit at line 0."); return 2;}
    DoubleLinkList *head = calloc(1, sizeof(DoubleLinkList) + line_length);
    head->line_length = line_length;
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
        line_length = strlen_asm(token_buffer); // add one for \0
        if (line_length > 200) {printf("Crossed the line length limit at line %d.", line_counter); return 2;}

        next = calloc(1, sizeof(DoubleLinkList) + line_length);
        memcpy(next->line, token_buffer, line_length);
        next->line_length = line_length;
        previous->next = next;
        next->previous = previous;
        previous = next;
        token_buffer += line_length;
    }
    moommap();

    uint16_t row, col; // store number of rows and columns of the terminal
    uint8_t current_col = 0; // for left-right scrolling
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    nonl(); // no new line, Enter key is handled differently
    #define ctrl(x)  ((x) & 0x1f) // macro to get value for Ctrl + <key>
    int key;
    DoubleLinkList *top;
    top = head;
    Cursor cursor = {top, 0};
    uint8_t running = 1;

    while (running == 1) {
        getmaxyx(stdscr, row, col); // get rows and columns, make it resizable
        next = top;
        for (int i = 0; i < row-2;) {
            if (next->line_length >= current_col)
                mvprintw(i, 0, "%s", next->line+current_col);
            else mvprintw(i, 0, "%s", next->line+next->line_length);
            next = next->next;
            if (next == 0) break;
            ++i;
        }

        key = wgetch(stdscr);
        mvprintw(row-1, 0, "                                ");
        switch (key) {
            case KEY_ENTER:
                mvprintw(row-1, 0, "key: %d", key);
                break;
            case ctrl('c'):
                mvprintw(row-1, 0, "key: ctrl c");
                break;
            case ctrl('q'):
                printf("Goodbye\n");
                running = 0;
            case ctrl('j'):
                mvprintw(row-1, 0, "key: ctrl j");
                break;
            case ctrl('f'):
                mvprintw(row-1, 0, "key: ctrl f");
                break;
            case 258:
                if (top->next != 0) top = top->next; break; // check EOF
            case 259:
                if (top->previous != 0) top = top->previous; break; // check BOF
            case 338:
                for (int i = 0; i < row -3;) {
                    if (top->next != 0) top = top->next;
                    else if (top->next == 0) break;
                    ++i;
                }
                break;
            case 339:
                for (int i = 0; i < row -3;) {
                    if (top->previous != 0) top = top->previous;
                    else if (top->previous == 0) break;
                    ++i;
                }
                break;
            case 555:
                if (current_col > 0) current_col--; break;
            case 570:
                if (current_col < 200) current_col++; break;
            default:
                mvprintw(row-1, 0, "key: %d", key);
                break;
        }

        refresh();
    }

    endwin();
    free_memory(head);
    return 0;
}
