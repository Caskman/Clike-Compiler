 /* Author: Eric Cascketta
  * Date: 9/15/2013
  * CSC 453
  */
%option yylineno
%{
	#include "clike.h"
	#include "clike.tab.h"
%}

%x COMMENT STRING

%%
 /* ===Comments=== */
"//".* {}
"/*" {inComments = 1; BEGIN(COMMENT);}
<COMMENT>"*/" {inComments = 0; BEGIN(INITIAL);}
<COMMENT>[^\n*]+ {}
<COMMENT>\n {line++;}
<COMMENT>"*" {}


 /* ===Keywords=== */
"_Bool" {return _BOOL;}
"_Complex" {return _COMPLEX;}
"auto" {return AUTO;}
"const" {return CONST;}
"restrict" {return RESTRICT;}
"enum" {return ENUM;}
"_Imaginary" {return _IMAGINARY;}
"inline" {return INLINE;}
"register" {return REGISTER;}
"static" {return STATIC;}
"volatile" {return VOLATILE;}
"break" {return BREAK;} 
"case" {return CASE;}
"char" {return CHAR;}
"continue" {return CONTINUE;}
"default" {return DEFAULT;}
"do" {return DO;}
"double" {return DOUBLE_DECL;}
"else" {return ELSE;}
"extern" {return EXTERN;}
"float" {return FLOAT;}
"for" {return FOR;}
"goto" {return GOTO;}
"if" {return IF;}
"int" {return INT;}
"long" {return LONG;}
"return" {return RETURN;}
"short" {return SHORT;}
"signed" {return SIGNED;}
"sizeof" {return SIZEOF;}
"struct" {return STRUCT;}
"switch" {return SWITCH;}
"typedef" {return TYPEDEF;}
"union" {return UNION;}
"unsigned" {return UNSIGNED;}
"void" {return VOID;}
"while" {return WHILE;}

 /* Identifiers */
[a-zA-Z_]+([a-zA-Z_]|[0-9])* {yylval.string = strdup(yytext); return ID;} 

 /* Decimal Integers */
[1-9][0-9]*(([uU][lL]{1,2})|([lL]{1,2}[uU]?))? {yylval.d = atoi(yytext); return INTEGER;}

 /* Octal Integers */
0[0-7]*(([uU][lL]{1,2})|([lL]{1,2}[uU]?))? { yylval.d = atoi(yytext); return INTEGER;}

 /* Hex Integers */
0[xX][0-9a-fA-F]+(([uU][lL]{1,2})|([lL]{1,2}[uU]?))? {yylval.d = atoi(yytext); return INTEGER;}

 /* Floating-point */
(([0-9]*[.][0-9]+)|([0-9]+[.]?))([eE][+-]?[0-9]+)?([fF])? {yylval.f = atof(yytext); return DOUBLE;}

 /* Chars */
['](([^'\\\n])|(([\a\b\f\n\r\t\v\'\"\\\\\?])|(\\[0-7]{1,3})|(\\x[0-9a-fA-F]+)))['] {return yytext[0];}




 /* Single-character tokens */
"!" |
"%" |
"^" |
"&" |
"*" |
"-" |
"+" |
"=" |
"~" |
"|" |
"." |
"<" |
">" |
"/" |
"?" |
"(" |
")" |
"[" |
"]" |
"{" |
"}" |
"," |
";" |
":" |
"#" {yylval.d = yytext[0]; return yytext[0];}

 /* Multi-character operators */
"+=" {yylval.d = PLUS_EQ; return PLUS_EQ;}
"-=" {yylval.d = MIN_EQ; return MIN_EQ;}
"*=" {yylval.d = MULT_EQ; return MULT_EQ;}
"/=" {yylval.d = DIV_EQ; return DIV_EQ;}
"%=" {yylval.d = PERC_EQ; return PERC_EQ;}
"<<=" {yylval.d = BSHFTL_EQ; return BSHFTL_EQ;}
">>=" {yylval.d = BSHFTR_EQ; return BSHFTR_EQ;}
"&=" {yylval.d = AND_EQ; return AND_EQ;}
"^=" {yylval.d = XOR_EQ; return XOR_EQ;}
"|=" {yylval.d = OR_EQ; return OR_EQ;}
"->" {yylval.d = DERIVES; return DERIVES;}
"++" {yylval.d = INC; return INC;}
"--" {yylval.d = DECR; return DECR;}
">>" {yylval.d = BSHFTR; return BSHFTR;}
"<<" {yylval.d = BSHFTL; return BSHFTL;}
">=" {yylval.d = GREAT_EQ; return GREAT_EQ;}
"<=" {yylval.d = LESS_EQ; return LESS_EQ;}
"==" {yylval.d = D_EQ; return D_EQ;}
"!=" {yylval.d = NOT_EQ; return NOT_EQ;}
"&&" {yylval.d = D_AMP; return D_AMP;}
"||" {yylval.d = D_BAR; return D_BAR;}
"..." {yylval.d = ELL; return ELL;}

 /* Strings */
\"(\\.|[^"])*\" {yylval.string = strdup(yytext); return STRING;}


 /* Newlines */
\n {line++;}

 /* Tabs and whitespace */
[ \t] {}

 /* Everything else */
. {fprintf(stderr,"Unknown character:%c\n",*yytext);}


%%

 /*int main() {
	yylex();
	yylex_destroy();
	if (inComments) {
		fprintf(stderr,"%d, Unterminated Comment\n",line);
	}
}*/

