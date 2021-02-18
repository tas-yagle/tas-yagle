/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : Grammar for Verilog                                         */
/*    Fichier : bgl_error.c                                                 */
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
#include BEH_H
#include AVT_H
#include "bgl_type.h"
#include "bgl_parse.h"

/* ###--------------------------------------------------------------### */
/*  function : bgl_error                                                */
/* ###--------------------------------------------------------------### */
void
bgl_error(int code, char *str1, bgl_bcompcontext *context)
{
    context->ERRFLG++;

    switch (code) {
    case 1:
         avt_errmsg(BGL_ERRMSG,"001",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "`%s` is incompatible with the entity name\n", str1);
        break;
    case 2:
         avt_errmsg(BGL_ERRMSG,"002",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "bad entity declaration\n");
        break;
    case 3:
         avt_errmsg(BGL_ERRMSG,"003",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "bad port clause declaration\n");
        break;
    case 4:
         avt_errmsg(BGL_ERRMSG,"004",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "port `%s` already declared\n", str1);
        break;
    case 5:
         avt_errmsg(BGL_ERRMSG,"005",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "illegal port declaration `%s` (mode, type, guard mark)\n", str1);
        break;
    case 6:
         avt_errmsg(BGL_ERRMSG,"006",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "bad port declaration\n");
        break;
    case 7:
         avt_errmsg(BGL_ERRMSG,"007",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "`%s` is incompatible with the architecture name\n", str1);
        break;
    case 8:
         avt_errmsg(BGL_ERRMSG,"008",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "bad architecture declaration\n");
        break;
    case 9:
         avt_errmsg(BGL_ERRMSG,"009",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "illegal declaration\n");
        break;
    case 10:
         avt_errmsg(BGL_ERRMSG,"010",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "signal `%s` already declared\n", str1);
        break;
    case 11:
         avt_errmsg(BGL_ERRMSG,"011",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "illegal signal declaration `%s` (type, guard mark)\n", str1);
        break;
    case 12:
         avt_errmsg(BGL_ERRMSG,"012",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "component `%s` already declared\n", str1);
        break;
    case 13:
         avt_errmsg(BGL_ERRMSG,"013",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "instance `%s` already declared\n", str1);
        break;
    case 14:
         avt_errmsg(BGL_ERRMSG,"014",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "`%s` unknown component\n", str1);
        break;
    case 15:
         avt_errmsg(BGL_ERRMSG,"015",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "illegal usage of implicit port map description\n");
        break;
    case 16:
         avt_errmsg(BGL_ERRMSG,"016",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "`%s` unknown local port\n", str1);
        break;
    case 17:
         avt_errmsg(BGL_ERRMSG,"017",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "`%s` unknown port or signal\n", str1);
        break;
    case 18:
         avt_errmsg(BGL_ERRMSG,"018",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "illegal concurrent statement\n");
        break;
    case 31:
         avt_errmsg(BGL_ERRMSG,"019",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "bad signal association\n");
        break;
    case 32:
         avt_errmsg(BGL_ERRMSG,"020",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "null array not supported\n");
        break;
    case 33:
         avt_errmsg(BGL_ERRMSG,"021",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "illegal constraint in declaration of type\n");
        break;
    case 36:
         avt_errmsg(BGL_ERRMSG,"022",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "signal `%s` used out of declared range\n", str1);
        break;
    case 38:
         avt_errmsg(BGL_ERRMSG,"023",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "width or/and type mismatch\n");
        break;
    case 41:
         avt_errmsg(BGL_ERRMSG,"024",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "port `%s` connected to more than one signal\n", str1);
        break;
    case 44:
         avt_errmsg(BGL_ERRMSG,"025",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "can only assign to/from an external connector\n");
        break;
    case 76:
         avt_errmsg(BGL_ERRMSG,"026",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "instance %s mismatch with the model\n", str1);
        break;
    case 90:
         avt_errmsg(BGL_ERRMSG,"027",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "Unhandled feature\n");
        break;
    case 91:
         avt_errmsg(BGL_ERRMSG,"028",AVT_FATAL,context->FILENAME,context->LINENUM,str1);
//        fprintf(stderr, "%s\n",str1);
        break;
    case 107:
         avt_errmsg(BGL_ERRMSG,"029",AVT_FATAL);
//        fprintf(stderr, "Cannot open result file\n");
        break;
    case 200:
         avt_errmsg(BGL_ERRMSG,"030",AVT_FATAL);
//        fprintf(stderr, "\n      cannot continue further more.\n");
//        fprintf(stderr, "\n              Have a nice day...\n");
        break;

    default:
         avt_errmsg(BGL_ERRMSG,"031",AVT_FATAL,context->FILENAME,context->LINENUM);
//        fprintf(stderr, "syntax error\n");
        break;
    }

    if (context->ERRFLG > V_INT_TAB[__VERILOG_MAXERR].VALUE) {
//        fprintf(stderr, "Too many errors. Cannot continue further more\n");
//        fprintf(stderr, "\n              Have a nice day...\n");
        bgl_bcompclean(context);
      avt_errmsg(BGL_ERRMSG,"032",AVT_FATAL);
//        EXIT(1);
    }

}

/* ###--------------------------------------------------------------### */
/*  function : bgl_warning                                              */
/*  content  : print out warning messages on the standard error output  */
/* ###--------------------------------------------------------------### */
void
bgl_warning(int code, char *str1)
{
    static char     first_time = 0;

    switch (code) {
    case 2:
        if (first_time != 1) {
          avt_errmsg(BGL_ERRMSG,"036",AVT_WARNING);
//            fprintf(stderr, "Warning %d : ", code);
//            fprintf(stderr, "consistency checks will be disabled\n");
            first_time = 1;
        }
        break;

    case 42:
        avt_errmsg(BGL_ERRMSG,"038",AVT_WARNING,"500",str1);
//        fprintf(stderr, "Warning : connection missing on port `%s`\n",
//                       str1);
        break;

//    default:
//        {
//            fprintf(stderr, "Warning %d : ", code);
//            fprintf(stderr, "unknown Warning code\n");
//        }
    }
}

/* ###--------------------------------------------------------------### */
/*  function : bgl_toolbug                                              */
/*  content  : print out bugs messages on the standard error output     */
/* ###--------------------------------------------------------------### */
void
bgl_toolbug(int code, char *str1, char *str2, int nbr1, bgl_bcompcontext *context)
{
//    fprintf(stderr, "Fatal error %d executing `%s`: ", code, str1);
    switch (code) {
    case 10:
         bgl_bcompclean(context);
         avt_errmsg(BGL_ERRMSG,"038",AVT_FATAL,"000",str1);
//        fprintf(stderr, "decompiler called on empty lofig\n");
        break;
    default:
         bgl_bcompclean(context);
         avt_errmsg(BGL_ERRMSG,"038",AVT_FATAL,"001",str1);
    }
//    EXIT(1);
#ifndef __ALL__WARNING__ 
  // prevent warning ..anto..
  str2=NULL;nbr1=0;
#endif
}


/* ###--------------------------------------------------------------### */
/*  function : bgl_message                                              */
/*  content  : print out messages on the standard error output          */
/* ###--------------------------------------------------------------### */
void
bgl_message(int code, char *str1, int nmb1)
{
//    switch (code) {
//    default:
//        fprintf(stderr, "bgl_message : code %d unknown.\n", code);
//    }
#ifndef __ALL__WARNING__ 
  // prevent warning ..anto..
  str1=NULL;nmb1=0;code=0;
#endif
}


