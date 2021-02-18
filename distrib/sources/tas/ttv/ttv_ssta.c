#include "ttv.h"
#include "ttv_util.h"
#include "ttv_ssta.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>

typedef struct
{
  ssta_entry_header header;
  ssta_entry_values values[];
} ssta_entry;

typedef struct
{
  char *filename;
  int error;
  int cachenum;
  ssta_entry_header header;
  ssta_entry_values *sev;
  float *sevsig;
} one_run_entry;

typedef struct
{
  int nb_run;
  int nb_val;
  int nb_sigval;
  int key;
  int cur_run;
  int cachenum;
  float totalloaded;
  one_run_entry *ore;
} ssta_delays;

ssta_delays *CURRENT_STORE=NULL;

static ttvfig_list *sort_ttvfig;

static inline long masked_line_type(long type)
{
  return type & (TTV_LINE_HZ|TTV_LINE_EV|TTV_LINE_PR|TTV_LINE_RC|TTV_LINE_R|TTV_LINE_S|
                 TTV_LINE_A|TTV_LINE_U|TTV_LINE_O|TTV_LINE_CONT);
}

static inline long masked_node_type(long type)
{
  return type & (TTV_NODE_UP|TTV_NODE_DOWN);
}

static inline long masked_sig_type(long type)
{
  return type & TTV_SIG_TYPEALL;
}

static int sortlines(const void *a0, const void *b0)
{
  char buf0[1024], buf1[1024];
  ttvline_list *a=*(ttvline_list **)a0;
  ttvline_list *b=*(ttvline_list **)b0;
  ttvevent_list *cmda, *cmdb;
  int res;
  
  if (masked_line_type(a->TYPE)<masked_line_type(b->TYPE)) return -1;
  if (masked_line_type(a->TYPE)>masked_line_type(b->TYPE)) return 1;
  if (masked_node_type(a->ROOT->TYPE)<masked_node_type(b->ROOT->TYPE)) return -1;
  if (masked_node_type(a->ROOT->TYPE)>masked_node_type(b->ROOT->TYPE)) return 1;
  if (masked_sig_type(a->ROOT->ROOT->TYPE)<masked_sig_type(b->ROOT->ROOT->TYPE)) return -1;
  if (masked_sig_type(a->ROOT->ROOT->TYPE)>masked_sig_type(b->ROOT->ROOT->TYPE)) return 1;
  if (masked_node_type(a->NODE->TYPE)<masked_node_type(b->NODE->TYPE)) return -1;
  if (masked_node_type(a->NODE->TYPE)>masked_node_type(b->NODE->TYPE)) return 1;
  if (masked_sig_type(a->NODE->ROOT->TYPE)<masked_sig_type(b->NODE->ROOT->TYPE)) return -1;
  if (masked_sig_type(a->NODE->ROOT->TYPE)>masked_sig_type(b->NODE->ROOT->TYPE)) return 1;

  ttv_getsigname(sort_ttvfig, buf0, a->ROOT->ROOT);
  ttv_getsigname(sort_ttvfig, buf1, b->ROOT->ROOT);
  res=strcmp(buf0, buf1);
  if (res<0) return -1;
  if (res>0) return 1;
  
  ttv_getsigname(sort_ttvfig, buf0, a->NODE->ROOT);
  ttv_getsigname(sort_ttvfig, buf1, b->NODE->ROOT);
  res=strcmp(buf0, buf1);
  if (res<0) return -1;
  if (res>0) return 1;
  cmda = ttv_getlinecmd(sort_ttvfig,a,TTV_LINE_CMDMAX) ;
  cmdb = ttv_getlinecmd(sort_ttvfig,b,TTV_LINE_CMDMAX) ;
  if (cmda!=NULL && cmdb!=NULL)
  {
    if (masked_node_type(cmda->TYPE)<masked_node_type(cmdb->TYPE)) return -1;
    if (masked_node_type(cmda->TYPE)>masked_node_type(cmdb->TYPE)) return 1;
    if (masked_sig_type(cmda->ROOT->TYPE)<masked_sig_type(cmdb->ROOT->TYPE)) return -1;
    if (masked_sig_type(cmda->ROOT->TYPE)>masked_sig_type(cmdb->ROOT->TYPE)) return 1;
    ttv_getsigname(sort_ttvfig, buf0, cmda->ROOT);
    ttv_getsigname(sort_ttvfig, buf1, cmdb->ROOT);
    res=strcmp(buf0, buf1);
    if (res<0) return -1;
    if (res>0) return 1;
  }
  else if (cmda!=NULL && cmdb==NULL) return -1;
  else if (cmda==NULL && cmdb!=NULL) return 1;
  return 0;
}

