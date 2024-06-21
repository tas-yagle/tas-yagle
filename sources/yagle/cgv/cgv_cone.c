#include MUT_H
#include MLO_H
#include CNS_H
#include CGV_H

extern cgvfig_list *loc_buildbox(cgvfig_list *Figure);
extern cgvfig_list *loc_route(cgvfig_list *Figure);
extern void loc_clean_all(cgvfig_list *Figure);
extern cgvbox_list *loc_add_transparence(cgvfig_list *Figure, cgvnet_list *Net);
extern void default_box(cgvbox_list *Box,  int H_ref);
int loc_test_trans(cgvnet_list *Net, cgvcol_list *Column);
extern void organiser_cellules(cgvcol_list *c, int centre);
extern int cgv_has_input(chain_list *cl);

static losig_list *getnext(lotrs_list *tr, losig_list *from)
{
  if (tr->SOURCE->SIG==from) return tr->DRAIN->SIG;
  if (tr->DRAIN->SIG==from) return tr->SOURCE->SIG;
  EXIT(7);
  return NULL;
}

static ht *temp_losig_ht=NULL;

static cgvnet_list *getnet(cgvfig_list *CgvFigure, losig_list *ls, char *name)
{
  cgvnet_list *CgvNet;
  ptype_list *p;
  long l;

  if (temp_losig_ht!=NULL && (l=gethtitem(temp_losig_ht, name))!=EMPTYHT)
    return (cgvnet_list *)l;

  if (ls!=NULL && (p=getptype(ls->USER, CGV_NET_TYPE))!=NULL) return (cgvnet_list *)p->DATA;
  CgvNet = addcgvnet(CgvFigure);
  CgvNet->SOURCE = ls;
  CgvNet->SOURCE_TYPE = CGV_SOURCE_LOSIG;
  if (ls!=NULL && ls->NAMECHAIN!=NULL)
    CgvNet->NAME=ls->NAMECHAIN->DATA;
  else if (name!=NULL)
    CgvNet->NAME=name;
  else
    CgvNet->NAME=namealloc("?");
  if (ls!=NULL) ls->USER = addptype(ls->USER, CGV_NET_TYPE, CgvNet);

  if (name!=NULL)
    {
      if (temp_losig_ht==NULL) temp_losig_ht=addht(10);
      addhtitem(temp_losig_ht, name, (long)CgvNet);
    }
  return CgvNet;
}

typedef struct noeud
{
  struct noeud *PARAL, *NEXT;
  link_list *LINK;
  int flag;
  cgvbox_list *box;
  int pos;
  cgvnet_list *net; // net le + proche de la sortie
} noeud;


void distroy_arbre(noeud *no)
{
  noeud *tmp;
  lotrs_list *tr;
  if (no==NULL) return;
  distroy_arbre(no->PARAL);
  while (no!=NULL)
    {
      tmp=no->NEXT;
      tr=no->LINK->ULINK.LOTRS;
      if (getptype(tr->USER, CGV_BOX_TYPE)!=NULL)
        tr->USER=delptype(tr->USER, CGV_BOX_TYPE);
      if (getptype(tr->USER, CGV_MARK_TYPE)!=NULL)
        tr->USER=delptype(tr->USER, CGV_MARK_TYPE);
/*      if (getptype(no->LINK->ULINK.LOTRS->USER, CGV_BOX_TYPE)!=NULL)
        no->LINK->ULINK.LOTRS->USER=delptype(no->LINK->ULINK.LOTRS->USER, CGV_BOX_TYPE);*/
      mbkfree(no);
      no=tmp;
    }
}

