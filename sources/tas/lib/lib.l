%{
#include <string.h>
#include "lib.tab.h"
#include MUT_H

#define YY_NO_UNPUT
#define ECHO /*fprintf(stdout,"\ntoken %s\n",yytext)*/
int LIB_LINE = 1 ;


%}
%x COMMENT 
%x LEFTPAR 
operateur      [+\*/\^\-]
number         [-+]?[0-9]*\.?[0-9]*([Ee][-+]?[0-9]+)?
string         \"[^"\\]*((\\\")+[^\\]+(\\\")+[^"]*)*\"	 
string2        [A-Za-z!@#$%^&_+=\|~\?][A-Za-z0-9!@#$%^&_+=\|~\?]*[\<\{\[\(][-0-9:]+[\]\}\>\)]	 
ident          [-a-zA-Z0-9!@#$%^&_+=|~?<>.]+


%%
\n                       {LIB_LINE++ ;                                                     }
{operateur}              {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_OPE);     }
{number}                 {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_NUMBER) ; }
{string}                 {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_STRING) ; }
{string2}                {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_STRING) ; }
{ident}                  {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_IDENT) ;  }
\,                       {ECHO; return (_COMMA) ;                                          }             
\;                       {ECHO; return (_SEMI)  ;                                          } 
\(                       {ECHO; BEGIN(LEFTPAR)  ;   return (_LPAR) ;                       }  
\{                       {ECHO; return (_LCURLY);                                          }   
\}                       {ECHO; return (_RCURLY);                                          } 
\:                       {ECHO; return (_COLON) ;                                          }   
[ \t]                    {                                                                 }
\r                       {                                                                 }
"/*"	                 {ECHO; BEGIN(COMMENT) ;                                           }
<COMMENT>[^*\n]*         {                                                                 } 
<COMMENT>"*"+[^*/\n]*    {                                                                 }
<COMMENT>\n              {LIB_LINE++ ;                                                     } 
<COMMENT>"*"+"/"	     {ECHO; BEGIN(INITIAL);                                            }
<LEFTPAR>{operateur}     {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_OPE);     }
<LEFTPAR>{number}        {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_NUMBER) ; }
<LEFTPAR>{string}        {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_STRING) ; }
<LEFTPAR>{string2}       {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_STRING) ; }
<LEFTPAR>{ident}         {ECHO; liblval.stringtype = mbkstrdup(yytext); return (_IDENT) ;  }
<LEFTPAR>\,              {ECHO; return (_COMMA) ;                                          }    
<LEFTPAR>\)              {ECHO; BEGIN(INITIAL); return (_RPAR)  ;                          }   
%%

int libwrap() { return 1 ; }
