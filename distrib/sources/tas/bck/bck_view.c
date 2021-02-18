/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BCK Version 1.00                                            */
/*    Fichier : bck_view.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*                                                                          */
/****************************************************************************/

#include "bck_view.h" 

/****************************************************************************/
/* defines                                                                  */
/****************************************************************************/

/****************************************************************************/
/* globals                                                                  */
/****************************************************************************/

/****************************************************************************/
/* bck_viewtrans                                                            */
/****************************************************************************/

void bck_viewtrans (bck_translist *trans)
{
	switch (trans->EVENT1) {
		case EVNO :  
			fprintf (stdout, "_NO_ ") ;
			break ;
		case EV01 :  
			fprintf (stdout, "_01_ ") ;
			break ;
		case EV10 :  
			fprintf (stdout, "_10_ ") ;
			break ;
		case EV0Z :  
			fprintf (stdout, "_0z_ ") ;
			break ;
		case EVZ0 :  
			fprintf (stdout, "_z0_ ") ;
			break ;
		case EV1Z :  
			fprintf (stdout, "_1z_ ") ;
			break ;
		case EVZ1 :  
			fprintf (stdout, "_z1_ ") ;
			break ;
		case EV__ :  
			fprintf (stdout, "____ ") ;
			break ;
		case POSEDGE :
			fprintf (stdout, "_posedge_ ") ;
			break ;
		case NEGEDGE :
			fprintf (stdout, "_negedge_ ") ;
			break ;
	}
	switch (trans->EVENT2) {
		case EVNO :  
			fprintf (stdout, "_NO_ ") ;
			break ;
		case EV01 :  
			fprintf (stdout, "_01_ ") ;
			break ;
		case EV10 :  
			fprintf (stdout, "_10_ ") ;
			break ;
		case EV0Z :  
			fprintf (stdout, "_0z_ ") ;
			break ;
		case EVZ0 :  
			fprintf (stdout, "_z0_ ") ;
			break ;
		case EV1Z :  
			fprintf (stdout, "_1z_ ") ;
			break ;
		case EVZ1 :  
			fprintf (stdout, "_z1_ ") ;
			break ;
		case EV0X :  
			fprintf (stdout, "_0x_ ") ;
			break ;
		case EVX0 :  
			fprintf (stdout, "_x0_ ") ;
			break ;
		case EV1X :  
			fprintf (stdout, "_1x_ ") ;
			break ;
		case EVX1 :  
			fprintf (stdout, "_x1_ ") ;
			break ;
		case EVZX :  
			fprintf (stdout, "_zx_ ") ;
			break ;
		case EVXZ :  
			fprintf (stdout, "_xz_ ") ;
			break ;
		case EV_Z :  
			fprintf (stdout, "__z_ ") ;
			break ;
		case EV__ :  
			fprintf (stdout, "____ ") ;
			break ;
		case POSEDGE :
			fprintf (stdout, "_posedge_ ") ;
			break ;
		case NEGEDGE :
			fprintf (stdout, "_negedge_ ") ;
			break ;
	}

}

/****************************************************************************/
/* bck_viewdelay                                                            */
/****************************************************************************/

void bck_viewdelay (bck_delaylist *delay)
{
	bck_translist *trans ;

	if (delay->TYPE & IOPATH) 
		fprintf (stdout, "IOPATH ") ;
	if (delay->TYPE & INTERCONNECT) 
		fprintf (stdout, "INTERCONNECT ") ;

	if (delay->START)
		//fprintf (stdout, "IOPATH %s ", delay->START->NAME) ;
		fprintf (stdout, "%s ", (char*)delay->START) ;
	else
		fprintf (stdout, "start => ") ;
		
	if (delay->END)
		//fprintf (stdout, "%s / ", delay->END->NAME) ;
		fprintf (stdout, "%s ", (char*)delay->END) ;
	else
		fprintf (stdout, "end ") ;
		
	for (trans = delay->TRANSLIST ; trans ; trans = trans->NEXT) {
		fprintf (stdout, "( ") ;
		bck_viewtrans (trans) ;
		fprintf (stdout, "%ld ) ", trans->VALUE) ;
	}
	fprintf (stdout, "\n") ;
}

/****************************************************************************/
/* bck_viewcheck                                                            */
/****************************************************************************/

void bck_viewcheck (bck_checklist *check)
{
	bck_translist *trans ;

	switch (check->TYPE) {
		case SETUP : 
			fprintf (stdout, "SETUP ") ;
			break ;
		case HOLD :   
			fprintf (stdout, "HOLD ") ;
			break ;
		case RECOVERY : 
			fprintf (stdout, "RECOVERY ") ;
			break ;
		case REMOVAL :
			fprintf (stdout, "REMOVAL ") ;
			break ;
	}

	if (check->DATA)
		//fprintf (stdout, "%s ", check->DATA->NAME) ;
		fprintf (stdout, "%s ", (char*)check->DATA) ;
	else
		fprintf (stdout, "data => ") ;
		
	if (check->COMMAND)
		//fprintf (stdout, "%s ", check->COMMAND->NAME) ;
		fprintf (stdout, "%s ", (char*)check->COMMAND) ;
	else
		fprintf (stdout, "command ") ;
		
	for (trans = check->TRANSLIST ; trans ; trans = trans->NEXT) {
		fprintf (stdout, "( ") ;
		bck_viewtrans (trans) ;
		fprintf (stdout, "%ld ) ", trans->VALUE) ;
	}
	fprintf (stdout, "\n") ;
}
	
/****************************************************************************/
/* bck_view                                                                 */
/****************************************************************************/

void bck_view (bck_annot *annot) 
{
	bck_delaylist *delay ;
	bck_checklist *check ;

	for (delay = annot->DELAYS ; delay ; delay = delay->NEXT) 
		bck_viewdelay (delay) ;
	
	for (check = annot->CHECKS ; check ; check = check->NEXT) 
		bck_viewcheck (check) ;
	
	fprintf (stdout, "\n") ;
}
