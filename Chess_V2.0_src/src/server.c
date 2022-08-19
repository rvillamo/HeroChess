/* ClockServer.c: simple TCP/IP server example with timeout support
 * Author: Rainer Doemer, 2/17/15
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <assert.h>
#include "piece.h"
#include "board.h"
#include "game.h"
#include "movelist.h"
#include "move.h"
#include "settings.h"
#include "replay.h"
#include "ai.h"

//#define DEBUG 1

//this is a struct that describes a game, 
//including its associated movelist, board, and whose turn it is
struct FullGame
{
    PIECE **myBoard;
    MLIST *myList;
    char curTurnColor;
    int player_fd_1;
    int player_fd_2;
    int isAI;//a datamember that determines if this particular game is being done with an AI, instead of a human
};
typedef struct FullGame FullGame;

void InitializeGame(FullGame *myGame)
{
    myGame->myBoard = makeBoard();
    initializeBoard(myGame->myBoard);
    myGame->myList = NewMoveList();
    myGame->curTurnColor = 'w';
    myGame->player_fd_1 = -1;//signals an empty player fd socket
    myGame->player_fd_2 = -1;
}

FullGame *NewGame()
{
    FullGame *myGame;
    myGame = (FullGame*)malloc(sizeof(FullGame));
    if(!myGame)
    {
        perror("Out of memory! Aborting...");
        exit(10);
    }
    return myGame;
}
/* #define DEBUG */ /* be verbose */

/*** type definitions ****************************************************/

typedef void (*ClientHandler)(int DataSocketFD, FullGame *myGame);
typedef void (*TimeoutHandler)(void);

/*** global variables ****************************************************/

const char *Program /* program name for descriptive diagnostics */
    = NULL;
int Shutdown        /* keep running until Shutdown == 1 */
    = 0;
char ClockBuffer[26]    /* current time in printable format */
    = "";

/*** global functions ****************************************************/


void RemoveChar( char str[], char t )
{
  int i,j;
  for(i=0; i<strlen(str); i++)
  {
    if (str[i]==t) 
      for (j=i; j<strlen(str); j++)
      {
        str[j]=str[j+1];   
      } 
  }
}


void FatalError(        /* print error diagnostics and abort */
    const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
} /* end of FatalError */

int MakeServerSocket(       /* create a socket on this server */
    uint16_t PortNo)
{
    int ServSocketFD;
    struct sockaddr_in ServSocketName;

    /* create the socket */
    ServSocketFD = socket(PF_INET, SOCK_STREAM, 0);
    if (ServSocketFD < 0)
    {   FatalError("service socket creation failed");
    }
    /* bind the socket to this server */
    ServSocketName.sin_family = AF_INET;
    ServSocketName.sin_port = htons(PortNo);
    ServSocketName.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(ServSocketFD, (struct sockaddr*)&ServSocketName,
        sizeof(ServSocketName)) < 0)
    {   FatalError("binding the server to a socket failed");
    }
    /* start listening to this socket */
    if (listen(ServSocketFD, 5) < 0)    /* max 5 clients in backlog */
    {   FatalError("listening on socket failed");
    }
    return ServSocketFD;
} /* end of MakeServerSocket */

void PrintCurrentTime(void) /*  print/update the current real time */
{
    time_t CurrentTime; /* seconds since 1970 (see 'man 2 time') */
    char   *TimeString; /* printable time string (see 'man ctime') */
    char   Wheel,
       *WheelChars = "|/-\\";
    static int WheelIndex = 0;

    CurrentTime = time(NULL);   /* get current real time (in seconds) */
    TimeString = ctime(&CurrentTime);   /* convert to printable format */
    strncpy(ClockBuffer, TimeString, 25);
    ClockBuffer[24] = 0;    /* remove unwanted '/n' at the end */
    WheelIndex = (WheelIndex+1) % 4;
    Wheel = WheelChars[WheelIndex];
    printf("\rClock: %s %c",    /* print from beginning of current line */
    ClockBuffer, Wheel);    /* print time plus a rotating wheel */

    fflush(stdout);
} /* end of PrintCurrentTime */

