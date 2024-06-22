#include <stdarg.h>

#include AVT_H
#include GEN_H
#include MSL_H
#define API_USE_REAL_TYPES
#include "gen_API_netlist.h"
#include "gen_API.h"

static mbkContext gen_api_ctx[32];
/*lofig_list *gen_api_head_lofig;
ht         *gen_api_ht_lofig;*/
static int recur=0;

void gen_API_Action_Initialize ()// commentaire pour desactiver l'ajout de token
{
  char *str;
  
  // Contextualize HEAD_LOFIG ___________________________
  
  gen_api_ctx[recur].HEAD_LOFIG=GEN_HEAD_LOFIG;
  gen_api_ctx[recur].HT_LOFIG=GEN_HT_LOFIG;
  
  mbkSwitchContext(&gen_api_ctx[recur]);
  
  recur++;
  /*
    gen_api_head_lofig = HEAD_LOFIG;
    HEAD_LOFIG = GEN_HEAD_LOFIG;
    
    gen_api_ht_lofig = HT_LOFIG;
    HT_LOFIG = GEN_HT_LOFIG;
  */
  if ((str = avt_gethashvar ("GEN_SEPAR")))
    GEN_SEPAR = str[0];
  else
    GEN_SEPAR = '.';
  
  // Global variables initialization
/*  if (TRANS_TO_SUPPRESS) {
    freechain (TRANS_TO_SUPPRESS);
    TRANS_TO_SUPPRESS = NULL;
  }*/
}

    // Release HEAD_LOFIG _________________________________

void gen_API_Action_Terminate ()// commentaire pour desactiver l'ajout de token
{
  recur--;
  if (recur<0) avt_fprintf(stderr,"¤6internal_error in gen_API¤.");

  mbkSwitchContext(&gen_api_ctx[recur]);
  /*
    GEN_HEAD_LOFIG = HEAD_LOFIG;
    HEAD_LOFIG = gen_api_head_lofig;
    
    GEN_HT_LOFIG = HT_LOFIG;
    HT_LOFIG = gen_api_ht_lofig;
  */
}
//_____________________________________________________________________________
//_____________________________________________________________________________

lofig_list *gns_GetNetlist ()
{
    gen_rcenv ();

    gen_update_loins (CUR_HIER_LOFIG, CUR_CORRESP_TABLE);
    gen_update_transistors (CUR_HIER_LOFIG, CUR_CORRESP_TABLE);
    return CUR_HIER_LOFIG;
}

lofig_list *gns_GetInstanceNetlist (char *name)
{
  CorrespondingInstance *xi;
  lofig_list *lf, *duplf;
  gen_rcenv ();
  xi=gns_GetCorrespondingInstance(name);
  if (xi==NULL) return NULL;
  lf=gen_getlofig(xi->CRT->GENIUS_FIGNAME);
  duplf=rduplofig(lf);
  lofigchain(duplf);
  gen_update_loins(duplf, xi->CRT);
  gen_update_transistors (duplf, xi->CRT);
  duplf->USER=addptype(duplf->USER, GEN_ORIG_INS_CORRESP_PTYPE, xi->CRT);
  return duplf;
}

lofig_list *gns_DuplicateNetlist (lofig_list *lf)
{
  lofig_list *duplf;
  duplf=rduplofig(lf);
  lofigchain(duplf);
  return duplf;
}
//_____________________________________________________________________________
//_____________________________________________________________________________

void gns_DumpCorrespTable ()
{
    FILE *f;
    char buf[1024];

    sprintf (buf, "%s.dic", CUR_HIER_LOFIG->NAME);
    if ((f = fopen (buf, "w+"))) gen_drive_corresp_htable (f, buf, NULL, 1, CUR_CORRESP_TABLE);
}

// Fonctions de correspondance et de nommage _____________________________________

//_____________________________________________________________________________
//_____________________________________________________________________________

