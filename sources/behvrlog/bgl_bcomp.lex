/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : Lexical analyser for Verilog                                */
/*    Fichier : bgl_bcomp.lex                                               */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

%option c++
%option yyclass="bgl_bcomplexer"

%{
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cctype>

#include "bgl_ifdstreambuf.h"

using namespace std;

extern "C" {
#include MUT_H
#include BEH_H
#include LOG_H
#include BGL_H
}

#include "bgl_type.h"
extern "C" {
  #include "bgl_bcomp.tab.h"
}
#include "bgl_parse.h"
#include "bgl_util.h"
#include "bgl_class.h"

int yyFlexLexer::yywrap() {
    return (1);
}

void bgl_bcomplexer::embedded_yylex(int& result, YYSTYPE *lvalp, bgl_bcompcontext *local_context) {
    context = local_context;
    result = yylex();
    *lvalp = semantic_value;
}

extern "C" bgl_bcompcontext *bgl_getcontext(void *parm) {
    return &(((bgl_bcompparser *)parm)->context);
}

extern "C" int bgl_bcomplex(YYSTYPE *lvalp, void *parm) {
    int result;
    bgl_bcompcontext *ptcontext;

    ptcontext = bgl_getcontext(parm);
    ((bgl_bcompparser *)parm)->currentLexer->embedded_yylex(result, lvalp, ptcontext);
    return result;
}

extern "C" void *bgl_initparser(FILE *ptinbuf) {
    bgl_bcomplexer *ptlexer;
    bgl_bcompparser *ptparser;

    ptlexer = new bgl_bcomplexer;
    ptparser = new bgl_bcompparser(ptlexer, ptinbuf);
    ptparser->currentLexer->switch_streams(ptparser->bgl_infile, NULL);
    return (void *)ptparser;
}

extern "C" void bgl_delparser(void *parm) {
    delete ((bgl_bcompparser *)parm)->currentLexer;
    delete (bgl_bcompparser *)parm;
}

static char *
stripchar (char *str, char c)
{
    char    buffer[1024];
    int     i = 0;
    int     j = 0;

    while (str[i] != 0) {
        if (str[i] != c) buffer[j++] = str[i];
        i++;
    }
    buffer[j] = 0;

    return namealloc(buffer);
}

/* ###--------------------------------------------------------------### */
/* function :     search                                                */
/* description :  check that an identifier is a reserved word or not    */
/* called func. : addht, addhtitem, gethtitem, namealloc                */
/* ###--------------------------------------------------------------### */

static int search (char *key)
{
    static ht *pt_hash = NULL;

    if (pt_hash == NULL) {
        pt_hash = addht (107);

        addhtitem (pt_hash, namealloc("always")       , T_ALWAYS       );
        addhtitem (pt_hash, namealloc("and")          , T_AND          );
        addhtitem (pt_hash, namealloc("assign")       , T_ASSIGN       );
        addhtitem (pt_hash, namealloc("attribute")    , T_ATTRIBUTE    );

        addhtitem (pt_hash, namealloc("begin")        , T_BEGIN        );
        addhtitem (pt_hash, namealloc("buf")          , T_BUF          );
        addhtitem (pt_hash, namealloc("bufif0")       , T_BUFIF0       );
        addhtitem (pt_hash, namealloc("bufif1")       , T_BUFIF1       );

        addhtitem (pt_hash, namealloc("case")         , T_CASE         );
        addhtitem (pt_hash, namealloc("casex")        , T_CASEX        );
        addhtitem (pt_hash, namealloc("casez")        , T_CASEZ        );
        addhtitem (pt_hash, namealloc("cmos")         , T_CMOS         );

        addhtitem (pt_hash, namealloc("deassign")     , T_DEASSIGN     );
        addhtitem (pt_hash, namealloc("default")      , T_DEFAULT      );
        addhtitem (pt_hash, namealloc("defparam")     , T_DEFPARAM     );
        addhtitem (pt_hash, namealloc("disable")      , T_DISABLE      );

        addhtitem (pt_hash, namealloc("edge")         , T_EDGE         );
        addhtitem (pt_hash, namealloc("else")         , T_ELSE         );
        addhtitem (pt_hash, namealloc("end")          , T_END          );
        addhtitem (pt_hash, namealloc("endattribute") , T_ENDATTRIBUTE );
        addhtitem (pt_hash, namealloc("endcase")      , T_ENDCASE      );
        addhtitem (pt_hash, namealloc("endfunction")  , T_ENDFUNCTION  );
        addhtitem (pt_hash, namealloc("endmodule")    , T_ENDMODULE    );
        addhtitem (pt_hash, namealloc("endprimitive") , T_ENDPRIMITIVE );
        addhtitem (pt_hash, namealloc("endspecify")   , T_ENDSPECIFY   );
        addhtitem (pt_hash, namealloc("endtable")     , T_ENDTABLE     );
        addhtitem (pt_hash, namealloc("endtask")      , T_ENDTASK      );
        addhtitem (pt_hash, namealloc("event")        , T_EVENT        );

        addhtitem (pt_hash, namealloc("for")          , T_FOR          );
        addhtitem (pt_hash, namealloc("force")        , T_FORCE        );
        addhtitem (pt_hash, namealloc("forever")      , T_FOREVER      );
        addhtitem (pt_hash, namealloc("fork")         , T_FORK         );
        addhtitem (pt_hash, namealloc("function")     , T_FUNCTION     );

        addhtitem (pt_hash, namealloc("highz0")       , T_HIGHZ0       );
        addhtitem (pt_hash, namealloc("highz1")       , T_HIGHZ1       );

        addhtitem (pt_hash, namealloc("if")           , T_IF           );
        addhtitem (pt_hash, namealloc("ifnone")       , T_IFNONE       );
        addhtitem (pt_hash, namealloc("initial")      , T_INITIAL      );
        addhtitem (pt_hash, namealloc("inout")        , T_INOUT        );
        addhtitem (pt_hash, namealloc("input")        , T_INPUT        );
        addhtitem (pt_hash, namealloc("integer")      , T_INTEGER      );

        addhtitem (pt_hash, namealloc("join")         , T_JOIN         );

        addhtitem (pt_hash, namealloc("large")        , T_LARGE        );

        addhtitem (pt_hash, namealloc("medium")       , T_MEDIUM       );
        addhtitem (pt_hash, namealloc("module")       , T_MODULE       );
        addhtitem (pt_hash, namealloc("macromodule")  , T_MACROMODULE  );

        addhtitem (pt_hash, namealloc("nand")         , T_NAND         );
        addhtitem (pt_hash, namealloc("negedge")      , T_NEGEDGE      );
        addhtitem (pt_hash, namealloc("nmos")         , T_NMOS         );
        addhtitem (pt_hash, namealloc("nor")          , T_NOR          );
        addhtitem (pt_hash, namealloc("not")          , T_NOT          );
        addhtitem (pt_hash, namealloc("notif0")       , T_NOTIF0       );
        addhtitem (pt_hash, namealloc("notif1")       , T_NOTIF1       );

        addhtitem (pt_hash, namealloc("or")           , T_OR           );
        addhtitem (pt_hash, namealloc("output")       , T_OUTPUT       );

        addhtitem (pt_hash, namealloc("parameter")    , T_PARAMETER    );
        addhtitem (pt_hash, namealloc("pmos")         , T_PMOS         );
        addhtitem (pt_hash, namealloc("posedge")      , T_POSEDGE      );
        addhtitem (pt_hash, namealloc("primitive")    , T_PRIMITIVE    );
        addhtitem (pt_hash, namealloc("pull0")        , T_PULL0        );
        addhtitem (pt_hash, namealloc("pull1")        , T_PULL1        );
        addhtitem (pt_hash, namealloc("pulldown")     , T_PULLDOWN     );
        addhtitem (pt_hash, namealloc("pullup")       , T_PULLUP       );

        addhtitem (pt_hash, namealloc("rcmos")        , T_RCMOS        );
        addhtitem (pt_hash, namealloc("real")         , T_REAL         );
        addhtitem (pt_hash, namealloc("realtime")     , T_REALTIME     );
        addhtitem (pt_hash, namealloc("reg")          , T_REG          );
        addhtitem (pt_hash, namealloc("release")      , T_RELEASE      );
        addhtitem (pt_hash, namealloc("repeat")       , T_REPEAT       );
        addhtitem (pt_hash, namealloc("rnmos")        , T_RNMOS        );
        addhtitem (pt_hash, namealloc("rpmos")        , T_RPMOS        );
        addhtitem (pt_hash, namealloc("rtran")        , T_RTRAN        );
        addhtitem (pt_hash, namealloc("rtranif0")     , T_RTRANIF0     );
        addhtitem (pt_hash, namealloc("rtranif1")     , T_RTRANIF1     );

        addhtitem (pt_hash, namealloc("scalared")     , T_SCALARED     );
        addhtitem (pt_hash, namealloc("signed")       , T_SIGNED       );
        addhtitem (pt_hash, namealloc("small")        , T_SMALL        );
        addhtitem (pt_hash, namealloc("specify")      , T_SPECIFY      );
        addhtitem (pt_hash, namealloc("specparam")    , T_SPECPARAM    );
        addhtitem (pt_hash, namealloc("strength")     , T_STRENGTH     );
        addhtitem (pt_hash, namealloc("strong0")      , T_STRONG0      );
        addhtitem (pt_hash, namealloc("strong1")      , T_STRONG1      );
        addhtitem (pt_hash, namealloc("supply0")      , T_SUPPLY0      );
        addhtitem (pt_hash, namealloc("supply1")      , T_SUPPLY1      );

        addhtitem (pt_hash, namealloc("table")        , T_TABLE        );
        addhtitem (pt_hash, namealloc("task")         , T_TASK         );
        addhtitem (pt_hash, namealloc("time")         , T_TIME         );
        addhtitem (pt_hash, namealloc("tran")         , T_TRAN         );
        addhtitem (pt_hash, namealloc("tranif0")      , T_TRANIF0      );
        addhtitem (pt_hash, namealloc("tranif1")      , T_TRANIF1      );
        addhtitem (pt_hash, namealloc("tri")          , T_TRI          );
        addhtitem (pt_hash, namealloc("tri0")         , T_TRI0         );
        addhtitem (pt_hash, namealloc("tri1")         , T_TRI1         );
        addhtitem (pt_hash, namealloc("triand")       , T_TRIAND       );
        addhtitem (pt_hash, namealloc("trior")        , T_TRIOR        );
        addhtitem (pt_hash, namealloc("trireg")       , T_TRIREG       );

        addhtitem (pt_hash, namealloc("unsigned")     , T_UNSIGNED     );

        addhtitem (pt_hash, namealloc("vectored")     , T_VECTORED     );

        addhtitem (pt_hash, namealloc("wait")         , T_WAIT         );
        addhtitem (pt_hash, namealloc("wand")         , T_WAND         );
        addhtitem (pt_hash, namealloc("weak0")        , T_WEAK0        );
        addhtitem (pt_hash, namealloc("weak1")        , T_WEAK1        );
        addhtitem (pt_hash, namealloc("while")        , T_WHILE        );
        addhtitem (pt_hash, namealloc("wire")         , T_WIRE         );
        addhtitem (pt_hash, namealloc("wor")          , T_WOR          );

        addhtitem (pt_hash, namealloc("xnor")         , T_XNOR         );
        addhtitem (pt_hash, namealloc("xor")          , T_XOR          );
    }
    return (gethtitem (pt_hash, namealloc(key)));
}

%}

