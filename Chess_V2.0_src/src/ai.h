/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* ai.h: contains declarations of functions in ai.c for the automated	*/
/*			player gameplay					*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/04/23		Keane Wong		Initial Version		*/
/************************************************************************/

#ifndef AI_H
#define AI_H
#include "piece.h"
#include "movelist.h"
#include "tree.h"

// A function to makemove on the AI's behalf based on what its turn is. Recursively searches to a depth of moves equal to Depth. 
int GetAITurn(PIECE **myBoard, char computerColor, MLIST *myList, int Depth);

// private functions
// returns value based on type of inputted piece;  positive values for friendly pieces and negative values for enemy pieces
int GetValue(PIECE *myPiece, char curTurnColor);

// determines worth of board by adding all the pieces' values; analyzes risks and benefits
int SumBoard(PIECE **myBoard, char curTurnColor);

// evaluates board and determines what move would leave it in the best position
MOVE *HighestEval(PIECE **myBoard, char curTurnColor, MLIST *myList);

// gets every single possible move on the board and stores them as children to root, all connected to one another by pointer links
void AllPossibilities(PIECE **myBoard, char curTurnColor, MLIST *myList, TREENODE *root);

// makes a random hardcoded opening move from a small library
int MakeOpeningMove(PIECE **myBoard, char curTurnColor, MLIST *myList);


int MakeRandomOpeningMove(PIECE **myBoard, char curTurnColor, MLIST *myList);
void AIPromote(PIECE **myBoard);

// private functions
// make a move without appending it to myList, instead adds the move to an empty node or blankNode; similar to MakeMoveNoAppend
int MakeMoveAppendNode(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor, MLIST *myList, TREENODE *blankNode);

// a helper function to recursively look through the various levels of moves
int SearchMovesRecursive(PIECE **myBoard, char computerColor, MLIST *myList,TREENODE *myNode, MOVE *bestMove, int Depth, int originalDepth);

// A set of opening moves
// makes the first ai move using a small library of known chess moves
int MakeOpeningMove(PIECE **myBoard, char curTurnColor, MLIST *myList);

// sicilian defense opening move
int SicilianDefense(PIECE **myBoard, char curTurnColor, MLIST *myList);

// queen's gambit opening move
int QueensGambit(PIECE **myBoard, char curTurnColor, MLIST *myList);

// english opening, opening move
int EnglishOpening(PIECE **myBoard, char curTurnColor, MLIST *myList);

// dutch defense opening move
int DutchDefense(PIECE **myBoard, char curTurnColor, MLIST *myList);

#endif

// EOF //
