
%{
	#include "clike.h"
	#include "stringlist.h"
	#include <string.h>
	#include "exprlist_imp.h"
	#include "exprlist.h"
	#include "stmtlist.h"
%}

%union {
	int d;
	double f;
	String string;
	Sym *sym;
	SymList *sym_list;
	TypeList *type_list;
	StringList *string_list;
	Expr *expr;
	ExprList *expr_list;
	Assg *assg;
	Stmt *stmt;
	StmtList *stmt_list;
}

%token <d> INTEGER
%token <f> DOUBLE
%token <string> ID STRING
%token <d> _BOOL _COMPLEX AUTO CONST RESTRICT ENUM _IMAGINARY INLINE REGISTER STATIC VOLATILE BREAK CASE CHAR CONTINUE DEFAULT DO DOUBLE_DECL ELSE EXTERN
%token <d> FLOAT FOR GOTO IF INT LONG RETURN SHORT SIGNED SIZEOF STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID WHILE D_AMP D_BAR D_EQ NOT_EQ LESS_EQ GREAT_EQ
%token <d> PLUS_EQ MIN_EQ MULT_EQ DIV_EQ PERC_EQ BSHFTL_EQ BSHFTR_EQ AND_EQ XOR_EQ OR_EQ DERIVES INC DECR BSHFTR BSHFTL ELL UMINUS NOT
%token <d> '-' '+' '*' '/' '!' '>' '<'

%type <sym_list> dcl
%type <d> type array_size opt_array_size
%type <sym_list> f_prot_list dclr_list opt_loc_dcl_list loc_dcl_list loc_dcl
%type <sym> f_prot dclr func func_header
%type <type_list> type_list
%type <string_list> id_list
%type <expr> expr array_index opt_array_index opt_expr
%type <expr_list> opt_expr_list expr_list
%type <assg> assg opt_assg
%type <stmt> stmt opt_stmt else_clause for_control
%type <stmt_list> stmt_list opt_stmt_list


%left D_BAR
%left D_AMP
%nonassoc '<' '>' LESS_EQ GREAT_EQ D_EQ NOT_EQ
%left '+' '-'
%left '/' '*'
%right UMINUS NOT

%expect 1

%%
opt_prog_element_list: 
	/* epsilon */
	| prog_element_list
prog_element_list:
	prog_element
	| prog_element prog_element_list
prog_element:
	dcl ';' {checkSymListForDups($1); checkAndLogSymTable(global_sym_table,$1);}
	| func 
func:
	func_header opt_loc_dcl_list {current_function = $1; checkAndLogFuncWithSymTable(global_sym_table,$1); reconcileArgsCreateScope($1,$2); setScope(current_function->scope);} '{' opt_loc_dcl_list {addLocalsToScope($5);} opt_stmt_list '}' {setScope(global_sym_table);}
opt_stmt_list:
	/* epsilon */ {$$ = newStmtList();}
	| stmt_list {$$ = $1;}
stmt_list:
	stmt ';' {$$ = makeSEStmtList($1);}
	| stmt ';' stmt_list {$$ = appendStmt($3,$1);}
opt_stmt:
	/* epsilon */ {$$ = NULL;}
	| stmt {$$ = $1;}
stmt:
	WHILE '(' expr ')' opt_stmt {$$ = createWhileStmt($3,$5);}
	| IF '(' expr ')' stmt else_clause {$$ = createIfStmt($3,$6,$5);}
	| FOR for_control opt_stmt {$$ = createForStmt($2,$3);}
	| ID '(' opt_expr_list ')' {$$ = createCallStmt($1,$3);}
	| RETURN opt_expr {$$ = createReturnStmt($2);}
	| assg {$$ = createAssgStmt($1);}
	| '{' opt_stmt_list '}' {$$ = createListStmt($2);}
for_control:
	'(' opt_assg ';' opt_expr ';' opt_assg ')' {$$ = createForControl($2,$4,$6);}
else_clause:
	/* epsilon */ {$$ = NULL;}
	| ELSE stmt {$$ = createElseStmt($2);}
opt_assg:
	/* epsilon */ {$$ = NULL;}
	| assg {$$ = $1;}
assg:
	ID opt_array_index '=' expr {$$ = createAssg($1,$2,$4);}
opt_array_index:
	/* epsilon */ {$$ = NULL;}
	| array_index {$$ = $1;}
array_index:
	'[' expr ']' {$$ = $2;}
