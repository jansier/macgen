/*
	Author Jan Sierpina
*/

struct Directive {
	struct Directive *previous;
	char *name;
	char *definition;
	char *parameters[16];
	int defLine; // defined in line in processed file
	int prNmb; // number of parameters
};

void addToDirectives(struct Directive *directive);
struct Directive *getDirective(const char *name, int line);
