/****************************************************************************/
/*                                                                          */
/*                          AVERTEC S.A. 2002                               */
/*                                                                          */
/*    Produit : cnsdiff                                                     */
/*    Fichier : cnsdiff.h                                                   */
/*    Auteur(s) : Romaric Thevenent                                         */
/*                                                                          */
/****************************************************************************/

/* les inclusions systemes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

/* inclusion mbk */

#include MUT_H
#include MLO_H
#include BEH_H
#include RCN_H
#include CNS_H
#include LOG_H

#ifdef AVERTEC
#include AVT_H
#endif

#define DIFF_OPT_FIRST           ((long) 0x00000201)
#define DIFF_OPT_ALL             ((long) 0x00000202)
#define DIFF_OPT_OUTFILE         ((long) 0x00000203)
#define DIFF_OPT_VERBOSE         ((long) 0x00000204)
#define DIFF_OPT_HELP            ((long) 0x00000205)
#define DIFF_OPT_NOOUT           ((long) 0x00000206)
#define DIFF_OPT_TEST            ((long) 0x00000207)

#define CNS_ERROR_USAGE          ((long) 0x00000101)
#define CNS_ERROR_OUTPUT         ((long) 0x00000102)
#define CNS_ERROR_CNSFIG         ((long) 0x00000103)
#define CNS_ERROR_HELP           ((long) 0x00000104)
#define CNS_ERROR_NOFILE         ((long) 0x00000105)





long        cnsdiff_getopt();
int         cnsdiff_makefigdiff();

