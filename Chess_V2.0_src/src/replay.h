/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* replay.h: contains declarations of functions in replay.c for printing*/
/*				the game replay				*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/05/02		Paul Lee		Initial Version		*/
/************************************************************************/

#ifndef REPLAY_H
#define REPLAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "piece.h"
#include "move.h"
#include "movelist.h"
#include "board.h"


/* function to generate game replay file */
void replay(MLIST *l);

void printFormat(FILE *fptr, char *fname);

#endif
