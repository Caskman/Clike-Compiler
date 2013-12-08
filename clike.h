
#ifndef CLIKE
#define CLIKE
#define CASKDEBUG 0

#include "symlist.h"
#include "clike_types.h"
#include "stringksymvhashtable.h"
#include "exprlist_imp.h"
#include "exprlist.h"
#include "assglist_imp.h"
#include "stmtlist_imp.h"

void yyerror(char const *s);
void logg(char const *s);
int yylex_destroy();
int yyparse();


extern int yylineno;
extern int inComments;
extern int line;
extern StringKSymVHashTable *global_sym_table;
extern Sym *current_function;
extern StringKSymVHashTable *current_scope;

// extern Type current_type;


int main();
void checkAndLogSymTable(StringKSymVHashTable *table,SymList *list);
void checkMatchingFuncSym(StringKSymVHashTable *table,Sym *new_func_sym,Sym *sym_entry);
void checkSymListForDups(SymList *list);
Sym* newFProt(Type type,char *id,TypeList *type_list);
SymList* setTypesSymList(SymList *list,Type type);
Sym* newVarDecl(Type type,String id,int array_size);
Sym* newFunctionHeader(Type type,String id,StringList *id_list);
void checkFunction(StringKSymVHashTable *table,Sym *function);
SymList* changeIDListToSymListAndSetType(StringList *id_list,Type type);
void reconcileArgsCreateScope(Sym *func,SymList *decl_list);
Sym* checkAndLogFuncWithSymTable(StringKSymVHashTable *table,Sym *func);
Expr* newExpr(Type type);
Expr* resolveExpr(int operator,Expr *a,Expr *b);
void setScope(StringKSymVHashTable *table);
Expr* idToExpr(String s,ExprList *expr_list,Expr *expr);
Assg* createAssg(String s,Expr *index,Expr *expr);
Stmt* createListStmt(StmtList *list);
Stmt* createAssgStmt(Assg *assg);
Stmt* createReturnStmt(Expr *expr);
Stmt* createCallStmt(String id,ExprList *arg_list);
Stmt* createForStmt(Stmt *for_control,Stmt *stmt);
Stmt* createForControl(Assg *assg1,Expr *expr,Assg *assg2);
Stmt* createIfStmt(Expr *expr,Stmt *else_clause,Stmt *stmt);
Stmt* createWhileStmt(Expr *expr,Stmt *stmt);
Stmt* createElseStmt(Stmt *stmt);
void addLocalsToScope(SymList *symlist);
char* getTypeString(Type type);
int checkTypeCompat(int type1,int type2);
void cleanUpScope();
int checkCallArgs(Sym *func,ExprList *arg_list);

#endif
