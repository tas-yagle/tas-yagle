%{
#include MUT_H
#include <string.h>
#include "tlfarea.tab.h"

#define YY_NO_UNPUT

int tlf_flagcell=0;
int tlf_flagarea=0;
ht  *tlf_tablasharea;
long tlf_get;
char *tlf_name;
%}


cell          [cC][eE][lL][lL] 
area          [aA][rR][eE][aA]                          
id            [^ ()\t\n\r]*
wsp           [ ()\t\n\r]*



%%

{cell}        {  tlf_flagcell=1 ;}

{area}        {  tlf_flagarea=1 ;}

{id}          { 
                  if(tlf_flagcell) {
                      tlfarealval.text = namealloc (yytext) ;
                      tlf_name=tlfarealval.text;
                      tlf_flagcell=0;
                  }
                  if(tlf_flagarea) {
                      tlfarealval.real = strtod (yytext, (char**)NULL);
                      tlf_flagarea=0;
                      tlf_get=addhtitem(tlf_tablasharea,tlf_name,(long)(tlfarealval.real*1000.0));
                  }
                  
              }
{wsp}         {;}
               

%%



int yywrap () { return 1 ; }
