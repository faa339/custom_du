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
void errorPrint(char* errorstr, char* dirname);
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
	
	char *directoryname;
	if(argc==2)
	{
		directoryname = argv[1];
		if(strcmp(&directoryname[strlen(directoryname)-1], "/") == 0)
		{//Checking to see if given an extra / then replacing it
			directoryname[strlen(directoryname)-1] = 0;
		}
		if(IsSLink(directoryname))
		{
			printf("0\t%s\n",directoryname);
			exit(EXIT_SUCCESS);
		};	
	}else directoryname = ".";

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
	//Function to check if file is symbolic link
	struct stat testBuf;
	if((lstat(dirname, &testBuf)) < 0) ErrorHandler();
	if(S_ISLNK(testBuf.st_mode)) 
		return 1;
	return 0;
}

void errorPrint(char* errorstr, char* dirname)
{
	/*
	This function constructs an error string and prints it.
	This is specifically for EACCES error -- we don't have 
	permission to open the directory in question
	*/
	strcpy(errorstr, "du: Cannot read directory '");
	strcat(errorstr, dirname);
	strcat(errorstr, "': ");
	strcat(errorstr, strerror(errno));
	fprintf(stderr,"%s\n", errorstr);
}

int DirTraverse(char* dirname)
{
	/*This function traverses the filetree and adds up sizes of 
	each directory and their contents.
	We're always returning statBuf.st_blocks/2 because du blocksize 
	is different from regular system blocksize. */
	DIR *directptr;
	struct dirent *direntryptr;
	struct stat statBuf;
	char temp[PATH_MAX];
	int totsize=0, subdirsize = 0,stattest=0;
	
	//Check if we've got a directory to begin with
	//Could have been passed a regular file, symlink, or hardlink
	if((directptr = opendir(dirname)) == NULL)
	{
		//if we dont have permission to open the file, print the error
		if(errno == EACCES) errorPrint(temp, dirname);
		stattest=lstat(dirname, &statBuf);
		if(stattest<0) ErrorHandler();
		if(S_ISLNK(statBuf.st_mode))
			return 0;
		return (statBuf.st_blocks/2);
	}

	while((direntryptr = readdir(directptr)) != NULL)
	{ 	//Loop over all directory entries & add their sizes
		if(strcmp(direntryptr->d_name, "..") != 0
			&& strcmp(direntryptr->d_name, ".") != 0)
		{
			strcpy(temp, dirname);
			strcat(temp, "/");
			strcat(temp, direntryptr->d_name);
			stattest=lstat(temp, &statBuf);
			if (stattest<0) ErrorHandler();

			/*Important: had to separate the directory addition & the 
			regular addition to prevent a double-add logical issue*/
			if (S_ISDIR(statBuf.st_mode) && !(S_ISLNK(statBuf.st_mode))) 
			{
				subdirsize = DirTraverse(temp);
				printf("%d\t%s\n",subdirsize, temp);
				totsize+=subdirsize;
			}else totsize+=(statBuf.st_blocks/2);
		}
	}
	//Add the size of the current directory after all other entries
	if((lstat(".", &statBuf)) < 0) ErrorHandler();
	totsize += (statBuf.st_blocks/2);
	closedir(directptr);
	return totsize;
}