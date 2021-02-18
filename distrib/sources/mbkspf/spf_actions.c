/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf_actions.c                                               */
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

#include AVT_H
#include MUT_H
#include MLO_H
#include EQT_H
#include MSL_H
#include "spf_actions.h"
#include "spf_annot.h"
#include "spf_util.h"

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

#define LOCAL_NET_HTABE_SIZE 50

static ht *local_net_htable=NULL, *local_net_htable_connector=NULL, *local_node_htable=NULL;
static ht *local_resi_htable=NULL, *local_capa_htable=NULL;
static ht *pending_crosstalk_capacitances=NULL;
static int nodecounter, pending_node_counter;
static char *thegroundsignal="";
char *spf_ParsedFile="";
int spf_missing_signals;

static losig_list *currentnet=NULL;
static void spf_tag_node(locon_list *lc, ht *nodes);
  
typedef struct
{
  losig_list *signal;
  int id, realid, line;
  chain_list *crosstalk_capacitances;
} pending_capacitances_info;

typedef struct pending_capacitance
{
  struct pending_capacitance *next;
  losig_list *sig1, *sig2;
  int num1, num2;
  float val;
} pending_capacitance;

typedef struct spf_node
{
  losig_list *ls;
  long nodenum;
} spf_node;

static pending_capacitance *all_pending_capacitance=NULL;
static HeapAlloc pending_crosstalk_capacitances_heap, pending_crosstalk_capacitances_info_heap;
static AdvancedNameAllocator *spf_ana=NULL;
static AdvancedTableAllocator *spf_ata;

/****************************************************************************/
/*     private                                                              */
/****************************************************************************/

pending_capacitances_info *spf_getpendinginfo(char *name)
{
  long l;
  if (pending_crosstalk_capacitances==NULL) return NULL;
  if ((l=gethtitem(pending_crosstalk_capacitances, name))==EMPTYHT) return NULL;
  return (pending_capacitances_info *)l;
}

pending_capacitances_info *spf_givependinginfo(char *name)
{
  long l;
  pending_capacitances_info *pci;
  if ((pci=spf_getpendinginfo(name))==NULL)
    {
      pci=(pending_capacitances_info *)AddHeapItem(&pending_crosstalk_capacitances_info_heap);
      pci->signal=NULL;
      pci->id=0; pci->realid=-1; pci->line=spf_Line;
      pci->crosstalk_capacitances=NULL;
      addhtitem(pending_crosstalk_capacitances, name, (long)pci);
    }
  return pci;
}
void spf_associate_node(char *name, losig_list *ls, long nodenum)
{
  int idx;
  int exists;
  spf_node *sn;
  if (spf_ana==NULL)
  {
    spf_ana=CreateAdvancedNameAllocator('n');
    spf_ata=CreateAdvancedTableAllocator (1000, sizeof (spf_node));
  }
  idx=AdvancedNameAllocEx (spf_ana, name, &exists);
  sn = (spf_node *) GetAdvancedTableElem (spf_ata, idx);
  sn->ls=ls;
  sn->nodenum=nodenum;
}

void spf_check_update_pending_node(char *name, losig_list *ls, int nodenum)
{
  pending_capacitances_info *pci;
  if ((pci=spf_getpendinginfo(name))!=NULL)
    {
      if (pci->realid<0)
        {
          pci->signal=ls;
          pci->realid=nodenum;
//          printf("| update %s : %s %d\n", name, getsigname(ls), nodenum);
        }
    }
  spf_associate_node(name, ls, nodenum);
}
int spf_find_pending_node(char *name, losig_list **ls, long *nodenum)
{
  pending_capacitances_info *pci;
  int exists, idx;
  spf_node *sn;
  if ((pci=spf_getpendinginfo(name))!=NULL)
    {
      if (pci->realid>=0)
        {
          *ls=pci->signal;
          *nodenum=pci->realid;
//          printf("| found %s : %s %ld\n", name, getsigname(*ls), *nodenum);
          return 1;
        }
    }
   else
    {
       if (spf_ana==NULL)
       {
         spf_ana=CreateAdvancedNameAllocator('n');
         spf_ata=CreateAdvancedTableAllocator (1000, sizeof (spf_node));
       }
       idx=AdvancedNameAllocEx (spf_ana, name, &exists);
       sn = (spf_node *) GetAdvancedTableElem (spf_ata, idx);
       if (!exists) sn->ls=NULL;
       if (sn->ls!=NULL)
       {
         *ls=sn->ls;
         *nodenum=sn->nodenum;
         return 1;
       }
    }
  return 0;
}

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

double spf_femto() 
{
	return (1e-15) ; 
} 

/****************************************************************************/

double spf_pico() 
{
	return (1e-12) ; 
} 

/****************************************************************************/

double spf_nano() 
{
	return (1e-9) ; 
} 

/****************************************************************************/

