/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_main.c                                                  */
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

#include LIB_H

extern int   libparse() ;
extern FILE *libin ;
/****************************************************************************/
/*    functions                                                             */
/****************************************************************************/

int main (int argc, char **argv)
{
	mbkenv () ;
    elpenv();
    libenv();

    if((libin = fopen(argv[1], "r"))) {
        if(!libparse()) {
            fclose(libin) ;
        }
        else {
            fprintf(stderr, "error!!!!!!!\n") ;
            fclose(libin) ;
        }
    }

	EXIT (EXIT_SUCCESS) ;
}
