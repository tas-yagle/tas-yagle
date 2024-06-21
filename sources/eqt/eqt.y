/**************************/
/*     eqt.y              */
/**************************/

%{
#include AVT_H
#include EQT_H
#include MUT_H
#include "eqt_util.h"

/******************************************************************************/
/* private                                                                    */
/******************************************************************************/

extern eqt_ctx *EQT_CTX;
typedef struct { struct eqt_node *NODE ; double VALUE ; } noeud_val_type;

/******************************************************************************/
/* function declarations                                                      */
/******************************************************************************/

int yyerror();
int yylex();

%}

%union {
  char                                         *text ;
  double                                        real ;
  struct { struct eqt_node *NODE ; double VALUE ; }    noeud_val ;
  struct eqt_node                              *noeud;  
  struct chain *chainl;
} ;

%token <text> T_NUMBER
%token <text> T_VAR
%token <text> T_USER
%token T_TLFONE T_TLFZERO T_NOTEQ

%left '?' ':'
%left T_OR T_XOR 
%left T_AND
%left T_INFEQ T_INF T_SUPEQ T_SUP T_EQ T_NOTEQ
%left T_NOT
%left '-' '+'
%left '*' '/'
%right T_QUOTE
%left NEG
%right T_SQRT T_EXP T_LOG T_USER 
%right T_SIN T_COS T_TAN T_ASIN T_ACOS T_ATAN
%right T_ATAN2 T_SINH T_COSH T_TANH T_LOG10 
%right T_CEIL T_FLOOR T_FABS T_ABS T_NOMMOD T_NOMDEV T_NOMSUB
%left '^' T_POW

%type <noeud_val> equ
%type <real>      number
%type <chainl>     paramlist
%start choix_resolv

%%


choix_resolv        : equ
                    {  
                      switch (EQT_CTX->EQT_MODE_CHOICE) {
                             case EQTNORMAL : EQT_CTX->EQUA_NODE = $1.NODE ; 
                                              break ;
                             case EQTFAST   : EQT_CTX->EQUA_VAL = $1.VALUE ;
                                              break ;
                             }
                    }
                    | '\n'
                    ;

paramlist           : paramlist ',' equ
                    {
                      noeud_val_type *nvt=(noeud_val_type *)mbkalloc(sizeof(noeud_val_type));
                      memcpy(nvt, &$3, sizeof(noeud_val_type));
                      $$=append($1, addchain(NULL, nvt));
                    }
                    |
                    equ                    
                    {
                      noeud_val_type *nvt=(noeud_val_type *)mbkalloc(sizeof(noeud_val_type));
                      memcpy(nvt, &$1, sizeof(noeud_val_type));
                      $$=addchain(NULL, nvt);
                    }
                    ;

