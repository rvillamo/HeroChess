#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "piece.h"
#include "board.h"
#include "movelist.h"

// wrapper function that calls other isLegal functions on a case by case basis
int isLegal(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor, MLIST *myList)
{
	if( colDestination < 0 || colDestination > 7 || rowDestination < 0 || rowDestination > 7 )
	{
		//printf("Error: Out of bounds destination\n");
		return 0;
	}
	if(colSource < 0 || colSource > 7 || rowSource < 0 || rowSource > 7 )
	{
		//printf("Error: Out of bounds destination\n");
		return 0;
	}
	if(colSource == colDestination && rowSource == rowDestination)
	{
		//printf("Error: Destination same as beginning\n");
		return 0;
	}
	if(isEmpty(myBoard,colSource,rowSource) == 1)
	{
		//if there isnt anything at the selected source then it automatically fails
		//printf("Error: No piece at selected location\n");  
		return 0;
	}
	if(GetColor(getPiece(myBoard,colSource,rowSource)) != curTurnColor)
	{
		//printf("Error: Opponent's piece selected! It is currently player %c's turn.\n",curTurnColor);
		return 0;
	}
	if(GetType(getPiece(myBoard,colSource,rowSource)) != 'K' )//castling
	{
		if(GetColor(getPiece(myBoard,colSource,rowSource)) == GetColor(getPiece(myBoard,colDestination,rowDestination)) )
		{
			//printf("Error: Destination square contains a friendly piece. Illegal move\n");
			////printf("%c %d %d",GetColor(getPiece(myBoard,colSource,rowSource)),colSource,rowSource);
			////printf("%c %d %d",GetColor(getPiece(myBoard,colDestination,rowDestination)),colDestination,rowDestination);
			return 0;
		}
	}
	else if(GetType(getPiece(myBoard,colDestination,rowDestination)) != 'R' && GetColor(getPiece(myBoard,colDestination,rowDestination)) == curTurnColor)
	{
		return 0;
	}

	switch(GetType(getPiece(myBoard,colSource,rowSource)))
	{
		case 'P': 
		{
			int p = isLegalPawn(myBoard,colSource,rowSource,colDestination,rowDestination, curTurnColor, myList);
			if(p)//we create a temporary array to test the move and see if it opens up to a check
			{
				
				PIECE **tempBoard = copyBoard(myBoard);
				movePiece(tempBoard,colSource,rowSource,colDestination,rowDestination);//move piece to destination
				int forward;
				if(p ==2)//if en passant we move the pawn right behind the destination
				{
					forward = curTurnColor=='w' ? 1 : -1;
					movePiece(tempBoard,colSource, rowSource, colDestination, rowDestination-forward);
				}
				if(!isChecked(tempBoard, curTurnColor))//if its not checked
				{
					deleteBoard(tempBoard);
					return 1;//return a success marker
				}
				else	//if it is checked
				{
					//printf("Error: This move leaves friendly king open to check\n");
					deleteBoard(tempBoard);
					return 0;
				}
				
				return 1;
			}
			else
			{
				//printf("Error: Illegal move.\n");
				return 0;
			}
			break;
		}
		case 'R': 
		{
			if(isLegalRook(myBoard,colSource,rowSource,colDestination,rowDestination, curTurnColor))
			{
				PIECE **tempBoard = copyBoard(myBoard);
				movePiece(tempBoard,colSource,rowSource,colDestination,rowDestination);
				if(!isChecked(tempBoard, curTurnColor))//if its not checked
				{
					deleteBoard(tempBoard);
					return 1;//return a success marker
				}
				else
				{
					//printf("Error: This move leaves friendly king open to check\n");
					deleteBoard(tempBoard);
					return 0;
				}
				
				return 1;
			}
			else
			{
				//printf("Error: Illegal move.\n");
				return 0;
			}
			break;
		}
		case 'N': 
		{
			if(isLegalKnight(myBoard,colSource,rowSource,colDestination,rowDestination, curTurnColor))
			{
				
				PIECE **tempBoard = copyBoard(myBoard);
				movePiece(tempBoard,colSource,rowSource,colDestination,rowDestination);
				if(!isChecked(tempBoard, curTurnColor))//if its not checked
				{
					deleteBoard(tempBoard);
					return 1;//return a success marker
				}
				else
				{
					//printf("Error: This move leaves friendly king open to check\n");
					deleteBoard(tempBoard);
					return 0;
				}
				
				return 1;
			}
			else
			{
				//printf("Error: Illegal move.\n");
				return 0;
			}
			break;
		}
		case 'B': 
		{
			if(isLegalBishop(myBoard,colSource,rowSource,colDestination,rowDestination, curTurnColor))
			{
				PIECE **tempBoard = copyBoard(myBoard);
				movePiece(tempBoard,colSource,rowSource,colDestination,rowDestination);
				if(!isChecked(tempBoard, curTurnColor))//if its not checked
				{
					deleteBoard(tempBoard);
					return 1;//return a success marker
				}
				else
				{
					//printf("Error: This move leaves friendly king open to check\n");
					deleteBoard(tempBoard);
					return 0;
				}
				return 1;
			}
			else
			{
				//printf("Error: Illegal move.\n");
				return 0;
			}
			break;
		}
		case 'K': 
		{
			if(isLegalKing(myBoard,colSource,rowSource,colDestination,rowDestination, curTurnColor)) //in the event of castling, we need to do a special checl
			{

				PIECE **tempBoard = copyBoard(myBoard);
				if(GetType(getPiece(myBoard,colDestination,rowDestination)) == 'R')//this checks for castling. If we're castling we need to do a special maneuver
				{
					int distance = colDestination - colSource;
					//distance = distance>0 ? distance : -1*distance;//ensure distance is positive
					if(distance<0)
					{
						movePiece(tempBoard, colSource, rowSource, colSource-2, rowSource);
						movePiece(tempBoard, colDestination, rowDestination, colSource-1,rowDestination);
					}
					else
					{
						movePiece(tempBoard, colSource, rowSource, colSource+2, rowSource);
						movePiece(tempBoard, colDestination, rowDestination, colSource+1,rowDestination);
					}
				}
				else//if we arent castling
				{
					movePiece(tempBoard, colSource, rowSource, colDestination,rowDestination);
				}

				if(!isChecked(tempBoard, curTurnColor))//if its not checked
				{
					deleteBoard(tempBoard);
					return 1;//return a success marker
				}
				else
				{
					//printf("Error: This move leaves friendly king open to check\n");
					deleteBoard(tempBoard);
					return 0;
				}
				
				return 1;
			}
			else
			{
				//printf("Error: Illegal move.\n");
				return 0;
			}
			break;
		}
		case 'Q': 
		{
			if(isLegalQueen(myBoard,colSource,rowSource,colDestination,rowDestination, curTurnColor))
			{
				
				PIECE **tempBoard = copyBoard(myBoard);
				movePiece(tempBoard,colSource,rowSource,colDestination,rowDestination);
				if(!isChecked(tempBoard, curTurnColor))//if its not checked
				{
					deleteBoard(tempBoard);
					return 1;//return a success marker
				}
				else
				{
					//printf("Error: This move leaves friendly king open to check\n");
					deleteBoard(tempBoard);
					return 0;
				}
				
				return 1;
			}
			else
			{
				//printf("Error: Illegal move.\n");
				return 0;
			}
			break;
		}
	}
	//printf("Error: Invalid type present \n");
	return 0;


}

