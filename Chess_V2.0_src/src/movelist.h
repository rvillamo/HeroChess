/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* movelist.h: contains declarations of structs and functions in  	*/
/*				movelist.c				*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/04/23		Keane Wong		Initial Version		*/
/************************************************************************/

#ifndef MOVELIST_H
#define MOVELIST_H
#include "move.h"
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <assert.h>

/* alias */
typedef struct MoveList MLIST;
typedef struct MoveListEntry MENTRY;

/* list structs */
struct MoveList{
	int length;
	MENTRY *first;
	MENTRY *last;
};

struct MoveListEntry{
	MLIST *list;
	MENTRY *next;
	MENTRY *prev;
	MOVE *move;
};

/* allocate a new move list */ 
MLIST *NewMoveList(void); 

/* delete a move list (and all entries) */ 
void DeleteMoveList(MLIST *l); 

/* print a move list */ 
void PrintMoveList(MLIST *l);

/* append a move at end of list */
void AppendMove(MLIST *l, PIECE *piece, PIECE *removedpiece, int colS, int rowS, int colD, int rowD);

/* prepend a move to the beginning of list */
// void PrependStudent(MLIST *l, MOVE *m);

/* remove the last move from the list */
MOVE *RemoveLastMove(MLIST *l);

/* remove the first element of the movelist*/
MOVE *RemoveFirstMove(MLIST *l);

/* length of the move list*/
int GetLength(MLIST *l);

#endif

// EOF //
