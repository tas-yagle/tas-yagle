/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TMA Version 1                                               */
/*    Fichier : tma_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*                                                                          */
/****************************************************************************/
#include "tma.h"
/****************************************************************************/

tma_context_list *TMA_CONTEXT = NULL;

/****************************************************************************/

int tmaenv(long type)
{
    tma_context_list *newcontext;
    char *str ;
    tma_context_list *next;

    newcontext = (tma_context_list *)mbkalloc(sizeof(tma_context_list));
    next = TMA_CONTEXT;
    if (TMA_CONTEXT != NULL) {
        *newcontext = *TMA_CONTEXT;
        TMA_CONTEXT = newcontext;
    }
    else {
        TMA_CONTEXT = newcontext;
        TMA_CONTEXT->TMA_TYPE_TTVFIG = 'T';
    } 
    newcontext->NEXT = next;

    if (type!=0)
    {
      if ((type & TTV_FILE_DTX)==TTV_FILE_DTX)
        TMA_CONTEXT->TMA_TYPE_TTVFIG = 'D';
      else if ((type & TTV_FILE_CTX)==TTV_FILE_CTX)
         TMA_CONTEXT->TMA_TYPE_TTVFIG = 'D';
      else
         TMA_CONTEXT->TMA_TYPE_TTVFIG = 'T';
    }
    else
    {
      if (V_BOOL_TAB[__TMA_DTXIN].VALUE == 1 ) 
          TMA_CONTEXT->TMA_TYPE_TTVFIG = 'D';
      else if (V_BOOL_TAB[__TMA_TTXIN].VALUE == 1 ) 
          TMA_CONTEXT->TMA_TYPE_TTVFIG = 'T';
      else if (V_BOOL_TAB[__TMA_CTXIN].VALUE == 1 ) 
          TMA_CONTEXT->TMA_TYPE_TTVFIG = 'D';
    }
    return 0;
}

