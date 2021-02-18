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

#include TLF_H
#include "tlf_parse.h"

/****************************************************************************/
/*    functions                                                             */
/****************************************************************************/

int main (int argc, char **argv) 
{
    chain_list *ttvch;
    
    mbkenv () ;
    elpenv ();
    mccenv () ;
    tlfenv() ;

    if (argc != 3) {
        printf ("usage : tlftr <filename> version\n") ;
        EXIT (EXIT_FAILURE) ;
    }
   
    
    ttvch = tlf_load (argv[1], atoi(argv[2]));
    
    
    tlf_exit () ;
    
    
    
    EXIT (EXIT_SUCCESS) ;

}
