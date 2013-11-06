

#ifndef STRINGKSYMVHASHTABLE_IMP
#define STRINGKSYMVHASHTABLE_IMP

#include "clike_types.h"
#include "stringlist.h"
#include "typelist.h"

typedef struct sym {
    int sym_type;
    String id;
    int is_defined;
    Type type;
    TypeList *args_type_list;
    StringList *args_id_list;
} Sym;

void printString(String *data);
void printSym(Sym *data);
unsigned int hashString(String *key);
void freeString(String *data);
int compareString(String *a,String *b);
String* dupString(String *data);
Sym* dupSym(Sym *data);

void freeSym(Sym *data);

#endif