equ                 : number            
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addvalue ($1) ;
                      else   $$.VALUE = $1 ;
                    }
                    | T_VAR               
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addvarname ($1) ;
                      else   $$.VALUE = eqt_getvar (EQT_CTX, $1) ;
                    }
                    | T_TLFONE            
                    { 
                      $$.NODE = eqt_addvalue (1) ;
                    }
                    | T_TLFZERO           
                    { 
                      $$.NODE = eqt_addvalue (0) ;
                    }
                    /* Arithmetic operation */
                    | equ '+' equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addbinarynode (EQTPLUS,$1.NODE,$3.NODE ) ;
                      else   $$.VALUE = $1.VALUE + $3.VALUE ;    
                    }
                    | equ '-' equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addbinarynode (EQTMIN ,$1.NODE,$3.NODE) ;
                      else   $$.VALUE = $1.VALUE - $3.VALUE ;      
                    }
                    | equ '*' equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addbinarynode (EQTMULT,$1.NODE,$3.NODE) ;
                      else   $$.VALUE = $1.VALUE * $3.VALUE ;         
                    }
                    | equ '/' equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addbinarynode (EQTDIV ,$1.NODE,$3.NODE) ;
                      else   $$.VALUE = $1.VALUE / $3.VALUE ; 
                    }
                    | '-' equ %prec NEG  
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTMIN ,$2.NODE ) ;
                      else   $$.VALUE = -$2.VALUE ; 
                    }          
                    | '+' equ %prec NEG  
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = $2.NODE;
                      else   $$.VALUE = $2.VALUE ; 
                    }          
                    | T_SQRT equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTSQRT,$2.NODE) ;
                      else   $$.VALUE = sqrt($2.VALUE) ; 
                    }
                    | T_EXP  equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTEXP ,$2.NODE) ;
                      else   $$.VALUE = exp($2.VALUE) ; 
                    }
                    | T_LOG  equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTLOG ,$2.NODE) ;
                      else   $$.VALUE = log($2.VALUE) ; 
                    }
                    | T_VAR '(' paramlist ')'
                    {
                      int index=eqt_getindex(EQT_CTX, $1);
                      chain_list *cl;
                      if (index==-1)
                      {
                        if (V_BOOL_TAB[__SPI_REPLACE_V_IN_EXPR].VALUE && strcmp($1,"v")==0)
                        {
                          if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addvalue (0/*V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE*/) ;
                          else
                             $$.VALUE = 0;/*V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE */;
                        }
                        else
                        if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                          { 
                            chain_list *ch, *lst;
                            void *old;
                            lst=dupchainlst($3);
                            for (ch=lst;  ch!=NULL; ch=ch->NEXT)
                              ch->DATA=((noeud_val_type*)ch->DATA)->NODE;
                            
                            $$.NODE = eqt_adddotfuncnode($1, lst);

                            freechain(lst);
                          }
                        else
                          {
                            double tab[64];
                            int i;
                            chain_list *ch, *lst=NULL;
                            for (i=0, ch=$3;  ch!=NULL; ch=ch->NEXT, i++)
                              lst=addchain(lst, &((noeud_val_type *)ch->DATA)->VALUE);
                            lst=reverse(lst);
                            
                            $$.VALUE = eqt_execdotfunc(EQT_CTX, $1, lst, 1);
                            freechain(lst);
                          }
                      }
                      else {
                        int nb=countchain($3);
                        if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                          {                            
                            switch(nb)
                            {
                              case 3:
                                 $$.NODE = eqt_addternarynode  (EQTUSER | (index << 8) , ((noeud_val_type *)$3->DATA)->NODE, ((noeud_val_type *)$3->NEXT->DATA)->NODE, ((noeud_val_type *)$3->NEXT->NEXT->DATA)->NODE);
                                 break;
                              case 2:
                                 $$.NODE = eqt_addbinarynode  (EQTUSER | (index << 8) , ((noeud_val_type *)$3->DATA)->NODE, ((noeud_val_type *)$3->NEXT->DATA)->NODE) ;
                                 break;
                              case 1:
                                 $$.NODE = eqt_addunarynode  (EQTUSER | (index << 8) , ((noeud_val_type *)$3->DATA)->NODE) ;
                                 break;
                              default:
                                 YYABORT;
                            }
                          }
                        else {
                          switch(nb)
                            {
                              case 3:
                                 if (EQT_CTX->FUNCTAB3[index]==NULL)
                                 {
                                   avt_errmsg(EQT_ERRMSG, "009", AVT_ERROR, EQT_CTX->NAMETAB[index]);
                                   YYABORT;
                                 }
                                 $$.VALUE = EQT_CTX->FUNCTAB3[index](((noeud_val_type *)$3->DATA)->VALUE, ((noeud_val_type*)$3->NEXT->DATA)->VALUE, ((noeud_val_type*)$3->NEXT->NEXT->DATA)->VALUE) ; 
                                 break;
                              case 2:
                                 if (EQT_CTX->FUNCTAB2[index]==NULL)
                                 {
                                   avt_errmsg(EQT_ERRMSG, "009", AVT_ERROR, EQT_CTX->NAMETAB[index]);
                                   YYABORT;
                                 }
                                 $$.VALUE = EQT_CTX->FUNCTAB2[index](((noeud_val_type *)$3->DATA)->VALUE, ((noeud_val_type *)$3->NEXT->DATA)->VALUE);
                                 break;
                              case 1:
                                 if (EQT_CTX->FUNCTAB[index]==NULL)
                                 {
                                   avt_errmsg(EQT_ERRMSG, "009", AVT_ERROR, EQT_CTX->NAMETAB[index]);
                                   YYABORT;
                                 }
                                 $$.VALUE = EQT_CTX->FUNCTAB[index](((noeud_val_type *)$3->DATA)->VALUE);
                                 break;
                              default:
                                 YYABORT;
                            }   
                        }
                      }
                      for (cl=$3; cl!=NULL; cl=cl->NEXT) mbkfree(cl->DATA);
                      freechain($3);
                    }
                    | T_SIN  equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTSIN ,$2.NODE) ;
                      else   $$.VALUE = sin($2.VALUE) ; 
                    }
                    | T_COS  equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTCOS ,$2.NODE) ;
                      else   $$.VALUE = cos($2.VALUE) ; 
                    }
                    | T_TAN  equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTTAN ,$2.NODE) ;
                      else   $$.VALUE = tan($2.VALUE) ; 
                    }
                    | T_ASIN  equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTASIN ,$2.NODE) ;
                      else   $$.VALUE = asin($2.VALUE) ; 
                    }
                    | T_ACOS  equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTACOS ,$2.NODE) ;
                      else   $$.VALUE = acos($2.VALUE) ; 
                    }
                    | T_ATAN  equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTATAN ,$2.NODE) ;
                      else   $$.VALUE = atan($2.VALUE) ; 
                    }
                    | T_ATAN2 '(' equ ',' equ ')'         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addbinarynode (EQTATAN2,$3.NODE,$5.NODE) ;
                      else   $$.VALUE = atan2($3.VALUE , $5.VALUE) ;         
                    }
                    | T_SINH equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTSINH ,$2.NODE) ;
                      else   $$.VALUE = sinh($2.VALUE) ; 
                    }
                    | T_COSH equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTCOSH ,$2.NODE) ;
                      else   $$.VALUE = cosh($2.VALUE) ; 
                    }
                    | T_TANH equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTTANH ,$2.NODE) ;
                      else   $$.VALUE = tanh($2.VALUE) ; 
                    }
                    | T_LOG10  equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTLOG10 ,$2.NODE) ;
                      else   $$.VALUE = log10($2.VALUE) ; 
                    }
                    | T_CEIL equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTCEIL ,$2.NODE) ;
                      else   $$.VALUE = ceil($2.VALUE) ; 
                    }
                    | T_FLOOR equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTFLOOR ,$2.NODE) ;
                      else   $$.VALUE = floor($2.VALUE) ; 
                    }
                    | T_FABS equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTFABS ,$2.NODE) ;
                      else   $$.VALUE = fabs($2.VALUE) ; 
                    }
                    | T_ABS equ         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode  (EQTABS ,$2.NODE) ;
                      else   $$.VALUE = fabs($2.VALUE) ; 
                    }
                    | equ '^' equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addbinarynode (EQTPOW ,$1.NODE,$3.NODE) ;
                      else   $$.VALUE = pow($1.VALUE, $3.VALUE) ; 
                    }
                    | T_POW '(' equ ',' equ ')'         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addbinarynode (EQTPOW ,$3.NODE,$5.NODE) ;
                      else   $$.VALUE = pow($3.VALUE, $5.VALUE) ; 
                    }
                    | T_NOMMOD '('  T_VAR ')'         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode (EQTNOMMOD ,eqt_addvarname ($3)) ;
                      else {
                       if (EQT_CTX->SPECIAL_FUNC[EQTSPECFUNC_NOM_MOD]!=NULL) 
                         $$.VALUE = EQT_CTX->SPECIAL_FUNC[EQTSPECFUNC_NOM_MOD](namealloc($3)) ; 
                       else
                         yyerror();
                      }
                    }
                    | T_NOMDEV '('  T_VAR ')'         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode (EQTNOMDEV ,eqt_addvarname ($3)) ;
                      else {
                       if (EQT_CTX->SPECIAL_FUNC[EQTSPECFUNC_NOM_DEV]!=NULL) 
                         $$.VALUE = EQT_CTX->SPECIAL_FUNC[EQTSPECFUNC_NOM_DEV](namealloc($3)) ; 
                       else
                         yyerror();
                      }
                    }
                     | T_NOMSUB '('  T_VAR ')'         
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = eqt_addunarynode (EQTNOMSUB ,eqt_addvarname ($3)) ;
                      else {
                       if (EQT_CTX->SPECIAL_FUNC[EQTSPECFUNC_NOM_SUB]!=NULL) 
                         $$.VALUE = EQT_CTX->SPECIAL_FUNC[EQTSPECFUNC_NOM_SUB](namealloc($3)) ; 
                       else
                         yyerror();
                      }
                    }
                    /* Logical operation */
                    | equ T_NOTEQ equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addbinarynode (EQTNOTEQ, $1.NODE, $3.NODE) ;
                      else 
                        $$.VALUE = mbk_cmpdouble($1.VALUE, $3.VALUE, EQT_PRECISION)!=0;
