#include MUT_H
#include MLO_H
#include MLU_H
#include AVT_H
#include RCN_H

int mbk_rc_are_internal(loins_list *li, locon_list *lc)
{
  int dir;
  ptype_list *p;
  if ((p=getptype(lc->USER, MBK_RC_ARE_INTERNAL))!=NULL) return (int)(long)p->DATA;
  if (lc->DIRECTION=='I' || lc->DIRECTION==UNKNOWN) dir=0;
  else dir=1;
  return dir;
}

void mbk_set_rc_internal(locon_list *lc)
{
  ptype_list *p;
  if ((p=getptype(lc->USER, MBK_RC_ARE_INTERNAL))==NULL)
    p=lc->USER=addptype(lc->USER, MBK_RC_ARE_INTERNAL, NULL);

  p->DATA=(void *)1;
}

void mbk_set_rc_external(locon_list *lc)
{
  ptype_list *p;
  if ((p=getptype(lc->USER, MBK_RC_ARE_INTERNAL))==NULL)
    p=lc->USER=addptype(lc->USER, MBK_RC_ARE_INTERNAL, NULL);

  p->DATA=(void *)0;
}

// renvoie la liste de tous les models utilisés dans la hierarchie
// d'une lofig dans l'ordre inverse (bas niveau -> top niveau)
// tabin doit valoir NULL
void mbk_build_hierarchy_list(lofig_list *lf, ht *tabin, chain_list **list)
{
  ht *tab;
  loins_list *li;
  lofig_list *slf;

  if (tabin==NULL) tab=addht(10);
  else tab=tabin;

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      if (gethtitem(tab,li->FIGNAME)==EMPTYHT)
        {
          addhtitem(tab, li->FIGNAME, 1);
          slf=getloadedlofig(li->FIGNAME);
          mbk_build_hierarchy_list(slf, tab, list);
          *list=addchain(*list, slf);
        }
    }
  if (tabin==NULL)
    {
      delht(tab);
      *list=addchain(*list, lf);
      *list=reverse(*list);
    }
}

/*
  fonctions de decoupage de netlist
  point d'entree: mbk_modify_hierarchy
  bbox : netlist a decouper
  recognised : list des instances mappées sur bbox
  paralleltransistorwerehidden : flag indiquant les se sont des instances genius, dans ce cas
  les transistor paralleles seront recherchés de dupliqués
*/

#define CORRESP 0x20050526
#define MARK    0x20050526
#define INTERNAL_TAG 0x1
#define TAGGED_TAG   0x2
#define RCTAKEN_TAG  0x4
#define NORC_TAG     0x8
#define RCINSIDE_TAG 0x10
#define DONECTC_TAG  0x20
#define ININSTANCE_TAG 0x40
#define ONLY_ININSTANCE_TAG 0x80

#define OUT_CORRESP_TAG 0x0400

static char *BBOXPREFIX="bbox";
static long INSPTYPE=0, TRANPTYPE=0;

/*
  pref= prefix a assicier aux nouvelles bbox
  insptype = numero du ptype contenant la liste des instances a l'interieur
  des instances a decouper
  tranptype = numero du ptype contenant la liste de transistor a l'interieur
  des instances a decouper
*/
void mbk_set_cut_info(char *pref, long insptype, long tranptype)
{
  BBOXPREFIX=namealloc(pref);
  INSPTYPE=insptype;
  TRANPTYPE=tranptype;
}

static char *namebbox(char *main, char *ins)
{
  static char temp[200];
  sprintf(temp,"%s_%s_%s",BBOXPREFIX,main,ins);
  return temp;
}
static char *nametop(char *main)
{
  static char temp[200];
  sprintf(temp,"%s_%s",main,BBOXPREFIX);
  return temp;
}
static void *getcorresp(ptype_list **pt)
{
  ptype_list *p;
  if ((p=getptype(*pt, CORRESP))==NULL) return NULL;
  return p->DATA;
}