static chain_list *ttv_getalllines(ttvfig_list *tvf)
{
  
  long type;
  chain_list *endlist, *cl, *alllines;
  ttvline_list *line;
  ttvsig_list *tvs;
  int i;
        
  ttv_disablecache(tvf); // desactivation du cache
  
  if ((ttv_getloadedfigtypes(tvf) & TTV_FILE_DTX)==TTV_FILE_DTX)
    type = TTV_FIND_LINE;
  else
    type = TTV_FIND_PATH;
  
/*  cl=ttv_levelise(tvf,tvf->INFO->LEVEL,ttv_getloadedfigtypes(tvf));
  freechain(cl);*/

  endlist=ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_TYPEALL,NULL) ;
  
  alllines=NULL;
  for (cl=endlist; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if((type & TTV_FIND_PATH) ==  TTV_FIND_PATH)
        {
          ttv_expfigsig(tvf,tvs,tvs->ROOT->INFO->LEVEL,tvf->INFO->LEVEL,
                        TTV_STS_CL_PJT, TTV_FILE_TTX);
        }
      else
        {
          ttv_expfigsig(tvf,tvs,tvs->ROOT->INFO->LEVEL,tvf->INFO->LEVEL,
                        TTV_STS_CLS_FED, TTV_FILE_DTX);
        }

      for (i=0; i<2; i++)
        {
           if ((type & TTV_FIND_PATH) ==  TTV_FIND_PATH)
             line=tvs->NODE[i].INPATH;
           else
             line=tvs->NODE[i].INLINE;

           while (line!=NULL)
             {
               if(!((((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
                     (line->FIG != tvf)) ||
                    (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
                     (line->FIG->INFO->LEVEL < tvs->ROOT->INFO->LEVEL))))
                 {
                   alllines=addchain(alllines, line);
                 }
               line=line->NEXT;
             }
        }
    }
  
  freechain(endlist);
  return alllines;
}

static int sortsig(const void *a0, const void *b0)
{
  char buf0[1024], buf1[1024];
  ttvsig_list *a=*(ttvsig_list **)a0;
  ttvsig_list *b=*(ttvsig_list **)b0;
  int res;
  
  ttv_getsigname(sort_ttvfig, buf0, a);
  ttv_getsigname(sort_ttvfig, buf1, b);
  res=strcmp(buf0, buf1);
  if (res<0) return -1;
  if (res>0) return 1;
  
  return 0;
}

ttvsig_list **ttv_sortsig(ttvfig_list *tvf, int *nbsig)
{
  long type;
  chain_list *endlist, *cl, *alllines;
  ttvline_list *line;
  int i, val;
  ttvsig_list **tab;

  ttv_disablecache(tvf); // desactivation du cache
  
  if ((ttv_getloadedfigtypes(tvf) & TTV_FILE_DTX)==TTV_FILE_DTX)
    type = TTV_FIND_LINE;
  else
    type = TTV_FIND_PATH;
  
  endlist=ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_TYPEALL,NULL) ;
  *nbsig=val=countchain(endlist);
  if (val==0) return NULL;
  tab=mbkalloc(sizeof(ttvsig_list *)*val);
  for (i=0, cl=endlist; i<val; i++, cl=cl->NEXT)
     tab[i]=cl->DATA;

  sort_ttvfig=tvf;
  qsort(tab, val, sizeof(ttvline_list *), sortsig);
  freechain(endlist);
  return tab;
}

