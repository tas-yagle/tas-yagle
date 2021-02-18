/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : gsp_get_patterns.c                                          */
/*                                                                          */
/*    (c) copyright 1991-2003 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/
/* Determination des patterns pour la sensibilisation d'un chemin           */
/****************************************************************************/
#include "gsp.h"


int GSP_QUIET_MODE=0;
int GSP_REAL_ORDER_SET=0, GSP_LOOP_MODE=0;
int GSP_FOUND_SOLUTION_NUMBER=0;

static ht *GSP_CORREL=NULL;
static chain_list *correlheap=NULL;

chain_list *gsp_addcorrel(chain_list *head, void *name0, void *name1, int tog)
{
  chain_list *cl;
  gsp_correl *gc;
  if (correlheap!=NULL)
  {
    cl=correlheap;
    correlheap=correlheap->NEXT;
    cl->NEXT=head;
    gc=(gsp_correl *)cl->DATA;
  }
  else
  {
    gc=(gsp_correl *)mbkalloc(sizeof(gsp_correl));
    cl=addchain(head, gc);
  }
  gc->dest=name0;
  gc->orig=name1;
  gc->tog=tog;
  return cl;
}

gsp_correl *gsp_getcorrel(chain_list *head, void *name)
{
  while (head!=NULL && ((gsp_correl *)head->DATA)->dest!=name) head=head->NEXT;
  if (head==NULL) return NULL;
  return (gsp_correl *)head->DATA;
}

void gsp_freecorrel(chain_list *head)
{
  correlheap=append(correlheap, head);
}

void gsp_AddCorrelation(char *name0, char *name1, int tog)
{
  long l;
  if (GSP_CORREL==NULL) GSP_CORREL=addht(10);
  l=gethtitem(GSP_CORREL, namealloc(name0));
  if (l!=EMPTYHT) freeptype((ptype_list *)l);
  addhtitem(GSP_CORREL, namealloc(name0), (long)addptype(NULL, tog, namealloc(name1)));
}



/*****************************************************************************\
  gsp_verif_pat ()

  Verifie que les signaux a coller sont tous externes.
  Supprime les mauvais collages.                      

\*****************************************************************************/

ptype_list *gsp_verif_pat (cnsfig_list *cf, lofig_list *fig,ptype_list *patterns)
{
  ptype_list *ptype, *assic=NULL, *pt;
  ptype_list *respat = NULL;
  char *orgname;
  losig_list *ls;
  cone_list *cn;
  int err;
  losig_list *osig;

  if (!patterns || gsp_is_pat_indpd (patterns) == 'Y' ||
      patterns->TYPE == GSP_STUCK_INDPD )
    return patterns;
  for (ptype = patterns ; ptype ; ptype = ptype->NEXT) {
    if (ptype->TYPE == GSP_STUCK_INDPD) continue;
    if ( efg_get_locon (fig,(char*)ptype->DATA) ) 
      respat = addptype (respat,ptype->TYPE,ptype->DATA);
    else 
      {
        err=1;
        if ((ls=mbk_quickly_getlosigbyname(fig, (char *)ptype->DATA))!=NULL)
          {
            osig=efg_get_org_sig(ls);
            //if ((orgname=efg_get_origsigname(ls))==NULL) orgname=getsigname(ls);
            cn=efg_getcone(osig);
//            cn=getcone(cf, 0, orgname);
            if (cn!=NULL)
              {
                assic=addptype(assic,(long)ptype->DATA,cn);
                cn->USER=addptype(cn->USER, EFG_CONE_STUCK_GSP, (ptype->TYPE==GSP_STUCK_ONE)?(void *)1:(void *)0);
//                cn->XM=(ptype->TYPE==GSP_STUCK_ONE)?1:0;
                err=0;
                avt_log(LOGGSP,2,"gsp_vefif_pat : Stuck %s to %d deleted..., initial condition used instead\n",
                        cn->NAME/*(char*)ptype->DATA*/, ptype->TYPE==GSP_STUCK_ONE?1:0);
              }
          }
        if (err)  avt_log(LOGGSP,2,"gsp_vefif_pat : Stuck %s to %d deleted...\n",
                          (char*)ptype->DATA, ptype->TYPE==GSP_STUCK_ONE?1:0);
      }
  }
  if (assic!=NULL)
    {
      if ((pt=getptype(fig->USER, EFG_ADDITIONAL_IC))==NULL)
        pt=fig->USER=addptype(fig->USER, EFG_ADDITIONAL_IC, NULL);
      pt->DATA=append((chain_list *)assic, (chain_list *)pt->DATA);
    }
  freeptype (patterns);
  return respat;
}

/*****************************************************************************\
  gsp_is_pat_indpd ()
\*****************************************************************************/
char gsp_is_pat_indpd (ptype_list *patterns)
{
  char res = 'N';

  if (patterns != NULL) {
    if (patterns == CBH_GOOD_TRANS)
      res = 'Y';
  }
  return res;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_FindSigSlope                                               */
/*                                                                           */
/* NB : le signal doit etre marque par EFG_SIG_SET                           */
/*                                                                           */
/* Description :                                                             */
/*                                                                           */
/*            Retourne 1 si le signal possede une transition up              */
/*            Retourne 0 si le signal possede une transition down            */
/*            Retourne -1 si le signal n est pas marque par EFG_SIG_SET      */
/*                                                                           */
/*****************************************************************************/
int gsp_FindSigSlope(sig)
     losig_list *sig;
{
  ptype_list *ptuser;
  int res = -1 ;

  ptuser = getptype(sig->USER,EFG_SIG_SET) ;
  if (!ptuser) { 
    /* transition non specifiee dans le .inf */
    return -1 ;
  }
  else {
    if (ptuser->DATA == EFG_SIG_SET_RISE)
      res = 1 ;
    else if (ptuser->DATA == EFG_SIG_SET_FALL)
      res = 0 ;
    else 
      res = -1 ;
  }
  return res;
}
/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_FindState()                                                */
/*                                                                           */
/* Trouve l'etat d une entree du subckt a positionner                        */
/*                                                                           */
/* NB : les noms des signaux de la ptype_list sont complets :                */
/*                                                                           */
/*      <=> nom_fig_pale.nom_instance.nom_sig                                */
/*                                                                           */
/*****************************************************************************/
char *gsp_FindState(lofig,name,headptype)
     lofig_list *lofig;
     char       *name;
     ptype_list *headptype;
{
  ptype_list *ptype;
  char        buf[2048];
  char       *value = "0";
  char       *newname; /* nom_fig.nom_sig */

  if (!strcmp(name,"vdd"))
    value = "vdd";
  else if (!strcmp(name,"vss"))
    value = "0";
  else {
    sprintf(buf,"%s%c%s",lofig->NAME,SEPAR,name);
    newname = namealloc (buf);
    for ( ptype = headptype ; ptype ; ptype = ptype->NEXT ) {
      if ( !strcmp(newname,(char*)ptype->DATA)) {
        if (ptype->TYPE == GSP_STUCK_ONE) {
          value = "vdd" ;
        }
        else {
          value = "0" ;
        }
        break;
      }
    }
  }
  return value;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: gsp_traversecones()                                             */
/*                                                                           */
/*****************************************************************************/
void gsp_traversecones(ptcone,depth)
     cone_list *ptcone;
     int depth;
{
  edge_list  *ptoutedge;
  cone_list  *ptoutcone;
  ptype_list *ptuser;
  gsp_cone_info *ci;

  depth++;
  ci=gsp_get_cone_info(ptcone);
  //    ptuser = getptype(ptcone->USER, GSP_CONE_NUMBER);
  if (ci->Date == 0)
    ci->Date = depth;
  avt_log(LOGGSP,2,"traversecone %3ld] %s\n",(long)ptuser->DATA,ptcone->NAME);

  for (ptoutedge = ptcone->OUTCONE; ptoutedge; ptoutedge = ptoutedge->NEXT) {
    if ((ptoutedge->TYPE & CNS_EXT) != 0) continue;
    if ((ptoutedge->TYPE & CNS_BLEEDER) != 0) continue;
    if ((ptoutedge->TYPE & CNS_FEEDBACK) != 0) continue;
    ptoutcone = ptoutedge->UEDGE.CONE;
    if (gsp_get_cone_info(ptoutcone) != NULL) { 
      gsp_traversecones(ptoutcone,depth);
    }
  }
  depth--;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: gsp_numbercones()                                               */
/*                                                                           */
/*****************************************************************************/
void gsp_new_numbercones (figname,ptconelist)
     char *figname;
     chain_list *ptconelist;
{
  cone_list  *ptcone;
  chain_list *chain;
  losig_list *ptlosig;
  long number;
  gsp_cone_info *ci;

  for ( chain = ptconelist ; chain ; chain=chain->NEXT ) {
    ptcone = (cone_list*)chain->DATA;
    ptlosig = efg_getlosigcone (ptcone);
    number = (long)efg_get_hier_sigptype (ptlosig,figname,EFG_SIG_SET_NUM) ;
    if ((ci=gsp_get_cone_info(ptcone))==NULL)
      {
        ci=gsp_create_cone_info(ptcone);
        ci->Date=number;
        ci->MinDate=number;
      }
    //    ptcone->USER = addptype(ptcone->USER, GSP_CONE_NUMBER, (void*)number);
    avt_log(LOGGSP,2,"New cone '%s' %s%s (number=%ld)\n",
            ptcone->NAME,!efg_test_signal_flags(ptlosig, EFG_SIG_SET_OUTPATH)?"+":"",gsp_FindSigSlope(ptlosig)?"R":"F",number) ;
  }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: gsp_numbercones()                                               */
/*                                                                           */
/*****************************************************************************/
chain_list *gsp_numbercones(ptconelist)
     chain_list *ptconelist;
{
  cone_list  *ptcone;
  edge_list  *ptinedge;
  chain_list *ptchain;
  gsp_cone_info *ci;

  if (!ptconelist) return NULL;
  if ( avt_islog(2,LOGGSP) ) {
    avt_log(LOGGSP,2,"\n-------------------------------------------\n") ;
    avt_log(LOGGSP,2," TAS : Finding spice paterns for simulation \n");
    avt_log(LOGGSP,2,"-------------------------------------------\n") ;
  }
  /* mark all the cones in the list */
  for (ptchain = ptconelist; ptchain; ptchain = ptchain->NEXT) {
    ptcone = (cone_list *)ptchain->DATA;
    if ((ci=gsp_get_cone_info(ptcone))==NULL)
      {
        ci=gsp_create_cone_info(ptcone);
        ci->Date=0;
        ci->MinDate=0;
      }
  }

  if (!ptconelist->NEXT)
    ptcone = (cone_list *)ptconelist->DATA;
  else {
    /* identify the first cone in the list */
    for (ptchain = ptconelist; ptchain; ptchain = ptchain->NEXT) {
      for (ptinedge = ((cone_list *)ptchain->DATA)->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
        if ((ptinedge->TYPE & CNS_EXT) != 0) continue;
        if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
        if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
        ptcone = ptinedge->UEDGE.CONE;
        if (gsp_create_cone_info(ptcone) != NULL)
          break;
      }
      if (ptinedge == NULL) break;
    }
    if (ptchain == NULL) return NULL;
    ptcone = (cone_list *)ptchain->DATA;
  }

  /* order the cones */
  gsp_traversecones(ptcone,0);
  return ptconelist;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: gsp_buildspicecct()                                             */
/*                                                                           */
/*****************************************************************************/
pCircuit gsp_buildspicecct(ptconelist)
     chain_list *ptconelist;
{
  chain_list *ptchain ;
  edge_list  *ptinedge;
  cone_list  *ptcone, *cone, *memsym;
  pCircuit    ptcct;
  losig_list *ptlosig;
  chain_list *l1, *l2;

  ptcct = initializeCct("spice", 50, 50);

  for (ptchain = ptconelist; ptchain; ptchain = ptchain->NEXT) {
    cone=(cone_list *)ptchain->DATA;
    l1=NULL, l2=NULL; memsym=NULL;
    for (ptinedge = cone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
      if ((ptinedge->TYPE & CNS_EXT) != 0) continue;
      if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
      if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
      if ((ptinedge->TYPE & CNS_MEMSYM) != 0) memsym=ptinedge->UEDGE.CONE;
      ptcone = ptinedge->UEDGE.CONE;
      ptlosig = efg_getlosigcone(ptcone);
      if (ptlosig && (getptype(ptlosig->USER, EFG_SIG_SET) == NULL)) {
        l1=addchain(l1, ptcone->NAME);
      }
    }
    if (memsym!=NULL)
      {
        for (ptinedge = memsym->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
          if ((ptinedge->TYPE & CNS_EXT) != 0) continue;
          if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
          if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
          if ((ptinedge->TYPE & CNS_MEMSYM) != 0) memsym=ptinedge->UEDGE.CONE;
          ptcone = ptinedge->UEDGE.CONE;
          ptlosig = efg_getlosigcone(ptcone);
          if (ptlosig && (getptype(ptlosig->USER, EFG_SIG_SET) == NULL)) {
            l2=addchain(l2, ptcone->NAME);
          }
        }
      }
    l1=reverse(l1);
    l2=reverse(l2);
    while (l1!=NULL || l2!=NULL)
      {
        if (l1!=NULL) { addInputCct_no_NA(ptcct, (char *)l1->DATA); l1=delchain(l1, l1); }
        if (l2!=NULL) { addInputCct_no_NA(ptcct, (char *)l2->DATA); l2=delchain(l2, l2); }
      }
  }
  return ptcct;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: gsp_buildconecct()                                              */
/*                                                                           */
/*****************************************************************************/
pCircuit gsp_buildconecct(ptcone)
     cone_list  *ptcone;
{
  edge_list  *ptinedge;
  pCircuit    ptcct;

  ptcct = initializeCct(ptcone->NAME, 20, 20);

  for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
    if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
    if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
    if ((ptinedge->TYPE & CNS_EXT) != 0) {
      addInputCct_no_NA(ptcct, ptinedge->UEDGE.LOCON->NAME);
    }
    else addInputCct_no_NA(ptcct, ptinedge->UEDGE.CONE->NAME);
  }
  return ptcct;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: gsp_buildexpconecct()                                           */
/*                                                                           */
/* Construit un circuit equivalent pour une liste de cone                    */
/*                                                                           */
/*****************************************************************************/
pCircuit gsp_buildexpconecct(pCircuit ptcct, chain_list *conelist)
{
  cone_list  *ptcone;
  chain_list *chain;
  edge_list  *ptinedge;

  if (!conelist) return ptcct;

  ptcone = (cone_list*)conelist->DATA;
  if (!ptcct) ptcct = initializeCct(ptcone->NAME, 20, 20);

  for (chain = conelist ; chain ; chain = chain->NEXT) {
    ptcone = (cone_list*)chain->DATA;
    for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
      if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
      if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
      if ((ptinedge->TYPE & CNS_EXT) != 0) {
        addInputCct_no_NA(ptcct, ptinedge->UEDGE.LOCON->NAME);
      }
      else addInputCct_no_NA(ptcct, ptinedge->UEDGE.CONE->NAME);
    }
  }
  return ptcct;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_SetConstraints                                             */
/*                                                                           */
/*****************************************************************************/
pNode gsp_SetConstraints (pCircuit ptconecct,chain_list *bddchainState,chain_list *bddchainEvent,pNode rup,pNode rdn,char outtransition,char hz,pNode *oic, pNode *ofc)
{
  chain_list *chain, *tmpabl;
  pNode       cond1, cond2;
  pNode       Tf, Ti, condition;
  pNode       ic, fc; 
  char *lfc, *lic;

  if (outtransition=='U') outtransition=0x1;
  else if (outtransition=='D') outtransition=0x2;

  if (hz=='Y')
    {
      avt_log(LOGGSP,2,"\n----------> HZ constraint <----------\n");
      ic = applyBinBdd (AND, notBdd(rup),notBdd(rdn)), lic="rhz"; 
      if ( avt_islog(2,LOGGSP ) ) {
        avt_log(LOGGSP,2,"rhz : ");
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, ic));
        freeExpr(tmpabl);
      }
    }
  else
    {
      if (outtransition & 0x2) ic=rup, lic="rup";
      else ic=rdn, lic="rdn";
    }

  if (outtransition & 0x1) fc=rup, lfc="rup";
  else fc=rdn, lfc="rdn";

  avt_log(LOGGSP,2,"==== Constraints application ====\n");
  for (chain = bddchainState ; chain ; chain=chain->NEXT) {
    Tf = (pNode)chain->DATA;
    Ti = Tf; // this input (on path) must be stable
      
    ic = constraintBdd (ic,Ti) ;
    fc = constraintBdd (fc,Tf) ;
    if ( avt_islog(2,LOGGSP ) ) {
      avt_log(LOGGSP,2,"IC (%s) constrainted by : ", lic);
      displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, Ti));
      freeExpr(tmpabl);
      avt_log(LOGGSP,2,"IC : ");
      displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, ic));
      freeExpr(tmpabl);
      avt_log(LOGGSP,2,"FC (%s) constrainted by : ", lfc);
      displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, Tf));
      freeExpr(tmpabl);
      avt_log(LOGGSP,2,"FC : ");
      displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, fc));
      freeExpr(tmpabl);
    }
  }
    
  for (chain = bddchainEvent ; chain ; chain=chain->NEXT) 
    {
      Tf = (pNode)chain->DATA;
      Ti = notBdd (Tf);
      ic = constraintBdd (ic,Ti) ;
      fc = constraintBdd (fc,Tf) ;
      if ( avt_islog(2,LOGGSP ) ) {
        avt_log(LOGGSP,2,"IC (%s) constrainted by : ", lic);
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, Ti));
        freeExpr(tmpabl);
        avt_log(LOGGSP,2,"IC : ");
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, ic));
        freeExpr(tmpabl);
        avt_log(LOGGSP,2,"FC (%s) constrainted by : ", lfc);
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, Tf));
        freeExpr(tmpabl);
        avt_log(LOGGSP,2,"FC : ");
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, fc));
        freeExpr(tmpabl);
      }
    }
  if (oic!=NULL) *oic=ic;
  if (ofc!=NULL) *ofc=fc;

  cond1 = ic;
  cond2 = fc;
    
  condition = applyBinBdd (AND, cond1,cond2) ;
  
  return condition;
}

int gsp_GetOutputState (pCircuit ptconecct, chain_list *allinputcone,pNode rup,pNode rdn, int avant_apres, chain_list *bef, chain_list *aft, losig_list *pathsig)
{
  chain_list *cl, *tmpabl;
  pNode       Ti;
  cone_list *cn;
  int val;
  gsp_cone_info *ci;
  losig_list *cone_ls;

  avt_log(LOGGSP,2,"==== Constraints application for %s state====\n", avant_apres==-1?"before":"after");
  for (cl=allinputcone; cl!=NULL; cl=cl->NEXT)
    {
      cn=(cone_list *)cl->DATA;
      if ((ci=gsp_get_cone_info(cn))!=NULL)
        {
          cone_ls=efg_getlosigcone(cn);
          if (avant_apres==-1)
            {
              if (bef!=NULL && cone_ls!=pathsig)
                {
                  if (getchain(bef, cone_ls)!=NULL) val=ci->AfterTransitionState;
                  else val=ci->BeforeTransitionState;
                }
              else
                val=ci->BeforeTransitionState;
            }
          else
            {
              if (bef!=NULL && cone_ls!=pathsig)
                {
                  if (getchain(bef, cone_ls)!=NULL) val=ci->AfterTransitionState;
                  else val=ci->BeforeTransitionState;
                }
              else
                val=ci->AfterTransitionState;
            }
          if (val!=-1)
            {
              Ti=ablToBddCct(ptconecct, tmpabl=createAtom_no_NA(cn->NAME));
              if (val==0) Ti=notBdd(Ti);
              freeExpr(tmpabl);
              rdn = constraintBdd (rdn,Ti) ;
              rup = constraintBdd (rup,Ti) ;
              if ( avt_islog(2,LOGGSP ) ) {
                avt_log(LOGGSP,2,"sup/sdn constrainted by : ");
                displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, Ti)); freeExpr(tmpabl);
                avt_log(LOGGSP,2,"sup : ");
                displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, rup)); freeExpr(tmpabl);
                avt_log(LOGGSP,2,"sdn : ");
                displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, rdn)); freeExpr(tmpabl);
              }
            }
        }
    }

  if (rup==BDD_one && rdn==BDD_zero) return 1;
  if (rdn==BDD_one && rup==BDD_zero) return 0;
  
  return -1;
}

