/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* tree.c: contains definitions of structs and functions pertaining to	*/
/*			the tree implementation				*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/05/06		Keane Wong		Initial Version		*/
/************************************************************************/

#include "tree.h"



TREENODE *NewNode(MOVE *newMove)
{
	TREENODE *newnode = (TREENODE*)malloc(sizeof(TREENODE));
	if (!newnode)
	{ 
		perror("Out of memory! Aborting...\n");
		exit(10);
	} /* fi */
	newnode->potMove = newMove;
	newnode->child = NULL;
	newnode->nextSibling = NULL;
	return newnode;
}

TREENODE *EmptyNode()//same as newnode but is empty, no move. Used as a placeholder
{
	TREENODE *newnode = (TREENODE*)malloc(sizeof(TREENODE));
	if (!newnode)
	{ 
		perror("Out of memory! Aborting...\n");
		exit(10);
	} //fi 
	newnode->potMove = NULL;
	newnode->child = NULL;
	newnode->nextSibling = NULL;
	return newnode;
}





MOVE *GetNodeMove(TREENODE *node){
	return node->potMove;
}

void SetMove(TREENODE *node, MOVE *newMove)
{
	if(GetNodeMove(node) != NULL)
	{
		DeleteMove(GetNodeMove(node));
	}
	node->potMove = newMove;
}

/*
int GetMod(TREENODE *node)
{
	return node->Modifier;
}

void SetMod(TREENODE *node, int newmod)
{
	node->Modifier = newmod;
}
*/


int isEmptyNode(TREENODE *node)
{
	if(node == NULL)
	{
		return 1;
	}
	if(GetNodeMove(node) == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void SetChild(TREENODE *parent, TREENODE *newChild)
{
	if(parent->child != NULL)//if there is already a child to the node we delete it and overwrite it
	{
		DeleteNode(parent->child);
	}
	parent->child = newChild;

}

TREENODE *GetChild(TREENODE *node)
{
	return node->child;
}

void SetNext(TREENODE *node, TREENODE *newSibling)
{
	if(node->nextSibling!=NULL)
	{
		DeleteNode(node->nextSibling);
	}
	node->nextSibling = newSibling;
}

TREENODE *GetNext(TREENODE *node)
{
	return node->nextSibling;
}

void DeleteNode(TREENODE *nodeTBD)
{
	DeleteMove(GetNodeMove(nodeTBD));
	free(nodeTBD);
}



void DeleteNodeRecursive(TREENODE *nodeTBD)
{
	//printf("one\n");
	if(isEmptyNode(nodeTBD))
	{
		return;
	}
	DeleteNodeRecursive(GetNext(nodeTBD));
	DeleteNodeRecursive(GetChild(nodeTBD));
	DeleteNode(nodeTBD);
	

}

void PrintAcross(TREENODE *node)
{
	TREENODE *curNode = node;
	while(node!=NULL)
	{
		PrintMove(GetNodeMove(node));
		curNode = GetNext(curNode);
	}
}

void PrintAll(TREENODE *node)
{
	//printf("MOVINGc: ");
	if(node == NULL)
	{
		return;
	}
	PrintAll(GetNext(node));
	PrintAll(GetChild(node));
	
	if(GetNodeMove(node) !=NULL)
	{
		PrintMove(GetNodeMove(node));
	}
	else
	{
		printf("Empty Node\n");
	}
}


















