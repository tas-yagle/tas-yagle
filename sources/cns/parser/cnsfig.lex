/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Dpartement ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
%{
#include <stdlib.h>
#include        "prsall.h"
#include        "y.tab.h"
extern int      cnsfiglineno;
#define YY_NO_UNPUT
%}

digit    [0-9]
number   {digit}+
oper     "+"|"-"

%%
\n             {cnsfiglineno++;}

[ \t]+         {}

^#.*$          {}

CNS            {return cnsCNS;}

({digit}{digit}):({digit}{digit}):({digit}{digit}) {return cnsTIME;}

SCALE          {return cnsSCALE;}

CONNECTORS     {return cnsCONNECTORS;}

SIGNALS        {return cnsSIGNALS;}

WIRES          {return cnsWIRES;}

CROSSTALKS     {return cnsCROSSTALKS;}

TRANSISTORS    {return cnsTRANSISTORS;}

INSTANCES      {return cnsINSTANCES;}

CELLS          {return cnsCELLS;}

CONES          {return cnsCONES;}

INPUT          {return cnsINPUT;}

OUTPUT         {return cnsOUTPUT;}

BRANCHES       {return cnsBRANCHES;}

USER           {return cnsUSER;}

PARALLEL       {return cnsPARALLEL;}

PARATRANS      {return cnsPARATRANS;}

SWITCH         {return cnsSWITCH;}

SIGNATURE      {return cnsSIGNATURE;}

SIGIDX         {return cnsSIGIDX;}

BLEEDER        {return cnsBLEEDER;}

INTERNAL       {return cnsINTERNAL;}

#              {return cnsNODES;}

{oper}?{digit}*"."{number}([e|E]{oper}?{number})? {strcpy(yylval.cstr, yytext); return(cnsSTRING);}

{oper}?{number}            {strcpy(yylval.cstr, yytext); return(cnsSTRING);}

[0-9a-zA-Z&_!?$%+-][^ \t\n()\=]*  {strcpy(yylval.cstr, yytext); return(cnsSTRING);}
\"[0-9a-zA-Z&_!?$%+-][^ \t\n()\=]*[ ][0-9]*\"  {strcpy(yylval.cstr, yytext+1); (yylval.cstr)[strlen(yytext)-2] = 0; return(cnsSTRING);}

[()\[\]\=]           {return(*yytext);}

"-"/" "        {return(*yytext);}

[^ \t\n]       {}

%%

int yywrap() {return(1);}