opt_expr_list:
	/* epsilon */ {$$ = newExprList();}
	| expr_list {$$ = $1;}
expr_list:
	expr {$$ = makeSEExprList($1);}
	| expr ',' expr_list {$$ = appendExpr($3,$1);}
opt_expr:
	/* epsilon */ {$$ = NULL;}
	| expr {$$ = $1;}
expr:
    '-' expr %prec UMINUS {$$ = resolveExpr($1,$2,NULL);}
    | '!' expr %prec NOT {$$ = resolveExpr($1,$2,NULL);}
    | expr '+' expr {$$ = resolveExpr($2,$1,$3);}
    | expr '-' expr {$$ = resolveExpr($2,$1,$3);}
    | expr '/' expr {$$ = resolveExpr($2,$1,$3);}
    | expr '*' expr {$$ = resolveExpr($2,$1,$3);}
    | expr D_AMP expr {$$ = resolveExpr($2,$1,$3);}
    | expr D_EQ expr {$$ = resolveExpr($2,$1,$3);}
    | expr D_BAR expr {$$ = resolveExpr($2,$1,$3);}
    | expr NOT_EQ expr {$$ = resolveExpr($2,$1,$3);}
    | expr LESS_EQ expr {$$ = resolveExpr($2,$1,$3);}
    | expr GREAT_EQ expr {$$ = resolveExpr($2,$1,$3);}
    | expr '<' expr {$$ = resolveExpr($2,$1,$3);}
    | expr '>' expr {$$ = resolveExpr($2,$1,$3);}
    | '(' expr ')'  {$$ = $2;}
    | ID {$$ = idToExpr($1,NULL,NULL);}
	| ID '[' expr ']' {$$ = idToExpr($1,NULL,$3);}
	| ID '(' opt_expr_list ')' {$$ = idToExpr($1,$3,NULL);}
	| INTEGER {$$ = newExpr(INT);}
	| DOUBLE {$$ = newExpr(DOUBLE);}
opt_loc_dcl_list:
	/* epsilon */ {$$ = newSymList();}
	| loc_dcl_list {$$ = $1;}
loc_dcl_list:
	loc_dcl {$$ = $1;}
	| loc_dcl loc_dcl_list {$$ = appendSymListToList($2,$1);}
loc_dcl:
	type id_list ';' {$$ = changeIDListToSymListAndSetType($2,$1);}
id_list:
	ID {$$ = makeSEStringListWoP($1);}
	| ID ',' id_list {$$ = appendStringWoP($3,$1);}
func_header:
	ID '(' ')' {$$ = newFunctionHeader(INT,$1,newStringList());}
	| VOID ID '(' ')' {$$ = newFunctionHeader($1,$2,newStringList());}
	| type ID '(' ')' {$$ = newFunctionHeader($1,$2,newStringList());}
	| ID '(' id_list ')' {$$ = newFunctionHeader(INT,$1,$3);}
	| VOID ID '(' id_list ')' {$$ = newFunctionHeader($1,$2,$4);}
	| type ID '(' id_list ')' {$$ = newFunctionHeader($1,$2,$4);}
dcl:
	type dclr_list {$$ = setTypesSymList($2,$1);}
	| VOID f_prot_list {$$ = setTypesSymList($2,$1);}
dclr_list:
	dclr {$$ = makeSESymList($1);}
	| dclr ',' dclr_list {$$ = appendSym($3,$1);}
dclr:
	f_prot {$$ = $1;}
	| ID opt_array_size {$$ = newVarDecl(VOID,$1,$2);}
opt_array_size:
	/* epsilon */ {$$ = -1;}
	| array_size {$$ = $1;}
array_size:
	'[' INTEGER ']' {$$ = $2;}
f_prot_list:
	f_prot {$$ = makeSESymList($1);}
	| f_prot ',' f_prot_list {$$ = appendSym($3,$1);}
f_prot:
	ID '(' type_list ')' {$$ = newFProt(0,$1,$3);}
	| ID '(' ')' {$$ = newFProt(0,$1,newTypeList());}
type_list:
	type {$$ = makeSETypeListWoP($1);}
	| type ',' type_list {$$ = appendTypeWoP($3,$1);}
type:
	CHAR {$$ = CHAR;}
	| INT {$$ = INT;}
	| FLOAT {$$ = FLOAT;}
	| DOUBLE_DECL {$$ = DOUBLE_DECL;}
%%




