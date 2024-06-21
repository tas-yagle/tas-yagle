#include AVT_H
#include GEN_H

chain_list *ALL_LOINS;
chain_list *ALL_LOTRS;
chain_list *ALL_HIERARCHY_LOTRS;
chain_list *ALL_SWAPS;
lofig_list *current_lofig;
ptype_list *ALL_ENV, *GEN_USER;
lofig_list *CUR_HIER_LOFIG;
corresp_t *CUR_CORRESP_TABLE;
ALL_FOR_GNS *LATEST_GNS_RUN;
lofig_list *GENIUS_GLOBAL_LOFIG;
ht *GNS_TEMPLATE_HT;
lofig_list *GEN_HEAD_LOFIG = NULL;
ht *GEN_HT_LOFIG = NULL;

chain_list *reject_list;

static mbkContext *EXTERNAL_LOFIGS;

lofig_list *(*external_getlofig)(char *name);

mbkContext *genius_external_getcontext()
{
  return EXTERNAL_LOFIGS;
}

void genius_external_setcontext(mbkContext *ctx)
{
  EXTERNAL_LOFIGS=ctx;
}

lofig_list *genius_external_getlofig(char *name)
{
  lofig_list *lf;
  mbkSwitchContext(EXTERNAL_LOFIGS);  
  lf=getlofig(name, 'A');
  mbkSwitchContext(EXTERNAL_LOFIGS);
  return lf;
}

typedef struct
{
  ht *oHT;
  lofig_list *oHL;
  corresp_t *oCT;
  lofig_list *oCHL;
  lofig_list *oMAINLF;
  ptype_list *var;
  mbkContext *oldctx;
  mbkContext newcontext;
  lofig_list *(*tmpfunc)(char *name);
  ht *templateht;
} __ctx;

static chain_list *__ctx_chain=NULL;


lofig_list *gen_getlofig (char *name)
{
  lofig_list *head_lofig, *fig;
  ht *ht_lofig;

  // Contextualize HEAD_LOFIG ___________________________

  head_lofig = HEAD_LOFIG;
  HEAD_LOFIG = GEN_HEAD_LOFIG;

  ht_lofig = HT_LOFIG;
  HT_LOFIG = GEN_HT_LOFIG;

  fig = getloadedlofig (name);

  // Release HEAD_LOFIG _________________________________

  GEN_HEAD_LOFIG = HEAD_LOFIG;
  HEAD_LOFIG = head_lofig;

  GEN_HT_LOFIG = HT_LOFIG;
  HT_LOFIG = ht_lofig;

  return fig;
}

void in_genius_context_of(ALL_FOR_GNS *all, subinst_t *sins)
{
  ptype_list *var;
  corresp_t *table;
  char temp[1024];
  __ctx *tmp;


  tmp=(__ctx *)mbkalloc(sizeof(__ctx));
  tmp->oHT=GEN_HT_LOFIG; tmp->oHL=GEN_HEAD_LOFIG;
  tmp->oCHL=CUR_HIER_LOFIG; tmp->oCT=CUR_CORRESP_TABLE;
  tmp->oMAINLF=GENIUS_GLOBAL_LOFIG; tmp->var=ALL_ENV;
  tmp->oldctx=genius_external_getcontext();
  tmp->newcontext.HT_LOFIG=HT_LOFIG;
  tmp->newcontext.HEAD_LOFIG=HEAD_LOFIG;
  tmp->tmpfunc=external_getlofig;
  tmp->templateht=GNS_TEMPLATE_HT;

  external_getlofig=genius_external_getlofig;
  genius_external_setcontext(&all->external_ctx);

  __ctx_chain=addchain(__ctx_chain, tmp);

  table=sins->CRT;
  var=table->VAR;
  var=addptype(var, (long)table->GENIUS_FIGNAME, namealloc("model"));
  var=addptype(var, (long)table->GENIUS_INSNAME, namealloc("instance"));

  if (table->ARCHISTART==0)
    var=addptype(var, (long)table->FIGNAME, namealloc("archi"));
  else
    {
      strncpy(temp, &table->GENIUS_FIGNAME[(int)table->ARCHISTART], table->ARCHILENGTH);
      temp[(int)table->ARCHILENGTH]='\0';
      var=addptype(var, (long)namealloc(temp), namealloc("archi"));
    }

  GENIUS_GLOBAL_LOFIG=all->GLOBAL_LOFIG;
  GEN_HEAD_LOFIG = all->HIER_HEAD_LOFIG;
  GEN_HT_LOFIG = all->HIER_HT_LOFIG;
  CUR_HIER_LOFIG=gen_getlofig(table->GENIUS_FIGNAME);
  CUR_CORRESP_TABLE=table;
  GEN_USER=NULL;
  ALL_ENV=var;
  GNS_TEMPLATE_HT=all->TEMPLATE_HT;
  current_lofig=NULL;
}