void gns_ViewLo (Netlist *ptfig)
{
    if (!ptfig) return;

    viewlofig ((lofig_list*)ptfig);
}

//_____________________________________________________________________________

void gns_DriveNetlist (Netlist *ptfig, char *format, char *path, char *name)
{
    char prev_out_lo[5];
    char *prev_work_lib = WORK_LIB;
    char *oldname;

    if (!ptfig) return;

    if (name!=NULL)
      {
        oldname=ptfig->NAME;
        ptfig->NAME=namealloc(name);
      }

    strcpy (prev_out_lo, OUT_LO);
    WORK_LIB = path;
    
    if (!strcasecmp (format, "spice")) {
      strcpy (OUT_LO, "spi");
      savelofig ((lofig_list*)ptfig);
    } 
    else if (!strcasecmp (format, "vhdl")) {
      strcpy (OUT_LO, "vhd");
      savelofig ((lofig_list*)ptfig);
    } 
    else if (!strcasecmp (format, "al")) {
      strcpy (OUT_LO, "al");
      savelofig ((lofig_list*)ptfig);
    } 
    else if (!strcasecmp (format, "verilog")) {
      strcpy (OUT_LO, "vlg");
      savelofig ((lofig_list*)ptfig);
    } 
    else
      fprintf (stderr, "Supported formats are 'spice', 'vhdl' and 'verilog'\n");
    
    WORK_LIB = prev_work_lib;
    strcpy (OUT_LO, prev_out_lo);

    if (name!=NULL)
      ptfig->NAME=oldname;
}

//_____________________________________________________________________________

extern chain_list *expansedloins, *originalloins;

//_____________________________________________________________________________

//_____________________________________________________________________________

char *gns_GetInstanceName(loins_list *li)
{
  return li->INSNAME;
}

//_____________________________________________________________________________



//_____________________________________________________________________________

char *gns_GetInstanceModelName(loins_list *li)
{
  return li->FIGNAME;
}

//_____________________________________________________________________________

void gns_GetModelSignalRange(char *name, int *left, int *right)
{
  chain_list *arbo;
  losig_list *modells;
  ptype_list *p;
  tree_list *tree;
//  ht *sight;
  long l;

  if (current_lofig==NULL)
    {
      fprintf(stderr,"%s: Can't call gns_GetSignalRange() outside genius recognition process\n",gen_info()); 
      EXIT(1);
    }
  arbo=gen_hierarchical_split(gen_makesignalname(name));
  if (arbo->NEXT!=NULL)
    {
      freechain(arbo);
      fprintf(stderr,"%s: gns_GetSignalRange() called with a hierarchical name '%s'\n",gen_info(),name);     
      EXIT(1);
    }

//  sight=gen_get_losig_ht(current_lofig);

  modells=mbk_quickly_getlosigbyname(current_lofig, vectorradical((char *)arbo->DATA));
  if (modells==NULL)
     modells=mbk_quickly_getlosigbyname(current_lofig, (char *)arbo->DATA);
/*
          if ((l=gethtitem(sight, vectorradical((char *)arbo->DATA)))!=EMPTYHT
      || (l=gethtitem(sight, (char *)arbo->DATA))!=EMPTYHT
      )
    modells=(losig_list *)l;
  else
    modells=NULL;
    */
/*
  for (modells=current_lofig->LOSIG;
       modells!=NULL && modells->NAMECHAIN->DATA!=vectorradical((char *)arbo->DATA);
       modells=modells->NEXT) ;
  
  if (modells==NULL && current_lofig->LOINS==NULL)
    {
      for (modells=current_lofig->LOSIG;
       modells!=NULL && modells->NAMECHAIN->DATA!=(char *)arbo->DATA;
       modells=modells->NEXT) ;
    }
*/
  freechain(arbo);

  if (modells!=NULL)
    {
      if (!current_lofig->LOINS)
        {
          *left=*right=-1;
          return;
        }
      p=getptype(modells->USER, GEN_REAL_RANGE_PTYPE);
      if (p==NULL)
        EXIT(49);
      tree=(tree_list *)p->DATA;
      
      if (tree==NULL)
        *left=*right=-1;
      else
        if (gen_is_token_to (TOKEN(tree)) || gen_is_token_downto (TOKEN(tree))) 
          {
            *left=Eval_Exp_VHDL(tree->NEXT->DATA, ALL_ENV); 
            *right=Eval_Exp_VHDL(tree->NEXT->NEXT->DATA, ALL_ENV);
          }
        else 
          *left=*right=-1;

      return;
    }

  fprintf(stderr,"%s: gns_GetSignalRange() can't find signal '%s' in model\n",gen_info(),name);
  EXIT(1);
}

