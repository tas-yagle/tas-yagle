%{
#include MUT_H
#include <string.h>
#include "tlf3.tab.h"
#define ECHO /*fprintf(stdout,"\ntoken %s\n",yytext)*/

extern int        TLF_LINE ;
static int        parlevel ;
static int        reglevel ;
static int        exprlevel;
static int        PREVIOUS ;
static char       expr_tab[1024] ;

#define YY_NO_UNPUT

%}
%x DONTCARE 
%x COMMENT 
%x REGDEF
%x EXPR_LOGIC


string                    [^\(\)\n]*
upper_alpha               [A-Z]
lower_alpha               [a-z]
num                       [0-9]
alpha                     ({upper_alpha}|{lower_alpha})
alphanum                  ({alpha}|{num})
number                    -?{num}+(\.({num}*))?
enumber                   -?{num}+(\.({num}*))?(e|E)[+|-]?{num}+
ident                     {alpha}(([[|./:_]?)+{alphanum}((\[|\\\[|$){num}+(\]|\\\]|$))?)*
vect_ident                {alpha}(([[|./:_]?)+{alphanum}((\[|\\\[|$){num}+:{num}+(\]|\\\]|$))?)*
vect                      \[{num}+\]
comment                   \/\/
qstring                   \".*\"
cell                      [cC][eE][lL][lL]                             
celltype                  [cC][eE][lL][lL][tT][yY][pP][eE]                         
seq                       [sS][eE][qQ]                           
comb                      [cC][oO][mM][bB]                          
ignore                    [iI][gG][nN][oO][rR][eE]                           
model                     [mM][oO][dD][eE][lL]                            
spline                    [sS][pP][lL][iI][nN][eE]                           
const                     [cC][oO][nN][sS][tT]                            
data                      [dD][aA][tT][aA]                          
input                     [iI][nN][pP][uU][tT]                            
output                    [oO][uU][tT][pP][uU][tT]                           
bidir                     [bB][iI][dD][iI][rR]                            
clock                     [cC][lL][oO][cC][kK]                            
clock_slew_axis           [cC][lL][oO][cC][kK]_[sS][lL][eE][wW]_[aA][xX][iI][sS]            
input_slew_axis           [iI][nN][pP][uU][tT]_[sS][lL][eE][wW]_[aA][xX][iI][sS]            
slew_axis                 [sS][lL][eE][wW]_[aA][xX][iI][sS]                  
load_axis                 [lL][oO][aA][dD]_[aA][xX][iI][sS]                  
axis                      [aA][xX][iI][sS]                  
path                      [pP][aA][tT][hH]                       
path_extension            [pP][aA][tT][hH]_[eE][xX][tT][eE][nN][sS][iI][oO][nN]             
delay                     [dD][eE][lL][aA][yY]                      
slew                      [sS][lL][eE][wW]                       
t01                       01                         
t10                       10                         
t0z                       0[zZ]                         
t1z                       1[zZ]                         
tz0                       [zZ]0                         
tz1                       [zZ]1                         
negedge                   [nN][eE][gG][eE][dD][gG][eE]                    
posedge                   [pP][oO][sS][eE][dD][gG][eE]                    
setup                     [sS][eE][tT][uU][pP]                      
hold                      [hH][oO][lL][dD]                       
pin                       [pP][iI][nN]                        
pintype                   [pP][iI][nN][tT][yY][pP][eE]                    
ground                    [gG][rR][oO][uU][nN][dD]                     
pindir                    [pP][iI][nN][dD][iI][rR]                     
pin_cap                   [pP][iI][nN]_[cC][aA][pP]                    
timing_props              [tT][iI][mM][iI][nN][gG]_[pP][rR][oO][pP][sS]                     
for_bits                  [fF][oO][rR]_[bB][iI][tT][sS]                   
vendor                    [vV][eE][nN][dD][oO][rR]                           
environment               [eE][nN][vV][iI][rR][oO][nN][mM][eE][nN][tT]                      
version                   [vV][eE][rR][sS][iI][oO][nN]                          
tlf_version               [tT][lL][fF]_[vV][eE][rR][sS][iI][oO][nN]                      
header                    [hH][eE][aA][dD][eE][rR]                           
date                      [dD][aA][tT][eE]                          
library                   [lL][iI][bB][rR][aA][rR][yY]                       
technology                [tT][eE][cC][hH][nN][oO][lL][oO][gG][yY]                       
control                   [cC][oO][nN][tT][rR][oO][lL]                          
tristate                  [tT][rR][iI][sS][tT][aA][tT][eE]                         
asynch                    [aA][sS][yY][nN][cC][hH]                           
low                       [lL][oO][wW]                              
high                      [hH][iI][gG][hH]                             
krc                       [kK][rR][cC]                        
proc_mult_setup           [pP][rR][oO][cC]_[mM][uU][lL][tT]_[sS][eE][tT][uU][pP]            
proc_mult_hold            [pP][rR][oO][cC]_[mM][uU][lL][tT]_[hH][oO][lL][dD]             
proc_mult_mpwl            [pP][rR][oO][cC]_[mM][uU][lL][tT]_[mM][pP][wW][lL]             
proc_mult_mpwh            [pP][rR][oO][cC]_[mM][uU][lL][tT]_[mM][pP][wW][hH]             
proc_mult_period          [pP][rR][oO][cC]_[mM][uU][lL][tT]_[pP][eE][rR][iI][oO][dD]           
proc_mult_propagation     [pP][rR][oO][cC]_[mM][uU][lL][tT]_[pP][rR][oO][pP][aA][gG][aA][tT][iI][oO][nN]      
proc_mult_transition      [pP][rR][oO][cC]_[mM][uU][lL][tT]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]       
volt_mult_setup           [vV][oO][lL][tT]_[mM][uU][lL][tT]_[sS][eE][tT][uU][pP]            
volt_mult_hold            [vV][oO][lL][tT]_[mM][uU][lL][tT]_[hH][oO][lL][dD]             
volt_mult_mpwl            [vV][oO][lL][tT]_[mM][uU][lL][tT]_[mM][pP][wW][lL]             
volt_mult_mpwh            [vV][oO][lL][tT]_[mM][uU][lL][tT]_[mM][pP][wW][hH]             
volt_mult_period          [vV][oO][lL][tT]_[mM][uU][lL][tT]_[pP][eE][rR][iI][oO][dD]           
volt_mult_propagation     [vV][oO][lL][tT]_[mM][uU][lL][tT]_[pP][rR][oO][pP][aA][gG][aA][tT][iI][oO][nN]      
volt_mult_transition      [vV][oO][lL][tT]_[mM][uU][lL][tT]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]       
temp_mult_setup           [tT][eE][mM][pP]_[mM][uU][lL][tT]_[sS][eE][tT][uU][pP]            
temp_mult_hold            [tT][eE][mM][pP]_[mM][uU][lL][tT]_[hH][oO][lL][dD]             
temp_mult_mpwl            [tT][eE][mM][pP]_[mM][uU][lL][tT]_[mM][pP][wW][lL]             
temp_mult_mpwh            [tT][eE][mM][pP]_[mM][uU][lL][tT]_[mM][pP][wW][hH]             
temp_mult_period          [tT][eE][mM][pP]_[mM][uU][lL][tT]_[pP][eE][rR][iI][oO][dD]           
temp_mult_propagation     [tT][eE][mM][pP]_[mM][uU][lL][tT]_[pP][rR][oO][pP][aA][gG][aA][tT][iI][oO][nN]      
temp_mult_transition      [tT][eE][mM][pP]_[mM][uU][lL][tT]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]       
table_input_threshold     [tT][aA][bB][lL][eE]_[iI][nN][pP][uU][tT]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]      
table_output_threshold    [tT][aA][bB][lL][eE]_[oO][uU][tT][pP][uU][tT]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]     
table_transition_start    [tT][aA][bB][lL][eE]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]_[sS][tT][aA][rR][tT]     
table_transition_end      [tT][aA][bB][lL][eE]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]_[eE][nN][dD]       
for_cell                  [fF][oO][rR]_[cC][eE][lL][lL]                   
proc_var                  [pP][rR][oO][cC]_[vV][aA][rR]                         
net_cap                   [nN][eE][tT]_[cC][aA][pP]                       
net_res                   [nN][eE][tT]_[rR][eE][sS]                       
load_limit                [lL][oO][aA][dD]_[lL][iI][mM][iI][tT]                    
slew_limit                [sS][lL][eE][wW]_[lL][iI][mM][iI][tT]                    
temperature               [tT][eE][mM][pP][eE][rR][aA][tT][uU][rR][eE]                      
voltage                   [vV][oO][lL][tT][aA][gG][eE]                          
temp_mult_net_res         [tT][eE][mM][pP]_[mM][uU][lL][tT]_[nN][eE][tT]_[rR][eE][sS]                
area                      [aA][rR][eE][aA]                          
cond                      [cC][oO][nN][dD]                          
sdf_cond                  [sS][dD][fF]_[cC][oO][nN][dD]                         
mpwh                      [mM][pP][wW][hH]                          
mpwl                      [mM][pP][wW][lL]                          
recovery                  [rR][eE][cC][oO][vV][eE][rR][yY]                         
removal                   [rR][eE][mM][oO][vV][aA][lL]                          
register                  [rR][eE][gG][iI][sS][tT][eE][rR]                         
latch                     [lL][aA][tT][cC][hH]                         
function                  [fF][uU][nN][cC][tT][iI][oO][nN]                         
enable                    [eE][nN][aA][bB][lL][eE]                        
generated_by              [gG][eE][nN][eE][rR][aA][tT][eE][dD]_[bB][yY]
incell                    [iI][nN][cC][eE][lL][lL]                             
outcell                   [oO][uU][tT][cC][eE][lL][lL]                             
fast                      [fF][aA][sS][tT]
slow                      [sS][lL][oO][wW]
inverted_output           [iI][nN][vV][eE][rR][tT][eE][dD]_[oO][uU][tT][pP][uU][tT]
clear                     [cC][lL][eE][aA][rR] 
set                       [sS][eE][tT] 
insertion_delay           [iI][nN][sS][eE][rR][tT][iI][oO][nN]_[dD][eE][lL][aA][yY]
slave_clock               [sS][lL][aA][vV][eE]_[cC][lL][oO][cC][kK]
linear                    [lL][iI][nN][eE][aA][rR]
value                     [vV][aA][lL][uU][eE]



