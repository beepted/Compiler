/*

###############################################################
#	Programming Languages & Compiler Construction	      #
#		Stage 1: Scanner & Parser		      #
#	   Member 1: Sarthak Dash, 2006B4A7549P               #
#         Member 2: Rajat Tibrewal, 2006B5A7472P              #
#		Release Date: 28 Feb, 2010		      #
###############################################################

*/

#include "parser.h"

STable symbols;
FILE *token_file, *ftree;
int line_num = 1;
int parse_error_flag = 0;
char current_token[50];
void constructSTable();
void constructParsingTable();
int hashfunc(char *str, int size);
Symbol findSymbol(char *name);
Symbol insertSymbol(char *name, Type type);
Production insertProduction(ProductionTable prodtable, Symbol symbol);
Production findProduction(ProductionTable prodtable, char *terminal);
void saveParsingTables();
void loadParsingTables();
void advance(FILE *fp);

ParseTree initializeParser(char *filename)
{
	//input file is the token file name
	FILE *fp;
	char ptreename[50];
	ParseTree parse_tree = (ParseTree)malloc(sizeof(struct ptree));
	//check if parsing table is already available. 
	//If yes then read it, else compute and save it.
	fp = fopen("parsingtable","r");
	if(fp==NULL)
	{
		//construct first-follow sets
		constructSTable();
		//construct predictive parsing table
		constructParsingTable();
		//save parsing table to disk file
		saveParsingTables();
	}
	else
	{
		//read parsing table from disk
		loadParsingTables(fp);
		fclose(fp);
	}
	//Open token file and read the first symbol.
	token_file = fopen(filename,"r");
	advance(token_file);
	//invoke parser. Assume starting symbol of the grammar is "Programs"
	parse_tree = parse(findSymbol("Programs"),parse_tree);
	//now print the parse tree into the file
	strcpy(ptreename,filename);
	strcat(ptreename,".ptree");
	ftree = fopen(ptreename,"w");
	printParseTree(parse_tree,0);

	/*Expected call for the symbol table*/
	//dfs_parsetree(parse_tree);
	/*Call for Symbol Table*/

	fclose(ftree);
	fclose(token_file);
	return parse_tree;
}

void advance(FILE *fp)
{
	/*advance to next token in file and keep track of line number*/
	char c;
	if(!feof(fp))
	{
		fscanf(fp,"%*[ ]");
		fscanf(fp,"%[a-zA-Z0-9_:]",current_token);
		fscanf(fp,"%*[ ]");
		c=fgetc(fp);
		while(c=='\n') 
		{
			line_num++;
			c=fgetc(fp);
		}
		fseek(fp,-1,SEEK_CUR);
	}
	else
	{
		fprintf(stderr,"ERROR:: Unexpected end of file!\n");
	}
}

