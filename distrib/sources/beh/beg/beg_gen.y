/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BEG_GEN Version 1.00                                        */
/*    Fichier : beg_gen.y                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/


%{
#include MUT_H
#include LOG_H
#include EQT_H
#include <stdlib.h>
#include "beg_gen.h"


chain_list  *BEG_GEN_RES;
char        *BEG_GEN_STR;
char         BEG_BUF[128];
extern char *beg_gentext;

/****************************************************************************/
/*{{{                    Function declarations                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

int                  yyerror();
int                  yylex();
static chain_list   *beg_doAtomAndFreeStr(char *name);
static int           beg_doIntAndFreeStr(char *number);

%}

%union
{
  char          *text;
  struct chain  *abl;
  int            num;
  char           car;
};

/*}}}************************************************************************/
/*{{{                    Tokens                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

%token <text> _NUMBER
%token        _EQUAL
%token        _LEFTPAR
%token        _RIGHPAR
%token <text> _ZERO
%token <text> _HZ
%token <text> _CONFLICT
%token <text> _UNDEF
%token        _REP
%token        _CAT
%token        BEG_AND
%token        _NAND
%token        _OR
%token        _NOR
%token        _XOR
%token        _NXOR
%token        _NOT
%token <text> _ONE
%token        _QUOTE
%token        _EVENT
%token        _STABLE
%token        _COLON
%token <text> _BITSTR
%token <text> _NAME
%token        _TO
%token        _DOWNTO
%token        _PLUS
%token        _MINUS
%token        _DIV
%token        _MULT

%left         _PLUS
%left         _MINUS
%left         _DIV
%left         _MULT
%left         _REP
%left         _CAT
%left         _QUOTE
%left         BEG_AND
%left         _NAND
%left         _OR
%left         _NOR
%left         _XOR
%left         _NXOR
%left         _NOT
%left         _OPERATOR

%type <car>  num_operator
%type <text> name
%type <num>  number
%type <num>  bin_operator
%type <num>  stable_delay
%type <text> direction
%type <text> simple_name
%type <text> slice_name
%type <text> operation
%type <text> variable_name
%type <text> alphanum

%type <abl>  attribute_expression
%type <abl>  primary
%type <abl>  atribute_atom
%type <abl>  all_atom
%type <abl>  equality

%type <abl>  bitstr

%type <abl>  concatenation
%type <abl>  replication
%type <abl>  expression
%type <abl>  all

%start all


/*}}}************************************************************************/
/*{{{                    Rules                                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

%%

all :
//                          {yydebug=0;}
    expression            {BEG_GEN_RES = $1;}
  ;

// le preambule

bin_operator :
    _OR                   {$$ = OR; }
  | BEG_AND                  {$$ = AND;}
  | _XOR                  {$$ = XOR;}
  | _NOR                  {$$ = NOR; }
  | _NAND                 {$$ = NAND;}
  | _NXOR                 {$$ = NXOR;}
  ;

number :
    _NUMBER               {$$ = beg_doIntAndFreeStr($1);}
  ;

name :
    _NAME                 {$$ = $1;}
  ;
direction :
    _TO                   {;}
  | _DOWNTO               {;}
  | _COLON                {;}
  ;

bitstr :
    _BITSTR               {$$ = createBitStr($1);}
  ;

slice_name :
    name
    _LEFTPAR
    number
    direction
    number
    _RIGHPAR
    {
      sprintf(BEG_BUF,"%s %d:%d",$1,$3,$5);
      mbkfree($1);
      $$ = mbkstrdup(BEG_BUF);
    }
  ;

alphanum :
    _NAME                 {$$ = $1;}
  | _NUMBER               {$$ = $1;}
  | _LEFTPAR
    operation
    _RIGHPAR
    {
      sprintf(BEG_BUF,"(%s)",$2);
      $$ = mbkstrdup(BEG_BUF);
    }
  ;

num_operator :
    _PLUS                 {$$ = '+';}
  | _MINUS                {$$ = '-';}
  | _MULT                 {$$ = '*';}
  | _DIV                  {$$ = '+';}
  ;

operation :
    alphanum              {$$ = $1;}
  | alphanum
    num_operator
    operation
    {
      sprintf(BEG_BUF,"%s%c%s",$1,$2,$3);
      $$ = mbkstrdup(BEG_BUF);
    }
  ;

variable_name :
    name
    _LEFTPAR
    operation
    _RIGHPAR
    {
      char *tmp;

      tmp   = eqt_getSimpleEquation($3);
      sprintf(BEG_BUF,"%s %s",$1,tmp);
      mbkfree($1);
      mbkfree($3);
      mbkfree(tmp);
      $$    = mbkstrdup(BEG_BUF);
    }
  ;

simple_name :
    name                  {$$ = $1;}
  | variable_name         {$$ = $1;}
  ;

all_atom :
    simple_name           {$$ = beg_doAtomAndFreeStr($1);}
  | slice_name            {$$ = beg_doAtomAndFreeStr($1);}
  ;

equality :
    all_atom
    _EQUAL
    bitstr                {$$ = beg_genEquCnd($1,$3);}
    ;

primary :
    bitstr                {$$ = $1;}
  | concatenation         {$$ = $1;}
  | replication           {$$ = $1;}
  | all_atom              {$$ = $1;}
  | equality              {$$ = $1;}
  | _ZERO                 {$$ = createAtom(namealloc("'0'"));}
  | _ONE                  {$$ = createAtom(namealloc("'1'"));}
  | _HZ                   {$$ = createAtom(namealloc("'z'"));}
  | _UNDEF                {$$ = createAtom(namealloc("'u'"));}
  | _CONFLICT             {$$ = createAtom(namealloc("'x'"));}
  | attribute_expression  {$$ = $1;}
  | _NOT
    primary               {$$ = notExpr($2);}
  | _LEFTPAR
    expression
    _RIGHPAR              {$$ = $2;}
  ;

stable_delay :
    _STABLE               {$$ = 0;}
  | stable_delay
    _LEFTPAR
    number
    _RIGHPAR              {$$ = $3;}
  ;

event :
    _EVENT                {;}
  ;

atribute_atom :
    simple_name
    _QUOTE                {$$ = beg_doAtomAndFreeStr($1);}
  ;

attribute_expression :
    atribute_atom
    event                 {$$ = notExpr(beg_genStable($1,0));}
  | atribute_atom
    stable_delay          {$$ = beg_genStable($1,$2);}
  ;

concatenation :
    primary
    _CAT
    primary               {$$ = concatAbl($1,$3);}
  ;

replication :
    _REP
    number
    primary               {$$ = replicateAbl($3,$2);}
  ;

expression :
    primary               {$$ = $1;}
  | primary
    bin_operator
    expression            {$$ = createBinExpr($2,$1,$3);}
  ;

%%


/*}}}************************************************************************/
/*{{{                    Functions                                          */
/****************************************************************************/
/*{{{                    yyerror()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int yyerror()
{
  beg_initError();
  beg_error(BEG_ERR_PARSER,"%s at %s",BEG_GEN_STR,beg_gentext);
  return EXIT_FAILURE;
}

/*}}}************************************************************************/
/*{{{                    beg_genStable()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *beg_genStable(chain_list *atm,int attrib)
{
  chain_list    *abl;
  int            way     = 0;

  if (!ATOM(atm) && OPER(atm) == NOT)
  {
    atm         = notExpr(atm);
    way         = 1;
  }
  abl           = createExpr(STABLE);
  abl->NEXT     = addchain(NULL,atm);
  if (attrib)
    ATTRIB(abl) = addchain(addchain(NULL,(void*)(long)attrib),NULL);
  if (way)
    abl         = notExpr(abl);
  return abl;
}

/*}}}************************************************************************/
/*{{{                    beg_genEquCnd()                                    */
/*                                                                          */
/* generate an abl from an equality                                         */
/****************************************************************************/
chain_list *beg_genEquCnd(chain_list *abl, chain_list *bitstr)
{
  char           bitStr[255];
  chain_list    *res    = NULL;
  chain_list    *tmp;
  int            i;

  sprintf(bitStr,"%s",(char*)bitstr->DATA);
  freeExpr(bitstr);

  for (i = 1; bitStr[i] != '"' && bitStr[i] != '\''; i ++)
  {
    tmp     = getAblAtPos(abl,i-1);
    if (bitStr[i] == '0')
      tmp   = notExpr(tmp);
    res     = (res) ? concatAbl(res,tmp) : tmp ;
  }
  freeExpr(abl);

  return res;
}

/*}}}************************************************************************/
/*{{{                    beg_genStr2Abl()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *beg_genStr2Abl(char *str)
{
  BEG_GEN_RES = NULL;
  BEG_GEN_STR = str;
  yyparse();
  return BEG_GEN_RES;
}

/*}}}************************************************************************/
/*{{{                    beg_doAtomAndFreeStr()                             */
/*                                                                          */
/* create an atom and free the allocated name                               */
/*                                                                          */
/****************************************************************************/
static chain_list *beg_doAtomAndFreeStr(char *name)
{
  char      *tmp;

  tmp   = namealloc(name);
  mbkfree(name);
  return createAtom(tmp);
}

/*}}}************************************************************************/
/*{{{                    beg_doIntAndFreeStr()                             */
/*                                                                          */
/* create an atom and free the allocated name                               */
/*                                                                          */
/****************************************************************************/
static int beg_doIntAndFreeStr(char *number)
{
  int        tmp;

  tmp   = atoi(number);
  mbkfree(number);
  return tmp;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
