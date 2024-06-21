/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit : SDF Version 1.00                                              */
/*    Fichier : sdf.y                                                         */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Gilles Augustins                                            */
/*                                                                            */
/******************************************************************************/

%{
#include BCK_H
#include "bck_sdf.h"

/******************************************************************************/
/* defines                                                                    */
/******************************************************************************/

#define PORT_HT_SIZE 128
#define MAX(a,b) (a)>(b)?(a):(b)

/******************************************************************************/
/* functions                                                                  */
/******************************************************************************/

int isextern (char *name, char separ)
{
	return strchr (name, separ) ? 0 : 1 ;
}

/******************************************************************************/

char *unquote (char *qstr)
{
	int  i = 1 ; 
	char str[1024] ;

	while (qstr[i] != '\0')
		str[i - 1] = qstr[i++] ;
	str[i - 2] = '\0' ;

	return namealloc (str) ;
}

/******************************************************************************/

locon_list *insgetlocon (loins_list *ptins, char *name)
{
	locon_list  *ptcon;
	
	if (!ptins)
		return NULL ;

	for (ptcon = ptins->LOCON ; ptcon ; ptcon = ptcon->NEXT)
	   if (ptcon->NAME == name)
	       return ptcon ;

    return NULL ; /* never reached */
}

/******************************************************************************/

char *portname (char *name, char separ)
{
	int i = 0, j = 0 ;
	char buf[1024] ;

	while (name[j] != '\0') {
		buf[i] = name[j] ;
		i = name[j++] == separ ? 0 : i + 1 ; 
	}
	buf[i] = '\0' ;

	return namealloc (buf) ;
}

/******************************************************************************/

char *instname (char *name, char separ)
{
	int i = 0, last = 0 ;
	char buf[1024] ;
	
	
	while (name[i] != '\0') {
		buf[i] = name[i] ;
		if (name[i] == separ) 
			last = i ;
		i++ ;
	}
	buf[i] = '\0' ;

	if (last)
		buf[last] = '\0' ;

	return namealloc (buf) ;
}
																
/******************************************************************************/

loins_list *bck_getloins (char* insname, ht *htins)
{
	long value ;
	
	value = gethtitem (htins, insname) ;

	if (value == EMPTYHT || value == DELETEHT) {
	    fprintf (stderr, " *** bck error : instance %s doesn't exist ***\n", insname) ;
		EXIT (EXIT_FAILURE) ;
	} else 
		return (loins_list*)value ;
}

/******************************************************************************/
/* private                                                                    */
/******************************************************************************/

	char           buf[1024] ;
	char           separ = '.' ;
	long           delaytype ;
	long           transition ;
	char          *celltype ;
	char          *toplevel ;
	char          *path ;
	loins_list    *loins, *ptins ;
	locon_list    *start, *end, *data, *command ;
	chain_list    *loinschain ;
	bck_annot     *annot ;
	bck_delaylist *delays, *rcdelay ;
	bck_checklist *checks ;
	bck_translist *trans, *pttrans ;
	double         sdf_timescale = 1e3 ; /* default is ns */
	int            DEBUG = 0 ;
	ht            *htins ;
	ht            *htport = NULL ;

/******************************************************************************/
/* function declarations                                                      */
/******************************************************************************/

int yyerror();
int yylex();

%}

%union {
  char                                *text ;
  char                                 car ;
  double                               real ;
  long                                 integer ;
  struct chain                        *chainlist ;
  struct bck_trans                    *translist ;
  struct { char *PORT ; long TRANS ; } tport ;
} ;

