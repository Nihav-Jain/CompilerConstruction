/**
 *	@author Nihav Jain
 *	@desc parserDef.h - structures and type definitions for the parser
 */

#ifndef _PARSERDEF_H
#define _PARSERDEF_H

#include "lexerDef.h"
typedef boolean* Set;

typedef int** table;
//typedef ParseTable* table;

typedef struct{
	char key[30];
	int value;
}KeyValue;
typedef KeyValue* HashTable;

typedef struct _rule{
	int lhs;
	int *productionRule;
	int numTerms;
	int ruleNum;
} Rule;

typedef Rule* Rules;

typedef struct _firstsetinfo{
	int ruleNum;
	Set firstSet;
} FirstSetInfo;
typedef FirstSetInfo* FirstSet;

typedef struct _element{
	char name[30];
	boolean terminal;
	Set followSet;
	boolean isEpsilonProduction;
	int epsilonRule;
	FirstSet firstSet;
	boolean firstSetDone;
	int ruleCount;
} Element;
typedef Element* Terms;

typedef struct{
	Terms allTokens;
	Rules rules;
	HashTable hashTable;
	int numTerminals;
	int numNonTerminals;
} GrammarInfo;
typedef GrammarInfo* grammar;

struct _node;
typedef struct _node Node;
typedef Node *Link;
struct _node{
	int index;
	Link next;
};
typedef struct{
	Link head;
	int top;
}Stack;

struct _parsetreenode;
typedef struct _parsetreenode ParseTreeNode;
typedef ParseTreeNode *parseTree;
struct _parsetreenode {
  char nodeSymbol[50];
  parseTree parent;
  parseTree *list;
  int numChildren;
  boolean completed;
  boolean visited;
  tokenInfo tokeninfo;
};

#endif
