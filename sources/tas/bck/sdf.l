%{
#include "bck.h"
#include "sdf.tab.h"
#include "bck_sdf.h"

#define ECHO 
#define YY_NO_UNPUT

char *strip (char *name)
{
	int i, l ;
	char *buf ;
	
	i = l =  strlen (name) ;
	if ((name[l - 1] == ']') && (name[l - 2] != '\\')) {
		while (name[--i] != '[') 
			if (i == 0)
				return name ;
		name[i] = ' ' ;
		name[l - 1] = '\0' ;
	} 
	
	buf = (char*)mbkalloc (1024 * sizeof (char)) ;

	i = l = 0 ;
	while (name[i] != '\0') {
		if (name[i] != '\\')
			buf[l++] = name[i] ;
		i++ ;
	}
	buf[l] = '\0' ;

	return buf ;
}

extern int  LINE ;
ptype_list *ptype ;
ht         *CASEHT ;
char       *name ;

%}

upalpha                   [A-Z]
lowalpha                  [a-z]
num                       [0-9]
alpha                     ({upalpha}|{lowalpha})
alphanum                  ({alpha}|{num})
dnumber                   {num}+
number                    (("."{num}+)|({num}+("."{num}+)?))(e[+|-]?{num}+)?
rnumber                   -?(("."{num}+)|({num}+("."{num}+)?))(e[+|-]?{num}+)?
qstring                   \"[^\"\t\n]*\"
divider                   "."|"/"
ident                     ({alpha}|_)({alphanum}|_|(\\.))*("["{num}+"]")?|("["{num}+:{num}+"]")?
identifier                {ident}({divider}{ident})*

%x COMMENT

