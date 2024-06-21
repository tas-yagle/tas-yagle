%{
#include <string.h>
#include "tlf4.tab.h"
#include "tlf_parse.h"

#define YY_NO_UNPUT

extern int TLF_LINE ;
static int        parlevel ;
static int        reglevel ;
static int        attribute ;
static int        genmodel ;
static int        PREVIOUS ;
static int        exprlevel;
static char       expr_tab[1024] ;
static char       *str ;

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
ident                     {alpha}(([|./_]?)+{alphanum}?((\[|\\\[|$){num}+(\]|\\\]|$))?)*
vect_ident                {alpha}(([|./_]?)+{alphanum}?((\[|\\\[|$){num}+:{num}+(\]|\\\]|$))?)*
vect                      \[{num}+\]
comment                   \/\/
qstring                   \".*\"
cell                      [cC][eE][lL][lL]                             
celltype                  [cC][eE][lL][lL][tT][yY][pP][eE]                         
seq                       [sS][eE][qQ]                           
comb                      [cC][oO][mM][bB]                          
ignore                    [iI][gG][nN][oO][rR][eE]                           
timing_model              [tT][iI][mM][iI][nN][gG]_[mM][oO][dD][eE][lL]                            
energy_model              [eE][nN][eE][rR][gG][yY]_[mM][oO][dD][eE][lL]                            
internal_energy           [iI][nN][tT][eE][rR][nN][aA][lL]_[eE][nN][eE][rR][gG][yY]
spline                    [sS][pP][lL][iI][nN][eE]                           
const                     [cC][oO][nN][sS][tT]                            
data                      [dD][aA][tT][aA]                          
input                     [iI][nN][pP][uU][tT]                            
output                    [oO][uU][tT][pP][uU][tT]                           
bidir                     [bB][iI][dD][iI][rR]                            
bus                       [bB][uU][sS]
bustype                   [bB][uU][sS][tT][yY][pP][eE]                         
clock_slew_axis           [cC][lL][oO][cC][kK]_[sS][lL][eE][wW]_[aA][xX][iI][sS]            
input_slew_axis           [iI][nN][pP][uU][tT]_[sS][lL][eE][wW]_[aA][xX][iI][sS]            
slew_axis                 [sS][lL][eE][wW]_[aA][xX][iI][sS]                  
load_axis                 [lL][oO][aA][dD]_[aA][xX][iI][sS]                  
load2_axis                [lL][oO][aA][dD][2]_[aA][xX][iI][sS]                  
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
supply                    [sS][uU][pP][pP][lL][yY]                     
pindir                    [pP][iI][nN][dD][iI][rR]                     
capacitance               [cC][aA][pP][aA][cC][iI][tT][aA][nN][cC][eE]                    
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
process_mult_model        [pP][rR][oO][cC][eE][sS][sS]_[mM][uU][lL][tT]_[mM][oO][dD][eE][lL]
voltage_mult_model        [vV][oO][lL][tT][aA][gG][eE]_[mM][uU][lL][tT]_[mM][oO][dD][eE][lL]
temperature_mult_model    [tT][eE][mM][pP][eE][rR][aA][tT][uU][rR][eE]_[mM][uU][lL][tT]_[mM][oO][dD][eE][lL]
proc_mult                 [pP][rR][oO][cC]_[mM][uU][lL][tT]
proc_mult_ienergy         [pP][rR][oO][cC]_[mM][uU][lL][tT]_[iI][eE][nN][eE][rR][gG][yY]
proc_mult_senergy         [pP][rR][oO][cC]_[mM][uU][lL][tT]_[sS][eE][nN][eE][rR][gG][yY]
proc_mult_cspower         [pP][rR][oO][cC]_[mM][uU][lL][tT]_[cC][sS][pP][oO][wW][eE][rR]
proc_mult_setup           [pP][rR][oO][cC]_[mM][uU][lL][tT]_[sS][eE][tT][uU][pP]            
proc_mult_hold            [pP][rR][oO][cC]_[mM][uU][lL][tT]_[hH][oO][lL][dD]             
proc_mult_mpwl            [pP][rR][oO][cC]_[mM][uU][lL][tT]_[mM][pP][wW][lL]             
proc_mult_mpwh            [pP][rR][oO][cC]_[mM][uU][lL][tT]_[mM][pP][wW][hH]             
proc_mult_recovery        [pP][rR][oO][cC]_[mM][uU][lL][tT]_[rR][eE][cC][oO][vV][eE][rR][yY] 
proc_mult_period          [pP][rR][oO][cC]_[mM][uU][lL][tT]_[pP][eE][rR][iI][oO][dD]           
proc_mult_propagation     [pP][rR][oO][cC]_[mM][uU][lL][tT]_[pP][rR][oO][pP][aA][gG][aA][tT][iI][oO][nN]      
proc_mult_transition      [pP][rR][oO][cC]_[mM][uU][lL][tT]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]       
proc_mult_net_res         [pP][rR][oO][cC]_[mM][uU][lL][tT]_[nN][eE][tT]_[rR][eE][sS]                       
proc_mult_net_cap         [pP][rR][oO][cC]_[mM][uU][lL][tT]_[nN][eE][tT]_[cC][aA][pP]                       
proc_mult_capacitance     [pP][rR][oO][cC]_[mM][uU][lL][tT]_[cC][aA][pP][aA][cC][iI][tT][aA][nN][cC][eE]                    
volt_mult                 [vV][oO][lL][tT]_[mM][uU][lL][tT]
volt_mult_cspower         [vV][oO][lL][tT]_[mM][uU][lL][tT]_[cC][sS][pP][oO][wW][eE][rR]
volt_mult_ienergy         [vV][oO][lL][tT]_[mM][uU][lL][tT]_[iI][eE][nN][eE][rR][gG][yY]
volt_mult_senergy         [vV][oO][lL][tT]_[mM][uU][lL][tT]_[sS][eE][nN][eE][rR][gG][yY]
volt_mult_setup           [vV][oO][lL][tT]_[mM][uU][lL][tT]_[sS][eE][tT][uU][pP]            
volt_mult_hold            [vV][oO][lL][tT]_[mM][uU][lL][tT]_[hH][oO][lL][dD]             
volt_mult_mpwl            [vV][oO][lL][tT]_[mM][uU][lL][tT]_[mM][pP][wW][lL]             
volt_mult_mpwh            [vV][oO][lL][tT]_[mM][uU][lL][tT]_[mM][pP][wW][hH]             
volt_mult_period          [vV][oO][lL][tT]_[mM][uU][lL][tT]_[pP][eE][rR][iI][oO][dD]           
volt_mult_propagation     [vV][oO][lL][tT]_[mM][uU][lL][tT]_[pP][rR][oO][pP][aA][gG][aA][tT][iI][oO][nN]      
volt_mult_transition      [vV][oO][lL][tT]_[mM][uU][lL][tT]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]       
volt_mult_recovery        [vV][oO][lL][tT]_[mM][uU][lL][tT]_[rR][eE][cC][oO][vV][eE][rR][yY] 
volt_mult_net_res         [vV][oO][lL][tT]_[mM][uU][lL][tT]_[nN][eE][tT]_[rR][eE][sS]                       
volt_mult_net_cap         [vV][oO][lL][tT]_[mM][uU][lL][tT]_[nN][eE][tT]_[cC][aA][pP]                       
volt_mult_capacitance     [vV][oO][lL][tT]_[mM][uU][lL][tT]_[cC][aA][pP][aA][cC][iI][tT][aA][nN][cC][eE]                    
temp_mult                 [tT][eE][mM][pP]_[mM][uU][lL][tT]
temp_mult_cspower         [tT][eE][mM][pP]_[mM][uU][lL][tT]_[cC][sS][pP][oO][wW][eE][rR]
temp_mult_ienergy         [tT][eE][mM][pP]_[mM][uU][lL][tT]_[iI][eE][nN][eE][rR][gG][yY]
temp_mult_senergy         [tT][eE][mM][pP]_[mM][uU][lL][tT]_[sS][eE][nN][eE][rR][gG][yY]
temp_mult_setup           [tT][eE][mM][pP]_[mM][uU][lL][tT]_[sS][eE][tT][uU][pP]            
temp_mult_hold            [tT][eE][mM][pP]_[mM][uU][lL][tT]_[hH][oO][lL][dD]             
temp_mult_mpwl            [tT][eE][mM][pP]_[mM][uU][lL][tT]_[mM][pP][wW][lL]             
temp_mult_mpwh            [tT][eE][mM][pP]_[mM][uU][lL][tT]_[mM][pP][wW][hH]             
temp_mult_period          [tT][eE][mM][pP]_[mM][uU][lL][tT]_[pP][eE][rR][iI][oO][dD]           
temp_mult_propagation     [tT][eE][mM][pP]_[mM][uU][lL][tT]_[pP][rR][oO][pP][aA][gG][aA][tT][iI][oO][nN]      
temp_mult_recovery        [tT][eE][mM][pP]_[mM][uU][lL][tT]_[rR][eE][cC][oO][vV][eE][rR][yY] 
temp_mult_transition      [tT][eE][mM][pP]_[mM][uU][lL][tT]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]       
temp_mult_net_res         [tT][eE][mM][pP]_[mM][uU][lL][tT]_[nN][eE][tT]_[rR][eE][sS]                
temp_mult_net_cap         [tT][eE][mM][pP]_[mM][uU][lL][tT]_[nN][eE][tT]_[cC][aA][pP]                       
temp_mult_capacitance     [tT][eE][mM][pP]_[mM][uU][lL][tT]_[cC][aA][pP][aA][cC][iI][tT][aA][nN][cC][eE]                    
table_input_threshold     [tT][aA][bB][lL][eE]_[iI][nN][pP][uU][tT]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]      
table_output_threshold    [tT][aA][bB][lL][eE]_[oO][uU][tT][pP][uU][tT]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]     
table_transition_start    [tT][aA][bB][lL][eE]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]_[sS][tT][aA][rR][tT]     
table_transition_end      [tT][aA][bB][lL][eE]_[tT][rR][aA][nN][sS][iI][tT][iI][oO][nN]_[eE][nN][dD]       
for_cell                  [fF][oO][rR]_[cC][eE][lL][lL]                   
proc_var                  [pP][rR][oO][cC]_[vV][aA][rR]                         
net_cap                   [nN][eE][tT]_[cC][aA][pP]                       
net_cap_model             [nN][eE][tT]_[cC][aA][pP]_[mM][oO][dD][eE][lL]                       
net_res                   [nN][eE][tT]_[rR][eE][sS]                       
net_res_model             [nN][eE][tT]_[rR][eE][sS]_[mM][oO][dD][eE][lL]                       
load_limit                [lL][oO][aA][dD]_[lL][iI][mM][iI][tT]                    
slew_limit                [sS][lL][eE][wW]_[lL][iI][mM][iI][tT]                    
temperature               [tT][eE][mM][pP][eE][rR][aA][tT][uU][rR][eE]                      
voltage                   [vV][oO][lL][tT][aA][gG][eE]                          
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
define_attribute          [dD][eE][fF][iI][nN][eE]_[aA][tT][tT][rR][iI][bB][uU][tT][eE]
properties                [pP][rR][oO][pP][eE][rR][tT][iI][eE][sS]
cell_spower               [cC][eE][lL][lL]_[sS][pP][oO][wW][eE][rR]
other_pins                [oO][tT][hH][eE][rR]_[pP][iI][nN][sS]
dont_use                  [dD][oO][nN][tT]_[uU][sS][eE]
rise                      [rR][iI][sS][eE]
fall                      [fF][aA][lL][lL]
clear_preset_var1         [cC][lL][eE][aA][rR]_[pP][rR][eE][sS][eE][tT]_[vV][aA][rR][1]
clear_preset_var2         [cC][lL][eE][aA][rR]_[pP][rR][eE][sS][eE][tT]_[vV][aA][rR][2]
clock                     [Cc][lL][oO][cC][kK]
clock_pin                 [Cc][lL][oO][cC][kK]_[pP][iI][nN]
drivetype                 [dD][rR][iI][vV][eE][tT][yY][pP][eE]                
dont_touch                [dD][oO][nN][tT]_[tT][oO][uU][cC][hh]              
sc_energy                 [sS][cC]_[eE][nN][eE][rR][gG][yY]              
scan_pintype              [sS][cC][aA][nN]_[pP][iI][nN][tT][yY][pP][eE]          
test_register             [tT][eE][sS][tT]_[rR][eE][gG][iI][sS][tT][eE][rR]        
internal                  [iI][nN][tT][eE][rR][nN][aA][lL]
unit                      [uU][nN][iI][tT]
pull                      [pP][uU][lL][lL]                  
inductance_unit           [iI][nN][dD][uU][cC][tT][aA][nN][cC][eE]_[uU][nN][iI][tT]           
area_unit                 [aA][rR][eE][aA]_[uU][nN][iI][tT]                 
cap_unit                  [cC][aA][pP]_[uU][nN][iI][tT]                  
conductance_unit          [cC][oO][nN][dD][uU][cC][tT][aA][nN][cC][eE]_[uU][nN][iI][tT]          
current_unit              [cC][uU][rR][rR][eE][nN][tT]_[uU][nN][iI][tT]              
res_unit                  [rR][eE][sS]_[uU][nN][iI][tT]                  
time_unit                 [tT][iI][mM][eE]_[uU][nN][iI][tT]                
temperature_unit          [tT][eE][mM][pP][eE][rR][aA][tT][uU][rR][eE]_[uU][nN][iI][tT]        
volt_unit                 [vV][oO][lL][tT]_[uU][nN][iI][tT]              
power_unit                [pP][oO][wW][eE][rR]_[uU][nN][iI][tT]             
wireload_by_area          [wW][iI][rR][eE][lL][oO][aA][dD]_[bB][yY]_[aA][rR][eE][aA]
insertion_delay           [iI][nN][sS][eE][rR][tT][iI][oO][nN]_[dD][eE][lL][aA][yY]
for_pin                   [fF][oO][rR]_[pP][iI][nN]
input_threshold_pct       [iI][nN][pP][uU][tT]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]_[pP][cC][tT]       
output_threshold_pct      [oO][uU][tT][pP][uU][tT]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]_[pP][cC][tT]      
slew_lower_threshold_pct  [sS][lL][eE][wW]_[lL][oO][wW][eE][rR]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]_[pP][cC][tT]  
slew_upper_threshold_pct  [sS][lL][eE][wW]_[uU][pP][pP][eE][rR]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]_[pP][cC][tT]  
slew_measure_lower_threshold_pct  [sS][lL][eE][wW]_[mM][eE][aA][sS][uU][rR][eE]_[lL][oO][wW][eE][rR]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]_[pP][cC][tT]  
slew_measure_upper_threshold_pct  [sS][lL][eE][wW]_[mM][eE][aA][sS][uU][rR][eE]_[uU][pP][pP][eE][rR]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]_[pP][cC][tT]  
pvt_conds                 [pP][vV][tT]_[cC][oO][nN][dD][sS]                 
default_pvt_conds         [dD][eE][fF][aA][uU][lL][tT]_[pP][vV][tT]_[cC][oO][nN][dD][sS]         
wireload                  [wW][iI][rR][eE][lL][oO][aA][dD]                  
default_wireload_mode     [dD][eE][fF][aA][uU][lL][tT]_[wW][iI][rR][eE][lL][oO][aA][dD]_[mM][oO][dD][eE]     
default_wireload_group    [dD][eE][fF][aA][uU][lL][tT]_[wW][iI][rR][eE][lL][oO][aA][dD]_[gG][rR][oO][uU][pP]    
input_voltage             [iI][nN][pP][uU][tT]_[vV][oO][lL][tT][aA][gG][eE]             
output_voltage            [oO][uU][tT][pP][uU][tT]_[vV][oO][lL][tT][aA][gG][eE]            
input_fanload             [iI][nN][pP][uU][tT]_[fF][aA][nN][lL][oO][aA][dD]             
fanout_limit              [fF][aA][nN][oO][uU][tT]_[lL][iI][mM][iI][tT]              
pad_props                 [pP][aA][dD]_[pP][rR][oO][pP][sS]                 
memory_props              [mM][eE][mM][oO][rR][yY]_[pP][rR][oO][pP][sS]                 
vdrop_limit               [vV][dD][rR][oO][pP]_[lL][iI][mM][iI][tT]                 
eq_cells                  [eE][qQ]_[cC][eE][lL][lL][sS]
memory_bus                [mM][eE][mM][oO][rR][yY]_[bB][uU][sS] 
period                    [pP][eE][rR][iI][oO][dD]           
state_table               [sS][tT][aA][tT][eE]_[tT][aA][bB][lL][eE]
state_function            [sS][tT][aA][tT][eE]_[fF][uU][nN][cC][tT][iI][oO][nN]
map_to_stpin              [mM][aA][pP]_[tT][oO]_[sS][tT][pP][iI][nN]
slave_clock               [sS][lL][aA][vV][eE]_[cC][lL][oO][cC][kK]
linear                    [lL][iI][nN][eE][aA][rR]
value                     [vV][aA][lL][uU][eE]
propagation_delay_table   [pP][rR][oO][pP][aA][gG][aA][tT][iI][oO][nN]_[dD][eE][lL][aA][yY]_[tT][aA][bB][lL][eE]













                        