%%
\n                        {       TLF_LINE++ ;                                                       }
{comment}.*$              {                                                                          }
[ \t]                     {                                                                          }
<DONTCARE>\(              { ECHO; parlevel++ ;                                                       }
<DONTCARE>[^\(\)\n]*      { ECHO;                                                                    }
<DONTCARE>\n              { ECHO; TLF_LINE++ ;                                                       }
<DONTCARE>\)              { ECHO; parlevel-- ; if (!parlevel) BEGIN PREVIOUS ;                       }
\/\*                      {       PREVIOUS = INITIAL ; BEGIN COMMENT ;                               }
<COMMENT>[^\*\n]*         {                                                                          }
<COMMENT>\*+              {                                                                          }
<COMMENT>\n               {       TLF_LINE++ ;                                                       }
<COMMENT>\*+\/            {       BEGIN PREVIOUS ;                                                   }
\(                        { ECHO; return ('(') ;                                                     }
\)                        { ECHO; return (')') ;                                                     }
:                         { ECHO; return (':') ;                                                     }
=>                        { ECHO; return (TLF3_TOKEN_ARROW) ;                                                  }
{cell}                    { ECHO; return (TLF3_TOKEN_CELL) ;                                                   }
{celltype}                { ECHO; return (TLF3_TOKEN_CELLTYPE) ;                                               }
{seq}                     { ECHO; return (TLF3_TOKEN_SEQ) ;                                                    }
{comb}                    { ECHO; return (TLF3_TOKEN_COMB) ;                                                   }
{ignore}                  { ECHO; return (TLF3_TOKEN_IGNORE) ;                                                 }
{incell}                  { ECHO; return (TLF3_TOKEN_INCELL) ;                                                 }
{outcell}                 { ECHO; return (TLF3_TOKEN_OUTCELL) ;                                                }
{model}                   { ECHO; return (TLF3_TOKEN_MODEL) ;                                                  }
{spline}                  { ECHO; return (TLF3_TOKEN_SPLINE) ;                                                 }
{const}                   { ECHO; return (TLF3_TOKEN_CONST) ;                                                  }
{data}                    { ECHO; return (TLF3_TOKEN_DATA) ;                                                   }
{input}                   { ECHO; return (TLF3_TOKEN_INPUT) ;                                                  }
{output}                  { ECHO; return (TLF3_TOKEN_OUTPUT) ;                                                 }
{bidir}                   { ECHO; return (TLF3_TOKEN_BIDIR) ;                                                  }
{clock}                   { ECHO; return (TLF3_TOKEN_CLOCK) ;                                                  }
{clock_slew_axis}         { ECHO; return (TLF3_TOKEN_CLOCK_SLEW_AXIS) ;                                        }
{input_slew_axis}         { ECHO; return (TLF3_TOKEN_INPUT_SLEW_AXIS) ;                                        }
{slew_axis}               { ECHO; return (TLF3_TOKEN_SLEW_AXIS) ;                                              }
{load_axis}               { ECHO; return (TLF3_TOKEN_LOAD_AXIS) ;                                              }
{axis}                    { ECHO; return (TLF3_TOKEN_AXIS) ;                                                   }
{path}                    { ECHO; return (TLF3_TOKEN_PATH) ;                                                   }
{path_extension}          { ECHO; return (TLF3_TOKEN_PATH) ;                                                   }
{fast}                    { ECHO; return (TLF3_TOKEN_FAST) ;                                                   }
{slow}                    { ECHO; return (TLF3_TOKEN_SLOW) ;                                                   }
{delay}                   { ECHO; return (TLF3_TOKEN_DELAY) ;                                                  }
{slew}                    { ECHO; return (TLF3_TOKEN_SLEW) ;                                                   }
{t01}                     { ECHO; return (TLF3_TOKEN_01) ;                                                     } 
{t10}                     { ECHO; return (TLF3_TOKEN_10) ;                                                     }
{t0z}                     { ECHO; return (TLF3_TOKEN_0Z) ;                                                     }
{t1z}                     { ECHO; return (TLF3_TOKEN_1Z) ;                                                     }
{tz0}                     { ECHO; return (TLF3_TOKEN_Z0) ;                                                     }
{tz1}                     { ECHO; return (TLF3_TOKEN_Z1) ;                                                     }
{negedge}                 { ECHO; return (TLF3_TOKEN_NEGEDGE) ;                                                }
{posedge}                 { ECHO; return (TLF3_TOKEN_POSEDGE) ;                                                }
{setup}                   { ECHO; return (TLF3_TOKEN_SETUP) ;                                                  }
{hold}                    { ECHO; return (TLF3_TOKEN_HOLD) ;                                                   }
{pin}                     { ECHO; return (TLF3_TOKEN_PIN) ;                                                    }
{pintype}                 { ECHO; return (TLF3_TOKEN_PINTYPE) ;                                                }
{ground}                  { ECHO; return (TLF3_TOKEN_GROUND) ;                                                 }
{pindir}                  { ECHO; return (TLF3_TOKEN_PINDIR) ;                                                 }
{pin_cap}                 { ECHO; return (TLF3_TOKEN_PIN_CAP) ;                                                }
{timing_props}            { ECHO; return (TLF3_TOKEN_TIMING_PROPS) ;                                           }
{for_bits}                { ECHO; return (TLF3_TOKEN_FOR_BITS) ;                                               }
{vendor}                  { ECHO; return (TLF3_TOKEN_VENDOR) ;                                                 }
{environment}             { ECHO; return (TLF3_TOKEN_ENVIRONMENT) ;                                            }
{version}                 { ECHO; return (TLF3_TOKEN_VERSION) ;                                                }
{tlf_version}             { ECHO; return (TLF3_TOKEN_TLF_VERSION) ;                                            }
{header}                  { ECHO; return (TLF3_TOKEN_HEADER) ;                                                 }
{date}                    { ECHO; return (TLF3_TOKEN_DATE) ;                                                   }
{library}                 { ECHO; return (TLF3_TOKEN_LIBRARY) ;                                                }
{technology}              { ECHO; return (TLF3_TOKEN_TECHNOLOGY) ;                                             }
{control}                 { ECHO; return (TLF3_TOKEN_CONTROL) ;                                                }
{tristate}                { ECHO; return (TLF3_TOKEN_TRISTATE) ;                                               }
{asynch}                  { ECHO; return (TLF3_TOKEN_ASYNCH) ;                                                 }
{low}                     { ECHO; return (TLF3_TOKEN_LOW) ;                                                    }
{high}                    { ECHO; return (TLF3_TOKEN_HIGH) ;                                                   }
{krc}                     { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               } 
{linear}                  { ECHO; return (TLF3_TOKEN_LINEAR);                                                  } 
{value}                   { ECHO; return (TLF3_TOKEN_VALUE);                                                   } 
"~"                       { ECHO; return ('~');                                                      } 
{insertion_delay}         { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               } 
{generated_by}            { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               } 
{proc_mult_setup}         { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{proc_mult_hold}          { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{proc_mult_mpwl}          { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{proc_mult_mpwh}          { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{proc_mult_period}        { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{proc_mult_propagation}   { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{proc_mult_transition}    { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{volt_mult_setup}         { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{volt_mult_hold}          { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{volt_mult_mpwl}          { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{volt_mult_mpwh}          { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{volt_mult_period}        { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{volt_mult_propagation}   { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{volt_mult_transition}    { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{temp_mult_setup}         { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{temp_mult_hold}          { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{temp_mult_mpwl}          { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{temp_mult_mpwh}          { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{temp_mult_period}        { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{temp_mult_propagation}   { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{temp_mult_transition}    { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{table_input_threshold}   { ECHO; return TLF3_TOKEN_INPUT_THRESHOLD;                                           }
{table_output_threshold}  { ECHO; return TLF3_TOKEN_OUTPUT_THRESHOLD;                                          }
{table_transition_start}  { ECHO; return TLF3_TOKEN_TRANSITION_START;                                          }
{table_transition_end}    { ECHO; return TLF3_TOKEN_TRANSITION_END;                                            } 
{for_cell}                { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{proc_var}                { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{net_cap}                 { ECHO; return TLF3_TOKEN_NET_CAP;                                                   }
{net_res}                 { ECHO; return TLF3_TOKEN_NET_RES;                                                   }
{load_limit}              { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{slew_limit}              { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{temperature}             { ECHO; return TLF3_TOKEN_TEMPERATURE ;                                                    }
{voltage}                 { ECHO; return TLF3_TOKEN_VOLTAGE ;                                                        }
{temp_mult_net_res}       { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{area}                    { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{cond}                    { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{sdf_cond}                { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{mpwh}                    { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{mpwl}                    { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{recovery}                { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{removal}                 { ECHO; parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;               }
{function}                { ECHO; exprlevel = 0; PREVIOUS = INITIAL ; BEGIN EXPR_LOGIC; return (TLF3_TOKEN_FUNCTION); }
<EXPR_LOGIC>\(            { ECHO; exprlevel++  ; 
                                  if(exprlevel == 1) strcpy(expr_tab,yytext);
                                  else if(exprlevel > 1)  strcat(expr_tab,yytext);                   }
<EXPR_LOGIC>{string}      { ECHO; if (exprlevel > 0) strcat(expr_tab,yytext);                        }
<EXPR_LOGIC>\)            { ECHO; exprlevel--  ; strcat(expr_tab,yytext);
                                  if(!exprlevel) {
                                    strcat(expr_tab, "\0");
                                    BEGIN PREVIOUS ;
                                    tlf3lval.text = namealloc(expr_tab);
                                    return (TLF3_TOKEN_EXPR) ; }                                               }
{enable}                  { exprlevel = 0 ; PREVIOUS = INITIAL  ; BEGIN EXPR_LOGIC ; return (TLF3_TOKEN_ENABLE);          }
{register}                { ECHO; reglevel = 0 ; BEGIN REGDEF ; return (TLF3_TOKEN_REGISTER) ;                 }
{latch}                   { ECHO; reglevel = 0 ; BEGIN REGDEF ; return (TLF3_TOKEN_LATCH) ;                    }
<REGDEF>\(                { ECHO; reglevel++   ; return ('(') ;                                      }
<REGDEF>{clear}           { ECHO; exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF3_TOKEN_CLEAR);      }
<REGDEF>{set}             { ECHO; exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF3_TOKEN_SET)  ;      }
<REGDEF>{clock}           { ECHO; exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF3_TOKEN_CLOCK_REG);  }
<REGDEF>{slave_clock}     { ECHO; exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF3_TOKEN_SLAVE_CLOCK);}
<REGDEF>{input}           { ECHO; exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF3_TOKEN_INPUT_REG);  }
<REGDEF>{output}          { ECHO; return (TLF3_TOKEN_OUTPUT) ;                                                 }
<REGDEF>{inverted_output} { ECHO; return (TLF3_TOKEN_INVERTED_OUTPUT) ;                                        }
<REGDEF>{ident}           { ECHO; tlf3lval.text = namealloc (yytext) ; return (TLF3_TOKEN_IDENT) ;             }
<REGDEF>\n                { ECHO; TLF_LINE++ ;                                                       }
<REGDEF>\)                { ECHO; reglevel-- ; if (!reglevel) BEGIN INITIAL ; return (')') ;         }
<REGDEF>\/\*              {       PREVIOUS = REGDEF ; BEGIN COMMENT ;                                }
<REGDEF>{comment}.*$      {                                                                          }
<REGDEF>[ \t]             {                                                                          }
{qstring}                 { ECHO; tlf3lval.text = strdup (yytext) ; return (TLF3_TOKEN_QSTRING) ;           }
{ident}                   { ECHO; tlf3lval.text = namealloc (yytext) ; return (TLF3_TOKEN_IDENT) ;             }
{vect_ident}              { ECHO; tlf3lval.text = strdup (yytext) ; return (TLF3_TOKEN_VECT_IDENT) ;           }
{vect}                    { ECHO; tlf3lval.text = strdup (yytext) ; return (TLF3_TOKEN_VECT) ;                 }
{number}                  { ECHO; tlf3lval.real = strtod (yytext, (char**)NULL) ; return (TLF3_TOKEN_NUMBER) ; }
{enumber}                 { ECHO; tlf3lval.real = strtod (yytext, (char**)NULL) ; return (TLF3_TOKEN_ENUMBER) ;}

%%

int yywrap() { return 1 ; }