//#define SSTA_DEBUG_SORT
ttvline_list **ttv_sortlines(ttvfig_list *tvf, int *nblines, int *key)
{
  
  chain_list *cl, *alllines;
  ttvline_list *line;
  int i, keyi=0;
  long val=0;
  ttvline_list **tab;
  char buf0[1024];
        
  alllines=ttv_getalllines(tvf);
  *nblines=val=countchain(alllines);
  if (val==0) return NULL;
  tab=mbkalloc(sizeof(ttvline_list *)*val);
  for (i=0, cl=alllines; i<val; i++, cl=cl->NEXT)
     tab[i]=cl->DATA;

  sort_ttvfig=tvf;
  qsort(tab, val, sizeof(ttvline_list *), sortlines);
  freechain(alllines);
#ifdef SSTA_DEBUG_SORT
  {
    FILE *f;
    f=fopen("order.txt","w");
    for (i=0; i<val; i++)
      {
        ttv_getsigname(tvf, buf0, tab[i]->NODE->ROOT);
        fprintf(f, "%s %x %x", buf0, masked_node_type(tab[i]->NODE->TYPE), masked_sig_type(tab[i]->NODE->ROOT->TYPE));
        ttv_getsigname(tvf, buf0, tab[i]->ROOT->ROOT);
        fprintf(f, " %s %x %x", buf0, masked_node_type(tab[i]->ROOT->TYPE), masked_sig_type(tab[i]->ROOT->ROOT->TYPE));

        fprintf(f," %x\n", masked_line_type(tab[i]->TYPE));
      }
    fclose(f);
    
  }
  exit(15);
#endif
  for (i=0; i<val; i++)
    {
      ttv_getsigname(tvf, buf0, tab[i]->NODE->ROOT);
      keyi+=mbk_sumchar(buf0);
      keyi+=masked_node_type(tab[i]->NODE->TYPE)*13+masked_sig_type(tab[i]->NODE->ROOT->TYPE)*21;
      ttv_getsigname(tvf, buf0, tab[i]->ROOT->ROOT);
      keyi+=mbk_sumchar(buf0)*17;
      keyi+=masked_node_type(tab[i]->ROOT->TYPE)*23+masked_sig_type(tab[i]->ROOT->ROOT->TYPE)*11;
      keyi+=masked_line_type(tab[i]->TYPE)*103;
    }
  *key=keyi;
  return tab;
}

