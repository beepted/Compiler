/*
###############################################################
#	Programming Languages & Compiler Construction	      #
#  Stage 1,2,3: Scanner,Parser,SymbolTable,AST,CodeGenerator  #
#	   Member 1: Sarthak Dash, 2006B4A7549P               #
#         Member 2: Rajat Tibrewal, 2006B5A7472P              #
#		Release Date: 22 April, 2010		      #
###############################################################
*/

#include "codegen.h"

FILE *code;
char reg[NUM_REGISTERS][3];
int current_reg, current_label;
void generate_datasection(SymbolTable symboltable);
void generate_codesection(Syntaxtree ast);
void generate_MainFun(Syntaxtree ast);
void generate_Statement(Syntaxtree ast);
void generate_AssignmentStmt(Syntaxtree ast);
void generate_ConditionalStmt(Syntaxtree ast);
void generate_RepetitiveStmt(Syntaxtree ast);
void generate_IN_Stmt(Syntaxtree ast);
void generate_OUT_Stmt(Syntaxtree ast);
void generate_Expr(Syntaxtree ast);
void generate_BoolExp(Syntaxtree ast);
void generate_INOUTMacros();

void generate_x86code(Syntaxtree ast, SymbolTable symboltable, char *filename)
{
	char asm_file[50];
	strcpy(reg[0],"AX");
	strcpy(reg[1],"BX");
	strcpy(reg[2],"CX");
	strcpy(reg[3],"DX");
	current_reg=0;
	current_label=0;
	strcpy(asm_file,filename);
	strcat(asm_file,".asm");
	code = fopen(asm_file,"w");
	fprintf(code,".model small\n.stack 1024\n");
	generate_datasection(symboltable);
	generate_codesection(ast);
	generate_INOUTMacros();
	fprintf(code,"end\n");
}

void generate_datasection(SymbolTable symboltable)
{
	fprintf(code,".data\n");
	fprintf(code,"newline\tdb\t10,13,'$'\n");
	int i = 0;
	for (i=0;i<ENTRIES_PER_HASHTABLE;i++)
	{
		if (symboltable->htable->sym_node[i].dff == FUNCTION)  //means its an entry for the function; not needed in our case
			continue;
		if (symboltable->htable->sym_node[i].lexeme[0] != '\0')  //means there's definitely a value
			fprintf(code,"%s\tdw\t?\n",symboltable->htable->sym_node[i].lexeme);
	}
	fprintf(code,"strvar\tdb\t?,'$'\n");
}
void generate_codesection(Syntaxtree ast)
{
	fprintf(code,".code\n.startup\n");
	generate_MainFun(ast->first_child);
	fprintf(code,".exit\n");
}

void generate_MainFun(Syntaxtree ast)
{
	Syntaxtree child;
	child = ast->first_child;
	while(child!=NULL)
	{
		generate_Statement(child);
		child=child->sibling;
	}
}

void generate_Statement(Syntaxtree ast)
{
	if(strcmp(ast->name,"AssignmentStmt")==0)
		generate_AssignmentStmt(ast);
	else if(strcmp(ast->name,"ConditionalStmt")==0)
		generate_ConditionalStmt(ast);
	else if(strcmp(ast->name,"RepetitiveStmt")==0)
		generate_RepetitiveStmt(ast);
	else if(strcmp(ast->name,"IN_Stmt")==0)
		generate_IN_Stmt(ast);
	else if(strcmp(ast->name,"OUT_Stmt")==0)
		generate_OUT_Stmt(ast);
}