static void setcorresp(ptype_list **pt, void *item)
{
  ptype_list *p;
  if ((p=getptype(*pt, CORRESP))==NULL)
    *pt=addptype(*pt, CORRESP, item);
  else
    p->DATA=item;
}
static void delcorresp(ptype_list **pt)
{
  if (getptype(*pt, CORRESP)!=NULL)
    *pt=delptype(*pt, CORRESP);
}

static int isinternal(losig_list *ls)
{
  ptype_list *pt;
  chain_list *cl;
  if ((pt=getptype(ls->USER, LOFIGCHAIN))==NULL) return 0;
  for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
    if (getptype(((locon_list *)cl->DATA)->USER, MARK)==NULL) return 0;
  return 1;
}

static void updatelocons(chain_list *loconlst, void *root)
{
  locon_list **lcc, *orig;
  chain_list *cl;
  for (cl=loconlst; cl!=NULL; cl=cl->NEXT)
    {
      lcc=(locon_list **)cl->DATA;
      orig=*lcc;
      *lcc=duplocon (*lcc);
      (*lcc)->ROOT=root;
      (*lcc)->PNODE=NULL;
      setcorresp(&(*lcc)->USER, orig);
      setcorresp(&orig->USER, *lcc);
      if ((*lcc)->SIG!=NULL) (*lcc)->SIG=(losig_list *)getcorresp(&((*lcc)->SIG->USER));
    }
}

static lotrs_list *DUPTRS(lotrs_list *tr)
{
  lotrs_list *newtr;
  chain_list *cl=NULL;

  newtr=duplotrs(tr);
  if (newtr->BULK!=NULL) cl=addchain(cl, &newtr->BULK);
  cl=addchain(cl, &newtr->DRAIN);
  cl=addchain(cl, &newtr->SOURCE);
  cl=addchain(cl, &newtr->GRID);
  updatelocons(cl, newtr);
  freechain(cl);
  return newtr;
}

