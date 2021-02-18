%{
#include MUT_H
#include <string.h>
#include "libarea.tab.h"

#define YY_NO_UNPUT

int lib_flagcell=0;
int lib_flagarea=0;
extern ht  *tut_tablasharea;
long lib_get;
char *lib_name;
%}


cell          [cC][eE][lL][lL] 
area          [aA][rR][eE][aA]                          
id            [^ ():;\t\n\r]*
wsp           [ ():;\t\n\r]*



%%

{cell}        {  lib_flagcell=1 ;}

{area}        {  lib_flagarea=1 ;}

{id}          { 
                  if(lib_flagcell) {
                      libarealval.text = namealloc (yytext) ;
                      lib_name=libarealval.text;
                      lib_flagcell=0;
                  }
                  if(lib_flagarea) {
                      libarealval.real = strtod (yytext, (char**)NULL);
                      lib_flagarea=0;
                      lib_get=addhtitem(tut_tablasharea,lib_name,(long)(libarealval.real*1000.0));
                  }
                  
              }
{wsp}         {;}
               

%%



int yywrap () { return 1 ; }
