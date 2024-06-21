/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : Grammar for Verilog                                         */
/*    Fichier : mgl_parse.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include AVT_H
#include MUT_H
#include MLO_H
#include "mgl_type.h"
#include "mgl_parse.h"
#include "mgl_error.h"
#include "mgl_util.h"

int          MGL_PARSE_ESC_VECTORS;

int          MGL_USE_LIBRARY = 0;

char        *MGL_VDD = "AVT_MGL_VDD";
char        *MGL_VSS = "AVT_MGL_VSS";

static int   MGL_PRELOAD = 0;
void
parseverilognetlist(libname)
    char           *libname;
{
    MGL_PRELOAD = 1;
    verilogloadlofig(NULL, libname, 'A');
    MGL_PRELOAD = 0;
}

void
verilogloadlofig(pt_lofig, figname, mode)
    struct lofig   *pt_lofig;
    char           *figname;
    char            mode;
{
    mgl_scompcontext *ptcontext;
    void             *ptparser;

    struct lofig   *pt_lofig_tmp;
    char            local_mbk_fast_mode;
    char           *str, *suffix;
    char            buffer[1024];
    FILE           *fp;

    mgl_scompdebug = 0;

    if ((mode != 'A') && (mode != 'P') && (mode != 'C')) {
        printf("verilogloadfig : Bad mode '%c' asked\n", mode);
        EXIT(1);
    }

    MGL_PARSE_ESC_VECTORS = V_BOOL_TAB[__MGL_PARSE_ESC_VECTORS].VALUE;

    suffix = V_STR_TAB[__MGL_FILE_SUFFIX].VALUE;
    if (suffix == NULL) suffix = namealloc("v");
    MGL_USE_LIBRARY = V_BOOL_TAB[__MGL_USE_LIBRARY].VALUE;

    /* FAST_MODE asked for MBK */
    local_mbk_fast_mode = FAST_MODE;
    FAST_MODE = 'Y';

    /* Open file */
    if (MGL_PRELOAD) suffix = NULL;
    fp = mbkfopen(figname, suffix, READ_TEXT);
    if (fp == NULL) {
        if (suffix == NULL) sprintf(buffer, "%s", figname);
        else sprintf(buffer, "%s.%s", figname, suffix);
        fprintf(stderr, "\n*** mbk error *** file does not exist : %s\n", buffer);
        EXIT(1);
    }

    /* Initialization of some variables */
    ptparser = mgl_initparser(fp);
    ptcontext = mgl_getcontext(ptparser);
    strcpy(ptcontext->FILENAME, figname);
    ptcontext->LINENUM = 1;
    ptcontext->LOFIG = NULL;
    ptcontext->TOPFIG = NULL;
    ptcontext->BLDFLG = 1;
    ptcontext->ONLY_ORIENT = 0;
    ptcontext->ERRFLG = 0;

    /* TRACE_MODE asked for MBK */
    if (TRACE_MODE == 'Y') {
        printf("\n--- mbk --- parsing file : %s.%s in mode : %c\n", figname, IN_LO, mode);
    }

    ptcontext->LOFIG = pt_lofig;      /* passing main parameter */
    ptcontext->TOPFIG = pt_lofig;     /* passing main parameter */

    /* ------------------------------------------------------------------- */
    /* Parsing  : If mode is P or A, then normal parsing, if mode is C    */
    /* then parsing of a new figure, then from the new one, we fill the   */
    /* old one.                                                           */
    /* ------------------------------------------------------------------- */

    if ((mode == 'P') || (mode == 'A')) {
        if (mgl_scompparse(ptparser) != 0) {
            fprintf(stderr, "\n*** mbk error *** abnormal parsing for : %s\n", ptcontext->FILENAME);
            FAST_MODE = local_mbk_fast_mode;
            mgl_scompclean(ptcontext);
            mgl_delparser(ptparser);
            fclose(fp);
            EXIT(1);
        }
    }

    if (mode == 'C') {
        /* ---------------------------------------------------------------- */
        /* Saving the lofig pointer, creating a new one to allow the        */
        /* parsing of the figure in 'A' mode.                               */
        /* ---------------------------------------------------------------- */
        pt_lofig_tmp = pt_lofig;
        ptcontext->LOFIG = (lofig_list *) mbkalloc(sizeof(lofig_list));
        ptcontext->LOFIG->MODE = 'A';
        ptcontext->LOFIG->NAME = namealloc(figname);
        ptcontext->LOFIG->MODELCHAIN = NULL;
        ptcontext->LOFIG->LOINS = NULL;
        ptcontext->LOFIG->LOTRS = NULL;
        ptcontext->LOFIG->LOCON = NULL;
        ptcontext->LOFIG->LOSIG = NULL;
        mbk_init_NewBKSIG(&ptcontext->LOFIG->BKSIG);
        ptcontext->LOFIG->USER = NULL;
        ptcontext->LOFIG->NEXT = NULL;

        if (mgl_scompparse(ptparser) != 0) {
            fprintf(stderr, "\n*** mbk error *** abnormal parsing for : %s\n", ptcontext->FILENAME);
            FAST_MODE = local_mbk_fast_mode;
            mgl_scompclean(ptcontext);
            mgl_delparser(ptparser);
            fclose(fp);
            EXIT(1);
        }
        /* ---------------------------------------------------------------- */
        /* Now, with the new figure, we duplicate the new informations      */
        /* to fill the old one.                                             */
        /* ---------------------------------------------------------------- */
        pt_lofig = mgl_fill(pt_lofig_tmp, ptcontext->LOFIG);
    }

    FAST_MODE = local_mbk_fast_mode;
    mgl_delparser(ptparser);
    fclose(fp);
}
