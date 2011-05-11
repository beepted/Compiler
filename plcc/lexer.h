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
#define BUFFER_SIZE 1024
#define HASH_SIZE 24

typedef struct token
{
	char token_name[50];
	char token_lexeme[25];
}Token;

typedef struct buffer
{
	char buf[BUFFER_SIZE];
}*Buffer;

typedef struct hash
{
	Token *bucket;
}*Hashtable;



extern char* initializeScanner(char *filename); // initializes the scanner
extern Token nextToken(); // should scan and return the next token
extern void printTokenStream(FILE f); // prints out the token stream for a given program 