%token _SYMBOLIC0  "~&"                      
%token _SYMBOLIC1  "~|"                      
%token _SYMBOLIC2  "^~"                      
%token _SYMBOLIC3  "~^"                      
%token _SYMBOLIC4  "=="                      
%token _SYMBOLIC5  "!="                      
%token _SYMBOLIC6  "==="                     
%token _SYMBOLIC7  "!=="                     
%token _SYMBOLIC8  "&&"                      
%token _SYMBOLIC9  "||"                      
%token _SYMBOLIC10 "<="                      
%token _SYMBOLIC11 ">="                      
%token _SYMBOLIC12 ">>"                      
%token _SYMBOLIC13 "<<"                      
%token _DELAYFILE      
%token _SDFVERSION     
%token _DESIGN         
%token _DATE           
%token _VENDOR         
%token _PROGRAM        
%token _VERSION        
%token _DIVIDER        
%token _VOLTAGE        
%token _PROCESS        
%token _TEMPERATURE    
%token _TIMESCALE      
%token _CELL           
%token _CELLTYPE       
%token _INSTANCE       
%token _DELAY          
%token _TIMINGCHECK    
%token _TIMINGENV      
%token _PATHPULSE      
%token _PATHPULSEPERCENT 
%token _ABSOLUTE       
%token _INCREMENT      
%token _IOPATH         
%token _RETAIN         
%token _COND           
%token _CONDELSE       
%token _PORT           
%token _INTERCONNECT   
%token _DEVICE         
%token _SETUP          
%token _HOLD           
%token _SETUPHOLD      
%token _RECOVERY       
%token _REMOVAL        
%token _RECREM         
%token _SKEW           
%token _WIDTH          
%token _PERIOD         
%token _NOCHANGE       
%token _SCOND          
%token _CCOND          
%token _NAME           
%token _EXCEPTION      
%token _PATHCONSTRAINT 
%token _PERIODCONSTRAINT 
%token _SUM            
%token _DIFF           
%token _SKEWCONSTRAINT 
%token _ARRIVAL        
%token _DEPARTURE      
%token _SLACK          
%token _WAVEFORM       
%token _NEGEDGE        
%token _POSEDGE        
%token _US
%token _NS
%token _PS
%token _01             
%token _10             
%token _0Z             
%token _Z1             
%token _1Z             
%token _Z0             
%token _B0             
%token _B1             
%token <real> _NUMBER
%token <text> _IDENTIFIER
%token <text> _QSTRING

%type <text>      INSTANCE_PATH
%type <text>      port_instance
%type <integer>   EDGE_IDENTIFIER
%type <tport>     port_edge
%type <tport>     port_tchk
%type <tport>     port_spec
%type <real>      triple
%type <real>      value 
%type <real>      delval 
%type <real>      number 
%type <translist> delval_list

%start delay_file 

%%

delay_file         : '(' _DELAYFILE sdf_header cell_list ')' 
                   ;
                   
sdf_header         : sdf_version design_name date vendor program_name program_version hierarchy_divider voltage process temperature time_scale 
                   {
				       htins = addht (100000) ;
					   for (ptins = LOFIG->LOINS ; ptins ; ptins = ptins->NEXT) 
					       addhtitem (htins, ptins->INSNAME, (long)ptins) ;
				   }
			       ;
                   
sdf_version        : _SDFVERSION _QSTRING ')' { mbkfree($2); }
			       ;
				   
design_name        : _DESIGN _QSTRING ')'
                   {
				       toplevel = unquote ($2) ;
                       mbkfree($2);
				   }
				   | empty
			       ;
                   
date               : _DATE _QSTRING ')' { mbkfree($2); }
				   | empty
			       ;
                   
vendor             : _VENDOR _QSTRING ')' { mbkfree($2); }
				   | empty
			       ;
			       
program_name       : _PROGRAM _QSTRING ')' { mbkfree($2); }
				   | empty
			       ;
			       
program_version    : _VERSION _QSTRING ')' { mbkfree($2); }
				   | empty
				   ;
				   
hierarchy_divider  : _DIVIDER HCHAR ')'
				   | empty
				   ;
                   
HCHAR              : '.'
                   {
				       separ = '.' ;
				   }
                   | '/'
                   {
				       separ = '/' ;
				   }
				   ;
				   
voltage            : _VOLTAGE triple ')'
                   | _VOLTAGE number ')'
				   | empty
				   ;
				   
process            : _PROCESS _QSTRING ')'  { mbkfree($2); }
				   | empty
				   ;
				   
temperature        : _TEMPERATURE triple ')' 
                   | _TEMPERATURE number ')' 
				   | empty
				   ;
				   
time_scale         : _TIMESCALE TSVALUE ')'
				   | empty
				   ;
                   
TSVALUE            : number _US
                   {
				       sdf_timescale = $1 * 1e6 ;
				   }
                   | number _NS
                   {
				       sdf_timescale = $1 * 1e3 ;
				   }
				   | number _PS
                   {
				       sdf_timescale = $1 ;
				   }
				   ;
                   
