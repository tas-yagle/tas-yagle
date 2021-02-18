/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit : GCF Version 1.00                                              */
/*    Fichier : gcf.y                                                         */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Gilles Augustins                                            */
/*                                                                            */
/******************************************************************************/

%{
#include "tlf_parse.h" 

/******************************************************************************/
/* private                                                                    */
/******************************************************************************/

typedef struct gcf_edge {
    long POSEDGE ;
    long NEGEDGE ;
} gcf_edge ;

gcf_edge       *ed ;
ht         *clockht, *vclockht ;
chain_list *ch, *vclockchain = NULL ;
chain_list *chfrom, *chto ;
char        clocktype ;
char       *ckname, *ckalias, *alckalias, *chipname ;
char       *srelative, *hrelative ;
long        nfrom, nto ;
long        tedge ;
float       stablerise, unstablerise, stablefall, unstablefall ;
float       setuprise, holdrise, setupfall, holdfall ;
float       timescale ;

/******************************************************************************/
/* function declarations                                                      */
/******************************************************************************/

int yyerror ();
int yylex ();

%}

%union {
  struct { struct chain *FROMLIST ; struct chain *TOLIST ; } arct ;
  char                                                      *text ;
  float                                                      real ;
  struct chain                                              *list ;
} ;

%start gcf_file 

%token        GCF_TOKEN_ARRIVAL
%token        GCF_TOKEN_CELL
%token        GCF_TOKEN_CLOCK
%token        GCF_TOKEN_CSCALE 
%token        GCF_TOKEN_DATE 
%token        GCF_TOKEN_DELIM
%token        GCF_TOKEN_DEPARTURE
%token        GCF_TOKEN_DESIGN 
%token        GCF_TOKEN_DISABLE
%token <real> GCF_TOKEN_ENUMBER
%token        GCF_TOKEN_ENVIRONMENT
%token        GCF_TOKEN_EXCEPTIONS
%token        GCF_TOKEN_FROM
%token        GCF_TOKEN_GCF
%token        GCF_TOKEN_GLOBALS
%token        GCF_TOKEN_GLOBAL_SUBSET_TIMING
%token        GCF_TOKEN_HEADER
%token <text> GCF_TOKEN_IDENT
%token        GCF_TOKEN_LEFTPAR
%token        GCF_TOKEN_NEGEDGE
%token <real> GCF_TOKEN_NUMBER
%token        GCF_TOKEN_POSEDGE
%token <text> GCF_TOKEN_QSTRING
%token <text> GCF_TOKEN_QIDENT
%token        GCF_TOKEN_RIGHTPAR
%token        GCF_TOKEN_RSCALE 
%token        GCF_TOKEN_SUBSET_TIMING
%token        GCF_TOKEN_STAR
%token        GCF_TOKEN_TO
%token        GCF_TOKEN_TSCALE 
%token        GCF_TOKEN_VERSION
%token        GCF_TOKEN_VSCALE 
%token        GCF_TOKEN_WAVEFORM

%type  <arct> arc
%type  <text> clockspec
%type  <text> nconnector
%type  <list> from
%type  <text> ident
%type  <list> ident_list
%type  <real> number
%type  <list> to
%type  <real> wnumber

%%

gcf_file        : GCF_TOKEN_LEFTPAR gcf header globals cell GCF_TOKEN_RIGHTPAR 
                {
                    if (GCF_MODE == SPECOUT_DUMP) {
                        for (ch = vclockchain ; ch ; ch = ch->NEXT)
                            mbkfree(ch->DATA) ;
                        freechain (vclockchain) ;
                    }
                }
                ;
                
