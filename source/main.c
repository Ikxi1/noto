#include <pokkenizer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>

struct link_list{
	void *previous;
	struct link_list *next;
	char line[];
};

void free_memory(struct link_list *head) {
	struct link_list *next = head;

	while (next->next != NULL) {
		struct link_list *now = next;
		next = next->next;
		free(now);
	}
}

void print_lines(struct link_list *head) {
	struct link_list *next = head;

	while (next->next != NULL) {
		printf(next->line);
		next = next->next;
	}
}

int main(const int argc, char **argv) {
	if (argc < 2) {printf("Provide a file name."); return 1;}
	const char delimiters1 = 0;
	const char delimiters2 = 10;
	char *token_buffer = get_pokke_buffer(argv[1], &delimiters1, &delimiters2);

	size_t line_length = strlen_asm(token_buffer);
	if (line_length > 200) {printf("Crossed the line length limit at line 0."); return 2;}
	struct link_list *previous = calloc(1, sizeof(struct link_list) + line_length);
	struct link_list *head = previous;
	struct link_list *next;
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

		next = calloc(1, sizeof(struct link_list) + line_length);
		memcpy(next->line, token_buffer, line_length);
		previous->next = next;
		next->previous = previous;
		previous = next;
		token_buffer += line_length;
	}
	moommap();

	uint16_t row, col; // store number of rows and columns of the terminal
	initscr();
	getmaxyx(stdscr, row, col); // get rows and columns
	next = head;

	//while (1) {
		for (int i = 0; i < row; ++i) {
			mvprintw(i, 0, "%s", next->line);
			next = next->next;
		}
		refresh();
	//}

	getch();
	endwin();
	free_memory(head);
	return 0;
}