void gsp_compute_avant_apres(pCircuit ptconecct, gsp_cone_info *cone_ci, chain_list *allinputcone, pNode rup, pNode rdn, int initialstate, chain_list *bef, chain_list *aft, losig_list *pathsig)
{
  if (initialstate)
    {
      cone_ci->BeforeTransitionState=gsp_GetOutputState (ptconecct, allinputcone, rup, rdn, -1,bef,aft,pathsig);
    }
  else
    {
      cone_ci->AfterTransitionState=gsp_GetOutputState (ptconecct, allinputcone, rup, rdn, 1,bef,aft,pathsig);
    }
  avt_log(LOGGSP,2,"Initial State : %s",cone_ci->BeforeTransitionState==-1?"unknown":cone_ci->BeforeTransitionState==1?"1":"0");
  avt_log(LOGGSP,2,", Final State : %s\n",cone_ci->AfterTransitionState==-1?"unknown":cone_ci->AfterTransitionState==1?"1":"0");
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_treat_correlation                                          */
/*                                                                           */
/*****************************************************************************/
void gsp_treat_correlation ( chain_list *cone_outpath, 
                             pCircuit ptconecct,
                             ptype_list *var2exp,
                             pNode *rup, pNode *rdn )
{
  chain_list *sup_abl,*sdn_abl, *tmpabl;
  ptype_list *ptuser;
  chain_list *abl_tmp ;
  char *name;


  if (cone_outpath != NULL) {
    sup_abl = bddToAblCct(ptconecct, *rup);
    sdn_abl = bddToAblCct(ptconecct, *rdn);

    for(ptuser = var2exp ; ptuser ; ptuser = ptuser->NEXT) {
      avt_log(LOGGSP,2,"-- Correlated signal found, new expression :\n");
      name = (char*)ptuser->TYPE;
      abl_tmp = (chain_list*)ptuser->DATA;
      if ( avt_islog(2,LOGGSP) ) {
        avt_log(LOGGSP,2,"-- substitute '%s' by expr :",name);
        displayExprLog(LOGGSP,2,abl_tmp);
      }
      sup_abl = substExpr (tmpabl=sup_abl,name,abl_tmp); freeExpr(tmpabl);
      sdn_abl = substExpr (tmpabl=sdn_abl,name,abl_tmp); freeExpr(tmpabl);
    }

    *rup = ablToBddCct(ptconecct,sup_abl);
    *rdn = ablToBddCct(ptconecct,sdn_abl);
    if ( avt_islog(2,LOGGSP) ) {
      avt_log(LOGGSP,2,"<-> rup : ") ;
      displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, *rup));
      freeExpr(tmpabl);
      avt_log(LOGGSP,2,"<-> rdn : ") ;
      displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, *rdn));
      freeExpr(tmpabl);
    }
    if ( equalBdd( *rup ,BDD_zero) != 1 ) {
      if ( avt_islog(2,LOGGSP) ) {
        avt_log(LOGGSP,2,"<-> rup has no bad correlation due to subsitution(s)\n") ;
        avt_log(LOGGSP,2,"<-> orig rup : ") ;
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, *rup));
        freeExpr(tmpabl);
      }
    }
    if ( equalBdd( *rdn ,BDD_zero) != 1 ) {
      if ( avt_islog(2,LOGGSP) ) {
        avt_log(LOGGSP,2,"<-> rdn has no bad correlation due to subsitution(s)\n") ;
        avt_log(LOGGSP,2,"<-> orig rdn : ") ;
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, *rdn));
        freeExpr(tmpabl);
      }
    }
    freeExpr ( sup_abl );
    freeExpr ( sdn_abl );
  }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_treat_switch                                               */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_treat_switch ( chain_list *detect_switch,
                               chain_list **sup_abl, chain_list **sdn_abl)
{
  ptype_list *switchcmd = NULL;
  branch_list *branch[2];
  lotrs_list  *lotrs;
  int i;
  link_list *link ;
  chain_list *chain, *tmpabl;
  losig_list *sig,*oppsig;
  ptype_list *ptype;
  chain_list *cst_abl;
  char *cstname;
  cone_list *cone,*opposite_switch_cmd;
  char *cmd,*oppcmd;
  lotrs_list *newtrs;
  locon_list *locon;
 
  for ( chain = detect_switch ; chain ; chain=chain->NEXT ) {
    ptype = NULL;
    
    cone = ((edge_list*)chain->DATA)->UEDGE.CONE;
    sig = efg_getlosigcone (cone);
    if ( getptype (sig->USER,EFG_SIG_SET) ) {
      ptype = getptype (((edge_list*)chain->DATA)->USER,CNS_SWITCHPAIR);
      opposite_switch_cmd = (cone_list*)ptype->DATA;
      oppsig = efg_getlosigcone (opposite_switch_cmd);
      if  ( oppsig && getptype (oppsig->USER,EFG_SIG_SET) ) {
        oppsig->USER = addptype (oppsig->USER,GSP_SWITCHPAIR,sig);
        avt_log(LOGGSP,2,"Switches: opposite cmd on path %s and %s found!\n",
                getsigname(oppsig),getsigname(sig)) ;
      }
      continue;
    }
 
    if ( !ptype ) {
      ptype = getptype (((edge_list*)chain->DATA)->USER,CNS_SWITCHPAIR);
      opposite_switch_cmd = (cone_list*)ptype->DATA;
      oppsig = efg_getlosigcone (opposite_switch_cmd);
    }
    
    // si la commande du switch n est pas un signal du chemin alors on continue...
    if  ( oppsig && !getptype (oppsig->USER,EFG_SIG_SET) ) {
      cmd = getsigname(sig);
      oppcmd = getsigname(oppsig);
      
      for ( ptype = switchcmd ; ptype ; ptype=ptype->NEXT ) {
        if ( ((char*)ptype->TYPE == cmd) || ((char*)ptype->DATA == oppcmd) ||
             ((char*)ptype->TYPE == oppcmd) || ((char*)ptype->DATA == cmd) )
          break;
      }
      if ( !ptype )
        switchcmd = addptype (switchcmd,(long)cmd,(void*)oppcmd);
    }
    else {
      if (EFGCONTEXT->DESTFIG!=NULL)
        {
          int tog=0;
          chain_list *conelist=NULL;
          cone_list *tcone;
          gsp_FindSimpleCorrelationForToBeStuckConnectors(cone, 0, 100, &tog,1,&conelist);
          conelist=addchain(conelist, cone);

          while (conelist!=NULL)
            {
              tcone=(cone_list *)conelist->DATA;
              EFGCONTEXT->ALLADDEDCONES=addchain(EFGCONTEXT->ALLADDEDCONES, tcone);
              
              branch[0] = tcone->BRVDD ;
              branch[1] = tcone->BRVSS ;
            
              for(i = 0 ; i < 2 ; i++) {
                for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT) {
                  for(link = branch[i]->LINK ; link != NULL ; link = link->NEXT) {
                    if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0) {
                      lotrs = link->ULINK.LOTRS ;
                      if ( lotrs->TRNAME )
                        avt_log(LOGGSP,2,"Adding cone '%s' extra transistor %s for switches\n",tcone->NAME,lotrs->TRNAME) ;
                      newtrs = efg_addlotrs (EFGCONTEXT->DESTFIG,lotrs,1);
                      if (getptype(newtrs->GRID->SIG->USER, EFG_SIG_SET)==NULL)
                        newtrs->GRID->SIG->USER = addptype (newtrs->GRID->SIG->USER, EFG_SIG_SET, NULL);
                      if (getptype(newtrs->DRAIN->SIG->USER, EFG_SIG_SET)==NULL)
                        newtrs->DRAIN->SIG->USER = addptype (newtrs->DRAIN->SIG->USER, EFG_SIG_SET, NULL);
                      if (getptype(newtrs->SOURCE->SIG->USER, EFG_SIG_SET)==NULL)
                        newtrs->SOURCE->SIG->USER = addptype (newtrs->SOURCE->SIG->USER, EFG_SIG_SET, NULL);
                    }
                  }
                }
              }
              conelist=delchain(conelist, conelist);
            }
   
          if (( locon = efg_get_locon (EFGCONTEXT->DESTFIG,cone->NAME) ))
            locon->SIG->USER = addptype (locon->SIG->USER,EFG_SIG_SET,NULL);
        }
      cstname = opposite_switch_cmd->NAME;
      
      cst_abl =  notExpr (createAtom_no_NA ( cstname ));
      
      if ( avt_islog(2,LOGGSP) ) {
        avt_log(LOGGSP,2,"Switches detected : \n") ;
        avt_log(LOGGSP,2,">>> old sup_abl : ") ;
        displayExprLog(LOGGSP,2, *sup_abl );
        avt_log(LOGGSP,2,">>> old sdn_abl : ") ;
        displayExprLog(LOGGSP,2, *sdn_abl );
      }
      *sup_abl = substExpr (tmpabl=*sup_abl,cone->NAME,cst_abl); freeExpr(tmpabl);
      *sdn_abl = substExpr (tmpabl=*sdn_abl,cone->NAME,cst_abl); freeExpr(tmpabl);
      freeExpr(cst_abl);
      if ( avt_islog(2,LOGGSP) ) {
        avt_log(LOGGSP,2,"Substitutions for switches (%s): \n",cone->NAME) ;
        avt_log(LOGGSP,2,">>> new sup_abl : ") ;
        displayExprLog(LOGGSP,2, *sup_abl );
        avt_log(LOGGSP,2,">>> new sdn_abl : ") ;
        displayExprLog(LOGGSP,2, *sdn_abl );
      }
    }
  }
  return switchcmd;
}

/*****************************************************************************\
 FUNCTION : gsp_add_switch_stuck
\*****************************************************************************/
void gsp_add_switch_stuck ( pNode *condition, pCircuit Cct,ptype_list *switchcmd) 
{
  ptype_list *ptype;
  chain_list *switch_cst=NULL;
  char *cmd,*oppcmd;
  int e0, e1;
  chain_list *res_abl, *tmpabl;

  res_abl = bddToAblCct (Cct,*condition) ;
  
  for ( ptype = switchcmd ; ptype ; ptype=ptype->NEXT ) {
    cmd = (char*)ptype->TYPE;
    oppcmd = (char*)ptype->DATA;
    e0=searchExpr(res_abl, cmd);
    e1=searchExpr(res_abl, oppcmd);

    if ((e0 && !e1) || (e1 && !e0)) {
      tmpabl = createExpr (XOR);
      addQExpr (tmpabl, createAtom_no_NA(oppcmd));
      addQExpr (tmpabl, createAtom_no_NA(cmd));
      if ( avt_islog(2,LOGGSP) ) {
        avt_log(LOGGSP,2,"**> Switch: adding condition => ");
        displayExprLog(LOGGSP,2, tmpabl/*switch_cst */);
      }
      switch_cst = createExpr (AND);
      addQExpr (switch_cst, res_abl);
      addQExpr (switch_cst, tmpabl);
      res_abl=switch_cst;
    }
  }
  *condition=ablToBddCct ( Cct, res_abl);
  freeExpr(res_abl);
}
/*****************************************************************************\
 FUNCTION : gsp_replace_cmd_by_alim
\*****************************************************************************/
void gsp_replace_cmd_by_alim (cone_list *cone, losig_list *command, long stuck)
{
  losig_list *alim,*destcmd;
  lotrs_list *destlotrs,*lotrs;
  branch_list *branch[3] ;
  int i;
  link_list *link ;

  if (!EFGCONTEXT) return;
  alim = (stuck  == GSP_STUCK_ONE) ? EFGCONTEXT->VDDDESTFIG:EFGCONTEXT->VSSDESTFIG;

  branch[0] = cone->BRVDD ;
  branch[1] = cone->BRVSS ;
  branch[2] = cone->BREXT ;

  if ((destcmd = efg_get_ext_sig (command)) == NULL) return;

  for(i = 0 ; i < 3 ; i++)
    {
      for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
        {
          for(link = branch[i]->LINK ; link != NULL ; link = link->NEXT)
            {
              if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
                {
                  lotrs = link->ULINK.LOTRS ;
                  destlotrs = efg_get_ext_trs (lotrs);

                  if (!destlotrs) continue;

                  if ( destlotrs->DRAIN->SIG == destcmd ) {
                    if( !getptype( lotrs->DRAIN->SIG->USER, EFG_SIG_CLK ) ) {
                      efg_add_lotrs_capa_dup (EFGCONTEXT->DESTFIG,destlotrs->DRAIN);
                      destlotrs->DRAIN->SIG = alim;
                      avt_log(LOGGSP,2,"Trans %s : drain replaced by %s\n",
                              destlotrs->TRNAME,(stuck==GSP_STUCK_ONE)?"vdd":"vss");
                    }
                    else
                      avt_log(LOGGSP,2,"Trans %s : drain is a command on an another path (setup or hold mode). not stucked\n",
                              destlotrs->TRNAME );
                  }
                  if ( destlotrs->SOURCE->SIG == destcmd ) {
                    if( !getptype( lotrs->SOURCE->SIG->USER, EFG_SIG_CLK ) ) {
                      efg_add_lotrs_capa_dup (EFGCONTEXT->DESTFIG,destlotrs->SOURCE);
                      destlotrs->SOURCE->SIG = alim;
                      avt_log(LOGGSP,2,"Trans %s : source replaced by %s\n",
                              destlotrs->TRNAME,(stuck==GSP_STUCK_ONE)?"vdd":"vss");
                    }
                    else
                      avt_log(LOGGSP,2,"Trans %s : source is a command on an another path (setup or hold mode). not stucked\n",
                              destlotrs->TRNAME );
                  }
                  if ( destlotrs->GRID->SIG == destcmd ) {
                    if( !getptype( lotrs->GRID->SIG->USER, EFG_SIG_CLK ) ) {
                      efg_add_lotrs_capa_dup (EFGCONTEXT->DESTFIG,destlotrs->GRID);
                      destlotrs->GRID->SIG = alim;
                      avt_log(LOGGSP,2,"Trans %s : Gate replaced by %s\n",
                              destlotrs->TRNAME,(stuck==GSP_STUCK_ONE)?"vdd":"vss");
                    }
                    else
                      avt_log(LOGGSP,2,"Trans %s : gate is a command on an another path (setup or hold mode). not stucked\n",
                              destlotrs->TRNAME );
                  }
                }
            }
        }
    }
}


