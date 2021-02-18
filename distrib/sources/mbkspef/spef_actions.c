
/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPEF Version 1.00                                            */
/*    Fichier : spef_actions.c                                               */
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

#include <ctype.h>
#include "spef_actions.h"
#include "spef_annot.h"
#include "spef_util.h"
#include AVT_H

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

#define LOCAL_NET_HTABE_SIZE 50

static char *thegroundsignal="";
char *spef_ParsedFile="";
static losig_list *currentnet=NULL;
ptype_list *post_connection=NULL;
/****************************************************************************/
/*     private                                                              */
/****************************************************************************/

static char *spef_check_keep_sub(char *name, char ch)
{
  if ((SPEF_CARDS & KEEP__INSTANCE)==0 && ch=='x')
    {
      if (tolower(name[0])=='x') return &name[1];
    }
  if ((SPEF_CARDS & KEEP__TRANSISTOR)==0 && ch=='m')
    {
      if (tolower(name[0])=='m') return &name[1];
    }
  if ((SPEF_CARDS & KEEP__RESISTANCE)==0 && ch=='r')
    {
      if (tolower(name[0])=='r') return &name[1];
    }
  if ((SPEF_CARDS & KEEP__DIODE)==0 && ch=='d')
    {
      if (tolower(name[0])=='d') return &name[1];
    }
  if ((SPEF_CARDS & KEEP__CAPA)==0 && ch=='c')
    {
      if (tolower(name[0])=='c') return &name[1];
    }
  return name;
}

char *spef_check_keep(char *name, char ch, int nalloc)
{
  if (nalloc) 
    return namealloc(spef_check_keep_sub(name, ch));
  else
    return spef_check_keep_sub(name, ch);
}

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

int spef_AddCapaOrDiodeConnector(char *allname, char *instname, char *signame,int nodenum, char mode)
{
  losig_list *ptsig;
  ptype_list *ptype;
  loctc_list *lctc;
  char *name, *dname;
  long value, node;
  char buf[1024];
  int pos;
  char *posi, *negi;

#ifndef __ALL__WARNING__
  allname	= NULL; 
#endif
  lctc=spef_getcapabyname(dname=spef_spi_devect(spef_check_keep(instname,mode,0)));
  if (lctc==NULL && (name=namealloc(spef_check_keep(instname,mode,0)))!=dname)
    lctc=spef_getcapabyname(name);
  if (lctc==NULL && instname[0]=='*'){
      if((value = getititem(namemaptable, atoi(instname + 1))) != EMPTYHT){
          name = namealloc(spef_check_keep((char*)value,mode,0));
          lctc = spef_getcapabyname(name);
          if (lctc==NULL && vectorindex(name)!=-1)
            lctc = spef_getcapabyname(mbk_vect(spef_check_keep(name,mode,0), '[', ']'));
      }
  }

  if (lctc==NULL) return 0;
    
  if (mode=='d') posi=ANNOT_D_POS, negi=ANNOT_D_NEG;
  else  posi=ANNOT_C_POS, negi=ANNOT_C_NEG;

  if (strcasecmp(signame,posi?posi:"1")==0) pos=1;
  else if (strcasecmp(signame,negi?negi:"2")==0) pos=0;
  else 
   {
     if (!SPEF_IN_CACHE) avt_errmsg(SPE_ERRMSG, "021", AVT_ERROR, spef_ParsedFile, Line, mode=='d'?"diode":"capacitance", instname,signame);
     return 1;
   }

  if (pos==1) ptsig =lctc->SIG1; else ptsig =lctc->SIG2;
  spef_checkpnode_parasitic(pos==1?&lctc->NODE1:&lctc->NODE2, ptsig, &node);
  return 1;
}

