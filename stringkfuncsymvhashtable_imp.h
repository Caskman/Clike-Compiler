

#ifndef STRINGKFUNCSYMVHASHTABLE_IMP
#define STRINGKFUNCSYMVHASHTABLE_IMP

#include "clike_types.h"
#include "stringlist.h"
#include "typelist.h"

typedef struct func_sym {
    String id;
    int is_defined;
    Type return_type;
    TypeList *args_type_list;
    StringList *args_id_list;
} FuncSym;

void printString(String *data);
void printFuncSym(FuncSym *data);
unsigned int hashString(String *key);
void freeString(String *data);
int compareString(String *a,String *b);
String* dupString(String *data);
FuncSym* dupFuncSym(FuncSym *data);

void freeFuncSym(FuncSym *data);

#endif