losig_list *gsp_GetOppositeSwitchCommandSignal(cone_list *ptcone, char *name)
{
  edge_list *ed;
  ptype_list *pt;

  for (ed=ptcone->INCONE; ed!=NULL; ed=ed->NEXT)
    if ((ed->TYPE & CNS_EXT)==0 && ed->UEDGE.CONE->NAME==name)
      {
        if ((pt=getptype(ed->USER,CNS_SWITCHPAIR))!=NULL)
          return efg_getlosigcone((cone_list*)pt->DATA);
      }
  return NULL;
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_calcconetransfer                                           */
/*                                                                           */
/*****************************************************************************/

static int checkoutofpathsig_input(losig_list *input, gsp_cone_info *ci, int min, int max)
{
  if (ci==NULL) return 0;
  if (!efg_test_signal_flags(input, EFG_SIG_SET_OUTPATH)) return 0; // do nothing
  if (min==-1 || max==-1) return 0;
  if (ci->Date<min || max<ci->MinDate)
    {
      if (ci->Date<min) return 1; // entree plus rapide, utiliser l'etat final
      else return -1; // entree plus lente, utiliser l'etat initial
    }
  else
    return 0;
}

static int gsp_also_exist_cone(edge_list *ptinedge, locon_list *lc)
{
  while (ptinedge!=NULL)
    {
      if ((ptinedge->TYPE & (CNS_VDD|CNS_VSS|CNS_BLEEDER|CNS_FEEDBACK|CNS_EXT))==0)
        {
          if (ptinedge->UEDGE.CONE->NAME==lc->NAME) return 1;
        }
      ptinedge = ptinedge->NEXT;
    }
  return 0;
}

void gsp_constraint_abl_with_stuck( cone_list *cone, chain_list **sup_abl, chain_list **sdn_abl )
{
  edge_list  *inedge ;
  cone_list  *incone ;
  chain_list *atom ;
  chain_list *tmpexpr;

  for( inedge = cone->INCONE ; inedge ; inedge = inedge->NEXT ) {
  
    if ((inedge->TYPE & CNS_VDD)      != 0 ) continue;
    if ((inedge->TYPE & CNS_VSS)      != 0 ) continue;
    if ((inedge->TYPE & CNS_BLEEDER)  != 0 ) continue;
    if ((inedge->TYPE & CNS_FEEDBACK) != 0 ) continue;
    if ((inedge->TYPE & CNS_EXT)      != 0 ) continue ;
    incone = inedge->UEDGE.CONE;
    
    atom = NULL ;
    if( ( incone->TECTYPE & CNS_ZERO ) == CNS_ZERO ) atom = createAtom("'0'");
    if( ( incone->TECTYPE & CNS_ONE  ) == CNS_ONE  ) atom = createAtom("'1'");

    if( atom ) {
      substPhyExpr( *sup_abl, incone->NAME, atom );
      substPhyExpr( *sdn_abl, incone->NAME, atom );
      freeExpr(atom);
    }
  }
  *sup_abl=simplif10Expr(tmpexpr=*sup_abl); freeExpr(tmpexpr);
  *sdn_abl=simplif10Expr(tmpexpr=*sdn_abl); freeExpr(tmpexpr);
}

#define GSP_TMP_MARK_PTYPE 0xfab70615

void gsp_mark_cone_inputs(cone_list *cone, int mark)
{
  edge_list  *ptinedge;
  cone_list *ptcone;
  losig_list *sig;
  for (ptinedge = cone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) 
    {
      if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
      if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
      if ((ptinedge->TYPE & CNS_EXT) == 0) 
        {
          ptcone = ptinedge->UEDGE.CONE;
          /* do nothing if cone HZ or CONFLIT */
          sig = efg_getlosigcone(ptcone) ;
          if (mark)
            sig->USER=addptype(sig->USER,GSP_TMP_MARK_PTYPE,NULL);
          else
            sig->USER=testanddelptype(sig->USER,GSP_TMP_MARK_PTYPE);
        }
    }
}
typedef struct
{
  char *sigd, *sigs, inv;
  losig_list *lsigd, *lsigs;
} gsp_simple_correlation;

losig_list *gsp_FindSimpleCorrelationForToBeStuckConnectors(cone_list *cone,int depth,int maxdepth, int *tog, int setonly, chain_list **conelist)
{
  cone_list  *ptcone;
  edge_list  *ptinedge;
  losig_list *sig;
  ptype_list *ptype;
  
  if ( depth == maxdepth ) return NULL;
  if ((cone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS && cone->INCONE!=NULL && cone->INCONE->NEXT==NULL)
    {
      // inverseur
      *tog=((*tog)+1) & 1;
      ptinedge=cone->INCONE;
      if ((ptinedge->TYPE & CNS_EXT) == 0)
        {
          ptcone = ptinedge->UEDGE.CONE;
          sig = efg_getlosigcone(ptcone) ;
          if (setonly==-1)
          {
            if (getptype(sig->USER,GSP_TMP_MARK_PTYPE)!=NULL)
              return sig;
          }
          else if (setonly)
            {
              if (getptype(sig->USER,EFG_SIG_SET)!=NULL)
                {
                  return sig;
                }
            }
          else
            {
              if (getptype(sig->USER,GSP_TMP_MARK_PTYPE)!=NULL && getptype(sig->USER,EFG_SIG_SET)==NULL)
                {
                  return sig;
                }
            }
          sig=gsp_FindSimpleCorrelationForToBeStuckConnectors(ptcone, depth+1, maxdepth, tog, setonly, conelist);
          if (sig!=NULL)
            {
              if (conelist!=NULL) *conelist=addchain(*conelist, ptcone);
              return sig;
            }
        }
    }
  return NULL;
}

losig_list *gsp_GetUserCorrelation(char *dst, char *figname, int *tog, int notset)
{
  long l;
  if (GSP_CORREL!=NULL && (l=gethtitem(GSP_CORREL, dst))!=EMPTYHT)
  {
    lofig_list *lf;
    ptype_list *pt;
    losig_list *ls;
    lf=getloadedlofig(figname);
    pt=(ptype_list *)l;
    *tog=pt->TYPE;
    ls=mbk_quickly_getlosigbyname(lf, (char *)pt->DATA);
    if (ls!=NULL && getptype(ls->USER,GSP_TMP_MARK_PTYPE)!=NULL)
    {
      if (notset==-1 || (notset && getptype(ls->USER,EFG_SIG_SET)==NULL)
          || (!notset && getptype(ls->USER,EFG_SIG_SET)!=NULL))
       return ls;
    }
  }
  return NULL;
}

gsp_simple_correlation *gsp_find_top_correlated_signal(chain_list *corel, gsp_simple_correlation *gsc, int *inv)
{
  int changed;
  gsp_simple_correlation *ngsc;
  chain_list *cl;
  

  *inv=gsc->inv;
  do
    {
      changed=0;
      for( cl=corel; cl!=NULL; cl=cl->NEXT ) 
        {
          ngsc=(gsp_simple_correlation *)cl->DATA;
          if (gsc->sigs==ngsc->sigd)
            {
              *inv=((*inv)+ngsc->inv) & 1;
              gsc=ngsc;
              changed=1;
            }
        }
    } while (changed);
  return gsc;
}
void gsp_constraint_abl_with_Correlations( chain_list *corel, chain_list *sup_abl, chain_list *sdn_abl )
{
  chain_list *expr ;
  chain_list *cl ;
  gsp_simple_correlation *gsc, *ngsc;
  int inv;
  
  for( cl=corel; cl!=NULL; cl=cl->NEXT ) 
    {
      gsc=(gsp_simple_correlation *)cl->DATA;
      ngsc=gsp_find_top_correlated_signal(corel, gsc, &inv);

      expr = createAtom_no_NA( ngsc->sigs );
      if (inv) expr = notExpr( expr ) ;

      avt_log(LOGGSP,2,"external correlation: %s = %s%s%s\n", gsc->sigd, inv?"not(":"", ngsc->sigs, inv?")":"");

      substPhyExpr( sup_abl, gsc->sigd, expr );
      substPhyExpr( sdn_abl, gsc->sigd, expr );

      freeExpr( expr );
    }
}

static losig_list *gsp_GetDirectCorrel(chain_list *corel, losig_list *me, int *inv)
{
  chain_list *cl;
  gsp_simple_correlation *gsc;
  for( cl=corel; cl!=NULL; cl=cl->NEXT ) 
    {
      gsc=(gsp_simple_correlation *)cl->DATA;
      *inv=gsc->inv;
      if (gsc->lsigd==me) return gsc->lsigs;
    }
  return NULL;
}

static void gsp_add_Correlated_stuck ( pNode *condition, pCircuit Cct,chain_list *corel) 
{
  chain_list *switch_cst=NULL, *cl;
  int inv;
  gsp_simple_correlation *gsc, *ngsc;
  int e0, e1;
  chain_list *res_abl, *tmpabl;

  res_abl = bddToAblCct (Cct,*condition) ;
  
  for( cl=corel; cl!=NULL; cl=cl->NEXT ) 
    {
      gsc=(gsp_simple_correlation *)cl->DATA;
      ngsc=gsp_find_top_correlated_signal(corel, gsc, &inv);
      e0=searchExpr(res_abl, ngsc->sigs);
      e1=searchExpr(res_abl, gsc->sigd);

      if ((e0 && !e1) || (e1 && !e0)) {        
        tmpabl = createExpr (XOR);
        addQExpr (tmpabl, createAtom_no_NA(ngsc->sigs));
        addQExpr (tmpabl, createAtom_no_NA(gsc->sigd));
        if (!inv)
          {
            tmpabl = notExpr(tmpabl);
          }
        if ( avt_islog(2,LOGGSP) ) {
          avt_log(LOGGSP,2,"-- correlation: adding condition => ");
          displayExprLog(LOGGSP,2, tmpabl);
        }
        switch_cst = createExpr (AND);
        addQExpr (switch_cst, res_abl);
        addQExpr (switch_cst, tmpabl);
        res_abl=switch_cst;
                        
      }
    }
  *condition=ablToBddCct ( Cct, res_abl);
  freeExpr(res_abl);
}

static void freechaindata(chain_list *cl)
{
  while (cl!=NULL)
    {
      mbkfree(cl->DATA);
      cl=delchain(cl, cl);
    }
}
static int gsp_guessedmemsymrise(gsp_cone_info *cone_ci, losig_list *memsymsig)
{
  ptype_list *ptuser;
  if (cone_ci->precharged)
    {
      ptuser = getptype(memsymsig->USER,EFG_SIG_SET) ;
      if (ptuser->DATA == EFG_SIG_SET_RISE && cone_ci->Date<0) return 1;
    }
  return 0;
}

void printsol(ptype_list *pt)
{
  while (pt!=NULL)
    {
      printf("%d    %s\n",(int)pt->TYPE,(char *)pt->DATA);
      pt=pt->NEXT;
    }
  printf("------\n");
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : Gsp_AddConeOutPath                                             */
/*                                                                           */
/* Rajoute les cones hors chemin pour resoudre le pb de correlation des      */
/* entrees d'un cone                                                         */
/*                                                                           */
/*****************************************************************************/


static chain_list *gsp_AddConeOutPath(chain_list *headchain,cone_list  *cone,chain_list **losigchain,int depth,int maxdepth, cone_list *orig)
{
  cone_list  *ptcone;
  edge_list  *ptinedge;
  losig_list *sig;
  chain_list *chain;

  if ( depth == maxdepth
       || (cone->TYPE & (CNS_TRI|CNS_CONFLICT|CNS_EXT)) != 0) return headchain;
  depth++;
  for (ptinedge = cone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
    if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
    if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
    else if ((ptinedge->TYPE & CNS_EXT) == 0)
    {
      ptcone = ptinedge->UEDGE.CONE;
      if (ptcone!=orig)
      {
         /* teste si le sig est marque ou est un connecteur */
         /* si ce n'est pas le cas : recursivite            */
         sig = efg_getlosigcone(ptcone) ;
         if (getptype(sig->USER,EFG_SIG_SET) || getptype(sig->USER,GSP_TMP_MARK_PTYPE)) {
           if ( !getchain(*losigchain, sig) && getptype(sig->USER,EFG_SIG_SET))
             *losigchain = addchain (*losigchain,sig);
         }
         else if  ( gsp_SigInInterface(sig) != 'Y')
           headchain=gsp_AddConeOutPath(headchain,ptcone, losigchain,depth,maxdepth, orig); 
       }
     }
  }
  if (cone!=orig)
    headchain = addchain (headchain,(void*)cone);
  return headchain;
}


static int gsp_checkfulldatawrite(cone_list  *ptcone)
{
  ptype_list *pt;
  chain_list *upsup, *downsup;
  losig_list *sig;
  cone_list *ptinputcone;
  int tog=0, ret=0;
  edge_list *ptinedge;
  
  cnsConeFunction_once(ptcone,1);

  pt = getptype(ptcone->USER, CNS_UPEXPR);
  upsup=supportChain_listExpr((chain_list *)pt->DATA);
  pt = getptype(ptcone->USER, CNS_DNEXPR);
  downsup=supportChain_listExpr((chain_list *)pt->DATA);
  for (ptinedge = ptcone->INCONE; ptinedge && ret==0; ptinedge = ptinedge->NEXT) 
    {
      if ((ptinedge->TYPE & CNS_VDD) != 0) continue;
      if ((ptinedge->TYPE & CNS_VSS) != 0) continue;
      if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
      if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
      if ((ptinedge->TYPE & CNS_EXT) != 0) continue;
      if ((ptinedge->TYPE & CNS_COMMAND) != 0) continue;
      if ((ptinedge->TYPE & CNS_MEMSYM) != 0) continue;
      // une data
      ptinputcone = ptinedge->UEDGE.CONE;
      sig = efg_getlosigcone(ptinputcone) ;
      if ((getptype(sig->USER,EFG_SIG_SET)!=NULL || 
           gsp_FindSimpleCorrelationForToBeStuckConnectors(ptinputcone, 0, 4, &tog,1,NULL)!=NULL) &&
          getchain(upsup, ptinputcone->NAME)!=NULL &&
          getchain(downsup, ptinputcone->NAME)!=NULL
          ) ret=1;
    }
  freechain(upsup);
  freechain(downsup);
  return ret;
}

static int gsp_coneatonestep(cone_list *ptcone, cone_list *input)
{
  cone_list *ptinputcone;
  edge_list *ptinedge;
  
  if (ptcone!=NULL && input!=NULL)
  {
    for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) 
      {
        if ((ptinedge->TYPE & CNS_VDD) != 0) continue;
        if ((ptinedge->TYPE & CNS_VSS) != 0) continue;
        if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
        if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
        if ((ptinedge->TYPE & CNS_EXT) != 0) continue;
        if ((ptinedge->TYPE & CNS_MEMSYM) != 0) continue;
        // une data
        ptinputcone = ptinedge->UEDGE.CONE;
        if (ptinputcone==input) return 1;
      }
  }
  return 0;
}

void gsp_applystucks(pNode *condition, ptype_list *switchcmd, pCircuit ptconecct, chain_list *gsp_simple_correlation_list, pNode *rup, pNode *rdn, int maskcmd, ptype_list *cmdchain, int loglvl1, int disacmd)
{
  pNode cst ;
  chain_list *tmpabl;
  if ( equalBdd( *condition ,BDD_zero) != 1 ) {
    if ( switchcmd ) {
      gsp_add_switch_stuck ( condition, ptconecct, switchcmd);
    }
    gsp_add_Correlated_stuck(condition, ptconecct, gsp_simple_correlation_list );

    if (gsp_simple_correlation_list!=NULL && loglvl1)
      {
        avt_log(LOGGSP,2,">>> after correlation : ") ;
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, *condition));
        freeExpr(tmpabl);
      }
    if (maskcmd) {
      ptype_list *ptype,*local_cst = NULL;
      chain_list *ablcst;
      pNode cst;
      char *cmdname;
      losig_list *oppositesig;
      int inv;

      while (cmdchain!=NULL)
      {
        // filter with command state          
        cmdname=getsigname((losig_list *)cmdchain->DATA);
        if ( cmdchain->TYPE!=0)
          ablcst = createAtom_no_NA( cmdname ) ;
        else
          ablcst = notExpr (createAtom_no_NA ( cmdname));
        if (disacmd) ablcst = notExpr (ablcst);
        cst = ablToBddCct(ptconecct, ablcst);
        *condition = applyBinBdd (AND, *condition, cst) ;
        if (loglvl1)
        {
          avt_log(LOGGSP,2,">>> final cond after command (%s=%ld) mask :",cmdname,(cmdchain->TYPE+disacmd)&1) ;
          displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, *condition));
          freeExpr(tmpabl);
        }
        freeExpr(ablcst);
        *rup = applyBinBdd (AND, *rup, cst) ;
        *rdn = applyBinBdd (AND, *rdn, cst) ;
        cmdchain=cmdchain->NEXT;
      }
    }
 }
}