double spf_micro() 
{
	return (1e-6) ; 
} 

/****************************************************************************/

double spf_milli()
{
	return (1e-3) ; 
} 

/****************************************************************************/

double spf_kilo() 
{
	return (1e3) ; 
} 

static char *spf_check_keep(char *name, char ch)
{
  if ((SPF_CARDS & KEEP__INSTANCE)==0 && ch=='x')
    {
      if (tolower(name[0])=='x') return &name[1];
    }
  if ((SPF_CARDS & KEEP__TRANSISTOR)==0 && ch=='m')
    {
      if (tolower(name[0])=='m') return &name[1];
    }
  if ((SPF_CARDS & KEEP__RESISTANCE)==0 && ch=='r')
    {
      if (tolower(name[0])=='r') return &name[1];
    }
  if ((SPF_CARDS & KEEP__DIODE)==0 && ch=='d')
    {
      if (tolower(name[0])=='d') return &name[1];
    }
  if ((SPF_CARDS & KEEP__CAPA)==0 && ch=='c')
    {
      if (tolower(name[0])=='c') return &name[1];
    }
  return name;
}

// add a transistor connector to the local list of nodes
void spf_AddTransistorConnector(char *allname, char *instname, char *signame)
{
  lotrs_list *tr;
  locon_list *lc;
  char *name, *ourname;
  ptype_list *pt;

  tr=getlotrsbyname(ourname=spf_spi_devect(spf_check_keep(instname,'m')));
  if (tr==NULL)
    {
      avt_errmsg(SPF_ERRMSG, "012", AVT_WARNING, ourname, spf_Line);
      return;
    }
  
  if (strcasecmp(signame, ANNOT_T_G?ANNOT_T_G:"g")==0) lc=tr->GRID;
  else if (strcasecmp(signame, ANNOT_T_D?ANNOT_T_D:"d")==0) lc=tr->DRAIN;
  else if (strcasecmp(signame, ANNOT_T_S?ANNOT_T_S:"s")==0) lc=tr->SOURCE;
  else if (strcasecmp(signame, ANNOT_T_B?ANNOT_T_B:"b")==0) lc=tr->BULK;
  else lc=NULL;
  
  if (lc==NULL)
    {
      avt_errmsg(SPF_ERRMSG, "013", AVT_WARNING, ourname, signame, spf_Line);
      return;
    }

  name=spf_spi_devect(allname);

  if (SPF_PREVERSE_RC)
  {
    spf_tag_node(lc, local_node_htable);
    if (gethtitem(local_net_htable,name)!=EMPTYHT)
    {
      long l;
      if ((l=gethtitem(local_net_htable_connector, name))!=EMPTYHT)
      {
        lc=(locon_list *)l;
        if ((pt=getptype(lc->USER, PNODENAME))!=NULL)
        {
          freechain((chain_list *)pt->DATA);
          lc->USER = delptype( lc->USER, PNODENAME );
        }
        lc->USER = addptype(lc->USER, PNODENAME, addchain(NULL, name));
        spf_check_update_pending_node(name, lc->SIG, lc->PNODE->DATA);
      }
      return;
    }
  }
  
  addhtitem(local_net_htable,name,nodecounter);
  addhtitem(local_net_htable_connector,name,(long)lc);

  // une incoherence dans le dspf
  if (lc->SIG->PRCN==NULL)
  {
    avt_errmsg(SPF_ERRMSG, "014", AVT_WARNING, ourname, signame, spf_Line);
    addlorcnet(lc->SIG);
  }
  // ----
   
  if (lc->PNODE!=NULL) { freenum(lc->PNODE); lc->PNODE=NULL; }
  setloconnode(lc,nodecounter); 

  if ((pt=getptype(lc->USER, PNODENAME))!=NULL)
  {
    freechain((chain_list *)pt->DATA);
    lc->USER = delptype( lc->USER, PNODENAME );
  }
  lc->USER = addptype(lc->USER, PNODENAME, addchain(NULL, name));
  spf_check_update_pending_node(name, lc->SIG, lc->PNODE->DATA);
	
#ifdef SPF_DEBUG
  printf(" T: %s %s %s (%d) is %s\n",allname,instname, signame,nodecounter,(char *)lc->SIG->NAMECHAIN->DATA);
#endif

  nodecounter++;
}

