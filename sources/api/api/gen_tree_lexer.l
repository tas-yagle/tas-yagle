/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_tree_lexer.lex                                          */
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
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include MUT_H
#include "gen_tree_utils.h"
//#include "gen_model_utils.h"
#include "gen_tree_parser.h"

#define yylval gen_tree_parserlval

int lineno=1;                       /* to count the number of lines parsed */
char *genius_yyin=NULL;
static int start_C=0, count=0, start_VHDL=0, in_compon=0;                 /*to lock start condition */

extern char *model_corresp(char *name);

chain_list *PRAGMA_SPLIT(char *line);
int countret(char *str);
static char *known_pragmas[]={"symmetric","coupled","exclude","without","unused","exclude_at_end","stop_at_power_supplies","forcematch"};

#define YY_NO_UNPUT
/*
#define YY_INPUT(buf,res,taille_max) \
{ \
if (yyin!=NULL) { int c = fgetc(yyin); res = (c == EOF) ? YY_NULL : (buf[0] = c, 1);  } \
else { if (*genius_yyin=='\0') res=YY_NULL; else {buf[0] = *genius_yyin; genius_yyin++; res=1; } } \
}
*/
%}
num ([0-9]+)
hexa (0[xX](([1-9a-fA-F][0-9a-fA-F]*)|0))
letter ('.')
word_vhdl ([a-zA-Z](_?[a-zA-Z0-9])*)
word_c ([a-zA-Z_][a-zA-Z0-9_]*)

%x C
%x VHDL
%x COMMENT_C
%x STRING

%%
<INITIAL,VHDL,C>[ \t]							{}
<INITIAL,VHDL,C,COMMENT_C>\n		    	   {lineno++;}
<VHDL>\"									{BEGIN STRING; start_C=0;}
<C,STRING>\"\"								{yylval.string=(char*)mbkalloc(1);
                                     yylval.string[0]='\0';
                                     return GEN_TOKEN_STRING;}
<C>\"					{BEGIN STRING; start_C=1;}					
<STRING>\"                          {if (start_C) BEGIN C;
                                     else BEGIN INITIAL;}
<STRING>\\\"                        {yylval.string=(char*)mbkalloc(2);
                                     yylval.string[0]='"';
                                     yylval.string[1]='\0';
                                     return GEN_TOKEN_STRING;}
<STRING>\\n                         {yylval.string=(char*)mbkalloc(2);
                                     yylval.string[0]='\n';
                                     yylval.string[1]='\0';
                                     return GEN_TOKEN_STRING;}
<STRING>\\r                         {yylval.string=(char*)mbkalloc(2);
                                     yylval.string[0]='\r';
                                     yylval.string[1]='\0';
                                     return GEN_TOKEN_STRING;}
<STRING>\\b                         {yylval.string=(char*)mbkalloc(2);
                                     yylval.string[0]='\b';
                                     yylval.string[1]='\0';
                                     return GEN_TOKEN_STRING;}
<STRING>\\\\                        {yylval.string=(char*)mbkalloc(2);
                                     yylval.string[0]='\\';
                                     yylval.string[1]='\0';
                                     return GEN_TOKEN_STRING;}
<STRING>\\t                         {yylval.string=(char*)mbkalloc(2);
                                     yylval.string[0]='\t';
                                     yylval.string[1]='\0';
                                     return GEN_TOKEN_STRING;}
<STRING>\\.                         {yylval.string=(char*)mbkalloc(3);
                                     strcpy(yylval.string,yytext);
                                     return GEN_TOKEN_STRING;}
<STRING>[^\\\"]*                    {int i,taille=strlen(yytext);
                                     yylval.string=(char*) mbkalloc(taille+1);
                                     strcpy(yylval.string,yytext);
                                     for (i=0;i<taille;i++) 
                                       if (yytext[i]=='\n') lineno++;
                                     return GEN_TOKEN_STRING;}
<VHDL,C,INITIAL>\/\*	     		     		   	   {BEGIN COMMENT_C;}
<COMMENT_C>\*\/	     	   			{if (start_C) BEGIN C; else if (start_VHDL) BEGIN VHDL; else BEGIN INITIAL;}
<COMMENT_C>.	     	   			   {}
<VHDL,C,INITIAL>\/\/[^\n]*                                {}
<VHDL,INITIAL>\-\-.*					{ chain_list *cl;
  cl=PRAGMA_SPLIT(&yytext[2]);
  if (cl!=NULL) { unsigned int i; for (i=0;i<sizeof(known_pragmas)/sizeof(*known_pragmas);i++) if (strcmp(known_pragmas[i],(char *)cl->DATA)==0) { yylval.chain=cl; return GEN_TOKEN_PRAGMA_SYM; } }
  freechain(cl);
 }
