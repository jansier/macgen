/*
	Author Jan Sierpina
*/

#include <string.h>
#include <stdio.h>
#include "hashlist.h"
#define PRIME 701

struct Directive *directives[PRIME];
int hash(const char *name);

void addToDirectives(struct Directive *directive) {
	int h=hash(directive->name);
    
    if(directives[h]==NULL) {
        directives[h]=directive;
        directives[h]->previous = directive;
	} else {
        directive->previous = directives[h]->previous;
        directives[h]->previous = directive;
	}
}

struct Directive *getDirective(const char *name, int line) {
	int h=hash(name);
    if(directives[h]==NULL || strlen(name)==0) {
		return NULL;
	} else {
		struct Directive *directive;
        directive = directives[h];
		do {
			directive=directive->previous;
		} while(strcmp(directive->name, name)!=0 && directive != directives[h]);
		
		if(strcmp(directive->name, name)==0 && directive->defLine < line) {
			return directive;
		} else {
			return NULL;
		}
	}
}

int hash(const char *name) {
    int k=0;
    while(*name!='\0') {
        k*=256;
        k+=*name;
        k%=PRIME;
        name++;
    }
    return k;
}