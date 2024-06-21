%{
#include VCD_H
#include "vcd.tab.h"
#include "vcd_parse.h"

#define ECHO  
#define YY_NO_UNPUT

%}

upalpha  [A-Z]
lowalpha [a-z]
num      [0-9]
alpha    ({upalpha}|{lowalpha})
alphanum ({alpha}|{num})
number   {num}+
ident    {alpha}({alphanum}|_)*
range    \[{number}\:{number}\]
index    \[{number}\]
string   [^ \t\n]+
line     [^\n]*

%x SCOPE
%x DC
%x LN
%s TS
%x DO

%%
<TS>\$end        { ECHO; BEGIN INITIAL; return _END;                      }
<DC>\$end        { ECHO; BEGIN INITIAL;                                   }
<DC>[^\n]*\$end  { ECHO; BEGIN INITIAL;                                   }
<SCOPE>[^\n]*\$end  { ECHO; BEGIN INITIAL; vcdlval.txt = strdup (yytext); return _LINE; }
<DC>[^\n]*       { ECHO;                                                  }
<DC>\n           { ECHO; vcd_line++;                                      }
\n               { ECHO; vcd_line++;                                      }
[ \t]*           { ECHO;                                                  }
\$date           { ECHO; BEGIN DC;                                        } 
\$version        { ECHO; BEGIN DC;                                        }
\$comment        { ECHO; BEGIN DC;                                        }
\$scope          { ECHO; BEGIN SCOPE; return _SCOPE;                      }
\$upscope        { ECHO; BEGIN DC;                                        }
\$enddefinitions { ECHO; BEGIN DC;                                        }
\$end            { ECHO; return _END;                                     }
\$timescale      { ECHO; BEGIN TS; return _TIMESCALE;                     }
\$var            { ECHO; return _VAR;                                     }
\$dumpvars       { ECHO; BEGIN LN;                                        }
^#.*$            { ECHO; BEGIN LN; vcdlval.txt = strdup (yytext); return _LINE; }
{range}          { ECHO; vcdlval.txt = strdup (yytext); return _RANGE;    }
{index}          { ECHO; vcdlval.txt = strdup (yytext); return _INDEX;    }
{string}         { ECHO; vcdlval.txt = strdup (yytext); return _STRING;   }
<LN>\$end        { ECHO;                                                  }
<LN>\$dumpvars   { ECHO;                                                  }
<LN>\$dumpon     { ECHO; BEGIN DO;                                        }
<LN>\$dumpoff    { ECHO; BEGIN DO;                                        }
<LN>\$dumpall    { ECHO; BEGIN DO;                                        }
<LN>{line}       { ECHO; vcdlval.txt = strdup (yytext); return _LINE;     } 
<LN>[\n]         { ECHO; vcd_line++;                                      }
<LN><<EOF>>      { ECHO; BEGIN INITIAL;                                   }
<DO>\$end        { ECHO; BEGIN LN;                                        }
<DO>[^\n]*\$end  { ECHO; BEGIN LN;                                        }
<DO>[^\n]*       { ECHO;                                                  }
<DO>\n           { ECHO; vcd_line++;                                      }
%%

int yywrap () { return 1; }