int spf_AddResistanceConnector(char *allname, char *instname, char *signame)
{
  int pos;
  lowire_list *lw;
  ptype_list *pt;
  long nodefound=-1, l;
  char *name;
  
  if (local_resi_htable==NULL || currentnet==NULL || currentnet->PRCN==NULL || currentnet->PRCN->PWIRE==NULL) return 0;

  name=spf_spi_devect(spf_check_keep(instname,'r'));
  l=gethtitem(local_resi_htable, name);
  if (l==EMPTYHT) return 0;

  if ((ANNOT_R_POS==NULL
       && (strcasecmp(signame, "pos")==0 || strcasecmp(signame, "1")==0))
      || (ANNOT_R_POS!=NULL && strcasecmp(signame, ANNOT_R_POS)==0)) pos=1;
  else if ((ANNOT_R_NEG==NULL
       && (strcasecmp(signame, "neg")==0 || strcasecmp(signame, "2")==0))
      || (ANNOT_R_NEG!=NULL && strcasecmp(signame, ANNOT_R_NEG)==0)) pos=0;
  else
   {
     avt_errmsg(SPF_ERRMSG, "022", AVT_WARNING, signame, spf_Line);
     return 0;
   }

  lw=(lowire_list *)l;
  
  if ((pt=getptype(lw->USER, RESINAME))!=NULL && pt->DATA==name)
    {
      if (pos) nodefound=lw->NODE1;
      else nodefound=lw->NODE2;
      if (!(SPF_PREVERSE_RC && gethtitem(local_node_htable, (void *)nodefound)==EMPTYHT))
      {
        if (pos) { lw->NODE1=nodecounter; nodefound=lw->NODE1; }
        else { lw->NODE2=nodecounter; nodefound=lw->NODE2; }
        nodecounter++;
      }
      else addhtitem(local_node_htable, (void *)nodefound, 0);
    }
  
  if (nodefound==-1) return 0;
  
  addhtitem(local_net_htable,spf_spi_devect(allname),nodefound);
  spf_check_update_pending_node(name, currentnet, nodefound);
  return 1;
}

int spf_AddCapacitanceConnector(char *allname, char *instname, char *signame, char start)
{
  int pos;
  loctc_list *pctc;
  ptype_list *pt;
  long nodefound=-1, l;
  char *name, *an;
  losig_list *ls;
  char *posi, *negi;
  
  if (local_capa_htable==NULL || currentnet==NULL || currentnet->PRCN==NULL || currentnet->PRCN->PCTC==NULL) return 0;

  name=spf_spi_devect(spf_check_keep(instname,start));
  l=gethtitem(local_capa_htable, name);
  if (l==EMPTYHT) return 0;

  if (start=='d') posi=ANNOT_D_POS, negi=ANNOT_D_NEG;
  else  posi=ANNOT_C_POS, negi=ANNOT_C_NEG;

  if (strcasecmp(signame,posi?posi:"1")==0) pos=1;
  else if (strcasecmp(signame,negi?negi:"2")==0) pos=0;
  else 
   {
     avt_errmsg(SPF_ERRMSG, "026", AVT_WARNING, signame, spf_Line);
     return 0;
   }

  pctc=(loctc_list *)l;

  if ((pt=getptype(pctc->USER, MSL_CAPANAME))!=NULL && pt->DATA==name)
    {
      if (pos) nodefound=pctc->NODE1, ls=pctc->SIG1;
      else nodefound=pctc->NODE2, ls=pctc->SIG2;
      if (!(SPF_PREVERSE_RC && gethtitem(local_node_htable, (void *)nodefound)==EMPTYHT))
      {
        if (pos) { pctc->NODE1=nodecounter; nodefound=pctc->NODE1; ls=pctc->SIG1; }
        else { pctc->NODE2=nodecounter; nodefound=pctc->NODE2;  ls=pctc->SIG2; }
        nodecounter++;
      }
      else addhtitem(local_node_htable, (void *)nodefound, 0);
    }
  
  if (nodefound==-1) return 0;
  
  addhtitem(local_net_htable,an=spf_spi_devect(allname),nodefound);
  spf_check_update_pending_node(an, ls, nodefound);
  return 1;
}

