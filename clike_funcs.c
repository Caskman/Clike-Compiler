
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include "clike.h"
#include "stringksymvhashtable.h"
#include "clike.tab.h"
#include "clike_types.h"
#include "exprlist_imp.h"
#include "exprlist.h"
#include "stmtlist.h"
#include "quadlist_imp.h"
#include "quadlist.h"
#include "quadlistlist_imp.h"
#include "quadlistlist.h"
#include "stringkstringvhashtable.h"
#include "stringkstringvhashtable_imp.h"
#include "instrlist_imp.h"
#include "instrlist.h"

int line = 1;
int inComments = 0;
int is_duplicate_function = 0;
int error_thrown = 0;
int print_quads = 0;
Sym *current_function;
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

String dupStringWoP(String s) {
    String news = (String) malloc(sizeof(char)*(strlen(s) + 1));
    strcpy(news,s);
    return news;
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

Sym* newSym(String id,int sym_type,int is_defined,Type type,TypeList *args_type_list,StringList *args_id_list,StringKSymVHashTable *scope,int array_size,StmtList *body,int offset) {
    Sym *f = (Sym*)malloc(sizeof(Sym));
    f->id = id;
    f->sym_type = sym_type;
    f->is_defined = is_defined;
    f->type = type;
    f->args_type_list = args_type_list;
    f->args_id_list = args_id_list;
    f->scope = scope;
    f->array_size = array_size;
    f->body = body;
    f->offset = offset;
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

Sym* dupSym(Sym *data) { // TODO doesn't seem to used ever but still required by templatelist
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
    if (data != NULL) free(data);
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

Expr* newExpr(Type type,int intcon,double doublecon,Sym *sym,Expr *left,Expr *right,int operator,ExprList *arg_list,Expr *index_expr) {
    Expr *newe = (Expr*)malloc(sizeof(Expr));
    newe->type = type;
    newe->intcon = intcon;
    newe->doublecon = doublecon;
    newe->sym = sym;
    newe->left = left;
    newe->right = right;
    newe->operator = operator;
    newe->arg_list = arg_list;
    newe->index_expr = index_expr;
    return newe;
}

void freeExprShallow(Expr *expr) {
    if (expr != NULL) {
        freeExprListOnly(expr->arg_list);
    }
}

Expr* newExprASTNode(Type type,Expr *left,Expr *right,int operator) {
    return newExpr(type,-1,0.0,NULL,left,right,operator,NULL,NULL);
}

Expr* emptyExpr() {
    return newExpr(CLIKE_ERROR,-1,0.0,NULL,NULL,NULL,-1,NULL,NULL);
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
    return newStmt(CLIKE_ERROR,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
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

//===============Quad==================

void printQuad(Quad *data) {
    switch (data->type) {
    case QUAD_DUMMY: printf("\t%s\n",data->string); break;
    case QUAD_ENTER: printf("\tenter %d\n", data->intcon); break;
    case QUAD_INIT_INT: printf("\t%s := %d\n",data->dest->id,data->intcon); break;
    case QUAD_INIT_DOUBLE: printf("\t%s := %f\n",data->dest->id,data->doublecon); break;
    case QUAD_NEG: printf("\t%s := -%s\n",data->dest->id,data->src->id); break;
    case QUAD_INV: printf("\t%s := !%s\n",data->dest->id,data->src->id); break;
    case QUAD_MV: printf("\t%s := %s\n",data->dest->id,data->src->id); break;
    case QUAD_ASSG: printf("\t%s := %s %s %s\n",data->dest->id,data->src->id,getOperatorString(data->op),data->src2->id); break;
    case QUAD_GOTO: printf("\tgoto %s\n",data->string); break;
    case QUAD_BRANCH: printf("\tif %s %s %d goto %s\n",data->src->id,getOperatorString(data->op),data->intcon,data->string); break;
    case QUAD_LABEL: printf("%s:\n", data->string); break;
    case QUAD_INDX_L: printf("\t%s[%s] := %s\n",data->dest->id,data->src->id,data->src2->id); break;
    case QUAD_INDX_R: printf("\t%s := %s[%s]\n",data->dest->id,data->src->id,data->src2->id); break;
    case QUAD_PARAM: printf("\tparam %s,%d\n",data->src->id,data->intcon); break;
    case QUAD_CALL: printf("\tcall %s\n",data->src->id); break;
    case QUAD_RETVAL: printf("\tretval %s\n",data->src->id); break;
    case QUAD_RETURN: printf("\treturn\n"); break;
    case QUAD_RETRIEVE: printf("\tretrieve %s\n",data->dest->id); break;
    case QUAD_GLOBAL: printf("\tglobal %s %s[%d]\n",getTypeString(data->src->type),data->src->id,data->src->array_size); break;
    default: printf("ERROR ERROR ERROR\n"); break;
    }
}

void freeQuad(Quad *data) {
    if (data != NULL) {
        free(data);
    }
}

Quad* dupQuad(Quad *data) {
    return NULL;
}

int compareQuad(Quad *a,Quad *b) {
    return 0;
}

Quad* newQuad(int type,Sym *dest,Sym *src,Sym *src2,String string,int op,int intcon,double doublecon) {
    Quad *newq = (Quad*)malloc(sizeof(Quad));
    newq->type = type;
    newq->dest = dest;
    newq->src = src;
    newq->src2 = src2;
    newq->string = string;
    newq->op = op;
    newq->intcon = intcon;
    newq->doublecon = doublecon;
    return newq;
}

Quad* newDummyQuad(char *s) {
    return newQuad(QUAD_DUMMY,NULL,NULL,NULL,strdup(s),-1,-1,0.0);
}

//======================================

//================Instr=============================


void printInstr(Instr *data) {
    switch (data->type) {
    case INSTR_COMMENT:
        printf("\t# %s\n",data->src); break;
    }
}

void freeInstr(Instr *data) {
    if (data != NULL) free(data);
}

Instr* dupInstr(Instr *data) {
    return NULL;
}

int compareInstr(Instr *a,Instr *b) {
    return 0;
}

Instr* newInstr(int type,String dest,String src,String src2,int i) {
    Instr *newi = (Instr*)malloc(sizeof(Instr));
    newi->type = type;
    newi->dest = dest;
    newi->src = src;
    newi->src2 = src2;
    newi->i = i;
    return newi;
}



//======================================

//================MISC=============================

String toStr(int i) {
    int size = 1,temp,j;
    for (temp = i/10; temp != 0; temp /= 10,size++); // get num digits

    // create string buffer and set null char
    char *s = (char*)malloc(sizeof(char)*(size+1));
    s[size] = '\0';

    // set each digit
    for (j = size - 1,temp = i; j >= 0; j--, temp /= 10) {
        s[j] = (temp % 10) + '0';
    }
    return s;
}

//======================================

// TODO
// finish std lib implementations, still need to do todouble
// then resolve compile errors and move on
// implement something to make all labels unique, except for main?
// create register flyweights and make flyweight initializers; don't need freers since arena malloc plan will take care of that

//================FINAL CODE GENERATION=============================

InstrList* genRetrieveCode(Quad *quad) {

    // QUAD_RETRIEVE

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("retrieve a return value..."),NULL,-1));
}

InstrList* genRetValCode(Quad *quad) {

    // QUAD_RETVAL

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("set a var as a return value..."),NULL,-1));
}

InstrList* genParamCode(Quad *quad) {

    // QUAD_PARAM

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("assign a parameter..."),NULL,-1));
}

