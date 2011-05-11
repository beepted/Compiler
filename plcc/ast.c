/*
###############################################################
#	Programming Languages & Compiler Construction	      #
#	Stage 1 and 2: Scanner,Parser,SymbolTable,AST	      #
#	   Member 1: Sarthak Dash, 2006B4A7549P               #
#         Member 2: Rajat Tibrewal, 2006B5A7472P              #
#		Release Date: 25 March, 2010		      #
###############################################################
*/

#include "ast.h"

FILE *ast_file;
int return_expected, return_encountered, error;
char function_name[50];
SymbolTable symtab,ST;
symbolnode* symnode;

/*function declarations*/
Syntaxtree createChild(Syntaxtree parent);
Syntaxtree createSibling(Syntaxtree child);
symbolnode* findlink2symboltable(SymbolTable symtab,char *str);
void findsymboltable(char *str,SymbolTable symtab);
void printAST(Syntaxtree S, int level);
void insertsyntaxtree(Syntaxtree S, char *str,symbolnode* symnode);
void Programs(ParseTree p_tree, Syntaxtree S);
void MainFun(ParseTree p_tree, Syntaxtree S);
void Funct(ParseTree p_tree, Syntaxtree S);
void Declaration(ParseTree p_tree, Syntaxtree S);
void Statement(ParseTree p_tree, Syntaxtree S);
void ResultType(ParseTree p_tree, Syntaxtree S);
void FunDeclTypesRest(ParseTree p_tree, Syntaxtree S);
void Parameters(ParseTree p_tree, Syntaxtree S);
void IntVar(ParseTree p_tree, Syntaxtree S);
void FunctType(ParseTree p_tree, Syntaxtree S);
void FormalParamList(ParseTree p_tree, Syntaxtree S);
void AssignmentStmt(ParseTree p_tree, Syntaxtree S);
void ConditionalStmt(ParseTree p_tree, Syntaxtree S);
void RepetitiveStmt(ParseTree p_tree, Syntaxtree S);
void ReturnStmt(ParseTree p_tree, Syntaxtree S);
void IO_Stmt(ParseTree p_tree, Syntaxtree S);
void IN_Stmt(ParseTree p_tree, Syntaxtree S);
void OUT_Stmt(ParseTree p_tree, Syntaxtree S);
void FunctionCallStmt(ParseTree p_tree, Syntaxtree S);
void FunctionCall(ParseTree p_tree, Syntaxtree S);
void ActualParams(ParseTree p_tree, Syntaxtree S);
void ParamsList(ParseTree p_tree, Syntaxtree S);
void Expr(ParseTree p_tree, Syntaxtree S);
void Term(ParseTree p_tree, Syntaxtree S);
Syntaxtree Tprime(ParseTree p_tree, Syntaxtree S);
Syntaxtree Eprime(ParseTree p_tree, Syntaxtree S);
void Factor(ParseTree p_tree, Syntaxtree S);
void PLUS_Expr(ParseTree term, ParseTree eprime, Syntaxtree S);
void MINUS_Expr(ParseTree term, ParseTree eprime, Syntaxtree S);
void TIMES_Expr(ParseTree factor, ParseTree tprime, Syntaxtree S);
void DIV_Expr(ParseTree factor, ParseTree tprime, Syntaxtree S);
void BoolExp(ParseTree p_tree, Syntaxtree S);
/*End of function declarations*/

Syntaxtree initializeAST(SymbolTable sym, ParseTree p_tree, char *filename) 
{
	/*start AST formation from root*/
	char ast_filename[50];
	Syntaxtree S = (Syntaxtree)malloc(sizeof(struct AST));
	symtab = sym;
	error=0;
	Programs(p_tree,S);
	strcpy(ast_filename,filename);
	strcat(ast_filename,".ast");
	ast_file = fopen(ast_filename,"w");
	printAST(S,0);
	fclose(ast_file);
	if(error==1)
	{
		fprintf(stderr,"Abstract Syntax Tree has Type errors. Exiting...\n");
		exit(0);
	}
	return S;
}

