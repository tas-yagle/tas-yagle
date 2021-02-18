%{
#include <string.h>
#include "gcf.tab.h"

extern int GCF_LINE ;
static int        parlevel ;

#define YY_NO_UNPUT

%}
%x DONTCARE 

upper_alpha  [A-Z]
lower_alpha  [a-z]
num          [0-9]
alpha        ({upper_alpha}|{lower_alpha})
alphanum     ({alpha}|{num})
number       -?{num}+(\.({num}*))?
enumber      -?{num}+(\.({num}*))?(e|E)[+|-]?{num}+
identifier   {alpha}(([[./:_]?)+{alphanum}((\[|$){num}+(\]|$))?)*
qidentifier  \"{identifier}\"
qstring      \"[^\"]*\"
star         \*

%%
\n                             { GCF_LINE++ ;                                    }
[ \t]                          {                                                 }
<DONTCARE>\(                   { parlevel++ ;                                    }
<DONTCARE>[^\(\)\n]*           {                                                 }
<DONTCARE>\n                   { GCF_LINE++ ;                                    }
<DONTCARE>\)                   { parlevel-- ; if (!parlevel) BEGIN INITIAL ;     }
\(                             { return (GCF_TOKEN_LEFTPAR) ;                             }
\)                             { return (GCF_TOKEN_RIGHTPAR) ;                            }
GCF                            { return (GCF_TOKEN_GCF) ;                                 }
HEADER                         { return (GCF_TOKEN_HEADER) ;                              }
VERSION                        { return (GCF_TOKEN_VERSION) ;                             }
DESIGN                         { return (GCF_TOKEN_DESIGN) ;                              }
DATE                           { return (GCF_TOKEN_DATE) ;                                }
DELIMITERS                     { return (GCF_TOKEN_DELIM) ;                               }
TIME_SCALE                     { return (GCF_TOKEN_TSCALE) ;                              }
CAP_SCALE                      { return (GCF_TOKEN_CSCALE) ;                              }
RES_SCALE                      { return (GCF_TOKEN_RSCALE) ;                              }
VOLTAGE_SCALE                  { return (GCF_TOKEN_VSCALE) ;                              }
SUBSET[ ]TIMING                { return (GCF_TOKEN_SUBSET_TIMING) ;                       }
GLOBALS_SUBSET[ ]TIMING        { return (GCF_TOKEN_GLOBAL_SUBSET_TIMING) ;                }
GLOBALS                        { return (GCF_TOKEN_GLOBALS) ;                             }
WAVEFORM                       { return (GCF_TOKEN_WAVEFORM) ;                            }
POSEDGE                        { return (GCF_TOKEN_POSEDGE) ;                             }
NEGEDGE                        { return (GCF_TOKEN_NEGEDGE) ;                             }
CELL                           { return (GCF_TOKEN_CELL) ;                                }
ENVIRONMENT                    { return (GCF_TOKEN_ENVIRONMENT) ;                         }
DEPARTURE                      { return (GCF_TOKEN_DEPARTURE) ;                           }
ARRIVAL                        { return (GCF_TOKEN_ARRIVAL) ;                             }
CLOCK                          { return (GCF_TOKEN_CLOCK) ;                               }
EXCEPTIONS                     { return (GCF_TOKEN_EXCEPTIONS) ;                          }
DISABLE                        { return (GCF_TOKEN_DISABLE) ;                             }
FROM                           { return (GCF_TOKEN_FROM) ;                                }
TO                             { return (GCF_TOKEN_TO) ;                                  }
\(PROGRAM                      { parlevel = 1 ; BEGIN DONTCARE ;                 }
\(MAX_TRANSITION_TIME          { parlevel = 1 ; BEGIN DONTCARE ;                 }
\(GLOBALS_SUBSET[ ]ENVIRONMENT { parlevel = 1 ; BEGIN DONTCARE ;                 }
\(SUBSET[ ]PARASITICS          { parlevel = 1 ; BEGIN DONTCARE ;                 }
\(LEVEL                        { parlevel = 1 ; BEGIN DONTCARE ;                 }
{star}                         { return (GCF_TOKEN_STAR) ;                                }
{identifier}                   { gcflval.text = strdup (yytext) ; 
                                 return (GCF_TOKEN_IDENT) ;                               }
{qidentifier}                  { gcflval.text = strdup (yytext) ; 
                                 return (GCF_TOKEN_QIDENT) ;                              }
{number}                       { gcflval.real = strtod (yytext, (char**)NULL) ; 
                                 return (GCF_TOKEN_NUMBER) ;                              }
{enumber}                      { gcflval.real = strtod (yytext, (char**)NULL) ; 
                                 return (GCF_TOKEN_ENUMBER) ;                             }
{qstring}                      { gcflval.text = strdup (yytext) ; 
                                 return (GCF_TOKEN_QSTRING) ;                             }

%%

int yywrap() { return 1 ; }
