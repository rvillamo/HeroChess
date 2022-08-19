/* server2.c: simple interactive TCP/IP server program
 * based on: http://www.linuxhowtos.org/data/6/server.c
 * extended by Rainer Doemer, 2/11/15
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
/* #include "database.h" */

/* #define DEBUG */	/* be very verbose */

/********** function declarations **********/
void FatalError(const char *Program, const char *ErrorMsg);
int appendUser(char username[100]);
int appendPass(char password[100]);
int checkUser(char user[100]);
/* int checkPass(char user[100], char pass[100]); */

/********** main function **********/
int main(int argc, char *argv[])
{
	/********** socket **********/
    int l, n, next;
    int ServSocketFD,	/* socket file descriptor for service */
	DataSocketFD,	/* socket file descriptor for data */
	PortNo;		/* port number */
    socklen_t ClientLen;
    struct sockaddr_in
	ServerAddress,	/* server address (this host) */
	ClientAddress;	/* client address we connect with */
    char RecvBuf[256];	/* message buffer for receiving a message */
    char SendBuf[256];	/* message buffer for sending a response */
    int shutdown = 0, new = 0,
	reg = 0;

    printf("%s: Starting...\n", argv[0]);
    if (argc < 2)
    {   fprintf(stderr, "Usage: %s port\n", argv[0]);
	exit(10);
    }
    PortNo = atoi(argv[1]);	/* get the port number */
#ifdef DEBUG
    printf("%s: Using port %d...\n", argv[0], PortNo);
#endif
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be greater 2000\n",
		argv[0], PortNo);
        exit(10);
    }
#ifdef DEBUG
    printf("%s: Creating a socket to serve clients...\n", argv[0]);
#endif
    ServSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (ServSocketFD < 0)
    {   FatalError(argv[0], "service socket creation failed");
    }
#ifdef DEBUG
    printf("%s: Preparing the server address...\n", argv[0]);
#endif
    memset(&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
#ifdef DEBUG
    printf("%s: Assigning the server name to the socket...\n", argv[0]);
#endif
    if (bind(ServSocketFD, (struct sockaddr*)&ServerAddress,
		sizeof(ServerAddress)) < 0)
    {   FatalError(argv[0], "binding the server to a socket failed");
    }
    printf("%s: Listening on port %d...\n", argv[0], PortNo);
    if (listen(ServSocketFD, 5) < 0)	/* max 5 clients in backlog */
    {   FatalError(argv[0], "listening on socket failed");
    }
	/********** end of socket code **********/

	/********** code below relates to login/registration **********/
    do{ new = 0;
	reg = 0;
	next = 0;
	ClientLen = sizeof(ClientAddress);
	DataSocketFD = accept(ServSocketFD, (struct sockaddr*)&ClientAddress,
		&ClientLen);
	if (DataSocketFD < 0)
	{   FatalError(argv[0], "data socket creation (accept) failed");
	}
	printf("%s: Accepted connection from client.\n", argv[0]);
#ifdef DEBUG
	printf("%s: Client address:port = %u:%hu.\n", argv[0],
			ClientAddress.sin_addr.s_addr, ntohs(ClientAddress.sin_port));
#endif
	 do{ shutdown = 0; 
	    n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
	    if (n < 0) 
	    {   FatalError(argv[0], "reading from data socket failed");
	    }
	    RecvBuf[n] = 0;
	    printf("%s: Received message: %s\n", argv[0], RecvBuf);
	
	/********** new user welcome **********/
	    if (0 == strcmp(RecvBuf, "1"))
	    {   new = 1;
#ifdef DEBUG
		printf("%s: Received '1' message from client.\n", argv[0]);
#endif
		strncpy(SendBuf, "Welcome, nice to meet you!", sizeof(SendBuf)-1);
		SendBuf[sizeof(SendBuf)-1] = 0;
		
	    }
	
	/********** registered user welcome **********/
	    else if (0 == strcmp(RecvBuf, "2"))
	    {   reg = 1;
#ifdef DEBUG
		printf("%s: Received '2' message from client.\n", argv[0]);
#endif
		strncpy(SendBuf, "Welcome!", sizeof(SendBuf)-1);
		SendBuf[sizeof(SendBuf)-1] = 0;
	    }		


	/********** shutdown server (exit) **********/
	    else if (0 == strcmp(RecvBuf, "3"))
	    {   shutdown = 1;
#ifdef DEBUG
		printf("%s: Received '3' message from client.\n", argv[0]);
#endif
		strncpy(SendBuf, "server shutdown", sizeof(SendBuf)-1);
		SendBuf[sizeof(SendBuf)-1] = 0;
	    }

	    else
	    {   strncpy(SendBuf, "server echo ", sizeof(SendBuf)-1);
		SendBuf[sizeof(SendBuf)-1] = 0;
		strncat(SendBuf, RecvBuf, sizeof(SendBuf)-1-strlen(SendBuf));
	    }
	    l = strlen(SendBuf);

	/* send message to client(s) */
	    printf("%s: Sending response: %s.\n", argv[0], SendBuf);
	    n = write(DataSocketFD, SendBuf, l);

	    if (n < 0)
	    {   FatalError(argv[0], "writing to data socket failed");
	    }	


	} while((new == 0) && (reg == 0) && (shutdown == 0));

	/********** handling a new user **********/
	while (((new == 1) && (reg == 0) && (shutdown == 0)) || next == 0)
	{
		n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
		if (n < 0) 
		{   FatalError(argv[0], "reading from data socket failed");
		}

		/* username verification handling */
		int verified; /* 0 indicates the name not being verified, vice versa */
		printf("%s: Received username: %s\n", argv[0], RecvBuf);
	
		verified = checkUser(RecvBuf);

		/* if username doesn't already exist in the database: */
		if(verified == 0) 
		{
			/* appending username to the database */
			appendUser(RecvBuf);	
	#ifdef DEBUG
			printf("%s: Username has successfully been added to the database!\n", argv[0]);
	#endif
			strncpy(SendBuf, "Username has been verified!", sizeof(SendBuf)-1);
			SendBuf[sizeof(SendBuf)-1] = 0;
			printf("%s: Sending response: %s.\n", argv[0], SendBuf);
			n = write(DataSocketFD, SendBuf, l);
			if (n < 0)
			{   
				FatalError(argv[0], "writing to data socket failed");
			}
			next = 1;
		}

		/* if username already exists in the database: */
		if(verified != 0) 
		{
	#ifdef DEBUG
			printf("%s: Username has not been added to the database!\n", argv[0]);
	#endif
			strncpy(SendBuf, "Username already exists!", sizeof(SendBuf)-1);
			SendBuf[sizeof(SendBuf)-1] = 0;
			printf("%s: Sending response: %s.\n", argv[0], SendBuf);
			n = write(DataSocketFD, SendBuf, l);
			if (n < 0)
			{   
				FatalError(argv[0], "writing to data socket failed");
			}
		}

		RecvBuf[n] = 0;
		
	}

	if (next == 1)
	{
		n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
		if (n < 0) 
		{   FatalError(argv[0], "reading from data socket failed");
		}
#ifdef DEBUG
		printf("%s: Received password: %s\n", argv[0], RecvBuf);
#endif
		/* password append handling */
		appendPass(RecvBuf);	
			
#ifdef DEBUG
		printf("Password has successfully been added to the database!\n");
#endif
		strncpy(SendBuf, "Password received!", sizeof(SendBuf)-1);
		SendBuf[sizeof(SendBuf)-1] = 0;
		printf("%s: Sending response: %s.\n", argv[0], SendBuf);
		n = write(DataSocketFD, SendBuf, l);
		if (n < 0)
		{   
			FatalError(argv[0], "writing to data socket failed");
		}
		next = 0;
		shutdown = 1;
	}
	
	/********** handling a registered user **********/
	if ((new == 0) && (reg == 1) && (shutdown == 0))
	{
		n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
		if (n < 0) 
		{   FatalError(argv[0], "reading from data socket failed");
		}

		/* username verification handling */
		RecvBuf[n] = 0;
		printf("%s: Received username: %s\n", argv[0], RecvBuf);
		
#ifdef DEBUG
		printf("%s: Username has successfully been added to the database!\n", argv[0]);
#endif

		n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
		if (n < 0) 
		{   FatalError(argv[0], "reading from data socket failed");
		}

		/* username and password verification handling */
		RecvBuf[n] = 0;
		printf("Received password\n");
		
#ifdef DEBUG
		printf("Password has successfully been added to the database!\n");
#endif
	}


	/* closing the socket */
	printf("%s: Received last message from client, closing data connection.\n", argv[0]);
	close(DataSocketFD);

    } while(shutdown == 0);

    printf("%s: Shutting down.\n", argv[0]);
    close(ServSocketFD);
    return 0;
}

