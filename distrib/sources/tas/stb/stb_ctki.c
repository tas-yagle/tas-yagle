/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_ctki.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                Grégoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>


#include MUT_H
#include STB_H
#include RCN_H
#include TRC_H
#include TTV_H

#include "stb_ctk.h"
#include "stb_ctki.h"

int stbi_fillagrlist( stbfig_list *stbfig,
                      lofig_list  *lofig,
                      losig_list   *victime,
                      char          sens,
                      chain_list   *headagr
                    )
{
  ttvsig_list   *ttvsig ;
  ttvevent_list *ptevent ;

  ttvsig = ttv_getttvsig( stbfig->FIG,
                          stbfig->FIG->INFO->LEVEL,
                          TTV_FIND_LINE,
                          stbfig->FIG->INSNAME,
                          victime,
                          1
                        );
  if( !ttvsig )
    return 0;
    
  switch( sens )
  {
  case RCXI_DW:
    ptevent = & (ttvsig->NODE[0]) ;
    break ;
  case RCXI_UP:
    ptevent = & (ttvsig->NODE[1]) ;
    break ;
  default :
    return 0;
  }
  
  stb_fillttvsigrcxparam( stbfig->FIG, 
                          stbfig->FIG->INFO->LEVEL, 
                          TTV_FIND_LINE | TTV_FIND_MIN | TTV_FIND_MAX, 
                          headagr 
                        ) ;
  if( ( stbfig->CTKMODE & STB_CTK_WORST ) == STB_CTK_WORST )
    stb_fillinactifrcxparam( stbfig->FIG, ptevent, headagr, 'N' );
  else
    stb_fillactifrcxparam( stbfig->FIG, ptevent, headagr, 'N' );

  lofig = NULL;
  
  return 1;
}
