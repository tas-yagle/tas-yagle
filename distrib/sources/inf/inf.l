%{
#include<string.h>
#include MUT_H
#include INF_H
#include"inf.tab.h"
#undef ECHO
#define ECHO {}
//#define ECHO fprintf(stdout,"{%s %d} ",yytext,*yytext);
#define YY_NO_UNPUT

extern int infParseLine  ;

void stripquotes(char *, char *);

static long search (char *key)
{
    static ht *pt_hash = NULL;
    static NameAllocator na;
    if (pt_hash == NULL) {
        pt_hash = addht (100);
        CreateNameAllocator(127, &na, 'n');
        addhtitem (pt_hash, NameAlloc(&na, "begin"), T_BEGIN);
        addhtitem (pt_hash, NameAlloc(&na, "clock"), T_CLOCK);
        addhtitem (pt_hash, NameAlloc(&na, "connectors"), T_CONNECTORS);
        addhtitem (pt_hash, NameAlloc(&na, "groups"), T_GROUPS);
        addhtitem (pt_hash, NameAlloc(&na, "asynchronous"), T_ASYNCHRONOUS);
        addhtitem (pt_hash, NameAlloc(&na, "equivalent"), T_EQUIVALENT);
        addhtitem (pt_hash, NameAlloc(&na, "disable"), T_DISABLE);
        addhtitem (pt_hash, NameAlloc(&na, "stable"), T_STABLE);
        addhtitem (pt_hash, NameAlloc(&na, "down"), T_DOWN);
//        addhtitem (pt_hash, NameAlloc(&na, "default"), T_DEFAULT);
        addhtitem (pt_hash, NameAlloc(&na, "end"), T_END);
        addhtitem (pt_hash, NameAlloc(&na, "start"), T_START);
        addhtitem (pt_hash, NameAlloc(&na, "holdtime"), T_HOLDTIME);
        addhtitem (pt_hash, NameAlloc(&na, "input"), T_INPUT);
//        addhtitem (pt_hash, NameAlloc(&na, "name"), T_NAME);
        addhtitem (pt_hash, NameAlloc(&na, "stability"), T_STABILITY);
        addhtitem (pt_hash, NameAlloc(&na, "output"), T_OUTPUT);
        addhtitem (pt_hash, NameAlloc(&na, "period"), T_PERIOD);
        addhtitem (pt_hash, NameAlloc(&na, "setuptime"), T_SETUPTIME);
        addhtitem (pt_hash, NameAlloc(&na, "version"), T_VERSION);
        addhtitem (pt_hash, NameAlloc(&na, "unstable"), T_UNSTABLE);
        addhtitem (pt_hash, NameAlloc(&na, "verify"), T_VERIFY);
        addhtitem (pt_hash, NameAlloc(&na, "specify"), T_SPECIFY);
        addhtitem (pt_hash, NameAlloc(&na, "up"), T_UP);
        addhtitem (pt_hash, NameAlloc(&na, "down"), T_DOWN);
        addhtitem (pt_hash, NameAlloc(&na, "void"), T_VOID);
        addhtitem (pt_hash, NameAlloc(&na, "noverif"), T_NOVERIF);
        addhtitem (pt_hash, NameAlloc(&na, "rising"), T_RISING);
        addhtitem (pt_hash, NameAlloc(&na, "falling"), T_FALLING);
        addhtitem (pt_hash, NameAlloc(&na, "from"), T_FROM);
        addhtitem (pt_hash, NameAlloc(&na, "for"), T_FOR);
        addhtitem (pt_hash, NameAlloc(&na, "to"), T_TO);
        addhtitem (pt_hash, NameAlloc(&na, "internal"), T_INTERNAL);
        addhtitem (pt_hash, NameAlloc(&na, "nodes"), T_NODES);
        addhtitem (pt_hash, NameAlloc(&na, "after"), T_AFTER);
        addhtitem (pt_hash, NameAlloc(&na, "before"), T_BEFORE);
        addhtitem (pt_hash, NameAlloc(&na, "conditioned"), T_CONDITIONED);
        addhtitem (pt_hash, NameAlloc(&na, "command"), T_COMMAND);
        addhtitem (pt_hash, NameAlloc(&na, "states"), T_STATES);
        addhtitem (pt_hash, NameAlloc(&na, "without"), T_WITHOUT);
        addhtitem (pt_hash, NameAlloc(&na, "precharge"), T_PRECHARGE);
        addhtitem (pt_hash, NameAlloc(&na, "modelloop"), T_MODELLOOP);
        addhtitem (pt_hash, NameAlloc(&na, "evaluate"), T_EVALUATE);
        addhtitem (pt_hash, NameAlloc(&na, "multiple"), T_MULTIPLE);
        addhtitem (pt_hash, NameAlloc(&na, "priority"), T_PRIORITY);
        addhtitem (pt_hash, NameAlloc(&na, "memory"), T_MEMORY);

        addhtitem (pt_hash, NameAlloc(&na, "rename"), T_RENAME);
        addhtitem (pt_hash, NameAlloc(&na, "constraint"), T_CONSTRAINT);
        addhtitem (pt_hash, NameAlloc(&na, "sensitive"), T_SENSITIVE);
        addhtitem (pt_hash, NameAlloc(&na, "suppress"), T_SUPPRESS);
        addhtitem (pt_hash, NameAlloc(&na, "dirout"), T_DIROUT);
        addhtitem (pt_hash, NameAlloc(&na, "dlatch"), T_DLATCH);
        addhtitem (pt_hash, NameAlloc(&na, "bypass"), T_BYPASS);
        addhtitem (pt_hash, NameAlloc(&na, "flipflop"), T_FLIPFLOP);
        addhtitem (pt_hash, NameAlloc(&na, "marksig"), T_MARKSIG);
        addhtitem (pt_hash, NameAlloc(&na, "marktrans"), T_MARKTRANS);

        addhtitem (pt_hash, NameAlloc(&na, "temp"), T_TEMP);
        addhtitem (pt_hash, NameAlloc(&na, "supply"), T_SUPPLY);
        addhtitem (pt_hash, NameAlloc(&na, "inter"), T_INTER);
        addhtitem (pt_hash, NameAlloc(&na, "break"), T_BREAK);
        addhtitem (pt_hash, NameAlloc(&na, "stop"), T_STOP);
        addhtitem (pt_hash, NameAlloc(&na, "pathin"), T_PATHIN);
        addhtitem (pt_hash, NameAlloc(&na, "pathout"), T_PATHOUT);
        addhtitem (pt_hash, NameAlloc(&na, "pathsigs"), T_PATHSIGS);

        addhtitem (pt_hash, NameAlloc(&na, "rc"), T_RC);
        addhtitem (pt_hash, NameAlloc(&na, "norc"), T_NORC);
//        addhtitem (pt_hash, NameAlloc(&na, "elm"), T_ELM);
//        addhtitem (pt_hash, NameAlloc(&na, "rcn"), T_RCN);
        addhtitem (pt_hash, NameAlloc(&na, "inout"), T_INOUT);
        addhtitem (pt_hash, NameAlloc(&na, "in"), T_IN);
        addhtitem (pt_hash, NameAlloc(&na, "mutex"), T_MUTEX);
        addhtitem (pt_hash, NameAlloc(&na, "crosstalk"), T_CROSSTALK);

        addhtitem (pt_hash, NameAlloc(&na, "muxup"), T_MUXU);
        addhtitem (pt_hash, NameAlloc(&na, "muxdn"), T_MUXD);
        addhtitem (pt_hash, NameAlloc(&na, "cmpup"), T_CMPU);
        addhtitem (pt_hash, NameAlloc(&na, "cmpdn"), T_CMPD);
        addhtitem (pt_hash, NameAlloc(&na, "norising"), T_NORISING);
        addhtitem (pt_hash, NameAlloc(&na, "nofalling"), T_NOFALLING);
        addhtitem (pt_hash, NameAlloc(&na, "falsepath"), T_FALSEPATH);
        addhtitem (pt_hash, NameAlloc(&na, "falseslack"), T_FALSESLACK);
        addhtitem (pt_hash, NameAlloc(&na, "delay"), T_DELAY);
        addhtitem (pt_hash, NameAlloc(&na, "rise"), T_RISE);
        addhtitem (pt_hash, NameAlloc(&na, "fall"), T_FALL);

        addhtitem (pt_hash, NameAlloc(&na, "operating"), T_OPERATING);
        addhtitem (pt_hash, NameAlloc(&na, "conditions"), T_CONDITION);
        addhtitem (pt_hash, NameAlloc(&na, "pinslope"), T_PINSLEW);
        addhtitem (pt_hash, NameAlloc(&na, "pinslew"), T_PINSLEW);
        addhtitem (pt_hash, NameAlloc(&na, "asynchron"), T_ASYNCHRON);
        addhtitem (pt_hash, NameAlloc(&na, "inputs"), T_INPUTS);
        addhtitem (pt_hash, NameAlloc(&na, "notlatch"), T_NOTLATCH);
        addhtitem (pt_hash, NameAlloc(&na, "cklatch"), T_CKLATCH);
        addhtitem (pt_hash, NameAlloc(&na, "ckprech"), T_CKPRECH);
        addhtitem (pt_hash, NameAlloc(&na, "name"), TOK_NAME);
        addhtitem (pt_hash, NameAlloc(&na, "version"), T_VERSION);

        addhtitem (pt_hash, NameAlloc(&na, "ps"), T_UNITPS);
        addhtitem (pt_hash, NameAlloc(&na, "ns"), T_UNITNS);
        addhtitem (pt_hash, NameAlloc(&na, "us"), T_UNITUS);
        addhtitem (pt_hash, NameAlloc(&na, "pf"), T_UNITPF);
        addhtitem (pt_hash, NameAlloc(&na, "ff"), T_UNITFF);

        addhtitem (pt_hash, NameAlloc(&na, "slopein"), T_SLOPEIN);
        addhtitem (pt_hash, NameAlloc(&na, "capaout"), T_CAPAOUT);

        addhtitem (pt_hash, NameAlloc(&na, "path"), T_PATH);
        addhtitem (pt_hash, NameAlloc(&na, "delay"), T_DELAY);
        addhtitem (pt_hash, NameAlloc(&na, "margin"), T_MARGIN);
        addhtitem (pt_hash, NameAlloc(&na, "multicycle"), T_MULTICYCLE);
        addhtitem (pt_hash, NameAlloc(&na, "cycles"), T_CYCLE);
        addhtitem (pt_hash, NameAlloc(&na, "virtual"), T_VIRTUAL);
        addhtitem (pt_hash, NameAlloc(&na, "ideal"), T_IDEAL);
        addhtitem (pt_hash, NameAlloc(&na, "latency"), T_LATENCY);
        addhtitem (pt_hash, NameAlloc(&na, "ignore"), T_IGNORE);
        addhtitem (pt_hash, NameAlloc(&na, "connector"), T_CONNECTOR);
        addhtitem (pt_hash, NameAlloc(&na, "directions"), T_DIRECTIONS);
        addhtitem (pt_hash, NameAlloc(&na, "donotcross"), T_DONTCROSS);
        addhtitem (pt_hash, NameAlloc(&na, "transparent"), T_TRANSPARENT);
        addhtitem (pt_hash, NameAlloc(&na, "parasitics"), T_PARISITICS);
        addhtitem (pt_hash, NameAlloc(&na, "nocheck"), T_NOCHECK);
        addhtitem (pt_hash, NameAlloc(&na, "capacitance"), T_CAPACITANCE);
        addhtitem (pt_hash, NameAlloc(&na, "setuphold"), T_SETUPHOLD);
        addhtitem (pt_hash, NameAlloc(&na, "gate"), T_GATE);
        addhtitem (pt_hash, NameAlloc(&na, "characteristics"), T_CHARACT);
        addhtitem (pt_hash, NameAlloc(&na, "directives"), T_DIRECTIVES);
        addhtitem (pt_hash, NameAlloc(&na, "with"), T_WITH);
        addhtitem (pt_hash, NameAlloc(&na, "switchingprobability"), T_SWITCHPROBA);
        addhtitem (pt_hash, NameAlloc(&na, "clockuncertainty"), T_CLOCKUNCERTAINTY);
        addhtitem (pt_hash, NameAlloc(&na, "clocklatencies"), T_CLOCK_LATENCIES);
        addhtitem (pt_hash, NameAlloc(&na, "memsym"), T_MEMSYM);
        addhtitem (pt_hash, NameAlloc(&na, "strictsetup"), T_STRICT_SETUP);
        addhtitem (pt_hash, NameAlloc(&na, "keeptristatebehaviour"), T_KEEP_TRISTATE_BEHAVIOUR);

        addhtitem (pt_hash, NameAlloc(&na, "rstype"), T_MARKRS);
        addhtitem (pt_hash, NameAlloc(&na, "stabflags"), T_STABFLAGS);
    }
    return (gethtitem (pt_hash, NameAlloc(&na, key)));
}

%}