gcf             : GCF_TOKEN_GCF
                {
                    if (GCF_MODE == CLOCK_DETECT) {
                        clockht = addht (GCF_SIZEHCK) ;
                        vclockht = addht (GCF_SIZEHCK) ;
                    }
                
                    if (GCF_MODE == CLOCK_DUMP) {
                        fprintf (GCF_OUT, "\nclock connectors\nbegin\n") ;
                    }
                  
                    if (GCF_MODE == SPECIN_DUMP) {
                        fprintf (GCF_OUT, "\nspecify input connectors\nbegin\n") ;
                    }
                  
                    if (GCF_MODE == SPECOUT_DUMP) {
                        fprintf (GCF_OUT, "\nverify output connectors\nbegin\n") ;
                    }
                }
                ;
                
header          : GCF_TOKEN_LEFTPAR GCF_TOKEN_HEADER version design date delimiters tscale cscale rscale vscale GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == CLOCK_DETECT) {
                        fprintf (GCF_OUT, "name %s ;\n", chipname) ;
                        fprintf (GCF_OUT, "version 1.00 ;\n") ;
                        fprintf (GCF_OUT, "setuptime 0 ;\n") ;
                        fprintf (GCF_OUT, "holdtime 0 ;\n") ;
                    }
                }
                ;
                
version         : GCF_TOKEN_LEFTPAR GCF_TOKEN_VERSION GCF_TOKEN_QSTRING GCF_TOKEN_RIGHTPAR
                | GCF_TOKEN_LEFTPAR GCF_TOKEN_VERSION GCF_TOKEN_QIDENT GCF_TOKEN_RIGHTPAR
                ;
                
design          : GCF_TOKEN_LEFTPAR GCF_TOKEN_DESIGN GCF_TOKEN_QSTRING GCF_TOKEN_RIGHTPAR
                {
                    chipname = stm_unquote ($3) ;
                }
                | GCF_TOKEN_LEFTPAR GCF_TOKEN_DESIGN GCF_TOKEN_QIDENT GCF_TOKEN_RIGHTPAR
                {
                    chipname = stm_unquote ($3) ;
                }
                ;
                
date            : GCF_TOKEN_LEFTPAR GCF_TOKEN_DATE GCF_TOKEN_QSTRING GCF_TOKEN_RIGHTPAR
                | GCF_TOKEN_LEFTPAR GCF_TOKEN_DATE GCF_TOKEN_QIDENT GCF_TOKEN_RIGHTPAR
                ;
                
delimiters      : GCF_TOKEN_LEFTPAR GCF_TOKEN_DELIM GCF_TOKEN_QSTRING GCF_TOKEN_RIGHTPAR
                | GCF_TOKEN_LEFTPAR GCF_TOKEN_DELIM GCF_TOKEN_QIDENT GCF_TOKEN_RIGHTPAR
                ;
                
tscale          : GCF_TOKEN_LEFTPAR GCF_TOKEN_TSCALE number GCF_TOKEN_RIGHTPAR
                {
                    timescale = $3 ;
                }
                ;
                
cscale          : GCF_TOKEN_LEFTPAR GCF_TOKEN_CSCALE number GCF_TOKEN_RIGHTPAR
                ;
                
rscale          : GCF_TOKEN_LEFTPAR GCF_TOKEN_RSCALE number GCF_TOKEN_RIGHTPAR
                ;
                
vscale          : GCF_TOKEN_LEFTPAR GCF_TOKEN_VSCALE number GCF_TOKEN_RIGHTPAR
                ;
                
globals         : GCF_TOKEN_LEFTPAR GCF_TOKEN_GLOBALS globtmgsubset GCF_TOKEN_RIGHTPAR
                ;
                
globtmgsubset   : GCF_TOKEN_LEFTPAR GCF_TOKEN_GLOBAL_SUBSET_TIMING wave_list GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == CLOCK_DUMP) {
                        fprintf (GCF_OUT, "end ;\n") ;
                    }
                }
                ;
                
wave_list       : empty 
                | wave_list wave
                ;
                
