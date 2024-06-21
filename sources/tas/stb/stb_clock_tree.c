#include <stdlib.h>

#include MUT_H
#include STB_H
#include "stb_clocks.h"
#include "stb_clock_tree.h"

static void stb_clock_tree_mark_diverging_node(stbfig_list *stbfig, ttvevent_list *event)
{
 stbnode *cknode;
 ttvevent_list *ckevent;
 ttvline_list *line;
 char level;
     
 if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
  level = stbfig->FIG->INFO->LEVEL;
 else
  level = event->ROOT->ROOT->INFO->LEVEL;

 if (stbfig->GRAPH == STB_RED_GRAPH)
  {
   ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                  TTV_STS_CL_PJT, TTV_FILE_TTX);
   line = event->INPATH;
  }
 else
  {
   ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                  TTV_STS_CLS_FED, TTV_FILE_DTX);
   line = event->INLINE;
  }

 for (; line; line = line->NEXT)
  {
   if (((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
       (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
        (line->FIG != stbfig->FIG)) ||
       (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
        (line->FIG != stbfig->FIG) &&
        ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)))
    continue;

   ckevent = line->NODE;
   if (getptype(ckevent->USER, TTV_NODE_MARQUE)!=NULL)
     {
       if ((cknode = stb_getstbnode (ckevent))!=NULL)
         cknode->FLAG|=STB_NODE_DIVERGING;
     }
   else ckevent->USER=addptype(ckevent->USER, TTV_NODE_MARQUE, 0);
  }
}

void stb_free_clock_tree_info(ttvevent_list *node)
{
  node->USER=testanddelptype(node->USER, STB_CLOCK_TREE_INFO);
}

void stb_clock_tree_mark_divergence(stbfig_list *stbfig)
{
  chain_list *cl;
  ttvevent_list *node;
  ptype_list *pt;
  for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT)
    {
      node=(ttvevent_list *)cl->DATA;
      stb_clock_tree_mark_diverging_node(stbfig, node);
    }
  for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT)
    {
      node=(ttvevent_list *)cl->DATA;
      node->USER=testanddelptype(node->USER, TTV_NODE_MARQUE);
      if ((pt=getptype(node->USER, STB_CLOCK_TREE_INFO))!=NULL)
      {
        DelHeapItem(&stbfig->CLOCK_TREE_HEAP, pt->DATA);
        stb_free_clock_tree_info(node);
      }
    }
}

static inline stb_clock_tree *stb_get_clock_tree(ttvevent_list *node)
{
  ptype_list *pt;

  if (node==NULL) return NULL;
  if ((pt=getptype(node->USER, STB_CLOCK_TREE_INFO))!=NULL)
    return (stb_clock_tree *)pt->DATA;
  return NULL;
}


static stb_clock_tree *stb_give_clock_tree(stbfig_list *sbf, ttvevent_list *node)
{
  stb_clock_tree *sct;

  if ((sct=stb_get_clock_tree(node))==NULL)
    {
      sct=(stb_clock_tree *)AddHeapItem(&sbf->CLOCK_TREE_HEAP);
      sct->level=-1;
      sct->delaymin=sct->delaymax=TTV_NOTIME;
      sct->previous_divergence=sct->previous_divergence_hz=NULL;
      node->USER=addptype(node->USER, STB_CLOCK_TREE_INFO, sct);
    }
  return sct;
}


static int importantnode(stbnode *node)
{
  if (node!=NULL &&
       ((node->FLAG & STB_NODE_DIVERGING)!=0
       || (node->EVENT->ROOT->TYPE & TTV_SIG_Q)!=0)) return 1;
  return 0;
}

void stb_mark_path_diverging_node(stbfig_list *sbf, ttvpath_list *pth)
{
  long delay;
  ttvcritic_list *tc;
  ttvevent_list *prev;
  stb_clock_tree *sct;
  stbnode *node;
  int level=0;
  
  delay=0;
  prev=NULL;

  for (tc=pth->CRITIC; tc!=NULL; tc=tc->NEXT)
    {
      delay+=tc->DELAY;
      node=stb_getstbnode (tc->NODE);
      if (tc==pth->CRITIC || tc->NEXT==NULL || importantnode(node)!=0)
        {
          sct=stb_give_clock_tree(sbf, tc->NODE);
          sct->level=level++;
          if (tc->NEXT==NULL && (pth->TYPE & TTV_FIND_HZ)==TTV_FIND_HZ) sct->previous_divergence_hz=prev;
          else sct->previous_divergence=prev;
          prev=tc->NODE;
          if ((pth->TYPE & TTV_FIND_MAX)==TTV_FIND_MAX)
            {
              if (sct->delaymax==TTV_NOTIME || sct->delaymax<delay)
                {
                  sct->delaymax=delay;
                  if (sct->delaymin==TTV_NOTIME) sct->delaymin=delay;
                }
            }
          else
            {
              if (sct->delaymin==TTV_NOTIME || sct->delaymin>delay)
                {
                  sct->delaymin=delay;
                  if (sct->delaymax==TTV_NOTIME || sct->delaymax<delay) sct->delaymax=delay;
                }
            }
        }
    }
}

