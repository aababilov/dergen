#ifndef LEX_H_
#define LEX_H_

extern int yylex(void);
extern int init_lex(void);
extern char *inFile, *outFile;
extern int lineno;
extern struct expr dep, indep;

#endif /*LEX_H_*/
