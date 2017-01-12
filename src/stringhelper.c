/*
	Author Jan Sierpina
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stringhelper.h"

const char *pointFirstChar(const char *line) {
	const char *point=line;
	while((*point==SPACE || *point == TAB || *point == ENDLINE) && *point!='\0')
		point++;
	return point;
}

bool isNameChar(char c) {
	if(canBeginName(c) || (c>='0' && c <= '9'))
		return true;
	else
		return false;
}

bool canBeginName(char c) {
	if((c >= 'a' && c<='z') || (c >= 'A' && c <= 'Z') || c=='_')
		return true;
	else
		return false;
}

const char *nameEnds(const char *nameBegin) {
	char const *nameEnd = nameBegin;
	
	if(canBeginName(*nameBegin)) {
		while(isNameChar(*nameEnd))
			nameEnd++;
	}
	
	return nameEnd;
}

// copy and terminate - kopiuj i dodaj '\0' na koncu
void cpandtrm (char **dest, const char *begin, int length) { 
	*dest = malloc(length+1);
	strncpy(*dest, begin, length);
	*(*dest+length)='\0';
}

bool isDirective(const char *line) {
	const char *firstChar;
	firstChar = pointFirstChar(line);
	
	if(*firstChar == '#') {
		return true;
	} else {
		return false;
	}
}

void getFirstName(const char *blockStart, const char **begin, const char **end, int blockLength) {
	*begin=blockStart;
	while(!canBeginName(**begin) && *begin<blockStart+blockLength) {
		*begin=pointFirstChar(*begin+1);
	}
	*end=nameEnds(*begin);
	
	if(*end>blockStart+blockLength) {
		*begin=blockStart+blockLength;
		end=begin;
	}
}

const char *pointFirstCharInBlock(const char *begin, const char *end) {
	const char *point=begin;
	while((*point==SPACE || *point == TAB || *point == ENDLINE) && point<end)
		point++;
	if(point == end)
		return NULL;
	else
		return point;
}

const char *findClosingBracket(const char *begin, const char *end) {
	int numOpened=1;
	const char *point=begin;
	while(numOpened!=0 && point!=end) {
		point++;
		if(*point=='(')
			numOpened++;
		else if(*point==')')
			numOpened--;
	}
	if(point >= end)
		return NULL;
	else
		return point;
}

void memsafecpy(char **dest, int *destWritten, int *destSize, const char *begin, int length) {
	while(*destWritten+length>=*destSize) {
		*destSize*=2;
		*dest = realloc(*dest, *destSize);
	}
	memcpy(*dest+*destWritten, begin, length);
	*destWritten+=length;
}

void getBasePath(char **basePath, const char *path) {
	const char *endBase;
	endBase=strrchr(path, '/');
	if(endBase==NULL) {
		*basePath = "";
	} else {
		int length = endBase-path+1;
		cpandtrm(basePath, path, length);
	}
}

void getFullPath(char **dest, const char *basePath, const char *filePath) {
	int length=strlen(basePath)+strlen(filePath);
	*dest=malloc(length+1);
	**dest='\0';
	strcat(*dest, basePath);
	strcat(*dest, filePath);
}
