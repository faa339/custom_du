/* 
Author: Faris Alotaibi

Description: This is a program implementing 
the utility du. Usage is as follows:

./du [directoryname]

*/
#define _DEFAULT_SOURCE
#define MAX_SIZE 4096

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>



void ErrorHandler(void);
void DirTraverse(char* dirname);


int main(int argc, char** argv)
{
	errno = 0;
	if(argc>2)
	{
		fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	char *directoryname = ".";
	if(argc==2) directoryname = argv[1];

	DirTraverse(directoryname);
	exit(EXIT_SUCCESS);
}

void ErrorHandler(void)
{
	perror("du Error");
	exit(EXIT_FAILURE);
}

void DirTraverse(char* dirname)
{
	DIR *directptr;
	struct dirent *direntryptr;
	struct stat statBuf;
	char temp[MAX_SIZE];
	//Base case: we've tried opening something that isnt a directory
	if((directptr = opendir(dirname)) == NULL) 
		return;

	while((direntryptr = readdir(directptr)) != NULL)
	{
		if(strcmp(direntryptr->d_name, ".") != 0 &&
			strcmp(direntryptr->d_name, "..") != 0 )
		{
			strcpy(temp, dirname);
			strcat(temp, "/");
			strcat(temp, direntryptr->d_name);
			lstat(temp, &statBuf);
			
			if (S_ISDIR(statBuf.st_mode))
			{
				DirTraverse(temp);
				printf("%ld\t%s\n",(statBuf.st_blocks), temp);
			}

		}
	}
	closedir(directptr);
}
