/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTVREN Version 1                                            */
/*    Fichier : ttvren.c                                                    */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/
#include "ttvren.h"

/****************************************************************************/
/*{{{                    ren_ttvfigren()                                    */
/*                                                                          */
/* renomme une ttvfig avec une extension                                    */
/*                                                                          */
/****************************************************************************/
void ren_ttvfigren(mask)
long        mask;
{
  chain_list    *chainx;
  ttvfig_list   *fig;
  ttvinfo_list  *info;
  chain_list    *ttvfiglist = NULL;
  ht            *htfig;
  char           newname[1024];
  timing_cell   *cell, *newcell;
    
  if ((mask&REN_OPT_HIER)==REN_OPT_HIER)
    ttvfiglist = addchain(ttvfiglist,(void *)TTVFIG);
  else
  {
    ttvfiglist = ttv_getttvfiglist(TTVFIG);
    ttvfiglist = reverse(ttvfiglist);
  }
  htfig = addht(1024);

  for(chainx = ttvfiglist; chainx; chainx = delchain(chainx,chainx))
  {
    fig   = chainx->DATA;
    info  = fig->INFO;

    if (gethtitem(htfig,info)==EMPTYHT)
    {
      addhtitem(htfig,info,(long)fig);
      
      if ((mask & REN_OPT_TTX)==REN_OPT_TTX)
        ttv_parsttvfig(fig,TTV_STS_TTX,TTV_FILE_TTX) ;
      if ((mask & REN_OPT_DTX)==REN_OPT_DTX)
        ttv_parsttvfig(fig,TTV_STS_DTX,TTV_FILE_DTX) ;
      if ( (mask & REN_OPT_STM)
           && (cell = stm_getcell(info->FIGNAME)))
        {
          sprintf(newname,"%s%s",cell->NAME,NEWNAME);
          newcell                   = stm_addcell(newname);
          newcell->MODEL_LIST       = reverse(cell->MODEL_LIST);
          newcell->MODEL_HT         = cell->MODEL_HT;
//          newcell->SIM_MODEL_LIST   = reverse(cell->SIM_MODEL_LIST);
//          newcell->SIM_MODEL_HT     = cell->SIM_MODEL_HT;
          newcell->MODEL_HTSIGN     = cell->MODEL_HTSIGN;

          stm_drivecell(newname);
        }

      sprintf(newname,"%s%s",info->FIGNAME,NEWNAME);
      info->FIGNAME = namealloc(newname);
      
      if ((mask&REN_OPT_TTX)==REN_OPT_TTX)
        ttv_drittv(fig,TTV_FILE_TTX,0, NULL);
      if ((mask&REN_OPT_DTX)==REN_OPT_DTX)
        ttv_drittv(fig,TTV_FILE_DTX,0, NULL);
    }
  }
  delht(htfig);
}

/*}}}************************************************************************/