static void run_branch(cgvfig_list *CgvFigure, link_list *lk, losig_list *src, char dir, noeud **arbre)
{
  losig_list *next;
  cgvcon_list *CgvCon;
  cgvbox_list *CgvBox;
  cgvnet_list *CgvNet;
  locon_list *s, *d, *s0, *s1;
  lotrs_list *tr;
  char temp[100];
  struct noeud *no;

  if (lk==NULL || (lk->TYPE & CNS_EXT)!=0) return;
//  printf("+> %s\n",lk->ULINK.LOTRS->TRNAME);

  if (getptype(lk->ULINK.LOTRS->USER, CGV_BOX_TYPE)==NULL)
    {
      for (no=*arbre; no!=NULL && no->LINK->ULINK.LOTRS!=lk->ULINK.LOTRS; no=no->PARAL) ; 
//      printf("(t0)");
    }
  else
    {
      no=(struct noeud *)getptype(lk->ULINK.LOTRS->USER, CGV_MARK_TYPE)->DATA;
//      printf("(t1)");
    }

  /*
  if (no!=NULL) {printf("multi %s\n",lk->ULINK.LOTRS->TRNAME);}
  else printf("add %s (%p)\n",lk->ULINK.LOTRS->TRNAME,lk->ULINK.LOTRS);
*/
  next=getnext(tr=lk->ULINK.LOTRS, src);
//  printf("-> %s\n",tr->TRNAME);
  if (no==NULL)
    {
      if (tr->TRNAME!=NULL)
        CgvBox = addcgvbox(CgvFigure, tr->TRNAME);
      else
        CgvBox = addcgvbox(CgvFigure, namealloc("?"));

      lk->USER=addptype(lk->USER, CGV_BOX_TYPE, CgvBox);
      tr->USER=addptype(tr->USER, CGV_BOX_TYPE, CgvBox);

      CgvBox->SOURCE = (void *) tr;
      CgvBox->SOURCE_TYPE = CGV_SOURCE_LOTRS;
      // grille
      CgvCon = addcgvboxcon(CgvFigure, CgvBox, tr->GRID->NAME, CGV_CON_IN);
      CgvCon->SOURCE = tr->GRID;
      CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
      CgvNet=getnet(CgvFigure, tr->GRID->SIG, (char *)tr->GRID->SIG->NAMECHAIN->DATA);
      addcgvnetcon(CgvNet, CgvCon);

      // les autres
      if (tr->DRAIN->SIG==src) { s0=tr->DRAIN; s1=tr->SOURCE; }
      else { s1=tr->DRAIN; s0=tr->SOURCE; }
      d=tr->DRAIN; s=tr->SOURCE;
      if (dir=='s') s=tr->DRAIN, d=tr->SOURCE;
      if (lk->NEXT!=NULL || dir=='e')
        {
          if (MLO_IS_TRANSN(tr->TYPE)) strcpy(temp,"tn_n_n");
          else strcpy(temp,"tp_n_n");
          if (dir=='d' || dir=='e')
            {
              CgvCon = addcgvboxcon(CgvFigure, CgvBox, tr->SOURCE->NAME, CGV_CON_OUT);
              CgvCon->SOURCE = s;
              CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
              CgvNet=getnet(CgvFigure, src, (char *)src->NAMECHAIN->DATA);
              addcgvnetcon(CgvNet, CgvCon);
              // ---
              CgvCon = addcgvboxcon(CgvFigure, CgvBox, tr->DRAIN->NAME, CGV_CON_OUT);
              CgvCon->SOURCE = d;
              CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
              CgvNet=getnet(CgvFigure, next, (char *)next->NAMECHAIN->DATA);
              addcgvnetcon(CgvNet, CgvCon);
            }
          else
            {
              CgvCon = addcgvboxcon(CgvFigure, CgvBox, tr->SOURCE->NAME, CGV_CON_OUT);
              CgvCon->SOURCE = s;
              CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
              CgvNet=getnet(CgvFigure, next, (char *)next->NAMECHAIN->DATA);
              addcgvnetcon(CgvNet, CgvCon);
              // ---
              CgvCon = addcgvboxcon(CgvFigure, CgvBox, tr->DRAIN->NAME, CGV_CON_OUT);
              CgvCon->SOURCE = d;
              CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
              CgvNet=getnet(CgvFigure, src, (char *)src->NAMECHAIN->DATA);
              addcgvnetcon(CgvNet, CgvCon);

            }
        }
      else
        {
          if (dir=='d')
            {
              if (MLO_IS_TRANSN(tr->TYPE)) strcpy(temp,"tn_vdd_n");
              else strcpy(temp,"tp_vdd_n");
              CgvCon = addcgvboxcon(CgvFigure, CgvBox, s->NAME, CGV_CON_OUT);
              CgvCon->SOURCE = s;
              CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
              CgvNet=getnet(CgvFigure, src, (char *)src->NAMECHAIN->DATA);
              addcgvnetcon(CgvNet, CgvCon);
            }
          else
            {
              if (MLO_IS_TRANSN(tr->TYPE)) strcpy(temp,"tn_n_vss");
              else strcpy(temp,"tp_n_vss");
              CgvCon = addcgvboxcon(CgvFigure, CgvBox, s->NAME, CGV_CON_OUT);
              CgvCon->SOURCE = s;
              CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
              CgvNet=getnet(CgvFigure, src, (char *)src->NAMECHAIN->DATA);
              addcgvnetcon(CgvNet, CgvCon);
            }
        }
      CgvBox->SYMBOL = cgv_getselfcellsymbol(CgvFigure->LIBRARY, namealloc(temp));
      no=(noeud *)mbkalloc(sizeof(noeud));
      no->LINK=lk;
      no->PARAL=*arbre;
      no->NEXT=NULL;
      no->flag=0;
      no->box=CgvBox;
      no->net=getnet(CgvFigure, src, (char *)src->NAMECHAIN->DATA);
      tr->USER=addptype(tr->USER, CGV_MARK_TYPE, no);
      *arbre=no;
    }

  switch(dir)
    {
    case 'd': no->flag=1; break;
    case 'e': no->flag=4; break;
    default: no->flag=2; break; //vss
    }
  run_branch(CgvFigure, lk->NEXT, next, dir, &no->NEXT);
}

