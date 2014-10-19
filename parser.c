#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "parserDef.h"
#include "lexer.h"
#include "parser.h"

/**
 *	@author Nihav Jain
 *	@desc parser.c - function implementations for syntax checking by parsing the extracted tokens
 */

int epsi;

/**
 *	@method createGrammar
 *	@desc populates the grammar, terminals, non-terminal, rules and first  and follow sets of the non-terminals
 *	@param {grammar} G - pointer to the grammar for the language being implemented
 *	@return grammar
 */
grammar createGrammar(grammar G)
{
	FILE *fp = fopen("grammar.txt", "r");
	int numNonTerminals, numTerminals, k;
	fscanf(fp, "%d %d", &numNonTerminals, &numTerminals);
	int i, total = numNonTerminals + numTerminals;
	G->allTokens = (Terms)malloc(total * sizeof(Element));	
	G->numNonTerminals = numNonTerminals;
	G->numTerminals = numTerminals;
	G->hashTable = (HashTable)malloc(total * sizeof(KeyValue));
	for(i=0;i<total;i++)
		G->hashTable[i].value = -1;
	char inp[30];
	////printf("%d %d", numNonTerminals, numTerminals);
	for(i=0;i<total;i++)
	{
		fscanf(fp, "%s", inp);
		hashInsert(&G->hashTable, inp, i, total);
		strcpy(G->allTokens[i].name, inp);
		G->allTokens[i].ruleCount = 0;
		G->allTokens[i].firstSetDone = false;
		G->allTokens[i].terminal = false;
		G->allTokens[i].isEpsilonProduction = false;
		if(!(inp[0] >= 'a' && inp[0] <= 'z'))
		{
			G->allTokens[i].terminal = true;
			G->allTokens[i].ruleCount = 1;
		}
		G->allTokens[i].followSet = (Set)malloc(numTerminals * sizeof(boolean));
		for(k=0;k<numTerminals;k++)
			G->allTokens[i].followSet[k] = false;
	}
	char epsilon[8] = "EPSILON";
	epsi = hashSearch(&G->hashTable, epsilon, total);
	
	//uncomment to PRINT THE HASH VALUES of all terminals and non-terminals

	/*
	for(i=0;i<total;i++)
		printf("%d. %s\t%d\n", i, G->hashTable[i].key, G->hashTable[i].value);	
	*/
	
	int totalRules;
	fscanf(fp, "%d", &totalRules);
	G->rules = (Rules)malloc(totalRules * sizeof(Rule));
	int numTerms, j, index;
	char inpTerm[30];
	int count;
	for(i=0;i<totalRules;i++)
	{
		fscanf(fp, "%d", &numTerms);
		fscanf(fp, "%s", inpTerm);
		G->rules[i].numTerms = numTerms;
		index = hashSearch(&G->hashTable, inpTerm, total);
		if(index == -1)
		{
			break;
		}
		else
			G->rules[i].lhs = index;
			
		count = G->allTokens[index].ruleCount;
		if(count == 0)
			G->allTokens[index].firstSet = (FirstSet)malloc(sizeof(FirstSetInfo));
		else
			G->allTokens[index].firstSet = (FirstSet)realloc(G->allTokens[index].firstSet, (count+1) * sizeof(FirstSetInfo));
		G->allTokens[index].firstSet[count].ruleNum = i;
		G->allTokens[index].firstSet[count].firstSet = (Set)malloc(G->numTerminals * sizeof(boolean));
		for(k=0;k<G->numTerminals;k++)
			G->allTokens[index].firstSet[count].firstSet[k] = false;
		G->allTokens[index].ruleCount++;
		
		G->rules[i].ruleNum = i;	
		G->rules[i].productionRule = (int *)malloc(numTerms * sizeof(int));	
		for(j=0;j<numTerms;j++)
		{
			fscanf(fp, "%s", inpTerm);
			index = hashSearch(&G->hashTable, inpTerm, total);
			if(index == -1)
			{
				break;
			}
			else
				G->rules[i].productionRule[j] = index;
		}
		if(j < numTerms)
			break;
	}
	
	//uncomment to PRINT ALL RULES

	/*
	for(i=0;i<totalRules;i++)
	{
		printf("%d %s --> ", i+1, G->allTokens[G->rules[i].lhs].name);
		for(j=0;j<G->rules[i].numTerms;j++)
			printf("%s ", G->allTokens[G->rules[i].productionRule[j]].name);
		printf("\n");	
	}
	
	for(i=0;i<G->numNonTerminals;i++)
	{
		//printf("%s = ", G->allTokens[i].name);
		for(j=0;j<G->allTokens[i].ruleCount;j++)
		{
			printf("%d ", G->allTokens[i].firstSet[j].ruleNum);
		}
		printf("\n");
	}
	*/
	
	generateFirstSets(G);

	//uncomment to print the FIRST set of all non-terminals

	/*
	for(i=0;i<G->numNonTerminals;i++)
	{
		//printf("%s,", G->allTokens[i].name);
		for(j=0;j<G->allTokens[i].ruleCount;j++)
		{
			Set s = G->allTokens[i].firstSet[j].firstSet;
			for(k=0;k<G->numTerminals;k++)
			{
				if(s[k])
					printf("%s, ", G->allTokens[G->numNonTerminals + k].name);
			}
		}
		printf("\n");
	}
	*/

	char buff[30];
	char *buff3;
	int numFollow;
	int oldLHS = -1;

	//populating FOLLOW set
	for(i = 0; i<totalRules; i++)
	{
		if(oldLHS == G->rules[i].lhs)
			continue;
		fscanf(fp, "%d", &numFollow);
		for(k=0;k<numFollow;k++)
		{
			fscanf(fp, "%s", buff);
			j = hashSearch(&G->hashTable, buff, total);
			if(j >= 0)
			{
				G->allTokens[G->rules[i].lhs].followSet[j-numNonTerminals] = true;
			}
		}
		oldLHS = G->rules[i].lhs;
	}

	//uncomment to view FOLLOW set of all non-terminals

	/*
	for(i=0;i<G->numNonTerminals;i++)
	{
		printf("%s,", G->allTokens[i].name);
			for(k=0;k<G->numTerminals;k++)
			{
				if(G->allTokens[i].followSet[k])
					printf("%s,", G->allTokens[G->numNonTerminals + k].name);
			}
		printf("\n");
	}
	*/
	return G;
}

