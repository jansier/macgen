/*
	Author Jan Sierpina
*/
#include <stdbool.h>
#define TAB 9
#define SPACE 32
#define ENDLINE 10
#define QUOTION 34
#define DEF_LEN 7
#define INCL_LEN 8

const char *pointFirstChar(const char *line);
bool isNameChar(char c);
bool canBeginName(char c);
const char *nameEnds(const char *nameBegin);
void cpandtrm (char **dest, const char *begin, int length);
bool isDirective(const char *line);
void getFirstName(const char *blockStart, const char **begin, const char **end, int blockLength);
const char *pointFirstCharInBlock(const char *begin, const char *end);
const char *findClosingBracket(const char *begin, const char *end);
const char *findNextComma(const char *begin, const char *end);
void memsafecpy(char **dest, int *destWritten, int *destSize, const char *begin, int length);
void getBasePath(char **basePath, const char *path);
void getFullPath(char **dest, const char *basePath, const char *filePath);
