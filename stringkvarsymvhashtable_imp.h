

#ifndef STRINGKVARSYMVHASHTABLE_IMP
#define STRINGKVARSYMVHASHTABLE_IMP

#include "clike_types.h"


typedef struct var_sym {
    String id;
    Type type;
} VarSym;

void printString(String *data);
void printVarSym(VarSym *data);
unsigned int hashString(String *key);
void freeString(String *data);
int compareString(String *a,String *b);
String* dupString(String *data);
VarSym* dupVarSym(VarSym *data);

void freeVarSym(VarSym *data);

#endif