// add an instance connector to the local list of nodes
void spf_AddInstanceConnector(char *allname, char *instname, char *signame)
{
  loins_list *ls;
  locon_list *lc;
  char *name, *name0, *ourname;
  ptype_list *pt;
  long l;
  int cnt;
  
  if (currentnet==NULL) return;

  name0=spf_spi_devect(allname); // was spf_rename

  if (gethtitem(local_net_htable,name0)!=EMPTYHT)
  {
    if ((l=gethtitem(local_net_htable_connector, name0))!=EMPTYHT)
    {
      lc=(locon_list *)l;
      if ((pt=getptype(lc->USER, PNODENAME))!=NULL)
      {
        freechain((chain_list *)pt->DATA);
        lc->USER = delptype( lc->USER, PNODENAME );
      }
      lc->USER = addptype(lc->USER, PNODENAME, addchain(NULL, name0));
      spf_check_update_pending_node(name0, lc->SIG, lc->PNODE->DATA);
    }
    return;
  }
 
  if (spf_AddResistanceConnector(allname, instname, signame)) return;
  if (spf_AddCapacitanceConnector(allname, instname, signame, 'd')) return; // diode
  if (spf_AddCapacitanceConnector(allname, instname, signame, 'c')) return; // capa

  ls=getloinsbyname(ourname=spf_spi_devect(spf_check_keep(instname,'x'))); //getloins(Lofig,instname);
  if (ls==NULL)
    {
      if (spf_Lofig->LOTRS!=NULL)
        {
          spf_AddTransistorConnector(allname, instname, signame);
          return;
        }
      
      avt_errmsg(SPF_ERRMSG, "015", AVT_WARNING, ourname, spf_Line);
      return;
    }
    
  name=spf_spi_devect(signame); // was spf_rename
  for (lc=ls->LOCON;lc!=NULL && lc->NAME!=name;lc=lc->NEXT) ;
  
  if (lc==NULL)
    {
      avt_errmsg(SPF_ERRMSG, "016", AVT_WARNING, ourname, signame, spf_Line);
      return;
    }

//  name=spf_spi_devect(allname); // was spf_rename

  // to preverse the number of nodes on the connector
  // and avoid discrepency betwwen instance and model
  if ((cnt=lc->FLAGS)==0 || cnt==-1) cnt=1; 

  if (SPF_PREVERSE_RC)
  {
    spf_tag_node(lc, local_node_htable);
    if (gethtitem(local_net_htable,name0)!=EMPTYHT)
    {
      if ((l=gethtitem(local_net_htable_connector, name0))!=EMPTYHT)
      {
        lc=(locon_list *)l;
        if ((pt=getptype(lc->USER, PNODENAME))!=NULL)
        {
          freechain((chain_list *)pt->DATA);
          lc->USER = delptype( lc->USER, PNODENAME );
        }
        lc->USER = addptype(lc->USER, PNODENAME, addchain(NULL, name0));
        spf_check_update_pending_node(name, lc->SIG, lc->PNODE->DATA);
      }
      return;
    }
  }

  if (lc->PNODE!=NULL) { freenum(lc->PNODE); lc->PNODE=NULL; }

  addhtitem(local_net_htable,name0,nodecounter);
  addhtitem(local_net_htable_connector,name0,(long)lc);

  while (cnt>0)
    {
      setloconnode(lc,nodecounter); 
      nodecounter++;
      cnt--;
    }

  if ((pt=getptype(lc->USER, PNODENAME))!=NULL)
  {
    freechain((chain_list *)pt->DATA);
    lc->USER = delptype( lc->USER, PNODENAME );
  }
  lc->USER = addptype(lc->USER, PNODENAME, addchain(NULL, name0));
  spf_check_update_pending_node(name0, lc->SIG, lc->PNODE->DATA);

	
#ifdef SPF_DEBUG
  printf(" I: %s %s %s (%d) is %s\n",allname,$3,$4,nodecounter-1,(char *)lc->SIG->NAMECHAIN->DATA);
#endif

}

// add a connector to the local list of nodes
void spf_AddConnector(char *sig)
{
  locon_list *lc;
  char *name;
  ptype_list *pt;
  char mes[200];
  long l;
  int cnt;

  if (currentnet==NULL) return;

  name=spf_spi_devect(sig); // was spf_rename

  if (gethtitem(local_net_htable,name)!=EMPTYHT)
  {
   if ((l=gethtitem(local_net_htable_connector, name))!=EMPTYHT)
    {
      lc=(locon_list *)l;
      if ((pt=getptype(lc->USER, PNODENAME))!=NULL)
      {
        freechain((chain_list *)pt->DATA);
        lc->USER = delptype( lc->USER, PNODENAME );
      }
      lc->USER = addptype(lc->USER, PNODENAME, addchain(NULL, name));
      spf_check_update_pending_node(name, lc->SIG, lc->PNODE->DATA);
      return;
    }
  }
  for (lc=spf_Lofig->LOCON; lc!=NULL && lc->NAME!=name; lc=lc->NEXT) ;

//  lc=getlocon(spf_Lofig,name);
  if (lc==NULL)
    {
      avt_errmsg(SPF_ERRMSG, "017", AVT_WARNING, sig, spf_Line);
      return;
    }

  if (lc->SIG->PRCN==NULL)
  {
    addlorcnet(lc->SIG);
    avt_errmsg(SPF_ERRMSG, "018", AVT_WARNING, sig, spf_Line);
  }

  // to preverse the number of nodes on the connector
  // and avoid discrepency betwwen instance and model
  if ((cnt=lc->FLAGS)==0 || cnt==-1) cnt=1; 

  if (SPF_PREVERSE_RC)
  {
    spf_tag_node(lc, local_node_htable);
    if (gethtitem(local_net_htable,name)!=EMPTYHT)
    {
      if ((l=gethtitem(local_net_htable_connector, name))!=EMPTYHT)
      {
        lc=(locon_list *)l;
        if ((pt=getptype(lc->USER, PNODENAME))!=NULL)
        {
          freechain((chain_list *)pt->DATA);
          lc->USER = delptype( lc->USER, PNODENAME );
        }
        lc->USER = addptype(lc->USER, PNODENAME, addchain(NULL, name));
        spf_check_update_pending_node(name, lc->SIG, lc->PNODE->DATA);
      }
      return;
    }
  }

  if (lc->PNODE!=NULL) { freenum(lc->PNODE); lc->PNODE=NULL; }
  addhtitem(local_net_htable,name,nodecounter);
  addhtitem(local_net_htable_connector,name,(long)lc);
  while (cnt>0)
    {
      setloconnode(lc,nodecounter);
      nodecounter++;
      cnt--;
    }

  if ((pt=getptype(lc->USER, PNODENAME))!=NULL)
  {
    freechain((chain_list *)pt->DATA);
    lc->USER = delptype( lc->USER, PNODENAME );
  }
  lc->USER = addptype(lc->USER, PNODENAME, addchain(NULL, name));
  spf_check_update_pending_node(name, lc->SIG, lc->PNODE->DATA);

#ifdef SPF_DEBUG
  printf(" P: %s (%d)\n",sig,nodecounter-1);
#endif

}

