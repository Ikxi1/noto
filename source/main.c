#include <pokkenizer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


struct double_link_list{
	// void *previous;
	struct double_link_list *next;
	char line[];
};

void free_memory(struct double_link_list *head) {
	struct double_link_list *next = head;

	while (next->next != NULL) {
		struct double_link_list *now = next;
		next = next->next;
		free(now);
	}
}

void print_lines(struct double_link_list *head) {
	struct double_link_list *next = head;

	while (next->next != NULL) {
		printf(next->line);
		next = next->next;
	}
}

int main(const int argc, char **argv) {
	if (argc < 1) {printf("Provide a file name."); return 1;}
	const char delimiters1 = 0;
	const char delimiters2 = 10;
	char *token_buffer = get_pokke_buffer(argv[1], &delimiters1, &delimiters2);

	size_t line_length = strlen_asm(token_buffer);
	if (line_length > 200) {printf("Crossed the line length limit at line 0."); return 2;}
	struct double_link_list *previous = calloc(1, sizeof(struct double_link_list) + line_length);
	struct double_link_list *head = previous;
	memcpy(previous->line, token_buffer, line_length);
	token_buffer += line_length;

	while (*token_buffer != '\0') {
		static int line_counter = 1;
		line_counter++;
		if (*token_buffer == 10) {
			// handle new line
			line_counter--; // otherwise newlines get detected as their own individual line
		}
		line_length = strlen_asm(token_buffer); // add one for \0
		if (line_length > 200) {printf("Crossed the line length limit at line %d.", line_counter); return 2;}

		struct double_link_list *next = calloc(1, sizeof(struct double_link_list) + line_length);

		memcpy(next->line, token_buffer, line_length);

		previous->next = next;

		previous = next;

		token_buffer += line_length;
	}

	while (1) print_lines(head);

	free_memory(head);

	moommap(token_buffer);

	return 0;
}