/**
 *	@method createParseTable
 *	@desc creates the parse table according to the FIRST and FOLLOW sets of all non-terminals 
 *	@param {grammar} G 	- contains grammar of the language being implemented
 *	@param {table} T 	- pointer to the parse table
 *	@return table
 */
table createParseTable(grammar G, table T)
{
	int i,j,k;
	for(i=0; i<G->numNonTerminals;i++)
	{
		for(j=0; j<G->allTokens[i].ruleCount; j++)
		{
			for(k=0; k<G->numTerminals; k++)
			{
				if(G->allTokens[i].firstSet[j].firstSet[k])
					T[i][k] = G->allTokens[i].firstSet[j].ruleNum;
			}
		}

		if(G->allTokens[i].isEpsilonProduction)
		{
			for(j=0;j<G->numTerminals;j++)
			{
				if(G->allTokens[i].followSet[j])
					T[i][j] = G->allTokens[i].firstSet[G->allTokens[i].epsilonRule].ruleNum;
			}
		}
	}

	//uncomment to print the PARSE TABLE


	/*
	printf(",");
	for(i=0;i<numTerminals;i++)
		printf("%s,", G->allTokens[i+numNonTerminals].name);
	printf("\n");
	
	for(i=0;i<numNonTerminals;i++)
	{
		printf("%s,", G->allTokens[i].name);
		for(j=0;j<numTerminals;j++)
		{
			printf("%d,", pt[i][j]);
		}
		printf("\n");
	}
	*/
	return T;
}