// add a subnode to the local list of nodes
void spf_AddSubNode(char *sig)
{
  char *name;
  name=spf_spi_devect(sig); // was spf_rename
  
  if (gethtitem(local_net_htable,name)!=EMPTYHT) return;

  addhtitem(local_net_htable,name,nodecounter);
  spf_check_update_pending_node(name, currentnet, nodecounter);
#ifdef SPF_DEBUG
  printf(" S: %s (%d)\n",name,nodecounter);
#endif

  nodecounter++;
}

// add a capacitance between 2 nodes
void spf_AddCapacitance(char *namesig0, char *namesig1, float capa)
{
  losig_list *sig0, *sig1;
  long node0=-1, node1=-1;
  char *s1, *s0;
  pending_capacitances_info *pci;
  pending_capacitance *pc;
  ptype_list *pt;

  if (currentnet==NULL) {
    MBK_PARSE_ERROR.NB_CAPA++ ;
    return;
  }

  if (capa<0)
    {
      avt_errmsg(SPF_ERRMSG, "019", AVT_WARNING, spf_Line);
      MBK_PARSE_ERROR.NB_CAPA++ ;
      return;
    }

  getsignalandnode(spf_Lofig,currentnet,s0=spf_spi_devect(namesig0),local_net_htable,thegroundsignal,&sig0,&node0, 1);

  getsignalandnode(spf_Lofig,currentnet,s1=spf_spi_devect(namesig1),local_net_htable,thegroundsignal,&sig1,&node1, 1);

  if (sig0==groundlosig && node1==-1)
  {
    getsignalandnode(spf_Lofig,currentnet,s1,local_net_htable,thegroundsignal,&sig1,&node1, 0);
  }
  else if (sig1==groundlosig && node0==-1)
  {
    getsignalandnode(spf_Lofig,currentnet,s0,local_net_htable,thegroundsignal,&sig0,&node0, 0);
  }

#ifdef SPF_DEBUG
  printf(" C: %s (%ld) %s (%ld) %g\n",(char *)sig0->NAMECHAIN->DATA,node0,(char *)sig1->NAMECHAIN->DATA,node1,capa);
#endif

  if (node0!=-1 && node1!=-1)
    {
      if( !sig0->PRCN ) givelorcnet( sig0 );
      if( !sig1->PRCN ) givelorcnet( sig1 );
      addloctc(sig0,node0,sig1,node1,capa);
      if (sig0!=sig1)
      { 
        if ((pt=getptype(sig0->USER, SPF_ORIG_CAPA_SUM))==NULL)
          pt=sig0->USER=addptype(sig0->USER, SPF_ORIG_CAPA_SUM, (void *)0);
        *(float *)&pt->DATA += capa;
        if ((pt=getptype(sig1->USER, SPF_ORIG_CAPA_SUM))==NULL)
          pt=sig1->USER=addptype(sig1->USER, SPF_ORIG_CAPA_SUM, (void *)0);
        *(float *)&pt->DATA += capa;
      }
    }
  else
    {
      // pending
      if (pending_crosstalk_capacitances==NULL)
        {
          pending_crosstalk_capacitances=addht(1000);
          CreateHeap(sizeof(pending_capacitance), 10000, &pending_crosstalk_capacitances_heap);
          CreateHeap(sizeof(pending_capacitances_info), 10000, &pending_crosstalk_capacitances_info_heap);
          pending_node_counter=-1;
        }
      pc=(pending_capacitance *)AddHeapItem(&pending_crosstalk_capacitances_heap);
      pc->sig1=sig0;
      pc->sig2=sig1;
      pc->val=capa;
      pc->next=all_pending_capacitance;
      all_pending_capacitance=pc;
      if (node0==-1)
        {
          pci=spf_givependinginfo(s0);
          if (pci->signal==NULL) pci->signal=currentnet;
          if (pci->id==0) pci->id=pending_node_counter--;
          pc->num1=pci->id;
          pci->crosstalk_capacitances=addchain(pci->crosstalk_capacitances, pc);
        }
      else
        pc->num1=node0;

      if (node1==-1)
        {
          pci=spf_givependinginfo(s1);
          if (pci->signal==NULL) pci->signal=currentnet;
          if (pci->id==0) pci->id=pending_node_counter--;
          pc->num2=pci->id;
          pci->crosstalk_capacitances=addchain(pci->crosstalk_capacitances, pc);
        }
      else
        pc->num2=node1;
                        
    }
}

