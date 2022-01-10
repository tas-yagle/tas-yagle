#include GEN_H
#include MSL_H
#include AVT_H
#include MUT_H
#include MLO_H

static int COMPACT=1;

#define SPGNS "* gns> "



static ht_v2 *NAMETOLONG;
static chain_list *ALLNAMES;

static void addname(char *name, long *index)
{
  long idx;
  idx=gethtitem_v2(NAMETOLONG, name);
  if (idx==EMPTYHT)
    {
      addhtitem_v2(NAMETOLONG, name, *index);
      ALLNAMES=addchain(ALLNAMES, name);
      (*index)++;
    }
}

static int getnameindex(char *name)
{
  int l;
  if ((l=gethtitem_v2(NAMETOLONG, name))==EMPTYHT)
    {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 5);
      //printf("%s not found\n",name);
      EXIT(5);
    }

  return l;
}

static void inttochar(char *temp, int value)
{
  int i=0;
  if (value==0) temp[i++]=D_BASE_START;
  else
    {
      while (value!=0)
        {
          temp[i++]=D_BASE_START+(value % D_BASE);
          value/=D_BASE;
        }
    }
  temp[i]='\0';
}


static void correct(char *name, char *dest)
{
  char *c;
  strcpy(dest, name);
  c=strchr(dest,' ');
  if (c!=NULL) *c='_';
}

static void _DriveHeader(FILE *f, corresp_t *crt)
{
  ptype_list *p;
  char name[120];
  char name0[120];
  correct(crt->GENIUS_FIGNAME, name);
  correct(crt->FIGNAME, name0);

  if (!COMPACT)
    {
      fprintf(f,SPGNS"CRT %s %s %s %d %d %d\n",
              crt->GENIUS_INSNAME, name0, name,
              crt->ARCHISTART, crt->ARCHILENGTH, crt->FLAGS
              );
    }
  else
    {
      char code0[10], code1[10],code2[10];
      inttochar(code0, getnameindex(crt->GENIUS_INSNAME));
      inttochar(code1, getnameindex(namealloc(name0)));
      inttochar(code2, getnameindex(namealloc(name)));

      fprintf(f,SPGNS"CRT %s %s %s %d %d %d\n",
              code0, code1, code2,
              crt->ARCHISTART, crt->ARCHILENGTH, crt->FLAGS
              );
    }

  if (crt->VAR!=NULL)
    {
      fprintf(f,SPGNS"  VAR");
      for (p=crt->VAR; p!=NULL; p=p->NEXT)
        fprintf(f," %s %ld ", (char *)p->DATA, p->TYPE);
      fprintf(f,"\n");
    }
}

static void compact_DriveList(FILE *f)
{
  chain_list *cl;
  int count=1024;
  ALLNAMES=reverse(ALLNAMES);

  for (cl=ALLNAMES; cl!=NULL; cl=cl->NEXT)
    {
      if (count>80)
        {
          fprintf(f,SPGNS"d");
          count=strlen(SPGNS"d");
        }
      fprintf(f," %s", (char *)cl->DATA);
      count+=strlen((char *)cl->DATA)+2;
      if (count>80) fprintf(f,"\n");
    }

  fprintf(f,"\n\n");
}

static void _DriveTOPLevel(FILE *f, subinst_t *sins)
{
  while (sins!=NULL)
    {
      fprintf(f,SPGNS"TOPLEVEL %s %s\n", sins->INSNAME, sins->CRT->GENIUS_INSNAME);
      sins=sins->NEXT;
    }

}
 
static void _DriveInstances(FILE *f, subinst_t *sins)
{
  if (sins==NULL) return;
  if (!COMPACT)
    {
      while (sins!=NULL)
        {
          fprintf(f,SPGNS"  i %s %s %ld\n", sins->INSNAME, sins->CRT->GENIUS_INSNAME, sins->FLAGS);
          sins=sins->NEXT;
        }
    }
  else
    {
      int count=100;
      char code0[10], code1[10];
      while (sins!=NULL)
        {
          if (count>10)
            {
              fprintf(f,SPGNS" ci");
              count=0;
            }
          inttochar(code0, getnameindex(sins->INSNAME));
          inttochar(code1, getnameindex(sins->CRT->GENIUS_INSNAME));
          fprintf(f," %s %s %ld", code0, code1, sins->FLAGS);
          count++;
          if (count>10) fprintf(f,"\n");
          sins=sins->NEXT;
        }
      if (count<=10) fprintf(f,"\n");
    }
}