cell_list          : '(' cell ')'
                   | cell_list '(' cell ')' 
				   ;
                   
cell               : _CELL '(' celltype ')' '(' cell_instance ')' timing_spec_list 
                   {
				       	if (htport) { 
					   		delht (htport) ;
					   		htport = NULL ;
						}
				       	//bck_view (annot) ;
                        if (loins==NULL) mbkfree(annot);
				   }
                   ;
                   
celltype           : _CELLTYPE _QSTRING  
                   {
				       celltype = unquote ($2) ;
					   delays = NULL ;
					   checks = NULL ;
                       mbkfree($2);
				   }
                   ;
                   
cell_instance      : _INSTANCE INSTANCE_PATH 
                   {
				       annot = bck_addannot () ;
					   
					   if ($2 != toplevel) 
					       loins = bck_getloins ($2, htins) ;
                       else
                           loins=NULL;
					   if (loins) 
					       loins->USER = addptype (loins->USER, PTYPE_BCK_INS, annot) ;
				   }
                   | _INSTANCE '*' 
				   {
					   if (LOFIG) {
						   annot = bck_addannot () ;
				           for (ptins = LOFIG->LOINS ; ptins ; ptins = ptins->NEXT) {
					           if (ptins->FIGNAME == celltype) {
							       ptins->USER = addptype (ptins->USER, PTYPE_BCK_INS, annot) ;
								   loins = ptins ;
							   }
						   }
					   }
				   }
                   | _INSTANCE 
				   {
					   if (LOFIG) {
						   annot = bck_addannot () ;
				           for (ptins = LOFIG->LOINS ; ptins ; ptins = ptins->NEXT) {
					           if (ptins->FIGNAME == celltype) {
							       ptins->USER = addptype (ptins->USER, PTYPE_BCK_INS, annot) ;
								   loins = ptins ;
							   }
						   }
					   }
				   }
                   ;
                   
INSTANCE_PATH      : _IDENTIFIER 
                   {
				       $$ = $1 ;
				   }
				   ;
                   
timing_spec_list   : empty
                   | timing_spec_list timing_spec
				   ;
                   
timing_spec        : '(' del_spec ')'
				   {
				   	   if (delays)
				           annot->DELAYS = delays ;
				   }
                   | '(' tc_spec  ')'
				   {
				       if (checks)
				           annot->CHECKS = checks ;
				   }
                   | '(' te_spec  ')'
				   {
				   }
                   ;
                   
/* delays */       
                   
del_spec           : _DELAY deltype_list 
                   ;
                   
deltype_list       : '(' deltype ')'
                   | deltype_list '(' deltype ')'
				   ;
                   
deltype            : _PATHPULSE input_output_path value
                   | _PATHPULSE input_output_path value value
                   | _PATHPULSE value
                   | _PATHPULSE value value
                   | _PATHPULSEPERCENT input_output_path value
                   | _PATHPULSEPERCENT input_output_path value value
                   | _PATHPULSEPERCENT value
                   | _PATHPULSEPERCENT value value
				   | delaytype del_def_list
				   ;

delaytype          : _ABSOLUTE 
                   {
				       delaytype = ABSOLUTE ;
				   }
				   | _INCREMENT 
                   {
				       delaytype = INCREMENT ;
				   }
				   ;
                   
input_output_path  : port_instance port_instance
                   {
				   }
                   ;
                   
del_def_list       : '(' del_def ')'
                   | del_def_list '(' del_def ')'
				   ;
				   
