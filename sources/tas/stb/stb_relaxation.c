/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_relaxation.c                                            */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>

#include MUT_H
#include STB_H

#include "stb_util.h"
#include "stb_init.h"
#include "stb_error.h"
#include "stb_transfer.h"
#include "stb_relaxation.h"
#include "stb_relaxation_correction.h"

/*****************************************************************************
*                           fonction stb_relaxation()                        *
*****************************************************************************/

int stb_relaxation(stbfig)
stbfig_list *stbfig;
{
 ttvevent_list *ptevent;
 stbnode       *node;
 chain_list     *chain;
 int            i = 0;
 char           oldsilent ;
 char           namebuf[2048];
 char           message[2048];

 stbfig->CHANGEFLAG = 1 ;

 oldsilent = STB_SILENT ;

 stb_clean_relax_correction_info(stbfig);
 for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
 {
   ptevent = (ttvevent_list *)chain->DATA;
   node = stb_getstbnode (ptevent) ;
   node->FLAG&=~STB_NODE_STABCORRECT;
 }
 stb_set_relax(1);
 
 while(stbfig->CHANGEFLAG != 0)
   {
#ifdef DELAY_DEBUG_STAT
    fprintf(stdout,"\r(it %d, %ld)\r", i, stbfig->CHANGEFLAG);
    fflush(stdout);
#endif
    stbfig->CHANGEFLAG = 0 ;
    stbfig->STABILITYFLAG = STB_STABLE ;

    if (i==500)
     avt_errmsg(STB_ERRMSG, "048", AVT_WARNING, i);

    if (STB_TRACE_MODE == 'Y')
      {
       printf("\nStarting iteration %d:\n\n", i);
      }


    if (i==0) stb_set_speed_up(0);
    else stb_set_speed_up(1);

    for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
      {
       ptevent = (ttvevent_list *)chain->DATA;
       if (i==1 && (((ptevent->ROOT->TYPE & TTV_SIG_CI)==TTV_SIG_CI
                     || (ptevent->ROOT->TYPE & TTV_SIG_CB)==TTV_SIG_CB))
           )
           ptevent->TYPE|=TTV_NODE_MARK_1;

       stb_transferstbnode(stbfig,ptevent,STB_RC_DELAY) ;

       if (STB_TRACE_MODE == 'Y')
         {
          stb_viewstbnode(stbfig, ptevent);
         }
      }
    STB_SILENT = 'Y' ;
    i++;
   }

 stb_set_speed_up(0);
 stb_set_relax(0);

#ifdef DELAY_DEBUG_STAT
 fprintf(stdout,"\n");
#endif

  STB_SILENT = oldsilent ;

  for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
    {
     ptevent = (ttvevent_list *)chain->DATA;
     ptevent->TYPE&=~TTV_NODE_MARK_1;
     node = stb_getstbnode (ptevent) ;
     if(node)
      if(node->FLAG == STB_NODE_NODATA)
       {
         ttv_getsigname (stbfig->FIG, namebuf, node->EVENT->ROOT);
        if ((ptevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
         sprintf (message, "'%s' rising", namebuf);
        else
         sprintf (message, "'%s' falling", namebuf);
		stb_error (ERR_NO_DATA, message, 0, STB_WARNING);
       }
    }

  stb_clean_relax_correction_path_info(stbfig);
  
  return i;
}