//_____________________________________________________________________________

chain_list *gns_GetModelConnectorList()
{
  chain_list *lst;
  locon_list *lc;

  if (current_lofig==NULL)
    {
      fprintf(stderr,"%s: Can't call gns_GetConnectorList() at the end of genius process\n",gen_info()); 
      EXIT(1);
    }

  for (lc=current_lofig->LOCON, lst=NULL; lc!=NULL; lc=lc->NEXT)
    {
      lst=addchain(lst, lc);
    }

  lst=reverse(lst);

  return lst;
}

//_____________________________________________________________________________

chain_list *gns_GetConnectorList(lofig_list *lf)
{
  chain_list *lst;
  locon_list *lc;

  for (lc=lf->LOCON, lst=NULL; lc!=NULL; lc=lc->NEXT)
    lst=addchain(lst, lc);

  return lst;
}

//_____________________________________________________________________________

char *gns_GetConnectorName(locon_list *lc)
{
  return lc->NAME;
}

//_____________________________________________________________________________

losig_list *gns_GetConnectorSignal(locon_list *lc)
{
  return lc->SIG;
}

//_____________________________________________________________________________

char *gns_GetConnectorDirection(locon_list *lc)
{
  switch(lc->DIRECTION)
    {
    case IN: return namealloc("in");
    case OUT: return namealloc("out");
    case INOUT: return namealloc("inout");
    case UNKNOWN: return namealloc("linkage");
    case TRISTATE:
    case TRANSCV: return namealloc("mux_bit");
    }
  return namealloc("linkage");
}

//_____________________________________________________________________________

char *gns_Vectorize(char *name, int index)
{
  char temp[1024];
  sprintf(temp,"%s(%d)",name,index);
  return namealloc(temp);
}

//_____________________________________________________________________________

char *gns_Vectorize2D(char *name, int index0, int index1)
{
  char temp[1024];
  sprintf(temp,"%s(%d)(%d)",name,index0,index1);
  return namealloc(temp);
}

//_____________________________________________________________________________

chain_list *gns_GetModelSignalList()
{
  chain_list *lst;
  losig_list *ls;

  if (current_lofig==NULL)
    {
      fprintf(stderr,"%s: Can't call gns_GetModelSignalList() at the end of genius process\n",gen_info()); 
      EXIT(1);
    }

  for (ls=current_lofig->LOSIG, lst=NULL; ls!=NULL; ls=ls->NEXT)
    lst=addchain(lst, ls);

  lst=reverse(lst);

  return lst;
}

//_____________________________________________________________________________

char *gns_GetSignalName(losig_list *ls)
{
  if (!ls)
    {    
      fprintf (stderr, "Error in 'gns_GetSignalName', parameter 'ls' is NULL\n");
      return NULL;
    }
  
  if (vectorindex((char *)ls->NAMECHAIN->DATA)==-1)
    return (char *)ls->NAMECHAIN->DATA;
  return gns_Vectorize(vectorradical((char *)ls->NAMECHAIN->DATA), vectorindex((char *)ls->NAMECHAIN->DATA));
}

//_____________________________________________________________________________

int gns_IsSignalExternal(losig_list *ls)
{
  return ls->TYPE=='E';
}