static loins_list *DUPLOINS(loins_list *li)
{
  loins_list *newli;
  chain_list *cl=NULL;
  locon_list *lc, **lcc;

  newli=duploins(li);
  for (lc=newli->LOCON, lcc=&newli->LOCON; lc!=NULL; lcc=&lc->NEXT, lc=lc->NEXT) cl=addchain(cl, lcc);
  updatelocons(cl, newli);
  freechain(cl);
  return newli;
}
static lofig_list *duplicategnsinstance(lofig_list *mainlf, loins_list *gnsli, int paralleltansistorhidden)
{
  ptype_list *pt;
  chain_list *transcl=NULL, *inscl=NULL, *cl, *paral=NULL, *ch;
  lotrs_list *tr, *newtr;
  loins_list *li, *newli;
  locon_list *lc, *lc0;
  losig_list *ls, *sig;
  lofig_list *reslf;
  int sigindex=0, cnt;
  char *signame;

  if ((pt=getptype(gnsli->USER, TRANPTYPE))!=NULL) transcl=(chain_list *)pt->DATA;
  if ((pt=getptype(gnsli->USER, INSPTYPE))!=NULL) inscl=(chain_list *)pt->DATA;

  if (paralleltansistorhidden)
    {
      for (cl=transcl; cl!=NULL; cl=cl->NEXT)
        {
          tr=(lotrs_list *)cl->DATA;
          if ((pt=getptype(tr->USER,MBK_TRANS_PARALLEL))!=NULL)
            for (ch=(chain_list *)pt->DATA; ch!=NULL; ch=ch->NEXT)
              if (ch->DATA!=tr) paral=addchain(paral, ch->DATA);
        }
    }
  
  transcl=append(dupchainlst(transcl), paral);

  // marquage des connecteurs de transistors
  for (cl=transcl; cl!=NULL; cl=cl->NEXT)
    {
      tr=(lotrs_list *)cl->DATA;
      tr->GRID->USER=addptype(tr->GRID->USER, MARK, tr->GRID);
      tr->GRID->SIG->FLAGS=INTERNAL_TAG;
      tr->DRAIN->USER=addptype(tr->DRAIN->USER, MARK, tr->DRAIN);
      tr->DRAIN->SIG->FLAGS=INTERNAL_TAG;
      tr->SOURCE->USER=addptype(tr->SOURCE->USER, MARK, tr->SOURCE);
      tr->SOURCE->SIG->FLAGS=INTERNAL_TAG;
      if (tr->BULK!=NULL) 
        {
          tr->BULK->USER=addptype(tr->BULK->USER, MARK, tr->BULK);
          tr->BULK->SIG->FLAGS=INTERNAL_TAG;
        }
      tr->USER=addptype(tr->USER, CORRESP, 0);
    }
  for (cl=inscl; cl!=NULL; cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
        {
          lc->USER=addptype(lc->USER, MARK, lc);
          lc->SIG->FLAGS=INTERNAL_TAG;
        }
      li->USER=addptype(li->USER, CORRESP, 0);
    }

  reslf = addlofig(namebbox(mainlf->NAME, gnsli->INSNAME));

  for (lc=gnsli->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      if (lc->SIG!=NULL && (lc->SIG->FLAGS & TAGGED_TAG)==0)
        {
          ls = addlosig(reslf, sigindex++, addchain(NULL, lc->NAME), 'E');
          ls->FLAGS=0;
          lc0=addlocon(reslf, lc->NAME, ls, lc->DIRECTION);
          lc->SIG->FLAGS=2;
          lc->SIG->FLAGS|=ININSTANCE_TAG|TAGGED_TAG;
          setcorresp(&lc->SIG->USER, ls);
          setcorresp(&ls->USER, lc->SIG);
          if (mbk_rc_are_internal(gnsli, lc)!=0) ls->FLAGS|=RCINSIDE_TAG;
          lc0->USER=addptype(lc0->USER, OUT_CORRESP_TAG, lc->SIG);
        }
    }

  for (sig=mainlf->LOSIG; sig!=NULL; sig=sig->NEXT)
    {
      if ((sig->FLAGS & INTERNAL_TAG)!=0)
        {
          signame=(char *)sig->NAMECHAIN->DATA;
          if (isinternal(sig))
            {
              ls = addlosig(reslf, sigindex++, addchain(NULL, signame), 'I');
              sig->FLAGS=ONLY_ININSTANCE_TAG;
            }
          else
            {
              ls = addlosig(reslf, sigindex++, addchain(NULL, signame), 'E');
              lc0=addlocon(reslf, signame, ls, UNKNOWN);
              lc0->USER=addptype(lc0->USER, OUT_CORRESP_TAG, sig);
            }
          ls->FLAGS=0;
          sig->FLAGS|=ININSTANCE_TAG|TAGGED_TAG;
          setcorresp(&sig->USER, ls);
          setcorresp(&ls->USER, sig);
        }
    }

  for (cl=transcl; cl!=NULL; cl=cl->NEXT)
    {
      tr=(lotrs_list *)cl->DATA;
      tr->GRID->USER=delptype(tr->GRID->USER, MARK);
      tr->DRAIN->USER=delptype(tr->DRAIN->USER, MARK);
      tr->SOURCE->USER=delptype(tr->SOURCE->USER, MARK);
      if (tr->BULK!=NULL) 
        {
          tr->BULK->USER=delptype(tr->BULK->USER, MARK);
        }
    }
  for (cl=inscl; cl!=NULL; cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
        {
          lc->USER=delptype(lc->USER, MARK);
        }
    }


  reslf->LOCON=(locon_list *)reverse((chain_list *)reslf->LOCON);

  // duplication des transistors
  cnt=0;
  for (cl=transcl; cl!=NULL; cl=cl->NEXT)
    {
      tr=(lotrs_list *)cl->DATA;
      newtr=DUPTRS(tr);
      newtr->NEXT=reslf->LOTRS;
      reslf->LOTRS=newtr;
    }

  // duplication des instances
  for (cl=inscl; cl!=NULL; cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      newli=DUPLOINS(li);
      newli->NEXT=reslf->LOINS;
      reslf->LOINS=newli;
    }

  //
  lofigchain(reslf);

  freechain(transcl);
  return reslf;
}

