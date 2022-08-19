/* database program */
// currently buggy; doesn't verify user correctly

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void appendUser(char username[100]);
void appendPass(char password[100]);
int checkUser(char user[100]);
int checkPass(int lineNum, char pass[100]);
		
void appendUser(char username[100])
{
	FILE *fp1;
	
	fp1 = fopen("bin/record.txt", "a+");
	/*if(fp1 == NULL)
	{
		printf("error in opening file: \n");
		return 1;
	}*/

	fprintf(fp1, "Username: %s", username);
	fclose(fp1);
}

void appendPass(char password[100])
{
	FILE *fp1;

	fp1 = fopen("bin/record.txt", "a+");
	/*if(fp1 == NULL)
	{
		printf("Error opening file\n");
		return 1;		
	}*/

	fprintf(fp1, "\nPassword: %s\n\n", password);
	fclose(fp1);
}

int checkUser(char user[100])
{
	char line[301];
	int lineNum = 1;
	int found;
	
	FILE *fp1 = fopen("bin/record.txt", "r");
	if (fp1 == NULL)
	{
		printf("Error! File missing\n");
		exit (10);
	}

        while(fgets(line, 300, fp1) != NULL)
        {	
	
		if (strstr(line, user) != NULL)
		{
			/* printf("User exists line %d\n", lineNum); */
			found = lineNum;
		}

		lineNum++;
		/* if (strstr(line, user) == NULL)
		{
			 printf("User does not exist\n"); 
			found = 0;
		} */
	}
	
	if ((found < 1) || (found > lineNum))
	{
		found= 0;
	}

	fclose(fp1);
	return found;	
}

int checkPass(int lineNum, char pass[100])
{
	char line[301];
	int found;
	int passLine = 1;

	FILE *fp1;
	fp1 = fopen("bin/record.txt", "r");
	
	while(fgets(line, 300, fp1) != NULL)
	{
		if ((strstr(line, pass) != NULL) && (passLine == (lineNum + 1)))
		{
			found = passLine;
		}
		
		passLine++;
	}

	if ((passLine < 1) || (found > passLine))
	{
		found = 0;
	}
	
	fclose(fp1);
	return found;
} 

// EOF //
