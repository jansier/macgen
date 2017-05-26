all:
	gcc src/macgen.c src/filestack.c src/definedirective.c src/includedirective.c src/stringhelper.c src/hashlist.c -o macgen -std=c99 -Wall -Wextra