void ttv_sendttvdelays(ttvfig_list *tvf, int ctk, int nbpvt)
{
  ttvline_list **tab;
  ttvsig_list **tabsig;
  int nb, i, ok, total, start, max, nbsig;
  ssta_entry *se;
  char buf[16024], name[1024], *res;
  int *inttab, *desttab, key;
  float *capas;
  
  res=mbk_commtalk("ask:resdir", -1); ok=(strcmp(res, "NULL")!=0);
  if (ok)
  {
    sprintf(name, "%s/%s-%u-%u.ssta.%sstored", res, tvf->INFO->FIGNAME, mbk_get_montecarlo_global_seed_init_saved(), eqt_getinitseed(),ctk?"ctk.":"");
  }
  free(res);
  if (ok)
  {
  tab=ttv_sortlines(tvf, &nb, &key);
  tabsig=ttv_sortsig(tvf, &nbsig);

  se=(ssta_entry *)mbkalloc(total=(sizeof(ssta_entry_header)+sizeof(ssta_entry_values)*nb));
  inttab=(int *)se;
  se->header.nb=nb;
  se->header.nbsig=nbsig;
  se->header.key=key;
  se->header.nbpvt=nbpvt;
  se->header.globalseed=mbk_get_montecarlo_global_seed_init_saved();
  se->header.mainseed=eqt_getinitseed();
  for (i=0; i<nb; i++)
  {
    se->values[i].delaymax=ttv_getdelaymax(tab[i]);
    se->values[i].delaymin=ttv_getdelaymin(tab[i]);
    se->values[i].slopemax=ttv_getslopemax(tab[i]);
    se->values[i].slopemin=ttv_getslopemin(tab[i]);
  }

  sprintf(buf, "ask:getsetflag:%s", name);
  res=mbk_commtalk(buf, -1); ok=(strcmp(res, "ok")==0); free(res);
  if (ok)
    {
      sprintf(buf, "ask:createfile:%s", name);
      res=mbk_commtalk(buf, -1); ok=(strcmp(res, "ok")==0); free(res);
      if (ok)
      {
        strcpy(buf,"ask:writefile32:");
        start=0;
        desttab=(int *)&buf[16]; // aligne
        while (ok && total>0)
        {
           if (total>16000) max=16000;
           else max=total;
           for (i=0; i<max/4; i++)
             desttab[i]=htonl(inttab[start+i]);
           res=mbk_commtalk(buf, max+16); ok=(strcmp(res, "ok")==0); free(res);
           total-=max;
           start+=max/4;
        }
        start=0;
        total=sizeof(float)*nbsig;
        while (ok && total>0)
        {
           if (total>16000) max=16000;
           else max=total;
           for (i=0; i<max/4; i++)
           {
             *(float *)&desttab[i]=tabsig[start+i]->CAPA;
             desttab[i]=htonl(desttab[i]);
           }
           res=mbk_commtalk(buf, max+16); ok=(strcmp(res, "ok")==0); free(res);
           total-=max;
           start+=max/4;
        }
        if (ok)
        {
           res=mbk_commtalk("ask:closefile", -1);
           free(res);
        }
      }
      if (!ok)
        avt_errmsg( MBK_ERRMSG, "068", AVT_FATAL, name );
    }
    if (tab!=NULL) mbkfree(tab);
    if (tabsig!=NULL) mbkfree(tabsig);
  }
  sprintf(buf, "ask:unsetflag:%s", name);
  res=mbk_commtalk(buf, -1); ok=(strcmp(res, "ok")==0); free(res);
}


static int ssta_readfile(one_run_entry *ore, int headeronly)
{
  FILE *f;
  int i, ret=1;
  if ((f=mbkfopen(ore->filename, NULL, "r"))!=NULL)
    {
      if (fread(&ore->header, sizeof(ore->header), 1, f)==1)
        {
          ore->header.nb=ntohl(ore->header.nb);
          ore->header.nbsig=ntohl(ore->header.nbsig);
          ore->header.nbpvt=ntohl(ore->header.nbpvt);
          ore->header.key=ntohl(ore->header.key);
          ore->header.globalseed=ntohl(ore->header.globalseed);
          ore->header.mainseed=ntohl(ore->header.mainseed);
          ore->sev=NULL;
          if (!headeronly)
            {
              ore->sev=(ssta_entry_values *)mbkalloc(sizeof(ssta_entry_values)*ore->header.nb);
              if (fread(ore->sev, sizeof(ssta_entry_values), ore->header.nb, f)==(unsigned int)ore->header.nb)
                {
                  for (i=0; i<ore->header.nb; i++)
                    {
                      ore->sev[i].delaymin=ntohl(ore->sev[i].delaymin);
                      ore->sev[i].delaymax=ntohl(ore->sev[i].delaymax);
                      ore->sev[i].slopemin=ntohl(ore->sev[i].slopemin);
                      ore->sev[i].slopemax=ntohl(ore->sev[i].slopemax);
                    }
                  ore->sevsig=(float *)mbkalloc(sizeof(float)*ore->header.nbsig);
                  if (fread(ore->sevsig, sizeof(float), ore->header.nbsig, f)==(unsigned int)ore->header.nbsig)
                   {
                      int *temp=(int *)ore->sevsig;
                      for (i=0; i<ore->header.nbsig; i++)
                         *(int *)&ore->sevsig[i]=ntohl(temp[i]);
                      ret=0;
                   }
                  else
                   {
                      mbkfree(ore->sevsig);
                      mbkfree(ore->sev);
                      ore->sev=NULL;
                      ore->sevsig=NULL;
                   }

                }
              else
                {
                  mbkfree(ore->sev);
                  ore->sev=NULL;
                }
            }
          else
            ret=0;
        }
      fclose(f);
    }
  return ret;
}