wave            : GCF_TOKEN_LEFTPAR GCF_TOKEN_WAVEFORM clockname number edges GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == CLOCK_DUMP && clocktype == REAL_CLOCK) 
                        fprintf (GCF_OUT, "    period %ld ;\n", ftol ($4 * (1e12 * timescale))) ;
                }
                ;
                
clockname       : GCF_TOKEN_QIDENT
                {
                    if (GCF_MODE == CLOCK_DUMP) {
                        ckalias = stm_unquote ($1) ;
                        alckalias = namealloc (ckalias) ;
                        ckname = (char*)gethtitem (clockht, alckalias) ;
                        if ((long)ckname == EMPTYHT) {
                            clocktype = VIRTUAL_CLOCK ;
                            ed = (gcf_edge*) mbkalloc (sizeof (struct gcf_edge)) ;
                            addhtitem (vclockht, alckalias, (long)ed) ;
                            vclockchain = addchain (vclockchain, ed) ; 
                        } else {
                            fprintf (GCF_OUT, "  %s :\n", ckname) ;
                            clocktype = REAL_CLOCK ;
                        }
                    }
                }
                ;
                
edges           : posedge negedge
                | negedge posedge
                ;
                
posedge         : GCF_TOKEN_LEFTPAR GCF_TOKEN_POSEDGE number GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == CLOCK_DUMP && clocktype == REAL_CLOCK) 
                        fprintf (GCF_OUT, "    up     %ld ;\n", ftol ($3 * (1e12 * timescale))) ;
                    if (GCF_MODE == CLOCK_DUMP && clocktype == VIRTUAL_CLOCK)  {
                        ed->POSEDGE = ftol ($3 * (1e12 * timescale)) ;
                    }
                }
                ;
                
negedge         : GCF_TOKEN_LEFTPAR GCF_TOKEN_NEGEDGE number GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == CLOCK_DUMP && clocktype == REAL_CLOCK) 
                        fprintf (GCF_OUT, "    down   %ld ;\n", ftol ($3 * (1e12 * timescale))) ;
                    if (GCF_MODE == CLOCK_DUMP && clocktype == VIRTUAL_CLOCK)  {
                        ed->NEGEDGE = ftol ($3 * (1e12 * timescale)) ;
                    }
                }
                ;
                
cell            : GCF_TOKEN_LEFTPAR GCF_TOKEN_CELL celldef tmgsubset parasubset GCF_TOKEN_RIGHTPAR
                ;
                
celldef         : GCF_TOKEN_LEFTPAR GCF_TOKEN_RIGHTPAR
                ;
                
tmgsubset       : GCF_TOKEN_LEFTPAR GCF_TOKEN_SUBSET_TIMING tmgsubsetenv tmgsubsetexc GCF_TOKEN_RIGHTPAR
                ;
                
tmgsubsetenv    : GCF_TOKEN_LEFTPAR GCF_TOKEN_ENVIRONMENT event_list GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == SPECIN_DUMP || GCF_MODE == SPECOUT_DUMP) {
                        fprintf (GCF_OUT, "end ;\n") ;
                    }
                }
                ;
                
tmgsubsetexc    : GCF_TOKEN_LEFTPAR beginexcept except_list GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == DISABLE_DUMP) 
                        fprintf (GCF_OUT, "end ;\n") ;
                }
                ;
                

beginexcept     : GCF_TOKEN_EXCEPTIONS 
                {
                    if (GCF_MODE == DISABLE_DUMP) 
                        fprintf (GCF_OUT, "\ndisable\nbegin\n") ;
                    
                }
                ;
except_list     : empty
                | except_list except
                ;
                
