#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>


#include MUT_H
#include STB_H
#include RCN_H
#include TRC_H
#include TTV_H

#define STB_CTK_LAST_IT_INFO 0xfab70626

typedef struct
{
  long min, max;
} last_it_info;

static HeapAlloc myheap;

void stb_ctk_drive_iteration_report_save_last_iteration_info(stbfig_list *sb)
{
  long delaymin, delaymax, level;
  last_it_info *lii;
  chain_list *cl;
  ttvevent_list *event;
  ttvline_list *line;

  if (V_INT_TAB[__STB_CTK_MAX_REPORTED_SIGNAL].VALUE<=0) return;

  CreateHeap(sizeof(last_it_info), 8096, &myheap);

  for (cl=sb->NODE; cl!=NULL; cl=cl->NEXT)
    {
      event=(ttvevent_list *)cl->DATA;

      if ((sb->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
        level = sb->FIG->INFO->LEVEL;
      else
        level = event->ROOT->ROOT->INFO->LEVEL;

      if (sb->GRAPH == STB_RED_GRAPH)
        {
          ttv_expfigsig (sb->FIG, event->ROOT, level, sb->FIG->INFO->LEVEL,
                         TTV_STS_CL_PJT, TTV_FILE_TTX);
          line = event->INPATH;
        }
      else
        {
          ttv_expfigsig (sb->FIG, event->ROOT, level, sb->FIG->INFO->LEVEL,
                         TTV_STS_CLS_FED, TTV_FILE_DTX);
          line = event->INLINE;
        }
      
      
      for (; line; line = line->NEXT)
        {
          if (((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
              (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
               (line->FIG != sb->FIG)) ||
              (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
               (line->FIG != sb->FIG) &&
               ((sb->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)))
            continue;
          
          lii=AddHeapItem(&myheap);
          delaymin = ttv_getdelaymin (line);
          delaymax = ttv_getdelaymax (line);
          if (delaymin == TTV_NOTIME) delaymin = delaymax;
          if (delaymax == TTV_NOTIME) delaymax = delaymin;
          lii->min=delaymin;
          lii->max=delaymax;
          line->USER=addptype(line->USER, STB_CTK_LAST_IT_INFO, lii);
        }
    }
}

typedef struct
{
  long min, max;
  long oldmin, oldmax;
  ttvsig_list *tvs;
} diff_info;

static int stb_ctk_drive_iteration_sort_max(const void *a, const void *b)
{
  diff_info *dia, *dib;
  dia=(diff_info *)a;
  dib=(diff_info *)b;
  if (dia->max-dia->oldmax<dib->max-dib->oldmax) return 1;
  if (dia->max-dia->oldmax>dib->max-dib->oldmax) return -1;
  return 0;
}
static int stb_ctk_drive_iteration_sort_min(const void *a, const void *b)
{
  diff_info *dia, *dib;
  dia=(diff_info *)a;
  dib=(diff_info *)b;
  if (dia->oldmin-dia->min<dib->oldmin-dib->min) return 1;
  if (dia->oldmin-dia->min>dib->oldmin-dib->min) return -1;
  return 0;
}
static void stb_ctk_drive_iteration_report_min_max_change_board(FILE *f, stbfig_list *sb, diff_info *di, int nb, int max)
{
  Board *b;
  char buf[1024];
  int i;
  long v1, v2;

  b=Board_CreateBoard();
  Board_SetSize(b, 0, 10, 'l');
  Board_SetSize(b, 1, 10, 'l');
  Board_SetSize(b, 2, 7, 'r');
  Board_SetSize(b, 3, 7, 'r');
  Board_SetSize(b, 4, 5, 'r');

  Board_NewLine(b);
  Board_SetValue(b, 0, "Net Name");
  Board_SetValue(b, 1, "Node Name");
  Board_SetValue(b, 2, "Old Value");
  Board_SetValue(b, 3, "New value");
  Board_SetValue(b, 4, "Ratio");

  Board_NewSeparation(b);

  for (i=0; i<nb && i<V_INT_TAB[__STB_CTK_MAX_REPORTED_SIGNAL].VALUE; i++)
    {
      if (max) v1=di[i].oldmax, v2=di[i].max;
      else v1=di[i].oldmin, v2=di[i].min;
      
      if (v1-v2!=0)
        {
          Board_NewLine(b);
          Board_SetValue(b, 0, ttv_getnetname(sb->FIG, buf, di[i].tvs));
          Board_SetValue(b, 1, ttv_getsigname(sb->FIG, buf, di[i].tvs));
          sprintf(buf, "%.1f", v1/TTV_UNIT);
          Board_SetValue(b, 2, buf);
          sprintf(buf, "%.1f", v2/TTV_UNIT);
          Board_SetValue(b, 3, buf);
          sprintf(buf, "%+.1f%%", (v2-v1)*100.0/(float)v1);
          Board_SetValue(b, 4, buf);
        }
    }
  Board_Display(f, b, "");
  Board_FreeBoard(b);
}

static void stb_ctk_drive_iteration_report_min_max_change(FILE *f, stbfig_list *sb)
{
  long delaymin, delaymax, level;
  last_it_info *lii;
  chain_list *cl;
  ttvevent_list *event;
  ptype_list *pt;
  diff_info *di, *odi;
  int nb, i;
  ttvline_list *line;

  nb=(countchain(sb->NODE)/2)+1;
  di=(diff_info *)mbkalloc(sizeof(diff_info)*nb);
  nb=0;

  for (cl=sb->NODE; cl!=NULL; cl=cl->NEXT)
    {
      event=(ttvevent_list *)cl->DATA;

      if ((sb->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
        level = sb->FIG->INFO->LEVEL;
      else
        level = event->ROOT->ROOT->INFO->LEVEL;

      if (sb->GRAPH == STB_RED_GRAPH)
        {
          ttv_expfigsig (sb->FIG, event->ROOT, level, sb->FIG->INFO->LEVEL,
                         TTV_STS_CL_PJT, TTV_FILE_TTX);
          line = event->INPATH;
        }
      else
        {
          ttv_expfigsig (sb->FIG, event->ROOT, level, sb->FIG->INFO->LEVEL,
                         TTV_STS_CLS_FED, TTV_FILE_DTX);
          line = event->INLINE;
        }
      
      
      for (; line; line = line->NEXT)
        {
          if (((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
              (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
               (line->FIG != sb->FIG)) ||
              (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
               (line->FIG != sb->FIG) &&
               ((sb->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)))
            continue;

          delaymin = ttv_getdelaymin (line);
          delaymax = ttv_getdelaymax (line);
          if (delaymin == TTV_NOTIME) delaymin = delaymax;
          if (delaymax == TTV_NOTIME) delaymax = delaymin;

          if ((pt=getptype(line->USER, STB_CTK_LAST_IT_INFO))!=NULL)
            {
              lii=(last_it_info *)pt->DATA;
              if ((pt=getptype(event->ROOT->USER, STB_CTK_LAST_IT_INFO))==NULL)
                {
                  odi=&di[nb++];
                  odi->tvs=event->ROOT;
                  odi->oldmin=lii->min;
                  odi->oldmax=lii->max;
                  odi->min=delaymin;
                  odi->max=delaymax;
                  event->ROOT->USER=addptype(event->ROOT->USER, STB_CTK_LAST_IT_INFO, odi);
                }
              else
                {
                  odi=(diff_info *)pt->DATA;
                  if (odi->oldmin-odi->min>lii->min-delaymin)
                    {
                      odi->oldmin=lii->min;
                      odi->min=delaymin;
                    }
                  if (odi->max-odi->oldmax>delaymax-lii->max)
                    {
                      odi->oldmax=lii->max;
                      odi->max=delaymax;
                    }
                }
              line->USER=delptype(line->USER, STB_CTK_LAST_IT_INFO);
            }
        }
    }
  DeleteHeap(&myheap);

  fprintf(f,"\n# --------- Most affected MAX delays -----------------\n\n");

  qsort(di, nb, sizeof(diff_info),  stb_ctk_drive_iteration_sort_max);

  stb_ctk_drive_iteration_report_min_max_change_board(f, sb, di, nb, 1);

  fprintf(f,"\n# --------- Most affected MIN delays -----------------\n\n");

  qsort(di, nb, sizeof(diff_info),  stb_ctk_drive_iteration_sort_min);

  stb_ctk_drive_iteration_report_min_max_change_board(f, sb, di, nb, 0);

  for (i=0; i<nb; i++)
    {
      di[i].tvs->USER=testanddelptype(di[i].tvs->USER, STB_CTK_LAST_IT_INFO);
    }

  mbkfree(di);
}

typedef struct
{
  long setup, hold;
  ttvsig_list *tvs;
} sh_info;

static int stb_ctk_drive_iteration_sort_setup(const void *a, const void *b)
{
  sh_info *sha, *shb;
  sha=(sh_info *)a;
  shb=(sh_info *)b;
  if (sha->setup==TTV_NOTIME && shb->setup==TTV_NOTIME) return 1;
  if (sha->setup==TTV_NOTIME) return 1;
  if (shb->setup==TTV_NOTIME) return -1;
  if (sha->setup<shb->setup) return -1;
  if (sha->setup>shb->setup) return 1;
  return 0;
}

static int stb_ctk_drive_iteration_sort_hold(const void *a, const void *b)
{
  sh_info *sha, *shb;
  sha=(sh_info *)a;
  shb=(sh_info *)b;
  if (sha->hold==TTV_NOTIME && shb->hold==TTV_NOTIME) return 1;
  if (sha->hold==TTV_NOTIME) return 1;
  if (shb->hold==TTV_NOTIME) return -1;
  if (sha->hold<shb->hold) return -1;
  if (sha->hold>shb->hold) return 1;
  return 0;
}

static void stb_ctk_drive_iteration_report_slack_board(FILE *f, stbfig_list *sb, sh_info *shi, int nb, int setup)
{
  Board *b;
  char buf[1024];
  int i;
  long v1;

  b=Board_CreateBoard();
  Board_SetSize(b, 0, 10, 'l');
  Board_SetSize(b, 1, 10, 'l');
  Board_SetSize(b, 2, 7, 'r');

  Board_NewLine(b);
  Board_SetValue(b, 0, "Net Name");
  Board_SetValue(b, 1, "Node Name");
  Board_SetValue(b, 2, "Slack");

  Board_NewSeparation(b);

  for (i=0; i<nb && i<V_INT_TAB[__STB_CTK_MAX_REPORTED_SIGNAL].VALUE; i++)
    {
      if (setup) v1=shi[i].setup;
      else v1=shi[i].hold;
      
      if (v1!=TTV_NOTIME)
        {
          Board_NewLine(b);
          Board_SetValue(b, 0, ttv_getnetname(sb->FIG, buf, shi[i].tvs));
          Board_SetValue(b, 1, ttv_getsigname(sb->FIG, buf, shi[i].tvs));
          sprintf(buf, "%.1f", v1/TTV_UNIT);
          Board_SetValue(b, 2, buf);
        }
    }
  Board_Display(f, b, "");
  Board_FreeBoard(b);
}



void stb_ctk_drive_iteration_report_slacks(FILE *f,stbfig_list *sb)
{
  stbdebug_list *debug ;
  stbdebug_list *deb ;
  long maxs, maxh;
  chain_list *cl;
  int nb, i;
  sh_info *shi;
  ttvevent_list *event;

  nb=(countchain(sb->NODE)/2)+1;
  shi=(sh_info *)mbkalloc(sizeof(sh_info)*nb);
  nb=0;

  for (cl=sb->NODE; cl!=NULL; cl=cl->NEXT)
    {
      event=(ttvevent_list *)cl->DATA;

      if (getptype(event->ROOT->USER, STB_CTK_LAST_IT_INFO)==NULL)
        {
          debug = stb_debugstberror(sb,event->ROOT,(long)0, 0);

          maxs=TTV_NOTIME, maxh=TTV_NOTIME;
          for(deb = debug ; deb ; deb = deb->NEXT) 
            {
              if (deb->SETUP != STB_NO_TIME && (maxs==TTV_NOTIME || maxs>deb->SETUP))
                maxs=deb->SETUP;
              if (deb->HOLD != STB_NO_TIME && (maxh==TTV_NOTIME || maxh>deb->HOLD))
                maxh=deb->HOLD;
            }
          stb_freestbdebuglist(debug) ;
          shi[nb].tvs=event->ROOT;
          shi[nb].setup=maxs;
          shi[nb].hold=maxh;
          event->ROOT->USER=addptype(event->ROOT->USER, STB_CTK_LAST_IT_INFO, &shi[nb]);
          nb++;
        }
    }

  fprintf(f,"\n# --------- Lowest SETUP slacks -----------------\n\n");
  
  qsort(shi, nb, sizeof(sh_info),  stb_ctk_drive_iteration_sort_setup);

  stb_ctk_drive_iteration_report_slack_board(f, sb, shi, nb, 1);

  fprintf(f,"\n# --------- Lowest HOLD slacks -----------------\n\n");
  
  qsort(shi, nb, sizeof(sh_info),  stb_ctk_drive_iteration_sort_hold);

  stb_ctk_drive_iteration_report_slack_board(f, sb, shi, nb, 0);


  for (i=0; i<nb; i++)
    {
      shi[i].tvs->USER=testanddelptype(shi[i].tvs->USER, STB_CTK_LAST_IT_INFO);
    }
  
  mbkfree(shi);
}

typedef struct
{
  ttvsig_list *tvs;
  int T, A, N, I, C, P;
} pn_info;

static int stb_ctk_drive_iteration_sort_noise(const void *a, const void *b)
{
  pn_info *pna, *pnb;
  pna=(pn_info *)a;
  pnb=(pn_info *)b;
  if (pna->T<pnb->T) return 1;
  if (pna->T>pnb->T) return -1;
  return 0;
}

static void stb_ctk_drive_iteration_report_noise_board(FILE *f, stbfig_list *sb, pn_info *pni, int nb)
{
  Board *b;
  char buf[1024];
  int i;

  b=Board_CreateBoard();
  Board_SetSize(b, 0, 10, 'l');
  Board_SetSize(b, 1, 10, 'l');
  Board_SetSize(b, 2, 7, 'r');
  Board_SetSize(b, 3, 7, 'r');
  Board_SetSize(b, 4, 7, 'r');
  Board_SetSize(b, 5, 7, 'r');
  Board_SetSize(b, 6, 7, 'r');

  Board_NewLine(b);
  Board_SetValue(b, 0, "Net Name");
  Board_SetValue(b, 1, "Node Name");
  Board_SetValue(b, 2, "Total");
  Board_SetValue(b, 3, "Noise");
  Board_SetValue(b, 4, "Interval");
  Board_SetValue(b, 5, "Activity");
  Board_SetValue(b, 6, "Xtalk");
  Board_SetValue(b, 7, "Proba");

  Board_NewSeparation(b);

  for (i=0; i<nb && i<V_INT_TAB[__STB_CTK_MAX_REPORTED_SIGNAL].VALUE; i++)
    {
      if (pni[i].T>=0)
        {
          Board_NewLine(b);
          Board_SetValue(b, 0, ttv_getnetname(sb->FIG, buf, pni[i].tvs));
          Board_SetValue(b, 1, ttv_getsigname(sb->FIG, buf, pni[i].tvs));
          sprintf(buf, "%d%%", pni[i].T);
          Board_SetValue(b, 2, buf);
          sprintf(buf, "%d", pni[i].N);
          Board_SetValue(b, 3, buf);
          sprintf(buf, "%d", pni[i].I);
          Board_SetValue(b, 4, buf);
          sprintf(buf, "%d", pni[i].A);
          Board_SetValue(b, 5, buf);
          sprintf(buf, "%d", pni[i].C);
          Board_SetValue(b, 6, buf);
          sprintf(buf, "%d", pni[i].P);
          Board_SetValue(b, 7, buf);
        }
    }
  Board_Display(f, b, "");
  Board_FreeBoard(b);
}

static void ctk_MarkCtkStatNodeFromEvent( stb_ctk_stat *stat )
{
  int n ;
  
  for( n=0 ; n<stat->NBELEM ; n++ ) 
    {
      stat->TAB[n].NODE->USER=addptype(stat->TAB[n].NODE->USER, STB_CTK_LAST_IT_INFO, (void *)(long)n);
    }
}

void stb_ctk_drive_iteration_report_noise(FILE *f, stbfig_list *sb)
{
  stb_ctk_stat *stat;
  int idx, t, nb, i;
  chain_list *cl;
  ttvevent_list *event;
  pn_info *opni, *pni;
  ptype_list *pt;

  fprintf(f,"\n# --------- Highest SCORES -----------------\n\n");

  if(stb_ctk_get_stat(sb)!=NULL) stb_ctk_clean_stat( sb);
//  ttv_init_stm(stbfig->FIG);
//  CtkMutexFree( stbfig->FIG );
//  CtkMutexInit( stbfig->FIG );
  stb_ctk_fill_stat(sb, 0);

  if ((stat = stb_ctk_get_stat(sb))!=NULL)
    {

      nb=(countchain(sb->NODE)/2)+1;
      pni=(pn_info *)mbkalloc(sizeof(pn_info)*nb);
      nb=0;
      
      ctk_MarkCtkStatNodeFromEvent(stat);

      for (cl=sb->NODE; cl!=NULL; cl=cl->NEXT)
        {
          event=(ttvevent_list *)cl->DATA;

          if ((pt=getptype(event->ROOT->USER, STB_CTK_LAST_IT_INFO))!=NULL) opni=(pn_info *)pt->DATA;
          else
            {
              opni=&pni[nb++];
              opni->T=-1;
              opni->tvs=event->ROOT;
              event->ROOT->USER=addptype(event->ROOT->USER, STB_CTK_LAST_IT_INFO, opni);
            }
                   
          if ((pt=getptype(event->USER, STB_CTK_LAST_IT_INFO))!=NULL)
            {
              idx=(int)(long)pt->DATA;
              event->USER=delptype(event->USER, STB_CTK_LAST_IT_INFO);
            }
          else idx=-1;

          if (idx>=0)
            {
              if ((t=stb_ctk_get_score_total( &stat->TAB[idx] ))>opni->T)
                {
                  opni->N=stb_ctk_get_score_noise( &stat->TAB[idx] );
                  opni->I=stb_ctk_get_score_interval( &stat->TAB[idx] );
                  opni->A=stb_ctk_get_score_activity( &stat->TAB[idx] );
                  opni->T=t;
                  opni->C=stb_ctk_get_score_ctk( &stat->TAB[idx] );
                  opni->P=stb_ctk_get_score_probability( &stat->TAB[idx] );
                }
            }          
        }

      qsort(pni, nb, sizeof(pn_info), stb_ctk_drive_iteration_sort_noise);
      
      stb_ctk_drive_iteration_report_noise_board(f, sb, pni, nb);

      for (i=0; i<nb; i++)
        {
          pni[i].tvs->USER=testanddelptype(pni[i].tvs->USER, STB_CTK_LAST_IT_INFO);
        }
      
      mbkfree(pni);
      stb_ctk_clean_stat( sb);
    }
}




void stb_ctk_drive_iteration_report(stbfig_list *sb, int it_num)
{
  FILE *f;
  char buf[1024];

  if (V_INT_TAB[__STB_CTK_MAX_REPORTED_SIGNAL].VALUE>0)
    {
      sprintf(buf,"%s.iteration#%02d.log", sb->FIG->INFO->FIGNAME, it_num);
      
      if ((f=fopen(buf, "w"))!=NULL)
        {
          sprintf(buf,"Crosstalk iteration %d information", it_num);
          avt_printExecInfo(f, "#", buf, "");
          
          if (it_num>0) stb_ctk_drive_iteration_report_min_max_change(f, sb);
          
          stb_ctk_drive_iteration_report_slacks(f, sb);
          
          if (it_num>0) stb_ctk_drive_iteration_report_noise(f, sb);
          
          fclose(f);
        }
    }
}