void printAST(Syntaxtree S, int level)
{
	int i,j;
	Syntaxtree child, sibling;
	if(S!=NULL)
	{
		if(level==0)
			fprintf(ast_file,"%s\n",S->name);
		else
		{
			for(i=0;i<level;i++)
				fprintf(ast_file,"  ");
			if (S->idlink == NULL)
				fprintf(ast_file,"|_%s\n",S->name);
			else if (S->idlink != NULL)
			{
				if(S->idlink->dff==NORMAL)
				{
					fprintf(ast_file,"|_%s Name:%s Value:%d Type:%d\n", \
					S->name,S->idlink->lexeme,S->idlink->value,S->idlink->typ.idtype);
				}
				else
				{
					fprintf(ast_file,"|_%s Name:%s Value:%d Return Type:%d", \
					S->name,S->idlink->lexeme,S->idlink->value,S->idlink->typ.fun_type.result);
					fprintf(ast_file," Params:");
					for(j=0;j<S->idlink->typ.fun_type.num_params;j++)
						fprintf(ast_file,"%d,",S->idlink->typ.fun_type.params[j]);
					fprintf(ast_file,"\n");
				}
			}
		}
		S = S->first_child;
		while(S!=NULL)
		{
			printAST(S,level+1);
			S = S->sibling;
		}
	}
}

Syntaxtree createChild(Syntaxtree parent) //creates a child of a node in the AST
{
	Syntaxtree child = (Syntaxtree)malloc(sizeof(struct AST));
	Syntaxtree elder;
	child->parent = parent;
	child->sibling = NULL;
	child->first_child = NULL;
	child->idlink = NULL;
	if(parent->first_child == NULL)
		parent->first_child = child;
	else
	{
		elder=parent->first_child;
		while(elder->sibling!=NULL) elder=elder->sibling;
		elder->sibling = child;
	}
	return child;
}

Syntaxtree createSibling(Syntaxtree child) //creates a sibling of the node in the AST
{
	Syntaxtree sibling = (Syntaxtree)malloc(sizeof(struct AST));
	sibling->parent = child->parent;
	sibling->first_child = NULL;
	sibling->idlink = NULL;
	while(child->sibling!=NULL) child=child->sibling;
	child->sibling = sibling;
	return sibling;
}
	
void insertsyntaxtree(Syntaxtree S, char *str, symbolnode* symnode) //inserts data in the node of the syntaxtree
{
	strcpy(S->name,str);
	S->idlink = symnode;
	if(symnode!=NULL)
	{
		if(symnode->dff==NORMAL)
			S->type=symnode->typ.idtype;
		else if(symnode->dff==FUNCTION)
			S->type=symnode->typ.fun_type.result;
	}
}
void findsymboltable(char *function_name,SymbolTable symtab) //finds the symboltable corresponding to the function_name
{
	
	int i;
	if (strcmp(function_name,symtab->htable->hashname) == 0)
	{
		//id is present at this position of the SymbolTable
		ST = symtab;
		return;
	}
	for (i=0;i<symtab->num_of_branches;i++)
		findsymboltable(function_name,symtab->next[i]);
}
	
symbolnode* findlink2symboltable (SymbolTable symtab,char *id)  //returns the link to the correponding id which is present in the symboltable
{
	int hvalue,i=0;
	SymbolTable temp;
	temp = symtab;
	hvalue = hashfunc_symtable(id);
	while (i<60)  //here 60 refers to entries for hashtable
	{
		hvalue = (hvalue+i+(i*i))%60;
		if (strcmp(temp->htable->sym_node[hvalue].lexeme,id) ==0)
			return &(symtab->htable->sym_node[hvalue]);
		i++;
	}
	return NULL;
}