int ttv_ssta_getline_index(ttvline_list *tvl)
{
  ptype_list *pt;
  if ((pt=getptype(tvl->USER, TTV_SSTA_LINE_INDEX))!=NULL)
    return (int)(long)pt->DATA;
  return -1;
}

static void ttv_ssta_setline_index(ttvline_list *tvl, int index)
{
  ptype_list *pt;
  if ((pt=getptype(tvl->USER, TTV_SSTA_LINE_INDEX))!=NULL)
    pt->DATA=(void *)(long)index;
  else
    tvl->USER=addptype(tvl->USER, TTV_SSTA_LINE_INDEX, (void *)(long)index);
}
int ttv_ssta_getsig_index(ttvsig_list *tvs)
{
  ptype_list *pt;
  if ((pt=getptype(tvs->USER, TTV_SSTA_SIG_INDEX))!=NULL)
    return (int)(long)pt->DATA;
  return -1;
}

static void ttv_ssta_setsig_index(ttvsig_list *tvs, int index)
{
  ptype_list *pt;
  if ((pt=getptype(tvs->USER, TTV_SSTA_SIG_INDEX))!=NULL)
    pt->DATA=(void *)(long)index;
  else
    tvs->USER=addptype(tvs->USER, TTV_SSTA_SIG_INDEX, (void *)(long)index);
}

static ssta_delays *ttv_ssta_getstore(ttvfig_list *tvf)
{
  ptype_list *pt;
  if ((pt=getptype(tvf->USER, TTV_SSTA_STORE))!=NULL)
    return (ssta_delays *)pt->DATA;
  return NULL;
}

int ttv_ssta_set_run_number(ttvfig_list *tvf, int num)
{
  ssta_delays *sd;
  if ((sd=ttv_ssta_getstore(tvf))!=NULL)
    {
      if (num>=0 && num<sd->nb_run && !sd->ore[num].error)
        {
          sd->cur_run=num;
          return 0;
        }
    }
  return 1;
}

int ttv_ssta_set_checkloadedstore(ttvfig_list *tvf)
{
  ssta_delays *sd;
  int ret=0, i, minnum=-1;
  if ((sd=ttv_ssta_getstore(tvf))!=NULL)
    {
      CURRENT_STORE=sd;
      if (!CURRENT_STORE->ore[CURRENT_STORE->cur_run].error && CURRENT_STORE->ore[CURRENT_STORE->cur_run].sev==NULL)
        {
          if (CURRENT_STORE->totalloaded+sizeof(ssta_entry_values)*CURRENT_STORE->nb_val+sizeof(float)*CURRENT_STORE->nb_sigval>V_FLOAT_TAB[__TTV_SSTA_CACHE_SIZE].VALUE)
          {
             for (i=0; i<CURRENT_STORE->nb_run; i++)
                if (!CURRENT_STORE->ore[i].error && CURRENT_STORE->ore[i].sev!=NULL
                    && (minnum==-1 || minnum>CURRENT_STORE->ore[i].cachenum)) minnum=i;
             if (minnum!=-1)
             {
                mbkfree(CURRENT_STORE->ore[minnum].sev);
                CURRENT_STORE->ore[minnum].sev=NULL;
             }
             CURRENT_STORE->cachenum++;
             CURRENT_STORE->ore[CURRENT_STORE->cur_run].cachenum=CURRENT_STORE->cachenum;
          }
          else
            CURRENT_STORE->totalloaded+=sizeof(ssta_entry_values)*CURRENT_STORE->nb_val+sizeof(float)*CURRENT_STORE->nb_sigval;
          if (ssta_readfile(&CURRENT_STORE->ore[CURRENT_STORE->cur_run], 0))
            {
              CURRENT_STORE->ore[CURRENT_STORE->cur_run].error=1;
              avt_errmsg( TTV_ERRMSG, "062", AVT_ERROR, CURRENT_STORE->ore[CURRENT_STORE->cur_run].filename);
              //printf("error reading ssta stored values in file '%s'\n",CURRENT_STORE->ore[CURRENT_STORE->cur_run].filename);
              ret=1;
            }
        }
    }
  else
    CURRENT_STORE=NULL;
  return ret;
}

