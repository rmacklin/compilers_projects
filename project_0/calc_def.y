
%{
	#include <stdio.h>
	int yylex(void);
	void yyerror(char *);
%}

%%

List	: Cmd '.' ListP		{printf(" parsed command\n");}
	;

ListP	: Cmd '.' ListP		{printf(" parsed command\n");}
	| /* empty */
	;

Cmd	: Expr Args
	;

Args	: 's' 'm' '[' Expr ']'
	| /* empty */
	;

Expr	: Term ExprP
	;

ExprP	: '+' Term ExprP
	| '-' Term ExprP
	| /* empty */
	;

Term	: Factor TermP
	;

TermP	: '*' Factor TermP
	| '/' Factor TermP
	| /* empty */
	;

Factor	: '(' Expr ')'
	| 'm' '[' Expr ']'
	| 'n'
        ;
%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
	return;
}

int main(void) {
	yyparse();
	return 0;
}
