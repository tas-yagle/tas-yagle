/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SDF Version 1.00                                            */
/*    Fichier : bck_post.h                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include BCK_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern bck_translist *bck_expandrctrans (bck_translist *head) ;
extern bck_translist *bck_expandiotrans (bck_translist *head) ;
extern void           bck_expandtrans   (lofig_list *lofig) ;
