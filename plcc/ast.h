/*
###############################################################
#	Programming Languages & Compiler Construction	      #
#	Stage 1 and 2: Scanner,Parser,SymbolTable,AST	      #
#	   Member 1: Sarthak Dash, 2006B4A7549P               #
#         Member 2: Rajat Tibrewal, 2006B5A7472P              #
#		Release Date: 25 March, 2010		      #
###############################################################
*/

#ifndef GUARD
#define GUARD
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#endif

#ifndef SYMTABLE
#define SYMTABLE
#include "symboltable.h"
#endif

#ifndef PARSER
#define PARSER
#include "parser.h"
#endif

typedef struct AST *Syntaxtree;

struct AST
{
	char name[50];
	symbolnode* idlink;
	Typ type;
	Syntaxtree first_child;
	Syntaxtree parent;
	Syntaxtree sibling;
};

extern Syntaxtree initializeAST(SymbolTable sym, ParseTree p_tree, char *filename);
