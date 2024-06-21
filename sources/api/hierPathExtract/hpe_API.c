#include <stdio.h>
#include <math.h>
#include API_H
#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include AVT_H

#define API_USE_REAL_TYPES
#include "hpe_API.h"

#define HPE_LOINS_MARK  ((long)0xc1c67050)
#define HPE_LOSIG_MARK  ((long)0xc1c67051)
#define HPE_CORRESP     ((long)0xfab0706)

#define UNDEFINED  -1000000

typedef struct
{
  double vdd, vss;
  double temp;
  char *process;
} InstanceVariation;

static ht *allvariations=NULL;

/****************************************************************************/
/*{{{                    error                                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
error(int n)
{
  printf("%s:%d error\n",__FILE__,n);
}

/*}}}************************************************************************/
/*{{{                    isMarkedCell                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static ptype_list *
isMarkedCell(loins_list *ins)
{
  return getptype(ins->USER,HPE_LOINS_MARK);
}

/*}}}************************************************************************/
/*{{{                    markCell                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
markCell(loins_list *ins)
{
  
  if (isMarkedCell(ins))
    return;
  ins->USER   = addptype(ins->USER,HPE_LOINS_MARK,NULL);
  if (!isMarkedCell(ins))
    error(__LINE__);
}

/*}}}************************************************************************/
/*{{{                    unmarkCell                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
unmarkCell(loins_list *ins)
{
  if (!isMarkedCell(ins))
    return;
  ins->USER   = delptype(ins->USER,HPE_LOINS_MARK);
  if (isMarkedCell(ins))
    error(__LINE__);
}

/*}}}************************************************************************/
/*{{{                    isMarkedSig                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static ptype_list *
isMarkedSig(losig_list *sig)
{
  return getptype(sig->USER,HPE_LOSIG_MARK);
}

/*}}}************************************************************************/
/*{{{                    markSig                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
markSig(losig_list *sig)
{
  if (isMarkedSig(sig))
    return;
  sig->USER   = addptype(sig->USER,HPE_LOSIG_MARK,NULL);
  if (!isMarkedSig(sig))
    error(__LINE__);
}

/*}}}************************************************************************/
/*{{{                    unmarkSig                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
unmarkSig(losig_list *sig)
{
  if (!isMarkedSig(sig))
    return;
  sig->USER   = delptype(sig->USER,HPE_LOSIG_MARK);
  if (isMarkedSig(sig))
    error(__LINE__);
}

/*}}}************************************************************************/
/*{{{                    markNeighbor                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
markNeighbor(loins_list *ins)
{
  loins_list *insz;
  locon_list *conx, *conz;
  losig_list *sigx;
  ptype_list *px;
  chain_list *cx;

  for (conx = ins->LOCON; conx; conx = conx->NEXT)
  {
    sigx = conx->SIG;
    if (!mbk_LosigIsVSS(sigx) && !mbk_LosigIsVDD(sigx))
      {
        markSig(sigx);
        if ((px = getptype(sigx->USER,LOFIGCHAIN)))
          for (cx = px->DATA; cx; cx = cx->NEXT)
            {
              conz = cx->DATA;
              switch (conz->TYPE)
                {
                case 'I':
                  insz = conz->ROOT;
                  markCell(insz);
                case 'E':
                  break;
                case 'T':
                default :
                  error(__LINE__);
                  break;
                }
            }
      }
    
  }
}

/*}}}************************************************************************/
/*{{{                    markCells                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
markCells(lofig_list *top, chain_list *cells)
{
  loins_list    *ins;
  chain_list    *cx;
  
  for (cx = cells; cx; cx = cx->NEXT)
    if ((ins = mbk_quickly_getloinsbyname(top,namealloc(cx->DATA))))
    {
      markCell(ins);
      markNeighbor(ins);
    }
    else
      printf("-- cell %s doesn't exist\n",(char*)cx->DATA);
}

/*}}}************************************************************************/
/*{{{                    viewMarkedInstance                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
viewMarkedInstance(lofig_list *top)
{
  loins_list *insx;

  for (insx = top->LOINS; insx; insx = insx->NEXT)
    if (isMarkedCell(insx))
      printf("Found instance: %s.%s\n",top->NAME,insx->INSNAME);
}

/*}}}************************************************************************/
/*{{{                    viewMarkedSignal                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
viewMarkedSignal(lofig_list *top)
{
  losig_list *sigx;

  for (sigx = top->LOSIG; sigx; sigx = sigx->NEXT)
    if (isMarkedSig(sigx))
      printf("Found signal: %s.%s\n",top->NAME,getsigname(sigx));
}

/*}}}************************************************************************/
/*{{{                    hpe_getSubNetlist                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
lofig_list *
hpe_getSubNetlist(char *topname, chain_list *cells)
{
  lofig_list    *top;
  
  if (!(top = getloadedlofig(topname)))
    return NULL;
  else
  {
    lofigchain(top);
    markCells(top,cells);
  }

  return top;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int notconnectedmode=0;

static losig_list *hpe_get_sig_corresp(losig_list *ls)
{
  ptype_list *pt;
  if ((pt=getptype(ls->USER, HPE_CORRESP))!=NULL) return (losig_list *)pt->DATA;
  return NULL;
}

static void hpe_add_sig_corresp(losig_list *ls, losig_list *csp)
{
  ls->USER=addptype(ls->USER, HPE_CORRESP, csp);
}

static locon_list *hpe_get_con_corresp(locon_list *lc)
{
  ptype_list *pt;
  if ((pt=getptype(lc->USER, HPE_CORRESP))!=NULL) return (locon_list *)pt->DATA;
  return NULL;
}

static void hpe_add_con_corresp(locon_list *lc, locon_list *csp)
{
  lc->USER=addptype(lc->USER, HPE_CORRESP, csp);
}

static void hpe_remove_con_corresp(locon_list *lc)
{
  if (getptype(lc->USER, HPE_CORRESP)!=NULL)
    lc->USER=delptype(lc->USER, HPE_CORRESP);
}

static void hpe_remove_sig_corresp(losig_list *ls)
{
  if (getptype(ls->USER, HPE_CORRESP)!=NULL)
    ls->USER=delptype(ls->USER, HPE_CORRESP);
}

static void hpe_clean_netlist(lofig_list *lf)
{
  losig_list *ls;
  chain_list *cl;
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      hpe_remove_sig_corresp(ls);
      for (cl=(chain_list *)getptype(ls->USER, LOFIGCHAIN)->DATA; cl!=NULL; cl=cl->NEXT)
        hpe_remove_con_corresp((locon_list *)cl->DATA);
    }
}

static losig_list *hpe_getground(lofig_list *lf, losig_list **gnd)
{
  if (*gnd==NULL)
    {
      losig_list *ls;
      int idx=0;
      char *vssname;

      vssname=mbk_getvssname();

      for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
        {
          if (ls->INDEX>idx) idx=ls->INDEX;
          if (mbk_LosigIsVSS(ls) && strcmp(getsigname(ls), vssname)==0)
            {
              if (!ls->PRCN) addlorcnet (ls);
              *gnd=ls;
              break;
            }
        }

      if (*gnd==NULL)
        {
          *gnd=addlosig(lf,idx+1,addchain(NULL,mbk_getvssname()),INTERNAL);
          addlorcnet(*gnd);
          (*gnd)->USER=addptype((*gnd)->USER, LOFIGCHAIN, NULL);
        }
    }
  return *gnd;
}


/*}}}************************************************************************/
/*{{{                    unmarkNetlist                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void unmarkNetlist(lofig_list *topfig)
{
  loins_list *insx;
  losig_list *sigx;

  for (insx = topfig->LOINS; insx; insx = insx->NEXT)
    unmarkCell(insx);
  for (sigx = topfig->LOSIG; sigx; sigx = sigx->NEXT)
    unmarkSig(sigx);
}

/*}}}************************************************************************/
/*{{{                    hpe_ExtractNetlist                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
lofig_list *
hpe_ExtractNetlist(lofig_list *lf, char *name)
{
  lofig_list *newlf;
  loins_list *li, *newli;
  losig_list *ls;
  locon_list *lc, *lc0;
  int idx;
  chain_list *cl;

  if ((newlf=getloadedlofig(name))!=NULL)
    {
      unlocklofig(newlf);
      dellofig(name);
    }

  newlf=addlofig(name);
  locklofig(newlf);
  idx=1;

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      if (isMarkedCell(li))
        {
          // creation des signaux autour de l'instance
          cl=NULL;
          for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
            {
              if ((ls=hpe_get_sig_corresp(lc->SIG))==NULL)
                {
                  if (isMarkedSig(lc->SIG))
                    ls=addlosig(newlf,idx++,dupchainlst (lc->SIG->NAMECHAIN),lc->SIG->TYPE);
                  else
                    {
                      if (notconnectedmode)
                        ls=addlosig(newlf,idx++,dupchainlst (lc->SIG->NAMECHAIN),INTERNAL);
                      else
                        {
                          ls=addlosig(newlf,idx++,dupchainlst (lc->SIG->NAMECHAIN),EXTERNAL);
                          addlocon(newlf, getsigname(lc->SIG), ls, UNKNOWN);
                        }
                    }
                  // ajout des correspondances de signaux
                  hpe_add_sig_corresp(lc->SIG, ls);
                  hpe_add_sig_corresp(ls, lc->SIG);
                }
              cl=addchain(cl,ls);
            }

          // creation de l'instance
          cl=reverse(cl);
          newli=addloins(newlf, li->INSNAME, getloadedlofig(li->FIGNAME), cl);
          freechain(cl);

          // ajout des correspondances des connecteurs
          for (lc=li->LOCON, lc0=newli->LOCON; lc!=NULL && lc0!=NULL; lc=lc->NEXT, lc0=lc0->NEXT)
            hpe_add_con_corresp(lc0, lc);
        }
    }  

  // interface de la netlist
  for (lc=lf->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      if (isMarkedSig(lc->SIG))
        {
          if ((ls=hpe_get_sig_corresp(lc->SIG))==NULL) exit(6);
          lc0=addlocon(newlf, lc->NAME, ls, lc->DIRECTION);
          hpe_add_con_corresp(lc0, lc);
        }
    }

  lofigchain(newlf);
  return newlf;
}

/*}}}************************************************************************/
/*{{{                    hpe_AddRC                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
hpe_AddRC(lofig_list *origlofig, lofig_list* fig)
{
  losig_list *sig, *cir_sig, *gnd=NULL;
  losig_list *x_sig, *x_cir_sig;
  lowire_list *wire;
  loctc_list *ctc;
  chain_list *ch, *rclist;
  ptype_list *p;
  int x_node;
  chain_list *cl;
  locon_list *lc, *lc0;
  num_list *nl;
  
  if (fig->LOSIG==NULL || getptype(fig->LOSIG->USER, LOFIGCHAIN)==NULL)
    lofigchain (fig);

  for (sig = fig->LOSIG; sig; sig = sig->NEXT) 
    {
      //      if (mbk_LosigIsVSS(sig)) continue;

      cir_sig = hpe_get_sig_corresp (sig);
      if (!isMarkedSig(cir_sig)) continue;
      if (!cir_sig->PRCN) continue;
      
      rcn_refresh_signal(origlofig, cir_sig);
      
      if (!cir_sig->PRCN->PWIRE && !cir_sig->PRCN->PCTC) continue;
      
      if (!sig->PRCN) addlorcnet (sig);
      
      // R
      rclist = NULL;
      for (wire = cir_sig->PRCN->PWIRE; wire; wire = wire->NEXT) // to reverse wires
        rclist = addchain (rclist, wire);
      
      for (ch = rclist; ch; ch = ch->NEXT) 
        {
          wire = (lowire_list*)ch->DATA;
          addlowire (sig, 0, wire->RESI, wire->CAPA, wire->NODE1, wire->NODE2);
          rcn_addcapa( sig, wire->CAPA );
        }
      freechain (rclist);
      
      // C
      rclist = NULL;
      for (ch = cir_sig->PRCN->PCTC; ch; ch = ch->NEXT) // to reverse ctcs
        rclist = addchain (rclist, ch->DATA);

      for (ch = rclist; ch; ch = ch->NEXT) 
        {
          ctc = (loctc_list*)ch->DATA;
          if (ctc->SIG1 == cir_sig) x_cir_sig = ctc->SIG2;
          else x_cir_sig = ctc->SIG1;
          
          if (!isMarkedSig(x_cir_sig)) x_sig=NULL;
          else x_sig=hpe_get_sig_corresp (x_cir_sig);

          if (x_sig==NULL)
            {
              addloctc (sig, rcn_ctcnode (ctc, cir_sig), hpe_getground(fig, &gnd), 1, ctc->CAPA);
              rcn_addcapa( sig, ctc->CAPA );
              rcn_addcapa( hpe_getground(fig, &gnd), ctc->CAPA);
            }
          else
            { 
              if (sig->INDEX<x_sig->INDEX)
                {
                  x_node = rcn_ctcnode (ctc, x_cir_sig);
                  if (!x_sig->PRCN) x_sig->PRCN = addlorcnet (x_sig);
                  
                  addloctc (sig, rcn_ctcnode (ctc, cir_sig), x_sig, x_node, ctc->CAPA);
                  rcn_addcapa( sig, ctc->CAPA );
                  rcn_addcapa( x_sig, ctc->CAPA);
                }
            }
        }
      freechain (rclist);
      
      // pnodes des connecteurs
      if ((p=getptype(sig->USER, LOFIGCHAIN))!=NULL)
        {
          for (cl=(chain_list *)p->DATA; cl!=NULL; cl=cl->NEXT)
            {
              lc=(locon_list *)cl->DATA;
              if ((lc0=hpe_get_con_corresp(lc))!=NULL)
                {
                  for (nl=lc0->PNODE; nl!=NULL; nl=nl->NEXT)
                    lc->PNODE=addnum(lc->PNODE, nl->DATA);
                  lc->PNODE=(num_list *)reverse((chain_list *)lc->PNODE);
                }
            }
        }
    }

  // merge des capas si c'est possible
  rcn_mergectclofig(fig);

  for (sig = fig->LOSIG; sig; sig = sig->NEXT)
    rcn_add_low_capa_ifneeded(sig, hpe_getground(fig, &gnd));
}

// ->> Instance variations

InstanceVariation *hpe_getvariation(char *name)
{
  long l;
  InstanceVariation *iv;

  name=namealloc(name);
  if (allvariations==NULL) allvariations=addht(10);
  if ((l=gethtitem(allvariations, name))==EMPTYHT)
    {
      iv=(InstanceVariation *)mbkalloc(sizeof(InstanceVariation));
      iv->vdd=iv->vss=iv->temp=UNDEFINED;
      iv->process=NULL;
      addhtitem(allvariations, name, (long)iv);
      return iv;
    }
  return (InstanceVariation *)l;
}

void hpe_SetInstanceVariation(char *cellname, char *parameter, char *value)
{
  InstanceVariation *iv;

  iv=hpe_getvariation(cellname);
  if (strcasecmp(parameter, "vdd")==0) iv->vdd=atof(value);
  else if (strcasecmp(parameter, "vss")==0) iv->vss=atof(value);
  else if (strcasecmp(parameter, "temp")==0) iv->temp=atof(value);
  else if (strcasecmp(parameter, "process")==0) iv->process=sensitive_namealloc(value);
  else printf("unknown parameter '%s' set for instance '%s'\n",parameter,cellname);
}

static char *hpe_getcellnamefromtrs(char *trname)
{
  char buf[1024], *c;
  if (trname==NULL) return "?";
  strcpy(buf, trname);
  if ((c=strrchr(buf, SEPAR))!=NULL) *c='\0'; // on enleve le nom du transistor
  return namealloc(buf);
}

static losig_list *hpe_createalim(ht *alimsig, lofig_list *lf, double alim)
{
  losig_list *ls;
  long l;
  int idx=0;
  char name0[1024], *name;
  
  sprintf(name0, "AVT_variation_%s%ldmV", alim<0?"minus":"",mbk_long_round(fabs(alim*1e3)));
  name=namealloc(name0);

  if ((l=gethtitem(alimsig, name))!=EMPTYHT) return (losig_list *)l;

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      if (ls->INDEX>idx) idx=ls->INDEX;
    }
  
  ls=addlosig(lf,idx+1,addchain(NULL,name),INTERNAL);
  addlorcnet(ls);
  ls->USER=addptype(ls->USER, LOFIGCHAIN, NULL);
  addlosigalim(ls, alim, NULL);
  addhtitem(alimsig, name, (long)ls);
  return ls;
}

static void unlinkconnectorifneeded(locon_list *lc, int vdd, losig_list *newsig)
{
  if (lc!=NULL && ((vdd && mbk_LosigIsVDD(lc->SIG)) || (!vdd && mbk_LosigIsVSS(lc->SIG))))
    {
      lc->SIG=newsig;
      freenum(lc->PNODE);
      lc->PNODE=NULL;
    }
}

static void hpe_ApplyVariations(ht *cellvar, lofig_list *destfig)
{
  lotrs_list *lt;
  long l;
  chain_list *cl;
  losig_list *sig=NULL;
  InstanceVariation *iv;
  char *cellname;
  ht *alimsig;

  if (cellvar==NULL) return;

  if ((cl=GetAllHTElems(cellvar))==NULL) return;
  
  rflattenlofig(destfig, YES, NO);

  alimsig=addht(10);

  for (lt=destfig->LOTRS; lt!=NULL; lt=lt->NEXT)
    {
      if ((l=gethtitem(cellvar, (cellname=hpe_getcellnamefromtrs(lt->TRNAME))))!=EMPTYHT)
        {
          iv=(InstanceVariation *)l;
          if (iv->vdd!=UNDEFINED)
            {
              sig=hpe_createalim(alimsig, destfig, iv->vdd);
              unlinkconnectorifneeded(lt->SOURCE, 1, sig);
              unlinkconnectorifneeded(lt->GRID, 1, sig);
              unlinkconnectorifneeded(lt->DRAIN, 1, sig);
              unlinkconnectorifneeded(lt->BULK, 1, sig);
            }
          if (iv->vss!=UNDEFINED)
            {
              sig=hpe_createalim(alimsig, destfig, iv->vss);
              unlinkconnectorifneeded(lt->SOURCE, 0, sig);
              unlinkconnectorifneeded(lt->GRID, 0, sig);
              unlinkconnectorifneeded(lt->DRAIN, 0, sig);
              unlinkconnectorifneeded(lt->BULK, 0, sig);
            }
          // temp? process?
        }
    }

  delht(alimsig);

  while (cl!=NULL)
    {
      mbkfree(cl->DATA);
      cl=delchain(cl, cl);
    }
  delht(cellvar);

  lofigchain(destfig);
}

/*}}}************************************************************************/
/*{{{                    hpe_CreateSubNetlist                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
lofig_list *
hpe_CreateSubNetlist(char *subnetlist, char *topname, chain_list *cells)
{
  lofig_list *topfig;
  lofig_list *subfig;
 
  topfig = hpe_getSubNetlist(topname,cells);
  if (topfig==NULL)
    {
      printf("could not find figure '%s'\n",topname);
      return NULL;
    }

//  viewMarkedInstance(topfig);
//  viewMarkedSignal(topfig);
  
  subfig = hpe_ExtractNetlist(topfig,subnetlist);
  hpe_AddRC(topfig,subfig);

  unmarkNetlist(topfig);
  hpe_clean_netlist(topfig);
  hpe_clean_netlist(subfig);

  //viewMarkedInstance(topfig);
  //viewMarkedSignal(topfig);
  hpe_ApplyVariations(allvariations, subfig);
  allvariations=NULL;
  return subfig;
}

/*}}}************************************************************************/

