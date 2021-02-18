/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Fichier : stm_modfct_share.c                                          */
/*                                                                          */
/*    © copyright 2003 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "stm.h"

/****************************************************************************/

static ht   *SHARED_ELT_HT     = NULL;
static ht   *SHARED_CLEANNING  = NULL;

static stm_share *stm_modfct_cpyShare   (stm_share *share);

/****************************************************************************/
/*{{{                    stm_modfct_freeShare()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int stm_modfct_freeShare(stm_share *share)
{
  ptype_list    *ptypex;
  chain_list    *pt;
  void          (*func)(void *);
  int            res = 1;
  
  for (ptypex = share->USER; ptypex; ptypex = ptypex->NEXT)
  {
    pt      = (void *)gethtitem(SHARED_CLEANNING,(void *)ptypex->TYPE);
    if ((long)pt == EMPTYHT)
      res       = 0;
    else
    {
      func      = (void*)pt->DATA;
      if (!pt->NEXT)
      {
        delchain(pt,pt);
        delhtitem(SHARED_CLEANNING,(void *)ptypex->TYPE);
        if (!SHARED_CLEANNING->count)
        {
          delht(SHARED_CLEANNING);
          SHARED_CLEANNING  = NULL;
        }
      }
      else
        pt->NEXT  = (void*)(((long)pt->NEXT) - 1);
      func(ptypex->DATA);
    }
  }
  mbkfree(share);
  delhtitem(SHARED_ELT_HT,share);
  if (!SHARED_ELT_HT->count)
  {
    delht(SHARED_ELT_HT);
    SHARED_ELT_HT = NULL;
  }
  
  return res;
}

/*}}}************************************************************************/
/*{{{                    stm_modfct_addShareCleanning()                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int stm_modfct_addShareCleanning(long type, void(*func)(void *))
{
  int            res = 1;
  chain_list    *pt;

  if (!SHARED_CLEANNING)
    SHARED_CLEANNING    = addht(50);
  if ((long)(pt = (chain_list *)gethtitem(SHARED_CLEANNING,(void *)type)) == EMPTYHT)
    addhtitem(SHARED_CLEANNING,(void *)type,(long)addchain(0,func));
  else if (pt->DATA != func)
    res     = 0;
  else
    pt->NEXT    = (void *)((long)pt->NEXT + 1L);
  
  return res;
}

/*}}}************************************************************************/
/*{{{                    stm_modfct_getSharedType()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
ptype_list *stm_modfct_getSharedType(timing_function *fct, long type)
{
  stm_share     *share;

  share         = stm_modfct_getShared(fct);
  return getptype(share->USER,type);
}

/*}}}************************************************************************/
/*{{{                    stm_modfct_addSharedType()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
ptype_list *stm_modfct_addSharedType(timing_function *fct, long type,
                                     void *data, void (*func)(void *))
{
  stm_share     *share;

  share         = stm_modfct_getShared(fct);
  share->USER   = addptype(share->USER,type,data);
  stm_modfct_addShareCleanning(type, func);

  return share->USER;
}

/*}}}************************************************************************/
/*{{{                    stm_modfct_unShare()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int stm_modfct_unShare(stm_share *share)
{
  int        nb, res = 1;
  
  if ( SHARED_ELT_HT && share)
    switch (nb = gethtitem(SHARED_ELT_HT,share))
    {
      case 0 :
           res  = stm_modfct_freeShare(share);
      case EMPTYHT :
           break;
      default :
           sethtitem(SHARED_ELT_HT,share,nb-1);
    }
  
  return res;
}

/*}}}************************************************************************/
/*{{{                    stm_modfct_newShare()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
stm_share *stm_modfct_newShare(void)
{
  stm_share   *share;

  share           = mbkalloc(sizeof(struct stm_share));
  share->USER     = NULL;

  if (!SHARED_ELT_HT)
    SHARED_ELT_HT = addht(50);
  addhtitem(SHARED_ELT_HT,share,0);

  return share;
}


/*}}}************************************************************************/
/*{{{                    stm_modfct_getShared()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
stm_share *stm_modfct_getShared(timing_function *fct)
{
  if (!fct->SHARE)
    fct->SHARE      = stm_modfct_newShare();
  
  return fct->SHARE;
}

/*}}}************************************************************************/
/*{{{                    stm_modfct_cpyShare()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static stm_share *stm_modfct_cpyShare(stm_share *share)
{
  if (share)
    sethtitem(SHARED_ELT_HT,share,gethtitem(SHARED_ELT_HT,share)+1);
  
  return share;
}

/*}}}************************************************************************/
/*{{{                    stm_modfct_share()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_modfct_share(timing_function *fct, timing_function *orig)
{
  fct->SHARE    = stm_modfct_cpyShare(stm_modfct_getShared(orig));
}

/*}}}************************************************************************/
