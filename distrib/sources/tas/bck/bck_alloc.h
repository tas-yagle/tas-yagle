/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BCK Version 1.00                                            */
/*    Fichier : bck_alloc.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*                                                                          */
/****************************************************************************/

#include BCK_H

extern bck_translist *bck_addtrans   (bck_translist *head) ;
extern bck_translist *bck_deltrans   (bck_translist *head, bck_translist *del) ;
extern void           bck_freetrans  (bck_translist *head) ;
extern bck_delaylist *bck_adddelay   (bck_delaylist *head, long type) ;
extern bck_checklist *bck_addcheck   (bck_checklist *head, long type) ;
extern bck_annot     *bck_addannot   () ;
extern void           bck_freetrans  (bck_translist *trans) ;
extern void           bck_freedelays (bck_delaylist *delays) ;
extern void           bck_freechecks (bck_checklist *checks) ;
extern void           bck_freeannot  (bck_annot     *annot) ;