/**
 *	@method parseInputSourceCode
 *	@desc parses the input source code using a STACK and generates the corresponding PARSE TREE
 *	@param {char *} testcaseFile 	- path of imput program file
 *	@param {table} T 				- pointer to parse table
 *	@return parseTree 				- pointer to parse tree for the given program source code
 */
parseTree  parseInputSourceCode(char *testcaseFile, table T)
{
	FILE *fp = fopen(testcaseFile, "r");

	FILE *dfafp = fopen("dfa.txt", "r");
	TreeNode *dfa;
	dfa = createDFA(dfafp, dfa);

	buffersize k = 1000;
	buffer b = createBuffer(k);
	
	grammar g = (grammar)malloc(sizeof(GrammarInfo));
	g = createGrammar(g);
	int i,j;
	table pt = (table)malloc(g->numNonTerminals * sizeof(int *));
	for(i=0;i<g->numNonTerminals;i++)
	{
		pt[i] = (int *)malloc(g->numTerminals * sizeof(int));
		for(j=0;j<g->numTerminals;j++)
			pt[i][j] = -1;
	}

	pt = createParseTable(g, pt);
	boolean success = true;

	Stack stack;
	stack.top = -1;
	stack.head = NULL;

	int dollar = hashSearch(&g->hashTable, "$", g->numTerminals + g->numNonTerminals);

	tokenInfo info = getNextToken(fp, dfa, b, k);
	stack = push(dollar, stack);
	
	parseTree tree = (parseTree)malloc(sizeof(ParseTreeNode));
	tree->parent = NULL;

	strcpy(tree->nodeSymbol, g->allTokens[g->rules[0].lhs].name);
	strcpy(tree->tokeninfo.tokenName, info.tokenName);
	tree->tokeninfo.value = (char *)malloc(strlen(info.value) * sizeof(char));
	strcpy(tree->tokeninfo.value, info.value);
	tree->tokeninfo.lineNum = info.lineNum;
	tree->completed = false;
	tree->visited = false;
	tree->numChildren = 0;

	parseTree curNode = tree;
	
	Link origHead = stack.head;
	
	stack = insertRule(g, 0, stack);
			curNode->numChildren = g->rules[0].numTerms;
			curNode->list = (parseTree *)malloc((curNode->numChildren)*sizeof(parseTree));
			curNode->completed = false;
			for(i=0;i<curNode->numChildren;i++)
			{
				curNode->list[i] = (parseTree)malloc(sizeof(ParseTreeNode));
				strcpy(curNode->list[i]->nodeSymbol, g->allTokens[g->rules[0].productionRule[i]].name);				
				(curNode->list[i])->completed = false;
				curNode->list[i]->visited = false;
				curNode->list[i]->numChildren = 0;
				curNode->list[i]->parent = curNode;
			}
			curNode = curNode->list[0];
	
	int tokenIndex, nonterm;
	int count = 0;

	while(info.value != NULL)
	{
		tokenIndex = hashSearch(&g->hashTable, info.tokenName, g->numTerminals + g->numNonTerminals);

		if(peep(stack) == dollar)
			break;

		if(tokenIndex == peep(stack))
		{
			stack = pop(stack);
			strcpy(curNode->nodeSymbol, g->allTokens[tokenIndex].name);
			strcpy(curNode->tokeninfo.tokenName, info.tokenName);

			curNode->tokeninfo.value = (char *)malloc(strlen(info.value) * sizeof(char));
			strcpy(curNode->tokeninfo.value, info.value);
			curNode->completed = true;
			while(curNode->parent != NULL)
			{
				int h = curNode->parent->numChildren;
				for(i=0;i<curNode->parent->numChildren;i++)
				{
					if(curNode->parent->list[i]->completed == false)
					{
						curNode = curNode->parent->list[i];
						break;
					}
				}
				if(i == h)
				{
					curNode->parent->completed = true;
					curNode = curNode->parent;
				}
				else
					break;
			}
			
			info = getNextToken(fp, dfa, b, k);
		}
		else if(g->allTokens[peep(stack)].terminal)
		{
			printf("ERROR_5: The token %s for lexeme %s does not match at line %d. The expected token here is %s\n", info.tokenName, info.value, info.lineNum, g->allTokens[peep(stack)].name);
			success = false;			
			break;
		}
		else
		{
			
			nonterm = peep(stack);
			stack = pop(stack);
			int ruleIndex = pt[nonterm][tokenIndex - g->numNonTerminals];
			stack = insertRule(g, ruleIndex, stack);
			
			curNode->numChildren = g->rules[ruleIndex].numTerms;
			curNode->list = (parseTree *)malloc(curNode->numChildren*sizeof(parseTree));
			strcpy(curNode->tokeninfo.tokenName, info.tokenName);
			curNode->tokeninfo.value = (char *)malloc(strlen(info.value) * sizeof(char));
			strcpy(curNode->tokeninfo.value, info.value);
			curNode->tokeninfo.lineNum = info.lineNum;
			curNode->completed = false;
			curNode->visited = false;
			for(i=0;i<curNode->numChildren;i++)
			{
				curNode->list[i] = (parseTree)malloc(sizeof(ParseTreeNode));
				strcpy(curNode->list[i]->nodeSymbol, g->allTokens[g->rules[ruleIndex].productionRule[i]].name);
				curNode->list[i]->completed = false;
				curNode->list[i]->visited = false;
				curNode->list[i]->numChildren = 0;
				curNode->list[i]->parent = curNode;
			}
			curNode = curNode->list[0];
		
			if(peep(stack) == hashSearch(&g->hashTable, "EPSILON", g->numTerminals + g->numNonTerminals))
			{
				stack = pop(stack);
				strcpy(curNode->nodeSymbol, g->allTokens[tokenIndex].name);
				strcpy(curNode->tokeninfo.tokenName, info.tokenName);
				curNode->tokeninfo.value = (char *)malloc(strlen(info.value) * sizeof(char));
				strcpy(curNode->tokeninfo.value, info.value);
				curNode->completed = true;
				
				
				while(curNode->parent != NULL)
				{
					int h = curNode->parent->numChildren;
					for(i=0;i<curNode->parent->numChildren;i++)
					{
						if(curNode->parent->list[i]->completed == false)
						{
							curNode = curNode->parent->list[i];
							break;
						}
					}
					if(i == h)
					{
						curNode->parent->completed = true;
						curNode = curNode->parent;
					}
					else
						break;
				}
				
			}
		}

		//uncomment to PRINT THE STATE OF THE STACK at each iteration	

		//printStack(stack, g);			
		
	}
	if(success)
	{
		if(info.value == NULL && stack.top > 0)
			printf("ERROR_6: Missing tokens at the end of the program\n");
		else if(info.value != NULL && stack.top == 0)
			printf("ERROR_7: Syntax error in line %d\n", info.lineNum); 
		else
			printf("Compiled Successfully: Input source code is syntactically correct!!\n");
	}
	fclose(fp);
	return tree;
}

