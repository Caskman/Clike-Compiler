
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "clike.h"
#include "stringksymvhashtable.h"
#include "clike.tab.h"
#include "clike_types.h"

int line = 1;
int inComments = 0;
Type current_type = -1;
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

Sym* newSym(String id,int sym_type,int is_defined,Type type,TypeList *args_type_list,StringList *args_id_list) {
    Sym *f = (Sym*)malloc(sizeof(Sym));
    f->id = id;
    f->sym_type = sym_type;
    f->is_defined = is_defined;
    f->type = type;
    f->args_type_list = args_type_list;
    f->args_id_list = args_id_list;
    return f;
}

void printSym(Sym *data) {
    // printf("");
}
void freeSym(Sym *data) {
    free(data->id);
    freeTypeList(data->args_type_list);
    freeStringList(data->args_id_list);
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
    return f;
}


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
void checkAndLogSymTable(StringKSymVHashTable *table,SymList *list) {
    SymNode *node;
    Sym *sym;

    for (node = list->head->next; node != NULL; node = node->next) {
        sym = getValueStringKSymVHashTable(table,&node->data->id);
        if (sym == NULL) {// match does not exist
            String *id = dupString(&node->data->id);
            putStringKSymVHashTable(table,id,dupSym(node->data));
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


/* construct a Sym object */
Sym* newFProt(Type type,char *id,TypeList *type_list) {
    return newSym(id,CLIKE_FUNC,0,type,type_list,NULL);
}

Sym* newVarDecl(Type type,String id,int array_size) {
    return newSym(id,CLIKE_VAR,1,type,NULL,NULL);
}

SymList* setTypesSymList(SymList *list,Type type) {
    SymNode *node;
    for (node = list->head->next; node != NULL; node = node->next) {
        node->data->type = type;
    }
    return list;
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
    global_sym_table = newStringKSymVHashTable(20);
	yyparse();
	yylex_destroy();
	if (inComments) {
		printf("%d, Unterminated Comment\n",line);
	}
    return 0;
}


