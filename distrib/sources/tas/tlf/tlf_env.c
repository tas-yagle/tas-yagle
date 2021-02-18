/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : TLF Version 1.00                                            */
/*    Fichier : tlf_env.c                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#include TLF_H

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

char *TLF_IN = "tlf" ;
long TLF_BUFSIZE = 1024;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void tlfenv ()
{
    char *env ;
    static int doneenv=0;
    
    
    stmenv () ;
    
    if (doneenv==0) {
      readlibfile("tlf3", parsetlf3, 0);
      readlibfile("tlf4", parsetlf4, 0);
    }   
    doneenv=1;
}


void tlf_exit () 
{
    stm_exit () ;
}