//_____________________________________________________________________________

chain_list *gns_GetInstanceConnectorList(loins_list *ls)
{
  chain_list *lst;
  locon_list *lc;

  if (current_lofig==NULL)
    {
      fprintf(stderr,"%s: Can't call gns_GetConnectorList() at the end of genius process\n",gen_info()); 
      EXIT(1);
    }

 
  for (lc=ls->LOCON, lst=NULL; lc!=NULL; lc=lc->NEXT)
    lst=addchain(lst, lc);

  if (getptype(ls->USER, GEN_FOUNDINS_PTYPE)!=NULL)
    lst=reverse(lst);

  return lst;
}

locon_list *gns_GetInstanceConnector(loins_list *ls, char *name)
{
  chain_list *lst, *arbo;
  locon_list *lc;

  arbo=gen_hierarchical_split(gen_makesignalname(name));
  if (arbo->NEXT!=NULL)
    {
      freechain(arbo);
      fprintf(stderr,"%s: gns_GetInstanceConnector() called with a hierarchical name '%s'\n",gen_info(),name);     
      EXIT(1);
    }
  for (lc=ls->LOCON, lst=NULL; lc!=NULL; lc=lc->NEXT)
    {
      if (vectorradical(lc->NAME)==vectorradical((char *)arbo->DATA) 
	  && vectorindex(lc->NAME)==vectorindex((char *)arbo->DATA)) 
	{
	  freechain(arbo);
	  return lc;
	}
    }

  freechain(arbo);
  fprintf(stderr,"%s: gns_GetInstanceConnector() can't find connector '%s' for instance\n",gen_info(),name);
  return NULL;
}

loins_list *gns_GetInstance(lofig_list *lf, char *name)
{
  loins_list *li;

  name=namealloc(gen_makeinstancename(name));

  for (li=lf->LOINS; li!=NULL && li->INSNAME!=name; li=li->NEXT) ;

  return li;
}

//_____________________________________________________________________________

chain_list *gns_GetAllCorrespondingInstances()
{
  chain_list *lst;
  subinst_t *cl;

  for (cl=CUR_CORRESP_TABLE->SUBINSTS, lst=NULL; cl!=NULL; cl=cl->NEXT)
    lst=addchain(lst, cl);

  return lst;
}

//_____________________________________________________________________________

chain_list *gns_GetAllCorrespondingTransistors()
{
  return GetAllHTElems(CUR_CORRESP_TABLE->TRANSISTORS);
}

//_____________________________________________________________________________

chain_list *gns_GetAllCorrespondingInstanceModels()
{
  chain_list *lst, *cl, *cl1;
  subinst_t *si;

  cl=gns_GetAllCorrespondingInstances();
  while (cl!=NULL)
    {
      si=(subinst_t *)cl->DATA;
      for (cl1=lst; cl1!=NULL && cl1->DATA!=si->CRT->GENIUS_FIGNAME; cl1=cl1->NEXT) ;
      if (cl1==NULL)
        lst=addchain(lst, si->CRT->GENIUS_FIGNAME);
      cl=cl->NEXT;
    }

  return lst;
}

//_____________________________________________________________________________

//_____________________________________________________________________________

locon_list *gns_GetTransistorGrid(lotrs_list *lt)
{
  return lt->GRID;
}

//_____________________________________________________________________________

locon_list *gns_GetTransistorDrain(lotrs_list *lt)
{
  return lt->DRAIN;
}

//_____________________________________________________________________________

locon_list *gns_GetTransistorSource(lotrs_list *lt)
{
  return lt->SOURCE;
}

//_____________________________________________________________________________

char gns_GetTransistorType(lotrs_list *lt)
{
  if (MLO_IS_TRANSN(lt->TYPE)) return 'n';
  return 'p';
}

//_____________________________________________________________________________

char *gns_GetTransistorTypeName(lotrs_list *lt)
{
  return getlotrsmodel(lt);
}