static char *devect (char *s)
{
  int i, j;
  char temp[200];

  for (i = 0, j = 0; s[i] != '\0'; i++) 
    {
      if (s[i] != ')' && s[i] != ']')
        temp[j++] = s[i];
    }
  temp[j] = '\0';

  for (i = 0; i < j; i++)
    if (temp[i] == '[' || temp[i] == '(')
      temp[i] = ' ';

  return namealloc(temp);
}

losig_list *sigoncone(cone_list *cn)
{
  losig_list *conesig=NULL;
  lotrs_list *tr;
  if (cn->BRVDD!=NULL) tr=cn->BRVDD->LINK->ULINK.LOTRS;
  else if (cn->BRVSS!=NULL) tr=cn->BRVSS->LINK->ULINK.LOTRS;
  else if (cn->BREXT!=NULL) 
    {
      if ((cn->BREXT->LINK->TYPE & CNS_EXT)!=0)
        return cn->BREXT->LINK->ULINK.LOCON->SIG;

      tr=cn->BREXT->LINK->ULINK.LOTRS;
    }
  else 
    {/* fprintf(stderr,"cone is empty\n");*/ return NULL; }

  if (devect((char *)tr->SOURCE->SIG->NAMECHAIN->DATA)==devect(cn->NAME))
    conesig=tr->SOURCE->SIG;
  else if (devect((char *)tr->DRAIN->SIG->NAMECHAIN->DATA)==devect(cn->NAME))
    conesig=tr->DRAIN->SIG;
  return conesig;
}