%x PAR COMMENTARY

num   [0-9]
exp   [eE]
value -?{num}*(\.{num}+)?({exp}-?{num}+)?

%%
\/\*				   {BEGIN COMMENTARY;}
<COMMENTARY>[^(\n|(\*\/))]+	   {}
<COMMENTARY>\*		           {}
<COMMENTARY>\*\/		   {BEGIN INITIAL;}
<COMMENTARY>.		           {}
[ \t]+                                   {ECHO;}
[\n]                                     {ECHO;infParseLine++;}
^[ \t]*[#]+.*                                  {ECHO;}
{value}                                  {ECHO; inflval.val = strtod (yytext, (char**)NULL) ; return (T_VAL);} 
\"[^\" \t\n]*\"                            {ECHO;/* inflval.str = */stripquotes(yytext,inflval.tab); return(T_NAME);}
[^ \"\t\n;:+{},~!=<>\(\)\[\]0-9][^ \t\n;:{},~+!=<>]* {long token;
                                           ECHO; 
                                           token=search(yytext);
                                           if (token!=EMPTYHT) return token;
                                           //inflval.str = stripquotes(yytext);
                                           stripquotes(yytext, inflval.tab);
                                           return(T_NAME);
                                          }
.                                      {ECHO; return(*yytext) ; } 
%%
//[:]                                      {ECHO; return(*yytext) ; } 
//[{]                                      {ECHO; return(*yytext) ; } 
//[}]                                      {ECHO; return(*yytext) ; } 
//[,]                                      {ECHO; return(*yytext) ; } 
//[~]                                      {ECHO; return(*yytext) ; } 
//[<]                                      {ECHO; return(*yytext) ; } 
//[>]                                      {ECHO; return(*yytext) ; } 
//[!]                                      {ECHO; return(*yytext) ; } 
//[=]                                      {ECHO; return(*yytext) ; } 

void stripquotes (char *source, char *dest)
{
  int i = 0 ;
  
  while (*source != 0) {
      if ((dest[i] = *(source++)) != '"') i++;
  }
  dest[i] = '\0';
}

int yywrap()
{
return(1);
}