/********** function definitions **********/
void FatalError(const char *Program, const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!", stderr);
    exit(20);
} /* end of FatalError */

int appendUser(char username[100])
{
	FILE *fp1;
	
	fp1 = fopen("record.txt", "a+");
	if(fp1 == NULL)
	{
		printf("error in opening file: \n");
		return 1;
	}

	fprintf(fp1, "Username: %s\n", username);
	fclose(fp1);
	return 0;
}

int appendPass(char password[100])
{
	FILE *fp1;

	fp1 = fopen("record.txt", "a+");
	if(fp1 == NULL)
	{
		printf("Error opening file\n");
		return 1;		
	}

	fprintf(fp1, "Password: %s\n\n", password);
	fclose(fp1);
	return 0;
}

int checkUser(char user[100])
{
	char line[301];
	int lineNum = 1;
	int found;
	
	FILE *fp1 = fopen("record.txt", "r");
	if (fp1 == NULL)
	{
		printf("Error! File missing\n");
		exit (10);
	}

        while(fgets(line, 300, fp1) != NULL)
        {	
	
		if (strstr(line, user) != NULL)
		{
			printf("User exists line %d\n", lineNum);
			found = lineNum;
		}

		lineNum++;
		if (strstr(line, user) == NULL)
		{
			/* printf("User does not exist\n"); */
			found = 0;
		}
	}
	fclose(fp1);
	return found;	
}
/*
int checkPass(char user[100], char pass[100])
{
	char line[301];
        int passLine = 1;

        FILE *fp1 = fopen("record.txt", "r");
        if (fp1 == NULL)
        {
                printf("Error! File missing\n");
                exit(10);
        }

        lineNum = checkUser(user);

        while(fgets(line, 300, fp1) != NULL)
                if ((strstr(line, pass) != NULL) && (passLine == (lineNum + 1)))
                {
                        printf("Welcome line %d\n", passLine);
                        return passLine;
                }

                passLine++;

                if ((strstr(line, pass) == NULL) || (passLine != (lineNum + 1)))
                {
                        printf("wrong\n");

                }
        }
        fclose(fp1);
        return 0;	
}
*/

/* EOF server2.c */