void generate_AssignmentStmt(Syntaxtree ast)
{
	int expr_reg = current_reg;
	fprintf(code,"\n;AssignmentStmt\n");
	generate_Expr(ast->first_child->sibling);
	fprintf(code,"MOV %s,%s\n",ast->first_child->name,reg[expr_reg]);
	current_reg = expr_reg;
	fprintf(code,";EndAssignmentStmt\n");
}
void generate_ConditionalStmt(Syntaxtree ast)
{
	int cond_label = current_label;
	int left_var = current_reg;
	Syntaxtree child;

	fprintf(code,"\n;ConditionalStmt\n");
	fprintf(code,"MOV %s,%s\n",reg[left_var],ast->first_child->first_child->name);
	current_reg++;
	fprintf(code,"MOV %s,%s\n",reg[current_reg],ast->first_child->first_child->sibling->name);
	fprintf(code,"CMP %s,%s\n",reg[left_var],reg[current_reg]);
	generate_BoolExp(ast->first_child);
	current_label++;

	child=ast->first_child->sibling;
	while(child!=NULL)
	{
		current_reg = left_var;
		generate_Statement(child);
		child=child->sibling;
	}

	current_reg = left_var;
	fprintf(code,";EndConditionalStmt\n");
	fprintf(code,"Label%d:\n",cond_label);
}
void generate_RepetitiveStmt(Syntaxtree ast)
{
	int start_label = current_label;
	int left_var = current_reg;
	Syntaxtree child;

	fprintf(code,"\n;RepititiveStmt\n");
	fprintf(code,"Label%d:\n",start_label);
	fprintf(code,"MOV %s,%s\n",reg[left_var],ast->first_child->first_child->name);
	current_reg++;
	fprintf(code,"MOV %s,%s\n",reg[current_reg],ast->first_child->first_child->sibling->name);
	fprintf(code,"CMP %s,%s\n",reg[left_var],reg[current_reg]);
	current_label++;
	generate_BoolExp(ast->first_child);
	current_label++;	

	child=ast->first_child->sibling;
	while(child!=NULL)
	{
		current_reg = left_var;
		generate_Statement(child);
		child=child->sibling;
	}

	current_reg = left_var;
	fprintf(code,"JMP Label%d\n",start_label);
	fprintf(code,";EndRepititiveStmt\n");
	fprintf(code,"Label%d:\n",start_label+1);
}
void generate_OUT_Stmt(Syntaxtree ast)
{
	fprintf(code,"\n;OUTStmt\n");
	fprintf(code,"MOV AX,%s\n",ast->first_child->name);
	fprintf(code,"CALL OUTPUT\n");
	fprintf(code,";EndOUTStmt\n");
}
void generate_IN_Stmt(Syntaxtree ast)
{
	fprintf(code,"\n;INStmt\n");
	fprintf(code,"CALL INPUT\n");
	fprintf(code,"MOV %s,AX\n",ast->first_child->name);
	fprintf(code,";EndINStmt\n");
}

