
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "clike.h"
#include "stringksymvhashtable.h"
#include "clike.tab.h"
#include "clike_types.h"
#include "exprlist_imp.h"
#include "stmtlist.h"

int line = 1;
int inComments = 0;
int is_duplicate_function = 0;
Sym *current_function;
// Type current_type = -1;
StringKSymVHashTable *current_scope;
StringKSymVHashTable *global_sym_table;


//================String=====================

void printString(String *data) {
    printf("%s",(char*)*data);
}

void freeString(String *data) {
    free(*data);
    free(data);
}

int compareString(String *a,String *b) {
    return strcmp((char*)*a,(char*)*b);
}

String* dupString(String *data) {
    String *s = (String*)malloc(sizeof(String));
    *s = (char*)malloc((sizeof(char)*strlen(*data)) + 1);
    strcpy(*s,*data);
    return s;
}

unsigned int hashString(String *key) {
    char *s = (char*)*key;
    int length = strlen(s),i;
    unsigned int hashvalue = 7;

    for (i = 0; i < length; i++) {
        hashvalue = hashvalue*31 + s[i];
    }

    return hashvalue;
}

//==========================================

//================Sym================

Sym* newSym(String id,int sym_type,int is_defined,Type type,TypeList *args_type_list,StringList *args_id_list,StringKSymVHashTable *scope,int array_size) {
    Sym *f = (Sym*)malloc(sizeof(Sym));
    f->id = id;
    f->sym_type = sym_type;
    f->is_defined = is_defined;
    f->type = type;
    f->args_type_list = args_type_list;
    f->args_id_list = args_id_list;
    f->scope = scope;
    f->array_size = array_size;
    return f;
}

void printSym(Sym *data) {
    // printf("");
}
void freeSym(Sym *data) {
    free(data->id);
    freeTypeList(data->args_type_list);
    freeStringList(data->args_id_list);
    freeStringKSymVHashTable(data->scope);
    free(data);
}

void freeSymShallow(Sym *sym) {
    freeTypeListOnly(sym->args_type_list);
    freeStringListOnly(sym->args_id_list);
    freeStringKSymVHashTableOnly(sym->scope);
}

int compareSym(Sym *a,Sym *b) {
    return compareString(&a->id,&b->id);
}

Sym* dupSym(Sym *data) { // TODO doesn't seem to used ever
    Sym *f = (Sym*)malloc(sizeof(Sym));
    f->id = (char*)strdup(data->id);
    f->sym_type = data->sym_type;
    f->is_defined = data->is_defined;
    f->type = data->type;
    if (data->args_type_list != NULL) f->args_type_list = dupTypeListShallow(data->args_type_list);
    if (data->args_id_list != NULL) f->args_id_list = dupStringListShallow(data->args_id_list);
    f->scope = NULL;
    f->array_size = data->array_size;
    if (data->body != NULL) f->body = dupStmtListShallow(data->body);
    return f;
}

//==========================================

//===============Type===================

void printType(Type *data) {
    printf("%d",*data);
}

void freeType(Type *data) {
    free(data);
}

int compareType(Type *a,Type *b) {
    return *a - *b;
}

Type* dupType(Type *data) {
    Type *t = (Type*) malloc(sizeof(Type));
    *t = *data;
    return t;
}

//======================================

//===============Expr==================

void printExpr(Expr *data) {

}
void freeExpr(Expr *data) {
    if (data == NULL) return;
    free(data);
}
Expr* dupExpr(Expr *data) {
    if (data == NULL) return NULL;
    Expr *newe = (Expr*)malloc(sizeof(Expr));
    newe->type = data->type;
    return newe;
}
int compareExpr(Expr *a,Expr *b) {
    return a->type - b->type;
}

Expr* newExpr(Type type) {
    Expr *newe = (Expr*)malloc(sizeof(Expr));
    newe->type = type;
    return newe;
}

Expr* emptyExpr() {
    return newExpr(VOID);
}

//======================================