except          : GCF_TOKEN_LEFTPAR GCF_TOKEN_DISABLE arc GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == DISABLE_DUMP) {
                        if ($3.FROMLIST && $3.TOLIST) {
                            for (chfrom = $3.FROMLIST ; chfrom ; chfrom = chfrom->NEXT) {
                                nfrom = gethtitem (clockht, namealloc ((char*)chfrom->DATA)) ;
                                for (chto = $3.TOLIST ; chto ; chto = chto->NEXT) {
                                    nto = gethtitem (clockht, namealloc ((char*)chto->DATA)) ;
                                    if (nfrom != EMPTYHT && nto != EMPTYHT)
                                        fprintf (GCF_OUT, "  from %s to %s ;\n", (char*)nfrom, (char*)nto) ;
                                }
                            }
                            freechain ($3.FROMLIST) ;
                            freechain ($3.TOLIST) ;
                        }
                        if (!$3.FROMLIST && $3.TOLIST) {
                            for (chto = $3.TOLIST ; chto ; chto = chto->NEXT) {
                                nto = gethtitem (clockht, namealloc ((char*)chto->DATA)) ;
                                if (nto != EMPTYHT)
                                    fprintf (GCF_OUT, "  to %s ;\n", (char*)nto) ;
                            }
                            freechain ($3.TOLIST) ;
                        }
                        if ($3.FROMLIST && !$3.TOLIST) {
                            for (chfrom = $3.FROMLIST ; chfrom ; chfrom = chfrom->NEXT) {
                                nfrom = gethtitem (clockht, namealloc ((char*)chfrom->DATA)) ;
                                if (nfrom != EMPTYHT)
                                    fprintf (GCF_OUT, "  from %s ;\n", (char*)nfrom) ;
                            }
                            freechain ($3.FROMLIST) ;
                        }
                    }
                }
                ;
                
arc             : GCF_TOKEN_LEFTPAR from to GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == DISABLE_DUMP) {
                        $$.FROMLIST = $2 ;
                        $$.TOLIST = $3 ;
                    }
                }
                | from
                {
                    if (GCF_MODE == DISABLE_DUMP) {
                        $$.FROMLIST = $1 ;
                        $$.TOLIST = NULL ;
                    }
                }
                | to
                {
                    if (GCF_MODE == DISABLE_DUMP) {
                        $$.FROMLIST = NULL ;
                        $$.TOLIST = $1 ;
                    }
                }
                ;
                
from            : GCF_TOKEN_LEFTPAR GCF_TOKEN_FROM ident_list GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == DISABLE_DUMP)
                        $$ = $3 ;
                }
                ;
                
to              : GCF_TOKEN_LEFTPAR GCF_TOKEN_TO ident_list GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == DISABLE_DUMP) 
                        $$ = $3 ;
                }
                ;
                
ident_list      : ident
                {
                    $$ = addchain (NULL, $1) ;
                }
                | ident ident_list
                {
                    $$ = addchain ($2, $1) ;
                }
                ;

event_list      : event
                | event_list event
                ;
                
event           : arrival 
                | departure
                | clock
                ;
                
nconnector      : GCF_TOKEN_IDENT
                { 
                    $$ = $1 ;
                }
                | empty
                {
                    $$ = "default" ;
                }
                ;
                
