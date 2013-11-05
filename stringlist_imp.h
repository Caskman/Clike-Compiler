
#ifndef STRINGLIST_IMP
#define STRINGLIST_IMP

#include "clike_types.h"

void printString(String *data);
void freeString(String *data);
String* dupString(String *data);
int compareString(String *a,String *b);

#endif
