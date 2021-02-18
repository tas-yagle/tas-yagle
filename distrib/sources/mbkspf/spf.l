%{
#include <string.h>
#include "y.tab.h"

#define YY_NO_UNPUT

extern int spf_Line ;
static int spf_i, ripmod, outputeol=0 ;

void debug()
{
}

//#devicename   {alphanum}({hdiv}|{alphanum}|{vector})*
%}
%s IDENTIFIER
%x COMMENT DOT RIPITALL

upper_alpha  [A-Z]
lower_alpha  [a-z]
num          [0-9]
space        [ \t]
eol          \n
eolcont      \n[ \t]*\+
emptyline    \n[ \t]*$
alpha        ({upper_alpha}|{lower_alpha})
alphanum     ({alpha}|{num})
version      {num}+(\.{num})+
integer      {num}+
number       -?{num}+(\.({num}*))?
enumber      -?{num}+(\.({num}*))?(e|E)[+|-]?{num}+
v0           (\[{num}+\])
v1           (\<{num}+\>)
v2           (\{{num}+\})
v3           (\({num}+\))
v4           (\\\[{num}+\\\])
vector       ({v0}|{v1}|{v2}|{v3}|{v4})
hdiv         ([[./:_|@#\$%\-!+\\'`~,;=\?\^\&\*])
identifier   [^() \t\n."'*][^() \t\n]*
qstring      "\"".*"\"" 
resistor     ^(r|R){identifier}
capacity     ^(c|C){identifier}
startbar     ^\*\|{space}*
aste         \*
dollar       \$
begindot     ^[ \t]*\.
%%
^\*                       { BEGIN RIPITALL; ripmod=0;}
{startbar}[^ ]+           {
                            outputeol=1;
                            if (strcasecmp(yytext,"*|p")==0) return (_PIN);
                            if (strcasecmp(yytext,"*|i")==0) return (_INST);
                            if (strcasecmp(yytext,"*|s")==0) return (_SUB);

                            ripmod=0;
                            if (strcasecmp(yytext,"*|design")==0) return (_DESIGN) ;
                            if (strcasecmp(yytext,"*|date")==0) return (_DATE) ;
                            if (strcasecmp(yytext,"*|vendor")==0) return (_VENDOR) ;
                            if (strcasecmp(yytext,"*|program")==0) return (_PROGRAM) ;
                            if (strcasecmp(yytext,"*|version")==0) return (_VERSION) ;
                            if (strcasecmp(yytext,"*|dspf")==0) return (_DSPF) ;
                            if (strcasecmp(yytext,"*|ground_net")==0) return (_GROUND_NET) ;
                            if (strcasecmp(yytext,"*|net")==0) return (_NET) ;

                            BEGIN RIPITALL;
                            if (strcasecmp(yytext,"*|divider")==0) ripmod=1;
                            if (strcasecmp(yytext,"*|delimiter")==0) ripmod=2;
                            if (strcasecmp(yytext,"*|busbit")==0) ripmod=3;

                            if (ripmod==0) {outputeol=0;}
                          }
<RIPITALL>.*$             {BEGIN INITIAL;
                           if (ripmod==1) { sprintf(spflval.text, "*|divider%s", yytext); return _DIVIDER; }
                           if (ripmod==2) { sprintf(spflval.text, "*|delimiter%s", yytext); return _DELIMITER; }
                           if (ripmod==3) { sprintf(spflval.text, "*|busbit%s", yytext); return _BUSBIT; }
                          }
{eol}                     {
                           spf_Line++;
                           if (outputeol)
                           {
                             outputeol=0;
                             return (_ENDOFSPICELINE);
                           }
                          }

{eolcont}                 {spf_Line++; }
{emptyline}               {spf_Line++; }
{space}                   {                        /* separators                 */          }
\(                        {return (_LEFTPAR) ;                                               }
\)                        {return (_RIGHTPAR) ;                                              }
<COMMENT>{eolcont}.*$     {spf_Line++;}
<COMMENT>{emptyline}      {spf_Line++;}
<COMMENT>{eol}            {BEGIN INITIAL; spf_Line++;}
<COMMENT>.*               {BEGIN INITIAL; spf_Line++;}
{begindot}[Ss][Uu][Bb][cC][kK][tT] {BEGIN INITIAL; outputeol=1; return (_SUBCKT) ;      /* native                     */          }
{begindot}[Ee][nN][dD]             {BEGIN INITIAL; outputeol=1; return (_END) ;         /* SPICE                      */          }
{begindot}[eE][nN][dD][sS]         {BEGIN INITIAL; outputeol=1; return (_ENDS) ;        /* statements                 */          }
{begindot}.                        {BEGIN COMMENT;}
{resistor}                {outputeol=1; strcpy(spflval.text, yytext) ; return (_RESISTANCE) ;            }
{capacity}                {outputeol=1; strcpy(spflval.text, yytext) ; return (_CAPACITANCE) ;           }
                                                                                                       
^[ \t]*\+                {}
^[ \t]*[abABd-qD-Qs-zS-Z].*$     {BEGIN COMMENT;}

{dollar}.*$              {}

{qstring}                {strcpy(spflval.text, yytext) ; return (_QSTRING) ; }
{identifier}             {strcpy(spflval.text, yytext) ; return (_TEXT) ; }
.                        {outputeol=1; return *yytext;}
%%

int yywrap() { return 1; }