/****************************************************************************/
int spef_AddResistorConnector(char *allname, char *instname, char *signame,int nodenum)
{
  losig_list *ptsig;
  ptype_list *ptype;
  lowire_list *lw;
  char *name, *dname;
  long value;
  char buf[1024];
  int pos;
  long node;

#ifndef __ALL__WARNING__
  allname	= NULL; 
#endif
  lw=spef_getresibyname(dname=spef_spi_devect(spef_check_keep(instname,'r',0)));
  if (lw==NULL && (name=namealloc(spef_check_keep(instname,'r',0)))!=dname)
    lw=spef_getresibyname(name);
  if (lw==NULL && instname[0]=='*'){
      if((value = getititem(namemaptable, atoi(instname + 1))) != EMPTYHT){
          name = namealloc(spef_check_keep((char*)value,'r',0));
          lw = spef_getresibyname(name);
          if (lw==NULL && vectorindex(name)!=-1)
            lw = spef_getresibyname(mbk_vect(spef_check_keep(name,'r',0), '[', ']'));
      }
  }

  if (lw==NULL) return 0;
    
//  name=spef_spi_devect(signame);
  if ((ANNOT_R_POS==NULL
       && (strcasecmp(signame, "pos")==0 || strcasecmp(signame, "1")==0))
      || (ANNOT_R_POS!=NULL && strcasecmp(signame, ANNOT_R_POS)==0)) pos=1;
  else if ((ANNOT_R_NEG==NULL
       && (strcasecmp(signame, "neg")==0 || strcasecmp(signame, "2")==0))
      || (ANNOT_R_NEG!=NULL && strcasecmp(signame, ANNOT_R_NEG)==0)) pos=0;
  else 
   {
      if (!SPEF_IN_CACHE) avt_errmsg(SPE_ERRMSG, "020", AVT_ERROR, spef_ParsedFile, Line, instname,signame);
      return 1;
   }

  ptsig = (losig_list *)getptype(lw->USER, SPEF_RESISIG)->DATA;
  spef_checkpnode_parasitic(pos==1?&lw->NODE1:&lw->NODE2, ptsig, &node);
  return 1;
}


void spef_AddTransistorConnector(char *allname, char *instname, char *signame,int nodenum)
{
  lotrs_list *lt;
  locon_list *lc;
  losig_list *ptsig;
  ptype_list *ptype;
  char *name, *dname;
  long value, node;
  char buf[1024];
  long updatepnode=-1;

#ifndef __ALL__WARNING__
  allname	= NULL; 
#endif
  lt=spef_getlotrsbyname(dname=spef_spi_devect(spef_check_keep(instname,'m',0)));
  if (lt==NULL && (name=namealloc(spef_check_keep(instname,'m',0)))!=dname)
    lt=spef_getlotrsbyname(name);
  if (lt==NULL && instname[0]=='*'){
      if((value = getititem(namemaptable, atoi(instname + 1))) != EMPTYHT){
          name = namealloc(spef_check_keep((char*)value,'m',0));
          lt = spef_getlotrsbyname(name);
          if (lt==NULL && vectorindex(name)!=-1)
            lt = spef_getlotrsbyname(mbk_vect(spef_check_keep(name,'m',0), '[', ']'));
      }
  }

  if (lt==NULL)
    {
      char mes[200];
      if (spef_AddResistorConnector(allname, instname, signame,nodenum)) return;
      if (spef_AddCapaOrDiodeConnector(allname, instname, signame,nodenum,'c')) return;
      if (spef_AddCapaOrDiodeConnector(allname, instname, signame,nodenum,'d')) return;

      if (!SPEF_IN_CACHE) avt_errmsg(SPE_ERRMSG, "018", AVT_ERROR, spef_ParsedFile, Line, instname);
      return;
    }
    
//  name=spef_spi_devect(signame);
  name=signame;

  if (strcasecmp(signame, ANNOT_T_G?ANNOT_T_G:"g")==0) lc=lt->GRID;
  else if (strcasecmp(signame, ANNOT_T_D?ANNOT_T_D:"d")==0) lc=lt->DRAIN;
  else if (strcasecmp(signame, ANNOT_T_S?ANNOT_T_S:"s")==0) lc=lt->SOURCE;
  else if (strcasecmp(signame, ANNOT_T_B?ANNOT_T_B:"b")==0) lc=lt->BULK;
  else lc=NULL;
  
  if (lc==NULL)
    {
      if (!SPEF_IN_CACHE) avt_errmsg(SPE_ERRMSG, "019", AVT_ERROR, spef_ParsedFile, Line, instname,signame);
      return;
    }

//  name=spef_spi_devect(allname);

  ptsig = lc->SIG;
  if (!SPEF_IN_CACHE) spef_examine_add_shortcircuit(nodenum, lc);
  spef_checkpnode(lc, &node);

  if ((ptype=getptype(lc->USER, PNODENAME))==NULL) ptype=lc->USER=addptype(lc->USER, PNODENAME, NULL);
  sprintf(buf,"%s%c%s", instname, SPEF_INFO->DELIMITER, signame);
  freechain((chain_list *)ptype->DATA);
  ptype->DATA=addchain(NULL, namealloc(buf));
}

