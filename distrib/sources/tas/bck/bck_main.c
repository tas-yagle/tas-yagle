/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SDF Version 1.00                                            */
/*    Fichier : sdf_main.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*    includes                                                              */
/****************************************************************************/

#include BCK_H
#include "bck_sdf.h"

/****************************************************************************/
/*    functions                                                             */
/****************************************************************************/

int main (int argc, char **argv) 
{
	lofig_list *lofig ;
	mbkenv () ;

	if (argc != 2) {
		printf ("usage : sdfparse <filename>\n") ;
		EXIT (EXIT_FAILURE) ;
	}
	
    lofig = bck_sdfparse (argv[1], getlofig (argv[1], 'A')) ;
	
	EXIT (EXIT_SUCCESS) ;

}