InstrList* genIndexCode(Quad *quad) {

    // QUAD_INDX_L
    // QUAD_INDX_R

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("index assign a var..."),NULL,-1));
}

InstrList* genLabelCode(Quad *quad) {

    // QUAD_LABEL

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("label..."),NULL,-1));
}

InstrList* genBranchCode(Quad *quad) {

    // QUAD_BRANCH

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("branch somewhere..."),NULL,-1));
}

InstrList* genJumpCode(Quad *quad) {

    // QUAD_GOTO
    // QUAD_CALL

    // QUAD_RETURN

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("jump somewhere..."),NULL,-1));
}

InstrList* genAssgCode(Quad *quad) {

    // QUAD_MV
    // QUAD_ASSG

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("assign a var..."),NULL,-1));
}

InstrList* genInvCode(Quad *quad) {

    // QUAD_NEG
    // QUAD_INV

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("invert a var..."),NULL,-1));
}

InstrList* genInitCode(Quad *quad) {

    // QUAD_INIT_INT
    // QUAD_INIT_DOUBLE

    return makeSEInstrList(newInstr(INSTR_COMMENT,NULL,dupStringWoP("init a var..."),NULL,-1));
}

void errorMIPSGen(String s) {
    fprintf(stderr,"\tERROR QUAD SWITCH TO MIPS %s\n",s);
}

