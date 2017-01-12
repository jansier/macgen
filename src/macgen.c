/*
	Author Jan Sierpina
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filestack.h"
#include "includedirective.h"
#include "definedirective.h"
#include "stringhelper.h"

#define RED "\x1b[31m"
#define WHITE "\x1b[0m"
#define BLUE "\x1b[34m"

struct File *fileStack;
char *buffer;
char *output;
int bufferSize;
int bufferWritten;
int linesWritten=0;

void handleIncludeDirective(const char *line, int length, int lineNum, const char *basePath);
void handleDefineDirective(const char *line, int length, int lineNum);
void readFile(const char *path);
void errorFindLine(const char *path, const int lineEr, char *er);
void handleFindLineInclude(const char *line, int length, int lineNum, const int lineEr, char *erText, const char *basePath);
void error(const char *text, int lineNum, const char *line);
void writeOut(const char *line, int length);
void errorFile(const char *filePath);

int main(int argc, char *args[]) {
	if(argc != 2) {
		printf("Nieprawidlowa liczba parametrow\n");
	} else {
		bufferSize = 1024;
		bufferWritten = 0;
		buffer = malloc(bufferSize);
		char *filePath;
		filePath = malloc(sizeof(args[1]));
		strcpy(filePath, args[1]);
		
		fileStack = newFileStack();
		pushFile(fileStack, filePath, 0);
		readFile(filePath);
		popFile(fileStack);
		
		char *er;
		int lineEr;
		lineEr=resolveDefineDirectives(&er, &output, buffer, bufferWritten, 1);
		if(lineEr==0)
			printf("%s", output);
		else {
			bufferSize = 1024;
			bufferWritten = 0;
			buffer = malloc(bufferSize);
			linesWritten=0;
			fileStack = newFileStack();
			pushFile(fileStack, filePath, 0);
			
			errorFindLine(filePath, lineEr, er);
			popFile(fileStack);
		}
	}
	
	return 0;
}

void handleIncludeDirective(const char *line, int length, int lineNum, const char *basePath) {
	int lineEr;
	char *er;
	char *includedFilePath;
	lineEr = getIncludedFilePath(&er, &includedFilePath, line, (int)length);
	
	if(lineEr == 0) {
		pushFile(fileStack, includedFilePath, lineNum);
		char *filePath;
		getFullPath(&filePath, basePath, includedFilePath);
		readFile(filePath);
		popFile(fileStack);
	} else {
		error(er, lineNum, line);
	}
}

void handleDefineDirective(const char *line, int length, int lineNum) {
	int lineEr;
	char *er;
	lineEr = addDirective(&er, line, length, linesWritten);
	if(lineEr != 0) {
		error(er, lineNum, line);
	}
}

void readFile(const char* path) {
	char *basePath;
	getBasePath(&basePath, path);
	
	FILE *file = fopen(path, "r");
    char *line=NULL;
    size_t bufferSize;
    ssize_t length;
	
    if (file == NULL) {
		errorFile(path);
	}

	int lineNum=1;
    while ((length = getline(&line, &bufferSize, file)) != -1) {
        if(isDirective(line)) {
			if(isIncludeDirective(line, length)) {
				handleIncludeDirective(line, length, lineNum, basePath);
			} else if (isDefineDirective(line, length)) {
				handleDefineDirective(line, length, lineNum);
			} else {
				error("Nierozpoznana dyrektywa", lineNum, line);
			}
		} else {
			writeOut(line, length);
		}
		
		lineNum++;
    }
    fclose(file);
}

void handleFindLineInclude(const char *line, int length, int lineNum, const int lineEr, char *erText, const char *basePath) {
	int result;
	char *er;
	char *includedFilePath;
	result = getIncludedFilePath(&er, &includedFilePath, line, (int)length);
	
	if(result == 0) {
		pushFile(fileStack, includedFilePath, lineNum);
		char *filePath;
		getFullPath(&filePath, basePath, includedFilePath);
		errorFindLine(filePath, lineEr, erText);
		popFile(fileStack);
	} else {
		error(er, lineNum, line);
	}
}
void errorFindLine(const char *path, const int lineEr, char *er) {
	char *basePath;
	getBasePath(&basePath, path);
	
	FILE *file = fopen(path, "r");
    char *line=NULL;
    size_t bufferSize;
    ssize_t length;
		
    if (file == NULL) {
		errorFile(path);
	}

	int lineNum=1;
    while ((length = getline(&line, &bufferSize, file)) != -1) {
        if(isDirective(line)) {
			if(isIncludeDirective(line, length)) {
				handleFindLineInclude(line, length, lineNum, lineEr, er, basePath);
			} else if (isDefineDirective(line, length)) {
				handleDefineDirective(line, length, lineNum);
			} else {
				error("Nierozpoznana dyrektywa", lineNum, line);
			}
		} else {
			writeOut(line, length);
			if(linesWritten==lineEr) {
				error(er, lineNum, line);
			}
		}
		
		lineNum++;
    }
    fclose(file);
}

void error(const char *text, int lineNum, const char *line) {
	printf(RED "Blad!: %s\n", text);
	printf(WHITE "%s", line);
	printf(BLUE "Wiersz %d w: ", lineNum);
	printStack(fileStack);
	exit(0);
}

void errorFile(const char *filePath) {
	printf(RED "Blad!: Nie udalo sie otworzyc pliku ");
	printf(BLUE "%s\n", filePath);
	printStack(fileStack);
	exit(0);	
}

void writeOut(const char *line, int length) {
	while(bufferWritten+length>=bufferSize) {
		bufferSize*=2;
		buffer = realloc(buffer, bufferSize);
	}
	strcat(buffer, line);
	bufferWritten+=length;
	linesWritten++;
}
