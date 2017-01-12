/*
	Author Jan Sierpina
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "filestack.h"

#define BLUE "\x1b[34m"

struct File {
	struct File *previous;
	char *path;
	int inclusionLine;
};

FileStack newFileStack() {
	FileStack fileStack;
	fileStack = malloc(sizeof(FileStack));
	fileStack->previous = NULL;
	return fileStack;
}

void printStack(FileStack fileStack) {
	if(fileStack->previous == NULL)
		printf("Stos jest pusty\n");
	else {
		FileStack point;
		point = fileStack->previous;
		printf(BLUE "%s\n", point->path);
		
		while(point != fileStack) {
			printf(BLUE ">Wiersz %d w: %s\n", point->inclusionLine, point->previous->path);
			point=point->previous;
		}
	}
}

void pushFile(FileStack fileStack, char* path, int inclusionLine) {
	if(fileStack->previous == NULL) {
		fileStack->previous = fileStack;
		fileStack->path = malloc(sizeof(path));
		strcpy(fileStack->path, path);
		fileStack->inclusionLine = inclusionLine;
	} else {
		struct File *file;
		file = malloc(sizeof(struct File));
		file->path = malloc(sizeof(path));
		file->inclusionLine = inclusionLine;
		strcpy(file->path, path);
		file->previous = fileStack->previous;
		fileStack->previous = file;
	}
}

void popFile(FileStack fileStack) {
	struct File *tmp = fileStack->previous;
	
	free(tmp->path);
	if(fileStack == tmp) {
		fileStack->previous = NULL;
		fileStack->inclusionLine = 0;
	} else {
		fileStack->previous=fileStack->previous->previous;
		free(tmp);
	}
}
