%{
#include <string.h>
#include MUT_H

#define API_HIDE_TOKENS

#include API_H
#include "stmmodel.tab.h"
extern int STM_LINE;
static int        parlevel;

#define ECHO /*fprintf(stdout,"\ntoken %s line %d\n",yytext, STM_LINE)*/
#define YY_NO_UNPUT

%}
%x DONTCARE 
%x COMMENT 

upper_alpha  [A-Z]
lower_alpha  [a-z]
num 	     [0-9]
alpha   	 ({upper_alpha}|{lower_alpha})
alphanum     ({alpha}|{num})
number       -?{num}+(\.({num}*))?
enumber      -?{num}+(\.({num}*))?(e|E)[+|-]?{num}+
identifier   {alpha}(([\[|\\./:$_\-|!+@{}\#\]<>]?)+{alphanum}((\[|\\\[|$){num}+(\]|\\\]|$))?)*
comment      \/\/

%%
\n                       { ECHO; STM_LINE++; }
{comment}.*$             { ECHO; }
[ \t]                    { ECHO; }
<DONTCARE>\(             { ECHO; parlevel++; }
<DONTCARE>[^\(\)\n]*     { ECHO; }
<DONTCARE>\n             { ECHO; STM_LINE++; }
<DONTCARE>\)   		     { ECHO; parlevel--; if (!parlevel) BEGIN INITIAL; }
\/\*                     { ECHO; BEGIN COMMENT; }
<COMMENT>[^\*\n]*        { ECHO; }
<COMMENT>\*+             { ECHO; }
<COMMENT>\n              { ECHO; STM_LINE++;}
<COMMENT>\*+\/           { ECHO; BEGIN INITIAL; }
\(    	                 { ECHO; return '('; }
\)   				     { ECHO; return ')'; }
unknown           	     { ECHO; return (_UNKNOWN); }
model              	     { ECHO; return (_MODEL); }
name               	     { ECHO; return (_NAME); }
vdd               	     { ECHO; return (_VDD); }
vth               	     { ECHO; return (_VTH); }
varthmin               	 { ECHO; return (_VARTHMIN); }
varthmax               	 { ECHO; return (_VARTHMAX); }
vt               	     { ECHO; return (_VT); }
vf               	     { ECHO; return (_VF); }
tran               	     { ECHO; return (_TRAN); }
spline             	     { ECHO; return (_SPLINE); }
scm_dual           	     { ECHO; return (_SCM_DUAL); }
scm_good          	     { ECHO; return (_SCM_GOOD); }
scm_false          	     { ECHO; return (_SCM_FALSE); }
scm_path          	     { ECHO; return (_SCM_PATH); }
scm_cst          	     { ECHO; return (_SCM_CST); }
dual               	     { ECHO; return (_DUAL); }
false            	     { ECHO; return (_FALSE); }
path            	     { ECHO; return (_PATH); }
link_out          	     { ECHO; return (_LINK_OUT); }
link_pass                { ECHO; return (_LINK_PASS); }
link_dual                { ECHO; return (_LINK_DUAL); }
pconf0                   { ECHO; return (_PCONF0); }
pconf1                   { ECHO; return (_PCONF1); }
capai                    { ECHO; return (_CAPAI); }
capai0                   { ECHO; return (_CAPAI0); }
capai1                   { ECHO; return (_CAPAI1); }
capai2                   { ECHO; return (_CAPAI2); }
capai3                   { ECHO; return (_CAPAI3); }
vt0                      { ECHO; return (_VT0); }
vt0c                     { ECHO; return (_VT0C); }
irap                     { ECHO; return (_IRAP); }
vddin                    { ECHO; return (_VDDIN); }
threshold                { ECHO; return (_THRESHOLD); }
ithr                     { ECHO; return (_INPUT_THR); }
rint                     { ECHO; return (_RINT); }
vint                     { ECHO; return (_VINT); }
chalf                    { ECHO; return (_CHALF); }
qinit                    { ECHO; return (_QINIT); }
rconf                    { ECHO; return (_RCONF); }
kr                       { ECHO; return (_KR); }
imax                     { ECHO; return (_IMAX); }
an                       { ECHO; return (_AN); }
bn                       { ECHO; return (_BN); }
acti                     { ECHO; return (_ACTI); }
bcti                     { ECHO; return (_BCTI); }
ci                       { ECHO; return (_CI); }
cf                       { ECHO; return (_CF); }
k3                       { ECHO; return (_K3); }
k4                       { ECHO; return (_K4); }
k5                       { ECHO; return (_K5); }
rtot                     { ECHO; return (_RTOT); }
kf                       { ECHO; return (_KF); }
vddmax                   { ECHO; return (_VDDMAX); }
vf_input                 { ECHO; return (_VF_INPUT); }
rsat                     { ECHO; return (_RSAT); }
rlin                     { ECHO; return (_RLIN); }
drc                      { ECHO; return (_DRC); }
rbr                      { ECHO; return (_RBR); }
cbr                      { ECHO; return (_CBR); }
const             	     { ECHO; return (_CST); }
slew                     { ECHO; return (_SLEW); }
delay                    { ECHO; return (_DELAY); }
data                     { ECHO; return (_DATA); }
clock_slew_axis          { ECHO; return (_CLOCK_SLEW_AXIS); }
input_slew_axis          { ECHO; return (_INPUT_SLEW_AXIS); }
clock_slope_axis         { ECHO; return (_CLOCK_SLEW_AXIS); }
input_slope_axis         { ECHO; return (_INPUT_SLEW_AXIS); }
slew_axis                { ECHO; return (_SLEW_AXIS); }
load_axis                { ECHO; return (_LOAD_AXIS); }
orders                   { ECHO; return (_ORDERS); }
coefs                    { ECHO; return (_COEFS); }
spdm                     { ECHO; return (_SPDM); }
template                 { ECHO; parlevel = 0; BEGIN DONTCARE; }
noise_scr                { ECHO; return (_NOISE_SCR); }
energy_table             { ECHO; return (_ENERGY_TABLE); }
energy_const             { ECHO; return (_ENERGY_CONST); }
energy_model             { ECHO; return (_ENERGY_MODEL); }
fct                      { ECHO; return (_FCT); }
file                     { ECHO; return (_FILE); }
instance                 { ECHO; return (_INSTANCE); }
localname                { ECHO; return (_LOCALNAME); }
sim_model                { ECHO; return (_SIM_MODEL); }
function                 { ECHO; return (_FUNCTION); }
iv                       { ECHO; return (_IV); }
nve                      { ECHO; return (_NVE); }
nvs                      { ECHO; return (_NVS); }
ve                       { ECHO; return (_VE); }
vs                       { ECHO; return (_VS); }
conf                     { ECHO; return (_CONFLICT); }
current                  { ECHO; return (_CURRENT); }
input                    { ECHO; return (_INPUT); }
initial                  { ECHO; return (_INITIAL); }
{identifier}             { ECHO; stmmodellval.t_pchar = mbkstrdup (yytext); return (_IDENTIFIER); }
{number}                 { ECHO; stmmodellval.t_struct3.t_float = strtod (yytext, (char**)NULL); strcpy(stmmodellval.t_struct3.t_float_string, yytext); return (_NUMBER); }
{enumber}                { ECHO; stmmodellval.t_struct3.t_float = strtod (yytext, (char**)NULL);  strcpy(stmmodellval.t_struct3.t_float_string, yytext); return (_ENUMBER);}
'.'                      { ECHO; stmmodellval.t_char=yytext[1]; return (_ECHAR); }
%%

int yywrap() { return 1; }
