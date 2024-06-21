%{

#include "tbg.h"

t_port *port;
int right_entity = 0;

int yyerror ();
int yylex ();

chain_list *ch;

%}

%union {
  char                    *t_pchar;
  long                     t_long;
  struct {int B0; int B1;} t_range;
  struct chain            *t_chain;
};

%token            _ENTITY
%token            _INTEGER
%token            _STD_LOGIC
%token            _STD_LOGIC_VECTOR
%token            _IN
%token            _INOUT
%token            _OUT
%token            _PORT
%token            _GENERIC
%token            _DOWNTO
%token            _TO
%token            _END
%token <t_pchar>  _IDENTIFIER
%token            _IS
%token <t_long>   _NUMBER

%left '-' '+' 
%left '*' '/' 
%left NEG    

%type <t_range> range
%type <t_long>  direction
%type <t_long>  bound
%type <t_chain> idlist;

%start entity


%%

entity    : _ENTITY _IDENTIFIER {if (!strcasecmp ($2, DESIGN_NAME)) right_entity = 1;} _IS gen_decl port_decl _END 
          ;

gen_decl  : _GENERIC '(' gen_list gen ')' ';'
          | _GENERIC '('              ')' ';'
          | /* empty */
          ;

gen_list  : gen_list gen ';'
          | gen ';'

gen       : idlist ':' _INTEGER 
          {
          }
          ;

port_decl : _PORT '(' port_list port ')' ';'
          | _PORT '('                ')' ';'
          ;
          
port_list : port_list port ';'
          | port ';'
          ;

port      : idlist ':' direction type range
          {
              if (right_entity) {
                  for  (ch = reverse ($1); ch; ch = ch->NEXT) {
                      if (!HEAD_PORT) {
                          HEAD_PORT = (t_port*)mbkalloc (sizeof (struct t_port));
                          HEAD_PORT->NAME = (char*)ch->DATA;
                          HEAD_PORT->NEXT = NULL;
                          HEAD_PORT->FLAG = 0;
                          HEAD_PORT->VALUE = NULL;
                          HEAD_PORT->DIRECTION = $3;
                          HEAD_PORT->B0 = $5.B0;
                          HEAD_PORT->B1 = $5.B1;
                          port = HEAD_PORT;
                      } else {
                          port->NEXT = (t_port*)mbkalloc (sizeof (struct t_port));
                          port = port->NEXT;
                          port->NEXT = NULL;
                          port->FLAG = 0;
                          port->VALUE = NULL;
                          port->NAME = (char*)ch->DATA;
                          port->DIRECTION = $3;
                          port->B0 = $5.B0;
                          port->B1 = $5.B1;
                      }
                  }
              }
          }
          ;

idlist    : idlist ',' _IDENTIFIER
          {
            $$ = addchain ($$, $3);
          }
          | _IDENTIFIER
          {
            $$ = addchain (NULL, $1);
          }
          ;

direction : _IN
          {
            $$ = T_IN;
          }
          | _INOUT
          {
            $$ = T_INOUT;
          }
          | _OUT
          {
            $$ = T_OUT;
          }
          ;

type      : _STD_LOGIC_VECTOR
          | _STD_LOGIC
          ;

range     : '(' bound _TO bound ')'
          {
            $$.B0 = $2;
            $$.B1 = $4;
          }
          | '(' bound _DOWNTO bound ')'
          {
            $$.B0 = $2;
            $$.B1 = $4;
          }
          | /* empty */
          {
            $$.B0 = -1;
            $$.B1 = -1;
          }
          ;

bound     : _NUMBER            
          { 
            $$ = $1;
          }
          | _IDENTIFIER
          {
            long val = gethtitem (GENERICS_HT, namealloc ($1));
            if (val == EMPTYHT) {
                fprintf (stderr, "Please provide a value for %s\n", $1);
                EXIT (0);
            } else
                $$ = val;
          }
          /* Arithmetic operation */
          | bound '+' bound       
          {
            $$ = $1 + $3;
          }
          | bound '-' bound       
          { 
            $$ = $1 - $3;
          }
          | bound '*' bound       
          { 
            $$ = $1 * $3;
          }
          | bound '/' bound       
          { 
            $$ = $1 / $3;
          }
          | '-' bound %prec NEG  
          { 
            $$ =  - $2;
          }          
          | '(' bound ')'        
          { 
            $$ = $2;
          }
          ;
%%

int yyerror ()
{
    fprintf (stderr, "syntax error line %d\n", LINE);
    return 0;
}

