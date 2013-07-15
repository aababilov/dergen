%{
#include "dgutils.h"
#include "lex.h"
  /*#define YYDEBUG 1*/
%}
%union { 
	struct func2 *func;
	struct frec *funcrec;
}
%start fseq
%token <func> VARCONST
%token <funcrec> FUNC
%type <func> derreq expr primary
%left '+' '-'
%left '*' '/'
%right '^'
%left UNARY
%%
fseq: 
	  /* empty */
	  	{ }
	| fseq derreq
	;
derreq:
	  VARCONST '(' VARCONST ')' ':'
		{
			printf("%s(%s):\n", $1->func.s, $3->func.s); 
			setit(&dep, $1->func.s);
			setit(&indep, $3->func.s);
		}
		expr '=' expr ';'
		{
			printf("%s = %s\n", $7->der.s, $9->der.s);
			free_func2($7);
			free_func2($9);
		}	
	| error ';'
		{ yyerror("error in syntax"); yyerrok; }
	;
primary:
	  VARCONST
	| '(' expr ')'
		{ $$ = $2; }
	| '{' expr '}'
		{ $$ = $2; }
	| '[' expr ']'
		{ $$ = $2; }
	| FUNC '(' expr ')'
		{
			$$ = creat_func($1, $3);
			free_func2($3);
		}
	;
expr: 
	  primary
	  	{ $$ = $1; 
	  	  fprintf(stderr, "func %s; der %s\n", $1->func.s, $1->der.s);
	  	}
	| expr '+' expr
		{
			$$ = (struct func2 *)malloc(sizeof(struct func2));
			copyit(&$$->func, 
				addit(&$$->func, &$1->func, &$3->func, 1));
			copyit(&$$->der,
				addit(&$$->der, &$1->der, &$3->der, 1));
			free_func2($1);
			free_func2($3);
		}
	| expr '-' expr
		{
			$$ = (struct func2 *)malloc(sizeof(struct func2));
			copyit(&$$->func, 
				addit(&$$->func, &$1->func, &$3->func, 0));
			copyit(&$$->der,
				addit(&$$->der, &$1->der, &$3->der, 0));
			free_func2($1);
			free_func2($3);
		}
	| expr '*' expr
		{
			struct expr pr1, pr2;
			pr1.s = pr2.s = NULL;		
			$$ = (struct func2 *)malloc(sizeof(struct func2));
			copyit(&$$->func,
				mulit(&$$->func, &$1->func, &$3->func, 1));
			copyit(&$$->der,
				addit(&$$->der, 
					mulit(&pr1, &$1->der, &$3->func, 1),
					mulit(&pr2, &$1->func, &$3->der, 1), 
					1));
			free_func2($1);
			free_func2($3);
			free(pr1.s);
			free(pr2.s);
		}
	| expr '/' expr
		{
			struct expr pr1, pr2, nom, denom;
			pr1.s = pr2.s = nom.s = denom.s = NULL;		
			$$ = (struct func2 *)malloc(sizeof(struct func2));
			copyit(&$$->func, 
				mulit(&$$->func, &$1->func, &$3->func, 0));
			copyit(&$$->der,
				mulit(&$$->der,
					addit(&nom,
						mulit(&pr1, &$1->der, 
							&$3->func, 1),
						mulit(&pr2, &$1->func, 
							&$3->der, 1), 
					0),
				expit(&denom, &$3->func, &two),
				0));
			free_func2($1);
			free_func2($3);
			free(pr1.s);
			free(pr2.s);
			free(nom.s);
			free(denom.s);
		}
	| expr '^' expr
		{
			struct expr p1, p2, p3, subt, tmp;
			p1.s = p2.s = p3.s = subt.s = tmp.s = NULL;	
			$$ = (struct func2 *)malloc(sizeof(struct func2));
			copyit(&$$->func, 
				expit(&$$->func, &$1->func, &$3->func));	
			copyit(&tmp, expit(&tmp, &$1->func, 
				addit(&subt, &$3->func, &one, 0)));		
			copyit(&p1, 
				mulit(&p1, mulit(&p2, &$3->func, &tmp, 1),
					&$1->der, 1));
			free(p2.s);
			free(tmp.s);
			copyit(&p2, 
				mulit(&p2, 
					mulit(&p3, lnit(&tmp, &$1->func), 
						&$$->func, 1),
					&$3->der, 1));
			copyit(&$$->der,
				addit(&$$->der, &p1, &p2, 1));
			free(p1.s);
			free(p2.s);
			free(p3.s);
			free(subt.s);
			free(tmp.s);
		}		
	| '-' expr %prec UNARY
		{
			$$ = (struct func2 *)malloc(sizeof(struct func2));
			negate(&$$->func, &$2->func);
			negate(&$$->der, &$2->der);
			free_func2($2);
		}
	| '+' expr %prec UNARY
		{
			$$ = $2;
		}
	;
