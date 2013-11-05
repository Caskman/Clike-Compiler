
#ifndef CLIKE
#define CLIKE
#define CASKDEBUG 0

#include "funcsymlist.h"
#include "clike_types.h"
#include "stringkfuncsymvhashtable.h"
#include "stringkvarsymvhashtable.h"

void yyerror(char const *s);
void logg(char const *s);
int yylex_destroy();
int yyparse();


extern int yylineno;
extern int inComments;
extern int line;
extern StringKFuncSymVHashTable *global_func_table;
extern StringKVarSymVHashTable *global_var_table;
extern Type current_type;


int main();
void checkFuncSymTable(StringKFuncSymVHashTable *table,FuncSymList *list);
void checkFuncSymListForDups(FuncSymList *list);
FuncSym* newFProt(Type type,char *id,TypeList *type_list);
void setTypesFuncSymList(FuncSymList *list,Type type);

#endif
