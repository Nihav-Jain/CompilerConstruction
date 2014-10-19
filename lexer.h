/**
 *	@author Nihav Jain
 *	@desc lexer.h - function definitions for the lexer
 *	NOTE: majority of definitions could be improved but they are as per assignment requirements
 */

#ifndef _LEXER_H
#define _LEXER_H

extern FILE *getStream(FILE* fp, buffer B, buffersize k);
extern buffer createBuffer(buffersize size);
extern tokenInfo getNextToken(FILE *fp, TreeNode *dfa, buffer b, buffersize k);
extern TreeNode *createDFA(FILE* fp, TreeNode *dfa);
extern tokenInfo extractInfo(tokenInfo info, buffer b, buffersize k);

#endif