del_def            : _IOPATH port_spec port_instance retain_list delval_list
                   {
						start = insgetlocon (loins, $2.PORT) ;
						end   = insgetlocon (loins, $3) ;
						if (start && end) {
				   			delays = bck_adddelay (delays, delaytype | IOPATH) ;
							for (pttrans = $5 ; pttrans ; pttrans = pttrans->NEXT) 
						    	pttrans->EVENT1 = $2.TRANS ;
							delays->TRANSLIST   = $5 ;
							delays->START       = start ;
							delays->END         = end ;
						}
				   }
                   | _COND _QSTRING cond_port_expr '(' _IOPATH port_spec port_instance retain_list delval_list ')' 
                   {
				   		start = insgetlocon (loins, $6.PORT) ;
						end   = insgetlocon (loins, $7) ;
						if (start && end) {
				   			delays = bck_adddelay (delays, delaytype | IOPATH) ;
							for (pttrans = $9 ; pttrans ; pttrans = pttrans->NEXT) 
						    	pttrans->EVENT1 = $6.TRANS ;
							delays->TRANSLIST   = $9 ;
							delays->START       = start ;
							delays->END         = end ;
						}
                        mbkfree($2);
				   }
                   | _COND cond_port_expr '(' _IOPATH port_spec port_instance retain_list delval_list ')' 
                   {
						start = insgetlocon (loins, $5.PORT) ;
						end   = insgetlocon (loins, $6) ;
						if (start && end) {
				   			delays = bck_adddelay (delays, delaytype | IOPATH) ;
							for (pttrans = $8 ; pttrans ; pttrans = pttrans->NEXT) 
						    	pttrans->EVENT1 = $5.TRANS ;
							delays->TRANSLIST   = $8 ;
							delays->START       = start ;
							delays->END         = end ;
						}
				   }
                   | _CONDELSE _QSTRING cond_port_expr '(' _IOPATH port_spec port_instance retain_list delval_list ')' 
                   {
				   		start = insgetlocon (loins, $6.PORT) ;
						end   = insgetlocon (loins, $7) ;
						if (start && end) {
				   			delays = bck_adddelay (delays, delaytype | IOPATH) ;
							for (pttrans = $9 ; pttrans ; pttrans = pttrans->NEXT) 
						    	pttrans->EVENT1 = $6.TRANS ;
							delays->TRANSLIST   = $9 ;
							delays->START       = start ;
							delays->END         = end ;
						}
                        mbkfree($2);
				   }
                   | _CONDELSE cond_port_expr '(' _IOPATH port_spec port_instance retain_list delval_list ')'
                   {
						start = insgetlocon (loins, $5.PORT) ;
						end   = insgetlocon (loins, $6) ;
						if (start && end) {
				   			delays = bck_adddelay (delays, delaytype | IOPATH) ;
							for (pttrans = $8 ; pttrans ; pttrans = pttrans->NEXT) 
						    	pttrans->EVENT1 = $5.TRANS ;
							delays->TRANSLIST   = $8 ;
							delays->START       = start ;
							delays->END         = end ;
						}
				   }
				   | _PORT port_instance delval_list
				   {
				   		if (loins) {
							end = insgetlocon (loins, $2) ;
							if (end) {
								ptype_list *ptype ;
								if((ptype = getptype(end->SIG->USER, PTYPE_BCK_RC)) == NULL) {
				   					rcdelay = bck_adddelay (NULL, delaytype | INTERCONNECT) ;
									end->SIG->USER = addptype (end->SIG->USER, PTYPE_BCK_RC, rcdelay) ;
								} else {
									ptype->DATA = bck_adddelay ((bck_delaylist *)ptype->DATA, delaytype | INTERCONNECT) ;
									rcdelay = (bck_delaylist *)ptype->DATA ;
								}
								for (pttrans = $3 ; pttrans ; pttrans = pttrans->NEXT) 
						    		pttrans->EVENT1 = EV__ ;
								rcdelay->TRANSLIST = $3 ;
								rcdelay->START = NULL ;
								rcdelay->END   = end ; 
							}
							else {
								fprintf (stderr, "sdf warning : connector %s not found in instance %s\n",$2,loins->INSNAME) ;
							}
						} else 
							fprintf (stderr, "sdf warning : PORT directive at top level - ignored\n") ;
				   }
				   | _INTERCONNECT port_instance port_instance delval_list
				   {
						if (!isextern ($2, separ)) {
							ptins = bck_getloins (instname ($2, separ), htins) ;
							start = insgetlocon (ptins, portname ($2, separ)) ;
						} else 
							start = getlocon (LOFIG, $2) ;
								
						if (!isextern ($3, separ)) {
							ptins = bck_getloins (instname ($3, separ), htins) ;
							end = insgetlocon (ptins, portname ($3, separ)) ;
						} else 
							end = getlocon (LOFIG, $3) ;

						if (start && end) {
							ptype_list *ptype ;
							if((ptype = getptype(end->SIG->USER, PTYPE_BCK_RC)) == NULL) {
				   				rcdelay = bck_adddelay (NULL, delaytype | INTERCONNECT) ;
								end->SIG->USER = addptype (end->SIG->USER, PTYPE_BCK_RC, rcdelay) ;
							} else {
								ptype->DATA = bck_adddelay ((bck_delaylist *)ptype->DATA, delaytype | INTERCONNECT) ;
								rcdelay = (bck_delaylist *)ptype->DATA ;
							}
							for (pttrans = $4 ; pttrans ; pttrans = pttrans->NEXT) 
						    	pttrans->EVENT1 = EV__ ;
							rcdelay->TRANSLIST = $4 ;
							rcdelay->START = start ;
							rcdelay->END   = end ; 
						}
				   }
				   | _DEVICE port_instance delval_list
				   | _DEVICE               delval_list
				   ;
				   
