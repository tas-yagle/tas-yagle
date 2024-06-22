#define API_USE_REAL_TYPES
#include "gen_API_netlist.h"
#include "gen_API.h"
#include "gen_API_include.h"
#include TRC_H
#include MUT_H
#include "api_common_structures.h"
#include "api_communication.h"

// Globals ____________________________________________________________________
//
#define GEN_UNKNOWN_OUTLOAD    0
#define GEN_DYNAMIC_OUTLOAD    1
#define GEN_TRANSISTOR_OUTLOAD 2
#define GEN_NO_OUTLOAD         3
#define GEN_BEST  'B'
#define GEN_WORST 'W'

#define IN_RC        1
#define INTERNAL_RC  2
#define OUT_RC       4
#define NO_RC        8
#define ALL_RC       16
#define XTALK_TO_GND 32


#define TEMP_PTYPE  0x1254689
#define TEMP_PTYPE2 0x1254690
#define DELETE_ME_LATER 0x1254691
#define MORE_INTERNAL_CAPA 0x1254692
#define CORRESP_FOR_GLOBAL 0x1254693

typedef struct
{
  int count;
  ht *per_signal_internal_locons;
} more_capa;

int GEN_SIM_OUTLOAD = GEN_DYNAMIC_OUTLOAD; // <= valeur par defaut
ht *GEN_REVERSE_CORRESP = NULL;
chain_list *GENIUS_CATAL = NULL;


// en attendant rcxi
//extern RCXFLOAT rcx_getcoefctc( lofig_list*, losig_list*, rcx_slope*, char, RCXFLOAT, RCXFLOAT*);
// -----------------
extern void gen_add_RC(lofig_list* fig, corresp_t *table, int rc, ctk_struct *cs);
extern lofig_list *gen_flatten(lofig_list *figi, corresp_t *table, chain_list *trs2sup,  chain_list *trs2keep);


static int realcleanaftersuppress(lofig_list *fig);
static void init_suppress_marks(lofig_list *fig);
static more_capa *graballinternalconnectors(lofig_list *fig);
static void gen_add_fake_rc (lofig_list *fig);
static void gen_strip_lofig (lofig_list *fig, int keepinterf);

//_____________________________________________________________________________
// PATCH RCX

#define GETFLAG(m,b) (m & (1<<b))
#define RCXNOWIRE    ((long)14)

void patchlofig_if_needed(lofig_list *lf)
{
  losig_list *ls;
  rcx_list *rcx;
  chain_list *cl;
  locon_list *lc;
  int cnt;
  
  for (ls=lf->LOSIG, cnt=0; ls!=NULL && cnt<10; ls=ls->NEXT, cnt++)
    if (getrcx(ls)!=NULL) break;

  if (ls==NULL || cnt>=10) return; // <= rcx pas encore passe dessus

  if (getptype(lf->USER, GEN_VISITED_PTYPE)!=NULL) return;

//  avt_fprintf(stdout,"¤6¤~    PATCH RCX    ¤...."); fflush(stdout);
  
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      rcx=getrcx(ls);
      if(rcx==NULL || GETFLAG(rcx->FLAG, RCXNOWIRE)) 
        {
          cl=(chain_list *)getptype(ls->USER, LOFIGCHAIN_LOCAL/*LOFIGCHAIN*/)->DATA;
          while (cl!=NULL)
            {
              lc=(locon_list *)cl->DATA;
              if (lc->PNODE!=NULL && (lc->PNODE->NEXT!=NULL || lc->PNODE->DATA!=1))
                {
                  freenum(lc->PNODE);
                  lc->PNODE=addnum(NULL, 1);
                }
              cl=cl->NEXT;
            }
          freelorcnet(ls);
        }
    }

//  avt_fprintf(stdout," DONE¤.\n"); fflush(stdout);
  
  lf->USER=addptype(lf->USER, GEN_VISITED_PTYPE, NULL);
}

//_____________________________________________________________________________






static ctk_struct *gns_call_ctk_if_needed(lofig_list *lf)
{
  ctk_struct *cs=NULL;
  lofig_list *clf;
  if (TTV_COM==NULL || CTK_API_COM==NULL) return NULL;
  CTK_API_COM(CTK_getctknetlist, &clf);
  if (clf==lf)
    {
      CTK_API_COM(CTK_getctkstruct, &cs);
      if (cs==NULL && STB_COM!=NULL)
        {
          
          CTK_API_COM(CTK_rundefaultaction, NULL);
          CTK_API_COM(CTK_getctkstruct, &cs);
        }
    }
  return cs;
}

void dispp()
{
  losig_list *sig;
  for (sig = GENIUS_GLOBAL_LOFIG->LOSIG; sig; sig = sig->NEXT) 
    {
      if (strcasecmp(gen_losigname(sig),"sig99_3")==0) break;
    }
  if (sig==NULL) printf("not found\n");
  else
    {
      viewlosig(sig);
    }
}

void gen_update_losigalim (lofig_list* fig, corresp_t *table)
{
   losig_list *sig, *cir_sig;
   ptype_list *pt;
   for (sig = fig->LOSIG; sig; sig = sig->NEXT)
    {

      if (!gen_isalim(sig)) continue;
      // corresponding signal
      cir_sig = gen_corresp_sig (gen_losigname (sig), table);
      if (cir_sig==NULL && (pt=getptype(sig->USER, CORRESP_FOR_GLOBAL))!=NULL)
        cir_sig = gen_corresp_sig (gen_losigname (sig), (corresp_t *)pt->DATA);

       if (cir_sig)
          {
             mbk_copylosiginfo(cir_sig, sig);
          }
    }
}
lofig_list *gns_FlattenNetlist (lofig_list *fig, int rc)
{
    lofig_list *flat_fig;
    ptype_list *p, *p0, *orig;
    corresp_t *temp;
    int tr_count, ins_count;
    chain_list *trs2sup=NULL, *trs2keep=NULL;

    if (!fig) return NULL;

    
    // Local lofigchain
    if (GENIUS_GLOBAL_LOFIG->LOSIG==NULL || getptype(GENIUS_GLOBAL_LOFIG->LOSIG->USER, LOFIGCHAIN_LOCAL)==NULL)
      lofigchain_local (GENIUS_GLOBAL_LOFIG);
    
    // ***** temporaire *******
    patchlofig_if_needed(GENIUS_GLOBAL_LOFIG);


    orig=getptype(fig->USER, GEN_ORIG_INS_CORRESP_PTYPE);
    
    if (orig!=NULL)
      {
        temp=CUR_CORRESP_TABLE;
        CUR_CORRESP_TABLE=(corresp_t*)orig->DATA;
      }

    if ((p=getptype(fig->USER, GEN_TRANS_TO_SUPPRESS_PTYPE))!=NULL)
      trs2sup=(chain_list *)p->DATA;

    if ((p=getptype(fig->USER, GEN_TRANS_TO_KEEP_PTYPE))!=NULL)
      trs2keep=(chain_list *)p->DATA;


    // Flatten ____________________________________
    mbk_debugstat(NULL,1);
    if (fig->LOINS) // GENIUS model
        flat_fig = gen_flatten (fig, CUR_CORRESP_TABLE, trs2sup, trs2keep);
    else { 
        flat_fig = rduplofig (fig);
        gen_update_transistors (flat_fig, CUR_CORRESP_TABLE);
    }

    if ((p0=getptype(fig->USER, MORE_INTERNAL_CAPA))!=NULL)
      {
        more_capa *mc;
        mc=(more_capa *)p0->DATA;
        mc->count++;
        flat_fig->USER=addptype(flat_fig->USER, MORE_INTERNAL_CAPA, mc);
      }

#ifdef STAT
    mbk_debugstat("flatten:",0);
#endif

    gen_add_parallel_transistors (flat_fig, CUR_CORRESP_TABLE);

#ifdef STAT
    mbk_debugstat("parall trans:",0);
#endif

    init_suppress_marks(flat_fig);
    tr_count=gen_suppress_trans (flat_fig, trs2sup, trs2keep/*TRANS_TO_SUPPRESS*/);

#ifdef STAT
    mbk_debugstat("sup prep:",0);
#endif


    if (trs2sup!=NULL || trs2keep!=NULL)
      {
        flat_fig->USER=addptype(flat_fig->USER, MORE_INTERNAL_CAPA, graballinternalconnectors(flat_fig));
      }

    // Add RCs ____________________________________
    if (rc & (INTERNAL_RC | OUT_RC | IN_RC | ALL_RC))
      {
        ctk_struct *cs;
        cs=gns_call_ctk_if_needed(fig);
        gen_add_RC (flat_fig, CUR_CORRESP_TABLE, rc, cs);
        // destroy cs
      }

#ifdef STAT
    mbk_debugstat("addrc:",0);
#endif

    // Suppress transistors _______________________________
    ins_count=realcleanaftersuppress(flat_fig);

#ifdef STAT
    mbk_debugstat("suppress trans:",0);
#endif

    // Add output capas
    gen_add_external_capas (flat_fig, CUR_CORRESP_TABLE);
    gen_update_losigalim(flat_fig, CUR_CORRESP_TABLE);

    if (orig!=NULL)
      {
        CUR_CORRESP_TABLE=temp;
      }

#ifdef STAT
    mbk_debugstat("ext capa:",0);
#endif

    rcn_mergectclofig(flat_fig);

    if (trs2sup!=NULL || trs2keep!=NULL)
      {
//        avt_error("genapi", -1, AVT_INFO,"%d transistors and %d instances suppressed for '%s'\n",tr_count,ins_count, fig->NAME);
        if (trs2sup!=NULL) { freechain(trs2sup); fig->USER=delptype(fig->USER, GEN_TRANS_TO_SUPPRESS_PTYPE); }
        if (trs2keep!=NULL) { freechain(trs2keep); fig->USER=delptype(fig->USER, GEN_TRANS_TO_KEEP_PTYPE); }
      }

//    freechain(TRANS_TO_SUPPRESS); TRANS_TO_SUPPRESS=NULL;

    gen_add_fake_rc(flat_fig);
    return flat_fig;
}

static void copy_info(lofig_list *dest, lofig_list *src)
{
  lotrs_list *trs, *trd;
  ptype_list *p;

  for (trd=dest->LOTRS, trs=src->LOTRS; trd!=NULL; trd=trd->NEXT, trs=trs->NEXT)
    {
      if (trs->TRNAME!=trd->TRNAME) EXIT(-8);
      if ((p=getptype(trs->USER, GEN_TRANS_PARALLEL))!=NULL)
        trd->USER=addptype(trd->USER, GEN_TRANS_PARALLEL, p->DATA);
    }
}

