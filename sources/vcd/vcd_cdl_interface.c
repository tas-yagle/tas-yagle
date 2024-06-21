/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : VCD Version 2.00                                            */
/*    Fichier : vcd_cdl_interface.c                                         */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "vcd_cdl_interface.h"

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
/*{{{                    cdl_parse()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cdl_parse(char *filename)
{
  vcd_parse_core(filename, NULL);
}

/*}}}************************************************************************/
/*{{{                    cdl_list_sigs()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void cdl_goto_next_time ()
{
    chain_list *events;
    vcd_goto_next_time ();
    events = cdl_get_current_events ();
    if (!vcd_count_events (events)) vcd_goto_next_time ();
    cdl_free_event_list (events);
}

/*}}}************************************************************************/
/*{{{                    cdl_list_sigs()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

chain_list *cdl_getnextevent (chain_list *event)
{
    return event->NEXT;
}

/*}}}************************************************************************/
/*{{{                    cdl_list_sigs()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

chain_list *cdl_getnextsig (chain_list *sig)
{
    return sig->NEXT;
}

/*}}}************************************************************************/
/*{{{                    cdl_list_sigs()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

char *cdl_getsigname (chain_list *sig)
{
    return (char*)sig->DATA;
}

/*}}}************************************************************************/
/*{{{                    cdl_list_sigs()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *cdl_get_sig_list(void)
{
  char        buf[1024], vectname[1024], test, *name;
  int         i, j, left, right, ind;
  chain_list *sig_list = NULL;
  chain_list *chain;

  if (!vcd_db_check() || !vcd_cdl_parsed())
    return NULL;

  for (i = 0; i < vcd_db->SIG_IDX; i++)
  {
    for(chain = vcd_db->SIG_TAB[i]; chain; chain = chain->NEXT){
        name    = (char*)chain->DATA;
        if (sscanf(name,"%[^[][%d:%d]%c",vectname,&left,&right,&test) == 3)
        {
          ind   = (left > right) ? -1 : 1;
          for (j = left; j != right + ind; j += ind)
          {
            sprintf(buf,"%s(%d)",vectname,j);
            if ((name = vcd_getCDLFromVCDName(namealloc(buf))))
              sig_list = addchain (sig_list, name);
          }
        }
        else if ((name = vcd_getCDLFromVCDName(name)))
          sig_list = addchain (sig_list, name);
    }
  }
  return sig_list;
}

/*}}}************************************************************************/
/*{{{                    cdl_list_sigs()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cdl_list_sigs(void)
{
  char       buf[1024], vectname[1024], test, *name;
  int        i, j, left, right, ind;
  chain_list *chain;

  if (!vcd_db_check() || !vcd_cdl_parsed())
    return;

  for (i = 0; i < vcd_db->SIG_IDX; i++)
  {
      for(chain = vcd_db->SIG_TAB[i]; chain; chain = chain->NEXT){
          name    = (char*)chain->DATA;
          if (sscanf(name,"%[^[][%d:%d]%c",vectname,&left,&right,&test) == 3)
          {
            ind   = (left > right) ? -1 : 1;
            for (j = left; j != right + ind; j += ind)
            {
              sprintf(buf,"%s(%d)",vectname,j);
              if ((name = vcd_getCDLFromVCDName(namealloc(buf))))
                fprintf(stdout,"   %s\n",name);
            }
          }
          else if ((name = vcd_getCDLFromVCDName(name)))
            fprintf(stdout,"   %s\n",name);
      }
  }
}

/*}}}************************************************************************/
/*{{{                    cdl_get_waveform()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
trans_list *cdl_get_waveform(char *signal)
{
  trans_list    *waveform, *tranx;
  char          *vcd, *cdl, vectname[1024], test;
  char          *tmp, nval[2];
  int            ind = -1, left, right;
  chain_list    *chain;
     

  // trouver le correspondant de 'signal'
  cdl       = namealloc(signal);
  vcd       = vcd_getVCDFromCDLName(cdl);
  
  if (sscanf(vcd,"%[^(](%d)%c",vectname,&ind,&test) == 2)
    vcd     = vectname;
  
  waveform  = vcd_get_waveform (vcd);

  // post-traiter waveform

  if (waveform && ind != -1)
  {
//    tmp     = vcd_db->SIG_TAB[gethtitem(vcd_db->SIG_HT,namealloc(vcd))];
    for(chain = vcd_db->SIG_TAB[gethtitem(vcd_db->SIG_HT,namealloc(vcd))]; chain; chain = chain->NEXT){
        tmp = (char*)chain->DATA;
        if (sscanf(tmp,"%*[^[][%d:%d]%c",&left,&right,&test) == 2)
        {
          ind           = abs(left - ind);
          waveform      = vcd_dupTransList(waveform);
          for (tranx = waveform; tranx; tranx = tranx->NEXT)
          {
            tmp         = tranx->VAL;
            sprintf(nval,"%c",tmp[ind]);
            tranx->VAL  = mbkstrdup(nval);
            mbkfree(tmp);
          }
        }
        else
        {
          /* TODO perhaps free of waveform */
          waveform  = NULL;
        }
    }
  }
  
  return waveform;
}

/*}}}************************************************************************/
/*{{{                    cdl_get_current_events()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

char cdl_geteventval (chain_list *event)
{
    trans_list *trn;

    if (!event) return 0;

    trn = (trans_list*)event->DATA;

    return trn->VAL[0];
}



/*}}}************************************************************************/
/*{{{                    cdl_get_current_events()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *cdl_geteventsig (chain_list *event)
{
    if (!event) return NULL;

    return cdl_gettranssig ((trans_list*)event->DATA);
}


/*}}}************************************************************************/
/*{{{                    cdl_get_current_events()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *cdl_get_current_events(void)
{
  chain_list    *current_events, *chainx, *res = NULL;

  current_events    = vcd_get_current_events();
  
  // post-traiter current_events
  for (chainx = current_events; chainx; chainx = chainx->NEXT)
    vcd_unVectEvent(&res,chainx->DATA);

  return reverse(res);
}

/*}}}************************************************************************/
/*{{{                    cdl_get_state()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void cdl_free_event_list (chain_list *events)
{
    trans_list *trn;
    chain_list *evt;

    if (!events) return;
    if (!events->DATA) return; // has already been freed

    for (evt = events; evt; evt = evt->NEXT) {
        if (!(trn = (trans_list*)evt->DATA)) continue;
        mbkfree (trn->VAL);
        mbkfree (trn);
        evt->DATA = NULL;
    }
    freechain (events);
}

/*}}}************************************************************************/
/*{{{                    cdl_get_state()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

chain_list *cdl_get_state(void)
{
  chain_list    *state, *chainx, *res = NULL;

  state             = vcd_get_state();

    // post-traiter state
  for (chainx = state; chainx; chainx = chainx->NEXT)
    vcd_unVectState(&res,chainx->DATA);
  
  return reverse(res);
}

/*}}}************************************************************************/