void out_genius_context_of()
{
  __ctx *tmp;
  chain_list *cl;
  if (__ctx_chain==NULL) return;

  tmp=(__ctx *)__ctx_chain->DATA;
  ALL_ENV->NEXT->NEXT->NEXT=NULL;
  freeptype(ALL_ENV);
  external_getlofig=tmp->tmpfunc;
  genius_external_setcontext(tmp->oldctx);
  GEN_HT_LOFIG=tmp->oHT; GEN_HEAD_LOFIG=tmp->oHL;
  CUR_HIER_LOFIG=tmp->oCHL; CUR_CORRESP_TABLE=tmp->oCT;
  GENIUS_GLOBAL_LOFIG=tmp->oMAINLF; ALL_ENV=tmp->var;
  GNS_TEMPLATE_HT=tmp->templateht;
  mbkfree(tmp);
  cl=__ctx_chain; __ctx_chain=__ctx_chain->NEXT;
  cl->NEXT=NULL;
  freechain(cl);
}

char *gen_makeinstancename (char *s)
{
  int i, j;
  static char temp[200];

  for (i = 0, j = 0; s[i] != '\0'; i++) {
    if (s[i] != ' ' && s[i] != ')' && s[i] != ']')
      temp[j++] = s[i];
  }
  temp[j] = '\0';

  for (i = 0; i < j; i++)
    if (temp[i] == '[' || temp[i] == '(')
      temp[i] = '_';

  return temp;
}

chain_list *gen_hierarchical_split (char *name)
{
  char *s, *start = name;
  chain_list *cl = NULL;

  s = strchr (start, '.');

  while (s) {
    *s = '\0';
    cl = addchain (cl, namealloc (start));
    *s = '.';
    start = s + 1;
    s = strchr (start, '.');
  }
  cl = addchain (cl, namealloc (start));

  return reverse (cl);
}
chain_list *gen_goto (chain_list * arbo, corresp_t ** crt, int tag)
{
  corresp_t *crt0, *level;
  subinst_t *cl;
  level = *crt;
  while (arbo->NEXT != NULL) {
    for (cl = level->SUBINSTS; cl != NULL; cl = cl->NEXT) {
      crt0 = cl->CRT;
      if (cl->INSNAME == arbo->DATA)
        break;
    }
    if (cl == NULL || (cl->FLAGS & LOINS_IS_BLACKBOX)!=0)
      {
        if (tag==0)
          return NULL;
        return arbo;
      }
    arbo = arbo->NEXT;
    level = crt0;
  }
  *crt = level;
  return arbo;
}

static char *repack(chain_list *lst, char *buf)
{
  strcpy(buf,"");
  while (lst!=NULL)
    {
      strcat(buf, lst->DATA);
      if (lst->NEXT!=NULL)
        strcat(buf, ".");
      lst=lst->NEXT;
    }
  return buf;
}

