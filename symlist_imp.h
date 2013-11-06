
#ifndef SYMLIST_IMP
#define SYMLIST_IMP

#include "stringksymvhashtable_imp.h"

void printSym(Sym *data);
void freeSym(Sym *data);
Sym* dupSym(Sym *data);
int compareSym(Sym *a,Sym *b);

#endif
