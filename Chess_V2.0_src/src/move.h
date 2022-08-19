/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* move.h: contains declarations of structs and functions in move.c 	*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/04/23		Keane Wong		Initial Version		*/
/************************************************************************/

#ifndef MOVE_H
#define MOVE_H

#include "piece.h"
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <assert.h>

/* move struct */
struct Move{
	PIECE piece;
	PIECE removedPiece;
	char *source;	
	char *destination;
};

/* alias */
typedef struct Move MOVE;

/* Create struct that contains each moves */
MOVE *NewMove(PIECE *piece, PIECE *removedPiece, char *source, char *destination);

/* Delete Move */
void DeleteMove(MOVE *m);

/* returns removed piece */
PIECE GetRemovedPiece(MOVE *m);

/* return move as char pointer*/
char *GetSource(MOVE *m);

/* returns destination */
char *GetDestination(MOVE *m);

/* return player number */
PIECE GetPiece(MOVE *m);

/* prints move */
void PrintMove(MOVE *m);

/* returns column source */
int getColS(MOVE* m);

/* returns row source */
int getRowS(MOVE* m);

/* returns column destination */
int getColD(MOVE* m);

/* returns row destination */
int getRowD(MOVE* m);

#endif

// EOF //
