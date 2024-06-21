/**************************/
/*     eqt.l              */
/**************************/



%{
#include "eqt.tab.h"
#include EQT_H
#include "eqt_util.h"
#define ECHO /*fprintf(stderr,"\ntoken %s\n",yytext)*/
#define YY_NO_UNPUT
/*static YY_BUFFER_STATE yy_current_buffer = 0;*/

extern eqt_ctx *EQT_CTX;
static char    *var;

#define YY_USER_INIT  yy_current_buffer = yy_scan_string (EQT_CTX->EQUA_STR) ;
#define yyterminate() yy_delete_buffer (yy_current_buffer) ; yy_init = 1 ; return YY_NULL ;
 
%}

upalpha                   [A-Z]
lowalpha                  [a-z]
num                       [0-9]
alpha                     ({upalpha}|{lowalpha})
unit                      {alpha}*
alphanum                  ({alpha}|{num})
exposant                  [eE]
sqrt                      ([sS][qQ][rR][tT])
exp                       ([eE][xX][pP])
log                       ([lL][oO][gG])
sin                       ([sS][iI][nN])
cos                       ([cC][oO][sS])
tan                       ([tT][aA][nN])
asin                      ([aA][sS][iI][nN])
acos                      ([aA][cC][oO][sS])
atan                      ([aA][tT][aA][nN])
atan2                     ([aA][tT][aA][nN]2)
sinh                      ([sS][iI][nN][hH])
cosh                      ([cC][oO][sS][hH])
tanh                      ([tT][aA][nN][hH])
log10                     ([lL][oO][gG]10)
ceil                      ([cC][eE][iL])
floor                     ([fF][lL][oO][oO][rR])
fabs                      ([fF][aA][bB][sS])
abs                       ([aA][bB][sS])
pow                       ([pP][oO][wW])
pwr                       ([pP][wW][rR])
number                    (("."{num}+)|({num}+("."{num}*)?))({exposant}[+|-]?{num}+)?({unit})?
variable                  (({alpha}|_)(\[|\]|{alpha}|{num}|_)*)|(\"[^\"]*\")
LogicAnd                  ([aA][nN][dD])
LogicXor                  ([xX][oO][rR])
LogicNot                  ([nN][oO][tT])
LogicOr                   ([oO][rR])
TlfOne                    (1'[bB]1)|('[bB]1)
TlfZero                   (1'[bB]0)|('[bB]0)
nom_mod                   ([nN][oO][mM]_[mM][oO][dD])
nom_dev                   ([nN][oO][mM]_[dD][eE][vV])
nom_sub                   ([nN][oO][mM]_[sS][uU][bB])


%%
\?                {ECHO ; return '?'         ;}
\:                {ECHO ; return ':'         ;}
[ \t]             {ECHO ;                     }
=                 {ECHO ; return '='         ;} 
\+                {ECHO ; return '+'         ;} 
-                 {ECHO ; return '-'         ;} 
\/                {ECHO ; return '/'         ;} 
\*                {ECHO ; return '*'         ;} 
[({]              {ECHO ; return '('         ;} 
[)}]              {ECHO ; return ')'         ;} 
\*\*              {ECHO ; return '^'         ;}
\^                {ECHO ; return '^'         ;}
,                 {ECHO ; return ','         ;}
{pow}             {ECHO ; return T_POW       ;}
{pwr}             {ECHO ; return T_POW       ;}
{LogicAnd}        {ECHO ; return T_AND       ;}
&&                {ECHO ; return T_AND       ;}
&                 {ECHO ; return T_AND       ;}
{LogicXor}        {ECHO ; return T_XOR       ;}
{LogicNot}        {ECHO ; return T_NOT       ;}
!                 {ECHO ; return T_NOT       ;}
~                 {ECHO ; return T_NOT       ;}
{LogicOr}         {ECHO ; return T_OR        ;}
\|\|              {ECHO ; return T_OR        ;}
\|                {ECHO ; return T_OR        ;}
\<                {ECHO ; return T_INF       ;}
\>                {ECHO ; return T_SUP       ;}
\<=               {ECHO ; return T_INFEQ     ;}
\>=               {ECHO ; return T_SUPEQ     ;}
==                {ECHO ; return T_EQ        ;}
!=                {ECHO ; return T_NOTEQ     ;}
'                 {ECHO ; return T_QUOTE     ;}
{TlfOne}          {ECHO ; return T_TLFONE    ;}
{TlfZero}         {ECHO ; return T_TLFZERO   ;}
{sqrt}            {ECHO ; return T_SQRT      ;}
{exp}             {ECHO ; return T_EXP       ;}
{log}             {ECHO ; return T_LOG       ;}
{sin}             {ECHO ; return T_SIN       ;}
{cos}             {ECHO ; return T_COS       ;}
{tan}             {ECHO ; return T_TAN       ;}
{asin}            {ECHO ; return T_ASIN      ;}
{acos}            {ECHO ; return T_ACOS      ;}
{atan}            {ECHO ; return T_ATAN      ;}
{atan2}           {ECHO ; return T_ATAN2     ;}
{sinh}            {ECHO ; return T_SINH      ;}
{cosh}            {ECHO ; return T_COSH      ;}
{tanh}            {ECHO ; return T_TANH      ;}
{log10}           {ECHO ; return T_LOG10     ;}
{ceil}            {ECHO ; return T_CEIL      ;}
{floor}           {ECHO ; return T_FLOOR     ;}
{fabs}            {ECHO ; return T_FABS      ;}
{abs}             {ECHO ; return T_ABS       ;}
{nom_dev}         {ECHO ; return T_NOMDEV    ;}
{nom_mod}         {ECHO ; return T_NOMMOD    ;}
{nom_sub}         {ECHO ; return T_NOMSUB    ;}

{number}          {ECHO ; eqtlval.text = mbkstrdup (yytext); return (T_NUMBER) ; }
{variable}        {ECHO ; 
                   var = namealloc (yytext) ;
                   eqtlval.text = var ;
                   return T_VAR ;
/*
				   if (eqt_getindex (EQT_CTX, var) == -1)  
					  return T_VAR ;
				   else 
				      return T_USER;
*/
		  }
%%

int yywrap () { return 1 ; }
void eqtinit () {  yy_delete_buffer (yy_current_buffer) ; yy_init = 1 ; }
