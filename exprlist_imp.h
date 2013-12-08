
#ifndef EXPRLIST_IMP
#define EXPRLIST_IMP


typedef int Type;
typedef struct expr_list ExprList;
typedef struct sym Sym;

typedef struct expr {
    Type type;
    int intcon,operator;
    double doublecon;
    Sym *sym;
    struct expr *left,*right,*index_expr;
    ExprList *arg_list;
} Expr;

void printExpr(Expr *data);
void freeExpr(Expr *data);
Expr* dupExpr(Expr *data);
int compareExpr(Expr *a,Expr *b);

#endif
