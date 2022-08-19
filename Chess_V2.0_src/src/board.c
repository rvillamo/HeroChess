/************************************************************************/
/* 		HeroChess V1.0 by The Avengineers			*/
/* 		EECS 22L - University of California, Irvine		*/
/*									*/
/* board.c: contains definitions of structs and functions pertaining to	*/
/*				the board				*/
/*									*/
/* Modifications:							*/
/* Date (YYYY/MM/DD)	Author			Description		*/
/* 2021/04/17		Paul Lee		Initial Version		*/
/************************************************************************/

#include "board.h"

// initializes the game board
PIECE **makeBoard(){
	int i = 0;
	PIECE **board;
	board = (PIECE **)malloc(sizeof(PIECE*) * size);
	for(i = 0; i<size; i++){
		board[i] = (PIECE *)malloc(sizeof(PIECE) * size);
	}
	return board;
}

// deletes the board, freeing the memory and deallocating the array
void deleteBoard(PIECE **myBoard){
	assert(myBoard);		
	free(myBoard);
}

// returns copy of board 
PIECE **copyBoard(PIECE **myBoard){
	int i = 0, j = 0;
	PIECE **newBoard = makeBoard();
	for(i = 0; i<size; i++){
	    for(j = 0; j < size; j++){
			newBoard[i][j] = myBoard[i][j];
        }
    }
	return newBoard;
}

// returns piece at position colrow 
PIECE *getPiece(PIECE **myBoard, int col, int row){
	
	PIECE *p = &myBoard[col][row];
	return p;
}

// returns copy of piece
PIECE getCopy(PIECE **myBoard, int col, int row){
	PIECE p = myBoard[col][row];
	return p;
}

// places the piece specified at colrow, returning 0 if successful and 1 if not successful
void placePiece(PIECE **myBoard, PIECE *p, int col, int row){
	myBoard[col][row] = *p;	
}

// removes and returns piece from specified colrow and fills the spot with null; returns null if empty
PIECE removePiece(PIECE **myBoard, int col, int row){
	PIECE p = myBoard[col][row];	
	myBoard[col][row] = makeBlank();
	return p;
}

// returns colrow destination from piece being moved and makes source blank
PIECE *movePiece(PIECE** myBoard, int colSource, int rowSource, int colDestination,int rowDestination){
	PIECE *p = NULL;
	p = &myBoard[colDestination][rowDestination];
	//printf("%c%c\n", GetColor(&myBoard[colDestination][rowDestination]), GetType(&myBoard[colDestination][rowDestination]));
	PIECE *p1 = getPiece(myBoard, colSource, rowSource); //added
	myBoard[colDestination][rowDestination] = *p1;
	//myBoard[colDestination][rowDestination] = myBoard[colSource][rowSource];
	myBoard[colSource][rowSource] = makeBlank();	
	return p;
}

// prints board to terminal
void printBoard(PIECE** myBoard){
	int i = 0, j = 0;
	printf("   ----------------------------------------\n");
	for(i = size-1; i >= 0; i--){
		printf("%d", i+1);
		for(j = 0; j < size; j++){	
			printf(" | %c%c", GetColor(&myBoard[j][i]), GetType(&myBoard[j][i]));
		}
		printf(" |\n");
		printf("   ----------------------------------------\n");

	}	
	printf("     a    b    c    d    e    f    g    h\n");
}

// prints board to replay text file 
void printReplayBoard(PIECE** myBoard, FILE *fptr){
	int i = 0, j = 0;
	fprintf(fptr, "   ----------------------------------------\n");
	for(i = size-1; i >= 0; i--){
		fprintf(fptr, "%d", i+1);
		for(j = 0; j < size; j++){	
			fprintf(fptr, " | %c%c", GetColor(&myBoard[j][i]), GetType(&myBoard[j][i]));
		}
		fprintf(fptr, " |\n");
		fprintf(fptr, "   ----------------------------------------\n");

	}	
	fprintf(fptr, "     a    b    c    d    e    f    g    h\n");
}

// places black and whites pieces to initailized board 
void initializeBoard(PIECE** myBoard){
	int i = 0, j = 0;
	PIECE *wRl = NewPiece('R', 'w');
	PIECE *wRr = NewPiece('R', 'w');
	PIECE *wNl = NewPiece('N', 'w');
	PIECE *wNr = NewPiece('N', 'w');
	PIECE *wBl = NewPiece('B', 'w');
	PIECE *wBr = NewPiece('B', 'w');
	PIECE *wQ = NewPiece('Q', 'w');
	PIECE *wK = NewPiece('K', 'w');

	placePiece(myBoard, wRl, 0, 0);
	placePiece(myBoard, wRr, 7, 0);
	placePiece(myBoard, wNl, 1, 0);
	placePiece(myBoard, wNr, 6, 0);
	placePiece(myBoard, wBl, 2, 0);
	placePiece(myBoard, wBr, 5, 0);
	placePiece(myBoard, wK,  4, 0);
	placePiece(myBoard, wQ,  3, 0);

	PIECE *bRl = NewPiece('R', 'b');
	PIECE *bRr = NewPiece('R', 'b');
	PIECE *bNl = NewPiece('N', 'b');
	PIECE *bNr = NewPiece('N', 'b');
	PIECE *bBl = NewPiece('B', 'b');
	PIECE *bBr = NewPiece('B', 'b');
	PIECE *bQ = NewPiece('Q', 'b');
	PIECE *bK = NewPiece('K', 'b');

	placePiece(myBoard, bRl, 0, 7);
	placePiece(myBoard, bRr, 7, 7);
	placePiece(myBoard, bNl, 1, 7);
	placePiece(myBoard, bNr, 6, 7);
	placePiece(myBoard, bBl, 2, 7);
	placePiece(myBoard, bBr, 5, 7);
	placePiece(myBoard, bK,  4, 7);
	placePiece(myBoard, bQ,  3, 7);

	for(i = 0; i < size; i++){
		PIECE *wP = NewPiece('P', 'w');
		placePiece(myBoard, wP, i, 1);
	}
	for(i = 0; i < size; i++){
        PIECE *bP = NewPiece('P', 'b');
        placePiece(myBoard, bP, i, 6);
    }

	for(i = 2; i < size-2; i++){
		for(j = 0; j < size; j++){	
			myBoard[j][i] = makeBlank();	
		}
	}
}

// returns 1 if selected source is empty and 0 if it is not empty
int isEmpty(PIECE **myBoard, int col, int row){
	return (GetColor(&myBoard[col][row]) == ' ' && GetType(&myBoard[col][row]) == ' ');
}

// checks if pawn reached the opposite end of the board and asks user type of piece they want to promote to 
void checkForPromotion(PIECE **myBoard){
	int i;
	char c;
	for(i = 0; i < size; i++){
		if(GetColor(&myBoard[i][7]) == 'w' && GetType(&myBoard[i][7]) == 'P'){
			printf("Enter the piece type for the pawn two be promoted to: \n");
			c = getchar();
			SetType(&myBoard[i][7], c);
		}
	}
	for(i = 0; i < size; i++){
        if(GetColor(&myBoard[i][0]) == 'b' && GetType(&myBoard[i][0]) == 'P'){
        printf("Enter the piece type for the pawn two be promoted to: \n");
	        c = getchar();
            SetType(&myBoard[i][0], c);
        }
    }
}

// EOF //