ssta_entry_values *ttv_ssta_getvalues(ttvline_list *tvl)
{
  if (CURRENT_STORE!=NULL && !CURRENT_STORE->ore[CURRENT_STORE->cur_run].error && CURRENT_STORE->ore[CURRENT_STORE->cur_run].sev!=NULL)
    {
      int index;
      index=ttv_ssta_getline_index(tvl);
      if (index>=0)
        return &CURRENT_STORE->ore[CURRENT_STORE->cur_run].sev[index];
    }
  return NULL;
}

int ttv_ssta_getsigcapa(ttvsig_list *tvs, float *capa)
{
  if (CURRENT_STORE!=NULL && !CURRENT_STORE->ore[CURRENT_STORE->cur_run].error && CURRENT_STORE->ore[CURRENT_STORE->cur_run].sevsig!=NULL)
    {
      int index;
      index=ttv_ssta_getsig_index(tvs);
      if (index>=0)
        *capa=CURRENT_STORE->ore[CURRENT_STORE->cur_run].sevsig[index];
      return 1;
    }
  return 0;
}

int ttv_ssta_loadresults(ttvfig_list *tvf, chain_list *filenames, chain_list *order)
{
  ttvline_list **lines;
  int nblines, i, nbrun, res, j, nbsig, key;
  ssta_delays *sd;
  chain_list *cl;
  ptype_list *pt;
  ttvsig_list **sigs;

  if ((nbrun=countchain(filenames))==0) return 1;
  
  lines=ttv_sortlines(tvf, &nblines, &key);
  if (lines!=NULL)
  {
    for (i=0; i<nblines; i++)
      ttv_ssta_setline_index(lines[i], i);
    mbkfree(lines);
  }
  sigs=ttv_sortsig(tvf, &nbsig);
  if (sigs!=NULL)
  {
    for (i=0; i<nbsig; i++)
      ttv_ssta_setsig_index(sigs[i], i);
    mbkfree(sigs);
  }

  if ((sd=ttv_ssta_getstore(tvf))!=NULL)
    {
      for (j=0; j<sd->nb_run; j++)
        {
          mbkfree(sd->ore[j].filename);
          if (sd->ore[j].sev!=NULL) mbkfree(sd->ore[j].sev);
          if (sd->ore[j].sevsig!=NULL) mbkfree(sd->ore[j].sevsig);
        }
      mbkfree(sd->ore);
    }
  else
    {
      sd=mbkalloc(sizeof(ssta_delays));
      tvf->USER=addptype(tvf->USER, TTV_SSTA_STORE, sd);
    }
  
  sd->nb_run=nbrun;
  sd->nb_val=nblines;
  sd->nb_sigval=nbsig;
  sd->key=key;
  sd->cur_run=0;
  sd->ore=mbkalloc(sizeof(one_run_entry)*sd->nb_run);
  sd->totalloaded=0;
  sd->cachenum=1;

  for (i=0, cl=filenames; i<sd->nb_run; cl=cl->NEXT, i++)
    {
      sd->ore[i].filename=mbkstrdup((char *)cl->DATA);
      res=ssta_readfile(&sd->ore[i], 1);
      if (res!=0) sd->ore[i].error=1;
      else if (sd->ore[i].header.nb!=sd->nb_val) sd->ore[i].error=2;
      else if (sd->ore[i].header.nbsig!=sd->nb_sigval) sd->ore[i].error=3;
      else if (sd->ore[i].header.key!=sd->key) sd->ore[i].error=4;
      else sd->ore[i].error=0;
      sd->ore[i].cachenum=0;
    }

  if (order!=NULL)
    {
      unsigned int gs, ms;
      chain_list *ch;
      one_run_entry *ores;
      nbrun=countchain(order);
      ores=mbkalloc(sizeof(one_run_entry)*nbrun);
      for (cl=order, i=0; cl!=NULL; cl=cl->NEXT, i++)
        {
          ch=(chain_list *)cl->DATA;
          gs=(unsigned int)(unsigned long)ch->DATA;
          ms=(unsigned int)(unsigned long)ch->NEXT->DATA;
          for (j=0; j<sd->nb_run; j++)
            {
              if (ms==sd->ore[j].header.mainseed && gs==sd->ore[j].header.globalseed) break;
            }
          if (j>=sd->nb_run)
            {
              ores[i].filename=NULL, ores[i].error=3;
              ores[i].header.mainseed=ms;
              ores[i].header.globalseed=gs;
            }
          else
            {
              memcpy(&ores[i], &sd->ore[j], sizeof(one_run_entry));
              ores[i].filename=strdup(ores[i].filename);
            }
        }
      for (j=0; j<sd->nb_run; j++) mbkfree(sd->ore[j].filename);
      mbkfree(sd->ore);
      sd->nb_run=nbrun;
      sd->ore=ores;
    }

  for (i=0; i<sd->nb_run; i++)
    if (sd->ore[i].filename==NULL)
      avt_errmsg( TTV_ERRMSG, "063", AVT_WARNING, i+1, sd->ore[i].header.globalseed, sd->ore[i].header.mainseed );
    else
    {
      if (sd->ore[i].error==1)
        avt_errmsg( TTV_ERRMSG, "061", AVT_ERROR, sd->ore[i].filename, "read error");
      else if (sd->ore[i].error==2)
      {
        char buf[1024];
        sprintf(buf,"mismatching number of line values (UTD:%d, SSTA run:%d)", sd->nb_val, sd->ore[i].header.nb);
        avt_errmsg( TTV_ERRMSG, "061", AVT_ERROR, sd->ore[i].filename, buf);
      }
      else if (sd->ore[i].error==3)
      {
        char buf[1024];
        sprintf(buf,"mismatching number of capacitance values (UTD:%d, SSTA run:%d)", sd->nb_sigval, sd->ore[i].header.nbsig);
        avt_errmsg( TTV_ERRMSG, "061", AVT_ERROR, sd->ore[i].filename, buf);
      }
      else if (sd->ore[i].error>3)
      {
        char buf[1024];
        sprintf(buf,"mismatching UTD structure (keys: UTD:%x, SSTA run:%x)", sd->key, sd->ore[i].header.key);
        avt_errmsg( TTV_ERRMSG, "061", AVT_ERROR, sd->ore[i].filename, buf);
      }
     }

  return sd->nb_run;
}

