%{
#include <string.h>
#include "spef_dnet.tab.h"
#include MUT_H
#define YY_NO_UNPUT
#define ECHO /*printf("token= %s\n", yytext)*/
extern int Line ;


%}
%s IDENTIFIER
%x COMMENT

upper        [A-Z]
lower        [a-z]
digit  	     [0-9]
space        [ \t]
eol          \n
alpha   	 ({upper}|{lower})
sign         ({pos_sign}|{neg_sign})
pos_sign     \+
neg_sign     \-
pos_decimal  {digit}+[.]{digit}*
pos_integer  {digit}+
pos_fraction [.]{digit}+
pos_exp      {pos_radix}{exp_char}{integer}
pos_radix    ({pos_integer}|{pos_decimal}|{pos_fraction})
pos_float    ({pos_decimal}|{pos_fraction}|{pos_exp})
pos_number   ({pos_integer}|{pos_float})
decimal      {sign}?{digit}+[.]{digit}*
integer      {sign}?{digit}+
fraction     {sign}?[.]{digit}+
exp          {radix}{exp_char}{integer}
radix        ({integer}|{decimal}|{fraction})
exp_char     [E]|[e]
float        ({decimal}|{fraction}|{exp})
number       ({integer}|{float})
v0           (\[{digit}+\])
v1           (\<{digit}+\>)
v2           (\{{digit}+\})
v3           (\({digit}+\))
v4           (\\\[{digit}+\\\])
vector       ({v0}|{v1}|{v2}|{v3}|{v4})
hdiv         ([[./:_|@#\$%\-!+\\'`~,;=\?\^\&\*])
identifier   {alpha}({hdiv}|{alpha}|{digit}|{vector})*
identifier2  {integer}{hdiv}?({hdiv}|{alpha}|{digit}|{vector})*
qstring      "\"".*"\"" 
hchar        [./:|]

prefix_bus_delim [[{(<]
suffix_bus_delim []})>]
  
%%
<COMMENT>.*$             {BEGIN INITIAL ;         /* end of line      */          }
{eol}                    {
						   Line++; 
						 }
^"//"                    {BEGIN COMMENT ;         /* comment      */              }
{space}                  {        /* separators                 */                }
I                        {ECHO; return _SPEF_I;                                        }
B                        {ECHO; return _SPEF_B;                                        }
O                        {ECHO; return _SPEF_O;                                        }
\*C                      {ECHO; return _E_C;                                      }
\*L                      {ECHO; return _E_L;                                      }
\*S                      {ECHO; return _E_S;                                      } 
\*D                      {ECHO; return _E_D;                                      }
\*V                      {ECHO; return _E_V;                                      }
\*D_NET                  {ECHO; return _E_D_NET;                                  }
\*CONN                   {ECHO; return _E_CONN;                                   }
\*P                      {ECHO; return _E_P;                                      }
\*I                      {ECHO; return _E_I;                                      }
\*x                      {ECHO; return _E_X;                                      }
\*CAP                    {ECHO; return _E_CAP;                                    }
\*RES                    {ECHO; return _E_RES;                                    }
\*END                    {ECHO; return _E_END;                                    }
{pos_integer}            {ECHO; spef_dnetlval.UL.longint = atol(yytext) ; strcpy(spef_dnetlval.UL.text,yytext) ; return _POS_INTEGER; }
{pos_number}             {ECHO; spef_dnetlval.UR.real = atof(yytext) ; strcpy(spef_dnetlval.UR.text,yytext) ; return _POS_NUMBER;     }
{number}                 {ECHO; spef_dnetlval.UR.real = atof(yytext) ; strcpy(spef_dnetlval.UR.text,yytext) ; return _NUMBER;         }
\*{pos_integer}          {ECHO; spef_dnetlval.text = mbkstrdup(yytext) ; return _INDEX;                    }
{prefix_bus_delim}       {ECHO; spef_dnetlval.text = mbkstrdup(yytext) ; return _PREFIX_BUS_DELIM;         }
{suffix_bus_delim}       {ECHO; spef_dnetlval.text = mbkstrdup(yytext) ; return _SUFFIX_BUS_DELIM;         }
{hchar}                  {ECHO; spef_dnetlval.text = mbkstrdup(yytext) ; return (_HCHAR) ;                 }
{qstring}                {ECHO; spef_dnetlval.text = mbkstrdup(yytext) ; return (_QSTRING) ;               }
{identifier}             {ECHO; spef_dnetlval.text = mbkstrdup(yytext) ; return (_IDENTIFIER) ;            }
{identifier2}             {ECHO; spef_dnetlval.text = mbkstrdup(yytext) ; return (_IDENTIFIER) ;            }
\*{pos_integer}{hchar}{pos_integer} {ECHO; spef_dnetlval.text = mbkstrdup(yytext) ; return (_IDENTIFIER) ; }
\*{pos_integer}{hchar}{identifier}  {ECHO; spef_dnetlval.text = mbkstrdup(yytext) ; return (_IDENTIFIER) ; }

%%

int yywrap() { return 1; }