InstrList* generateFunctionMIPSCode(QuadList *function_code) {
    QuadNode *qnode;
    InstrList *function_list = newInstrList(),*temp_list;
    // Quad *enterquad = functioncode->head->next->data;
    // create prototype
    // create space on the stack, save $fp and $ra, and update $fp
    // appendInstr(function_list,newInstr(INSTR_SUBU,get_sp_reg(),get_sp_reg(),toStr(enterquad->intcon + 8))); // subu $sp,$sp,(8+locals_bytes)
    // appendInstr(function_list,newInstr(INSTR_SW,NULL,get_ra_reg(),get_sp_reg(),4)); // sw $ra,4($sp)
    // appendInstr(function_list,newInstr(INSTR_SW,NULL,get_fp_reg(),get_sp_reg(),0)); // sw $fp,0($sp)
    // appendInstr(function_list,newInstr(INSTR_ADDU,get_fp_reg(),get_sp_reg(),toStr(enterquad->intcon + 4))); // addu $fp,$sp,(4 + locals_bytes)



    for (qnode = function_code->head->next; qnode != NULL; qnode = qnode->next) {
        switch (qnode->data->type) {
        case QUAD_INIT_INT:
        case QUAD_INIT_DOUBLE:
            temp_list = genInitCode(qnode->data); break;
        case QUAD_NEG:
        case QUAD_INV:
            temp_list = genInvCode(qnode->data); break;
        case QUAD_MV:
        case QUAD_ASSG:
            temp_list = genAssgCode(qnode->data); break;
        case QUAD_GOTO:
        case QUAD_CALL:
        case QUAD_RETURN:
            temp_list = genJumpCode(qnode->data); break;
        case QUAD_BRANCH:
            temp_list = genBranchCode(qnode->data); break;
        case QUAD_LABEL:
            temp_list = genLabelCode(qnode->data); break;
        case QUAD_INDX_L:
        case QUAD_INDX_R:
            temp_list = genIndexCode(qnode->data); break;
        case QUAD_PARAM:
            temp_list = genParamCode(qnode->data); break;
        case QUAD_RETVAL:
            temp_list = genRetValCode(qnode->data); break;
        case QUAD_RETRIEVE:
            temp_list = genRetrieveCode(qnode->data); break;
        case QUAD_ENTER:
            temp_list = newInstrList(); break;
        default:
            if (qnode->data->type == QUAD_GLOBAL) errorMIPSGen("global");
            else if (qnode->data->type == QUAD_DUMMY) errorMIPSGen("dummy");
            else errorMIPSGen("unknown");
            break;
        }
        appendInstrListToListShallow(function_list,temp_list);
        freeInstrListOnly(temp_list);
    }


    return function_list;
}

InstrList* generateGlobalMIPSCode(QuadList *globals) {

    //TODO generate global instructions

    return newInstrList();
}

void printStdLibs() {

    // TODO print standard libraries from their files

}

int isGlobalList(QuadList *qlist) {
    if (qlist->size > 0 && qlist->head->next->data->type == QUAD_GLOBAL) return 1;
    else return 0;
}

void generateMIPS(QuadListList *code) {
    InstrList *final_code = newInstrList(),*instrs;
    QuadListNode *qlnode;
    for (qlnode = code->head->next; qlnode != NULL; qlnode = qlnode->next) {
        if (isGlobalList(qlnode->data)) {
            instrs = generateGlobalMIPSCode(qlnode->data);
        } else instrs = generateFunctionMIPSCode(qlnode->data);
        appendInstrListToListShallow(final_code,instrs);
        freeInstrListOnly(instrs);
    }



    printInstrList(final_code);
    printStdLibs();

    freeInstrListOnly(final_code);
}




//======================================


//================INTERMEDIATE CODE GENERATION=============================

Sym* getValueDest(QuadList *list) { // bugs in template list's previous references will probably show up here
    QuadNode* node;
    for (node = list->tail; node != list->head; node = node->prev) {
        if (node->data->dest != NULL) {
            return node->data->dest;
        }
    }
    return NULL;
}

int getSymBytes(Sym *sym) {
    if (sym->type == CHAR) return 1;
    else if (sym->type == DOUBLE_DECL) return 8;
    else return 4;
}

Sym* getTempSym(StringKSymVHashTable *scope,int *locals_bytes,Type type) {
    int i; String name,num;
    for (i = 0; ; i++) {
        num = toStr(i);
        name = (String)malloc(sizeof(char)*(1 + strlen(num) + 1));
        strcpy(name,"t");
        strcat(name,num);
        if (getValueStringKSymVHashTable(scope,&name) == NULL) {
            Sym *news = newSym(name,CLIKE_VAR,1,type,NULL,NULL,NULL,-1,NULL,-1);
            (*locals_bytes) += getSymBytes(news);
            putStringKSymVHashTable(scope,dupString(&name),news);
            return news;
        }
        free(num);
        free(name);
    }
    return NULL;
}

Quad* getLabel(String func_name,StringKStringVHashTable *labels,String content) {
    String suffix,s; int i;

    for (i = 0; ; i++) {
        suffix = toStr(i);
        s = (String)malloc(sizeof(char)*(strlen(func_name) + strlen(content) + strlen(suffix) + 1));
        strcpy(s,func_name);
        strcat(s,content);
        strcat(s,suffix);
        if (getValueStringKStringVHashTable(labels,&s) == NULL) {
            Quad *q = newQuad(QUAD_LABEL,NULL,NULL,NULL,s,-1,-1,0.0);
            String *t = (String*)malloc(sizeof(String));
            *t = s;
            putStringKStringVHashTable(labels,dupString(t),t);
            return q;
        }
        free(s);
    } 
    return NULL;
}

void exprQuadError() {
    fprintf(stderr,"ERROR GENERATING EXPRESSION QUADS\n");
}

