#include <stdio.h>
#include "dgutils.h"
#include "lex.h"

extern int yydebug;
int yyparse (void);

int main(int argc, char* argv[])
{
	init_lex();
	yydebug = 1;
	yyparse();
	
	return 0;
}