// add an instance connector to the local list of nodes
void spef_AddInstanceConnector(char *allname, char *instname, char *signame, int nodenum)
{
  loins_list *ls;
  locon_list *lc;
  losig_list *ptsig;
  ptype_list *ptype;
  char *name, *dname;
  long value, node;
  char buf[1024];
  long updatepnode=-1;

  if (spef_getcurnet()==NULL) return;

  ls=spef_getloinsbyname(dname=spef_spi_devect(spef_check_keep(instname,'x',0)));
  if (ls==NULL && (name=namealloc(spef_check_keep(instname,'x',0)))!=dname)
    ls=spef_getloinsbyname(name);
    
  if (ls==NULL && instname[0]=='*'){
      if((value = getititem(namemaptable, atoi(instname + 1))) != EMPTYHT){
          name = namealloc((char*)value);
          ls = spef_getloinsbyname(spef_check_keep(name,'x',0));
          if (ls==NULL && vectorindex(name)!=-1)
            ls = spef_getloinsbyname(mbk_vect(spef_check_keep(name,'x',0), '[', ']'));
      }
  }

  if (ls==NULL)
    {
      spef_AddTransistorConnector(allname, instname, signame, nodenum);
      return;
    }
    
  name=spef_spi_devect(signame);
  for (lc=ls->LOCON;lc!=NULL && lc->NAME!=name;lc=lc->NEXT) ;
  
  if (lc==NULL)
    {
      avt_errmsg(SPE_ERRMSG, "017", AVT_ERROR, spef_ParsedFile, Line, instname,signame);
      return;
    }

//  name=spef_spi_devect(allname); // <- ca sert a qq chose??

  
  ptsig = lc->SIG;
  if (!SPEF_IN_CACHE) spef_examine_add_shortcircuit(nodenum, lc);
  spef_checkpnode(lc, &node);
  
  if ((ptype=getptype(lc->USER, PNODENAME))==NULL) ptype=lc->USER=addptype(lc->USER, PNODENAME, NULL);
  sprintf(buf,"%s%c%s", instname, SPEF_INFO->DELIMITER, signame);
  freechain((chain_list *)ptype->DATA);
  ptype->DATA=addchain(NULL, namealloc(buf));
}

/****************************************************************************/
// add a connector to the local list of nodes
void spef_AddConnector(char *sig, int nodenum)
{
  losig_list *ptsig;
  ptype_list *ptype;
  locon_list *lc;
  char *name;
  long value, node;
  long updatepnode=-1;

  if (spef_getcurnet()==NULL) return;

  name=spef_spi_devect(sig);
  if (sig[0] == '*'){
      if((value = getititem(namemaptable, atoi(sig + 1))) != EMPTYHT){
          name = namealloc((char*)value);
          for (lc = Lofig->LOCON; lc!=NULL && lc->NAME!=name; lc=lc->NEXT) ;
      }
  }else{
      for (lc = Lofig->LOCON; lc!=NULL && lc->NAME!=name; lc=lc->NEXT) ;
  }

  if (lc==NULL)
  {
    avt_errmsg(SPE_ERRMSG, "024", AVT_ERROR, spef_ParsedFile, Line, name);
    return;
  }
  
  ptsig = lc->SIG;
  if (!SPEF_IN_CACHE) spef_examine_add_shortcircuit(nodenum, lc);
  spef_checkpnode(lc, &node);

  if ((ptype=getptype(lc->USER, PNODENAME))==NULL)
    ptype=lc->USER=addptype(lc->USER, PNODENAME, NULL);
  freechain((chain_list *)ptype->DATA);
  ptype->DATA=addchain(NULL, name);
}

