/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BEG_GEN Version 1.00                                        */
/*    Fichier : beg_gen.l                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/


%{
#include "beg_gen.tab.h"
#include MUT_H
#include "beg_gen.h"

#define ECHO // fprintf(stderr,"\n-=<{oo}>=- %s\n",yytext)
#define YY_NO_UNPUT
/* static YY_BUFFER_STATE yy_current_buffer = 0; */

extern char   *BEG_GEN_STR ;

#define YY_USER_INIT  yy_current_buffer = yy_scan_string (BEG_GEN_STR) ;
#define yyterminate() yy_delete_buffer (yy_current_buffer) ; yy_init = 1 ; return YY_NULL ;
 
%}

Upalpha             [A-Z]
Lowalpha            [a-z]
Num                 [0-9]*
SpeNum              (x|X|u|U|z|Z)
BinNum				(0|1|{SpeNum})
OctNum				([0-7]|{SpeNum})
HexNum           	({Num}|[a-fA-F]|{SpeNum})
Alpha               ({Upalpha}|{Lowalpha})
AlphaNum            ({Alpha}|{Num})
Variable            {Alpha}({AlphaNum}|[_/.@])*

LogicAnd            ([aA][nN][dD])
LogicOr             ([oO][rR])
LogicXor            ([xX][oO][rR])
LogicNot            ([nN][oO][tT])
LogicNor           	([Nn][oO][rR])
LogicNand           ([Nn][aA][nN][dD])
LogicNxor           ([Nn][xX][oO][rR])
Event				([Ee][Vv][Ee][Nn][Tt])
Stable				([Ss][Tt][Aa][Bb][Ll][Ee])

To					([Tt][Oo])
DownTo				([Dd][Oo][Ww][Nn][Tt][Oo])

BaseBin             (0[B|b])
BaseHex				(0[X|x])
BaseOct				(0[O|o])
Zero                ('0')
One                 ('1')
Hz					('[Zz]')
Undef				('[Uu]')
Conflict			('[Xx]')

%%

[ \t]               {ECHO ;                      }
=                   {ECHO ; return _EQUAL       ;} 
\(                  {ECHO ; return _LEFTPAR     ;} 
\)                  {ECHO ; return _RIGHPAR     ;} 
\^                  {ECHO ; return _REP         ;}
&					{ECHO ; return _CAT         ;}
'                   {ECHO ; return _QUOTE       ;}
:                   {ECHO ; return _COLON       ;}
{LogicAnd}          {ECHO ; return BEG_AND         ;}
{LogicOr}           {ECHO ; return _OR          ;}
{LogicXor}          {ECHO ; return _XOR         ;}
{LogicNot}          {ECHO ; return _NOT         ;}
{LogicNand}         {ECHO ; return _NAND        ;}
{LogicNor}          {ECHO ; return _NOR         ;}
{LogicNxor}         {ECHO ; return _NXOR        ;}
{Zero}              {ECHO ; return _ZERO        ;}
{One}               {ECHO ; return _ONE			;}
{Hz}                {ECHO ; return _HZ			;}
{Undef}             {ECHO ; return _UNDEF		;}
{Conflict}          {ECHO ; return _CONFLICT	;}
{To}				{ECHO ; return _TO			;}
{DownTo}			{ECHO ; return _DOWNTO		;}
{Event}				{ECHO ; return _EVENT		;}
{Stable}			{ECHO ; return _STABLE		;}

{BaseBin}([ ]?{BinNum}(_?{BinNum})*) {
                    ECHO ; beg_genlval.text = beg_genBitStr(yytext);
					return _BITSTR     										;}
{BaseOct}([ ]?{OctNum}(_?{OctNum})*) {
                    ECHO ; beg_genlval.text = beg_genBitStr(yytext);
					return _BITSTR     										;}
{BaseHex}([ ]?{HexNum}(_?{HexNum})*) {
                    ECHO ; beg_genlval.text = beg_genBitStr(yytext);
					return _BITSTR     										;}
{Variable}          {ECHO ; beg_genlval.text = mbkstrdup(yytext); return _NAME  ;}
{Num}               {ECHO ; beg_genlval.text = mbkstrdup(yytext); return _NUMBER;}

\+					{ECHO ; return _PLUS	;}
\*					{ECHO ; return _MULT	;}
\-					{ECHO ; return _MINUS	;}
\/					{ECHO ; return _DIV	;}

%%

int yywrap  ()	{ return (1) ; }


