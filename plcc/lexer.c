/*

###############################################################
#	Programming Languages & Compiler Construction	      #
#		Stage 1: Scanner & Parser		      #
#	   Member 1: Sarthak Dash, 2006B4A7549P               #
#         Member 2: Rajat Tibrewal, 2006B5A7472P              #
#		Release Date: 28 Feb, 2010		      #
###############################################################

*/

#include "lexer.h"

FILE *fp;
FILE *ftok;
Hashtable h;
Buffer buffer[2];
int use;   //used to choose between the buffers as in which one to load at what time
int Terminate = 1;
int lexeme_begin;
int forward;
int line_no = 1;
char tokenfile[50];
int fill_buffer(FILE *fp, Buffer buffer);
void scanner(FILE* fp, char *filename);
int hashfunc_lex(char *str);
void insert_tok(char *token_name,char *token_lexeme);

char* initializeScanner(char *filename) // initializes the scanner
{
	//initialises and stores the given tokens in the hash table(which is referred to as Symbol table in the question)
	int i=0;
	char tok_name[13][10]={"begin","end","main","function","result","none","int","if","call","while","get","print","return"};
	char tok_lex[13][15]={"TK_KEY_BEGIN","TK_KEY_END","TK_KEY_MAIN","TK_KEY_FUN","TK_KEY_RESULT","TK_KEY_NONE","TK_KEY_INT",\
	"TK_KEY_IF","TK_KEY_CALL","TK_KEY_WHILE","TK_KEY_GET","TK_KEY_PRINT","TK_KEY_RETURN"};
	fp=fopen(filename,"r");
	if(fp==NULL)
	{
		fprintf(stderr,"ERROR:: File does not exist.\n");
		exit(1);
	}
		
	buffer[0]=(Buffer)malloc(sizeof(struct buffer));
	buffer[1]=(Buffer)malloc(sizeof(struct buffer));
	h = (Hashtable)malloc(sizeof(struct hash));
	h->bucket = (Token *)malloc(sizeof(struct token)*HASH_SIZE);
	for (i=0;i<13;i++)
		insert_tok(tok_lex[i],tok_name[i]);
	strcpy(tokenfile,filename);
	strcat(tokenfile,".tok");
	scanner(fp,tokenfile);
	fclose(fp);
	return tokenfile;
}

void insert_tok(char *token_name,char *token_lexeme)
{
	//inserts the tokens into the hash table by open-addressing method
	int a;
	int i = 0;
	a = hashfunc_lex(token_lexeme);
	while (1)
	{
		a = (a+(i)+(i*i)) % HASH_SIZE;
		if (h->bucket[a].token_name[0] == '\0' && h->bucket[a].token_lexeme[0] == '\0')
		{
			strcpy(h->bucket[a].token_name,token_name);
			strcpy(h->bucket[a].token_lexeme,token_lexeme);
			return;
		}
		i++;
	}
}

int hashfunc_lex(char *str)
{
	//computes the hash value of the given string
	int i;
	int sum = 0;
	for (i=0;i<strlen(str);i++)
		sum+= (i+1)*((int)str[i]);
	return sum % HASH_SIZE;
}

void scanner(FILE *fp, char *filename)
{
	Token next_token;
	int readbt;   //tells me the number of bytes read into the buffer
	ftok = fopen(filename,"w");
	use = 0;   //initialising the use so that the first of the buffer will only get filled
	readbt = fill_buffer(fp,buffer[use]);
	buffer[use]->buf[BUFFER_SIZE-1] = EOF;  // so as to check during lexical analysis whether we have reached the end of the buffer or not
	if (readbt < BUFFER_SIZE)
	{
		buffer[use]->buf[readbt] = EOF;
		Terminate = 1;  //setting the termination condition
		// This means that the complete 1024 bytes can't be read i.e this is the last segment of the input program
	}
	lexeme_begin = 0;
	forward = 0; 
	while (1)
	{
		next_token=nextToken(); 
		if (strcmp(next_token.token_name,"END")==0 && strcmp(next_token.token_lexeme,"END")==0)
		{
			//the lexical phase is over.
			fprintf(ftok,"%s\n","EOF");
			fclose(ftok);
			return;
		}
		//puts all the tokens into a particular file
		fprintf(ftok,"%s ",next_token.token_name);
	}

}