static losig_list *findme(cone_list *cn, char *name)
{
  branch_list *br;
  link_list *lk;
  for (br=cn->BRVDD; br!=NULL; br=br->NEXT)
    for (lk=br->LINK; lk!=NULL; lk=lk->NEXT)
      {
        if (lk->ULINK.LOTRS->GRID->SIG->NAMECHAIN->DATA==name) return lk->ULINK.LOTRS->GRID->SIG;
      }

  for (br=cn->BRVSS; br!=NULL; br=br->NEXT)
    for (lk=br->LINK; lk!=NULL; lk=lk->NEXT)
      {
        if (lk->ULINK.LOTRS->GRID->SIG->NAMECHAIN->DATA==name) return lk->ULINK.LOTRS->GRID->SIG;
      }
  return NULL;
}


static void inittab(cgvcol_list *col)
{
  int i;
  for (i=0;i<100;i++)
    col->tab[i]=MARGE/5;
}
cgvfig_list *cgv_build_cone_fig(cone_list *cn, cgvfig_list *CgvFigure, noeud **arbre)
{
  losig_list *conesig, *ls;
  branch_list *br;
  cgvnet_list *CgvNet;
  edge_list *eg;
  cgvcon_list *CgvCon;
  char *name;
  
  *arbre=NULL;

  conesig=sigoncone(cn);
  if (conesig==NULL) return NULL;

  for (br=cn->BRVDD; br!=NULL; br=br->NEXT)
    {
//      printf(" : ");
      run_branch(CgvFigure, br->LINK, conesig, 'd', arbre);
    }
  for (br=cn->BRVSS; br!=NULL; br=br->NEXT)
    {
//      printf(" : ");
      run_branch(CgvFigure, br->LINK, conesig, 's', arbre);
    }
  for (br=cn->BREXT; br!=NULL; br=br->NEXT)
    {
      run_branch(CgvFigure, br->LINK, conesig, 'e', arbre);
    }

  CgvNet=getnet(CgvFigure, conesig, (char *)conesig->NAMECHAIN->DATA);
  CgvCon = addcgvfigcon(CgvFigure, cn->NAME, CGV_CON_OUT);
  CgvCon->SOURCE = cn;
  CgvCon->SOURCE_TYPE = CGV_SOURCE_CNSCONE;
  addcgvnetcon(CgvNet, CgvCon);
  if (!(cn->TYPE & CNS_EXT)) SetCgvConFake(CgvCon);

  for (eg=cn->INCONE; eg!=NULL; eg=eg->NEXT)
    {
      if (eg->TYPE & CNS_EXT)
        {
          CgvNet=getnet(CgvFigure, eg->UEDGE.LOCON->SIG, eg->UEDGE.LOCON->NAME);
          name=eg->UEDGE.LOCON->NAME;
        }
      else
        {
          ls=sigoncone(eg->UEDGE.CONE);
          if (ls==NULL) ls=findme(cn, eg->UEDGE.CONE->NAME);
          CgvNet=getnet(CgvFigure, ls, eg->UEDGE.CONE->NAME); 
          if (ls!=NULL) name=ls->NAMECHAIN->DATA;
          else name=eg->UEDGE.CONE->NAME;
        }
      
      CgvCon = addcgvfigcon(CgvFigure, name, CGV_CON_IN);
      CgvCon->SOURCE = eg;
      CgvCon->SOURCE_TYPE = CGV_SOURCE_CNSEDGE;
      addcgvnetcon(CgvNet, CgvCon);
      if (!(eg->TYPE & CNS_EXT) && !(eg->UEDGE.CONE->TYPE & CNS_EXT)) SetCgvConFake(CgvCon);
    }
 
  for (CgvNet=CgvFigure->NET; CgvNet!=NULL; CgvNet=CgvNet->NEXT)
    {
      ls=(losig_list *)CgvNet->SOURCE;
      if (ls!=NULL) ls->USER=testanddelptype(ls->USER, CGV_NET_TYPE);
    }
  
  //    buildcgvfig(CgvFigure);
  //  strcpy(getcgvfileext(CgvFigure), IN_LO);
  return (CgvFigure);
}

