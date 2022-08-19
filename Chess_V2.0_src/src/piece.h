/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* piece.h: contains declarations of structs and functions in piece.c	*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/04/17		Paul Lee		Initial Version		*/
/************************************************************************/
 
#ifndef PIECE_H
#define PIECE_H
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 

/* struct for pieces */
struct Piece{
	char type;
	char color;
};

typedef struct Piece PIECE;

PIECE *NewPiece(char type, char color);

PIECE makeBlank();

/* return player number */
char GetType(PIECE *mypiece);

char GetColor(PIECE *mypiece);

void SetType(PIECE *mypiece, char c);

void SetColor(PIECE *mypiece, char c);

#endif