ttv_line_ssta_info *ttv_ssta_get_ssta_info(ttvfig_list *tvf)
{
  ptype_list *pt;
  if ((pt=getptype(tvf->USER, TTV_SSTA_STORE_STATS))!=NULL)
    return (ttv_line_ssta_info *)pt->DATA;
  return NULL;
}

int ttv_ssta_buildlinestat(ttvfig_list *tvf)
{
  chain_list *alllines, *cl;
  ttv_line_ssta_info *tlsi;
  ttvline_list *tvl;
  ptype_list *pt;
  int num, idx, nb, i, realnum, start;
  ssta_delays *sd;
  float val;
  ssta_entry_values *sev;

  if ((sd=ttv_ssta_getstore(tvf))!=NULL)
    {
      alllines=ttv_getalllines(tvf);
      nb=countchain(alllines);
      if ((tlsi=ttv_ssta_get_ssta_info(tvf))!=NULL)
        {
          mbkfree(tlsi);
          tvf->USER=delptype(tvf->USER, TTV_SSTA_STORE_STATS);
        }
      
      tlsi=mbkalloc(sizeof(ttv_line_ssta_info)*nb);
      tvf->USER=addptype(tvf->USER, TTV_SSTA_STORE_STATS, tlsi);

      for(i=0; i<nb; i++)
        {
          tlsi[i].delaymax.min=tlsi[i].delaymin.min=1;
          tlsi[i].delaymax.max=tlsi[i].delaymin.max=-1;
          tlsi[i].delaymax.moy=tlsi[i].delaymin.moy=0;
          tlsi[i].delaymax.var=tlsi[i].delaymin.var=0;
        }
          
      realnum=0;
      num=start=sd->cur_run;
      // pass 1 compute mean
      do 
        {
          ttv_ssta_set_run_number(tvf, num);
          if (ttv_ssta_set_checkloadedstore(tvf)==0)
            {
              for (cl=alllines; cl!=NULL; cl=cl->NEXT)
                {
                  tvl=(ttvline_list *)cl->DATA;
                  if ((sev=ttv_ssta_getvalues(tvl))!=NULL)
                    {
                      idx=ttv_ssta_getline_index(tvl);
                      val=_LONG_TO_DOUBLE(sev->delaymin);                      
                      if (val<tlsi[idx].delaymin.min) tlsi[idx].delaymin.min=val;
                      if (val>tlsi[idx].delaymin.max) tlsi[idx].delaymin.max=val;
                      tlsi[idx].delaymin.moy+=val;
                      val=_LONG_TO_DOUBLE(sev->delaymax);                      
                      if (val<tlsi[idx].delaymax.min) tlsi[idx].delaymax.min=val;
                      if (val>tlsi[idx].delaymax.max) tlsi[idx].delaymax.max=val;
                      tlsi[idx].delaymax.moy+=val;
                    }
                }
              realnum++;
            }
          num++;
          if (num>=sd->nb_run) num=0;
        } while (num!=start);

      for(i=0; i<nb; i++)
        {
          tlsi[i].delaymin.moy/=realnum;
          tlsi[i].delaymax.moy/=realnum;
        }

      // pass 2 compute variance
      do 
        {
          num--;
          if (num<0) num=sd->nb_run-1;
          ttv_ssta_set_run_number(tvf, num);
          if (ttv_ssta_set_checkloadedstore(tvf)==0)
            {
              for (cl=alllines; cl!=NULL; cl=cl->NEXT)
                {
                  tvl=(ttvline_list *)cl->DATA;
                  if ((sev=ttv_ssta_getvalues(tvl))!=NULL)
                    {
                      idx=ttv_ssta_getline_index(tvl);
                      val=_LONG_TO_DOUBLE(sev->delaymin);
                      tlsi[idx].delaymin.var+=pow(tlsi[idx].delaymin.moy-val, 2);
                      val=_LONG_TO_DOUBLE(sev->delaymax);                      
                      tlsi[idx].delaymax.var+=pow(tlsi[idx].delaymax.moy-val, 2);
                    }
                }
            }
        } while (num!=start);

      for(i=0; i<nb; i++)
        {
          tlsi[i].delaymin.var/=realnum;
          tlsi[i].delaymax.var/=realnum;
        }

      freechain(alllines);
      return 0;
    }
  return 1;
}
