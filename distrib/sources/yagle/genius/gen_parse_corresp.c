
#include GEN_H
#include MSL_H
#include AVT_H
#include MLU_H

#define SPGNS "gns>"
#define MAXLEXEM 512


static ALL_FOR_GNS *cur;
static int dicosize, dicoindex;
static char **dicotab;


static int chartoint(char *temp)
{
  int value=0, i;

  for (i=strlen(temp)-1;i>=0;i--)
    {
      value=(value*D_BASE)+((unsigned char)temp[i])-D_BASE_START;
    }
  return value;
}

static char *nameget(char *indexcode)
{
  int index;
  index=chartoint(indexcode);
  if (index>dicoindex)
    EXIT(5);
  return dicotab[index];
}

static int splitline(char *line, char **tab)
{
  char c[2048], s;
  int cnt=0, i=0, start;
  strcpy(c, line);
  do 
    {
      while (c[i]!='\n' && (c[i]==' ' || c[i]=='\t')) i++;
      start=i;
      if (c[i]=='\n') return cnt;
      while (c[i]!='\n' && c[i]!=' ' && c[i]!='\t') i++;
      s=c[i]; c[i]='\0';
      tab[cnt++]=mbkstrdup(&c[start]);
      c[i]=s;
    } while (cnt<MAXLEXEM && c[i]!='\n');
  return cnt;
}

static corresp_t *__crt;
static ht *ALL_CRT, *ALL_TRS, *ALL_SIG;


static void prepht(lofig_list *lf)
{
  losig_list *ls;
  lotrs_list *lt;
  int cnt;

  for (ls=lf->LOSIG, cnt=0; ls!=NULL; ls=ls->NEXT, cnt++) ;
  if (cnt==0) cnt++;
  ALL_SIG=addht(cnt);
  for (lt=lf->LOTRS, cnt=0; lt!=NULL; lt=lt->NEXT, cnt++) ;
  if (cnt==0) cnt++;
  ALL_TRS=addht(cnt);

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      if (ls->NAMECHAIN!=NULL) addhtitem(ALL_SIG, namealloc(gen_makesignalname(gen_losigname(ls))), (long)ls);
    }

  for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
    if (lt->TRNAME!=NULL) addhtitem(ALL_TRS, lt->TRNAME, (long)lt);
}

static HierLofigInfo *cur_hli=NULL;


static APICallFunc *buildcallfunc(char **tab, int cnt)
{
  APICallFunc *cf;
  chain_list *cl=NULL;
  int i;
  cf=(APICallFunc *)mbkalloc(sizeof(APICallFunc));
  cf->NAME=sensitive_namealloc(tab[0]);
  for (i=1; i<cnt;i++)
    {
      if (tab[i][0]=='\'')
        {
          cl=APIAddCharTARG(cl, "?", tab[i][1]);
        }
      else if (tab[i][0]=='"')
        {
          char temp[4096];
          int j;
          strcpy(temp, &tab[i][1]);
          temp[strlen(temp)-1]='\0';
          for (j=0; temp[j]!='\0'; j++) if (temp[j]=='ù') temp[j]=' ';
          cl=APIAddPointerTARG(cl, "?", "char", 1, sensitive_namealloc(temp));
        }
      else if (strchr(tab[i],'.')!=NULL)
        {
          cl=APIAddDoubleTARG(cl, "?", atof(tab[i]));
        }
      else 
        {
          cl=APIAddIntTARG(cl, "?", atoi(tab[i]));
        }
    }
  cf->ARGS=reverse(cl);
  return cf;
}