static void _DriveTransistors(FILE *f, corresp_t *crt, lofig_list *lf)
{
  lotrs_list *lt, *ctr;

  if (!COMPACT)
    {
      for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
        {
          ctr=gen_GetCorrespondingTransistor(lt->TRNAME, crt);
          if (ctr==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "017", AVT_FATAL, lt->TRNAME);
              //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] could not find correspondance for transistor '%s'\n",lt->TRNAME);
              EXIT(1);
            }
          else
            {
              if (ctr->TRNAME!=NULL)
                fprintf(f,SPGNS"  t %s %s\n", lt->TRNAME, 
                        ctr->TRNAME
                        );
              else
                {
                  avt_errmsg(GNS_ERRMSG, "018", AVT_FATAL);
                  //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] found a transistor with no name\n");
                  EXIT(1);
                }
            }
        }
    }
  else
    {
      int count=100;
      char code0[10], code1[10];
      for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
        {
          ctr=gen_GetCorrespondingTransistor(lt->TRNAME, crt);
          if (ctr==NULL)
            avt_errmsg(GNS_ERRMSG, "017", AVT_FATAL, lt->TRNAME);

          if (count>15)
            {
              fprintf(f,SPGNS" ct");
              count=0;
            }
          inttochar(code0, getnameindex(lt->TRNAME));
          inttochar(code1, getnameindex(ctr->TRNAME));
          fprintf(f," %s %s", code0, code1);
          count++;
          if (count>15) fprintf(f,"\n");
        }
      if (count<=15) fprintf(f,"\n");
    }
}

static void __vectorize(char *name, char *dest)
{
  if (vectorindex(name)==-1)
    strcpy(dest, name);
  else
    sprintf(dest,"%s(%d)",vectorradical(name),vectorindex(name));
}

static void _DriveSignals(FILE *f, corresp_t *crt, lofig_list *lf)
{
  losig_list *ls, *cls;
  char sig[200], sig0[200];

  if (!COMPACT)
    {
      for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
        {
          __vectorize(gen_losigname(ls), sig);
          cls=gen_corresp_sig (gen_losigname(ls), crt);
          if (cls==NULL && !gen_wasunused())
            {
              avt_errmsg(GNS_ERRMSG, "019", AVT_FATAL, gen_losigname(ls));
              //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] could not find correspondance for signal '%s'\n",gen_losigname(ls));
              EXIT(1);
            }
          else
            {
              if (gen_wasunused()) strcpy(sig0,"{NULL}");
              else __vectorize(gen_losigname(cls), sig0);
              
              fprintf(f,SPGNS"  s %s %s\n", sig, sig0);
            }
        }
    }
  else
    {
      int count=100;
      char code0[10], code1[10];
      for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
        {
          __vectorize(gen_losigname(ls), sig);
          cls=gen_corresp_sig (gen_losigname(ls), crt);
          if (cls!=NULL)
            __vectorize(gen_losigname(cls), sig0);
          else
            if (gen_wasunused()) strcpy(sig0,"{NULL}");
            else 
              {
                avt_errmsg(GNS_ERRMSG, "019", AVT_FATAL, gen_losigname(ls));
                //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] could not find correspondance for signal '%s'\n",gen_losigname(ls));
                //EXIT(1);
              }
          if (count>15)
            {
              fprintf(f,SPGNS" cs");
              count=0;
            }
          inttochar(code0, getnameindex(namealloc(sig)));
          inttochar(code1, getnameindex(namealloc(sig0)));
          fprintf(f," %s %s", code0, code1);
          count++;
          if (count>15) fprintf(f,"\n");
        }
       if (count<=15) fprintf(f,"\n");
    }
}

