#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "lexerDef.h"
#include "lexer.h"

/**
 *	@author Nihav Jain
 *	@desc lexer.c - tokenizes the input program source code into meaningful lexemes, flags syntax error for invalid tokens
 */

int lineNum = 1;	//to store line number for error reporting

/**
 *	@method getStream
 *	@desc takes the input stream from the program test file and fills the TWIN BUFFER accordingly
 *	@param {FILE *} fp 		- file pointer to input program source code
 *	@param {buffer} b 		- buffer to store input program source code
 *	@param {buffersize} k 	- size of buffer
 *	@return FILE *			- current position of file pointer for input program source code
 */
FILE *getStream(FILE* fp, buffer B, buffersize k)
{
	int c, count = 0;
	int i, j, start, end;
	if(B->forward - B->buff == k)
	{
		start = k;
		end = 2*k;
	}
	else if(B->forward  - B->buff == 2 * k)
	{
		start = 0;
		end = k;
	}
	for(j=start;j<end;j++)
	{
		B->buff[j] = fgetc(fp);
		if(B->buff[j] == EOF)
			break;
	}
	
	return fp;
}

/**
 *	@method createDFA
 *	@desc generates the graph structure representing the Deterministic finite automaton
 *	@param {FILE *} fp		- file pointer containing the tokens and their respective patterns
 *	@param {TreeNode *} dfa - root of the tree representing the DFA
 *	@return TreeNode * 		- root of the tree representing the DFA
 */
TreeNode* createDFA(FILE* fp, TreeNode *dfa)
{
	// first line contains the total no. of nodes in the DFA
	int totalNodes;
	fscanf(fp, "%d", &totalNodes);
	dfa = (TreeNode *)malloc(totalNodes * sizeof(TreeNode));
	int i, j, numChildren, child;
	char flush;
	fscanf(fp, "%c", &flush);
	// followed by N number of lines each containing the character for that node
	for(i=0;i<totalNodes;i++)
	{
		fscanf(fp, "%c", &(dfa[i].character));
		fscanf(fp, "%c", &flush);
	}
	// followed by the list of index of children of each node
	// on line i if the indexes are 23 12 36, it means the i th node has the 23rd, 12th and 36th node as children
	for(i=0;i<totalNodes;i++)
	{
		fscanf(fp, "%d", &(dfa[i].numChildren));
		dfa[i].isFinal = false;
		dfa[i].translate = false;
		dfa[i].list = (ChildList *)malloc(dfa[i].numChildren * sizeof(ChildList));
		for(j=0;j<dfa[i].numChildren;j++)
		{
			fscanf(fp, "%d", &child);
			child = child-1;
			dfa[i].list[j] = &dfa[child];
		}
	}
	
	flush = fgetc(fp);
	// followed by list of nodes which are final states and the token represented by them
	int index;
	token tok;
	fscanf(fp, "%d %s", &index, tok);
	while(index != 0)
	{
		index--;
		dfa[index].isFinal = true;
		strcpy(dfa[index].tokenName, tok);
		fscanf(fp, "%d %s", &index, tok);
	}
	// followed by list of nodes which need to be translated for generalized regular expressions
	// this was done to handle cases when multiple symbols point to same state
	fscanf(fp, "%d", &index);
	while(index != 0)
	{
		dfa[index-1].translate = true;
		fscanf(fp, "%d", &index);
	}
	
	return dfa;
}

/**
 *	@method createBuffer
 *	@desc creates a buffer of given size i.e. TWIN BUFFER of double the given size
 *	@param {buffersize} size 	- size of buffer
 *	@return buffer 				- to store input file characters
 */
buffer createBuffer(buffersize size)
{ 
	buffer b = (buffer)malloc(sizeof(buff));
	b->buff = (char *)malloc(2*size);
	b->begin = b->buff;
	b->forward = b->buff + 2 * size;
	b->lastFilled = size;
	return b;
}

/**
 *	@method getNextToken
 *	@desc returns the next token parsed from the input file, null if no more tokens are present
 *	@param {FILE *} fp 	- file pointer to input file
 *	@param {TreeNode *} - root of DFA
 *	@param {buffer} 	- buffer containing input characters
 *	@param {buffersize} - size of buffer
 *	@return tokenInfo 	- next token parsed from the input program, null if no more tokens are found
 */