static int parseMetho(char **tab, int cnt)
{
  int ret=1;
  ArcInfo *ai;
  if (cnt==4 && strcasecmp(tab[2], "fname")==0)
    {
      lofig_list *lf=getlofig(tab[3],'A');
      cur_hli=gethierlofiginfo(lf);
    }
  else
    {
      if (cur_hli!=NULL)
        {
          if (cnt>=4 && strcasecmp(tab[2],"ttv")==0)
            {
              cur_hli->BUILD_TTV=buildcallfunc(&tab[3], cnt-3);
            }
          else if (cnt>=4 && strcasecmp(tab[2],"beh")==0)
            {
              cur_hli->BUILD_BEH=buildcallfunc(&tab[3], cnt-3);
            }
          else if (cnt>4 && strcasecmp(tab[2],"aim")==0)
            {
              ai=getarcinfo(cur_hli, namealloc(tab[3]));
              ai->MODEL=buildcallfunc(&tab[4], cnt-4);
            }
          else if (cnt>4 && strcasecmp(tab[2],"ais")==0)
            {
              ai=getarcinfo(cur_hli, namealloc(tab[3]));
              ai->SIM=buildcallfunc(&tab[4], cnt-4);
            }
          else if (cnt>4 && strcasecmp(tab[2],"aie")==0)
            {
              ai=getarcinfo(cur_hli, namealloc(tab[3]));
              ai->ENV=buildcallfunc(&tab[4], cnt-4);
            }
          else
            {
              ret=0;
            }
        }
      else ret=0;
    }
  return ret;
}

	  
static int ParseCorresp(char *tmp, int process)
{
  char *tab[MAXLEXEM], *c;
  int cnt, i, ret=0;
  long l;
  chain_list *cl;


  cnt=splitline(tmp, tab);
  if (cnt>2 && strcasecmp(tab[1],SPGNS)==0 && process==0)
  {
     for (i=0;i<cnt; i++) mbkfree(tab[i]);
     return TRUE;
  }
  if (cnt>2 && strcasecmp(tab[1],SPGNS)==0)
    {
      if (cnt==9 && strcasecmp(tab[2],"crt")==0)
        {
          int as, al;
          __crt=(corresp_t *)AddHeapItem (&cur->corresp_heap);
          as=atoi(tab[6]); al=atoi(tab[7]);
          
          if (dicotab==NULL)
            {
              __crt->GENIUS_INSNAME=namealloc(tab[3]);
              __crt->FIGNAME=namealloc(tab[4]);
              
              if (as!=0) tab[5][as-1]=' ';
              __crt->GENIUS_FIGNAME=namealloc(tab[5]);
            }
          else
            {
              char temp[128];
              __crt->GENIUS_INSNAME=nameget(tab[3]);
              __crt->FIGNAME=nameget(tab[4]);
              strcpy(temp, nameget(tab[5]));
              if (as!=0) temp[as-1]=' ';
              __crt->GENIUS_FIGNAME=namealloc(temp);
            }

          __crt->ARCHISTART=as;
          __crt->ARCHILENGTH=al;
          __crt->FLAGS=atoi(tab[8]);
          __crt->TRANSISTORS=NULL;
          __crt->SIGNALS=NULL;
          __crt->SUBINSTS=NULL;
          __crt->VAR=NULL;
          addhtitem(ALL_CRT, __crt->GENIUS_INSNAME, (long)__crt);
        }
      else if (cnt==3 && strcasecmp(tab[2],"endcrt")==0)
        {
          __crt=NULL;
        }
      else if (cnt==5 && strcasecmp(tab[2],"t")==0)
        {
          if (__crt==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "068", AVT_ERROR, msl_line);
              //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, transistor out of context\n",msl_line);
              ret=1;
            }
          else
            {
              c=namealloc(tab[4]);
              if ((l=gethtitem(ALL_TRS, c))==EMPTYHT)
                {
                  avt_errmsg(GNS_ERRMSG, "069", AVT_ERROR, msl_line, tab[4]);
                  //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, could not find transistor '%s' in original netlist\n",msl_line, tab[4]);
                  ret=1;
                }
              else
                {
                  if (__crt->TRANSISTORS==NULL) __crt->TRANSISTORS=addht(10);
                  addhtitem(__crt->TRANSISTORS, namealloc(gen_makesignalname(tab[3])), l);
                }
            }
        }
      else if (cnt==5 && strcasecmp(tab[2],"s")==0)
        {
          if (__crt==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "070", AVT_ERROR, msl_line);
              //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, signal out of context\n",msl_line);
              ret=1;
            }
          else
            {
              c=namealloc(gen_makesignalname(tab[4]));
              if ((l=gethtitem(ALL_SIG, c))==EMPTYHT)
                {
                  avt_errmsg(GNS_ERRMSG, "071", AVT_ERROR, msl_line, tab[4]);
//                  avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, could not find signal '%s' in original netlist\n",msl_line, tab[4]);
                  ret=1;
                }
              else
                {
                  if (__crt->SIGNALS==NULL) __crt->SIGNALS=addht(10);
                  c=namealloc(gen_makesignalname(tab[3])); 
                  addhtitem(__crt->SIGNALS, c, l);
                }
            }
        }
      else if (cnt==6 && strcasecmp(tab[2],"i")==0)
        {
          if (__crt==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "072", AVT_ERROR, msl_line);
              avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, instance out of context\n",msl_line);
              ret=1;
            }
          else
            {
              c=namealloc(tab[4]);
              if ((l=gethtitem(ALL_CRT, c))==EMPTYHT)
                {
                  avt_errmsg(GNS_ERRMSG, "073", AVT_ERROR, msl_line, tab[4]);
                  //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, could not find instance correspondance '%s' table\n",msl_line, tab[4]);
                  ret=1;
                }
              else
                {
                 subinst_t *sins;
                 corresp_t *crt=(corresp_t *)l;
                 sins=(subinst_t *)AddHeapItem(&cur->subinst_heap);
                 sins->INSNAME=namealloc(tab[3]);
                 sins->FLAGS=atoi(tab[5]);
                 sins->CRT=crt;
                 sins->NEXT=__crt->SUBINSTS;
                 __crt->SUBINSTS=sins;
                }
            }
        }
      else if (cnt==5 && strcasecmp(tab[2],"toplevel")==0)
        {
          c=namealloc(tab[4]);
          if ((l=gethtitem(ALL_CRT, c))==EMPTYHT)
            {
              avt_errmsg(GNS_ERRMSG, "074", AVT_ERROR, msl_line);
              //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, could not find instance correspondance table\n",msl_line);
              ret=1;
            }
          else
            {
              subinst_t *sins;
              corresp_t *crt=(corresp_t *)l;
              sins=(subinst_t *)AddHeapItem(&cur->subinst_heap);
              sins->INSNAME=namealloc(tab[3]);
              sins->FLAGS=crt->FLAGS;
              sins->CRT=crt;
              sins->NEXT=cur->TOP_LEVEL_SUBINST;
              cur->TOP_LEVEL_SUBINST=sins;
            }
        }
      else if (strcasecmp(tab[2],"var")==0)
        {
          if (__crt==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "075", AVT_ERROR, msl_line);
              //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, variables out of context\n",msl_line);
              ret=1;
            }
          else
            {
              i=3;
              while (i+1<cnt)
                {
                  __crt->VAR=addptype(__crt->VAR, atol(tab[i+1]), namealloc(tab[i]));
                  i+=2;
                }
            }
        }
      else if (cnt==4 && strcasecmp(tab[2],"dicosize")==0)
        {
          dicosize=atoi(tab[3]);
          dicotab=(char **)mbkalloc(sizeof(char *)*dicosize);
        }
      else if (cnt>2 && strcasecmp(tab[2],"d")==0)
        {
          if (dicotab==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "076", AVT_FATAL, msl_line);
              //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, dictionary entry without dictionary mode\n",msl_line);
              EXIT(1);
            }
          for (i=3;i<cnt;i++)
            {
              if (dicoindex==dicosize)
                {
                  avt_errmsg(GNS_ERRMSG, "077", AVT_FATAL, msl_line);
//                  avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, too many entries in dictionary\n",msl_line);
                  EXIT(1);
                }
              dicotab[dicoindex++]=namealloc(tab[i]);
            }
        }
      else if (cnt>2 && strcasecmp(tab[2],"ct")==0)
        {
          if (__crt==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "068", AVT_ERROR, msl_line);
//              avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, transistor out of context\n",msl_line);
              ret=1;
            }
          else
            {
              for (i=3;i<cnt;i+=2)
                {
                  c=nameget(tab[i+1]);
                  if ((l=gethtitem(ALL_TRS, c))==EMPTYHT)
                    {
                      avt_errmsg(GNS_ERRMSG, "069", AVT_ERROR, msl_line, c);
                      //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, could not find transistor '%s' in original netlist\n",msl_line, c);
                      ret=1;
                    }
                  else
                    {
                      if (__crt->TRANSISTORS==NULL) __crt->TRANSISTORS=addht(10);
                      addhtitem(__crt->TRANSISTORS, namealloc(gen_makesignalname(nameget(tab[i]))), l);
                    }
                }
            }
        }
      else if (cnt>2 && strcasecmp(tab[2],"cs")==0)
        {
          if (__crt==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "070", AVT_ERROR, msl_line);
              //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, signal out of context\n",msl_line);
              ret=1;
            }
          else
            {
               for (i=3;i<cnt;i+=2)
                {
                  c=namealloc(gen_makesignalname(nameget(tab[i+1])));
                  if ((l=gethtitem(ALL_SIG, c))==EMPTYHT)
                    {
                      avt_errmsg(GNS_ERRMSG, "071", AVT_ERROR, msl_line, c);
//                      avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, could not find signal '%s' in original netlist\n",msl_line, c);
                      ret=1;
                    }
                  else
                    {
                      if (__crt->SIGNALS==NULL) __crt->SIGNALS=addht(10);
                      c=namealloc(gen_makesignalname(nameget(tab[i]))); 
                      addhtitem(__crt->SIGNALS, c, l);
                    }
                }
            }
        }
      else if (strcasecmp(tab[2],"ci")==0)
        {
          if (__crt==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "072", AVT_ERROR, msl_line);
//              avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, instance out of context\n",msl_line);
              ret=1;
            }
          else
            {
              for (i=3;i<cnt;i+=3)
                {
                  c=namealloc(nameget(tab[i+1]));
                  if ((l=gethtitem(ALL_CRT, c))==EMPTYHT)
                    {
                      avt_errmsg(GNS_ERRMSG, "073", AVT_ERROR, msl_line, c);
                      //avt_fprintf(stderr,"[¤6¤+Error¤.] While parsing correspondance tables, line %d, could not find instance correspondance '%s' table\n",msl_line, c);
                      ret=1;
                    }
                  else
                    {
                      subinst_t *sins;
                      corresp_t *crt=(corresp_t *)l;
                      sins=(subinst_t *)AddHeapItem(&cur->subinst_heap);
                      sins->INSNAME=namealloc(nameget(tab[i]));
                      sins->FLAGS=atoi(tab[i+2]);
                      sins->CRT=crt;
                      sins->NEXT=__crt->SUBINSTS;
                      __crt->SUBINSTS=sins;
                    }
                }
            }
        }
      else if (cnt>3 && (strcasecmp(tab[2],"TModelP")==0 || strcasecmp(tab[2],"TModelN")==0))
        {
          chain_list **head;
          if (strcasecmp(tab[2],"TModelP")==0) head=&TPMOS; else head=&TNMOS;
          for (i=3;i<cnt;i++)
            {
              c=namealloc(tab[i]);
              for (cl=*head; cl!=NULL && cl->DATA!=c; cl=cl->NEXT) ;
              if (cl==NULL) *head=addchain(*head, c);
            }
        }
      else 
        if (!parseMetho(tab, cnt))
          {
            avt_errmsg(GNS_ERRMSG, "078", AVT_ERROR, msl_line);
//            avt_fprintf(stderr,"[¤4¤+Warning¤.] While parsing correspondance tables, line %d ignored\n",msl_line);
          }
    }
  
  for (i=0;i<cnt; i++) mbkfree(tab[i]);
  return TRUE;
}