// checks that pawn's destination is at most 2 spaces ahead for the first turn then 1 space ahead; supports en passant
int isLegalPawn(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor, MLIST *myList)
{

	//char enemyColor = curTurnColor=='w' ? 'b' : 'w';//determines enemy color
	int forward;
	if(GetColor(getPiece(myBoard,colSource, rowSource)) == 'w')
	{
		forward = 1;//because white is in the front row, numbered starting from 0, we need a forward multiplier to determine which was is direction for our chosen piece
	}
	else
	{
		forward = -1;
	}
	char enemyColor = curTurnColor=='w' ? 'b' : 'w';
	if(isEmpty(myBoard,colDestination,rowDestination) )//if the spot is empty
	{
		if(abs(colSource-colDestination) == 1 && (forward*(rowDestination-rowSource)) == 1 )//checking for en passant, this covers a diagonal move
		{
			if(GetType(getPiece(myBoard,colDestination,rowDestination-forward)) == 'P' && GetColor(getPiece(myBoard,colDestination,rowDestination-forward)) == enemyColor)//if the piece right behind destination is an enemy pawn we can capture it
			{
				if(myList->last->move->source[0] == 'A' + colDestination && myList->last->move->source[1] == '1'+rowDestination+forward)//checking if enemypawn was previously in space in front of the destination square. If it is then the move is en passant and is legal
				{
					return 2;
				}
			}
		}
		if(colSource == colDestination && (forward*(rowDestination-rowSource) == 1))//checking if pawn is moving only forward
		{
			return 1;
		}
		if(colSource == colDestination && (forward*(rowDestination-rowSource) == 2))//covers a 2 square jump 
		{
			if((curTurnColor == 'w' && rowSource == 1 ) || (curTurnColor == 'b' && rowSource == 6 ))//checks if the pawn is in initial position
			{
				if(isEmpty(myBoard,colDestination,rowSource+forward) )//if the spot one spot forward from the pawn is empty, then it is legal
				{
					return 1;
				}
			}
		}

	}
	else //if the spot has a piece in it. We've already checked if the piece there is friendly or not, so by this point itll always be an enemy piece
	{
		if ((colDestination-colSource == 1 || colSource-colDestination ==1) && (forward*(rowDestination-rowSource) == 1))//checks for a diagonal capture
		{
			return 1;
		}	
	}
    ////printf("What is this\n");
	return 0;//if it didnt get caught by any of the above then its invalid
}