/**
 *	@method printParseTree
 *	@desc print the parse tree in the given file
 *	@param {parseTree} tree - parse tree of the program
 *	@param {char *} outfile - path of file in which parse tree is to be printed
 */
void printParseTree(parseTree tree, char *outfile)
{
	FILE *fp = fopen(outfile, "w");
	fprintf(fp, "lexemeCurrentNode\tlineno\ttoken\tvalueIfNumber\tparentNodeSymbol\tisLeafNode(yes/no)\tNodeSymbol\n");
	printParseTreeDFS(tree, fp);
	fclose(fp);
}

/**
 *	@method printParseTreeDFS
 *	@desc prints the parse tree in the specified format using DFS (to better reperesent the tree structure)
 *	@param {parseTree} tree - parse tree of the program
 *	@param {FILE *} fp 		- file pointer in which parse tree is to be printed
 */
void printParseTreeDFS(parseTree tree, FILE *fp)
{	
	if(tree == NULL)
		return;
	if(true == tree->visited)
		return;

	int i;
	char isLeaf[4] = "yes";
	char nodeSym[30];
	strcpy(nodeSym, tree->nodeSymbol);
	if(tree->numChildren > 0)
		fprintf(fp, "-----\t\t");
	else{
		fprintf(fp, "%s\t\t", tree->tokeninfo.value);
		strcpy(isLeaf, "no");
		strcpy(nodeSym, "-----");
	}
	fprintf(fp, "%d\t\t%s\t\t", tree->tokeninfo.lineNum, tree->tokeninfo.tokenName);
	if(strcmp(tree->tokeninfo.tokenName, "NUM") == 0 || strcmp(tree->tokeninfo.tokenName, "RNUM") == 0)
		fprintf(fp, "%s\t\t", tree->tokeninfo.value);
	else
		fprintf(fp, "-----\t\t");
	if(tree->parent == NULL)
		fprintf(fp, "ROOT\t\t");
	else
		fprintf(fp, "%s\t\t", tree->parent->nodeSymbol);
	fprintf(fp, "%s\t\t%s\n", isLeaf, nodeSym);
	 
	tree->visited = true;
	for(i=0;i<tree->numChildren;i++)
	{
		printParseTreeDFS(tree->list[i], fp);
	}

}