static void _PrintCall(FILE *f, APICallFunc *cf)
{
  chain_list *cl;
  t_arg *ta;
  fprintf(f," %s", cf->NAME);
  for (cl=cf->ARGS; cl!=NULL; cl=cl->NEXT)
    {
      ta=(t_arg *)cl->DATA;
      if (ta->POINTER==0)
        {
          if (strcmp(ta->TYPE,"char")==0) fprintf(f," '%c'",*(char *)ta->VALUE);
          else if (strcmp(ta->TYPE,"int")==0) fprintf(f," %d",*(int *)ta->VALUE);
          else if (strcmp(ta->TYPE,"double")==0) fprintf(f,"%e",*(double *)ta->VALUE);
          else 
            {
              avt_errmsg(GNS_ERRMSG, "020", AVT_FATAL, ta->TYPE);
              //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] can't drive '%s' type in function call\n",ta->TYPE);
              EXIT(1);
            }
        }
      else
        {
          if (ta->POINTER==1 && strcmp(ta->TYPE,"char")==0)
            {
              char temp[4096];
              int i;
              strcpy(temp, *(char **)ta->VALUE);
              for (i=0;temp[i]!='\0'; i++) if (temp[i]==' ') temp[i]='ù';
              fprintf(f," \"%s\"", temp);
            }
          else
            {
              avt_errmsg(GNS_ERRMSG, "021", AVT_FATAL);
              //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] can't drive pointer type in function call\n",ta->TYPE);
              EXIT(1);
            }
        }

    }
}

static void _DriveMetho(FILE *f, lofig_list *lf)
{
  chain_list *arcs, *cl;
  HierLofigInfo *hli;
  ArcInfo *ai;
  char temp[120];
  int tag=0;

  hli=gethierlofiginfo(lf);
  arcs=GetAllHTElems(hli->ARCS);
  
  if (hli->BUILD_TTV!=NULL || hli->BUILD_BEH!=NULL || arcs!=NULL)
    {
      correct(lf->NAME, temp);
      fprintf(f,SPGNS"fname %s\n",temp);
      tag++;
    }

  if (hli->BUILD_TTV!=NULL)
    {
      fprintf(f,SPGNS" ttv"); _PrintCall(f, hli->BUILD_TTV); fprintf(f,"\n");
    }
  if (hli->BUILD_BEH!=NULL)
    {
      fprintf(f,SPGNS" beh"); _PrintCall(f, hli->BUILD_BEH); fprintf(f,"\n");
    }
  for (cl=arcs; cl!=NULL; cl=cl->NEXT)
    {
      ai=(ArcInfo *)cl->DATA;
      if (ai->MODEL!=NULL)
        {
          fprintf(f,SPGNS" aim %s",ai->arc_name); _PrintCall(f, ai->MODEL); fprintf(f,"\n");
        }
      if (ai->SIM!=NULL)
        {
          fprintf(f,SPGNS" ais %s",ai->arc_name); _PrintCall(f, ai->SIM); fprintf(f,"\n");
        }
      if (ai->ENV!=NULL)
        {
          fprintf(f,SPGNS" aie %s",ai->arc_name); _PrintCall(f, ai->ENV); fprintf(f,"\n");
        }
    }
  freechain(arcs);
  if (tag) fprintf(f,"\n");
}


static void recur_drive_corresp(FILE *f, subinst_t *si)
{
  subinst_t *sins;
  lofig_list *lf;

  for (sins=si->CRT->SUBINSTS; sins!=NULL; sins=sins->NEXT)
    recur_drive_corresp(f, sins);
  
  _DriveHeader(f, si->CRT);

  lf=getloadedlofig(si->CRT->GENIUS_FIGNAME);
  if (lf==NULL)
    {
      avt_errmsg(GNS_ERRMSG, "022", AVT_FATAL, si->CRT->GENIUS_FIGNAME);
      //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] can't find figure '%s'\n",si->CRT->GENIUS_FIGNAME);
      EXIT(2);
    }
  _DriveTransistors(f, si->CRT, lf);
  _DriveSignals(f, si->CRT, lf);
  _DriveInstances(f, si->CRT->SUBINSTS);

  fprintf(f,SPGNS"ENDCRT\n\n");
}

static void nametoindex_DriveHeader(corresp_t *crt, long *index)
{
  ptype_list *p;
  char name[120];
  char name0[120];
  correct(crt->GENIUS_FIGNAME, name);
  correct(crt->FIGNAME, name0);
  addname(crt->GENIUS_INSNAME, index);
  addname(namealloc(name0), index);
  addname(namealloc(name), index);

  if (crt->VAR!=NULL)
    {
      for (p=crt->VAR; p!=NULL; p=p->NEXT)
        addname((char *)p->DATA, index);
    }
}

