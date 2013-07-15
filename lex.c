#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dgutils.h"
#include "lex.h"
#include "parser.h"

char *inFile, *outFile;
char c, buf[1024 + 3];
int lineno, bufl;
struct expr dep, indep;

struct frec *start = NULL, *fnd;

void add_frec(const char *name, char *tmpl)
{
  struct frec *t = (struct frec *)malloc(sizeof(struct frec));
  int tmpl_l = strlen(tmpl), i;
  t->next = start;
  setit(&t->name, name);
  t->tmpl = (char *)malloc(tmpl_l);
  memcpy(t->tmpl, tmpl, tmpl_l + 1);
  for (t->cnt = i = 0; tmpl[i]; ++i)
    if (tmpl[i] == '%') ++t->cnt;
  t->noparl = tmpl_l - t->cnt;
  start = t;
}

int find_frec(const char *name)
{
	for (fnd = start; fnd != NULL; fnd = fnd->next) 
		if (strcmp(name, fnd->name.s) == 0)
			return 1;
	return 0;
}

char nextc(void)
{
	if (feof(stdin)) return 0;
	if ((c = getchar()) == '\n') ++lineno;
	return c;
}

int init_lex(void)
{
	nextc();
	dep.s = indep.s = "";
	add_frec("sin", "cos(%)'");
	add_frec("cos", "-sin(%)'");
	add_frec("tg", "1 / cos(%) ^ 2");
	add_frec("ctg", "-1' / sin(%) ^ 2");
	add_frec("arcsin", "1 / (1 - % ^ 2) ^ 0.5");
	add_frec("arccos", "-1 / (1 - % ^ 2) ^ 0.5");
	add_frec("arctg", "1 / (1 + % ^ 2)");	
	add_frec("arcctg", "-1 / (1 + % ^ 2)");	
	add_frec("sh", "ch(%)");
	add_frec("ch", "sh(%)");	
	add_frec("th", "1 / ch(%) ^ 2");
	add_frec("cth", "1 / sh(%) ^ 2");
	add_frec("Arsh", "1 / (% ^ 2 + 1) ^ 0.5");	
	add_frec("Arch", "1 / (% ^ 2 - 1) ^ 0.5");	
	add_frec("ln", "1 / %");
	add_frec("exp", "e ^ %");	
	add_frec("log", "1 / (% * ln 10)");
}

#define ALUND (isalpha(c) || c == '_')

void read_idntf(void)
{
	buf[0] = c;
	nextc();
	for (bufl = 1; isalnum(c) || c == '_'; nextc(), ++bufl)
		if (bufl >= sizeof(buf) - 2)
		{
			for (yyerror("Identifier is too long"); isalpha(c) || c == '_'; nextc());
			break;
		}
		else buf[bufl] = c;
	buf[bufl] = '\0';
	fprintf(stderr, "idnt: %s\n", buf);
}

int yylex(void)
{
	int t;
	again:
	while (isspace(c)) nextc();
	if (ALUND || isdigit(c))
	{		
		read_idntf();
		yylval.func = (struct func2 *)malloc(sizeof(struct func2));
		setit(&yylval.func->func, buf);
		yylval.func->func.lvl = LVL_PRIMARY;
		yylval.func->der.lvl = LVL_PRIMARY;		
		if (find_frec(buf))
		{
			yylval.funcrec = fnd;
			return FUNC;
		}
		if (strcmp(buf, dep.s) == 0)
		{			
			fprintf(stderr, "dep\n");
			buf[bufl] = '\'';
			buf[++bufl] = '\0';
			setit(&yylval.func->der, buf);
		}
		else if (strcmp(buf, indep.s) == 0)
		{			
			fprintf(stderr, "inep\n");
			setit(&yylval.func->der, "1");
		}
		else
		{
			fprintf(stderr, "noth\n");
			setit(&yylval.func->der, "0");
		}
		return VARCONST;
	}

	t = c;
	if (c == '#')
	{
		read_idntf();
		goto again;
	}
	else 
	{
		nextc();
		return t;
	}
}
