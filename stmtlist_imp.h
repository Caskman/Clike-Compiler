
#ifndef STMTLIST_IMP
#define STMTLIST_IMP

#define STMT_IF 34
#define STMT_WHILE 234
#define STMT_FOR 678
#define STMT_FORCON 927
#define STMT_RETURN 654
#define STMT_ASSG 32
#define STMT_STMTLIST 84
#define STMT_FUNCCALL 9283
#define STMT_ELSE 8274
#define STMT_EMPTY 2746

#include "exprlist_imp.h"
#include "exprlist.h"
#include "assglist_imp.h"
#include "stringksymvhashtable_imp.h"

typedef struct stmt_list StmtList;

typedef struct stmt {
    int stmt_type;
    StmtList *stmt_list;
    struct stmt *else_clause,*stmt;
    Expr *expr;
    Assg *assg,*assg2;
    Sym *sym;
    ExprList *expr_list;
} Stmt;

void printStmt(Stmt *data);
void freeStmt(Stmt *data);
Stmt* dupStmt(Stmt *data);
int compareStmt(Stmt *a,Stmt *b);

#endif
