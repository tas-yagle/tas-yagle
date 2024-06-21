%{
#include STB_H
#include "stb_parse.h"
#include "stb.tab.h"

#define ECHO
//fprintf(stderr,"\n-=<{oo}>=- %s\n",yytext)
#define YY_NO_UNPUT
/* ###--------------------------------------------------------------### */
/* function      : search                                               */
/* description   : check that an identifier is a reserved word or not   */
/* called func.  : addht, addhtitem, gethtitem, namealloc               */
/* ###--------------------------------------------------------------### */

extern int yylineno;

static char *stripquotes();

static int search (key)
    char  *key;
{
    static ht *pt_hash = NULL;

    if (pt_hash == NULL) {
        pt_hash = addht (100);
        addhtitem (pt_hash, namealloc("begin"), _BEGIN);
        addhtitem (pt_hash, namealloc("clock"), _CLOCK);
        addhtitem (pt_hash, namealloc("connectors"), _CONNECTORS);
        addhtitem (pt_hash, namealloc("groups"), _GROUPS);
        addhtitem (pt_hash, namealloc("asynchronous"), _ASYNCHRONOUS);
        addhtitem (pt_hash, namealloc("equivalent"), _EQUIVALENT);
        addhtitem (pt_hash, namealloc("disable"), _DISABLE);
        addhtitem (pt_hash, namealloc("stable"), _STABLE);
        addhtitem (pt_hash, namealloc("down"), _DOWN);
        addhtitem (pt_hash, namealloc("default"), _DEFAULT);
        addhtitem (pt_hash, namealloc("end"), _END);
        addhtitem (pt_hash, namealloc("holdtime"), _HOLDTIME);
        addhtitem (pt_hash, namealloc("input"), _INPUT);
        addhtitem (pt_hash, namealloc("name"), _NAME);
        addhtitem (pt_hash, namealloc("stability"), _STABILITY);
        addhtitem (pt_hash, namealloc("output"), _OUTPUT);
        addhtitem (pt_hash, namealloc("period"), _PERIOD);
        addhtitem (pt_hash, namealloc("setuptime"), _SETUPTIME);
        addhtitem (pt_hash, namealloc("version"), _VERSION);
        addhtitem (pt_hash, namealloc("unstable"), _UNSTABLE);
        addhtitem (pt_hash, namealloc("verify"), _VERIFY);
        addhtitem (pt_hash, namealloc("specify"), _SPECIFY);
        addhtitem (pt_hash, namealloc("up"), _UP);
        addhtitem (pt_hash, namealloc("down"), _DOWN);
        addhtitem (pt_hash, namealloc("noverif"), _NOVERIF);
        addhtitem (pt_hash, namealloc("rising"), _RISING);
        addhtitem (pt_hash, namealloc("falling"), _FALLING);
        addhtitem (pt_hash, namealloc("from"), _FROM);
        addhtitem (pt_hash, namealloc("for"), _FOR);
        addhtitem (pt_hash, namealloc("to"), _TO);
        addhtitem (pt_hash, namealloc("internal"), _INTERNAL);
        addhtitem (pt_hash, namealloc("nodes"), _NODES);
        addhtitem (pt_hash, namealloc("after"), _AFTER);
        addhtitem (pt_hash, namealloc("before"), _BEFORE);
        addhtitem (pt_hash, namealloc("conditioned"), _CONDITIONED);
        addhtitem (pt_hash, namealloc("command"), _COMMAND);
        addhtitem (pt_hash, namealloc("states"), _STATES);
        addhtitem (pt_hash, namealloc("without"), _WITHOUT);
        addhtitem (pt_hash, namealloc("precharge"), _PRECHARGE);
        addhtitem (pt_hash, namealloc("evaluate"), _EVALUATE);
        addhtitem (pt_hash, namealloc("multiple"), _MULTIPLE);
        addhtitem (pt_hash, namealloc("priority"), _PRIORITY);
        addhtitem (pt_hash, namealloc("memory"), _MEMORY);
    }
    return (gethtitem (pt_hash, namealloc(key)));
}

%}

upper_case_letter         [A-Z]
digit                     [0-9]
wildcard                  [*]
allowed                   [/]
space_character           [ \t]
format_effector           [\t\v\r\l\f]
end_of_line               \n
lower_case_letter         [a-z]
wildcard_letter           ({upper_case_letter}|{lower_case_letter}|{wildcard})
wildcard_letter_or_digit  ({wildcard_letter}|{digit}|{allowed})
decimal_literal           {integer}(\.{integer})?
integer                   [\-]?{digit}(_?{digit})*
comment                   ^[ \t]*[#]+.*
  
%%
{end_of_line}      {ECHO;yylineno++;}
{space_character}  {ECHO;}
{comment}          {ECHO;}
\(                 {ECHO;return(LeftParen);}
\)                 {ECHO;return(RightParen);}
\:                 {ECHO;return(Colon);}
\;                 {ECHO;return(Semicolon);}
\,                 {ECHO;return(Comma);}
\!                 {ECHO;return(Not);}


\"[^ \t\n]*\" {
    ECHO;
    stblval.text = stripquotes(yytext);
    return (Identifier);
}

{wildcard_letter}((([.]?)|(_*)){wildcard_letter_or_digit})*(\[{integer}\])? {
    int itoken;

    ECHO;
    itoken = search (yytext);
    if (itoken == EMPTYHT) {
        stblval.text = namealloc(yytext);
        return (Identifier);
    }
    else {
        return (itoken);
    }
}

{integer} {
    ECHO;
    stblval.valu = (int) strtol(yytext,(char**)NULL,10);
    return (Integer);
}

{decimal_literal} {
    ECHO;
    stblval.valu = (int) strtod(yytext,(char**)NULL);
    return (decimal_literal);
}

%%

static char *stripquotes(source)
    char   *source;
{
    char    dest[1024];
    int     i = 0, j = 0;

    while (*source != 0 && i < 1023) {
        if ((dest[i] = source[j++]) != '"') i++;
    }
    dest[i] = 0;

    return namealloc(dest);
}

int stbclosefile()
{
 return(fclose(stbin)) ;
}

int yywrap() {return 1;}
