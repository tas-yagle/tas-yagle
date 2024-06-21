/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPEF Version 1.00                                           */
/*    Fichier : spef100.h                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

//#define SPEF_DEBUG

#ifndef SPE
#define SPE


/* liste des inclusions */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

/* inclusions mbk */
#include MUT_H
#include MLO_H
#include RCN_H

#define SPEF_NODE_MIN  0x00000002
#define SPEF_INDEX     0x00079350
#define SPEF_FILE      0x00079351
#define SPEF_HEAP      0x00079352

// zinaps:
#define SPEF_CACHE_PTYPE 0x13311331

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/
typedef struct spef_info {
    char *SPEF;
    char *VENDOR;
    char *PROGRAM;
    char *VERSION;
    chain_list *DESIGN_FLOW;
    char DIVIDER;
    char DELIMITER;
    char PREFIX_BUS_DELIMITER;
    char SUFFIX_BUS_DELIMITER;
    char SPEF_T_UNIT;
    float SPEF_T_SCALE;
    char SPEF_CAP_UNIT;
    float SPEF_CAP_SCALE;
    char SPEF_RES_UNIT;
    float SPEF_RES_SCALE;
    char SPEF_L_UNIT;
    float SPEF_L_SCALE;
} spef_info;

extern spef_info *SPEF_INFO;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
extern void spef_drive(lofig_list *lofig, FILE *f);


extern int spef_error();
void parsespef(char *filename);
extern int spef_quiet;
#endif