ParseTree parse(Symbol current_symbol, ParseTree root)
{
	/*recursive function to parse the tokens*/
	/*current_symbol: The symbol currently on top of the stack*/
	/*root: The pointer to the root of the relevant subtree of the Parse Tree*/
	int i;
	char id_or_num[50];
	char temp_token[20];
	ProductionTable prodtable;
	Production production;
	List prod_right;
	TreeNode tnode = (TreeNode)malloc(sizeof(struct tnode));
	strcpy(tnode->name,current_symbol->name);
	tnode->type = current_symbol->type;
	tnode->line_num = line_num;

	//first check if current_symbol is a terminal
	if(current_symbol->type == TERMINAL)
	{
		//Check if the current_symbol is ε. If yes then simply continue
		if(strcmp(current_symbol->name, "ε")==0)
		{
			root->node = tnode;
			root->num_branches = 0;
			return root;
		}
		//Check if its TK_ID or TK_NUM to strip additional data
		if((strcmp(current_symbol->name, "TK_ID")==0)||(strcmp(current_symbol->name, "TK_NUM")==0))
		{
			sscanf(current_token,"%*[^:]%*[:]%s",id_or_num);
			sscanf(current_token,"%[^:]",current_token);
			tnode->id_or_num = (char *)malloc(50*sizeof(char));
			strcpy(tnode->id_or_num,id_or_num);
		}

		//check if current token is a match
		if(strcmp(current_symbol->name, current_token)==0)
		{
			root->node = tnode;
			root->num_branches = 0;
			//we can eat the current token and move ahead
			if(strcmp(current_token,"EOF")!=0)
				advance(token_file);
			return root;
		}
		else
		{
			//Error Handling. return NULL.
			fprintf(stderr,"ERROR at line %d:: Expected %s. Found %s.\n",line_num,current_symbol->name,current_token);
			exit(1);
		}
	}
	else
	{
		//this is a nonterminal. so we need to apply the appropriate production from the parsing table
		sscanf(current_token,"%[^:]",temp_token);
		prodtable = current_symbol->prodtable;
		production = findProduction(prodtable, temp_token);
		if(production==NULL)
		{
			//Error Handling. return NULL.
			fprintf(stderr,"ERROR at line %d:: Expected symbols are ",line_num);
			for(i=0;i<current_symbol->num_first;i++)
				printf("%s, ",current_symbol->first[i].symbol->name);
			if(current_symbol->nullable==YES)
				for(i=0;i<current_symbol->num_follow;i++)
					printf("%s, ",current_symbol->follow[i].symbol->name);
			printf("Found %s.\n",temp_token);
			exit(1);
		}
		//appropriate production found. add current symbol to tree.
		root->node = tnode;
		prod_right = production->prod_right;
		//now create as many branches at current root as symbols in prod_right.
		root->branches = (ParseTree *)malloc(sizeof(ParseTree)*(production->num_symbols));
		root->num_branches = production->num_symbols;
		for(i=0;i<production->num_symbols;i++)
		{
			//applying rules for all symbols on right hand side of the rule.
			root->branches[i] = (ParseTree)malloc(sizeof(struct ptree));
			root->branches[i] = parse(prod_right[i].symbol, root->branches[i]);
		}
	}
	return root;	
}

void printParseTree(ParseTree parse_tree, int level)
{
	/*Prints the Parse Tree into the file 'parse_tree'*/
	/*level: The current depth of the parse tree*/
	int i,j;
	if(parse_tree->num_branches>0)
	{
		if(level==0)
			fprintf(ftree,"%s\n",parse_tree->node->name);
		for(i=0;i<parse_tree->num_branches;i++)
		{
			for(j=0;j<level;j++)
				fprintf(ftree,"  ");
			fprintf(ftree,"|_%s",parse_tree->branches[i]->node->name);
			if((strcmp(parse_tree->branches[i]->node->name, "TK_ID")==0)||\
			(strcmp(parse_tree->branches[i]->node->name, "TK_NUM")==0))
				fprintf(ftree,":%s",parse_tree->branches[i]->node->id_or_num);
			fprintf(ftree,"\n");
			printParseTree(parse_tree->branches[i],level+1);
		}
	}
}