//_____________________________________________________________________________

double gns_GetTransistorParameter(char *name, lotrs_list *lt)
{
  if (strcasecmp(name,"w")==0)
    return ((double)lt->WIDTH/SCALE_X)*1e-6;
  if (strcasecmp(name,"l")==0)
    return ((double)lt->LENGTH/SCALE_X)*1e-6;

  if (strcasecmp(name,"as")==0)
  {
    if (lt->XS<0) return -1;
    else return ((double) lt->XS * lt->WIDTH / ( SCALE_X * SCALE_X ))*1e-12;
  }
  if (strcasecmp(name,"ad")==0)
  {
    if (lt->XD<0) return -1;
    else return ((double) lt->XD * lt->WIDTH / ( SCALE_X * SCALE_X ))*1e-12;
  }
  if (strcasecmp(name,"ps")==0)
  {
    if (lt->PS<0) return -1;
    else return ((double) lt->PS/SCALE_X)*1e-6;
  }
  if (strcasecmp(name,"pd")==0)
  {
    if (lt->PD<0) return -1;
    else return ((double) lt->PD/SCALE_X)*1e-6;
  }

  if (strcasecmp(name,"x")==0)
    return ((double) lt->X);
  if (strcasecmp(name,"y")==0)
    return ((double) lt->Y);

  fprintf(stderr,"%s: gns_GetTransistorParameter() can't find parameter '%s' for transistor\n",gen_info(),name);
  return 0.0;
}

//_____________________________________________________________________________

char *gns_GetTransistorName(lotrs_list *lt)
{
  return lt->TRNAME==NULL?namealloc("??"):lt->TRNAME;
}

//_____________________________________________________________________________

chain_list *gns_GetAllTransistorsConnectedtoSignal(losig_list *ls)
{
  chain_list *lst, *cl;
  locon_list *lc;
  
  for (cl=getptype(ls->USER, LOFIGCHAIN)->DATA, lst=NULL; cl!=NULL; cl=cl->NEXT)
    {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='T')
        {
          lst=addchain(lst, lc->ROOT);
        }
    }

  return lst;
}

//_____________________________________________________________________________

locon_list *gns_GetExternalConnectorOfSignal(losig_list *ls)
{
  chain_list *cl;
  locon_list *lc;
  
  for (cl=getptype(ls->USER, LOFIGCHAIN)->DATA; cl!=NULL; cl=cl->NEXT)
    {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='E')
        {
          return lc;
        }
    }

  return NULL;
}

//_____________________________________________________________________________

int gns_VectorIndex(char *name)
{
  return vectorindex(name);
}

//_____________________________________________________________________________

char *gns_VectorRadical(char *name)
{
  return vectorradical(namealloc(name));
}

//_____________________________________________________________________________

char *gns_CreateVhdlName(char *name)
{
  return bvl_vhdlname(name);
}

//_____________________________________________________________________________

void gns_ChangeInstanceModelName(loins_list *li, char *name)
{
  if (li==NULL)
    {
      addhtitem(NEW_LOINS_NAMES_HT, CUR_CORRESP_TABLE->GENIUS_INSNAME, (long)namealloc(name));
    }
  else
    {
      li->FIGNAME=namealloc(name);
    }
}

//_____________________________________________________________________________

locon_list *gns_GetConnector (lofig_list *netlist, char *con_name)
{
    return gen_get_connector (netlist, con_name);
}

//_____________________________________________________________________________

double gns_GetConnectorCapa (locon_list *lc)
{
    locon_list *ptcon;
    chain_list *ch;
    double capa = 0.0;

    if (lc->DIRECTION != 'I') return 0.0;

    for (ch = gen_get_lofigchain (lc->SIG); ch; ch = ch->NEXT) {
        ptcon = (locon_list*)ch->DATA;
        if (ptcon->TYPE != 'T' || ptcon->NAME == lc->NAME) continue;
        capa += gen_get_transistor_capa (ptcon);
    }

    return capa;
}