chain_list * gsp_calcconetransfer (char *figname,cone_list  *ptcone,char corner, int initialstatemode, int fixed, chain_list **icabl, chain_list **fcabl, chain_list **allcorrel)
{
  ptype_list *ptype,*switchcmd = NULL;
  cone_list  *ptinputcone;
  pCircuit    ptconecct;
  ptype_list *ptuser1, *ptuser2 ;
  losig_list *oppsig,*command=NULL,*ptpathsig; /* latest sig for input cone */ 
  losig_list *ptpathsig2, *outlosig ;
  losig_list *losig, *memsym ;
  locon_list *locon ;
  chain_list *sup_abl, *sdn_abl ;
  pNode       sup, sdn, ic, fc ;
  pNode       rup, rdn, mybdd ;
  int        number, maxnumber,res, current_cone_number,  current_cone_number_min;
  edge_list  *ptinedge ;
  ptype_list *ptuser;
  pNode       condition ; 
  char        outtransition = 'X'; /* U for UP , D for Down */
  char        intransition = 'X';  /* U for UP , D for Down */
  char        HZ;            /* flag on outlosig      */
  char       *conename;  
  ptype_list *var2exp = NULL;
  chain_list *chain;
  ptype_list *cmdchain=NULL ;
  chain_list *losigchain = NULL, *after_current_cone_losigchain=NULL ;
  ptype_list *out_of_path_stuck=NULL, *pt;
  chain_list *bddchainState = NULL ; // contraint a positionner par etat
  chain_list *bddchainEvent = NULL ; // contraint a positionner par event
  chain_list *res_abl = NULL ;
  chain_list *cone_outpath = NULL ;
  chain_list *exp_bdd = NULL ;     /* abl expansees des sup des cone_outpath */
  chain_list *detect_switch = NULL, *tmpatom, *tmpabl;
  int latch=0,sigendflag;
  int flag,InputIsCmd,cmdflag;
  int done, avant_apres;
  int loglvl1=0, cone_is_outpath=0, outcone_pathnum, disacmd=0;
  long val;
  gsp_cone_info *cone_ci, *ci, *memsym_ci;
  chain_list *allinputcone;
  int last_cone_number_min, last_cone_number_max, remove;
  gsp_simple_correlation *gsc;
  chain_list *gsp_simple_correlation_list=NULL;
  spisig_list *spisig;
  int guessed, state, fulldatawrite, indep_var, checkmode=0, nbpathsigbefore, exitnow=0, nbatonestep=0;
  char *alternate_ci_cmd;
  int tog, this_fulldatawrite=0;
  losig_list *lsigd;

  if (icabl!=NULL) *icabl=NULL;
  if (fcabl!=NULL) *fcabl=NULL;

  losig = efg_getlosigcone(ptcone) ;

  if (!efg_test_signal_flags(losig, EFG_SIG_SET_OUTPATH) && (initialstatemode & GSP_COMPUTE_INITIAL_STATE)) return NULL;

  if ((cone_ci=gsp_get_cone_info(ptcone))!=NULL)
    {
      current_cone_number = cone_ci->Date;
      current_cone_number_min = cone_ci->MinDate;
    }
  else
    current_cone_number = current_cone_number_min = -1;

  if ( avt_islog(1,LOGGSP) ) {
        
    avt_log(LOGGSP,2,"\n") ;
    avt_log(LOGGSP,2,"-------------------------------------------\n") ;
    avt_log(LOGGSP,2,"    CONE [%d|%d] : %s\n",current_cone_number_min,current_cone_number,ptcone->NAME) ;
    avt_log(LOGGSP,2,"-------------------------------------------\n") ;
    if (cone_ci->cmd_state & EFG_SPISIG_HZ_MASK)
      {
        avt_log(LOGGSP,2,"Cone has HZ transition, no analsis done\n") ;
      }
    else
      {
        if (cone_ci->precharged) avt_log(LOGGSP,2,"Cone is considered precharged\n") ;
        if (fixed!=0)
          {
            if (fixed & 0x2)
              avt_log(LOGGSP,2,"Cone IC is considered to be %d\n", fixed & 0x1) ;
            if (fixed & 0x20)
              avt_log(LOGGSP,2,"Cone FC is considered to be %d\n", (fixed & 0x10)>>4) ;
          }
            
        loglvl1 = 1;
      }
  }
  if (cone_ci->cmd_state & EFG_SPISIG_HZ_MASK)
    {
      if (initialstatemode) return NULL;
      return createAtom("'1'");
    }

  if (current_cone_number<current_cone_number_min) current_cone_number=current_cone_number_min;

  guessed=gsp_guessedmemsymrise(cone_ci, losig);

  if (efg_test_signal_flags(losig, EFG_SIG_SET_OUTPATH))
    {
      if (loglvl1) avt_log(LOGGSP,2,"NOT ON PATH\n") ;
      cone_is_outpath=1;
    }

  /* obtain up and down cone expressions */
  if ( (ptcone->TYPE & CNS_LATCH) == CNS_LATCH ) {
    latch=1;
    avt_log(LOGGSP,2,"%s is a latch\n",ptcone->NAME);
  }

  cnsConeFunction_once(ptcone,1);

  if ((ptcone->TYPE & CNS_MEMSYM)!=0 && gsp_checkfulldatawrite(ptcone))
   this_fulldatawrite=1;

  do
    {
      fulldatawrite=0;
      indep_var=0;
      allinputcone=NULL;
      sigendflag=0;
      flag=0;
      InputIsCmd=0;
      cmdflag=0;
      ptpathsig = NULL;
      var2exp = NULL;
      cmdchain=NULL ;
      losigchain = NULL, after_current_cone_losigchain=NULL ;
      out_of_path_stuck=NULL;
      bddchainState = NULL ;
      bddchainEvent = NULL ;
      res_abl = NULL ;
      cone_outpath = NULL ;
      exp_bdd = NULL ; 
      detect_switch = NULL;
      gsp_simple_correlation_list=NULL;        
      nbpathsigbefore=0;
      alternate_ci_cmd=NULL;

      ptuser1 = getptype(ptcone->USER, CNS_UPEXPR);
      if (!ptuser1->DATA) {
        if (!GSP_QUIET_MODE)
          {
            avt_errmsg(GSP_ERRMSG, "001", AVT_WARNING, ptcone->NAME);
          }
        sup_abl = createAtom("'0'");
      }
      else 
        sup_abl = copyExpr((chain_list *)ptuser1->DATA); /* sup original */
      ptuser2 = getptype(ptcone->USER, CNS_DNEXPR);
      if (!ptuser2->DATA) {
        if (!GSP_QUIET_MODE)
          {
            avt_errmsg(GSP_ERRMSG, "002", AVT_WARNING, ptcone->NAME);
          }
        sdn_abl = createAtom("'0'");
      }
      else
        sdn_abl = copyExpr((chain_list *)ptuser2->DATA); /* sdn original */

      if ( loglvl1 ) {
        avt_log(LOGGSP,3,">>> sup : ") ;
        displayExprLog(LOGGSP,3,sup_abl);
        avt_log(LOGGSP,3,">>> sdn : ") ;
        displayExprLog(LOGGSP,3,sdn_abl);
      }

      outlosig = efg_getlosigcone(ptcone);
      if ((getptype(outlosig->USER,EFG_SIG_END)) || cone_ci->cmd!=NULL) sigendflag =1;
      if ((spisig = efg_GetSpiSig(EFGCONTEXT->SPISIG,getsigname(outlosig)))!=NULL) outcone_pathnum=spisig->PATHNUM;
      else outcone_pathnum=0;

      /* trouve le signal le  + tardif et regle eventuellement les pb 
       * de correlation des entrees du cone */
      gsp_mark_cone_inputs(ptcone, 1);
      maxnumber = -1;
      memsym=NULL;
      last_cone_number_min=current_cone_number_min; last_cone_number_max=current_cone_number;
      for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
        cmdflag=0;
        if ((ptinedge->TYPE & CNS_VDD) != 0) continue;
        if ((ptinedge->TYPE & CNS_VSS) != 0) continue;
        if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
        if ((ptinedge->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
        if ((ptinedge->TYPE & CNS_EXT) != 0) {
          locon = ptinedge->UEDGE.LOCON;
          if ( locon->SIG == outlosig || gsp_also_exist_cone(ptcone->INCONE, locon)) continue;
          if (getptype(locon->SIG->USER,EFG_SIG_BEG)) {
            if (getchain(losigchain, locon->SIG)==NULL)
              losigchain = addchain(losigchain, locon->SIG);
            last_cone_number_min=last_cone_number_max=-1;
            ptpathsig = locon->SIG;
          }
          if ( locon->TYPE == 'C' ) {// connector from gns instances
            if ( efg_sig_is_onpath (locon->SIG,figname) ) {
              if (!efg_test_signal_flags(locon->SIG, EFG_SIG_SET_OUTPATH))
                {
                  number = (long)efg_get_hier_sigptype (locon->SIG, figname,EFG_SIG_SET_NUM) ;
                  if (number > maxnumber && (ptpathsig==NULL || number<current_cone_number)) {
                    maxnumber = number;
                    last_cone_number_min=last_cone_number_max=number;
                    if (getchain(losigchain, locon->SIG)==NULL)
                      losigchain = addchain(losigchain, locon->SIG);
                    ptpathsig = locon->SIG;
                  }
                }
            }
          }
          continue;
        }
        ptinputcone = ptinedge->UEDGE.CONE;
        if((ptinputcone->TECTYPE & CNS_ZERO)==CNS_ZERO || (ptinputcone->TECTYPE & CNS_ONE)==CNS_ONE) continue;
        // Traitement des latchs 
        if ( (ptinedge->TYPE & CNS_COMMAND) == CNS_COMMAND && ptinputcone->NAME==cone_ci->cmd) {
          int tog, dtog=0;
          if ( (cone_ci->cmd_state & EFG_SPISIG_CMDDIR_MASK) == EFG_SPISIG_CMDDIR_MASK)
             dtog=1;
          avt_log(LOGGSP,2,"Command: %s (val=%d)\n",ptinputcone->NAME,dtog);
          if (!this_fulldatawrite && cone_ci->Date==-1)
          {
            avt_log(LOGGSP,2,"-- detected that data can not be fully written thru memsym '%s', command will be disabled\n",ptcone->NAME);
            disacmd=1;
          }
          command = efg_getlosigcone(ptinputcone) ;
          cmdchain = addptype (cmdchain,dtog,command);
          cmdflag = 1;
          if ((lsigd=gsp_FindSimpleCorrelationForToBeStuckConnectors(ptinputcone, 0, V_INT_TAB[__EFG_MAX_DEPTH].VALUE, &tog,-1,NULL))!=NULL
              || (lsigd=gsp_GetUserCorrelation(ptinputcone->NAME, figname, &tog, -1))!=NULL)
          {
            command = lsigd ;
            cmdchain = addptype (cmdchain,(dtog+tog)&1,command);
            avt_log(LOGGSP,2,"Deduced Command: %s (val=%d)\n",getsigname(command), (dtog+tog)&1);
          }
          if ( sigendflag )
            continue;
        }

        if (cone_is_outpath || (initialstatemode & GSP_FORCE_COMPUTE_INITIAL_STATE)) allinputcone=addchain(allinputcone, ptinputcone);
        ci=gsp_get_cone_info(ptinputcone);

        if ( getptype (ptinedge->USER,CNS_SWITCHPAIR)) 
          detect_switch = addchain (detect_switch,ptinedge);

        losig = efg_getlosigcone(ptinputcone) ;
        if ((ptinedge->TYPE & CNS_MEMSYM) != 0)
          {
            memsym=losig, memsym_ci=ci;
            if (cone_ci->cmd!=NULL)
              {
                if (gsp_checkfulldatawrite(ptinputcone)) fulldatawrite=1;
                avt_log(LOGGSP,2,"-- detected that data %s be fully written thru memsym '%s'\n",fulldatawrite?"can":"can not",ptinputcone->NAME);
              }
          }
        if (efg_test_signal_flags(losig, EFG_SIG_SET_OUTPATH) || (ci && ci->usestate))
          {
            out_of_path_stuck=addptype(out_of_path_stuck, (long)ci, losig);
          }
        else if (ci != NULL) {
          if ( losig == outlosig ) continue;
          if ((number = ci->Date)==-1) number=ci->MinDate;
            

          if (getptype(losig->USER,EFG_SIG_SET)) 
            {
              if (/*GSP_REAL_ORDER_SET &&*/ current_cone_number!=-1 && number>current_cone_number)
                after_current_cone_losigchain=addchain(after_current_cone_losigchain, losig);
              else
                {
                  if (getchain(losigchain, losig)==NULL)
                    {
                      losigchain = addchain(losigchain, losig);
                      if (getchain(after_current_cone_losigchain, ptpathsig)!=NULL)
                        maxnumber=-1;
                    }
                }
              if ((lsigd=gsp_GetUserCorrelation(ptinputcone->NAME, figname, &tog, 1))!=NULL)
              {
                gsc=mbkalloc(sizeof(*gsc));  
                gsc->sigd=getsigname(lsigd);
                gsc->sigs=getsigname(efg_getlosigcone(ptinputcone));
                gsc->lsigd=lsigd;
                gsc->lsigs=efg_getlosigcone(ptinputcone);
                gsc->inv=tog;
                if (gsp_getcorrel(*allcorrel, gsc->sigd)==NULL)
                   *allcorrel=gsp_addcorrel(*allcorrel, gsc->sigd, gsc->sigs, tog);
                gsp_simple_correlation_list=addchain(gsp_simple_correlation_list, gsc);
              }
           } 
          if ((ptpathsig==memsym && this_fulldatawrite && ci->Date==-1) || (number > maxnumber && (ptpathsig==NULL || number<current_cone_number))) {
            maxnumber = number;
            last_cone_number_min=ci->MinDate; last_cone_number_max=ci->Date;
            ptpathsig = losig;
            if ( cmdflag )
              InputIsCmd = 1;
            else
              InputIsCmd = 0;
          }
        }
        else {
          losig_list *sigext;

          losig = efg_getlosigcone(ptinputcone) ;

          if (getptype(losig->USER,EFG_SIG_BEG)) {
            if (getchain(losigchain, losig)==NULL)
              losigchain = addchain(losigchain, losig);
            if ( !ptpathsig ) {
              last_cone_number_min=last_cone_number_max=-1;
              ptpathsig = losig;
              if ( cmdflag )
                InputIsCmd = 1;
              else
                InputIsCmd = 0;
            }
          }

          if ( losig ) {
            sigext = efg_get_ext_sig ( losig );
            if (sigext && sigext->TYPE != 'E') {
              /*si entree du cone = signal a coller */
              ptpathsig2 = efg_getlosigcone(ptinputcone);
              if ( ptpathsig2 == outlosig ) continue;
              if (ptpathsig2 && (getptype(ptpathsig2->USER,EFG_SIG_SET) != NULL)) {
                if (getchain(losigchain, ptpathsig2)==NULL)
                  losigchain = addchain(losigchain, ptpathsig2);
                if ( !ptpathsig ) {
                  last_cone_number_min=-1; last_cone_number_max=-1;
                  ptpathsig = ptpathsig2;
                  if ( cmdflag )
                    InputIsCmd = 1;
                  else
                    InputIsCmd = 0;
                }
                else indep_var++;
              }
              else if (V_INT_TAB[__EFG_MAX_DEPTH].VALUE > 0) {
                if ((ptinedge->TYPE & CNS_LOOP)==0)
                {
                  chain_list *old=cone_outpath;
                  cone_outpath = gsp_AddConeOutPath(cone_outpath,
                                                    ptinputcone,
                                                    &losigchain,
                                                    0,
                                                    V_INT_TAB[__EFG_MAX_DEPTH].VALUE,ptcone);
                  if ( cone_outpath!=old ) {
                    exp_bdd      = gsp_GetSupFromCone(cone_outpath);
                    var2exp = addptype(var2exp,
                                       (long)ptinputcone->NAME,
                                       (void*)exp_bdd) ;
                  }
                  else indep_var++;
                }
                else indep_var++;
              }
            }
            else// if ((ptcone->TYPE & CNS_PRECHARGE)==CNS_PRECHARGE)
              {
                int tog=0;
                losig_list *lsigd;
                if ((lsigd=gsp_FindSimpleCorrelationForToBeStuckConnectors(ptinputcone, 0, V_INT_TAB[__EFG_MAX_DEPTH].VALUE, &tog,0,NULL))!=NULL
                    || (lsigd=gsp_GetUserCorrelation(ptinputcone->NAME, figname, &tog, 1))!=NULL)
                  {
                    gsc=mbkalloc(sizeof(*gsc));  
                    gsc->sigd=getsigname(lsigd);
                    gsc->sigs=getsigname(efg_getlosigcone(ptinputcone));
                    gsc->lsigd=lsigd;
                    gsc->lsigs=efg_getlosigcone(ptinputcone);
                    gsc->inv=tog;
                    if (gsp_getcorrel(*allcorrel, gsc->sigd)==NULL)
                       *allcorrel=gsp_addcorrel(*allcorrel, gsc->sigd, gsc->sigs, tog);
                    gsp_simple_correlation_list=addchain(gsp_simple_correlation_list, gsc);
                  }
                else indep_var++;
              }
          }
        }
      }

      // correction
      if (ptpathsig!=NULL && getchain(losigchain, ptpathsig)==NULL)
        {
          after_current_cone_losigchain=delchaindata(after_current_cone_losigchain,  ptpathsig);
          losigchain=addchain(losigchain, ptpathsig);
        }

      if (cone_ci->precharged)
        {
          if ((spisig = efg_GetSpiSig(EFGCONTEXT->SPISIG,ptcone->NAME))!=NULL)
            spisig->ADDIC|=EFG_ADDIC_FORCE_1;
        }

      gsp_mark_cone_inputs(ptcone, 0);
    
      /* construction du circuit equivalent au cone */
      ptconecct = gsp_buildconecct(ptcone);
      ptconecct = gsp_buildexpconecct(ptconecct, cone_outpath);
    
      gsp_constraint_abl_with_stuck( ptcone, &sup_abl, &sdn_abl );
     
      // switch treatment ...
      if ( detect_switch ) {
        switchcmd = gsp_treat_switch ( detect_switch, &sup_abl, &sdn_abl);
        for (pt=switchcmd; pt!=NULL; pt=pt->NEXT)
        {
          if (gsp_getcorrel(*allcorrel, (char *)pt->TYPE)==NULL)
            *allcorrel=gsp_addcorrel(*allcorrel, (char *)pt->TYPE, (char *)pt->DATA, 1);
        }
        freechain (detect_switch);
      }

      /* remplace dans les abl les switchs pour imposer que leurs entrées
         sont complémentaire. si ce n'est pas fait, gsp_SpiceFindInput()
         n'a aucune raison de prendre en compte cette contrainte */
      gsp_constraint_abl_with_switch( switchcmd, sup_abl, sdn_abl );
      gsp_constraint_abl_with_Correlations(gsp_simple_correlation_list, sup_abl, sdn_abl);

      sup = ablToBddCct(ptconecct, sup_abl);
      sdn = ablToBddCct(ptconecct, sdn_abl);

      freeExpr(sup_abl);
      freeExpr(sdn_abl);

      /* obtain non-conflicting expressions */
      res = gsp_FindSigSlope(outlosig) ;

      if (cone_ci->precharged && res==0) 
        {
          rup = applyBinBdd(AND, notBdd(sup), notBdd(sdn));
          avt_log(LOGGSP,2,"-- initial state (rup) set to hz expression\n") ;
        }
      else
        rup = applyBinBdd(AND, sup, notBdd(sdn));
      if (cone_ci->precharged && res==1)
        {
          rdn = applyBinBdd(AND, notBdd(sdn), notBdd(sup));
          avt_log(LOGGSP,2,"-- initial state (rdn) set to hz expresison\n") ;
        }
      else
        rdn = applyBinBdd(AND, sdn, notBdd(sup));

      if ( loglvl1 ) {
        avt_log(LOGGSP,2,">>> rup : ") ;
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, rup));
        freeExpr(tmpabl);
        avt_log(LOGGSP,2,">>> rdn : ") ;
        displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, rdn));
        freeExpr(tmpabl);
      }
      /* substitution des abl si sig correlees */
      if (cone_outpath != NULL) 
      {
        gsp_treat_correlation ( cone_outpath, ptconecct, var2exp, &rup, &rdn );
        freechain(cone_outpath) ;
      }
    
      /*----------------------------------------------------------*/
      /*                                                          */
      /* Positionnement du signal le + tardif en tete de chain    */
      /*                                                          */
      /*----------------------------------------------------------*/
      losigchain = gsp_classlosig (losigchain , ptpathsig) ;

      ptuser2=NULL;
      for (pt=out_of_path_stuck; pt!=NULL; pt=ptuser1)
        {
          ptuser1=pt->NEXT;
          ci=(gsp_cone_info *)pt->TYPE;
          avant_apres=checkoutofpathsig_input((losig_list*)pt->DATA, ci, last_cone_number_min, last_cone_number_max);
          remove=1;
          if (avant_apres!=0)
            {
              if (avant_apres==-1)
                val=ci->BeforeTransitionState;
              else
                val=ci->AfterTransitionState;

              if (val!=-1)
                {
                  if (avant_apres==-1) val|=0x20;
                  pt->TYPE=val;
                  ptuser2=pt;
                  remove=0;
                }
            }
          if (remove)
            {
              if (ptuser2==NULL) out_of_path_stuck=pt->NEXT;
              else ptuser2->NEXT=pt->NEXT;
              pt->NEXT=NULL;
              freeptype(pt);
            }
        }

      if (ptpathsig && losigchain->NEXT) for ( chain = losigchain->NEXT; chain ; chain=chain->NEXT ) nbpathsigbefore++;

      if (loglvl1) {
        if (ptpathsig)
          {
            avt_log(LOGGSP,2,"==> Latest signal: %s %s<==\n",efg_getconename(ptpathsig),memsym==ptpathsig?"(Memsym)":"") ;
            if ( losigchain->NEXT ) {
              avt_log(LOGGSP,2,"==> Other signal on path (earlier):\n") ;
              for ( chain = losigchain->NEXT; chain ; chain=chain->NEXT )
                {
                  avt_log(LOGGSP,2,"- %s %s\n",efg_getconename((losig_list*)chain->DATA),memsym==chain->DATA?"(Memsym)":"") ;
                }
            }
          }
        else
          avt_log(LOGGSP,2,"==> No input signal on path <==\n") ;
        if (after_current_cone_losigchain)
          {
            avt_log(LOGGSP,2,"==> Other signal on path (later):\n") ;
            for ( chain = after_current_cone_losigchain; chain ; chain=chain->NEXT ) 
              avt_log(LOGGSP,2,"+ %s %s\n",efg_getconename((losig_list*)chain->DATA),memsym==chain->DATA?"(Memsym)":"") ;
          }
        if (out_of_path_stuck!=NULL)
          {
            avt_log(LOGGSP,2,"==> Out of path signal:\n") ;
            for (pt=out_of_path_stuck; pt!=NULL; pt=pt->NEXT)
              {
                avt_log(LOGGSP,2,"%c %s \n",(pt->TYPE & 0x20)!=0?'+':'-', efg_getconename((losig_list*)pt->DATA)) ;
              }
          }
      }

      if (cone_is_outpath || (initialstatemode & GSP_FORCE_COMPUTE_INITIAL_STATE))
        {
          if (cone_ci)
            {
              gsp_compute_avant_apres(ptconecct, cone_ci, allinputcone, sup, sdn, initialstatemode,cone_ci->Date==-1?losigchain:NULL,cone_ci->Date==-1?after_current_cone_losigchain:NULL,ptpathsig);
              if (cone_ci->Date!=-1)
                {
                  if ((initialstatemode & GSP_FORCE_COMPUTE_INITIAL_STATE) && cone_ci->BeforeTransitionState!=-1 && cone_ci->AfterTransitionState!=-1)
                    {
                      if ((spisig = efg_GetSpiSig(EFGCONTEXT->SPISIG,ptcone->NAME))!=NULL)
                        spisig->ADDIC|=cone_ci->BeforeTransitionState==0?EFG_ADDIC_FORCE_0:EFG_ADDIC_FORCE_1;
                      cone_ci->usestate=1;
                    }
                }
              else
                gsp_compute_avant_apres(ptconecct, cone_ci, allinputcone, sup, sdn, 0,losigchain,after_current_cone_losigchain, ptpathsig);
            }
          freechain(allinputcone); freeptype(out_of_path_stuck);
          freechain(losigchain); freechain(after_current_cone_losigchain);            
          destroyCct(ptconecct);

          freeptype ( switchcmd );
          freechaindata(gsp_simple_correlation_list);
          freeptype(cmdchain);
          if (initialstatemode) return NULL;
          return(createAtom("'1'"));
        }

      /*----------------------------------------------------------*/
      /*                                                          */
      /* Retrouver le front du signal le plus tardif              */
      /*                                                          */
      /*----------------------------------------------------------*/
      if (!ptpathsig)
        {
          freeptype(out_of_path_stuck);
          freechain(losigchain); freechain(after_current_cone_losigchain);            
          destroyCct(ptconecct);
          freeptype ( switchcmd );
          freechaindata(gsp_simple_correlation_list);
          freeptype(cmdchain);
          return (createAtom("'0'"));
        }
      res = gsp_FindSigSlope(ptpathsig) ;
      if (res < 0) { 
        /* transition non specifiee dans le .inf */
        freeptype(out_of_path_stuck);
        freechain(losigchain); freechain(after_current_cone_losigchain);
        destroyCct(ptconecct);
        freeptype ( switchcmd );
        freechaindata(gsp_simple_correlation_list);
        freeptype(cmdchain);
        return(createAtom("'0'"));
      }
    
      /*----------------------------------------------------------*/
      /*                                                          */
      /* 1) Detecter la transition de la sortie                   */
      /* 2) Retourne l'abl correspondant en fonction du front     */
      /*    du signal le + tardif                                 */
      /* NB: Condition a retourner en fonction du front de sortie */
      /* Pour _/-  cond = ( Rdn/Ti ) . ( Rup/Tf )                 */
      /* Pour -\_  cond = ( Rup/Ti ) . ( Rdn/Tf )                 */
      /* Ti : etat initial du front d'entree                      */
      /* Tf : etat final du front d'entree                        */
      /*                                                          */
      /*----------------------------------------------------------*/

      /*----------------------------------------------------------*/
      /* Nature du front d'entree le + tardif donne par le .inf   */
      /*----------------------------------------------------------*/
      else {
        if (res == 1) {
          intransition = 'U' ;
          if ( loglvl1 ) {
            conename = efg_getconename ( ptpathsig );
            avt_log(LOGGSP,2,"<  UP  > transition for |  IN cone | %s\n",
                    conename) ;
          }
        }
        else if (res == 0) {
          intransition = 'D' ;
          if ( loglvl1 ) {
            conename = efg_getconename ( ptpathsig );
            avt_log(LOGGSP,2,"< DOWN > transition for |  IN cone | %s\n",
                    conename) ;
          }
        }
        else {
          if ( loglvl1 )
            avt_log(LOGGSP,2," In signal %s : unknown transition matched!!!\n",
                    getsigname(ptpathsig)) ;
          freechain(losigchain); freechain(after_current_cone_losigchain); 
          destroyCct(ptconecct);
          freeptype ( switchcmd );
          freechaindata(gsp_simple_correlation_list);
          freeptype(cmdchain);
          return createAtom("'0'") ;
        }
      }
    
      /*----------------------------------------------------------*/
      /* Affectation des etats si +sieurs transitions             */
      /* sont specifiees en entree d'un cone: cad ke les entrees  */
      /* arrivant avant le signal le plus tardif doivent etre     */
      /* stables                                                  */
      /*----------------------------------------------------------*/
      /* 1) la tete de losigchain contient le sig le + tardif     */
      /*----------------------------------------------------------*/
      /* 2) Positionne les etats des autres entrees du cone       */
      /*----------------------------------------------------------*/
    
      if ( intransition == 'U') {
        ptpathsig = (losig_list*)losigchain->DATA ;
        conename = efg_getconename ( ptpathsig );
        bddchainEvent = addchain(bddchainEvent,ablToBddCct(ptconecct,
                                                           tmpatom=createAtom_no_NA( conename ))) ;
        freeExpr(tmpatom);
      }
      else {
        ptpathsig = (losig_list*)losigchain->DATA ;
        conename = efg_getconename ( ptpathsig );
        bddchainEvent = addchain(bddchainEvent,notBdd(ablToBddCct(ptconecct,
                                                                  tmpatom=createAtom_no_NA( conename )))) ;
        freeExpr(tmpatom);
      }
      for (chain = losigchain->NEXT ; chain ; chain = chain->NEXT) {
        done = 0;
        ptpathsig = (losig_list*)chain->DATA ;
        if (ptpathsig==memsym && !fulldatawrite && cone_ci->cmd!=NULL) continue;
        for (pt=cmdchain; pt!=NULL && pt->DATA!=ptpathsig; pt=pt->NEXT) ;
        if (pt!=NULL) continue;
        if ( (ptype = getptype (ptpathsig->USER,GSP_SWITCHPAIR)) ) {
          oppsig = (losig_list*)ptype->DATA;
          if ( oppsig == (losig_list*)losigchain->DATA ) {
            ptuser = getptype(ptpathsig->USER,EFG_SIG_SET) ;
            if (ptuser->DATA == EFG_SIG_SET_FALL) {
              conename = efg_getconename ( ptpathsig );
              bddchainEvent = addchain(bddchainEvent,notBdd(ablToBddCct(ptconecct,
                                                                        tmpabl=createAtom_no_NA( conename )))) ;
              avt_log(LOGGSP,2,"< DOWN > event for | early IN switch command cone | %s\n", conename);
            }
            else {
              conename = efg_getconename ( ptpathsig );
              bddchainEvent = addchain(bddchainEvent,(ablToBddCct(ptconecct,
                                                                  tmpabl=createAtom_no_NA( conename )))) ;
              avt_log(LOGGSP,2,"<  UP  > event for | early IN switch command cone | %s\n", conename);
            }
            done = 1;
            freeExpr(tmpabl);
          }
          ptpathsig->USER = delptype (ptpathsig->USER,GSP_SWITCHPAIR);
        }
        if (!done) {
          ptuser = getptype(ptpathsig->USER,EFG_SIG_SET) ;
          if (ptpathsig==memsym && gsp_guessedmemsymrise(memsym_ci, memsym))
            {
              conename = efg_getconename ( ptpathsig );
              bddchainState = addchain(bddchainState,(ablToBddCct(ptconecct,
                                                                  tmpabl=createAtom_no_NA( conename )))) ;
              avt_log(LOGGSP,2,"< 1 > state for | precharged IN cone | %s\n", conename);
            }
          else if (ptuser->DATA == EFG_SIG_SET_FALL) {
            int atonestep;
            conename = efg_getconename ( ptpathsig );
            atonestep=gsp_coneatonestep(efg_getcone((losig_list *)losigchain->DATA), efg_getcone(ptpathsig));
            if (atonestep) nbatonestep++;
            if ((ptcone->TYPE & CNS_MEMSYM)!=CNS_MEMSYM && (checkmode==0 || atonestep==0))
              bddchainState = addchain(bddchainState,notBdd(ablToBddCct(ptconecct,tmpabl=createAtom_no_NA( conename )))) ;
            else
              bddchainEvent = addchain(bddchainEvent,notBdd(ablToBddCct(ptconecct,tmpabl=createAtom_no_NA( conename )))) ;
            if ((ptcone->TYPE & CNS_PRECHARGE)==CNS_PRECHARGE)
              {
                bddchainState = addchain(bddchainState,ablToBddCct(ptconecct,
                                                                   tmpabl=createAtom_no_NA( conename ))) ;
                avt_log(LOGGSP,2,"< 0 & 1 > state for | early IN cone | %s\n", conename);
              }
            else
              if ((ptcone->TYPE & CNS_MEMSYM)!=CNS_MEMSYM && (checkmode==0 || atonestep==0))
                avt_log(LOGGSP,2,"< 0 > state for | early IN cone | %s\n", conename);
              else
                avt_log(LOGGSP,2,"< DOWN > transition for IN cone | %s\n", conename);
          }
          else {
            int atonestep;
            conename = efg_getconename ( ptpathsig );
            atonestep=gsp_coneatonestep(efg_getcone((losig_list *)losigchain->DATA), efg_getcone(ptpathsig));
            if (atonestep) nbatonestep++;
            if ((ptcone->TYPE & CNS_MEMSYM)!=CNS_MEMSYM && (checkmode==0 || atonestep==0))
              bddchainState = addchain(bddchainState,ablToBddCct(ptconecct, tmpabl=createAtom_no_NA( conename ))) ;
            else
              bddchainEvent = addchain(bddchainEvent,ablToBddCct(ptconecct, tmpabl=createAtom_no_NA( conename )));
            if ((ptcone->TYPE & CNS_PRECHARGE)==CNS_PRECHARGE)
              {
                bddchainState = addchain(bddchainState,(notBdd(ablToBddCct(ptconecct,
                                                                           tmpabl=createAtom_no_NA( conename ))))) ;
                avt_log(LOGGSP,2,"< 0 & 1 > state for | early IN cone | %s\n", conename);
              }
            else
              if ((ptcone->TYPE & CNS_MEMSYM)!=CNS_MEMSYM && (checkmode==0 || atonestep==0))
                avt_log(LOGGSP,2,"< 1 > state for | early IN cone | %s\n", conename);
              else
                avt_log(LOGGSP,2,"< UP > transition for IN cone | %s\n", conename);
          }
          freeExpr(tmpabl);
        }
      }
      // signaux plus tardifs, on prend l'etat initial
      for (chain = after_current_cone_losigchain ; chain ; chain = chain->NEXT) {
        done = 0;
        ptpathsig = (losig_list*)chain->DATA ;
        if (ptpathsig==memsym && !fulldatawrite && cone_ci->cmd!=NULL)
        {
          conename = efg_getconename ( ptpathsig );
          avt_log(LOGGSP,2,"< REMOVED MEMSYM > for | later IN cone | %s\n", conename);
          tmpabl=createAtom_no_NA( conename );
          bddchainState = addchain(bddchainState,notBdd(ablToBddCct(ptconecct,tmpabl))) ;
          bddchainState = addchain(bddchainState,ablToBddCct(ptconecct,tmpabl)) ;
          freeExpr(tmpabl);
          continue;
        }
        for (pt=cmdchain; pt!=NULL && pt->DATA!=ptpathsig; pt=pt->NEXT) ;
        if (pt!=NULL) continue;
        if (!initialstatemode && getptype(ptpathsig->USER, EFG_FORCE_IC)==NULL)
          {
            if ((spisig = efg_GetSpiSig(EFGCONTEXT->SPISIG,efg_getconename(ptpathsig)))!=NULL && (spisig->PATHNUM & outcone_pathnum)!=0)
              ptpathsig->USER=addptype(ptpathsig->USER, EFG_FORCE_IC, NULL);
          }
        if ( (ptype = getptype (ptpathsig->USER,GSP_SWITCHPAIR)) ) {
          oppsig = (losig_list*)ptype->DATA;
          if ( oppsig == (losig_list*)losigchain->DATA ) {
            ptuser = getptype(ptpathsig->USER,EFG_SIG_SET) ;
            if (ptuser->DATA == EFG_SIG_SET_RISE) state=0; else state=1;
            if (state==0) {
              conename = efg_getconename ( ptpathsig );
              bddchainEvent = addchain(bddchainEvent,notBdd(ablToBddCct(ptconecct,
                                                                        tmpabl=createAtom_no_NA( conename )))) ;
              avt_log(LOGGSP,2,"< DOWN > event for | later IN switch command cone | %s\n", conename);
            }
            else {
              conename = efg_getconename ( ptpathsig );
              bddchainEvent = addchain(bddchainEvent,(ablToBddCct(ptconecct,
                                                                  tmpabl=createAtom_no_NA( conename )))) ;
              avt_log(LOGGSP,2,"<  UP  > event for | later IN switch command cone | %s\n", conename);
            }
            done = 1;
            freeExpr(tmpabl);
          }
          ptpathsig->USER = delptype (ptpathsig->USER,GSP_SWITCHPAIR);
        }
        if (!done) {
          ptuser = getptype(ptpathsig->USER,EFG_SIG_SET) ;
          if (ptuser->DATA == EFG_SIG_SET_RISE) state=0; else state=1;
          if (state==0) {
            conename = efg_getconename ( ptpathsig );
            bddchainState = addchain(bddchainState,notBdd(ablToBddCct(ptconecct,
                                                                      tmpabl=createAtom_no_NA( conename )))) ;
            avt_log(LOGGSP,2,"< 0 > state for | later IN cone | %s\n", conename);
          }
          else {
            conename = efg_getconename ( ptpathsig );
            bddchainState = addchain(bddchainState,(ablToBddCct(ptconecct,
                                                                tmpabl=createAtom_no_NA( conename )))) ;
            avt_log(LOGGSP,2,"< 1 > state for | later IN cone | %s\n", conename);
          }
          freeExpr(tmpabl);
        }
      }
      for (pt=out_of_path_stuck; pt!=NULL; pt=pt->NEXT)
        {
          done=0;
          ptpathsig=(losig_list *)pt->DATA;
          if ( (ptype = getptype (ptpathsig->USER,GSP_SWITCHPAIR)) ) {
            oppsig = (losig_list*)ptype->DATA;
            if ( oppsig == (losig_list*)losigchain->DATA ) {
              ptuser = getptype(oppsig->USER,EFG_SIG_SET) ;
              if (ptuser->DATA == EFG_SIG_SET_RISE) state=0; else state=1;
              if (state==0) {
                conename = efg_getconename ( ptpathsig );
                bddchainEvent = addchain(bddchainEvent,notBdd(ablToBddCct(ptconecct,
                                                                          tmpabl=createAtom_no_NA( conename )))) ;
                avt_log(LOGGSP,2,"< DOWN > event for | later IN switch command cone | %s\n", conename);
              }
              else {
                conename = efg_getconename ( ptpathsig );
                bddchainEvent = addchain(bddchainEvent,(ablToBddCct(ptconecct,
                                                                    tmpabl=createAtom_no_NA( conename )))) ;
                avt_log(LOGGSP,2,"<  UP  > event for | later IN switch command cone | %s\n", conename);
              }
              done = 1;
              freeExpr(tmpabl);
            }
            ptpathsig->USER = delptype (ptpathsig->USER,GSP_SWITCHPAIR);
          }
          if (!done)
          {
            conename = efg_getconename ((losig_list *)pt->DATA);
            mybdd=ablToBddCct(ptconecct, tmpabl=createAtom_no_NA( conename ));
            if ((pt->TYPE & 0x1)==0) mybdd=notBdd(mybdd);
            bddchainState = addchain(bddchainState, mybdd);
            avt_log(LOGGSP,2,"< %d > state for | %s IN cone | %s\n", pt->TYPE & 0x1,(pt->TYPE & 0x20)==0?"early":"later", conename);
            freeExpr(tmpabl);
          }
        }
   
      freeptype(out_of_path_stuck);

      /*----------------------------------------------------------*/
      /* Calcul des contraintes de bdd                            */
      /*                                                          */
      /* En fonction de la transition de la sortie                */
      /*----------------------------------------------------------*/
      res = gsp_FindSigSlope(outlosig) ;

      if (fixed!=0)
        {
          int ic, fc;
          if (fixed & 0x2) ic=(fixed & 0x1);
          else ic=res?0:1;
          if (fixed & 0x20) fc=(fixed & 0x10)>>4;
          else fc=res?1:0;
          avt_log(LOGGSP,2,"< ic:%d fc:%d > transition for | OUT cone | %s\n", ic, fc, ptcone->NAME) ;
          outtransition = (ic<<1)|fc ;
          cone_ci->used_ic=ic;
        }
      else if (res == 1) {
        outtransition = 'U' ;
        cone_ci->used_ic=0;
        avt_log(LOGGSP,2,"<  UP  > transition for | OUT cone | %s\n",ptcone->NAME) ;
      }
      else if (res == 0)
        {
          outtransition = 'D' ;
          cone_ci->used_ic=1;
          avt_log(LOGGSP,2,"< DOWN > transition for | OUT cone | %s\n",ptcone->NAME) ;
        }
      else
        {
          avt_log(LOGGSP,2,"Unknown transition for out signal %s\n",ptcone->NAME) ;
          if (!GSP_QUIET_MODE) // fprintf(stderr,"[GSP ERR] Unknown transition for out signal %s\n",ptcone->NAME) ;
            avt_errmsg(GSP_ERRMSG, "003", AVT_ERROR, ptcone->NAME);
        
          freechain(losigchain); freechain(after_current_cone_losigchain);
          destroyCct(ptconecct);
          freeptype ( switchcmd );
          freechaindata(gsp_simple_correlation_list);
          freeptype(cmdchain);
          return createAtom("'0'") ;
        }

      freechain(losigchain);
      freechain(after_current_cone_losigchain);

      if ((ptcone->TYPE & CNS_TRI) != 0) HZ = 'Y' ;
      else HZ='N';

      if (HZ == 'N') {
        int j=0, donothing=0;
        char *order;
        
        if ((ptcone->TYPE & CNS_MEMSYM)!=0) order="ch"; // conflictuel prioritaire puis hz
        else if (latch) order="b"; // "hc"; // hz puis conflictuel
        else
        {
          order="h"; // hz only
          if (checkmode==0 && nbatonestep>0)
            donothing=1;
        }

        condition = gsp_SetConstraints (ptconecct,bddchainState,bddchainEvent,rup,rdn,outtransition,'N',&ic,&fc) ;
        gsp_applystucks(&condition, switchcmd, ptconecct, gsp_simple_correlation_list, &rup, &rdn, (sigendflag && latch)?1:0, cmdchain, loglvl1, disacmd);
          
        if (!donothing)
        {
          while (order[j]!='\0' && equalBdd( condition ,BDD_zero) == 1)
            {
              if (order[j]=='h')
                {
                  if ( loglvl1 )
                    avt_log(LOGGSP,2,"\n###=> condition not found! Retring with HZ on output...\n\n") ;
                  condition = gsp_SetConstraints (ptconecct,bddchainState,bddchainEvent,rup,rdn,outtransition,'Y',&ic,&fc) ;
                }
              if (order[j]=='c')
                {
                  pNode       nrup=rup, nrdn=rdn;
                  if ( loglvl1 )
                    avt_log(LOGGSP,2,"\n###=> condition not found! Retring with possibly conflictual output transition...\n\n") ;
                  if (outtransition=='U') nrup=sup;
                  else nrdn=sdn;
                  condition = gsp_SetConstraints (ptconecct,bddchainState,bddchainEvent,nrup,nrdn,outtransition,'N',&ic,&fc) ;
                }
              if (order[j]=='b')
              {
                pNode       nrup=rup, nrdn=rdn;
                pNode res, ic0, fc0;
                if ( loglvl1 )
                    avt_log(LOGGSP,2,"\n###=> condition not found! Retring with HZ/CONFLICTUAL output...\n\n") ;
                res = gsp_SetConstraints (ptconecct,bddchainState,bddchainEvent,rup,rdn,outtransition,'Y',&ic0,&fc0) ;
                if (outtransition=='U') nrup=sup;
                else nrdn=sdn;
                condition = gsp_SetConstraints (ptconecct,bddchainState,bddchainEvent,nrup,nrdn,outtransition,'N',&ic,&fc) ;
                condition=applyBinBdd(OR, condition, res);
                ic=applyBinBdd(OR, ic, ic0);
                fc=applyBinBdd(OR, fc, fc0);
                avt_log(LOGGSP,2,"Merged conditions: ");
                displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, condition));
                freeExpr(tmpabl);
              }
              gsp_applystucks(&condition, switchcmd, ptconecct, gsp_simple_correlation_list, &rup, &rdn, (sigendflag && latch)?1:0, cmdchain, loglvl1, disacmd);
              j++;
            }
        }
        
      }
      else {
        // la transition de sortie part de l etat HZ 
        // on ne peut pas partir d'un etat HZ car on
        // ne pourrait plus estime la propagation  
        avt_log(LOGGSP,2,"\n###=> HZ detected but threated if not...\n") ;
        condition = gsp_SetConstraints (ptconecct,bddchainState,bddchainEvent,rup,rdn,outtransition,'N',&ic,&fc) ;
        gsp_applystucks(&condition, switchcmd, ptconecct, gsp_simple_correlation_list, &rup, &rdn, (sigendflag && latch)?1:0, cmdchain, loglvl1, disacmd);
        
        if ( equalBdd( condition ,BDD_zero) == 1 ) {
          avt_log(LOGGSP,2,"\n###=> Real HZ detected!!!\n\n") ;
          condition = gsp_SetConstraints (ptconecct,bddchainState,bddchainEvent,rup,rdn,outtransition,'Y',&ic,&fc) ;
          gsp_applystucks(&condition, switchcmd, ptconecct, gsp_simple_correlation_list, &rup, &rdn, (sigendflag && latch)?1:0, cmdchain, loglvl1, disacmd);
        }
      }


      if ( bddchainState )
        freechain(bddchainState);
      if ( bddchainEvent )
        freechain(bddchainEvent);
      if (var2exp != NULL) {
        for (ptuser1=var2exp; ptuser1!=NULL; ptuser1=ptuser1->NEXT)
          {
            freeExpr(ptuser1->DATA); 
          }
        freeptype(var2exp);
        //freechain(exp_bdd) ;
      }

      if ( equalBdd( condition ,BDD_zero) != 1 ) {
/*        if ( switchcmd ) {
          gsp_add_switch_stuck ( &condition, ptconecct, switchcmd);
        }
        gsp_add_Correlated_stuck(&condition, ptconecct, gsp_simple_correlation_list );

        if (gsp_simple_correlation_list!=NULL)
          {
            avt_log(LOGGSP,2,">>> final cond : ") ;
            displayExprLog(LOGGSP,2,tmpabl=bddToAblCct(ptconecct, condition));
            freeExpr(tmpabl);
          }*/
        if (sigendflag && latch ) {
          ptype_list *ptype,*local_cst = NULL;
          chain_list *ablcst;
          pNode cst;
          char *cmdname;
          losig_list *oppositesig;
          int inv;
/*
          if (cone_ci->cmd)
            {
              // filter with command state
              if ( (cone_ci->cmd_state & EFG_SPISIG_CMDDIR_MASK) == EFG_SPISIG_CMDDIR_MASK)
                ablcst = createAtom_no_NA( cone_ci->cmd ) ;
              else
                ablcst = notExpr (createAtom_no_NA ( cone_ci->cmd));
              cst = ablToBddCct(ptconecct, ablcst);
              freeExpr(ablcst);
              condition = applyBinBdd (AND, condition, cst) ;
              rup = applyBinBdd (AND, rup, cst) ;
              rdn = applyBinBdd (AND, rdn, cst) ;
            }
*/
          flag=0;
          local_cst = gsp_SpiceFindInput (local_cst,condition,ptconecct,corner,&flag,NULL);
          for ( ptype = local_cst ; ptype ; ptype=ptype->NEXT ) {
            long stuck = ptype->TYPE;
            for ( pt = cmdchain ; pt ; pt=pt->NEXT ) {
              command = (losig_list*)pt->DATA;
              cmdname = efg_getconename(command);
              if ( (char*)ptype->DATA == cmdname ) {
                avt_log(LOGGSP,2,"%s will be replaced by %s\n",
                        cmdname,(stuck==GSP_STUCK_ONE)?"vdd":"vss");
                gsp_replace_cmd_by_alim (ptcone,command,stuck);
                if ((oppositesig=gsp_GetOppositeSwitchCommandSignal(ptcone, cmdname))!=NULL)
                  {
                    avt_log(LOGGSP,2,"%s (opposite switch command) will be replaced by %s\n",
                            efg_getconename(oppositesig),(stuck==GSP_STUCK_ONE)?"vss":"vdd");
                    gsp_replace_cmd_by_alim (ptcone,oppositesig,(stuck==GSP_STUCK_ONE)?GSP_STUCK_ZERO:GSP_STUCK_ONE);
                  }
                if ((oppositesig=gsp_GetDirectCorrel(gsp_simple_correlation_list, command, &inv))!=NULL)
                  {
                    inv=(inv+stuck) &1;
                    avt_log(LOGGSP,2,"%s (correlated signal) will be replaced by %s\n",
                            efg_getconename(oppositesig),(inv==0)?"vss":"vdd");
                    gsp_replace_cmd_by_alim (ptcone,oppositesig,(inv==0)?GSP_STUCK_ZERO:GSP_STUCK_ONE);
                    if ( inv == 1 )
                      ablcst = createAtom_no_NA( efg_getconename(oppositesig) ) ;
                    else
                      ablcst = notExpr (createAtom_no_NA ( efg_getconename(oppositesig)));
                    cst = ablToBddCct(ptconecct, ablcst);
                    freeExpr(ablcst);
                    condition = constraintBdd (condition,cst) ;
                  }
                // delete the cmd constraint
                if ( stuck == GSP_STUCK_ONE )
                  ablcst = createAtom_no_NA( cmdname ) ;
                else
                  ablcst = notExpr (createAtom_no_NA ( cmdname));
                cst = ablToBddCct(ptconecct, ablcst);
                freeExpr(ablcst);
                condition = constraintBdd (condition,cst) ;
              }
            }
          }


          if ( local_cst ) freeptype (local_cst);
        }
      }

      freeptype ( switchcmd );
      freechaindata(gsp_simple_correlation_list);
      if ( cmdchain ) freeptype (cmdchain);

      if ( equalBdd( condition ,BDD_zero) == 1 && checkmode==0 && nbpathsigbefore>0 && nbatonestep>0)
        {
          checkmode=1;
          avt_log(LOGGSP,2,"\n----- retrying with glitcher mode initial conditions -----\n") ;
        }
      else
        {
          res_abl = bddToAblCct (ptconecct,condition) ;
          if (icabl!=NULL) *icabl = supportChain_listBddExpr(ptconecct,ic) ;
          if (fcabl!=NULL) *fcabl = supportChain_listBddExpr(ptconecct,fc) ;
          exitnow=1;
        }
      destroyCct(ptconecct);
    }  while (!exitnow);
  return res_abl ;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_SpiceFindInput                                             */