lotrs_list *_gen_GetCorrespondingTransistor(char *name, corresp_t * level, int mode)
{
  chain_list *arbo, *mainarbo;
  long entry;
  char *trname;
  char buf[200];

  mainarbo = arbo = gen_hierarchical_split (gen_makeinstancename (name));

  arbo = gen_goto (arbo, &level, 1);
  if (arbo == NULL) {
    freechain (mainarbo);
    if (!mode) avt_errmsg(GNS_ERRMSG, "036", AVT_ERROR, gen_info (), name);
             //fprintf (stderr, "%s: gns_GetCorrespondingTransistor() can't go thru '%s'\n", gen_info (), name);
    return NULL;
  }
    
  if (level->TRANSISTORS!=NULL)
    {
      if (arbo->NEXT==NULL) trname=arbo->DATA;
      else trname=namealloc(repack(arbo, buf));

      entry=gethtitem(level->TRANSISTORS, trname);
      
      freechain (mainarbo);        
      if (entry!=EMPTYHT) return (lotrs_list *)entry;
    }
  if (!mode) avt_errmsg(GNS_ERRMSG, "006", AVT_ERROR, gen_info (), name);
             //fprintf (stderr, "%s: gns_GetCorrespondingTransistor() can't find transistor '%s' in model\n", gen_info (), name);
  return NULL;
}

lotrs_list *gen_GetCorrespondingTransistor(char *name, corresp_t * level)
{
  return _gen_GetCorrespondingTransistor(name, level, 0); // display errors
}

subinst_t *gen_GetCorrespondingInstance (char *name, corresp_t * level)
{
  subinst_t *cl;
  corresp_t *crt;
  chain_list *arbo, *mainarbo;
  char buf[200];
  char *insname;

  mainarbo = arbo = gen_hierarchical_split (gen_makeinstancename (name));

  arbo = gen_goto (arbo, &level, 1);
  if (arbo == NULL) {
    freechain (mainarbo);
    avt_errmsg(GNS_ERRMSG, "036", AVT_FATAL, gen_info (), name);
    //fprintf (stderr, "%s: gns_GetCorrespondingInstance() can't go thru '%s'\n", gen_info (), name);
    EXIT (1);
  }

  if (arbo->NEXT==NULL) insname=arbo->DATA;
  else insname=namealloc(repack(arbo, buf));

  for (cl = level->SUBINSTS; cl != NULL; cl = cl->NEXT) {
    crt = cl->CRT;
    if (cl->INSNAME == insname) {
      freechain (mainarbo);
      return cl;
    }
  }
  freechain (mainarbo);        
  avt_errmsg(GNS_ERRMSG, "037", AVT_ERROR, gen_info (), name);
  //fprintf (stderr, "%s: gns_GetCorrespondingInstance() can't find instance '%s' in model\n", gen_info (), name);
  return NULL;
}

subinst_t *gen_get_hier_instance(ALL_FOR_GNS *all, char *hiername)
{
  char temp[1024], *c, *nm;
  subinst_t *sins;
  strcpy(temp, hiername);
  c=strchr(temp,'.');
  if (c!=NULL)
    {
      *c='\0';
    }
  nm=namealloc(temp);
  for (sins=all->TOP_LEVEL_SUBINST; sins!=NULL && sins->INSNAME!=nm; sins=sins->NEXT) ;
  if (c==NULL || sins==NULL) return sins;
  return gen_GetCorrespondingInstance(&c[1], sins->CRT);
}

int is_genius_instance(lofig_list *lf, char *name)
{
  if (LATEST_GNS_RUN==NULL) return 0;
  if (LATEST_GNS_RUN->GLOBAL_LOFIG!=lf) return 0;
  if (gen_get_hier_instance(LATEST_GNS_RUN, name)!=NULL) return 1;
  return 0;
}

