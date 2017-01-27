/*
	Author Jan Sierpina
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stringhelper.h"
#include "includedirective.h"

bool isIncludeDirective(const char *line, int length) {
	const char *firstChar;
	firstChar = pointFirstChar(line);
	
	if(firstChar+INCL_LEN<=line+length && memcmp(firstChar, "#include", INCL_LEN)==0) {
		const char *termChar=firstChar+INCL_LEN;
		if((*termChar==SPACE || *termChar == TAB || *termChar == ENDLINE) && *pointFirstChar(termChar)=='"') {
			return true;			
		} else {
			return false;
		}
	} else {
		return false;
	}
}

int getIncludedFilePath(char **er, char **includedFilePath, const char *line, int length) {
	if(!isIncludeDirective(line, length)) {
		*er = "Nieoczekiwany blad";
		return 1;
	} else {
		const char *openQuotion, *sharp;
		sharp = pointFirstChar(line);
		openQuotion = pointFirstChar(sharp+8);
		if(*openQuotion != QUOTION) {
			*er = "Brak otwierajacego \"";
			return 1;
		} else {
			char *termQuotion=memchr(openQuotion+1, QUOTION, strlen(openQuotion+1));
			if(termQuotion==NULL) {
				*er = "Brak domykajacego \"";
				return 1;
			} else {
				int pathLength;
				pathLength = termQuotion-openQuotion-1;
				*includedFilePath = malloc(pathLength+1);
				strncpy(*includedFilePath, openQuotion+1, pathLength);
				*(*includedFilePath+pathLength)='\0';
				return 0;
			}
		}
	}
}