retain_list        : empty
                   | retain_list '(' _RETAIN delval_list ')'
				   ;

/* timing checks */

tc_spec            : _TIMINGCHECK tchk_def_list 
                   ;
                   
tchk_def_list      : '(' tchk_def ')'
                   | tchk_def_list '(' tchk_def ')'
				   ;
                   
tchk_def           : _SETUP     port_tchk port_tchk value
                   {
				   	   data    = insgetlocon (loins, $2.PORT) ;
					   command = insgetlocon (loins, $3.PORT) ;
					   if (data && command) {
				           checks = bck_addcheck (checks, SETUP) ;
					       checks->DATA              = data ; 
					       checks->COMMAND           = command ;
					       checks->TRANSLIST         = bck_addtrans (checks->TRANSLIST) ;
					       checks->TRANSLIST->EVENT1 = $2.TRANS ;
					       checks->TRANSLIST->EVENT2 = $3.TRANS ;
					       checks->TRANSLIST->VALUE  = $4 ;
					   }
				   }
                   | _HOLD      port_tchk port_tchk value
                   {
				   	   data    = insgetlocon (loins, $2.PORT) ;
					   command = insgetlocon (loins, $3.PORT) ;
					   if (data && command) {
				           checks = bck_addcheck (checks, HOLD) ;
					       checks->DATA              = data ; 
					       checks->COMMAND           = command ; 
					       checks->TRANSLIST         = bck_addtrans (checks->TRANSLIST) ;
					       checks->TRANSLIST->EVENT1 = $2.TRANS ;
					       checks->TRANSLIST->EVENT2 = $3.TRANS ;
					       checks->TRANSLIST->VALUE  = $4 ;
					    }
				   }
				   | _SETUPHOLD port_tchk port_tchk value value scond ccond
                   {
				       data    = insgetlocon (loins, $2.PORT) ;
					   command = insgetlocon (loins, $3.PORT) ;
					   if (data && command) {
				           checks = bck_addcheck (checks, SETUP) ;
					       checks->DATA              = data ; 
					       checks->COMMAND           = command ;
					       checks->TRANSLIST         = bck_addtrans (checks->TRANSLIST) ;
					       checks->TRANSLIST->EVENT1 = $2.TRANS ;
					       checks->TRANSLIST->EVENT2 = $3.TRANS ;
					       checks->TRANSLIST->VALUE  = $4 ;
				           checks = bck_addcheck (checks, HOLD) ;
					       checks->DATA              = data ; 
					       checks->COMMAND           = command ;
					       checks->TRANSLIST         = bck_addtrans (checks->TRANSLIST) ;
					       checks->TRANSLIST->EVENT1 = $2.TRANS ;
					       checks->TRANSLIST->EVENT2 = $3.TRANS ;
					       checks->TRANSLIST->VALUE  = $5 ;
				       }
				   }
				   | _RECOVERY  port_tchk port_tchk value
                   {
				   	   data    = insgetlocon (loins, $2.PORT) ;
					   command = insgetlocon (loins, $3.PORT) ;
					   if (data && command) {
				           checks = bck_addcheck (checks, RECOVERY) ;
					       checks->DATA              = data ; 
					       checks->COMMAND           = command ; 
					       checks->TRANSLIST         = bck_addtrans (checks->TRANSLIST) ;
					       checks->TRANSLIST->EVENT1 = $2.TRANS ;
					       checks->TRANSLIST->EVENT2 = $3.TRANS ;
					       checks->TRANSLIST->VALUE  = $4 ;
					    }
				   }
				   | _REMOVAL   port_tchk port_tchk value
                   {
				   	   data    = insgetlocon (loins, $2.PORT) ;
					   command = insgetlocon (loins, $3.PORT) ;
					   if (data && command) {
				           checks = bck_addcheck (checks, REMOVAL) ;
					       checks->DATA              = data ; 
					       checks->COMMAND           = command ; 
					       checks->TRANSLIST         = bck_addtrans (checks->TRANSLIST) ;
					       checks->TRANSLIST->EVENT1 = $2.TRANS ;
					       checks->TRANSLIST->EVENT2 = $3.TRANS ;
					       checks->TRANSLIST->VALUE  = $4 ;
					    }
				   }
				   | _RECREM    port_tchk port_tchk value value scond ccond
                   {
				       data    = insgetlocon (loins, $2.PORT) ;
					   command = insgetlocon (loins, $3.PORT) ;
					   if (data && command) {
				           checks = bck_addcheck (checks, RECOVERY) ;
					       checks->DATA              = data ; 
					       checks->COMMAND           = command ;
					       checks->TRANSLIST         = bck_addtrans (checks->TRANSLIST) ;
					       checks->TRANSLIST->EVENT1 = $2.TRANS ;
					       checks->TRANSLIST->EVENT2 = $3.TRANS ;
					       checks->TRANSLIST->VALUE  = $4 ;
				           checks = bck_addcheck (checks, REMOVAL) ;
					       checks->DATA              = data ; 
					       checks->COMMAND           = command ;
					       checks->TRANSLIST         = bck_addtrans (checks->TRANSLIST) ;
					       checks->TRANSLIST->EVENT1 = $2.TRANS ;
					       checks->TRANSLIST->EVENT2 = $3.TRANS ;
					       checks->TRANSLIST->VALUE  = $5 ;
				       }
				   }
				   | _SKEW      port_tchk port_tchk value
				   | _WIDTH     port_tchk value
				   | _PERIOD    port_tchk value
				   | _NOCHANGE  port_tchk port_tchk value value
				   ;
                   
