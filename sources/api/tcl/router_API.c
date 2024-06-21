/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : ROUTER Version 1                                            */
/*    Fichier : router.c                                                    */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include MUT_H
#include MLO_H
#include RCN_H

typedef struct router_data {
  float MINCAPA;
  float MAXCAPA;
  float MINRES;
  float MAXRES;
  int MAXWIRE;
} router_dt;

static int router_seed=-1;

void avt_SetSEED(int val)
{
  router_seed=val;
}
/****************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
initRouter(router_dt *rte)
{
  if (router_seed!=-1) srand(router_seed);
  rte->MINCAPA = 100.0;
  rte->MAXCAPA = 10000.0;
  rte->MINRES = 100.0;
  rte->MAXRES = 10000.0;
  rte->MAXWIRE = 1000;
}

long max( long a, long b )
{
  if( a<b)
    return b;
  return a;
}

float route_rand(float min, float max)
{
  int a, b, c;

  a = (int)(max*100000.0);
  b = (int)(min*100000.0);
  c = b + rand()%(a-b+1);
  return ((float)c/100000.0);
}

float route_capa(router_dt *rte)
{
  return ((float)route_rand(rte->MINCAPA*1e15,rte->MAXCAPA*1e15))*1e-3 ;
}

float route_resis(router_dt *rte)
{
  return ((float)route_rand(rte->MINRES,rte->MAXRES)/100.0) ;
}

/*}}}************************************************************************/
/*{{{                    route_treelosig                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
long
route_treelosig(losig_list *losig, long node, int nbcon,
                num_list **loconlist, router_dt *rte)
{
  num_list *ptnum ;
  long nodex ;
  int nbwirelist ;
  int nbbranch ;
  int i ; 

  nbwirelist = route_rand(0,rte->MAXWIRE) ;

  for (i = 1; i < nbwirelist; i++, node++)
    addlowire(losig,0,route_resis(rte),route_capa(rte),node,node+1) ;

  if (nbcon == 1)
  {
    ptnum = *loconlist ;
    *loconlist = (*loconlist)->NEXT ;
    ptnum->NEXT = NULL ;
    addlowire(losig,0,route_resis(rte),route_capa(rte),node,ptnum->DATA) ;
    freenum(ptnum) ;
    
    return node ;
  }
  else
    nbbranch = route_rand(1,nbcon-1) ;

  addlowire(losig,0,route_resis(rte),route_capa(rte),node,node+1) ;

  nodex = node + 1;
  node = route_treelosig(losig,node+1,nbbranch,loconlist,rte) ;

  addlowire(losig,0,route_resis(rte),route_capa(rte),nodex,node+1) ;

  node = route_treelosig(losig,node+1,nbcon-nbbranch,loconlist,rte) ;

  return node;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
route_losig(losig_list *losig, router_dt *rte)
{
  locon_list *locon = NULL ;
  ptype_list *ptype ;
  chain_list *chain ;
  num_list *ptnum = NULL ;
  int nbcon ; 

  ptype = getptype(losig->USER,LOFIGCHAIN) ;

  if (losig->PRCN != NULL)
    freelorcnet(losig) ;
  addlorcnet(losig) ;

  for (chain = ptype->DATA , nbcon = 0; chain; chain = chain->NEXT, nbcon++)
  {
    locon = (locon_list *)chain->DATA ;
    setloconnode(locon,nbcon+1) ;
    if(chain->NEXT != NULL)
      ptnum = addnum(ptnum,locon->PNODE->DATA) ;
  }

  if (locon && nbcon != 1)
    route_treelosig(losig,locon->PNODE->DATA,nbcon-1,&ptnum,rte) ;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
routeCC(lofig_list *lofig, router_dt *rte)
{
  losig_list *losig ;
  losig_list **tabsig ;
  int nbsig ;
  int i ;
  int j ;
  int k ;
  int a;

  for (losig = lofig->LOSIG, nbsig = 0; losig; losig = losig->NEXT)
    if (!(mbk_LosigIsVDD(losig) || mbk_LosigIsVSS(losig)))
      nbsig++ ;

  tabsig = (losig_list **)mbkalloc(nbsig * sizeof(losig_list*)) ;

  for (losig = lofig->LOSIG, nbsig = 0; losig; losig = losig->NEXT)
    if (!(mbk_LosigIsVDD(losig) || mbk_LosigIsVSS(losig)))
      tabsig[nbsig++] = losig ;

  for (i = 0; i < nbsig;  i++)
    for (j = 1; j < tabsig[i]->PRCN->NBNODE; j++)
      if (route_rand(0,1))
      {
        k = route_rand(0,nbsig-1) ;    
        a = route_rand(1,max(1,tabsig[k]->PRCN->NBNODE-1));
        if (k != i)
          addloctc(tabsig[i],j,tabsig[k],a,route_capa(rte));
      }
  
  mbkfree(tabsig) ;
}

/*}}}************************************************************************/
/*{{{                    routeRC                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
routeRC(lofig_list *lofig,router_dt *rte)
{
  losig_list *losig ;

  for(losig = lofig->LOSIG; losig; losig = losig->NEXT)
    if (!(mbk_LosigIsVDD(losig) || mbk_LosigIsVSS(losig)))
      route_losig(losig,rte);
}

/*}}}************************************************************************/
/*{{{                    avt_AddRC                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void avt_AddRC(lofig_list *lofig, int maxwire,
               double minc, double maxc, double minr, double maxr)
{
  router_dt rte;
  
  // for debugging purpose
  //srand(67);
  //printf("MC:%f mC:%f MR:%f mR:%f MW:%d\n",maxc,minc,maxr,minr,maxwire);
  
  if (lofig!=NULL)
    {
      initRouter(&rte);
      
      if (minc >= 0) rte.MINCAPA = minc;
      if (maxc >= 0) rte.MAXCAPA = maxc;
      if (minr >= 0) rte.MINRES = (int)(100.0*minr);
      if (maxr >= 0) rte.MAXRES = (int)(100.0*maxr);
      if (maxwire >= 0) rte.MAXWIRE = maxwire;
      
      if(rte.MINCAPA > rte.MAXCAPA) rte.MINCAPA = rte.MAXCAPA ;
      if(rte.MINRES > rte.MAXRES) rte.MINRES = rte.MAXRES ;
      
      routeRC(lofig,&rte);
    }
}

/*}}}************************************************************************/
/*{{{                    avt_AddCC                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
avt_AddCC(lofig_list *lofig, double minc, double maxc)
{
  router_dt rte;

  if (lofig!=NULL)
    {
      initRouter(&rte);
      
      if (minc >= 0) rte.MINCAPA = minc;
      if (maxc >= 0) rte.MAXCAPA = maxc;
      
      if (rte.MINCAPA > rte.MAXCAPA) rte.MINCAPA = rte.MAXCAPA ;
      
      routeCC(lofig,&rte);
    }
}

/*}}}************************************************************************/