static void nametoindex_DriveTransistors(corresp_t *crt, lofig_list *lf, long *index)
{
  lotrs_list *lt, *ctr;

  for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
    {
      ctr=gen_GetCorrespondingTransistor(lt->TRNAME, crt);
      if (ctr==NULL)
        {
          avt_errmsg(GNS_ERRMSG, "017", AVT_ERROR, lt->TRNAME);
          //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] could not find correspondance for transistor '%s'\n",lt->TRNAME);
          continue;
        }
      else
        {
          if (ctr->TRNAME!=NULL)
            {
              addname(lt->TRNAME, index);
              addname(ctr->TRNAME, index);
            }
          else
            {
              avt_errmsg(GNS_ERRMSG, "018", AVT_FATAL);
              //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] found a transistor with no name\n");
              EXIT(1);
            }
        }
    }
}
static void nametoindex_DriveInstances(subinst_t *sins, long *index)
{
  while (sins!=NULL)
    {
      addname(sins->INSNAME, index);
      addname(sins->CRT->GENIUS_INSNAME, index);
      sins=sins->NEXT;
    }
}

static void nametoindex_DriveSignals(corresp_t *crt, lofig_list *lf, long *index)
{
  losig_list *ls, *cls;
  char sig[200], sig0[200];

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      __vectorize(gen_losigname(ls), sig);
      cls=gen_corresp_sig (gen_losigname(ls), crt);
      if (cls==NULL && !gen_wasunused())
        {
          avt_errmsg(GNS_ERRMSG, "019", AVT_ERROR, gen_losigname(ls));
          //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] could not find correspondance for signal '%s'\n",gen_losigname(ls));
          continue;
        }
      else
        {
          if (gen_wasunused()) strcpy(sig0,"{NULL}");
          else __vectorize(gen_losigname(cls), sig0);

          addname(namealloc(sig), index);
          addname(namealloc(sig0), index);
        }
    }
}

static void recur_nametoindex(subinst_t *si, long *index)
{
  subinst_t *sins;
  lofig_list *lf;

  for (sins=si->CRT->SUBINSTS; sins!=NULL; sins=sins->NEXT)
    recur_nametoindex(sins, index);
  
  nametoindex_DriveHeader(si->CRT, index);

  lf=getloadedlofig(si->CRT->GENIUS_FIGNAME);
  if (lf==NULL)
    {
      avt_errmsg(GNS_ERRMSG, "022", AVT_FATAL, si->CRT->GENIUS_FIGNAME);
      //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "][" AVT_BLUE "crt driver" AVT_RESET "] can't find figure '%s'\n",si->CRT->GENIUS_FIGNAME);
      EXIT(2);
    }
  nametoindex_DriveTransistors(si->CRT, lf, index);
  nametoindex_DriveSignals(si->CRT, lf, index);
  nametoindex_DriveInstances(si->CRT->SUBINSTS, index);

}


static void DriveCorresp(FILE *f, ALL_FOR_GNS *all)
{
  subinst_t *sins;
  mbkContext tmp;
  chain_list *cl, *ch;
  lofig_list *lf;
  long index=0;
  char oldSPI_VECTOR[64];
  char temp[1024];
  int oldasins;

  
  strcpy(oldSPI_VECTOR, SPI_VECTOR);
  strcpy(SPI_VECTOR, "[]");
  oldasins=V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE;
  V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE=0;

  for (cl=NULL, lf=all->HIER_HEAD_LOFIG; lf!=NULL; lf=lf->NEXT)
    cl=addchain(cl, lf);

  sprintf(temp,"correspondance tables of '%s'",all->FIGNAME);
  spiceprintdate(temp, f);
  fprintf(f,SPGNS"TModelP");
  for (ch=TPMOS; ch!=NULL; ch=ch->NEXT) fprintf(f," %s",(char *)ch->DATA);
  fprintf(f,"\n");
  fprintf(f,SPGNS"TModelN");
  for (ch=TNMOS; ch!=NULL; ch=ch->NEXT) fprintf(f," %s",(char *)ch->DATA);
  fprintf(f,"\n");

  spicesavelofigsinfile(cl, f);

  for (lf=all->HIER_HEAD_LOFIG; lf!=NULL; lf=lf->NEXT)
    _DriveMetho(f, lf);

  freechain(cl);

  tmp.HT_LOFIG=all->HIER_HT_LOFIG;
  tmp.HEAD_LOFIG=all->HIER_HEAD_LOFIG;

  mbkSwitchContext(&tmp);

  if (COMPACT)
    {
      NAMETOLONG=addht_v2(0);
      ALLNAMES=NULL;
      for (sins=all->TOP_LEVEL_SUBINST; sins!=NULL; sins=sins->NEXT)
        recur_nametoindex(sins, &index);
      fprintf(f,SPGNS"DICOSIZE %ld\n",index);
      compact_DriveList(f);
    }

  for (sins=all->TOP_LEVEL_SUBINST; sins!=NULL; sins=sins->NEXT)
    recur_drive_corresp(f, sins);

  _DriveTOPLevel(f, all->TOP_LEVEL_SUBINST);

  if (COMPACT)
    {
      delht_v2(NAMETOLONG);
      freechain(ALLNAMES);
    }

  mbkSwitchContext(&tmp);

  V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE=oldasins;
  strcpy(SPI_VECTOR, oldSPI_VECTOR);
}