int RockPaperScissors(char A, char B)//returns a 2 if A wins, a 1 if B wins, 0 if tie, -1 if error
{
    if(A != 'R' && A != 'P' && A != 'S')
    {
        printf("ERROR: Character A is invalid\n");
        return -1;
    }
    if(B != 'R' && B != 'P' && B != 'S')
    {
        printf("ERROR: Character B is invalid\n");
        return -1; 
    }
    if(A==B)
    {
        return 0;
    }
    if( (A == 'R' && B == 'S') || (A == 'P' && B == 'R') || (A == 'S' && B == 'P'))
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

void ProcessRequest(        /* process a game request by a client */
    int DataSocketFD, FullGame *myGame)
{
    //printf("Something something something power rangers\n");
    int  n;
    char RecvBuf[256];  /* message buffer for receiving a message */
    char SendBuf[256];  /* message buffer for sending a response */
    char ChatBuf[256];
    int DecidedWhoIsWhite = 0; 
    int hasMessageToSend = 0;
    char messageToSendTo;
        if(myGame->player_fd_1 != -1 && myGame->player_fd_2 != -1)//checking if theres one player in each FD, i.e at least two players logged in
        {

            printf("Waiting on REQUESTING BOARD request\n");
            memset(RecvBuf,0,256);
            n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
            printf("Requesting board request receieved: %s\n",RecvBuf);
            printf("Player 1 fd: %d\nPlayer 2 fd: %d\n",myGame->player_fd_1,myGame->player_fd_2);
            if(strcmp("REQUESTING_BOARD",RecvBuf)==0)
            {
                //do nothing, but do continue;
            }
            char PColor1 = 0;
            char PColor2 = 0;
            if(!DecidedWhoIsWhite)
            {

                memset(SendBuf,0,256);
                strncpy(SendBuf,"BLACK_WHITE",sizeof(SendBuf)-1);   
                printf("Sending turn decision to player 2. Code: %s\n", SendBuf);
                n = write(myGame->player_fd_2,SendBuf,11);
                if(n<0)
                {FatalError("Writing to data socket failed");
                }
                printf("Receiving responses:\n");
                memset(RecvBuf,0,256);
                n = read(myGame->player_fd_2,RecvBuf,1);
                if(n<0)
                {FatalError("Reading from data socket failed");
                }
                printf("Got %s from User 2\n",RecvBuf);
                if(strcmp(RecvBuf,"W")!=0 && strcmp(RecvBuf,"B")!=0)
                {
                    printf("Receieved: %s instead of 'B' or 'W'",RecvBuf);
                    FatalError("Somethign went wrong client side\n");
                }
                PColor2 = RecvBuf[0];


                printf("Sending turn decision to player 1. Code: %s\n", SendBuf);
                n = write(myGame->player_fd_1,SendBuf,11);
                if(n<0)
                {FatalError("Writing to data socket failed");
                }

                printf("Receiving other player's response:\n");
                memset(RecvBuf,0,256);
                n = read(myGame->player_fd_1,RecvBuf,1);
                if(n<0)
                {FatalError("Reading from data socket failed");
                }
                printf("Got %s from User 1\n", RecvBuf);
                if(strcmp(RecvBuf,"W") != 0 && strcmp(RecvBuf,"B") != 0)
                {
                    printf("Receieved: %s instead of 'B' or 'W'",RecvBuf);
                    FatalError("Somethign went wrong client side\n");
                }
                PColor1 = RecvBuf[0];

                while(PColor1 == PColor2)
                {


                    char RPS2;
                    char RPS1;
                    printf("Both players want same color. Sending RPS protocol:\n");
                    memset(SendBuf,0,256);
                    strncpy(SendBuf,"RPS_REQUEST",sizeof(SendBuf)-1);
                    printf("Sending turn decision to each player. Code: %s\n", SendBuf);
                    n = write(myGame->player_fd_2,SendBuf,11);
                    if(n<0)
                    {FatalError("Writing to data socket failed");
                    }
                    printf("Receiving responses:\n");
                    memset(RecvBuf,0,256);
                    n = read(myGame->player_fd_2,RecvBuf,sizeof(RecvBuf)-1);
                    if(n<0)
                    {FatalError("Reading from data socket failed");
                    }
                    while(strcmp(RecvBuf,"R")!=0 && strcmp(RecvBuf,"P")!=0 && strcmp(RecvBuf,"S")!=0)
                    {
                        printf("Receieved: %s instead of 'R' or 'P' or 'S'\n",RecvBuf);
                        n = read(myGame->player_fd_2,RecvBuf,sizeof(RecvBuf));
                        //FatalError("Reading from data socket failed");
                    }
                    printf("Ok recieved %s from player 2\n",RecvBuf);
                    RPS2 = RecvBuf[0];
    
                    n = write(myGame->player_fd_1,SendBuf,11);
                    if(n<0)
                    {FatalError("Writing to data socket failed");
                    }
                    printf("Receiving other player's response:\n");
                    memset(RecvBuf,0,256);
                    n = read(myGame->player_fd_1,RecvBuf,sizeof(RecvBuf)-1);
                    if(n<0)
                    {FatalError("Reading from data socket failed");
                    }
                    while(strcmp(RecvBuf,"R")!=0 && strcmp(RecvBuf,"P")!=0 && strcmp(RecvBuf,"S")!=0)
                    {
                        printf("Receieved: %s instead of 'R' or 'P' or 'S'\n ",RecvBuf);
                        n = read(myGame->player_fd_1,RecvBuf,sizeof(RecvBuf));
                        //FatalError("Reading from data socket failed");
                    }
                    printf("Ok recieved %s from player 1\n",RecvBuf);
                    RPS1 = RecvBuf[0];

                    int RPSResult = RockPaperScissors(RPS1,RPS2);
                    if(!RPSResult)
                    {
                        printf("Both tied. Need another game\n");
                    }
                    else if (RPSResult == -1)
                    {
                        printf("Invalid RPS inputs\n");
                    }
                    else if(RPSResult == 2)
                    {
                        //PColor 1 won so they dont have to change their color
                        PColor2 = (PColor1 == 'W' ? 'B' : 'W');
                    }
                    else if (RPSResult == 1)
                    {
                        PColor1 = (PColor2 == 'W' ? 'B' : 'W');
                        //PColor2 won so they dont have to change their color
                    }
                }
                printf("Sending respective colors:\nPlayer 1:%c\nPlayer 2:%c\n",PColor1,PColor2);

                memset(SendBuf,0,256);
                strncpy(SendBuf,"YOU_ARE_",sizeof(SendBuf)-1);
                SendBuf[8] = PColor2;
                printf("Sending color decision to Player 2. Code: %s\n", SendBuf);
                n = write(myGame->player_fd_2,SendBuf,sizeof(SendBuf)-1);
                if(n<0)
                {FatalError("Writing to data socket failed");
                }

                //memset(RecvBuf,0,256);
                //printf("RecvBuf: %d\n",sizeof(RecvBuf));
                n = read(myGame->player_fd_2, RecvBuf, sizeof(RecvBuf));
                if(n<0)
                {FatalError("Reading from data socket failed");
                }
                while(strcmp(RecvBuf,"OK4") != 0)
                {
                    printf("Recieved '%s' instead of OK4\n",RecvBuf);
                    //FatalError("Invalid OK4");
                    n = read(myGame->player_fd_2,RecvBuf,sizeof(RecvBuf));
                }
                #ifdef DEBUG
                printf("OK4 reached\n");
                #endif
                SendBuf[8] = PColor1;
                printf("Sending color decision to Player 1. Code: %s\n", SendBuf);
                n = write(myGame->player_fd_1,SendBuf,sizeof(SendBuf)-1);
                if(n<0)
                {FatalError("Writing to data socket failed");
                }

                memset(RecvBuf,0,256);
                n = read(myGame->player_fd_1,RecvBuf,sizeof(RecvBuf));
                if(n<0)
                {FatalError("Writing to data socket failed");
                }
                while(strcmp(RecvBuf,"OK4") != 0)
                {
                    printf("Recieved '%s' instead of OK4\n",RecvBuf);
                    //FatalError("Reading from data socket failed");
                    n = read(myGame->player_fd_1,RecvBuf,sizeof(RecvBuf));
                }
                #ifdef DEBUG
                printf("OK4 reached\n");
                #endif
                DecidedWhoIsWhite = 1;
            }
            int won=0;
            int whitePlayerFD;
            int blackPlayerFD;
            if(PColor1 == 'W')
            {
                whitePlayerFD = myGame->player_fd_1;
                blackPlayerFD = myGame->player_fd_2;
            }
            else
            {
                whitePlayerFD = myGame->player_fd_2;
                blackPlayerFD = myGame->player_fd_1;
            }
            while(!won)
            {

                char curTurnColor = myGame->curTurnColor;
                char enemyTurnColor = curTurnColor == 'w' ? 'b' : 'w';
                printf("Found a player!\n");
                int curTurnFD = 0;
                printf("Player 1 fd: %d\nPlayer 2 fd: %d\n",myGame->player_fd_1,myGame->player_fd_2);
                if(myGame->curTurnColor == 'w')
                {
                    curTurnFD = whitePlayerFD;
                }
                else
                {
                    curTurnFD = blackPlayerFD;
                }
                #ifdef DEBUG
                printf("Current fd: %d\n",curTurnFD);
                #endif
                printBoard(myGame->myBoard);

                if(hasMessageToSend && curTurnColor == messageToSendTo)
                { 
                    printf("Sending message to user: %s\n",ChatBuf);
                    n = write(curTurnFD,ChatBuf,sizeof(ChatBuf)-1);
                    if(n<0)
                    {FatalError("Error writing to socket");
                    }
                    memset(RecvBuf,0,256);
                    n = read(curTurnFD,RecvBuf,sizeof(RecvBuf)-1);
                    if(n<0)
                    {FatalError("Error reading from socket");
                    }
                    if(strcmp(RecvBuf,"OK6")!=0)
                    {
                        printf("Recieved %s instead of OK6\n", RecvBuf);
                        FatalError("Somethignn went wrong client side");
                    }
                    hasMessageToSend = 0;
                }

                strncpy(SendBuf,"PRINT_BOARD",sizeof(SendBuf)-1);
                printf("Sending string: '%s'\n",SendBuf);
                int n = write(curTurnFD,SendBuf,11);
                if(n<0)
                {FatalError("Writing to data socket failed");
                }
                memset(RecvBuf,0,256);
                n = read(curTurnFD,RecvBuf,sizeof(RecvBuf)-1);
                if(n<0)
                {
                    FatalError("Somethignn went wrong client side hh\n");
                }
                if(strcmp(RecvBuf,"OK1")!=0)
                {
                    printf("Receieved: %s instead of OK 1",RecvBuf);
                    FatalError("Somethign went wrong client side\n");
                }
                #ifdef DEBUG
                printf("First ok reached\n");
                #endif
                strncpy(SendBuf,"",sizeof(SendBuf)-1);
                for(int i = 0; i <128; i+=2)
                 {
                     int realCoord = i/2;
                     int col = realCoord/8;
                     int row = realCoord%8;
                     char chC = GetColor(getPiece(myGame->myBoard,col,row));
                     char chT = GetType(getPiece(myGame->myBoard,col,row));
                     SendBuf[i]  = chC;
                     SendBuf[i+1] = chT;
                 }
                //printf("%s",SendBuf);
                 printf("Now sending Board data: %s\n",SendBuf);
                n = write(curTurnFD,SendBuf,sizeof(SendBuf)-1);
                //n = write(myGame->player_fd_2,SendBuf,sizeof(SendBuf-1));
                
                printf("Waiting for ok 2...\n");
                memset(RecvBuf,0,256);
                n = read(curTurnFD,RecvBuf,sizeof(RecvBuf)-1);
                if(n<0)
                {

                    FatalError("Somethignn went wrong client side hh\n");
                }
                if(strcmp(RecvBuf,"OK2")!=0)
                {
                    printf("Receieved: %s instead of OK2",RecvBuf);
                    FatalError("Somethign went wrong client side\n");
                    memset(RecvBuf,0,256);
                    /*
                    n = read(curTurnFD,RecvBuf,(sizeof(RecvBuf)-1));
                    printf("Readiing in again. Got this: %s\n",RecvBuf);
                    memset(RecvBuf,0,256);
                    n = read(curTurnFD,RecvBuf,(sizeof(RecvBuf)-1));
                    printf("Readiing in again. Got this: %s\n",RecvBuf);
                    */
                }

                int gotMove = 0;//flag to see if we have a move and not a message
                while(!gotMove)
                {
                    //MOVE REQUEST
                    printf("Now requesting move\n");
                    strncpy(SendBuf,"REQUESTING_MOVE",sizeof(SendBuf));
                    #ifdef DEBUG
                    printf("Message: %s\n",SendBuf);
                    #endif
                    n = write(curTurnFD,SendBuf,sizeof(SendBuf)-1);
                    if(n<0)
                    {FatalError("writing to data socket failed");
                    }
                    #ifdef DEBUG
                    printf("Skipped requesesting move\n");
                    #endif
                    
                    //MOVE DATA READ
                    memset(RecvBuf,0,256);
                    printf("Now waiting for move: \n");
                    //printf("In receiv buffer before reading: %s\n",RecvBuf);
                    n  = read(curTurnFD,RecvBuf,sizeof(RecvBuf)-1);
                    if(n<0)
                    {FatalError("reading data socket failed");
                    }
                    if(RecvBuf[0] == '-')
                    {
                        strcpy(ChatBuf,RecvBuf);
                        hasMessageToSend = 1;
                        messageToSendTo = (curTurnColor == 'w' ? 'b' : 'w');
                        printf("Got a message to other client!%s\n\n",ChatBuf);
    
                    }
                    else if(RecvBuf[0] != '+')
                    {
                        printf("Got this: %s instead of the proper '+' protocol code\n", RecvBuf);
                        FatalError("Dun f'd up");
                        //n = read(curTurnFD,RecvBuf,sizeof(RecvBuf-1));
                        //printf("Got this reee%s\n",RecvBuf);
                    }
                    else
                    {
                        gotMove = 1;
                    }
                    //executing move
                }

                printf("Receieved move: %s\n",RecvBuf);
                int colS = RecvBuf[1]-'A';
                int rowS = RecvBuf[2]-'1';
                int colD = RecvBuf[3]-'A';
                int rowD = RecvBuf[4]-'1';
                PIECE **board = myGame->myBoard;
                MLIST *myList = myGame->myList;
                if(!MakeMove(board,colS,rowS,colD,rowD,curTurnColor,myList))
                {
                //invalid move handling
                printf("WRONG MOVE DONT DO THAT PLS \n");
                strncpy(SendBuf,"INVALID_MOVE",sizeof(SendBuf)-1);
                }
                else
                {
                    if(isChecked(board,enemyTurnColor))
                    {
                        if(isCheckmate(board,enemyTurnColor,myList))
                        {
    
                            strncpy(SendBuf,"WIN_ACHIEVED ",sizeof(SendBuf)-1);
                            char winningChar = curTurnColor - 32;//this just ascii shifts the char to uppercase
                            SendBuf[13] = winningChar;
                            won  = 1;
                        }
                        else
                        {
                            strncpy(SendBuf,"SUCCESSFUL_MOVE_CHECK_",sizeof(SendBuf)-1);
                            char checkedChar = enemyTurnColor-32;
                            SendBuf[22] = checkedChar;
                            myGame->curTurnColor = enemyTurnColor;
                        }
                    }
                    else//if they didnt make a check but successful move
                    {
                        myGame->curTurnColor = enemyTurnColor;//flipping turn color
        
                    
                        strncpy(SendBuf, "VALID_MOVE", sizeof(SendBuf)-1);
                    }
                }

                printf("Sending to client: %s\n",SendBuf);
                n = write(curTurnFD,SendBuf,sizeof(SendBuf)-1);
                if(n<0)
                {FatalError("writing to socket failed");
                }

                memset(RecvBuf,0,256);
                n = read(curTurnFD,RecvBuf,sizeof(RecvBuf));
                if(n<0)
                {FatalError("writing to data socket failed");
                }
                if(strcmp(RecvBuf,"OK3")!=0)
                {
                    printf("Receieved: %s instead of OK3",RecvBuf);
                    FatalError("Somethign went wrong client side\n");
                }
                
            }
        }
        /*else
        {
            printf("Not enough players, reutrning\n");
            strcpy(SendBuf,"MORE_PLAYERS");
            write(DataSocketFD,SendBuf,sizeof(SendBuf)-1);
            if(n<0)
            {FatalError("writing to data socket failed");
            }
            return;
        }*/
    
    memset(RecvBuf,0,256);
    n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
    if (n < 0) 
    {   FatalError("reading from data socket failed");
    }
    RecvBuf[n] = 0;
#ifdef DEBUG
    printf("%s: Received message: %s\n", Program, RecvBuf);
#endif
    
}

/*
    if (RecvBuf[0] == '+')
    {   
        //this just checks if the request is occuring during another player's turn
        if((myGame->curTurnColor == 'w' && DataSocketFD != myGame->player_fd_1 )
            || (myGame->curTurnColor == 'b' && DataSocketFD !=myGame->player_fd_2))
        {
            strncpy(SendBuf,"Not currently your turn!",sizeof(SendBuf)-1);

        }
        else//if it isnt, we continue on
        {
        RemoveChar(RecvBuf, RecvBuf[0]);//removing protocol character
        char curTurnColor = myGame->curTurnColor;//setting turn color to the one stored in myGame
        char enemyTurnColor = (curTurnColor=='w' ? 'b' : 'w');//enemy's color
        int colS = RecvBuf[0]-'A';
        int rowS = RecvBuf[1]-'1';
        int colD = RecvBuf[2]-'A';
        int rowD = RecvBuf[3]-'1';
        PIECE **board = myGame->myBoard;
        MLIST *myList = myGame->myList;
        if(!MakeMove(board,colS,rowS,colD,rowD,curTurnColor,myList))
        {
            strncpy(SendBuf,"INVALID_MOVE",sizeof(SendBuf)-1);
        }
        else
        {
            if(isChecked(board,enemyTurnColor))
            {
                if(isCheckmate(board,enemyTurnColor,myList))
                {

                    strncpy(SendBuf,"WIN_ACHIEVED ",sizeof(SendBuf)-1);
                    char winningChar = curTurnColor - 32;//this just ascii shifts the char to uppercase
                    SendBuf[13] = winningChar;

                }
                else
                {
                    strncpy(SendBuf,"SUCCESSFUL_MOVE_CHECK_",sizeof(SendBuf)-1);
                    char checkedChar = enemyTurnColor-32;
                    SendBuf[22] = checkedChar;
                }
            }
            myGame->curTurnColor = enemyTurnColor;//flipping turn color

        
          strncpy(SendBuf, "MOVE FUNCTION EXECUTED", sizeof(SendBuf)-1);
        }
        }
        SendBuf[sizeof(SendBuf)-1] = 0;
        strncat(SendBuf, ClockBuffer, sizeof(SendBuf)-1-strlen(SendBuf));
        
    }

    else if (RecvBuf[0] == '/')
    {
        RemoveChar(RecvBuf, RecvBuf[0]);
        strncpy(SendBuf, "FIRST LOGIN FUNCTION EXECUTED", sizeof(SendBuf)-1);
        SendBuf[sizeof(SendBuf)-1] = 0;
        strncat(SendBuf, ClockBuffer, sizeof(SendBuf)-1-strlen(SendBuf));
    }
    else
    {
        printf("Unknown protocol code\n");
        FatalError("Unknown code");
    }
*/   
        


    //l = strlen(SendBuf);
#ifdef DEBUG
    //printf("%s: Sending response: %s.\n", Program, SendBuf);
#endif
    //n = write(DataSocketFD, SendBuf, l);
    /*if (n < 0)
    {   FatalError("writing to data socket failed");
    }*/
    


/* end of ProcessRequest */

void ServerMainLoop(        /* simple server main loop */
    int ServSocketFD,       /* server socket to wait on */
    ClientHandler HandleClient, /* client handler to call */
    TimeoutHandler HandleTimeout,   /* timeout handler to call */
    int Timeout)            /* timeout in micro seconds */
{
    int DataSocketFD;   /* socket for a new client */
    socklen_t ClientLen;
    struct sockaddr_in
    ClientAddress;  /* client address we connect with */
    fd_set ActiveFDs;   /* socket file descriptors to select from */
    fd_set ReadFDs; /* socket file descriptors ready to read from */
    struct timeval TimeVal;
    int res, i;
    //char SendBuf[256];
    //int l;
    FD_ZERO(&ActiveFDs);        /* set of active sockets */
    FD_SET(ServSocketFD, &ActiveFDs);   /* server socket is active */
    //INITIALIZING GAME BOARD: CURRENTLY ONLY ACCEPTING 2 USERS
    FullGame *myGame = NULL;
    myGame = NewGame();
    InitializeGame(myGame);
    //END OF GAME BOARD INITAILIZATION SEQUENCE

    //int clientOneIsHandled = 0;
    while(!Shutdown)
    {   ReadFDs = ActiveFDs;
    TimeVal.tv_sec  = Timeout / 1000000;    /* seconds */
    TimeVal.tv_usec = Timeout % 1000000;    /* microseconds */
    

    

    //ALL CODE AFTER THIS IS HANDLING THE CLIENT'S RESPONSE
    /* block until input arrives on active sockets or until timeout */
    res = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
    if (res < 0)
    {   FatalError("wait for input or timeout (select) failed");
    }
    if (res == 0)   /* timeout occurred */
    {
#ifdef DEBUG
        printf("%s: Handling timeout...\n", Program);
#endif
        HandleTimeout();
    }
    else        /* some FDs have data ready to read */
    {   for(i=0; i<FD_SETSIZE; i++)
        {   if (FD_ISSET(i, &ReadFDs))
        {   if (i == ServSocketFD)
            {   /* connection request on server socket */
#ifdef DEBUG
            printf("%s: Accepting new client %d...\n", Program, i);
#endif
            ClientLen = sizeof(ClientAddress);
            DataSocketFD = accept(ServSocketFD,
                (struct sockaddr*)&ClientAddress, &ClientLen);
            if (DataSocketFD < 0)
            {   FatalError("data socket creation (accept) failed");
            }
#ifdef DEBUG
            printf("%s: Client %d connected from %s:%hu.\n",
                Program, i,
                inet_ntoa(ClientAddress.sin_addr),
                ntohs(ClientAddress.sin_port));
#endif
            

                #ifdef DEBUG
                printf("Data socket is %d\n",DataSocketFD);
                #endif 
                //CODE TO ADD NEW USER TO AVAILABLE GAME BOARD
                //This code adds the new client to the first available file descriptor
                if(myGame->player_fd_1 == -1 && DataSocketFD != myGame->player_fd_2)
                {
                    myGame->player_fd_1 = DataSocketFD;
                }
                else if (myGame->player_fd_2 == -1 && DataSocketFD != myGame->player_fd_1)
                {
                    myGame->player_fd_2 = DataSocketFD;
                }
                //END OF CODE TO ADD NEW USER TO AVAILABLE GAME BOARD FD's


                 //CODE TO HANDLE THE GAME
                 //FD_CLR(i,&ActiveFDs);
                FD_SET(DataSocketFD, &ActiveFDs);
            }
            else
            {   /* active communication with a client */
#ifdef DEBUG
            printf("%s: Dealing with client %d...\n", Program, i);
#endif
            //if the first flag is unchecked AND one of the players is unregistered, or if both are registered
           /* if( !clientOneIsHandled || !(myGame->player_fd_1 ==-1 || myGame->player_fd_2 ==-1) )
            {

                
                clientOneIsHandled=1;
                printf("Client one handled\n");
            }
           */
            HandleClient(i, myGame);

#ifdef DEBUG
            printf("%s: Closing client %d connection.\n", Program, i);
#endif
            //printf("%s: Closing client %d connection.\n", Program, i);
            close(i);
            FD_CLR(i, &ActiveFDs);
            }
        }

        }
    }
    }
} /* end of ServerMainLoop */

/*** main function *******************************************************/

int main(int argc, char *argv[])
{
    int ServSocketFD;   /* socket file descriptor for service */
    int PortNo;     /* port number */

    Program = argv[0];  /* publish program name (for diagnostics) */
#ifdef DEBUG
    printf("%s: Starting...\n", Program);
#endif
    if (argc < 2)
    {   fprintf(stderr, "Usage: %s port\n", Program);
    exit(10);
    }
    PortNo = atoi(argv[1]); /* get the port number */
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
        Program, PortNo);
        exit(10);
    }
#ifdef DEBUG
    printf("%s: Creating the server socket...\n", Program);
#endif
    ServSocketFD = MakeServerSocket(PortNo);
    printf("%s: Waiting for connections at port %d...\n", Program, PortNo);
    ServerMainLoop(ServSocketFD, ProcessRequest,
            PrintCurrentTime, 250000);
    printf("\n%s: Shutting down.\n", Program);
    close(ServSocketFD);
    return 0;
}




































/* EOF ClockServer.c */


