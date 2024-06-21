/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SDF Version 1.00                                            */
/*    Fichier : bck_sdf.c                                                   */
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
#include "bck_sdf.h" 

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

int         LINE ;
int         PROCESS = TYPICAL ;
lofig_list *LOFIG   = NULL ;

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern int   sdfparse() ;
extern FILE *sdfin ;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

lofig_list *bck_sdfparse (char *sdfname, lofig_list *lofig)
{
    if (!lofig) return NULL;

	LOFIG = lofig ; 
	lofigchain (LOFIG) ;

	if (TRACE_MODE == 'Y')
		fprintf (stdout, "--- SDF --- Parsing %s\n", sdfname) ;

    if ((sdfin = mbkfopen (sdfname, "sdf", READ_TEXT))) {
    	LINE = 1 ;
    	sdfparse () ;
		if (TRACE_MODE == 'Y')
			fprintf (stdout, "--- SDF --- Parsing ok\n") ;
    	fclose (sdfin) ; 
		return LOFIG ;
	} 

	fprintf (stderr, "sdf error : cannot open SDF file %s\n", sdfname) ;	
	EXIT (EXIT_FAILURE) ;
}
