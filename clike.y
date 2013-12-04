
%{
	#include "clike.h"
	#include "stringlist.h"
	#include <string.h>
	#include "exprlist_imp.h"
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
%type <expr> expr


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
	func_header opt_loc_dcl_list {current_function = $1; checkAndLogFuncWithSymTable(global_sym_table,$1); reconcileArgsCreateScope($1,$2); setScope(current_function->scope);} '{' opt_loc_dcl_list opt_stmt_list '}' {setScope(global_sym_table);}
opt_stmt_list:
	/* epsilon */
	| stmt_list
stmt_list:
	stmt ';'
	| stmt ';' stmt_list
opt_stmt:
	/* epsilon */
	| stmt
stmt:
	WHILE '(' expr ')' opt_stmt
	| IF '(' expr ')' stmt else_clause
	| FOR for_control opt_stmt
	| ID '(' opt_expr_list ')'
	| RETURN opt_expr 
	| assg
	| '{' opt_stmt_list '}'
for_control:
	'(' opt_assg ';' opt_expr ';' opt_assg ')'
else_clause:
	/* epsilon */
	| ELSE stmt
opt_assg:
	/* epsilon */
	| assg
assg:
	ID opt_array_index '=' expr
opt_array_index:
	/* epsilon */
	| array_index
array_index:
	'[' expr ']'
opt_expr_list:
	/* epsilon */
	| expr_list
expr_list:
	expr
	| expr ',' expr_list
opt_expr:
	/* epsilon */
	| expr
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
	| ID opt_id_decorator {$$ = stringToExpr(current_scope,$1);}
	| INTEGER {$$ = newExpr(INT);}
	| DOUBLE {$$ = newExpr(DOUBLE);}
opt_id_decorator:
	/* epsilon */
	| id_decorator
id_decorator:
	'(' opt_expr_list ')'
	| '[' expr ']'
opt_loc_dcl_list:
	/* epsilon */ {$$ = newSymList();}
	| loc_dcl_list {$$ = $1;}
loc_dcl_list:
	loc_dcl {$$ = $1;}
	| loc_dcl loc_dcl_list {$$ = appendSymListToList($2,$1);}
loc_dcl:
	type id_list ';' {$$ = changeIDListToStringListAndSetType($2,$1);}
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




