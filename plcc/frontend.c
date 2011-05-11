/*

###############################################################
#	Programming Languages & Compiler Construction	      #
#		Stage 1: Scanner & Parser		      #
#	   Member 1: Sarthak Dash, 2006B4A7549P               #
#         Member 2: Rajat Tibrewal, 2006B5A7472P              #
#		Release Date: 28 Feb, 2010		      #
###############################################################

*/

#ifndef GUARD
#define GUARD
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#endif
#include "lexer.h"
#ifndef PARSER
#define PARSER
#include "parser.h"
#endif
#ifndef SYMTABLE
#define SYMTABLE
#include "symboltable.h"
#endif
#ifndef STREE
#define STREE
#include "ast.h"
#endif
#ifndef CODE
#define CODE
#include "codegen.h"
#endif

int main(int argc, char *argv[])
{
	char *file = initializeScanner(argv[1]);
	printf("Lexical Analysis Complete!\n");
	ParseTree parse_tree = initializeParser(file);
	printf("Parsing Completed!\n");
	SymbolTable Symtab = symboltable(parse_tree,argv[1]);
	printf("Symbol Table Generated!\n");
	Syntaxtree S = initializeAST(Symtab,parse_tree,argv[1]);
	printf("Type checked Abstract Syntax Tree Generated!\n");
	generate_x86code(S,Symtab,argv[1]);
	printf("8086 Assembly Code generated in file: %s.asm\n",argv[1]);
	return 0;
}