port_tchk          : port_spec
                   {
				       $$ = $1 ;
				   }
                   | '(' _COND _QSTRING timing_tchk_cond port_spec ')'
                   {
				       $$ = $5 ;
                       mbkfree($3);
				   }
                   | '(' _COND timing_tchk_cond port_spec ')'
                   {
				       $$ = $4 ;
				   }
				   ;
                   
scond              : _SCOND _QSTRING timing_tchk_cond ')' { mbkfree($2); }
                   | _SCOND          timing_tchk_cond ')'
				   | empty
				   ;
                   
ccond              : _CCOND _QSTRING timing_tchk_cond ')' { mbkfree($2); }
                   | _CCOND          timing_tchk_cond ')'
				   | empty
				   ;
                   
port_spec          : port_instance
                   {
				       $$.PORT = $1 ;
					   $$.TRANS = EV__ ;
				   }
                   | port_edge
                   {
				       $$ = $1 ;
				   } 
				   ;
                   
port_edge          : '(' EDGE_IDENTIFIER port_instance ')'
                   {
				       $$.PORT = $3 ;
					   $$.TRANS = $2 ;
				   }
                   ;
                   
EDGE_IDENTIFIER    : _POSEDGE
                   {
				       $$ = POSEDGE ;
				   }
                   | _NEGEDGE
                   {
				       $$ = NEGEDGE ;
				   }
				   | _01
                   {
				       $$ = EV01 ;
				   }
				   | _10
                   {
				       $$ = EV10 ;
				   }
				   | _0Z
                   {
				       $$ = EV0Z ;
				   }
				   | _Z1
                   {
				       $$ = EVZ1 ;
				   }
				   | _1Z
                   {
				       $$ = EV1Z ;
				   }
				   | _Z0
                   {
				       $$ = EVZ0 ;
				   }
				   ;
                   

port_instance      : _IDENTIFIER
                   {
				       $$ = $1 ;
				   }
				   ;
				   

/* timing environment */

te_spec            : _TIMINGENV te_def_list 
                   ;
                   
te_def_list        : '(' te_def ')' 
                   | te_def_list '(' te_def ')'
				   ;
                   