Token nextToken() // should scan and return the next token
{
	int readbt,value,count,hvalue,i;  //used to take account of the number of bytes read
	char storebuffer[BUFFER_SIZE];
	Token tok;
	tok.token_name[0]='\0';
	tok.token_lexeme[0]='\0';
	char ch;
	count = 0;
	int state = 0;   //denotes the current state of the Automaton for the Lexical Analysis,initially on 0
	while (1)
	{
		while ((ch=buffer[use]->buf[forward])!=EOF)
		{
			value = (int)ch;  //value stores the integer part of the character read
			switch (state)  // this segment is used to find out id or num or a keyword
			{
				case 0: 
				if (value>=48 && value<=57)
				{
					state = 5 ; // for a number
					storebuffer[count] = ch;
					forward++;
					count++;
				}
				else if ((value>=65 && value<=90)||(value>=97 && value <=122))
				{
					storebuffer[count] = ch;
					count++;
					forward++;
					state = 4;
				}
				break;
				case 3:	
				if (ch != '=')  //Error i.e. already a ! has been seen but there's no = symbol
				{
					printf("ERROR at line %d:: Expected =.\n",line_no);
					exit(0);
				}
				break;
				case 4:	
				if ((value>=65 && value<=90)||(value>=97 && value <=122)||(value>=48 && value <=57))
				{
					storebuffer[count] = ch;
					count++;
					forward++;
					state = 4;
				}
				else
				{
					// its the end of the word
					//compare with the hash table and find out whether its a id or a keyword
					lexeme_begin = forward;
					i = 0;
					storebuffer[count]='\0';
					hvalue = hashfunc_lex(storebuffer);
					while (1)
					{
						hvalue = (hvalue + i +(i*i)) % HASH_SIZE;
						if (i == HASH_SIZE || (h->bucket[hvalue].token_name[0] == '\0' && \
						h->bucket[hvalue].token_lexeme[0] == '\0'))
						{
							// means its not a keyword
							strcpy(tok.token_name,"TK_ID:");
							strcat(tok.token_name,storebuffer);
							strcpy(tok.token_lexeme,storebuffer);
							return tok;
						}
						if (strcmp(storebuffer,h->bucket[hvalue].token_lexeme) == 0)
						{
							//means its a keyword
							return h->bucket[hvalue];
						}
						i++;
					}
					state = 0;
					count = 0;
					storebuffer[0]='\0';
				}
				break;
				case 5:	//extract the numbers out of the input file
				if (value >=48 && value<=57)
				{
					storebuffer[count] = ch;
					forward++;
					count++;
					state = 5;
				}
				else
				{
					lexeme_begin = forward;
					storebuffer[count]='\0';
					strcpy(tok.token_name,"TK_NUM:");
					strcat(tok.token_name,storebuffer);
					strcpy(tok.token_lexeme,storebuffer);
					storebuffer[0]='\0';
					count = 0;
					return tok;
				}
				break;
			}
			if (state !=4 && state !=5)  //checks out the remaining symbols as per the tokens list
			{
				switch (ch)
				{
					case '\t':lexeme_begin++;
						forward++;
						break;
					case '\n':line_no++;
						lexeme_begin++;
						forward++;
						fprintf(ftok,"\n");
						break;
					case ' ':if (state == 1)
						{
							strcpy(tok.token_name,"TK_LT");
							strcpy(tok.token_lexeme,"<");
							lexeme_begin = forward+1;
							forward++;
							state = 0;
							return tok;
						}
						if (state == 2)
						{
							strcpy(tok.token_name,"TK_GT");
							strcpy(tok.token_lexeme,">");
							lexeme_begin = forward+1;
							forward++;
							state = 0;
							return tok;
						}
						if (state == 0)
						{
							lexeme_begin++;
							forward++;
						}
						break;
					case '(':strcpy(tok.token_name,"TK_OPEN");
						strcpy(tok.token_lexeme,"(");
						lexeme_begin++;
						forward++;
						return tok;
					case ')':strcpy(tok.token_name,"TK_CLOSE");
						strcpy(tok.token_lexeme,")");
						lexeme_begin++;
						forward++;
						return tok;
					case ';':strcpy(tok.token_name,"TK_SEMI");
						strcpy(tok.token_lexeme,";");
						lexeme_begin++;
						forward++;
						return tok;
					case '=':if (state == 0)  
					//once when we read = , this segments differentiates between equal,less-than-equal and related symbols.
						{
							strcpy(tok.token_name,"TK_EQUAL");
						strcpy(tok.token_lexeme,"=");
							lexeme_begin++;
						}
						else if (state == 1)
						{
							strcpy(tok.token_name,"TK_LEQ");
						strcpy(tok.token_lexeme,"<=");
							lexeme_begin = forward+1;
							state = 0;
						}
						else if (state == 2)
						{
							strcpy(tok.token_name,"TK_GEQ");
						strcpy(tok.token_lexeme,">=");
							lexeme_begin = forward+1;
							state = 0;
						}
						else if (state == 3)
						{
							strcpy(tok.token_name,"TK_NOTEQUAL");
						strcpy(tok.token_lexeme,"!=");
							lexeme_begin = forward+1;
							state = 0;
						}
						forward++;
						return tok;
					case '+':strcpy(tok.token_name,"TK_PLUS");
						strcpy(tok.token_lexeme,"+");
						lexeme_begin++;
						forward++;
						return tok;
					case '-':strcpy(tok.token_name,"TK_MINUS");
						strcpy(tok.token_lexeme,"-");
						lexeme_begin++;
						forward++;
						return tok;
					case '*':strcpy(tok.token_name,"TK_TIMES");
						strcpy(tok.token_lexeme,"*");
						lexeme_begin++;
						forward++;
						return tok;
					case '<': state = 1;  // for checking less than or less than equal to
						if (buffer[use]->buf[forward+1] != '=')
						{
							strcpy(tok.token_name,"TK_LT");
							strcpy(tok.token_lexeme,">");
							lexeme_begin = forward+1;
							forward++;
							state=0;
							return tok;
						}
						forward++;
						break;
					case '!': state = 3;
						forward++;   //checking out what happens if an = sign doesnot follow the ! symbol i.e. for errors
						break;

					case '>':state = 2;  //for checking greater than or greater than equal to
						if (buffer[use]->buf[forward+1] != '=')
						{
							strcpy(tok.token_name,"TK_GT");
							strcpy(tok.token_lexeme,">");
							lexeme_begin = forward+1;
							forward++;
							state=0;
							return tok;
						}
						forward++;
						break;
					case '.':strcpy(tok.token_name,"TK_DOT");
						strcpy(tok.token_lexeme,".");
						lexeme_begin++;
						forward++;
						return tok;
					case ',':strcpy(tok.token_name,"TK_COMMA");
						strcpy(tok.token_lexeme,",");
						lexeme_begin++;
						forward++;
						return tok;
					case '/':strcpy(tok.token_name,"TK_DIV");
						strcpy(tok.token_lexeme,"/");
						lexeme_begin++;
						forward++;
						return tok;
					default:printf("ERROR at line %d:: Unrecognized symbol %c\n",line_no,ch);
						exit(1);
				}

			}
			// perform the DFA traversal over in this section
		}
		if (Terminate == 1)  // the last block has been read
		{
			strcpy(tok.token_name,"END");
			strcpy(tok.token_lexeme,"END");
			return tok;
		}
		// When the forward reaches the EOF marker i.e. one of the lexeme is across two buffers
		use = 1-use;  //changing the buffer
		readbt = fill_buffer(fp,buffer[use]);
		buffer[use]->buf[BUFFER_SIZE-1] = EOF;
		if (readbt < BUFFER_SIZE)
		{
			buffer[use]->buf[readbt] = EOF;
			Terminate = 1;
		}	
		forward = 0;
	}



}

void printTokenStream(FILE f) // prints out the token stream for a given program 
{

}

int fill_buffer(FILE *fp, Buffer buffer)  //used to read the buffer with the given number of characters
{
	int read_bytes;
	read_bytes = fread(buffer->buf,sizeof(char),BUFFER_SIZE-1,fp);
	return read_bytes;
}
