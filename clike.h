
#ifndef CLIKE
#define CLIKE
#define CASKDEBUG 0

#include "symlist.h"
#include "clike_types.h"
#include "stringksymvhashtable.h"

void yyerror(char const *s);
void logg(char const *s);
int yylex_destroy();
int yyparse();


extern int yylineno;
extern int inComments;
extern int line;
extern StringKSymVHashTable *global_sym_table;
extern Type current_type;


int main();
void checkAndLogSymTable(StringKSymVHashTable *table,SymList *list);
void checkMatchingFuncSym(StringKSymVHashTable *table,Sym *new_func_sym,Sym *sym_entry);
void checkSymListForDups(SymList *list);
Sym* newFProt(Type type,char *id,TypeList *type_list);
SymList* setTypesSymList(SymList *list,Type type);
Sym* newVarDecl(Type type,String id,int array_size);

#endif
