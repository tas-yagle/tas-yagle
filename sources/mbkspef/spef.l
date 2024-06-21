%{
#include <string.h>
#include "spef.tab.h"
#include MUT_H
#define YY_NO_UNPUT
#define ECHO /*printf("token= %s\n", yytext)*/
extern int Line ;
/*
vector       ([\<\>\[\]\{\}\(\)])
*/
%}
%s IDENTIFIER
%x COMMENT NOI

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

atbegin ^{space}*

prefix_bus_delim [[{(<]
suffix_bus_delim []})>]
  
%%
<INITIAL,NOI>{eol}                    { BEGIN INITIAL;
						   Line++; 
						 }
<COMMENT>{eol}           {
						   Line++; 
						 }
<COMMENT>.               {                        /* multi line comment       */  }
"//".*$                  {                        /* comment                  */  }
"/"\*                    {BEGIN COMMENT ;         /* begin multi line comment */  }
<COMMENT>\*"/"           {BEGIN INITIAL ;         /* end multi line comment   */  }
<INITIAL,NOI>{space}                  {        /* separators                 */                }
<INITIAL,NOI>\*SPEF                   {ECHO; return _E_SPEF;                                   }
<INITIAL,NOI>\*DESIGN                 {ECHO; return _E_DESIGN;                                 }
<INITIAL,NOI>\*DATE                   {ECHO; return _E_DATE;                                   }
<INITIAL,NOI>\*VENDOR                 {ECHO; return _E_VENDOR;                                 }
<INITIAL,NOI>\*PROGRAM                {ECHO; return _E_PROGRAM;                                }
<INITIAL,NOI>\*VERSION                {ECHO; return _E_VERSION;                                }
<INITIAL,NOI>\*DESIGN_FLOW            {ECHO; return _E_DESIGN_FLOW;                            }
<INITIAL,NOI>\*DIVIDER                {ECHO; return _E_DIVIDER;                                }
<INITIAL,NOI>\*DELIMITER              {ECHO; return _E_DELIMITER;                              }
<INITIAL,NOI>\*BUS_DELIMITER          {ECHO; return _E_BUS_DELIMITER;                          }
<INITIAL,NOI>\*T_UNIT                 {ECHO; return _E_T_UNIT;                                 }
<INITIAL,NOI>NS                       {ECHO; return _NS;                                       }
<INITIAL,NOI>PS                       {ECHO; return _PS;                                       }
<INITIAL,NOI>\*C_UNIT                 {ECHO; return _E_C_UNIT;                                 }
<INITIAL,NOI>PF                       {ECHO; return _PF;                                       }
<INITIAL,NOI>FF                       {ECHO; return _FF;                                       }
<INITIAL,NOI>\*R_UNIT                 {ECHO; return _E_R_UNIT;                                 }
<INITIAL,NOI>OHM                      {ECHO; return _OHM;                                      }
<INITIAL,NOI>KOHM                     {ECHO; return _KOHM;                                     }
<INITIAL,NOI>\*L_UNIT                 {ECHO; return _E_L_UNIT;                                 }
<INITIAL,NOI>HENRY                    {ECHO; return _HENRY;                                    }
<INITIAL,NOI>MH                       {ECHO; return _MH;                                       }
<INITIAL,NOI>UH                       {ECHO; return _UH;                                       }
<INITIAL,NOI>\*NAME_MAP               {ECHO; return _E_NAME_MAP;                               }
<INITIAL,NOI>\*PORTS                  {ECHO; return _E_PORTS;                                  }
<INITIAL,NOI>I                        {ECHO; return _SPEF_I;                                        }
<INITIAL,NOI>B                        {ECHO; return _SPEF_B;                                        }
<INITIAL,NOI>O                        {ECHO; return _SPEF_O;                                        }
<INITIAL,NOI>X                        {ECHO; return _SPEF_X;                                        }
<INITIAL,NOI>\*C                      {ECHO; return _E_C;                                      }
<INITIAL,NOI>\*L                      {ECHO; BEGIN NOI;return _E_L;                                    }
<INITIAL,NOI>\*S                      {ECHO; BEGIN NOI;return _E_S;           } 
<INITIAL,NOI>\*D                      {ECHO; return _E_D;                                      }
<INITIAL,NOI>\*V                      {ECHO; return _E_V;                                      }
<INITIAL,NOI>\*D_NET                  {ECHO; return _E_D_NET;                                  }
<INITIAL,NOI>\*CONN                   {ECHO; return _E_CONN;                                   }
<INITIAL,NOI>\*P                      {ECHO; return _E_P;                                      }
<INITIAL,NOI>\*I                      {ECHO; return _E_I;                                      }
<INITIAL,NOI>\*x                      {ECHO; return _E_X;                                      }
<INITIAL,NOI>\*CAP                    {ECHO; return _E_CAP;                                    }
<INITIAL,NOI>\*RES                    {ECHO; return _E_RES;                                    }
<INITIAL,NOI>\*END                    {ECHO; return _E_END;                                    }
<INITIAL,NOI>\*GROUND_NETS            {ECHO; return _E_GROUND_NETS;}
<INITIAL,NOI>\*POWER_NETS             {ECHO; return _E_POWER_NETS;}
<INITIAL,NOI>\*PHYSICAL_PORTS         {ECHO; return _E_PHYSICAL_PORTS;}
<INITIAL,NOI>\*DEFINE                 {ECHO; return _E_DEFINE;}
<INITIAL,NOI>\*PDEFINE                {ECHO; return _E_PDEFINE;}
<INITIAL,NOI>\*R_NET                  {ECHO; return _E_RNET;}
<INITIAL,NOI>\*R_PNET                  {ECHO; return _E_RPNET;}
<INITIAL,NOI>\*D_PNET                  {ECHO; return _E_DPNET;}
<INITIAL,NOI>\*INDUC                   {ECHO; return _E_INDUC;}
<INITIAL,NOI>\*DRIVER                  {ECHO; return _E_DRIVER;}
<INITIAL,NOI>\*CELL                   {ECHO; return _E_CELL;}
<INITIAL,NOI>\*C2_R1_C1               {ECHO; return _E_C2_R1_C1;}
<INITIAL,NOI>\*LOADS                  {ECHO; return _E_LOADS;}
<INITIAL,NOI>\*RC                     {ECHO; return _E_RC;}
<INITIAL,NOI>\*Q                      {ECHO; return _E_Q;}
<INITIAL,NOI>\*K                      {ECHO; return _E_K;}
<INITIAL,NOI>\*N                      {ECHO; return _E_N;  }
<INITIAL,NOI>{pos_integer}            {ECHO; speflval.UL.longint = atol(yytext) ; strcpy(speflval.UL.text,yytext) ; return _POS_INTEGER; }
<INITIAL,NOI>{pos_number}             {ECHO; speflval.UR.real = atof(yytext) ; strcpy(speflval.UR.text,yytext) ; return _POS_NUMBER;     }
<INITIAL,NOI>{number}                 {ECHO; speflval.UR.real = atof(yytext) ; strcpy(speflval.UR.text,yytext) ; return _NUMBER;         }
<INITIAL,NOI>\*{pos_integer}          {ECHO; speflval.text = mbkstrdup(yytext) ; return _INDEX;                    }
<INITIAL>{hchar}                  {ECHO; return *yytext; }
<INITIAL,NOI>{qstring}                {ECHO; speflval.text = mbkstrdup(yytext) ; return (_QSTRING) ;               }
<INITIAL,NOI>{identifier}             {ECHO; speflval.text = mbkstrdup(yytext) ; return (_IDENTIFIER) ;            }
<INITIAL>{identifier2}            {ECHO; speflval.text = mbkstrdup(yytext) ; return (_IDENTIFIER) ;            }
<INITIAL,NOI>\*{pos_integer}{hchar}{pos_integer} {ECHO; speflval.text = mbkstrdup(yytext) ; return (_IDENTIFIER) ; }
<INITIAL,NOI>\*{pos_integer}{hchar}{identifier}  {ECHO; speflval.text = mbkstrdup(yytext) ; return (_IDENTIFIER) ; }
<INITIAL>{prefix_bus_delim}|{suffix_bus_delim} {ECHO; return *yytext;}
<INITIAL>.
<NOI>.     {ECHO; return *yytext;}
%%

int yywrap() { return 1; }