static losig_list *gen_get_ground (lofig_list *fig)
{
  losig_list *sig;
  
  for (sig = fig->LOSIG; sig; sig = sig->NEXT) 
    if (mbk_LosigIsVSS (sig))
      break;

  return sig;
}


static void addrcininstance(lofig_list *mainlf, lofig_list *inslf)
{
  int dir;
  locon_list *lc, *lc0, *inslc;
  chain_list *cl;
  ptype_list *pt;
  losig_list *ls, *ground, *sig, *ctcsig1, *ctcsig2;
  long node1, node2;
  num_list *nm;
  lowire_list *wire;
  loctc_list *ctc;
  int capaignored=0, capatoground=0, capatotal=0;
  int resitotal=0;
  int togroundflag=0;
  if (!(ground = gen_get_ground (inslf))) 
    {
      avt_error("gnshr", 3, AVT_ERR, "could not find an ground power in circuit '%s'\n", inslf->NAME);
      EXIT(1);
    }

  for (lc=inslf->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      if ((lc->SIG->FLAGS & RCINSIDE_TAG)!=0) dir=1;
      else dir=0;

      lc->SIG->FLAGS|=TAGGED_TAG;

      ls=(losig_list *)getcorresp(&lc->SIG->USER);
      if (!ls->PRCN || (ls->PRCN->PCTC==NULL && ls->PRCN->PWIRE==NULL)) continue;

      if ((ls->FLAGS & RCTAKEN_TAG)!=0) dir=0;

      if (lc->SIG->PRCN==NULL) addlorcnet (lc->SIG);
 
      if (dir==0) 
        {
          // RC are external
          pt=getptype(ls->USER, LOFIGCHAIN);
          for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
            {
              lc0=(locon_list *)cl->DATA;
              if ((inslc=(locon_list *)getcorresp(&lc0->USER))!=NULL)
                {
                  for (nm=lc0->PNODE; nm!=NULL; nm=nm->NEXT)
                    {
                      setloconnode (inslc, nm->DATA);
                      if (ls->PRCN->NBNODE>2) setloconnode (lc, nm->DATA);
                    }
                  inslc->PNODE=(num_list *)reverse((chain_list *)inslc->PNODE);
                }
            }
          if (ls->PRCN->NBNODE==2) setloconnode (lc, 1);
          lc->PNODE=(num_list *)reverse((chain_list *)lc->PNODE);
          lc->SIG->FLAGS|=NORC_TAG;
        }
      else
        {
          // RC are internal
          pt=getptype(ls->USER, LOFIGCHAIN);
          for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
            {
              lc0=(locon_list *)cl->DATA;
              if ((inslc=(locon_list *)getcorresp(&lc0->USER))!=NULL)
                {
                  for (nm=lc0->PNODE; nm!=NULL; nm=nm->NEXT)
                    {
                      
                      setloconnode (inslc, nm->DATA);
                    }
                  inslc->PNODE=(num_list *)reverse((chain_list *)inslc->PNODE);
                }
              else
                {
                  for (nm=lc0->PNODE; nm!=NULL; nm=nm->NEXT)
                    {
                      if (ls->PRCN->NBNODE>2) setloconnode (lc, nm->DATA);
                    }
                }
            }
          if (ls->PRCN->NBNODE==2) setloconnode (lc, 1);
          lc->PNODE=(num_list *)reverse((chain_list *)lc->PNODE);
        }
    }

  for (sig=inslf->LOSIG; sig!=NULL; sig=sig->NEXT)
    {
      if ((sig->FLAGS & NORC_TAG)!=0) continue;

      ls=(losig_list *)getcorresp(&sig->USER);
      if (!ls->PRCN || (ls->PRCN->PWIRE==NULL && ls->PRCN->PCTC==NULL)) continue;

      if (sig->PRCN==NULL) addlorcnet (sig);

      if ((sig->FLAGS & TAGGED_TAG)==0)
        {
          // PNODE pour les signaux internes
          pt=getptype(ls->USER, LOFIGCHAIN);
          for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
            {
              lc0=(locon_list *)cl->DATA;
              if ((inslc=(locon_list *)getcorresp(&lc0->USER))!=NULL)
                {
                  for (nm=lc0->PNODE; nm!=NULL; nm=nm->NEXT)
                    {
                      setloconnode (inslc, nm->DATA);
                    }
                  inslc->PNODE=(num_list *)reverse((chain_list *)inslc->PNODE);
                }
            }
        }

      if ((ls->FLAGS & RCTAKEN_TAG)!=0) continue;

      rcn_refresh_signal(mainlf, ls);

      // WIRE
      for (wire = ls->PRCN->PWIRE; wire!=NULL; wire = wire->NEXT) 
        {
          addlowire (sig, 0, wire->RESI, wire->CAPA, wire->NODE1, wire->NODE2);
          resitotal++;
        }

      // CTC
      for (cl = ls->PRCN->PCTC; cl!=NULL; cl = cl->NEXT)
        {
          ctc = (loctc_list*)cl->DATA;
          togroundflag=0;

          if ( (ctcsig1=(losig_list *)getcorresp(&ctc->SIG1->USER))==NULL
               || (ctcsig1->FLAGS & NORC_TAG)!=0 ) {ctcsig1=ground;node1=1;capatoground++;togroundflag++;}
          else if ((ctc->SIG1->FLAGS & RCTAKEN_TAG)!=0) continue;
          else node1=rcn_ctcnode (ctc, ctc->SIG1);
          if ( (ctcsig2=(losig_list *)getcorresp(&ctc->SIG2->USER))==NULL
               || (ctcsig2->FLAGS & NORC_TAG)!=0 ) {ctcsig2=ground;node2=1;capatoground++;togroundflag++;}
          else if ((ctc->SIG2->FLAGS & RCTAKEN_TAG)!=0) continue;
          else node2=rcn_ctcnode (ctc, ctc->SIG2);

          capatotal++;
          if (ctcsig1==ctcsig2) 
            { 
              capaignored++;
              if (ctcsig1==ground) capatoground-=2;
              continue;
            }
          if (ctcsig1->PRCN==NULL) addlorcnet (ctcsig1);
          if (ctcsig2->PRCN==NULL) addlorcnet (ctcsig2);
          addloctc (ctcsig1, node1, ctcsig2, node2, ctc->CAPA);
        }
      ls->FLAGS|=RCTAKEN_TAG|DONECTC_TAG;
    }

  // some cleanup and update
  for (sig=inslf->LOSIG; sig!=NULL; sig=sig->NEXT)
    {
      ls=(losig_list *)getcorresp(&sig->USER);
      delcorresp(&ls->USER);
      delcorresp(&sig->USER);
      pt=getptype(ls->USER, LOFIGCHAIN);
      for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
        {
          lc=(locon_list *)cl->DATA;
          
          lc0=(locon_list *)getcorresp(&lc->USER);
          if (lc0!=NULL)
            delcorresp(&lc0->USER);
          delcorresp(&lc->USER);
        }
    }

  rcn_mergectclofig(inslf);
  avt_fprintf(stdout,"GNS Instance '¤3%s¤.'\n", inslf->NAME);
  avt_fprintf(stdout,"¤+Capas:  Total   Ignored   Toground  Resistances Signals Transistors Instances\n");
  avt_fprintf(stdout,"¤1       %6d    %6d     %6d   %6d      %6d    %6d    %6d\n", capatotal, capaignored, capatoground, resitotal, countchain((chain_list *)inslf->LOSIG), countchain((chain_list *)inslf->LOTRS),countchain((chain_list *)inslf->LOINS) );


}

