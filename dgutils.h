#ifndef DGUTILS_
#define DGUTILS_
struct expr {
	int l, lvl;
	char *s;
};

struct func2 {
	struct expr func, der;
	struct func2 *next;
};

struct frec {
	struct expr name;
	char *tmpl;
	int noparl, cnt;
	struct frec *next;
};

#define LVL_PRIMARY 4
#define LVL_UNARY 3
#define LVL_EXP 2
#define LVL_MUL 1 
#define LVL_ADD 0
#define SET_LNG(e, lng) e->s = (char *)malloc((e->l = (lng)) + 1)

extern struct expr zero, one, two;

extern struct expr *addit(struct expr *rslt, struct expr *e1, struct expr *e2, int add);
extern struct expr *mulit(struct expr *rslt, struct expr *e1, struct expr *e2, int mul);
extern struct expr *expit(struct expr *rslt, struct expr *e1, struct expr *e2);
extern struct expr *negate(struct expr *rslt, struct expr *e);
extern struct expr *lnit(struct expr *rslt, struct expr *e);
extern void concat(struct expr *rslt, ...);
extern struct expr * setit(struct expr *rslt, const char *s);
extern void copyf(struct func2 *dest, const struct func2 *src);
extern int yyerror(char *line);
extern struct func2 *creat_func(struct frec *func, struct func2 *arg);
extern void free_func2(struct func2 *f);

extern void init_s(void);

#endif /*DGUTILS_*/
