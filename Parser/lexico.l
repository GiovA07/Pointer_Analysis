%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sintaxis.tab.h"
extern YYSTYPE yylval;

%}

%option noyywrap
%option yylineno

letra [a-zA-Z_]   
digito [0-9]   


%%


"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }

{letra}({letra}|{digito})*  { 
    yylval.string = strdup(yytext); 
    return ID; 
}

"="  { return ASIGNACION; }
"&"  { return REF; }
"*"  { return DEREF; }
";"  { return ';'; }


"{"                         { return TLLAVE_OP; }
"}"                         { return TLLAVE_CL; }
"("                         { return TPAR_OP; }
")"                         { return TPAR_CL; }


"//".*						/** Comentario de linea **/
"/*"([^*]|[*][^/])*"*/"  	/**  Comentario bloque  **/

[#\$]                       { return -1; }


.|\n                          ; /* ignore all the rest */
[ \t\r\n]+                  ; /* ignorar espacios/blancos */


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
