/************************************************************/
/*                         slib.l                           */
/************************************************************/

%{
#include <stdlib.h>
#include "slib.tab.h"
#include MUT_H
#include SLB_H

#define ECHO 
#define YY_NO_UNPUT
static char *slib_stripquotes();
extern int slibParseLine  ;

%}
liste                     [!?#%@._A-Za-z0-9]
num                       [\.]?[0-9]*[\.]?[0-9]*
expression                {liste}*
operateur                 [+\*/\^\-]

%x COMMENT

%%
"//".*\n                  { ECHO ;slibParseLine++; }
"/*"                      { ECHO ;  BEGIN COMMENT ;}
<COMMENT>[^*\n]*          { ECHO ;                 }
<COMMENT>"*"+[^*/\n]*     { ECHO ;                 }
<COMMENT>\n               { ECHO ;slibParseLine++; }
<COMMENT>"*"+"/"          { ECHO ;  BEGIN INITIAL ;}
\n                        { ECHO ;slibParseLine++; }
\\\n                      { ECHO ;slibParseLine++; }

[ \t]                    {ECHO ;                    }
\=                       {ECHO ; return (EGAL)     ;}
";"                      {ECHO ; return (SEMI)     ;}
":"                      {ECHO ; return (COLON)    ;}
","                      {ECHO ; return (COMMA)    ;}
\".*\"                   {ECHO ; sliblval.string_type = slib_stripquotes(yytext); return (STRING)     ;}
"("                      {ECHO ; return (LPAR)     ;}
")"                      {ECHO ; return (RPAR)     ;}
"{"                      {ECHO ; return (LCURLY)   ;}
"}"                      {ECHO ; return (RCURLY)   ;}
"library"                {ECHO ; return (LIB)      ;}
"symbol"                 {ECHO ; return (SYMBOL)   ;}
"canonical_name"         {ECHO ; return (SYMBNAME) ;}
"layer"                  {ECHO ; return (LAYER)    ;}
"sub_symbol"             {ECHO ; return (SUB_SYMB) ;}
"set_usable_area"        {ECHO ; return (AREA)     ;}
"annotate_symbol"        {ECHO ; return (ANNOTATE) ;}
"line"                   {ECHO ; return (LINE)     ;}
"arc"                    {ECHO ; return (ARC)      ;}
"circle"                 {ECHO ; return (CIRCLE)   ;}
"pin"                    {ECHO ; return (PIN)      ;}
set_route_grid           {ECHO ; return (T_GRID)   ;}
{operateur}              {ECHO ; sliblval.string_type = namealloc(yytext); return (OPE)      ;}
{num}                    {ECHO ; sliblval.string_type = namealloc(yytext); return (NUM)      ;}
{expression}             {ECHO ; sliblval.string_type = namealloc(yytext); return (STRING)   ;}

%%
static char *slib_stripquotes(char   *source)
{
    char    dest[1024];
    int     i = 0;

    while (*source != 0 && i < 1023) {
        if ((dest[i] = *(source++)) != '"') i++;
    }
    dest[i] = '\0';
    return namealloc(dest);
}

int yywrap () { return 1 ; }