void constructSTable()
{
	/*Reads in FIRST-FOLLOW Sets and puts all grammar symbols into Hash Table*/
	FILE *fp;
	char symbol[20];
	int i,num;
	symbols = (STable)malloc(sizeof(struct stable));
	symbols->symbol = (Symbol *)malloc(sizeof(struct symbol)*S_TABLE_SIZE);
	symbols->size = S_TABLE_SIZE;
	for(i=0;i<S_TABLE_SIZE;i++)
	{
		symbols->symbol[i]=NULL;
	}
	Symbol nonterm,term;

	fp=fopen("ffsets","r");
	while(!feof(fp))
	{
		//read symbol name and if nullable
		fscanf(fp,"%s",symbol);
		nonterm = insertSymbol(symbol,NONTERMINAL);
		fscanf(fp,"%s",symbol);
		if(strcmp(symbol,"NO")==0)
			nonterm->nullable = NO;
		else
			nonterm->nullable = YES;
		//read FIRST set
		fscanf(fp,"%d",&num);
		nonterm->num_first=num;
		nonterm->first=(List)malloc(sizeof(struct list)*num);
		for(i=0;i<num;i++)
		{
			fscanf(fp,"%s",symbol);
			term = findSymbol(symbol);
			if(term==NULL)
				term = insertSymbol(symbol,TERMINAL);
			nonterm->first[i].symbol=term;
		}
		//read FOLLOW set
		fscanf(fp,"%d",&num);
		nonterm->num_follow=num;
		nonterm->follow=(List)malloc(sizeof(struct list)*num);
		for(i=0;i<num;i++)
		{
			fscanf(fp,"%s",symbol);
			term = findSymbol(symbol);
			if(term==NULL)
				term = insertSymbol(symbol,TERMINAL);
			nonterm->follow[i].symbol=term;
		}
	}
	fclose(fp);
	//manually add terminal entry for ε (null production)
	term = insertSymbol("ε",TERMINAL); 
	term->nullable = YES;
	term->num_first = 0;
	term->num_follow = 0;
	term->first=NULL;
	term->follow=NULL;
	term->prodtable=NULL;
}

void constructParsingTable()
{
	/*read the set of Productions and constructs Parsing Table*/
	FILE *fp;
	int i,j,num,prod_count;
	char name[50];
	Symbol symbol,nonterminal;
	List prod_right, first_set;
	Production production;

	fp = fopen("productions","r");
	while(!feof(fp))
	{
		fscanf(fp,"%d %s",&num,name);
		//abstract out the non terminal whose production it is
		nonterminal = findSymbol(name);
		//construct list of symbols on the right hand side of production
		prod_right = (List)malloc(sizeof(struct list)*(num-1));
		for(i=0;i<(num-1);i++)
		{
			fscanf(fp,"%s",name);
			symbol = findSymbol(name);
			if(symbol==NULL)
				symbol = insertSymbol(name,TERMINAL);
			prod_right[i].symbol=symbol;
		}
		//now we need to fill the parsing table corresponding to nonterminal and the production
		if(nonterminal->prodtable==NULL)
		{
			nonterminal->prodtable = (ProductionTable)malloc(sizeof(struct prodtable));
			nonterminal->prodtable->productions = (Production)malloc((PROD_TABLE_SIZE)*sizeof(struct production));
		}
		//enter production X->beta in row X, column T for each T in FIRST[beta]
		for(i=0;i<(num-1);i++)
		{
			first_set=prod_right[i].symbol->first;
			for(j=0;j<prod_right[i].symbol->num_first;j++)
			{
				production = insertProduction(nonterminal->prodtable, first_set[j].symbol);	
				production->prod_right=prod_right;
				production->num_symbols=(num-1);
			}
			if(prod_right[i].symbol->nullable==YES)
				continue;
			else
				break;
		}
		//enter production X->beta in row X, column T for each T in FOLLOW[X], if nullable(beta)
		if(i==(num-1))
		{
			//nullable(beta)==YES
			for(j=0;j<nonterminal->num_follow;j++)
			{
				production = insertProduction(nonterminal->prodtable, nonterminal->follow[j].symbol);	
				production->prod_right=prod_right;
				production->num_symbols=(num-1);
			}
		}
		//calculate number of production criteria entered for the current symbol
		prod_count=0;
		for(i=0;i<PROD_TABLE_SIZE;i++)
		{
			if(nonterminal->prodtable->productions[i].terminal!=NULL)
				prod_count++;
		}
		nonterminal->prodtable->num_productions=prod_count;
	}
	fclose(fp);
}

int hashfunc(char *str, int size)
{
	int i;
	int sum = 0;
	for (i=0;i<strlen(str);i++)
		sum+= (i+1)*((int)str[i]);
	return sum % size;
}

