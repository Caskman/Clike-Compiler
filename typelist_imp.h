
#ifndef TYPELIST_IMP
#define TYPELIST_IMP

#include "clike_types.h"

void printType(Type *data);
void freeType(Type *data);
Type* dupType(Type *data);
int compareType(Type *a,Type *b);

#endif
