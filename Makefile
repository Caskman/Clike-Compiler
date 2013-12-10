FLAGS=
TEMPLATE_EXE="../Structures/template/template"

clike: clike_funcs.c clike.tab.c clike.lex.c stringksymvhashtable.c typelist.c stringlist.c symlist.c exprlist.c stmtlist.c quadlistlist.c quadlist.c stringkstringvhashtable.c
	gcc -Wall ${FLAGS} clike_funcs.c clike.lex.c -lfl -o $@ clike.tab.c stringksymvhashtable.c stringksymventrylist.c typelist.c stringlist.c symlist.c exprlist.c stmtlist.c quadlistlist.c quadlist.c stringkstringvhashtable.c stringkstringventrylist.c

clike.lex.c: clike.tab.c clike.l
	flex -o clike.lex.c clike.l

clike.tab.c: clike.y
	bison -dv clike.y --report=solved

stringksymvhashtable.c: stringksymvhashtable_imp.h
	${TEMPLATE_EXE} -t "String,Sym"

typelist.c: typelist_imp.h
	${TEMPLATE_EXE} -l Type

stringlist.c: stringlist_imp.h
	${TEMPLATE_EXE} -l String

symlist.c: symlist_imp.h
	${TEMPLATE_EXE} -l Sym

exprlist.c: exprlist_imp.h
	${TEMPLATE_EXE} -l Expr

stmtlist.c: stmtlist_imp.h
	${TEMPLATE_EXE} -l Stmt

quadlistlist.c: quadlistlist_imp.h
	${TEMPLATE_EXE} -l QuadList

stringkstringvhashtable.c: stringkstringvhashtable_imp.h
	${TEMPLATE_EXE} -t "String,String"

quadlist.c: quadlist_imp.h
	${TEMPLATE_EXE} -l Quad

.PHONY: clean 

clean:
	rm -f clike *.a *.o clike.lex.* clike.tab.* `ls | grep .hashtable[.].` `ls | grep .list[.].` `ls | grep .hashtabletypes[.].` `ls | grep .entrylist_imp[.].` *.output








