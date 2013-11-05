
#ifndef CLIKE
#define CLIKE
#define CASKDEBUG 0

void yyerror(char const *s);
void logg(char const *s);
int yylex_destroy();
int yyparse();

extern int yylineno;
extern int inComments;
extern int line;

int main();
FuncSymList* makeSEFuncSymList(FuncSym *data);

#endif
