/**
 *	@author Nihav Jain
 *	@desc parser.h - function definitions for the parser
 *	NOTE: majority of definitions could be improved but they are as per assignment requirements
 */

#ifndef _PARSER_H
#define _PARSER_H

extern grammar createGrammar(grammar g);
extern table createParseTable(grammar G, table T);
extern void generateFirstSets(grammar g);
extern void getFirstSet(grammar g, int index);

extern void hashInsert(HashTable *hashTable, char *key, int value, int m);
extern int hashSearch(HashTable *hashTable, char *key, int m);
extern int hashProbing(char *key, int index, int m);
extern int hash(char *key);

extern Set insert(Set s, int x);
extern Set unionOf(Set s1, Set s2, int size);

extern parseTree  parseInputSourceCode(char *testcaseFile, table T);
extern Stack insertRule(grammar g, int ruleIndex, Stack head);
extern Stack push(int data, Stack head);
extern Stack pop(Stack head);
extern int peep(Stack head);
extern void printStack(Stack stack, grammar g);
extern void printParseTree(parseTree tree, char *outfile);
extern void printParseTreeDFS(parseTree tree, FILE *fp);

#endif
