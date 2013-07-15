#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lex.h"
#include "dgutils.h"

static char *p;
static int rlen, par1, par2;
struct expr 
zero = {1, LVL_PRIMARY, "0"},
	one = {1, LVL_PRIMARY, "1"},
	two = {1, LVL_PRIMARY, "2"};

void copyit(struct expr *dest, struct expr *src)
{
	if (dest != src)
	{
		SET_LNG(dest, src->l);
		memcpy(dest->s, src->s, src->l + 1);
	}
}

struct expr *addit(struct expr *rslt, struct expr *e1, struct expr *e2, int add)
{
	if (memcmp(e2->s, "0", 2) == 0) return e1;
	if (memcmp(e1->s, "0", 2) == 0)
		if (add) return e2;
		else
		{	
			rslt->lvl = LVL_UNARY;
			SET_LNG(rslt, e2->l + 1);
			rslt->s[0] = '-';
			memcpy(rslt->s + 1, e2->s, e2->l + 1);
			return rslt;
		}
	
	rslt->lvl = LVL_ADD;
	SET_LNG(rslt, e1->l + e2->l + 3);
	memcpy(rslt->s, e1->s, e1->l);
	memcpy(rslt->s + e1->l, add ? " + " : " - ", 3);
	memcpy(rslt->s + e1->l + 3, e2->s, e2->l);
	rslt->s[rslt->l] = '\0';
	return rslt;
}

struct expr *mulit(struct expr *rslt, struct expr *e1, struct expr *e2, int mul)
{
	if (memcmp(e2->s, "1", 2) == 0) 
	{
		fprintf(stderr, "yes, 1\n");
		return e1;
	}
	if (memcmp(e1->s, "0", 2) == 0) 
	{
		e1->lvl = LVL_PRIMARY;
		return e1;
	}
	if (memcmp(e2->s, "0", 2) == 0) 
	{
		e2->lvl = LVL_PRIMARY;
		return e2;
	}
	if (memcmp(e1->s, "1", 2) == 0 && mul) 
		return e2;
	
	rslt->lvl = LVL_MUL;
	rlen = e1->l + e2->l + 3;
	if (par1 = e1->lvl < LVL_MUL) rlen += 2;
	if (par2 = e2->lvl <= LVL_MUL) rlen += 2;	
	SET_LNG(rslt, rlen);
	p = rslt->s;
	
	if (par1) *(p++) = '(';
	memcpy(p, e1->s, e1->l);
	p += e1->l;
	if (par1) *(p++) = ')';
	
	memcpy(p, mul ? " * " : " / ", 3);
	p += 3;
	
	if (par2) *(p++) = '(';
	memcpy(p, e2->s, e2->l);
	p += e2->l;
	if (par2) *(p++) = ')';
	*p = '\0';
	return rslt;
}

struct expr *expit(struct expr *rslt, struct expr *e1, struct expr *e2)
{
	if (memcmp(e2->s, "1", 2) == 0) return e1;
	if (memcmp(e2->s, "0", 2) == 0)
	{
		rslt->lvl = LVL_PRIMARY;
		SET_LNG(rslt, 1);
		memcpy(rslt->s, "1", 2);
		return rslt;
	}
	
	rslt->lvl = LVL_EXP;
	rlen = e1->l + e2->l + 3;
	if (par1 = e1->lvl <= LVL_EXP) rlen += 2;
	if (par2 = e2->lvl < LVL_EXP) rlen += 2;
	SET_LNG(rslt, rlen);
	p = rslt->s;

	if (par1) *(p++) = '(';
	memcpy(p, e1->s, e1->l);
	p += e1->l;
	if (par1) *(p++) = ')';
	
	memcpy(p, " ^ ", 3);
	p += 3;
	
	if (par2) *(p++) = '(';
	memcpy(p, e2->s, e2->l);
	p += e2->l;
	if (par2) *(p++) = ')';
	*p = '\0';
	return rslt;
}


struct expr *negate(struct expr *rslt, struct expr *e)
{
	SET_LNG(rslt, e->l + 1);
	rslt->s[0] = '-';
	memcpy(rslt->s, e->s, rslt->l);
	return rslt;
}

struct expr *lnit(struct expr *rslt, struct expr *e)
{
	if (memcmp(e->s, "1", 2) == 0) return &zero;
	if (memcmp(e->s, "e", 2) == 0) return &one;
	SET_LNG(rslt, e->l + 4);
	memcpy(rslt->s, "ln(", 3);
	memcpy(rslt->s + 3, e->s, e->l);
	memcpy(rslt->s + 3 + e->l, ")", 2);
	return rslt;
}

void concat(struct expr *rslt, ...)
{
	va_list vl;
	char *p;
	struct expr *tmp;
	va_start(vl, rslt);
	rslt->l = 0;
	fprintf(stderr, "rslt: %s\n", rslt->s);
	while (tmp = va_arg(vl, struct expr *)) {
		rslt->l += tmp->l;
		fprintf(stderr, "\targ: %s\n", tmp->s);
	}
	va_end(vl);
	
	rslt->s = realloc(rslt->s, rslt->l + 1);
	rslt->s[rslt->l] = '\0';
	va_start(vl, rslt);
	p = rslt->s;
	while (tmp = va_arg(vl, struct expr *)) 
		p = memcpy(p, tmp->s, tmp->l) + tmp->l;	
	
	va_end(vl);	
}

struct expr *setit(struct expr *rslt, const char *s)
{
	rslt->l = strlen(s);
	rslt->s = (char *)malloc(rslt->l + 1);
	rslt->s[rslt->l] = '\0';
	memcpy(rslt->s, s, rslt->l);
	return rslt;
}

struct func2 *creat_func(struct frec *func, struct func2 *arg)
{
	int cnt = 0, slen = 0, nder_len = 0, i;
	struct func2 *t;
	struct expr expr;
	char *pf, *pt;
	t = (struct func2 *)malloc(sizeof(struct func2));
	fprintf(stderr, "Creat start\n");

	//set function nder
	t->func.lvl = LVL_PRIMARY;
	SET_LNG((&t->func), func->name.l + arg->func.l + 2);
	memcpy(t->func.s, func->name.s, func->name.l);
	t->func.s[func->name.l] = '(';
	memcpy(t->func.s + func->name.l + 1, arg->func.s, arg->func.l);
	memcpy(t->func.s + t->func.l - 1, ")", 2);
	fprintf(stderr, "so far, nder is %s\n", t->func.s);

	//set der
	expr.lvl = LVL_PRIMARY;
	SET_LNG((&expr), func->noparl + func->cnt * arg->func.l);
	fprintf(stderr, "mem alloced: %s\n", func->tmpl);
	for (pt = expr.s, pf = func->tmpl; *pf; ++pf)
		if (*pf != '%') {
			*(pt++) = *pf;
		} else {
			memcpy(pt, arg->func.s, arg->func.l);
			fprintf(stderr, "Added\n");
			pt += arg->func.l;
		}
	*pt = '\0';
	fprintf(stderr, "expr: %s\n", expr.s);
	copyit(&t->der, mulit(&t->der, &expr, &arg->der, 1));
	fprintf(stderr, "der: %s\n", t->der.s);
	free(expr.s);	
	return t;
}

void free_func2(struct func2 *f)
{
	free(f->func.s);
	free(f->der.s);
	free(f);

}

int yyerror(char *line)
{
	fprintf(stderr, "line %d: %s\n", lineno, line);
	return 0;
}
