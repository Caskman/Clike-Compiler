
#ifndef FUNCSYMLIST_IMP
#define FUNCSYMLIST_IMP

#include "stringkfuncsymvhashtable_imp.h"

void printFuncSym(FuncSym *data);
void freeFuncSym(FuncSym *data);
FuncSym* dupFuncSym(FuncSym *data);
int compareFuncSym(FuncSym *a,FuncSym *b);

#endif
