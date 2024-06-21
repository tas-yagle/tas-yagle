%{
#include <string.h>
#include "y.tab.h"
#include "tbg.h"

#define ECHO 
#define YY_NO_UNPUT

%}
%x ENT 

upper_alpha      [A-Z]
lower_alpha      [a-z]
num 	         [0-9]
alpha   	     ({upper_alpha}|{lower_alpha})
alphanum         ({alpha}|{num})
number           {num}+
identifier       {alpha}({alphanum}|_)*
comment          \-\- 
is               [iI][sS]
port             [pP][oO][rR][tT]
generic          [gG][eE][nN][eE][rR][iI][cC]
to               [tT][oO]
downto           [dD][oO][wW][nN][tT][oO]
std_logic        [sS][tT][dD]_[lL][oO][gG][iI][cC]
std_logic_vector [sS][tT][dD]_[lL][oO][gG][iI][cC]_[vV][eE][cC][tT][oO][rR]
entity           [eE][nN][tT][iI][tT][yY]
end              [eE][nN][dD]
in               [iI][nN]
inout            [iI][nN][oO][uU][tT]
linkage          [lL][iI][Nn][kK][Aa][Gg][Ee]
out              [oO][uU][tT]
integer          [iI][nN][tT][eE][gG][eE][rR]

%%
\n                      { ECHO; LINE++; }
[ \t]                   { ECHO; }
{comment}.*$            { ECHO; }
{entity}                { ECHO; BEGIN ENT; return _ENTITY;}
[ \t{comment}].*$       { ECHO; }
<ENT>\n                 { ECHO; LINE++; }
<ENT>\+                 { ECHO; return '+'; } 
<ENT>-                  { ECHO; return '-'; } 
<ENT>\/                 { ECHO; return '/'; } 
<ENT>\*                 { ECHO; return '*'; } 
<ENT>\(                 { ECHO; return '('; }
<ENT>\)                 { ECHO; return ')'; }
<ENT>\;                 { ECHO; return ';'; }
<ENT>\,                 { ECHO; return ','; }
<ENT>\:                 { ECHO; return ':'; }
<ENT>[ \t]              { ECHO; }
<ENT>{comment}.*$       { ECHO; }
<ENT>{is}               { ECHO; return _IS;}
<ENT>{in}               { ECHO; return _IN;}
<ENT>{linkage}          { ECHO; return _IN;}
<ENT>{inout}            { ECHO; return _INOUT;}
<ENT>{out}              { ECHO; return _OUT;}
<ENT>{port}             { ECHO; return _PORT;}
<ENT>{generic}          { ECHO; return _GENERIC;}
<ENT>{to}               { ECHO; return _TO;}
<ENT>{downto}           { ECHO; return _DOWNTO;}
<ENT>{std_logic}        { ECHO; return _STD_LOGIC;}
<ENT>{std_logic_vector} { ECHO; return _STD_LOGIC_VECTOR;}
<ENT>{integer}          { ECHO; return _INTEGER;}
<ENT>{end}              { ECHO; BEGIN INITIAL; return _END;}
<ENT>{identifier}       { ECHO; vhdllval.t_pchar = strdup (yytext); return _IDENTIFIER; }
<ENT>{number}           { ECHO; vhdllval.t_long = atoi (yytext); return _NUMBER; }

%%

int yywrap() { return 1; }