static void cleanpnodes(lofig_list *lf)
{
  chain_list *cl;
  ptype_list *pt;
  losig_list *ls;
  locon_list *lc;

  while (lf!=NULL)
    {
      for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
        {
          if ((pt=getptype(ls->USER, LOFIGCHAIN))!=NULL)
            {
              for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
                {
                  lc=(locon_list *)cl->DATA;
                  freenum(lc->PNODE);
                  lc->PNODE=NULL;
                }
            }
        }
      lf=lf->NEXT;
    }
}


ALL_FOR_GNS *gnsParseCorrespondanceTables(char *name)
{
  char tzmp, temp[1024];
  int tymp;
  ALL_FOR_GNS *all;
  mbkContext *mc;
  char *oldconfig;

  oldconfig=V_STR_TAB[__MBK_SPICE_KEEP_NAMES].VALUE;
//  if (oldconfig==NULL) oldconfig="";
  V_STR_TAB[__MBK_SPICE_KEEP_NAMES].VALUE=strdup("All");
//  avt_sethashvar("avtSpiKeepNames", "All");
  /*
  tzmp=MBK_NO_TRNAME;
  tymp=SPI_PARSE_ALLNAME;
  MBK_NO_TRNAME='N';
  SPI_PARSE_ALLNAME=1;
*/
  sprintf(temp,"%s.gns",name);

  if (filepath(temp,NULL)==NULL) return NULL;

  cur=all=(ALL_FOR_GNS *)mbkalloc(sizeof(ALL_FOR_GNS));
  all->FIGNAME=namealloc(name);

  CreateHeap (sizeof (subinst_t), 0, &all->subinst_heap);
  CreateHeap (sizeof (corresp_t), 0, &all->corresp_heap);   
  
  all->ALL_INSTANCES=NULL;
  all->GLOBAL_LOFIG=getlofig(name, 'A');
  all->TOP_LEVEL_SUBINST=NULL;

  mbk_removeparallel(all->GLOBAL_LOFIG, 0);

  mc=mbkCreateContext();
  mbkSwitchContext(mc);

  __crt=NULL;
  ALL_CRT=addht(10000);
  prepht(all->GLOBAL_LOFIG);
  dicotab=NULL; dicoindex=0;

  mslAddExtension(ParseCorresp); // --->
  parsespice(temp);
  mslRmvExtension(ParseCorresp); // <---

  delht(ALL_CRT);
  delht(ALL_TRS);
  delht(ALL_SIG);

  mbkSwitchContext(mc);
  all->HIER_HEAD_LOFIG=mc->HEAD_LOFIG;
  all->HIER_HT_LOFIG=mc->HT_LOFIG;

  cleanpnodes(all->HIER_HEAD_LOFIG);

  mbkFreeContext(mc);
  
  free(V_STR_TAB[__MBK_SPICE_KEEP_NAMES].VALUE);
  V_STR_TAB[__MBK_SPICE_KEEP_NAMES].VALUE=oldconfig;
//  avt_sethashvar("avtSpiKeepNames", oldconfig);
/*
  MBK_NO_TRNAME=tzmp;
  SPI_PARSE_ALLNAME=tymp;
*/
  mbk_restoreparallel(all->GLOBAL_LOFIG);

  return all;
}

/*
 [!] pour la destruction d'un context gns:

- supprimer les tables de hash servant a accelerer les recherche des losigs dans la netlist hierarchique

*/
