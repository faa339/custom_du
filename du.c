/* 
Author: Faris Alotaibi

Description: This is a program implementing 
the utility du. Usage is as follows:

./du [directoryname]

*/
#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>


void ErrorHandler(void);
int DirTraverse(char* dirname);
int IsSLink(char* dirname);

int main(int argc, char** argv)
{
	errno = 0;
	if(argc>2)
	{
		fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	char *directoryname = ".";
	if(argc==2)
	{
		directoryname = argv[1];
		if(IsSLink(directoryname))
		{
			printf("0\t%s\n",directoryname);
			exit(EXIT_SUCCESS);
		};	
	};

	int directorysize = DirTraverse(directoryname);
	printf("%d\t%s\n", directorysize, directoryname);
	exit(EXIT_SUCCESS);
}

void ErrorHandler(void)
{
	perror("du Error");
	exit(EXIT_FAILURE);
}

int IsSLink(char* dirname)
{
	struct stat testBuf;
	lstat(dirname, &testBuf);
	if(S_ISLNK(testBuf.st_mode)) 
		return 1;

	return 0;
}

int DirTraverse(char* dirname)
{
	DIR *directptr;
	struct dirent *direntryptr;
	struct stat statBuf;
	char temp[PATH_MAX];
	int totfilesize=0, dirsize = 0,stattest=0, dircount = 0;
	//Base case: we've tried opening something that isnt a directory
	if((directptr = opendir(dirname)) == NULL)
	{
		stattest=lstat(dirname, &statBuf);
		if(stattest<0) ErrorHandler();
		if(S_ISLNK(statBuf.st_mode))
			return 0;
		return statBuf.st_blocks/2;
	}
	//Get the directory's size first before getting its entries sizes
	stattest = lstat(dirname, & statBuf);
	if(stattest<0) ErrorHandler();
	totfilesize+= statBuf.st_blocks/2;

	while((direntryptr = readdir(directptr)) != NULL)
	{
		if(strcmp(direntryptr->d_name, "..") != 0
			&& (strcmp(direntryptr->d_name,".") != 0))
		{
			strcpy(temp, dirname);
			strcat(temp, "/");
			strcat(temp, direntryptr->d_name);
			stattest=lstat(temp, &statBuf);
			if (stattest<0) ErrorHandler();


			if (S_ISDIR(statBuf.st_mode)) 
			{
				dirsize = DirTraverse(temp);
				dircount++;
				printf("%d\t%s\n",dirsize, temp);

			}
			totfilesize+=(statBuf.st_blocks/2) + dirsize;
		}
	}
	closedir(directptr);
	return totfilesize - (4*dircount);
}