// add a resistor between 2 nodes
void spf_AddResistor(char *namesig0, char *namesig1, float resi)
{
  losig_list *sig0, *sig1;
  long node0=-1, node1=-1;

  if (currentnet==NULL) {
    MBK_PARSE_ERROR.NB_RESI++ ;
    return;
  }

  if (resi<0)
    {
      avt_errmsg(SPF_ERRMSG, "020", AVT_WARNING, spf_Line);
      MBK_PARSE_ERROR.NB_RESI++ ;
      return;
    }

  getsignalandnode(spf_Lofig,currentnet,spf_spi_devect(namesig0),local_net_htable,thegroundsignal,&sig0,&node0, 0);

  if (node0==-1) {
    MBK_PARSE_ERROR.NB_RESI++ ;
    return;
  }
/*  if (sig0==NULL)
    EXIT(50);*/


  getsignalandnode(spf_Lofig,currentnet,spf_spi_devect(namesig1),local_net_htable,thegroundsignal,&sig1,&node1, 0);

  if (node1==-1) {
    MBK_PARSE_ERROR.NB_RESI++ ;
    return;
  }
/*  if (sig1==NULL)
    EXIT(50);*/

#ifdef SPF_DEBUG
  printf(" R: %s (%ld)-(%ld) %g\n",(char *)sig0->NAMECHAIN->DATA,node0,node1,resi);
#endif

  addlowire(sig0,0,resi,0.0,node0,node1);
}

static void spf_tag_node(locon_list *lc, ht *nodes)
{
  ptype_list *pt;
  num_list *nl;
  char buf[1024], *cname, *sn;
  long anode;
  lotrs_list *ltr;
  loins_list *li;
  chain_list *ph_interf, *real_interf;

  if (lc->TYPE=='E')
    {
      ph_interf=NULL;
      if ((pt=getptype(spf_Lofig->USER, PH_INTERF))!=NULL) ph_interf=(chain_list *)pt->DATA;

      real_interf=NULL;
      if ((pt=getptype(spf_Lofig->USER, PH_REAL_INTERF))!=NULL) real_interf=(chain_list *)pt->DATA;

      if (ph_interf==NULL || real_interf==NULL) ph_interf=real_interf=NULL;
    }

  if (lc->TYPE=='E' && ph_interf!=NULL && lc->PNODE!=NULL && lc->PNODE->NEXT!=NULL)
    {
      for (nl=lc->PNODE; nl!=NULL; nl=nl->NEXT)
        {
          while (ph_interf!=NULL && real_interf!=NULL && ph_interf->DATA!=lc->NAME)
            {
              ph_interf=ph_interf->NEXT;
              real_interf=real_interf->NEXT;
            }
          if (ph_interf!=NULL && real_interf!=NULL)
            {
              anode=nl->DATA;
              cname=(char *)real_interf->DATA;
              //                  printf("%ld of %s -> %s",anode,lc->NAME,cname);
              addhtitem(local_net_htable, sn=spf_spi_devect(cname), anode);
              spf_check_update_pending_node(sn, lc->SIG, anode);
              addhtitem(local_net_htable_connector, sn, (long)lc);
              addhtitem(nodes, (void *)anode, 0);
              ph_interf=ph_interf->NEXT;
              real_interf=real_interf->NEXT;
            }
        }
    }
  else
    {    
      anode=-1;
      for (nl=lc->PNODE; nl!=NULL; nl=nl->NEXT)
        {
          anode=nl->DATA;
        }
      if (anode!=-1 && gethtitem(nodes, (void *)anode)==EMPTYHT) 
        { 
          if (lc->TYPE=='T')
            {
              ltr=(lotrs_list *)lc->ROOT;
              if (ltr->TRNAME!=NULL)
                sprintf(buf, "%s%c%c", ltr->TRNAME,spf_getDelimiter(),tolower(lc->NAME[0]));
              else
                strcpy(buf,"");
            }
          else if (lc->TYPE=='I')
            {
              li=(loins_list *)lc->ROOT;
              sprintf(buf, "%s%c%s", li->INSNAME,spf_getDelimiter(),lc->NAME);
            }
          else
            sprintf(buf, "%s", lc->NAME);
  
          if (buf[0]!='\0')
            {
              //          printf("%s -> %ld\n",spf_spi_devect(buf), anode);
              addhtitem(local_net_htable, sn=spf_spi_devect(buf), anode);
              spf_check_update_pending_node(sn, lc->SIG, anode);
              addhtitem(local_net_htable_connector, sn, (long)lc);
              addhtitem(nodes, (void *)anode, 0);
            }
        }
    }
}

