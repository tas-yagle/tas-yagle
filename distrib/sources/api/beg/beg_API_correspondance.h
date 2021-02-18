/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : beg_API_correspondance.h                                    */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef BEG_API_CORRESPONDANCE_H
#define BEG_API_CORRESPONDANCE_H

#ifndef BEGAPI_HT_BASE
#define BEGAPI_HT_BASE       13
#endif

#include MUT_H
#include MLO_H
//#include BEH_H
//#include BVL_H
#include BHL_H
#include BEG_H
//#include BEF_H
#include GEN_H
//#include LOG_H

#include "gen_API.h"
//#include "mbk_API.h"
//extern 					BEGAPI_NO_CORRESP;

void begUpdateChanges(ht *org, ht *dest, chain_list *before_change, chain_list *after_change);
void begWriteCorrespondanceList(char *name);
chain_list *begGetNamesRad(befig_list *befig);
chain_list *begGetNames(befig_list *befig);

void  begAddCorresp(char *curbefig, char *name);
void begTransfertCorresp(char *dest, char *source, int update);
void begTraceAndCorresp(ht *corresp_ht, char *name, int left, int right, int trace, int corresp);
void begAddCorrespName(ht *htb, char *key, char *name);
void begUpdateCorresp(ht *h);
void begInitCorrespondance(void);

#endif