static int moretogo(noeud *run, int flag)
{
  while (run!=NULL)
    {
      if ((run->flag & flag)!=0) return 1;
      run=run->PARAL;
    }
  return 0;
}
static void run_arbre(cgvfig_list *CgvFigure, noeud *no, cgvcol_list **start_d, cgvcol_list **start_s, int yd, int ys, int posd, int poss, long mmarge)
{
  noeud *run;
  cgvcol_list *startd=*start_d, *starts=*start_s, *old, *last;
  cgvcel_list *Cell;
  cgvbox_list *b;
  noeud *nno;

  last=old=startd;
//  printf(" * ");
  for (run=no; run!=NULL; run=run->PARAL)
    {
      if ((run->flag & 0x0f0)==0 && (run->flag & 5)!=0) // vdd ou externe
        {
          run->flag|=0x0f0;
          
          Cell = addcgvcel(startd);
          addcgvboxtocgvcel(run->box, Cell);
          run->box->Y=yd;
          run->box->misc.count=posd; startd->tab[50+posd]|=0x0800000;
          nno=run;
//          printf("pos+: %p c=%d %s\n",run->box,run->box->misc.count,run->box->NAME);
          last=startd;
          run_arbre(CgvFigure, run->NEXT, &startd, &starts, run->box->Y+run->box->DY+mmarge, ys, posd+1, poss, mmarge);
          if (moretogo(run->PARAL, 5) || posd==1)
            {
              if (startd==HEAD_CGVCOL)
                {
                  startd=addcgvcol(); inittab(startd);
                }
              else
                {
                  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
                  startd=startd->NEXT;
                  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
                }
            }
        }
    }
  if (posd>1 && old!=last)
    {
      HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
      while (old!=last)
        {
          b=loc_add_transparence(CgvFigure, nno->net);
          b->NAME="passp";
          Cell = addcgvcel(old);
          addcgvboxtocgvcel(b, Cell);
          default_box(b, XYAG_DEFAULT_HEIGHT);
          b->Y=yd-b->CON_IN->Y_REL;
          old=old->NEXT;
        }
      HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
    }


  old=starts;
  for (run=no; run!=NULL; run=run->PARAL)
    {
      if ((run->flag & 0x0f0)==0 && (run->flag & 2)!=0) // vss
        {
          run->flag|=0x0f0;
          
          Cell = addcgvcel(starts);
          addcgvboxtocgvcel(run->box, Cell);
          run->box->Y=ys-run->box->DY-mmarge;
          run->box->misc.count=poss; starts->tab[50+poss]|=0x0800000;
          nno=run;

//          printf("pos-: %p c=%d %s\n",run->box,run->box->misc.count,run->box->NAME);
          last=starts;
          run_arbre(CgvFigure, run->NEXT, &startd, &starts, yd, run->box->Y, posd, poss-1, mmarge);
          if (moretogo(run->PARAL, 2) || poss==-1)
            {
              if (starts==HEAD_CGVCOL) 
                {
                  starts=addcgvcol(); inittab(starts);
                }
              else
                {
                  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
                  starts=starts->NEXT;
                  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
                }
            }
        }
    }

  if (poss>1 && old!=last)
    {
      HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
      while (old!=last)
        {
          b=loc_add_transparence(CgvFigure, nno->net);
          b->NAME="passp";
          Cell = addcgvcel(old);
          addcgvboxtocgvcel(b, Cell);
          default_box(b, XYAG_DEFAULT_HEIGHT);
          b->Y=nno->box->Y+b->CON_IN->Y_REL;
          old=old->NEXT;
        }
      HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
    }
  *start_d=startd;
  *start_s=starts;
//  *start=NULL;
}