te_def             : cns_def
                   | tenv_def
				   ;
                   
cns_def            : _PATHCONSTRAINT name port_instance port_instance_list value value
                   | _PERIODCONSTRAINT port_instance value exception
				   | _SUM constr_path constr_path_list value value
				   | _SUM constr_path constr_path_list value
				   | _DIFF constr_path constr_path value value
				   | _DIFF constr_path constr_path value
				   | _SKEWCONSTRAINT port_spec value
                   ;

name               : '(' _NAME _QSTRING ')' { mbkfree($3); }
                   | '(' _NAME ')'
				   ;

constr_path_list   : constr_path
                   | constr_path_list constr_path
				   ;

constr_path        : '(' port_instance port_instance ')'
                   ;

exception          : '(' _EXCEPTION cell_instance_list ')'
                   ;

cell_instance_list : cell_instance
                   | cell_instance_list cell_instance
				   ;

tenv_def           : _ARRIVAL   port_edge port_instance value value value value
                   | _ARRIVAL             port_instance value value value value
                   | _DEPARTURE port_edge port_instance value value value value
				   | _DEPARTURE           port_instance value value value value
				   | _SLACK               port_instance value value value value number
				   | _SLACK               port_instance value value value value
				   | _WAVEFORM            port_instance number edge_list
				   ;

edge_list          : pos_pair_list
                   | neg_pair_list
				   ;

pos_pair_list      : '(' _POSEDGE number number ')' '(' _NEGEDGE number number ')'
                   | '(' _POSEDGE number         ')' '(' _NEGEDGE number number ')'
                   | '(' _POSEDGE number number ')' '(' _NEGEDGE number         ')'
                   | '(' _POSEDGE number         ')' '(' _NEGEDGE number         ')'
				   ;

neg_pair_list      : '(' _NEGEDGE number number ')' '(' _POSEDGE number number ')'
                   | '(' _NEGEDGE number         ')' '(' _POSEDGE number number ')'
                   | '(' _NEGEDGE number number ')' '(' _POSEDGE number         ')'
                   | '(' _NEGEDGE number         ')' '(' _POSEDGE number         ')'
				   ;


port_instance_list : port_instance 
                   {
				   }
                   | port_instance_list port_instance
                   {
				   }
				   ;

/* values */

delval_list        : delval
                   {
				       trans = NULL ;
					   if ($1 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV__ ;
					       trans->VALUE  = $1 ;
					   }
					   $$ = trans ;
				   }
                   | delval delval
                   {
				       trans = NULL ;
					   if ($1 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV01 ;
					       trans->VALUE  = $1 ;
					   }
					   if ($2 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV10 ;
					       trans->VALUE  = $2 ;
					   }
					   $$ = trans ;
				   }
                   | delval delval delval
                   {
				       trans = NULL ;
					   if ($1 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV01 ;
					       trans->VALUE  = $1 ;
					   }
					   if ($2 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV10 ;
					       trans->VALUE  = $2 ;
					   }
					   if ($3 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV_Z ;
					       trans->VALUE  = $3 ;
					   }
					   $$ = trans ;
				   }
                   | delval delval delval delval delval delval
                   {
				       trans = NULL ;
					   if ($1 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV01 ;
					       trans->VALUE  = $1 ;
					   }
					   if ($2 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV10 ;
					       trans->VALUE  = $2 ;
					   }
					   if ($3 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV0Z ;
					       trans->VALUE  = $3 ;
					   }
					   if ($4 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EVZ1 ;
					       trans->VALUE  = $4 ;
					   }
					   if ($5 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV1Z ;
					       trans->VALUE  = $5 ;
					   }
					   if ($6 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EVZ0 ;
					       trans->VALUE  = $6 ;
					   }
					   $$ = trans ;
				   }
                   | delval delval delval delval delval delval delval delval delval delval delval delval
                   {
				       trans = NULL ;
					   if ($1 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV01 ;
					       trans->VALUE  = $1 ;
					   }
					   if ($2 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV10 ;
					       trans->VALUE  = $2 ;
					   }
					   if ($3 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV0Z ;
					       trans->VALUE  = $3 ;
					   }
					   if ($4 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EVZ1 ;
					       trans->VALUE  = $4 ;
					   }
					   if ($5 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV1Z ;
					       trans->VALUE  = $5 ;
					   }
					   if ($6 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EVZ0 ;
					       trans->VALUE  = $6 ;
					   }
					   if ($7 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV0X ;
					       trans->VALUE  = $7 ;
					   }
					   if ($8 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EVX1 ;
					       trans->VALUE  = $8 ;
					   }
					   if ($9 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EV1X ;
					       trans->VALUE  = $9 ;
					   }
					   if ($10 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EVX0 ;
					       trans->VALUE  = $10 ;
					   }
					   if ($11 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EVXZ ;
					       trans->VALUE  = $11 ;
					   }
					   if ($12 != NO_VALUE) {
				           trans        = bck_addtrans (trans) ;
					       trans->EVENT2 = EVZX ;
					       trans->VALUE  = $12 ;
					   }
					   $$ = trans ;
				   }
				   ;
                   