<VHDL>[bB][eE][gG][iI][nN]           { return GEN_TOKEN_BEGIN; }
<VHDL>[eE][nN][dD]                   { count--; if (in_compon==1) in_compon=2; return GEN_TOKEN_END;}
<INITIAL>void                        { start_C=2; BEGIN C; return GEN_TOKEN_VOIDTOKEN; }
<C>void                              { return GEN_TOKEN_VOIDTOKEN; }
<INITIAL>[eE][nN][tT][iI][tT][yY]       { count++; start_VHDL=1; BEGIN VHDL; return GEN_TOKEN_ENTITY;}
<INITIAL>[aA][rR][cC][hH][iI][tT][eE][cC][tT][uU][rR][eE]	{ count++; start_VHDL=1; BEGIN VHDL; return GEN_TOKEN_ARCHITECTURE;}
<VHDL>[oO][fF]		       	 	       	   {return GEN_TOKEN_OF;}
<VHDL>[iI][sS]	 			  	       	      {return GEN_TOKEN_IS;}
<VHDL>[cC][oO][mM][pP][oO][nN][eE][nN][tT] 	{ if (in_compon==0) { count++; in_compon=1; } return GEN_TOKEN_COMPONENT;}
<VHDL>[sS][iI][gG][nN][aA][lL]	  		   {return GEN_TOKEN_SIGNAL;}
<VHDL>[vV][aA][rR][iI][aA][bB][lL][eE] 	{return GEN_TOKEN_VARIABLE;}
<VHDL>[iI][nN][tT][eE][gG][eE][rR] 			{return GEN_TOKEN_INTEGER;}
<VHDL>[oO][uU][tT] 		       				{return GEN_TOKEN_OUT;}
<VHDL>[iI][nN][oO][uU][tT] 	    		   {return GEN_TOKEN_INOUT;}
<VHDL>[dD][oO][wW][nN][tT][oO]	  			{return GEN_TOKEN_DOWNTO;}
<VHDL>[tT][oO]	       		       			{return GEN_TOKEN_TO;}
<VHDL>[bB][iI][tT]	       		  		   {return GEN_TOKEN_BIT;}
<VHDL>[bB][iI][tT]_[vV][eE][cC][tT][oO][rR]   {return GEN_TOKEN_BIT_VECTOR;}
<VHDL>[wW][oO][rR]_[bB][iI][tT]	   		{return GEN_TOKEN_WOR_BIT;}
<VHDL>[bB][uU][sS]	       		   		{return GEN_TOKEN_BUS;}
<VHDL>[mM][uU][xX]_[bB][iI][tT]	   		{return GEN_TOKEN_MUX_BIT;}
<VHDL>[wW][oO][rR]_[vV][eE][cC][tT][oO][rR]  	{return GEN_TOKEN_WOR_VECTOR;}
<VHDL>[mM][uU][xX]_[vV][eE][cC][tT][oO][rR]  	{return GEN_TOKEN_MUX_VECTOR;}
<VHDL>[pP][oO][rR][tT]	       	   		{return GEN_TOKEN_PORT;}
<VHDL>[mM][aA][pP]		       	   		{return GEN_TOKEN_MAP;}
<VHDL>[fF][oO][rR]		       	   		{ count++; return GEN_TOKEN_FOR;}
<VHDL>[gG][eE][nN][eE][rR][aA][tT][eE]	 	{return GEN_TOKEN_GENERATE;}
<VHDL>[gG][eE][nN][eE][rR][iI][cC]			{return GEN_TOKEN_GENERIC;}
<VHDL>[iI][nN]		       	       			{return GEN_TOKEN_IN;}
<VHDL>[aA][nN][dD]		       	   		{return GNS_TOKEN_AND;}
<VHDL>[nN][aA][nN][dD]		       			{return GEN_TOKEN_NAND;}
<VHDL>[oO][rR]		       	       			{return GEN_TOKEN_OR;}
<VHDL>[nN][oO][rR]		       	   		{return GEN_TOKEN_NOR;}
<VHDL>[xX][oO][rR]		       	   		{return GEN_TOKEN_XOR;}
<VHDL>[nN][oO][tT]		       	   		{return GEN_TOKEN_NOT;}
<VHDL>[mM][oO][dD]		       	   		{return GEN_TOKEN_MOD;}
<VHDL>[rR][eE][mM]		       	   		{return GEN_TOKEN_REM;}
<VHDL>[aA][bB][sS]		       	   		{return GEN_TOKEN_ABSOL;}
<INITIAL>{word_c}[ \n\t]*[\*]*[ \n\t]*{word_c}[ \n\t]*\( { 
                         yylval.string=sensitive_namealloc(yytext);
                         start_C=1; BEGIN C; 
                         lineno+=countret(yylval.string);
                         return GEN_TOKEN_FONCTION_HEADER;
                         }
<C>exclude	       			       	{return GEN_TOKEN_EXCLUDE;}
<C>printf  			       				{return GEN_TOKEN_PRINTF;}
<C>fprintf  			       			{return GEN_TOKEN_FPRINTF;}
<C>sprintf  			       			{return GEN_TOKEN_SPRINTF;}
<C>exit 			    	       		   {return GEN_TOKEN_EXIT;}
<C>return 			    	       		{return GEN_TOKEN_RETURN;}
<C>break 			    	       		{return GEN_TOKEN_BREAK;}
<C>NULL 			    	       		{return GEN_TOKEN_NULL;}
<C>for 		      	       			{return GEN_TOKEN_FOR;}
<C>do 			    	       		   {return GEN_TOKEN_DO;}
<C>while      	       					{return GEN_TOKEN_WHILE;}
<C>if 			       					{return GEN_TOKEN_IF;}
<C>else 	       							{return GEN_TOKEN_ELSE;}

<INITIAL>int               { start_C=2; BEGIN C; return GEN_TOKEN_INTEGER;}
<INITIAL>long              { start_C=2; BEGIN C; return GEN_TOKEN_LONG;}
<INITIAL>double            { start_C=2; BEGIN C; return GEN_TOKEN_DOUBLE; }
<INITIAL>FILE 	           { start_C=2; BEGIN C; return GEN_TOKEN_FILE;}
<INITIAL>char 	           { start_C=2; BEGIN C; return GEN_TOKEN_CHAR;}
<INITIAL>{word_c}          { 
                             yylval.string=sensitive_namealloc(yytext);
                             
                             start_C=2; BEGIN C;
                             return GEN_TOKEN_IDENT;
                           }
<C>int 	       			       		{/*only lowcase in C */return GEN_TOKEN_INTEGER;}
<C>long 	       			       	{ return GEN_TOKEN_LONG; }
<C>double                                       { return GEN_TOKEN_DOUBLE; }
<C>FILE 	       			       	{return GEN_TOKEN_FILE;}
<C>char 	       			       	{return GEN_TOKEN_CHAR;}
<C>static	       			       	{return GEN_TOKEN_STATIC;}
<C>READ_TEXT      			       	{yylval.string=
                                       (char*) mbkalloc(strlen(READ_TEXT)+1);
                                     strcpy(yylval.string,READ_TEXT);
                                     return GEN_TOKEN_STRING;}
<C>WRITE_TEXT      			       	{yylval.string=
                                       (char*) mbkalloc(strlen(WRITE_TEXT)+1);
                                     strcpy(yylval.string,WRITE_TEXT);
                                     return GEN_TOKEN_STRING;}
\*\*					{return GEN_TOKEN_POW;}
<VHDL,C>[,]             { return ','; }
<VHDL,C>[;]             { if (start_VHDL && count==0) { start_VHDL=0; BEGIN INITIAL; }
                          if (start_C==2) {start_C=0; BEGIN INITIAL; }
                          if (in_compon==2) in_compon=0;
                          return ';';
                        }
<VHDL,C>\(								{return '(';}
<VHDL,C>\)								{return ')';}
<C>\=											{return '=';}
<VHDL>\=								{return GEN_TOKEN_EG;}
<C>\+\+ 										{return GEN_TOKEN_INC;}	
<C>\+\=										{return GEN_TOKEN_EG_ADD;}
<VHDL,C>\+								{return '+';}
<C>--											{return GEN_TOKEN_DEC;}
<C>-\=										{return GEN_TOKEN_EG_SUB;}
<VHDL,C>-								{return '-';}
<C>\*\=										{return GEN_TOKEN_EG_MUL;}
<VHDL,C>\*								{return '*';}
<C>\/\=										{return GEN_TOKEN_EG_DIV;}
<VHDL,C>\/								{return '/';}
<C>%\=										{return GEN_TOKEN_EG_MOD;}
<C>%											{return '%';}
<C>\^\=										{return GEN_TOKEN_EG_XOR;}
<C>\^											{return '^';}
<C>!											{return '!';}
<C>&&											{return GNS_TOKEN_AND;}
<C>\&\=										{return GEN_TOKEN_EG_AND;}
<VHDL,C>&								{return '&';}
<C>\|\|										{return GEN_TOKEN_OR;}
<C>\|\=										{return GEN_TOKEN_EG_OR;}
<C>\|											{return '|';}
<C>~											{return '~';}
<C>\<\<										{return GEN_TOKEN_SHL;}
<VHDL,C>\<								{return '<';}
<C>\>\>										{return GEN_TOKEN_SHR;}
<VHDL,C>\>								{return '>';}
<C>\=\=										{return GEN_TOKEN_EG;}
<C>\<\=										{return GEN_TOKEN_INFEG;}
<C>\>\=										{return GEN_TOKEN_SUPEG;}
<C>!\=										{return GEN_TOKEN_NOTEG;}
<VHDL>\/\=											{return GEN_TOKEN_NOTEG;}
<C>\?											{return '?';}
<VHDL,C>\:								{return ':';}
<C>\[											{return '[';}
<C>\]											{return ']';}
<VHDL>\=\>											{return GEN_TOKEN_EQUI;}
<VHDL,C>{letter}				{yylval.integer=yytext[1]; return GEN_TOKEN_DIGIT_CHAR;}
<VHDL,C>{num}(\.{num})?[eE][+-]?{num} 	{yylval.real=(double *)mbkalloc(sizeof(double)); *yylval.real=strtod(yytext,NULL); return GEN_TOKEN_DIGIT_DOUBLE;}
<VHDL,C>{num}\.{num} 	{yylval.real=(double *)mbkalloc(sizeof(double)); *yylval.real=strtod(yytext,NULL); return GEN_TOKEN_DIGIT_DOUBLE;}
<VHDL,C>{num}					{yylval.integer=atoi(yytext);
      	 	     		          	 	 return GEN_TOKEN_DIGIT;}
<C>{hexa}				{int i,val,taille=strlen(yytext)-1;
                                     yylval.integer=0;
                                     for (i=taille;i>=2/*0x*/;i--) {
                                      if (yytext[i]>='a')
                                          val=yytext[i]-'a'+10;
                                      else if (yytext[i]>='A') 
                                          val=yytext[i]-'A'+10;
                                      else val=yytext[i]-'0';
                                      yylval.integer+=val*pow(16,taille-i);
                                     }
                                     return GEN_TOKEN_DIGIT;}
<VHDL>{word_vhdl}			{yylval.string=namealloc(yytext); return GEN_TOKEN_IDENT;}
<C>{word_c}			{ yylval.string=sensitive_namealloc(yytext); return GEN_TOKEN_IDENT;}
<C>\{                                {count++;return '{';}
<C>\}                                {count--;if (count==0) {BEGIN INITIAL;start_C=0;} return '}';}
<INITIAL,VHDL,C,COMMENT_C,STRING><<EOF>>	{return GEN_TOKEN_STOP;}
<INITIAL,VHDL,C,COMMENT_C,STRING>.			{return *yytext;}
%%

/*****************************************************************************/
/*                stop searching other files to parse ?                      */
/*****************************************************************************/
extern int yywrap() {
       return 1;
}

void init_lexer()
{
  start_C=0, count=0, start_VHDL=0, in_compon=0;
BEGIN INITIAL;
}


char *mysep(char **next, char sep)
{
  char *first=*next;
  while (**next==sep && **next!='\0') (*next)++;
  if (**next=='\0') { first=*next; *next=NULL; return first; }
  first=*next;
  while (**next!=sep && **next!='\0') (*next)++;
  if (**next=='\0') *next=NULL;
  else
    {
      **next='\0';
      (*next)++; 
    }
  return first;
}


chain_list *PRAGMA_SPLIT(char *line)
{
  chain_list *cl=NULL;
  char *token, *next;
  next=line;
//  printf(" '%s'\n",line);
  token=mysep(&next, ' ');
 // printf(" %s\n",token);
  if (next==NULL || strcmp(token, "pragma")!=0) return NULL;
  do 
    {
      token=mysep(&next, ' ');
      if (token[0]!='\0')
	{
	//  printf("  %s\n",token);
	  cl=addchain(cl, model_corresp(token));
	}
    } while (next!=NULL);
  cl=reverse(cl);
  return cl;
}

int countret(char *str)
{
  int i, c=0;
  for (i=0; str[i]!='\0'; i++) if (str[i]=='\n') c++;
  return c;
}
