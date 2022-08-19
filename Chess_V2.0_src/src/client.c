/* ClockClient.c: simple interactive TCP/IP client for ClockServer
 * Author: Rainer Doemer, 2/16/15 (updated 2/20/17)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include "database.c"
#include "game.h"
#include "board.h"
#include "piece.h"
//#define DEBUG 	/* be verbose */

/*** global variables ****************************************************/

const char *Program	/* program name for descriptive diagnostics */
	= NULL;

/*** global functions ****************************************************/

void FatalError(		/* print error diagnostics and abort */
	const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
} /* end of FatalError */

int main(int argc, char *argv[])
{
    int l, n;
    int SocketFD = -1;	/* socket file descriptor */
	int PortNo;		/* port number */
    struct sockaddr_in
	ServerAddress;	/* server address we connect with */
    struct hostent
	*Server;	/* server host */
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */
	char UsrNme[100];
	char PassWd[100];
    Program = argv[0];	/* publish program name (for diagnostics) */
#ifdef DEBUG
    printf("%s: Starting...\n", argv[0]);
#endif
    
    //Code used to verify proper host and port number, as well as initialize socketaddress 


    if (argc < 3)
    {   fprintf(stderr, "Usage: %s hostname port\n", Program);
	exit(10);
    }
    Server = gethostbyname(argv[1]);
    if (Server == NULL)
    {   fprintf(stderr, "%s: no such host named '%s'\n", Program, argv[1]);
        exit(10);
    }
    PortNo = atoi(argv[2]);
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
		Program, PortNo);
        exit(10);
    }
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr = *(struct in_addr*)Server->h_addr_list[0];
	PIECE **myBoard = NULL;
	myBoard = makeBoard();
    //
	int inGame = 0;
	int LoggedIn = 0;
	printf("Welcome! Lets begin by logging you in.\n");
	while(!LoggedIn)
	{
		printf("1. New User\n2. Returning User\n3. Exit\n");
		fgets(UsrNme, sizeof(UsrNme), stdin);
		l = strlen(UsrNme);
		if (UsrNme[l-1] == '\n')
		{   UsrNme[--l] = 0;
		}
		if(strcmp(UsrNme,"1")==0)
		{
			printf("Welcome, new agent, to the Avengineers initiative\nEnter your designated S.H.I.E.L.D ID (Username):\n");
			printf("(No Spaces, newline characters, or a preexisting name)\n");
			fgets(UsrNme, sizeof(UsrNme), stdin);
			l = strlen(UsrNme);
			if (UsrNme[l-1] == '\n')
			{   UsrNme[--l] = 0;
			}
			printf("Enter your desired Credentials:\n");
			fgets(PassWd, sizeof(PassWd), stdin);
			l = strlen(PassWd);
			if (PassWd[l-1] == '\n')
			{   PassWd[--l] = 0;
			}
			appendUser(UsrNme);
			
			appendPass(PassWd);
			printf("User now registered. Redirecting to main menu now:\n");
			
		}
		else if(strcmp(UsrNme,"2")==0)
		{
			printf("Welcome back.\nDesignated S.H.I.E.L.D ID (Username):\n");
			fgets(UsrNme, sizeof(UsrNme), stdin);
			l = strlen(UsrNme);
			if (UsrNme[l-1] == '\n')
			{   UsrNme[--l] = 0;
			}
			int p = checkUser(UsrNme);
			if(p)
			{
				printf("Hello User: %s\nDesignated S.H.I.E.L.D CREDENTIALS (Password):\n",UsrNme);
				fgets(UsrNme, sizeof(PassWd), stdin);
				l = strlen(PassWd);
				if (PassWd[l-1] == '\n')
				{   PassWd[--l] = 0;
				}

				if(checkPass(p,PassWd))
				{
					LoggedIn = 1;
				}
				else
				{
					printf("Credentials unverified. Returning to main menu now\n");
				}
			}
			else
			{
				printf("ID not found. Redirecting to main menu\n");
			}
		}
		else if(strcmp(UsrNme,"3")==0)
		{
			printf("Goodbye!\n");
			printf("Ending socket connections....\n");
			if(SocketFD!=-1)
			{
				close(SocketFD);
			}
			return 0;
		}
		memset(UsrNme,0,100);
		memset(PassWd,0,100);
	}	


	//this is a flag to signify we are in the middle of a game. This just prevents the client from closing the connection in the mean time
	int inMiddleOfTurn = 0;
    do
    {	
	if (1)

	{   
		if(!inMiddleOfTurn)
		{
			SocketFD = socket(AF_INET, SOCK_STREAM, 0);
	    	if (SocketFD < 0)
	    	{   FatalError("socket creation failed");
	    	}
	    	printf("%s: Connecting to the server at port %d...\n",
				Program, PortNo);
	    	if (connect(SocketFD, (struct sockaddr*)&ServerAddress,
				sizeof(ServerAddress)) < 0)
	    	{   FatalError("connecting to server failed");
	    	}
	    }
		
	    if(!inGame)
	    {

		    strncpy(SendBuf,"REQUESTING_BOARD",sizeof(SendBuf)-1);
		    #ifdef DEBUG
		    printf("Sending REQUESTING_BOARD request\n");
		    #endif
		    //printf("%s\n",SendBuf);
		    //printf("0 if true: %d\n",strcmp(SendBuf,"REQUESTING_BOARD"));
		   	n = write(SocketFD,SendBuf,sizeof(SendBuf)-1);
		   	if (n < 0) 
		    {   FatalError("writing to socket failed\n");
		    }
		  
		 }

		    printf("Now waiting for response:\n");
		    //printf("Current buffer: %s\n",RecvBuf);
		    memset(RecvBuf,0,sizeof(RecvBuf));
		    n = read(SocketFD,RecvBuf,sizeof(RecvBuf)-1);
		    if(n<0)
		    {
		    	FatalError("Error reading from socket");
		    }
		    #ifdef DEBUG
		    printf("Ready to play: %s\n",RecvBuf);
		    #endif
		    if(strcmp("MORE_PLAYERS",RecvBuf) != 0)
		    {
		    	#ifdef DEBUG
		    	printf("We are now in game!\n");
		    	#endif
		    	inGame=1;
		    }

		    //sending it a second time to try and get the write to go through
		   /* strncpy(SendBuf,"REQUESTING_BOARD",sizeof(SendBuf)-1);
		    printf("Sending REQUESTING_BOARD request\n");
		   	n = write(SocketFD,SendBuf,sizeof(SendBuf)-1);
		   	if (n < 0) 
		    {   FatalError("writing to socket failed\n");
		    }
		    */
		   
	    //SECTION TO INTERPRET RESPONSE
	    //-------------------------------------------------------------------

	    if(strcmp("FIRST MENU", RecvBuf) == 0)
	    {
	    	printf("Please select 1 or 2:\n");
	    	printf("\t 1. New user\n");
	    	printf("\t 2. Returning user\n");
	    	fgets(SendBuf, sizeof(SendBuf), stdin);
			l = strlen(SendBuf);
			if (SendBuf[l-1] == '\n')
			{   SendBuf[--l] = 0;	//is this meant to be an escape sequence?
			}
			#ifdef DEBUG
			printf("%s: Sending message '%s'...\n", Program, SendBuf);
			#endif
	    	n = write(SocketFD, SendBuf, l);
	    	if (n < 0)
	    	{   FatalError("writing to socket failed");
	    	}

	    }
	    else if(strcmp("NEW_USERNAME", RecvBuf) == 0)
	    {
	    	printf("Welcome, nice to meet you! \n");
	    	printf("Enter a new username for HeroChess (6-8 characters, no spaces):\n");
	    	printf("(Suggestions: BlckWdw, FE_Man, THOR)\n");
	    	fgets(SendBuf, sizeof(SendBuf), stdin);
			l = strlen(SendBuf);
			if (SendBuf[l-1] == '\n')
			{   SendBuf[--l] = 0;	//is this meant to be an escape sequence?
			}
			printf("%s: Sending password '%s'...\n", Program, SendBuf);
	    	n = write(SocketFD, SendBuf, l);
	    	if (n < 0)
	    	{   FatalError("writing to socket failed");
	    	}
	    }
	   	else if(strcmp("NEW_PASSWORD", RecvBuf) == 0)
	    {
	    	printf("Enter a password(8 characters, must contain one number and special character , 4, @, !, *):\n");
	    	fgets(SendBuf, sizeof(SendBuf), stdin);
			l = strlen(SendBuf);
			if (SendBuf[l-1] == '\n')
			{   SendBuf[--l] = 0;	//is this meant to be an escape sequence?
			}
			printf("%s: Sending password '%s'...\n", Program, SendBuf);
	    	n = write(SocketFD, SendBuf, l);
	    	if (n < 0)
	    	{   FatalError("writing to socket failed");
	    	}
	    }
	    else if (strcmp("REQUESTING_USERNAME",RecvBuf) == 0)
	    {
			printf("Welcome, back! \n");
	    	printf("Enter your username:\n");
	    	fgets(SendBuf, sizeof(SendBuf), stdin);
			l = strlen(SendBuf);
			if (SendBuf[l-1] == '\n')
			{   SendBuf[--l] = 0;	//is this meant to be an escape sequence?
			}
			printf("%s: Sending username '%s'...\n", Program, SendBuf);
	    	n = write(SocketFD, SendBuf, l);
	    	if (n < 0)
	    	{   FatalError("writing to socket failed");
	    	}
	    }
	    else if(strcmp("REQUESTING_PASSWORD", RecvBuf) == 0)
	    {
	    	printf("Enter your password:\n");
	  
	    	fgets(SendBuf, sizeof(SendBuf), stdin);
			l = strlen(SendBuf);
			if (SendBuf[l-1] == '\n')
			{   SendBuf[--l] = 0;	//is this meant to be an escape sequence?
			}
			printf("%s: Sending password '%s'...\n", Program, SendBuf);
	    	n = write(SocketFD, SendBuf, l);
	    	if (n < 0)
	    	{   FatalError("writing to socket failed");
	    	}
	    }
	    else if(strcmp("INVALID_USERNAME", RecvBuf) == 0)
	    {
	    	printf("Error: Invalid username. Please try again\n");
	    	
	    }
	    else if(strcmp("INVALID_PASSWORD", RecvBuf) == 0)
	    {
	    	printf("Error: Invalid password. Please try again\n");
	    	
	    }
	    else if (strcmp("REQUESTING_MOVE",RecvBuf) == 0)
	    {
	    	
	    	printf("Your move: (Or enter '--' to send a message instead)\n");
	    	printf("Select a piece:\n");
	    	char colS;
	    	char rowS;
	    	char colD;
	    	char rowD;
	    	scanf("%c%c", &colS, &rowS);
	    	if(colS == '-' && rowS == '-')
	    	{
	    		while ( getchar() != '\n' );
	    		printf("What would you like to say to them?\n");
	    		fgets(SendBuf, sizeof(SendBuf), stdin);
				int l = strlen(SendBuf);
				if (SendBuf[l-1] == '\n')
				{   SendBuf[--l] = 0;
				}
				for(int i = l; i > 0; i--)
				{
					SendBuf[i] = SendBuf[i-1];
				}
				SendBuf[0] = '-';
				n = write(SocketFD,SendBuf,sizeof(SendBuf)-1);
				if(n<0)
				{
					FatalError("Writing to data socket failed");
				}
				printf("Sent message: %s\n",SendBuf);

				//while ( getchar() != '\n' );
	    	}
	    	else
	    	{
	    		printf("Where would you like to move this piece?\n");
	    		//while ( getchar() != '\n' )
	    		scanf("\n%c%c", &colD, &rowD);
	    		printf("OK Moving Piece %c%c to %c%c\n",colS,rowS,colD,rowD);
	    		memset(SendBuf,0,256);
				SendBuf[0] = '+';
				SendBuf[1] = colS;
				SendBuf[2] = rowS;
				SendBuf[3] = colD;
				SendBuf[4] = rowD;
				if(SendBuf[1]>=97 && SendBuf[1]<=104)
				{
					SendBuf[1]-=32;
				}
				
				if(SendBuf[3]>=97 && SendBuf[3]<=104)
				{
					SendBuf[3]-=32;
				}
				#ifdef DEBUG
				printf("%s: Sending move '%s'...\n", Program, SendBuf);
				#endif
	    		n = write(SocketFD, SendBuf, sizeof(SendBuf)-1);
	    		if (n < 0)
	    		{   FatalError("writing to socket failed");
	    		}
	    		//n = write(SocketFD,SendBuf,sizeof(SendBuf)-1);
	    		//n = read(SocketFD,RecvBuf,sizeof(RecvBuf));
	    		//inMiddleOfTurn = 0;
	    		while ( getchar() != '\n' );
	    	}
	    	
	    }
	    else if (strcmp("INVALID_MOVE",RecvBuf) == 0)
	    {
	    	printf("Invalid move: Please enter a new move\n");
	    	strncpy(SendBuf,"OK3",sizeof(SendBuf)-1);
	    	#ifdef DEBUG
	    	printf("Now sending confirmation %s\n",SendBuf);
	    	#endif
	    	n  =  write(SocketFD, SendBuf, sizeof(SendBuf)-1);
	    	if(n<0)
	    	{
	    		FatalError("Error writing to socket");
	    	}

	    }
	    else if (strcmp("VALID_MOVE",RecvBuf) == 0)
	    {
	    	printf("Valid move. Now waiting on Opponent:\n");
	    	memset(SendBuf,0,256);
	    	strncpy(SendBuf,"OK3",sizeof(SendBuf)-1);
	    	#ifdef DEBUG
	    	printf("Now sending confirmation %s\n",SendBuf);
	    	#endif
	    	n  =  write(SocketFD, SendBuf, sizeof(SendBuf)-1);
	    	if(n<0)
	    	{
	    		FatalError("Error writing to socket");
	    	}
	    	//inMiddleOfTurn = 0;
	    }
	    else if (strcmp("WIN_ACHIEVED B",RecvBuf) == 0)
	    {
	    	printf("Checkmate; Player Black Wins!\n");

	    	memset(SendBuf,0,256);
	    	strncpy(SendBuf,"OK3",sizeof(SendBuf)-1);
	    	#ifdef DEBUG
	    	printf("Now sending confirmation %s\n",SendBuf);
	    	#endif
	    	n  =  write(SocketFD, SendBuf, sizeof(SendBuf)-1);
	    	if(n<0)
	    	{
	    		FatalError("Error writing to socket");
	    	}
	    	inMiddleOfTurn = 0;
	    }
	    else if (strcmp("WIN_ACHIEVED W",RecvBuf) == 0)
	    {
	    	printf("Checkmate; Player White Wins!\n");

	    	memset(SendBuf,0,256);
	    	strncpy(SendBuf,"OK3",sizeof(SendBuf)-1);
	    	#ifdef DEBUG
	    	printf("Now sending confirmation %s\n",SendBuf);
	    	#endif
	    	n  =  write(SocketFD, SendBuf, sizeof(SendBuf)-1);
	    	if(n<0)
	    	{
	    		FatalError("Error writing to socket");
	    	}
	    	inMiddleOfTurn = 0;
	    }
	    else if (strcmp("PRINT_BOARD",RecvBuf) == 0)
	    {

	    	memset(SendBuf,0,256);
	    	strncpy(SendBuf, "OK1", sizeof(SendBuf));
	    	int ll = strlen(SendBuf);
	    	#ifdef DEBUG
	    	printf("Sending: %s\n",SendBuf);
	    	#endif
	    	n = write(SocketFD,SendBuf,ll);
	    	if(n<0)
	    	{
	    		FatalError("Error writing");
	    	}
	    	//code to read in board data into myBoard
	    	#ifdef DEBUG
	    	printf("Attempting to read in from socket\n");
	    	#endif
	    	n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	    	for(int i = 0; i <128; i+=2)
	    	{
	    		int realCoord = i/2;
	    		int col = realCoord/8;
	    		int row = realCoord%8;
	    		char chC = RecvBuf[i];
	    		char chT = RecvBuf[i+1];
	    		SetColor(getPiece(myBoard,col,row),chC);
	    		SetType(getPiece(myBoard,col,row),chT);

	    	}
	    	if(n < 0)
	    	{
	    		FatalError("Board data not read, Fatal Error. Exiting\n");
	    	}
	    	printBoard(myBoard);

	    	memset(SendBuf,0,256);
	    	strncpy(SendBuf, "OK2", sizeof(SendBuf)-1);
	    	ll = strlen(SendBuf);
	    	#ifdef DEBUG
	    	printf("Sending: %s\n",SendBuf);
	    	#endif
	    	n = write(SocketFD,SendBuf,ll);
	    	if(n<0)
	    	{
	    		FatalError("Error writing");
	    	}
			inMiddleOfTurn = 1;

	    }
	    else if (strcmp("SUCCESSFUL_MOVE_CHECK_W",RecvBuf) == 0)
	    {
	    	printf("Valid move. White player is in check\n");

	    	memset(SendBuf,0,256);
	    	strncpy(SendBuf,"OK3",sizeof(SendBuf)-1);
	    	#ifdef DEBUG
	    	printf("Now sending confirmation %s\n",SendBuf);
	    	#endif
	    	n  =  write(SocketFD, SendBuf, sizeof(SendBuf)-1);
	    	if(n<0)
	    	{
	    		FatalError("Error writing to socket");
	    	}
	    }
	    else if (strcmp("SUCCESSFUL_MOVE_CHECK_B",RecvBuf) == 0)
	    {
	    	printf("Valid move. Black player is in check\n");
	    	
	    	memset(SendBuf,0,256);
	    	strncpy(SendBuf,"OK3",sizeof(SendBuf)-1);
	    	#ifdef DEBUG
	    	printf("Now sending confirmation %s\n",SendBuf);
	    	#endif
	    	n  =  write(SocketFD, SendBuf, sizeof(SendBuf)-1);
	    	if(n<0)
	    	{
	    		FatalError("Error writing to socket");
	    	}
	    }
	    else if (strcmp("BLACK_WHITE",RecvBuf) == 0)
	    {
	    	printf("Would you like to be the black player or white player? (W/B, capital letter only)\n");
	    	char c;
	    	c = getchar();
	    	char j;
	    	while((j = getchar()) != EOF && j != '\n'); // This will eat up all other characters
	    	memset(SendBuf,0,256);
	    	SendBuf[0] = c;
	    	#ifdef DEBUG
	    	printf("Sending %s\n", SendBuf);
	    	#endif
    		n = write(SocketFD,SendBuf,sizeof(SendBuf)-1);
    		if(n<0)
    		{
    			FatalError("Writing to socket failed");
    		}
    		inMiddleOfTurn = 1;
    		inGame = 1;
	    }
	    else if (strcmp("RPS_REQUEST",RecvBuf) == 0)
	    {
	    	printf("Enemy player wants your color!\nPlay an additional game of RPS to be the color you selected (Enter R/P/S)\n");
	    	char c;
	    	c = getchar();
	    	char j;
	    	while((j = getchar()) != EOF && j != '\n'); // This will eat up all other characters
	    	memset(SendBuf,0,256);
	    	SendBuf[0] = c;
	    	#ifdef DEBUG
	    	printf("Sending %s\n", SendBuf);
	    	#endif
    		n = write(SocketFD,SendBuf,sizeof(SendBuf)-1);
    		if(n<0)
    		{
    			FatalError("Writing to socket failed");
    		}
    		inMiddleOfTurn = 1;
	    }
	    else if (strcmp("YOU_ARE_B",RecvBuf) == 0 || strcmp("YOU_ARE_W",RecvBuf) == 0)
	    {
	    	printf("Winner decided! You are %s player\n", RecvBuf[8] == 'W' ? "White" : "Black");
	    	
	    	//memset(SendBuf,0,256);
	    	strcpy(SendBuf, "OK4");
	    	#ifdef DEBUG
	    	printf("Sending %s\n", SendBuf);
	    	#endif
    		n = write(SocketFD,SendBuf,sizeof(SendBuf)-1);
    		//n = write(SocketFD,SendBuf,sizeof(SendBuf)-1);
    		if(n<0)
    		{
    			FatalError("Writing to socket failed");
    		}
    		#ifdef DEBUG
    		printf("Sent %s\n",SendBuf);
    		#endif
    		inMiddleOfTurn = 1;
	    }
	    else if (RecvBuf[0] == '-')
	    {

	    	printf("Message from opponent recieved:\n\"%s\"\n",RecvBuf);
	    	strcpy(SendBuf,"OK6");
	    	#ifdef DEBUG
	    	printf("Sending %s\n",SendBuf);
	    	#endif
	    	n = write(SocketFD,SendBuf,sizeof(SendBuf)-1);
	    	if(n<0)
	    	{FatalError("Writign to socket failed");
	    	}

	    }
	    else
	    {
	    	#ifdef DEBUG
	    	printf("Message from Server: %s\n", RecvBuf);//else, print the displayed message
	    	#endif
	    	n = write(SocketFD,"OK",sizeof(SendBuf)-1);
	    }



	    //------------------------------------------------------------------
	    //END OF SECTION TO INTERPRET RESPONSE

#ifdef DEBUG
	    //printf("%s: Closing the connection...\n", Program);
#endif
	    /*shutdown(SocketFD, SHUT_WR);
	    int res;
	    for(;;) 
	    {
        	res=read(SocketFD, SendBuf, 256);
        	if(res < 0) {
        	    perror("reading");
        	    exit(1);
        	}
        	if(!res)
        	    break;
    	}	*/

	    if(!inMiddleOfTurn)
	    {
	    	close(SocketFD);
		}
	}
    } while(0 != strcmp("SHUTDOWN", SendBuf));
    printf("%s: Exiting...\n", Program);
    return 0;
}
























/* EOF ClockClient.c */













