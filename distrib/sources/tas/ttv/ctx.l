%{
#include <string.h>
#include MUT_H
#include "ctx.tab.h"

extern int CTX_LINE ;

#define ECHO /*fprintf(stderr,"%s\n",yytext)*/
#define YY_NO_UNPUT

%}

upper_alpha  [A-Z]
lower_alpha  [a-z]
num 	     [0-9]
alpha   	 ({upper_alpha}|{lower_alpha})
alphanum     ({alpha}|{num})
integer      -?{num}+
number       -?{num}+(\.({num}*))?
enumber      -?{num}+(\.({num}*))?(e|E)[+|-]?{num}+
version_number {num}+(\.({num}*))+_?{identifier}
identifier   {alpha}(([\[|\\./:$_\-|+@{}\#\]<>]?)+{alphanum}((\[|\\\[|$){num}+(\]|\\\]|$))?)*
comment      \/\/

%%
\n                       { ECHO; CTX_LINE++; }
{comment}.*$             { ECHO; }
[ \t]                    { ECHO; }
^H                       { ECHO; return(_H) ;}
^G                       { ECHO; return(_G) ;}
^M                       { ECHO; return(_M) ;}
^I                       { ECHO; return(_I) ;}
^D                       { ECHO; return(_D) ;}
^E                       { ECHO; return(_E) ;}
^F                       { ECHO; return(_F) ;}
\(                       { ECHO; return('(') ;}
\)                       { ECHO; return(')') ;}
\;                       { ECHO; return(';') ;}
{integer}                { ECHO; ctxlval.t_long = strtol (yytext, (char**)NULL, 10); return (_INTEGER); }
{number}                 { ECHO; ctxlval.t_float = strtod (yytext, (char**)NULL); return (_NUMBER); }
{enumber}                { ECHO; ctxlval.t_float = strtod (yytext, (char**)NULL); return (_ENUMBER);}
{version_number}         { ECHO; ctxlval.t_pchar = namealloc (yytext); return (_VERSION_NUMBER); }
{identifier}             { ECHO; ctxlval.t_pchar = namealloc (yytext); return (_IDENTIFIER); }
%%

int yywrap() { return 1; }