loins_list *ADDGNSLOINS(lofig_list *lf, lofig_list *insfig)
{
  chain_list *siglist=NULL;
  locon_list *lc;
  losig_list *ls;

  for (lc=insfig->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      ls=(losig_list *)getptype(lc->USER, OUT_CORRESP_TAG)->DATA;
      siglist=addchain(siglist, getcorresp(&ls->USER));
    }

  siglist=reverse(siglist);
  return addloins (lf, insfig->NAME, insfig, siglist);
}

static lofig_list *duplicatemainfigure(lofig_list *mainlf, chain_list *gnsinscl, chain_list **dupgnsinscl)
{
  chain_list *transcl=NULL, *inscl=NULL, *cl, *paral=NULL;
  lotrs_list *tr, *newtr;
  loins_list *li, *newli;
  locon_list *lc, *lc0;
  losig_list *ls, *sig;
  lofig_list *reslf;
  int sigindex=0, cnt;
  char *signame, *insfigname;


  for (tr=mainlf->LOTRS; tr!=NULL; tr=tr->NEXT)
    {
      if (getptype(tr->USER, CORRESP)!=NULL) continue;
      transcl=addchain(transcl, tr);
    }
  
  transcl=append(dupchainlst(transcl), paral);

  for (li=mainlf->LOINS; li!=NULL; li=li->NEXT)
    {
      if (getptype(li->USER, CORRESP)!=NULL) continue;
      inscl=addchain(inscl, li);
    }

  reslf = addlofig(nametop(mainlf->NAME));

  for (lc=mainlf->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      ls = addlosig(reslf, sigindex++, addchain(NULL, lc->NAME), 'E');
      ls->FLAGS=0;
      lc0=addlocon(reslf, lc->NAME, ls, lc->DIRECTION);
      setcorresp(&lc->SIG->USER, ls);
      setcorresp(&ls->USER, lc->SIG);
      setcorresp(&lc->USER, lc0);

      lc->SIG->FLAGS|=INTERNAL_TAG;
    }
  cnt=0;
  for (sig=mainlf->LOSIG; sig!=NULL; sig=sig->NEXT)
    {
      if ((sig->FLAGS & (ONLY_ININSTANCE_TAG|INTERNAL_TAG))==0)
        {
          signame=(char *)sig->NAMECHAIN->DATA;
          ls = addlosig(reslf, sigindex++, addchain(NULL, signame), 'I');
          ls->FLAGS=0;
          setcorresp(&sig->USER, ls);
          setcorresp(&ls->USER, sig);
        }
    }

  reslf->LOCON=(locon_list *)reverse((chain_list *)reslf->LOCON);

  // duplication des transistors
  for (cl=transcl; cl!=NULL; cl=cl->NEXT)
    {
      tr=(lotrs_list *)cl->DATA;
      newtr=DUPTRS(tr);
      newtr->NEXT=reslf->LOTRS;
      reslf->LOTRS=newtr;
    }
  // duplication des instances
  for (cl=inscl; cl!=NULL; cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      newli=DUPLOINS(li);
      newli->NEXT=reslf->LOINS;
      reslf->LOINS=newli;
    }
  *dupgnsinscl=NULL;
  for (cl=gnsinscl; cl!=NULL; cl=cl->NEXT)
    {
      insfigname=namebbox(mainlf->NAME, ((loins_list *)cl->DATA)->INSNAME);
      *dupgnsinscl=addchain(*dupgnsinscl, ADDGNSLOINS(reslf, getloadedlofig(insfigname)));
    }

  //
  lofigchain(reslf);

  freechain(transcl);
  return reslf;
}


