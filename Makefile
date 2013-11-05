FLAGS=
TEMPLATE_EXE="../Structures/template/template"

parse: clike_funcs.c clike.tab.c clike.lex.c stringkfuncsymvhashtable.c stringkvarsymvhashtable.c typelist.c stringlist.c funcsymlist.c
	gcc -Wall ${FLAGS} clike_funcs.c clike.lex.c -lfl -o $@ clike.tab.c stringkfuncsymvhashtable.c stringkfuncsymventrylist.c stringkvarsymvhashtable.c stringkvarsymventrylist.c typelist.c stringlist.c funcsymlist.c 

clike.lex.c: clike.tab.c clike.l
	flex -o clike.lex.c clike.l

clike.tab.c: clike.y
	bison -dv clike.y --report=solved

stringkfuncsymvhashtable.c: stringkfuncsymvhashtable_imp.h
	${TEMPLATE_EXE} -t "String,FuncSym"

stringkvarsymvhashtable.c: stringkvarsymvhashtable_imp.h
	${TEMPLATE_EXE} -t "String,VarSym"

typelist.c: typelist_imp.h
	${TEMPLATE_EXE} -l Type

stringlist.c: stringlist_imp.h
	${TEMPLATE_EXE} -l String

funcsymlist.c: funcsymlist_imp.h
	${TEMPLATE_EXE} -l FuncSym

.PHONY: clean 

clean:
	rm -f parse *.a *.o clike.lex.* clike.tab.* `ls | grep .hashtable[.].` `ls | grep .list[.].` `ls | grep .hashtabletypes[.].` `ls | grep .entrylist_imp[.].` *.output

# parse: clike_structures.a clike_funcs.o clike.tab.o clike.lex.o  
# 	gcc -Wall ${FLAGS} clike_funcs.o clike_structures.a clike.lex.o -lfl -o $@ clike.tab.o funcsymlist.o

# clike.lex.o: clike.tab.o clike.l
# 	flex -o clike.lex.c clike.l
# 	gcc ${FLAGS} -c clike.lex.c

# clike_funcs.o: clike_funcs.c clike.tab.o
# 	gcc -Wall ${FLAGS} -c clike_funcs.c

# clike.tab.o: clike.y
# 	bison -dv clike.y --report=solved
# 	gcc ${FLAGS} -c clike.tab.c

# clike_structures.a: typelist.o stringlist.o stringkfuncsymvhashtable.a stringkvarsymvhashtable.a funcsymlist.o
# 	ar cr $@ `ar t stringkfuncsymvhashtable.a` `ar t stringkvarsymvhashtable.a` typelist.o stringlist.o funcsymlist.o

# stringkfuncsymvhashtable.a: stringkfuncsymvhashtable_imp.h
# 	${TEMPLATE_EXE} -t "String,FuncSym"
# 	gcc -Wall ${FLAGS} -c stringkfuncsymventrylist.c
# 	gcc -Wall ${FLAGS} -c stringkfuncsymvhashtable.c
# 	ar cr $@ stringkfuncsymvhashtable.o stringkfuncsymventrylist.o

# stringkvarsymvhashtable.a: stringkvarsymvhashtable_imp.h
# 	${TEMPLATE_EXE} -t "String,VarSym"
# 	gcc -Wall ${FLAGS} -c stringkvarsymventrylist.c
# 	gcc -Wall ${FLAGS} -c stringkvarsymvhashtable.c
# 	ar cr $@ stringkvarsymvhashtable.o stringkvarsymventrylist.o

# typelist.o: typelist_imp.h
# 	${TEMPLATE_EXE} -l Type
# 	gcc -Wall ${FLAGS} -c typelist.c

# stringlist.o: stringlist_imp.h
# 	${TEMPLATE_EXE} -l String
# 	gcc -Wall ${FLAGS} -c stringlist.c

# funcsymlist.o: funcsymlist_imp.h
# 	${TEMPLATE_EXE} -l FuncSym
# 	gcc -Wall ${FLAGS} -c funcsymlist.c


# .PHONY: clean 

# clean:
# 	rm -f parse *.a *.o clike.lex.* clike.tab.* `ls | grep .hashtable[.].` `ls | grep .list[.].` `ls | grep .hashtabletypes[.].` `ls | grep .entrylist_imp[.].` *.output







