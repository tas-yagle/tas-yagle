#include API_H
#include "gen_lofig.h"
#include "gen_corresp.h"
#include "gen_model_global.h"

#define SMALL_PRIME 103



//_____________________________________________________________________________
//_____________________________________________________________________________
// initial call pour gns_build_netlist () _____________________________________

lofig_list *gen_build_netlist (char *modelname, corresp_t *crt)
{
  lofig_list *fig;

  if (current_lofig->LOTRS!=NULL || current_lofig->LOINS==NULL) 
    fig = gen_build_from_FCL (current_lofig, modelname);
  else
    fig = gen_build_from_GNS (current_lofig, ALL_LOINS, ALL_LOTRS, modelname);

  //    fig->USER = addptype (fig->USER, LOFIG_LOCK, 0);
  fig->USER = addptype (fig->USER, GEN_ORIG_CORRESP_PTYPE, crt);
  return fig;
}
//_____________________________________________________________________________
//_____________________________________________________________________________

int gen_endianess (losig_list *sig)
{
  tree_list *array_sig;
  int endian = 0;
  ptype_list *ptype;

  if ((ptype = getptype (sig->USER, GEN_REAL_RANGE_PTYPE))) {
    if ((array_sig = (tree_list*)ptype->DATA)) {
      if (gen_is_token_to (TOKEN (array_sig)))
        endian = MBK_BIG_ENDIAN;
      else 
        if (gen_is_token_downto (TOKEN (array_sig)))
          endian = MBK_LITTLE_ENDIAN;
        else
          endian = 0;
    }
    else
      endian = 0;
  }

  return endian;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_sort_sigs (lofig_list *fig, ht *endht)
{
  chain_list *ordered_list, *ch;
  losig_list *sig;

  ordered_list = sort_sigs (fig->LOSIG, endht);

  fig->LOSIG = (losig_list*)ordered_list->DATA;
  sig = fig->LOSIG;
  for (ch = ordered_list->NEXT; ch; ch = ch->NEXT) {
    sig->NEXT = (losig_list*)ch->DATA;
    sig = sig->NEXT;
  }
  sig->NEXT = NULL;

  freechain (ordered_list);
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_update_sigcon (lofig_list *fig)
{
  locon_list *ptcon;

  for (ptcon = fig->LOCON; ptcon; ptcon = ptcon->NEXT) {
    if (ptcon->NAME != ptcon->SIG->NAMECHAIN->DATA) {
      ptcon->SIG->NAMECHAIN = addchain (ptcon->SIG->NAMECHAIN, ptcon->NAME);
      freechain (ptcon->SIG->NAMECHAIN->NEXT);
      ptcon->SIG->NAMECHAIN->NEXT = NULL;
    }
  }
}

//_____________________________________________________________________________
//_____________________________________________________________________________
extern char *FOR_VARIABLE;


chain_list *model_signal_name(loins_list *modelins, foundins_list *fil, long index)
{
  chain_list *cons, *allcons=NULL;
  locon_list *lc;
  int l, r;
  ptype_list *p;
  
  ALL_ENV=addptype(p=ALL_ENV, index, FOR_VARIABLE);
  for (lc=modelins->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      cons=GetModelSignalFromCON(lc, &l, &r, NULL, ALL_ENV, fil!=NULL?fil->VAR:NULL);
      if (l==UNDEF || r==UNDEF) 
      {
         avt_errmsg(GNS_ERRMSG, "040", AVT_FATAL, modelins->INSNAME, lc->NAME, l, r);
         //fprintf(stderr,"for model instance '%s' can not evaluate left or right bound for connector '%s' l=%d r=%d\n", modelins->INSNAME, lc->NAME, l, r);
         EXIT(5);
      }
      cons=reverse(cons);
      allcons=append(allcons, cons);
    }
  ALL_ENV->NEXT=NULL; freeptype(ALL_ENV); ALL_ENV=p;
  return allcons;
}
static void spec_cat(char *temp, char *s, char *s0)
{
  sprintf(temp,"%s*%s",s,s0);
}

char *modelname2circuitname(foundins_list *fli)
{
  
  ptype_list *var;
  char *temp;

  temp=fli->LOINS->FIGNAME;
  for (var=fli->VAR; var; var=var->NEXT)  
    if (var->TYPE!=UNDEF) temp=nameindex(temp, var->TYPE);
  
  return temp;
}

static int compare_loins(const void *a, const void *b)
{
  int ret;
  ptype_list *p, *p1;
  loins_list *a0=*(loins_list **)a, *b0=*(loins_list **)b;
  loins_list *oa, *ob;
  p=getptype(a0->USER, GEN_ORIG_LOINS_PTYPE); oa=(loins_list *)p->DATA;
  p1=getptype(b0->USER, GEN_ORIG_LOINS_PTYPE); ob=(loins_list *)p1->DATA;

  ret=strcmp(oa->INSNAME,ob->INSNAME);
  if (ret!=0) return ret;
  p=getptype (a0->USER, GEN_LOOP_INDEX_PTYPE);
  p1=getptype (b0->USER, GEN_LOOP_INDEX_PTYPE);
  if (p==NULL || p1==NULL) EXIT(7);
  if ((long)p->DATA<(long)p1->DATA) return -1;
  else if ((long)p->DATA>(long)p1->DATA) return 1;
  return 0;
}

loins_list *sort_loins(loins_list *loins)
{
  loins_list **tab, *li;
  int i, j;
  tab=(loins_list **)mbkalloc(sizeof(loins_list **)*16000);
  for (i=0, li=loins; li!=NULL; li=li->NEXT, i++)
    tab[i]=li;

  if (i>0)
    {
      qsort(tab, i, sizeof(loins_list *), compare_loins);
      
      for (j=0; j<i; j++)
        {
          if (j>0) tab[j-1]->NEXT=tab[j];
        }
      tab[j-1]->NEXT=NULL;
    }
  li=tab[0];
  mbkfree(tab);
  return li;
}


lofig_list *gen_build_from_GNS (lofig_list *lf, chain_list *loins, chain_list *lotrs, char *modelname)
{
  lofig_list *fig;
  lofig_list *head_lofig;
  ht         *ht_lofig;
  loins_list *ins, *newins, *modelins;
  locon_list *ptcon, *newcon;
  losig_list *grid, *source, *drain, *bulk, *sig, *assoc_sig, *sig0;
  lotrs_list *tr, *newtr;
  ptype_list *ptype;
  chain_list *ptch, *ch, *cl;
  char *name, *name0;
  int ial, eal;
  char insname[1024], temp[1024];
  long lsig;
  ht *htsig, *endht;
  int i, begin, end, index = 1, dir;
  losig_list *onevdd=NULL, *onevss=NULL;
  chain_list *sig_to_delete=NULL, *allcons;
  foundins_list *fil;
  int saved_flag;

  htsig = addht (SMALL_PRIME);

  // Contextualize HEAD_LOFIG ___________________________

  head_lofig = HEAD_LOFIG;
  HEAD_LOFIG = GEN_HEAD_LOFIG;

  ht_lofig = HT_LOFIG;
  HT_LOFIG = GEN_HT_LOFIG;

  fig = addlofig (modelname/*lf->NAME*/);

  for (sig = lf->LOSIG; sig; sig = sig->NEXT) {
      
    begin = gen_lower_bound (sig);
    end = gen_upper_bound (sig);
      
    if (begin != -2 && end != -2) {
      // put circuit/model correspondance in htsig
      if (!(ptype = getptype (sig->USER, GENSIGCHAIN))) continue;
      if (begin == -1 && end == -1) {
        assoc_sig = getlosigfrommark (ptype->DATA, -1);
        if (assoc_sig==NULL && !gns_isunusedsig(sig))
          {
            avt_errmsg(GNS_ERRMSG, "019", AVT_FATAL, gen_losigname (sig));
            //fprintf(stderr,"no correspondance found for signal '%s'\n", gen_losigname (sig));
            EXIT(4);
          }
        spec_cat(temp, gen_losigname (sig), assoc_sig==NULL?"?":gen_losigname (assoc_sig));
        sig0 = addlosig (fig, index++, addchain (NULL, gen_losigname (sig)), 'I');
        sig0->FLAGS=0;
        addhtitem (htsig, namealloc(temp), (long)sig0);
      } 
      else { 
        for (i = begin; i <= end; i++) {
          name = gen_vectorize (gen_losigname (sig), i);
          assoc_sig = getlosigfrommark (ptype->DATA, i);
          if (assoc_sig==NULL && !gns_isunusedsig(sig))
            {
              avt_errmsg(GNS_ERRMSG, "041", AVT_FATAL, gen_losigname (sig),i);
              //fprintf(stderr,"no correspondance found for signal '%s(%d)'\n", gen_losigname (sig),i);
              EXIT(4);
            }
          spec_cat(temp, name, assoc_sig==NULL?"?":gen_losigname (assoc_sig));
          sig0 = addlosig (fig, index++, addchain (NULL, name), 'I');
          sig0->FLAGS=0;
          addhtitem (htsig, namealloc(temp), (long)sig0);
        }
      }
    }
  }

  // Instances __________________________________________
    
  for (ch = loins; ch; ch = ch->NEXT) {

    fil=(foundins_list*)ch->DATA;
    ins = fil->LOINS;
    saved_flag = fil->SAVEDFLAG;
        
    /* new instance */

    ptype = getptype (ins->USER, GEN_LOOP_INDEX_PTYPE);
    modelins=fil->VISITED;
    name = modelins->INSNAME;
    if (ptype)
      {
        sprintf (insname, "%s_%ld", name, (long)ptype->DATA);
        allcons=model_signal_name(modelins, fil, (long)ptype->DATA);
      }
    else
      {
        strcpy (insname, name);
        allcons=model_signal_name(modelins, fil, UNDEF);
      }

    newins = (loins_list*)mbkalloc (sizeof (loins_list));
    newins->INSNAME = namealloc (insname);
    newins->FIGNAME = modelname2circuitname(fil); //ins->FIGNAME;
    newins->LOCON   = NULL;
    newins->USER    = NULL;
    newins->NEXT    = fig->LOINS;
    fig->LOINS      = newins;
               
    // on ajoute l'index pour les instance dans les FOR expanded ou pas
    if (ptype)
      newins->USER = addptype (newins->USER, GEN_LOOP_INDEX_PTYPE, ptype->DATA);
    else if ((ptype = getptype (modelins->USER, GEN_LOOP_INDEX_PTYPE)))
      newins->USER = addptype (newins->USER, GEN_LOOP_INDEX_PTYPE, ptype->DATA);

    // on ajoute un pointeur sur la loins non expanded dans le model
    if ((ptype = getptype (modelins->USER, GEN_ORIG_LOINS_PTYPE)))
      newins->USER = addptype (newins->USER, GEN_ORIG_LOINS_PTYPE, ptype->DATA);
    else
      newins->USER = addptype (newins->USER, GEN_ORIG_LOINS_PTYPE, modelins);
          

    // update model list
    for (ptch = fig->MODELCHAIN; ptch; ptch = ptch->NEXT)
      if (ptch->DATA == (void*)ins->FIGNAME) break;
    if (!ptch)
      fig->MODELCHAIN = addchain (fig->MODELCHAIN, (void*)ins->FIGNAME);
        
    // duplicate connectors and signals

    for (ptcon = ins->LOCON, cl=allcons; ptcon && cl; ptcon = ptcon->NEXT, cl=cl->NEXT) {
      newcon            = (locon_list*)mbkalloc (sizeof (locon_list));
      newcon->NAME      = ptcon->NAME;
      newcon->DIRECTION = ptcon->DIRECTION;
      if (newcon->DIRECTION == 'D' || newcon->DIRECTION == 'S')
        newcon->DIRECTION = 'I';
      newcon->TYPE      = 'I';


      if (ptcon->SIG!=NULL)
        name = gen_losigname (ptcon->SIG);
      else
        name="?";

      spec_cat(temp, (char *)cl->DATA, name);
      name=namealloc(temp);
      if ((lsig = gethtitem (htsig, name)) != EMPTYHT) {
        newcon->SIG =(losig_list*)lsig;
        newcon->SIG->FLAGS=1;
      }
      else
        {
          newcon->SIG = addlosig (fig, index++, addchain (NULL, gen_losigname (ptcon->SIG)), 'I');
          newcon->SIG->FLAGS=0;
        }

      newcon->ROOT  = (void*)newins;
      newcon->USER  = NULL;
      newcon->PNODE = NULL;
      newcon->NEXT  = newins->LOCON;
      newins->LOCON = newcon;
    }

    if (cl!=NULL || ptcon!=NULL) EXIT(2);
    freechain(allcons);
  }

 
  // Transistors ________________________________________           
    
  for (ch = lotrs; ch; ch = ch->NEXT) {
    tr = (lotrs_list*)ch->DATA;
 
    modelins=(loins_list *)getptype(tr->USER, GEN_VISITED_PTYPE)->DATA;
    cl=allcons=model_signal_name(modelins, NULL, UNDEF);
        
    // grid
    name = gen_losigname (tr->GRID->SIG);
    spec_cat(temp, (char *)cl->DATA, name);
    name=namealloc(temp); cl=cl->NEXT;
    if ((lsig = gethtitem (htsig, name)) == EMPTYHT) {
      grid = addlosig (fig, index++, addchain (NULL, gen_losigname (tr->GRID->SIG)), 'I');
      grid->FLAGS=0;
    } else
      { grid = (losig_list*)lsig; grid->FLAGS=1; }
        
    if (tr->FLAGS==0)      
      {
        // source
        name = gen_losigname (tr->SOURCE->SIG);
        spec_cat(temp, (char *)cl->DATA, name);
        name=namealloc(temp); 
        spec_cat(temp, (char *)cl->NEXT->DATA, gen_losigname (tr->DRAIN->SIG));
        name0=namealloc(temp);
      }
    else
      {
        spec_cat(temp, (char *)cl->DATA, gen_losigname (tr->DRAIN->SIG));
        name0=namealloc(temp);
        spec_cat(temp, (char *)cl->NEXT->DATA, gen_losigname (tr->SOURCE->SIG));
        name=namealloc(temp);
      }
    cl=cl->NEXT->NEXT;
    if ((lsig = gethtitem (htsig, name)) == EMPTYHT) {
      source = addlosig (fig, index++, addchain (NULL, gen_losigname (tr->SOURCE->SIG)), 'I');
      source->FLAGS=0;
    } else
      { source = (losig_list*)lsig; source->FLAGS=1; }

    // drain
    if ((lsig = gethtitem (htsig, name0)) == EMPTYHT) {
      drain = addlosig (fig, index++, addchain (NULL, gen_losigname (tr->DRAIN->SIG)), 'I');
      drain->FLAGS=0;
    } else
      { drain = (losig_list*)lsig; drain->FLAGS=1; }

    // bulk
    if (tr->BULK && tr->BULK->SIG) {
      name = gen_losigname (tr->BULK->SIG);
      spec_cat(temp, (char *)cl->DATA, name);
      name=namealloc(temp); cl=cl->NEXT;
      if ((lsig = gethtitem (htsig, name)) == EMPTYHT) {
        bulk = addlosig (fig, index++, addchain (NULL, gen_losigname (tr->BULK->SIG)), 'I');
        bulk->FLAGS=0;
      } else
        { bulk = (losig_list*)lsig; bulk->FLAGS=1; }
    }
    else
      bulk = NULL;

    freechain(allcons);

    if (tr->FLAGS<0 || tr->FLAGS>1) EXIT(7);
    if (tr->FLAGS==1) // matched reversed
      {
        losig_list *temp;
        temp=drain;
        drain=source;
        source=temp;
      }

    // transistor name

    name = modelins->INSNAME;

    if (MLO_IS_TRANSN(tr->TYPE))
      newtr = addlotrs (fig, TRANSN, tr->X, tr->Y, tr->WIDTH, tr->LENGTH, tr->PS, tr->PD,
                        tr->XS, tr->XD, grid, source, drain, bulk, name);
    else
      if (MLO_IS_TRANSP(tr->TYPE))
        newtr = addlotrs (fig, TRANSP, tr->X, tr->Y, tr->WIDTH, tr->LENGTH, tr->PS, tr->PD,
                          tr->XS, tr->XD, grid, source, drain, bulk, name);
    newtr->MODINDEX = tr->MODINDEX;
    newtr->TRNAME = name;

    // on ajoute l'index pour les transistors dans les FOR expanded
    if ((ptype = getptype (modelins->USER, GEN_LOOP_INDEX_PTYPE)))
      newtr->USER = addptype (newtr->USER, GEN_LOOP_INDEX_PTYPE, ptype->DATA);

    // on ajoute un pointeur sur la loins non expanded dans le model
    if ((ptype = getptype (modelins->USER, GEN_ORIG_LOINS_PTYPE)))
      newtr->USER = addptype (newtr->USER, GEN_ORIG_LOINS_PTYPE, ptype->DATA);
  }

  delht (htsig);
  htsig = addht (SMALL_PRIME);

  // Update signal's names ______________________________

  for (sig = fig->LOSIG; sig; sig = sig->NEXT)
    if (sig->FLAGS==1)
      {
        // priorité de l'alim externe sur l'alim interne
        eal = (mbk_LosigIsVDD(sig) && sig->TYPE == 'E'); ial = (mbk_LosigIsVDD(sig) && sig->TYPE == 'I');
        if ((onevdd==NULL && (eal || ial)) ||
            (onevdd!=NULL && eal)) onevdd=sig;
        eal = (mbk_LosigIsVSS(sig) && sig->TYPE == 'E'); ial = (mbk_LosigIsVSS(sig) && sig->TYPE == 'I');
        if ((onevss==NULL && (eal || ial)) ||
            (onevss!=NULL && eal)) onevss=sig;
      }
    
  for (ins= fig->LOINS; ins!=NULL; ins=ins->NEXT)
    {
      for (ptcon=ins->LOCON; ptcon!=NULL; ptcon=ptcon->NEXT)
        {
          sig=ptcon->SIG;
          if (sig->FLAGS!=1) // pas de correspondance
            {
              if (sig->FLAGS!=2) { sig_to_delete=addchain(sig_to_delete, sig); sig->FLAGS=2; }
                
              if (onevdd!=NULL && 
                  (mbk_LosigIsVDD(sig))
                  ) 
                {
                  ptcon->SIG=onevdd;
                }
              else
                if (onevss!=NULL && 
                  (mbk_LosigIsVSS(sig))
                    )
                  {
                    ptcon->SIG=onevss;
                  }
                else 
                  {
                    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 19);
                    EXIT(6);
                  }
            }
        }
    }
  for (tr= fig->LOTRS; tr!=NULL; tr=tr->NEXT)
    {
      locon_list *tab[4];
      tab[0]=tr->GRID; tab[1]=tr->DRAIN; tab[2]=tr->SOURCE; tab[3]=tr->BULK;
      for (i=0; i<4; i++)
        {
          if (tab[i]==NULL || tab[i]->SIG==NULL) continue;
          sig=tab[i]->SIG;
          if (sig->FLAGS!=1) // pas de correspondance
            {
              if (sig->FLAGS!=2) { sig_to_delete=addchain(sig_to_delete, sig); sig->FLAGS=2; }
              
              if (onevdd!=NULL && 
                  (mbk_LosigIsVDD(sig))
                  ) 
                {
                  tab[i]->SIG=onevdd;
                }
              else
                if (onevss!=NULL && 
                  (mbk_LosigIsVSS(sig))
                    )
                  {
                    tab[i]->SIG=onevss;
                  }
                else 
                  {
                    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 20);
                    EXIT(6);
                  }
            }
        }
    }
  
  delht (htsig);
    
  // Sort signals _______________________________________
    
  for (cl=sig_to_delete; cl!=NULL; cl=cl->NEXT)
    {
      sig0=(losig_list *)cl->DATA;
      dellosig(fig, sig0->INDEX);
    }
  freechain(sig_to_delete);

  endht = addht (SMALL_PRIME);

  for (sig = fig->LOSIG; sig; sig = sig->NEXT)
    addhtitem (endht, vectorradical (sig->NAMECHAIN->DATA), gen_endianess (sig));

  gen_sort_sigs (fig, endht);

  delht (endht);

  // Connectors _________________________________________

  for (ptcon = lf->LOCON; ptcon; ptcon = ptcon->NEXT) 
    {
      gen_get_bounds (ptcon->SIG, &begin, &end);
        
      if (begin == -1 || end == -1) 
        {
          for (sig = fig->LOSIG; sig; sig = sig->NEXT)
            if (ptcon->NAME == sig->NAMECHAIN->DATA) break;
            
          if (sig) {
            sig->TYPE = 'E';
            addlocon (fig, ptcon->NAME, sig, ptcon->DIRECTION);
          }
        }
      else 
        {
          if (begin<=end) dir=1; else dir=-1;
          for (i = begin; i != end+dir; i+=dir)
            { 
              name = gen_vectorize (ptcon->NAME, i);
              for (sig = fig->LOSIG; sig; sig = sig->NEXT) 
                if (name == sig->NAMECHAIN->DATA) break;
              if (sig) {
                sig->TYPE = 'E';
                addlocon (fig, name, sig, ptcon->DIRECTION);
              }
            }
        }
    }
    
  // delete FCL marks

  for (ptcon = fig->LOCON; ptcon; ptcon = ptcon->NEXT)
    if (ptcon->DIRECTION == 'D' || ptcon->DIRECTION == 'S')
      ptcon->DIRECTION = 'I';

  for (sig = fig->LOSIG; sig; sig = sig->NEXT)
    if (sig->TYPE == 'D' || sig->TYPE == 'S')
      sig->TYPE = 'E';

  // Release HEAD_LOFIG _________________________________
   
  GEN_HEAD_LOFIG = HEAD_LOFIG;
  HEAD_LOFIG = head_lofig;

  GEN_HT_LOFIG = HT_LOFIG;
  HT_LOFIG = ht_lofig;
    
  fig->LOINS=sort_loins(fig->LOINS);

  return fig;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_clean_RC (lofig_list *fig)
{
  losig_list *sig;

  lofigchain (fig);

  for (sig = fig->LOSIG; sig; sig = sig->NEXT)
    if (sig->PRCN) {
      freelorcnet (sig);
      sig->PRCN = NULL;
    }
}

//_____________________________________________________________________________
//_____________________________________________________________________________

lofig_list *gen_build_from_FCL (lofig_list *lf, char *modelname) 
{
  lofig_list *duplf;
  locon_list *ptcon/*, *vsscon, *vddcon*/;
  losig_list *ptsig;
  ptype_list *pt;

  // add the lofig to GEN_HEAD_LOFIG

  duplf = rduplofig (lf);
  duplf->NAME=modelname;

  gen_clean_RC (duplf);
  duplf->NEXT = GEN_HEAD_LOFIG;
  GEN_HEAD_LOFIG = duplf;

  if (!GEN_HT_LOFIG) GEN_HT_LOFIG = addht (50);
  addhtitem(GEN_HT_LOFIG, duplf->NAME, (long)duplf);

  // update connector's name

  gen_update_sigcon (duplf);
    
  // delete FCL marks

  for (ptcon = duplf->LOCON; ptcon; ptcon = ptcon->NEXT)
    {
      if (ptcon->DIRECTION == 'D' || ptcon->DIRECTION == 'S')
        ptcon->DIRECTION = 'I';
    }
    
  duplf->LOCON = (locon_list*)reverse ((chain_list*)duplf->LOCON);

  // Lofigchain
  lofigchain (duplf);

  for (ptsig = duplf->LOSIG; ptsig; ptsig = ptsig->NEXT)
    if (ptsig->TYPE == 'D' || ptsig->TYPE == 'S')
    {
      chain_list *cl;
      pt=getptype(ptsig->USER, LOFIGCHAIN);
      for (cl=(chain_list *)pt->DATA; cl!=NULL && ((locon_list *)cl->DATA)->TYPE!=EXTERNAL; cl=cl->NEXT) ;
      if (cl!=NULL) ptsig->TYPE = 'E';
      else ptsig->TYPE = 'I';
    }

  return duplf;
}