lofig_list *gns_AddRC (lofig_list *fig, int rc)
{
  lofig_list *flat_fig;
  ptype_list *p, *p0, *orig;
  corresp_t *temp;
  if (!fig) return NULL;
  
  orig=getptype(fig->USER, GEN_ORIG_INS_CORRESP_PTYPE);
  
  if (orig!=NULL)
    {
      temp=CUR_CORRESP_TABLE;
      CUR_CORRESP_TABLE=(corresp_t*)orig->DATA;
    }
  
  flat_fig = rduplofig (fig);
  copy_info(flat_fig, fig);
  lofigchain(flat_fig);

  if ((p0=getptype(fig->USER, MORE_INTERNAL_CAPA))!=NULL)
    {
      more_capa *mc;
      mc=(more_capa *)p0->DATA;
      mc->count++;
      flat_fig->USER=addptype(flat_fig->USER, MORE_INTERNAL_CAPA, mc);
    }

//  gen_update_transistors (flat_fig, CUR_CORRESP_TABLE);
  gen_add_parallel_transistors (flat_fig, CUR_CORRESP_TABLE);

  init_suppress_marks(flat_fig);

  // Add RCs ____________________________________
  if (rc & (INTERNAL_RC | OUT_RC | IN_RC | ALL_RC))
    {
      ctk_struct *cs;
      cs=gns_call_ctk_if_needed(fig);
      gen_add_RC (flat_fig, CUR_CORRESP_TABLE, rc, cs);
      // destroy cs
    }
  // Add output capas
  gen_add_external_capas (flat_fig, CUR_CORRESP_TABLE);
  
  if (orig!=NULL)
    {
      CUR_CORRESP_TABLE=temp;
    }

  rcn_mergectclofig(flat_fig);
  gen_add_fake_rc(flat_fig);
  return flat_fig;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gns_StripNetlist (lofig_list *fig)
{
    gen_strip_lofig (fig, 1);
}

void gns_StripNetlistFurther (lofig_list *fig)
{
    gen_strip_lofig (fig, 0);
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gns_SetLoad (Netlist *netlist, char *connector, double load) 
{
    gen_set_charac_capa ((lofig_list*)netlist, connector, load);
}

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________

static void addinlofigchain(locon_list *lc)
{
  ptype_list *p;
  if (lc==NULL || lc->SIG==NULL || (p=getptype(lc->SIG->USER, LOFIGCHAIN))==NULL) return;

  p->DATA=addchain((chain_list *)p->DATA, lc);
}

void gen_add_parallel_transistors (lofig_list *fig, corresp_t *table)
{
  lotrs_list *tr, *assoc_tr, *tr2, *as2;
  ptype_list *p, *pt;
  chain_list *cl;
  int count;

  for (tr = fig->LOTRS, count=0; tr; tr = tr->NEXT) 
    {
      if (getptype(tr->USER,GEN_TRANS_PARALLEL)!=NULL) continue;
      if ((assoc_tr = gen_corresp_trs (tr->TRNAME, table))) 
        {
          if ((p=getptype(assoc_tr->USER,MBK_TRANS_PARALLEL))!=NULL)
            {
              if (getptype(tr->USER,GEN_TRANS_PARALLEL)==NULL)
                {
                  for (cl=(chain_list *)p->DATA; cl!=NULL; cl=cl->NEXT)
                    {
                      as2=(lotrs_list *)cl->DATA;
                      if (as2!=assoc_tr)
                        {
                          tr2=addlotrs(fig, as2->TYPE, as2->X, as2->Y, as2->WIDTH, 
                                       as2->LENGTH, as2->PS, as2->PD, as2->XS, as2->XD, 
                                       tr->GRID->SIG, tr->DRAIN->SIG, tr->SOURCE->SIG,
                                       tr->BULK!=NULL?tr->BULK->SIG:NULL, as2->TRNAME);
                          tr2->MODINDEX=as2->MODINDEX;
                          if ((pt=getptype ( as2->USER, TRANS_FIGURE ))!=NULL)
                            tr2->USER=addptype(tr2->USER, TRANS_FIGURE, pt->DATA );
                          if ((pt = getptype (as2->USER, PARAM_CONTEXT)) != NULL)
                            tr2->USER = addptype (tr2->USER, PARAM_CONTEXT, eqt_dupvars((eqt_param *) pt->DATA));
                          tr2->USER=addptype(tr2->USER, GEN_TRANS_PARALLEL, as2);
                          addinlofigchain(tr2->GRID);
                          addinlofigchain(tr2->DRAIN);
                          addinlofigchain(tr2->SOURCE);
                          addinlofigchain(tr2->BULK);
                          count++;
//                          printf("adding // tr for '%s'/'%s' => '%s'\n",assoc_tr->TRNAME, tr->TRNAME, tr2->TRNAME);
                        }
                    }
                  tr->USER=addptype(tr->USER, GEN_TRANS_PARALLEL, NULL);
                }
            }
        }
      else 
        avt_errmsg(GNS_API_ERRMSG, "001", AVT_WARNING, tr->TRNAME);
 //       avt_error ("GNS", 0, AVT_WAR, "no corresponding transistor to %s\n", tr->TRNAME);
    }
}




//_____________________________________________________________________________
//_____________________________________________________________________________

int gen_strcount (char *sig, char c)
{
    int i = 0, n = 0;
    while (sig[i] != '\0') {
        if (sig[i] == c) n++;
        i++;
    }
    return n;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_update_signal (lofig_list *fig)
{
    losig_list *sig;
    chain_list *ch;
    char *name;
    int depth, length;
    int maxdepth, maxlength;

    for (sig = fig->LOSIG; sig; sig = sig->NEXT) {
        maxdepth = 1000; 
        maxlength = 1000000;
        for (ch = sig->NAMECHAIN; ch; ch = ch->NEXT) {
            depth = gen_strcount (ch->DATA, SEPAR);
            length = strlen (ch->DATA);
            if (depth < maxdepth) {
                maxdepth = depth;
                maxlength = length;
                name = (char*)ch->DATA;
            }
            else
            if (depth == maxdepth) {
                length = strlen (ch->DATA);
                if (length < maxlength) {
                    maxlength = length;
                    name = (char*)ch->DATA;
                }
            }
        }
        freechain (sig->NAMECHAIN);
        sig->NAMECHAIN = addchain (NULL, name);        
        if (sig->PRCN && (sig->PRCN->PWIRE || sig->PRCN->PCTC)) 
          avt_errmsg(GNS_API_ERRMSG, "002", AVT_WARNING, name);
          //avt_error ("GNS", 0, AVT_WAR, "'%s' has an RC network!\n", name);

    }
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_replace_lofig (lofig_list *fig_to_replace, lofig_list *new_fig)
{
    lofig_list *ptfig;

    ptfig = HEAD_LOFIG;
    while (ptfig->NEXT) {
        if (ptfig->NEXT == fig_to_replace) {
            new_fig->NEXT = fig_to_replace->NEXT;
            ptfig->NEXT = new_fig;
            break;
        }
        ptfig = ptfig->NEXT;
    }
}

//_____________________________________________________________________________
//_____________________________________________________________________________


static chain_list *new_li, *new_trs;
static void grab_new_loins(loins_list *li)
{
  new_li=addchain(new_li, li);
}
static void grab_new_lotrs(lotrs_list *trs)
{
  new_trs=addchain(new_trs, trs);
}

static inline int is_ins_to_supp(char *name, chain_list *trs2sup, chain_list *trs2keep)
{
  chain_list *ch;
  int i, j;
  char *insname;
  int aste;

  i=strlen(name);
  if (trs2keep==NULL)
    {
      for (ch = trs2sup; ch; ch = ch->NEXT)
        {
          insname=(char *)ch->DATA;
          j=strlen(insname);
          if (j>0 && insname[j-1]=='*') { aste=1; j--; } else aste=0;

          if (i >= j
              && (name[j]=='\0' || name[j]==GEN_SEPAR || aste)
              && strncmp(name,insname,j)==0
              )
            return 1;
        }
      return 0;
    }
//  if (trs2sup==NULL)
    {
      for (ch = trs2keep; ch; ch = ch->NEXT)
        {
          insname=(char *)ch->DATA;
          j=strlen(insname);
          if (j>0 && insname[j-1]=='*') { aste=1; j--; } else aste=0;

          if (i >= j
              && (name[j]=='\0' || name[j]==GEN_SEPAR || aste)
              && strncmp(name,(char *)ch->DATA,j)==0
              )
            return 0;
          else if (i < j
                   && (insname[i]=='\0' || insname[i]==GEN_SEPAR || aste)
                   && strncmp(name,insname,i)==0
                   )
            return 0;
        }          
      return 1;
    }

  if (trs2keep==NULL)
    return 0;
  return 1;
}
static inline int is_ins_to_keep(char *name, chain_list *trs2sup, chain_list *trs2keep)
{
  chain_list *ch;
  int i, j;
  char *insname;
  int aste;

  i=strlen(name);

  for (ch = trs2keep; ch; ch = ch->NEXT)
    {
      insname=(char *)ch->DATA;
      j=strlen(insname);
      if (j>0 && insname[j-1]=='*') { aste=1; j--; } else aste=0;
      
      if (i >= j
          && (name[j]=='\0' || name[j]==GEN_SEPAR || aste)
          && strncmp(name,(char *)ch->DATA,j)==0
          )
        return 1;
/*      else if (i < j
               && (insname[i]=='\0' || insname[i]==GEN_SEPAR || aste)
               && strncmp(name,insname,i)==0
               )
        return 1;*/
    }          
  return 0;
}

static void gen_addcrtforglobal(losig_list *ls, corresp_t *top)
{
  if (mbk_LosigIsGlobal(ls) && ls->TYPE=='I')
    if (getptype(ls->USER, CORRESP_FOR_GLOBAL)==NULL)
      ls->USER=addptype(ls->USER, CORRESP_FOR_GLOBAL, top);
}

void gen_rflattenlofig(lofig_list *ptfig, char concat, corresp_t *top, chain_list *trs2sup, chain_list *trs2keep)
{
  loins_list *p;
  lotrs_list *tr, *ltr;
  subinst_t *cl;
  char *r;
  ht *visited;
  chain_list /**instosupp=NULL, *trstosupp=NULL,*/ *ch;
  int add, countt=0, counti=0;
  long l;
  locon_list *lc;
  
//  static int tm=0;
  
  visited=addht(10000);
  
  for (p=ptfig->LOINS; p!=NULL; p=p->NEXT) 
    {
      for (cl=top->SUBINSTS; cl!=NULL; cl=cl->NEXT)
        {
//          crt=cl->CRT;
          if (cl->INSNAME==p->INSNAME) break;
        }
      if (cl==NULL) {printf("gen_rflattenlofig: instance '%s' was not found\n",p->INSNAME);EXIT(123);}
      if (cl->CRT->FLAGS & LOINS_IS_EXCLUDE)
        {
          addhtitem(visited, cl->CRT, 0);
        }
      p->USER=addptype(p->USER, TEMP_PTYPE, cl);
      p->FIGNAME=cl->CRT->GENIUS_FIGNAME;
      if (is_ins_to_supp(p->INSNAME, trs2sup, trs2keep))
        p->USER=addptype(p->USER, DELETE_ME_LATER, cl);
    }
  for (tr=ptfig->LOTRS; tr!=NULL; tr=tr->NEXT) 
    {
      l=gethtitem (top->TRANSISTORS, tr->TRNAME);
      if (l==EMPTYHT) EXIT(20);
      ltr = (lotrs_list *)l;
      addhtitem(visited, ltr, 0);
      tr->USER=addptype(tr->USER, TEMP_PTYPE, ltr);
      tr->FLAGS=0;
    }


  FlattenOnCreateLOTRS=grab_new_lotrs;
  FlattenOnCreateLOINS=grab_new_loins;

//  if (tm==0) tm=1,viewlofig(ptfig),savelofig(ptfig);
  p = ptfig->LOINS;
  while (p!=NULL) 
    {
      cl=(subinst_t *)getptype(p->USER, TEMP_PTYPE)->DATA;
      
      if (cl==NULL) { p=p->NEXT; continue; } // instance exclude a supprimer
      if (cl->FLAGS & LOINS_IS_BLACKBOX) 
        {
          if (getptype(p->USER, GEN_POST_BBOX_PTYPE)==NULL)
            p->USER=addptype(p->USER, GEN_POST_BBOX_PTYPE, cl);
          p=p->NEXT;
          continue; 
        }
      for (ch=GENIUS_CATAL; ch!=NULL && ch->DATA!=p->FIGNAME; ch=ch->NEXT) ;
      if (ch
          || getptype(p->USER, DELETE_ME_LATER)!=NULL
          /*|| is_ins_to_keep(p->INSNAME, trs2sup, trs2keep)*/) { p=p->NEXT; continue; }

      top=cl->CRT;
      new_li=new_trs=NULL;
      flattenlofig(ptfig, p->INSNAME, concat);
      
      for (ch=new_li; ch!=NULL; ch=ch->NEXT)        
        {
          p=(loins_list *)ch->DATA; 
          for (lc=p->LOCON; lc!=NULL; lc=lc->NEXT)
            if (lc->SIG!=NULL)
              gen_addcrtforglobal(lc->SIG, top);

          if (getptype(p->USER, TEMP_PTYPE)==NULL)
            {
              r=strrchr(p->INSNAME, GEN_SEPAR);
              r=namealloc(&r[1]);
              for (cl=top->SUBINSTS; cl!=NULL; cl=cl->NEXT)
                {
//                  crt=cl->CRT;
                  if (cl->INSNAME==r) break;
                }
              if (cl==NULL) {printf("gen_rflattenlofig: instance '%s' was not found\n",p->INSNAME);EXIT(123);}
              add=1;
              if (!(cl->FLAGS & LOINS_IS_BLACKBOX))
              {
                if (cl->CRT->FLAGS & LOINS_IS_EXCLUDE)
                  {
                    if (gethtitem(visited, cl->CRT)!=EMPTYHT)
                      {
                        counti++;
                        //                      instosupp=addchain(instosupp, p);
                        p->USER=addptype(p->USER, TEMP_PTYPE, NULL); // marquage instance exclude a supprimer
                        p->USER=addptype(p->USER, TEMP_PTYPE2, NULL); // marquage instance exclude a supprimer
                      add=0;
                      }
                    else
                      addhtitem(visited, cl->CRT, 0);
                  }
                
                p->FIGNAME=cl->CRT->GENIUS_FIGNAME;
              }
              if (add) 
                {
                  p->USER=addptype(p->USER, TEMP_PTYPE, cl);
                  if (is_ins_to_supp(p->INSNAME, trs2sup, trs2keep))
                    p->USER=addptype(p->USER, DELETE_ME_LATER, cl);
                }
            }
        }
      for (ch=new_trs; ch!=NULL; ch=ch->NEXT)        
        {
          tr=(lotrs_list *)ch->DATA; 
          tr->FLAGS=0;
          gen_addcrtforglobal(tr->GRID->SIG, top);
          gen_addcrtforglobal(tr->SOURCE->SIG, top);
          gen_addcrtforglobal(tr->DRAIN->SIG, top);
          if (tr->BULK!=NULL && tr->BULK->SIG)
            gen_addcrtforglobal(tr->BULK->SIG, top);
          if (getptype(tr->USER, TEMP_PTYPE)==NULL)
            {
              r=strrchr(tr->TRNAME, GEN_SEPAR);
              r=namealloc(&r[1]);
              l=gethtitem (top->TRANSISTORS, r);
              if (l==EMPTYHT) EXIT(20);
              ltr = (lotrs_list *)l;
              
              if (gethtitem(visited, ltr)!=EMPTYHT)
                {
//                  trstosupp=addchain(trstosupp, tr);
                  tr->FLAGS='R';                 
                  countt++;
                  tr->USER=addptype(tr->USER, TEMP_PTYPE, NULL); // marquage instance exclude a supprimer
                }
              else
                {
                  tr->USER=addptype(tr->USER, TEMP_PTYPE, ltr);
                  addhtitem(visited, ltr, 0);
                }
            }
        }
      freechain(new_li); freechain(new_trs);
      p = ptfig->LOINS;
    }
  
  delht(visited);

/*  
  for (ch=instosupp; ch!=NULL; ch=ch->NEXT)
    {
      printf("deleting sup observed %s\n",((loins_list *)ch->DATA)->INSNAME);
      delloins(ptfig, ((loins_list *)ch->DATA)->INSNAME);
    }
  
  
    for (ch=trstosupp; ch!=NULL; ch=ch->NEXT)
    {
    printf("deleting sup observed %s\n",((lotrs_list *)ch->DATA)->TRNAME);
    dellotrs(ptfig, (lotrs_list *)ch->DATA);
    }
*/

  gen_del_flagged_lotrs (ptfig, 'R');
  gen_del_flagged_loins (ptfig, TEMP_PTYPE2);

  if (countt>0 || counti>0)
    {
      printf("duplicate observed deleted : %d instances, %d transistors\n",counti,countt);
      lofigchain(ptfig);
    }

//  freechain(trstosupp);
//  freechain(instosupp);
  for (p=ptfig->LOINS; p!=NULL; p=p->NEXT)
    if (getptype(p->USER, TEMP_PTYPE)!=NULL) p->USER=delptype(p->USER, TEMP_PTYPE);
  
  for (tr=ptfig->LOTRS; tr!=NULL; tr=tr->NEXT)
    if (getptype(tr->USER, TEMP_PTYPE)!=NULL) tr->USER=delptype(tr->USER, TEMP_PTYPE);
  
  freechain(ptfig->MODELCHAIN);
  ptfig->MODELCHAIN = NULL;
  for (p=ptfig->LOINS; p!=NULL; p=p->NEXT) 
    {
      if (getptype(p->USER, DELETE_ME_LATER)!=NULL) continue;
      for (ch=ptfig->MODELCHAIN; ch!=NULL && ch->DATA!=p->FIGNAME; ch=ch->NEXT) ;
      if (ch==NULL) ptfig->MODELCHAIN=addchain(ptfig->MODELCHAIN, p->FIGNAME);
    }
  FlattenOnCreateLOTRS=NULL;
  FlattenOnCreateLOINS=NULL;

  freechain(GENIUS_CATAL);
  GENIUS_CATAL=NULL;
/*  savelofig(ptfig);
  EXIT(1);*/
}

int gen_exist_lofig (char *name)
{
	FILE *f;
    if (gethtitem (HT_LOFIG, name) != EMPTYHT)
        return 1;
	
    if ((f = mbkfopen (name, IN_LO, READ_TEXT))) {
        fclose (f);
        return 1;
    }

    return 0;
}

static void gen_add_fake_rc (lofig_list *fig)
{
    losig_list *sig;
    ptype_list *ptype;
    losig_list *gnd;
    chain_list *cl;

    gnd = gen_get_ground (fig);
    if (gnd==NULL) return;

    if (gnd->PRCN==NULL)
      addlorcnet (gnd);
    for (sig = fig->LOSIG; sig!=NULL; sig=sig->NEXT) 
      {
        if ((ptype = getptype (sig->USER, LOFIGCHAIN))!=NULL && ptype->DATA==NULL) continue;
        cl=(chain_list *)ptype->DATA;
        if (cl->NEXT==NULL)
          {
            if (sig->PRCN==NULL)
              {
                addlorcnet (sig);
                addloctc (sig, 0, gnd, 0, 1e-5);
              }
          }
      }
}

void gen_add_blackbox_capas (lofig_list *fig)
{
    loins_list *ptins;
    lofig_list *ptfig;
    locon_list *ptcon_ins, *ptcon_root;
    losig_list *gnd;
    double capa;

    gnd = gen_get_ground (fig);

    for (ptins = fig->LOINS; ptins; ptins = ptins->NEXT) {
        if (gen_exist_lofig (ptins->FIGNAME)) {
            ptfig = getlofig (ptins->FIGNAME, 'A');
            ptcon_ins = ptfig->LOCON;
            for (ptcon_root = ptins->LOCON; ptcon_root; ptcon_root = ptcon_root->NEXT) {
                if (ptcon_root->NAME != ptcon_ins->NAME) {
                    avt_errmsg(GNS_API_ERRMSG, "003", AVT_ERROR, ptins->INSNAME, ptins->FIGNAME);
/*                    avt_error ("GNS", 0, AVT_WAR, "gen_add_blackbox_capas: connectors discrepancy between `%s' and `%s'\n", 
                            ptins->INSNAME, ptins->FIGNAME);*/
                    break;
                }

                rcn_refresh_signal(fig, ptcon_ins->SIG);

                if (ptcon_ins->SIG->PRCN) {
                    capa = ptcon_ins->SIG->PRCN->RCCAPA;
                    if (ptcon_root->PNODE)
                        addloctc (ptcon_root->SIG, ptcon_root->PNODE->DATA, gnd, 1, capa);
                    else
                        addloctc (ptcon_root->SIG, 0, gnd, 0, capa);
                }
                
                if (ptcon_ins)
                    ptcon_ins = ptcon_ins->NEXT;
                else {
                    avt_errmsg(GNS_API_ERRMSG, "004", AVT_ERROR, ptins->INSNAME, ptins->FIGNAME);
/*                    avt_error ("GNS", 0, AVT_WAR, " - warning (gen_add_blackbox_capas): number of connectors differs between `%s' and `%s'\n", 
                            ptins->INSNAME, ptins->FIGNAME);*/
                    break;
                }
            }
        }
    }
}

lofig_list *gen_flatten (lofig_list *fig, corresp_t *table, chain_list *trs2sup, chain_list *trs2keep)
{
    lofig_list *head_lofig, *dupfig;
    ht         *ht_lofig;
    char        separ;

    // Contextualize HEAD_LOFIG ___________________________

    head_lofig = HEAD_LOFIG;
    HEAD_LOFIG = GEN_HEAD_LOFIG;

    ht_lofig = HT_LOFIG;
    HT_LOFIG = GEN_HT_LOFIG;

    // Duplicate hierarchical lofig _______________________

    dupfig = rduplofig (fig);
    gen_clean_RC (dupfig);
    gen_replace_lofig (fig, dupfig);
    
    // Flatten ____________________________________________

    lofigchain (dupfig);
    separ = SEPAR;
    SEPAR = GEN_SEPAR;

    gen_rflattenlofig (dupfig, 'Y', table, trs2sup, trs2keep);

    // Replace the flat lofig in HEAD_LOFIG by the hierarchical one

    gen_replace_lofig (dupfig, fig);

    // Update signal's names ______________________________

    gen_update_signal (dupfig);

    // Update connector's names ___________________________

    gen_update_sigcon (dupfig);

    // Update transistor sizes ____________________________

    gen_update_transistors (dupfig, table);
    
    // Release HEAD_LOFIG _________________________________

    SEPAR = separ;
    GEN_HEAD_LOFIG = HEAD_LOFIG;
    HEAD_LOFIG = head_lofig;

    GEN_HT_LOFIG = HT_LOFIG;
    HT_LOFIG = ht_lofig;

    dupfig->NEXT = NULL;
    return dupfig;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gns_FreeNetlist (lofig_list *netlist)
{
  gen_Destroy_MC(netlist);
  freeflatmodel (netlist);
}
    
//_____________________________________________________________________________
//_____________________________________________________________________________
//________________ Add RCs ____________________________________________________

int gen_take_this_rc (losig_list *ls, chain_list *lofigch, int rc)
{
    chain_list *ch;
    locon_list *con=NULL;

    if (ls->TYPE==INTERNAL)
      ch=NULL;
    else
      for (ch = lofigch; ch; ch = ch->NEXT) {
        con = (locon_list*)ch->DATA; 
        if (con->TYPE == 'E') break;
      }
    
    //internal connector
    if (!ch && (rc & (INTERNAL_RC | ALL_RC))!=0) return 1; 
    if (!con) return 0;

    // external connector
    switch (con->DIRECTION) {
        case 'O': 
        case 'T': 
            if (rc & (OUT_RC | ALL_RC)) return 1;
            break;
        case 'B': 
        case 'Z': 
            if (rc & (OUT_RC | IN_RC | ALL_RC)) return 1;
            break;
        case 'I': 
            if (rc & (IN_RC | ALL_RC)) return 1;
            break;
        case 'X': 
        default:
            if (rc & ALL_RC) return 1;
            fprintf (stderr, " - warning [gen_take_this_rc]: unknown connector orientation for '%s'\n", con->NAME);
            return 0;
    }
    return 0;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

int gen_search_external (chain_list *lofigch)
{
    chain_list *ch;
    locon_list *con;

    for (ch = lofigch; ch; ch = ch->NEXT) {
        con = (locon_list*)ch->DATA; 
        if (con->TYPE == 'E' && con->DIRECTION == 'I')
            return 1;
    }
    return 0;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

// adds a blocked transistor on the signal 'sig'. Connect 'sig' to the drain, 
// grid or source, according to whom is connected 'cir_ptcon'. 
//
void gen_add_blocked_trans (lofig_list *fig, locon_list *ptcon, losig_list *gnd, losig_list *vdd, locon_list *cir_ptcon)
{
    losig_list *drain, *grid, *source;
    lotrs_list *cir_tr, *tr;
    char type = 0;
    ptype_list *pt;

    if (cir_ptcon->TYPE == 'T') {
        cir_tr = cir_ptcon->ROOT;
        if (cir_tr->DRAIN == cir_ptcon) {
            drain = ptcon->SIG;
            source = MLO_IS_TRANSN(cir_tr->TYPE) ? gnd : vdd;
            grid = MLO_IS_TRANSN(cir_tr->TYPE) ? gnd : vdd;
        }
        else 
        if (cir_tr->SOURCE == cir_ptcon) {
            source = ptcon->SIG;
            drain = MLO_IS_TRANSN(cir_tr->TYPE) ? gnd : vdd;
            grid = MLO_IS_TRANSN(cir_tr->TYPE) ? gnd : vdd;
        }
        else 
        if (cir_tr->GRID == cir_ptcon) {
            source = MLO_IS_TRANSN(cir_tr->TYPE) ? gnd : vdd;
            drain = MLO_IS_TRANSN(cir_tr->TYPE) ? gnd : vdd;
            grid = ptcon->SIG;
        }

        if (MLO_IS_TRANSN(cir_tr->TYPE)) type = TRANSN;
        else
          if (MLO_IS_TRANSP(cir_tr->TYPE)) type = TRANSP;

        tr = addlotrs (fig, type, cir_tr->X, cir_tr->Y, cir_tr->WIDTH, 
                cir_tr->LENGTH, cir_tr->PS, cir_tr->PD, cir_tr->XS, cir_tr->XD, 
                grid, drain, source, NULL, cir_tr->TRNAME);
        tr->MODINDEX = cir_tr->MODINDEX;
        pt=getptype ( cir_tr->USER, TRANS_FIGURE );
        if ((pt=getptype ( cir_tr->USER, TRANS_FIGURE ))!=NULL)
          tr->USER=addptype(tr->USER, TRANS_FIGURE, pt->DATA );
        if ((pt = getptype (cir_tr->USER, PARAM_CONTEXT)) != NULL)
          tr->USER = addptype (tr->USER, PARAM_CONTEXT, eqt_dupvars((eqt_param *) pt->DATA));

        if (ptcon->PNODE) {
            if (cir_tr->DRAIN == cir_ptcon) {
                if (!tr->DRAIN->SIG->PRCN) addlorcnet (tr->DRAIN->SIG);
                setloconnode (tr->DRAIN, cir_ptcon->PNODE->DATA);
            }
            else 
            if (cir_tr->SOURCE == cir_ptcon) {
                if (!tr->SOURCE->SIG->PRCN) addlorcnet (tr->SOURCE->SIG);
                setloconnode (tr->SOURCE, cir_ptcon->PNODE->DATA);
            }
            else 
            if (cir_tr->GRID == cir_ptcon) {
                if (!tr->GRID->SIG->PRCN) addlorcnet (tr->GRID->SIG);
                setloconnode (tr->GRID, cir_ptcon->PNODE->DATA);
            }
        }
    }
}
//_____________________________________________________________________________
void gns_AddExternalTransistors (char *str)
{
    if (!strcmp (str, "dynamic"))
        GEN_SIM_OUTLOAD = GEN_DYNAMIC_OUTLOAD;
    else
    if (!strcmp (str, "transistor"))
        GEN_SIM_OUTLOAD = GEN_TRANSISTOR_OUTLOAD;
    else
    if (!strcmp (str, "none"))
        GEN_SIM_OUTLOAD = GEN_NO_OUTLOAD;
    else
      avt_errmsg(GNS_API_ERRMSG, "005", AVT_ERROR);
//      avt_error ("GNS", 0, AVT_WAR, "gns_AddExternalTransistors, wrong parameter\n");
}
//_____________________________________________________________________________

void gen_rcenv ()
{
    static int env = 0;
    char *str;

    if (!env) env = 1;
    else return;

    elpenv ();
    mccenv ();

    // How to add external transistors ?
    if ((str = avt_gethashvar ("GEN_SIM_OUTLOAD"))) {
        if (!strcasecmp (str, "dynamic"))
          GEN_SIM_OUTLOAD = GEN_DYNAMIC_OUTLOAD;
        else if (!strcasecmp (str, "transistor"))
          GEN_SIM_OUTLOAD = GEN_TRANSISTOR_OUTLOAD;
        else if (!strcasecmp (str, "none"))
          GEN_SIM_OUTLOAD = GEN_NO_OUTLOAD;
    }
}

//_____________________________________________________________________________

losig_list *gen_get_ground (lofig_list *fig)
{
    losig_list *sig;
  
    for (sig = fig->LOSIG; sig; sig = sig->NEXT) 
        if (mbk_LosigIsVSS(sig))
            break;

    return sig;
}

//_____________________________________________________________________________

losig_list *gen_get_alim (lofig_list *fig)
{
    losig_list *sig;

    for (sig = fig->LOSIG; sig; sig = sig->NEXT) 
        if (mbk_LosigIsVDD (sig))
            break;

    return sig;
}

//_____________________________________________________________________________

double gen_get_transistor_capa (locon_list *cir_ptcon)
{
    double trcapa = 0.0;
    lotrs_list *trs, *shrinked_trs;
    mbkContext tmpctx;
    
    memcpy(&tmpctx, genius_external_getcontext(), sizeof(tmpctx));
    mbkSwitchContext(&tmpctx); // passage dans le context NORMAL pour les modeles de transistors

    trs = (lotrs_list*)cir_ptcon->ROOT;
    shrinked_trs = trs;
//    shrinked_trs = duplotrs (trs);
    ELP_CALC_ONLY_CAPA = 1;
    elpLoadOnceElp();
    //elpLotrsShrink (shrinked_trs);
    if (trs->GRID == cir_ptcon)
        trcapa = elpGetCapaFromLocon (shrinked_trs->GRID,ELP_CAPA_TYPICAL,elpTYPICAL);
    else
    if (trs->DRAIN == cir_ptcon)
        trcapa = elpGetCapaFromLocon (shrinked_trs->DRAIN,ELP_CAPA_TYPICAL,elpTYPICAL);
    else
    if (trs->SOURCE == cir_ptcon)
        trcapa = elpGetCapaFromLocon (shrinked_trs->SOURCE,ELP_CAPA_TYPICAL,elpTYPICAL);

//    mbkfree (shrinked_trs);
    ELP_CALC_ONLY_CAPA = 0;

    mbkSwitchContext(&tmpctx);
    return trcapa;

}

//_____________________________________________________________________________

void gen_add_equicapa_from_cir (losig_list *ls, losig_list *gnd, locon_list *cir_ptcon)
{
    double trcapa = 0.0;
    
    if (gnd==NULL) return;

    trcapa = gen_get_transistor_capa (cir_ptcon) ;

    if (trcapa>=1e-5) // 1e-17 Farad
      {
        if (!ls->PRCN)
          ls->PRCN = addlorcnet (ls);
        if (!gnd->PRCN)
          gnd->PRCN = addlorcnet (gnd);
        
        if ((/*ls->PRCN->PCTC || */ls->PRCN->PWIRE) && cir_ptcon->PNODE!=NULL) // new lofig constructed with RCs
          addloctc (ls, cir_ptcon->PNODE->DATA, gnd, 1, trcapa);
        else // new lofig constructed without RCs
          addloctc (ls, 0, gnd, 0, trcapa);
      }
}


chain_list *_gen_graball_lotrs(corresp_t *crt, chain_list *cur)
{
  chain_list *trl, *cl, *ch;
  lotrs_list *tr, *as2;
  subinst_t *si;
  ptype_list *p;

  if (crt->TRANSISTORS!=NULL)
    {
      trl=GetAllHTElems(crt->TRANSISTORS);
      for (ch=trl; ch!=NULL; ch=ch->NEXT)
        {
          tr=(lotrs_list *)ch->DATA;
          if (getptype(tr->USER, GEN_MARK_PTYPE)==NULL)
            {
              if ((p=getptype(tr->USER,MBK_TRANS_PARALLEL))!=NULL)
                {
                  for (cl=(chain_list *)p->DATA; cl!=NULL; cl=cl->NEXT)
                    {
                      as2=(lotrs_list *)cl->DATA;
                      cur=addchain(cur, as2);
                      as2->USER=addptype(as2->USER, GEN_MARK_PTYPE, NULL);
                    }
                }
              else
                {
                  cur=addchain(cur, tr);
                  tr->USER=addptype(tr->USER, GEN_MARK_PTYPE, NULL);
                }
            }
        }
      freechain(trl);
    }        
  
  for (si=crt->SUBINSTS; si!=NULL; si=si->NEXT)
    cur=_gen_graball_lotrs(si->CRT, cur);
  return cur;
}

chain_list *gen_graball_lotrs(corresp_t *crt)
{
  chain_list *cl, *ch;
  lotrs_list *tr;
  ch=cl=_gen_graball_lotrs(crt, NULL);
  while (ch!=NULL)
    {
      tr=(lotrs_list *)ch->DATA;
      tr->USER=delptype(tr->USER, GEN_MARK_PTYPE);
      ch=ch->NEXT;
    }
  return cl;
}


//_____________________________________________________________________________
//_____________________________________________________________________________

void gns_ShowOutsideInfo(char *signame, FILE *f)
{
  lotrs_list *cir_tr;
  losig_list *sig, *cir_sig;
  chain_list *ch, *cir_ch, *lofigch, *cir_lofigch;
  ptype_list *ptype;
  locon_list *cir_ptcon;
  chain_list *trsl=NULL;

  // Local lofigchain
  if (GENIUS_GLOBAL_LOFIG->LOSIG==NULL || getptype(GENIUS_GLOBAL_LOFIG->LOSIG->USER, LOFIGCHAIN_LOCAL)==NULL)
    lofigchain_local (GENIUS_GLOBAL_LOFIG);

  sig=mbk_getlosigbyname(CUR_HIER_LOFIG, signame);
  // corresponding signal
  cir_sig = gen_corresp_sig (signame, CUR_CORRESP_TABLE);

  fprintf(f, "* Objects connected outside of instance\n* on signal '%s' (%s in the circuit)\n", gen_losigname(sig), gen_losigname(cir_sig));
  fprintf(f, "_node_ _on object___________________\n");
  fprintf(f, "______ _____________________________\n");
  // lofigchains of current and corresponding signals
  lofigch = gen_get_lofigchain (sig);

  if ((ptype = getptype (cir_sig->USER, LOFIGCHAIN_LOCAL)))
    cir_lofigch = (chain_list*)ptype->DATA;
  else
    cir_lofigch = NULL;

  trsl=gen_graball_lotrs(CUR_CORRESP_TABLE);

  // for each connector on the model signal, retrieve the corresponding
  // connector in the circuit, flag it and duplicate its lonodes.
  for (ch = trsl; ch; ch = ch->NEXT) 
    {
      cir_tr = (lotrs_list*)ch->DATA;
      cir_tr->USER = addptype (cir_tr->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
    }

  // connect the remaining transistors to ext connector (only one)
  for (cir_ch = cir_lofigch; cir_ch; cir_ch = cir_ch->NEXT) 
    {  
      cir_ptcon = (locon_list*)cir_ch->DATA;
/*      if (cir_ptcon->TYPE == 'I')
        {
          fprintf(f, " %4ld  %10s of Instance   '%s'\n", cir_ptcon->PNODE!=NULL?cir_ptcon->PNODE->DATA:-1,cir_ptcon->NAME, ((loins_list *)cir_ptcon->ROOT)->INSNAME);
        }
      else*/ if (cir_ptcon->TYPE == 'T')
        {
          if (getptype (((lotrs_list *)cir_ptcon->ROOT)->USER, GENIUS_VISITED_LOCON_PTYPE)==NULL) 
            {
              fprintf(f, " %4ld  %10s of Transistor '%s'\n", cir_ptcon->PNODE!=NULL?cir_ptcon->PNODE->DATA:-1,cir_ptcon->NAME, ((lotrs_list *)cir_ptcon->ROOT)->TRNAME!=NULL?((lotrs_list *)cir_ptcon->ROOT)->TRNAME:"?");
            }
        }
      else if (cir_ptcon->TYPE == 'E')
        {
          fprintf(f, " %4ld    External connector '%s'\n", cir_ptcon->PNODE!=NULL?cir_ptcon->PNODE->DATA:-1,cir_ptcon->NAME);
        }
    }
     
  for (ch = trsl; ch; ch = ch->NEXT) 
    {
      cir_tr = (lotrs_list*)ch->DATA;
      cir_tr->USER = delptype (cir_tr->USER, GENIUS_VISITED_LOCON_PTYPE);
    }
  freechain(trsl);
}

void gen_add_external_capas (lofig_list* fig, corresp_t *table)
{
    //lonode_list *ptnode;
    lotrs_list *tr, *cir_tr;
    losig_list *sig, *cir_sig, *gnd, *vdd;
    chain_list *ch, *cir_ch, *lofigch, *cir_lofigch;
    ptype_list *ptype, *p;
    locon_list *ptcon, *cir_ptcon;

    if (GEN_SIM_OUTLOAD == GEN_NO_OUTLOAD || GEN_SIM_OUTLOAD == GEN_UNKNOWN_OUTLOAD) 
        return;

    // Local lofigchain
    if (GENIUS_GLOBAL_LOFIG->LOSIG==NULL || getptype(GENIUS_GLOBAL_LOFIG->LOSIG->USER, LOFIGCHAIN_LOCAL)==NULL)
      lofigchain_local (GENIUS_GLOBAL_LOFIG);
    
    gnd = gen_get_ground (fig);
    vdd = gen_get_alim (fig);

    // add rcs
    for (sig = fig->LOSIG; sig; sig = sig->NEXT) {

        // corresponding signal
        cir_sig = gen_corresp_sig (gen_losigname (sig), table);
        if (!cir_sig) continue;

        // lofigchains of current and corresponding signals
        lofigch = gen_get_lofigchain (sig);

        if ((ptype = getptype (cir_sig->USER, LOFIGCHAIN_LOCAL)))
            cir_lofigch = (chain_list*)ptype->DATA;
        else
            cir_lofigch = NULL;

        // search for input connector in lofigchain - in this case, do not drive RCs
        if (gen_search_external (lofigch)) continue;
            
        // for each connector on the model signal, retrieve the corresponding
        // connector in the circuit, flag it and duplicate its lonodes.
        for (ch = lofigch; ch; ch = ch->NEXT) {
            ptcon = (locon_list*)ch->DATA;
            if (ptcon->TYPE == 'T') {
                tr = (lotrs_list*)ptcon->ROOT;

                // prise en compte des transistors parallels
                if ((p=getptype(tr->USER,GEN_TRANS_PARALLEL))!=NULL && p->DATA!=NULL)
                  cir_tr = (lotrs_list *)p->DATA;
                else
                  cir_tr = gen_corresp_trs (tr->TRNAME, table);
                // ----

//                cir_tr = gen_corresp_trs (tr->TRNAME, table);
                if (cir_tr->SOURCE->SIG == cir_sig) {
                    cir_ptcon = cir_tr->SOURCE;
                    cir_ptcon->USER = addptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                }
                else
                if (cir_tr->DRAIN->SIG == cir_sig) {
                    cir_ptcon = cir_tr->DRAIN;
                    cir_ptcon->USER = addptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                }
                else
                if (cir_tr->GRID->SIG == cir_sig) {
                    cir_ptcon = cir_tr->GRID;
                    cir_ptcon->USER = addptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                }
            } 
        }

        // connect the remaining transistors to ext connector (only one)
        for (ch = lofigch; ch; ch = ch->NEXT) {
            ptcon = (locon_list*)ch->DATA;
            if (ptcon->TYPE == 'E') { // external connector of model
                for (cir_ch = cir_lofigch; cir_ch; cir_ch = cir_ch->NEXT) {  
                    cir_ptcon = (locon_list*)cir_ch->DATA;                  
                    if (cir_ptcon->TYPE == 'T')
                      {
                        if (!getptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE)) {
                          // add capacitances
                          if (GEN_SIM_OUTLOAD == GEN_DYNAMIC_OUTLOAD)
                            gen_add_equicapa_from_cir (ptcon->SIG, gnd, cir_ptcon);
                          else
                            // add external transistors
                            if (GEN_SIM_OUTLOAD == GEN_TRANSISTOR_OUTLOAD)
                              gen_add_blocked_trans (fig, ptcon, gnd, vdd, cir_ptcon);
                        }
                      }
                }
                break; // only one
            }
        }

        // clean ptypes
        for (ch = lofigch; ch; ch = ch->NEXT) {
            ptcon = (locon_list*)ch->DATA;
            if (ptcon->TYPE == 'T') {
                tr = (lotrs_list*)ptcon->ROOT;

                 // prise en compte des transistors parallels
                if ((p=getptype(tr->USER,GEN_TRANS_PARALLEL))!=NULL && p->DATA!=NULL)
                  cir_tr = (lotrs_list *)p->DATA;
                else
                  cir_tr = gen_corresp_trs (tr->TRNAME, table);
                // ----
//                cir_tr = gen_corresp_trs (tr->TRNAME, table);
                if (cir_tr->SOURCE->SIG == cir_sig) {
                    cir_ptcon = cir_tr->SOURCE;
                    cir_ptcon->USER = delptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE);
                }
                else
                if (cir_tr->DRAIN->SIG == cir_sig) {
                    cir_ptcon = cir_tr->DRAIN;
                    cir_ptcon->USER = delptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE);
                }
                else
                if (cir_tr->GRID->SIG == cir_sig) {
                    cir_ptcon = cir_tr->GRID;
                    cir_ptcon->USER = delptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE);
                }
            } 
        }
    }

//    dellofigchain_local (GENIUS_GLOBAL_LOFIG);
}

//_____________________________________________________________________________

void gen_make_reverse_corresp (lofig_list *fig, corresp_t *table)
{
    losig_list *sig;

    GEN_REVERSE_CORRESP = addht (10003);
    for (sig = fig->LOSIG; sig; sig = sig->NEXT)
      if (sig->FLAGS!='D')
        addhtitem (GEN_REVERSE_CORRESP, gen_corresp_sig (gen_losigname (sig), table), (long)sig);
}

//_____________________________________________________________________________

void gen_del_reverse_corresp ()
{
    delht (GEN_REVERSE_CORRESP);
}

//_____________________________________________________________________________

losig_list *gen_reverse_corresp_sig (losig_list *rev_sig)
{
    long sig;

    sig = gethtitem (GEN_REVERSE_CORRESP, rev_sig);
    if (sig != EMPTYHT)
        return (losig_list*)sig;
    else
        return NULL;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
void gen_Destroy_MC(lofig_list *fig)
{
  ptype_list *p;
  more_capa *mc;
  if ((p=getptype(fig->USER, MORE_INTERNAL_CAPA))!=NULL)
    {
      mc=(more_capa *)p->DATA;
      mc->count--;
      if (mc->count==0)
        {
          chain_list *cl, *ch;
          ch=cl=GetAllHTElems(mc->per_signal_internal_locons);
          while (cl!=NULL) { freechain((chain_list *)cl->DATA); cl=cl->NEXT; }
          freechain(ch);
          mbkfree(mc);
        }
      fig->USER=delptype(fig->USER, MORE_INTERNAL_CAPA);
    }
}



void gen_add_RC (lofig_list* fig, corresp_t *table, int rc, ctk_struct *cs)
{
    lotrs_list *tr, *cir_tr;
    losig_list *sig, *cir_sig, *gnd, *vdd, *ls;
    losig_list *x_sig, *x_cir_sig;
    lowire_list *wire;
    loctc_list *ctc;
    chain_list *ch, *cir_ch, *lofigch, *cir_lofigch, *rclist;
    ptype_list *ptype, *p;
    locon_list *ptcon, *cir_ptcon;
    num_list *pnode;
    ht *node_ht;
    int x_node;
    loins_list *ins;
    subinst_t *si;
    int all_in_model;
    chain_list *cl, *lst;
    locon_list *lc;
    float coef, ctkcoef, gndctkratio;
    ctk_PerVictimParam *ctkvp;
    more_capa *mc=NULL;
    long l;
    chain_list *all_locons;
    num_list *nl;
    int MAX_NODE=10000000;

    if (V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_LTSPICE) MAX_NODE=64;
    
    //viewlofig (GENIUS_GLOBAL_LOFIG);
    gen_make_reverse_corresp (fig, table);

    // Local lofigchain
    if (GENIUS_GLOBAL_LOFIG->LOSIG==NULL || getptype(GENIUS_GLOBAL_LOFIG->LOSIG->USER, LOFIGCHAIN_LOCAL)==NULL)
      lofigchain_local (GENIUS_GLOBAL_LOFIG);

    // ***** temporaire *******
    patchlofig_if_needed(GENIUS_GLOBAL_LOFIG);

    if (fig->LOSIG==NULL || getptype(fig->LOSIG->USER, LOFIGCHAIN)==NULL)
      lofigchain (fig);

    if (!(vdd = gen_get_alim (fig))) {
        fprintf (stderr, " - warning [gen_add_RC]: no alimentation signal found\n");
        return;
    }

    if (!(gnd = gen_get_ground (fig))) {
        fprintf (stderr, " - warning [gen_add_RC]: no ground signal found\n");
        return;
    }
    
    if (!vdd->PRCN) vdd->PRCN = addlorcnet (vdd);
    if (!gnd->PRCN) gnd->PRCN = addlorcnet (gnd);

    if ((p=getptype(fig->USER, MORE_INTERNAL_CAPA))!=NULL)
      mc=(more_capa *)p->DATA;


    // add rcs
    for (sig = fig->LOSIG; sig; sig = sig->NEXT) {
        if (sig == vdd || sig == gnd || sig->FLAGS=='D') continue;
        if (gen_isalim(sig)) continue;
        // corresponding signal
        cir_sig = gen_corresp_sig (gen_losigname (sig), table);

        if (!cir_sig)
          {
            if (gen_wasunused()) continue;
            fprintf(stderr,"A correspondance could not be found for signal '%s'\n", gen_losigname (sig));
            EXIT (9); 
          }
        if (!cir_sig->PRCN) { continue;}

                
        rcn_refresh_signal(GENIUS_GLOBAL_LOFIG, cir_sig);

        if (!cir_sig->PRCN->PWIRE && !cir_sig->PRCN->PCTC)
          {
            // add internal capa
            if (mc!=NULL && (l=gethtitem(mc->per_signal_internal_locons, gen_losigname(sig)))!=EMPTYHT)
              {
                for (cl=(chain_list *)l; cl!=NULL; cl=cl->NEXT)
                  {
                    lc=(locon_list *)cl->DATA;
                    if (lc->TYPE=='T' && getptype(lc->USER, GENIUS_VISITED_LOCON_PTYPE)==NULL)
                      {
                        lc->USER = addptype (lc->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                        all_locons=addchain(all_locons, cir_ptcon);
                        gen_add_equicapa_from_cir (sig, gnd, lc);
                      }
                  }
              }
            continue;
          }

        // lofigchains of current and corresponding signals
        lofigch = gen_get_lofigchain (sig);

        if ((ptype = getptype (cir_sig->USER, LOFIGCHAIN_LOCAL)))
            cir_lofigch = (chain_list*)ptype->DATA;
        else
            cir_lofigch = NULL;

        // search for input connector in lofigchain - in this case, do not drive RCs
        if (!gen_take_this_rc (sig, lofigch, rc)) continue;
            
        // duplicate PRCN
        if (!mbk_LosigIsVSS(sig))
            if (!sig->PRCN) sig->PRCN = addlorcnet (sig);

        rclist = NULL;
        for (wire = cir_sig->PRCN->PWIRE; wire; wire = wire->NEXT) // to reverse wires
            rclist = addchain (rclist, wire);

        // retreive the ctc coef
        rcxi_getcoefctc(GENIUS_GLOBAL_LOFIG, cir_sig, RCXI_UP, 1e-12, RCXI_MAX, &coef);

        // parametres ctk pour la victime
        ctkvp=NULL;
        if (cs!=NULL)
          {
            long l;
            if ((l=gethtitem_v2(cs->victim_ctk_info, cir_sig))!=EMPTYHT)
              ctkvp=(ctk_PerVictimParam *)l;
          }

        //printf(" %s %g\n",gen_losigname(sig), coef);
        for (ch = rclist; ch; ch = ch->NEXT) {
            wire = (lowire_list*)ch->DATA;
            addlowire (sig, 0, wire->RESI, wire->CAPA*coef, wire->NODE1, wire->NODE2);
        }
        freechain (rclist);
        rclist = NULL;
        for (ch = cir_sig->PRCN->PCTC; ch; ch = ch->NEXT) // to reverse ctcs
            rclist = addchain (rclist, ch->DATA);
        for (ch = rclist; ch; ch = ch->NEXT) {
            ctc = (loctc_list*)ch->DATA;
            if (ctc->SIG1 == cir_sig) x_cir_sig = ctc->SIG2;
            else x_cir_sig = ctc->SIG1;
/*            if (strcasecmp(gen_losigname(sig),"sense_amplifier.base3.sig")==0)
              printf("%s %d %s %d %g\n",gen_losigname(ctc->SIG1),ctc->NODE1,gen_losigname(ctc->SIG2),ctc->NODE2,ctc->CAPA);*/
            if (mbk_LosigIsVSS(ctc->SIG1) || mbk_LosigIsVSS(ctc->SIG2)) // ground capa
                addloctc (sig, rcn_ctcnode (ctc, cir_sig), gnd, 1, ctc->CAPA*coef);
            else
            if (mbk_LosigIsVDD(ctc->SIG1) || mbk_LosigIsVDD(ctc->SIG2)) // ground capa
                addloctc (sig, rcn_ctcnode (ctc, cir_sig), vdd, 1, ctc->CAPA*coef);
            else 
              { // xtalk capa
                // if it's a node to 0, we ignore the capa except for alims.
                // The ctccoef will take care of the capa effect
                // on the ground capacitances
                if (rcn_ctcnode (ctc, x_cir_sig)==0) continue; 
                gndctkratio=1;
                if (cs!=NULL && cs->crosstalk_to_ground) gndctkratio=cs->crosstalk_to_ground_ratio;
                if ((rc | XTALK_TO_GND)!=0 || (cs!=NULL && cs->crosstalk_to_ground))
                    addloctc (sig, rcn_ctcnode (ctc, cir_sig), gnd, 1, ctc->CAPA*gndctkratio);
                else {
                    if (ctc->SIG1 == cir_sig) x_cir_sig = ctc->SIG2;
                    else x_cir_sig = ctc->SIG1;
                    x_sig = gen_reverse_corresp_sig (x_cir_sig);
                    if (x_sig) {
                        x_node = rcn_ctcnode (ctc, x_cir_sig);
                        if (!x_sig->PRCN) x_sig->PRCN = addlorcnet (x_sig);

                    
                        addloctc (sig, rcn_ctcnode (ctc, cir_sig), x_sig, x_node, ctc->CAPA);


                    } else {
                        /*
                        avt_error ("GNS", 0, AVT_WAR, "Warning: xtalk capacitance on `%s' (%g pF) reported to ground\n", 
                            gen_losigname (sig), ctc->CAPA);
                        fprintf (stdout, "         (pending signal outside of model)\n");
                        */
                      if (cs!=NULL)
                        ctkcoef=cs->external_crosstalk_to_ground_ratio;
                      else
                        ctkcoef=1;
                      if (ctkvp!=NULL)
                        {
                          long l;
                          ctk_VictimVSAgressorParam *vvsap;
                          if ((l=gethtitem_v2(ctkvp->agressor_ctk_info, x_cir_sig))!=EMPTYHT)
                            {
                              vvsap=(ctk_VictimVSAgressorParam *)l;
                              ctkcoef=vvsap->ctk_ratio;
                            }
                        }
                      addloctc (sig, rcn_ctcnode (ctc, cir_sig), gnd, 1, ctc->CAPA*ctkcoef);
                    }
                }
            }
        }
        freechain (rclist);

        all_locons=NULL;

        // for each connector on the model signal, retrieve the corresponding
        // connector in the circuit, flag it and duplicate its lonodes.
        for (ch = lofigch; ch; ch = ch->NEXT) {
            ptcon = (locon_list*)ch->DATA;
            if (ptcon->TYPE == 'T') {
                tr = (lotrs_list*)ptcon->ROOT;

                // prise en compte des transistors parallels
                if ((p=getptype(tr->USER,GEN_TRANS_PARALLEL))!=NULL && p->DATA!=NULL)
                  cir_tr = (lotrs_list *)p->DATA;
                else
                  cir_tr = gen_corresp_trs (tr->TRNAME, table);
                // ----

                if (cir_tr->SOURCE->SIG == cir_sig && (ptcon->NAME==MBK_SOURCE_NAME || ptcon->NAME==MBK_DRAIN_NAME)) {
                    cir_ptcon = cir_tr->SOURCE;
                    cir_ptcon->USER = addptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                    all_locons=addchain(all_locons, cir_ptcon);
                    for (pnode = cir_ptcon->PNODE; pnode; pnode = pnode->NEXT)
                        setloconnode (ptcon, pnode->DATA);
                }
                else
                if (cir_tr->DRAIN->SIG == cir_sig  && (ptcon->NAME==MBK_SOURCE_NAME || ptcon->NAME==MBK_DRAIN_NAME)) {
                    cir_ptcon = cir_tr->DRAIN;
                    cir_ptcon->USER = addptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                    all_locons=addchain(all_locons, cir_ptcon);
                    for (pnode = cir_ptcon->PNODE; pnode; pnode = pnode->NEXT)
                        setloconnode (ptcon, pnode->DATA);
                }
                else
                if (cir_tr->GRID->SIG == cir_sig && ptcon->NAME==MBK_GRID_NAME) {
                    cir_ptcon = cir_tr->GRID;
                    cir_ptcon->USER = addptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                    all_locons=addchain(all_locons, cir_ptcon);
                    for (pnode = cir_ptcon->PNODE; pnode; pnode = pnode->NEXT)
                        setloconnode (ptcon, pnode->DATA);
                }
                else
                if (cir_tr->BULK) {
                    if (cir_tr->BULK->SIG == cir_sig && ptcon->NAME==MBK_BULK_NAME) {
                        cir_ptcon = cir_tr->BULK;
                        cir_ptcon->USER = addptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                        all_locons=addchain(all_locons, cir_ptcon);
                        for (pnode = cir_ptcon->PNODE; pnode; pnode = pnode->NEXT)
                            setloconnode (ptcon, pnode->DATA);
                    }
                }
                else {
                    avt_errmsg(GNS_API_ERRMSG, "006", AVT_ERROR, tr->TRNAME, cir_tr->TRNAME ? cir_tr->TRNAME : "?", gen_losigname (sig), gen_losigname (cir_sig));
              /*      avt_error ("GNS", 0, AVT_WAR, " - can't reconnect transistor `%s' (`%s') to signal `%s' (`%s')\n", tr->TRNAME, 
                            cir_tr->TRNAME ? cir_tr->TRNAME : "?", gen_losigname (sig), gen_losigname (cir_sig));*/
                }
            } 
            else
            if (ptcon->TYPE == 'I') {
              ins = (loins_list*)ptcon->ROOT;
              if ((p=getptype(ins->USER, DELETE_ME_LATER))==NULL)
                {
                  si = gen_GetCorrespondingInstance (ins->INSNAME, table);
                  ls=gen_corresp_sig (ptcon->NAME, si->CRT);
                  cl = lst = GrabAllConnectorsThruCorresp (ptcon->NAME, ls, si->CRT, NULL);
                  while (cl) {
                    lc = (locon_list*)cl->DATA;
                    lc->USER = addptype (lc->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                    all_locons=addchain(all_locons, lc);
                    for (pnode = lc->PNODE; pnode; pnode = pnode->NEXT) setloconnode (ptcon, pnode->DATA);
                    cl = cl->NEXT;
                  }
                  freechain (lst);
                }
            }
        }

        // add internal capa
        if (mc!=NULL && (l=gethtitem(mc->per_signal_internal_locons, gen_losigname(sig)))!=EMPTYHT)
          {
            for (cl=(chain_list *)l; cl!=NULL; cl=cl->NEXT)
              {
                lc=(locon_list *)cl->DATA;
                if (lc->TYPE=='T' && getptype(lc->USER, GENIUS_VISITED_LOCON_PTYPE)==NULL)
                  {
                    lc->USER = addptype (lc->USER, GENIUS_VISITED_LOCON_PTYPE, NULL);
                    all_locons=addchain(all_locons, lc);
                    gen_add_equicapa_from_cir (sig, gnd, lc);
                  }
              }
          }

        if (sig->TYPE==EXTERNAL)
          {
            // connect the remaining transistors to ext connector (only one)
            for (ch = lofigch; ch; ch = ch->NEXT) {
              ptcon = (locon_list*)ch->DATA;
              if (ptcon->TYPE == 'E') {
                int cnt=0;
                node_ht = addht (1000);
                all_in_model = 1;
                for (cir_ch = cir_lofigch; cir_ch; cir_ch = cir_ch->NEXT) {
                  cir_ptcon = (locon_list*)cir_ch->DATA;
                  if (!getptype (cir_ptcon->USER, GENIUS_VISITED_LOCON_PTYPE)) {
                    for (nl=cir_ptcon->PNODE; nl!=NULL && cnt<MAX_NODE; nl=nl->NEXT)
                      {
                        //                    if (cir_ptcon->PNODE) {
                        if (gethtitem (node_ht, (void*)nl->DATA) == EMPTYHT) {
                          setloconnode (ptcon, nl->DATA);
                          addhtitem (node_ht, (void*)nl->DATA, 1);
                          all_in_model = 0;
                          cnt++;
                        }
                      }
                  }
                }
                if (all_in_model)
                {
                  avt_errmsg(GNS_API_ERRMSG, "007", AVT_WARNING, ptcon->NAME, fig->NAME);
                  for (cir_ch = cir_lofigch; cir_ch; cir_ch = cir_ch->NEXT) {
                    cir_ptcon = (locon_list*)cir_ch->DATA;
                    for (nl=cir_ptcon->PNODE; nl!=NULL; nl=nl->NEXT)
                      {
                          setloconnode (ptcon, nl->DATA);
                          addhtitem (node_ht, (void*)nl->DATA, 1);
                          break;
                      }
                    if (nl!=NULL) break;
                  }

                }
                delht (node_ht);
                break; // only one
              }
            }
          }

        // clean ptypes
        for (cl=all_locons; cl!=NULL; cl=cl->NEXT)
          {
            lc=(locon_list *)cl->DATA;
            while (getptype(lc->USER, GENIUS_VISITED_LOCON_PTYPE)!=NULL)
              lc->USER = delptype (lc->USER, GENIUS_VISITED_LOCON_PTYPE);
          }
        
        freechain(all_locons);

    }
    gen_Destroy_MC(fig);
//    dellofigchain_local (GENIUS_GLOBAL_LOFIG);

    // mise a jour des pnode des connecteurs d'alim
    for (sig = fig->LOSIG; sig; sig = sig->NEXT) 
      {
        if (gen_isalim(sig))
          {
            if (sig->PRCN)
              {
                for (ch=(chain_list *)getptype(sig->USER, LOFIGCHAIN)->DATA; ch!=NULL; ch=ch->NEXT)
                  {
                    lc=(locon_list *)ch->DATA;
                    if (lc->PNODE==NULL)
                      setloconnode (lc, sig->PRCN->NBNODE-1);
                  }
              }
          }        
      }


    gen_del_reverse_corresp ();
}

//_____________________________________________________________________________
//_____________________________________________________________________________

lotrs_list *gen_get_transistor (lofig_list *netlist, char *tr_name)
{
    char *trname = namealloc (tr_name);
    lotrs_list *tr;

    for (tr = netlist->LOTRS; tr; tr = tr->NEXT)
        if (tr->TRNAME == trname) 
            return tr;

    return NULL;
}


//_____________________________________________________________________________
//_____________________________________________________________________________
    
locon_list *gen_get_connector (lofig_list *netlist, char *con_name)
{
    char *subname, *trname, buf[2048];
    char *cname;
    locon_list *c;
    lotrs_list *tr;
    loins_list *li;
    

    strcpy (buf, gen_makesignalname(con_name));
    subname = strrchr (buf, GEN_SEPAR);

    if (!subname) { // external connector
      cname=namealloc(buf);
      for (c = netlist->LOCON; c; c = c->NEXT)
        if (c->NAME == cname)
          return c;
    }
    else { // transistor or instance connector
      *subname++ = '\0';
      cname = namealloc (subname);
      trname = namealloc (buf);

      for (li = netlist->LOINS; li; li = li->NEXT)
        {
          if (li->INSNAME == trname) 
            {
              for (c=li->LOCON; c!=NULL && c->NAME!=cname; c=c->NEXT) ;
              if (c!=NULL) return c;
            }          
        }

      for (tr = netlist->LOTRS; tr; tr = tr->NEXT)
            if (tr->TRNAME == trname) {
                if (tr->GRID->NAME == cname) return tr->GRID;
                if (tr->DRAIN->NAME == cname) return tr->DRAIN;
                if (tr->SOURCE->NAME == cname) return tr->SOURCE;
                if (tr->BULK->NAME == cname) return tr->BULK;
            }
    }

    return NULL;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
    
losig_list *gen_get_signal (lofig_list *netlist, char *signame)
{
    losig_list *sig;
    char *subname, *insname, buf[2048];
    char *sname;
    chain_list *ch;
    lofig_list *fig;
    loins_list *ins;

    sname = namealloc (signame);

    if (netlist->LOINS) { // hierarchical netlist
        strcpy (buf, sname);
        subname = strchr (buf, GEN_SEPAR);
        if (!subname) {
            sname = namealloc (buf);
            for (sig = netlist->LOSIG; sig; sig = sig->NEXT)
                for (ch = sig->NAMECHAIN; ch; ch = ch->NEXT)
                    if (ch->DATA == sname)
                        return sig;
            return NULL;
        }
        else {
            *subname++ = '\0';
            insname = namealloc (buf);
            for (ins = netlist->LOINS; ins; ins = ins->NEXT)
                if (ins->INSNAME == insname) {
                    fig = getlofig (ins->FIGNAME, 'A');
                    break;
                }
            if (!fig)
                return NULL;
            else
                return gen_get_signal (fig, subname);
        }
    }
    else { // flat netlist
        for (sig = netlist->LOSIG; sig; sig = sig->NEXT)
            for (ch = sig->NAMECHAIN; ch; ch = ch->NEXT)
                if (ch->DATA == sname) return sig;
    }

    return NULL;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

loins_list *gen_find_ins (lofig_list *fig, char *insname)
{
    char *subname, *subinsname, buf[2048];
    char *sname;
    lofig_list *subfig;
    loins_list *ins;

    if (fig->LOINS) { 
        strcpy (buf, insname);
        subname = strchr (buf, GEN_SEPAR);
        if (!subname) {
            sname = namealloc (buf);
            for (ins = fig->LOINS; ins; ins = ins->NEXT)
                if (ins->INSNAME == sname) return ins;
            return NULL;
        }
        else {
            *subname++ = '\0';
            subinsname = namealloc (buf);
            for (ins = fig->LOINS; ins; ins = ins->NEXT)
                if (ins->INSNAME == subinsname) {
                    subfig = getlofig (ins->FIGNAME, 'A');
                    break;
                }
            if (!subfig)
                return NULL;
            else
                return gen_find_ins (subfig, subname);
        }
    }
    else { 
        avt_errmsg(GNS_API_ERRMSG, "008", AVT_ERROR, insname, fig->NAME);
//        avt_error ("GNS", 0, AVT_WAR, "warning - no instance `%s': netlist `%s' is flat\n", insname, fig->NAME);
        return NULL;
    }
}

//_____________________________________________________________________________

void gen_reduce_inslist (lofig_list *fig, chain_list *inslist)
{
    chain_list *ch;
    ptype_list *p;
    p=getptype(fig->USER, GEN_ORIG_CORRESP_PTYPE);
    
    if (p==NULL)
      {
        for (ch = inslist; ch; ch = ch->NEXT)
          gns_ReduceInstance (fig, (char*)ch->DATA);
      }
    else
      {
        printf("[!] internal: to do!!\n");
      }
    
}

//_____________________________________________________________________________

chain_list *gen_add_trs2sup (char *insname, loins_list *ins, chain_list *trs2sup)
{
    lofig_list *fig;
    lotrs_list *trs;
    loins_list *sub_ins;
    char buf[2048];

    if (!ins) return NULL;

    fig = getlofig (ins->FIGNAME, 'A');

    for (trs = fig->LOTRS; trs; trs = trs->NEXT) {
        sprintf (buf, "%s%c%s", insname, GEN_SEPAR, trs->TRNAME);
        trs2sup = addchain (trs2sup, namealloc (buf));
    }
    for (sub_ins = fig->LOINS; sub_ins; sub_ins = sub_ins->NEXT) {
        sprintf (buf, "%s%c%s", insname, GEN_SEPAR, sub_ins->INSNAME);
        trs2sup = gen_add_trs2sup (buf, sub_ins, trs2sup);
    }

    return trs2sup;
}

//_____________________________________________________________________________

void gns_ReduceInstance (lofig_list *fig, char *ins_name)
{
  ptype_list *p;
  if (!fig) return;
  
  if ((p=getptype(fig->USER, GEN_TRANS_TO_SUPPRESS_PTYPE))==NULL)
    fig->USER=p=addptype(fig->USER, GEN_TRANS_TO_SUPPRESS_PTYPE, NULL);

  p->DATA=addchain((chain_list *)p->DATA, namealloc(gen_makeinstancename(ins_name)));
//  gen_reduce_ins (fig, namealloc(gen_makeinstancename(ins_name)));
}

void gns_KeepInstance (lofig_list *fig, char *ins_name)
{
  ptype_list *p;
  if (!fig) return;
  
  if ((p=getptype(fig->USER, GEN_TRANS_TO_KEEP_PTYPE))==NULL)
    fig->USER=p=addptype(fig->USER, GEN_TRANS_TO_KEEP_PTYPE, NULL);

  p->DATA=addchain((chain_list *)p->DATA, namealloc(gen_makeinstancename(ins_name)));
//  gen_reduce_ins (fig, namealloc(gen_makeinstancename(ins_name)));
}

//_____________________________________________________________________________
/*
void gen_reduce_ins (lofig_list *fig, char *insname)
{
    TRANS_TO_SUPPRESS = addchain (TRANS_TO_SUPPRESS, insname);
}
*/
//_____________________________________________________________________________

void gen_freelotrs (lotrs_list *trs)
{
    if (trs->GRID->PNODE)
        delrcnlocon (trs->GRID);
    delloconuser (trs->GRID);
    free (trs->GRID);
                   
    if (trs->SOURCE->PNODE)
        delrcnlocon (trs->SOURCE);
    delloconuser (trs->SOURCE);
    free (trs->SOURCE);

    if (trs->DRAIN->PNODE)
        delrcnlocon (trs->DRAIN);
    delloconuser(trs->DRAIN);
    free (trs->DRAIN);
    
    if (trs->BULK)
      {
        if (trs->BULK->PNODE)
          delrcnlocon (trs->BULK);
        delloconuser (trs->BULK);
      }
                                            
    dellotrsuser (trs);
    free (trs);
}

//_____________________________________________________________________________

void gen_freelosig (losig_list *sig)
{
    if (sig->PRCN)
        freelorcnet (sig);
    sig->INDEX = 0L;

    freechain (sig->NAMECHAIN);
    sig->NAMECHAIN = NULL;

    dellosiguser (sig);
}

//_____________________________________________________________________________

chain_list *gen_get_lofigchain (losig_list *sig) 
{
    ptype_list *ptype;
    
    if ((ptype = getptype (sig->USER, LOFIGCHAIN)))
        return (chain_list*)ptype->DATA;
    else {
        avt_errmsg(GNS_API_ERRMSG, "009", AVT_ERROR, 1);
//        avt_error ("GNS", 0, AVT_WAR, " - warning: gen_get_lofigchain returns NULL\n");
        return NULL;
    }
}

//_____________________________________________________________________________

int gen_isalim (losig_list *sig)
{
    return (mbk_LosigIsVSS(sig) || mbk_LosigIsVDD(sig));
}

//_____________________________________________________________________________

typedef struct orphancapa
{
  struct orphancapa *NEXT;
  float capa;
  int node;
  losig_list *ls;
} orphancapa;

static orphancapa *getctc(HeapAlloc *ha, orphancapa *head, losig_list *ls, char flag)
{
  chain_list *ch;
  losig_list *me, *other;
  orphancapa *oc;
  loctc_list *ctc;
  
  if (ls->PRCN==NULL) return head;

  for (ch = ls->PRCN->PCTC; ch; ch = ch->NEXT)
    {
      ctc = (loctc_list*)ch->DATA;
      if (ctc->SIG1 == ls) { other = ctc->SIG2; }
      else other=ctc->SIG1;
      if (other->FLAGS==flag) continue;
      oc=(orphancapa *)AddHeapItem(ha);
      oc->capa=ctc->CAPA;
      oc->ls=other;
      oc->node= rcn_ctcnode (ctc, other);
      oc->NEXT=head;
      head=oc;
    }
  return head;
}

static void dispatchctc(lofig_list *lf, orphancapa *oc)
{
  losig_list *gnd;
  if (oc==NULL) return;
  if ((gnd = gen_get_ground (lf))==NULL)
    {
      avt_errmsg(GNS_API_ERRMSG, "010", AVT_ERROR, gen_info());
//      avt_error("genapi", 1, AVT_WAR, "%s: no ground signal can be found in netlist\n", gen_info);
      return;
    }
  if (!gnd->PRCN) gnd->PRCN = addlorcnet (gnd);
  while (oc!=NULL)
    {
      addloctc (oc->ls, oc->node, gnd, 1, oc->capa);
      oc=oc->NEXT;
    }
}


void gen_del_flagged_losig (lofig_list *fig, char flag)
{
  losig_list *sig, *prev_sig, *next_sig;
  orphancapa *oc=NULL;
  HeapAlloc ha;

  CreateHeap(sizeof(orphancapa), 6000, &ha);
  for (prev_sig=NULL, sig=fig->LOSIG; sig!=NULL; sig= next_sig)
    {
      next_sig=sig->NEXT;
      if (sig->TYPE!=EXTERNAL && sig->FLAGS == flag)
        {
          if (prev_sig==NULL) fig->LOSIG=next_sig;
          else prev_sig->NEXT=next_sig;
          oc=getctc(&ha, oc, sig, flag);
          gen_freelosig (sig);
        }
      else 
        prev_sig=sig;
    }

  // redispath ctc
  dispatchctc(fig, oc);
  DeleteHeap(&ha);
}

//_____________________________________________________________________________

void gen_del_flagged_locon (lofig_list *fig, char flag)
{
  locon_list *con, *prev_con, *next_con;
  
  for (prev_con=NULL, con=fig->LOCON; con!=NULL; con= next_con)
    {
      next_con=con->NEXT;
      if (con->FLAGS == flag)
        {
          if (prev_con==NULL) fig->LOCON=next_con;
          else prev_con->NEXT=next_con;
          mbkfree (con);
        }
      else 
        prev_con=con;
    }
}

void gen_freeloins(loins_list *ptins)
{
  locon_list *ptcon;
  void    *pt=NULL;

  for (ptcon = ptins->LOCON; ptcon; ptcon = (locon_list *)pt) {
    if( ptcon->PNODE )
      delrcnlocon( ptcon );
    pt = (void *)ptcon->NEXT;
    delloconuser( ptcon );
    mbkfree((void *)ptcon);
  }

  delloinsuser( ptins );
  mbkfree((void *)ptins);
}

//_____________________________________________________________________________

void gen_del_flagged_lotrs (lofig_list *fig, char flag)
{
    lotrs_list *trs, *next_trs, *prev_trs;

    for (prev_trs=NULL, trs=fig->LOTRS; trs!=NULL; trs= next_trs)
      {
        next_trs=trs->NEXT;
        if (trs->FLAGS == flag)
          {
            if (prev_trs==NULL) fig->LOTRS=next_trs;
            else prev_trs->NEXT=next_trs;
            gen_freelotrs (trs);
          }
        else 
          prev_trs=trs;
      }
}

int gen_del_flagged_loins (lofig_list *fig, long flag)
{
  loins_list *li, *next, *prev;
  int cnt=0;
  for (li=fig->LOINS, prev=NULL; li!=NULL; li=next)
    {
      next=li->NEXT;
      if (getptype(li->USER, flag)!=NULL)
        {
          if (prev==NULL) fig->LOINS=li->NEXT; else prev->NEXT=li->NEXT;
          gen_freeloins(li);
          cnt++;
        }
      else prev=li;
    }
  return cnt;
}

//_____________________________________________________________________________
#define DD_ALIM    1
#define DD_NO_SUPP 2

void init_suppress_marks(lofig_list *fig)
{
  lotrs_list *trs;
  losig_list *sig;
  locon_list *ptcon;

  for (trs = fig->LOTRS; trs; trs = trs->NEXT) {
    trs->FLAGS = 0;
    trs->GRID->FLAGS = 0;
    trs->DRAIN->FLAGS = 0;
    trs->SOURCE->FLAGS = 0;
  }

  for (sig = fig->LOSIG; sig; sig = sig->NEXT)
    {
      if (gen_isalim (sig)) sig->FLAGS = DD_ALIM;
      else sig->FLAGS = 0;
    }

  for (ptcon = fig->LOCON; ptcon; ptcon = ptcon->NEXT)
    ptcon->FLAGS = 0;
}

int gen_suppress_trans (lofig_list *fig, chain_list *trs2sup, chain_list *trs2keep)
{
    lotrs_list *trs;
    losig_list *sig;
    loins_list *li;
    locon_list *lc;
    char suppress;
    int count=0;
    ptype_list *p;
    
    if (!trs2sup && !trs2keep) return 0;

#ifdef STAT
    fprintf(stderr,"init:");mbk_debugstat(0);
#endif

    for (trs = fig->LOTRS; trs; trs = trs->NEXT)
      {
        if (is_ins_to_supp(trs->TRNAME, trs2sup, trs2keep))
          {
            trs->FLAGS = 'S'; 
            trs->GRID->FLAGS = 'S';
            trs->SOURCE->FLAGS = 'S';
            trs->DRAIN->FLAGS = 'S';
            count++;
//            break;
          }
        else
          {
            trs->GRID->SIG->FLAGS |= DD_NO_SUPP;
            trs->SOURCE->SIG->FLAGS |= DD_NO_SUPP;
            trs->DRAIN->SIG->FLAGS |= DD_NO_SUPP;
          }
      }

    for (li = fig->LOINS; li!=NULL; li=li->NEXT)
      {
        if (is_ins_to_supp(li->INSNAME, trs2sup, trs2keep))
          {
            for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
              lc->FLAGS='S';
//            break;
          }
        else
          {
            for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
              lc->SIG->FLAGS |= DD_NO_SUPP;
          }
      }

#ifdef STAT
    fprintf(stderr,"1:");mbk_debugstat(0);
#endif

    // flag 'D' transistors and signals surrounded only by 'S' transistors
    for (trs = fig->LOTRS; trs; trs = trs->NEXT) {
        suppress = 'Y';
        // grid
        if (suppress == 'Y') {
          sig = trs->GRID->SIG;
          if ((sig->FLAGS & DD_ALIM)==0 && sig->TYPE!=EXTERNAL && (sig->FLAGS & DD_NO_SUPP)!=0)
            suppress = 'N';
        }
        // drain
        if (suppress == 'Y') {
          sig = trs->DRAIN->SIG;
          if ((sig->FLAGS & DD_ALIM)==0 && sig->TYPE!=EXTERNAL && (sig->FLAGS & DD_NO_SUPP)!=0)
            suppress = 'N';
        }
        // source
        if (suppress == 'Y') {
          sig = trs->SOURCE->SIG;
          if ((sig->FLAGS & DD_ALIM)==0 && sig->TYPE!=EXTERNAL && (sig->FLAGS & DD_NO_SUPP)!=0)
            suppress = 'N';
        }
        if (suppress == 'Y')
          trs->FLAGS = 'D'; 
    }

    for (li = fig->LOINS; li!=NULL; li=li->NEXT)
      {
        for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
          {
            if ((lc->SIG->FLAGS & DD_ALIM)==0 && (lc->SIG->TYPE!=EXTERNAL) && (lc->SIG->FLAGS & DD_NO_SUPP)!=0) break;
          }
        if (lc==NULL)
          li->USER=addptype(li->USER, DELETE_ME_LATER, NULL);
      }

#ifdef STAT
    fprintf(stderr,"2:");mbk_debugstat(0);
#endif

    for (li = fig->LOINS; li!=NULL; li=li->NEXT)
      {
        if ((p=getptype(li->USER, DELETE_ME_LATER))!=NULL && p->DATA==NULL)
          {
            for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
              {
                if ((lc->SIG->FLAGS & DD_ALIM)==0 && lc->SIG->TYPE!=EXTERNAL)
                  lc->SIG->FLAGS = 'D';
              }
          }
      }

    for (trs = fig->LOTRS; trs; trs = trs->NEXT) {
        if (trs->FLAGS == 'D') { 
            trs->GRID->FLAGS = 'D';
            trs->DRAIN->FLAGS = 'D';
            trs->SOURCE->FLAGS = 'D';
            if ((trs->GRID->SIG->FLAGS & DD_ALIM)==0 && trs->GRID->SIG->TYPE!=EXTERNAL)
                trs->GRID->SIG->FLAGS = 'D';
            if ((trs->DRAIN->SIG->FLAGS & DD_ALIM)==0 && trs->DRAIN->SIG->TYPE!=EXTERNAL)
                trs->DRAIN->SIG->FLAGS = 'D';
            if ((trs->SOURCE->SIG->FLAGS & DD_ALIM)==0 && trs->SOURCE->SIG->TYPE!=EXTERNAL)
                trs->SOURCE->SIG->FLAGS = 'D';
        }
    }

#ifdef STAT
    fprintf(stderr,"3:");mbk_debugstat(0);
#endif

    return count;
}


static more_capa *graballinternalconnectors(lofig_list *fig)
{
  losig_list *ls;
  loins_list *li;
  locon_list *lc;
  ptype_list *p;
  subinst_t *si;
  chain_list *locons;
  ht *ic;
  more_capa *mc;
  long l;

  mc=(more_capa *)mbkalloc(sizeof(more_capa));  
  ic=addht(1200);
  mc->per_signal_internal_locons=ic;
  mc->count=1;

  for (li = fig->LOINS; li; li = li->NEXT) 
    {
      if ((p=getptype(li->USER, DELETE_ME_LATER))!=NULL && p->DATA!=NULL)
        {
          si = (subinst_t *)p->DATA;
          if (si->FLAGS & LOINS_IS_BLACKBOX) continue;
          for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
            {
              if ((lc->SIG->FLAGS & DD_ALIM)==0)
                {
                  if ((l=gethtitem(ic, gen_losigname(lc->SIG)))==EMPTYHT)
                    locons=NULL;
                  else
                    locons=(chain_list *)l;

                  ls=gen_corresp_sig (lc->NAME, si->CRT);
                  locons = GrabAllConnectorsThruCorresp (lc->NAME, ls, si->CRT, locons);
                  addhtitem(ic, gen_losigname(lc->SIG), (long)locons);
                }
            }
        }
    }
  return mc;
}

static int realcleanaftersuppress(lofig_list *fig)
{
  lotrs_list *trs;
  losig_list *gnd;
  int count;

  gnd = gen_get_ground (fig);

  for (trs = fig->LOTRS; trs; trs = trs->NEXT) {
    if (trs->FLAGS != 'S') continue;
    //if (GEN_SIM_OUTLOAD == GEN_DYNAMIC_OUTLOAD) {
    if (trs->GRID->SIG->FLAGS != 'D') 
      gen_add_equicapa_from_cir (trs->GRID->SIG, gnd, trs->GRID);
    if (trs->DRAIN->SIG->FLAGS != 'D')
      gen_add_equicapa_from_cir (trs->DRAIN->SIG, gnd, trs->DRAIN);
    if (trs->SOURCE->SIG->FLAGS != 'D')
      gen_add_equicapa_from_cir (trs->SOURCE->SIG, gnd, trs->SOURCE);
    //}
   //else
    /*
      if (GEN_SIM_OUTLOAD == GEN_TRANSISTOR_OUTLOAD) {
      if (trs->GRID->SIG->FLAGS != 'D') 
      gen_add_blocked_trans (fig, trs->GRID, gnd, vdd, trs->GRID);
      if (trs->DRAIN->SIG->FLAGS != 'D')
      gen_add_blocked_trans (fig, trs->DRAIN, gnd, vdd, trs->DRAIN);
      if (trs->SOURCE->SIG->FLAGS != 'D')
      gen_add_blocked_trans (fig, trs->SOURCE, gnd, vdd, trs->SOURCE);
      }
    */
  }

  count=gen_del_flagged_loins (fig, DELETE_ME_LATER);

  gen_del_flagged_losig (fig, 'D');
  
  // delete 'D' and 'S' transistors
  gen_del_flagged_lotrs (fig, 'S');
  gen_del_flagged_lotrs (fig, 'D');
  
  // update lofigchain
  lofigchain (fig);
#if STAT
  fprintf(stderr,"reste:");mbk_debugstat(0);
#endif
  return count;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_set_charac_capa (lofig_list *fig, char *locon, double capa)
{
    locon_list *ptcon;
    char *cname = namealloc (locon);
    losig_list *gnd = gen_get_ground (fig);
    loctc_list *ctc;
    ptype_list *ptype;
    chain_list *ch;

    
    capa *= 1.0e12; // remet la capa en F
    for (ptcon = fig->LOCON; ptcon && ptcon->NAME != cname; ptcon = ptcon->NEXT);
    if (!ptcon) {
        avt_errmsg(GNS_API_ERRMSG, "011", AVT_ERROR, cname, fig->NAME);
//        avt_error ("GNS", 0, AVT_WAR, " - warning: no connector `%s' in netlist `%s'\n", cname, fig->NAME);
        return;
    }

    if (!gnd->PRCN) addlorcnet (gnd);

    if (!ptcon->SIG->PRCN) {
        addlorcnet (ptcon->SIG);
        ctc = addloctc (ptcon->SIG, 0, gnd, 0, capa);
        ctc->USER = addptype (ctc->USER, CHARAC_CAPA_PTYPE, NULL);
    }
    else {
        for (ch = ptcon->SIG->PRCN->PCTC; ch; ch = ch->NEXT) {
            ctc = (loctc_list*)ch->DATA;
            if ((ptype = getptype (ctc->USER, CHARAC_CAPA_PTYPE))) {
                ctc->CAPA = capa;
                break;
            }
        }
        if (!ch) {
            ctc = addloctc (ptcon->SIG, ptcon->PNODE->DATA, gnd, 0, capa);
            ctc->USER = addptype (ctc->USER, CHARAC_CAPA_PTYPE, NULL);
        }
    }
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_clean_lofig (lofig_list *fig)
{
    int only_external;
    losig_list *sig;
    ptype_list *ptype;
    chain_list *ch;

    for (sig = fig->LOSIG; sig; sig = sig->NEXT) 
      {
        sig->FLAGS =0;
        if (sig->TYPE!=EXTERNAL)
          {
            if ((ptype = getptype (sig->USER, LOFIGCHAIN))) {
              if (!ptype->DATA)
                sig->FLAGS = 'D';
            }
          }
      }
    gen_del_flagged_losig (fig, 'D');
//    gen_del_flagged_locon (fig, 'D');
}

/*----------------------------------------------------------------------------*/
// FUNCTION : gen_verif_locon
//
// verifie si le connecteur se trouve dans une bonne instance
// renvoie le nom complet de l'instance si le connecteur est valide
//
// Retourne 0 si connecteur valide
//
/*----------------------------------------------------------------------------*/
int gen_verif_locon (locon_list *con,char *insname,char **fullinsname)
{
    char *trname,*right;
    int   res = 0;
    char insname2verif[1024];

    strcpy(insname2verif, gen_makeinstancename(insname));

    if (con->TYPE == 'T') {
        trname = ((lotrs_list*)con->ROOT)->TRNAME;
        if (!strstr(trname,insname2verif))
            res = 1;
        else
            gen_rightunconcatname (trname,&(*fullinsname),&right,GEN_SEPAR);
    }
    else if (con->TYPE == 'I') {
        if (strncmp(((loins_list*)con->ROOT)->INSNAME,insname2verif,strlen(insname2verif))!=0)
            res = 1;
        else {
            *fullinsname = ((loins_list*)con->ROOT)->INSNAME;
        }
    }
    else 
        res = 1;

    return res;
}

/*----------------------------------------------------------------------------*/
int gen_comp_awe_delay_con_loop (const void *cona, const void *conb)
{
    locon_list *con1 = (locon_list*)((*(chain_list**)cona))->DATA;
    locon_list *con2 = (locon_list*)((*(chain_list**)conb))->DATA;
    ptype_list *ptype;
    double      dawe1;
    double      dawe2;
    int         res = 0;

    ptype = getptype (con1->USER,TRC_LOCON_AWE);
    dawe1 = *(double*)ptype->DATA;
    ptype = getptype (con2->USER,TRC_LOCON_AWE);
    dawe2 = *(double*)ptype->DATA;
    if (dawe1 < dawe2)
        res = -1;
    else if (dawe1 == dawe2)
        res = 0;
    else
        res = 1;
    return res;
}

/*----------------------------------------------------------------------------*/
int gen_comp_awe_delay_con (const void *cona, const void *conb)
{
    awelist *awe1 = *(awelist**)cona;
    awelist *awe2 = *(awelist**)conb;
    RCXFLOAT    dawe1;
    RCXFLOAT    dawe2;
    int         res = 0;

    dawe1 = awe1->DMAX;
    dawe2 = awe2->DMAX;
    if (dawe1 < dawe2)
        res = -1;
    else if (dawe1 == dawe2)
        res = 0;
    else
        res = 1;
    return res;
}


/*----------------------------------------------------------------------------*/
/* gen_leftunconcatname */
/*----------------------------------------------------------------------------*/
void gen_leftunconcatname(char *name, char **left, char **right, char separ)
{
  int   i;
  char  str[2048];

  // On recherche le premier séparateur.

  for( i=0;
       name[i] != separ && name[i]!= 0 ;
       i++ )
    str[i] = name[i];
  str[i]=0;

  if( name[i] != separ ) {
    // Il n'y a plus de séparateur : il ne reste donc que le right.
    *right = name;
    *left  =NULL;
  } else {
    *left  = namealloc( str );
    *right = namealloc( name+i+1 );
  }
}

/*----------------------------------------------------------------------------*/
/* gen_rightunconcatname */
/*----------------------------------------------------------------------------*/
void gen_rightunconcatname(char *name, char **left, char **right, char separ)
{
  int   i, m;
  char  str[2048];

  // Positionne sur le dernier caractère;
  for( i=0, m=-1 ; name[i] ; i++)
    if( name[i] == separ )
      m=i;

  if( m == -1 ) {
    // Il n'y a plus de séparateur : il ne reste donc que le left
    *right = NULL;
    *left  = name;
  } else {
    // strlcpy( str, name, m );   pas disponible sous Solaris 5.6
    for( i=0 ; i < m ; i++ )
      str[i] = name[i];
    str[i] = '\0';

    *right = namealloc( name+m+1 );
    *left  = namealloc( str );
  }
}

/*----------------------------------------------------------------------------*/
void gen_get_min_max_instance_index (lofig_list *fig, char *inst_radical, int *min, int *max)
{
    loins_list *loins;
    lofig_list *newfig;
    int         res;
    char       *insname2find,*name;
    char       *right,*newright,*newinsname;
    static int firstcall = 1;
    
    if (!fig || !inst_radical)
        return;
    if (firstcall == 1) {
        *min = 100;
        *max = -1;
    }

    gen_leftunconcatname (inst_radical,&insname2find,&right,GEN_SEPAR);

    for (loins = fig->LOINS ; loins; loins = loins->NEXT) {
        if (loins->INSNAME == insname2find) {
            gen_leftunconcatname (right,&newinsname,&newright,GEN_SEPAR);
            if (newinsname) {
                newfig = gen_getlofig (loins->FIGNAME);
                firstcall = 0;
                gen_get_min_max_instance_index (newfig,right,min,max);
            }
            else {
                newfig = gen_getlofig (loins->FIGNAME);
                for (loins = newfig->LOINS ; loins ; loins = loins->NEXT) {
                    if ((name = strchr (loins->INSNAME,(int)SEPAR)) != NULL) {
                        res = (int)strtol (&name[1],&name,0);
                        if (*min >= res)
                            *min = res;
                        if (*max <= res)
                            *max = res;
                    }
                }
                return;
            }
        }
    }
    firstcall = 1;
}


/*----------------------------------------------------------------------------*/
chain_list *gen_order_locon_awe_loop (losig_list *sig)
{
    chain_list *orderchain = NULL, *initchain = NULL;
    chain_list **tabchain = NULL;
    chain_list *chain;
    locon_list *locon;
    ptype_list *ptype,*ptype_awe;
    int         i,nb_locon = 0;

    if (!sig) return NULL;

    if ((ptype = getptype(sig->USER,LOFIGCHAIN)) == NULL)
        return NULL;
   
    for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
        chain = chain->NEXT) {
        locon = (locon_list *)chain->DATA ;
        if ((ptype_awe = getptype (locon->USER,TRC_LOCON_AWE)) != NULL) {
            initchain = addchain (initchain,locon);
            nb_locon++;
        }
    }

    tabchain = (chain_list**)mbkalloc(sizeof(chain_list*)*nb_locon);
    for (chain = initchain , nb_locon = 0  ; chain ; 
         chain = chain->NEXT, nb_locon++) {
        tabchain[nb_locon] = chain;
    }

    qsort(tabchain, nb_locon, sizeof(chain_list*), gen_comp_awe_delay_con_loop);

    for (i = 0 ; i < nb_locon ; i++)
        orderchain = addchain (orderchain,(tabchain[i])->DATA); 

    freechain (initchain);
    mbkfree (tabchain);

    return orderchain;
}

/*----------------------------------------------------------------------------*/
chain_list *gen_order_locon_awe (awelist *awe_list, 
                                 locon_list **cmax,// connecteur avec delay awe max
                                 locon_list **cmin // connecteur avec delay awe min
                                 )
{
    awelist *awe;
    chain_list *orderchain = NULL;
    awelist **tabawe = NULL;
    int         i,nb_locon = 0;

    if (!awe_list) return NULL;

    for (awe = awe_list, nb_locon = 0; awe ; awe = awe->NEXT, nb_locon++) ;
    
    tabawe = (awelist**)mbkalloc(sizeof(awelist*)*nb_locon);

    for (awe = awe_list, nb_locon = 0; awe ; awe = awe->NEXT, nb_locon++) 
      {
        tabawe[nb_locon] = awe;
      } 
    
    qsort(tabawe, nb_locon, sizeof(awelist*), gen_comp_awe_delay_con);

    for (i = 0 ; i < nb_locon ; i++) 
        orderchain = addchain (orderchain,(tabawe[i])->LOCON); 

    *cmax = (tabawe[0])->LOCON;
    *cmin = (tabawe[nb_locon-1])->LOCON;

    mbkfree (tabawe);

    return orderchain;
}

/*----------------------------------------------------------------------------*/
chain_list *gen_get_instance_loopdelayawe_sub( lofig_list *lofig, locon_list *begin,
                                               double vmax, double smax, double smin, 
                                               double *tmax, double *tmin, double *fmax, double *fmin,
                                               locon_list **con_min, locon_list **con_max)
{
    losig_list *losig;
    chain_list *chain_con_awe_ordered, *cl;
   
    //ptype_list *ptype;
    awelist    *awe_list;
    rcx_slope   slopemax;
 
    if (!begin) return NULL;

    TRC_MARK_CON = 1;
    losig = begin->SIG;
    smax = smax * 1.0e12;
    smin = smin * 1.0e12;
    
    RCX_PTRTESTANDSET( tmax, -1.0 );
    RCX_PTRTESTANDSET( tmin, -1.0 );
    RCX_PTRTESTANDSET( fmax, -1.0 );
    RCX_PTRTESTANDSET( fmin, -1.0 );
 
    if( !losig->PRCN || !losig->PRCN->PWIRE )
      {
        chain_con_awe_ordered=NULL;
        *con_min=NULL; *con_max=NULL;
        for (cl=getptype(losig->USER, LOFIGCHAIN)->DATA; cl!=NULL; cl=cl->NEXT)
          {
            if (cl->DATA!=begin)
              {
                chain_con_awe_ordered=addchain(chain_con_awe_ordered, cl->DATA);
                if (*con_min==NULL) *con_min=(locon_list *)cl->DATA;
                if (cl->NEXT==NULL) *con_max=(locon_list *)cl->DATA;
              }
          }
        return chain_con_awe_ordered;
      }
    if( smax < 0.0 || smin < 0.0 ) return NULL;
    if( rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK )
        return NULL;

    slopemax.F0MAX  = smax ;
    slopemax.FCCMAX = smax ;
    slopemax.SENS   = TRC_SLOPE_UP ;
    slopemax.CCA    = -1.0 ;
    slopemax.MIN.slope = smin ;                 slopemax.MAX.slope = smax ;
    slopemax.MIN.slnrm = smin ;                 slopemax.MAX.slnrm = smax ;
    slopemax.MIN.vend  = vmax ;                 slopemax.MAX.vend  = vmax ;
    slopemax.MIN.vt    = 0.0 ;                  slopemax.MAX.vt    = 0.0 ;
    slopemax.MIN.vdd   = vmax ;                 slopemax.MAX.vdd   = vmax ;
    slopemax.MIN.vsat  = -1.0 ;                 slopemax.MAX.vsat  = -1.0 ;
    slopemax.MIN.rlin  = -1.0 ;                 slopemax.MAX.rlin  = -1.0 ;
    slopemax.MIN.vth   = vmax/2.0 ;             slopemax.MAX.vth   = vmax/2.0 ;
    slopemax.MIN.r     = -1.0 ;                 slopemax.MAX.r     = -1.0 ;
    slopemax.MIN.c1    = -1.0 ;                 slopemax.MAX.c1    = -1.0 ;
    slopemax.MIN.c2    = -1.0 ;                 slopemax.MAX.c2    = -1.0 ;
    slopemax.MIN.pwl=NULL;                      slopemax.MAX.pwl=NULL;
   
    /*
    if ( chkloop( losig ) ) {
        rcx_delayloop ( lofig,
                        losig,
                        ((locon_list*)begin),
                       &slopemax,
                        smax,
                        smin,
                        vmax,
                        0.0,
                        0.0,
                        tmax,
                        tmin,
                        fmax,
                        fmin,
                        con_max,
                        con_min
                      );
        chain_con_awe_ordered = gen_order_locon_awe_loop (losig);
    }
    else {
    */
        awe_list = getawedelaylist( lofig,
                                    losig,
                                    (locon_list*)begin,
                                   &slopemax,
                                    0.0
                            );
        chain_con_awe_ordered = gen_order_locon_awe (awe_list,con_max,con_min);
    /*
    }
    */
    TRC_MARK_CON = 0;
    return chain_con_awe_ordered;
}

char *gen_get_instance_loopdelayawe( lofig_list *lofig, char type, char *insname,
                                     locon_list *begin, double vmax, double smax, double smin, 
                                     double *tmax, double *tmin, double *fmax, double *fmin )
{
  chain_list *chain_con_awe_ordered, *chain;
  char        flag_cmax = 'Y';
  char        flag_cmin = 'Y';
  locon_list *con_min,*con_max;
  char       *inst_wc=NULL,*inst_bc=NULL,*ins=NULL;
    
  chain_con_awe_ordered = gen_get_instance_loopdelayawe_sub(lofig, begin, vmax, smax, smin, tmax, tmin, fmax, fmin, &con_min, &con_max);

    if (con_max) {
        if (gen_verif_locon (con_max,insname,&inst_wc))
            flag_cmax = 'N';
    }
    if (con_min) {
        if (gen_verif_locon (con_min,insname,&inst_bc))
            flag_cmin = 'N';
    }

    if (flag_cmax == 'N') {
        for (chain = chain_con_awe_ordered ; chain ; chain = chain->NEXT) {
            locon_list *con = (locon_list*)chain->DATA;
            if (!gen_verif_locon (con,insname,&inst_wc))
                break;
        }
    }
 
    if (flag_cmin == 'N') {
        for (chain = reverse(chain_con_awe_ordered) ; chain ; chain = chain->NEXT) {
            locon_list *con = (locon_list*)chain->DATA;
            if (!gen_verif_locon (con,insname,&inst_bc))
                break;
        }
    }
    
    if (chain_con_awe_ordered) {
        // TODO : liberation du ptype TRC_LOCON_AWE ds locon
        // pb : on ne peut pas le faire car sinon on ne peut
        // pas executer gns_GetBestInstance suivi de gns_KeepBestInstance
        // car le ptype aura disparu...
        /*
        for (chain = chain_con_awe_ordered ; chain ; chain = chain->NEXT) {
            locon_list *locon = (locon_list*)chain->DATA;
            if ((ptype = getptype (locon->USER,TRC_LOCON_AWE)) != NULL) {
                free (ptype->DATA);
                locon->USER = delptype (locon->USER,TRC_LOCON_AWE);
            }
        }*/
        freechain (chain_con_awe_ordered);
    }
 
    RCX_PTRTESTANDSET( tmax, *tmax * 1.0e-12 );
    RCX_PTRTESTANDSET( tmin, *tmin * 1.0e-12 );
    RCX_PTRTESTANDSET( fmax, *fmax * 1.0e-12 );
    RCX_PTRTESTANDSET( fmin, *fmin * 1.0e-12 );
    if (type == GEN_BEST)
        ins = inst_bc;
    else
        ins = inst_wc;


    return ins;
}

/*----------------------------------------------------------------------------*/
char *gns_AWE_GetWorstInstance ( lofig_list *lofig, char *insname, 
                                     locon_list *begin, double vdd)
{
    double smax = 200.0e-12;
    double smin = 200.0e-12;
    double tmin,tmax,fmax,fmin;

    return gen_get_instance_loopdelayawe(lofig, 
                                         GEN_WORST,
                                         insname,
                                         begin,
                                         vdd,
                                         smax, smin,
                                         &tmax, &tmin, &fmax, &fmin );
}

void gns_AWE_GetOrderedInstanceIndex(lofig_list *lofig, char *rule, locon_list *begin, int **tab, int *nb)
{
  chain_list *cl, *ch;
  int *t, n, idx;
  locon_list *lc;
  double smax = 200.0e-12;
  double smin = 200.0e-12;
  double tmin,tmax,fmax,fmin;
  locon_list *con_min,*con_max;
  char *newrule;

  newrule=gen_makeinstancename(rule); // ATTENTION: newrule est un pointeur vers un buffer static

  cl=gen_get_instance_loopdelayawe_sub(lofig, begin, 1.0, smax, smin, &tmax, &tmin, &fmax, &fmin, &con_min, &con_max);

  for (ch=cl, n=0; ch!=NULL; ch=ch->NEXT, n++) ;
  t=malloc(sizeof(int)*n);
  for (ch=cl, n=0; ch!=NULL; ch=ch->NEXT)
    {
      lc=(locon_list *)ch->DATA;
      switch(lc->TYPE)
        {
        case 'T':
          if (mbk_FindNumber(((lotrs_list *)lc->ROOT)->TRNAME, newrule, &idx)==0)
            t[n++]=idx;
          break;
        case 'I':
          if (mbk_FindNumber(((loins_list *)lc->ROOT)->INSNAME, newrule, &idx)==0)
            t[n++]=idx;
          break;
        }
    }
  freechain(cl);
  *tab=t;
  *nb=n;
}

/*----------------------------------------------------------------------------*/
char *gns_AWE_GetBestInstance ( lofig_list *lofig, char *insname, 
                                    locon_list *begin, double vdd)
{
    double smax = 200.0e-12;
    double smin = 200.0e-12;
    double tmin,tmax,fmax,fmin;

    return gen_get_instance_loopdelayawe(lofig,
                                         GEN_BEST, 
                                         insname,
                                         begin,
                                         vdd,
                                         smax, smin,
                                         &tmax, &tmin, &fmax, &fmin );
}

/*----------------------------------------------------------------------------*/

lofig_list *gns_AWE_KeepBestInstance ( lofig_list *hierfig, lofig_list *flatfig, 
                                           char *insname, locon_list *begin,double vdd)
{
    char *ins2keep;
    char  instance_name[2048];
    double smax = 200.0e-12;
    double smin = 200.0e-12;
    double tmin,tmax,fmax,fmin;
    char   *left,*right;
    chain_list *insnamelist,*chain;
    subinst_t *tab;
    ptype_list *ptype;
    int    rc = IN_RC|INTERNAL_RC|OUT_RC;

    if ((!hierfig) || (!flatfig)) return NULL;
    rightunconcatname (insname,&left,&right);
    if (!right) right = left;
    
    ptype = getptype (hierfig->USER,GEN_ORIG_CORRESP_PTYPE);
    if (!ptype) {
        avt_errmsg(GNS_API_ERRMSG, "012", AVT_ERROR);
//        avt_error ("GNS", 0, AVT_WAR,"gns_KeepBestInstanceDelayAWE : no model correspondance!\n");
        return NULL;
    }
    tab = gen_GetCorrespondingInstance(left,(corresp_t*)ptype->DATA);
    insnamelist = gen_GetInstanceWithSameRadical (gen_getlofig(tab->CRT->GENIUS_FIGNAME),
                                                  right
                                                  );
    ins2keep = gen_get_instance_loopdelayawe (flatfig,
                                              GEN_BEST,
                                              insname,
                                              begin,
                                              vdd,
                                              smax, smin,
                                              &tmax, &tmin, &fmax, &fmin );
    if (!ins2keep) {
        avt_errmsg(GNS_API_ERRMSG, "013", AVT_ERROR);
//        avt_error ("GNS", 0, AVT_WAR,"gns_KeepBestInstanceDelayAWE : no instance founded !\n");
        return NULL;
    }
    rightunconcatname (ins2keep,&left,&right);
    if (!right) 
        gns_ReduceInstance (hierfig,left);
    else
    for (chain = insnamelist ; chain ; chain = chain->NEXT) {
        if (strcmp(right,(char*)chain->DATA)) {
            sprintf(instance_name,"%s.%s",left,(char*)chain->DATA);
            gns_ReduceInstance (hierfig,namealloc(instance_name));
        }
    }
    return gns_FlattenNetlist (hierfig,rc);
}


/*----------------------------------------------------------------------------*/

lofig_list *gns_AWE_KeepWorstInstance ( lofig_list *hierfig, lofig_list *flatfig, 
                                           char *insname, locon_list *begin,double vdd)
{
    char *ins2keep;
    char  instance_name[2048];
    double smax = 200.0e-12;
    double smin = 200.0e-12;
    double tmin,tmax,fmax,fmin;
    char   *left,*right;
    chain_list *insnamelist,*chain;
    subinst_t *tab;
    ptype_list *ptype;
    int    rc = IN_RC|INTERNAL_RC|OUT_RC;

    if ((!hierfig) || (!flatfig)) return NULL;
    rightunconcatname (insname,&left,&right);
    if (!right) right = left;
    
    ptype = getptype (hierfig->USER,GEN_ORIG_CORRESP_PTYPE);
    if (!ptype) {
        avt_errmsg(GNS_API_ERRMSG, "014", AVT_ERROR);
//        avt_error ("GNS", 0, AVT_WAR,"gns_KeepWorstInstanceDelayAWE : no model correspondance!\n");
        return NULL;
    }
    tab = gen_GetCorrespondingInstance(left,(corresp_t*)ptype->DATA);
    insnamelist = gen_GetInstanceWithSameRadical (gen_getlofig(tab->CRT->GENIUS_FIGNAME),
                                                  right
                                                  );
    ins2keep = gen_get_instance_loopdelayawe (flatfig,
                                              GEN_WORST,
                                              insname,
                                              begin,
                                              vdd,
                                              smax, smin,
                                              &tmax, &tmin, &fmax, &fmin );
    if (!ins2keep) {
        avt_errmsg(GNS_API_ERRMSG, "015", AVT_ERROR);
//        avt_error ("GNS", 0, AVT_WAR,"gns_KeepWorstInstanceDelayAWE : no instance founded !\n");
        return NULL;
    }
    rightunconcatname (ins2keep,&left,&right);
    if (!right) 
        gns_ReduceInstance (hierfig,left);
    else
    for (chain = insnamelist ; chain ; chain = chain->NEXT) {
        if (strcmp(right,(char*)chain->DATA)) {
            sprintf(instance_name,"%s.%s",left,(char*)chain->DATA);
            gns_ReduceInstance (hierfig,namealloc(instance_name));
        }
    }
    return gns_FlattenNetlist (hierfig,rc);
}

void gns_SetModelAsLeaf(char *name)
{
  if (name==NULL)
    {
      freechain(GENIUS_CATAL);
      GENIUS_CATAL=NULL;
    }
  else
    GENIUS_CATAL=addchain(GENIUS_CATAL, namealloc(name));
}

void gns_RenameInstanceFigure(lofig_list *lf, char *instance, char *origname, char *newname)
{
  loins_list *li;
  chain_list *ptchain;

  origname=namealloc(origname);
  newname=namealloc(newname);

  if (instance!=NULL) instance=namealloc(gen_makeinstancename(instance));
  

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      if ((instance==NULL || li->INSNAME==instance) && li->FIGNAME==origname)
        li->FIGNAME=newname;
    }

  freechain(lf->MODELCHAIN);
  lf->MODELCHAIN=NULL;

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      for (ptchain = lf->MODELCHAIN; ptchain && ptchain->DATA!=li->FIGNAME; ptchain = ptchain->NEXT) ;

      if (ptchain==NULL)
        lf->MODELCHAIN=addchain(lf->MODELCHAIN, li->FIGNAME);
    }
}



static lofig_list *flatbbox(loins_list *li, lofig_list *lf)
{
  mbkContext *curctx;
  lofig_list *flatbb;
  curctx=mbkCreateContext();
  transfert_needed_lofigs(lf, curctx);
  flatbb=rduplofig(lf);
  lofigchain(flatbb);
  mbk_transfert_loins_params(li, lf, flatbb);
  mbkSwitchContext(curctx);
  flatbb=flatOutsideList(flatbb);
  mbkFreeAndSwitchContext(curctx);
  return flatbb;
}

void gns_FillBlackBoxes(lofig_list *lf, chain_list *moremodels)
{
  loins_list *li, *rli;
  lofig_list *bblf, *flatbblf;
  ptype_list *pt;
  subinst_t *sins;
  char temp[1024], *nn;
  chain_list *names=NULL, *list=NULL, *cl;
  ht *ht1;
  long l;

  ht1=addht(128);
  for (cl=moremodels; cl!=NULL; cl=cl->NEXT)
    addhtitem(ht1, ((lofig_list *)cl->DATA)->NAME, (long)cl->DATA);

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      if ((pt=getptype(li->USER, GEN_POST_BBOX_PTYPE))!=NULL)
        {
          sins=(subinst_t *)pt->DATA;
          li->USER=delptype(li->USER, GEN_POST_BBOX_PTYPE);

          if ((l=gethtitem(ht1, li->FIGNAME))!=EMPTYHT) bblf=(lofig_list *)l;
          else bblf=gns_GetBlackboxNetlist(li->FIGNAME);

          sprintf(temp,"_tempfill_%s", li->FIGNAME);
          nn=namealloc(temp);
          li->FIGNAME=nn;
          if (gethtitem(HT_LOFIG, nn)==EMPTYHT)
            {
//              printf(".flattening bbox '%s', ins '%s'\n", nn, sins->CRT->GENIUS_INSNAME);
              rli=gen_findinstance(GENIUS_GLOBAL_LOFIG, sins->CRT->GENIUS_INSNAME);
              flatbblf=flatbbox(rli, bblf);
              flatbblf->NAME=nn;
              addhtitem(HT_LOFIG, nn, (long)flatbblf);
              names=addchain(names, flatbblf);
            }
          list=addchain(list, li);
        }
    }

  delht(ht1);

  if (list!=NULL)
    {
      AdvancedNameAllocator *ana;
      ana=CreateAdvancedNameAllocator(CASE_SENSITIVE);

      flattenlofig_bypointer(lf, list, ana, 'Y');
      flatten_setup_realname_from_hiername(lf, ana);

      FreeAdvancedNameAllocator(ana);
      freechain(list);
      for (list=names; list!=NULL; list=list->NEXT)
        {
          bblf=(lofig_list *)list->DATA;
          delhtitem(HT_LOFIG, bblf->NAME);
          freelofig(bblf);
        }
      freechain(names);
    }
  gen_clean_lofig(lf);
  rcn_mergectclofig(lf);
}

void gns_ChangeNetlistName(lofig_list *lf, char *name)
{
  lf->NAME=namealloc(name);
}


static void gen_strip_lofig (lofig_list *fig, int keepinterf)
{
    int only_external;
    losig_list *sig;
    ptype_list *ptype;
    chain_list *ch;
    
    for (sig = fig->LOSIG; sig; sig = sig->NEXT) 
      {
        sig->FLAGS =0;
        ptype = getptype (sig->USER, LOFIGCHAIN);
        if (ptype!=NULL)
          {
            for (ch = (chain_list*)ptype->DATA; ch; ch = ch->NEXT)
              ((locon_list*)ch->DATA)->FLAGS = 0;
          }
        if ((sig->TYPE==INTERNAL || !keepinterf) && ptype!=NULL)
          {
            if (!ptype->DATA)
              sig->FLAGS = 'D';
            else 
              {
                only_external = 1;
                for (ch = (chain_list*)ptype->DATA; ch; ch = ch->NEXT)
                  if (((locon_list*)ch->DATA)->TYPE != 'E') {
                    only_external = 0;
                    break;
                  }
                if (only_external) {
                  freelorcnet (sig);
                  for (ch = (chain_list*)ptype->DATA; ch; ch = ch->NEXT)
                    ((locon_list*)ch->DATA)->FLAGS = 'D';
                  
                  sig->FLAGS = 'D';
                }
              }
          }
      }
    gen_del_flagged_losig (fig, 'D');
    gen_del_flagged_locon (fig, 'D');
    lofigchain(fig);
    for (sig = fig->LOSIG; sig; sig = sig->NEXT)
      rcn_add_low_capa_ifneeded(sig, gen_get_ground(fig));
}
