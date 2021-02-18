/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_library_lexer.lex                                       */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 04/05/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/


%{
#include "string.h"
#include "stdlib.h"
#include MUT_H
#define API_HIDE_TOKENS
#include "api_lib.h"
#include "gen_library_parser.h"

#define yylval gen_library_parserlval

int lineno_library=1;              /* to count the number of lines parsed */

#define YY_NO_UNPUT

%}

word ([a-zA-Z0-9_\.\/]*)
num (([1-9][0-9]*)|0)

%x COMMENTARY

%%
<INITIAL>rule(s)?                           {return _RULE;}
<INITIAL>action(s)?                         {return _ACTIONS;}

[ \t]				   {}
<COMMENTARY,INITIAL>\n		   {lineno_library++;}
\-\-.*				   {}
\/\*				   {BEGIN COMMENTARY;}
<COMMENTARY>[^(\n|(\*\/))]+	   {}
<COMMENTARY>\*		           {}
<COMMENTARY>\*\/		   {BEGIN INITIAL;}
<COMMENTARY>.		           {}
<INITIAL>\/\/[^\n]*              {}
<INITIAL>{num}			   {yylval.integer=atoi(yytext); return _DIGIT;}
<INITIAL>{word}		      	   {yylval.string=(char*)mbkalloc(strlen(yytext)+1); yylval.string=strcpy(yylval.string,yytext); return _IDENT;}
<INITIAL>\"[^\"]*\"	      	   { char buf[1024]; yylval.string=(char*)mbkalloc(strlen(yytext)+1); yylval.string=strcpy(yylval.string,&yytext[1]); yylval.string[strlen(yylval.string)-1]='\0'; return _IDENT;}
<INITIAL>.		   {return *yytext;}
%%



/*****************************************************************************/
/*                stop searching other files to parse ?                      */
/*****************************************************************************/
extern int yywrap() {
   return 1;
}