static void addmainfigurerc(lofig_list *mainlf, lofig_list *maincutlf, chain_list *newgnsinscl)
{
  locon_list *lc, *lc0, *inslc;
  chain_list *cl;
  ptype_list *pt;
  losig_list *ls, *ground, *sig, *ctcsig1, *ctcsig2;
  long node1, node2;
  num_list *nm;
  lowire_list *wire;
  loctc_list *ctc;
  lofig_list *inslf;
  loins_list *li;
  int capaignored=0, capatoground=0, capatotal=0;
  int resitotal=0;
  int togroundflag=0;

  if (!(ground = gen_get_ground (maincutlf))) 
    {
      avt_error("gnshr", 3, AVT_ERR, "could not find an ground power in circuit '%s'\n", maincutlf->NAME);
      EXIT(1);
    }

  // set gns instance pnode in the right order
  for (cl=newgnsinscl; cl!=NULL; cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      inslf=getloadedlofig(li->FIGNAME);

      for (lc=li->LOCON, lc0=inslf->LOCON; lc!=NULL && lc0!=NULL; lc=lc->NEXT, lc0=lc0->NEXT)
        {
          lc->SIG->FLAGS|=TAGGED_TAG;
          ls=(losig_list *)getcorresp(&lc->SIG->USER);
          //          if ((ls->FLAGS & RCTAKEN_TAG)!=0) continue;
          if (!ls->PRCN || (ls->PRCN->PWIRE==NULL && ls->PRCN->PCTC==NULL)) continue;
          if (lc->SIG->PRCN==NULL) addlorcnet (lc->SIG);
          for (nm=lc0->PNODE; nm!=NULL; nm=nm->NEXT)
            {
              setloconnode (lc, nm->DATA);
            }
          lc->PNODE=(num_list *)reverse((chain_list *)lc->PNODE);
        }
      if (lc!=lc0) EXIT(5);
    }

  for (sig=maincutlf->LOSIG; sig!=NULL; sig=sig->NEXT)
    {

      ls=(losig_list *)getcorresp(&sig->USER);
      //      if ((ls->FLAGS & RCTAKEN_TAG)!=0) continue;

      if (!ls->PRCN || (ls->PRCN->PWIRE==NULL && ls->PRCN->PCTC==NULL)) continue;

      if (sig->PRCN==NULL) addlorcnet (sig);

      // PNODE pour les autres internes
      pt=getptype(ls->USER, LOFIGCHAIN);
      for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
        {
          lc0=(locon_list *)cl->DATA;
          if ((inslc=(locon_list *)getcorresp(&lc0->USER))!=NULL)
            {
              for (nm=lc0->PNODE; nm!=NULL; nm=nm->NEXT)
                {
                  setloconnode (inslc, nm->DATA);
                }
              inslc->PNODE=(num_list *)reverse((chain_list *)inslc->PNODE);
            }
        }

      if ((ls->FLAGS & RCTAKEN_TAG)!=0) continue;

      rcn_refresh_signal(mainlf, ls);

      //      printf(" - ---------- traite %s\n",gen_losigname(ls));
      // WIRE
      for (wire = ls->PRCN->PWIRE; wire!=NULL; wire = wire->NEXT) 
        {
          addlowire (sig, 0, wire->RESI, wire->CAPA, wire->NODE1, wire->NODE2);
          resitotal++;
        }

      // CTC
      for (cl = ls->PRCN->PCTC; cl!=NULL; cl = cl->NEXT)
        {
          ctc = (loctc_list*)cl->DATA;
          togroundflag=0;

          if ( (ctcsig1=(losig_list *)getcorresp(&ctc->SIG1->USER))==NULL
               || (ctc->SIG1->FLAGS & DONECTC_TAG)!=0 ) {ctcsig1=ground;node1=1;capatoground++;togroundflag++;}
          else if ((ctc->SIG1->FLAGS & RCTAKEN_TAG)!=0) continue;
          else node1=rcn_ctcnode (ctc, ctc->SIG1);
          if ( (ctcsig2=(losig_list *)getcorresp(&ctc->SIG2->USER))==NULL
               || (ctc->SIG2->FLAGS & DONECTC_TAG)!=0 ) {ctcsig2=ground;node2=1;capatoground++;togroundflag++;}
          else if ((ctc->SIG2->FLAGS & RCTAKEN_TAG)!=0) continue;
          else node2=rcn_ctcnode (ctc, ctc->SIG2);

          capatotal++;
          if (ctcsig1==ctcsig2) 
            { 
              capaignored++;
              if (togroundflag==2) capatoground-=2;
              continue;
            }
          if (ctcsig1->PRCN==NULL) addlorcnet (ctcsig1);
          if (ctcsig2->PRCN==NULL) addlorcnet (ctcsig2);

          addloctc (ctcsig1, node1, ctcsig2, node2, ctc->CAPA);
        }
      ls->FLAGS|=RCTAKEN_TAG;
    }

  // some cleanup and update
  for (sig=maincutlf->LOSIG; sig!=NULL; sig=sig->NEXT)
    {
      ls=(losig_list *)getcorresp(&sig->USER);
      delcorresp(&ls->USER);
      delcorresp(&sig->USER);
      pt=getptype(ls->USER, LOFIGCHAIN);
      for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
        {
          lc=(locon_list *)cl->DATA;
          
          lc0=(locon_list *)getcorresp(&lc->USER);
          if (lc0!=NULL)
            delcorresp(&lc0->USER);
          delcorresp(&lc->USER);
        }
      //      if ((sig->FLAGS & NORC_TAG)!=0) continue;
    }

  rcn_mergectclofig(maincutlf);

  avt_fprintf(stdout,"Blackbox TOP '¤3%s¤.'\n",maincutlf->NAME);
  avt_fprintf(stdout,"¤+Capas:  Total   Ignored   Toground  Resistances Signals Transistors Instances\n");
  avt_fprintf(stdout,"¤1       %6d    %6d     %6d   %6d      %6d    %6d    %6d\n", capatotal, capaignored, capatoground, resitotal, countchain((chain_list *)maincutlf->LOSIG), countchain((chain_list *)maincutlf->LOTRS),countchain((chain_list *)maincutlf->LOINS) );
}