cgvfig_list *cgv_place_cone_fig(noeud *arbre, cgvfig_list *CgvFigure, long mmarge)
{
  cgvcon_list *CgvCon;
  cgvcol_list *Column;
  int posy;

  Column=addcgvcol(); inittab(Column);
  addcgvcontocgvcel(CgvFigure->CON_OUT, addcgvcel(Column));
  CgvFigure->CON_OUT->Y_REL=0;

  Column=addcgvcol(); inittab(Column);

  run_arbre(CgvFigure, arbre, &Column, &Column,mmarge, 0, 1, -1, mmarge);

  Column=HEAD_CGVCOL;
  posy=0;
  for (CgvCon=CgvFigure->CON_IN; CgvCon!=NULL; CgvCon=CgvCon->NEXT)
    {
      addcgvcontocgvcel(CgvCon, addcgvcel(Column));
      CgvCon->Y_REL=posy;CgvCon->X_REL=0;
      posy+=mmarge;
    }
  return (CgvFigure);
}

cgvcol_list *getlastcol(cgvnet_list *net, cgvcol_list *sc, int *isin)
{
  cgvcel_list *cel;
  cgvbox_list *b;
  cgvcon_list *con;
  cgvcol_list *last=NULL, *prev=NULL;

  while (sc!=NULL)
    {
      for (cel=sc->CELL; cel!=NULL; cel=cel->NEXT)
        {
          if (cel->TYPE == CGV_CEL_BOX)
	    {
	      b=(cgvbox_list *)cel->ROOT;
              if (IsCgvBoxTransparence(b)) continue;
              for (con=b->CON_IN; con!=NULL && con->NET!=net; con=con->NEXT) ;
              if (con==NULL)
                {
                  for (con=b->CON_OUT; con!=NULL && con->NET!=net; con=con->NEXT) ;
                  if (con!=NULL) *isin=0;
                }
              else *isin=1;
              if (con!=NULL) last=sc;
            }
          else
            {
              con=(cgvcon_list *)cel->ROOT;
              if (IsCgvConExternal(con) && IsCgvConIn(con) && con->NET==net)
                {*isin=1; return prev;}
            }
        }
      prev=sc;
      sc=sc->NEXT;
    }
  return last;
}

static void clean_cone(cone_list *cn)
{
  branch_list *br;
  link_list *lk;
  for (br=cn->BRVSS; br!=NULL; br=br->NEXT)
    {
      for (lk=br->LINK; lk!=NULL; lk=lk->NEXT)
        {
          if (getptype(lk->USER, CGV_BOX_TYPE)!=NULL)
            lk->USER=delptype(lk->USER, CGV_BOX_TYPE);
          if (getptype(lk->ULINK.LOTRS->USER, CGV_BOX_TYPE)!=NULL)
            lk->USER=delptype(lk->ULINK.LOTRS->USER, CGV_BOX_TYPE);
        }
    }
  for (br=cn->BRVDD; br!=NULL; br=br->NEXT)
    {
      for (lk=br->LINK; lk!=NULL; lk=lk->NEXT)
        {
          if (getptype(lk->USER, CGV_BOX_TYPE)!=NULL)
            lk->USER=delptype(lk->USER, CGV_BOX_TYPE);
          if (getptype(lk->ULINK.LOTRS->USER, CGV_BOX_TYPE)!=NULL)
            lk->USER=delptype(lk->ULINK.LOTRS->USER, CGV_BOX_TYPE);
        }
    }
  for (br=cn->BREXT; br!=NULL; br=br->NEXT)
    {
      for (lk=br->LINK; lk!=NULL; lk=lk->NEXT)
        {
          if (getptype(lk->USER, CGV_BOX_TYPE)!=NULL)
            lk->USER=delptype(lk->USER, CGV_BOX_TYPE);          
        }
    }
}