Symbol findSymbol(char *name)
{
	/*Finds and returns a pointer to a particular Symbol in the Hash Table*/	
	int a;
	int i = 0;
	a = hashfunc(name,S_TABLE_SIZE);
	while (1)
	{
		a = (a+(i)+(i*i)) % symbols->size;
		if(symbols->symbol[a]==NULL)
		{
			return NULL;
		}
		else if(strcmp(symbols->symbol[a]->name,name)==0)
		{
			//the symbol is already there. return it.
			return symbols->symbol[a];
		}
		//keep looking.
		i++;
	}
}

Symbol insertSymbol(char *name, Type type)
{	
	/*Check and accordingly inserts a particular Symbol into the Hash Table*/
	int a;
	int i = 0;
	a = hashfunc(name,S_TABLE_SIZE);
	while (1)
	{
		//hashing with open-addressing used
		a = (a+(i)+(i*i)) % symbols->size;
		if(symbols->symbol[a]==NULL)
		{
			//the symbol is new. insert and return it.
			symbols->symbol[a] = (Symbol)malloc(sizeof(struct symbol));
			strcpy(symbols->symbol[a]->name,name);
			symbols->symbol[a]->type=type;
			symbols->symbol[a]->prodtable=NULL;
			if(type==TERMINAL)
			{
				symbols->symbol[a]->nullable = NO;
				symbols->symbol[a]->num_first = 1;
				symbols->symbol[a]->num_follow = 0;
				symbols->symbol[a]->first=(List)malloc(sizeof(struct list));
				symbols->symbol[a]->first->symbol=symbols->symbol[a];
				symbols->symbol[a]->follow=NULL;
			}
			return symbols->symbol[a];
		}
		else if(strcmp(symbols->symbol[a]->name,name)==0)
		{
			//the symbol is already there. return it.
			return symbols->symbol[a];
		}
		//keep looking.
		i++;
	}
}

Production findProduction(ProductionTable prodtable, char *terminal)
{
	/*searches for the production to be applied given a current token and current symbol*/
	int a;
	int i = 0;
	a = hashfunc(terminal,PROD_TABLE_SIZE);
	while (1)
	{
		a = (a+(i)+(i*i)) % PROD_TABLE_SIZE;
		if(prodtable->productions[a].terminal==NULL)
		{
			return NULL;
		}
		else if(strcmp(prodtable->productions[a].terminal->name,terminal)==0)
		{
			//the symbol is already there. return it.
			return &(prodtable->productions[a]);
		}
		//keep looking.
		i++;
	}
}

Production insertProduction(ProductionTable prodtable, Symbol symbol)
{	
	/*inserts a production to be applied for a given current_token and current_symbol*/
	int a;
	int i = 0;
	a = hashfunc(symbol->name,PROD_TABLE_SIZE);
	while (1)
	{
		a = (a+(i)+(i*i)) % PROD_TABLE_SIZE;
		if(prodtable->productions[a].terminal==NULL)
		{
			//the symbol is new. insert and return it.
			prodtable->productions[a].terminal=symbol;
			return &(prodtable->productions[a]);
		}
		else if(strcmp(prodtable->productions[a].terminal->name,symbol->name)==0)
		{
			//the symbol is already there. return it.
			return &(prodtable->productions[a]);
		}
		//keep looking.
		i++;
	}
}