//===============Assg==================

Assg* newAssg(Sym *sym,Expr *index,Expr *expr) {
    Assg *newa = (Assg*)malloc(sizeof(Assg));
    newa->sym = sym;
    newa->index = index;
    newa->expr = expr;
    return newa;
}

Assg* emptyAssg() {
    return newAssg(NULL,NULL,NULL);
}


//======================================

//===============Stmt==================

Stmt* newStmt(int stmt_type, StmtList *stmt_list,Stmt *else_clause,Stmt *stmt,Expr *expr,Assg *assg,Assg *assg2,Sym *sym,ExprList *expr_list) {
    Stmt *news = (Stmt*) malloc(sizeof(Stmt));
    news->stmt_type = stmt_type;
    news->stmt_list = stmt_list;
    news->else_clause = else_clause;
    news->stmt = stmt;
    news->expr = expr;
    news->assg = assg;
    news->assg2 = assg2;
    news->sym = sym;
    news->expr_list = expr_list;
    return news;
}

Stmt* emptyStmt() {
    return newStmt(STMT_EMPTY,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

void printStmt(Stmt *data) {

}

void freeStmt(Stmt *data) {

}

Stmt* dupStmt(Stmt *data) {
    return NULL;
}

int compareStmt(Stmt *a,Stmt *b) {
    return 0;
}

//======================================

//================SEMANTICS=============================

int hasReturnStmt(StmtList *stmt_list) {
    StmtNode *snode;
    for (snode = stmt_list->head->next; snode != NULL; snode = snode->next) {
        switch (snode->data->stmt_type) {
        case STMT_STMTLIST:
            if (hasReturnStmt(snode->data->stmt_list)) return 1;
            break;
        case STMT_RETURN:
            return 1;
            break;
        }
    }
    return 0;
}

void finalizeFunction(Sym *func,StmtList *stmt_list) {
    if ((func->type != VOID && hasReturnStmt(stmt_list)) || func->type == VOID) {
        func->body = stmt_list;
    } else {
        yyerror("");
        fprintf(stderr, "\tnon-void function <%s> must have a return\n",func->id);
    }
}

void addLocalsToScope(SymList *symlist) {
    StringKSymVHashTable *scope = current_scope;
    SymNode *snode;

    for (snode = symlist->head->next; snode != NULL; snode = snode->next) {
        if (getValueStringKSymVHashTable(scope,&snode->data->id) != NULL) {
            yyerror("");
            fprintf(stderr, "\tvariable <%s> is already defined\n", snode->data->id);
        } else {
            putStringKSymVHashTable(scope,dupString(&snode->data->id),snode->data);
        }
    }
}

Stmt* createListStmt(StmtList *list) {
    return newStmt(STMT_STMTLIST,list,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

Stmt* createAssgStmt(Assg *assg) {
    return newStmt(STMT_ASSG,NULL,NULL,NULL,NULL,assg,NULL,NULL,NULL);
}

Stmt* createReturnStmt(Expr *expr) {
    if (expr != NULL && !checkTypeCompat(expr->type,current_function->type)) {
        yyerror("");
        fprintf(stderr, "\texpression type <%s> does not match function return type <%s>\n",getTypeString(expr->type),getTypeString(current_function->type));
    }
    return newStmt(STMT_RETURN,NULL,NULL,NULL,expr,NULL,NULL,NULL,NULL);
}

// int checkArgList(Sym *func,ExprList *arg_list) {
//     ExprNode *enode; TypeNode *tnode; int i;
//     for (i = 1, tnode = func->args_type_list->head->next,enode = arg_list->head->next;
//          tnode != NULL && enode != NULL; tnode = tnode->next,enode = enode->next,i++) {
//         if (*tnode->data != enode->data->type) {
//             yyerror("");
//             fprintf(stderr,"\tmismatched argument type for argument %d\n",i);
//             return 0;
//         }
//     }
//     return 1;
// }

Stmt* createCallStmt(String id,ExprList *arg_list) {
    Sym *sym;
    if ((sym = getValueStringKSymVHashTable(global_sym_table,&id)) != NULL) { // check global symbol table
        if (sym->sym_type == CLIKE_FUNC) { // symbol must be a function
            if (checkCallArgs(sym,arg_list)) { // argument types must match function arguments type
                if (sym->type == VOID) { // type must be void to be a standalone call
                    return newStmt(STMT_FUNCCALL,NULL,NULL,NULL,NULL,NULL,NULL,sym,arg_list);
                } else {
                    yyerror("");
                    fprintf(stderr, "\tcannot call a non-void function outside of an assignment\n");
                }
            } else {
                // yyerror("");
                // fprintf(stderr,"\t\n");
            }
        } else {
            yyerror("");
            fprintf(stderr,"\t<%s> is not a function\n",id);
        }
    } else {
        yyerror("");
        fprintf(stderr,"\tfunction <%s> does not exist\n",id);
    }

    freeExprListOnly(arg_list);
    return emptyStmt();
}

Stmt* createForStmt(Stmt *for_control,Stmt *stmt) {

    Stmt *new_stmt = newStmt(STMT_FOR,NULL,NULL,stmt,for_control->expr,for_control->assg,for_control->assg2,NULL,NULL);
    // TODO free(for_control); goes here
    return new_stmt;
}

Stmt* createForControl(Assg *assg1,Expr *expr,Assg *assg2) {

    if (expr->type != BOOL) {
        expr = newExpr(BOOL);
        yyerror("");
        fprintf(stderr,"\tfor loop expression condition must evaluate to a boolean\n");
    }

    return newStmt(STMT_FORCON,NULL,NULL,NULL,expr,assg1,assg2,NULL,NULL);
}

Stmt* createElseStmt(Stmt *stmt) {
    return newStmt(STMT_ELSE,NULL,NULL,stmt,NULL,NULL,NULL,NULL,NULL);
}

Stmt* createIfStmt(Expr *expr,Stmt *else_clause,Stmt *stmt) {
    if (expr->type != BOOL) {
        yyerror("");
        fprintf(stderr, "\tconditional must evaluate to a boolean\n");
        expr = newExpr(BOOL);
    }
    return newStmt(STMT_IF,NULL,else_clause,stmt,NULL,NULL,NULL,NULL,NULL);
}

Stmt* createWhileStmt(Expr *expr,Stmt *stmt) {
    if (expr->type != BOOL) {
        yyerror("");
        fprintf(stderr, "\twhile condition must evaluate to a boolean\n");
        expr = newExpr(BOOL);
    }
    return newStmt(STMT_WHILE,NULL,NULL,stmt,expr,NULL,NULL,NULL,NULL);
}

Assg* createAssg(String s,Expr *index,Expr *expr) {
    Sym *sym;
    if ((sym = getValueStringKSymVHashTable(current_scope,&s)) == NULL) {
        if ((sym = getValueStringKSymVHashTable(global_sym_table,&s)) == NULL) {
            yyerror("");
            fprintf(stderr,"\tidentifier <%s> does not exist\n",s);
        }
    }

    if (sym != NULL) {
        if (sym->sym_type == CLIKE_VAR) {// check that the symbol is a variable
            if ((index != NULL && sym->array_size != -1) || (index == NULL && sym->array_size == -1)) { // check for array index if needed
                if ((index != NULL && index->type == INT) || index == NULL) { // check that the index is an integer
                    if (checkTypeCompat(sym->type,expr->type)) {// check that the lhs and rhs types are the same
                        return newAssg(sym,index,expr);
                    } else {
                        yyerror("");
                        fprintf(stderr,"\ttypes are incompatible for assignment: <%s> and <%s>\n",getTypeString(sym->type),getTypeString(expr->type));
                    }
                } else {
                    yyerror("");
                    fprintf(stderr,"\tarray index must evaluate to an integer value\n");
                }
                
            } else {
                if (index == NULL && sym->array_size != -1) {
                    yyerror("");
                    fprintf(stderr,"\tmust provide an array index for variable <%s>\n",s);
                } else if (index != NULL && sym->array_size == -1) {
                    yyerror("");
                    fprintf(stderr,"\tat variable <%s>: cannot index a variable that is not an array\n",s);
                }
            }
        } else {
            yyerror("");
            fprintf(stderr,"\tcannot assign a value to function <%s>\n",s);
        }
    }
    return emptyAssg();
}

int checkTypeCompat(int type1,int type2) {
    if (type1 == type2) return 1;
    else if ((type1 == INT && type2 == CHAR) || (type1 == CHAR && type2 == INT)) return 1;
    // else if (type1 == DOUBLE_DECL && type2 == INT) return 1;
    return 0;
}

/* checks the symbol table against the list of function symbols; each function symbol could be a prototype or a function definition */
void checkAndLogSymTable(StringKSymVHashTable *table,SymList *list) {
    SymNode *node;
    Sym *sym;

    for (node = list->head->next; node != NULL; node = node->next) {
        sym = getValueStringKSymVHashTable(table,&node->data->id);
        if (sym == NULL) {// match does not exist
            String *id = dupString(&node->data->id);
            putStringKSymVHashTable(table,id,node->data);
            // printf("INT:%d DOUBLE_DECL:%d FLOAT:%d CHAR:%d\n",INT,DOUBLE_DECL,FLOAT,CHAR);
            // printTypeList(node->data->args_type_list);
        } else {// match exists
            if (node->data->sym_type != sym->sym_type) {
                yyerror("");
                if (sym->sym_type == CLIKE_VAR) {
                    fprintf(stderr,"\tvariable <%s> is already defined, so a function named <%s> cannot exist\n",sym->id,sym->id);
                } else {
                    fprintf(stderr,"\tfunction <%s> is already defined, so a variable named <%s> cannot exist\n",sym->id,sym->id);
                }
            } else if (sym->sym_type == CLIKE_FUNC) {
                checkMatchingFuncSym(table,node->data,sym);
            } else if (sym->sym_type == CLIKE_VAR) {
                yyerror("");
                fprintf(stderr,"\t<%s> has already been defined\n",sym->id);
            }
        }
    }
    freeSymListOnly(list);
}

/* Given a new function symbol and a matched existing function symbol entry from the table,
 * checks if the existing entry (prototype) can be updated with the new symbol (definition) */
void checkMatchingFuncSym(StringKSymVHashTable *table,Sym *new_func_sym,Sym *sym_entry) {
    if (new_func_sym->is_defined) { // if the current function is not a function prototype TODO this clause will probably never execute
        if (sym_entry->is_defined) {// the function grabbed from the table is not a prototype
            // incorrect because you can't define a function twice
            yyerror("");
            fprintf(stderr,"\tTHIS SHOULD NOT APPEARduplicate definition of <%s>\n",sym_entry->id);
        } else {// the function grabbed from the table is a prototype
            // prototype was already in the table and now you have a definition -- CORRECT
            if (sym_entry->type != new_func_sym->type) { // prototype and definition return types do not match
                yyerror("");
                fprintf(stderr,"\tTHIS SHOULD NOT APPEAR<%s> function return type does not match prototype\n",sym_entry->id);
            } else if (compareStringList(sym_entry->args_id_list,new_func_sym->args_id_list) != 0) { 
                yyerror(""); // TODO not sure if this is  correct, prototype doesn't have a list of ids for its arguments, just a list of types
                fprintf(stderr,"\tTHIS SHOULD NOT APPEAR<%s> function argument list does not match prototype argument list\n",sym_entry->id);
            } else {// if all's good, replace the prototype entry with the function definition
                freeSym(removeStringKSymVHashTable(table,&sym_entry->id));
                putStringKSymVHashTable(table,dupString(&new_func_sym->id),new_func_sym);
                printf("THIS SHOULD NOT APPEAR\n");
            }
        }
    } else { // if the current function is a function prototype
        if (sym_entry->is_defined) { // the function grabbed from the table is not a prototype
            // Function has already been defined
            yyerror("");
            fprintf(stderr,"\tfunction <%s> has already been defined\n",new_func_sym->id);
            // free(message);
        } else { // the function grabbed from the table is a prototype
            // Function prototype has already been declared

            yyerror("");
            fprintf(stderr,"\tfunction prototype <%s> has already been defined\n",sym_entry->id);
        }
        freeSymShallow(new_func_sym);
    }
}

/* checks the list to see if there are any Sym objects with the same id */
void checkSymListForDups(SymList *list) {
    SymNode *cur,*node;
    for (cur = list->head->next; cur != NULL; cur = cur->next) {
        for (node = cur->next; node != NULL; node = node->next) {
            if (compareString(&cur->data->id,&node->data->id) == 0) { // duplicate found
                yyerror("");
                fprintf(stderr,"\tduplicate function prototype identifiers\n");
            }
        }
    }
}

void checkFunction(StringKSymVHashTable *table,Sym *function) { // TODO this function never seems to be used
    SymList *list= makeSESymList(function);
    checkAndLogSymTable(table,list);
    freeSymListOnly(list);
}

Sym* checkAndLogFuncWithSymTable(StringKSymVHashTable *table,Sym *func) {
    Sym *prototype = getValueStringKSymVHashTable(table,&func->id);
    is_duplicate_function = 0;
    if (prototype != NULL) { // symbol exists
        if (!prototype->is_defined) { // is an actual prototype
            if (prototype->type != func->type) { // must have the same type
                yyerror("");
                fprintf(stderr, "\tprototype and definition types must match\n");
            }

            prototype->type = func->type;
            prototype->args_id_list = dupStringListShallow(func->args_id_list);
            freeSymShallow(func);
            return prototype;
        } else { // not a prototype, error
            yyerror("");
            fprintf(stderr, "\tfunction <%s> is already defined\n",prototype->id);
            is_duplicate_function = 1;
            return func;
        }
        // func->args_type_list = dupTypeList(prototype->args_type_list);
        // freeSym(prototype);
    } else { // symbol does not exist
        putStringKSymVHashTable(table,dupString(&func->id),func);
        return func;
    }
    // putStringKSymVHashTable(table,dupString(&func->id),func);
}

void cleanUpScope() {
    if (is_duplicate_function) {
        freeSymShallow(current_function);
    }

    setScope(global_sym_table);
}

/* construct a Sym object */
Sym* newFProt(Type type,char *id,TypeList *type_list) {
    return newSym(id,CLIKE_FUNC,0,type,type_list,NULL,NULL,-1);
}

Sym* newVarDecl(Type type,String id,int array_size) {
    return newSym(id,CLIKE_VAR,1,type,NULL,NULL,NULL,array_size);
}

Sym* newFunctionHeader(Type type,String id,StringList *id_list) {
    return newSym(id,CLIKE_FUNC,1,type,NULL,id_list,NULL,-1);
}

SymList* setTypesSymList(SymList *list,Type type) {
    SymNode *node;
    for (node = list->head->next; node != NULL; node = node->next) {
        node->data->type = type;
    }
    return list;
}

SymList* stringListToSymList(StringList *string_list) {
    SymList *sym_list = newSymList();
    StringNode *node;
    for (node = string_list->head->next; node != NULL; node = node->next) {
        // String *dupstring = dupString(node->data);
        appendSym(sym_list,newSym(*node->data,CLIKE_VAR,1,-1,NULL,NULL,NULL,-1));
        // free(dupstring);
    }
    return sym_list;
}

SymList* changeIDListToSymListAndSetType(StringList *id_list,Type type) {
    return setTypesSymList(stringListToSymList(id_list),type);
}

// Right now, all Strings in the args_id_list are duplicated and used to create a symbol for each arg. 
void reconcileArgsCreateScope(Sym *func,SymList *decl_list) {
    // SymNode *sym_node;
    StringNode *string_node;
    SymNode *sym_node,*sym_node2;
    TypeNode *type_node;

    // create function scope
    func->scope = newStringKSymVHashTable(5);

    SymList *parameters = newSymList();

    // add all of the id parameters to the scope and add them to the parameter list at the same time
    for (string_node = func->args_id_list->head->next; string_node != NULL; string_node = string_node->next) {
        // String *dupstring = dupString(string_node->data);
        Sym *newsym = newSym(*string_node->data,CLIKE_VAR,1,-1,NULL,NULL,NULL,-1);
        // free(dupstring);
        appendSym(parameters,newsym);
        putStringKSymVHashTable(func->scope,dupString(string_node->data),newsym);
    }

    SymList *dups = newSymList();
    // search the param type decl list for dups and toss extras
    for (sym_node = decl_list->head->next; sym_node != NULL; sym_node = sym_node->next) {
        for (sym_node2 = sym_node->next; sym_node2 != NULL; sym_node2 = sym_node2->next) {
            if (compareSym(sym_node->data,sym_node2->data) == 0) {
                yyerror("");
                fprintf(stderr, "\tduplicate local variable declaration\n");
                appendSym(dups,sym_node2->data);
            }
        }
    }
    for (sym_node = dups->head->next; sym_node != NULL; sym_node = sym_node->next) {
        removeSym(decl_list,sym_node->data);
    }

    freeSymListOnly(dups);


    // iterate through the parameter type declaration list and update each symbol from the scope table with the type given
    // if a declaration in the type list was not found amongst the parameters, error
    for (sym_node = decl_list->head->next; sym_node != NULL; sym_node = sym_node->next) {
        Sym* arg = getValueStringKSymVHashTable(func->scope,&sym_node->data->id);
        if (arg != NULL) arg->type = sym_node->data->type;
        else {
            yyerror("");
            fprintf(stderr,"\tparameter type declaration <%s> does not exist as an argument\n",sym_node->data->id);
        }
    }

    // iterate through the parameter list and every parameter should not have a type value of -1, otherwise error
    for (sym_node = parameters->head->next; sym_node != NULL; sym_node = sym_node->next) {
        if (sym_node->data->type == -1) {
            yyerror("");
            fprintf(stderr,"\tparameter type declaration for <%s> is missing\n",sym_node->data->id);
        }
    }

    // if the type list exists, iterate through the type list and the parameter list simultaneously and make sure the types match
    if (func->args_type_list != NULL) {
        for (sym_node = parameters->head->next,type_node = func->args_type_list->head->next; 
            sym_node != NULL && type_node != NULL; sym_node = sym_node->next,type_node = type_node->next) {
            if (sym_node->data->type != *type_node->data) {
                yyerror("");
                fprintf(stderr,"\tformal type of parameter <%s> does not match prototype parameter type declaration\n",sym_node->data->id);
            }
        }
        if (sym_node != NULL ||  type_node != NULL) {
            if (sym_node == NULL) {
                yyerror("");
                fprintf(stderr,"\ttoo many type declarations\n");
            } else if (type_node == NULL) {
                yyerror("");
                fprintf(stderr,"\ttoo few type declarations\n");
            }
        }
    } else { // since the type list doesn't exist, create and add it to the symbol
        // iterate through the id list, grab the corresponding symbol from scope, and add it's type to the list.
        TypeList *type_list = newTypeList();
        for (string_node = func->args_id_list->head->next; string_node != NULL; string_node = string_node->next) {
            Sym *sym = getValueStringKSymVHashTable(func->scope,string_node->data);
            appendTypeWoP(type_list,sym->type);
        }
        func->args_type_list = type_list;
    }


    freeSymListOnly(parameters);
}

char* getOperatorString(int operator) {
    char *ret = (char*)malloc(sizeof(char)*3); // TODO arena malloc here
    switch (operator) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '>':
        case '<':
        case '!':
            sprintf(ret,"%c",operator);
            break;
        case GREAT_EQ:
            sprintf(ret,">=");
            break;
        case LESS_EQ:
            sprintf(ret,"<=");
            break;
        case NOT_EQ:
            sprintf(ret,"!=");
            break;
        case D_EQ:
            sprintf(ret,"==");
            break;
        case D_AMP:
            sprintf(ret,"&&");
            break;
        case D_BAR:
            sprintf(ret,"&&");
            break;

    }
    return ret;
}

char* getTypeString(Type type) {
    char *ret = (char*)malloc(sizeof(char)*12); // TODO arena malloc here
    sprintf(ret,"compiler error");
    switch (type) {
        case INT:
            sprintf(ret,"INT");
            break;
        case DOUBLE_DECL:
            sprintf(ret,"DOUBLE");
            break;
        case FLOAT:
            sprintf(ret,"FLOAT");
            break;
        case CHAR:
            sprintf(ret,"CHAR");
            break;
        case VOID:
            sprintf(ret,"VOID");
            break;
        case BOOL:
            sprintf(ret,"BOOL");
            break;
    }
    return ret;
}

void incompatOpError(int op,Type t) {
    // char *operator = getOperatorString(op);
    // char *type = getTypeString(t);
    yyerror("");
    fprintf(stderr,"\toperator %s is not compatible with type %s\n",getOperatorString(op),getTypeString(t));
    // free(operator);
    // free(type);
}

Expr* resolveExpr(int operator,Expr *a,Expr *b) {
    Type eff_a = (a->type == CHAR)?INT:a->type;

    Type eff_b;
    if (b != NULL) eff_b = (b->type == CHAR)?INT:b->type;
    switch (operator) {
        case BSHFTR:
        case BSHFTL:
            if (eff_a != INT) {
                incompatOpError(operator,a->type);
                return newExpr(INT);
            } else if (eff_b != INT) {
                incompatOpError(operator,b->type);
                return newExpr(INT);
            } else {
                return newExpr(INT);
            }

            break;
        case '+':
        case '-':
        case '*':
        case '/':
            // check for DOUBLE and INT take care of unary -  
            if (eff_a != INT && eff_a != DOUBLE_DECL) {
                incompatOpError(operator,a->type);
                return newExpr(INT);
            } else {
                if (b == NULL) {
                    if (operator == '-') return newExpr(eff_a);
                } else if (eff_b != INT && eff_b != DOUBLE_DECL) {
                    incompatOpError(operator,a->type);
                    return newExpr(INT);
                }
            }

            // types must be the same now
            if (eff_a == eff_b) {
                return newExpr(eff_a);
            } else {
                yyerror("");
                fprintf(stderr,"\tincompatible operands for operator %s\n",getOperatorString(operator));
                return newExpr(INT);
            }
        case GREAT_EQ:
        case LESS_EQ:
        case '>':
        case '<':
        case D_EQ:
        case NOT_EQ:
        case D_AMP:
        case D_BAR:
            if (eff_a == eff_b) {
                return newExpr(BOOL);
            } else {
                yyerror("");
                fprintf(stderr,"\toperands of a boolean expression must be the same type: <%s> and <%s>\n",getTypeString(a->type),getTypeString(b->type));
                return newExpr(BOOL);
            }
        case '!':
            if (eff_a == BOOL) return newExpr(BOOL);
            else {
                incompatOpError(operator,a->type);
                return newExpr(BOOL);
            }
    }
    yyerror("");
    fprintf(stderr,"\tUnknown operator <%c>\n",operator);
    return newExpr(VOID);
}

void setScope(StringKSymVHashTable *table) {
    current_scope = table;
}

int checkCallArgs(Sym *func,ExprList *arg_list) {
    if (func->args_type_list->size > arg_list->size) {
        yyerror("");
        fprintf(stderr,"\ttoo few arguments\n");
        return 0;
    } else if (func->args_type_list->size < arg_list->size) {
        yyerror("");
        fprintf(stderr,"\ttoo many arguments\n");
        return 0;
    }
    TypeNode *tnode; ExprNode *enode;
    int i;
    for (tnode = func->args_type_list->head->next, enode = arg_list->head->next, i=0;
            tnode != NULL && enode != NULL; tnode = tnode->next, enode = enode->next, i++) {
        if (*tnode->data != enode->data->type) {
            yyerror("");
            fprintf(stderr, "\targument %d is type <%s>, must be <%s>\n",arg_list->size-i,getTypeString(enode->data->type),getTypeString(*tnode->data));
            return 0;
        }
    }
    return 1;
}

Expr* idToExpr(String s,ExprList *expr_list,Expr *expr) {
    Sym *sym;
    if (expr_list == NULL && expr != NULL) { // array element accession POSSIBLE_REFACTORING_aetb
        if ((sym = getValueStringKSymVHashTable(current_scope,&s)) == NULL) { // check local scope
            if ((sym = getValueStringKSymVHashTable(global_sym_table,&s)) == NULL) { // check global scope
                yyerror("");
                fprintf(stderr,"\tidentifier <%s> does not exist\n",s);
            }
        }
        if (sym != NULL) {
            if (sym->array_size != -1) { // should have an array size
                if (expr->type == INT) {
                    return newExpr(sym->type);
                } else {
                    yyerror("");
                    fprintf(stderr,"\tat variable <%s>: array index must be an integer\n",s);
                }
            } else {
                yyerror("");
                fprintf(stderr,"\tvariable <%s> cannot be accessed as an array\n",s);
            }
        }
    } else if (expr_list != NULL && expr == NULL) { // function call
        if ((sym = getValueStringKSymVHashTable(global_sym_table,&s)) != NULL) { // check global scope
            if (sym->sym_type == CLIKE_FUNC) { // must be a function
                if (sym->type != VOID) { // function calls shouldn't return void
                    if (checkCallArgs(sym,expr_list)) { // check the arguments provided, number and type must match 
                        return newExpr(sym->type);
                    } else {
                        // yyerror("");
                        // fprintf(stderr,"\t\n");
                    }
                } else {
                    yyerror("");
                    fprintf(stderr, "\tfunction <%s> has void return\n",sym->id);
                }
            } else {
                yyerror("");
                fprintf(stderr,"\t<%s> is not a function\n",s);
            }
        } else {
            yyerror("");
            fprintf(stderr,"\tfunction <%s> does not exist\n",s);
        }
    } else { // local or global variable accession POSSIBLE_REFACTORING_aetb
        if ((sym = getValueStringKSymVHashTable(current_scope,&s)) == NULL) { // check local scope
            if ((sym = getValueStringKSymVHashTable(global_sym_table,&s)) == NULL) { // check global scope
                yyerror("");
                fprintf(stderr,"\tidentifier <%s> does not exist\n",s);
            }
        }

        if (sym != NULL) {
            if (sym->array_size == -1) { // should not have an array size
                return newExpr(sym->type);
            } else {
                yyerror("");
                fprintf(stderr,"\tvariable <%s> is missing an array index\n",s);
            }
        }

    }

    // error occurred, return an empty expression
    freeExprListOnly(expr_list);
    return emptyExpr();

}



//=====================================================

void yyerror(char const *s) {
	fprintf(stderr,"Line %d: %s\n",line,s);
}

void logg(char const *s) {
	if (CASKDEBUG)
		printf("%s\n",s);
}

void printConsts() {
    printf("INT: %d; DOUBLE: %d, CHAR: %d, VOID: %d\n",INT,DOUBLE_DECL,CHAR,VOID);
}

int main() {
    // printConsts();
    global_sym_table = newStringKSymVHashTable(20);
    setScope(global_sym_table);
	yyparse();
	yylex_destroy();
	if (inComments) {
		printf("%d, Unterminated Comment\n",line);
	}
    return 0;
}