arrival         : GCF_TOKEN_LEFTPAR GCF_TOKEN_ARRIVAL clockspec number number number number nconnector GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == SPECIN_DUMP) {
                        stablerise    = $4 ;
                        unstablerise  = $5 ;
                        stablefall    = $6 ;
                        unstablefall  = $7 ;
                        alckalias = namealloc (ckalias) ;
                        ckname    = (char*)gethtitem (clockht, alckalias) ;
                        fprintf (GCF_OUT, "  %s rising :\n", $8) ;
                        if ((long)ckname == EMPTYHT) {
                            ed = (gcf_edge*)gethtitem (vclockht, alckalias) ;
                            if ((long)ed != EMPTYHT) {
                                if (!strcmp ("rising", $3)) 
                                    tedge = ed->POSEDGE ;
                                else
                                    tedge = ed->NEGEDGE ;
                                fprintf (GCF_OUT, "    stable   %ld ;\n", 
                                ftolstretch (stablerise * (1e12 * timescale)) + tedge) ;
                                fprintf (GCF_OUT, "    unstable %ld ;\n", 
                                ftolstretch (unstablerise * (1e12 * timescale)) + tedge) ;
                            }
                       } else {
                          fprintf (GCF_OUT, "    stable   %ld after %s %s ;\n", 
                               ftolstretch (stablerise * (1e12 * timescale)), ckname, $3) ;
                          fprintf (GCF_OUT, "    unstable %ld after %s %s ;\n", 
                               ftolstretch (unstablerise * (1e12 * timescale)), ckname, $3) ;
                       }
                       fprintf (GCF_OUT, "  %s falling :\n", $8) ;
                       if ((long)ckname == EMPTYHT) {
                            ed = (gcf_edge*)gethtitem (vclockht, alckalias) ;
                            if ((long)ed != EMPTYHT) {
                                if (!strcmp ("rising", $3)) 
                                    tedge = ed->POSEDGE ;
                                else
                                    tedge = ed->NEGEDGE ;
                                fprintf (GCF_OUT, "    stable   %ld ;\n", 
                                ftolstretch (stablefall * (1e12 * timescale)) + tedge) ;
                                fprintf (GCF_OUT, "    unstable %ld ;\n", 
                                ftolstretch (unstablefall * (1e12 * timescale)) + tedge) ;
                            }
                       } else {
                          fprintf (GCF_OUT, "    stable   %ld after %s %s ;\n", 
                               ftolstretch (stablefall * (1e12 * timescale)), ckname, $3) ;
                          fprintf (GCF_OUT, "    unstable %ld after %s %s ;\n", 
                               ftolstretch (unstablefall * (1e12 * timescale)), ckname, $3) ;
                       }
                    }
                }
                ;
                