delval             : value 
                   {
				       $$ = $1 ;
				   }
                   | '(' value value ')'
                   {
				       $$ = $2 ;
				   }
                   | '(' value value value ')'
                   {
				       $$ = $2 ;
				   }
				   ;
                   
value              : '(' number ')'
                   {
				       $$ = $2 * sdf_timescale ;
				   }
                   | '(' triple ')'
                   {
				       $$ = $2 * sdf_timescale ;
				   }
                   | '(' ')'
                   {
				       $$ = NO_VALUE ;
				   }
				   ;
				   
triple             : number ':' number ':' number
                   {
				       switch (PROCESS) {
					      case BEST :
						      $$ = $1 ;
						      break ;
					      case TYPICAL :
						      $$ = $3 ;
						      break ;
					      case WORST :
						      $$ = $5 ;
						      break ;
					   }
				   }
				   |         ':'         ':' number
                   {
				       $$ = $3 ;
				   }
				   |         ':' number ':' number
                   {
				       switch (PROCESS) {
					      case BEST :
						      $$ = $2 ;
						      break ;
					      case TYPICAL :
						      $$ = $2 ;
						      break ;
					      case WORST :
						      $$ = $4 ;
						      break ;
					   }
				   }
				   |         ':' number ':'     
                   {
				       $$ = $2 ;
				   }
				   | number ':' number ':'
                   {
				       switch (PROCESS) {
					      case BEST :
						      $$ = $1 ;
						      break ;
					      case TYPICAL :
						      $$ = $3 ;
						      break ;
					      case WORST :
						      $$ = $3 ;
						      break ;
					   }
				   }
				   | number ':'         ':'
                   {
				       $$ = $1 ;
				   }
				   | number ':'         ':' number
                   {
				       switch (PROCESS) {
					      case BEST :
						      $$ = $1 ;
						      break ;
					      case TYPICAL :
						      $$ = $4 ;
						      break ;
					      case WORST :
						      $$ = $4 ;
						      break ;
					   }
				   }
				   ;

number             : _NUMBER
                   {
                    $$ = $1 ;
                   }
                   | _10
                   {
                    $$ = 10 ;
                   }
                   ;
                   
/* conditions */

cond_port_expr     : port_expr
				   | cond_port_expr port_expr
				   ;
                   
port_expr          : SCALAR_CONSTANT
                   | OPERATOR
                   | '(' cond_port_expr ')'
                   | _IDENTIFIER
                   {
				   }
				   ;
                   
SCALAR_CONSTANT    : _B1
                   | _B0
				   ;
                   
OPERATOR           : '+'
                   | '-'
                   | '!'
                   | '~'
                   | "~&"
				   | "~|"
                   | '*'
                   | '/'
                   | '%'
                   | '<'
                   | '>'
                   | '&'
                   | '|'
                   | '^'
                   | "=="
                   | "!="
                   | "==="
                   | "!=="
                   | "&&"
                   | "||"
                   | "<="
                   | ">="
				   | "^~"
				   | "~^"
				   | "<<"
				   | ">>"
				   ;
                   
timing_tchk_cond   : _ARRIVAL
                   ;
                   
empty              : /* empty */ 
                   ;

%%

extern char stbtext[] ;

int yyerror ()
{
	fprintf (stderr, "parse error line %d\n", LINE) ;
	return EXIT_FAILURE ;
}

