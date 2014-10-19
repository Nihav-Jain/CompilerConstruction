/**
 *	@author Nihav Jain
 *	@desc lexerDef.h - structures and type definitions for the lexer
 */

#ifndef _LEXERDEF_H
#define _LEXERDEF_H

typedef enum {false=0, true=1}boolean;

typedef int buffersize;
struct _buffer{
	char *buff;
	char *begin;
	char *forward;
	int lastFilled;
};
typedef struct _buffer buff;
typedef buff* buffer;

typedef char token[20];
struct _tokeninfo;
typedef struct _tokeninfo tokenInfo;
struct _tokeninfo{
	token tokenName;
	char *value;
	int lineNum;
};

struct _treenode;
typedef struct _treenode TreeNode;
typedef TreeNode *ChildList;
struct _treenode {
  char character;
  ChildList *list;
  int numChildren;
  boolean isFinal;
  boolean translate;
  token tokenName;
};

#endif