%%
"//".*\n                  { ECHO ;  LINE++ ;                               }
"/*"                      { ECHO ;  BEGIN COMMENT ;                        }                       
<COMMENT>[^*\n]*          { ECHO ;                                         }
<COMMENT>"*"+[^*/\n]*     { ECHO ;                                         }
<COMMENT>\n               { ECHO ;  LINE++ ;                               }
<COMMENT>"*"+"/"          { ECHO ;  BEGIN INITIAL ;                        }
\n                        { ECHO ;  LINE++ ;                               }
[ \t]                     { ECHO ;                                         }
DELAYFILE                 { ECHO ; return (_DELAYFILE) ;                   }
\([ ]*SDFVERSION          { ECHO ; return (_SDFVERSION) ;                  }
\([ ]*DESIGN              { ECHO ; return (_DESIGN) ;                      }
\([ ]*DATE                { ECHO ; return (_DATE) ;                        }
\([ ]*VENDOR              { ECHO ; return (_VENDOR) ;                      }
\([ ]*PROGRAM             { ECHO ; return (_PROGRAM) ;                     }
\([ ]*VERSION             { ECHO ; return (_VERSION) ;                     }
\([ ]*DIVIDER             { ECHO ; return (_DIVIDER) ;                     }
\([ ]*VOLTAGE             { ECHO ; return (_VOLTAGE) ;                     }
\([ ]*PROCESS             { ECHO ; return (_PROCESS) ;                     }
\([ ]*TEMPERATURE         { ECHO ; return (_TEMPERATURE) ;                 }
\([ ]*TIMESCALE           { ECHO ; return (_TIMESCALE) ;                   }
CELL                      { ECHO ; return (_CELL) ;                        }
CELLTYPE                  { ECHO ; return (_CELLTYPE) ;                    }
INSTANCE                  { ECHO ; return (_INSTANCE) ;                    }
DELAY                     { ECHO ; return (_DELAY) ;                       }
TIMINGCHECK               { ECHO ; return (_TIMINGCHECK) ;                 }
TIMINGENV                 { ECHO ; return (_TIMINGENV) ;                   }
PATHPULSE                 { ECHO ; return (_PATHPULSE) ;                   }
PATHPULSEPERCENT          { ECHO ; return (_PATHPULSEPERCENT) ;            }
ABSOLUTE                  { ECHO ; return (_ABSOLUTE) ;                    }
INCREMENT                 { ECHO ; return (_INCREMENT) ;                   }
IOPATH                    { ECHO ; return (_IOPATH) ;                      }
RETAIN                    { ECHO ; return (_RETAIN) ;                      }
COND                      { ECHO ; return (_COND) ;                        }
CONDELSE                  { ECHO ; return (_CONDELSE) ;                    }
PORT                      { ECHO ; return (_PORT) ;                        }
INTERCONNECT              { ECHO ; return (_INTERCONNECT) ;                }
DEVICE                    { ECHO ; return (_DEVICE) ;                      }
SETUP                     { ECHO ; return (_SETUP) ;                       }
HOLD                      { ECHO ; return (_HOLD) ;                        }
SETUPHOLD                 { ECHO ; return (_SETUPHOLD) ;                   }
RECOVERY                  { ECHO ; return (_RECOVERY) ;                    }
REMOVAL                   { ECHO ; return (_REMOVAL) ;                     }
RECREM                    { ECHO ; return (_RECREM) ;                      }
SKEW                      { ECHO ; return (_SKEW) ;                        }
WIDTH                     { ECHO ; return (_WIDTH) ;                       }
PERIOD                    { ECHO ; return (_PERIOD) ;                      }
NOCHANGE                  { ECHO ; return (_NOCHANGE) ;                    }
\([ ]*SCOND               { ECHO ; return (_SCOND) ;                       }
\([ ]*CCOND               { ECHO ; return (_CCOND) ;                       }
NAME                      { ECHO ; return (_NAME) ;                        }
EXCEPTION                 { ECHO ; return (_EXCEPTION) ;                   }
PATHCONSTRAINT            { ECHO ; return (_PATHCONSTRAINT) ;              }
PERIODCONSTRAINT          { ECHO ; return (_PERIODCONSTRAINT) ;            }
SUM                       { ECHO ; return (_SUM) ;                         }
DIFF                      { ECHO ; return (_DIFF ) ;                       }
SKEWCONSTRAINT            { ECHO ; return (_SKEWCONSTRAINT) ;              }
ARRIVAL                   { ECHO ; return (_ARRIVAL) ;                     }
DEPARTURE                 { ECHO ; return (_DEPARTURE) ;                   }
SLACK                     { ECHO ; return (_SLACK) ;                       }
WAVEFORM                  { ECHO ; return (_WAVEFORM) ;                    }
negedge                   { ECHO ; return (_NEGEDGE) ;                     }
posedge                   { ECHO ; return (_POSEDGE) ;                     }
us                        { ECHO ; return (_US) ;                          }
ns                        { ECHO ; return (_NS) ;                          }
ps                        { ECHO ; return (_PS) ;                          }
01                        { ECHO ; return (_01) ;                          }
10                        { ECHO ; return (_10) ;                          }
0z                        { ECHO ; return (_0Z) ;                          }
z1                        { ECHO ; return (_Z1) ;                          }
1z                        { ECHO ; return (_1Z) ;                          }
z0                        { ECHO ; return (_Z0) ;                          }
1'b0                      { ECHO ; return (_B0) ;                          }
1'b1                      { ECHO ; return (_B1) ;                          }
1'B0                      { ECHO ; return (_B0) ;                          }
1'B1                      { ECHO ; return (_B1) ;                          }
'b0                       { ECHO ; return (_B0) ;                          }
'b1                       { ECHO ; return (_B1) ;                          }
'B0                       { ECHO ; return (_B0) ;                          }
'B1                       { ECHO ; return (_B1) ;                          }
"?"                       { ECHO ; return ('?') ;                          }     
":"                       { ECHO ; return (':') ;                          }     
"("                       { ECHO ; return ('(') ;                          }     
")"                       { ECHO ; return (')') ;                          }     
"+"                       { ECHO ; return ('+') ;                          }     
"-"                       { ECHO ; return ('-') ;                          }     
"!"                       { ECHO ; return ('!') ;                          }     
"~"                       { ECHO ; return ('~') ;                          }     
"&"                       { ECHO ; return ('&') ;                          }     
"|"                       { ECHO ; return ('|') ;                          }     
"^"                       { ECHO ; return ('^') ;                          }      
"*"                       { ECHO ; return ('*') ;                          }     
"/"                       { ECHO ; return ('/') ;                          }     
"%"                       { ECHO ; return ('%') ;                          }     
"="                       { ECHO ; return ('=') ;                          }     
"<"                       { ECHO ; return ('<') ;                          }     
">"                       { ECHO ; return ('>') ;                          }     
"["                       { ECHO ; return ('[') ;                          }     
"]"                       { ECHO ; return (']') ;                          }     
"."                       { ECHO ; return ('.') ;                          }     
"~&"                      { ECHO ; return (_SYMBOLIC0) ;                   }  
"~|"                      { ECHO ; return (_SYMBOLIC1) ;                   }  
"^~"                      { ECHO ; return (_SYMBOLIC2) ;                   }  
"~^"                      { ECHO ; return (_SYMBOLIC3) ;                   }  
"=="                      { ECHO ; return (_SYMBOLIC4) ;                   }  
"!="                      { ECHO ; return (_SYMBOLIC5) ;                   }  
"==="                     { ECHO ; return (_SYMBOLIC6) ;                   }  
"!=="                     { ECHO ; return (_SYMBOLIC7) ;                   }  
"&&"                      { ECHO ; return (_SYMBOLIC8) ;                   }  
"||"                      { ECHO ; return (_SYMBOLIC9) ;                   }  
"<="                      { ECHO ; return (_SYMBOLIC10) ;                  }  
">="                      { ECHO ; return (_SYMBOLIC11) ;                  }  
">>"                      { ECHO ; return (_SYMBOLIC12) ;                  }  
"<<"                      { ECHO ; return (_SYMBOLIC13) ;                  }  
{qstring}                 { ECHO ; sdflval.text = strdup (yytext) ; return (_QSTRING) ;        }
{dnumber}                 { ECHO ; sdflval.real = strtod (yytext, (char**)NULL) ; return (_NUMBER) ; }
{number}                  { ECHO ; sdflval.real = strtod (yytext, (char**)NULL) ; return (_NUMBER) ; }
{rnumber}                 { ECHO ; sdflval.real = strtod (yytext, (char**)NULL) ; return (_NUMBER) ; }
{identifier}              { ECHO ; 
                                   name = strip (yytext) ;
                                   sdflval.text = namealloc (name) ; 
								   mbkfree(name) ;
								   return (_IDENTIFIER) ;
					   	  }

%%

int yywrap () { return 1 ; }