tokenInfo  getNextToken(FILE *fp, TreeNode *dfa, buffer b, buffersize k)
{

	tokenInfo info;
	info.value = NULL;
	TreeNode start = dfa[0];
	TreeNode temp = start;
	char lookahead;
	int i,j;

	boolean zenMode = false;	//represents 'comment' mode

	if(*(b->forward) == EOF)
	{
		return info;
	}
	
	// hardcoded lookup table
	// TODO: generalise by including in dfa.txt
	char *key[] = {"_main", "int", "end", "real", "string", "matrix", "if", "else", "endif", "print" , "read", "function"};
	char *value[] = {"MAIN", "INT", "END", "REAL", "STRING", "MATRIX", "IF", "ELSE", "ENDIF", "PRINT", "READ", "FUNCTION"};
	
	while(1)
	{
		i = b->forward - b->buff;
		
		//  if forward pointer reaches the end of a buffer, take input from getStream function
		if((i == k || i == 2*k) && b->lastFilled != i)
		{
			fp = getStream(fp, b, k);
			if(i == k)
				b->forward = b->buff + k;
			else
				b->forward = b->buff;
			b->lastFilled = i;
		}
		
		lookahead = *(b->forward);
		if(zenMode)
		{
			if(lookahead == '\n')
			{
				zenMode = false;
				lineNum++;
			}
			b->forward = b->forward + 1;
			b->begin = b->begin + 1;
			if(b->begin - b->buff >= 2*k)
				b->begin = b->buff;
			continue;
		}

		for(i=0;i<temp.numChildren;i++)
		{
			// translation is used when multiple symbols point to same state 
			//e.g. for numbers, 0,1,2,3,4,5,6,7,8,9, all point to same state
			//hence they have been translated to 1 particular symbol
			if(temp.translate)
			{
				if(lookahead >= 'a' && lookahead <= 'z'){
					lookahead = 'z';
				}
				else if(lookahead >= 'A' && lookahead <= 'Z'){
					lookahead = 'Z';
				}
				else if(lookahead >= '0' && lookahead <= '9'){
					lookahead = '9';
				}
			}
			if((temp.list[i])->character == lookahead)
				break;
		}
		if(i < temp.numChildren)
		{
			b->forward = b->forward + 1;
			temp = *(temp.list[i]);
		}
		else
		{
			if(temp.isFinal)
			{
				//parse to token
				info = extractInfo(info, b, k);
				strcpy(info.tokenName, temp.tokenName);
				info.lineNum = lineNum;
				break;
			}
			else
			{
				if(temp.character != dfa[0].character)
				{
					int size;
					if(b->begin < b->forward)
						size = (b->forward - b->begin);
					else if(b->begin > b->forward)
						size = (2*k - (b->begin - b->forward));
					if(size == 8)
						size = 9;
					char *unmatched = (char *)malloc(size * sizeof(char));
					j = 0;	
					while(b->begin != b->forward)
					{
						unmatched[j] = *(b->begin);
						b->begin = b->buff + (b->begin - b->buff + 1)%(2*k);
						j++;
					}
					printf("------------------\n");
					printf ("ERROR_3 - unknown pattern %s on line %d\n", unmatched, lineNum);
					printf("------------------\n");
					temp = dfa[0];
					continue;
				}
				else if(lookahead == ' ' || lookahead == '\t' || lookahead == '\n' || lookahead == '\r')
				{
					if(lookahead == '\n')
						lineNum++;
					b->forward = b->forward + 1;
					b->begin = b->begin + 1;
					if(b->begin - b->buff >= 2*k)
						b->begin = b->buff;
				}
				else if(lookahead == '#')
				{
					
					zenMode = true;
					b->forward = b->forward + 1;
					b->begin = b->begin + 1;
					if(b->begin - b->buff >= 2*k)
						b->begin = b->buff;
				}
				else
				{
					//printf("%d", *(b->forward));
					if(*(b->forward) == EOF)
						break;
					printf("------------------\n");
					printf ("ERROR_2 - unknown symbol %d at line %d - %ld\n", *(b->forward), lineNum, (b->forward - b->buff));
					printf("------------------\n");
					
					b->forward = b->forward + 1;
					b->begin = b->begin + 1;					
				}
			}
		}
	}

	// lookup table
	if(info.value != NULL)
	{
		if(strcmp(info.tokenName, "ID") == 0)
		{
			for(i=1;i<12;i++)
			{
				if(strcmp(info.value, key[i]) == 0)
				{
					strcpy(info.tokenName, value[i]);
					break;
				}
			}
		}
		else if(strcmp(info.tokenName, "FUNID") == 0)
		{
			if(strcmp(info.value, key[0]) == 0)
				strcpy(info.tokenName, value[0]);
		}
	}

	//hardcoded handling of a12 type lexical errors
	if(strcmp(info.tokenName, "ID") == 0)
	{
		char h = *(b->forward);
		if((h >= 'a' && h <= 'z') || (h >= 'A' && h <= 'Z') || (h >= '0' && h <= '9'))
		{
			printf ("ERROR_3 - unknown pattern %s%c on line %d\n", info.value, h, lineNum);
			return getNextToken(fp, dfa, b, k);	
		}
	}
	if(strcmp(info.tokenName, "ID") == 0 && strlen(info.value) > 20){
		printf("------------------\n");
		printf("ERROR_1 -  Identifier at line %d is longer than the prescribed length of 20 characters\n", lineNum);
		printf("------------------\n");
		return getNextToken(fp, dfa, b, k);
	}
	else if(strcmp(info.tokenName, "STR") == 0 && strlen(info.value) > 22){
		printf("------------------\n");
		printf("ERROR_1 -  Identifier at line %d is longer than the prescribed length of 20 characters\n", lineNum);
		printf("------------------\n");
		return getNextToken(fp, dfa, b, k);
	}
	
	return info;
}

/**
 *	@method extractInfo
 *	@desc extracts the token value between begin and forward pointers of the buffer
 *	@param {tokenInfo} info - to store the information of extracted token
 *	@param {buffer} b 		- buffer containing the character stream
 *	@param {buffersize} 	- size of buffer
 *	@return tokenInfo 		- contains info of extracted token
 */
tokenInfo extractInfo(tokenInfo info, buffer b, buffersize k)
{ 
	int size;
	if(b->begin < b->forward){
		size = (b->forward - b->begin);
	}
	else if(b->begin > b->forward){
		size = (2*k - (b->begin - b->forward));
	}
	if(size == 8)
		size = 9;
	info.value = (char *)malloc(size * sizeof(char));
	int j = 0;	
	while(b->begin != b->forward)
	{
		info.value[j] = *(b->begin);
		b->begin = b->buff + (b->begin - b->buff + 1)%(2*k);
		j++;
	}
	return info;
}