%%
\n                          { TLF_LINE++ ;                                                      }
{comment}.*$                {                                                                   }
[ \t]                       {                                                                   }
<DONTCARE>\(                { parlevel++ ;                                                      }
<DONTCARE>[^\(\)\n]*        {                                                                   }
<DONTCARE>\n                { TLF_LINE++ ;                                                      }
<DONTCARE>\)                { parlevel-- ; if (!parlevel) BEGIN PREVIOUS ;                      }
\/\*                        { PREVIOUS = INITIAL ; BEGIN COMMENT ;                              }
<COMMENT>[^\*\n]*           {                                                                   }
<COMMENT>\*+                {                                                                   }
<COMMENT>\n                 { TLF_LINE++ ;                                                      }
<COMMENT>\*+\/              { BEGIN PREVIOUS ;                                                  }
\(                          { return ('(') ;                                                    }
\)                          { return (')') ;                                                    }
:                           { return (':') ;                                                    }
=>                          { return (TLF4_TOKEN_ARROW) ;                                                 }
\*>                         { return (TLF4_TOKEN_BUS_ARROW) ;                                             }
{cell}                      { if (!attribute) genmodel = 0 ; attribute = 0 ; return (TLF4_TOKEN_CELL) ;   }
{celltype}                  { return (TLF4_TOKEN_CELLTYPE) ;                                              }
{seq}                       { return (TLF4_TOKEN_SEQ) ;                                                   }
{comb}                      { return (TLF4_TOKEN_COMB) ;                                                  }
{bus}                       { return (TLF4_TOKEN_BUS) ;                                                   }
{internal}                  { return (TLF4_TOKEN_INTERNAL) ;                                              }
{ignore}                    { return (TLF4_TOKEN_IGNORE) ;                                                }
{incell}                    { return (TLF4_TOKEN_INCELL) ;                                                }
{outcell}                   { return (TLF4_TOKEN_OUTCELL) ;                                               }
{rise}                      { return (TLF4_TOKEN_RISE) ;                                                  }
{fall}                      { return (TLF4_TOKEN_FALL) ;                                                  }
{timing_model}              { if (genmodel)
                                return (TLF4_TOKEN_GENMODEL);
                              else
                                return (TLF4_TOKEN_MODEL) ;   
                            }
{dont_use}                  {                                                                   }
{dont_touch}                {                                                                   }
{clock_pin}                 { return (TLF4_TOKEN_CLOCK_PIN) ;                                             }
{internal_energy}           { return (TLF4_TOKEN_ENERGY) ;                                                }
{spline}                    { return (TLF4_TOKEN_SPLINE) ;                                                }
{const}                     { return (TLF4_TOKEN_CONST) ;                                                 }
{data}                      { return (TLF4_TOKEN_DATA) ;                                                  }
{input}                     { return (TLF4_TOKEN_INPUT) ;                                                 }
{output}                    { return (TLF4_TOKEN_OUTPUT) ;                                                }
{bidir}                     { return (TLF4_TOKEN_BIDIR) ;                                                 }
{clock_slew_axis}           { return (TLF4_TOKEN_CLOCK_SLEW_AXIS) ;                                       }
{input_slew_axis}           { return (TLF4_TOKEN_INPUT_SLEW_AXIS) ;                                       }
{slew_axis}                 { return (TLF4_TOKEN_SLEW_AXIS) ;                                             }
{load_axis}                 { return (TLF4_TOKEN_LOAD_AXIS) ;                                             }
{load2_axis}                { return (TLF4_TOKEN_LOAD2_AXIS) ;                                            }
{path}                      { return (TLF4_TOKEN_PATH) ;                                                  }
{path_extension}            { return (TLF4_TOKEN_PATH) ;                                                  }
{fast}                      { return (TLF4_TOKEN_FAST) ;                                                  }
{slow}                      { return (TLF4_TOKEN_SLOW) ;                                                  }
{delay}                     { return (TLF4_TOKEN_DELAY) ;                                                 }
{slew}                      { return (TLF4_TOKEN_SLEW) ;                                                  }
{t01}                       { return (TLF4_TOKEN_01) ;                                                    } 
{t10}                       { return (TLF4_TOKEN_10) ;                                                    }
{t0z}                       { return (TLF4_TOKEN_0Z) ;                                                    }
{t1z}                       { return (TLF4_TOKEN_1Z) ;                                                    }
{tz0}                       { return (TLF4_TOKEN_Z0) ;                                                    }
{tz1}                       { return (TLF4_TOKEN_Z1) ;                                                    }
{negedge}                   { return (TLF4_TOKEN_NEGEDGE) ;                                               }
{posedge}                   { return (TLF4_TOKEN_POSEDGE) ;                                               }
{setup}                     { return (TLF4_TOKEN_SETUP) ;                                                 }
{hold}                      { return (TLF4_TOKEN_HOLD) ;                                                  }
{pin}                       { attribute = 0 ; return (TLF4_TOKEN_PIN) ;                                   }
{pintype}                   { return (TLF4_TOKEN_PINTYPE) ;                                               }
{bustype}                   { return (TLF4_TOKEN_PINTYPE) ;                                               }
{ground}                    { return (TLF4_TOKEN_GROUND) ;                                                }
{supply}                    { return (TLF4_TOKEN_SUPPLY) ;                                                }
{pindir}                    { return (TLF4_TOKEN_PINDIR) ;                                                }
{capacitance}               { return (TLF4_TOKEN_CAPACITANCE) ;                                           }
{vdrop_limit}               { return (TLF4_TOKEN_VDROP_LIMIT) ;                                           }
{timing_props}              { return (TLF4_TOKEN_TIMING_PROPS) ;                                          }
{for_bits}                  { return (TLF4_TOKEN_FOR_BITS) ;                                              }
{vendor}                    { return (TLF4_TOKEN_VENDOR) ;                                                }
{environment}               { return (TLF4_TOKEN_ENVIRONMENT) ;                                           }
{version}                   { return (TLF4_TOKEN_VERSION) ;                                               }
{tlf_version}               { return (TLF4_TOKEN_TLF_VERSION) ;                                           }
{header}                    { genmodel = 1 ; return (TLF4_TOKEN_HEADER) ;                                 }
{date}                      { return (TLF4_TOKEN_DATE) ;                                                  }
{library}                   { attribute = 0 ; return (TLF4_TOKEN_LIBRARY) ;                               }
{technology}                { return (TLF4_TOKEN_TECHNOLOGY) ;                                            }
{control}                   { return (TLF4_TOKEN_CONTROL) ;                                               }
{tristate}                  { return (TLF4_TOKEN_TRISTATE) ;                                              }
{asynch}                    { return (TLF4_TOKEN_ASYNCH) ;                                                }
{low}                       { return (TLF4_TOKEN_LOW) ;                                                   }
{high}                      { return (TLF4_TOKEN_HIGH) ;                                                  }
{define_attribute}          { attribute = 1 ; return (TLF4_TOKEN_DEFINE_ATTRIBUTE) ;                      }
{sc_energy}                 { return (TLF4_TOKEN_ENERGY) ;                                                } 
{for_pin}                   { return (TLF4_TOKEN_FOR_PIN) ;                                               } 
{linear}                    { return (TLF4_TOKEN_LINEAR);                                                 } 
{value}                     { return (TLF4_TOKEN_VALUE);                                                  } 
"~"                         { return ('~');                                                     } 
{propagation_delay_table}   { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{period}                    { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{pull}                      { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{insertion_delay}           { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{input_fanload}             { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }  
{fanout_limit}              { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }  
{pvt_conds}                 { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{default_pvt_conds}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{wireload}                  { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{default_wireload_mode}     { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{default_wireload_group}    { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{wireload_by_area}          { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{input_voltage}             { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{output_voltage}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{test_register}             { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{scan_pintype}              { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{drivetype}                 { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{other_pins}                { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{pad_props}                 { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{memory_props}              { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{memory_bus}                { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{cell_spower}               { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{properties}                { return (TLF4_TOKEN_PROPERTIES) ;                                            } 
{unit}                      { return (TLF4_TOKEN_UNIT) ;                                                  }
{input_threshold_pct}       { return TLF4_TOKEN_INPUT_THRESHOLD;                                          }     
{output_threshold_pct}      { return TLF4_TOKEN_OUTPUT_THRESHOLD;                                         }   
{slew_lower_threshold_pct}  { return TLF4_TOKEN_LOWER_THRESHOLD;                                          }
{slew_upper_threshold_pct}  { return TLF4_TOKEN_UPPER_THRESHOLD;                                          }
{slew_measure_lower_threshold_pct}  { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;      }
{slew_measure_upper_threshold_pct}  { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;      }
{inductance_unit}           { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{area_unit}                 { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{cap_unit}                  { return (TLF4_TOKEN_CAP_UNIT) ;                                              }
{conductance_unit}          { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{current_unit}              { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{res_unit}                  { return (TLF4_TOKEN_RES_UNIT) ;                                              } 
{time_unit}                 { return (TLF4_TOKEN_TIME_UNIT) ;                                             }
{temperature_unit}          { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{volt_unit}                 { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{power_unit}                { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{energy_model}              {  if (genmodel)
                                return (TLF4_TOKEN_GENMODEL);     
                               else
                                parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;            }
{krc}                       { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{generated_by}              { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{proc_mult}                 { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_ienergy}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_senergy}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{proc_mult_cspower}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{proc_mult_setup}           { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_net_res}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_net_cap}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_capacitance}     { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{process_mult_model}        { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{voltage_mult_model}        { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temperature_mult_model}    { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_hold}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_mpwl}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_mpwh}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_period}          { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_propagation}     { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_transition}      { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_mult_recovery}        { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_setup}           { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult}                 { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_cspower}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{volt_mult_ienergy}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{volt_mult_senergy}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{volt_mult_hold}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_mpwl}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_mpwh}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_period}          { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_propagation}     { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_transition}      { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_recovery}        { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_net_res}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_net_cap}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{volt_mult_capacitance}     { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult}                 { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_cspower}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{temp_mult_ienergy}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{temp_mult_senergy}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              } 
{temp_mult_setup}           { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_hold}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_mpwl}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_mpwh}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_period}          { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_propagation}     { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_transition}      { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_recovery}        { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_net_res}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_net_cap}         { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temp_mult_capacitance}     { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{table_input_threshold}     { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{table_output_threshold}    { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{table_transition_start}    { return TLF4_TOKEN_TRANSITION_START;                                         }
{table_transition_end}      { return TLF4_TOKEN_TRANSITION_END;                                           } 
{for_cell}                  { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{proc_var}                  { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{net_cap}                   { return TLF4_TOKEN_NET_CAP;                                                  }
{net_cap_model}             { return TLF4_TOKEN_NET_CAP_MODEL;                                            }
{net_res}                   { return TLF4_TOKEN_NET_RES;                                                  }
{net_res_model}             { return TLF4_TOKEN_NET_RES_MODEL;                                            }
{load_limit}                { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{slew_limit}                { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{temperature}               { return TLF4_TOKEN_TEMPERATURE ;                                             }
{voltage}                   { return TLF4_TOKEN_VOLTAGE ;                                                 }
{area}                      { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{eq_cells}                  { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{cond}                      { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{sdf_cond}                  { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{mpwh}                      { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{mpwl}                      { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{recovery}                  { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{removal}                   { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{function}                  { exprlevel = 0; PREVIOUS = INITIAL ; BEGIN EXPR_LOGIC; return (TLF4_TOKEN_FUNCTION); }
<EXPR_LOGIC>\(              { exprlevel++  ;/* expression logique avec toutes les ()*/ 
                                  if(exprlevel == 1) strcpy(expr_tab,yytext);
                                  else if(exprlevel > 1)  strcat(expr_tab,yytext);              }
<EXPR_LOGIC>{string}        { if (exprlevel > 0) strcat(expr_tab,yytext);                       }
<EXPR_LOGIC>\)              { exprlevel--  ; strcat(expr_tab,yytext);
                                  if(!exprlevel) {
                                    strcat(expr_tab, "\0");
                                    tlf4lval.text = namealloc(expr_tab);
                                    BEGIN PREVIOUS ;
                                    return (TLF4_TOKEN_EXPR) ; }                                          }                
{enable}                    { exprlevel = 0 ; PREVIOUS = INITIAL  ; BEGIN EXPR_LOGIC ; return (TLF4_TOKEN_ENABLE);               }
{state_table}               { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{state_function}            { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{map_to_stpin}              { parlevel = 0 ; PREVIOUS = INITIAL ; BEGIN DONTCARE ;              }
{register}                  { reglevel = 0 ; BEGIN REGDEF ; return (TLF4_TOKEN_REGISTER) ;                }
{latch}                     { reglevel = 0 ; BEGIN REGDEF ; return (TLF4_TOKEN_LATCH) ;                   }
<REGDEF>\(                  { reglevel++   ; return ('(') ;                                     }
<REGDEF>{clear}             { exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF4_TOKEN_CLEAR);                }
<REGDEF>{set}               { exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF4_TOKEN_SET)  ;                }
<REGDEF>{clock}             { exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF4_TOKEN_CLOCK_REG);            }
<REGDEF>{slave_clock}       { exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF4_TOKEN_SLAVE_CLOCK);          }
<REGDEF>{input}             { exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF4_TOKEN_INPUT_REG);            }
<REGDEF>{clear_preset_var1} { exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF4_TOKEN_CLEAR_PRESET_VAR1);    } 
<REGDEF>{clear_preset_var2} { exprlevel = 0 ; PREVIOUS = REGDEF  ; BEGIN EXPR_LOGIC ; return (TLF4_TOKEN_CLEAR_PRESET_VAR2);    } 
<REGDEF>{output}            { return (TLF4_TOKEN_OUTPUT) ;                                                }
<REGDEF>{inverted_output}   { return (TLF4_TOKEN_INVERTED_OUTPUT) ;                                       }
<REGDEF>{ident}             { tlf4lval.text = namealloc (yytext) ; return (TLF4_TOKEN_IDENT) ;            }
<REGDEF>\n                  { TLF_LINE++ ;                                                      }
<REGDEF>\)                  { reglevel-- ; if (!reglevel) BEGIN INITIAL ; return (')') ;        }
<REGDEF>\/\*                { PREVIOUS = REGDEF ; BEGIN COMMENT ;                               }
<REGDEF>{comment}.*$        {                                                                   }
<REGDEF>[ \t]               {                                                                   }
{qstring}                   { tlf4lval.text = mbkstrdup (yytext) ; return (TLF4_TOKEN_QSTRING) ;             }
{ident}                     { 
                              str = namealloc (yytext) ;
                              if (is_attribute (str)) {
                                tlf4lval.text = str ; 
                                return (TLF4_TOKEN_ATTRIBUTE) ; 
                              } else {
                                tlf4lval.text = str ; 
                                return (TLF4_TOKEN_IDENT) ; 
                              }
                            }
{vect_ident}                { tlf4lval.text = mbkstrdup (yytext) ; return (TLF4_TOKEN_VECT_IDENT) ;           }
{vect}                      { tlf4lval.text = mbkstrdup (yytext) ; return (TLF4_TOKEN_VECT) ;                 }
{number}                    { tlf4lval.real = strtod (yytext, (char**)NULL) ; return (TLF4_TOKEN_NUMBER) ; }
{enumber}                   { tlf4lval.real = strtod (yytext, (char**)NULL) ; return (TLF4_TOKEN_ENUMBER) ;}

%%

int yywrap() { return 1 ; }
