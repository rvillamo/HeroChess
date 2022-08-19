/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* game.h: contains declarations of functions within game.c		*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/04/17		Paul Lee		Initial Version		*/
/************************************************************************/

#ifndef GAME_H
#define GAME_H
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include "piece.h"
#include "movelist.h"

// wrapper function that calls other isLegal functions on a case by case basis
int isLegal(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor, MLIST *myList);

// checks that pawn's destination is at most 2 spaces ahead for the first turn then 1 space ahead; supports en passant
int isLegalPawn(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor, MLIST *myList);

// checks that knight's destination is 2 spaces vertically and 1 space horizontally; checks if destination is empty or has enemy piece
int isLegalKnight(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor);

// checks that rook's destination is any space vertical or horizontal to it; checks for enemy piece at its destination; supports castling
int isLegalRook(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor);

// checks that bishop's destination is any space diagonal to it; checks for enemy piece at its destination
int isLegalBishop(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor);

// checks that queen's destination is any space in any direction; checks for enemy pieces at its destination
int isLegalQueen(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor);

// checks that king's destination is one space in any direction; supports castling
int isLegalKing(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor);

// determines if move is legal and then executes it
int MakeMove(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor, MLIST *myList);

// wrapper function that calls the other isChecked functions much like how the isLegal function works
int isChecked(PIECE **myBoard, char turnColor);

// returns 1 if isCheckmate, does NOT check if the king is currently in check. For a true checkmate, use in conjunction with isChecked
int isCheckmate(PIECE **myBoard, char turnColor, MLIST *myList);


// private functions
// checks for rooks or queens horizontal or vertical of the piece at colKing and rowKing
int isCheckedByRQ(PIECE **myBoard, char enemyColor, int colKing, int rowKing);

// checks for bishops or queens diagonal in all directions of the piece at colKing and rowKing
int isCheckedByBQ(PIECE **myBoard, char enemyColor, int colKing, int rowKing);

// checks for knights that can check piece at colKing and rowKing
int isCheckedByN(PIECE **myBoard, char enemyColor, int colKing, int rowKing);

// checks for pawns that can check piece at colKing and rowKing
int isCheckedByP(PIECE **myBoard, char enemyColor, int colKing, int rowKing);

// checks for kings that can check piece at colKing and rowKing
int isCheckedByK(PIECE **myBoard, char enemyColor, int colKing, int rowKing);


// makeMove but doesnt modify myList
int MakeMoveNoAppend(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor, MLIST *myList);


#endif

// EOF //