static int spef_has_parasitics(losig_list *ls)
{
  if (mbk_LosigIsVDD(ls) || mbk_LosigIsVSS(ls) || getptype(ls->USER, SPEF_RESISIG)==NULL) return 0;
  return 1;
}

/****************************************************************************/
// add a capacitance between 2 nodes
chain_list *spef_AddCapacitance(chain_list *ctclist, char *namesig0, char *namesig1, float capa)
{
  losig_list *sig0, *sig1;
  long node0, node1, *pnode0, *pnode1;
  char flag = 0;
  loctc_list *ctcadded;

  if (spef_getcurnet()==NULL)
  {
    if (!SPEF_IN_CACHE) MBK_PARSE_ERROR.NB_CAPA++;
    return ctclist;
  }

  spef_recupnodeandsig (Lofig, namesig0, thegroundsignal, &sig0, &node0, &pnode0);
  if (sig0==NULL)
  {
    if (!SPEF_IN_CACHE)
    {
      MBK_PARSE_ERROR.NB_CAPA++;
      avt_errmsg(SPE_ERRMSG, "023", AVT_ERROR, spef_ParsedFile, Line, namesig0);
    }
    return ctclist;
  }
  if(sig0->PRCN)
      if(sig0->PRCN->NBNODE <= node0)
          sig0->PRCN->NBNODE = node0 + 1;

  flag = spef_recupnodeandsig (Lofig, namesig1, thegroundsignal, &sig1, &node1, &pnode1);
  if (sig1==NULL)
  {
    if (!SPEF_IN_CACHE)
    {
      MBK_PARSE_ERROR.NB_CAPA++;
      avt_errmsg(SPE_ERRMSG, "023", AVT_ERROR, spef_ParsedFile, Line, namesig1);
    }
    return ctclist;
  }
  if(sig1->PRCN)
      if(sig1->PRCN->NBNODE <= node1)
          sig1->PRCN->NBNODE = node1 + 1;

#ifdef SPEF_DEBUG
  printf(" C: %s (%ld) %s (%ld) %g\n",(char *)sig0->NAMECHAIN->DATA,node0,(char *)sig1->NAMECHAIN->DATA,node1,capa);
#endif
  if (SPEF_PRESERVE_RC && (spef_has_parasitics(sig0) || spef_has_parasitics(sig1)))
  {
    if(flag){
        addloctc(sig0,node0,sig1,node1,capa);
        rcn_addcapa( sig0, capa );
        rcn_addcapa( sig1, capa );
    }else
    {
      ctclist = spef_addloctc(ctclist,sig0,sig1,node0,node1,capa,&ctcadded);
      if (ctcadded!=NULL && pnode0!=NULL) post_connection=addptype(post_connection, (long)&ctcadded->NODE1, pnode0);
      if (ctcadded!=NULL && pnode1!=NULL) post_connection=addptype(post_connection, (long)&ctcadded->NODE2, pnode1);
    }
  }
  else if(SPEF_PRELOAD == 'N'){
      if(SPEF_LOFIG_CACHE){
          if(rcn_cache_addable_ctc(sig0, sig1)){
              ctcadded = addloctc(sig0,node0,sig1,node1,capa);
              if( ctcadded )
                nbloadelem += RCN_SIZEOFLOCTC;
          }
      }else{          
          if(flag){
              addloctc(sig0,node0,sig1,node1,capa);
              rcn_addcapa( sig0, capa );
              rcn_addcapa( sig1, capa );
          }else
              ctclist = spef_addloctc(ctclist,sig0,sig1,node0,node1,capa,&ctcadded);
      }
      
  }
  return ctclist;
}

