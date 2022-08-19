/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* tree.h: contains declarations of struct and functions in tree.c	*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/05/06		Keane Wong		Initial Version		*/
/************************************************************************/

#ifndef TREE_H
#define TREE_H
#include <stdio.h>
#include <stdlib.h>
#include "move.h"


typedef struct TreeNode TREENODE;
struct TreeNode{
	MOVE *potMove;//stands for potential move
	TREENODE *child;
	TREENODE *nextSibling;
	//int Modifier;	//each move represents a change in the state of the baord's value. This stores that change. CURRENTLY UNUSED: IGNORE
	
};



TREENODE *NewNode(MOVE *newMove);	//makes a new node, all data default to NULL

TREENODE *EmptyNode();

int isEmptyNode(TREENODE *node);

void SetChild(TREENODE *parent, TREENODE *newChild);	//sets the child of parent to newchild, deleting the previous one if there already was one present

TREENODE *GetChild(TREENODE *node);			//returns child, null if no child

void SetNext(TREENODE *node, TREENODE *newSibling);

TREENODE *GetNext(TREENODE *node);		//gets next sibling

MOVE *GetNodeMove(TREENODE *node);

void SetMove(TREENODE *node, MOVE *newMove);

//int GetMod(TREENODE *node);

//void SetMod(TREENODE *node, int newmod);

void DeleteNode(TREENODE *nodeTBD);		//deletes node and only that node

void DeleteNodeRecursive(TREENODE *nodeTBD);		//recursively deletes node and all siblings and children

void PrintAcross(TREENODE *node);

void PrintAll(TREENODE *node);






#endif