void Programs(ParseTree p_tree, Syntaxtree S)
{
	insertsyntaxtree(S,"Programs",NULL);
	MainFun(p_tree->branches[0]->branches[0], createChild(S));
	p_tree = p_tree->branches[0]->branches[1];
	while(strcmp(p_tree->branches[0]->node->name,"Funct")==0)
	{
		Funct(p_tree->branches[0], createChild(S));
		p_tree = p_tree->branches[1];
	}
}
void MainFun(ParseTree p_tree, Syntaxtree S)
{	
	insertsyntaxtree(S,"MainFun",NULL);
	return_expected=0;
	strcpy(function_name,"main");
	ParseTree orig_tree = p_tree;
	p_tree = p_tree->branches[2]->branches[0];
	while(strcmp(p_tree->branches[0]->node->name,"Declaration")==0)
	{
		Declaration(p_tree->branches[0], createChild(S));
		p_tree = p_tree->branches[1];
	}
	p_tree = orig_tree->branches[2]->branches[1];
	while(strcmp(p_tree->branches[0]->node->name,"Statement")==0)
	{
		Statement(p_tree->branches[0], createChild(S));
		p_tree = p_tree->branches[1];
	}
}
void Funct(ParseTree p_tree, Syntaxtree S)
{
	symbolnode* symnode;
	insertsyntaxtree(S,"Funct",NULL);

	findsymboltable(function_name,symtab);
	strcpy(function_name,p_tree->branches[1]->node->id_or_num);
	ST = searchone(function_name,ST);
	symnode = findlink2symboltable(ST,function_name);
	insertsyntaxtree(createChild(S),p_tree->branches[1]->node->id_or_num,symnode);
	
	//find if return statement is expected in this function
	if(strcmp(p_tree->branches[3]->branches[0]->node->name,"TK_KEY_INT")==0)
		return_expected=1;
	else
		return_expected=0;
	return_encountered=0;

	//incorporate result type, parameters
	ResultType(p_tree->branches[3], createChild(S));
	Parameters(p_tree->branches[5], createChild(S));
	
	//Matching Function Definition against Function Signature
	Syntaxtree sibling;
	int i;
	sibling=S->first_child->sibling;
	if(sibling->type!=symnode->typ.fun_type.result)
	{
		fprintf(stderr,"ERROR::Return type in definition of %s, doesn't match its signature at Line %d\n",function_name,p_tree->node->line_num);
		error=1;
	}
	i=0;
	sibling=sibling->sibling;
	while(sibling!=NULL && i<symnode->typ.fun_type.num_params)
	{
		if(sibling->type!=symnode->typ.fun_type.params[i])
		{
			fprintf(stderr,"ERROR::Parameters in Function %s mismatch function signature in Line %d\n",function_name,p_tree->node->line_num);
			error=1;
		}
		i++;
		sibling=sibling->sibling;
	}
	if(i!=symnode->typ.fun_type.num_params || sibling!=NULL)
	{
		fprintf(stderr,"ERROR::Parameters exceed in Function %s or function signature in Line %d\n",function_name,p_tree->node->line_num);
		error=1;
	}
	
	//incorporate Declarations and Statements
	ParseTree orig_tree = p_tree;
	p_tree = p_tree->branches[8]->branches[0];
	while(strcmp(p_tree->branches[0]->node->name,"Declaration")==0)
	{
		Declaration(p_tree->branches[0], createChild(S));
		p_tree = p_tree->branches[1];
	}
	p_tree = orig_tree->branches[8]->branches[1];
	while(strcmp(p_tree->branches[0]->node->name,"Statement")==0)
	{
		Statement(p_tree->branches[0], createChild(S));
		p_tree = p_tree->branches[1];
	}

	//check if we got return statement when it was expected
	if(return_expected==1 && return_encountered==0)
	{
		fprintf(stderr,"ERROR::Expected 'return' in function %s at Line %d\n",function_name,p_tree->node->line_num);
		exit(0);
	}
}
void Declaration(ParseTree p_tree, Syntaxtree S)
{
	if(strcmp(p_tree->branches[0]->node->name,"TK_KEY_INT") == 0)
		IntVar(p_tree,S);
	else if(strcmp(p_tree->branches[0]->node->name,"TK_KEY_FUN") == 0)
		FunctType(p_tree,S);
}