void gnsDriveCorrespondanceTables(ALL_FOR_GNS *all, int mode)
{
  FILE *f=mbkfopen(all->FIGNAME,"gns", WRITE_TEXT);
  if (f==NULL)
    {
      avt_errmsg(GNS_ERRMSG, "023", AVT_ERROR, all->FIGNAME);
      //avt_fprintf(stderr,"[" AVT_CYAN "" AVT_BOLD "Error" AVT_RESET "] Could not write file %s.gns\n",all->FIGNAME);
      return;
    }
  if (mode=='y' || mode=='Y') COMPACT=1;
  else COMPACT=0;
 
  DriveCorresp(f, all);
  fclose(f);
}


        
static void _ApplyConnectorOrientation(corresp_t *crt, lofig_list *lf)
{
  lotrs_list *ctr;
  losig_list *cls;
  chain_list *cl;
  locon_list *lc, *lc0, *clc0;
  long flag;
  ptype_list *pt;

  for (lc=lf->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      if (lc->DIRECTION == IN)
        flag = LOCON_INFORMATION_INPUT;
      else if (lc->DIRECTION == OUT || lc->DIRECTION == INOUT || lc->DIRECTION == TRISTATE)
        flag = LOCON_INFORMATION_OUTPUT;
      else
        flag = 0;

      if (flag != 0) 
        {
          cls=gen_corresp_sig (gen_losigname(lc->SIG), crt);          
          if (cls!=NULL && !mbk_LosigIsVSS(cls) && !mbk_LosigIsVDD(cls))
            {
              if ((pt = getptype (lc->SIG->USER, LOFIGCHAIN)) != NULL) 
                {
                  for (cl = (chain_list *) pt->DATA; cl != NULL; cl = cl->NEXT)
                    {
                      lc0 = (locon_list *) cl->DATA;
                      if (lc0->TYPE=='T' && lc0->NAME != MBK_GRID_NAME && lc0->NAME != MBK_BULK_NAME)
                        {
                          ctr=gen_GetCorrespondingTransistor(((lotrs_list *)lc0->ROOT)->TRNAME, crt);
                          if (ctr!=NULL)
                            {
                              if (ctr->DRAIN->SIG==cls) clc0=ctr->DRAIN;
                              else clc0=ctr->SOURCE;
                              
                              if ((pt = getptype (clc0->USER, LOCON_INFORMATION)) == NULL)
                                pt = clc0->USER = addptype (clc0->USER, LOCON_INFORMATION, 0);
                          
                              if ((long)pt->DATA != 0 && (long)pt->DATA != flag)
                                printf ("conflicting orientation information on net '%s' in figure '%s'\n", getsigname (lc->SIG), lf->NAME);
                              else
                                pt->DATA = (void *)(long)flag;
                            }
                        }
                    }
                }
            }
        }
    }
}

static void recur_applycondir(subinst_t *si)
{
  subinst_t *sins;
  lofig_list *lf;

  for (sins=si->CRT->SUBINSTS; sins!=NULL; sins=sins->NEXT)
    recur_applycondir(sins);

  lf=getloadedlofig(si->CRT->GENIUS_FIGNAME);
  if (lf==NULL)
    {
      avt_errmsg(GNS_ERRMSG, "022", AVT_FATAL, si->CRT->GENIUS_FIGNAME);
      EXIT(2);
    }
  _ApplyConnectorOrientation(si->CRT, lf);
}

void gnsApplyConnectorOrientation(ALL_FOR_GNS *all)
{
  subinst_t *sins;
  mbkContext tmp;
  
  tmp.HT_LOFIG=all->HIER_HT_LOFIG;
  tmp.HEAD_LOFIG=all->HIER_HEAD_LOFIG;

  mbkSwitchContext(&tmp);

  for (sins=all->TOP_LEVEL_SUBINST; sins!=NULL; sins=sins->NEXT)
    recur_applycondir(sins);

  mbkSwitchContext(&tmp);
}