departure       : GCF_TOKEN_LEFTPAR GCF_TOKEN_DEPARTURE clockspec wnumber wnumber wnumber wnumber nconnector GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == SPECOUT_DUMP) {
                        setuprise = $4 ;
                        setupfall = $5 ;
                        holdrise  = $6 ;
                        holdfall  = $7 ;
                        alckalias = namealloc (ckalias) ;
                        ckname    = (char*)gethtitem (clockht, alckalias) ;

                        /* rising */
                        
                        if ((long)ckname == EMPTYHT) {
                            ed = (gcf_edge*)gethtitem (vclockht, alckalias) ;
                            if ((long)ed != EMPTYHT) {
                                if (!strcmp ("rising", $3)) 
                                    tedge = ed->POSEDGE ;
                                else
                                    tedge = ed->NEGEDGE ;
                                if (holdrise == DC && holdfall == DC && setuprise != DC && setupfall != DC) {
                                    fprintf (GCF_OUT, "  %s rising :\n", $8) ;
                                    fprintf (GCF_OUT, "    stable   %ld ;\n", 
                                        tedge - ftolstretch (setuprise * (1e12 * timescale))) ;
                                    fprintf (GCF_OUT, "    unstable %ld ;\n", 
                                        tedge - ftolstretch (setuprise * (1e12 * timescale)) - 1) ; /* hold = setup - 1 */
                                }
                                else if (holdrise != DC && holdfall != DC && setuprise != DC && setupfall != DC) {
                                    fprintf (GCF_OUT, "  %s rising :\n", $8) ;
                                    fprintf (GCF_OUT, "    stable   %ld ;\n", 
                                        tedge - ftolstretch (setuprise * (1e12 * timescale))) ;
                                    fprintf (GCF_OUT, "    unstable %ld ;\n", 
                                        ftolstretch (holdrise * (1e12 * timescale)) + tedge) ;
                                }
                                else if (holdrise != DC && holdfall != DC && setuprise == DC && setupfall == DC) {
                                    fprintf (GCF_OUT, "  %s rising :\n", $8) ;
                                    fprintf (GCF_OUT, "    stable   %ld ;\n", 
                                        ftolstretch (holdrise * (1e12 * timescale)) + tedge + 1) ; /* setup = hold + 1 */
                                    fprintf (GCF_OUT, "    unstable %ld ;\n", 
                                        ftolstretch (holdrise * (1e12 * timescale)) + tedge) ;
                                }
                            }
                        } else {
                            if (holdrise == DC && holdfall == DC && setuprise != DC && setupfall != DC) {
                                fprintf (GCF_OUT, "  %s rising :\n", $8) ;
                                fprintf (GCF_OUT, "    stable   %ld before %s %s ;\n", 
                                    ftolstretch (setuprise * (1e12 * timescale)), ckname, $3) ;
                                fprintf (GCF_OUT, "    unstable %ld before %s %s ;\n", 
                                    ftolstretch (setuprise * (1e12 * timescale)) - 1, ckname, $3) ; /* hold = setup - 1 */
                            }
                            else if (holdrise != DC && holdfall != DC && setuprise != DC && setupfall != DC) {
                                fprintf (GCF_OUT, "  %s rising :\n", $8) ;
                                fprintf (GCF_OUT, "    stable   %ld before %s %s ;\n", 
                                    ftolstretch (setuprise * (1e12 * timescale)), ckname, $3) ;
                                fprintf (GCF_OUT, "    unstable %ld after %s %s ;\n", 
                                    ftolstretch (holdrise * (1e12 * timescale)), ckname, $3) ;
                            }
                            else if (holdrise != DC && holdfall != DC && setuprise == DC && setupfall == DC) {
                                fprintf (GCF_OUT, "  %s rising :\n", $8) ;
                                fprintf (GCF_OUT, "    stable   %ld after %s %s ;\n", 
                                    ftolstretch (setuprise * (1e12 * timescale)) + 1, ckname, $3) ; /* setup = hold + 1 */
                                fprintf (GCF_OUT, "    unstable %ld after %s %s ;\n", 
                                    ftolstretch (holdrise * (1e12 * timescale)), ckname, $3) ;
                            }
                        }

                        /* falling */

                        if ((long)ckname == EMPTYHT) {
                            ed = (gcf_edge*)gethtitem (vclockht, alckalias) ;
                            if ((long)ed != EMPTYHT) {
                                if (!strcmp ("falling", $3)) 
                                    tedge = ed->POSEDGE ;
                                else
                                    tedge = ed->NEGEDGE ;
                                if (holdfall == DC && holdfall == DC && setupfall != DC && setupfall != DC) {
                                    fprintf (GCF_OUT, "  %s falling :\n", $8) ;
                                    fprintf (GCF_OUT, "    stable   %ld ;\n", 
                                        tedge - ftolstretch (setupfall * (1e12 * timescale))) ;
                                    fprintf (GCF_OUT, "    unstable %ld ;\n", 
                                        tedge - ftolstretch (setupfall * (1e12 * timescale)) - 1) ; /* hold = setup - 1 */
                                }
                                else if (holdfall != DC && holdfall != DC && setupfall != DC && setupfall != DC) {
                                    fprintf (GCF_OUT, "  %s falling :\n", $8) ;
                                    fprintf (GCF_OUT, "    stable   %ld ;\n", 
                                        tedge - ftolstretch (setupfall * (1e12 * timescale))) ;
                                    fprintf (GCF_OUT, "    unstable %ld ;\n", 
                                        ftolstretch (holdfall * (1e12 * timescale)) + tedge) ;
                                }
                                else if (holdfall != DC && holdfall != DC && setupfall == DC && setupfall == DC) {
                                    fprintf (GCF_OUT, "  %s falling :\n", $8) ;
                                    fprintf (GCF_OUT, "    stable   %ld ;\n", 
                                        ftolstretch (holdfall * (1e12 * timescale)) + tedge + 1) ; /* setup = hold + 1 */
                                    fprintf (GCF_OUT, "    unstable %ld ;\n", 
                                        ftolstretch (holdfall * (1e12 * timescale)) + tedge) ;
                                }
                            }
                        } else {
                            if (holdfall == DC && holdfall == DC && setupfall != DC && setupfall != DC) {
                                fprintf (GCF_OUT, "  %s falling :\n", $8) ;
                                fprintf (GCF_OUT, "    stable   %ld before %s %s ;\n", 
                                    ftolstretch (setupfall * (1e12 * timescale)), ckname, $3) ;
                                fprintf (GCF_OUT, "    unstable %ld before %s %s ;\n", 
                                    ftolstretch (setupfall * (1e12 * timescale)) - 1, ckname, $3) ; /* hold = setup - 1 */
                            }
                            else if (holdfall != DC && holdfall != DC && setupfall != DC && setupfall != DC) {
                                fprintf (GCF_OUT, "  %s falling :\n", $8) ;
                                fprintf (GCF_OUT, "    stable   %ld before %s %s ;\n", 
                                    ftolstretch (setupfall * (1e12 * timescale)), ckname, $3) ;
                                fprintf (GCF_OUT, "    unstable %ld after %s %s ;\n", 
                                    ftolstretch (holdfall * (1e12 * timescale)), ckname, $3) ;
                            }
                            else if (holdfall != DC && holdfall != DC && setupfall == DC && setupfall == DC) {
                                fprintf (GCF_OUT, "  %s falling :\n", $8) ;
                                fprintf (GCF_OUT, "    stable   %ld after %s %s ;\n", 
                                    ftolstretch (setupfall * (1e12 * timescale)) + 1, ckname, $3) ; /* setup = hold + 1 */
                                fprintf (GCF_OUT, "    unstable %ld after %s %s ;\n", 
                                    ftolstretch (holdfall * (1e12 * timescale)), ckname, $3) ;
                            }
                        }
                    }
                }
                ;
                
