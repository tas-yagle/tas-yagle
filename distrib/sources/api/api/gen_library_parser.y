/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_library_parser.yac                                      */
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
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include MUT_H
//#include MLO_H
#define API_HIDE_TOKENS
#include API_H
  
#define yytext gen_library_parsertext
#define yyin gen_library_parserin
  
#define MAXENTRY 100


/*global variable*/
 static char *PATH;                        /*path for files*/
 static char *file, *subfig, *bbox;        /*name of the library*/
 static int order, format, keep, addaction, paramset;
 /* syntax error schedule */
 extern int yylex();                /* parse ahead (by step of one char)*/
 extern int lineno_library;	     /* line number to give user's information */
 extern char *yytext;               /* current parsed string */
 extern FILE *yyin;                 /* flow for LEX */
 static inline int yyerror();              /* display error message */
 static void forceexit();

 /*for internal use*/
 static lib_entry *put_inlist_warn(char *elem, lib_entry *liste, int warn);
 lib_entry *TakeDependancies(char *elem, lib_entry *liste);
 static lib_entry *head;

 %}


%union {
  char *string;
  char name[400];
  int integer;
  ptype_list *type;
  chain_list *chain_l;
  lib_entry *le;
}


%type <name> file_name
%type <le> files_list template files_list_sub .rule. .actions.
%type <chain_l> .define.
%start liste

%token _STOP _YES _NO _KEEP _FORMAT _PRIORITY _VHDL _SPICE _RULE _ACTIONS
%token <string> _IDENT
%token <integer> _DIGIT
%%


 /****************************************************************************/
 /*             A library file can contain several model files               */
 /****************************************************************************/

liste:
files_list { head=$1; }
| { head=NULL; }
;

files_list:
file_name {order=lineno_library; format=keep=0; bbox=subfig=NULL;} .params.ka. ';' {$$=put_inlist_warn($1,NULL, 1);}
|
'{' file_name '}' {order=lineno_library; format=keep=0; bbox=subfig=NULL;} .params.ka. ';'
{
  char temp[512];
  char temp2[512];
  sprintf(temp, "%s/%s", getdistpath_2(temp2), $2);
  $$=put_inlist_warn(temp,NULL, 1);
}
|
template { $$=$1; }
|
files_list file_name {order=lineno_library; format=keep=0; bbox=subfig=NULL; } .params.ka. ';' {$$=put_inlist_warn($2,$1, 1);}
|
files_list template { $$=(lib_entry *)append((chain_list *)$1, (chain_list *)$2); }
|
files_list '{' file_name '}' {order=lineno_library; format=keep=0; bbox=subfig=NULL;} .params.ka. ';'
{
  char temp[512];
  char temp2[512];
  sprintf(temp, "%s/%s", getdistpath_2(temp2), $3);
  $$=put_inlist_warn(temp,$1, 1);
}
;

template:
_IDENT _IDENT '{' .rule. .actions. .define. '}' {order=-1; format=keep=-1; bbox=subfig=NULL; } .params.ka. ';'
{
  chain_list *cl;
  lib_entry *le, *lepack;
  char temp[1024];

  for (le=$4; le!=NULL; le=le->NEXT)
    {
      if (keep!=-1) le->keep=keep;
      if (order!=-1) le->priority=order;
      le->model=namealloc($1);
    }

  if ($4==NULL) 
    {
      if (order==-1) order=lineno_library;
      if (keep==-1) keep=0;
      format=0;
      sprintf(temp,"%s.vhd", $1);
      $4=put_inlist_warn(temp,NULL, 1);
      $4->model=namealloc($1);
    }

  if ($5==NULL) 
    {
      order=lineno_library; format=keep=0;
      sprintf(temp,"%s.c", $1);
      $5=put_inlist_warn(temp,NULL, 1);
    }
  
  lepack=(lib_entry *)append((chain_list *)$4, (chain_list *)$5);
  
  cl=addchain($6, create_adt($1, $2));
  for (le=lepack; le!=NULL; le=le->NEXT)
    {
      if (le==lepack)
        le->paramset=cl;
      else
        le->paramset=dup_adt_list(cl);
      if (gen_find_template_corresp(NULL, le->paramset, "exec_function", temp)==0)
      {
        if (strcmp(temp,"exec_function")!=0)
          le->model=namealloc(temp);
      }
    }
  $$=lepack;
  mbkfree($1);
  mbkfree($2);
}
;

