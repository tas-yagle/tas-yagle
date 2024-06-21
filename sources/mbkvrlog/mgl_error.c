/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : Grammar for Verilog                                         */
/*    Fichier : mgl_error.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include MUT_H
#include MLO_H
#include AVT_H
#include "mgl_type.h"
#include "mgl_parse.h"

/* ###--------------------------------------------------------------### */
/*  function : mgl_error                                                */
/* ###--------------------------------------------------------------### */
void
mgl_error(int code, char *str1, mgl_scompcontext *context)
{
    context->ERRFLG++;
    if (code < 100) fprintf(stderr, "`%s` Error %d line %d :", context->FILENAME, code, context->LINENUM);
    else {
        if (code < 200) fprintf(stderr, "Error %d :", code);
    }

    switch (code) {
    case 1:
        fprintf(stderr, "`%s` is incompatible with the entity name\n", str1);
        break;
    case 2:
        fprintf(stderr, "bad entity declaration\n");
        break;
    case 3:
        fprintf(stderr, "bad port clause declaration\n");
        break;
    case 4:
        fprintf(stderr, "port `%s` already declared\n", str1);
        break;
    case 5:
        fprintf(stderr, "illegal port declaration `%s` (mode, type, guard mark)\n", str1);
        break;
    case 6:
        fprintf(stderr, "bad port declaration\n");
        break;
    case 7:
        fprintf(stderr, "`%s` is incompatible with the architecture name\n", str1);
        break;
    case 8:
        fprintf(stderr, "bad architecture declaration\n");
        break;
    case 9:
        fprintf(stderr, "illegal declaration\n");
        break;
    case 10:
        fprintf(stderr, "signal `%s` already declared\n", str1);
        break;
    case 11:
        fprintf(stderr, "illegal signal declaration `%s` (type, guard mark)\n", str1);
        break;
    case 12:
        fprintf(stderr, "component `%s` already declared\n", str1);
        break;
    case 13:
        fprintf(stderr, "instance `%s` already declared\n", str1);
        break;
    case 14:
        fprintf(stderr, "`%s` unknown component\n", str1);
        break;
    case 15:
        fprintf(stderr, "illegal usage of implicit port map description\n");
        break;
    case 16:
        fprintf(stderr, "`%s` unknown local port\n", str1);
        break;
    case 17:
        fprintf(stderr, "`%s` unknown port or signal\n", str1);
        break;
    case 18:
        fprintf(stderr, "illegal concurrent statement\n");
        break;
    case 31:
        fprintf(stderr, "bad signal association\n");
        break;
    case 32:
        fprintf(stderr, "null array not supported\n");
        break;
    case 33:
        fprintf(stderr, "illegal constraint in declaration of type\n");
        break;
    case 36:
        fprintf(stderr, "signal `%s` used out of declared range\n", str1);
        break;
    case 38:
        fprintf(stderr, "width or/and type mismatch\n");
        break;
    case 41:
        fprintf(stderr, "port `%s` connected to more than one signal\n", str1);
        break;
    case 44:
        fprintf(stderr, "can only assign to/from an external connector\n");
        break;
    case 76:
        fprintf(stderr, "instance %s mismatch with the model\n", str1);
        break;
    case 107:
        fprintf(stderr, "Cannot open result file\n");
        break;
    case 200:
        fprintf(stderr, "\n      cannot continue further more.\n");
        fprintf(stderr, "\n              Have a nice day...\n");
        break;

    default:
        fprintf(stderr, "syntax error\n");
        break;
    }

    if (context->ERRFLG > V_INT_TAB[__VERILOG_MAXERR].VALUE) {
        fprintf(stderr, "Too many errors. Cannot continue further more\n");
        fprintf(stderr, "\n              Have a nice day...\n");
        mgl_scompclean(context);
        EXIT(1);
    }

}

/* ###--------------------------------------------------------------### */
/*  function : mgl_warning                                              */
/*  content  : print out warning messages on the standard error output  */
/* ###--------------------------------------------------------------### */
void
mgl_warning(int code, char *str1)
{
    static char     first_time = 0;

    switch (code) {
    case 2:
        if (first_time != 1) {
            fprintf(stderr, "Warning %d : ", code);
            fprintf(stderr, "consistency checks will be disabled\n");
            first_time = 1;
        }
        break;

    case 42:
        fprintf(stderr, "Warning : connection missing on port `%s`\n",
                       str1);
        break;

    default:
        {
            fprintf(stderr, "Warning %d : ", code);
            fprintf(stderr, "unknown Warning code\n");
        }
    }
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_toolbug                                              */
/*  content  : print out bugs messages on the standard error output     */
/* ###--------------------------------------------------------------### */
void
mgl_toolbug(int code, char *str1, char *str2, int nbr1, mgl_scompcontext *context)
{
#ifndef __ALL__WARNING__
  str2	= NULL;
  nbr1	= 0;
#endif
    fprintf(stderr, "Fatal error %d executing `%s`: ", code, str1);
    switch (code) {
    case 10:
        fprintf(stderr, "decompiler called on empty lofig\n");
        break;
    }
    mgl_scompclean(context);
    EXIT(1);
}


/* ###--------------------------------------------------------------### */
/*  function : mgl_message                                              */
/*  content  : print out messages on the standard error output          */
/* ###--------------------------------------------------------------### */
void
mgl_message(int code, char *str1, int nmb1)
{
#ifndef __ALL__WARNING__
  str1	= NULL;
  nmb1	= 0;
#endif
    switch (code) {
    default:
        fprintf(stderr, "mgl_message : code %d unknown.\n", code);
    }
}