/***********************************************************************************/
// add a resistor between 2 nodes
void spef_AddResistor(char *namesig0, char *namesig1, float resi)
{
  losig_list *sig0, *sig1;
  long node0, node1, *pnode0, *pnode1;
  lowire_list *wireadded;

  if (spef_getcurnet()==NULL)
  {
    if (!SPEF_IN_CACHE) MBK_PARSE_ERROR.NB_RESI++;
    return;
  }

  spef_recupnodeandsig (Lofig, namesig0, thegroundsignal, &sig0, &node0, &pnode0);

  if (sig0==NULL)
  {
   if (!SPEF_IN_CACHE) MBK_PARSE_ERROR.NB_RESI++;
   return ;
  }
  if(sig0->PRCN)
      if(sig0->PRCN->NBNODE <= node0)
          sig0->PRCN->NBNODE = node0 + 1;

  spef_recupnodeandsig (Lofig, namesig1, thegroundsignal, &sig1, &node1, &pnode1);

  if (sig1==NULL)
  {
    if (!SPEF_IN_CACHE) MBK_PARSE_ERROR.NB_RESI++;
    return ;
  }
  if(sig1->PRCN)
      if(sig1->PRCN->NBNODE <= node1)
          sig1->PRCN->NBNODE = node1 + 1;

  if (sig1!=sig0)
    {
     if (!SPEF_IN_CACHE)
     {
       avt_errmsg(SPE_ERRMSG, "022", AVT_ERROR, spef_ParsedFile, Line, getsigname(sig0), getsigname(sig1));
       if (!SPEF_IN_CACHE) MBK_PARSE_ERROR.NB_RESI++;
     }
     return;
    }

#ifdef SPEF_DEBUG
  printf(" R: %s (%ld)-(%ld) %g\n",(char *)sig0->NAMECHAIN->DATA,node0,node1,resi);
#endif

  if (getptype(spef_getcurnet()->USER, SPEF_RESISIG)!=NULL)
  {
    wireadded=addlowire(sig0,0,resi,0.0,node0,node1);
    if (wireadded)
    {
      if (pnode0!=NULL) post_connection=addptype(post_connection, (long)&wireadded->NODE1, pnode0);
      if (pnode1!=NULL) post_connection=addptype(post_connection, (long)&wireadded->NODE2, pnode1);
    }
  } else if(SPEF_PRELOAD == 'N'){
      wireadded = addlowire(sig0,0,resi,0.0,node0,node1);
      if(SPEF_LOFIG_CACHE && wireadded)
          nbloadelem += RCN_SIZEOFLOWIRE;
  }
}

/****************************************************************************/
// select a new net, ste its capacitance and initiate the RC building
losig_list *spef_NewNet(char *sig, float capa)
{

  losig_list *ls;
#ifdef SPEF_DEBUG
  printf("NET %s %g\n",sig,capa);
#else
  capa	= 0.0;
#endif
  
  currentnet=ls=spef_getlosigbyname((sig));

  if (ls==NULL) 
    {
      if (!SPEF_IN_CACHE)
      {
        avt_errmsg(SPE_ERRMSG, "016", AVT_ERROR, spef_ParsedFile, Line, sig);
        MBK_PARSE_ERROR.NB_NET++;
      }
      return NULL;
    }

  if (givelorcnet(ls)==NULL)
    addlorcnet(ls);

  if (!SPEF_IN_CACHE) spef_examine_set_nodes(ls);
  
  /* Mis en commentaire par Grégoire le 2/01/03
  Cause : la capacité dans le fichier ne correspond pas à la somme totale
  des capacités que l'on trouve. Dans le cas le plus simple, ce n'est pas la
  même unité (capa des RC en pf et capa totale en fF !!!), et dans certains cas,
  il n'y a aucuns rapport. Il vaut donc mieux le recalculer proprement.
  
  rcn_setcapa(ls, capa);
  */
  return ls;
}

