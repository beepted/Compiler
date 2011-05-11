/*
###############################################################
#	Programming Languages & Compiler Construction	      #
#	Stage 1 and 2: Scanner,Parser,SymbolTable,AST	      #
#	   Member 1: Sarthak Dash, 2006B4A7549P               #
#         Member 2: Rajat Tibrewal, 2006B5A7472P              #
#		Release Date: 25 March, 2010		      #
###############################################################
*/


#include "symboltable.h"
#include "math.h"

typedef enum{FALSE=0,TRUE}Boolean;
typedef enum{NOTHING_RECEIVED,INT_RECEIVED,FUN_RECEIVED,PARAMETERS,RESULT,ID_RECEIVED,EQUAL_RECEIVED,INT_RESULT,NONE_RESULT}Status;

SymbolTable createnew(SymbolTable S,char *str);
void insert(symbolnode snode,HashTable h);
Boolean search(char *str,SymbolTable S);
//Here the defination of Searchone has been removed
void dfslookahead(ParseTree parse_tree,SymbolTable S);
void printSymbolTable(SymbolTable head,int count,FILE *fp);
void dfs_parsetree(ParseTree parse_tree,SymbolTable S);
int integer(char *str);
void fillvalue(SymbolTable S,char *str,int value);
void overloadcheck(char *str,SymbolTable S);


Status status=NOTHING_RECEIVED;
Boolean oloadcheck = FALSE;
symbolnode symglobalnode;
SymbolTable tempset,head;
int set = 0;
int paramcount = 0;
char globalstring[50];
char functionvalue[50];

