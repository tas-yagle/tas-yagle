
%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "apitype.h"

#include MUT_H
#include MLO_H
#define API_HIDE_TOKENS
#include API_H

#define T_UNKNOWN -58963

/* syntax error schedule */
extern int yylex();                /* parse ahead (by step of one char)*/
extern int lineno;                 /* line number to give user's information */
extern FILE *yyin;                 /* flow from LEX */
extern char *yytext;               /* current parsed string */
static inline int yyerror();              /* display error message */
char *wrapperfile;

 chain_list *proto_list=NULL, *includes=NULL;
 chain_list *alldefines=NULL;
 extern chain_list *mans;
 %}

/* %expect 1 */

%union {
  char *string;
  int integer;
  double real;
  defines *def;
  _ApiType sw;
  chain_list *chain;
}

%type <sw> base_type type .argument.
%type <chain> proto arguments .arguments. .includes. includes
%type <integer> pointer_depth

%token <integer> AVTWIG_TOKEN_DIGIT_CHAR AVTWIG_TOKEN_DIGIT
%token <real> AVTWIG_TOKEN_DIGIT_DOUBLE
%type <def> defines_val some_defines

%token AVTWIG_TOKEN_STOP                            /*EOF*/
%token AVTWIG_TOKEN_VOIDT
%token <string> AVTWIG_TOKEN_IDENT
%token AVTWIG_TOKEN_CHAR AVTWIG_TOKEN_CONST
%token AVTWIG_TOKEN_DOUBLE
%token AVTWIG_TOKEN_INT
%token AVTWIG_TOKEN_LONG
%token AVTWIG_TOKEN_EXTERN
%token AVTWIG_TOKEN_DEFINE
%token AVTWIG_TOKEN_VAR_PARAM AVTWIG_TOKEN_UNSIGNED

%token <string> AVTWIG_TOKEN_INCLUDE

%start start_parse

%%

start_parse:
.includes. proto_list { includes=$1; }
|
;

.includes.:
includes { $$=$1; }
|
{ $$ = NULL; }
;

includes:
AVTWIG_TOKEN_INCLUDE { $$=addchain(NULL, $1); }
|
AVTWIG_TOKEN_INCLUDE includes { $$=addchain($2, $1); }
;

proto_list:
  proto                       { proto_list=addchain(proto_list, $1); }
| proto_list proto            { proto_list=addchain(proto_list, $2); }
| some_defines                { if ($1!=NULL) alldefines=addchain(alldefines, $1); }
| proto_list some_defines     { if ($2!=NULL) alldefines=addchain(alldefines, $2); }
;


proto:
AVTWIG_TOKEN_EXTERN base_type AVTWIG_TOKEN_IDENT '(' .arguments. ')' ';'
{
  _ApiType *sw;
  sw=mbkalloc(sizeof(struct _ApiType));
  *sw=$2; //memcpy(sw,&$2,sizeof(struct _ApiType));
  $$=addchain($5, sw);
  $$=addchain($$, $3);
}
|
base_type AVTWIG_TOKEN_IDENT '(' .arguments. ')' ';'
{
  _ApiType *sw;
  sw=mbkalloc(sizeof(_ApiType));
  *sw=$1;
  $$=addchain($4, sw);
  $$=addchain($$, $2);
}
|
AVTWIG_TOKEN_IDENT '(' .arguments. ')' ';'
{
  _ApiType *sw;
  sw=mbkalloc(sizeof(_ApiType));
  sw->TYPE=T_INT;
  sw->TYPE_NAME="int";
  sw->varname="?";
  $$=addchain($3, sw);
  $$=addchain($$, $1);
}
;

some_defines:
AVTWIG_TOKEN_DEFINE AVTWIG_TOKEN_IDENT defines_val
{
  if ($3!=NULL)
    {
      $3->VARNAME=$2;
      $$=$3;
    }
  else
    $$=NULL;
}
;

