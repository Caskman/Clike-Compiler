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
[1-9][0-9]*(([uU][lL]{1,2})|([lL]{1,2}[uU]?))? { return INTEGER;}

 /* Octal Integers */
0[0-7]*(([uU][lL]{1,2})|([lL]{1,2}[uU]?))? { return INTEGER;}

 /* Hex Integers */
0[xX][0-9a-fA-F]+(([uU][lL]{1,2})|([lL]{1,2}[uU]?))? { return INTEGER;}

 /* Floating-point */
(([0-9]*[.][0-9]+)|([0-9]+[.]?))([eE][+-]?[0-9]+)?([fF])? { return DOUBLE;}

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
"#" {return yytext[0];}

 /* Multi-character operators */
"+=" {return PLUS_EQ;}
"-=" {return MIN_EQ;}
"*=" {return MULT_EQ;}
"/=" {return DIV_EQ;}
"%=" {return PERC_EQ;}
"<<=" {return BSHFTL_EQ;}
">>=" {return BSHFTR_EQ;}
"&=" {return AND_EQ;}
"^=" {return XOR_EQ;}
"|=" {return OR_EQ;}
"->" {return DERIVES;}
"++" {return INC;}
"--" {return DECR;}
">>" {return BSHFTR;}
"<<" {return BSHFTL;}
">=" {return GREAT_EQ;}
"<=" {return LESS_EQ;}
"==" {return D_EQ;}
"!=" {return NOT_EQ;}
"&&" {return D_AMP;}
"||" {return D_BAR;}
"..." {return ELL;}

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