SymbolTable symboltable(ParseTree parse_tree,char *file)   //is the main function for this part of the code
{
	int i;
	FILE *fp;
	char symbol_file[50];
	SymbolTable S;
	S=(SymbolTable)malloc(sizeof(struct symboltable));
	S->htable=(HashTable)malloc(sizeof(struct hashtable));


	S->htable->sym_node = (symbolnode *)malloc(sizeof(struct node)*ENTRIES_PER_HASHTABLE);
	S->htable->size = 0;
	strcpy(S->htable->hashname,"main");
	for (i=0;i<ENTRIES_PER_HASHTABLE;i++)
	{
		S->htable->sym_node[i].lexeme[0]='\0';
		//S->htable->sym_node[i].type[0]='\0';
		S->htable->sym_node[i].value=0;
	}
	S->next = (SymbolTable *)malloc(sizeof(SymbolTable)*MAX_NO_OF_BRANCHES);
	for (i=0;i<MAX_NO_OF_BRANCHES;i++)
		S->next[i] = NULL;
	S->num_of_branches = 0;
	S->previous = NULL;
	head = S;    //Till this point one hashtable is created which stores the symbol corresponding to the main function
	globalstring[0]='\0';
	functionvalue[0]='\0';
	dfs_parsetree(parse_tree,S);    //this function is responsible for creating the other symbol tables
	//printf("THE SYMBOL TABLE LOOKS LIKE \n");
	strcpy(symbol_file,file);
	strcat(symbol_file,".symtab");
	//puts(symbol_file);
	fp = fopen(symbol_file,"w");
	printSymbolTable(S,1,fp);
	fprintf(fp,"KEY FOR SYMBOLTABLE:::0 for Parameters and Result stands for INT and 1 for NONE Data Types:::And for values 0 stands for 0 as a value\n");
	fclose(fp);
	//printf("The address of the head symboltable looks like %x\n",head);
	return head;
}
void dfslookahead(ParseTree p_tree,SymbolTable S)   //this function finds out the name of the function thats coming ahead in the parsetree
{
	int i;
	if (strcmp(p_tree->node->name,"ε") == 0)
		set = 2;
	if ((strcmp(p_tree->node->name,"TK_ID") == 0) && set == 0)
	{
		//means you have read an ID
		tempset = searchone(p_tree->node->id_or_num,S);  //At this point we find out which of the hashtable we need to connect to for this particular function.
		strcpy(functionvalue,p_tree->node->id_or_num);
		set = 1;
	}
	for (i=0;i<p_tree->num_branches;i++)
	{
		if (set == 0)
			dfslookahead(p_tree->branches[i],S);
		else 
			return;
	}
}
void overloadcheck(char* str,SymbolTable S)  //this function checks for overloading i.e. if a function with the same name exists, its either a multiple declaration or overloading.
{
	int i;
	if (strcmp(str,S->htable->hashname)==0)
	{
		oloadcheck = TRUE;
		return;
	}
	for (i=0;i<S->num_of_branches;i++)
		overloadcheck(str,S->next[i]);
}
void dfs_parsetree(ParseTree parse_tree,SymbolTable S)  //this is the function that parses the parsetree and creates relevant symboltables along with scopes
{
//	int flag=0;
	int i,count=0;
	char current_token[50],id_or_num[20];
	current_token[0]='\0';id_or_num[0]='\0';
	Boolean sol = FALSE;
	symbolnode snode;
	if (parse_tree->node != NULL)
	{
		if (strcmp(parse_tree->node->name,"OtherFuns")==0)
		{
			ParseTree temp = parse_tree;
			dfslookahead(temp,S);
			if (set == 2)  //means its the END
				return;	
			set = 0;
			if (tempset != NULL)
			{
				overloadcheck(functionvalue,head);
				if (oloadcheck == TRUE)
				{
					fprintf(stderr,"Function %s has been overloaded\n",functionvalue);
					oloadcheck = FALSE;
					exit(1);
				}
				S=createnew(tempset,functionvalue);
				temp=NULL;
			}
			else
			{
				printf("ERROR::Function hasnot been declared earlier\n");
				exit(1);
			}
			functionvalue[0]='\0';
			tempset = NULL;
		}
		if (strcmp(parse_tree->node->name,"MainFun")==0)   //means first begin statement
		{
			count = 1;   //denotes that we are inside MainFun while parsing the ParseTree	
		}
		if (strcmp(parse_tree->node->name,"TK_KEY_NONE")==0)
		{
			if (status == PARAMETERS)   //means only one parameter that is also none
			{
				 //status = NONE_PARAMETERS;
				 paramcount = 0;   //1 signifies that we have to store only TK_KEY_NONE once
			}
			if (status == RESULT)
			    status = NONE_RESULT;
		}
		if (strcmp(parse_tree->node->name,"TK_KEY_INT")==0)
		{
			if (status != PARAMETERS && status != RESULT)
			{
				status = INT_RECEIVED;
			}
			else if (status == PARAMETERS)
			{
				paramcount++;
				//strcat(symglobalnode.type,"INT,");
			}
			else if (status == RESULT)
				status = INT_RESULT;
		}
		if (status == INT_RESULT || status == NONE_RESULT)
		{
			//strcat(symglobalnode.type,"INT");
			if (paramcount == 0)   //means there were no parameters ; only one i.e.NONE will be stored
			{
			    symglobalnode.typ.fun_type.params = (Typ*)malloc(sizeof(Typ));
			    if (symglobalnode.typ.fun_type.params == NULL)
			    {
				      fprintf(stderr,"ERROR::Memory Allocation problem\n");
				      exit(1);
			    }
			    symglobalnode.dff = FUNCTION;
			    symglobalnode.typ.fun_type.num_params = 0;
			    symglobalnode.typ.fun_type.params[0] = NONE;
			    //printf("\n");
			    if (status == INT_RESULT)
				  symglobalnode.typ.fun_type.result = INT;
			    else if (status == NONE_RESULT)
				  symglobalnode.typ.fun_type.result = NONE;
			    symglobalnode.value=0;
			    insert(symglobalnode,S->htable);
			    symglobalnode.lexeme[0]='\0';
			    status = NOTHING_RECEIVED;
			    paramcount = 0;
			}
			else{    
			symglobalnode.typ.fun_type.params = (Typ*)malloc(sizeof(Typ)*paramcount);
			if (symglobalnode.typ.fun_type.params == NULL)
			{
				fprintf(stderr,"ERROR::Memory Allocation problem\n");
				exit(1);
			}
			symglobalnode.dff = FUNCTION;
			symglobalnode.typ.fun_type.num_params = paramcount;
			//printf("Number of parameters for %s is %d\n",symglobalnode.lexeme,symglobalnode.typ.fun_type.num_params);
			for (i=0;i<paramcount;i++)
			{
				symglobalnode.typ.fun_type.params[i] = INT;
				//printf("Value status:%d ",symglobalnode.typ.fun_type.params[i]);  //TILL THIS POINT IT TAKES CORRECTLY
			}
			//printf("\n");
			if (status == INT_RESULT)
			    symglobalnode.typ.fun_type.result = INT;
			else if (status == NONE_RESULT)
			    symglobalnode.typ.fun_type.result = NONE;
			symglobalnode.value=0;
			insert(symglobalnode,S->htable);
			symglobalnode.lexeme[0]='\0';
			status = NOTHING_RECEIVED;
			paramcount = 0;
			}
		}
		if (strcmp(parse_tree->node->name,"TK_KEY_FUN")==0)
		  status = FUN_RECEIVED;
		else if (strcmp(parse_tree->node->name,"TK_SEMI") ==0)
			status = NOTHING_RECEIVED;
		if (strcmp(parse_tree->node->name,"TK_OPEN")==0 && status == FUN_RECEIVED)
		{
			status = PARAMETERS;
			//symglobalnode.type[0]='\0';
		}
		if (strcmp(parse_tree->node->name,"TK_KEY_RESULT")==0)
		{
			  status = RESULT;
			//if (status == NONE_PARAMETERS)
			//	status = NONE_RESULT;
			//else if (status == INT_PARAMETERS)
			//	status = INT_RESULT;
		}
		if (status == ID_RECEIVED)
		{
			if (strcmp(parse_tree->node->name,"TK_EQUAL") == 0)
				status = EQUAL_RECEIVED;
			else
				status = NOTHING_RECEIVED;
		}
		if (strcmp(parse_tree->node->name,"TK_NUM")==0 && status == EQUAL_RECEIVED)
		{
			int value;
			//printf("String value %s\n",parse_tree->node->id_or_num);
			value = integer(parse_tree->node->id_or_num);   //converts the value in the string format to integer format
			//printf("Values received %d\n",value);
			fillvalue(S,globalstring,value);   //fills in the corresponding values for the corresponding string
			globalstring[0]='\0';
			status = NOTHING_RECEIVED;
		}
		if (strcmp(parse_tree->node->name,"TK_ID")== 0)
		{
			if (status == EQUAL_RECEIVED)  //part of an assignment statement
			{
				status = NOTHING_RECEIVED;
				globalstring[0]='\0';
			}
			if (status == INT_RECEIVED)
			{
				sol=FALSE;
				sol=search(parse_tree->node->id_or_num,S);
				if (sol == FALSE)
				{
					strcpy(snode.lexeme,parse_tree->node->id_or_num);
					snode.typ.idtype = INT;
					snode.dff = NORMAL;
					//strcpy(snode.type,"INT");
					snode.value=0;
					insert(snode,S->htable);
				}
				else if (sol == TRUE) //Multiple Declarations of the same symbol
				{
					fprintf(stderr,"ERROR::Multiple declarations of the same symbol %s\n",parse_tree->node->id_or_num);
					exit(1);
				}
				status = ID_RECEIVED;  /*Line is new this session*/
				strcpy(globalstring,parse_tree->node->id_or_num);
				//status = NOTHING_RECEIVED;  /*Line has been changed*/
			}
			else 
			{
				if (status == NOTHING_RECEIVED)
				{
					sol = FALSE;
					sol = search(parse_tree->node->id_or_num,S);
					if (sol == FALSE)
					{
						printf("Symbol %s is not inserted in the Symbol Table and is undeclared\n",parse_tree->node->id_or_num);
						exit(1);
					}
					status = ID_RECEIVED;/*Line is new in this session*/
					strcpy(globalstring,parse_tree->node->id_or_num);
				}
				else
				{
					if (status == FUN_RECEIVED)
					{
						if (strcmp(parse_tree->node->id_or_num,S->htable->hashname)!=0)
						      strcpy(symglobalnode.lexeme,parse_tree->node->id_or_num);
					}
				
				}
			}
		}
	}
	else
		return;
	for (i=0;i<parse_tree->num_branches;i++)
		dfs_parsetree(parse_tree->branches[i],S);
}
int integer(char *string)  //converts a number as an string to integer value (atoi could also have been used)
{
	//printf("Value sent in is %s\n",string);
	int value = 0;
	int i;
	for (i=0;i<strlen(string);i++)
		value = value+(int)(pow(10,strlen(string)-1-i))*(string[i]-48);
	//printf("Value returned is %d\n",value);
	return value;
}
SymbolTable createnew(SymbolTable S,char *str)   //creates a new HashTable in the linked list and returns a fresh hashtable
{
	int i;
	SymbolTable temp;
	temp = (SymbolTable)malloc(sizeof(struct symboltable));
	temp->htable = (HashTable)malloc(sizeof(struct hashtable));
	temp->htable->sym_node = (symbolnode *)malloc(sizeof(struct node)*ENTRIES_PER_HASHTABLE);
	temp->htable->size=0;
	for (i=0;i<ENTRIES_PER_HASHTABLE;i++)
	{
		temp->htable->sym_node[i].lexeme[0]='\0';
		//temp->htable->sym_node[i].type[0]='\0';
		temp->htable->sym_node[i].value=0;
	}
	temp->next = (SymbolTable*)malloc(sizeof(SymbolTable)*MAX_NO_OF_BRANCHES);
	for (i=0;i<MAX_NO_OF_BRANCHES;i++)
		temp->next[i] = NULL;
	temp->num_of_branches = 0;
	strcpy(temp->htable->hashname,str);

	S->next[S->num_of_branches] = temp;
	S->num_of_branches++;
	temp->previous = S;
	//printf("NEW TABLE CREATED AT %x\n",temp);
	return temp;
}
int hashfunc_symtable(char *str)     //creates the hash value for putting in the particular hash table 
{
	//computes the hash value of the given string
	int i;
	int sum = 0;
	for (i=0;i<strlen(str);i++)
		sum+= (i+1)*((int)str[i]);
	return sum % ENTRIES_PER_HASHTABLE;
}
void insert(symbolnode snode,HashTable h)  //inserts a symboltable entry at its proper scope
{
	int i = 0;
	int hvalue;
	hvalue = hashfunc_symtable(snode.lexeme);
	while (1)
	{
		hvalue = (hvalue+(i)+(i*i))%ENTRIES_PER_HASHTABLE;
		if (h->sym_node[hvalue].lexeme[0] == '\0')   //means the slot is free
		{
			//insert the values
			strcpy(h->sym_node[hvalue].lexeme,snode.lexeme);
			//strcpy(h->sym_node[hvalue].type,snode.type);
			if (snode.dff == NORMAL)
				h->sym_node[hvalue].typ.idtype = snode.typ.idtype;
			else if (snode.dff == FUNCTION)
				h->sym_node[hvalue].typ.fun_type = snode.typ.fun_type;
			h->sym_node[hvalue].dff = snode.dff;
			h->sym_node[hvalue].value = snode.value;
			h->size = h->size+1;

			//printf("INSERT FUNCTION :: Type for %s is %d\n",h->sym_node[hvalue].lexeme,h->sym_node[hvalue].dff);
			return;
		}
		i++;
	}
}
void fillvalue(SymbolTable S,char *str,int value)  //populates the values for corresponding variables in the Symbol Table
{
	HashTable h = S->htable;
	int hvalue;
	int i=0;
	hvalue = hashfunc_symtable(str);
	//printf("String Received:%s Value:%d\n",str,value);
	while (1)
	{
		hvalue = (hvalue+(i)+(i*i))%ENTRIES_PER_HASHTABLE;
		if (strcmp(h->sym_node[hvalue].lexeme,str)==0)
		{
			h->sym_node[hvalue].value = value;
			//printf("Value put:%d for %s\n",h->sym_node[hvalue].value,str);
			return;
		}
		i++;
	}
}
Boolean search(char *lex,SymbolTable S)   //searches whether a particular identifier is present in the current scope or not.
{
	HashTable h = S->htable;
	int hvalue;
	int i = 0;
	hvalue = hashfunc_symtable(lex);
	if (h->sym_node[hvalue].lexeme[0] == '\0')   //means definitely its not there in the current hashtable
		return FALSE;
	else
	{
		while (i<=ENTRIES_PER_HASHTABLE)
		{
			if (strcmp(h->sym_node[hvalue].lexeme,lex) == 0)
				return TRUE;
			hvalue = (hvalue+i+(i*i))%ENTRIES_PER_HASHTABLE;
		}
		return FALSE;
	}
}
	