/**
 *	@method insertRule
 *	@desc inserts the given rule in the stack
 *	@param {grammar} g 		- grammar of language being implemented
 *	@param {int} ruleIndex 	- index of rule to be pushed into the stack
 *	@param {Stack} stack 	- stack containing rules for current statement being parsed
 *	@return Stack 			- pointer to stack
 */
Stack insertRule(grammar g, int ruleIndex, Stack stack)
{
	int i;

	for(i=g->rules[ruleIndex].numTerms-1;i>=0;i--)
	{
		stack = push(g->rules[ruleIndex].productionRule[i], stack);
		//printf("%s ",g->allTokens[g->rules[ruleIurnndex].productionRule[i]].name);
	}
	//printf("\n");
	return stack;
}

/**
 *	@method push
 *	@desc pushes the given data at the top of the stack
 *	@param {int} data 		- data to bepushed into the stack
 *	@param {Stack} stack 	- stack pointer
 *	@return Stack 			- updated stack pointer
 */
Stack push(int data, Stack stack)
{
	Link temp = (Link)malloc(sizeof(Node));
	temp->index = data;
	temp->next = stack.head;
	stack.head = temp;
	stack.top++;
	return stack;
}

/**
 *	@method pop
 *	@desc pops data at the top of the stack
 *	@param {Stack} stack 	- stack pointer
 *	@return Stack 			- updated stack pointer
 */
Stack pop(Stack stack)
{
	if(stack.top < 0)
		return stack;
	stack.head = stack.head->next;
	stack.top--;
	return stack;
}

/**
 *	@method printStack
 *	@desc prints the current state of the stack
 *	@param {Stack} Stack 	- current stack
 *	@param {grammar} g 		- grammer of the language being implemented
 */
void printStack(Stack stack, grammar g)
{
	printf("\n");
	printf("STACK -\n");
	Link temp = stack.head;
	while(temp != NULL)
	{
		printf("%s, ", g->allTokens[temp->index].name);
		temp = temp->next;
	}
	printf("\n\n");
}

/**
 *	@method peep
 *	@desc returns the data at the top of the stack without removing it
 *	@param {Stack} stack 	- current stack
 *	@return int 			- data on top of the stack
 */
int peep(Stack stack)
{ 
	return stack.head->index;
}

/**
 *	@method generateFirstSets
 *	@desc generates the FIRST set for all non-terminals
 *	@param {grammar} g - pointer to grammar where the set is to be stored
 */
void generateFirstSets(grammar g)
{
	int i, total = g->numNonTerminals + g->numTerminals;
	for(i=0;i<g->numNonTerminals;i++)
	{
		
		getFirstSet(g, i);
	}
}

/**
 *	@method getFirstSet
 *	@desc calculates and stores the FIRST set of the given non-terminal RECURSIVELY
 *	@param {grammar} g - pointer to grammar where the set is to be stored
 *	@param {int} index - index for the set
 */
