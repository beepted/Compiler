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
#define S_TABLE_SIZE 100
#define PROD_TABLE_SIZE 50

typedef struct symbol *Symbol;

typedef struct list
{
	Symbol symbol;
}*List;

typedef enum nullable
{
	YES=1,
	NO=0
}Nullable;

typedef enum type
{
	TERMINAL=1,
	NONTERMINAL=0
}Type;

typedef struct production
{
	Symbol terminal; //terminal symbol for which this production is chosen
	List prod_right;
	int num_symbols;
}*Production;

typedef struct prodtable //stores the productions to be applied for a given token
{
	Production productions;
	int num_productions;
}*ProductionTable;

struct symbol //terminal or non-terminal
{
	char name[20];
	List first;
	List follow;
	int num_first;
	int num_follow;
	Nullable nullable;
	Type type;
	ProductionTable prodtable;
};

typedef struct stable
{
	Symbol *symbol;
	int size;
}*STable;

typedef struct ptree *ParseTree;

typedef struct tnode
{
	char name[20];
	Type type;
	char *id_or_num;
	int line_num;
}*TreeNode;

struct ptree
{
	TreeNode node;
	ParseTree *branches;
	int num_branches;
};

extern ParseTree initializeParser(char *filename);
extern ParseTree parse(Symbol current_symbol, ParseTree root);
extern void printParseTree(ParseTree parse_tree, int level);
