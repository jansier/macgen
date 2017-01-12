/*
	Author Jan Sierpina
*/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "stringhelper.h"
#include "definedirective.h"

struct Directive {
	struct Directive *previous;
	char *name;
	char *definition;
	char *parameters[16];
	int defLine; // defined in line in processed file
	int prNmb; // number of parameters
};

void addToDirectives(struct Directive *directive);
void printDirective(const struct Directive *directive);
int updateLine(int line, const char *begin, const char *end);
struct Directive *getDirective(const char *name, int line);
int resolveParameters(char **er, char **output, struct Directive *directive, const char *insideBrackets, int line);
int resolves=0;

struct Directive *directives=NULL;

bool isDefineDirective(const char *line, int length) {
	const char *firstChar;
	firstChar = pointFirstChar(line);
	
	if(firstChar+DEF_LEN<=line+length && memcmp(firstChar, "#define", DEF_LEN)==0) {
		const char *termChar=firstChar+DEF_LEN;
		if(*termChar==SPACE || *termChar == TAB || *termChar == ENDLINE) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

int resolveParameters(char **er, char **output, struct Directive *directive,
		const char *insideBrackets, int line) {
	int outputWritten=0;
	int writeOut;
	int outputSize=32;
	*output=malloc(outputSize);
	char *parameters[directive->prNmb];
	int prFound=0;
	const char *parBegin=insideBrackets;
	
	const char *comma = strchr(parBegin, ',');
	while(comma!=NULL && prFound<directive->prNmb) {
		cpandtrm(&parameters[prFound], parBegin, comma-parBegin);
		prFound++;
		parBegin=comma+1;
		comma = strchr(parBegin, ',');
	}
	if(prFound==directive->prNmb-1) {
		cpandtrm(&parameters[prFound], parBegin, strlen(parBegin));
		prFound++;
	} else {
		*er = "Nieprawidlowa liczba parametrow";
		return line;
	}
	char *buffer;
	
	int lineEr = resolveDefineDirectives(er, &buffer, directive->definition,
					strlen(directive->definition), line);
	if(lineEr != 0) {
		 // blad w wywolaniu makra w definicji
		return lineEr;
	} else {
		const char *point=buffer;
		int bufferLength = strlen(buffer);
		
		while(point < buffer+bufferLength) {
			char *name;
			const char *nameBegin, *nameEnd;
			int lengthToEnd=(buffer+bufferLength)-point;
			getFirstName(point, &nameBegin, &nameEnd, lengthToEnd);
			
			writeOut=nameBegin-point;
			memsafecpy(output, &outputWritten, &outputSize, point, writeOut);
			point = nameBegin;
			int nameLength=nameEnd-nameBegin;
			cpandtrm(&name, nameBegin, nameLength);
			int par=-1;
			for(int i=0; i<directive->prNmb; i++) {
				if(memcmp(directive->parameters[i], name, nameLength)==0) {
					par=i;
					break;
				}
			}
			if(par==-1 || nameLength==0) { // name to nie parametr, przepisuje name
				writeOut=nameEnd-nameBegin;
				memsafecpy(output, &outputWritten, &outputSize, point, writeOut);
			} else { // name to ktorys z parametrow, podstawiam parametr
				writeOut=strlen(parameters[par]);
				memsafecpy(output, &outputWritten, &outputSize, parameters[par], writeOut);
			}
			point = nameEnd;
		}
	}
	
	*(*output+outputWritten)='\0';
					
	return 0;
}

int resolveDefineDirectives(char **er, char **output, const char *buffer, int bufferLength, int line) {
	if(resolves>1000) {
		*er = "Nastapila petla wywolan dyrektywy define";
		return line;
	}
	resolves++;
	
	int outputSize=32;
	int outputWritten=0;
	*output=malloc(outputSize);
	
	const char *point=buffer;
	while(point < buffer+bufferLength) {
		char *name;
		const char *nameBegin, *nameEnd;

		int lengthToEnd=(buffer+bufferLength)-point;
		getFirstName(point, &nameBegin, &nameEnd, lengthToEnd);
		
		int writeOut=nameBegin-point;
		memsafecpy(output, &outputWritten, &outputSize, point, writeOut);
		line = updateLine(line, point, nameBegin);
		point = nameBegin;
		
		int nameLength=nameEnd-nameBegin;
		cpandtrm(&name, nameBegin, nameLength);
		
		struct Directive *directive;
		directive = getDirective(name, line);
		
		if(directive!=NULL) { // point wskazuje na namebegin
			if(directive->prNmb==-1) { //dyrektywa bez nawiasow
				writeOut=strlen(directive->definition);
				memsafecpy(output, &outputWritten, &outputSize, directive->definition, writeOut);
				point = nameEnd;
			} else {// dyrektywa z nawiasami
				const char *openBracket, *closeBracket;
				openBracket = pointFirstCharInBlock(nameEnd, buffer+bufferLength);
				if(openBracket == NULL || *openBracket != '(') {
					// brak otwierajacego nawiasu jednak to nie wywolanie dyr
					writeOut=nameEnd-nameBegin;
					memsafecpy(output, &outputWritten, &outputSize, point, writeOut);
					point = nameEnd;
				} else {
					closeBracket = findClosingBracket(openBracket, buffer+bufferLength);
					if(closeBracket == NULL) {
						*er = "Brak nawiasu zamykajacego )";
						return line; // nie znaleziono zamykajacego
					} else if(directive->prNmb==0) { // dyrektywa z 0 parm
						if(pointFirstCharInBlock(openBracket+1, closeBracket+1)!=closeBracket) {
							*er = "Nieprawidlowa liczba parametrow";
							return line;
						} else {
							writeOut=strlen(directive->definition);
							memsafecpy(output, &outputWritten, &outputSize, directive->definition, writeOut);
						}
					} else { // dyrektywa z wieloma par
						char *insideBrackets;
						resolveDefineDirectives(er, &insideBrackets, openBracket+1,
							closeBracket-openBracket-1, line);
						
						char *processed;
						
						int lineEr=resolveParameters(er, &processed, directive, insideBrackets, line);
						
						if(lineEr == 0) {
							writeOut=strlen(processed);
							memsafecpy(output, &outputWritten, &outputSize, processed, writeOut);
						} else {
							// er okreslone przez wywolanie resolveParameters
							return lineEr;
						}
					}
					point = closeBracket+1;
				}
			}
			
			lengthToEnd=(buffer+bufferLength)-point;
		} else { // wypisz nazwe skoro nie ma takiej dyrektywy
			writeOut=nameEnd-nameBegin;
			memsafecpy(output, &outputWritten, &outputSize, point, writeOut);
			point = nameEnd;
		}
	}
	*(*output+outputWritten)='\0';
	
	return 0;
}

int addDirective(char **er, const char *line, int length, int defLine) {
	struct Directive *directive= malloc(sizeof(struct Directive));
	
	if(!isDefineDirective(line, length)) {
		*er = "Nieoczekiwany blad";
		return 1;
	} else {
		char const *nameBegin, *directiveBegin;
		directiveBegin = pointFirstChar(line);
		nameBegin = pointFirstChar(directiveBegin+DEF_LEN);
		char const *nameEnd = nameEnds(nameBegin);

		int nameLength=nameEnd-nameBegin;
		cpandtrm(&directive->name, nameBegin, nameLength);
		
		if(nameLength==0) {
			*er = "Nieprawidlowa nazwa dyrektywy define";
			return 1;
		/* definicja bez parametrow */
		} else if(*nameEnd==SPACE || *nameEnd==TAB || *nameEnd==ENDLINE) {
			const char *def_begin=pointFirstChar(nameEnd);
			int definitionLength=(line+length)-def_begin-1; // definicja jest bez zakonczenia wiersza
			if(definitionLength<0)//pusta definicja
				definitionLength=0;
			cpandtrm(&directive->definition, def_begin, definitionLength);
			directive->prNmb = -1; // dyrektywa bez parm, i bez nawiasow
		/* definicja z parametrami */
		} else if(*nameEnd=='(') {
			directive->prNmb = 0; // dyrektywa bez parametrow ale z nawias
			
			const char *openBracket, *closeBracket;
			openBracket=nameEnd;
			closeBracket = strchr(openBracket, ')');
			if(closeBracket == NULL) {
				*er = "Brak nawiasu zamykajacego )";
				return 1;
			} else { // nawiasy ok
				const char *def_begin=pointFirstChar(closeBracket+1);
				int definitionLength=(line+length)-def_begin-1; // definicja jest bez zakonczenia wiersza
				if(definitionLength<0) // pusta definicji
					definitionLength=0;
				
				cpandtrm(&directive->definition, def_begin, definitionLength);
		
				if(*pointFirstChar(openBracket+1)==')') {
					//ok
				} else {
					const char *blockBegin=openBracket; // bede analizowal bloki oddzielone przecinkami
					const char *blockEnd=memchr(blockBegin+1, ',', closeBracket-blockBegin-1);
					if(blockEnd==NULL)
						blockEnd=closeBracket;
					while(blockBegin!=blockEnd) {
						const char *parBegin=pointFirstChar(blockBegin+1);
						const char *parEnd=nameEnds(parBegin);
						int parLength=parEnd-parBegin;
							
						if(parLength==0) {
							*er = "Nieprawidlowy znak rozpoczynajacy nazwe parametru";
							return 1;
						} else if(pointFirstChar(parEnd)==blockEnd) { // znalazlem prawidlowy parametr
							cpandtrm(&directive->parameters[directive->prNmb], parBegin, parLength);
							directive->prNmb++;
						} else {
							*er = "Nazwa parametru zawiera niedozwolone znaki";
							return 1;
						}
						
						blockBegin=blockEnd;
						blockEnd=memchr(blockBegin+1, ',', closeBracket-blockBegin);
						if(blockEnd==NULL)
							blockEnd=closeBracket;
					}
				}
			}
		} else {
			*er = "Nieprawidlowa nazwa dyrektywy define";
			return 1;
		}
	}
	directive->defLine = defLine;
	if(getDirective(directive->name, defLine)!=NULL) {
		*er = "Dyrektywa o tej samej nazwie juz zdefiniowana";
		return 1;
	} else {
		addToDirectives(directive);
		return 0;
	}
}

struct Directive *getDirective(const char *name, int line) {
	if(directives==NULL || strlen(name)==0) {
		return NULL;
	} else {
		struct Directive *directive;
		directive = directives;
		do {
			directive=directive->previous;
		} while(strcmp(directive->name, name)!=0 && directive != directives);
		
		if(strcmp(directive->name, name)==0 && directive->defLine < line) {
			return directive;
		} else {
			return NULL;
		}
	}
}

void printDirectives() {
	if(directives==NULL) {
		printf("Brak dyrektyw\n");
	} else {
		struct Directive *directive;
		directive = directives;
		do {
			printDirective(directive->previous);
			directive=directive->previous;
		} while(directive != directives);
	}
}

void printDirective(const struct Directive *directive) {
	printf("Dyrektywa wiersz %d|%s|%s|%d Parametry:", directive->defLine, directive->name, directive->definition, directive->prNmb);
	for(int i=0; i<directive->prNmb; i++)
		printf(" %s,", directive->parameters[i]);
	printf("\n");
}

void addToDirectives(struct Directive *directive) {
	if(directives==NULL) {
		directives=directive;
		directives->previous = directive;
	} else {
		directive->previous = directives->previous;
		directives->previous = directive;
	}
}

int updateLine(int line, const char *begin, const char *end) {
	while(begin<end) {
		if(*begin==ENDLINE)
			line++;
		begin++;
	}
	return line;
}