.rule.:
_RULE '=' '{' files_list_sub '}' { $$=$4; }
|
_RULE '=' file_name {order=lineno_library; format=keep=0; bbox=subfig=NULL;} .params.ka. ';' {$$=put_inlist_warn($3, NULL, 1);}
|
{ $$ =NULL; }
;

.actions.:
_ACTIONS '=' '{' files_list_sub '}' { $$=$4; }
|
_ACTIONS '=' file_name {order=lineno_library; format=keep=0; bbox=subfig=NULL;} .params.ka. ';' {$$=put_inlist_warn($3, NULL, 1);}
|
{ $$=NULL; }
;

files_list_sub:
files_list_sub file_name {order=lineno_library; format=keep=0; bbox=subfig=NULL;} .params.ka. ';' {$$=put_inlist_warn($2, $1, 1);}
|
file_name {order=lineno_library; format=keep=0; bbox=subfig=NULL;} .params.ka. ';' {$$=put_inlist_warn($1, NULL, 1);}
;

.define.:
_IDENT '=' _IDENT
{
  $$=addchain(NULL, create_adt($1, $3));
  mbkfree($1);
  mbkfree($3);
}
|
.define. ',' _IDENT '=' _IDENT
{
  $$=addchain($1, create_adt($3, $5));
  mbkfree($5);
  mbkfree($3);
}
|
{
  $$=NULL;
}
;

.params.ka.:
':' .params.
|
;

.params.:
.order. .more.params.
|
;

.more.params.:
',' .params.
|
;

.order.:
_IDENT '=' _DIGIT     
{
  if (strcasecmp($1,"priority")==0) order =INT_MIN+$3;
  else { yyerror(); YYABORT; }
  mbkfree($1);
}
|
_IDENT '=' _IDENT
{
  if (strcasecmp($1,"format")==0)
    {
      if (strcasecmp($3, "spice")==0) format = 1;
      else if (strcasecmp($3, "vhdl")==0) format = 0;
      else if (strcasecmp($3, "spice_hr")==0) format = 2;
      else { yyerror(); YYABORT; }
    }
  else if (strcasecmp($1,"keep")==0)
    {
      if (strcasecmp($3, "yes")==0)  keep = 1;
      else if (strcasecmp($3, "no")==0)  keep = 0;
      else { yyerror(); YYABORT; }
    }
  else if (strcasecmp($1,"subfigure")==0)
    {
      subfig=mbkstrdup($3);
    }
  else if (strcasecmp($1,"match")==0)
    {
      bbox=mbkstrdup($3);
    }
  else
    { yyerror(); YYABORT; }
  mbkfree($1);
  mbkfree($3);
}
;

.action_type.:
'(' _VHDL ')' { addaction=1; }
|
;

file_name:
file_name '.' _IDENT { sprintf($$,"%s.%s",$1,$3); mbkfree($3); }
|
file_name '/' _IDENT { sprintf($$,"%s/%s",$1,$3); mbkfree($3); }
|
_IDENT
{
  strcpy($$, $1);
//  sprintf($$,"%s%s",PATH,$1);
  mbkfree($1);
  /*char *name = (char*)mbkalloc(strlen(PATH)+strlen($1)+1);
    name=strcpy(name, PATH);
    name=strcat(name, $1);
    mbkfree($1);
    $$=name;*/
}
|
'/' _IDENT
{
  sprintf($$, "/%s", $2);
  //  sprintf($$,"%s%s",PATH,$1);
  mbkfree($2);
}
;



%%


 /****************************************************************************/
 /*              user's message for syntaxical error                         */
 /****************************************************************************/
static inline int yyerror()
{
  switch ((int)yychar) {
     case _STOP: 
	fprintf(stderr,"%s:%d: unexpected end of file\n",file,lineno_library); 
     break;
     default: 
	fprintf(stderr,"%s:%d: syntax error happened at '%s'\n",file,lineno_library,yytext);
     break;
  }
  Inc_Error();
  return lineno_library;
}



 /****************************************************************************/
 /*    return the liste sorted by its order contained in TYPE field          */
 /* the first(or with the lowest digit) in first place in the list           */
 /****************************************************************************/