/*                                                                           */
/* Trouve des patterns satisfaisant la condition                             */
/* de transfert global du circuit                                            */
/* Retourne un ptype contenant les valeurs des entrees a positionner         */
/*                                                                           */
/* Le nom est complet <=> nom_instance.nom_sig                               */
/*                                                                           */
/*****************************************************************************/

static int getsoluce(pNode bdd, pCircuit circuit, ptype_list *start, chain_list **lst, char corner)
{
  char *name;
  int more, cnt=0;
  if (equalBdd(bdd,BDD_one) == 1)
    {
      cnt++;
      if (start!=NULL)
        {
          // gestion du corner
          if (*lst==NULL)
            *lst=addchain(*lst, dupptypelst(start));
          else
            {
              more=countchain((chain_list *)start)>countchain((chain_list *)(*lst)->DATA);
              if ((more && corner=='w') || (!more && corner=='b'))
                {
                  if (*lst!=NULL)
                    {
                      freeptype((*lst)->DATA);
                      freechain(*lst);
                    }
                  *lst=addchain(NULL, dupptypelst(start));
                }
            }
          // --
        }
    }
  else if (equalBdd(bdd,BDD_zero) != 1)
    {
      name = searchIndexCct (circuit,bdd->index) ;
      start=addptype(start, GSP_STUCK_ONE, name);
      cnt=getsoluce(bdd->high,circuit,start, lst, corner) ;
      start=delptype(start, GSP_STUCK_ONE);
      start=addptype(start, GSP_STUCK_ZERO, name);
      cnt+=getsoluce(bdd->low,circuit,start, lst, corner) ;
      start=delptype(start, GSP_STUCK_ZERO);
    }
  return cnt;
}

