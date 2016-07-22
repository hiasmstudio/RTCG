#ifndef _SHAREH_
#define _SHAREH_

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "CGTShare.h"

#define HANDLE FILE*

extern void lowerCase(char *to, const char *from);
extern void upperCase(char *to, const char *from);

extern void int_to_str(long i, char *buf);

extern int min(int x, int y);

#endif