//_____________________________________________________________________________

lotrs_list *gns_GetTransistor (lofig_list *netlist, char *tr_name)
{
    return gen_get_transistor (netlist, tr_name);
}

//_____________________________________________________________________________

lotrs_list *gns_GetCorrespondingTransistor (char *name)
{
  return gen_GetCorrespondingTransistor(name, CUR_CORRESP_TABLE);
//    return gen_corresp_trs (gen_makeinstancename(name), CUR_CORRESP_TABLE);
}

//_____________________________________________________________________________

losig_list *gns_GetCorrespondingSignal (char *name)
{
    return gen_corresp_sig (gen_makesignalname(name), CUR_CORRESP_TABLE);
}

//_____________________________________________________________________________

losig_list *gns_GetSignal (lofig_list *netlist, char *signame)
{
    lofig_list *head_lofig;
    ht         *ht_lofig;
    losig_list *sig;

    if (!netlist) {
        fprintf (stderr, "Error in 'gns_GetSignal', parameter 'netlist' is NULL\n");
        return NULL;
    }

    if (!signame) {
        fprintf (stderr, "Error in 'gns_GetSignal', parameter 'signame' is NULL\n");
        return NULL;
    }

    // Contextualize HEAD_LOFIG ___________________________
    head_lofig = HEAD_LOFIG;
    HEAD_LOFIG = GEN_HEAD_LOFIG;
    ht_lofig = HT_LOFIG;
    HT_LOFIG = GEN_HT_LOFIG;

    // Function call ______________________________________
    sig = gen_get_signal (netlist, signame);

    // Release HEAD_LOFIG _________________________________
    GEN_HEAD_LOFIG = HEAD_LOFIG;
    HEAD_LOFIG = head_lofig;
    GEN_HT_LOFIG = HT_LOFIG;
    HT_LOFIG = ht_lofig;

    return sig;
}


subinst_t  *gns_GetCorrespondingInstance(char *name)
{
  return gen_GetCorrespondingInstance(name, CUR_CORRESP_TABLE);
}
/*
lotrs_list *gns_GetCorrespondingTransistor(char *name)
{
  return gen_GetCorrespondingTransistor(name, CUR_CORRESP_TABLE);
}
*/
losig_list *gns_GetCorrespondingInstanceConnectorSignal(subinst_t *subins, char *name)
{
  loins_list *loins_in_model;
  locon_list *lc;
  losig_list *ls;
  char *conname;

  conname=namealloc(gen_makesignalname(name));
  loins_in_model=gen_findinstance(CUR_HIER_LOFIG, subins->INSNAME);
  if (loins_in_model==NULL) return NULL;
  for (lc=loins_in_model->LOCON; lc!=NULL && lc->NAME!=conname; lc=lc->NEXT) ;
  if (lc==NULL) return NULL;
  ls=gen_corresp_sig (gen_losigname(lc->SIG), CUR_CORRESP_TABLE);
  if (ls==NULL) return NULL;
  return ls;
}

char *gns_GetCorrespondingInstanceName(subinst_t *subins)
{
  return subins->CRT->GENIUS_INSNAME;
}

char *gns_GetCorrespondingInstanceModelName(subinst_t *subins)
{
  return subins->CRT->GENIUS_FIGNAME;
}
/*
chain_list *gns_GetAllCorrespondingInstances()
{
  return dupchainlst(CUR_CORRESP_TABLE->SUBINSTS);
}
*/

char *gen_change_hier_divider (char *name, char divider)
{
    char buf[4096];
    int i;

    for (i = 0; name[i] != '\0'; i++) {
        if (name[i] == SEPAR) 
            buf[i] = divider;
        else 
            buf[i] = name[i];
    }

    buf[i] = '\0';

    return namealloc (buf);
    
}

