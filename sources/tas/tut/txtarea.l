%{
#include MUT_H
#include <string.h>
#include "txtarea.tab.h"

#define YY_NO_UNPUT

int txt_flagwidth=0;
int txt_flagheight=0;
int txt_flagwidth2=0;
int txt_flagheight2=0;
int txt_flagarea=0;
extern ht  *tut_tablasharea;
long txt_get;
char *txt_name;
double txt_width;
double txt_height;
%}

upper_alpha   [A-Z]
lower_alpha   [a-z]
num           [0-9]
alpha         ({upper_alpha}|{lower_alpha})
alphanum      ({alpha}|{num})
number        -?{num}+(\.({num}*))?
ident         {alpha}(([[|./:_]?)+{alphanum}((\[|\\\[|$){num}+(\]|\\\]|$))?)*

width         [wW][iI][dD][tT][hH]
height        [hH][eE][iI][gG][hH][tT]
area          [aA][rR][eE][aA]  

wsp           [ \t\n\r]*



%%

{width}       { txt_flagwidth=1 ;}

{height}      { txt_flagheight=1 ;}

{area}        { txt_flagarea=1 ;}

{ident}       {   
                  if(!txt_flagarea && txt_flagwidth2 && txt_flagheight2){
                      txt_flagwidth2=0;
                      txt_flagheight2=0;
                      txt_get=addhtitem(tut_tablasharea,txt_name,(long)(txt_width*txt_height*1000.0));
                  }
                  else if(txt_flagarea){
                      txt_flagarea=0;
                  }
                  txtarealval.text = namealloc (yytext) ;
                  txt_name=txtarealval.text;
              }

{number}      { 
                  if(txt_flagarea) {
                      txtarealval.real = strtod (yytext, (char**)NULL);
                      txt_get=addhtitem(tut_tablasharea,txt_name,(long)(txtarealval.real*1000.0));
                  }
                  else if(txt_flagwidth) {
                      txt_width = strtod (yytext, (char**)NULL);
                      txt_flagwidth2=1;
                      txt_flagwidth=0;
                  }
                  else if(txt_flagheight) {
                      txt_height = strtod (yytext, (char**)NULL);
                      txt_flagheight2=1;
                      txt_flagheight=0;
                  }
                  
              }
{wsp}         {;}

%%



int yywrap () { return 1 ; }