QuadList* generateExprQuadList(Expr *expr,Sym *func,int *locals_bytes,StringKStringVHashTable *labels) {
    if (expr == NULL) return newQuadList();
    QuadList *list = newQuadList(),*left,*right,*index;
    Quad *quad,*label,*label2; Type type = -1;

    index = generateExprQuadList(expr->index_expr,func,locals_bytes,labels);
    left = generateExprQuadList(expr->left,func,locals_bytes,labels);
    right = generateExprQuadList(expr->right,func,locals_bytes,labels);

    Sym *l_result,*r_result,*index_result,*tempdest;

    l_result = getValueDest(left);
    r_result = getValueDest(right);
    index_result = getValueDest(index);
    // int type,Sym *dest,Sym *src,Sym *src2,String string,int op,int intcon,double doublecon
    switch (expr->operator) {
    case '-':
        if (r_result == NULL) {
            appendQuadListToListShallow(list,left); // eval expr
            tempdest = getTempSym(func->scope,locals_bytes,l_result->type);
            quad = newQuad(QUAD_NEG,tempdest,l_result,NULL,NULL,-1,-1,0.0);
            appendQuad(list,quad); // negate expr
            break;
        }
    case BSHFTR:
    case BSHFTL:
        type = INT;
    case '+':
    case '*':
    case '/':
    case GREAT_EQ:
    case LESS_EQ:
    case '>':
    case '<':
    case D_EQ:
    case NOT_EQ:
        type = (type == -1)?l_result->type:type;
        appendQuadListToListShallow(list,left); // eval left
        appendQuadListToListShallow(list,right); // eval right
        tempdest = getTempSym(func->scope,locals_bytes,INT);
        quad = newQuad(QUAD_ASSG,tempdest,l_result,r_result,NULL,expr->operator,-1,0.0);
        appendQuad(list,quad); // eval expr
        break;
    // int type,Sym *dest,Sym *src,Sym *src2,String string,int op,int intcon,double doublecon
    case D_AMP: // if the left side is true, complete the right side and return its result; if the left side is false, return false
        tempdest = getTempSym(func->scope,locals_bytes,INT);
        label = getLabel(func->id,labels,"falseand"); // false label
        label2 = getLabel(func->id,labels,"endand"); // end of and
        appendQuadListToListShallow(list,left); // eval left
        appendQuad(list,newQuad(QUAD_BRANCH,NULL,l_result,NULL,label->string,D_EQ,0,0.0)); // branch to false if false
        appendQuadListToListShallow(list,right); // eval right
        appendQuad(list,newQuad(QUAD_BRANCH,NULL,r_result,NULL,label->string,D_EQ,0,0.0)); // branch to false if false
        appendQuad(list,newQuad(QUAD_INIT_INT,tempdest,NULL,NULL,NULL,-1,1,0.0)); // set result to be true
        appendQuad(list,newQuad(QUAD_GOTO,NULL,NULL,NULL,label2->string,-1,-1,0.0)); // goto endand
        appendQuad(list,label); // add false label
        appendQuad(list,newQuad(QUAD_INIT_INT,tempdest,NULL,NULL,NULL,-1,0,0.0)); // set result to be false
        appendQuad(list,label2); // add endand label
        break;
    case D_BAR:
        tempdest = getTempSym(func->scope,locals_bytes,INT);
        label = getLabel(func->id,labels,"trueor"); // true label
        label2 = getLabel(func->id,labels,"endor"); // end of or
        appendQuadListToListShallow(list,left); // eval left
        appendQuad(list,newQuad(QUAD_BRANCH,NULL,l_result,NULL,label->string,D_EQ,1,0.0)); // branch to true if true
        appendQuadListToListShallow(list,right); // eval right
        appendQuad(list,newQuad(QUAD_BRANCH,NULL,r_result,NULL,label->string,D_EQ,1,0.0)); // branch to true if true
        appendQuad(list,newQuad(QUAD_INIT_INT,tempdest,NULL,NULL,NULL,-1,0,0.0)); // set result to be false
        appendQuad(list,newQuad(QUAD_GOTO,NULL,NULL,NULL,label2->string,-1,-1,0.0)); // goto endor
        appendQuad(list,label); // add true label
        appendQuad(list,newQuad(QUAD_INIT_INT,tempdest,NULL,NULL,NULL,-1,1,0.0)); // set result to be true
        appendQuad(list,label2); // add endor label
        break;
    case '!':
        appendQuadListToListShallow(list,left); // eval expr
        tempdest = getTempSym(func->scope,locals_bytes,INT); // create temp
        appendQuad(list,newQuad(QUAD_INV,tempdest,l_result,NULL,NULL,-1,-1,0.0)); // invert
        break;
    // int type,Sym *dest,Sym *src,Sym *src2,String string,int op,int intcon,double doublecon
    case -1:
        // generateMiscExprQuadList();
        if (expr->sym != NULL && expr->index_expr != NULL && expr->arg_list == NULL) { // array accession
            tempdest = getTempSym(func->scope,locals_bytes,INT); // get temp
            appendQuadListToListShallow(list,index); // eval index expr
            appendQuad(list,newQuad(QUAD_INDX_R,tempdest,expr->sym,index_result,NULL,-1,-1,0.0)); // add index accession
        } else if (expr->sym != NULL && expr->index_expr == NULL && expr->arg_list != NULL) { // func call
            left = generateFuncCallQuadList(expr->sym,expr->arg_list,func,locals_bytes,labels);
            l_result = getValueDest(left);
            appendQuadListToListShallow(list,left); // call function
        } else if (expr->sym != NULL && expr->index_expr == NULL && expr->arg_list == NULL) { // variable
            tempdest = getTempSym(func->scope,locals_bytes,expr->sym->type); // create temp
            appendQuad(list,newQuad(QUAD_MV,tempdest,expr->sym,NULL,NULL,-1,-1,0.0)); // move contents
        } else if (expr->type == INT) {
            tempdest = getTempSym(func->scope,locals_bytes,INT); // create temp
            appendQuad(list,newQuad(QUAD_INIT_INT,tempdest,NULL,NULL,NULL,-1,expr->intcon,0.0));
        } else if (expr->type == DOUBLE_DECL) {
            tempdest = getTempSym(func->scope,locals_bytes,DOUBLE_DECL); // create temp
            appendQuad(list,newQuad(QUAD_INIT_DOUBLE,tempdest,NULL,NULL,NULL,-1,-1,expr->doublecon));
        } else {
            exprQuadError();
        }
        break;
    }


    freeQuadListOnly(left);
    freeQuadListOnly(right);
    freeQuadListOnly(index);

    return list;
}