static void complete_soluce(pNode bdd, pCircuit circuit, ptype_list *start, ptype_list **list)
{
  ptype_list *pt, *pt1;
  char *name;
  if (equalBdd(bdd,BDD_one) == 1)
    {
      for (pt=start; pt!=NULL; pt=pt->NEXT)
        {
          for (pt1=*list; pt1!=NULL && pt1->DATA!=pt->DATA; pt1=pt1->NEXT) ;
          if (pt1==NULL)
            *list=addptype(*list, pt->TYPE==GSP_STUCK_ZERO?GSP_STUCK_ONE:GSP_STUCK_ZERO, pt->DATA);
        }
    }
  else if (equalBdd(bdd,BDD_zero) != 1)
    {
      name = searchIndexCct (circuit,bdd->index) ;

      start=addptype(start, GSP_STUCK_ONE, name);
      complete_soluce(bdd->high, circuit, start, list) ;
      start=delptype(start, GSP_STUCK_ONE);
      start=addptype(start, GSP_STUCK_ZERO, name);
      complete_soluce(bdd->low, circuit, start, list) ;
      start=delptype(start, GSP_STUCK_ZERO);
    }
}



ptype_list *gsp_SpiceFindInput_allsol (pNode bdd, pCircuit circuit, char corner, int *nbsol)
{
  chain_list *cl=NULL, *ch;
  ptype_list *pt;
  int cnt;
  cnt=getsoluce(bdd, circuit, NULL, &cl, corner);
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      complete_soluce(bdd, circuit, NULL, (ptype_list **)&ch->DATA);
      /*      printf("*");
              for (pt=(ptype_list *)ch->DATA; pt!=NULL; pt=pt->NEXT)
              printf("%5s:%ld ", (char *)pt->DATA, pt->TYPE);
              printf("\n");*/
    }
  avt_log(LOGGSP,2,"-- %d solutions found\n", cnt) ;
  if (nbsol!=NULL) *nbsol=cnt;
  if (cl==NULL) return NULL;
  pt=(ptype_list *)cl->DATA;
  freechain(cl);
  return pt;
}


ptype_list *gsp_SpiceFindInput (ptype_list *ptype,pNode bdd,pCircuit circuit,char corner,int *flag, int *nbsol)
{
  char       *name;
  ptype_list *ptptype=NULL;
  int already_set = 0;

  if (nbsol!=NULL) *nbsol=0;

  if (equalBdd(bdd,BDD_one) == 1) {
    /* les entrees du subckt peuvent prendre */
    /* toutes les valeurs possibles.         */
    ptype = addptype(ptype,GSP_STUCK_INDPD,NULL);
    if (nbsol!=NULL) *nbsol=1;
    return ptype;
  }
  else if (equalBdd(bdd,BDD_zero) == 1) { // cas rajoute 27/10/2005, est-ce la bonne action?
    return ptype;
  }

  if (circuit->countI<=V_INT_TAB[__CPE_MAX_VARIABLES].VALUE)
    return gsp_SpiceFindInput_allsol(bdd, circuit, corner,nbsol);
  
  name = searchIndexCct (circuit,bdd->index) ;
  // verif if deja stucke
  for ( ptptype = ptype ; ptptype ; ptptype=ptptype->NEXT ) {
    if ( ptptype->DATA == name ) {
      already_set = 1;
      break;
    }
  }

  if ( !already_set ) {
    if ( bdd->high == BDD_one) {
      if ( corner == 'w' ) {
        if ( !(*flag) )
          ptype = addptype(ptype,GSP_STUCK_ONE,name);
        else
          ptype = addptype(ptype,GSP_STUCK_ZERO,name);
      }
      else
        ptype = addptype(ptype,GSP_STUCK_ONE,name);
      *flag = 1;
    }
    else if ( bdd->low == BDD_one ) {
      if ( corner == 'w' ) {
        if ( !(*flag) )
          ptype = addptype(ptype,GSP_STUCK_ZERO,name);
        else
          ptype = addptype(ptype,GSP_STUCK_ONE,name);
      }
      else
        ptype = addptype(ptype,GSP_STUCK_ZERO,name);
      *flag = 1;
    }
    else {
      if ( bdd->high != BDD_zero) {
        if ( corner == 'w' ) {
          if ( !(*flag) )
            ptype = addptype(ptype,GSP_STUCK_ONE,name);
          else
            ptype = addptype(ptype,GSP_STUCK_ZERO,name);
        }
        else
          ptype = addptype(ptype,GSP_STUCK_ONE,name);
      }
      else if ( bdd->low != BDD_zero) {
        if ( corner == 'w' ) {
          if ( !(*flag) )
            ptype = addptype(ptype,GSP_STUCK_ZERO,name);
          else
            ptype = addptype(ptype,GSP_STUCK_ONE,name);
        }
        else 
          ptype = addptype(ptype,GSP_STUCK_ZERO,name);
      }
    }
    if ((equalBdd(bdd->high,BDD_one) != 1) && (equalBdd(bdd->high,BDD_zero) != 1)) 
      ptype = gsp_SpiceFindInput(ptype,bdd->high,circuit,corner,flag,NULL) ;
    if ((equalBdd(bdd->low,BDD_one) != 1) && (equalBdd(bdd->low,BDD_zero) != 1)) 
      ptype = gsp_SpiceFindInput(ptype,bdd->low,circuit,corner,flag,NULL) ;
  }
  else {
    if ( ptptype->TYPE == GSP_STUCK_ONE ) {
      if ((equalBdd(bdd->high,BDD_one) != 1) && (equalBdd(bdd->high,BDD_zero) != 1)) 
        ptype = gsp_SpiceFindInput(ptype,bdd->high,circuit,corner,flag,NULL) ;
    }
    else {
      if ((equalBdd(bdd->low,BDD_one) != 1) && (equalBdd(bdd->low,BDD_zero) != 1)) 
        ptype = gsp_SpiceFindInput(ptype,bdd->low,circuit,corner,flag,NULL) ;
    }
  }
  if (nbsol!=NULL) *nbsol=1;
  return ptype;
}

/*****************************************************************************\
  Rajoute des connecteurs a l interface du circuit pour appliquer
  les contraintes, s'ils n etaient pas initialement present
\*****************************************************************************/
void gsp_setextconnector (lofig_list *figext,ptype_list *setinput)
{
  ptype_list *ptype,*lofigchain;
  chain_list *chainlocon;
  locon_list *con,*locon,*newlocon;
  losig_list *losig;
  char       *conname;

  if (!figext || !setinput) return;
  for (ptype = setinput ; ptype ; ptype=ptype->NEXT) {
    conname = (char*)ptype->DATA;
    for (locon = figext->LOCON ; locon ; locon=locon->NEXT) {
      if (locon->NAME == conname)
        break;
    }
    if (!locon) {
      for (losig = figext->LOSIG ; losig ; losig=losig->NEXT) {
        if (getsigname (losig) == conname) {
          lofigchain  = getptype (losig->USER,LOFIGCHAIN);
          chainlocon = (chain_list *)lofigchain->DATA;
          con = (locon_list *)chainlocon->DATA;
          newlocon = addlocon (figext,conname,losig,'I');
          newlocon->TYPE = 'E';
          newlocon->SIG->TYPE = 'E';
          newlocon->PNODE = (num_list*)dupchainlst((chain_list*)con->PNODE);
          avt_log(LOGGSP,2,"[GSP MES] ADDing Extra connector %s\n",getsigname (con->SIG));
        }
      }
    }
  }
}

/*****************************************************************************\
 Func : gsp_update_cstname
\*****************************************************************************/
void gsp_update_cstname (lofig_list *fig, ptype_list *cstlist)
{
  ptype_list *ptype;
  losig_list *losig;
  losig_list *destsig;
  char *cstname;

  for ( ptype = cstlist ; ptype ; ptype = ptype->NEXT ) {
    cstname = (char*)ptype->DATA;
    if ( (losig = mbk_getlosigbyname( fig, cstname )) ) {
      destsig = efg_get_ext_sig (losig);
      if ( destsig )
        ptype->DATA = (void*)getsigname(destsig);
    }
  }
}

static chain_list *gsp_reordercone(chain_list *cl)
{
  chain_list *newcl=NULL, *addedcl=NULL;
  gsp_cone_info *cone_ci;

  while (cl!=NULL)
    {
      cone_ci=gsp_get_cone_info((cone_list *)cl->DATA);
      if (cone_ci->Date==-1)
        addedcl=addchain(addedcl, cl->DATA);
      else
        newcl=addchain(newcl, cl->DATA);
      cl=cl->NEXT;
    }
  return append(newcl, addedcl);
}

/*****************************************************************************/
/*                        function gsp_spisetinputs_from_cone()              */
/* set the remaining inputs of the extracted SPICE netlist                   */
/* return a ptype list which contains the name of the input & its value      */
/*****************************************************************************/
static void update_stuck_states(ptype_list *lst, chain_list *icsup, chain_list *fcsup)
{
  chain_list *cl;
  ptype_list *pt;
  int i;
  chain_list * pn[2]={icsup, fcsup};
  
  for (i=0; i<2; i++)
    {
      cl=pn[i];
      while (cl!=NULL)
        {
          for (pt=lst; pt!=NULL && pt->DATA!=cl->DATA; pt=pt->NEXT) ;
          if (pt!=NULL)
            pt->TYPE|=1<<i;

          cl=cl->NEXT;
        }
    }
}

static ptype_list *gsp_remove_drivencone_from_solution(ptype_list *sol)
{
  ptype_list *newpt=NULL, *pt;
  chain_list *cl;
  cone_list *cn;
  
  for (pt=sol; pt!=NULL; pt=pt->NEXT)
  {
    for (cl=EFGCONTEXT->ALLADDEDCONES; cl!=NULL; cl=cl->NEXT)
      if (((cone_list *)cl->DATA)->NAME==pt->DATA) break;
    if (cl==NULL)
      newpt=addptype(newpt, pt->TYPE, pt->DATA);
    else
      avt_log(LOGGSP,2," removing driven cone '%s' from solution\n", (char *)pt->DATA) ;

  }
  freeptype(sol);
  
  if (sol!=NULL && newpt==NULL)
    newpt=addptype(NULL,GSP_STUCK_INDPD,NULL);
  
  return newpt;
}