clockspec       : GCF_TOKEN_LEFTPAR GCF_TOKEN_POSEDGE GCF_TOKEN_QIDENT GCF_TOKEN_RIGHTPAR
                {
                    $$      = "rising" ;
                    ckalias = stm_unquote ($3) ;
                }
                | GCF_TOKEN_LEFTPAR GCF_TOKEN_NEGEDGE GCF_TOKEN_QIDENT GCF_TOKEN_RIGHTPAR
                {
                    $$      = "falling" ;
                    ckalias = stm_unquote ($3) ;
                }
                ;
                
clock           : GCF_TOKEN_LEFTPAR GCF_TOKEN_CLOCK GCF_TOKEN_QIDENT GCF_TOKEN_IDENT GCF_TOKEN_RIGHTPAR
                {
                    if (GCF_MODE == CLOCK_DETECT) {
                        ckalias = namealloc (stm_unquote ($3)) ;
                        ckname = $4 ;
                        addhtitem (clockht, ckalias, (long)ckname) ;
                    }
                }
                ;
                
ident           : GCF_TOKEN_IDENT
                {
                    $$ = $1 ;
                }
                | GCF_TOKEN_QIDENT
                {
                    $$ = stm_unquote ($1) ;
                }
                ;
                
parasubset      : empty
                ;
                
empty           : /* empty */ 
                ;
                
wnumber         : number
                { 
                    $$ = $1 ;
                }
                | GCF_TOKEN_STAR
                {
                    $$ = DC ;
                }
                ;

number          : GCF_TOKEN_NUMBER
                {
                    $$ = $1 ;
                }
                | GCF_TOKEN_ENUMBER
                {
                    $$ = $1 ;
                }
                ;
%%

int yyerror ()
{
    printf ("*** gcf error\n") ;
    printf ("*** parse error line %d\n", GCF_LINE) ;
    //printf ("*** cannot match token '%s'\n", yylval.text) ;
    return 0;
}