int cgv_add_transp(cgvfig_list *CgvFigure, long mmarge)
{
  cgvcon_list *con, *c0;
  cgvbox_list *box, *b;
  cgvcol_list *Column2;
  cgvcol_list *Column, *col;
  cgvcel_list *Cell, *cel;
  int isin, y, hasi;
  chain_list *cl;

  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);

  hasi=cgv_has_input(CgvFigure->CON_OUT->NET->CON_NET);

  for (Column=HEAD_CGVCOL->NEXT; Column!=NULL && Column->NEXT!=NULL; Column=Column->NEXT)
    {
      if (Column->NEXT->NEXT==NULL && !hasi) break;
      b=loc_add_transparence(CgvFigure, CgvFigure->CON_OUT->NET);
      b->NAME="pass0";
      Cell = addcgvcel(Column);
      addcgvboxtocgvcel(b, Cell);
      default_box(b, XYAG_DEFAULT_HEIGHT);
      b->Y=-b->CON_IN->Y_REL;
    }

  for (Column=HEAD_CGVCOL->NEXT; Column!=NULL && Column->NEXT!=NULL; Column=Column->NEXT)
    {
       for (Cell=Column->CELL; Cell!=NULL; Cell=Cell->NEXT)
        {
          if (Cell->TYPE == CGV_CEL_BOX)
	    {
	      box=(cgvbox_list *)Cell->ROOT;
              if (IsCgvBoxTransparence(box)) continue;
//              printf("%s %p %p %d c=%d\n",box->NAME,box,Column,box->Y,box->misc.count);
              for (con=box->CON_IN; con!=NULL; con=con->NEXT)
                {
                  y=-10000;
                  Column2=getlastcol(con->NET, Column, &isin);
                  if (Column!=Column2)
                    {
                      col=Column;
                      do
                        {
                          if (!isin && col->NEXT==Column2) break;
                          col=col->NEXT;
                          if (loc_test_trans(con->NET, col))
                            {
                              b=loc_add_transparence(CgvFigure, con->NET);
                              b->NAME="pass2";
                              cel = addcgvcel(col);
                              addcgvboxtocgvcel(b, cel);
                              default_box(b, XYAG_DEFAULT_HEIGHT);

                              if (y==-10000) y=box->Y+con->Y_REL;
                              
                              if (y==box->Y+con->Y_REL && (col->tab[50+box->misc.count] & 0x0800000)==0)
                                {
                                  col->tab[50+box->misc.count]|=0x0800000;
                                }
                              else
                                {
                                  if (box->misc.count>0)
                                    y=box->Y-(col->tab[50+box->misc.count] & 0x0fffff);
                                  else
                                    y=box->Y+box->DY+(col->tab[50+box->misc.count] & 0x0fffff);

                                  col->tab[50+box->misc.count]+=MARGE/5;
                                  if ((col->tab[50+box->misc.count] & 0x0fffff)>mmarge) return 1;
                                }
                              b->Y=y-b->CON_IN->Y_REL;
//                              printf("\ttrans at %d\n",b->Y);
                            }                          
                        } while (col!=Column2);
                    }
                }

            } 
          for (con=box->CON_OUT; con!=NULL; con=con->NEXT)
            {
              Column2=getlastcol(con->NET, Column, &isin);
              if (!(!isin && Column==Column2))
                {
                  col=Column;
                  do
                    {
                      if (loc_test_trans(con->NET, col))
                        {
                          b=loc_add_transparence(CgvFigure, con->NET);
                          b->NAME="pass1";
                          cel = addcgvcel(col);
                          addcgvboxtocgvcel(b, cel);
                          default_box(b, XYAG_DEFAULT_HEIGHT);
                          
                          if (box->misc.count>0)
                            y=box->Y-(col->tab[50+box->misc.count] & 0x0fffff);
                          else
                            y=box->Y+box->DY+(col->tab[50+box->misc.count] & 0x0fffff);
                          
                          col->tab[50+box->misc.count]+=MARGE/5;
                          if ((col->tab[50+box->misc.count] & 0x0fffff)>mmarge) return 1;
                          
                          b->Y=y-b->CON_IN->Y_REL;
                        }                          
                      if (!isin && col->NEXT==Column2) break;
                      if (col==Column2) break;
                      col=col->NEXT;
                    } while (1);
                }
            }
        }
    }

  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);

  for (Cell=HEAD_CGVCOL->CELL; Cell!=NULL; Cell=Cell->NEXT)
    {
      con=(cgvcon_list *)Cell->ROOT;
      
      for (cl=con->NET->CON_NET; cl!=NULL; cl=cl->NEXT)
        {
          c0=(cgvcon_list *)cl->DATA;
          if (IsCgvConInternal(c0))
            {
              b = (cgvbox_list *) c0->ROOT;
              if (((cgvcel_list *)b->USER)->COL==HEAD_CGVCOL->NEXT)
                {
                  con->Y_REL=b->Y+c0->Y_REL;
                  break;
                }
            }
        }
    }
  organiser_cellules(HEAD_CGVCOL, 0);
  return 0;
}

