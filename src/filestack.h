/*
	Author Jan Sierpina
*/
typedef struct File* FileStack;

FileStack newFileStack();
void pushFile(FileStack fileStack, char* path, int inclusionLine);
void printStack(FileStack fileStack);
void popFile(FileStack fileStack);