//                        $$.VALUE = $1.VALUE != $3.VALUE;
                    }
                    | equ T_EQ equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addbinarynode (EQTEQ, $1.NODE, $3.NODE) ;
                      else 
                        $$.VALUE = mbk_cmpdouble($1.VALUE, $3.VALUE, EQT_PRECISION)==0;
//                        $$.VALUE = $1.VALUE == $3.VALUE;
                    }
                    | equ T_SUP equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addbinarynode (EQTSUP, $1.NODE, $3.NODE) ;
                      else 
                        $$.VALUE = mbk_cmpdouble($1.VALUE, $3.VALUE, EQT_PRECISION)>0;
//                        $$.VALUE = $1.VALUE > $3.VALUE;
                    }
                    | equ T_SUPEQ equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addbinarynode (EQTSUPEQ, $1.NODE, $3.NODE) ;
                      else 
                        $$.VALUE = mbk_cmpdouble($1.VALUE, $3.VALUE, EQT_PRECISION)>=0;
//                        $$.VALUE = $1.VALUE >= $3.VALUE;
                    }
                    | equ T_INF equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addbinarynode (EQTINF, $1.NODE, $3.NODE) ;
                      else 
                        $$.VALUE = mbk_cmpdouble($1.VALUE, $3.VALUE, EQT_PRECISION)<0;
