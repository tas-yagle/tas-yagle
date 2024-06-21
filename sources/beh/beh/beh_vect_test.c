/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BEHVECT Version 1.00                                        */
/*    Fichier : behvect.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <setjmp.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifndef __P
# if defined(__STDC__) ||  defined(__GNUC__)
#  define __P(x) x
# else
#  define __P(x) ()
# endif
#endif

#include BEH_H
#include MUT_H
#include LOG_H

int main(argc, argv)
int     argc;
char    *argv[];
{
  chain_list *abl1  = NULL;
  chain_list *abl2  = NULL;
  chain_list *abl3  = NULL;
  chain_list *abl4  = NULL;
  chain_list *abl5  = NULL;
  chain_list *chainx;

  mbkenv();               /* MBK environment */


  abl1 = addchain(abl1,addchain(NULL,namealloc("B")));
  abl1 = addchain(abl1,addchain(NULL,namealloc("D")));
  abl1 = addchain(abl1,addchain(NULL,OR));
  displayExpr(abl1);

  abl2 = addchain(abl2,addchain(NULL,namealloc("T")));
  abl2 = addchain(abl2,addchain(NULL,namealloc("F")));
  abl2 = addchain(abl2,addchain(NULL,AND));
  displayExpr(abl2);

  abl3 = concatAbl(abl1,abl2);
  displayExpr(abl3);

  abl4 = concatAbl(abl3,addchain(NULL,namealloc("A")));
  displayExpr(abl4);
  displayExpr(getAblAtPos(abl4,1));

  abl5 = addchain(NULL,namealloc("b 6:4"));
  displayExpr(abl5);
  displayExpr(getAblAtPos(abl5,1));

  abl5 = addchain(NULL,namealloc("b 2:4"));
  displayExpr(abl5);
  displayExpr(getAblAtPos(abl5,1));

  abl4 = addchain(NULL,abl4);
  abl4 = addchain(abl4,abl5);
  abl4 = addchain(abl4,addchain(NULL,OR));
  displayExpr(abl4);
  displayExpr(getAblAtPos(abl4,1));

  abl4 = replicateAbl(abl4,3);
  displayExpr(abl4);
  chainx = supportChain_listExpr(abl4);
  for (;chainx;chainx=chainx->NEXT)
    printf("%s ",chainx->DATA);
  printf("\n");

  displayExpr(getAblAtPos(abl4,4));
  displayExpr(getAblAtPos(abl4,6));

  abl4 = replicateAbl(abl2,3);
  displayExpr(abl4);
  displayExpr(getAblAtPos(abl4,1));
  

  EXIT(EXIT_SUCCESS);
}
