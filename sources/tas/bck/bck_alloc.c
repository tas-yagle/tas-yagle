/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BCK Version 1.00                                            */
/*    Fichier : bck_alloc.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*                                                                          */
/****************************************************************************/

#include "bck_alloc.h" 

/****************************************************************************/
/* defines                                                                  */
/****************************************************************************/

#define BCK_HEAPSIZE 128

/****************************************************************************/
/* globals                                                                  */
/****************************************************************************/

bck_translist *HEAP_TRANS = NULL ;
bck_delaylist *HEAP_DELAY = NULL ;
bck_checklist *HEAP_CHECK = NULL ;

chain_list    *HEAP_TRANS_CHAIN = NULL ;
chain_list    *HEAP_DELAY_CHAIN = NULL ;
chain_list    *HEAP_CHECK_CHAIN = NULL ;

/****************************************************************************/
/* bck_translist                                                            */
/****************************************************************************/

bck_translist *bck_addtrans (bck_translist *head)
{
	int i ;
	bck_translist *trans ;
	
  	if (!HEAP_TRANS) {
    	HEAP_TRANS = (bck_translist*)mbkalloc (BCK_HEAPSIZE * sizeof (struct bck_trans)) ;
		HEAP_TRANS_CHAIN = addchain (HEAP_TRANS_CHAIN, HEAP_TRANS) ;
    	trans = HEAP_TRANS ;
    	for (i = 1 ; i < BCK_HEAPSIZE ; i++) {
      		trans->NEXT = trans + 1 ;
      		trans++ ;
    	}
    	trans->NEXT = NULL ;
  	}
  	trans = HEAP_TRANS ;
  	HEAP_TRANS = HEAP_TRANS->NEXT ;
	
	trans->NEXT          = head ;
	trans->EVENT1        = EVNO ;
	trans->EVENT2        = EVNO ;
	trans->VALUE         = NO_VALUE ;

	return trans ;
}

/****************************************************************************/

void bck_freetrans (bck_translist *head) 
{
	bck_translist *trans ;

	if (head) {
		for (trans = head ; trans->NEXT ; trans = trans->NEXT) ;
		trans->NEXT = HEAP_TRANS ;
		HEAP_TRANS = head ;
	}
}

/****************************************************************************/

bck_translist *bck_deltrans (bck_translist *head, bck_translist *del) 
{
	bck_translist *trans ;
	bck_translist *prev = NULL ;
	
	trans = head ;

	if (head == del) {
		head = head->NEXT ;
		del->NEXT = HEAP_TRANS ;
		HEAP_TRANS = del ;
		return head ;
	} 

	prev = head ;
	trans = head->NEXT ;
	
	while (trans) {
		if (trans == del) {
			prev->NEXT = del->NEXT ;
			del->NEXT = HEAP_TRANS ;
			HEAP_TRANS = del ;
			return head ;
		}
		prev = prev->NEXT ;
		trans = trans->NEXT ;
	}

	return NULL ; /* not found */
}

/****************************************************************************/

void bck_freetransheaps ()
{
	chain_list *pt ;
	
	for (pt = HEAP_TRANS_CHAIN ; pt ; pt = pt->NEXT)
		mbkfree(pt->DATA) ;

	freechain (HEAP_TRANS_CHAIN) ;
    HEAP_TRANS_CHAIN=NULL;
}

/****************************************************************************/
/* bck_delaylist                                                            */
/****************************************************************************/

bck_delaylist *bck_adddelay (bck_delaylist *head, long type)
{
	int i ;
	bck_delaylist *delay ;
	
  	if (!HEAP_DELAY) {
    	HEAP_DELAY = (bck_delaylist*)mbkalloc (BCK_HEAPSIZE * sizeof (struct bck_delay)) ;
		HEAP_DELAY_CHAIN = addchain (HEAP_DELAY_CHAIN, HEAP_DELAY) ;
    	delay = HEAP_DELAY ;
    	for (i = 1 ; i < BCK_HEAPSIZE ; i++) {
      		delay->NEXT = delay + 1 ;
      		delay++;
    	}
    	delay->NEXT = NULL ;
  	}
  	delay = HEAP_DELAY ;
  	HEAP_DELAY = HEAP_DELAY->NEXT ;
	
	delay->NEXT          = head ;
	delay->TYPE          = type ;
	delay->START         = NULL ;
	delay->END           = NULL ;
	delay->COND          = NULL ;
	delay->TRANSLIST     = NULL ; 

	return delay ;
}

/****************************************************************************/

void bck_freedelays (bck_delaylist *head) 
{
	bck_delaylist *delay ;

	if (head) {
		for (delay = head ; delay->NEXT ; delay = delay->NEXT)
			 bck_freetrans(delay->TRANSLIST) ;
		delay->NEXT = HEAP_DELAY ;
		HEAP_DELAY = head ;
	}
}

/****************************************************************************/

void bck_freedelayheaps ()
{
	chain_list *pt ;
	
	for (pt = HEAP_DELAY_CHAIN ; pt ; pt = pt->NEXT)
		mbkfree(pt->DATA) ;

	freechain (HEAP_DELAY_CHAIN) ;
    HEAP_DELAY_CHAIN=NULL;
}

/****************************************************************************/
/* bck_checklist                                                            */
/****************************************************************************/

bck_checklist *bck_addcheck (bck_checklist *head, long type)
{
	int i ;
	bck_checklist *check ;
	
  	if (!HEAP_CHECK) {
    	HEAP_CHECK = (bck_checklist*)mbkalloc (BCK_HEAPSIZE * sizeof (struct bck_check)) ;
		HEAP_CHECK_CHAIN = addchain (HEAP_CHECK_CHAIN, HEAP_CHECK) ;
    	check = HEAP_CHECK ;
    	for (i = 1 ; i < BCK_HEAPSIZE ; i++) {
      		check->NEXT = check + 1 ;
      		check++;
    	}
    	check->NEXT = NULL ;
  	}
  	check = HEAP_CHECK ;
  	HEAP_CHECK = HEAP_CHECK->NEXT ;
	
	check->NEXT          = head ;
	check->TYPE          = type ;
	check->COMMAND       = NULL ;
	check->DATA          = NULL ;
	check->COND          = NULL ;
	check->TRANSLIST     = NULL ; 

	return check ;
}

/****************************************************************************/

void bck_freechecks (bck_checklist *head) 
{
	bck_checklist *check ;

	if (head) {
		for (check = head ; check->NEXT ; check = check->NEXT)
			 bck_freetrans(check->TRANSLIST) ;
		check->NEXT = HEAP_CHECK ;
		HEAP_CHECK = head ;
	}
}

/****************************************************************************/

void bck_freecheckheaps ()
{
	chain_list *pt ;
	
	for (pt = HEAP_CHECK_CHAIN ; pt ; pt = pt->NEXT)
		mbkfree(pt->DATA) ;

	freechain (HEAP_CHECK_CHAIN) ;
    HEAP_CHECK_CHAIN=NULL;
}

/****************************************************************************/
/* bck_annot                                                                */
/****************************************************************************/

bck_annot *bck_addannot ()
{
	bck_annot *annot = (bck_annot*)mbkalloc (sizeof (struct bck_annot)) ;

	annot->DELAYS = NULL ;
	annot->CHECKS = NULL ;
	annot->ENVIRONMENT = NULL ;
	
	return annot ;
}

/****************************************************************************/

void bck_freeall ()
{
	bck_freecheckheaps () ;
	bck_freedelayheaps () ;
	bck_freetransheaps () ;
}