void IntVar(ParseTree p_tree, Syntaxtree S)
{
	int hvalue,i=0;
	insertsyntaxtree(S,"IntVar",NULL);  //function_name has got the thing.
	char id[50];
	strcpy(id,p_tree->branches[1]->node->id_or_num);
	findsymboltable(function_name,symtab);
	ST = searchone(id,ST);
	symnode = findlink2symboltable(ST,id);
	insertsyntaxtree(createChild(S), id,symnode);
}
void FunctType(ParseTree p_tree, Syntaxtree S)
{
	insertsyntaxtree(S,"FunctType",NULL);
	char ID_FunctName[50];
	strcpy(ID_FunctName, p_tree->branches[1]->node->id_or_num);

	findsymboltable(function_name,symtab);
	ST = searchone(ID_FunctName,ST);
	symnode = findlink2symboltable(ST,ID_FunctName);
	insertsyntaxtree(createChild(S), ID_FunctName,symnode);

	ResultType(p_tree->branches[6], createChild(S));
	insertsyntaxtree(createChild(S), p_tree->branches[3]->branches[0]->node->name,NULL);
	if(strcmp(p_tree->branches[3]->branches[0]->node->name,"TK_KEY_INT")==0)
		FunDeclTypesRest(p_tree->branches[3]->branches[1], S);
}
void ResultType(ParseTree p_tree, Syntaxtree S)
{
	insertsyntaxtree(S,p_tree->branches[0]->node->name,NULL);
}
void FunDeclTypesRest(ParseTree p_tree, Syntaxtree S)
{
	while(p_tree->num_branches>1)
	{
		insertsyntaxtree(createChild(S),"TK_KEY_INT",NULL);
		p_tree = p_tree->branches[2];
	}
}
void Parameters(ParseTree p_tree, Syntaxtree S)
{
	if(strcmp(p_tree->branches[0]->node->name,"TK_KEY_NONE") == 0)
		insertsyntaxtree(S, "TK_KEY_NONE",NULL);
	else
		FormalParamList(p_tree, S);
}
void FormalParamList(ParseTree p_tree, Syntaxtree S)
{
	char ID_formalParam[50];
	strcpy(ID_formalParam, p_tree->branches[0]->branches[1]->node->id_or_num);
	findsymboltable(function_name,symtab);
	ST = searchone(ID_formalParam,ST);
	symnode = findlink2symboltable(ST,ID_formalParam);

	insertsyntaxtree(S, ID_formalParam,symnode);
	p_tree = p_tree->branches[1];
	while(strcmp(p_tree->branches[0]->node->name,"TK_COMMA")==0)
	{
		strcpy(ID_formalParam, p_tree->branches[1]->branches[1]->node->id_or_num);
		findsymboltable(function_name,symtab);
		ST = searchone(ID_formalParam,ST);
		symnode = findlink2symboltable(ST,ID_formalParam);
		insertsyntaxtree(createSibling(S),ID_formalParam,symnode);
		p_tree = p_tree->branches[2];
	}
}
void Statement(ParseTree p_tree, Syntaxtree S)
{
	if(strcmp(p_tree->branches[0]->node->name,"AssignmentStmt")==0)
		AssignmentStmt(p_tree->branches[0],S);
	else if(strcmp(p_tree->branches[0]->node->name,"ConditionalStmt")==0)
		ConditionalStmt(p_tree->branches[0],S);
	else if(strcmp(p_tree->branches[0]->node->name,"RepetitiveStmt")==0)
		RepetitiveStmt(p_tree->branches[0],S);
	else if(strcmp(p_tree->branches[0]->node->name,"ReturnStmt")==0)
		ReturnStmt(p_tree->branches[0],S);
	else if(strcmp(p_tree->branches[0]->node->name,"IO_Stmt")==0)
		IO_Stmt(p_tree->branches[0],S);
	else if(strcmp(p_tree->branches[0]->node->name,"FunctionCallStmt")==0)
		FunctionCallStmt(p_tree->branches[0],S);
}
void AssignmentStmt(ParseTree p_tree, Syntaxtree S)
{
	char ID_left[50];
	insertsyntaxtree(S,"AssignmentStmt",NULL);

	//Incorporate ID Left
	strcpy(ID_left, p_tree->branches[0]->node->id_or_num);
	findsymboltable(function_name,symtab);
	ST = searchone(ID_left,ST);
	symnode = findlink2symboltable(ST,ID_left);
	//Type check
	if(symnode->dff==FUNCTION)
	{
		fprintf(stderr,"ERROR::Function name used as ID at Line %d\n",p_tree->node->line_num);
		error=1;
	}
	insertsyntaxtree(createChild(S), ID_left,symnode);
	
	//Incorporate right-side Expression
	Expr(p_tree->branches[2],createChild(S));

	//Type checking
	if(S->first_child->type==INT && S->first_child->sibling->type==INT)
		S->type=INT;
	else
	{
		S->type=ERROR;
		fprintf(stderr,"ERROR::Type mismatch in Assignment for ID %s at Line %d\n",ID_left,p_tree->node->line_num);
		error=1;
	}
}
void ConditionalStmt(ParseTree p_tree, Syntaxtree S)
{
	insertsyntaxtree(S,"ConditionalStmt",NULL);
	BoolExp(p_tree->branches[2],createChild(S));
	p_tree = p_tree->branches[5];
	while(strcmp(p_tree->branches[0]->node->name,"Statement")==0)
	{
		Statement(p_tree->branches[0], createChild(S));
		p_tree = p_tree->branches[1];
	}
}
void RepetitiveStmt(ParseTree p_tree, Syntaxtree S)
{
	insertsyntaxtree(S,"RepetitiveStmt",NULL);
	BoolExp(p_tree->branches[2],createChild(S));
	p_tree = p_tree->branches[5];
	while(strcmp(p_tree->branches[0]->node->name,"Statement")==0)
	{
		Statement(p_tree->branches[0], createChild(S));
		p_tree = p_tree->branches[1];
	}
}
void ReturnStmt(ParseTree p_tree, Syntaxtree S)
{
	if(return_expected==0)
	{
		fprintf(stderr,"ERROR::Unexpected 'return' in Function %s in Line %d\n",function_name,p_tree->node->line_num);
		exit(0);
	}
	return_encountered=1;
	char ID_return[50];
	strcpy(ID_return, p_tree->branches[1]->branches[0]->node->id_or_num);
	if ((ID_return[0]>=65 && ID_return[0]<=90) || (ID_return[0]>=97 && ID_return[0]<=122))
	{
		//This is an ID
		findsymboltable(function_name,symtab);
		ST = searchone(ID_return,ST);
		symnode = findlink2symboltable(ST,ID_return);
		//Type check
		if(symnode->dff==FUNCTION)
		{
			fprintf(stderr,"ERROR::Function name used as ID at Line %d\n",p_tree->node->line_num);
			error=1;
		}
		insertsyntaxtree(S,ID_return,symnode);
	}
	else
	{
		//This is a Number
		insertsyntaxtree(S, ID_return,NULL);
		S->type=INT;
	}
}
void IO_Stmt(ParseTree p_tree, Syntaxtree S)
{
	if(strcmp(p_tree->branches[0]->node->name,"TK_KEY_GET")==0)
		IN_Stmt(p_tree, S);
	else if(strcmp(p_tree->branches[0]->node->name,"TK_KEY_PRINT")==0)
		OUT_Stmt(p_tree, S);
}
void IN_Stmt(ParseTree p_tree, Syntaxtree S)
{
	char ID_input[50];
	insertsyntaxtree(S,"IN_Stmt",NULL);
	strcpy(ID_input, p_tree->branches[1]->node->id_or_num);
	findsymboltable(function_name,symtab);
	ST = searchone(ID_input,ST);
	symnode = findlink2symboltable(ST,ID_input);

	insertsyntaxtree(createChild(S),ID_input,symnode);
}
void OUT_Stmt(ParseTree p_tree, Syntaxtree S)
{
	char ID_output[50];
	insertsyntaxtree(S,"OUT_Stmt",NULL);
	strcpy(ID_output, p_tree->branches[1]->node->id_or_num);
	findsymboltable(function_name,symtab);
	ST = searchone(ID_output,ST);
	symnode = findlink2symboltable(ST,ID_output);


	insertsyntaxtree(createChild(S),ID_output,symnode);
}
void FunctionCallStmt(ParseTree p_tree, Syntaxtree S)
{
	FunctionCall(p_tree->branches[0],S);
	//Type Checking
	if(S->type==INT)
	{
		fprintf(stderr,"ERROR::Function %s Return type mismatch at Line %d\n",S->name,p_tree->node->line_num);
		error=1;
	}
}
void FunctionCall(ParseTree p_tree, Syntaxtree S)
{
	char ID_FunctName[50];
	symbolnode* localnode;
	insertsyntaxtree(S,"FunctionCall",NULL);
	strcpy(ID_FunctName, p_tree->branches[1]->node->id_or_num);
	if(strcmp(function_name,ID_FunctName)==0)
	{
		fprintf(stderr,"ERROR::Self referencing in function %s at Line %d\n",function_name,p_tree->node->line_num);
		exit(0);
	}
	findsymboltable(function_name,symtab);
	ST = searchone(ID_FunctName,ST);
	localnode = findlink2symboltable(ST,ID_FunctName);
	insertsyntaxtree(createChild(S),ID_FunctName,localnode);
	ActualParams(p_tree->branches[3],createChild(S));
	//Matching Function Call against Function Signature
	Syntaxtree sibling;
	int i;
	sibling=S->first_child->sibling;
	i=0;
	while(sibling!=NULL && i<localnode->typ.fun_type.num_params)
	{
		if(sibling->type!=localnode->typ.fun_type.params[i])
		{
			fprintf(stderr,"ERROR::Function Call doesn't match function signature in Line %d\n",p_tree->node->line_num);
			error=1;
		}
		i++;
		sibling=sibling->sibling;
	}
	if(i!=localnode->typ.fun_type.num_params || sibling !=NULL)
	{
		fprintf(stderr,"ERROR::Function Call doesn't match function signature in Line %d\n",p_tree->node->line_num);
		error=1;
	}
}
void ActualParams(ParseTree p_tree, Syntaxtree S) 
{
	char ID_ParamName[50];
	strcpy(ID_ParamName, p_tree->branches[0]->node->name);
	if(strcmp(ID_ParamName,"TK_KEY_NONE")==0)
		insertsyntaxtree(S,"TK_KEY_NONE",NULL);
	else
	{
		strcpy(ID_ParamName, p_tree->branches[0]->node->id_or_num);
		if ((ID_ParamName[0]>=65 && ID_ParamName[0]<=90) || (ID_ParamName[0]>=97 && ID_ParamName[0]<=122))
		{
			//This is an ID
			findsymboltable(function_name,symtab);
			ST = searchone(ID_ParamName,ST);
			symnode = findlink2symboltable(ST,ID_ParamName);
			//Type check
			if(symnode->dff==FUNCTION)
			{
				fprintf(stderr,"ERROR::Function name used as ID at Line %d\n",p_tree->node->line_num);
				error=1;
			}
			insertsyntaxtree(S, ID_ParamName,symnode);
		}
		else
		{
			//This is a Number
			insertsyntaxtree(S, ID_ParamName,NULL);
			S->type=INT;
		}
		ParamsList(p_tree->branches[1], S);
	}
}
void ParamsList(ParseTree p_tree, Syntaxtree S)
{
	char ID_ParamName[50];
	while(strcmp(p_tree->branches[0]->node->name,"TK_COMMA")==0)
	{
		strcpy(ID_ParamName, p_tree->branches[1]->branches[0]->node->id_or_num);
		if ((ID_ParamName[0]>=65 && ID_ParamName[0]<=90) || (ID_ParamName[0]>=97 && ID_ParamName[0]<=122))
		{
			//This is an ID
			findsymboltable(function_name,symtab);
			ST = searchone(ID_ParamName,ST);
			symnode = findlink2symboltable(ST,ID_ParamName);
			//Type check
			if(symnode->dff==FUNCTION)
			{
				fprintf(stderr,"ERROR::Function name used as ID at Line %d\n",p_tree->node->line_num);
				error=1;
			}
			insertsyntaxtree(createSibling(S), ID_ParamName,symnode);
		}
		else
		{
			//This is a Number
			insertsyntaxtree(createSibling(S), ID_ParamName,NULL);
			S->type=INT;
		}
		p_tree = p_tree->branches[1]->branches[1];
	}
}
void Expr(ParseTree p_tree, Syntaxtree S)
{
	Syntaxtree expr_attachment,left_tree,right_tree;
	if(p_tree->branches[1]->branches[0]->node->name[0]=='T')
	{
		expr_attachment = Eprime(p_tree->branches[1]->branches[2],S);
		if(strcmp(p_tree->branches[1]->branches[0]->node->name,"TK_PLUS")==0)
			insertsyntaxtree(expr_attachment,"PLUS_Expr",NULL);
		else if(strcmp(p_tree->branches[1]->branches[0]->node->name,"TK_MINUS")==0)
			insertsyntaxtree(expr_attachment,"MINUS_Expr",NULL);
		left_tree = createChild(expr_attachment);
		right_tree = createChild(expr_attachment);
		Term(p_tree->branches[0],left_tree);
		Term(p_tree->branches[1]->branches[1],right_tree);
	}
	else
	{
		Term(p_tree->branches[0],S);
	}
}
Syntaxtree Eprime(ParseTree p_tree, Syntaxtree S)
{
	Syntaxtree eprime_attach,left_tree;
	if(p_tree->branches[0]->node->name[0]=='T')
	{
		eprime_attach = Eprime(p_tree->branches[2],S);
		if(strcmp(p_tree->branches[0]->node->name,"TK_PLUS")==0)
			insertsyntaxtree(eprime_attach,"PLUS_Expr",NULL);
		else if(strcmp(p_tree->branches[0]->node->name,"TK_MINUS")==0)
			insertsyntaxtree(eprime_attach,"MINUS_Expr",NULL);
		left_tree = createChild(eprime_attach);
		Term(p_tree->branches[1],createChild(eprime_attach));
		return left_tree;
	}
	else
	{
		return S;
	}
}	
void Term(ParseTree p_tree, Syntaxtree S)
{
	Syntaxtree term_attachment,left_tree,right_tree;
	if(p_tree->branches[1]->branches[0]->node->name[0]=='T')
	{
		term_attachment = Tprime(p_tree->branches[1]->branches[2],S);
		if(strcmp(p_tree->branches[1]->branches[0]->node->name,"TK_TIMES")==0)
			insertsyntaxtree(term_attachment,"TIMES_Expr",NULL);
		else if(strcmp(p_tree->branches[1]->branches[0]->node->name,"TK_DIV")==0)
			insertsyntaxtree(term_attachment,"DIV_Expr",NULL);
		left_tree = createChild(term_attachment);
		right_tree = createChild(term_attachment);
		Factor(p_tree->branches[0],left_tree);
		Factor(p_tree->branches[1]->branches[1],right_tree);
	}
	else
	{
		Factor(p_tree->branches[0],S);
	}
}
Syntaxtree Tprime(ParseTree p_tree, Syntaxtree S)
{
	Syntaxtree tprime_attach,left_tree;
	if(p_tree->branches[0]->node->name[0]=='T')
	{
		tprime_attach = Tprime(p_tree->branches[2],S);
		if(strcmp(p_tree->branches[0]->node->name,"TK_TIMES")==0)
			insertsyntaxtree(tprime_attach,"TIMES_Expr",NULL);
		else if(strcmp(p_tree->branches[0]->node->name,"TK_DIV")==0)
			insertsyntaxtree(tprime_attach,"DIV_Expr",NULL);
		left_tree = createChild(tprime_attach);
		Factor(p_tree->branches[1],createChild(tprime_attach));
		return left_tree;
	}
	else
	{
		return S;
	}
}	
void Factor(ParseTree p_tree, Syntaxtree S)
{
	char ID_Name[50];
	symbolnode* symnode;
	if(strcmp(p_tree->branches[0]->node->name,"FunctionCall")==0)
	{
		FunctionCall(p_tree->branches[0], S);
		//Type Check function call
		if(S->type!=INT)
		{
			fprintf(stderr,"ERROR::Return Type of Function Call in Expression expected INT at Line %d\n",p_tree->node->line_num);
			S->type=ERROR;
			error=1;
		}
	}
	else if(strcmp(p_tree->branches[0]->node->name,"TK_OPEN")==0)
		Expr(p_tree->branches[1], S);
	else
	{
		strcpy(ID_Name, p_tree->branches[0]->node->id_or_num);
		if ((ID_Name[0]>=65 && ID_Name[0]<=90) || (ID_Name[0]>=97 && ID_Name[0]<=122))
		{
			//This is an ID
			findsymboltable(function_name,symtab);
			ST = searchone(ID_Name,ST);
			symnode = findlink2symboltable(ST,ID_Name);
			//Type check
			if(symnode->dff==FUNCTION)
			{
				fprintf(stderr,"ERROR::Function name used as ID at Line %d\n",p_tree->node->line_num);
				error=1;
			}
			insertsyntaxtree(S,ID_Name,symnode);
		}
		else
		{
			//This is a Number
			insertsyntaxtree(S,ID_Name,NULL);
			S->type=INT;
		}
	}
}
void PLUS_Expr(ParseTree term, ParseTree eprime, Syntaxtree S)
{
	insertsyntaxtree(S,"PLUS_Expr",NULL);
	Term(term,createChild(S));
	Eprime(eprime,createChild(S));
	//Type checking
	if(S->first_child->type==INT && S->first_child->sibling->type==INT)
		S->type=INT;
	else
	{
		fprintf(stderr,"ERROR::Type mismatch in Expression at Line %d\n",term->node->line_num);
		S->type=ERROR;
		error=1;
	}
}
void MINUS_Expr(ParseTree term, ParseTree eprime, Syntaxtree S)
{
	insertsyntaxtree(S,"MINUS_Expr",NULL);
	Term(term,createChild(S));
	Eprime(eprime,createChild(S));
	//Type checking
	if(S->first_child->type==INT && S->first_child->sibling->type==INT)
		S->type=INT;
	else
	{
		fprintf(stderr,"ERROR::Type mismatch in Expression at Line %d\n",term->node->line_num);
		S->type=ERROR;
		error=1;
	}
}
void TIMES_Expr(ParseTree factor, ParseTree tprime, Syntaxtree S)
{
	insertsyntaxtree(S,"TIMES_Expr",NULL);
	Factor(factor,createChild(S));
	Tprime(tprime,createChild(S));
	//Type checking
	if(S->first_child->type==INT && S->first_child->sibling->type==INT)
		S->type=INT;
	else
	{
		fprintf(stderr,"ERROR::Type mismatch in Expression at Line %d\n",factor->node->line_num);
		S->type=ERROR;
		error=1;
	}
}
void DIV_Expr(ParseTree factor, ParseTree tprime, Syntaxtree S)
{
	insertsyntaxtree(S,"DIV_Expr",NULL);
	Factor(factor,createChild(S));
	Tprime(tprime,createChild(S));
	//Type checking
	if(S->first_child->type==INT && S->first_child->sibling->type==INT)
		S->type=INT;
	else
	{
		fprintf(stderr,"ERROR::Type mismatch in Expression at Line %d\n",factor->node->line_num);
		S->type=ERROR;
		error=1;
	}
}
void BoolExp(ParseTree p_tree, Syntaxtree S)
{
	char ID[50];
	symbolnode* symnode;
	//Decide Bool Expression type
	if(strcmp(p_tree->branches[2]->branches[0]->node->name,"TK_EQUAL")==0)
		insertsyntaxtree(S,"EqualityExp",NULL);
	else if(strcmp(p_tree->branches[2]->branches[0]->node->name,"TK_NOTEQUAL")==0)
		insertsyntaxtree(S,"NotEqualityExp",NULL);
	else if(strcmp(p_tree->branches[2]->branches[0]->node->name,"TK_LT")==0)
		insertsyntaxtree(S,"LT_Exp",NULL);
	else if(strcmp(p_tree->branches[2]->branches[0]->node->name,"TK_GT")==0)
		insertsyntaxtree(S,"GT_Exp",NULL);
	else if(strcmp(p_tree->branches[2]->branches[0]->node->name,"TK_LEQ")==0)
		insertsyntaxtree(S,"LE_Exp",NULL);
	else if(strcmp(p_tree->branches[2]->branches[0]->node->name,"TK_GEQ")==0)
		insertsyntaxtree(S,"GE_Exp",NULL);

	//Extract left ID	
	strcpy(ID, p_tree->branches[0]->node->id_or_num);
	findsymboltable(function_name,symtab);
	ST = searchone(ID,ST);
	symnode = findlink2symboltable(ST,ID);
	//Type check
	if(symnode->dff==FUNCTION)
	{
		fprintf(stderr,"ERROR::Function name used as ID at Line %d\n",p_tree->node->line_num);
		error=1;
	}
	insertsyntaxtree(createChild(S),ID,symnode);

	//Extract right ID
	strcpy(ID, p_tree->branches[4]->node->id_or_num);
	findsymboltable(function_name,symtab);
	ST = searchone(ID,ST);
	symnode = findlink2symboltable(ST,ID);
	//Type check
	if(symnode->dff==FUNCTION)
	{
		fprintf(stderr,"ERROR::Function name used as ID at Line %d\n",p_tree->node->line_num);
		error=1;
	}
	insertsyntaxtree(createChild(S),ID,symnode);

	//Type checking
	if(S->first_child->type==INT && S->first_child->sibling->type==INT)
		S->type=INT;
	else
	{
		fprintf(stderr,"ERROR::Type mismatch in Boolean Expression at Line %d\n",p_tree->node->line_num);
		S->type=ERROR;
		error=1;
	}
}
