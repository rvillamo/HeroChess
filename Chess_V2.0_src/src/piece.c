/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* piece.c: contains defintions of structs and functions related to the	*/
/*				chess pieces				*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/04/17		Paul Lee		Initial Version		*/
/************************************************************************/

#include "piece.h"

/* structs for pieces */
PIECE *NewPiece(char type, char color){
	PIECE *p;
	p = (PIECE*)malloc(sizeof(PIECE));
	if (!p)
	{ 
		perror("Out of memory! Aborting...");
		exit(10);
	} /* fi */

	p->type = type;
	p->color = color;
	return p;
}

PIECE makeBlank(){
	PIECE p;
	p.type = ' ';
	p.color = ' ';
	return p;
}

/* return player number */
char GetType(PIECE *mypiece){
	return mypiece->type;
}

char GetColor(PIECE *mypiece){
	return mypiece->color;
}

void SetType(PIECE *mypiece, char c){
	mypiece->type = c;
}

void SetColor(PIECE *mypiece, char c){
	mypiece->color = c;
}