void gns_ChangeHierarchyDivider (lofig_list *netlist, char divider)
{
    lotrs_list *pttrs;
    losig_list *ptsig;

    for (pttrs = netlist->LOTRS; pttrs; pttrs = pttrs->NEXT)
        pttrs->TRNAME = gen_change_hier_divider (pttrs->TRNAME, divider);

    for (ptsig = netlist->LOSIG; ptsig; ptsig = ptsig->NEXT)
        ptsig->NAMECHAIN->DATA = gen_change_hier_divider ((char*)ptsig->NAMECHAIN->DATA, divider);
}

int gns_GetGeneric(char *name)
{
  return gen_getvariable(name);
}

char *gns_GetCurrentArchi()
{
  return gen_getarchi();
}

char *gns_GetCurrentModel()
{
  return gen_getmodel();
}

char *gns_GetCurrentInstance()
{
  return gen_getinstancename();
}


void ASSOCIATE_BEHAVIOUR(void *func)
{
  APICallFunc *cf=(APICallFunc *)func;
  t_arg *ret;
  HierLofigInfo *hli=gethierlofiginfo(CUR_HIER_LOFIG);

 if (hli->BUILD_BEH==NULL)
   {
     // ajout a la lofig hierarchique de l'info
     hli->BUILD_BEH=cf;
     // 
     if (APIExecAPICallFunc(APIGetExecutionContext(), cf, &ret, 0)) EXIT(1);
     if (ret!=NULL)
       {
         APIFreeTARG(ret);
       }
   }
 else
   {
     APIFreeTARGS(cf->ARGS);
     mbkfree(cf);
   }
}

void *callfunc(char *funcname, ...)
{
  va_list ap;
  int i, nb;
  APICallFunc *acf;
  libgetargfunc_type libfunc;
  char fname[1024];
  t_arg **tab;

  sprintf(fname, "wrap_getargs_%s",funcname);

  libfunc=(libgetargfunc_type)GetDynamicFunction(fname);

  if (libfunc==NULL)
    {
      fprintf(stderr,"callfunc: function '%s' can't be found in the dynamic libraries\n",fname);
      EXIT(3);
    }

  libfunc(&tab, &nb);

  va_start(ap, funcname);
  for (i = 0; i < nb; i++) 
    {
      if (tab[i]->POINTER>0)
        {
          if (tab[i]->POINTER==1 && strcmp(tab[i]->TYPE,"char")==0)
            *(void **)tab[i]->VALUE = sensitive_namealloc(va_arg(ap, void *));
          else
            {
              fprintf(stderr,"callfunc: only 'char *' pointer type can be used in function call\n");
              EXIT(3);
            }
            /**(void **)tab[i]->VALUE = sensitive_namealloc(va_arg(ap, void *));*/
        }
      else 
        if (strcmp(tab[i]->TYPE,"char")==0)
          *(char *)tab[i]->VALUE = va_arg(ap, int);
        else if (strcmp(tab[i]->TYPE,"int")==0)
          *(int *)tab[i]->VALUE = va_arg(ap, int);
        else if (strcmp(tab[i]->TYPE,"long")==0)
          *(long *)tab[i]->VALUE = va_arg(ap, long);
        else if (strcmp(tab[i]->TYPE,"double")==0)
          *(double *)tab[i]->VALUE = va_arg(ap, double);
    }
  va_end(ap);

  acf=(APICallFunc *)mbkalloc(sizeof(APICallFunc));
  acf->NAME=sensitive_namealloc(funcname);
  acf->ARGS=APIPrepareFunctionARGS(NULL, tab, nb, NULL);
  return acf;
}

