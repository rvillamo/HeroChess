/* main.c: file containing main function and print menu function */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "piece.h"
#include "board.h"
#include "game.h"
#include "movelist.h"
#include "settings.h"
#include "replay.h"
#include "ai.h"
#define INPUT_BUFFER 20
// print menu function declaration
int mainmenu(void);
int convertColumn(char a);
int convertRow(char b);
void undo(PIECE **board, MLIST *myList);
void gameTest();
// main function
int main(void){


	#ifdef DEBUG
    gameTest();
	#else
	PIECE **board = makeBoard();
	initializeBoard(board);
	MLIST *myList = NewMoveList();


				///MAIN PROGRAM: NON-DEBUG MODE
	char str[INPUT_BUFFER];//input buffer

	// welcome message prints once at the start of the first game
	printf("Welcome to HeroChess v1.0\n");

	// default settings: white - player 1, black - player 2
	int wp = 1;
	int bp = 2;

	// main menu keeps printing after each game until user decides to exit
	int exit = 0;
	while(exit == 0)
	{
		/* print board here? */
		int option = mainmenu();
		switch(option)
		{
			case 1:
			{
				/* HvH function*/
				/* print moves log? */
				int won = 1;
				char curTurnColor = 'w';

				while(won!=2)
				{
					printBoard(board);
					printf("To quit the game at any point, enter Q or q.\n");	
					printf("Player %d pick your piece: \n", curTurnColor=='w' ? wp:bp);
					fgets(str,INPUT_BUFFER,stdin);
					int column1;
					int row1;
					column1 = convertColumn(str[0]);
					row1 = convertRow(str[1]);
					
					if(column1 < 0 || row1 < 0)
					{
						continue;
					}

					printf("Where would you like to move this piece?\n");
					fgets(str,INPUT_BUFFER,stdin);
					int column2;
					int row2;
					column2 = convertColumn(str[0]);
					row2 = convertRow(str[1]);
					
					int success = MakeMove(board,column1, row1, column2, row2,curTurnColor,myList);
					if(success == 0)//signifies error
					{
						printf("Error. Try again\n");
						
					}
					else if (success == 1)
					{
						//printf("Successful move. \n");
						if(isChecked(board,((curTurnColor == 'w') ? 'b' : 'w')) )//checking if the opponent is in check. 
						{
							if(isCheckmate(board, ((curTurnColor == 'w') ? 'b' : 'w'), myList ))//if the opponent is in check we check for checkmate
							{
								printf("Checkmate. Winner is player %d!\n", curTurnColor=='w' ? wp:bp);
								won = 2;
							}
							else
							{
								printf("%c Player is in check.\n",((curTurnColor == 'w') ? 'b' : 'w'));	//
							}
						}
						else if (isCheckmate(board, ((curTurnColor == 'w') ? 'b' : 'w'), myList))//if the user is in checkmate but not checked it is stalemate
						{
							printf("Stalemate reached\n");
							won = 2;
						}
						if(won!=2)
						{
							//printf("Flipping color\n");
							curTurnColor = (curTurnColor == 'w') ? 'b' : 'w';//flipping turn color while the game still goes on
						}
					}
				}
				char o;
				printf("Do you want to save the replay of the game? (Y/N):");
				o = getchar();
				if(o == 'Y' || o == 'y'){
					replay(myList);
				}
				break;
				break;
			}
			case 2:
			{
				int won = 1;
				char curTurnColor = 'w';
				int firstTurn = 1;
				char u;
				printf("Would you like to be player 1 or 2?");
				int pp;
				scanf("%d\n",&pp);
				while ((getchar()) != '\n');
				if(pp == 2)
				{
					int aisucc = MakeOpeningMove(board,curTurnColor,myList);

					if(aisucc)
					{
					curTurnColor = (curTurnColor=='w' ? 'b' : 'w');
					firstTurn = 0;
					}
					else
					{
						printf("Unknown error opening move\n");
						exit = 1;
						return 1;
					}


				}
				int numRounds = 0;
				while(won!=2)
				{

					numRounds++;

					printf("Rounds have Elapsed: %d\n",numRounds);
					char enemyColor = (curTurnColor=='w' ? 'b' : 'w');
					printBoard(board);
					printf("To quit the game at any point, enter Q or q.\n");	
					printf("Player white pick your piece: \n");
					scanf("%[^\n]%*c", str);
					//fgets(str,INPUT_BUFFER,stdin);
					int column1;
					int row1;
					column1 = convertColumn(str[0]);
					row1 = convertRow(str[1]);

					if(column1 < 0 || row1 < 0)
					{
						continue;
					}

					printf("Where would you like to move this piece?\n");
					fgets(str,INPUT_BUFFER,stdin);
					int column2;
					int row2;
					column2 = convertColumn(str[0]);
					row2 = convertRow(str[1]);
					
					int success = MakeMove(board,column1, row1, column2, row2,curTurnColor,myList);
					if(success == 0)
					{
						printf("Error. Try again\n");
						
					}
					else if (success == 1)
					{
						if(isChecked(board,enemyColor))
						{
							if(isCheckmate(board, enemyColor, myList))
							{
								printf("Checkmate. Winner is Human\n");
								won = 2;
							}
							else
							{
								printf("Human Player is in check.\n");	
							}
						}
						else if (isCheckmate(board, ((curTurnColor == 'w') ? 'b' : 'w'), myList))//if the user is in checkmate but not checked it is stalemate
						{
							printf("Stalemate reached\n");
							won = 2;
						}
						printBoard(board);
						if(won!=2)
						{
							/*ai turn*/
							int aisuccess;
							curTurnColor = (curTurnColor == 'w' ? 'b' : 'w') ; 
							
							//MOVE *highesteval;
							if(firstTurn == 1)
							{
								printf("Opening move\n");
								aisuccess = MakeOpeningMove(board, curTurnColor, myList);
								firstTurn = 0;
							}
							else
							{
								printf("Doing AI move\n");
								aisuccess = GetAITurn(board,curTurnColor,myList, 4);
							}
							if(aisuccess == 0)
							{
								printf("Error. Try again\n");

							}
							else if (aisuccess == 1)
							{
								AIPromote(board);
								if(isChecked(board,curTurnColor == 'w' ? 'b' : 'w'))
								{
									if(isCheckmate(board, curTurnColor == 'w' ? 'b' : 'w', myList))
									{
										printBoard(board);
										printf("Checkmate. Winner is AI\n");
										won = 2;
									}
									else
									{
										printf("AI Player is in check.\n");	
									}
								}
								else if (isCheckmate(board, ((curTurnColor == 'w') ? 'b' : 'w'), myList))//if the user is in checkmate but not checked it is stalemate
								{
									printf("Stalemate reached\n");
									won = 2;
								}
								if(won!=2)
								{
									curTurnColor = (curTurnColor == 'b') ? 'w' : 'b';//flipping turn color while the game still goes on
								}
							}
						
						}
					}
					if(GetLength(myList)>2){
  						printBoard(board);
  						printf("Do you want to undo the last move? (Y/N)");
  						scanf("%[^\n]%*c", &u);
  						if(u == 'y' || u == 'Y'){
  							undo(board, myList);	
  						}
  					}
				}
				char o;
				printf("Do you want to save the replay of the game? (Y/N):");
				o = getchar();
				if(o == 'Y' || o == 'y'){
					replay(myList);
				}
				break;
			}
			case 3:
			{
				/* settings function */
				
				int option = 0;
				int done = 0;	
				
				/* settings menu keeps printing until user decides to go back to the main menu or makes changes */
				while (done == 0)
				{
					printf("Welcome to Settings!\n");
					printf("1. Change Player Colors\n");
					//printf("2. Change Timer Settings\n");
					printf("2. Go Back to Main Menu\n");
					scanf("%d", &option);
					getchar();
				
					switch(option)
					{
						case 1:
						{
							wp = whitepnum();
							bp = blackpnum(wp);
							done = 1;
							break;
						}
						
						//case 2:
						//{
						//	selection();
						//	done = 1;
						//	break;
						//}
						
						case 2:
						{
							done = 1;
							break;
						}
		
					}
				
				}
				break;
			}
			case 4:
			{
				printf("Thanks for playing. Exiting game...\n");
				exit=1;
				break;
			}
		}
		
	}
	#endif

	/* free memory of structs? */
}