void getFirstSet(grammar g, int index)
{
	int i,j,k;
	if(g->allTokens[index].firstSetDone)
		return;
	for(i = 0;i<g->allTokens[index].ruleCount;i++)
	{
		Rule r = g->rules[g->allTokens[index].firstSet[i].ruleNum];
		
		if(g->allTokens[r.productionRule[0]].terminal)
		{
			g->allTokens[index].firstSet[i].firstSet = insert(g->allTokens[index].firstSet[i].firstSet, r.productionRule[0] - g->numNonTerminals);
			if(r.productionRule[0] == epsi)
			{
				g->allTokens[index].isEpsilonProduction = true;
				g->allTokens[index].epsilonRule = i;
			}
			continue;
		}
		getFirstSet(g, r.productionRule[0]);
		for(j=0;j<g->allTokens[r.productionRule[0]].ruleCount;j++)
		{
			g->allTokens[index].firstSet[i].firstSet = unionOf(g->allTokens[index].firstSet[i].firstSet, g->allTokens[r.productionRule[0]].firstSet[j].firstSet, g->numTerminals);
		}
	}
	g->allTokens[index].firstSetDone = true;
}

/**
 *	@method insert
 *	@desc {SET_OPS} inserts value x in the set
 *	@param {Set} s 	- set in which value is to be inserted
 *	@param {int} x 	- value to be inserted
 *	@return Set 	- updated set
 */
Set insert(Set s, int x)
{
	s[x] = true;
	return s;
}

/**
 *	@method unionOf
 *	@desc {SET_OPS} returns Union of the given sets
 *	@param {Set} s1 	- set 1 for union operation
 *	@param {Set} s2 	- set 2 for union operation
 *	@param {int} size 	- size of set
 *	@return Set 		- union set
 */
Set unionOf(Set s1, Set s2, int size)
{
	int i;
	for(i=0;i<size;i++)
	{
		if(s2[i])
			s1[i] = true;
	}
	return s1;
}

/**
 *	@method hashInsert
 *	@desc {HASH_TABLE_OPS} insert the key-value pair in the hash table
 *	@param {HashTable *} hashTable 	- table where the key-value pair is to be stored
 *	@param {char *} key 			- string key for storage
 *	@param {int} value 				- value to be stored
 *	@param {int} m 					- hash parameter for probing 
 */
void hashInsert(HashTable *hashTable, char *key, int value, int m)
{
	int i, j;
	for(i=0;i<m;i++)
	{
		j = hashProbing(key, i, m);
		if((*hashTable)[j].value == -1)
		{
			(*hashTable)[j].value = value;
			strcpy((*hashTable)[j].key, key);
			break;
		}
	}
}

/**
 *	@method hashSearch
 *	@desc {HASH_TABLE_OPS} searches the hash table for the given key and return the corresponding value
 *	@param {HashTable *} hashTable - table where the key is to be searched
 *	@param {char *} key - string key for which the value is to be searched
 *	@param {int} m 		- hash parameter for probing 
 *	@return int 		- value for given key, -1 if not found
 */
int hashSearch(HashTable *hashTable, char *key, int m)
{
	int i = 0,j;
	do
	{
		j = hashProbing(key, i, m);
		if(strcmp((*hashTable)[j].key, key) == 0)
			return (*hashTable)[j].value;
		i++;	
	}while(i < m || (*hashTable)[j].value == -1);
	
	return -1;
}

/**
 *	@method hashProbing
 *	@desc {HASH_TABLE_OPS} probing for the hash value
 *	@param {char *} key - string key to be hashed
 *	@param {int} index 	- current index of table being probed
 *	@param {int} m 		- hash parameter for probing 
 *	@return int 		- index
 */
int hashProbing(char *key, int index, int m)
{
	return (hash(key) + index) % m;
}

/**
 *	@method hash
 *	@desc {HASH_TABLE_OPS} hash value
 *	@param {char *} key - string key to be hashed
 *	@return int 		- hash value
 */
int hash(char *key)
{
	int i, len = strlen(key), total = 0;
	for(i=1;i<len;i++)
	{
		total += (i * key[i]);
	}
	return total;
}
