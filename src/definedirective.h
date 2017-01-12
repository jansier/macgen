/*
	Author Jan Sierpina
*/
#include <stdbool.h>

void printDirectives();
int resolveDefineDirectives(char **er, char **output, const char *buffer, int bufferLength, int line);
int addDirective(char **er, const char *line, int length, int defLine);
bool isDefineDirective(const char *line, int length);