static stb_clock_tree *stb_find_common_node(ttvevent_list *a, ttvevent_list *b, int maxdepth, ttvevent_list **common_node, int hza, int hzb)
{
  stb_clock_tree *sct_a, *sct_b;
  ttvevent_list *sct_up;
  int depth=0, firsta=1, firstb=1;

  if (maxdepth==0) return NULL;

  *common_node=a;
  if ((sct_a=stb_get_clock_tree(a))==NULL) return NULL;
  if ((sct_b=stb_get_clock_tree(b))==NULL) return NULL;

  while (depth<maxdepth && sct_a!=NULL && sct_b!=NULL && sct_a!=sct_b)
    {
      if (sct_a->level==sct_b->level)
        {
          if (firsta && hza) sct_up=sct_a->previous_divergence_hz;
          else sct_up=sct_a->previous_divergence;
          *common_node=sct_up;
          sct_a=stb_get_clock_tree(sct_up);
          if (firstb && hzb) sct_up=sct_b->previous_divergence_hz;
          else sct_up=sct_b->previous_divergence;
          sct_b=stb_get_clock_tree(sct_up);
          firsta=0;
          firstb=0;
        }
      else if (sct_a->level<sct_b->level)
        {
          if (firstb && hzb) sct_up=sct_b->previous_divergence_hz;
          else sct_up=sct_b->previous_divergence;
          *common_node=sct_up;
          sct_b=stb_get_clock_tree(sct_up);
          firstb=0;
        }
      else 
        {
          if (firsta && hza) sct_up=sct_a->previous_divergence_hz;
          else sct_up=sct_a->previous_divergence;
          *common_node=sct_up;
          sct_a=stb_get_clock_tree(sct_up);
          firsta=0;
        }
      depth++;
    }
  if (sct_a==NULL || sct_b==NULL || depth>=maxdepth) return NULL;
  return sct_a;
}

int stb_get_common_node_info(ttvevent_list *a, ttvevent_list *b, int maxdepth, long *min, long *max, ttvevent_list **common_node, int hza, int hzb)
{
  stb_clock_tree *sct;
  if ((sct=stb_find_common_node(a, b, maxdepth, common_node, hza, hzb))==NULL)
    {
      *min=*max=0;
      *common_node=NULL;
      return 0;
    }
  *min=sct->delaymin;
  *max=sct->delaymax;
//  printf("common: for %s %s at node %s : %d %d\n", a->ROOT->NAME, b->ROOT->NAME, (*common_node)->ROOT->NAME, *min, *max);
  return 1;
}

ttvevent_list *stb_get_top_node(ttvevent_list *a, int hza)
{
  stb_clock_tree *sct_a;
  ttvevent_list *sct_up;
  ttvevent_list *top=NULL;
  int firsta=0;
   
  top=a;
  if ((sct_a=stb_get_clock_tree(a))==NULL) return top;

  if (hza) sct_up=sct_a->previous_divergence_hz;
  else sct_up=sct_a->previous_divergence;
  while (sct_up!=NULL)
    {
      top=sct_up;
      sct_a=stb_get_clock_tree(sct_up);
      sct_up=sct_a->previous_divergence;
    }
  return top;
}

long stb_get_generated_clock_common_node_info(ptype_list *geneclockinfo, ttvevent_list *a, ttvevent_list *b, int hza, int hzb, ttvevent_list **common)
{
  ttvevent_list *clocka, *clockb;
  long minmax;
  clocka=stb_get_top_node(a, hza);
  clockb=stb_get_top_node(b, hzb);
  minmax=stb_get_generated_clock_skew(geneclockinfo, clocka, clockb, common);
  if (minmax==TTV_NOTIME) return 0;
  return minmax;
}
                