void spf_grabexistingparasitics(losig_list *ls, int markonly)
{
  chain_list *cl;
  ptype_list *pt;
  locon_list *lc;

  if ((pt=getptype(ls->USER, LOFIGCHAIN))!=NULL) cl=(chain_list *)pt->DATA;
  else cl=NULL;

  while (cl!=NULL)
    {
      lc=(locon_list *)cl->DATA;
      if (markonly) lc->FLAGS=countchain((chain_list *)lc->PNODE);
      else lc->FLAGS=-1;
      cl=cl->NEXT;
    }
}

void filterparasiticsonalim(losig_list *me)
{
  chain_list *cl, *ncl, *scancon;
  loctc_list *lctc;
  ptype_list *pt;
  locon_list *ptcon;
  
  for (cl=me->PRCN->PCTC; cl!=NULL; cl=ncl)
  {
    ncl=cl->NEXT;
    lctc=(loctc_list*)cl->DATA;
    if ((lctc->SIG1==me && lctc->NODE1!=0) || (lctc->SIG2==me && lctc->NODE2!=0))
      delloctc( lctc );
  }
  while( me->PRCN->PWIRE )
    dellowire(me, me->PRCN->PWIRE->NODE1, me->PRCN->PWIRE->NODE2);
  
  if ((pt=getptype(me->USER, LOFIGCHAIN))!=NULL) cl=(chain_list *)pt->DATA;
  else cl=NULL;
  for( scancon = cl ; scancon ; scancon = scancon->NEXT )
  {
    ptcon = (locon_list*)(scancon->DATA);

    if( ptcon->PNODE )
    {
      freenum(ptcon->PNODE);
      ptcon->PNODE = NULL;
    }
  }

}

// select a new net, ste its capacitance and initiate the RC building
void spf_NewNet(char *sig, float capa)
{

  losig_list *ls, *newnet;
  lowire_list *lw;
  ptype_list *pt;

#ifdef SPF_DEBUG
  printf("NET %s %g\n",sig,capa);
#endif

  newnet=getlosigbyname(spf_spi_devect(sig));
  if (newnet!=NULL)
  {
    if ((pt=getptype(newnet->USER, SPF_ORIG_CAPA_TOT))==NULL)
      pt=newnet->USER=addptype(newnet->USER, SPF_ORIG_CAPA_TOT, (void *)0);

    *(float *)&pt->DATA += capa;
  }
  if (currentnet!=NULL)
    {
      if (newnet==currentnet) 
       {
         avt_errmsg(SPF_ERRMSG, "025", AVT_WARNING, sig, spf_Line);
         return;
       }
    }

  if (local_net_htable!=NULL) delht(local_net_htable);
  local_net_htable=addht(LOCAL_NET_HTABE_SIZE);
  if (local_net_htable_connector!=NULL) delht(local_net_htable_connector);
  local_net_htable_connector=addht(LOCAL_NET_HTABE_SIZE);
  if (local_resi_htable!=NULL) delht(local_resi_htable);
  local_resi_htable=NULL;
  if (local_node_htable!=NULL) delht(local_node_htable);
  local_node_htable=addht(512);
  if (local_capa_htable!=NULL) delht(local_capa_htable);
  local_capa_htable=NULL;
  nodecounter=1;
  currentnet=NULL;

  if (mbk_CheckREGEX(&SPF_IGNORE_SIGNAL, sig)!=0) return;

  currentnet=ls=newnet; //getlosigbyname(spf_spi_devect(sig));

  if (ls==NULL) { 
    spf_missing_signals++; 
    MBK_PARSE_ERROR.NB_NET++ ; 
    avt_errmsg(SPF_ERRMSG, "023", AVT_WARNING, sig, spf_Line);
    return ; 
  }

  if(ls->PRCN!=NULL)
    {
      if (SPF_PREVERSE_RC)
        {
          loctc_list *pctc;
          chain_list *cl;
          spf_grabexistingparasitics(currentnet, 0);
          nodecounter=ls->PRCN->NBNODE;

          local_resi_htable=addht(1000);
          for (lw=ls->PRCN->PWIRE; lw!=NULL; lw=lw->NEXT)
            {
              if ((pt=getptype(lw->USER, RESINAME))!=NULL)
                addhtitem(local_resi_htable,spf_spi_devect((char *)pt->DATA),(long)lw);
            }
          local_capa_htable=addht(1000);
          for (cl=ls->PRCN->PCTC; cl!=NULL; cl=cl->NEXT)
            {
              pctc=(loctc_list *)cl->DATA;
              if ((pt=getptype(pctc->USER, MSL_CAPANAME))!=NULL)
                addhtitem(local_capa_htable,spf_spi_devect((char *)pt->DATA),(long)pctc);
            }
        }
      else
        {          
          spf_grabexistingparasitics(currentnet, 1); // just to keep old the number of nodes
          if (ls->PRCN->NBNODE>1)
            {
              if (currentnet==groundlosig)
                filterparasiticsonalim(currentnet);
              else
                freelorcnet( ls );
            }
        }
    }
  else
    spf_grabexistingparasitics(currentnet, 1); // just to keep old the number of nodes

  if (ls->PRCN==NULL)
    addlorcnet(ls);
  rcn_setcapa(ls, capa);
}

