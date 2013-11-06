
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "clike.h"
#include "stringksymvhashtable.h"
#include "clike.tab.h"
#include "clike_types.h"
#include "exprlist_imp.h"

int line = 1;
int inComments = 0;
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
    *s = (char*)malloc(sizeof(char)*strlen(*data));
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

Sym* newSym(String id,int sym_type,int is_defined,Type type,TypeList *args_type_list,StringList *args_id_list,StringKSymVHashTable *scope) {
    Sym *f = (Sym*)malloc(sizeof(Sym));
    f->id = id;
    f->sym_type = sym_type;
    f->is_defined = is_defined;
    f->type = type;
    f->args_type_list = args_type_list;
    f->args_id_list = args_id_list;
    f->scope = scope;
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

int compareSym(Sym *a,Sym *b) {
    return compareString(&a->id,&b->id);
}

Sym* dupSym(Sym *data) {
    Sym *f = (Sym*)malloc(sizeof(Sym));
    f->id = (char*)strdup(data->id);
    f->sym_type = data->sym_type;
    f->is_defined = data->is_defined;
    f->type = data->type;
    if (data->args_type_list != NULL) f->args_type_list = dupTypeList(data->args_type_list);
    if (data->args_id_list != NULL) f->args_id_list = dupStringList(data->args_id_list);
    f->scope = NULL;
    return f;
}


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


//=====================================

//================SEMANTICS=============================


/* checks the symbol table against the list of function symbols; each function symbol could be a prototype or a function definition */
void checkAndLogSymTable(StringKSymVHashTable *table,SymList *list) {
    SymNode *node;
    Sym *sym;

    for (node = list->head->next; node != NULL; node = node->next) {
        sym = getValueStringKSymVHashTable(table,&node->data->id);
        if (sym == NULL) {// match does not exist
            String *id = dupString(&node->data->id);
            putStringKSymVHashTable(table,id,dupSym(node->data));
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
}

/* Given a new function symbol and a matched existing function symbol entry from the table,
 * checks if the existing entry (prototype) can be updated with the new symbol (definition) */
void checkMatchingFuncSym(StringKSymVHashTable *table,Sym *new_func_sym,Sym *sym_entry) {
    if (new_func_sym->is_defined) { // if the current function is not a function prototype
        if (sym_entry->is_defined) {// the function grabbed from the table is not a prototype
            // incorrect because you can't define a function twice
            yyerror("");
            fprintf(stderr,"\tduplicate definition of <%s>\n",sym_entry->id);
        } else {// the function grabbed from the table is a prototype
            // prototype was already in the table and now you have a definition -- CORRECT
            if (sym_entry->type != new_func_sym->type) {
                yyerror("");
                fprintf(stderr,"\t<%s> function return type does not match prototype\n",sym_entry->id);
            } else if (compareStringList(sym_entry->args_id_list,new_func_sym->args_id_list) != 0) {
                yyerror("");
                fprintf(stderr,"\t<%s> function argument list does not match prototype argument list\n",sym_entry->id);
            } else {// if all's good, replace the prototype entry with the function definition
                freeSym(removeStringKSymVHashTable(table,&sym_entry->id));
                putStringKSymVHashTable(table,dupString(&new_func_sym->id),dupSym(new_func_sym));
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

void checkFunction(StringKSymVHashTable *table,Sym *function) {
    SymList *list= makeSESymList(function);
    checkAndLogSymTable(table,list);
    freeSymListOnly(list);
}

void checkAndLogFuncWithSymTable(StringKSymVHashTable *table,Sym *func) {
    Sym *prototype = removeStringKSymVHashTable(table,&func->id);
    if (prototype != NULL) {
        func->args_type_list = dupTypeList(prototype->args_type_list);
        freeSym(prototype);
    }
    putStringKSymVHashTable(table,dupString(&func->id),func);
}

/* construct a Sym object */
Sym* newFProt(Type type,char *id,TypeList *type_list) {
    return newSym(id,CLIKE_FUNC,0,type,type_list,NULL,NULL);
}

Sym* newVarDecl(Type type,String id,int array_size) {
    return newSym(id,CLIKE_VAR,1,type,NULL,NULL,NULL);
}

Sym* newFunctionHeader(Type type,String id,StringList *id_list) {
    return newSym(id,CLIKE_FUNC,1,type,NULL,id_list,NULL);
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
        String *dupstring = dupString(node->data);
        appendSym(sym_list,newSym(*dupstring,CLIKE_VAR,1,-1,NULL,NULL,NULL));
        free(dupstring);
    }
    return sym_list;
}

SymList* changeIDListToStringListAndSetType(StringList *id_list,Type type) {
    return setTypesSymList(stringListToSymList(id_list),type);
}

void reconcileArgsCreateScope(Sym *func,SymList *decl_list) {
    // SymNode *sym_node;
    StringNode *string_node;
    SymNode *sym_node;
    TypeNode *type_node;

    // create function scope
    func->scope = newStringKSymVHashTable(5);

    SymList *parameters = newSymList();

    // add all of the id parameters to the scope and add them to the parameter list at the same time
    for (string_node = func->args_id_list->head->next; string_node != NULL; string_node = string_node->next) {
        String *dupstring = dupString(string_node->data);
        Sym *newsym = newSym(*dupstring,CLIKE_VAR,1,-1,NULL,NULL,NULL);
        free(dupstring);
        appendSym(parameters,newsym);
        putStringKSymVHashTable(func->scope,dupString(string_node->data),newsym);
    }

    // iterate through the parameter type declaration list and update each symbol from the scope table with the type given
    // if a declaration in the type list was not found amongst the parameters, error
    for (sym_node = decl_list->head->next; sym_node != NULL; sym_node = sym_node->next) {
        Sym* arg = getValueStringKSymVHashTable(func->scope,&sym_node->data->id);
        if (arg != NULL) arg->type = sym_node->data->type;
        else {
            yyerror("");
            fprintf(stderr,"\tparameter type declaration <%s> was not found in the parameter identifier list\n",sym_node->data->id);
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
    }


    freeSymListOnly(parameters);
}

char* getOperatorString(int operator) {
    char *ret = (char*)malloc(sizeof(char)*3);
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
    char *ret = (char*)malloc(sizeof(char)*12);
    switch (type) {
        case INT:
            sprintf(ret,"INT");
            break;
        case DOUBLE_DECL:
            sprintf(ret,"DOUBLE_DECL");
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
    char *operator = getOperatorString(op);
    char *type = getTypeString(t);
    yyerror("");
    fprintf(stderr,"\toperator %s is not compatible with type %s\n",operator,type);
    free(operator);
    free(type);
}

Expr* resolveExpr(int operator,Expr *a,Expr *b) {
    switch (operator) {
        case '+':
        case '-':
        case '*':
        case '/':
            // promote chars, check for DOUBLE, INT, CHAR, take care of unary -  
            if (a->type != INT && a->type != DOUBLE_DECL && a->type != CHAR) {
                incompatOpError(operator,a->type);
                return newExpr(INT);
            }
            if (a->type == CHAR) a->type = INT;
            if (b == NULL) {
                if (operator == '-') return newExpr(a->type);
            } else {
                if (b->type != INT && b->type != DOUBLE_DECL && b->type != CHAR) {
                    incompatOpError(operator,b->type);
                    return newExpr(INT);
                } else if (b->type == CHAR) b->type = INT;
            }
            // types must be the same now
            if (a->type == b->type) {
                return newExpr(a->type);
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
            if (a->type == b->type) {
                return newExpr(BOOL);
            } else {
                yyerror("");
                fprintf(stderr,"\toperands of a boolean expression must be the same type\n");
                return newExpr(BOOL);
            }
        case '!':
            if (a->type == BOOL) return newExpr(BOOL);
            else {
                incompatOpError(operator,a->type);
                return newExpr(BOOL);
            }
    }
    return newExpr(VOID);
}

void setScope(StringKSymVHashTable *table) {
    current_scope = table;
}

Expr* stringToExpr(StringKSymVHashTable *table, String s) {
    Sym* sym = getValueStringKSymVHashTable(table,&s);
    if (sym != NULL && sym->sym_type == CLIKE_VAR) return newExpr(sym->type);
    else return newExpr(VOID);
}



//=====================================================

void yyerror(char const *s) {
	fprintf(stderr,"Line %d: %s\n",line,s);
}

void logg(char const *s) {
	if (CASKDEBUG)
		printf("%s\n",s);
}



int main() {
    global_sym_table = newStringKSymVHashTable(20);
    setScope(global_sym_table);
	yyparse();
	yylex_destroy();
	if (inComments) {
		printf("%d, Unterminated Comment\n",line);
	}
    return 0;
}


