
#include AVT_H
#include API_H
#include "gen_corresp.h"
#define VERY_SMALL_PRIME 11

// Globals ____________________________________________________________________
//_____________________________________________________________________________

int CORRESP_DEBUG = 0;
int NO_UPDATE = 0;

HeapAlloc corresp_heap;
HeapAlloc subinst_heap;

void CorrespHeap_Manage (int mode)
{
  if (getenv("GEN_DEBUG_CORRESP")!=NULL)
    CORRESP_DEBUG=1;

  if (mode == 0) {
    CreateHeap (sizeof (subinst_t), 0, &subinst_heap);
    CreateHeap (sizeof (corresp_t), 0, &corresp_heap);
  }
  else {
    DeleteHeap (&corresp_heap);
    DeleteHeap (&subinst_heap);
  }
}

static int unused_tag;

int gen_wasunused()
{
  return unused_tag;
}

char *gen_vectorize (char *radix, int index)
{
  char buf[2048];

  sprintf (buf, "%s %d", radix, index);
  return namealloc (buf);
}

//_____________________________________________________________________________
//_____________________________________________________________________________

char *gen_losigname (losig_list * sig)
{
  return (char *)sig->NAMECHAIN->DATA;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

int gen_upper_bound (losig_list * sig)
{
  tree_list *array_sig;
  int end = -2;
  ptype_list *ptype;

  if ((ptype = getptype (sig->USER, GEN_REAL_RANGE_PTYPE))) {
    if ((array_sig = (tree_list *) ptype->DATA)) {
      if (gen_is_token_to (TOKEN (array_sig)))
        end = Eval_Exp_VHDL (array_sig->NEXT->NEXT->DATA, ALL_ENV);
      else if (gen_is_token_downto (TOKEN (array_sig)))
        end = Eval_Exp_VHDL (array_sig->NEXT->DATA, ALL_ENV);
      else {
        avt_errmsg(GNS_ERRMSG, "005", AVT_WARNING, 1);
        //fprintf (stderr, "warning - no range for %s\n", (char *)sig->NAMECHAIN->DATA);
        end = -1;
      }
    }
    else
      end = -1;
  }

  return end;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

int gen_lower_bound (losig_list * sig)
{
  tree_list *array_sig;
  int begin = -2;
  ptype_list *ptype;

  if ((ptype = getptype (sig->USER, GEN_REAL_RANGE_PTYPE))) {
    if ((array_sig = (tree_list *) ptype->DATA)) {
      if (gen_is_token_to (TOKEN (array_sig)))
        begin = Eval_Exp_VHDL (array_sig->NEXT->DATA, ALL_ENV);
      else if (gen_is_token_downto (TOKEN (array_sig)))
        begin = Eval_Exp_VHDL (array_sig->NEXT->NEXT->DATA, ALL_ENV);
      else {
        avt_errmsg(GNS_ERRMSG, "005", AVT_WARNING, 2);
//        fprintf (stderr, "warning - no range for %s\n", (char *)sig->NAMECHAIN->DATA);
        begin = -1;
      }
    }
    else
      begin = -1;
  }

  return begin;
}

void gen_get_bounds (losig_list *sig, int *left, int *right)
{
  tree_list *array_sig;
  ptype_list *ptype;

  *left=*right=-1;
  if ((ptype = getptype (sig->USER, GEN_REAL_RANGE_PTYPE))) {
    if ((array_sig = (tree_list *) ptype->DATA)) {
      if (gen_is_token_to (TOKEN (array_sig)) || gen_is_token_downto (TOKEN (array_sig)))
        {
          *left = Eval_Exp_VHDL (array_sig->NEXT->DATA, ALL_ENV);
          *right = Eval_Exp_VHDL (array_sig->NEXT->NEXT->DATA, ALL_ENV);
        }
      else 
        avt_errmsg(GNS_ERRMSG, "005", AVT_WARNING, 3);
//        fprintf (stderr, "warning - no range for %s\n", (char *)sig->NAMECHAIN->DATA);
    }
  }
}

//_____________________________________________________________________________
//_____________________________________________________________________________

char *gen_makesignalname (char *s)
{
  int i, j;
  int last = -1;
  static char temp[200];

  for (i = 0, j = 0; s[i] != '\0'; i++) {
    if (s[i] != ')' && s[i] != ']')
      temp[j++] = s[i];
    if (s[i] == '.')
      last = j-1;
  }
  temp[j] = '\0';

  for (i = 0; i < j; i++)
    if (temp[i] == '[' || temp[i] == '(') {
      if (last == -1 || i > last)
        temp[i] = ' ';
      else
        temp[i] = '_';
    }

  return temp;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

lotrs_list *gen_get_corresponding_transistor (char *name)
{
  chain_list *arbo, *lilist;
  loins_list *modelli;
  lotrs_list *lt, *modellt;
  ptype_list *ptype;
  char *nname, *radname;

  arbo = gen_hierarchical_split (gen_makeinstancename (name));
  if (arbo->NEXT && current_lofig->LOINS) {
    freechain (arbo);
    avt_errmsg(GNS_ERRMSG, "006", AVT_FATAL, gen_info (), name);
    /*fprintf (stderr, "%s: gns_get_corresponding_transistor() can't find transistor '%s' in model\n",
             gen_info (), name);*/
    EXIT (1);
  }
  freechain (arbo);
  nname=namealloc(name);
  radname=vectorradical (name);
  if (current_lofig->LOINS) {
    lilist = NULL;
    if (vectorindex (nname) != -1) {
      for (lilist = originalloins; lilist; lilist = lilist->NEXT) {
        if (radname == ((loins_list *) lilist->DATA)->INSNAME)
          break;
      }
    }

    if (lilist) {
      // c'est une requete sur une instance expansee
      char temp[200], *s;
      sprintf (temp, "%s_exp%s", radname, strchr (nname, ' '));
      while ((s = strchr (temp, ' ')))
        *s = '_';
      s = namealloc (temp);
      nname = s;
      for (lilist = expansedloins; lilist; lilist = lilist->NEXT) {
        if (nname == ((loins_list *) lilist->DATA)->INSNAME)
          break;
      }
      if (lilist)
        modelli = (loins_list *) lilist->DATA;
      else
        modelli = NULL;
    }
    else {
      for (modelli = current_lofig->LOINS;
           modelli && modelli->INSNAME != radname; modelli = modelli->NEXT);
    }

    if (modelli) {
      for (lilist = ALL_LOTRS; lilist; lilist = lilist->NEXT) {
        lt = (lotrs_list *) lilist->DATA;
        if (!lt->USER || lt->USER->TYPE != GEN_VISITED_PTYPE)
          EXIT (50);

        if (lt->USER->DATA == modelli)
          break;
      }
      if (lilist) {
        return (lotrs_list *) lt;
      }
    }
  }
  else {
    for (modellt = current_lofig->LOTRS;
         modellt && modellt->TRNAME != radname; modellt = modellt->NEXT);

    if (modellt) {
      if ((ptype = getptype (modellt->USER, FCL_CORRESP_PTYPE)) != NULL) return ptype->DATA;
      else return NULL;
    }
  }
  avt_errmsg(GNS_ERRMSG, "006", AVT_FATAL, gen_info (), name);
  /*fprintf (stderr, "%s: gns_get_corresponding_transistor () can't find transistor '%s' in model\n",
           gen_info (), name);*/
  EXIT (1);
  return NULL;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

losig_list *gen_get_corresponding_signal (char *name)
{
  losig_list *modells, *cirls;
  gensigchain_list *gs;
  ptype_list *ptype;
  char *nname, *radname;

  unused_tag=0;

  nname=namealloc(gen_makesignalname (name));
  radname=vectorradical (nname);

  for (modells = current_lofig->LOSIG;
       modells && !(modells->NAMECHAIN->DATA == radname ||
                    (!current_lofig->LOINS && modells->NAMECHAIN->DATA == nname));
       modells = modells->NEXT);

  if (modells) {
    if (current_lofig->LOINS) {
      // GENIUS model
      if ((ptype = getptype (modells->USER, GENSIGCHAIN))) {
        gs = (gensigchain_list *) ptype->DATA;
        if ((cirls = getlosigfrommark (gs, vectorindex (nname)))) {
          return cirls;
        }
        unused_tag=1;
      }
      else {
        return NULL;
      }
    }
    else {
      // FCL model
      if ((ptype = getptype (modells->USER, FCL_CORRESP_PTYPE)) != NULL) return (losig_list *)ptype->DATA;
      else return NULL;
    }
  }

  return NULL;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

corresp_t *gen_build_corresp_table (lofig_list * lf, chain_list * loins, chain_list * lotrs)
{
  losig_list *sig, *assoc_sig;
  lotrs_list *tr, *assoc_tr;
  loins_list *ins;
  int begin, end, i, n, index;
  ptype_list *ptype;
  corresp_t *level, *subinst;
  char buf[2048], mod_insname[1024], *name;
  chain_list *ch;
  subinst_t *sins;
  foundins_list *fil;
  long flags;

  level = (corresp_t *) AddHeapItem (&corresp_heap);	//mbkalloc (sizeof (struct corresp_t)); 
  level->SUBINSTS = NULL;
  level->SIGNALS = NULL;
  level->TRANSISTORS = NULL;
  level->FIGNAME = lf->NAME;
  level->FLAGS = 0;
  //    level->INSNAME=NULL;

  for (ptype = ALL_ENV; ptype; ptype = ptype->NEXT)
    if (ptype->DATA == namealloc ("instance"))
      level->GENIUS_INSNAME = (char *)ptype->TYPE;

  if (CORRESP_DEBUG > 1)
    fprintf (stdout, "\n____________ INSTANCE `%s'\n", level->GENIUS_INSNAME);

  // Circuit - Model corresp for signals

  n = 0;
  for (sig = lf->LOSIG; sig; sig = sig->NEXT) {
    begin = gen_lower_bound (sig);
    end = gen_upper_bound (sig);
    if (begin==UNDEF || end==UNDEF)
    {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 2);
      //fprintf(stderr,"could not compute range of signal '%s' in model '%s'\n",gen_losigname(sig), lf->NAME);
      EXIT(5);
    }
    if (begin != -2 && end != -2) {	// GENIUS model
      if (begin == -1 && end == -1)
        n++;
      else
        n += end - begin + 1;
    }
    else
      n++;
  }

  if (n)
    level->SIGNALS = addht (n * 2);

  for (sig = lf->LOSIG; sig; sig = sig->NEXT) {
    begin = gen_lower_bound (sig);
    end = gen_upper_bound (sig);
    if (begin != -2 && end != -2) {	// GENIUS model
      if (begin == -1 && end == -1) {
        if (!(assoc_sig = gen_get_corresponding_signal (gen_losigname (sig))))
          {
            if (gen_wasunused())
              {
                addhtitem (level->SIGNALS, sig->NAMECHAIN->DATA, (long)NULL);
              }
            continue;
          }
        addhtitem (level->SIGNALS, sig->NAMECHAIN->DATA, (long)assoc_sig);

        if (CORRESP_DEBUG > 1)
          fprintf (stdout, "   SIG %s -> %s\n", gen_losigname (sig), gen_losigname (assoc_sig));
      }
      else {
        for (i = begin; i <= end; i++) {
          sprintf (buf, "%s(%d)", gen_losigname (sig), i);
          if (!(assoc_sig = gen_get_corresponding_signal (buf)))
            {
              if (gen_wasunused())
                {
                  name = gen_vectorize (gen_losigname (sig), i);
                  addhtitem (level->SIGNALS, name, (long)NULL);
                }
              continue;
            }
          name = gen_vectorize (gen_losigname (sig), i);
          addhtitem (level->SIGNALS, name, (long)assoc_sig);
          if (CORRESP_DEBUG > 1)
            fprintf (stdout, "   SIG %s -> %s\n", name, gen_losigname (assoc_sig));
        }
      }
    }
    else {					// FCL model
      if (((index = vectorindex (gen_losigname (sig))) == -1))
        assoc_sig = gen_get_corresponding_signal (gen_losigname (sig));
      else {
        sprintf (buf, "%s(%d)", vectorradical (gen_losigname (sig)), index);
        assoc_sig = gen_get_corresponding_signal (buf);
      }
      if (!assoc_sig)
        continue;
      addhtitem (level->SIGNALS, sig->NAMECHAIN->DATA, (long)assoc_sig);
      if (CORRESP_DEBUG > 1)
        fprintf (stdout, "   SIG %s -> %s\n", gen_losigname (sig), gen_losigname (assoc_sig));
    }
  }

  // Circuit - Model corresp for transistors

  if (lf->LOTRS)				// FCL model
    for (n = 0, tr = lf->LOTRS; tr; n++, tr = tr->NEXT);
  else						// GENIUS model
    for (n = 0, ch = lotrs; ch; n++, ch = ch->NEXT);

  if (lf->LOTRS || lotrs)
    level->TRANSISTORS = addht (n * 2);

  if (lf->LOTRS)				// FCL model
    for (tr = lf->LOTRS; tr; tr = tr->NEXT) {
      tr->TRNAME = gen_canonize_trname (tr->TRNAME);
      assoc_tr = gen_get_corresponding_transistor (tr->TRNAME);
      addhtitem (level->TRANSISTORS, tr->TRNAME, (long)assoc_tr);
      if (CORRESP_DEBUG > 1) {
        if (assoc_tr->TRNAME)
          fprintf (stdout, "   TRS %s -> %s\n", (char *)tr->TRNAME, (char *)assoc_tr->TRNAME);
        else
          fprintf (stdout, "   TRS %s -> ???\n", (char *)tr->TRNAME);
      }
    }
  else						// GENIUS model
    for (ch = lotrs; ch; ch = ch->NEXT) {
      tr = (lotrs_list *) ch->DATA;
      if ((ptype = getptype (tr->USER, GEN_VISITED_PTYPE)))
        name = ((loins_list *) ptype->DATA)->INSNAME;
      else
        name = NULL;
      addhtitem (level->TRANSISTORS, name, (long)tr);

      if (CORRESP_DEBUG > 1) {
        if (assoc_tr->TRNAME)
          fprintf (stdout, "   TRS %s -> %s\n", name, (char *)assoc_tr->TRNAME);
        else
          fprintf (stdout, "   TRS %s -> ???\n", name);
      }
    }

  // Retrieve sub instances from GEN_USER_PTYPE, store them in level

  for (ch = loins; ch; ch = ch->NEXT) {
    fil = (foundins_list *) ch->DATA;
    ins = fil->LOINS;		// gen instance
    name = fil->VISITED->INSNAME;	// mod instance
    if ((ptype = getptype (ins->USER, GEN_LOOP_INDEX_PTYPE)))
      sprintf (mod_insname, "%s_%ld", name, (long)ptype->DATA);	// TODO parametrize instance vectorization
    else
      strcpy (mod_insname, name);

    sins = (subinst_t *) AddHeapItem (&subinst_heap);
    sins->INSNAME = namealloc (mod_insname);
    sins->CRT = NULL;
    if ((ptype = getptype (ins->USER, GEN_USER_PTYPE)))
      sins->CRT = subinst = (corresp_t *) ptype->DATA;
    else
      {
        exit(8);
        subinst = (corresp_t *) AddHeapItem (&corresp_heap);
        subinst->SUBINSTS = NULL;
        subinst->SIGNALS = NULL;
        subinst->TRANSISTORS = NULL;
        subinst->FIGNAME = ins->FIGNAME;
        subinst->GENIUS_FIGNAME = ins->FIGNAME;
        subinst->GENIUS_INSNAME = ins->INSNAME;
        subinst->FLAGS = LOINS_IS_BLACKBOX;
        ins->USER=addptype(ins->USER, GEN_USER_PTYPE, subinst);
        sins->CRT = subinst;
      }
    flags = (long)getptype (fil->VISITED->USER, GENIUS_LOINS_FLAGS_PTYPE)->DATA;
    sins->FLAGS = flags;
    if (fil->SAVEDFLAG) {
      sins->FLAGS |= LOINS_IS_BLACKBOX;
    }
    sins->NEXT = level->SUBINSTS;
    level->SUBINSTS = sins;
  }

  GEN_USER = addptype (NULL, GEN_USER_PTYPE, level);

  return level;
}

corresp_t *create_blackbox_corresp_table(loins_list *ins)
{
  corresp_t *subinst;
  locon_list *lc;
  int cnt;

  subinst = (corresp_t *) AddHeapItem (&corresp_heap);
  subinst->SUBINSTS = NULL;
  subinst->SIGNALS = NULL;
  subinst->TRANSISTORS = NULL;
  subinst->FIGNAME = ins->FIGNAME;
  subinst->GENIUS_FIGNAME = ins->FIGNAME;
  subinst->GENIUS_INSNAME = ins->INSNAME;
  subinst->FLAGS = LOINS_IS_BLACKBOX;
  subinst->VAR=NULL;
  subinst->ARCHISTART=0;
  subinst->ARCHILENGTH=0;
  ins->USER=addptype(ins->USER, GEN_USER_PTYPE, subinst);

  for (lc=ins->LOCON, cnt=0; lc!=NULL; lc=lc->NEXT, cnt++) ;
  subinst->SIGNALS = addht (cnt);

  for (lc=ins->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      addhtitem(subinst->SIGNALS, lc->NAME, (long)lc->SIG);
    }
  return subinst;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_free_corresp_table (corresp_t * dico)
{
  subinst_t *ch, *next;

  delht (dico->TRANSISTORS);
  delht (dico->SIGNALS);
  for (ch = dico->SUBINSTS; ch; ch = next) {
    next = ch->NEXT;
    gen_free_corresp_table (ch->CRT);
    DelHeapItem (&subinst_heap, ch);
  }
}


//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_drive_corresp_htable (FILE * f, char *name, char *path, int top, corresp_t * dico)
{
  long i;
  htitem *pEl;
  ht *pTable;
  char buf[1024];
  subinst_t *ch;
  void *nextkey ;
  long nextitem ;


  if (!top) {
    sprintf (buf, "%s.%s", path, dico->GENIUS_INSNAME);
    fprintf (f, "INSTANCE (%s) (\n", buf);
  }
  else {
    sprintf (buf, "%s", name);
    fprintf (f, "FIGURE (%s) (\n", buf);
  }

  fprintf (f, "    TRANSISTORS (\n");
  pTable = dico->TRANSISTORS;
  if (pTable) {
    scanhtkey( pTable, 1, &nextkey, &nextitem ) ;
    while( nextitem != EMPTYHT ) {
      fprintf (f, "        %s <=> %s\n", (char *)nextkey, ((lotrs_list *)nextitem)->TRNAME);
      scanhtkey( pTable, 0, &nextkey, &nextitem ) ;
    }
  }
  fprintf (f, "    )\n");

  fprintf (f, "    SIGNALS (\n");
  pTable = dico->SIGNALS;
  if (pTable) {
    scanhtkey( pTable, 1, &nextkey, &nextitem ) ;
    while( nextitem != EMPTYHT ) {
      fprintf (f, "        %s <=> %s\n", (char *)nextkey, gen_losigname ((losig_list *)nextitem));
      scanhtkey( pTable, 0, &nextkey, &nextitem ) ;
    }
  }
  fprintf (f, "    )\n");
  fprintf (f, ")\n\n");

  for (ch = dico->SUBINSTS; ch; ch = ch->NEXT)
    gen_drive_corresp_htable (f, NULL, buf, 0, ch->CRT);

}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_drive_corresp_table (FILE * f, char *path, int top, subinst_t * sins)
{
  long i;
  htitem *pEl;
  ht *pTable;
  char buf[1024];
  subinst_t *ch;
  void *nextkey ;
  long nextitem ;

  if (!top) {
    if (!strcmp (path, "NULL"))
      sprintf (buf, "%s", sins->INSNAME);
    else
      sprintf (buf, "%s.%s", path, sins->INSNAME);
  }
  else
    sprintf (buf, "NULL");

  pTable = sins->CRT->TRANSISTORS;
  if (pTable) {
    scanhtkey( pTable, 1, &nextkey, &nextitem ) ;
    while( nextitem != EMPTYHT ) {
      if (!top)
        fprintf (f, "T: %s.%s <=> %s\n", buf, (char *)nextkey, ((lotrs_list *) nextitem)->TRNAME);
      else
        fprintf (f, "T: %s <=> %s\n", (char *)nextkey, ((lotrs_list *) nextitem)->TRNAME);
      scanhtkey( pTable, 0, &nextkey, &nextitem ) ;
    }
  }
  fprintf (f, "\n");

  pTable = sins->CRT->SIGNALS;
  if (pTable) {
    scanhtkey( pTable, 1, &nextkey, &nextitem ) ;
    while( nextitem != EMPTYHT ) {
      if (!top)
        fprintf (f, "S: %s.%s <=> %s\n", buf, (char *)pEl->key, gen_losigname ((losig_list *) pEl->value));
      else
        fprintf (f, "S: %s <=> %s\n", (char *)pEl->key, gen_losigname ((losig_list *) pEl->value));
      scanhtkey( pTable, 0, &nextkey, &nextitem ) ;
    }
  }
  fprintf (f, "\n");

  for (ch = sins->CRT->SUBINSTS; ch; ch = ch->NEXT)
    gen_drive_corresp_table (f, buf, 0, ch);
}

//_____________________________________________________________________________
//_____________________________________________________________________________

lotrs_list *gen_hcorresp_trs (char *name, corresp_t * corresp)
{
  long trs;
  char *subname, *insname, buf[2048];
  subinst_t *ch;

  strcpy (buf, name);

  subname = strchr (buf, GEN_SEPAR);
  if (!subname) {
    if (corresp->TRANSISTORS) {
      if ((trs = gethtitem (corresp->TRANSISTORS, namealloc (buf))) != EMPTYHT)
        return (lotrs_list *) trs;
      else
        return NULL;	//transistor not found
    }
    else
      return NULL;		//no transistor in this instance
  }
  else {
    *subname++ = '\0';
    insname = namealloc (buf);
    for (ch = corresp->SUBINSTS; ch; ch = ch->NEXT)
      if (ch->INSNAME == insname)
        break;
    if (!ch)
      {
        if ((trs = gethtitem (corresp->TRANSISTORS, namealloc (name))) != EMPTYHT)
          return (lotrs_list *) trs;
        else
          return NULL;		//transistor not found
      }
    else
      return gen_hcorresp_trs (subname, ch->CRT);
  }
}

//_____________________________________________________________________________
//_____________________________________________________________________________

lotrs_list *gen_corresp_trs (char *name, corresp_t * crp_table)
{
  lotrs_list *trs;

  if (!(trs = gen_hcorresp_trs (name, crp_table)))
    return NULL;
  else {
    return trs;
  }
}

//_____________________________________________________________________________
//_____________________________________________________________________________


losig_list *gen_hcorresp_sig (char *name, corresp_t * corresp)
{
  long sig;
  char *subname, *insname, buf[2048];
  subinst_t *ch;

  unused_tag=0;
  strcpy (buf, name);

  subname = strchr (buf, GEN_SEPAR);
  if (!subname) {
    if ((sig = gethtitem (corresp->SIGNALS, namealloc (buf))) != EMPTYHT)
      {
        if ((losig_list *)sig==NULL) unused_tag=1;
        return (losig_list *) sig;
      }
    else
      return NULL;		//signal not found
  }
  else {
    *subname++ = '\0';
    insname = namealloc (buf);
    for (ch = corresp->SUBINSTS; ch; ch = ch->NEXT)
      if (ch->INSNAME == insname)
        break;
    if (!ch)
      {
        if ((sig = gethtitem (corresp->SIGNALS, namealloc (name))) != EMPTYHT)
          {
            if ((losig_list *)sig==NULL) unused_tag=1;
            return (losig_list *) sig;
          }
        else
          return NULL;		//instance not found
      }
    else
      return gen_hcorresp_sig (subname, ch->CRT);
  }
}

//_____________________________________________________________________________
//_____________________________________________________________________________

losig_list *gen_corresp_sig (char *name, corresp_t * crp_table)
{
  losig_list *sig;

  if (!(sig = gen_hcorresp_sig (name, crp_table)))
    return NULL;
  else {
    return sig;
  }
}

//_____________________________________________________________________________
//_____________________________________________________________________________
static int cnt = 0, max = 0;
static struct {
  char *name;
  int cnt;
  int nb;
  int tot;
} tab[100];

lofig_list *gen_add_real_corresp_ptypes (loins_list * ins, corresp_t * root_tbl, corresp_t * tbl, int *swapped)
{
  losig_list *f_corresp, *i_corresp;
  locon_list *f_con, *i_con, *ff_con;
  lofig_list *fig;
  ht *reverse_ht;
  char *name;
  int j, tot, unsolved;
  char temp[1024];
  chain_list *changed, *cl, *prev;

  cnt = 0;
  tot = 0;
  for (j = 0; j < max && tab[j].name != tbl->GENIUS_FIGNAME; j++);
  if (j >= max) {
    max++;
    tab[j].name = tbl->GENIUS_FIGNAME;
    tab[j].cnt = 0;
    tab[j].nb = 0;
  }
  
  // dump the root
  if (CORRESP_DEBUG > 0)
    fprintf (stdout, "\nINSTANCE `%s'\n", ins->INSNAME);
  if (CORRESP_DEBUG > 0)
    fprintf (stdout, "---ROOT\n");
  
  // get the figure
  fig = getlofig (tbl->GENIUS_FIGNAME /*ins->FIGNAME */ , 'A');
  
  for (i_con = ins->LOCON; i_con; i_con = i_con->NEXT) {
    if (i_con->SIG!=NULL)
      {
        name = gen_losigname (i_con->SIG);
        i_corresp = (losig_list *) gethtitem (root_tbl->SIGNALS, name);
        if ((long)i_corresp == EMPTYHT)
          avt_errmsg(GNS_ERRMSG, "005", AVT_WARNING, 4);
        //avt_fprintf (stdout, "" AVT_RED "" AVT_BOLD " - warning (in `%s'): no correspondance for connector `%s'\n", ins->INSNAME, i_con->NAME);
        else if (CORRESP_DEBUG > 0)
          fprintf (stdout, "   connector `%s' (`%s')\n", i_con->NAME, gen_losigname (i_corresp));
      }
  }
  

  // create reverse table
  if (CORRESP_DEBUG > 0)
    fprintf (stdout, "---INS\n");
  reverse_ht = addht (VERY_SMALL_PRIME);
  for (f_con = fig->LOCON, i_con=ins->LOCON; f_con; f_con = f_con->NEXT, i_con=i_con->NEXT) {
    f_con->FLAGS=0;
    if (i_con->SIG!=NULL)
      {
        if (f_con->NAME!=i_con->NAME) 
            { 
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 3);
              //printf("for %s: %s!=%s\n",tbl->GENIUS_FIGNAME, f_con->NAME,i_con->NAME);
              EXIT(77);
            }
        f_corresp = (losig_list *) gethtitem (tbl->SIGNALS, gen_losigname (f_con->SIG));
        if (CORRESP_DEBUG > 0)
          fprintf (stdout, "   connector `%s' (`%s')\n", f_con->NAME, gen_losigname (f_corresp));
        
        if (f_corresp==NULL)
          sprintf(temp,"%s*%d","?", i_con->FLAGS);
        else
          sprintf(temp,"%s*%d",gen_losigname (f_corresp), i_con->FLAGS);
        name=namealloc(temp);
        if (gethtitem (reverse_ht, name)==EMPTYHT)
          addhtitem (reverse_ht, name, (long)f_con);
        else
          // same signal on multiple symmetric connector
          addhtitem (reverse_ht, name, (long)1); 
      }
  }
  
  changed=NULL;
  unsolved=0;
  for (f_con = fig->LOCON, i_con=ins->LOCON; f_con; f_con = f_con->NEXT, i_con=i_con->NEXT)
    {
      tot++;
      if (i_con->SIG!=NULL)
        {
          f_corresp = (losig_list *) gethtitem (tbl->SIGNALS, f_con->NAME);
          i_corresp = (losig_list *) gethtitem (root_tbl->SIGNALS, gen_losigname (i_con->SIG));
          if (f_corresp != i_corresp)
            {
              *swapped = 1;
              cnt++;
              sprintf(temp,"%s*%d",gen_losigname (i_corresp), i_con->FLAGS);
              name=namealloc(temp);
              ff_con = (locon_list *) gethtitem (reverse_ht, name);
              if ((long)ff_con!=1) // if so, no swap is needed
                {
                  if ((long)ff_con==EMPTYHT)
                    {
                      avt_errmsg(GNS_ERRMSG, "007", AVT_FATAL, tbl->GENIUS_INSNAME,ins->INSNAME, f_con->NAME, gen_losigname(f_corresp),gen_losigname(i_corresp));
/*                      avt_fprintf(stdout,"" AVT_RED "" AVT_BOLD "Invalid symmetry detected for instance '%s' (%s) on connector '%s'<=>?\n", tbl->GENIUS_INSNAME,ins->INSNAME, f_con->NAME);
                      avt_fprintf(stdout,"%s %s\n", gen_losigname(f_corresp),gen_losigname(i_corresp));*/
                      EXIT(1);
                    }
                  f_con->USER = addptype (f_con->USER, FCL_REAL_CORRESP_PTYPE, ff_con->SIG);
                  if (CORRESP_DEBUG > 0)
                    fprintf (stdout, "      ===>>> real `%s' is `%s'\n", f_con->NAME, ff_con->NAME);
                  changed=addchain(changed, f_con);
                }
              else
                {
                  f_con->FLAGS=1;
                  unsolved++;
                }
            }
        }
    }
  
  for (f_con = fig->LOCON, i_con=ins->LOCON; unsolved>0 && f_con!=NULL; f_con = f_con->NEXT, i_con=i_con->NEXT)
    {
      if (f_con->FLAGS==1 && i_con->SIG!=NULL)
        {
          *swapped = 1;
          i_corresp = (losig_list *) gethtitem (root_tbl->SIGNALS, gen_losigname (i_con->SIG));
          sprintf(temp,"%s*%d",gen_losigname (i_corresp), i_con->FLAGS);
          name=namealloc(temp);
          
          // we look for a solution
          for (cl=changed, prev=NULL; cl!=NULL; prev=cl, cl=cl->NEXT)
            {
              ff_con=(locon_list *)cl->DATA;
              f_corresp = (losig_list *) gethtitem (tbl->SIGNALS, ff_con->NAME);
              sprintf(temp,"%s*%d",gen_losigname (f_corresp), i_con->FLAGS);
              if (name==namealloc(temp)) break;
            }
          
          if (cl!=NULL)
            {
              f_con->USER = addptype (f_con->USER, FCL_REAL_CORRESP_PTYPE, ff_con->SIG);
              if (prev==NULL) changed=cl->NEXT;
              else prev->NEXT=cl->NEXT;
              cl->NEXT=NULL;
              freechain(cl);
              unsolved--;
            }
        }
    }
  
  freechain(changed);
  tab[j].cnt += cnt;
  tab[j].nb++;
  tab[j].tot = tot;
  // delete reverse ht
  delht (reverse_ht);
  
  return fig;
}

static void displaycorrespdone(lofig_list *lf, loins_list *li)
{
  losig_list *ls;
  locon_list *lc, *lc0;
  ptype_list *pt;
  long wei, cpl;
  char g0[10], g1[10];

  avt_fprintf(stderr,"" AVT_RED "" AVT_BOLD "%20s %-20s %4s %4s\n", "Original", "Remapped on", "sym", "cpl");
  avt_fprintf(stderr,"" AVT_YELLOW "" AVT_BOLD "____________________ ____________________ ____ ____\n");
  for (lc=lf->LOCON, lc0=li->LOCON; lc!=NULL && lc0!=NULL; lc=lc->NEXT, lc0=lc0->NEXT)
    {
      pt=getptype(lc->USER, FCL_REAL_CORRESP_PTYPE);
      if (pt!=NULL)
        {
          ls=(losig_list *)pt->DATA;
          wei=lc0->FLAGS;
          sprintf(g0, "%4ld", wei);

          if ((pt=getptype(lc0->USER, FCL_COUPLING_PTYPE))!=NULL)
            {              
              cpl=(long)pt->DATA;
              sprintf(g1, "%4ld", cpl);
            }
          else strcpy(g1, "   ");
          avt_fprintf(stderr, "" AVT_RED "" AVT_BOLD "%19s > %-19s" AVT_RESET " %s %s\n", lc->NAME, gen_losigname(ls), g0, g1);
        }
    }
}
//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_check_corresp_table (loins_list * ins, corresp_t * root_tbl, corresp_t * tbl)
{
  losig_list *f_corresp, *i_corresp;
  locon_list *f_con, *i_con, *ff_con;
  lofig_list *fig;
  ht *reverse_ht;
  int swapped = 0;

  // dump the root
  if (CORRESP_DEBUG > 0)
    fprintf (stdout, "\nCHECK `%s'\n", ins->INSNAME);
  if (CORRESP_DEBUG > 0)
    fprintf (stdout, "---ROOT\n");
  for (i_con = ins->LOCON; i_con; i_con = i_con->NEXT) {
    if (!mbk_LosigIsVDD(i_con->SIG) && !mbk_LosigIsVSS(i_con->SIG)) {
      i_corresp = (losig_list *) gethtitem (root_tbl->SIGNALS, gen_losigname (i_con->SIG));
      if (CORRESP_DEBUG > 0)
        fprintf (stdout, "   connector `%s' (`%s')\n", i_con->NAME, gen_losigname (i_corresp));
    }
  }

  // get the figure
  fig = getlofig (tbl->GENIUS_FIGNAME, 'A');

  // create reverse table
  if (CORRESP_DEBUG > 0)
    fprintf (stdout, "---INS\n");
  reverse_ht = addht (VERY_SMALL_PRIME);
  for (f_con = fig->LOCON; f_con; f_con = f_con->NEXT) {
    f_corresp = (losig_list *) gethtitem (tbl->SIGNALS, gen_losigname (f_con->SIG));
    if (CORRESP_DEBUG > 0)
      fprintf (stdout, "   connector `%s' (`%s')\n", f_con->NAME, gen_losigname (f_corresp));
    addhtitem (reverse_ht, gen_losigname (f_corresp), (long)f_con);
  }

  // identify swaps
  for (f_con = fig->LOCON; f_con; f_con = f_con->NEXT) {
    for (i_con = ins->LOCON; i_con; i_con = i_con->NEXT)
      if (i_con->NAME == f_con->NAME)
        break;
    i_corresp = (losig_list *) gethtitem (root_tbl->SIGNALS, gen_losigname (i_con->SIG));
    if ((long)i_corresp != EMPTYHT) {
      if (!mbk_LosigIsVDD(i_corresp) && !mbk_LosigIsVSS(i_corresp)) {
        ff_con = (locon_list *) gethtitem (reverse_ht, gen_losigname (i_corresp));
        if (ff_con->NAME != f_con->NAME) {
          swapped = 1;
          if (CORRESP_DEBUG > 0)
            fprintf (stdout, "      ****** real `%s' is `%s'\n", f_con->NAME, ff_con->NAME);
        }
      }
    }
  }

  // delete reverse ht
  delht (reverse_ht);

  if (swapped/* && CORRESP_DEBUG > 0*/)
    avt_errmsg(GNS_ERRMSG, "005", AVT_WARNING, 5);
  //avt_fprintf (stdout, "" AVT_RED "" AVT_BOLD "   !!!!!!!!!!! CHECK FAILED on '%s'\n",ins->INSNAME);
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_clean_real_corresp_ptypes (lofig_list * fig)
{
  locon_list *f_con;

  // clean ptypes
  for (f_con = fig->LOCON; f_con; f_con = f_con->NEXT)
    if (getptype (f_con->USER, FCL_REAL_CORRESP_PTYPE))
      f_con->USER = delptype (f_con->USER, FCL_REAL_CORRESP_PTYPE);
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_swap_corresp_table (fclcorresp_list * fcl_clist, corresp_t * tbl, loins_list *li, lofig_list *lf)
{
  fclcorresp_list *cl;
  long corresp;
  subinst_t *ch;
  char *n_orig, *n_corresp;
  corresp_t *crp;
  ht *new_trs_ht, *new_sig_ht;

  if (CORRESP_DEBUG > 0)
    fprintf (stdout, "\nUPDATING `%s' (%s)\n", tbl->GENIUS_INSNAME, li->INSNAME);

  if (!fcl_clist) {
    displaycorrespdone(lf, li);
    avt_errmsg(GNS_ERRMSG, "008", AVT_FATAL, li->INSNAME, tbl->GENIUS_INSNAME, tbl->FIGNAME);
    //avt_error("gns", 2, AVT_ERR, "" AVT_RED "" AVT_BOLD " no FCL match for `%s' (instance:'%s' model:`%s')\n", li->INSNAME, tbl->GENIUS_INSNAME, tbl->FIGNAME);
    EXIT(1);
    return;
  }

  new_trs_ht = addht (VERY_SMALL_PRIME);
  new_sig_ht = addht (VERY_SMALL_PRIME);

  for (cl = fcl_clist; cl; cl = cl->NEXT) {
    if (cl->TYPE == 'S') {
      if (tbl->SIGNALS) {
        n_orig = gen_losigname ((losig_list *) cl->ORIG);
        n_corresp = gen_losigname ((losig_list *) cl->CORRESP);

        if (n_orig==n_corresp)
          {
            corresp =gethtitem (tbl->SIGNALS, n_orig);
            addhtitem (new_sig_ht, n_orig, corresp);
          }
        else
          {
            corresp = gethtitem (tbl->SIGNALS, n_corresp);
            if (corresp == EMPTYHT) 
            {
               avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 4);
               //      printf("zinaps: internal err 2\n");
               EXIT(5);
            }
            addhtitem (new_sig_ht, n_orig, corresp);
          }
        if (CORRESP_DEBUG > 0)
          fprintf (stdout, "   signal `%s' previously associted with `%s' is now associted with `%s'\n",
                   gen_losigname ((losig_list *) corresp), n_corresp, n_orig);
      }
    }
    if (cl->TYPE == 'T') {
      if (tbl->TRANSISTORS) {
        n_orig = ((lotrs_list *) cl->ORIG)->TRNAME;
        n_corresp = ((lotrs_list *) cl->CORRESP)->TRNAME;
        corresp = gethtitem (tbl->TRANSISTORS, n_corresp);
        addhtitem (new_trs_ht, n_orig, corresp);
        if (CORRESP_DEBUG > 0)
          fprintf (stdout, "   transistor `%s' previously associted with `%s' is now associted with `%s'\n",
                   ((lotrs_list *) corresp)->TRNAME?((lotrs_list *) corresp)->TRNAME:"?", n_corresp, n_orig);
      }
    }
  }

  if (tbl->SIGNALS) {
    delht (tbl->SIGNALS);
    tbl->SIGNALS = new_sig_ht;
  }
  else delht (new_sig_ht);

  if (tbl->TRANSISTORS) {
    delht (tbl->TRANSISTORS);
    tbl->TRANSISTORS = new_trs_ht;
  }
  else delht(new_trs_ht);

  for (ch = tbl->SUBINSTS; ch; ch = ch->NEXT) {
    crp = ch->CRT;
    for (cl = fcl_clist; cl; cl = cl->NEXT) {
      if (cl->TYPE == 'I') {
        n_orig = ((loins_list *) cl->ORIG)->INSNAME;
        n_corresp = ((loins_list *) cl->CORRESP)->INSNAME;
        if (n_corresp == ch->INSNAME) {
          ch->INSNAME = n_orig;
          if (CORRESP_DEBUG > 0)
            fprintf (stdout, "   table previously associted with `%s' is now associted with `%s'\n",
                     n_corresp, n_orig);
          break;
        }
      }
    }
  }
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_delalllofig ()
{
  lofig_list *head_lofig;
  ht *ht_lofig;

  // Contextualize HEAD_LOFIG ___________________________

  head_lofig = HEAD_LOFIG;
  HEAD_LOFIG = GEN_HEAD_LOFIG;

  ht_lofig = HT_LOFIG;
  HT_LOFIG = GEN_HT_LOFIG;

  delalllofig();

  
  // Release HEAD_LOFIG _________________________________

  GEN_HEAD_LOFIG = HEAD_LOFIG;
  HEAD_LOFIG = head_lofig;

  GEN_HT_LOFIG = HT_LOFIG;
  HT_LOFIG = ht_lofig;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

void gen_update_corresp_table (lofig_list * lf, corresp_t * crp_table, loins_list *li)
{
  lofig_list *head_lofig;
  ht *ht_lofig;

  if (NO_UPDATE)
    return;

  // Contextualize HEAD_LOFIG ___________________________

  head_lofig = HEAD_LOFIG;
  HEAD_LOFIG = GEN_HEAD_LOFIG;

  ht_lofig = HT_LOFIG;
  HT_LOFIG = GEN_HT_LOFIG;

  // top level instances have the connectors in the wrong way
  // so we invert them just for this operation

  li->LOCON=(locon_list *)reverse((chain_list *)li->LOCON);

  gen_update_corresp_table__recursive (lf, crp_table, li);

  li->LOCON=(locon_list *)reverse((chain_list *)li->LOCON);

  // Release HEAD_LOFIG _________________________________

  GEN_HEAD_LOFIG = HEAD_LOFIG;
  HEAD_LOFIG = head_lofig;

  GEN_HT_LOFIG = HT_LOFIG;
  HT_LOFIG = ht_lofig;
}

void gen_add_all_lofig_weight()
{
  lofig_list *head_lofig, *fig;
  ht *ht_lofig;
  // Contextualize HEAD_LOFIG ___________________________

  head_lofig = HEAD_LOFIG;
  HEAD_LOFIG = GEN_HEAD_LOFIG;

  ht_lofig = HT_LOFIG;
  HT_LOFIG = GEN_HT_LOFIG;

  for (fig = GEN_HEAD_LOFIG; fig; fig = fig->NEXT) {
    lofigchain (fig);
    ComputeWeightsForLoinsConnectors (fig);
  }

  GEN_HEAD_LOFIG = HEAD_LOFIG;
  HEAD_LOFIG = head_lofig;

  GEN_HT_LOFIG = HT_LOFIG;
  HT_LOFIG = ht_lofig;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

static lofig_list *addtoprealcorrespptype(loins_list *li, corresp_t *tbl, int *swapped)
{
  locon_list *lc0;
  lofig_list *fig;
  corresp_t fakeroot;

  fakeroot.SIGNALS=addht(1024);
  for (lc0=li->LOCON; lc0!=NULL; lc0=lc0->NEXT)
    {
      if (lc0->SIG!=NULL)
        addhtitem(fakeroot.SIGNALS, lc0->SIG->NAMECHAIN->DATA, (long)lc0->SIG);
    }

  fig = gen_add_real_corresp_ptypes (li, &fakeroot, tbl, swapped);

  delht(fakeroot.SIGNALS);

  return fig;
}

static void completecorresp(loins_list *li, corresp_t *tbl)
{
  locon_list *lc0;
  for (lc0=li->LOCON; lc0!=NULL; lc0=lc0->NEXT)
    {
      if (lc0->SIG==NULL)
        addhtitem(tbl->SIGNALS, lc0->NAME, (long)NULL);
    }
}

void gen_update_corresp_table__recursive (lofig_list * lf, corresp_t * root_tbl, loins_list *li)
{
  loins_list *ins;
  lofig_list *fig;
  fclcorresp_list *fcl_clist;
  subinst_t *ch;
  corresp_t *tbl;
  int swapped;


  if (li!=NULL) // top level pour alignement des vecteurs
    {
      swapped = 0;
      fig=addtoprealcorrespptype(li, root_tbl, &swapped);

      if (swapped)
        {

          if (root_tbl->FLAGS & LOINS_IS_UPDATED)
            {
              avt_errmsg(GNS_ERRMSG, "009", AVT_FATAL, li->INSNAME,li->FIGNAME);      
//              avt_fprintf(stderr,"[GNS][ " AVT_RED "" AVT_BOLD "ERROR" AVT_RESET " ] Vector ordering failed on instance '%s' (%s) who might be used as \"exclude\"\n. Try to add 'NoOrdering' in the 'GnsFlags' variable\n",li->INSNAME,li->FIGNAME);
              EXIT(1);
            }

          fcl_clist = fclFindCorrespondance (fig);
          gen_swap_corresp_table (fcl_clist, root_tbl, li, fig);
          fclFreeCorrespList (fcl_clist);
          gen_clean_real_corresp_ptypes (fig);
          completecorresp(li, root_tbl);
        }
      
      gen_update_corresp_table__recursive (lf, root_tbl, NULL);
      return;
    }

  if (root_tbl->FLAGS & LOINS_IS_UPDATED)
    return;

  root_tbl->FLAGS |= LOINS_IS_UPDATED;

  for (ins = lf->LOINS; ins; ins = ins->NEXT) {

    for (ch = root_tbl->SUBINSTS; ch; ch = ch->NEXT)
      if (ch->INSNAME == ins->INSNAME)
        break;
    if (!ch) {
      avt_errmsg(GNS_ERRMSG, "005", AVT_WARNING, 6);
/*      avt_fprintf (stderr, "" AVT_RED "" AVT_BOLD " - warning: can't find table for instance %s in %s\n", ins->INSNAME,
                   root_tbl->FIGNAME);*/
      return;
    }

    if (ch->FLAGS & LOINS_IS_BLACKBOX)
      continue;

    tbl = ch->CRT;

    swapped = 0;
    fig = gen_add_real_corresp_ptypes (ins, root_tbl, tbl, &swapped);

    if (swapped) {

      fcl_clist = fclFindCorrespondance (fig);
      gen_swap_corresp_table (fcl_clist, tbl, ins, fig);
      fclFreeCorrespList (fcl_clist);
      gen_clean_real_corresp_ptypes (fig);
    }

//    gen_check_corresp_table (ins, root_tbl, tbl); // enlever pour l'instant
    // peut etre remis si on untilise pas les "unused connectors"

    gen_update_corresp_table__recursive (fig, tbl, NULL);
  }
}

void dispst0 ()
{
  printf ("\n");
  {
    int j;
    for (j = 0; j < max; j++)
      avt_fprintf (stdout, "[Swaps] " AVT_BLUE "%s" AVT_BLACK " : " AVT_BLACK "%.2f" AVT_RESET "/" AVT_MAGENTA "%d" AVT_RESET "\n", tab[j].name, (float)tab[j].cnt / tab[j].nb,
                   tab[j].tot);
  }
}




loins_list *gen_findinstance(lofig_list *lf, char *name)
{
  return mbk_quickly_getloinsbyname(lf, name);
}


static int nbalim=0, nbins=0;

void gen_update_alims (lofig_list *lf, corresp_t *root_tbl, chain_list *distrib_sigs)
{
  loins_list *ins;
  lofig_list *fig;
  subinst_t *subins;
  chain_list *ch, *distrib;
  locon_list *lc, *lc0;
  losig_list *i_corresp,*f_corresp;


  if (root_tbl->FLAGS & LOINS_ALIM_UPDATED) return;

  root_tbl->FLAGS |= LOINS_ALIM_UPDATED;
  for (ins = lf->LOINS; ins; ins = ins->NEXT) 
    {
      distrib=NULL;
      for (subins = root_tbl->SUBINSTS; subins!=NULL; subins = subins->NEXT)
        {
          // -----------------
          if (ins->FIGNAME!=subins->CRT->GENIUS_FIGNAME)
            ins->FIGNAME=subins->CRT->GENIUS_FIGNAME;
          // on profite pour updater les fignames des instances pour que le drive donne des noms prenant en compte les multiples architectures
          // -----------------

          if (ins->INSNAME == subins->INSNAME)
            break;
        }

      if (subins->FLAGS & LOINS_IS_BLACKBOX) continue;

      for (lc=ins->LOCON; lc!=NULL; lc=lc->NEXT)
        {
          if (mbk_LosigIsVDD(lc->SIG)
              || mbk_LosigIsVSS(lc->SIG))
            {
              ch=(chain_list *)1;              
            }
          else
            for (ch=distrib_sigs; ch!=NULL && lc->NAME!=ch->DATA; ch=ch->NEXT) ;

          if (ch!=NULL)
            {
              for (ch=distrib; ch!=NULL && ch->DATA!=lc->NAME; ch=ch->NEXT) ;
              if (ch==NULL)
                {
                  int vdd, vss;
                  i_corresp = (losig_list *) gethtitem (root_tbl->SIGNALS, gen_losigname (lc->SIG));
                  f_corresp = (losig_list *) gethtitem (subins->CRT->SIGNALS, lc->NAME);
                  // est-ce clean?
                  // quand il y a une alim sur un signal symmetric mais qu'il a tete swape
                  // on le recherche une alim parmis les symmetrics
                  if (!mbk_LosigIsVDD(f_corresp)
                      && !mbk_LosigIsVSS(f_corresp))
                    {
                      vdd=mbk_LosigIsVDD(i_corresp);
                      vss=mbk_LosigIsVSS(i_corresp);
                      
                      for (lc0=ins->LOCON; lc0!=NULL; lc0=lc0->NEXT)
                        {
                          if (lc0==lc || lc->FLAGS!=lc0->FLAGS) continue;
                          f_corresp = (losig_list *) gethtitem (subins->CRT->SIGNALS, lc0->NAME);
                          if ((vdd && mbk_LosigIsVDD(f_corresp)) ||
                              (vss && mbk_LosigIsVSS(f_corresp))) break;
                        }
                      if (lc0==NULL) 
                        {
                          avt_errmsg(GNS_ERRMSG, "010", AVT_FATAL, gen_losigname (i_corresp), gen_losigname ((losig_list *) gethtitem (subins->CRT->SIGNALS, lc->NAME)), ins->INSNAME);
//                          avt_fprintf(stderr,"" AVT_RED "" AVT_BOLD "Found an alim linked to a 'not' alim : %s <=> %s in %s\n", gen_losigname (i_corresp), gen_losigname ((losig_list *) gethtitem (subins->CRT->SIGNALS, lc->NAME)), ins->INSNAME);
                          EXIT(1);
                        }
                    }
                  if (i_corresp!=f_corresp)
                    {
                      addhtitem (subins->CRT->SIGNALS, lc->NAME, (long)i_corresp);
                      distrib=addchain(distrib, lc->NAME);
                      nbalim++;
                    }
                }
            }
        }
      /*
//------------ a supprimer ----------
      for (lc=ins->LOCON; lc!=NULL; lc=lc->NEXT)
        {
          if (!mbk_LosigIsVDD(lc->SIG) && !mbk_LosigIsVSS(lc->SIG)) continue;
          if (gethtitem(subins->CRT->SIGNALS, lc->NAME)!=gethtitem(root_tbl->SIGNALS, gen_losigname (lc->SIG)))
            {
              printf("sig %s of %s locon %s not updated\n",gen_losigname (lc->SIG),ins->INSNAME,lc->NAME);
              EXIT(44);
            }
        }
//-----------------------------------
//
*/
      if (nbalim!=0) nbins++;

      fig=gen_getlofig(subins->CRT->GENIUS_FIGNAME);
      gen_update_alims (fig, subins->CRT, distrib);

      freechain(distrib);
    }
}
/*
void dispst1 ()
{
  avt_fprintf(stdout,"\n[Update Alims] " AVT_YELLOW "%d" AVT_RESET " alims in " AVT_YELLOW "%d" AVT_RESET " instances were updated", nbalim, nbins);
  nbalim=0, nbins=0;
}
*/

#define CONHT_PTYPE 0x11224455

static locon_list *quickfindlocon(lofig_list *lf, char *name)
{
  ptype_list *p;
  ht *h;
  locon_list *lc;
  long l;

  if ((p=getptype(lf->USER, CONHT_PTYPE))!=NULL)
    {
      h=(ht *)p->DATA;
    }
  else
    {
      h=addht(257);
      for (lc=lf->LOCON; lc!=NULL; lc=lc->NEXT)
        {
          addhtitem(h, lc->NAME, (long)lc);
        }
      lf->USER=addptype(lf->USER, CONHT_PTYPE, h);
    }
  if ((l=gethtitem(h, name))!=EMPTYHT) return (locon_list *)l;
  return NULL;
}

chain_list *GrabAllConnectorsThruCorresp(char *con_name, losig_list *realconls, corresp_t *CRT, chain_list *Last)
{
  lofig_list *lf;
  locon_list *lc;
  chain_list *cl, *ch;
  lotrs_list *lt, *lt0;
  subinst_t *sins;
  loins_list *li;
  ptype_list *p;

  if (CRT->FLAGS & LOINS_IS_BLACKBOX) 
    {
      li=gen_findinstance(GENIUS_GLOBAL_LOFIG, CRT->GENIUS_INSNAME);

      if (li==NULL)
        avt_errmsg(GNS_ERRMSG, "011", AVT_WARNING, CRT->GENIUS_INSNAME, CRT->GENIUS_FIGNAME);
      //avt_error ("GNS", 0, AVT_WAR, " - can't retrieve blackbox instance `%s' (`%s')\n", CRT->GENIUS_INSNAME, CRT->GENIUS_FIGNAME);
      else
        {
          for (lc=li->LOCON; lc!=NULL && lc->NAME!=con_name; lc=lc->NEXT) ;
          if (lc==NULL) 
            {
              avt_errmsg(GNS_ERRMSG, "012", AVT_WARNING, CRT->GENIUS_INSNAME, con_name, CRT->GENIUS_FIGNAME);
              //avt_error ("GNS", 0, AVT_WAR, " - can't retrieve blackbox connector `%s.%s' (`%s')\n", CRT->GENIUS_INSNAME, con_name, CRT->GENIUS_FIGNAME);
            }
          else Last=addchain(Last, lc);
        }
      return Last;
    }
  lf=gen_getlofig(CRT->GENIUS_FIGNAME);
  lc=quickfindlocon(lf, con_name);
  if (lc==NULL) 
    {
      avt_errmsg(GNS_ERRMSG, "005", AVT_WARNING, 7);
      //fprintf(stderr,"GrabAllConnectorsThruCorresp: connector '%s' not found\n",con_name);
      return Last;
    }
  cl=getptype(lc->SIG->USER, LOFIGCHAIN)->DATA;
  while (cl!=NULL)
    {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='T')
        {
          lt=gen_GetCorrespondingTransistor(((lotrs_list *)lc->ROOT)->TRNAME, CRT);          
          if (lc->NAME==MBK_GRID_NAME) Last=addchain(Last, lt->GRID);
          else if (lc->NAME==MBK_DRAIN_NAME || lc->NAME==MBK_SOURCE_NAME)
            {
              if (lt->DRAIN->SIG==realconls)
                Last=addchain(Last, lt->DRAIN);
              else
                if (lt->SOURCE->SIG==realconls)
                  Last=addchain(Last, lt->SOURCE);
                else EXIT(44);
            }
          else if (lc->NAME==MBK_BULK_NAME && lt->BULK!=NULL) Last=addchain(Last, lt->BULK);

          // parallel transistors
          if ((p=getptype(lt->USER,MBK_TRANS_PARALLEL))!=NULL)
            {
              lt0=lt;
              for (ch=(chain_list *)p->DATA; ch!=NULL; ch=ch->NEXT)
                {
                  lt=(lotrs_list *)ch->DATA;
                  if (lt!=lt0)
                    {
                      if (lc->NAME==MBK_GRID_NAME) Last=addchain(Last, lt->GRID);
                      else if (lc->NAME==MBK_DRAIN_NAME || lc->NAME==MBK_SOURCE_NAME)
                        {
                          if (lt->DRAIN->SIG==realconls)
                            Last=addchain(Last, lt->DRAIN);
                          else
                            if (lt->SOURCE->SIG==realconls)
                              Last=addchain(Last, lt->SOURCE);
                            else EXIT(44);
                        }
                      else if (lc->NAME==MBK_BULK_NAME && lt->BULK!=NULL) Last=addchain(Last, lt->BULK);
                    }
                }
            }
        }
      else
        if (lc->TYPE=='I')
          {
            sins=gen_GetCorrespondingInstance(((loins_list *)lc->ROOT)->INSNAME, CRT);
            Last=GrabAllConnectorsThruCorresp(lc->NAME, realconls, sins->CRT, Last);
          }
      cl=cl->NEXT;
    }
  return Last;
}

subinst_t *Add_subinst(subinst_t *head, char *name, corresp_t *crt, long flags)
{
  subinst_t *sins;
  sins = (subinst_t *) AddHeapItem (&subinst_heap);
  sins->INSNAME = name;
  sins->FLAGS=flags;
  sins->CRT=crt;
  sins->NEXT=head;
  return sins;
}

void recur_gen_fill_subinsts(ALL_FOR_GNS *all, subinst_t *si)
{
  subinst_t *sins;
  if (si->CRT==NULL) return;
  addhtitem(all->ALL_INSTANCES, si->CRT->GENIUS_INSNAME, (long)si);
  for (sins=si->CRT->SUBINSTS; sins!=NULL; sins=sins->NEXT)
    recur_gen_fill_subinsts(all, sins);
}

void gen_fill_subinsts(ALL_FOR_GNS *all)
{
  subinst_t *sins;
  if (all->ALL_INSTANCES!=NULL) return;  
  all->ALL_INSTANCES=addht(10000);
  for (sins=all->TOP_LEVEL_SUBINST; sins!=NULL; sins=sins->NEXT)
    recur_gen_fill_subinsts(all, sins);
}


static long rgetptype(ptype_list *pt, void *value)
{
  while (pt!=NULL && pt->DATA!=value) pt=pt->NEXT;
  if (pt!=NULL) return pt->TYPE;
  return -1;
}

int gen_getvariable(char *name)
{
  int val;
  val=rgetptype(ALL_ENV, namealloc(name));
  if (val==-1) 
    {
      avt_errmsg(GNS_ERRMSG, "013", AVT_WARNING, name);
      //avt_error("gns", 1, AVT_WAR, "can not find generic variable '%s', assumed value 0\n", name);
      val=0;
    }
  return val;
}

char *gen_getarchi()
{
  return (char *)rgetptype(ALL_ENV, namealloc("archi"));
}

char *gen_getmodel()
{
  return (char *)rgetptype(ALL_ENV, namealloc("model"));
}

char *gen_getinstancename()
{
  return (char *)rgetptype(ALL_ENV, namealloc("instance"));
}

void gen_grab_all_real_corresp(lofig_list *lf, chain_list **lotrs, ptype_list **losig, chain_list **loins)
{
  losig_list *ls;
  loins_list *li;
  lotrs_list *lt, *ttr, *ttr0;
  ht *ALL;
  subinst_t *sins;
  ptype_list *p;
  chain_list *ch;

  ALL=addht(1000);
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    if (gethtitem(ALL, ls)==EMPTYHT)
      {
        *losig=addptype(*losig, (long)ls, gen_corresp_sig (gen_makesignalname(gen_losigname(ls)), CUR_CORRESP_TABLE));
        addhtitem(ALL, ls, 0);
      }

  for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
    {
      if (gethtitem(ALL, lt)==EMPTYHT)
      {
        ttr=_gen_GetCorrespondingTransistor(lt->TRNAME, CUR_CORRESP_TABLE, 1);
        if (ttr!=NULL) 
          {            
            *lotrs=addchain(*lotrs, ttr);
            // parallel transistors
            if ((p=getptype(ttr->USER,MBK_TRANS_PARALLEL))!=NULL)
              {
                for (ch=(chain_list *)p->DATA; ch!=NULL; ch=ch->NEXT)
                  {
                    ttr0=(lotrs_list *)ch->DATA;
                    if (ttr0!=ttr && gethtitem(ALL, ttr0)==EMPTYHT)
                      {
                        *lotrs=addchain(*lotrs, ttr0);
                        addhtitem(ALL, ttr0, 0);
                      }
                  }
              }
          }
        addhtitem(ALL, lt, 0);
      }
    }

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      if (gethtitem(ALL, li)==EMPTYHT)
        {
          sins=gen_GetCorrespondingInstance(li->INSNAME, CUR_CORRESP_TABLE);
          *loins=addchain(*loins, sins->CRT->GENIUS_INSNAME);
          addhtitem(ALL, li, 0);
        }
    }
  delht(ALL);
}



void recur_gen_getlotrs_and_loins (corresp_t *root_tbl, chain_list **lotrs, chain_list **loins, ht* lfins)
{
  subinst_t *subins;
  long l;

  if (root_tbl->TRANSISTORS!=NULL)
    *lotrs=append(GetAllHTElems(root_tbl->TRANSISTORS), *lotrs);
  
  for (subins = root_tbl->SUBINSTS; subins!=NULL; subins = subins->NEXT)
    {
      if (subins->FLAGS & LOINS_IS_BLACKBOX) 
        {
          l=gethtitem(lfins, subins->CRT->GENIUS_INSNAME);
          if (l==EMPTYHT)
            {
              avt_errmsg(GNS_ERRMSG, "014", AVT_FATAL, subins->CRT->GENIUS_INSNAME);
              //printf("could not find black box '%s' in circuit\n",subins->CRT->GENIUS_INSNAME);
              EXIT(7);              
            }
          *loins=addchain(*loins, (void *)l);
        }
      else
        recur_gen_getlotrs_and_loins (subins->CRT, lotrs, loins, lfins);
    }
}

void UpdateTransistorsForYagle(ALL_FOR_GNS *all, int fromcns)
{
  ht *lfht;
  loins_list *li;
  subinst_t *sins;
  long l;
  lofig_list *lf;
  chain_list *lotrs, *loins, *floins=NULL;

  lf=all->GLOBAL_LOFIG;

  lfht=addht(1000);
  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    addhtitem(lfht, li->INSNAME, (long)li);

  for (sins=all->TOP_LEVEL_SUBINST; sins!=NULL; sins=sins->NEXT)
    {
      l=gethtitem(lfht, sins->CRT->GENIUS_INSNAME);
      if (l!=EMPTYHT)
        {
          li=(loins_list *)l;
          lotrs=NULL; loins=NULL;
          recur_gen_getlotrs_and_loins (sins->CRT, &lotrs, &loins, lfht);
          li->USER=addptype(li->USER, FCL_TRANSLIST_PTYPE, lotrs);
          li->USER=addptype(li->USER, FCL_INSLIST_PTYPE, loins);
          floins=addchain(floins, li);
        }
    }

  delht(lfht);

  fclMarkInstances(lf, floins, FALSE, fromcns);
  freechain(floins);
}

//#define NOSINGLE

int check_instances_connections(lofig_list *lf, char *info)
{
  losig_list *ls;
  ptype_list *p;
  chain_list *cl;
  locon_list *lc;
  int mask=0;
  int curindex, idx;
  char lastinfo[1024];

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      p=getptype(ls->USER, LOFIGCHAIN);
      if (p!=NULL)
        {
          for (cl=(chain_list *)p->DATA, curindex=-2; cl!=NULL; cl=cl->NEXT)
            {
              lc=(locon_list *)cl->DATA;
              if (lc->TYPE=='I')
                {
                  idx=vectorindex(lc->NAME);
                  if (curindex!=-2 && idx!=curindex)
                    {
                      if (curindex==-1 || idx==-1) 
                        {
#ifndef NOSINGLE
                          mask|=1; // single signal connected to a vector
                          if (info[0]=='\0')
                            {
                              if (idx==-1)
                                avt_errmsg(GNS_ERRMSG, "015", AVT_WARNING, lc->NAME,((loins_list *)lc->ROOT)->INSNAME,lastinfo);
                                //sprintf(info,"single connector '%s' (instance '%s') is linked to a vector connector %s",lc->NAME,((loins_list *)lc->ROOT)->INSNAME,lastinfo);
                              else
                                avt_errmsg(GNS_ERRMSG, "016", AVT_WARNING, lc->NAME,((loins_list *)lc->ROOT)->INSNAME,lastinfo);
//                                sprintf(info,"vector connector '%s' (instance '%s') is linked to a single connector %s",lc->NAME,((loins_list *)lc->ROOT)->INSNAME,lastinfo);
                            }
#endif
                        }
                      else
                        {
                          mask|=2; // vector index mismatch
                          sprintf(info,"vector connector index '%s' (instance '%s') mismatchs with index of %s",lc->NAME,((loins_list *)lc->ROOT)->INSNAME,lastinfo);
                        }
                    }
                  else 
                    {
                      curindex=idx;
                      sprintf(lastinfo,"'%s' (instance '%s')", lc->NAME, ((loins_list *)lc->ROOT)->INSNAME);
                    }
                }
            }

        }
    }
  return mask;
}