ptype_list *gsp_spisetinputs_from_cone(char *figname,chain_list *ptconelist,char corner, chain_list **allcorrel)
{
  chain_list *ptchain;
  chain_list *expr, *tmpatom, *tmpabl;
  chain_list *bddlist = NULL;
  pNode       globaltransfer, bres;
  chain_list *icabl, *fcabl;
  pCircuit    ptcct;
  ptype_list *setinput = NULL, *forcedsignals=NULL, *stuckstates=NULL;
  int flag, mode, fixed;
  cone_list *cone;
  losig_list *outlosig;
  gsp_cone_info *cone_ci;
  ptype_list *pt, *foundstates=NULL;

  *allcorrel=NULL;

  initializeBdd(0);

  globaltransfer = BDD_one;

  ptcct = gsp_buildspicecct(ptconelist);

  // compute initial state for out of path cones
  for (ptchain = ptconelist; ptchain; ptchain = ptchain->NEXT) 
    {
      cone=(cone_list *)ptchain->DATA;
      cone_ci=gsp_get_cone_info(cone);

      outlosig = efg_getlosigcone(cone);
      if (GSP_LOOP_MODE && (cone->TYPE & CNS_RS)!=0 && getptype(outlosig->USER,EFG_SIG_END)!=NULL)
        mode=GSP_FORCE_COMPUTE_INITIAL_STATE;
      else if (cone_ci->Date==-1)
        mode=GSP_FORCE_COMPUTE_INITIAL_STATE;
      else
        mode=GSP_COMPUTE_INITIAL_STATE;
      gsp_calcconetransfer(figname,(cone_list *)ptchain->DATA,corner,mode,0,NULL,NULL,allcorrel);
      if (cone_ci->Date==-1)
        {
          long cond=0;
          if (cone_ci->BeforeTransitionState!=-1) cond|=0x2 | cone_ci->BeforeTransitionState;
          if (cone_ci->AfterTransitionState!=-1) cond|=(0x2 | cone_ci->AfterTransitionState)<<4;
          stuckstates=addptype(stuckstates, 0, cone->NAME);
          foundstates=addptype(foundstates, cond, cone->NAME);
        }
    }

  /* for each cone calculate the condition for traversal */
  for (ptchain = ptconelist; ptchain; ptchain = ptchain->NEXT) 
    {
      cone=(cone_list *)ptchain->DATA;
      fixed=0;
      if (forcedsignals==NULL)
        {
          cone_ci=gsp_get_cone_info(cone);
          if (cone_ci->Date==-1)
            {
              if (bddlist != NULL)
                {
                  if (bddlist->NEXT != NULL)
                    globaltransfer = applyBdd(AND, bddlist);
                  else
                    globaltransfer = (pNode)bddlist->DATA;
                  if (equalBdd(globaltransfer,BDD_zero) != 1)
                    {
                      flag=0;
                      forcedsignals = gsp_SpiceFindInput(setinput,globaltransfer,ptcct,corner,&flag,NULL) ;
                    }
                }
            }            
        }
      if (forcedsignals!=NULL)
        {
          for (pt=forcedsignals; pt!=NULL && pt->DATA!=cone->NAME; pt=pt->NEXT) ;
          if (pt!=NULL)
            {
              int nfixed=0;
              if (pt->TYPE==GSP_STUCK_ONE) fixed=1;
              else if (pt->TYPE==GSP_STUCK_ZERO) fixed=0;
              for (pt=stuckstates; pt!=NULL && pt->DATA!=cone->NAME; pt=pt->NEXT) ;
              if (pt->TYPE & 1) nfixed|=0x2 | fixed;
              if (pt->TYPE & 2) nfixed|=(0x2 | fixed)<<4;
              fixed=nfixed;
            }
        }
      for (pt=foundstates; pt!=NULL && pt->DATA!=cone->NAME; pt=pt->NEXT) ;
      if (pt!=NULL) fixed|=pt->TYPE;

      if (!(expr = gsp_calcconetransfer(figname,cone,corner,0,fixed,&icabl,&fcabl,allcorrel))) 
        {
          if (!GSP_QUIET_MODE)
            {
              avt_errmsg(GSP_ERRMSG, "004", AVT_WARNING, cone->NAME);
            }
          if (bddlist != NULL) 
            {
              freechain (bddlist);
              bddlist = NULL;
            }
          break;
        }
      update_stuck_states(stuckstates, icabl, fcabl);
      freechain(icabl);
      freechain(fcabl);
      if (!GSP_QUIET_MODE)
        {
          if (equalExpr (expr,tmpatom=createAtom("'0'")) == 1)
            avt_errmsg(GSP_ERRMSG, "004", AVT_WARNING, cone->NAME);
          freeExpr(tmpatom);
        }

      if (( avt_islog(1,LOGGSP) ) && (expr != NULL)) 
        {
          avt_log(LOGGSP,1,"--> Local transfert condition for Cone %s : ", cone->NAME) ;
          displayExprLog(LOGGSP,1,expr);
        }
      setBddCeiling(10000);
      bres=ablToBddCct(ptcct, expr);
      if (!bddSystemAbandoned())
        {
          bddlist = addchain(bddlist, bres);
        }
      else
        {
          if (!GSP_QUIET_MODE)
            avt_log(LOGGSP,0,"ERROR: BDD system overflow, local transfert condition not taken into account\n") ;
          else
            avt_log(LOGGSP,1,"ERROR: BDD system overflow, local transfert condition not taken into account\n") ;
        }
      unsetBddCeiling();
      freeExpr(expr);
    }

  freeptype(forcedsignals);
  freeptype(foundstates);
  freeptype(stuckstates);

  /* calculate the global condition for path traversal */
  if (bddlist != NULL) {
    if (bddlist->NEXT != NULL) {
      globaltransfer = applyBdd(AND, bddlist);
    }
    else globaltransfer = (pNode)bddlist->DATA;
    freechain (bddlist);
  }
  else globaltransfer = BDD_zero;

  if ( avt_islog(1,LOGGSP) ) {
    avt_log(LOGGSP,2,"\n") ;
    avt_log(LOGGSP,2,"-------------------------------------------\n") ;
    avt_log(LOGGSP,1,"Global Path Condition: ");
    displayExprLog(LOGGSP,1,tmpabl=bddToAblCct(ptcct, globaltransfer));
    freeExpr(tmpabl);
    avt_log(LOGGSP,1,"-------------------------------------------\n") ;
  }

  GSP_FOUND_SOLUTION_NUMBER=0;
  if (equalBdd(globaltransfer,BDD_zero) != 1) {
    flag=0;
    setinput = gsp_SpiceFindInput(setinput,globaltransfer,ptcct,corner,&flag,&GSP_FOUND_SOLUTION_NUMBER) ;
  }
  else
  {
    if (!GSP_QUIET_MODE)
      avt_errmsg(GSP_ERRMSG, "005", AVT_WARNING);                
    gsp_freecorrel(*allcorrel);
    *allcorrel=NULL;
  }
  destroyCct(ptcct) ;

  destroyBdd(1);

  return setinput ;
}

/*****************************************************************************/
/*                        function gsp_con_is_constraint                     */
/*                                                                           */
/*****************************************************************************/
int gsp_con_is_constraint (ptype_list *constraints,char *namecon)
{
  char res = -1;
  ptype_list *ptype;

  if (gsp_is_pat_indpd (constraints) == 'N')
    for (ptype = constraints ; ptype ; ptype=ptype->NEXT) {
      if (ptype->TYPE == GSP_STUCK_INDPD) continue;
      if ((char*)ptype->DATA == namecon) {
        if (ptype->TYPE == GSP_STUCK_ONE)
          res = 1;
        else
          res = 0;
        break;
      }
    }
  return res;
}

/*****************************************************************************/
/*                        function gsp_fix_unset_input2zero                  */
/*                                                                           */
/* Obtient la liste des entrees non positionnes qui seront collees a zero    */
/* pour la simulation.                                                       */
/*                                                                           */
/*****************************************************************************/
chain_list *gsp_fix_unset_input2zero (lofig_list *figext,ptype_list **constraints,cnsfig_list *cnsfig)
{
  locon_list *locon,*con;
  chain_list *chain;
  ptype_list *ptype;
  ptype_list *linktype;
  lotrs_list *lotrs;
  lotrs_list *origlotrs;
  int ok, nogrid;
  cone_list *cone;
  char *orgname="?";
  chain_list *unsetvar=NULL;
  losig_list *osig;

  if (!figext || !(*constraints)) return NULL;
  for (locon = figext->LOCON ; locon ; locon=locon->NEXT) {
    ok = 2;
    if (getptype (locon->SIG->USER,EFG_SIG_SET) != NULL) continue;
//    if ((orgname=efg_get_origsigname(locon->SIG))==NULL) orgname=getsigname(locon->SIG);
    osig=efg_get_org_sig(locon->SIG);
    if ((cone=efg_getcone(osig))!=NULL/* || (cone=getcone(cnsfig, 0, orgname))!=NULL*/)
      {
        if((cone->TECTYPE & CNS_ZERO)==CNS_ZERO)
          {
            *constraints = addptype(*constraints, GSP_STUCK_ZERO, locon->NAME);
            avt_log(LOGGSP,2, "gsp_fix_unset_input2zero : stuck cone found, %s is stuck to 0\n", cone->NAME/*locon->NAME*/);
            continue;
          }
        else
          if((cone->TECTYPE & CNS_ONE)==CNS_ONE)
            {
              *constraints = addptype(*constraints, GSP_STUCK_ONE, locon->NAME);
              avt_log(LOGGSP,2, "gsp_fix_unset_input2zero : stuck cone found, %s is stuck to 1\n", cone->NAME/*locon->NAME*/);
              continue;
            }
      }
    orgname=getsigname(osig);
    if ((locon->DIRECTION == 'I') && 
        (gsp_con_is_constraint (*constraints,locon->NAME) < 0)) {
      ptype = getptype ( locon->SIG->USER,LOFIGCHAIN );
      for ( chain = (chain_list *)ptype->DATA ; chain ; chain=chain->NEXT ) {
        con = (locon_list*)chain->DATA;
        if ( con->TYPE == 'T' ) 
        {
          lotrs = (lotrs_list*)con->ROOT;
          if ((lotrs->FLAGS != EFG_BLOCK_TRS) && (lotrs->GRID == con) ) break;
        }
      }
      nogrid=(chain==NULL);

      for ( chain = (chain_list *)ptype->DATA ; chain ; chain=chain->NEXT ) {
        con = (locon_list*)chain->DATA;
        if ( con->TYPE == 'T' ) {
          lotrs = (lotrs_list*)con->ROOT;
          if ((lotrs->FLAGS != EFG_BLOCK_TRS) && 
              (lotrs->DRAIN == con || lotrs->SOURCE == con) ) {
            ok = 0;
            break;
          }
          // added to threat transistor as resistor...
          origlotrs = efg_get_org_trs ( lotrs );
          if ( origlotrs )
            // test if the link is resistive...
            if ( (linktype = getptype ( origlotrs->USER, CNS_LINKTYPE))) {
              if ( ((long)linktype->DATA & CNS_RESIST) == CNS_RESIST ) {
                if ( MLO_IS_TRANSN (origlotrs->TYPE) ) {
                  *constraints = addptype(*constraints,
                                          GSP_STUCK_ONE,
                                          locon->NAME);
                  avt_log(LOGGSP,2,
                          "gsp_fix_unset_input2zero : Resist branch found, %s is stuck to 1 \n",
                          orgname/*locon->NAME*/);
                }
                else {
                  *constraints = addptype(*constraints,
                                          GSP_STUCK_ZERO,
                                          locon->NAME);
                  avt_log(LOGGSP,2,
                          "gsp_fix_unset_input2zero : Resist branch found, %s is stuck to 0 \n",
                          orgname/*locon->NAME*/);
                }
                ok = 0;
                break;
              }
            }
        }
      }
      if ( ok )
        unsetvar=addchain(unsetvar, locon->SIG);
      else if (nogrid)
      {
        *constraints = addptype(*constraints, GSP_STUCK_ZERO, locon->NAME);
        avt_log(LOGGSP,2,
                "gsp_fix_unset_input2zero : input connector '%s' is arbitrarily set to 0 \n",
                orgname/*locon->NAME*/);
      }
    }
  }
  return unsetvar;
}

/*****************************************************************************/
/*                        function gsp_get_switch_cst                        */
/*                                                                           */
/* Obtient les contraintes des switchs.                                      */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_get_switch_cst (ptype_list *globalcst, chain_list *chaincone)
{
  ptype_list *ptype;
  chain_list *chain;
  cone_list  *cone,
    *opposite_switch_cmd;

  if (!chaincone) return NULL;

  for (chain = chaincone ; chain ; chain=chain->NEXT) {
    cone = ((edge_list*)chain->DATA)->UEDGE.CONE;
    ptype = getptype (((edge_list*)chain->DATA)->USER,CNS_SWITCHPAIR);
    opposite_switch_cmd = (cone_list*)ptype->DATA;

    if ( ((cone->TYPE & CNS_VDD) == CNS_VDD) && 
         ((opposite_switch_cmd->TYPE & CNS_VDD) != CNS_VDD) && 
         ((opposite_switch_cmd->TYPE & CNS_VSS) != CNS_VSS) && 
         (gsp_con_is_constraint (globalcst,opposite_switch_cmd->NAME) < 0 ))
      globalcst = gsp_FixSigCst (globalcst,opposite_switch_cmd->NAME,0);
    if ( ((cone->TYPE & CNS_VSS) == CNS_VSS) && 
         ((opposite_switch_cmd->TYPE & CNS_VDD) != CNS_VDD) && 
         ((opposite_switch_cmd->TYPE & CNS_VSS) != CNS_VSS) && 
         (gsp_con_is_constraint (globalcst,opposite_switch_cmd->NAME) < 0 ))
      globalcst = gsp_FixSigCst (globalcst,opposite_switch_cmd->NAME,1);
    else {
      switch ( gsp_con_is_constraint (globalcst,cone->NAME) ) {
      case 0 : if ((gsp_con_is_constraint (globalcst,opposite_switch_cmd->NAME) < 0) &&
                   ((opposite_switch_cmd->TYPE & CNS_VDD) != CNS_VDD) && 
                   ((opposite_switch_cmd->TYPE & CNS_VSS) != CNS_VSS))
        globalcst = gsp_FixSigCst (globalcst,
                                   opposite_switch_cmd->NAME,
                                   1);
        break;
      case 1 : if ((gsp_con_is_constraint (globalcst,opposite_switch_cmd->NAME) < 0) &&
                   ((opposite_switch_cmd->TYPE & CNS_VDD) != CNS_VDD) && 
                   ((opposite_switch_cmd->TYPE & CNS_VSS) != CNS_VSS))
        globalcst = gsp_FixSigCst (globalcst,
                                   opposite_switch_cmd->NAME,
                                   0);
        break;
      default :
        if ((((opposite_switch_cmd->TYPE & CNS_VDD) == CNS_VDD) &&
             (gsp_con_is_constraint (globalcst,cone->NAME) < 0 )) &&
            ((cone->TYPE & CNS_VDD) != CNS_VDD) && 
            ((cone->TYPE & CNS_VSS) != CNS_VSS))
          globalcst = gsp_FixSigCst (globalcst,cone->NAME,0);
        else if ((((opposite_switch_cmd->TYPE & CNS_VSS) == CNS_VSS) &&
                  (gsp_con_is_constraint(globalcst,cone->NAME) < 0)) &&
                 ((cone->TYPE & CNS_VDD) != CNS_VDD) && 
                 ((cone->TYPE & CNS_VSS) != CNS_VSS))
          globalcst = gsp_FixSigCst (globalcst,cone->NAME,1);
        else {
          switch ( gsp_con_is_constraint (globalcst,opposite_switch_cmd->NAME) ) {
          case 0 : if ((gsp_con_is_constraint (globalcst,cone->NAME) < 0) &&
                       ((cone->TYPE & CNS_VDD) != CNS_VDD) && 
                       ((cone->TYPE & CNS_VSS) != CNS_VSS))
            globalcst = gsp_FixSigCst (globalcst,
                                       cone->NAME,
                                       1);
            break;
          case 1 :  if ((gsp_con_is_constraint (globalcst,cone->NAME) < 0) &&
                        ((cone->TYPE & CNS_VDD) != CNS_VDD) && 
                        ((cone->TYPE & CNS_VSS) != CNS_VSS))
            globalcst = gsp_FixSigCst (globalcst,
                                       cone->NAME,
                                       0);
            break;
          default: globalcst = gsp_FixSigCst (globalcst,
                                              cone->NAME,
                                              0);
            globalcst = gsp_FixSigCst (globalcst,
                                       opposite_switch_cmd->NAME,
                                       0);
            break;
          }
        }
        break;
      }
    }

  }
  return globalcst;
}

/*****************************************************************************/
/*                        function gsp_get_patterns()                        */
/*                                                                           */
/* Obtient les patterns pour la simulation ainsi que la figure extraite      */
/*                                                                           */
/*****************************************************************************/
chain_list *gsp_build_initial_condition_pattern_hash(ptype_list *pt, chain_list *cone_lst, spisig_list *spisig)
{
  chain_list *h;
  gsp_cone_info *cone_ci;
  cone_list *ptcone;
  chain_list *abl;

  h=NULL;
  
  while (pt!=NULL)
    {
      if (pt->TYPE!=GSP_STUCK_INDPD)
        {
          abl=createAtom(pt->DATA);
          if (pt->TYPE==GSP_STUCK_ZERO) abl=notExpr(abl);
          h=addchain(h, abl);
        }
      pt=pt->NEXT;
    }
  while (cone_lst!=NULL)
    {
      ptcone=(cone_list *)cone_lst->DATA;
      if ((cone_ci=gsp_get_cone_info(ptcone))!=NULL)
	{
	  if (cone_ci->used_ic!=-1) 
            {
              abl=createAtom(ptcone->NAME);
              if (cone_ci->used_ic==0) abl=notExpr(abl);
              h=addchain(h, abl);
            }
	}
      cone_lst=cone_lst->NEXT;
    }
  while (spisig!=NULL)
    {
      if (spisig->START)
	{
	  if (getchain(h, spisig->NAME)==NULL)
            {
              abl=createAtom(spisig->NAME);
              if (spisig->EVENT==(long)EFG_SIG_SET_RISE) abl=notExpr(abl);
              h=addchain(h, abl);
            }
	}
      spisig=spisig->NEXT;
    }
  return h;
}

static char *gsp_get_original_sig_name(losig_list *ls)
{
  char *orgname;
  if ((orgname=efg_get_origsigname(ls))==NULL) orgname=getsigname(ls);
  return orgname;
}
ptype_list *gsp_compute_unset_inputs(chain_list *ic_list, chain_list *cone_lst, chain_list **unset_var, char corner)
{
  ptype_list *ptuser1, *res, *setinput;
  chain_list *abl, *cl;
  pCircuit    ptcct;
  pNode       mainbdd, subbdd;
  int flag;
  cone_list *ptcone;
  gsp_cone_info *cone_ci;

  res=NULL;

  if (*unset_var!=NULL)
    {
      avt_log(LOGGSP,2,"----- checking unset variable states\n") ;
      initializeBdd(0);

      ptcct = gsp_buildspicecct(cone_lst);
      while (cone_lst!=NULL && *unset_var!=NULL)
        {
          ptcone=(cone_list *)cone_lst->DATA;
          if ((cone_ci=gsp_get_cone_info(ptcone))!=NULL)
            {
              if (cone_ci->used_ic!=-1) 
                {
                  ptuser1 = getptype(ptcone->USER, cone_ci->used_ic==1?CNS_UPEXPR:CNS_DNEXPR);
                  if (!ptuser1->DATA)
                    abl = createAtom("'0'");
                  else 
                    abl = copyExpr((chain_list *)ptuser1->DATA);

                  mainbdd = ablToBddCct(ptcct, abl);
                  for (cl=ic_list; cl!=NULL; cl=cl->NEXT)
                    {
                      subbdd=ablToBddCct(ptcct, (chain_list *)cl->DATA);
                      mainbdd=constraintBdd(mainbdd, subbdd);
                    }

                  freeExpr(abl);
                  flag=0;
                  setinput = gsp_SpiceFindInput(NULL,mainbdd,ptcct,corner,&flag,NULL) ;
		   
                  if ( avt_islog(3,LOGGSP ) ) 
                    {
                      avt_log(LOGGSP,3,"-- cone '%s' expr after pattern: ", ptcone->NAME) ;
                      displayExprLog(LOGGSP,3,abl=bddToAblCct(ptcct, mainbdd));
                      freeExpr(abl);
                    }
                  for (ptuser1=setinput; ptuser1!=NULL; ptuser1=ptuser1->NEXT)
                    {
                      for (cl=*unset_var; cl!=NULL && gsp_get_original_sig_name((losig_list *)cl->DATA)!=ptuser1->DATA; cl=cl->NEXT) ;
                      if (cl!=NULL)
                        {
                          res=addptype(res, ptuser1->TYPE, ptuser1->DATA);
                          *unset_var=delchain(*unset_var, cl);
                          avt_log(LOGGSP,2,"-- unset variable '%s' set to %d to ensure cone '%s' initial contition\n", ptuser1->DATA, ptuser1->TYPE==GSP_STUCK_ZERO?0:1,ptcone->NAME) ;
                        }
                    }
                  freeptype(setinput);
                }
            }
          cone_lst=cone_lst->NEXT;
        }
      destroyCct(ptcct) ;

      destroyBdd(1);
    }
  return res;
}
				