defines_val:
AVTWIG_TOKEN_DIGIT
{
  defines *def=(defines *)malloc(sizeof(defines));
  def->type='I';
  def->T.I=$1;
  $$=def;
}
|
AVTWIG_TOKEN_DIGIT_DOUBLE
{
  defines *def=(defines *)malloc(sizeof(defines));
  def->type='D';
  def->T.D=$1;
  $$=def;
}
|
AVTWIG_TOKEN_DIGIT_CHAR
{
  defines *def=(defines *)malloc(sizeof(defines));
  def->type='C';
  def->T.I=$1;
  $$=def;
}
|
type { $$ = NULL; }
;

base_type:
type { $$ = $1; }
|
type pointer_depth 
{
  char temp[256];
  int i;
  $$ = $1; 
  strcpy(temp,$$.TYPE_NAME);
  strcat(temp," ");
  for (i=1;i<=$2;i++)
    strcat(temp,"*");
  $$.TYPE_NAME=strdup(temp);
  $$.TYPE=T_POINTER;
}
;

pointer_depth:
'*' { $$=1; }
|
pointer_depth '*' { $$ = $1+1; }
;

type:
AVTWIG_TOKEN_VOIDT { $$.TYPE = T_VOID; $$.TYPE_NAME="void"; $$.cst=0; }
|
AVTWIG_TOKEN_INT { $$.TYPE = T_INT; $$.TYPE_NAME="int"; $$.cst=0; }
|
AVTWIG_TOKEN_UNSIGNED AVTWIG_TOKEN_INT { $$.TYPE = T_INT; $$.TYPE_NAME="int"; $$.cst=0; }
|
AVTWIG_TOKEN_LONG { $$.TYPE = T_LONG; $$.TYPE_NAME="long"; $$.cst=0; }
|
AVTWIG_TOKEN_UNSIGNED AVTWIG_TOKEN_LONG { $$.TYPE = T_LONG; $$.TYPE_NAME="long"; $$.cst=0; }
|
AVTWIG_TOKEN_DOUBLE { $$.TYPE = T_DOUBLE; $$.TYPE_NAME="double"; $$.cst=0; }
|
AVTWIG_TOKEN_CHAR { $$.TYPE = T_CHAR; $$.TYPE_NAME="char";$$.cst=0; }
|
AVTWIG_TOKEN_IDENT { $$.TYPE = T_UNKNOWN; $$.TYPE_NAME=$1; $$.cst=0; }
|
AVTWIG_TOKEN_CONST AVTWIG_TOKEN_VOIDT { $$.TYPE = T_VOID; $$.TYPE_NAME="void"; $$.cst=1; }
|
AVTWIG_TOKEN_CONST AVTWIG_TOKEN_INT { $$.TYPE = T_INT; $$.TYPE_NAME="int"; $$.cst=1; }
|
AVTWIG_TOKEN_CONST AVTWIG_TOKEN_LONG { $$.TYPE = T_LONG; $$.TYPE_NAME="long"; $$.cst=1; }
|
AVTWIG_TOKEN_CONST AVTWIG_TOKEN_DOUBLE { $$.TYPE = T_DOUBLE; $$.TYPE_NAME="double"; $$.cst=1; }
|
AVTWIG_TOKEN_CONST AVTWIG_TOKEN_CHAR { $$.TYPE = T_CHAR; $$.TYPE_NAME="char";$$.cst=1;  }
|
AVTWIG_TOKEN_CONST AVTWIG_TOKEN_IDENT { $$.TYPE = T_UNKNOWN; $$.TYPE_NAME=$2; $$.cst=1; }
;

.arguments.:
arguments { $$ = $1; }
|
{ $$ = NULL; }
;

arguments:
.argument. ',' arguments
{
  _ApiType *sw;
  sw=mbkalloc(sizeof(_ApiType));
  *sw=$1;
  $$ = addchain($3,sw);
}
|
.argument.
{
  _ApiType *sw;
  sw=mbkalloc(sizeof(_ApiType));
  *sw=$1;
  $$ = addchain(NULL,sw);
}
;


.argument.:
base_type AVTWIG_TOKEN_IDENT { $$ = $1; $$.varname=$2; }
|
base_type { $$ = $1; $$.varname="?"; }
| 
AVTWIG_TOKEN_VAR_PARAM {  $$.TYPE = T_UNKNOWN; $$.TYPE_NAME="..."; $$.varname=""; $$.cst=0; }
;