void saveParsingTables()
{
	/*saves the data structures into file for quicker use next time onwards*/
	FILE *fp;
	fp = fopen("parsingtable","w");
	int i,j,k;
	Symbol s;
	ProductionTable pt;
	
	for(i=0;i<S_TABLE_SIZE;i++)
	{
		if(symbols->symbol[i]->name!=NULL)
		{
			//the symbol exists here. so fprint it.
			s = symbols->symbol[i];
			fprintf(fp,"%s %d %d\n",s->name,(int)s->nullable,(int)s->type);	
			//print in the FIRST list
			fprintf(fp,"%d",s->num_first);
			for(j=0;j<s->num_first;j++)
				fprintf(fp," %s",s->first[j].symbol->name);
			fprintf(fp,"\n");
			if(s->type==NONTERMINAL)
			{
				//print in FOLLOW only for non-terminals
				fprintf(fp,"%d",s->num_follow);
				for(j=0;j<s->num_follow;j++)
					fprintf(fp," %s",s->follow[j].symbol->name);
				fprintf(fp,"\n");
				//print in Production Table only for non-terminals
				pt=s->prodtable;
				fprintf(fp,"%d\n",pt->num_productions);
				for(j=0;j<PROD_TABLE_SIZE;j++)
				{
					if(pt->productions[j].terminal!=NULL)
					{
						fprintf(fp,"%s\n",pt->productions[j].terminal->name);
						fprintf(fp,"%d",pt->productions[j].num_symbols);
						for(k=0;k<pt->productions[j].num_symbols;k++)
							fprintf(fp," %s %d",pt->productions[j].prod_right[k].symbol->name,\
							(int)pt->productions[j].prod_right[k].symbol->type);
						fprintf(fp,"\n");
					}
				}
			}
		}
	}
	fclose(fp);
}

void loadParsingTables(FILE *fp)
{
	/*loads the saved data structures from the file*/
	int i,j,k,num;
	Nullable nullable;
	Type type;
	char sname[20];
	symbols = (STable)malloc(sizeof(struct stable));
	symbols->symbol = (Symbol *)malloc(sizeof(struct symbol)*S_TABLE_SIZE);
	symbols->size = S_TABLE_SIZE;
	Symbol s,term;
	Production production;
	
	while(!feof(fp))
	{
		//read symbol name, nullable & type
		fscanf(fp,"%s %d %d",sname,(int*)&nullable,(int*)&type);
		s=insertSymbol(sname,type);
		s->nullable=nullable;
		//read in FIRST set
		fscanf(fp,"%d",&num);
		s->num_first = num;
		s->first=(List)malloc(sizeof(struct list)*num);
		for(i=0;i<num;i++)
		{
			fscanf(fp,"%s",sname);
			term = findSymbol(sname);
			if(term==NULL)
				term = insertSymbol(sname,TERMINAL);
			s->first[i].symbol=term;
		}
		if(type==NONTERMINAL)
		{
			//read in FOLLOW set
			fscanf(fp,"%d",&num);
			s->num_follow=num;
			s->follow=(List)malloc(sizeof(struct list)*num);
			for(i=0;i<num;i++)
			{
				fscanf(fp,"%s",sname);
				term = findSymbol(sname);
				if(term==NULL)
					term = insertSymbol(sname,TERMINAL);
				s->follow[i].symbol=term;
			}
			//read in ProductionTable
			if(s->prodtable==NULL)
			{
				s->prodtable = (ProductionTable)malloc(sizeof(struct prodtable));
				s->prodtable->productions = (Production)malloc((PROD_TABLE_SIZE)*sizeof(struct production));
			}
			fscanf(fp,"%d",&num);
			s->prodtable->num_productions=num;
			for(i=0;i<num;i++)
			{
				fscanf(fp,"%s",sname);
				term = findSymbol(sname);
				if(term==NULL)
					term = insertSymbol(sname,TERMINAL);
				production = insertProduction(s->prodtable,term);
				fscanf(fp,"%d",&k);
				production->num_symbols = k;
				production->prod_right = (List)malloc(sizeof(struct list)*k);
				for(j=0;j<k;j++)
				{
					fscanf(fp,"%s %d",sname,(int*)&type);
					term = findSymbol(sname);
					if(term==NULL)
						term = insertSymbol(sname,type);
					production->prod_right[j].symbol=term;
				}
			}

		}
	}
}