void *callfunc_tcl(char *funcname, chain_list *args)
{
  int i, nb=countchain(args);
  APICallFunc *acf;
  t_arg **tab;
  chain_list *cl;

  acf=(APICallFunc *)mbkalloc(sizeof(APICallFunc));
  acf->NAME=sensitive_namealloc(funcname);
  acf->ARGS=NULL;
  if (nb>0)
  {
   char buf[20];
   tab=mbkalloc(sizeof (t_arg *)*nb);
   for (i = 0, cl=args; i < nb; i++, cl=cl->NEXT) 
     {
       sprintf(buf,"arg%d",i);
       tab[i]=APINewTARG("char", 1, namealloc(buf));
       *(void **)tab[i]->VALUE = sensitive_namealloc((char *)cl->DATA);
     }
   acf->ARGS=APIPrepareFunctionARGS(NULL, tab, nb, NULL);
   mbkfree(tab);
  }
  return acf;
}

void gns_DriveSpiceNetlistGroup(chain_list *cl, char *filename)
{
  int failed=0;
  char oldSPI_VECTOR[64];
  char temp[1024];

  strcpy(oldSPI_VECTOR, SPI_VECTOR);
  strcpy(SPI_VECTOR, "[]");

  if (!failed)
    {
      FILE *f;
      if ((f=mbkfopen(filename, NULL, WRITE_TEXT))!=NULL)
        {
          sprintf(temp,"'%s'", filename);
          spiceprintdate(temp, f);
          spicesavelofigsinfile(cl, f);
        }
      else
        fprintf(stderr, "could not openfile '%s'\n", filename);
    }
  
  strcpy(SPI_VECTOR, oldSPI_VECTOR);
}

int gns_IsTopLevel()
{
  if (CUR_CORRESP_TABLE->FLAGS & LOINS_IS_TOPLEVEL) return 1;
  return 0;
}

char *_equiv(char *name)
{
  template_corresp *tc;
  char res[1024], *c;

  tc=gen_get_template_corresp(GNS_TEMPLATE_HT, CUR_CORRESP_TABLE->FIGNAME);
  if (tc!=NULL)
    {
      if (gen_find_template_corresp(GNS_TEMPLATE_HT, tc->defines, name, res)==0)
        return namealloc(res);
    }

  c=strrchr(name,'.');
  if (c==NULL) return namealloc(name);
  return namealloc(c+1);
}

void gns_REJECT_INSTANCE()
{
  long k;
  char *curins;

  curins=gns_GetCurrentInstance();
  if ((k=gethtitem(LOINS_LIST_CONTROL, curins))==EMPTYHT) k=0;
  k&=~KEEP_INSTANCE;
  addhtitem(LOINS_LIST_CONTROL, curins, k);
}

void gns_KEEP_INSTANCE()
{
  long k;
  char *curins;

  curins=gns_GetCurrentInstance();
  if ((k=gethtitem(LOINS_LIST_CONTROL, curins))==EMPTYHT) k=0;
  k|=KEEP_INSTANCE;
  addhtitem(LOINS_LIST_CONTROL, curins, k);
}

void gns_REJECT_MODEL()
{
  long k;
  char *curmodel;

  curmodel=gns_GetCurrentModel();
  if ((k=gethtitem(LOINS_LIST_CONTROL, curmodel))==EMPTYHT) k=0;
  k&=~KEEP_INSTANCE;
  addhtitem(LOINS_LIST_CONTROL, curmodel, k);
}

void gns_KEEP_MODEL()
{
  long k;
  char *curmodel;

  curmodel=gns_GetCurrentModel();
  if ((k=gethtitem(LOINS_LIST_CONTROL, curmodel))==EMPTYHT) k=0;
  k|=KEEP_INSTANCE;
  addhtitem(LOINS_LIST_CONTROL, curmodel, k);
}

char *gns_GetWorkingFigureName()
{
  return LATEST_GNS_RUN->FIGNAME;
}

int gns_IsVss(losig_list *sig)
{
  return mbk_LosigIsVSS(sig);
}

int gns_IsVdd(losig_list *sig)
{
  return mbk_LosigIsVDD(sig);
}

int gns_IsBlackBox()
{
 return (CUR_CORRESP_TABLE->FLAGS & LOINS_IS_BLACKBOX)!=0;
}