void generate_INOUTMacros()
{
	//INPUT takes integer input and stores it in AX
	fprintf(code,"\nINPUT PROC NEAR\nMOV BX,10\nMOV SI,0\n");
	fprintf(code,"Lbl0:MOV AX,0\nMOV AH,01H\nINT 21H\nCMP AL,0DH\nJE Lbl1\n");
	fprintf(code,"MOV CL,AL\nMOV AX,SI\nMOV CH,0\nMUL BX\nSUB CX,30H\n");
	fprintf(code,"ADD AX,CX\nMOV SI,AX\nJMP Lbl0\n");
	fprintf(code,"Lbl1:MOV AX,SI\nPUSH AX\nPUSH DX\nMOV DX,offset newline\n");
	fprintf(code,"MOV AH,09H\nINT 21H\nPOP DX\nPOP AX\nRET\nINPUT ENDP\n");

	//OUTPUT takes integer from AX and outputs into strvar and prints it
	fprintf(code,"\nOUTPUT PROC NEAR\nCMP AX,0\nJNE Lbl11\nMOV DI,1\n");
	fprintf(code,"JMP Lbl9\nLbl11:PUSH AX\nMOV DI,0\nMOV SI,0\nLBL5:CMP AX,0\n");
	fprintf(code,"JNG LBL4\nADD DI,1\nMOV BX,10\nPUSH DX\nMOV DX,0\nDIV BX\n");
	fprintf(code,"POP DX\nJMP LBL5\nLBL4:POP AX\nLbl2:\nSUB DI,1\nCMP DI,0\n");
	fprintf(code,"JNG Lbl9\nPUSH AX\nMOV AX,1\nMOV BX,10\nPUSH DI\nLbl13:\n");
	fprintf(code,"CMP DI,0\nJE Lbl12\nMUL BX\nSUB DI,1\nJMP Lbl13\n");
	fprintf(code,"Lbl12:\nPOP DI\nMOV BX,AX\nPOP AX\nMOV DX,0\nDIV BX\n");
	fprintf(code,"MOV SI,DX\nADD AX,30H\nMOV strvar,AL\nMOV DX, offset strvar\n");
	fprintf(code,"MOV AH,09H\nINT 21H\nMOV AX,SI\nJMP Lbl2\nLbl9:\n");
	fprintf(code,"ADD AX,30H\nMOV strvar,AL\nMOV DX,offset strvar\nMOV AH,09H\n");
	fprintf(code,"INT 21H\nPUSH AX\nPUSH DX\nMOV DX,offset newline\nMOV AH,09H\n");
	fprintf(code,"INT 21H\nPOP DX\nPOP AX\nRET\nOUTPUT ENDP\n");
}
void generate_Expr(Syntaxtree ast)
{
	int result_reg = current_reg;
	int has_pushed = 0;
	fprintf(code,";Expression\n");
	if((ast->idlink!=NULL)||(ast->name[0]>='0' && ast->name[0]<='9'))
		fprintf(code,"MOV %s,%s\n",reg[result_reg],ast->name);
	else
	{
		generate_Expr(ast->first_child);
		fprintf(code,"PUSH %s\n",reg[result_reg]);
		current_reg = result_reg;
		generate_Expr(ast->first_child->sibling);
		fprintf(code,"MOV %s,%s\n",reg[result_reg+1],reg[result_reg]);
		fprintf(code,"POP %s\n",reg[result_reg]);

		if(strcmp(ast->name,"PLUS_Expr")==0)
			fprintf(code,"ADD %s,%s\n",reg[result_reg],reg[result_reg+1]);
		else if(strcmp(ast->name,"MINUS_Expr")==0)
			fprintf(code,"SUB %s,%s\n",reg[result_reg],reg[result_reg+1]);
		else if(strcmp(ast->name,"TIMES_Expr")==0)
		{
			//backup values of AX & DX
			if(current_reg!=0) fprintf(code,"PUSH AX\n");
			fprintf(code,"PUSH DX\n");
			//move multiplicand to AX
			if(current_reg!=0) fprintf(code,"MOV AX,%s\n",reg[result_reg]);
			//do the multiplication
			fprintf(code,"MUL %s\n",reg[result_reg+1]);
			//send result in AX to the result_reg
			if(current_reg!=0) fprintf(code,"MOV %s,AX\n",reg[result_reg]);
			//revert AX & DX to old values
			fprintf(code,"POP DX\n");
			if(current_reg!=0) fprintf(code,"POP AX\n");
		}
		else if(strcmp(ast->name,"DIV_Expr")==0)
		{
			//backup values of AL & AH
			if(current_reg!=0) fprintf(code,"PUSH AX\n");
			fprintf(code,"PUSH DX\nMOV DX,0\n");
			//move dividend to AX
			if(current_reg!=0) fprintf(code,"MOV AX,%s\n",reg[result_reg]);
			//do the division
			fprintf(code,"DIV %s\n",reg[result_reg+1]);
			//send quotient in AX to the result_reg
			if(current_reg!=0) fprintf(code,"MOV %s,AX\n",reg[result_reg]);
			//revert AX & DX to old values
			fprintf(code,"POP DX\n");
			if(current_reg!=0) fprintf(code,"POP AX\n");
		}
	}
	current_reg = result_reg;
	fprintf(code,";EndExpression\n");
}
void generate_BoolExp(Syntaxtree ast)
{
	fprintf(code,";BoolExpression\n");
	if(strcmp(ast->name,"LT_Exp")==0)
		fprintf(code,"JNL Label%d\n",current_label);
	else if(strcmp(ast->name,"GT_Exp")==0)
		fprintf(code,"JNG Label%d\n",current_label);
	else if(strcmp(ast->name,"LE_Exp")==0)
		fprintf(code,"JNLE Label%d\n",current_label);
	else if(strcmp(ast->name,"GE_Exp")==0)
		fprintf(code,"JNGE Label%d\n",current_label);
	else if(strcmp(ast->name,"EqualityExp")==0)
		fprintf(code,"JNE Label%d\n",current_label);
	else if(strcmp(ast->name,"NotEqualityExp")==0)
		fprintf(code,"JE Label%d\n",current_label);
	fprintf(code,";EndBoolExpression\n");
}