QuadList* generateFuncCallQuadList(Sym *callee,ExprList *arg_list,Sym *func,int *locals_bytes,StringKStringVHashTable *labels) {
    if (callee == NULL) return newQuadList();
    QuadList *list = newQuadList(),*parameter_quad;
    QuadListList *parameter_quads = newQuadListList();
    ExprNode *enode; QuadListNode *qnode; int i; Sym *result,*tempdest;
    TypeNode *tnode;

    // evaluate each parameter
    for (enode = arg_list->head->next; enode != NULL; enode = enode->next) {
        parameter_quad = generateExprQuadList(enode->data,func,locals_bytes,labels);
        appendQuadList(parameter_quads,parameter_quad);
        appendQuadListToListShallow(list,parameter_quad);
    }

    // set parameters
    for (i = 0, qnode = parameter_quads->head->next, tnode = callee->args_type_list->head->next;
         qnode != NULL; qnode = qnode->next, tnode = tnode->next) {
        result = getValueDest(qnode->data);
        appendQuad(list,newQuad(QUAD_PARAM,NULL,result,NULL,NULL,*tnode->data,i,0.0)); // set parameter
        freeQuadListOnly(qnode->data);
    }

    freeQuadListListOnly(parameter_quads);
    appendQuad(list,newQuad(QUAD_CALL,NULL,callee,NULL,NULL,-1,-1,0.0)); // call function
    if (callee->type != VOID) {
        tempdest = getTempSym(func->scope,locals_bytes,callee->type); // create temp
        appendQuad(list,newQuad(QUAD_RETRIEVE,tempdest,NULL,NULL,NULL,callee->type,-1,0.0)); // retrieve return value
    }


    return list;
}

QuadList* generateAssgStmtQuadList(Assg *assg,Sym *func,int *locals_bytes,StringKStringVHashTable *labels) {
    if (assg == NULL) return newQuadList();
    QuadList *list = newQuadList(),*index,*rhs; Sym *rhs_result,*index_result;

    index = generateExprQuadList(assg->index,func,locals_bytes,labels);
    rhs = generateExprQuadList(assg->expr,func,locals_bytes,labels);

    index_result = getValueDest(index);
    rhs_result = getValueDest(rhs);

    if (index_result == NULL) { // simple move
        appendQuadListToListShallow(list,rhs);
        appendQuad(list,newQuad(QUAD_MV,assg->sym,rhs_result,NULL,NULL,-1,-1,0.0));
    } else { // index assignment
        appendQuadListToListShallow(list,index);
        appendQuadListToListShallow(list,rhs);
        appendQuad(list,newQuad(QUAD_INDX_L,assg->sym,index_result,rhs_result,NULL,-1,-1,0.0));
    }


    freeQuadListOnly(index);
    freeQuadListOnly(rhs);

    return list;
}

QuadList* generateReturnStmtQuadList(Stmt *return_stmt,Sym *func,int *locals_bytes,StringKStringVHashTable *labels) {
    if (return_stmt == NULL) return newQuadList();
    QuadList *list = newQuadList(),*value;

    value = generateExprQuadList(return_stmt->expr,func,locals_bytes,labels);

    Sym *value_result = getValueDest(value);
    appendQuadListToListShallow(list,value); // eval expr
    appendQuad(list,newQuad(QUAD_RETVAL,NULL,value_result,NULL,NULL,-1,-1,0.0)); // set as return val
    appendQuad(list,newQuad(QUAD_RETURN,NULL,NULL,NULL,NULL,-1,-1,0.0)); // return


    freeQuadListOnly(value);

    return list;
}