lofig_list *mbk_modify_hierarchy(lofig_list *bbox, chain_list *recognised, int paralleltransistorwerehidden)
{
  losig_list *ls;
  chain_list *cl;
  lofig_list *insfig;
  lofig_list *rootfig;

  lofigchain(bbox);
  for (ls=bbox->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      ls->FLAGS=0;
    }

  for (cl=recognised; cl!=NULL; cl=cl->NEXT)
    {
      insfig=duplicategnsinstance(bbox, (loins_list *)cl->DATA, paralleltransistorwerehidden);
      addrcininstance(bbox, insfig);

      //      savelofig(insfig);

      for (ls=bbox->LOSIG; ls!=NULL; ls=ls->NEXT)
        {
          ls->FLAGS&=~(TAGGED_TAG|INTERNAL_TAG);          
        }
    }
  rootfig=duplicatemainfigure(bbox, recognised, &cl);
  addmainfigurerc(bbox, rootfig, cl);
  //  savelofig(rootfig);

  avt_fprintf(stdout,"\n");

  return rootfig;
}

unsigned int mbk_signlofig( lofig_list *ptfig )
{
  unsigned int n, s ;

  s = 0 ;

  n = countchain( (chain_list*)ptfig->LOCON );
  s = n ;

  n = countchain( (chain_list*)ptfig->LOSIG );
  s = ROT(s,8) ^ n ;

  n = countchain( (chain_list*)ptfig->LOINS );
  s = ROT(s,8) ^ n ;

  n = countchain( (chain_list*)ptfig->LOTRS );
  s = ROT(s,8) ^ n ;

  return s ;
}
