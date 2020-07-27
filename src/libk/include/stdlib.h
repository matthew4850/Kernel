#ifndef _STDLIB_H
#define _STDLIB_H 1

char* itoa(int value, char* str, int base);
int atoi(const char* str);
long int strtol (const char* str, char** endptr, int base);
char* utoa(unsigned int value, char * str, int base);

#endif