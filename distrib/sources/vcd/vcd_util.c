/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : vcd_util.c                                                  */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include "vcd_util.h"

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    vcd_dupTransList()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
trans_list *vcd_dupTransList(trans_list *trans)
{
  if (trans)
  {
    trans_list  *newtrans;

    newtrans                = vcd_dupTrans(trans);
    if ((newtrans->NEXT = vcd_dupTransList(trans->NEXT)))
      newtrans->NEXT->PREV  = newtrans;

    return newtrans;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    vcd_dupTrans()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
trans_list *vcd_dupTrans(trans_list *trans)
{
  if (trans)
  {
    trans_list  *newtrans;
    
    newtrans                = mbkalloc(sizeof(trans_list));
    memset(newtrans,0,sizeof(trans_list));
    newtrans->VAL           = mbkstrdup(trans->VAL);
    newtrans->INDEX         = trans->INDEX;
    newtrans->TIME          = trans->TIME;
    newtrans->BIT           = -1;

    return newtrans;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    vcd_getVectSize()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int vcd_getVectSize(char *name)
{
  int        left, right;
  char       test;

  if (sscanf(name,"%*[^[][%d:%d]%c",&left,&right,&test) == 2)
    return abs(left - right) + 1;
  else
    return -1; 
}
/*}}}************************************************************************/
/*{{{                    cdl_get_current_events()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *cdl_gettranssig (trans_list *trn)
{
    char *signame, *name;
    char buf[1024], buf2[1024], test;
    int left, right;
    chain_list *chain;
    chain_list *sig_list = NULL;
    
    if (!trn) return NULL;

    for(chain = vcd_db->SIG_TAB[trn->INDEX]; chain; chain = chain->NEXT){
        signame = (char*)chain->DATA;

        if (sscanf(signame,"%[^[][%d:%d]%c",buf,&left,&right,&test) == 3)
            sprintf (buf2, "%s(%d)", buf, trn->BIT);
        else
            sprintf (buf2, "%s", signame);
        
        if ((name = vcd_getCDLFromVCDName(namealloc(buf2))))
            sig_list = addchain (sig_list, name);
    }

    return sig_list;

}


/*}}}************************************************************************/
/*{{{                    vcd_unVectEvent()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void vcd_unVectEvent(chain_list **chain, trans_list *trans)
{
  trans_list    *tranx;
  char           vname[1024], test, cb[2];
  int            i, left, right;
  chain_list    *ch;

/*  for(ch = vcd_db->SIG_TAB[trans->INDEX]; ch; ch = ch->NEXT){
      if (sscanf((char*)ch->DATA,"%[^[][%d:%d]%c",
                 vname,&left,&right,&test) == 3)*/
      if (sscanf((char*)vcd_db->SIG_TAB[trans->INDEX]->DATA,"%[^[][%d:%d]%c",
                 vname,&left,&right,&test) == 3)
      {
        for (i = 0; trans->VAL[i] != '\0'; i ++)
          if (!trans->PREV || trans->VAL[i] != trans->PREV->VAL[i])
          {
            tranx       = vcd_dupTrans(trans);
            mbkfree(tranx->VAL);
            sprintf(cb,"%c",trans->VAL[i]);
            tranx->VAL  = mbkstrdup(cb);
            tranx->BIT  = (left > right) ? left -i: left +i ;
            if (cdl_gettranssig (tranx))
              *chain      = addchain(*chain,tranx);
            else
            {
              mbkfree (tranx->VAL);
              mbkfree (tranx);
            }
          }
      }   
      else
        if (cdl_gettranssig (trans)){
          if(!trans->PREV || (trans->PREV && (trans->VAL[0] != trans->PREV->VAL[0])))
          *chain  = addchain(*chain,vcd_dupTrans(trans));
        }
//  }
}

/*}}}************************************************************************/
/*{{{                    vcd_unVectState()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void vcd_unVectState(chain_list **chain, trans_list *trans)
{
  trans_list    *tranx;
  char           vname[1024], test, cb[2];
  int            i, left, right;
  chain_list    *ch;

/*  for(ch = vcd_db->SIG_TAB[trans->INDEX]; ch; ch = ch->NEXT){
      if (sscanf((char*)ch->DATA,"%[^[][%d:%d]%c",
                 vname,&left,&right,&test) == 3)*/
      if (sscanf((char*)vcd_db->SIG_TAB[trans->INDEX]->DATA,"%[^[][%d:%d]%c",
                 vname,&left,&right,&test) == 3)
      {
        for (i = 0; trans->VAL[i] != '\0'; i ++)
        {
          tranx         = vcd_dupTrans(trans);
          mbkfree(tranx->VAL);
          sprintf(cb,"%c",trans->VAL[i]);
          tranx->VAL    = mbkstrdup(cb);
          tranx->BIT    = (left > right) ? left -i: left +i ;
          *chain        = addchain(*chain,tranx);
        }
      }   
      else
        *chain          = addchain(*chain,vcd_dupTrans(trans));
//  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
float vcd_TimeUnit (char *name)
{
    float value = 1.0;
    
    if(!strcmp("ps", name)){
        value = 1e-12;
    }else if(!strcmp("ns", name)){
        value = 1e-9;
    }else if(!strcmp("us", name)){
        value = 1e-6;
    }else if(!strcmp("ms", name)){
        value = 1e-3;
    }else if(!strcmp("s", name)){
        value = 1;
    }
    return value;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
float vcd_get_ts(void)
{
    if (!vcd_db) return 0.0;
    return vcd_db->TIME_BASE*vcd_db->TIME_UNIT;
}
