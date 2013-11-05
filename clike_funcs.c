
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "clike.h"
#include "stringkfuncsymvhashtable.h"
#include "stringkvarsymvhashtable.h"
#include "clike.tab.h"

int line = 1;
int inComments = 0;
Type current_type = -1;
StringKFuncSymVHashTable *global_func_table;
StringKVarSymVHashTable *global_var_table;

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

//================FuncSym================

FuncSym* newFuncSym(String id,int is_defined,Type return_type,TypeList *args_type_list,StringList *args_id_list) {
    FuncSym *f = (FuncSym*)malloc(sizeof(FuncSym));
    f->id = id;
    f->is_defined = is_defined;
    f->return_type = return_type;
    f->args_type_list = args_type_list;
    f->args_id_list = args_id_list;
    return f;
}

void printFuncSym(FuncSym *data) {
    // printf("");
}
void freeFuncSym(FuncSym *data) {
    free(data->id);
    freeTypeList(data->args_type_list);
    freeStringList(data->args_id_list);
    free(data);
}

int compareFuncSym(FuncSym *a,FuncSym *b) {
    return compareString(&a->id,&b->id);
}

FuncSym* dupFuncSym(FuncSym *data) {
    FuncSym *f = (FuncSym*)malloc(sizeof(FuncSym));
    f->id = (char*)strdup(data->id);
    f->is_defined = data->is_defined;
    f->return_type = data->return_type;
    if (data->args_type_list != NULL) f->args_type_list = dupTypeList(data->args_type_list);
    if (data->args_id_list != NULL) f->args_id_list = dupStringList(data->args_id_list);
    return f;
}

//======================================

//==============VarSym===================

void printVarSym(VarSym *data) {

}

void freeVarSym(VarSym *data) {
    free(data->id);
    free(data);
}

int compareVarSym(VarSym *a,VarSym *b) {
    return compareString(&a->id,&b->id);
}

VarSym* dupVarSym(VarSym *data) {
    VarSym *v = (VarSym*)malloc(sizeof(VarSym));
    v->id = strdup(data->id);
    v->type = data->type;
    return v;
}


//========================================

//===============Type===================

void printType(Type *data) {

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


/* checks the symbol table against the list of function symbols; each function symbol could be a prototype or a function definition */
void checkFuncSymTable(StringKFuncSymVHashTable *table,FuncSymList *list) {
    FuncSymNode *node;
    FuncSym *func;

    for (node = list->head->next; node != NULL; node = node->next) {
        func = getValueStringKFuncSymVHashTable(table,&node->data->id);
        if (func == NULL) {// match does not exist
            String *id = dupString(&node->data->id);
            putStringKFuncSymVHashTable(table,id,dupFuncSym(node->data));
        } else {// match exists
            if (node->data->is_defined) { // if the current function is not a function prototype
                if (func->is_defined) {// the function grabbed from the table is not a prototype
                    // incorrect because you can't define a function twice
                    yyerror("");
                    fprintf(stderr,"\tduplicate definition of <%s>\n",func->id);
                } else {// the function grabbed from the table is a prototype
                    // prototype was already in the table and now you have a definition -- CORRECT
                    if (func->return_type != node->data->return_type) {
                        yyerror("");
                        fprintf(stderr,"\t<%s> function return type does not match prototype\n",func->id);
                    } else if (compareStringList(func->args_id_list,node->data->args_id_list) != 0) {
                        yyerror("");
                        fprintf(stderr,"\t<%s> function argument list does not match prototype argument list\n",func->id);
                    } else {// if all's good, replace the prototype entry with the function definition
                        freeFuncSym(removeStringKFuncSymVHashTable(table,&func->id));
                        putStringKFuncSymVHashTable(table,dupString(&node->data->id),dupFuncSym(node->data));
                    }
                }
            } else { // if the current function is a function prototype
                if (func->is_defined) { // the function grabbed from the table is not a prototype
                    // Function has already been defined
                    // char *message;
                    // casksprintf(&message,"function <%s> has already been defined",1,node->data->id);
                    yyerror("");
                    fprintf(stderr,"\tfunction <%s> has already been defined\n",node->data->id);
                    // free(message);
                } else { // the function grabbed from the table is a prototype
                    // Function prototype has already been declared

                    yyerror("");
                    fprintf(stderr,"\tfunction prototype <%s> has already been defined",func->id);
                }
            }
        }
    }


}

/* checks the list to see if there are any FuncSym objects with the same id */
void checkFuncSymListForDups(FuncSymList *list) {
    FuncSymNode *cur,*node;
    for (cur = list->head->next; cur != NULL; cur = cur->next) {
        for (node = cur->next; node != NULL; node = node->next) {
            if (compareString(&cur->data->id,&node->data->id) == 0) { // duplicate found
                yyerror("");
                fprintf(stderr,"\tduplicate function prototype identifiers\n");
            }
        }
    }
}

/* construct a FuncSym object */
FuncSym* newFProt(Type type,char *id,TypeList *type_list) {
    return newFuncSym(id,0,type,type_list,NULL);
}

void setTypesFuncSymList(FuncSymList *list,Type type) {
    FuncSymNode *node;
    for (node = list->head->next; node != NULL; node = node->next) {
        node->data->return_type = type;
    }
}

/* Only does strings for now */
// void casksprintf(char **dest,char *format,int n,...) {
//     va_list args;
//     va_start(args,n);
//     char *s,*message,*temp;
//     while (n--) {
//         temp = ()malloc();
//     }
// }




void yyerror(char const *s) {
	fprintf(stderr,"Line %d: %s\n",line,s);
}

void logg(char const *s) {
	if (CASKDEBUG)
		printf("%s\n",s);
}



int main() {
    global_func_table = newStringKFuncSymVHashTable(20);
    global_var_table = newStringKVarSymVHashTable(20);
	yyparse();
	yylex_destroy();
	if (inComments) {
		printf("%d, Unterminated Comment\n",line);
	}
    return 0;
}