QuadList* generateForStmtQuadList(Stmt *for_stmt,Sym *func,int *locals_bytes,StringKStringVHashTable *labels) {
    if (for_stmt == NULL) return newQuadList();
    QuadList *list = newQuadList(),*initial,*condition,*post_loop,*contents; Sym *condition_result;

    initial = generateAssgStmtQuadList(for_stmt->assg,func,locals_bytes,labels);
    condition = generateExprQuadList(for_stmt->expr,func,locals_bytes,labels);
    post_loop = generateAssgStmtQuadList(for_stmt->assg2,func,locals_bytes,labels);
    contents = generateStmtQuadList(for_stmt->stmt,func,locals_bytes,labels);
    condition_result = getValueDest(condition);
    Quad *content_label = getLabel(func->id,labels,"forcontents");
    Quad *condition_label = getLabel(func->id,labels,"forcondition");

    appendQuadListToListShallow(list,initial); // execute initial assg
    appendQuad(list,newQuad(QUAD_GOTO,NULL,NULL,NULL,condition_label->string,-1,-1,0.0)); // jump to condition label
    appendQuad(list,content_label); // add content label
    appendQuadListToListShallow(list,contents); // execute contents
    appendQuadListToListShallow(list,post_loop); // execute post_loop
    appendQuad(list,condition_label); // add condition label
    if (condition_result == NULL) {
        appendQuad(list,newQuad(QUAD_GOTO,NULL,NULL,NULL,content_label->string,-1,-1,0.0));
    } else {
        appendQuadListToListShallow(list,condition); // evaluate condition
        appendQuad(list,newQuad(QUAD_BRANCH,NULL,condition_result,NULL,content_label->string,D_EQ,1,0.0)); // branch to contents if condition is true
    }


    freeQuadListOnly(initial);
    freeQuadListOnly(condition);
    freeQuadListOnly(post_loop);
    freeQuadListOnly(contents);

    return list;
}

QuadList* generateWhileStmtQuadList(Stmt *while_stmt,Sym *func,int *locals_bytes,StringKStringVHashTable *labels) {
    if (while_stmt == NULL) return newQuadList();
    QuadList *list = newQuadList(),*contents,*condition; Sym *condition_result;

    condition = generateExprQuadList(while_stmt->expr,func,locals_bytes,labels);
    contents = generateStmtQuadList(while_stmt->stmt,func,locals_bytes,labels);
    condition_result = getValueDest(condition);
    Quad *begin_label = getLabel(func->id,labels,"whilebeg");
    Quad *condition_label = getLabel(func->id,labels,"whilecond");

    appendQuad(list,newQuad(QUAD_GOTO,NULL,NULL,NULL,condition_label->string,-1,-1,0.0)); // jump to condition
    appendQuad(list,begin_label); // add while begin label
    appendQuadListToListShallow(list,contents); // execute contents
    appendQuad(list,condition_label); // add condition label
    appendQuadListToListShallow(list,condition); // eval condition
    appendQuad(list,newQuad(QUAD_BRANCH,NULL,condition_result,NULL,begin_label->string,D_EQ,1,0.0)); // branch to whilebegin if true


    freeQuadListOnly(condition);
    freeQuadListOnly(contents);



    return list;
}

QuadList* generateIfStmtQuadList(Stmt *if_stmt,Sym *func,int *locals_bytes,StringKStringVHashTable *labels) {
    if (if_stmt == NULL) return newQuadList();
    QuadList *list = newQuadList(),*true_quads,*false_quads,*expr_quads; Sym *expr_result;
    Quad *true_label,*end_label;

    expr_quads = generateExprQuadList(if_stmt->expr,func,locals_bytes,labels);
    true_quads = generateStmtQuadList(if_stmt->stmt,func,locals_bytes,labels);
    false_quads = generateStmtQuadList(if_stmt->else_clause,func,locals_bytes,labels);
    expr_result = getValueDest(expr_quads);
    true_label = getLabel(func->id,labels,"iftrue");
    end_label = getLabel(func->id,labels,"endif");

    appendQuadListToListShallow(list,expr_quads);  // eval expr
    appendQuad(list,newQuad(QUAD_BRANCH,NULL,expr_result,NULL,true_label->string,D_EQ,1,0.0)); // branch to true on true
    appendQuadListToListShallow(list,false_quads); // eval false clause
    appendQuad(list,newQuad(QUAD_GOTO,NULL,NULL,NULL,end_label->string,-1,-1,0.0)); // jump to end of if
    appendQuad(list,true_label); // add true label
    appendQuadListToListShallow(list,true_quads); // eval true clause
    appendQuad(list,end_label); // add end if label


    freeQuadListOnly(expr_quads);
    freeQuadListOnly(true_quads);
    freeQuadListOnly(false_quads);

    return list;
}