// print menu function definition
int mainmenu()
{
	int select = 0;
	printf("1. Start New Game (Human v. Human)\n");
	printf("2. Start New Game (Human v. AI)\n");
	printf("3. Game Settings\n");
	printf("4. Exit Game\n");
	printf("Choose Option: ");
	scanf("%d",&select);
	getchar();
	return select;
}

int convertColumn(char a)
{
	int column1;
	if( a == 'Q' || a == 'q' )
	{
		printf("Quitting game... Thanks for playing!\n");
		exit(0);	
	}
	else if((a>'H' && a < 'a') || a  <'A' || a > 'h')
	{
		printf("Error: Out of bounds column option\n");
		column1 = -1;
	}
	else
	{
		column1 = (a >='A' && a <= 'H') ? (a-'A') : (a-'a');
	}
	return column1;
}
int convertRow(char b)
{
	return b-'1';
}

void undo(PIECE **board, MLIST *l){
	int i = 0;
	int cs, rs, cd ,rd;
	MOVE *m = NULL;
	PIECE p;
	for(i = 0; i < 2; i++){
		m = RemoveLastMove(l);
	    cs = getColS(m);
	    rs = getRowS(m);
	    cd = getColD(m);
	    rd = getRowD(m);
		p = GetRemovedPiece(m);
	    placePiece(board, &p, cs, rs);
		p = GetPiece(m);
	    placePiece(board, &p, cd, rd);
	}	
}

void gameTest(){
	PIECE **testBoard = makeBoard();
	initializeBoard(testBoard);
	MLIST *testList = NewMoveList();
	MakeMove(testBoard, 4, 1, 4, 3,'w',testList);
	printBoard(testBoard);
	printf("-----------------------------------\n");
	MakeMove(testBoard, 0, 6, 0, 4,'b',testList);
	printBoard(testBoard);
	printf("-----------------------------------\n");
	MakeMove(testBoard, 5, 0, 2, 3,'w',testList);
	printBoard(testBoard);
	printf("-----------------------------------\n");
	MakeMove(testBoard, 1, 7, 2, 5,'b',testList);
	printBoard(testBoard);
	printf("-----------------------------------\n");
	MakeMove(testBoard, 3, 0, 7, 4,'w',testList);
	printf("-----------------------------------\n");
	printBoard(testBoard);
	MakeMove(testBoard, 2, 6, 2, 5,'b',testList);
	printf("-----------------------------------\n");
	printBoard(testBoard);
	MakeMove(testBoard, 2, 3, 5, 6,'w',testList);
	printf("-----------------------------------\n");

	printBoard(testBoard);
	
	
	if(isChecked(testBoard, 'b') && isCheckmate(testBoard, 'b', testList))
	{
		printf("Checkmate. Winner is player white!\n");
	}
	replay(testList);
}