// checks that knight's destination is 2 spaces vertically and 1 space horizontally; checks if destination is empty or has enemy piece
int isLegalKnight(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor)
{
	if( (abs(rowSource - rowDestination) == 1 && abs(colSource-colDestination) == 2) || (abs(rowSource - rowDestination) == 2 && abs(colSource-colDestination) == 1) )//we only need to check if the knight's new position is an l shape and thats it
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// checks that rook's destination is any space vertical or horizontal to it; checks for enemy piece at destination; supports castling
int isLegalRook(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor)
{
	int directionToGo;

	if(rowSource == rowDestination)
	{
		directionToGo = colDestination-colSource > 0 ? 1 : -1;//if destination is greater than source, we need to go forward and if its less than or behind source, we go backwards
		for(int i = colSource+directionToGo; i != colDestination; i+= directionToGo)//starts at one square towards destination and repeatedly checks for empty spaces between then and here
		{
			if(!isEmpty(myBoard,i,rowSource))
			{
				//printf("Piece in the way at %d %d \n",i,rowSource);
				//printf("Piece type is \"%c\" \"%c\" \n", GetColor(getPiece(myBoard,i,rowSource)),GetType(getPiece(myBoard,i,rowSource)) );

				return 0;
			}
		}
		return 1;//if there were all empty spaces then the path is clear and its legal
	}
	else if (colSource == colDestination)
	{
		directionToGo = rowDestination-rowSource > 0 ? 1 : -1;//if destination is greater than source, we need to go forward and if its less than or behind source, we go backwards
		for(int i = rowSource+directionToGo; i != rowDestination; i+= directionToGo)//starts at one square towards destination and repeatedly checks for empty spaces between then and here
		{
			if(!isEmpty(myBoard,colSource,i))
			{
				//printf("Piece in the way at %d %d \n",colSource,i);
				//printf("Piece type is \"%c\" \"%c\" \n", GetColor(getPiece(myBoard,i,rowSource)),GetType(getPiece(myBoard,i,rowSource)) );
				return 0;
			}
		}
		return 1;//if there were all empty spaces then the path is clear and its legal
	}
	else 
	{
		return 0;
	}
}

// checks that bishop's destination is any space diagonal to it; checks for enemy pieces at its destination
int isLegalBishop(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor)
{
	int directionToGoX;
	int directionToGoY;
	if(abs(rowSource-rowDestination) == abs(colSource - colDestination))//checking to see if the diagonal is proportional, 1:1
	{
		directionToGoX = colDestination-colSource > 0 ? 1 : -1;//direction the X axis moves, or by column
		directionToGoY = rowDestination-rowSource > 0 ? 1 : -1;//direction the Y axis moves, or by row
		int j = rowSource+directionToGoY;
		for(int i = colSource+directionToGoX; i != colDestination; i+= directionToGoX)//starts at one square towards destination and repeatedly checks for empty spaces between then and here
		{
			if(!isEmpty(myBoard,i,j))
			{
				return 0;
			}
			j+=directionToGoY;
		}
		return 1;//if there were all empty spaces then the path is clear and its legal
	}

	else 
	{
		return 0;
	}
}

// checks that queen's destination is any space in any direction; checks for enemy pieces at its destination
int isLegalQueen(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor)//this function is just a combination of rook and bishop
{
	int directionToGo; //the straight move direction check
	int directionToGoX; //x and y directional checks
	int directionToGoY;
	if(rowSource == rowDestination)
	{
		directionToGo = colDestination-colSource > 0 ? 1 : -1;//if destination is greater than source, we need to go forward and if its less than or behind source, we go backwards
		for(int i = colSource+directionToGo; i != colDestination; i+= directionToGo)//starts at one square towards destination and repeatedly checks for empty spaces between then and here
		{
			if(!isEmpty(myBoard,i,rowSource))
			{
				return 0;
			}
		}
		return 1;//if there were all empty spaces then the path is clear and its legal
	}
	else if (colSource == colDestination)
	{
		directionToGo = rowDestination-rowSource > 0 ? 1 : -1;//if destination is greater than source, we need to go forward and if its less than or behind source, we go backwards
		for(int i = rowSource+directionToGo; i != rowDestination; i+= directionToGo)//starts at one square towards destination and repeatedly checks for empty spaces between then and here
		{
			if(!isEmpty(myBoard,colSource,i))
			{
				return 0;
			}
		}
		return 1;//if there were all empty spaces then the path is clear and its legal
	}
	if(abs(rowSource-rowDestination) == abs(colSource - colDestination))//checking to see if the diagonal is proportional, 1:1
	{
		directionToGoX = colDestination-colSource > 0 ? 1 : -1;//direction the X axis moves, or by column
		directionToGoY = rowDestination-rowSource > 0 ? 1 : -1;//direction the Y axis moves, or by row
		int j = rowSource+directionToGoY;
		for(int i = colSource+directionToGoX; i != colDestination; i+= directionToGoX)//starts at one square towards destination and repeatedly checks for empty spaces between then and here
		{
			if(!isEmpty(myBoard,i,j))
			{
				return 0;
			}
			j+=directionToGoY;
		}
		return 1;//if there were all empty spaces then the path is clear and its legal
	}
	else 
	{
		return 0;
	}
}

// checks that king's destination is one space in any direction; supports castling
int isLegalKing(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor)
{
	if(GetColor(getPiece(myBoard,colDestination,rowDestination)) == curTurnColor && GetType(getPiece(myBoard,colDestination,rowDestination)) == 'R')//if other piece is a friendly rook
	{
		if(curTurnColor == 'w')
		{
			if(colSource == 4 && rowSource == 0 && rowDestination == 0 && (colDestination == 0 || colDestination == 7))//king and rook has to be in a hardcoded starting position
			{
				int forward = (colDestination-colSource) > 0 ? 1 : -1;
				for(int i = colSource + forward; i != colDestination; i+=forward)
				{
					if(!isEmpty(myBoard,i,rowSource)) //if theres any pieces between the king and rook then it cant castle
					{
						return 0;
					}
				}
				return 1;//if it made it past all the checks then it is legal move
			}
			else
			{
				return 0;
			}
		}
		else if(curTurnColor == 'b')
		{
			if(colSource == 4 && rowSource == 7 && rowDestination == 7 && (colDestination == 0 || colDestination == 7))
			{
				int forward = (colDestination-colSource) > 0 ? 1 : -1;
				for(int i = colSource + forward; i != colDestination; i+=forward)
				{
					if(!isEmpty(myBoard,i,rowSource)) //if theres any pieces between the king and rook then it cant castle
					{
						return 0;
					}
				}
				return 1;//if it made it past all the checks then it is legal move
			}
			else
			{
				return 0;
			}
			
		}
		else
		{
			//printf("Invalid color \n");
			return 0;
		}
	}
	else if(abs(rowDestination-rowSource) <= 1 && abs(colDestination-colSource) <=1 )//ensures the king is only moving in a single square around it
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// determines if move is legal and then executes it
int MakeMove(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor, MLIST *myList)
{
	int isLeg = isLegal(myBoard,colSource,rowSource,colDestination,rowDestination,curTurnColor, myList);	
	if(isLeg == 1)
	{
		PIECE *removedPiece;	
		PIECE tempMoved = getCopy(myBoard, colDestination, rowDestination);
		PIECE *movedPiece = &tempMoved;

		if(GetType(getPiece(myBoard,colSource,rowSource)) == 'K' && GetType(getPiece(myBoard,colDestination,rowDestination)) == 'R' && GetColor(getPiece(myBoard,colDestination,rowDestination)) == curTurnColor)//checking for castling
		{
			//printf("Made it here\n");
			int directionToGo = (colDestination-colSource) > 0 ? 1 : -1;
			removedPiece = movePiece(myBoard,colSource, rowSource, colSource + directionToGo*2, rowDestination);//moving king 2 spaces towards rook. The removedPiece should be a blank square
			movePiece(myBoard,colDestination,rowDestination,colSource+directionToGo,rowDestination);//moving rook to space right next to the kin
			AppendMove(myList, movedPiece, removedPiece, colSource, rowSource, colDestination, rowDestination);
			return 1;
		}
		else if(GetType(getPiece(myBoard,colSource,rowSource)) == 'P')
		{
			int enPassant = 0;  //variable to track whether we did en passant or not
			int forward = curTurnColor == 'w' ? 1 : -1;
			char enemyColor = curTurnColor=='w' ? 'b' : 'w';
			//REPEATING CODE FROM ISLEGALPAWN FUNCTION. THIS IS USED TO DETERMINE IF WE DO THE EXTRA STEP OF REMOVING THE PAWN AS PER EN PASSANT
			if(isEmpty(myBoard,colDestination,rowDestination) )//if the spot is empty
			{
				if(abs(colSource-colDestination) == 1 && (forward*(rowDestination-rowSource)) == 1 )//checking for en passant, this covers a diagonal move
				{
					if(GetType(getPiece(myBoard,colDestination,rowDestination-forward)) == 'P' && GetColor(getPiece(myBoard,colDestination,rowDestination-forward)) == enemyColor)//if the piece right behind destination is an enemy pawn we can capture it
					{
						if(myList->last->move->source[0] == ('A' + colDestination) && myList->last->move->source[1] == ('1'+rowDestination+forward))//checking if enemypawn was previously in space in front of the destination square. If it is then the move is en passant and is legal
						{
							movePiece(myBoard,colSource,rowSource,colDestination,rowDestination);
							removedPiece = movePiece(myBoard,colSource, rowSource, colDestination,rowDestination-forward);//remove the piece right behind the destination square to capture en passant
							//MOVE *newMove = NewMove(getPiece(myBoard,colSource,rowSource),removedPawn), 
							enPassant = 1;

						}
						else
						{
							//printf("No en passant, list contents: %c, %c\n", myList->last->move->source[0],myList->last->move->source[1]);
						}
					}
				}
			}
			if(enPassant != 1)//if en passant didnt happen
			{
				removedPiece = movePiece(myBoard,colSource,rowSource,colDestination,rowDestination);
			}

		}
		else
		{
			removedPiece = movePiece(myBoard,colSource,rowSource,colDestination,rowDestination);

		}
		AppendMove(myList,movedPiece,removedPiece, colSource, rowSource, colDestination, rowDestination);
		return 1;
	}
	else if (isLeg == 0)
	{
		return 0;
	}
	else
	{
		//printf("Uknown error\n");
		return 0;
	}
	//return 0;
}


// used to makemove on the board but without appending to myList
int MakeMoveNoAppend(PIECE** myBoard, int colSource, int rowSource, int colDestination, int rowDestination, char curTurnColor, MLIST *myList)
{
	int isLeg = isLegal(myBoard,colSource,rowSource,colDestination,rowDestination,curTurnColor, myList);
	
	if(isLeg == 1)
	{
		PIECE *removedPiece;//tracking removed piece
		//PIECE *movedPiece = getPiece(myBoard,colSource,rowSource);

		if(GetType(getPiece(myBoard,colSource,rowSource)) == 'K' && GetType(getPiece(myBoard,colDestination,rowDestination)) == 'R' && GetColor(getPiece(myBoard,colDestination,rowDestination)) == curTurnColor)//checking for castling
		{
			int directionToGo = (colDestination-colSource) > 0 ? 1 : -1;
			removedPiece = movePiece(myBoard,colSource, rowSource, colSource + directionToGo*2, rowDestination);//moving king 2 spaces towards rook. The removedPiece should be a blank square
			movePiece(myBoard,colDestination,rowDestination,colSource+directionToGo,rowDestination);//moving rook to space right next to the kin
			
			return 1;
		}
		else if(GetType(getPiece(myBoard,colSource,rowSource)) == 'P')
		{
			int enPassant = 0;  //variable to track whether we did en passant or not
			int forward = curTurnColor == 'w' ? 1 : -1;
			char enemyColor = curTurnColor=='w' ? 'b' : 'w';
			//REPEATING CODE FROM ISLEGALPAWN FUNCTION. THIS IS USED TO DETERMINE IF WE DO THE EXTRA STEP OF REMOVING THE PAWN AS PER EN PASSANT
			if(isEmpty(myBoard,colDestination,rowDestination) )//if the spot is empty
			{
				if(abs(colSource-colDestination) == 1 && (forward*(rowDestination-rowSource)) == 1 )//checking for en passant, this covers a diagonal move
				{
					if(GetType(getPiece(myBoard,colDestination,rowDestination-forward)) == 'P' && GetColor(getPiece(myBoard,colDestination,rowDestination-forward)) == enemyColor)//if the piece right behind destination is an enemy pawn we can capture it
					{
						if(myList->last->move->source[0] == ('A' + colDestination) && myList->last->move->source[1] == ('1'+rowDestination+forward))//checking if enemypawn was previously in space in front of the destination square. If it is then the move is en passant and is legal
						{
							movePiece(myBoard,colSource,rowSource,colDestination,rowDestination);
							removedPiece = movePiece(myBoard,colSource, rowSource, colDestination,rowDestination-forward);//remove the piece right behind the destination square to capture en passant
							//MOVE *newMove = NewMove(getPiece(myBoard,colSource,rowSource),removedPawn), 
							enPassant = 1;

						}
						else
						{
							//printf("No en passant, list contents: %c, %c\n", myList->last->move->source[0],myList->last->move->source[1]);
						}
					}
				}
			}
			if(enPassant != 1)//if en passant didnt happen
			{
				removedPiece = movePiece(myBoard,colSource,rowSource,colDestination,rowDestination);
			}

		}
		else
		{
			removedPiece = movePiece(myBoard,colSource,rowSource,colDestination,rowDestination);

		}
		
		return 1;
	}
	else if (isLeg == 0)
	{
		return 0;
	}
	else
	{
		//printf("Uknown error\n");
		return 0;
	}
	//return 0;
}


// wrapper function that calls the other ischecked functions much like how the isLegal function works. 
int isChecked(PIECE **myBoard, char curTurnColor){
	int found = 0;
	int colKing;
	int rowKing;
	char enemyColor = (curTurnColor == 'w') ? 'b' : 'w';//swapping color
	for(int i =0; i < 8 && !found; i++)//finding coordinates for the king
	{
		for(int j = 0; j< 8 && !found; j++)
		{
			if(GetType(getPiece(myBoard,i,j)) == 'K'  && GetColor(getPiece(myBoard,i,j)) == curTurnColor)
			{
				found =1;
				colKing = i;
				rowKing = j;
			}
		}
	}
	
	//Returns location of piece that is checking the king
	int RQ = isCheckedByRQ(myBoard, enemyColor, colKing, rowKing);
	if(RQ)
	{
		return RQ;
	}
	int BQ = isCheckedByBQ(myBoard, enemyColor, colKing, rowKing);
	if(BQ)
	{
		return BQ;
	}
	int N  = isCheckedByN(myBoard, enemyColor, colKing, rowKing);
	if(N)
	{
		return N;
	}
	int P  = isCheckedByP(myBoard, enemyColor, colKing, rowKing);
	if(P)
	{
		return P;
	}
	int K  = isCheckedByK(myBoard, enemyColor, colKing, rowKing);
	if(K)
	{
		return K;
	}

	return 0;//if its not checked at all return 0

}

// checks for rooks or queens horizontal or vertical of the piece at colKing and rowKing
int isCheckedByRQ(PIECE **myBoard, char enemyColor, int colKing, int rowKing)//checks if the king at colKing is checked by a rook or queen
{

	//HORIZONTAL ROW CHECKS FOR ROOKS AND QUEENS. CHECKS ALL PIECES UP DOWN LEFT AND RIGHT
	for(int i = colKing+1; i < 8; i++)//loop to check all columns towards 7 edge. The presence of any rook, or queen, makes a check
	{
		if(!isEmpty(myBoard,i,rowKing))//Checking for an empty piece. Whatever the empty piece is, the loop ends immediately because its either a rook or queen, or it doesnt matter.
		{
			if(GetColor(getPiece(myBoard,i,rowKing)) == enemyColor && (GetType(getPiece(myBoard,i,rowKing)) == 'Q' || GetType(getPiece(myBoard,i,rowKing)) == 'R') )//checks for an enemy and simultaneously checks if its non-empty
			{
				//printf("Checked at %d %d by a queen or rook\n", i, rowKing);
				return (i*10+rowKing);//this implies that the rook or king is in line with the king and therefore checking it
			}
			else 
			{
				break;//if the piece isnt an enemy rook or queen then it doesnt matter what is behind it and we can move onto the next direction to check
			}

		}
	}

	for(int i = colKing-1; i >= 0; i--)//loop to check all columns towards 0 edge. The presence of any rook, or queen, makes a check
	{
		if(!isEmpty(myBoard,i,rowKing))//Checking for an empty piece
		{
			if(GetColor(getPiece(myBoard,i,rowKing)) == enemyColor && (GetType(getPiece(myBoard,i,rowKing)) == 'Q' || GetType(getPiece(myBoard,i,rowKing)) == 'R') )//checks for an enemy and simultaneously checks if its non-empty
			{
				//printf("Checked at %d %d by a queen or rook\n", i, rowKing);
				return (i*10 + rowKing);//this implies that the rook or king is in line with the king and therefore checking it
			}
			else 
			{
				break;//if the piece isnt an enemy rook or queen then it doesnt matter what is behind it and we can move onto the next direction to check
			}

		}
	}

	for(int j = rowKing+1; j < 8; j++)//loop to check all rows towards 7 edge. The presence of any rook, or queen, makes a check
	{
		if(!isEmpty(myBoard,colKing,j))//Checking for an empty piece
		{
			if(GetColor(getPiece(myBoard,colKing,j)) == enemyColor && (GetType(getPiece(myBoard,colKing,j)) == 'Q' || GetType(getPiece(myBoard,colKing,j)) == 'R') )//checks for an enemy and simultaneously checks if its non-empty
			{
				//printf("Checked at %d %d by a queen or rook\n", colKing, j);
				return (colKing*10 + j);//this implies that the rook or king is in line with the king and therefore checking it
			}
			else 
			{
				break;//if the piece isnt an enemy rook or queen then it doesnt matter what is behind it and we can move onto the next direction to check
			}

		}
	}

	for(int j = rowKing-1; j >= 0; j--)//loop to check all rows towards 0 edge. The presence of any rook, or queen, makes a check
	{
		if(!isEmpty(myBoard,colKing,j))//Checking for an empty piece
		{
			if(GetColor(getPiece(myBoard,colKing,j)) == enemyColor && (GetType(getPiece(myBoard,colKing,j)) == 'Q' || GetType(getPiece(myBoard,colKing,j)) == 'R') )//checks for an enemy and simultaneously checks if its non-empty
			{
				//printf("Checked at %d %d by a queen or rook\n", colKing, j);
				return (colKing*10 + j);//this implies that the rook or king is in line with the king and therefore checking it
			}
			else 
			{
				break;//if the piece isnt an enemy rook or queen then it doesnt matter what is behind it and we can move onto the next direction to check
			}
		}
	}
	return 0;

}

// checks for bishops or queens diagonal in all directions of the piece at colKing and rowKing
int isCheckedByBQ(PIECE **myBoard, char enemyColor, int colKing, int rowKing)
{
	int i;
	int j;
	for(i = colKing+1, j = rowKing +1; i<8 && j<8; i++,j++)
	{
		if(!isEmpty(myBoard,i,j))
		{
			if(GetColor(getPiece(myBoard,i,j)) == enemyColor && (GetType(getPiece(myBoard,i,j)) == 'Q' ||GetType(getPiece(myBoard,i,j)) == 'B' ))
			{
				//printf("Checked at %d %d by a bishop or queen\n",i,j);
				return (i*10+j);
			}
			else
			{
				break;
			}
		}
	}
	for(i = colKing+1,  j = rowKing - 1; i<8 && j>=0; i++,j--)
	{
		if(!isEmpty(myBoard,i,j))
		{
			if(GetColor(getPiece(myBoard,i,j)) == enemyColor && (GetType(getPiece(myBoard,i,j)) == 'Q' ||GetType(getPiece(myBoard,i,j)) == 'B' ))
			{
				//printf("Checked at %d %d by a bishop or queen\n",i,j);
				return (i*10+j);
			}
			else
			{
				break;
			}
		}
	}
	for(i = colKing-1, j = rowKing +1; i>=0 && j<8; i--,j++)
	{
		if(!isEmpty(myBoard,i,j))
		{
			if(GetColor(getPiece(myBoard,i,j)) == enemyColor && (GetType(getPiece(myBoard,i,j)) == 'Q' ||GetType(getPiece(myBoard,i,j)) == 'B' ))
			{
				//printf("Checked at %d %d by a bishop or queen\n",i,j);
				return (i*10+j);
			}
			else
			{
				break;
			}
		}
	}
	for(i = colKing-1, j = rowKing - 1; i>=0 && j>=0; i--,j--)
	{
		if(!isEmpty(myBoard,i,j))
		{
			if(GetColor(getPiece(myBoard,i,j)) == enemyColor && (GetType(getPiece(myBoard,i,j)) == 'Q' ||GetType(getPiece(myBoard,i,j)) == 'B' ))
			{
				//printf("Checked at %d %d by a bishop or queen\n",i,j);
				return (i*10+j);
			}
			else
			{
				break;
			}
		}
	}
	return 0;//if we found nothing then we return 0
}

// checks for knights that can check piece at colKing and rowKing
int isCheckedByN(PIECE **myBoard, char enemyColor, int colKing, int rowKing)//checks for nights
{
	if(colKing+2<8 && rowKing+1<8)
	{
		if(GetColor(getPiece(myBoard,colKing+2,rowKing+1)) == enemyColor && GetType(getPiece(myBoard,colKing+2,rowKing+1)) == 'N')
		{
			return ((colKing+2)*10 + rowKing+1);
		}
	}
	if(colKing+1<8 && rowKing+2<8)
	{
		if(GetColor(getPiece(myBoard,colKing+1,rowKing+2)) == enemyColor && GetType(getPiece(myBoard,colKing+1,rowKing+2)) == 'N')
		{
			return ((colKing+1)*10 + rowKing+2);
		}
	}
	if(colKing-1>=0 && rowKing+2<8)
	{
		if(GetColor(getPiece(myBoard,colKing-1,rowKing+2)) == enemyColor && GetType(getPiece(myBoard,colKing-1,rowKing+2)) == 'N')
		{
			return ((colKing-1)*10 + rowKing+2);
		}
	}
	if(colKing+2<8 && rowKing-1>=0)
	{
		if(GetColor(getPiece(myBoard,colKing+2,rowKing-1)) == enemyColor && GetType(getPiece(myBoard,colKing+2,rowKing-1)) == 'N')
		{
			return ((colKing+2)*10 + rowKing-1);
		}
	}
	if(colKing+1<8 && rowKing-2>=0)
	{
		if(GetColor(getPiece(myBoard,colKing+1,rowKing-2)) == enemyColor && GetType(getPiece(myBoard,colKing+1,rowKing-2)) == 'N')
		{
			return ((colKing+1)*10 + rowKing-2);
		}
	}
	if(colKing-2>=0 && rowKing+1<8)
	{
		if(GetColor(getPiece(myBoard,colKing-2,rowKing+1)) == enemyColor && GetType(getPiece(myBoard,colKing-2,rowKing+1)) == 'N')
		{
			return ((colKing-2)*10 + rowKing+1);
		}
	}
	if(colKing-1>=0 && rowKing-2>=0)
	{
		if(GetColor(getPiece(myBoard,colKing-1,rowKing-2)) == enemyColor && GetType(getPiece(myBoard,colKing-1,rowKing-2)) == 'N')
		{
			return ((colKing-1)*10 + rowKing-2);

		}
	}
	if(colKing-2>=0 && rowKing-1>=0)
	{
		if(GetColor(getPiece(myBoard,colKing-2,rowKing-1)) == enemyColor && GetType(getPiece(myBoard,colKing-2,rowKing-1)) == 'N')
		{
			return ((colKing-2)*10 + rowKing-1);
		}
	}
	return 0;
}

// checks for pawns that can check piece at colKing and rowKing
int isCheckedByP(PIECE **myBoard,char enemyColor, int colKing, int rowKing)
{
	int forward = (enemyColor == 'w' ? 1 : -1);//determines the forward direction for the enemy piece
	//a pawn can only check a king diagonally in one direction so this helps keep things simple.
	if(colKing+1 < 8 && GetType(getPiece(myBoard,colKing+1,rowKing - forward)) == 'P' && GetColor(getPiece(myBoard,colKing+1,rowKing-forward)) == enemyColor)
	{
		//printf("Checked at %d %d by Pawn\n",colKing+1,rowKing);
		return ((colKing+1)*10 + rowKing);
	}
	if(colKing-1 >= 0 && GetType(getPiece(myBoard,colKing-1,rowKing - forward)) == 'P' && GetColor(getPiece(myBoard,colKing+1,rowKing-forward)) == enemyColor)
	{
		//printf("Checked at %d %d by Pawn\n",colKing-1,rowKing);
		return ((colKing-1)*10 + rowKing);
	}
	return 0;
}

// checks for kings that can check piece at colKing and rowKing
int isCheckedByK(PIECE **myBoard,char enemyColor, int colKing, int rowKing)//checking all legal spaces around king to see if its another king
{
	if(colKing-1 >= 0 && rowKing-1 >= 0)
	{
		if(GetType(getPiece(myBoard,colKing-1,rowKing-1)) == 'K' && GetColor(getPiece(myBoard,colKing-1,rowKing-1)) == enemyColor)
		{
			return ((colKing-1)*10 + rowKing-1);
		}
	}
	if(colKing-1 >= 0 && rowKing+1 < 8)
	{
		if(GetType(getPiece(myBoard,colKing-1,rowKing+1)) == 'K' && GetColor(getPiece(myBoard,colKing-1,rowKing+1)) == enemyColor)
		{
			return ((colKing-1)*10 + rowKing+1);
		}
	}
	if(colKing+1 < 8 && rowKing-1 >= 0)
	{
		if(GetType(getPiece(myBoard,colKing+1,rowKing-1)) == 'K' && GetColor(getPiece(myBoard,colKing+1,rowKing-1)) == enemyColor)
		{
			return ((colKing+1)*10 + rowKing-1);
		}
	}
	if(colKing+1 < 8 && rowKing+1 < 8)
	{
		if(GetType(getPiece(myBoard,colKing+1,rowKing+1)) == 'K' && GetColor(getPiece(myBoard,colKing+1,rowKing+1)) == enemyColor)
		{
			return ((colKing+1)*10 + rowKing+1);
		}
	}
	if(rowKing-1 >= 0)
	{
		if(GetType(getPiece(myBoard,colKing,rowKing-1)) == 'K' && GetColor(getPiece(myBoard,colKing,rowKing-1)) == enemyColor)
		{
			return ((colKing)*10 + rowKing-1);
		}
	}
	if(rowKing+1 < 8)
	{
		if(GetType(getPiece(myBoard,colKing,rowKing+1)) == 'K' && GetColor(getPiece(myBoard,colKing,rowKing+1)) == enemyColor)
		{
			return ((colKing)*10 + rowKing+1);
		}
	}
	if(colKing-1 >= 0)
	{
		if(GetType(getPiece(myBoard,colKing-1,rowKing)) == 'K' && GetColor(getPiece(myBoard,colKing-1,rowKing)) == enemyColor)
		{
			return ((colKing-1)*10 + rowKing);
		}
	}
	if(colKing+1 < 8)
	{
		if(GetType(getPiece(myBoard,colKing+1,rowKing)) == 'K' && GetColor(getPiece(myBoard,colKing+1,rowKing)) == enemyColor)
		{
			return ((colKing+1)*10 + rowKing);
		}
	}
	return 0;
}

// returns 1 if isCheckmate
int isCheckmate(PIECE **myBoard, char curTurnColor, MLIST *myList)//0 indicates no checks, 1 indicates a check, 2 indicates a checkmate
{
	//char enemyColor = (curTurnColor== 'w' ? 'b' : 'w');
	/*if(!isChecked(myBoard,curTurnColor))//if theres no check at all then it cant be a checkmate and it returns a 1
	{
		return 0;	//indicates no check
	}*/
	PIECE **tempBoard = copyBoard(myBoard);
	for(int i = 0 ; i < 8; i++)//nested loop that goes through entire board, looking for friendly pieces that could potentially end check
	{
		for(int j = 0; j < 8; j++)
		{
			if(GetColor(getPiece(tempBoard,i,j)) != curTurnColor)//if its an enemy piece we cant manipulate it to do things so we skip it
			{
				continue;
			}
			switch(GetType(getPiece(tempBoard,i,j)))
			{
				case 'K':
				{
					if(MakeMoveNoAppend(tempBoard,i,j,i-4,j,curTurnColor,myList))//see if we can castle our way out of check
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);				//current way to reset board. Not very efficient of course, but can replace with the undo function once implemented
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i+3,j,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);				//current way to reset board. Not very efficient of course, but can replace with the undo function once implemented
							tempBoard = copyBoard(myBoard);
						}
					}
					for(int p = -1; p <=1; p++)//checking if we can move to any of the adjacent squares as the king and if we can, if we're still in check after moving. If we are then continue, but if we're out of check then return 0
					{
						for(int o = -1; o <=1; o++)
						{
							if(o !=0 || p != 0)
							{
								if(MakeMoveNoAppend(tempBoard,i,j,i+p,j+o,curTurnColor,myList))
								{
									if(!isChecked(tempBoard,curTurnColor))
									{
										deleteBoard(tempBoard);
										return 0;
									}
									else
									{
										deleteBoard(tempBoard);
										tempBoard = copyBoard(myBoard);
									}
								}
							}
						}
					}
					break;
				}
				case 'R':
				{
					for(int p = 0; p < 8; p++)
					{
						if(p != i)///checking all adjacent columns on the current row j
						{
							if(MakeMoveNoAppend(tempBoard,i, j, p, j,curTurnColor,myList))
							{
									if(!isChecked(tempBoard,curTurnColor))
									{
										deleteBoard(tempBoard);
										return 0;
									}
									else
									{
										deleteBoard(tempBoard);
										tempBoard = copyBoard(myBoard);
									}
							}
						}
						if(p != j)
						{
							if(MakeMoveNoAppend(tempBoard,i,j,i,p,curTurnColor,myList))
							{
								if(!isChecked(tempBoard,curTurnColor))
									{
										deleteBoard(tempBoard);
										return 0;
									}
									else
									{
										deleteBoard(tempBoard);
										tempBoard = copyBoard(myBoard);
									}
							}
						}
					}
					break;
				}
				case 'P':
				{
					int forward = curTurnColor == 'w' ? 1 : -1;

					if(MakeMoveNoAppend(tempBoard,i,j,i+1,j+forward,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i-1,j+forward,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i,j + forward,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i,j + 2*forward,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}

					break;
				}
				case 'B':
				{
					int startingCol = i<j ? 0 : i-j;
					int startingRow = i<j ? j-i : 0;
					int r = startingCol;
					int p;
					for(p = startingRow; p < 8 && r < 8; p++)
					{
						if(MakeMoveNoAppend(tempBoard,i,j,r,p,curTurnColor,myList))
						{
							if(!isChecked(tempBoard,curTurnColor))
							{
								deleteBoard(tempBoard);
								return 0;
							}
							else
							{
								deleteBoard(tempBoard);
								tempBoard = copyBoard(myBoard);
							}
						}
						r++;
					}
					r = startingCol;
					for(p = startingRow; p >= 0 && r<8; p-- )
					{
						if(MakeMoveNoAppend(tempBoard,i,j,r,p,curTurnColor,myList))
						{
							if(!isChecked(tempBoard,curTurnColor))
									{
										deleteBoard(tempBoard);
										return 0;
									}
									else
									{
										deleteBoard(tempBoard);
										tempBoard = copyBoard(myBoard);
									}
						}
						r++;
					}
					r = startingCol;
					for(p = startingRow; p < 8 && r>=0; p++ )
					{
						if(MakeMoveNoAppend(tempBoard,i,j,r,p,curTurnColor,myList))
						{
							if(!isChecked(tempBoard,curTurnColor))
							{
								deleteBoard(tempBoard);
								return 0;
							}
							else
							{
								deleteBoard(tempBoard);
								tempBoard = copyBoard(myBoard);
							}
						}
						r--;
					}
					break;
				}
				case 'N':
				{
					if(MakeMoveNoAppend(tempBoard,i,j,i+2,j+1,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i+1,j+2,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i-2,j+1,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i+1,j-2,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i+2,j-1,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i-1,j+2,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i-2,j-1,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					if(MakeMoveNoAppend(tempBoard,i,j,i-1,j-2,curTurnColor,myList))
					{
						if(!isChecked(tempBoard,curTurnColor))
						{
							deleteBoard(tempBoard);
							return 0;
						}
						else
						{
							deleteBoard(tempBoard);
							tempBoard = copyBoard(myBoard);
						}
					}
					break;
				}
				case 'Q'://just repeating code from bishop and rook
				{
					int startingCol = i<j ? 0 : i-j;
					int startingRow = i<j ? j-i : 0;
					int r = startingCol;
					int p;
					for(p = startingRow; p < 8 && r < 8; p++)
					{
						if(MakeMoveNoAppend(tempBoard,i,j,r,p,curTurnColor,myList))
						{
							if(!isChecked(tempBoard,curTurnColor))
									{
										deleteBoard(tempBoard);
										return 0;
									}
									else
									{
										deleteBoard(tempBoard);
										tempBoard = copyBoard(myBoard);
									}
						}
						r++;
					}
					r = startingCol;
					for(p = startingRow; p >= 0 && r<8; p-- )
					{
						if(MakeMoveNoAppend(tempBoard,i,j,r,p,curTurnColor,myList))
						{
							if(!isChecked(tempBoard,curTurnColor))
									{
										deleteBoard(tempBoard);
										return 0;
									}
									else
									{
										deleteBoard(tempBoard);
										tempBoard = copyBoard(myBoard);
									}
						}
						r++;
					}
					r = startingCol;
					for(p = startingRow; p < 8 && r>=0; p++ )
					{
						if(MakeMoveNoAppend(tempBoard,i,j,r,p,curTurnColor,myList))
						{
							if(!isChecked(tempBoard,curTurnColor))
							{
								deleteBoard(tempBoard);
								return 0;
							}
							else
							{
								deleteBoard(tempBoard);
								tempBoard = copyBoard(myBoard);
							}
						}
						r--;
					}

					for(int q = 0; q < 8; q++)
					{
						if(q != i)///checking all adjacent columns on the current row j
						{
							if(MakeMoveNoAppend(tempBoard,i, j, q, j,curTurnColor,myList))
							{
									if(!isChecked(tempBoard,curTurnColor))
									{
										deleteBoard(tempBoard);
										return 0;
									}
									else
									{
										deleteBoard(tempBoard);
										tempBoard = copyBoard(myBoard);
									}
							}
						}
						if(q != j)
						{
							if(MakeMoveNoAppend(tempBoard,i,j,i,q,curTurnColor,myList))
							{
								if(!isChecked(tempBoard,curTurnColor))
									{
										deleteBoard(tempBoard);
										return 0;
									}
									else
									{
										deleteBoard(tempBoard);
										tempBoard = copyBoard(myBoard);
									}
							}
						}
					}
					break;
				}
			}
		}
	}


	return 1;
}





/*
int isCheckedPiece(PIECE **myBoard, char curTurnColor, int colPiece, int rowPiece){//identical to the isChecked function but it doesnt check for checks around the king of curTurnColor. 
																	//Instead it checks to see if a specific spot or piece is checked, regardless of whether its a king or not
	char enemyColor = (curTurnColor == 'w') ? 'b' : 'w';//swapping color
	
	//Returns location of piece that is checking the king
	int RQ = isCheckedByRQ(myBoard, enemyColor, colPiece, rowPiece);
	if(RQ)
	{
		return RQ;
	}
	int BQ = isCheckedByBQ(myBoard, enemyColor, colPiece, rowPiece);
	if(BQ)
	{
		return BQ;
	}
	int N  = isCheckedByN(myBoard, enemyColor, colPiece, rowPiece);
	if(N)
	{
		return N;
	}
	int P  = isCheckedByP(myBoard, enemyColor, colPiece, rowPiece);
	if(p)
	{
		return P;
	}
	int K  = isCheckedByK(myBoard, enemyColor, colPiece, rowPiece);
	if(K)
	{
		return K;
	}

	return 0;//if its not checked at all return 0

}


*/

// EOF //
