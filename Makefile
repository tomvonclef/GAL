#
# Makefile for CS461 GAL Project
# ebegoli, 2/2011
#

CCC = g++
CCFLAGS= -O2 -g
LEX = flex
LFLAGS= -8     
YACC= bison 
YFLAGS= -d -y -t
SRC=src
BIN=bin
BUILD=build

RM = /bin/rm -f
CP = /bin/cp
MV = /bin/mv

all: clean gal.o write_python_funcs.o y.tab.o lex.yy.o
	${CCC} ${CCFLAGS} ${BUILD}/lex.yy.o ${BUILD}/y.tab.o\
	   ${BUILD}/gal.o ${BUILD}/write_python_funcs.o -o ${BUILD}/gal -lfl

gal.o:  ${SRC}/gal.h ${SRC}/gal.cpp 	
	${CP} ${SRC}/gal.h ${SRC}/gal.cpp ${BUILD}
	${CCC} -c ${BUILD}/gal.cpp -o ${BUILD}/gal.o 	 

y.tab.o: ${SRC}/gal.yacc 
	${CP} ${SRC}/gal.yacc ${BUILD}
	${YACC} ${YFLAGS} ${BUILD}/gal.yacc
	${MV} y.tab.* ${BUILD}
	${CCC} ${CCFLAGS} ${BUILD}/y.tab.c -c -o ${BUILD}/y.tab.o

write_python_funcs.o:  ${SRC}/write_python_funcs.h ${SRC}/write_python_funcs.cpp 	
	${CP} ${SRC}/write_python_funcs.h ${SRC}/write_python_funcs.cpp ${BUILD}
	${CCC} -c ${BUILD}/write_python_funcs.cpp -o ${BUILD}/write_python_funcs.o 	 

lex.yy.o: ${SRC}/gal.lex y.tab.o gal.o write_python_funcs.o
	${CP} ${SRC}/gal.lex ${BUILD}
	${LEX} $(LFLAGS) ${BUILD}/gal.lex
	${MV} lex.yy.c ${BUILD}
	${CCC} ${CCFLAGS} ${BUILD}/lex.yy.c -c -o ${BUILD}/lex.yy.o


verify:
	python --version
	flex --version
	bison --version

install: all
	${CP} ${BUILD}/gal ${BIN}
	${CP} ${SRC}/gal_funcs.py .


clean: 
	${RM} ${BUILD}/*

.PHONY: test

test:
	cd test && ./runtests.sh 

