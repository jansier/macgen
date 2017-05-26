Flags = -Wall -Wextra -std=c99
C = gcc

macgen: Makefile
	$(C) $(Flags) src/macgen.c src/filestack.c src/definedirective.c src/includedirective.c src/stringhelper.c src/hashlist.c -o macgen