/****************************************************************************/
// set the name of the ground signal
void spef_SetGroundSignal(char *sig)
{
#ifdef SPEF_DEBUG
  printf("ground is %s\n",sig);
#endif
  thegroundsignal=spef_spi_devect(sig);
}

/****************************************************************************/
void spef_setneg2posnode(lofig_list *Lofig)
{
    locon_list  *ptcon;
    losig_list  *ptsig;
    lowire_list *ptwire;
    loctc_list  *ptctc;
    chain_list  *chain;
    ptype_list *ptype ;
    num_list    *num;
    long node ;

    for (ptype=post_connection; ptype!=NULL; ptype=ptype->NEXT)
    {
//      printf("post move %p->%p %ld -> %ld\n",ptype->TYPE, ptype->DATA, *(long *)ptype->TYPE, *(long *)ptype->DATA);
      *(long *)ptype->TYPE=*(long *)ptype->DATA;
    }
    freeptype(post_connection);
    post_connection=NULL;
    
    lofigchain(Lofig) ;
    for(ptsig = Lofig->LOSIG; ptsig; ptsig = ptsig->NEXT){
        if ((ptype=getptype(ptsig->USER, SPEF_ATTRIB_HT))!=NULL)
        {
           delht((ht *)ptype->DATA);
           ptsig->USER=delptype(ptsig->USER, SPEF_ATTRIB_HT);
        }
        if(ptsig->PRCN){
            node = (long)0 ;
            for(ptwire = ptsig->PRCN->PWIRE; ptwire; ptwire = ptwire->NEXT){
                if(ptwire->NODE1 < 0)
                    ptwire->NODE1 = ptsig->PRCN->NBNODE + abs(ptwire->NODE1) - 1;
                if(ptwire->NODE2 < 0)
                    ptwire->NODE2 = ptsig->PRCN->NBNODE + abs(ptwire->NODE2) - 1;
                if(node < ptwire->NODE1)
                    node = ptwire->NODE1 ;
                if(node < ptwire->NODE2)
                    node = ptwire->NODE2 ;
                ptwire->USER=testanddelptype(ptwire->USER, SPEF_RESISIG);
            }
            for(chain = ptsig->PRCN->PCTC; chain; chain = chain->NEXT){
                ptctc = (loctc_list*)chain->DATA;
                if(ptctc->NODE1 < 0)
                    ptctc->NODE1 = ptctc->SIG1->PRCN->NBNODE + abs(ptctc->NODE1) - 1;
                if(ptctc->NODE2 < 0)
                    ptctc->NODE2 = ptctc->SIG2->PRCN->NBNODE + abs(ptctc->NODE2) - 1;
                if(ptsig == ptctc->SIG1)
                    if(node < ptctc->NODE1)
                        node = ptctc->NODE1 ;
                if(ptsig == ptctc->SIG2)
                    if(node < ptctc->NODE2)
                        node = ptctc->NODE2 ;
            }
            ptype = getptype(ptsig->USER,LOFIGCHAIN) ;
            for(chain = (chain_list *)ptype->DATA; chain; chain = chain->NEXT) {
                ptcon = (locon_list *)chain->DATA ;
                for(num = ptcon->PNODE; num; num = num->NEXT) {
                if(num->DATA < 0)
                    num->DATA = ptsig->PRCN->NBNODE + abs(num->DATA) - 1;
                if(node < num->DATA)
                    node = num->DATA ;
                }
            }
           ptsig->PRCN->NBNODE = node + 1 ; 
           if(getptype(ptsig->USER,SPEF_NODE_MIN) != NULL)
               ptsig->USER = delptype(ptsig->USER,SPEF_NODE_MIN) ;
        }
    }
}
