/* 
Author: Faris Alotaibi

Description: This is a program implementing 
the utility du. Usage is as follows:

./du [directoryname]

*/
#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void ErrorHandler(void);
void DirTraverse(char* dirname);

char pathname[4096];

int main(int argc, char** argv)
{
	if(argc>2)
	{
		fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	errno = 0;
	char *directoryname = ".";
	if(argc == 2)
	{
		directoryname = argv[1];
	}
	
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

	if((directptr = opendir(dirname)) == NULL) ErrorHandler();

	while((direntryptr = readdir(directptr)) != NULL)
	{
		if(strcmp(direntryptr->d_name, ".") != 0 &&
			strcmp(direntryptr->d_name, "..") != 0 )
		{
			lstat(direntryptr->d_name,&statBuf);
			if (S_ISDIR(statBuf.st_mode))
			{
				strcpy(pathname, dirname);
				strcat(pathname,"/");
				strcat(pathname, direntryptr->d_name);
				printf("%ld\t%s\n",statBuf.st_size, pathname);
				DirTraverse(pathname);
			}
		}	
	}

	closedir(directptr);
}