static void gsp_checkcorrel(chain_list **unsetvar, chain_list **allcorrel)
{
  chain_list *keptu=NULL, *cl, *ch, *keptc=NULL, *cl1;
  gsp_correl *gc;
  char *name, *orig;
  spisig_list *spisig;
  int kept;

  for (cl=*unsetvar; cl!=NULL; cl=cl->NEXT)
  {
    name=getsigname((losig_list *)cl->DATA);
    for (ch=*allcorrel; ch!=NULL; ch=ch->NEXT)
    {
      gc=(gsp_correl *)ch->DATA;
      if (gc->dest==name) {orig=gc->orig;break;}
      else if (gc->orig==name) {orig=gc->dest;break;}
    }
    kept=0;
    if (ch!=NULL)
    {
       if ((spisig = efg_GetSpiSig(EFGCONTEXT->SPISIG, orig))!=NULL)
       {
          for (cl1=EFGCONTEXT->ALLADDEDCONES; cl1!=NULL && ((cone_list *)cl1->DATA)->NAME!=name; cl1=cl1->NEXT) ;
          if (cl1==NULL)
          {
             keptc=gsp_addcorrel(keptc, cl->DATA, spisig->DESTSIG, gc->tog);
             avt_log(LOGGSP,2,"-- adding unset variable '%s' = %s%s%s\n", name, gc->tog?"not(":"\'", orig, gc->tog?")":"\'") ;
             kept=1;
          }
       }
    }
    if (!kept)
      keptu=addchain(keptu, cl->DATA);
  }
  gsp_freecorrel(*allcorrel);
  *allcorrel=keptc;
  freechain(*unsetvar);
  *unsetvar=keptu;
}

ptype_list *gsp_get_patterns(lofig_list *lofig,cnsfig_list *cnsfig,lofig_list *figext,chain_list *chaincone,chain_list *chaininstance, char corner, spisig_list *spisig, chain_list **allcorrel)
{
  loins_list *loins;
  ptype_list *cone_pat = NULL,
    *ins_pat = NULL,
    *globalpat = NULL,
    *spipat = NULL;
  chain_list *chain, *cl;
  char       *env;
  spisig_list *spisignal;
     
  *allcorrel=NULL;
  if (!lofig || !figext) 
    return NULL;

  if (chaincone != NULL) {
    chain_list *ochain;
    gsp_new_numbercones (lofig->NAME,chaincone);
    ochain=gsp_reordercone(chaincone);
    cone_pat = gsp_spisetinputs_from_cone ( lofig->NAME, ochain,corner,allcorrel );
    freechain(chaincone);
    chaincone=ochain;
  }

  if (EFGCONTEXT->DESTFIG)
    cone_pat=gsp_remove_drivencone_from_solution(cone_pat);

  if (chaininstance != NULL) {
    for (chain = chaininstance ; chain ; chain=chain->NEXT) {
      loins = (loins_list*)chain->DATA;
      spipat = gsp_spisetinputs_from_loins (loins);
      ins_pat = gsp_MergeCst (ins_pat,spipat);
      if (spipat != CBH_GOOD_TRANS)
        freeptype (spipat);
    }
    env = getenv("EFG_CALC_EQUI_CAPA") ;
    if ((env != NULL) && (strcmp(env,"yes") == 0))
      ins_pat = gsp_merge_all_hz_cst (figext,ins_pat);
    freechain (chaininstance);
  }
  //globalpat = gsp_ModifCstLst (globalpat,usrlstcst);
  globalpat = gsp_MergeCst (cone_pat,ins_pat);

  cl=gsp_build_initial_condition_pattern_hash(globalpat, chaincone, spisig);
	
  if ( V_STR_TAB[__EFG_SIG_ALIAS].VALUE )
    gsp_update_cstname ( lofig,globalpat);
    
  chain=gsp_fix_unset_input2zero (figext,&globalpat,cnsfig);
  gsp_checkcorrel(&chain, allcorrel);
  spipat=gsp_compute_unset_inputs(cl, chaincone, &chain, corner);

  if ( V_STR_TAB[__EFG_SIG_ALIAS].VALUE )
    gsp_update_cstname ( lofig,spipat);
  globalpat=(ptype_list *)append((chain_list *)globalpat, (chain_list *)spipat);

  while (cl!=NULL)
    {
      freeExpr((chain_list *)cl->DATA);
      cl=delchain(cl,cl);
    }
  for (cl=chain; cl!=NULL; cl=cl->NEXT)
    {
      globalpat=addptype(globalpat, GSP_STUCK_ZERO, getsigname((losig_list *)cl->DATA));
      avt_log(LOGGSP,2,"-- unset variable '%s' set to %d by default\n", gsp_get_original_sig_name((losig_list *)cl->DATA), 0) ;
    }
  freechain(chain);

  for (spisignal=spisig; spisignal!=NULL; spisignal=spisignal->NEXT)
    {
      if (getptype(spisignal->SRCSIG->USER, EFG_FORCE_IC)!=NULL) spisignal->ADDIC=1;
    }

  /*    // =====> liberation des differents marquages
        efg_FreeMarksOnExtFig (figext);
        efg_FreeMarksOnFig (lofig);
        efg_freecontext ();
  */
  globalpat = gsp_verif_pat (cnsfig,figext,globalpat);
    
  if (cnsfig != NULL) gsp_FreeMarksOnCnsfig (cnsfig);

  if ( avt_islog(2,LOGGSP) ) 
    gsp_PrintCstLst(globalpat);
  freechain (chaincone);
  return globalpat;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_SigInInterface                                             */
/*                                                                           */
/* Renvoie 'Y' si le signal est a l interface du circuit                     */
/*                                                                           */
/*****************************************************************************/
char gsp_SigInInterface(signal)
     losig_list *signal ;
{
  if((getptype(signal->USER,EFG_SIG_DRV) == NULL) &&
     (getptype(signal->USER,EFG_SIG_ALIM) == NULL) &&
     (getptype(signal->USER,EFG_SIG_TERMINAL) == NULL))
    return 'Y' ;
  else  if((getptype(signal->USER,EFG_SIG_DRV) == NULL) &&
           (getptype(signal->USER,EFG_SIG_ALIM) == NULL) &&
           (getptype(signal->USER,EFG_SIG_CONE) != NULL) &&
           (getptype(signal->USER,EFG_SIG_TERMINAL) != NULL) &&
           (getptype(signal->USER,EFG_SIG_INITIAL) == NULL))
    return 'Y' ;
  else
    return 'N';
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_GetSupFromCone                                             */
/*                                                                           */
/* Recupere les expression de mise a un des cones (sup).                     */
/*                                                                           */
/* Renvoie l'abl en effectuant un 'ET' logique de tous les sup.              */
/*                                                                           */
/*****************************************************************************/
chain_list *gsp_GetSupFromCone(conelist)
     chain_list *conelist;
{
  chain_list *chain;
  chain_list *sup_abl;
  chain_list *bddchain = NULL;
  pNode       globalsup;
  cone_list  *ptcone;
  ptype_list *ptuser;
  pCircuit    cct;

  if (!conelist) {
    avt_errmsg(GSP_ERRMSG, "006", AVT_WARNING); 
    return NULL;
  }
  cct = gsp_buildexpconecct(NULL, conelist);
  for (chain = conelist ; chain ; chain = chain->NEXT) {
    ptcone = (cone_list*)chain->DATA;
    if (!ptcone) {
      avt_errmsg(GSP_ERRMSG, "007", AVT_WARNING); 
      return NULL;
    }        
    cnsConeFunction_once(ptcone,1);
    //        cnsConeFunction(ptcone,1);
    ptuser = getptype(ptcone->USER, CNS_UPEXPR);
    if (ptuser != NULL) {
      /* apply bin "and" */
      sup_abl = copyExpr((chain_list *)ptuser->DATA);
      bddchain = addchain(bddchain,ablToBddCct(cct, sup_abl)) ;
      freeExpr(sup_abl);
    }
    else {
      avt_errmsg(GSP_ERRMSG, "008", AVT_WARNING, ptcone->NAME); 
    }
  }
  if (bddchain != NULL) {
    if (bddchain->NEXT != NULL) {
      globalsup = applyBdd(AND, bddchain);
    }
    else globalsup = (pNode)bddchain->DATA;
  }
  if (equalBdd(globalsup,BDD_zero) != 1) {
    sup_abl = bddToAblCct(cct,globalsup);
    destroyCct(cct);
    freechain(bddchain);
  }
  else
    sup_abl = NULL;
    
  return sup_abl;
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_spisetinputs_from_loins()                                            */
/*                                                                           */
/* Set input in a Cell LEVEL                                                 */
/* set the remaining inputs of the extracted SPICE netlist                   */
/* The loins list is the lowest instance level.                              */
/* If we can't traverse an instance, then we stop the process.               */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_spisetinputs_from_loins(loins)
     loins_list *loins;
{
  ptype_list *othermarksig=NULL;  /*si +sieurs entrees connues sur une cell*/
  locon_list *loconin;
  locon_list *loconout;
  long        transition;
  long        inslope;
  long        outslope;
  ptype_list *localtransfert = NULL;
  ptype_list *localcst= NULL;

  loconin  = gsp_GetLoconIn(loins,othermarksig,&inslope) ;
  loconout = gsp_GetLoconOut(loins,&outslope) ;
  if ((loconin == NULL) || (loconout == NULL)) {
    avt_errmsg(GSP_ERRMSG, "009", AVT_WARNING, loins->INSNAME); 
    return NULL;
  }
  if (inslope == (long)EFG_SIG_SET_RISE) {
    if (outslope == (long)EFG_SIG_SET_RISE)
      transition = CBH_TRANS_UU ;
    else
      transition = CBH_TRANS_UD ;
  }
  else {
    if (outslope == (long)EFG_SIG_SET_RISE)
      transition = CBH_TRANS_DU ;
    else
      transition = CBH_TRANS_DD ;
  }
  localtransfert = cbh_confLoinsForTrans(loins,
                                         loconin,
                                         loconout,
                                         transition,
                                         othermarksig);
  localcst = gsp_BuildListCst(localtransfert);
  if (localtransfert != CBH_GOOD_TRANS)
    freeptype (localtransfert);
  return localcst;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_FillPtypelistCst()                                         */
/*                                                                           */
/* Retourne une liste de connecteur avec leur valeur a postionner            */
/* Exclusion de la contrainte sur loconin.                                   */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_FillPtypelistCst(headchain,loconin)
     chain_list *headchain;
     locon_list *loconin;
{
  locon_list *locon;
  chain_list *chain;
  ptype_list *ptype;
  ptype_list *resptype;
    
  for (chain = headchain ; chain ; chain = chain->NEXT) {
    locon = (locon_list*)chain->DATA;
    if (locon != loconin) {
      if ((ptype = getptype(locon->SIG->USER,EFG_SIG_SET)) != NULL) {
        if (ptype->DATA == EFG_SIG_SET_RISE)
          resptype = addptype (resptype,(long)CBH_CONSTRAINT_1,(void*)locon) ;
        else
          resptype = addptype (resptype,(long)CBH_CONSTRAINT_0,(void*)locon) ;
      }
    }
  }
  return resptype;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_PrintCstLst                                                */
/*                                                                           */
/*****************************************************************************/
void gsp_PrintCstLst(headptype)
     ptype_list *headptype;
{
  ptype_list *ptype;
  char       *name;

  if (!headptype) 
    return;
  avt_log(LOGGSP,2,"\n");
  avt_log(LOGGSP,2,"+-----------------------------+\n");
  avt_log(LOGGSP,2,"|  Constraints list computed  |\n");
  avt_log(LOGGSP,2,"+-----------------------------+\n");
  avt_log(LOGGSP,2,"    Value      |    Signal     \n");
  if (headptype == CBH_GOOD_TRANS) {
    avt_log(LOGGSP,2,"!!! Any configuration is good !!!\n");
    return ;
  }
  for (ptype = headptype ; ptype ; ptype = ptype->NEXT) {
    if (ptype->TYPE == GSP_STUCK_INDPD) continue;
    name = (char*)ptype->DATA;
    if (ptype->TYPE == GSP_STUCK_ONE)
      avt_log(LOGGSP,2,"     vdd       |");
    else
      avt_log(LOGGSP,2,"      0        |");
    avt_log(LOGGSP,2," %s\n",name);
  }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_BuildListCst()                                             */
/*                                                                           */
/* Construit une ptypelist contenant le nom d'un signal dans le champ DATA   */
/* et sa valeur a positionner dans le champ TYPE.                            */
/*                                                                           */
/* le nom du sig est complet : instance.signame a partir du top level.       */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_BuildListCst(ptype)
     ptype_list *ptype;
{
  ptype_list *ptuser;
  ptype_list *globalptype = NULL;       /*TYPE = 1 ou 0, DATA = nom du SIG */
  locon_list *locon;
  losig_list *losig;
  char       *name;

  if (ptype == CBH_GOOD_TRANS)
    return addptype (NULL,GSP_STUCK_INDPD,NULL);
  for (ptuser = ptype ; ptuser ; ptuser = ptuser->NEXT) {
    locon = (locon_list*)ptuser->DATA;
    losig = locon->SIG;
    /* nom du sig a positionner : nom_instance.nom_connecteu_cell */
    name = efg_revect(getsigname(locon->SIG));
    if (ptuser->TYPE == CBH_CONSTRAINT_0) {
      globalptype = addptype(globalptype,
                             GSP_STUCK_ZERO,
                             name);
    }
    else {
      globalptype = addptype(globalptype,
                             GSP_STUCK_ONE,
                             name);
    }
  }
  return globalptype;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_VerifGoodCstList                                           */
/*                                                                           */
/* Verifie la coherence des contraintes a appliquer.                         */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_VerifGoodCstList(globalptype)
     ptype_list *globalptype;
{
  ptype_list *ptype1,*ptype2;
  char *name1,*name2;
  int  flag = 1;                             /* bonne liste de contraintes */

  if ((globalptype != NULL) && (globalptype != CBH_GOOD_TRANS)) {
    for (ptype1 = globalptype ; ptype1 ; ptype1 = ptype1->NEXT) {
      name1 = (char*)ptype1->DATA;
      for (ptype2 = ptype1->NEXT ; ptype2 ; ptype2 = ptype2->NEXT) {
        name2 = (char*)ptype2->DATA;
        if ((name2 == name1) && (ptype1->TYPE != ptype2->TYPE)) {
          flag = 0;
          break;
        }
      }
      if (flag == 0) {
        globalptype = NULL;
        break;
      }
    }
  }
  return globalptype;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_MergeCst                                                   */
/*                                                                           */
/* Merge deux listes de contraintes                                          */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_MergeCst (globalpat,spipattern)
     ptype_list *globalpat;
     ptype_list *spipattern;
{
  ptype_list *ptype;

  if ((globalpat != CBH_GOOD_TRANS) && (spipattern != CBH_GOOD_TRANS)) {
    for (ptype = spipattern ; ptype ; ptype = ptype->NEXT) {
      globalpat = addptype (globalpat,ptype->TYPE,ptype->DATA);
    }
  }
  else if ((globalpat == CBH_GOOD_TRANS) && (spipattern != CBH_GOOD_TRANS)) {
    globalpat = NULL;
    for (ptype = spipattern ; ptype ; ptype = ptype->NEXT) {
      globalpat = addptype (globalpat,ptype->TYPE,ptype->DATA);
    }
  }
  else if ((globalpat == NULL) && (spipattern != NULL))
    globalpat = spipattern;
  else if ((globalpat != CBH_GOOD_TRANS) && (spipattern == CBH_GOOD_TRANS))
    spipattern = NULL;
  else 
    spipattern = NULL;
  return globalpat;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_FixSigCst                                                  */
/*                                                                           */
/* Permet de fixer manuellement une contrainte sur un signal                 */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_FixSigCst (head,signame,cst)
     ptype_list *head;
     char       *signame;
     int         cst;
{
  long        typecst;

  if (cst == 1)
    typecst = GSP_STUCK_ONE;
  else if (cst == 0)
    typecst = GSP_STUCK_ZERO;
  else
    return head;
  head = addptype (head,typecst,(void*)namealloc(signame));
  return head;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_ModifCstLst                                                */
/*                                                                           */
/* Modifie la list de contraintes originale par une liste de cst personnelle */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_ModifCstLst (origlstcst,usercst)
     ptype_list *origlstcst;
     ptype_list *usercst;
{
  ptype_list *ptype1,*ptype2;
  char       *username;
  long        userval;

  if (usercst == NULL)
    return origlstcst;
  for (ptype1 = usercst ; ptype1 ; ptype1 = ptype1->NEXT) {
    userval = ptype1->TYPE;
    username = (char*)ptype1->DATA;
    for (ptype2 = origlstcst ; ptype2 ; ptype2 = ptype2->NEXT) {
      if (username == (char*)ptype2->DATA) {
        ptype2->TYPE = userval;
        break;
      }
    }
  }
  return origlstcst;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_get_hz_cst                                                 */
/*                                                                           */
/* Recupere une liste de contraintes a positionner pour les cellules         */
/*                                                                           */
/* de type tri-state                                                         */
/*                                                                           */
/*****************************************************************************/
chain_list *gsp_get_hz_cst (loins_list* ins)
{
  locon_list *locon;
  ptype_list *hzfunc;
  chain_list *chaincst = NULL;

  for (locon = ins->LOCON ; locon ; locon=locon->NEXT) {
    if ((hzfunc = cbh_getCstToDisableLoins (ins,locon,NULL)) != NULL)
      chaincst = addchain (chaincst,gsp_BuildListCst(hzfunc));
  }
  return chaincst;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_get_ins_out_path                                           */
/*                                                                           */
/* Recupere une liste d'instances hors chemin.                               */
/*                                                                           */
/*****************************************************************************/
chain_list *gsp_get_ins_out_path (lofig_list *fig)
{
  chain_list *ins_out_path = NULL;
  loins_list *loins;

  for (loins = fig->LOINS ; loins ; loins=loins->NEXT) {
    if ((getptype(loins->USER,EFG_INS_OUTPATH)) != NULL)
      ins_out_path = addchain (ins_out_path,loins);
  }
  return ins_out_path;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_merge_all_hz_cst                                           */
/*                                                                           */
/* Merge les contraintes a positionner avec les contraintes hz               */
/*                                                                           */
/*****************************************************************************/
ptype_list *gsp_merge_all_hz_cst (lofig_list *fig,ptype_list *normal_cst)
{
  chain_list *ins_out_path,*ins;
  chain_list *hzcstlist,*hzcst;
  ptype_list *ptype_cst;

  ins_out_path = gsp_get_ins_out_path (fig);
  for (ins = ins_out_path ; ins ; ins=ins->NEXT) {
    hzcstlist = gsp_get_hz_cst ((loins_list*)ins->DATA);
    for (hzcst = hzcstlist ; hzcst ; hzcst=hzcst->NEXT) {
      for (ptype_cst = (ptype_list*)hzcst->DATA ;
           ptype_cst ; ptype_cst=ptype_cst->NEXT) {
        normal_cst = gsp_MergeCst (normal_cst,ptype_cst);
        freeptype (ptype_cst);
      }
      freechain (hzcst);
    }
    freechain (hzcstlist);
  }
  if (ins_out_path != NULL) 
    freechain (ins_out_path);
  return normal_cst;
}

void gsp_constraint_abl_with_switch( ptype_list *switchlist, chain_list *sup_abl, chain_list *sdn_abl )
{
  ptype_list *ptype ;
  char       *cmd ;
  char       *oppcmd ;
  chain_list *expr ;
  chain_list *notexpr ;

  for( ptype = switchlist ; ptype ; ptype = ptype->NEXT ) {
    cmd    = (char*)ptype->DATA ;
    oppcmd = (char*)ptype->TYPE ;

    expr = createAtom_no_NA( cmd );
    notexpr = notExpr( expr ) ;

    substPhyExpr( sup_abl, oppcmd, notexpr );
    substPhyExpr( sdn_abl, oppcmd, notexpr );

    freeExpr( notexpr );
  }
}
