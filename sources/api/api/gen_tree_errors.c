/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : Count_Errors.c                                              */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 26/04/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include MUT_H

#define MAX_ERR 30              /* num of errors allowed */

static int num_err=0;           /* number of errors */


/***************************************************************************/
/*              Increment and return number of errors                      */
/*          If too many errors dump a message and stop the program         */
/***************************************************************************/
extern int Inc_Error() {
  
  num_err++;
  if (num_err>MAX_ERR) {
      fprintf(stderr,"other errors follow...\n");
      EXIT(3);
  }    
  return num_err;
}


/***************************************************************************/
/*                return the number of error                               */
/***************************************************************************/
extern int Get_Error() {
   return num_err;
}