HierLofigInfo *gethierlofiginfo(lofig_list *lf)
{
  ptype_list *p;
  HierLofigInfo *hli;

  if ((p=getptype(lf->USER, HIER_LOFIG_INFO_PTYPE))!=NULL)
    return (HierLofigInfo *)p->DATA;

  hli=(HierLofigInfo *)mbkalloc(sizeof(HierLofigInfo));
  hli->BUILD_TTV=NULL;
  hli->BUILD_BEH=NULL;
  hli->ARCS=addht(16);
  lf->USER=addptype(lf->USER, HIER_LOFIG_INFO_PTYPE, hli);
  return hli;
}

ArcInfo *getarcinfo(HierLofigInfo *hli, char *name)
{
  long l;
  ArcInfo *ai;
  if ((l=gethtitem(hli->ARCS, name))!=EMPTYHT)
    return (ArcInfo *)l;

  ai=(ArcInfo *)mbkalloc(sizeof(ArcInfo));
  ai->arc_name=name;
  ai->MODEL=NULL;
  ai->SIM=NULL;
  ai->ENV=NULL;
  ai->CTK_ENV=NULL;
  addhtitem(hli->ARCS, name, (long)ai);
  return ai;
}

ht *gen_get_losig_ht(lofig_list *lf)
{
  ht *sight;
  ptype_list *pt;
  losig_list *ls;

  if ((pt=getptype(lf->USER, GEN_HIERLOFIG_LOSIG_HT))!=NULL)
    return (ht *)pt->DATA;
  
  sight=addht(1024);
  
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    addhtitem(sight, ls->NAMECHAIN->DATA, (long)ls);

  lf->USER=addptype(lf->USER, GEN_HIERLOFIG_LOSIG_HT, sight);

  return sight;
}

void destroy_genius_context(ALL_FOR_GNS *all)
{
/*  ptype_list *var;
  corresp_t *table;
  char temp[1024];
  __ctx *tmp;


  tmp=(__ctx *)mbkalloc(sizeof(__ctx));
  tmp->oHT=GEN_HT_LOFIG; tmp->oHL=GEN_HEAD_LOFIG;
  tmp->oCHL=CUR_HIER_LOFIG; tmp->oCT=CUR_CORRESP_TABLE;
  tmp->oMAINLF=GENIUS_GLOBAL_LOFIG; tmp->var=ALL_ENV;
  tmp->oldctx=genius_external_getcontext();
  tmp->newcontext.HT_LOFIG=HT_LOFIG;
  tmp->newcontext.HEAD_LOFIG=HEAD_LOFIG;
  tmp->tmpfunc=external_getlofig;
  tmp->templateht=GNS_TEMPLATE_HT;

  external_getlofig=genius_external_getlofig;
  genius_external_setcontext(&tmp->newcontext);

  __ctx_chain=addchain(__ctx_chain, tmp);

  table=sins->CRT;
  var=table->VAR;
  var=addptype(var, (long)table->GENIUS_FIGNAME, namealloc("model"));
  var=addptype(var, (long)table->GENIUS_INSNAME, namealloc("instance"));

  if (table->ARCHISTART==0)
    var=addptype(var, (long)table->FIGNAME, namealloc("archi"));
  else
    {
      strncpy(temp, &table->GENIUS_FIGNAME[(int)table->ARCHISTART], table->ARCHILENGTH);
      temp[(int)table->ARCHILENGTH]='\0';
      var=addptype(var, (long)namealloc(temp), namealloc("archi"));
    }

  GENIUS_GLOBAL_LOFIG=all->GLOBAL_LOFIG;
  GEN_HEAD_LOFIG = all->HIER_HEAD_LOFIG;
  GEN_HT_LOFIG = all->HIER_HT_LOFIG;
  CUR_HIER_LOFIG=gen_getlofig(table->GENIUS_FIGNAME);
  CUR_CORRESP_TABLE=table;
  GEN_USER=NULL;
  ALL_ENV=var;
  GNS_TEMPLATE_HT=all->TEMPLATE_HT;
  current_lofig=NULL;
  // le external aussi
 */

  all = NULL;
}