cgvfig_list *getcgvfig_from_cone(cone_list *cn)
{
  char temp[1024], *name;
  cgvfig_list *CgvFigure;
  int scale, fail;
  long mmarge;
  noeud *arbre;

  sprintf(temp,"cone '%s'",cn->NAME);
  name=namealloc(temp);
  CgvFigure=getcgvfile(name, CGV_FROM_CONE, 0, NULL);
  CGV_WAS_PRESENT=1;
  if (CgvFigure!=NULL) return CgvFigure;
  CGV_WAS_PRESENT=0;


  if (CGV_LIB || SLIB_LIBRARY) 
    { 
      if (SLIB_LIBRARY)  scale = SLIB_LIBRARY->GRID;
      else scale = CGV_LIB->GRID;
    }
  else
    scale = 1024;

  mmarge=MARGE;
  do 
    {
      CgvFigure = addcgvfig(name,scale);
      CgvFigure->SOURCE = cn;
      CgvFigure->SOURCE_TYPE = CGV_FROM_CONE;
      CgvFigure->data0=CgvFigure->data1=CgvFigure->data2=CgvFigure->data_ZOOM=NULL;
      
      if (CGV_LIB || SLIB_LIBRARY) { 
        if (SLIB_LIBRARY)
          {
            CgvFigure->LIBRARY = slib_create_lib(name);
            CgvFigure->LIBRARY->REFHEIGHT = SLIB_LIBRARY->REFHEIGHT; 
          }
        else
          {
            CgvFigure->LIBRARY = slib_create_lib(name);
            //	 CgvFigure->LIBRARY->GRID = CGV_LIB->GRID;
            CgvFigure->LIBRARY->REFHEIGHT = CGV_LIB->REFHEIGHT; 
          }
        CgvFigure->LIBRARY->GRID = scale;
      }
      arbre=NULL;
      if ((CgvFigure=cgv_build_cone_fig(cn, CgvFigure, &arbre))==NULL) return NULL;
      loc_buildbox(CgvFigure);
      CgvFigure=cgv_place_cone_fig(arbre, CgvFigure, mmarge);
      fail=cgv_add_transp(CgvFigure, mmarge);
      if (fail)
        {
          mmarge*=2;
//          printf("failed, adding more space : %ld, and retrying...\n",mmarge);
          loc_clean_all(CgvFigure);
          clean_cone(cn);
          distroy_arbre(arbre);
          if (temp_losig_ht!=NULL)
            {
              delht(temp_losig_ht);
              temp_losig_ht=NULL;
            }
          freecgvfigure(CgvFigure);
        }
    } while (fail);
//  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
  loc_route(CgvFigure);
  loc_clean_all(CgvFigure);
  addcgvfile(name, CGV_FROM_CONE, 0, NULL, CgvFigure, NULL, NULL, 1);
  clean_cone(cn);
  distroy_arbre(arbre);
  if (temp_losig_ht!=NULL)
    {
      delht(temp_losig_ht);
      temp_losig_ht=NULL;
    }
  return CgvFigure;
}
