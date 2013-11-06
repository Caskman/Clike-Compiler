
#ifndef EXPRLIST_IMP
#define EXPRLIST_IMP

typedef int Type;
typedef struct expr {
    Type type;
} Expr;

void printExpr(Expr *data);
void freeExpr(Expr *data);
Expr* dupExpr(Expr *data);
int compareExpr(Expr *a,Expr *b);

#endif
