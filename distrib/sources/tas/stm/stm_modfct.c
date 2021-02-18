/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modfct.c                                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
/*{{{                    stm_modfct_alloc()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static timing_function *stm_modfct_alloc(char *ins, char *localname,
                                         APICallFunc *cf)
{
  timing_function   *tfct;
  
  tfct  = mbkalloc(sizeof(struct timing_function));

  tfct->INS         = ins;
  tfct->LOCALNAME   = localname;
  tfct->FCT         = (APICallFunc *)mbkalloc(sizeof(APICallFunc));
  tfct->FCT->NAME   = cf->NAME;
  tfct->FCT->ARGS   = APIDupARGS(cf->ARGS);
//  tfct->CACHE       = NULL;
  tfct->SHARE       = NULL;

  return tfct;
}

/*}}}************************************************************************/
/* function stm_modfct_duplicate ()                                         */
/****************************************************************************/

timing_function *stm_modfct_duplicate (timing_function *fct)
{
  timing_function   *res;

  if (!fct)
    return NULL;
  else
  {
    res     = stm_modfct_alloc(fct->INS,fct->LOCALNAME,fct->FCT);
//    stm_modfct_alloc_cache(res,stm_getCache(fct));
    stm_modfct_share(res,fct);
    
    return res;
  }
}

/****************************************************************************/
/* function stm_modfct_create ()                                            */
/****************************************************************************/

timing_function *stm_modfct_create (char *ins, char *localname, APICallFunc *cf)
{
    return stm_modfct_alloc(namealloc(ins),namealloc(localname),cf);
}

/****************************************************************************/
/* function stm_modfct_destroy ()                                           */
/****************************************************************************/

void stm_modfct_destroy (timing_function *fct)
{
    if (!fct) return;

    APIFreeTARGS(fct->FCT->ARGS);
//    stm_freeCache(fct->CACHE);
    stm_modfct_unShare(fct->SHARE);
    mbkfree(fct->FCT);
    mbkfree(fct);
}

/****************************************************************************/
