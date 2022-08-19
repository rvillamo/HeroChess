/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* move.c: contains definitions of structs and functions relating to 	*/
/*				moves					*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/04/23		Keane Wong		Initial Version		*/
/************************************************************************/

#include "move.h"

/* create new move struct */
MOVE *NewMove(PIECE *piece, PIECE *removedPiece, char *source, char *destination){
	MOVE *m;
	m = (MOVE *)malloc(sizeof(MOVE));
	if (!m)
	{ 
		perror("Out of memory! Aborting...");
		exit(10);
	} /* fi */
	m->piece = *piece;	
	m->removedPiece = *removedPiece;
	m->source = malloc(sizeof(char)*2);
	m->destination = malloc(sizeof(char)*2);
	strcpy(m->source, source);
	strcpy(m->destination, destination);
	return m;
} 

/* delete move */
void DeleteMove(MOVE *m){
	assert(m);	
	free(m);
}

/* return player number */
PIECE GetPiece(MOVE *m){
	return m->piece;
}

/* returns removed piece */
PIECE GetRemovedPiece(MOVE *m){
	return m->removedPiece;
}

/* returns Move */
char *GetSource(MOVE *m){
	return m->source;
}

/* returns destination */
char *GetDestination(MOVE *m){
	return m->destination;
}

/* prints move */
void PrintMove(MOVE *m){
	assert(m);
	printf(" %c%c %2s %2s\n", GetColor(&m->removedPiece), GetType(&m->removedPiece), m->source, m->destination);
}

/*create new move struct */

/* returns column source */
int getColS(MOVE *m){
	return ((m->source)[0]-'A');
}

/* returns column destination */
int getColD(MOVE *m){
	return ((m->destination)[0]-'A');
}

/* returns row source */
int getRowS(MOVE *m){
	return ((m->source)[1]-'1');
}

/* returns row destination */
int getRowD(MOVE *m){
	return ((m->destination)[1]-'1');
}

// EOF //
