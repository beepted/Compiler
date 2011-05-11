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
#ifndef PARSER
#define PARSER
#include "parser.h"
#endif
#define LEX_SIZE 50
#define TYPE_SIZE 40
#define ENTRIES_PER_HASHTABLE 60
#define MAX_NO_OF_BRANCHES 10

typedef enum{INT,NONE,ERROR}Typ;
typedef enum{NORMAL,FUNCTION}diff;

typedef struct funtype
{
	Typ *params;
	Typ result;
	int num_params;
}Funtype;

typedef struct node
{
	char lexeme[LEX_SIZE];
	union
	{
		Typ idtype;
		Funtype fun_type;
	}typ;
	diff dff;   //tag for knowing whether its an id or a function
	int value;   //storing the count of the parameters 
}symbolnode;

struct hashtable
{
	char hashname[LEX_SIZE];
	symbolnode *sym_node;
	int size;
};

typedef struct hashtable *HashTable;

typedef struct symboltable *SymbolTable;
struct symboltable
{
	HashTable htable;
	SymbolTable *next;
	SymbolTable previous;
	int num_of_branches;
};
//extern void fillsymboltable(SymbolTable S,symbolnode snode);
//The bottom defination has been pulled out from symboltable.c
extern SymbolTable searchone(char *str,SymbolTable S);
extern int hashfunc_symtable(char *str);
extern SymbolTable symboltable(ParseTree parse_tree,char *file);
