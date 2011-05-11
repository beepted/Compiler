/*
###############################################################
#	Programming Languages & Compiler Construction	      #
#  Stage 1,2,3: Scanner,Parser,SymbolTable,AST,CodeGenerator  #
#	   Member 1: Sarthak Dash, 2006B4A7549P               #
#         Member 2: Rajat Tibrewal, 2006B5A7472P              #
#		Release Date: 22 April, 2010		      #
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

#ifndef STREE
#define STREE
#include "ast.h"
#endif

#define NUM_REGISTERS 4
extern void generate_x86code(Syntaxtree ast, SymbolTable symboltable, char *filename); 
