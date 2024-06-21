%{
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include MUT_H
#include "apitype.h"

#define MAXCOMMENT 5000

#include "wrapper_parser.h"

int lineno=1;                       /* to count the number of lines parsed */
static char *zone;
static int i;
chain_list *mans=NULL;
chain_list *comment;
#define YY_NO_UNPUT

void addcomment();

%}
word_c ([a-zA-Z_][a-zA-Z0-9_]*)
num ([0-9]+)
hexa (0[xX](([1-9a-fA-F][0-9a-fA-F]*)|0))
letter ('.')

%x COMMENT_C

%%
\.\.\.                          { return AVTWIG_TOKEN_VAR_PARAM; }
\n                              { lineno++; }
<INITIAL>\/\*                   { BEGIN COMMENT_C; i=0; zone=mbkalloc(MAXCOMMENT); comment=NULL; }
<COMMENT_C>\*\/                 { BEGIN INITIAL; zone[i]='\0'; comment=addchain(comment,strdup(zone)); i=0; mbkfree(zone); addcomment(); comment=NULL; }
<COMMENT_C>.                    { zone[i++]=*yytext; }
<COMMENT_C>\n                   { zone[i]='\n'; zone[i+1]='\0'; comment=addchain(comment,strdup(zone)); i=0; lineno++; }
[ \t]                           { }
<INITIAL>\/\/[^\n]*             { }
\#ifdef[^\n]*                   { }
\#ifndef[^\n]*                  { }
\#endif[^\n]*                   { }
\#else[^\n]*                    { }
typedef[^\n]*                   { }
void                            { return AVTWIG_TOKEN_VOIDT; }
int                             { return AVTWIG_TOKEN_INT; }
long                            { return AVTWIG_TOKEN_LONG; }
double                          { return AVTWIG_TOKEN_DOUBLE; }
const                           { return AVTWIG_TOKEN_CONST; }
char                            { return AVTWIG_TOKEN_CHAR; }
unsigned                        { return AVTWIG_TOKEN_UNSIGNED; }
extern                          { return AVTWIG_TOKEN_EXTERN; }
\#undef[^\n]*                   { }
\#include[^\n]*                 { yylval.string=strdup(yytext); return AVTWIG_TOKEN_INCLUDE; }
\#define                        { yylval.string=strdup(yytext); return AVTWIG_TOKEN_DEFINE; }
{word_c}                        { yylval.string=strdup(yytext); return AVTWIG_TOKEN_IDENT; }
{letter}			{yylval.integer=yytext[1]; return AVTWIG_TOKEN_DIGIT_CHAR;}
{num}(\.{num})?[eE][+-]?{num} 	{yylval.real=strtod(yytext,NULL); return AVTWIG_TOKEN_DIGIT_DOUBLE;}
{num}\.{num} 	                {yylval.real=strtod(yytext,NULL); return AVTWIG_TOKEN_DIGIT_DOUBLE;}
{num}				{yylval.integer=atoi(yytext); return AVTWIG_TOKEN_DIGIT;}
{hexa}			        {int i,val,taille=strlen(yytext)-1;
                                 yylval.integer=0;
                                 for (i=taille;i>=2/*0x*/;i--) {
                                   if (yytext[i]>='a')
                                     val=yytext[i]-'a'+10;
                                   else if (yytext[i]>='A') 
                                          val=yytext[i]-'A'+10;
                                   else val=yytext[i]-'0';
                                   yylval.integer+=val*pow(16,taille-i);
                                 }
                                 return AVTWIG_TOKEN_DIGIT;
                                }
.                               { return *yytext; }
%%

extern int yywrap() {
       return 1;
}

void distroycomment()
{
 chain_list *cl;
 for (cl=comment;cl!=NULL;cl=cl->NEXT) free(cl->DATA);
 freechain(comment);
}

char *tonextword(char *l)
{
 while (*l==' ') l++;
 return l;
}

char *getword(char *l, char *buf)
{
 int i=0;
 while (*l!=' ' && *l!='\n' && *l!='\0') { buf[i++]=*l; l++; }
 buf[i]='\0';
 return l;
}

void addcomment()
{
 char *l;
 char word[100];
 comment=reverse(comment);
 if (comment==NULL || comment->NEXT==NULL) { distroycomment(); return; }
 l=(char *)comment->NEXT->DATA;
 l=tonextword(l);
 l=getword(l,word);
 if (strcmp(word,"MAN")!=0) { distroycomment(); return; }
 l=tonextword(l);
 l=getword(l,word);
 if (strlen(word)==0) { distroycomment(); return; }
 comment=addchain(comment, strdup(word));
 mans=addchain(mans, comment);
}