SymbolTable searchone(char *lex,SymbolTable S)   //returns true or false whether the corresponding symbol is present or not
{
	HashTable h = S->htable;
	int hvalue;
	int i = 0;
	hvalue = hashfunc_symtable(lex);
	if (h->sym_node[hvalue].lexeme[0] == '\0')   //means definitely its not there in the current hashtable
	{
			//fill in the detials
			if (S->previous == NULL)  //this means we have reached till the top of the symbol Table and the lexeme was not found
				return NULL;
			else
				return searchone(lex,S->previous);
	}
	else
	{
		while (i<=ENTRIES_PER_HASHTABLE)
		{
			if (strcmp(h->sym_node[hvalue].lexeme,lex) == 0)  //means the lexeme is found in the current scope
				return S;
			hvalue = (hvalue+i+(i*i))%ENTRIES_PER_HASHTABLE;
		}
		//If you reach this point; this means that the corresponding lexeme is not found in the present scope
			if (S->previous == NULL)
				return NULL;
			else
				return searchone(lex,S->previous);
	}
}
void printSymbolTable(SymbolTable head,int count,FILE *fp)  //prints the symbol Table in the corresponding variable
{
	int i,j;
	if (head->htable->size == 0)
		return;
	fprintf(fp,"**************Sub Symbol Table no %d for function %s**************\n",count,head->htable->hashname);
	count++;
	for (i=0;i<ENTRIES_PER_HASHTABLE;i++)
	{
		if (head->htable->sym_node[i].lexeme[0] != '\0')
		{
		if (head->htable->sym_node[i].dff == NORMAL)
		{
			fprintf(fp,"Lexeme:%s Type:%d Value:%d\n",head->htable->sym_node[i].lexeme,head->htable->sym_node[i].typ.idtype,head->htable->sym_node[i].value);
			//printf("%d is the status\n",head->htable->sym_node[i].dff);
		}
		else if (head->htable->sym_node[i].dff == FUNCTION)
		{
			fprintf(fp,"Lexeme:%s Value:%d  ",head->htable->sym_node[i].lexeme,head->htable->sym_node[i].value);
			fprintf(fp,"Parameter:");
			for (j=0;j<head->htable->sym_node[i].typ.fun_type.num_params;j++)
				fprintf(fp,"%d ,",head->htable->sym_node[i].typ.fun_type.params[j]);
			//fprintf(fp,"\n");
			fprintf(fp,"Result:%d\n",head->htable->sym_node[i].typ.fun_type.result);
		}	
		}
	}
	for (i=0;i<head->num_of_branches;i++)
	{
		printSymbolTable(head->next[i],count,fp);
		fprintf(fp,"###############################\n");
	}
}

