/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod.l                                                     */
/*                                                                            */
/*                                                                            */
/*    (c) copyright 2001 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Marc KUOCH                                                  */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

%{
#include "mcc_mod.tab.h"
#include MCC_H
#include "mcc_mod_util.h"
#define YY_NO_UNPUT
#define ECHO /*fprintf(stderr,"\ntoken %s\n",yytext)*/

int MCC_LINE;

char *mcc_libenv,*mcc_str,*mcc_strp,*mcc_buf ;

typedef struct mcc_context {
struct mcc_context *next ;
YY_BUFFER_STATE     buffer ;
int                 line ;
char               *libtype ;
char               *filename ;
char               *curlibtype ;
FILE               *file ;
} mcc_contextlist ;

mcc_contextlist *MCCCONTEXT = NULL ;
extern char *LIBTYPE ;
extern char *CURLIBTYPE ;
extern char *FILENAME ;
extern int popcontext() ;
extern int mcc_moderror() ;
%}

num                       [0-9]
wsp                       [ \t\r]
number                    [+-]?(("."{num}+)|({num}+("."{num}*)?))([e|E][+|-]?{num}+)?
string                    [a-zA-Z&_!?%][^ \"=:\r\t\n()]*
function                  [a-zA-Z_0-9]*\([^\n]*\)
alpha                     [a-zA-Z]
ident                     {alpha}{string}
symb_mod                  [mM][oO][dD][eE][lL]
symb_par                  [pP][aA][rR][aA][mM]
symb_lib                  [lL][iI][bB]
symb_include              [iI][nN][cC][lL][uU][dD][eE]
symb_endl                 [eE][nN][dD][lL]
symb_prot                 [pP][rR][oO][tT]
symb_unprot               [uU][nN][pP][rR][oO][tT]
symb_option               [oO][pP][tT][iI][oO][nN][sS]?
symb_subckt               [sS][uU][bB][cC][kK][tT]
symb_ends                 [eE][nN][dD][sS]
nmos                      [nN][mM][oO][sS]
pmos                      [pP][mM][oO][sS]
value                     [vV][aA][lL][uU][eE]

giga                      [gG]
kilo                      [kK]
mili                      [mM]
micro                     [uU]
nano                      [nN]
pico                      [pP]
femto                     [fF]

%x EQU UNIT ENV ENV2

%%
{wsp}*                     {ECHO                                        ;}
\n                         {ECHO ; MCC_LINE++                           ;}
^{wsp}*[*#].*$             {ECHO ; /* comment */                        ;}
[!].*$                     {ECHO ; /* comment */                        ;}
^{wsp}*\+                  {ECHO                                        ;}
^{wsp}*\.{symb_lib}        {ECHO ; return T_LIB                         ;}
^{wsp}*\.{symb_endl}       {ECHO ; return T_ENDL                        ;}
^{wsp}*\.{symb_subckt}     {ECHO ; return T_SUBCKT                      ;}
^{wsp}*\.{symb_ends}       {ECHO ; return T_ENDS                        ;}
^{wsp}*\.{symb_par}        {ECHO ; return T_VAR                         ;}
{symb_par}{wsp}*[:]        {ECHO ; return T_VARD                        ;}
^{wsp}*\.{symb_mod}        {ECHO ; return T_MODEL                       ;}
^{wsp}*\.{symb_include}    {ECHO ; return T_INCLUDE                     ;}
^{wsp}*\.{symb_prot}       {ECHO ;                                      ;}
^{wsp}*\.{symb_unprot}     {ECHO ;                                      ;}
^{wsp}*\.{symb_option}.*   {ECHO ;                                      ;}
^{wsp}*[dD]{num}*{string}* {ECHO ; return T_DIODE                       ;}
^{wsp}*[qQ]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[Js]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[Mm]{num}*{string}* {ECHO ; mcc_modlval.text = mbkstrdup(mcc_modtext) ; return T_TRANSISTOR ;}
^{wsp}*[rR]{num}*{string}* {ECHO ; return T_RESISTOR                    ;}
^{wsp}*[cC]{num}*{string}* {ECHO ; return T_CAPACITOR                   ;}
^{wsp}*[xX]{num}*{string}* {ECHO ; mcc_modlval.text = mbkstrdup(mcc_modtext) ; return T_INSTANCE ;}
^{wsp}*[lL]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[kK]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[Tt]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[vV]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[iI]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[gG]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[eE]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[fF]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
^{wsp}*[hH]{num}*{string}* {ECHO ; return T_ELEMENT                     ;}
"="                        {ECHO ; return '='                           ;} 
{nmos}                     {ECHO ; return T_NMOS                        ;}
{pmos}                     {ECHO ; return T_PMOS                        ;}
{value}                    {ECHO ; return T_VALUE                       ;}
\$                         {ECHO ; BEGIN ENV                            ;}

<UNIT>{giga}               {ECHO ; BEGIN INITIAL ; return T_GIGA        ;}
<UNIT>{kilo}               {ECHO ; BEGIN INITIAL ; return T_KILO        ;}
<UNIT>{mili}               {ECHO ; BEGIN INITIAL ; return T_MILI        ;}
<UNIT>{micro}              {ECHO ; BEGIN INITIAL ; return T_MICRO       ;}
<UNIT>{nano}               {ECHO ; BEGIN INITIAL ; return T_NANO        ;}
<UNIT>{pico}               {ECHO ; BEGIN INITIAL ; return T_PICO        ;}
<UNIT>{femto}              {ECHO ; BEGIN INITIAL ; return T_FEMTO       ;}
<UNIT>{wsp}*               {ECHO ; BEGIN INITIAL                        ;}
<UNIT>^{wsp}*\+            {ECHO ; BEGIN INITIAL                        ;}
<UNIT>\n                   {ECHO ; MCC_LINE++ ;BEGIN INITIAL            ;}

<ENV>{string}              {const char *env;
                            ECHO ; BEGIN INITIAL ;
                            if ((env = avt_gethashvar(mcc_modtext)) != NULL) {
                                 mcc_modlval.text = mbkstrdup(env) ;
                                 return (T_STRING) ; 
                            }
                            else {
                                 fprintf(stderr,"\nMCC ERROR : env %s is not set !!!\n",mcc_modtext);
                                 mcc_moderror();
                            }
                           }       
[{'`]               {ECHO ; mcc_str = NULL ; BEGIN EQU     ;}
<EQU>[^}'`\$\n]*      {ECHO ;
                     if (!mcc_str) {
                         mcc_str = mbkstrdup (mcc_modtext) ;
                     } 
                     else {
                        char * pt ;
                        mcc_buf = (char*)mbkalloc (1024 * sizeof (char)) ;
                        mcc_strp = mbkstrdup (mcc_modtext) ;
                        pt = strchr(mcc_strp,'+') ;
                        *pt = ' ' ;
                        sprintf (mcc_buf, "%s%s", mcc_str, mcc_strp) ;
                        mbkfree (mcc_strp) ;
                        mbkfree (mcc_str) ;
                        mcc_str = mcc_buf ;
                     }
                    }
<EQU>\n             {ECHO ; MCC_LINE++                    ; }
<EQU>[}'`]          {ECHO ; 
                     BEGIN INITIAL ;
                     mcc_modlval.text = mcc_str ;
                     return T_EQUATION ;
                    }
<EQU>\$             {ECHO ; BEGIN ENV2;}
<ENV2>[^'`\n]*      {const char *env;
                     ECHO ;
                     if ((env = avt_gethashvar(mcc_modtext)) != NULL ) {
                          mcc_libenv = mbkstrdup(env) ;
                     }
                     else {
                          fprintf(stderr,"\nMCC ERROR : env %s is not set !!!\n",mcc_modtext);
                          mcc_moderror();
                     }
                    }
<ENV2>['`]          {ECHO ; 
                     BEGIN INITIAL ;
                     mcc_modlval.text = mcc_libenv;
                     return T_STRING; }
                     

<<EOF>>             {ECHO ; if(popcontext() == 1) yyterminate() ;}
{number}            {ECHO ; mcc_modlval.real = strtod (mcc_modtext, (char**)NULL) ; BEGIN UNIT ; return (T_NUMBER)  ; }
{string}            {ECHO ; mcc_modlval.text = mbkstrdup(mcc_modtext) ; return (T_STRING) ; }
{function}          {ECHO ; mcc_modlval.text = mbkstrdup(mcc_modtext) ; return (T_STRING) ; }


%%

int mcc_modwrap () { return 1 ; }

int popcontext()
{
 mcc_contextlist *pt ;
 
 if(MCCCONTEXT == NULL)
   return 1 ;

 pt = MCCCONTEXT ;
 MCCCONTEXT = MCCCONTEXT->next ;
 MCC_LINE = pt->line ;
 FILENAME = pt->filename ;
 LIBTYPE = pt->libtype ;
 CURLIBTYPE = pt->curlibtype ;
 mcc_modin = pt->file ;
 mcc_mod_delete_buffer(YY_CURRENT_BUFFER) ;
 mcc_mod_switch_to_buffer(pt->buffer) ;
 mbkfree(pt) ;

 return 0 ;
}

void pushcontext(filename)
char *filename ;
{
 mcc_contextlist *pt ;
 pt = (mcc_contextlist *)mbkalloc(sizeof(mcc_contextlist)) ;
 pt->next = MCCCONTEXT ;
 MCCCONTEXT = pt ;
 pt->line = MCC_LINE ;
 pt->filename = FILENAME ;
 pt->libtype = LIBTYPE ;
 pt->curlibtype = CURLIBTYPE ;
 pt->buffer = YY_CURRENT_BUFFER ;
 pt->file = mcc_modin ;
 MCC_LINE = 1 ;
 CURLIBTYPE = NULL ;
 FILENAME = namealloc(filename) ;

 mcc_modin = mbkfopen (filename, NULL,READ_TEXT) ;
 if (!mcc_modin) {
      fprintf(stderr, "\nMCC ERROR: can't open file %s to parse!!!\n", filename) ;
      EXIT(1) ;
 }
 mcc_mod_switch_to_buffer(mcc_mod_create_buffer(mcc_modin, YY_BUF_SIZE)) ;

}

extern int mcc_moddebug ;

void yyinit (filename)
char *filename ;
{
 /*mcc_moddebug = 1 ;*/
 MCCCONTEXT = NULL ;
 LIBTYPE = NULL ;
 CURLIBTYPE = NULL ;
 MCC_LINE = 1 ;
 FILENAME = namealloc(filename) ;
}
