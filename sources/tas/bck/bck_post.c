/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SDF Version 1.00                                            */
/*    Fichier : bck_post.c                                                  */
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

#include "bck_post.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

bck_translist *bck_expandrctrans (bck_translist *head)
{
	bck_translist *trans, *newtrans = NULL ;

	for (trans = head ; trans ; trans = trans->NEXT) {
		if ((trans->EVENT1 == EV__) && (trans->EVENT2 != EV__)) {
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = trans->EVENT2 ;
			newtrans->EVENT2 = trans->EVENT2 ;
			newtrans->VALUE  = trans->VALUE ;
			continue ;
		} 
		if ((trans->EVENT1 != EV__) && (trans->EVENT2 == EV__)) {
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = trans->EVENT1 ;
			newtrans->EVENT2 = trans->EVENT1 ;
			newtrans->VALUE  = trans->VALUE ;
			continue ;
		} 
		if ((trans->EVENT1 == EV__) && (trans->EVENT2 == EV__)) {
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = EV01 ;
			newtrans->EVENT2 = EV01 ;
			newtrans->VALUE  = trans->VALUE ;
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = EV10;
			newtrans->EVENT2 = EV10 ;
			newtrans->VALUE  = trans->VALUE ;
			continue ;
		} 
		if ((trans->EVENT1 != EV__) && (trans->EVENT2 != EV__)) {
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = trans->EVENT1 ;
			newtrans->EVENT2 = trans->EVENT2 ;
			newtrans->VALUE  = trans->VALUE ;
			continue ;
		} 
	}

	bck_freetrans (trans) ;
	return newtrans ;
}

/******************************************************************************/

bck_translist *bck_expandiotrans (bck_translist *head)
{
	bck_translist *trans, *newtrans = NULL ;

	for (trans = head ; trans ; trans = trans->NEXT) {
		if ((trans->EVENT1 == EV__) && (trans->EVENT2 != EV__)) {
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = EV01 ;
			newtrans->EVENT2 = trans->EVENT2 ;
			newtrans->VALUE  = trans->VALUE ;
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = EV10 ;
			newtrans->EVENT2 = trans->EVENT2 ;
			newtrans->VALUE  = trans->VALUE ;
			continue ;
		} 
		if ((trans->EVENT1 != EV__) && (trans->EVENT2 == EV__)) {
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = trans->EVENT2 ;
			newtrans->EVENT2 = EV01 ;
			newtrans->VALUE  = trans->VALUE ;
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = trans->EVENT2 ;
			newtrans->EVENT2 = EV10 ;
			newtrans->VALUE  = trans->VALUE ;
			continue ;
		} 
		if ((trans->EVENT1 == EV__) && (trans->EVENT2 == EV__)) {
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = EV01 ;
			newtrans->EVENT2 = EV01 ;
			newtrans->VALUE  = trans->VALUE ;
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = EV10 ;
			newtrans->EVENT2 = EV01 ;
			newtrans->VALUE  = trans->VALUE ;
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = EV01;
			newtrans->EVENT2 = EV10 ;
			newtrans->VALUE  = trans->VALUE ;
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = EV10;
			newtrans->EVENT2 = EV10 ;
			newtrans->VALUE  = trans->VALUE ;
			continue ;
		} 
		if ((trans->EVENT1 != EV__) && (trans->EVENT2 != EV__)) {
			newtrans = bck_addtrans (newtrans) ;
			newtrans->EVENT1 = trans->EVENT1 ;
			newtrans->EVENT2 = trans->EVENT2 ;
			newtrans->VALUE  = trans->VALUE ;
			continue ;
		} 
	}

	bck_freetrans (trans) ;
	return newtrans ;
}

/******************************************************************************/

void bck_expandtrans (lofig_list *lofig)
{

	loins_list    *loins ;
	losig_list    *losig ;
	bck_annot     *annot ;
	bck_delaylist *delay ;
	bck_checklist *check ;
	ptype_list    *ptype ;
	
	for (loins = lofig->LOINS ; loins ; loins = loins->NEXT) {
		ptype = getptype (loins->USER, PTYPE_BCK_INS) ;
		if (ptype) {
			annot = (bck_annot*)ptype->DATA ;
			for(delay = annot->DELAYS ; delay != NULL ; delay = delay->NEXT) {
				delay->TRANSLIST = bck_expandiotrans (delay->TRANSLIST) ;
			}
			for(check = annot->CHECKS ; check != NULL ; check = check->NEXT) {
				check->TRANSLIST = bck_expandiotrans (check->TRANSLIST) ;
			}
		}
	}

	for (losig = lofig->LOSIG ; losig ; losig = losig->NEXT) {
		ptype = getptype (losig->USER, PTYPE_BCK_RC) ;
		if (ptype) {
			for(delay = (bck_delaylist*)ptype->DATA ; delay != NULL ; delay = delay->NEXT) {
			delay->TRANSLIST = bck_expandrctrans (delay->TRANSLIST) ;
			}
		}
	}
}

