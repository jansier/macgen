/*
	Author Jan Sierpina
*/
#include <stdbool.h>

bool isIncludeDirective(const char *line, int length);
int getIncludedFilePath(char **er, char **includedFilePath, const char *line, int length);
