
#ifndef ASSGLIST_IMP
#define ASSGLIST_IMP

typedef struct sym Sym;

// sym is the lhs id
// index is the expression determining the index for the lhs id
// expr is the rhs expression
typedef struct assg {
    Sym *sym;
    Expr *index,*expr;
} Assg;

// void printAssg(Assg *data);
// void freeAssg(Assg *data);
// Assg* dupAssg(Assg *data);
// int compareAssg(Assg *a,Assg *b);

#endif