//                        $$.VALUE = $1.VALUE < $3.VALUE;
                    }
                    | equ T_INFEQ equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addbinarynode (EQTINFEQ, $1.NODE, $3.NODE) ;
                      else 
                        $$.VALUE = mbk_cmpdouble($1.VALUE, $3.VALUE, EQT_PRECISION)<=0;
//                        $$.VALUE = $1.VALUE <= $3.VALUE;
                    }
                    | equ T_AND equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addbinarynode (EQTAND, $1.NODE, $3.NODE) ;
                      else 
                        $$.VALUE = (double)((int)($1.VALUE + 0.5) && (int)($3.VALUE + 0.5));
                    }
                    | equ T_OR equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addbinarynode (EQTOR,$1.NODE, $3.NODE) ;
                      else 
                        $$.VALUE = (double)((int)($1.VALUE + 0.5) || (int)($3.VALUE + 0.5));
                    }
                    | equ T_XOR equ       
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addbinarynode (EQTXOR, $1.NODE, $3.NODE) ;
                      else 
                        $$.VALUE = (double)((int)($1.VALUE + 0.5) ^ (int)($3.VALUE + 0.5));
                    }
                    | equ T_QUOTE %prec NEG
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addunarynode  (EQTNOT, $1.NODE) ;
                      else 
                        $$.VALUE = (double)!((int)($1.VALUE + 0.5));
                    }
                    | T_NOT equ %prec NEG  
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                        $$.NODE = eqt_addunarynode  (EQTNOT, $2.NODE) ;
                      else 
                        $$.VALUE = (double)!((int)($2.VALUE + 0.5));
                    }
                    | '(' equ ')'        
                    { 
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                             $$.NODE = $2.NODE;
                      else   $$.VALUE = $2.VALUE ; 
                    }
                    | equ '?' equ ':' equ
                    {
                      if (EQT_CTX->EQT_MODE_CHOICE == EQTNORMAL)
                      {
                        $$.NODE = eqt_addternarynode  (EQTIF, $1.NODE, $3.NODE, $5.NODE) ;
                      }
                      else
                      { 
                        if (mbk_long_round($1.VALUE)!=0) $$.VALUE=$3.VALUE;
                        else $$.VALUE=$5.VALUE;
                      }
                    }
                    ;

number              : T_NUMBER
                    {
                        char *unit;
                        double val = strtod ($1, &unit);
                        if (*unit == 0)
                            $$ = val;
                        else
                            $$ = val * eqt_getunit (EQT_CTX, unit);
                        mbkfree($1);
                    }
                    ;

%%

extern void eqtinit() ;

int yyerror ()
{
    eqtinit() ;
    eqt_freenode(EQT_CTX->EQUA_NODE);
    EQT_CTX->EQUA_NODE = NULL;
    return EXIT_FAILURE ;
}