%%


 /****************************************************************************/
 /*              user's message for syntaxical error                         */
 /****************************************************************************/
static inline int yyerror()
{
  switch ((int)yychar) 
    {
    case AVTWIG_TOKEN_STOP: 
      fprintf(stderr,"%s:%d: unexpected end of file\n",wrapperfile,lineno); 
      break;
    default: 
      fprintf(stderr,"%s:%d: syntax error append at '%s'\n",wrapperfile,lineno,yytext);
    }
  return 0;
}

extern char *tonextword(char *l);
extern char *getword(char *l, char *buf);

functionman *format_man(chain_list *lines)
{
  chain_list *cl;
  char *desc=NULL, *retval=NULL, *seealso=NULL, *errors=NULL, *l, *synop=NULL;
  char **cur=NULL, **oldcur=NULL, *uppersection=NULL, *categ=NULL;
  char *bigbuf=(char *)malloc(sizeof(char)*100000);
  char word[100], buf[4096];
  functionman *fman;
  
  strcpy(bigbuf,"");

  for (cl=lines;cl!=NULL;cl=cl->NEXT)
    {
      strcpy(buf, (char *)cl->DATA);
      l=tonextword(buf);
      l=getword(l, word);
      oldcur=cur;
      if (strcmp(word,"DESCRIPTION")==0) cur=&desc;
      else
        if (strcmp(word,"RETURN")==0)
          {
            l=tonextword(l);
            l=getword(l, word);
            if (strcmp(word,"VALUE")==0)
              cur=&retval;
          }
        else
          if (strcmp(word,"FIRSTOF")==0) cur=&uppersection;
          else
            if (strcmp(word,"SYNOPSIS")==0) cur=&synop;
          else
            if (strcmp(word,"ARGS")==0) cur=&errors;
            else 
              if (strcmp(word,"SEE")==0)
                {
                  l=tonextword(l);
                  l=getword(l, word);
                  if (strcmp(word,"ALSO")==0)
                    cur=&seealso;
                }
              else if (strcmp(word,"CATEG")==0)
               {
                 l=tonextword(l);
                 l=getword(l, word);
                 categ=strdup(word);
               }
      if (cur!=NULL && oldcur!=cur)
        {
          // a new section
          if (oldcur!=NULL)
            {
              if (*oldcur!=NULL)
                free(*oldcur);
              *oldcur=strdup(bigbuf);
            }
          if (*cur!=NULL)
            strcpy(bigbuf, *cur);
          else
            strcpy(bigbuf, "");
        }
    else
    {
      strcat(bigbuf, (char *)cl->DATA);
      //strcat(bigbuf, "\n.LP\n");
     }
     free(cl->DATA);
    }
  freechain(lines);
  if (cur!=NULL)
    {
      // a new section
      if (*cur!=NULL) free(*cur);
      *cur=strdup(bigbuf);
    }

  fman=(functionman *)mbkalloc(sizeof(functionman));
  fman->uppersection=uppersection?uppersection:"";
  fman->seealso=seealso?seealso:"";
  fman->errors=errors?errors:"";
  fman->retval=retval?retval:"";
  fman->desc=desc?desc:"";
  fman->categ=categ?categ:"";
  fman->synop=synop;

  free(bigbuf);

  return fman;
}


void setmans()
{
  chain_list *newcl=NULL, *cl, *ch;
  functionman *fman;
  for (cl=proto_list;cl!=NULL;cl=cl->NEXT)
    {
      for (ch=mans;ch!=NULL;ch=ch->NEXT)
        {
          if (ch->DATA!=NULL && strcmp((char *)((chain_list *)ch->DATA)->DATA,(char *)((chain_list *)cl->DATA)->DATA)==0) break;
        }

      if (ch==NULL)
        newcl=addchain(newcl, NULL);
      else
        {
          fman=format_man(((chain_list *)ch->DATA)->NEXT);
          fman->funcname=ch->DATA;
          newcl=addchain(newcl, fman);
          ((chain_list *)ch->DATA)->NEXT=NULL; freechain((chain_list *)ch->DATA);
          ch->DATA=NULL;
        }
    }
  newcl=reverse(newcl);
  freechain(mans);
  mans=newcl;
}