QuadList* generateStmtQuadList(Stmt *stmt,Sym *func,int *locals_bytes,StringKStringVHashTable *labels) {
    if (stmt == NULL) return newQuadList();
    QuadList *quad_list;
    switch (stmt->stmt_type) {
    case STMT_IF:
        quad_list = generateIfStmtQuadList(stmt,func,locals_bytes,labels); break;
    case STMT_WHILE:
        quad_list = generateWhileStmtQuadList(stmt,func,locals_bytes,labels); break;
    case STMT_FOR:
        quad_list = generateForStmtQuadList(stmt,func,locals_bytes,labels); break;
    case STMT_RETURN:
        quad_list = generateReturnStmtQuadList(stmt,func,locals_bytes,labels); break;
    case STMT_ASSG:
        quad_list = generateAssgStmtQuadList(stmt->assg,func,locals_bytes,labels); break;
    case STMT_STMTLIST:
        quad_list = generateStmtListQuadList(stmt->stmt_list,func,locals_bytes,labels); break;
    case STMT_FUNCCALL:
        quad_list = generateFuncCallQuadList(stmt->sym,stmt->expr_list,func,locals_bytes,labels); break;
    case STMT_ELSE:
        quad_list = generateStmtQuadList(stmt->stmt,func,locals_bytes,labels); break;
    default:
        fprintf(stderr, "SHOULD NOT SEE THIS generateStmtQuadList\n");
        switch (stmt->stmt_type) {
        case STMT_EMPTY:
            fprintf(stderr,"STMT_EMPTY\n"); break;
        case STMT_FORCON:
            fprintf(stderr,"STMT_FORCON\n"); break;
        }
        break;
    }
    return quad_list;
}

QuadList* generateStmtListQuadList(StmtList *stmt_list,Sym *func,int *locals_bytes,StringKStringVHashTable *labels) {
    StmtNode *stnode;
    QuadList *total_quad_list = newQuadList();
    QuadList *quad_list;
    for (stnode = stmt_list->head->next; stnode != NULL; stnode = stnode->next) {
        quad_list = generateStmtQuadList(stnode->data,func,locals_bytes,labels);

        appendQuadListToListShallow(total_quad_list,quad_list);
        freeQuadListOnly(quad_list);
        quad_list = NULL;

    }
    return total_quad_list;
}

QuadList* generateFuncQuadList(Sym *func,StringKStringVHashTable *labels) {
    // retrieve locals and determine size necessary to store locals and temporaries
    StringKSymVEntryList *scope_list = splatStringKSymVHashTable(func->scope);
    int locals_bytes = 0;
    StringKSymVEntryNode *ssnode;
    for (ssnode = scope_list->head->next; ssnode != NULL; ssnode = ssnode->next) locals_bytes += getSymBytes(ssnode->data->value);

    // generate label for the beginning of this function
    putStringKStringVHashTable(labels,dupString(&func->id),&func->id);

    // now generate quads for the function body
    // pass the function symbol and the address of the locals_bytes so that if temporaries 
    // are allocated, we can make room for them.  The temporaries will be added to the scope
    // and their size added to the locals_bytes
    QuadList *stmt_quads = generateStmtListQuadList(func->body,func,&locals_bytes,labels);

    prependQuad(stmt_quads,newQuad(QUAD_ENTER,NULL,func,NULL,NULL,-1,locals_bytes,0.0));
    prependQuad(stmt_quads,newQuad(QUAD_LABEL,NULL,NULL,NULL,func->id,-1,-1,0.0));


    if (stmt_quads->tail->data->type != QUAD_RETURN) appendQuad(stmt_quads,newQuad(QUAD_RETURN,NULL,NULL,NULL,NULL,-1,-1,0.0));

    return stmt_quads;
}

Quad* generateGlobalQuad(Sym *global) {
    return newQuad(QUAD_GLOBAL,NULL,global,NULL,NULL,-1,-1,0.0);
}

void generateCode(SymList *funcs) {
    if (error_thrown) return;
    QuadListList *functioncode = newQuadListList();

    // generate global quads first
    QuadList *global_quads = newQuadList(); Quad *global;
    StringKSymVEntryList *global_list = splatStringKSymVHashTable(global_sym_table);
    StringKSymVEntryNode *ssnode;
    for (ssnode = global_list->head->next; ssnode != NULL; ssnode = ssnode->next) {
        if (ssnode->data->value->sym_type == CLIKE_VAR) {
            global = generateGlobalQuad(ssnode->data->value);
            appendQuad(global_quads,global);
        }
    }
    appendQuadList(functioncode,global_quads);

    // generate function quads
    StringKStringVHashTable *labels = newStringKStringVHashTable(5);
    SymNode *snode;
    for (snode = funcs->head->next; snode != NULL; snode = snode->next) {

        if (snode->data->sym_type == CLIKE_FUNC) {
            QuadList *quad_list = generateFuncQuadList(snode->data,labels);
            appendQuadList(functioncode,quad_list);
        }
    }

    if (print_quads) printQuadListList(functioncode);
    else {
        generateMIPS(functioncode);
    }

    QuadListNode *qnode;
    for (qnode = functioncode->head->next; qnode != NULL; qnode = qnode->next) freeQuadListOnly(qnode->data);
    freeQuadListListOnly(functioncode);
}

//======================================


//================SEMANTICS=============================


Expr* newIntegerExpr(int value) {
    return newExpr(INT,value,0.0,NULL,NULL,NULL,-1,NULL,NULL);
}

Expr* newDoubleExpr(double value) {
    return newExpr(DOUBLE_DECL,-1,value,NULL,NULL,NULL,-1,NULL,NULL);
}

