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

#include SPE_H
extern int   spefparse() ;
extern FILE *spefin ;

/****************************************************************************/
/*    functions                                                             */
/****************************************************************************/

int main (int argc, char **argv) 
{

    mbkenv();
    if (argc != 2) {
        printf ("usage : spef <filename>\n") ;
        EXIT (EXIT_FAILURE) ;
    }
   
    if ((spefin = fopen (argv[1], "r"))) {
        spefparse () ;
        fclose (spefin) ; 
    } else { 
            fprintf (stderr, "SPEF ERROR : no file %s\n", argv[2]);
            EXIT (EXIT_FAILURE) ;
       }
    
   
    EXIT (EXIT_SUCCESS) ;

}
