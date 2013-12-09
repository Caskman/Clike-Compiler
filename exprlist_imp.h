
#ifndef EXPRLIST_IMP
#define EXPRLIST_IMP


typedef int Type;
typedef struct expr_list ExprList;
typedef struct sym Sym;

// type is the evaluated type of this expression
// intcon and doublecon are self explanatory
// operator is the ascii or enum value of the operator
// sym is the variable or function symbol of accession or a call
// left and right are for the expression trees
// index_expr is used with sym for array indexing
// arg_list is used with sym for function call parameters
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
