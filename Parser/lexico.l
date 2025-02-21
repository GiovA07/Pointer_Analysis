%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sintaxis.tab.h"
extern YYSTYPE yylval;

%}

%option noyywrap
%option yylineno

letra [a-zA-Z]      
digito [0-9]   


%%

{letra}({letra}|{digito})*  { 
    yylval.string = strdup(yytext); 
    return ID; 
}

"="  { return ASIGNACION; }
"&"  { return REF; }
"*"  { return DEREF; }
";"  { return ';'; }

.|\n                          ; /* ignore all the rest */


%%

void yyerror(){
	printf("%s%d\n","-> ERROR Sintactico en la linea: ",yylineno);
}

int main(int argc,char *argv[]){
	++argv,--argc;
	if (argc > 0)
		yyin = fopen(argv[0],"r");
	else
		yyin = stdin;

	yyparse();

}