// reset the htable used to collect a signal nodes
void spf_ResetLocalHTable()
{
  if (local_net_htable!=NULL) delht(local_net_htable); 
  local_net_htable=NULL;
  if (local_node_htable!=NULL) delht(local_node_htable); 
  local_node_htable=NULL;
  if (local_net_htable_connector!=NULL) delht(local_net_htable_connector); 
  local_net_htable_connector=NULL;
  if (local_resi_htable!=NULL) delht(local_resi_htable);
  local_resi_htable=NULL;
  if (local_capa_htable!=NULL) delht(local_capa_htable);
  local_capa_htable=NULL;
}

// set the name of the ground signal
void spf_SetGroundSignal(char *sig)
{
#ifdef SPF_DEBUG
  printf("ground is %s\n",sig);
#endif
  thegroundsignal=spf_spi_devect(sig);
}

void spf_finish_pending_capacitances()
{
  chain_list *entries, *cl;
  pending_capacitances_info *pci;
  pending_capacitance *pc;
  int id, notfound;
  ptype_list *pt;

  if (pending_crosstalk_capacitances!=NULL)
    {
      entries=GetAllHTKeys(pending_crosstalk_capacitances);
      while (entries!=NULL)
        {
          pci=(pending_capacitances_info *)gethtitem(pending_crosstalk_capacitances, entries->DATA);
          if (pci->realid<0)
            {
              if( !pci->signal->PRCN ) givelorcnet( pci->signal );
              pci->realid=pci->signal->PRCN->NBNODE++;
              avt_errmsg(SPF_ERRMSG, "027", AVT_WARNING, (char *)entries->DATA, pci->line, getsigname(pci->signal));
            }
          for (cl=pci->crosstalk_capacitances; cl!=NULL; cl=cl->NEXT)
            {
              pc=(pending_capacitance *)cl->DATA;
              if (pc->sig1==NULL && pc->num1==pci->id) pc->sig1=pci->signal, pc->num1=pci->realid;
              if (pc->sig2==NULL && pc->num2==pci->id) pc->sig2=pci->signal, pc->num2=pci->realid;
            }
          if (pci->signal==NULL)
              avt_errmsg(SPF_ERRMSG, "028", AVT_WARNING, (char *)entries->DATA, pci->line);
            
          freechain(pci->crosstalk_capacitances);
          entries=delchain(entries, entries);
        }
      DeleteHeap(&pending_crosstalk_capacitances_info_heap);
      delht(pending_crosstalk_capacitances);
      pending_crosstalk_capacitances=NULL;


      for (pc=all_pending_capacitance; pc!=NULL; pc=pc->next)
      {
        if (pc->sig1!=NULL && pc->sig2!=NULL)
        {
          addloctc(pc->sig1,pc->num1,pc->sig2,pc->num2,pc->val);
          if (pc->sig1!=pc->sig2)
          {
            if ((pt=getptype(pc->sig2->USER, SPF_ORIG_CAPA_SUM))==NULL)
              pt=pc->sig2->USER=addptype(pc->sig2->USER, SPF_ORIG_CAPA_SUM, (void *)0);
            *(float *)&pt->DATA += pc->val;
            if ((pt=getptype(pc->sig1->USER, SPF_ORIG_CAPA_SUM))==NULL)
              pt=pc->sig1->USER=addptype(pc->sig1->USER, SPF_ORIG_CAPA_SUM, (void *)0);
            *(float *)&pt->DATA += pc->val;
          }
        }
        else MBK_PARSE_ERROR.NB_CAPA++;
      }
      DeleteHeap(&pending_crosstalk_capacitances_heap);
    }
  if (spf_ana!=NULL)
  {
    FreeAdvancedNameAllocator (spf_ana);
    FreeAdvancedTableAllocator (spf_ata);
    spf_ana=NULL;
  }
}
