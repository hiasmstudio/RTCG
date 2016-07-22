#include "share.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void lowerCase(char *to, const char *from) {
	while(*from) {
		if(*from >= 'A' && *from <= 'Z')
			*to = (*from - 'A') + 'a';
		else
			*to = *from;
		from++;
		to++;
	}
	*to = '\0';
}

void upperCase(char *to, const char *from) {
	while(*from) {
		if(*from >= 'a' && *from <= 'z')
			*to = (*from - 'a') + 'A';
		else
			*to = *from;
		from++;
		to++;
	}
	*to = '\0';
}

void int_to_str(long i, char *buf) {
	sprintf(buf,  "%d",  (int)i);
}

int min(int x, int y) {
	return x < y ? x : y;
}