Sym* newDummyDeclSym() {
    return newSym(NULL,CLIKE_DECL,-1,-1,NULL,NULL,NULL,-1,NULL,-1);
}

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
    return new_stmt;
}

Stmt* createForControl(Assg *assg1,Expr *expr,Assg *assg2) {

    if (expr->type != BOOL) {
        expr = emptyExpr();
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
        expr = emptyExpr();
    }
    return newStmt(STMT_IF,NULL,else_clause,stmt,expr,NULL,NULL,NULL,NULL);
}

Stmt* createWhileStmt(Expr *expr,Stmt *stmt) {
    if (expr->type != BOOL) {
        yyerror("");
        fprintf(stderr, "\twhile condition must evaluate to a boolean\n");
        expr = emptyExpr();
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
    // new_func_sym is always a prototype

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
    return newSym(id,CLIKE_FUNC,0,type,type_list,NULL,NULL,-1,NULL,-1);
}

Sym* newVarDecl(Type type,String id,int array_size) {
    return newSym(id,CLIKE_VAR,1,type,NULL,NULL,NULL,array_size,NULL,-1);
}

Sym* newFunctionHeader(Type type,String id,StringList *id_list) {
    return newSym(id,CLIKE_FUNC,1,type,NULL,id_list,NULL,-1,NULL,-1);
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
        appendSym(sym_list,newSym(*node->data,CLIKE_VAR,1,-1,NULL,NULL,NULL,-1,NULL,-1));
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
        Sym *newsym = newSym(*string_node->data,CLIKE_VAR,1,-1,NULL,NULL,NULL,-1,NULL,-1);
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
    ret[2] = '\0';
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
    sprintf(ret,"undefined");
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
            } else if (eff_b != INT) {
                incompatOpError(operator,b->type);
            } else {
                return newExprASTNode(INT,a,b,operator);
            }

            break;
        case '+':
        case '-':
        case '*':
        case '/':
            // check for DOUBLE and INT take care of unary -  
            if (eff_a != INT && eff_a != DOUBLE_DECL) {
                incompatOpError(operator,a->type);
            } else {
                if (b == NULL) {
                    if (operator == '-') return newExprASTNode(eff_a,a,b,operator);
                } else if (eff_b != INT && eff_b != DOUBLE_DECL) {
                    incompatOpError(operator,a->type);
                }
            }

            // types must be the same now
            if (eff_a == eff_b) {
                return newExprASTNode(eff_a,a,b,operator);
            } else {
                yyerror("");
                fprintf(stderr,"\tincompatible operands for operator %s\n",getOperatorString(operator));
            }
            break;
        case GREAT_EQ:
        case LESS_EQ:
        case '>':
        case '<':
        case D_EQ:
        case NOT_EQ:
        case D_AMP:
        case D_BAR:
            if (eff_a == eff_b) {
                return newExprASTNode(BOOL,a,b,operator);
            } else {
                yyerror("");
                fprintf(stderr,"\toperands of a boolean expression must be the same type: <%s> and <%s>\n",getTypeString(a->type),getTypeString(b->type));
            }
            break;
        case '!':
            if (eff_a == BOOL) return newExprASTNode(BOOL,a,b,operator);
            else {
                incompatOpError(operator,a->type);
            }
            break;
        default:
            yyerror("");
            fprintf(stderr,"\tUnknown operator <%c>\n",operator);
            break;
    }
    // yyerror("");
    // fprintf(stderr,"\tUnknown operator <%c>\n",operator);
    freeExprShallow(a);
    freeExprShallow(b);
    return emptyExpr();
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
        if (!checkTypeCompat(*tnode->data,enode->data->type)) {
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
                    return newExpr(sym->type,-1,0.0,sym,NULL,NULL,-1,NULL,expr);
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
                        return newExpr(sym->type,-1,0.0,sym,NULL,NULL,-1,expr_list,NULL);
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
                return newExpr(sym->type,-1,0.0,sym,NULL,NULL,-1,NULL,NULL);
            } else {
                yyerror("");
                fprintf(stderr,"\tvariable <%s> is missing an array index\n",s);
            }
        }

    }

    // error occurred, return an empty expression
    ExprNode *enode;
    if (expr_list != NULL) for (enode = expr_list->head->next; enode != NULL; enode = enode->next) freeExprShallow(enode->data);
    freeExprListOnly(expr_list);
    freeExprShallow(expr);
    return emptyExpr();

}



//=====================================================

void yyerror(char const *s) {
    error_thrown = 1;
	fprintf(stderr,"Line %d: %s\n",line,s);
}

void logg(char const *s) {
	if (CASKDEBUG)
		printf("%s\n",s);
}

void printConsts() {
    printf("INT: %d; DOUBLE: %d, CHAR: %d, VOID: %d\n",INT,DOUBLE_DECL,CHAR,VOID);
}

int main(int argc,char **argv) {
    if (argc == 2) {
        if (strcmp(argv[1],"-im") == 0) {
            print_quads = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[1]);
            return 1;
        }
    } 


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