static lib_entry* sort_by_order(lib_entry *liste) 
{
   int current_priority;
   lib_entry *p,*q,*pred,*first,*ret;

   if (!liste) return NULL;

   /*bubble sort*/
   current_priority=liste->priority;
   q=NULL;
   pred=NULL;
   for (p=liste;p;p=p->NEXT) {
      if (p->priority<current_priority) {
         current_priority=p->priority;
         pred=q;
      }
      q=p;
   }   

   if (!pred) {/*first in liste to put on first place*/
      ret=sort_by_order(liste->NEXT);
      liste->NEXT=ret;
      return liste;
   }   
   else {/*an another than the first*/
      first=pred->NEXT;
      pred->NEXT=first->NEXT;   /*put over the first*/
      ret=sort_by_order(liste);
      first->NEXT=ret;
      return first;
   }
}   

 /****************************************************************************/
 /*                          to get started                                  */
 /* return list of model files sorted by order:first as the highest priority */
 /* if number is attached the lowest digit for the highest priority          */
 /****************************************************************************/
lib_entry *APIReadLibrary(char *library, char *path, FILE *debug)
{
   lib_entry *res;
   if (!library || !path) {
      fprintf(stderr,"Read_All: NULL pointer\n");
      EXIT(1);
   }

   PATH=path;   /* var. glob */
   file=library;        /*var. glob. */
   /* model file exists? */
   if (!(yyin = mbkfopen(library, NULL, READ_TEXT))) {
       fprintf(stderr, "Cannot open library file %s (error n°%d)\n",
               library,errno);
       EXIT(-2);
   }
   if (debug)
     fprintf(debug, "Opening library file %s....\n",library);
   paramset=0;
   yyparse();

   res = head;

   res= sort_by_order(res);
   
   fclose(yyin);

   if (debug)
     fprintf(debug, "%s Closed\n",library);

   return res;
}


 /****************************************************************************/
 /*                          to get started                                  */
 /****************************************************************************/
static lib_entry *put_inlist_warn(char *elem, lib_entry *liste, int warn)
{
  lib_entry *le,*le1;
 
  if (!elem) {
    fprintf(stderr,"put_inlist_warn: NULL pointer\n"); 
    EXIT(1);
  }

//   sprintf(temp,"%s%s",path, elem);

   for (le=liste;le!=NULL;le=le->NEXT) 
     {
      if (warn && !strcmp(le->name,elem))
	{
	  fprintf(stderr,"%s:%d: %s named several times. this one is ignored\n",file,lineno_library-1,elem);
	  return liste;
	}
      if (!le->NEXT)
	{
	  le1=(lib_entry *)mbkalloc(sizeof(lib_entry));
	  le->NEXT=le1;
	  le1->name=mbkstrdup(elem);
          le1->subfigurename=subfig;
          le1->match=bbox;
	  le1->entity="?";
	  le1->priority=order;
	  le1->format=format;
          le1->paramset=NULL;
          le1->model=NULL;
          le1->keep=keep;
	  le1->tree=NULL;
	  le1->NEXT=NULL;
	  return liste;
	}
   }   
   
   le=(lib_entry *)mbkalloc(sizeof(lib_entry));
   le->name=mbkstrdup(elem);
   le->subfigurename=subfig;
   le->match=bbox;
   le->entity="?";
   le->priority=order;
   le->format=format;
   le->paramset=NULL;
   le->model=NULL;
   le->keep=keep;
   le->tree=NULL;
   le->NEXT=NULL;
   return le;
}



 /****************************************************************************/
 /*************************** END of LEX&YACC ********************************/
 /****************************************************************************/

void APIFreeLibrary(lib_entry *le)
{
  lib_entry *nle;
  while (le!=NULL)
    {
      nle=le->NEXT;
      free(le->name);
      if (le->subfigurename!=NULL) free(le->subfigurename);
      if (le->match!=NULL) free(le->match);
      free_adt_list(le->paramset);
      mbkfree(le);
      le=nle;
    }
}