letter                  [a-zA-Z]
digit                   [0-9]
underscore              _
dollar                  \$
grave_accent            `
apostrophe              '
decimal_point           \.
colon                   :
white_space             [ \a\b\f\r\t\v<<EOF>>]
end_of_line             \n
comment                 \/\/.*$

decimal_literal         {integer}(\.{integer})?({exponent})?
integer                 {digit}({underscore}?{digit})*
exponent                ([eE][-+]?{integer})
based_integer           {extended_digit}({underscore}?{extended_digit})*
extended_digit          ({digit}|[a-fA-FxXzZ\?])
base_specifier          {apostrophe}(B|b|O|o|D|d|H|h)

%x comment table table_comment

%%

"/*"                  {BEGIN(comment);}
<comment>[^*\n]       {;}
<comment>"*"+[^*/\n]* {;}
<comment>\n           {context->LINENUM++;}
<comment>"*"+"/"      {BEGIN(INITIAL);}

<table>{white_space}    {;}
<table>{comment}        {;}
<table>"/*"             {BEGIN(table_comment);}
<table>\n               {context->LINENUM++;}
<table>.                {return (*yytext);}
<table>endtable         {BEGIN(INITIAL); return (T_ENDTABLE);}

<table_comment>[^*\n]       {;}
<table_comment>"*"+[^*/\n]* {;}
<table_comment>\n           {context->LINENUM++;}
<table_comment>"*"+"/"      {BEGIN(table);}

<INITIAL>({letter}|{underscore})({letter}|{digit}|{underscore}|{dollar})* {
    int itoken;

    itoken = search (yytext);
    if (itoken == EMPTYHT) {
        if (!BGL_CASE_SENSITIVE) {
            semantic_value.text = namealloc(yytext);
        }
        else {
            if (BGL_CASEHT == NULL) BGL_CASEHT = addht(200);
            semantic_value.text = bgl_getcasename(BGL_CASEHT, yytext);
        }
        return (Identifier);
    }
    else {
        if (itoken == T_TABLE) BEGIN(table);
        return (itoken);
    }
}

{dollar}setuphold.*$    {return SYS_SETUPHOLD;}
{dollar}setup.*$        {return SYS_SETUP;}
{dollar}hold.*$         {return SYS_HOLD;}
{dollar}skew.*$         {return SYS_SKEW;}
{dollar}recovery.*$     {return SYS_RECOVERY;}
{dollar}period.*$       {return SYS_PERIOD;}
{dollar}width.*$        {return SYS_WIDTH;}

{dollar}({letter}|{underscore})({letter}|{digit}|{underscore})* {
    semantic_value.text = namealloc(yytext + 1);
    return SystemIdentifier;
}

\\[^ \t\n]* {
    if (!BGL_CASE_SENSITIVE) {
        semantic_value.text = namealloc(yytext + 1);
    }
    else {
        if (BGL_CASEHT == NULL) BGL_CASEHT = addht(200);
        semantic_value.text = bgl_getcasename(BGL_CASEHT, yytext + 1);
    }
    return (Identifier);
}


{integer}{decimal_point}{integer}([Ee][+-]?{integer})? {
    semantic_value.text = stripchar(yytext, '_');
    return (RealLit);
}

{integer}|({integer}?{base_specifier}{white_space}*{based_integer}) {
    char *pt1, *pt2;
    int size=0;

    pt1 = stripchar(yytext, '_');
    if ((pt2 = strchr(pt1,'\'')) != NULL) {
        *pt2='\0';
        size=atoi(yytext);
	if (*(pt2+2)=='z' || *(pt2+2)=='Z')
	  semantic_value.integer.value = -1234567890;
	else
	  switch (tolower(*(pt2+1))) {
	  case 'b':
            semantic_value.integer.value = (int)strtol(pt2+2, NULL, 2);
            break;
	  case 'o':
            semantic_value.integer.value = (int)strtol(pt2+2, NULL, 8);
            break;
	  case 'd':
            semantic_value.integer.value = (int)strtol(pt2+2, NULL, 10);
            break;
	  case 'h':
            semantic_value.integer.value = (int)strtol(pt2+2, NULL, 16);
            break;
	  }
    }
    else {
        semantic_value.integer.value = (int)strtol(pt1, NULL, 10);
    }
    semantic_value.integer.size=size;
    return (IntegerLit);
}

{integer}{colon}{integer}{colon}{integer} {
    semantic_value.text = namealloc(yytext);
    return (MinTypMaxLit);
}

\"[^"]*\" {
    semantic_value.text = mbkstrdup(yytext + 1);
    return StringLit;
}

"==="               {return T_IDENTICAL;}
"!=="               {return T_NOT_IDENTICAL;}
"=>"                {return T_LEADTO;}
"*>"                {return T_ALLPATH;}
"?:"                {return T_CONDITIONAL;}
"=="                {return T_EQUALS;}
"!="                {return T_DOES_NOT_EQUAL;}
"&&"                {return T_LOGICAL_AND;}
"||"                {return T_LOGICAL_OR;}
"~&"                {return T_BITWISE_NAND;}
"~|"                {return T_BITWISE_NOR;}
"~^"                {return T_BITWISE_EXNOR;}
"^~"                {return T_BITWISE_EXNOR;}
"<="                {return T_LESS_THAN_OR_EQUAL_TO;}
">="                {return T_GREATER_THAN_OR_EQUAL_TO;}
"<<"                {return T_SHIFT_LEFT;}
">>"                {return T_SHIFT_RIGHT;}
\!                  {return T_LOGICAL_NOT;}
\~                  {return T_BITWISE_NOT;}
\&                  {return T_BITWISE_AND;}
\|                  {return T_BITWISE_OR;}
\^                  {return T_BITWISE_EXOR;}
\-                  {return T_MINUS;}
\+                  {return T_PLUS;}
\*                  {return T_MULTIPLY;}
\/                  {return T_DIVIDE;}
\%                  {return T_MODULO;}
\<                  {return T_LESS_THAN;}
\>                  {return T_GREATER_THAN;}

{white_space}       {;}
{comment}           {;}
{grave_accent}.*$   {;} /* comiler directive */
\n                  {context->LINENUM++;}
.                   {return (*yytext);}

%%

/* ###--------------------------------------------------------------### */
/*  function : yywrap                                                   */
/* ###--------------------------------------------------------------### */

int
yywrap()
{
    return (1);
}
