#ifndef POKKENIZER_H
#define POKKENIZER_H
#include <stddef.h>

char *get_pokke_buffer(const char *file_name, const char *delimiters1, const char *delimiters2);
void moommap();
size_t strlen_asm(char *string);

#endif
