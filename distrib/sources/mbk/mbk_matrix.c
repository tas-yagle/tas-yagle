
#include <stdio.h>
#include <math.h>
#include <limits.h>
#ifdef Linux
#include <values.h>
#else
#include <float.h>
#endif
#include MUT_H
#include AVT_H
//#include "mbk_matrix.h"

//#define FINEST_PIVOT
//#define GAUSS

// ----- optimizaions ------
#define OPTIM_REDUCE
#define OPTIM_MULTIPLY

// -------------------------
void mbk_FreeMatrix(mbk_matrix *mm)
{
  DeleteHeap(&mm->mbk_matrix_elem_heap);
  mbkfree(mm->line);
  mbkfree(mm->col);
  if (mm->resolve_line_order!=NULL) mbkfree(mm->resolve_line_order);
  if (mm->resolve_unk_order!=NULL) mbkfree(mm->resolve_unk_order);
  mbkfree(mm);
}

mbk_matrix *mbk_CreateMatrix(int nbx, int nby)
{
  int i;
  mbk_matrix *mm;
  mm=(mbk_matrix *)mbkalloc(sizeof(mbk_matrix));
  mm->tracker0=mm->tracker1=NULL;
  mm->line=(elem_index_tab *)mbkalloc(sizeof(elem_index_tab)*nby);
  mm->col=(elem_index_tab *)mbkalloc(sizeof(elem_index_tab)*nbx);
  for (i=0;i<nbx;i++) 
    {
      mm->col[i].first_elem=NULL;
      mm->col[i].last_elem=NULL;
      mm->col[i].tracker=NULL;
      mm->col[i].NZ=0;
    }
  for (i=0;i<nby;i++)
    {
      mm->line[i].first_elem=NULL;
      mm->line[i].last_elem=NULL;
      mm->line[i].tracker=NULL;
      mm->line[i].NZ=0;      
    }
  mm->nbx=nbx;
  mm->nby=nby;
  mm->resolve_line_order=NULL;
  mm->resolve_unk_order=NULL;
  CreateHeap(sizeof(mbk_matrix_elem), nbx>4096?nbx:4096, &mm->mbk_matrix_elem_heap);
  return mm;
}


static int from_line_border(mbk_matrix *mm, int x, int y,mbk_matrix_elem **found, mbk_matrix_elem **last)
{
  if (mm->col[x].first_elem==NULL) { *last=NULL; *found=NULL; return 1; }
  if (y==mm->col[x].first_elem->y) { *last=mm->col[x].first_elem; *found=*last; return 1;}
  else if (y<mm->col[x].first_elem->y) { *last=mm->col[x].first_elem; *found=NULL; return 1;}
  if (y==mm->col[x].last_elem->y) { *last=mm->col[x].last_elem; *found=*last; return 1;}
  else if (y>mm->col[x].last_elem->y) { *last=mm->col[x].last_elem; *found=NULL; return 1;}
  return 0;
}
static int from_col_border(mbk_matrix *mm, int x, int y, mbk_matrix_elem **found, mbk_matrix_elem **last)
{
  if (mm->line[y].first_elem==NULL) { *last=NULL; *found=NULL; return 1; }
  if (x==mm->line[y].first_elem->x) { *last=mm->line[y].first_elem; *found=*last; return 1;}
  else if (x<mm->line[y].first_elem->x) { *last=mm->line[y].first_elem; *found=NULL; return 1;}
  if (x==mm->line[y].last_elem->x) { *last=mm->line[y].last_elem; *found=*last; return 1;}
  else if (x>mm->line[y].last_elem->x) { *last=mm->line[y].last_elem; *found=NULL; return 1;}
  return 0;
}

static mbk_matrix_elem *goto_line(mbk_matrix *mm, mbk_matrix_elem *start, int y, mbk_matrix_elem **last)
{
  mbk_matrix_elem *res;

  *last=start;
  if (start==NULL) return NULL;

  if (start->y==y) return start;

  if (from_line_border(mm, start->x, y, &res, last)==1) return res;
  
  if (y>start->y)
    {
      while (start!=NULL && y>start->y)
        {
          *last=start;
          start=start->DOWN;
        }
    }
  else
    {
      while (start!=NULL && y<start->y)
        {
          *last=start;
          start=start->UP;
        }
    }
  if (start!=NULL && start->y==y) return start;
  return NULL;
}

static mbk_matrix_elem *goto_col(mbk_matrix *mm, mbk_matrix_elem *start, int x, mbk_matrix_elem **last)
{
  mbk_matrix_elem *res;

  *last=start;
  if (start==NULL) return NULL;

  if (start->x==x) return start;

  if (from_col_border(mm, x, start->y, &res, last)==1) return res;

  if (x>start->x)
    {
      while (start!=NULL && x>start->x)
        {
          *last=start;
          start=start->RIGHT;
        }
    }
  else
    {
      while (start!=NULL && x<start->x)
        {
          *last=start;
          start=start->LEFT;
        }
    }
  if (start!=NULL && start->x==x) return start;
  return NULL;
}

static mbk_matrix_elem *matrix_get(mbk_matrix *mm, int x, int y, mbk_matrix_elem **last)
{
  mbk_matrix_elem *res=NULL;
  *last=NULL;

  if (from_col_border(mm, x, y, &res, last)==1
      ||
      from_line_border(mm, x, y, &res, last)==1) return res;

  if (mm->tracker0!=NULL)
    {
      if (mm->tracker0->y==y)
        res=goto_col(mm, mm->tracker0, x, last);
      else
        if (mm->tracker0->x==x)
          res=goto_line(mm, mm->tracker0, y, last);
      if (res) mm->tracker0=res;
      if (*last!=NULL) return res;
    }
  if (mm->tracker1!=NULL && res==NULL)
    {
      if (mm->tracker1->y==y)
        res=goto_col(mm, mm->tracker1, x, last);
      else
        if (mm->tracker1->x==x)          
          res=goto_line(mm, mm->tracker1, y, last);
      if (res) mm->tracker1=res;
      if (*last!=NULL) return res;
    }
  if (res==NULL)
    res=goto_col(mm, mm->line[y].first_elem, x, last);

  return res;
}

static inline void get_up_down(mbk_matrix_elem *mme, int y, mbk_matrix_elem **up, mbk_matrix_elem **down)
{
  if (mme->y>y) { *down=mme; *up=(*down)->UP; }
  else { *up=mme; *down=(*up)->DOWN; }
}

static inline void get_left_right(mbk_matrix_elem *mme, int x, mbk_matrix_elem **left, mbk_matrix_elem **right)
{
  if (mme->x>x) { *right=mme; *left=(*right)->LEFT; }
  else { *left=mme; *right=(*left)->RIGHT; }
}

static mbk_matrix_elem *matrix_create_elem(mbk_matrix *mm, mbk_matrix_elem *left, mbk_matrix_elem *right, mbk_matrix_elem *up, mbk_matrix_elem *down, int x, int y)
{
  mbk_matrix_elem *mme;

  mme=AddHeapItem(&mm->mbk_matrix_elem_heap);
  mme->x=x;
  mme->y=y;
  mme->LEFT=left;
  mme->RIGHT=right;
  mme->UP=up;
  mme->DOWN=down;
  if (left==NULL) mm->line[mme->y].first_elem=mme;
  else left->RIGHT=mme;
  if (up==NULL) mm->col[mme->x].first_elem=mme;
  else up->DOWN=mme;
  if (right==NULL) mm->line[mme->y].last_elem=mme;
  else right->LEFT=mme;
  if (down==NULL) mm->col[mme->x].last_elem=mme;
  else down->UP=mme;
  mm->line[y].NZ++;
  mm->col[x].NZ++;
  return mme;
}

// a utiliser que si l'element n'existe pas
static mbk_matrix_elem *matrix_create_for_set(mbk_matrix *mm, int x, int y)
{
  mbk_matrix_elem *mme, *last;
  mbk_matrix_elem *up=NULL, *down=NULL, *left=NULL, *right=NULL;

  if (mm->tracker0!=NULL)
    {
      if (mm->tracker0->y==y)
        {
          goto_col(mm, mm->tracker0, x, &last);
          get_left_right(last, x, &left, &right);
        }
      else       
        if (mm->tracker0->x==x)
          {
            goto_line(mm, mm->tracker0, y, &last);
            get_up_down(last, y, &up, &down);
          }
    }
  if (mm->tracker1!=NULL)
    {
      if (mm->tracker1->y==y && (left==NULL && right==NULL))
        {
          goto_col(mm, mm->tracker1, x, &last);
          get_left_right(last, x, &left, &right);
        }
      else       
        if (mm->tracker1->x==x && (up==NULL && down==NULL))
          {
            goto_line(mm, mm->tracker1, y, &last);
            get_up_down(last, y, &up, &down);
          }
    }
  
  if (left==NULL && right==NULL)
    {
      goto_col(mm, mm->line[y].first_elem, x, &last);
      if (last!=NULL)
        get_left_right(last, x, &left, &right);
    }
  if (up==NULL && down==NULL)
    {
      goto_line(mm, mm->col[x].first_elem, y, &last);
      if (last!=NULL)
        get_up_down(last, y, &up, &down);
    }

  mme=matrix_create_elem(mm, left, right, up, down, x, y);

  return mme;
}


static inline void settracker(mbk_matrix *mm, mbk_matrix_elem *mme)
{
  int d1, d2;
  if (mm->tracker0==mme || mm->tracker1==mme) return;
  if (mm->tracker0==NULL) d1=0;
  else d1=(mm->tracker0->x-mme->x)+(mm->tracker0->y-mme->y);
  if (mm->tracker1==NULL) d2=0;
  else d2=(mm->tracker1->x-mme->x)+(mm->tracker1->y-mme->y);
  if (d1<0) d1=-d1;
  if (d2<0) d2=-d2;
  if (d1<d2) mm->tracker0=mme;
  else mm->tracker1=mme;
}

double mbk_GetMatrixValue(mbk_matrix *mm, int x, int y)
{
  mbk_matrix_elem *res, *last;
  res=matrix_get(mm, x, y, &last);
  if (res==NULL) 
    {
      if (last!=NULL) settracker(mm, last);
      return 0;
    }
  settracker(mm, res);
  return res->value;
}

static void matrix_remove(mbk_matrix *mm, mbk_matrix_elem *mme)
{
  if (mme->LEFT==NULL) mm->line[mme->y].first_elem=mme->RIGHT;
  else mme->LEFT->RIGHT=mme->RIGHT;

  if (mme->RIGHT==NULL) mm->line[mme->y].last_elem=mme->LEFT;
  else mme->RIGHT->LEFT=mme->LEFT;

  if (mme->UP==NULL) mm->col[mme->x].first_elem=mme->DOWN;
  else mme->UP->DOWN=mme->DOWN;

  if (mme->DOWN==NULL) mm->col[mme->x].last_elem=mme->UP;
  else mme->DOWN->UP=mme->UP;

  if (mm->tracker0==mme) mm->tracker0=mme->LEFT;
  if (mm->tracker1==mme) mm->tracker1=mme->LEFT;

  if (mm->col[mme->x].tracker==mme) mm->col[mme->x].tracker=mme->DOWN;
  if (mm->line[mme->y].tracker==mme) mm->line[mme->y].tracker=mme->LEFT;

  mm->line[mme->y].NZ--;
  mm->col[mme->x].NZ--;

  DelHeapItem(&mm->mbk_matrix_elem_heap, mme);
}

void matrix_clearline(mbk_matrix *mm, int i)
{
  mbk_matrix_elem *res, *nres;
  for (res=mm->line[i].first_elem; res!=NULL; res=nres)
    {
      nres=res->RIGHT;
      matrix_remove(mm, res);
    }
}

void matrix_clearcol(mbk_matrix *mm, int i)
{
  mbk_matrix_elem *res, *nres;
  for (res=mm->col[i].first_elem; res!=NULL; res=nres)
    {
      nres=res->DOWN;
      matrix_remove(mm, res);
    }
}

void mbk_SetMatrixValue(mbk_matrix *mm, int x, int y, double val)
{
  mbk_matrix_elem *res, *last;
  res=matrix_get(mm, x, y, &last);
  if (res!=NULL) 
    {
      if (val==0)
        {          
          matrix_remove(mm, res);
        }
      else
        {
          res->value=val;
          settracker(mm, res);
        }
    }
  else if (val!=0)
    {
      res=matrix_create_for_set(mm, x, y);
      res->value=val;
      settracker(mm, res);
    }
}

inline int mbk_GetMatrixNZUR(mbk_matrix *mm, int x)
{
  return mm->col[x].NZ;
}

inline int mbk_GetMatrixNZLC(mbk_matrix *mm, int y)
{
  return mm->line[y].NZ;
}

void mbk_DisplayMatrix(char *filename, mbk_matrix *mm)
{
  int i, j;
  char buf[20];
  FILE *f=stdout;

  if (filename!=NULL)
    {
      f=fopen(filename,"wt");
    }

  for (j=0; j<mm->nby; j++)
    {
      for (i=0; i<mm->nbx; i++)
        {        
          sprintf(buf, "%.3g", mbk_GetMatrixValue(mm, i, j));
          fprintf(f," %9s", buf);
        }
      fprintf(f,"\n");
    }

  fprintf(f,"NZUR:");
  for (i=0; i<mm->nbx; i++)
    {
      sprintf(buf, "%d", mbk_GetMatrixNZUR(mm, i));
      fprintf(f," %2s", buf);
    }
  fprintf(f,"\n");
  fprintf(f,"NZLC:");
  for (j=0; j<mm->nby; j++)
    {
      sprintf(buf, "%d", mbk_GetMatrixNZLC(mm, j));
      fprintf(f," %2s", buf);
    }
  fprintf(f,"\n");
  if (mm->resolve_line_order!=NULL)
    {
      fprintf(f,"RESOLVE LINE ORDER:");
      for (i=0; i<mm->nby; i++)
        fprintf(f," %d", mm->resolve_line_order[i]);
      fprintf(f,"\n");
    }
  if (mm->resolve_unk_order!=NULL)
    {
      fprintf(f,"RESOLVE UNK  ORDER:");
      for (i=0; i<mm->nby; i++)
        fprintf(f," %d", mm->resolve_unk_order[i]);
      fprintf(f,"\n");
    }
  if (f!=stdout) fclose(f);
}


static chain_list *find_lowestNZ(elem_index_tab *eit, chain_list **unlocked)
{
  int i, lowest=INT_MAX;
  chain_list *cl=NULL, **prev, *ch;
  
  prev=unlocked;
  ch=*unlocked;
  while (ch!=NULL)
    {
      i=(int)(long)ch->DATA;
      if (eit[i].NZ<lowest) 
        {
          freechain(cl);
          cl=addchain(NULL, prev);
          lowest=eit[i].NZ;
        }
#ifdef FINEST_PIVOT
      else if (eit[i].NZ==lowest) { cl=addchain(cl, prev); }
#endif
      prev=&ch->NEXT;
      ch=ch->NEXT;
    }
  return cl;
}

#if 0
static int findPivot(mbk_matrix *u, chain_list **unlockedlines, chain_list **unlockedcols, int *x, int *y)
{
  chain_list *colcandidates, *linecandidates;
  chain_list *clx, *cly, **tmp, *tmpfree, **bestx=NULL, **besty=NULL;
  double highest=-DBL_MAX, val;

  colcandidates=find_lowestNZ(u->col, unlockedcols);
  linecandidates=find_lowestNZ(u->line, unlockedlines);
  if (colcandidates->NEXT==NULL && linecandidates->NEXT==NULL)
    {
      tmp=(chain_list **)colcandidates->DATA;
      tmpfree=*tmp;
      *x=(int)(long)tmpfree->DATA;
      *tmp=(*tmp)->NEXT;
      tmpfree->NEXT=NULL; freechain(tmpfree);

      tmp=(chain_list **)linecandidates->DATA;
      tmpfree=*tmp;
      *y=(int)(long)tmpfree->DATA;
      *tmp=(*tmp)->NEXT;
      tmpfree->NEXT=NULL; freechain(tmpfree);
      // ligne et column du pivot sont enleves des listes des col et ligne non bloque
    }
  else
    {
      int i, j;
      for (cly=linecandidates; cly!=NULL; cly=cly->NEXT)
        {
          tmp=(chain_list **)cly->DATA;
          j=(int)(long)(*tmp)->DATA;
          for (clx=colcandidates; clx!=NULL; clx=clx->NEXT)
            {
              tmp=(chain_list **)clx->DATA;
              i=(int)(long)(*tmp)->DATA;
              val=mbk_GetMatrixValue(u, i, j);
              if (val<0) val=-val;
              if (val>highest)
                {
                  highest=val;
                  *x=i;
                  *y=j;
                  bestx=(chain_list **)clx->DATA;
                  besty=(chain_list **)cly->DATA;
                }
            }
        }
      if (bestx==NULL || besty==NULL)
        {
          avt_error("mbk_matrix", 2, AVT_ERR, "<NaN> or <Inf> value found in matrix\n");
          return 1;
        }
      // enleve ligne et column du pivot des listes des col et ligne non bloque
      tmpfree=*bestx; *bestx=(*bestx)->NEXT;
      tmpfree->NEXT=NULL; freechain(tmpfree);
      tmpfree=*besty; *besty=(*besty)->NEXT;
      tmpfree->NEXT=NULL; freechain(tmpfree);
    }
  freechain(colcandidates); 
  freechain(linecandidates);
  return 0;
}
#endif
/*
static chain_list **find_MAXPivot(mbk_matrix *u, int y, chain_list **unlockedcols, char *locking, double *highest, chain_list **who)
{
  mbk_matrix_elem *mme;
  chain_list *cl;
  int x;

  mme=u->line[y];

  for (cl=unlockedcols; cl!=NULL; cl=cl->NEXT)
    {
      x=(*cl)->DATA;
      val=mbk_GetMatrixValue(u, x, y);
      if (val<0) val=-val;
      if (val>*highest)
        {
          who=cl;
          *highest=val;
        }
    }
  retur who;
}
*/
static int findPivot(mbk_matrix *u, chain_list **unlockedlines, chain_list **unlockedcols, int *x, int *y)
{
  chain_list **colfound, *linecandidates, **linefound, *cl, *ch, **prev;
  chain_list **tmp, *tmpfree;
  double highest=-DBL_MAX, val;
  int xx, yy;

//  colcandidates=find_lowestNZ(u->col, unlockedcols);
  colfound=NULL; linefound=NULL;
  linecandidates=find_lowestNZ(u->line, unlockedlines);

// version en dessous, on parcourt quasiment toute la matrice dans notre cas
#if 0
  if (linecandidates->NEXT!=NULL)
    {
      colcandidates=find_lowestNZ(u->col, unlockedcols);
      for (cl=colcandidates; cl!=NULL; cl=cl->NEXT)
        {
          tmp=(chain_list **)ch->DATA;
          xx=(int)(long)(*tmp)->DATA;
          for (ch=linecandidates; ch!=NULL; ch=ch->NEXT)
        {
          tmp=(chain_list **)ch->DATA;
          yy=(int)(long)(*tmp)->DATA;
          
        }
        }
      freechain(colcandidates);
    }
#endif


  for (ch=linecandidates; ch!=NULL; ch=ch->NEXT)
    {
      tmp=(chain_list **)ch->DATA;
      yy=(int)(long)(*tmp)->DATA;
      prev=unlockedcols;
      for (cl=*unlockedcols; cl!=NULL; prev=&cl->NEXT, cl=cl->NEXT)
        {
          xx=(int)(long)cl->DATA;
          val=mbk_GetMatrixValue(u, xx, yy);
          if (val<0) val=-val;
          if (val>highest)
            {
              colfound=prev;
              linefound=(chain_list **)ch->DATA;
              highest=val;
            }
        }
#ifndef FINEST_PIVOT
      break;
#endif
    }

  if (colfound==NULL) return 1;

  *x=(int)(long)(*colfound)->DATA;
  *y=(int)(long)(*linefound)->DATA;

  // enleve ligne et column du pivot des listes des col et ligne non bloque
  tmpfree=*colfound; *colfound=(*colfound)->NEXT;
  tmpfree->NEXT=NULL; freechain(tmpfree);
  tmpfree=*linefound; *linefound=(*linefound)->NEXT;
  tmpfree->NEXT=NULL; freechain(tmpfree);

  freechain(linecandidates);
  return 0;
}

void mbk_MatrixCopyCol(mbk_matrix *u, mbk_matrix *l, int xu, int xl, char *locking)
{
  mbk_matrix_elem *res, *nres;
  for (res=l->col[xl].first_elem; res!=NULL; res=nres)
    {
      nres=res->DOWN;
      matrix_remove(l, res);
    }
  for (res=u->col[xu].first_elem; res!=NULL; res=res->DOWN)
    {
      if (locking==NULL || locking[res->y]==0)
        mbk_SetMatrixValue(l, xl, res->y, res->value);
    }
}

void mbk_MatrixCopyCol__notfinished(mbk_matrix *u, mbk_matrix *l, int xu, int xl, char *locking)
{
  mbk_matrix_elem *res, *nres, *mme;
  for (res=l->col[xl].first_elem; res!=NULL; res=nres)
    {
      nres=res->DOWN;
      matrix_remove(l, res);
    }
  nres=NULL; mme=NULL;
  for (res=u->col[xu].first_elem; res!=NULL; res=res->DOWN)
    {
      if (locking==NULL || locking[res->y]==0)
        {
          mme=matrix_create_elem(l, NULL, NULL, nres, NULL, xl, res->y);
          mme->value=res->value;
          if (nres==NULL) l->col[xl].first_elem=mme;
          else nres->DOWN=mme;
          nres=mme;
        }
    }
  l->col[xl].last_elem=mme;
}

void mbk_MatrixCopyCol__notfinished_linkline(mbk_matrix *l)
{
  mbk_matrix_elem *res;
  int xl;
  
  for (xl=0; xl<l->nby; xl++) l->line[xl].tracker=NULL;
  
  for (xl=0; xl<l->nbx; xl++)
    {
      for (res=l->col[xl].first_elem; res!=NULL; res=res->DOWN)
        {
          res->LEFT=l->line[res->y].tracker;
          if (res->LEFT!=NULL) res->LEFT->RIGHT=res;
          else l->line[res->y].first_elem=res;
          l->line[res->y].tracker=res;
        }
    }
  for (xl=0; xl<l->nby; xl++) l->line[xl].last_elem=l->line[xl].tracker;
}



void mbk_MatrixMultiplyLineBy(mbk_matrix *u, int y, char *locking, double mult)
{
  mbk_matrix_elem *res, *nres;

  for (res=u->line[y].first_elem; res!=NULL; res=nres)
    {
      nres=res->RIGHT;
      if (locking==NULL || locking[res->x]==0)
        {
          res->value*=mult;
          if (res->value==0)
            {
              matrix_remove(u, res);
            }
        }
    }
}

void mbk_MatrixDivideLineBy(mbk_matrix *u, int y, char *locking, double mult)
{
  mbk_matrix_elem *res, *nres;

  for (res=u->line[y].first_elem; res!=NULL; res=nres)
    {
      nres=res->RIGHT;
      if (locking==NULL || locking[res->x]==0)
        {
          res->value/=mult;
          if (res->value==0)
            {
              matrix_remove(u, res);
            }
        }
    }
}

mbk_matrix_elem *tracker_goto_line(mbk_matrix *mm, int x, int y)
{
  mbk_matrix_elem *res, *last;

  if (from_line_border(mm, x, y, &res, &last)==1)
    {
      mm->col[x].tracker=last;
    }
  else
    {
      last=NULL;
      if (mm->col[x].tracker==NULL) mm->col[x].tracker=mm->col[x].first_elem;      
      res=mm->col[x].tracker;
      if (res!=NULL)
        {
          if (y>res->y)
            {
              while (res!=NULL && y>res->y)
                {
                  last=res;
                  res=res->DOWN;
                }
              mm->col[x].tracker=last;
            }
          else
            {
              while (res!=NULL && y<res->y)
                {
                  last=res;
                  res=res->UP;
                }
            }
        }
      mm->col[x].tracker=last;
    }
  return  mm->col[x].tracker;
}

void followline(mbk_matrix *mm, int dy, int sy, char *locking, double mult)
{
  mbk_matrix_elem *a, *b, *na, *preva=NULL, *last;
  mbk_matrix_elem *left, *right, *up, *down;
  double val;
  a=mm->line[dy].first_elem;
  b=mm->line[sy].first_elem;

  while (a!=NULL || b!=NULL)
    {
      if (a!=NULL && b!=NULL && a->x==b->x)
        {              
          na=a->RIGHT;
          if (locking[a->x]==0)
            {
              val=a->value=a->value-(b->value*mult);
              if (val==0) matrix_remove(mm, a);
              else
                { 
                  a->value=val;
                  preva=a;
                }
            }
          else preva=a;
          a=na; b=b->RIGHT;
        }
      else if (b==NULL || (a!=NULL && a->x<b->x))
        {
          na=a->RIGHT;
          preva=a; a=na;
        }
      else //if (a->x>b->x)
        {
          //              na=a->RIGHT;
          val=0-(b->value*mult);
          if (locking[b->x]==0)
            {
              last=tracker_goto_line(mm, b->x, dy);
              if (last==NULL)
                left=right=up=down=NULL;
              else
                {
                  get_up_down(last, dy, &up, &down);
                }
              a=matrix_create_elem(mm, preva, a, up, down, b->x, dy);
              a->value=val;
              preva=a;
            }
          b=b->RIGHT;
        }
    }      
}



// elem(dy)(x) = elem(dy)(x) - elem(sy)(x) * mult 
void mbk_MatrixLineMultThenSub(mbk_matrix *u, int dy, int sy, char *locking, double mult)
{
/*  double sval, dval;
  int i;*/
  followline(u, dy, sy, locking, mult);

/*  for (i=0; i<u->nbx; i++)
    {
      if (locking==NULL || locking[i]==0)
        {
          sval=mbk_GetMatrixValue(u, i, sy);
          dval=mbk_GetMatrixValue(u, i, dy);
          dval=dval-(sval*mult);
          mbk_SetMatrixValue(u, i, dy, dval);
        }
    }
*/
}

void mbkMatrixSwapCols(mbk_matrix *u, int x0, int x1)
{
  int j;
  double val0, val1;
  for (j=0; j<u->nby; j++)
    {
      val0=mbk_GetMatrixValue(u, x0, j);
      val1=mbk_GetMatrixValue(u, x1, j);
      mbk_SetMatrixValue(u, x1, j, val0);
      mbk_SetMatrixValue(u, x0, j, val1);
    }
}

void mbkMatrixSwapLines(mbk_matrix *u, int y0, int y1)
{
  int j;
  double val0, val1;
  for (j=0; j<u->nbx; j++)
    {
      val0=mbk_GetMatrixValue(u, j, y0);
      val1=mbk_GetMatrixValue(u, j, y1);
      mbk_SetMatrixValue(u, j, y1, val0);
      mbk_SetMatrixValue(u, j, y0, val1);
    }
}

int mbk_CreateLUMatrix(mbk_matrix *u, mbk_matrix *l, mbk_matrix *sol)
{
  char *lockedCOL, *lockedLINE;
  int i, j, pivot_x, pivot_y;
  double pivot_value, val;
  mbk_matrix_elem *mme, *last;
  chain_list *unlockedCOL, *unlockedLINE, *cl;

  if (u->nbx!=u->nby)
    {
      avt_errmsg (MBK_ERRMSG, "036", AVT_ERROR);
      return 1;
    }
  if (l!=NULL && (u->nbx!=l->nbx || l->nbx!=l->nby)) 
    {
      avt_errmsg (MBK_ERRMSG, "037", AVT_ERROR);
      return 1;
    }

  if (u->resolve_line_order==NULL)
    {
      u->resolve_line_order=mbkalloc(sizeof(int)*u->nby);
      u->resolve_unk_order=mbkalloc(sizeof(int)*u->nby);
    }
  if (l!=NULL && l->resolve_line_order==NULL)
    {
      l->resolve_line_order=mbkalloc(sizeof(int)*l->nby);
      l->resolve_unk_order=mbkalloc(sizeof(int)*l->nby);
    }
  lockedCOL=mbkalloc(sizeof(char)*u->nbx);
  lockedLINE=mbkalloc(sizeof(char)*u->nby);
  unlockedCOL=unlockedLINE=NULL;

  for (i=u->nbx-1;i>=0;i--)
    {
      lockedCOL[i]=0;
      lockedLINE[i]=0;
      unlockedCOL=addchain(unlockedCOL, (void *)(long)i);
      unlockedLINE=addchain(unlockedLINE, (void *)(long)i);
    }

  for (i=0; i<u->nbx; i++)
    {
#ifndef GAUSS
      if (findPivot(u, &unlockedLINE, &unlockedCOL, &pivot_x, &pivot_y))
        {
          avt_errmsg (MBK_ERRMSG, "038", AVT_ERROR);
          return 1;
        }
#else
      pivot_x=pivot_y=i;
      unlockedLINE=unlockedLINE->NEXT;
      unlockedCOL=unlockedCOL->NEXT;
      
      if (i!=pivot_x)
        {
          mbkMatrixSwapCols(u, i, pivot_x);
        }
      if (i!=pivot_y)
        {
          mbkMatrixSwapLines(u, i, pivot_y);
        }
#endif

      if (l!=NULL)
        mbk_MatrixCopyCol__notfinished(u, l, pivot_x, pivot_y, lockedLINE);
      
      pivot_value=mbk_GetMatrixValue(u, pivot_x, pivot_y);
      if (pivot_value==0) 
        {
          avt_errmsg (MBK_ERRMSG, "039", AVT_ERROR);
          return 1;
        }

      if (l!=NULL)
        {
          l->resolve_line_order[i]=pivot_y;
          l->resolve_unk_order[i]=pivot_y;
        }
      u->resolve_line_order[u->nby-i-1]=pivot_y;
      u->resolve_unk_order[u->nby-i-1]=pivot_x;

      mbk_MatrixDivideLineBy(u, pivot_y, lockedCOL, pivot_value);
      if (sol!=NULL)
        mbk_MatrixDivideLineBy(sol, pivot_y, NULL, pivot_value);

      for (cl=unlockedLINE; cl!=NULL; cl=cl->NEXT)
        {
          j=(int)(long)cl->DATA;
#if 1
          if (cl==unlockedLINE)
            {
              mme=matrix_get(u, pivot_x, j, &last);
              if (mme==NULL) { val=0; mme=u->col[pivot_x].first_elem; }
              else { val=mme->value; mme=mme->DOWN; }
            }
          else
            {
              while (mme!=NULL && mme->y<j) { mme=mme->DOWN; }
              if (mme==NULL || mme->y!=j) val=0;
              else { val=mme->value; mme=mme->DOWN; }
            }
#else
          val=mbk_GetMatrixValue(u, pivot_x, j);
#endif
          if (val!=0)
            {
              mbk_MatrixLineMultThenSub(u, j, pivot_y, lockedCOL, val);
              if (sol!=NULL)
                mbk_MatrixLineMultThenSub(sol, j, pivot_y, NULL, val);
            }
        }
      
      lockedCOL[pivot_x]=1;
      lockedLINE[pivot_y]=1;
#if 0
      printf("U' *************\n");
      mbk_DisplayMatrix(stdout,u);
#endif
    }

  if (l!=NULL) mbk_MatrixCopyCol__notfinished_linkline(l);

  mbkfree(lockedLINE);
  mbkfree(lockedCOL);
  return 0;
}

mbk_matrix *mbk_MatrixMultiplyMatrix(mbk_matrix *a, mbk_matrix *b)
{
  mbk_matrix *res;
  int /*i,*/ j, k;
  double cumul;
  mbk_matrix_elem *mmx, *mmy, *mmxstart;
  if (a->nbx!=b->nby) 
    {
      avt_errmsg (MBK_ERRMSG, "040", AVT_ERROR);
      return NULL;
    }
  res=mbk_CreateMatrix(b->nbx, a->nby);

#ifndef OPTIM_MULTIPLY
  for (j=0; j<a->nby; j++)
    {
      for (k=0; k<b->nbx; k++)
        {
          cumul=0;
          for (i=0; i<b->nby; i++)
            {
              cumul+=mbk_GetMatrixValue(a, i, j)*mbk_GetMatrixValue(b, k, i);
            }
          mbk_SetMatrixValue(res, k, j, cumul);
        }
    }
#else
  for (j=0; j<a->nby; j++)
    {
      mmxstart=a->line[j].first_elem;
      for (k=0; k<b->nbx; k++)
        {
          mmy=b->col[k].first_elem;
          mmx=mmxstart;
          cumul=0;
          while (mmx!=NULL && mmy!=NULL)
            {             
              if (mmx->x<mmy->y) mmx=mmx->RIGHT;
              else if (mmy->y<mmx->x) mmy=mmy->DOWN;
              else
                {
                  cumul+=mmx->value*mmy->value;
                  mmx=mmx->RIGHT;
                  mmy=mmy->DOWN; 
                }
            }
          if (cumul!=0)
            mbk_SetMatrixValue(res, k, j, cumul);
        }
    }
#endif
  return res;
}

mbk_matrix *mbk_MatrixSubstractMatrix(mbk_matrix *a, mbk_matrix *b)
{
  mbk_matrix *res;
  int j, k;
  double cumul;
  if (a->nbx!=b->nbx || a->nby!=b->nby)
    {
      avt_errmsg (MBK_ERRMSG, "041", AVT_ERROR);
      return NULL;
    }

  res=mbk_CreateMatrix(a->nbx, a->nby);
  for (j=0; j<a->nby; j++)
    {
      for (k=0; k<a->nbx; k++)
        {
          cumul=mbk_GetMatrixValue(a, k, j)-mbk_GetMatrixValue(b, k, j);
          mbk_SetMatrixValue(res, k, j, cumul);
        }
    }
  return res;
}

mbk_matrix *mbk_MatrixDuplicateMatrix(mbk_matrix *a)
{
  mbk_matrix *res;
  int j, k;
  double cumul;
  res=mbk_CreateMatrix(a->nbx, a->nby);
  for (j=0; j<a->nby; j++)
    {
      for (k=0; k<a->nbx; k++)
        {
          cumul=mbk_GetMatrixValue(a, k, j);
          mbk_SetMatrixValue(res, k, j, cumul);
        }
    }
  return res;
}

void mbk_MatrixAddHalfLinkedElem(mbk_matrix *mm, int x, int y, double value)
{
  mbk_matrix_elem *mme;
  if (value==0) return;
  mme=matrix_create_elem(mm, NULL, NULL, mm->col[x].tracker, NULL, x, y);
  mme->value=value;
  if (mm->col[x].tracker!=NULL) mm->col[x].tracker->DOWN=mme;
  mm->col[x].tracker=mme;
}

void mbk_MatrixFinishElemLink(mbk_matrix *mm)
{
  mbk_MatrixCopyCol__notfinished_linkline(mm);
}

double *mbk_MatrixSolveUsingArray(mbk_matrix *a, double *solval, double *unkval)
{
  int o, i, u;
  double divval, cumul;
  mbk_matrix_elem *mme;
  
  if (a->resolve_line_order==NULL)
    {
      avt_errmsg (MBK_ERRMSG, "042", AVT_ERROR);
      return NULL;
    }
  if (unkval==NULL) unkval=mbkalloc(sizeof(double)*a->nbx);  

  for (o=0; o<a->nby; o++)
    {
      i=a->resolve_line_order[o];
      u=a->resolve_unk_order[o];
      cumul=0; divval=0;
      for (mme=a->line[i].first_elem; mme!=NULL; mme=mme->RIGHT)
        {
          if (mme->x==u) divval=mme->value;
          else cumul=cumul+(mme->value*unkval[mme->x]);
        }
      unkval[u]=(solval[i]-cumul)/divval;
    }
  return unkval;
}

mbk_matrix *mbk_MatrixSolve(mbk_matrix *a, mbk_matrix *sol)
{
  int i;
  double *unkval, *solval;
  mbk_matrix *res;

  if (sol->nbx!=1 || sol->nby!=a->nby)
    {
      avt_errmsg (MBK_ERRMSG, "043", AVT_ERROR);
      return NULL;
    }
  solval=mbkalloc(sizeof(double)*sol->nby);  
  for (i=0;i<sol->nby;i++) solval[i]=mbk_GetMatrixValue(sol, 0, i);
  
  unkval=mbk_MatrixSolveUsingArray(a, solval, NULL);

  mbkfree(solval);

  if ((res=mbk_CreateMatrix(1, a->nbx))==NULL) return res;
  
  for (i=0;i<a->nbx;i++) 
    if (unkval[i]!=0) mbk_SetMatrixValue(res, 0, i, unkval[i]);

  return res;
}

int mbk_MatrixReduce(mbk_matrix *a, int limit_index)
{
  int i, j;
  double pval, lval, sval;
  mbk_matrix_elem *mme;

#ifdef OPTIM_REDUCE
  for (i=a->nby-1; i>=limit_index; i--)
    {
      if ((mme=a->line[i].last_elem)==NULL || mme->x!=i)
        {
          pval=0;
          avt_errmsg (MBK_ERRMSG, "044", AVT_ERROR);
          return 1;
        }
      else
        {
          pval=mme->value;
        }
      mbk_MatrixDivideLineBy(a, i, NULL, -pval);
      for (j=0; j<i; j++)
        {
          if ((mme=a->line[j].last_elem)==NULL || mme->x!=i)
            lval=0;
          else
            {
              lval=mme->value;
            }
          if (lval!=0)
            {
              for (mme=a->line[i].first_elem; mme!=NULL && mme->x<i; mme=mme->RIGHT)
                {
                  sval=mbk_GetMatrixValue(a, mme->x, j);
                  mbk_SetMatrixValue(a, mme->x, j, sval+lval*mme->value);
                }
            }
        }
      matrix_clearcol(a, i);
      matrix_clearline(a, i);
    }
#else
  {
    int k;
  for (i=a->nby-1; i>=limit_index; i--)
    {
      pval=mbk_GetMatrixValue(a, i, i);
      mbk_MatrixDivideLineBy(a, i, NULL, -pval);
      for (k=0; k<i; k++)
        {
          pval=mbk_GetMatrixValue(a, k, i);
          for (j=0; j<i; j++)
            {
              lval=mbk_GetMatrixValue(a, i, j);
              if (lval!=0)
                {
                  sval=mbk_GetMatrixValue(a, k, j);
                  mbk_SetMatrixValue(a, k, j,
                                     sval+lval*pval);
                }
            }
        }
    }
  }
#endif
  return 0;
}

int mbk_MatrixTraverse(mbk_matrix *a, matrix_traverse_func mtf, void *data)
{
  int i, ret;
  mbk_matrix_elem *mme, *nextmme;

  for (i=0; i<a->nby; i++)
    {
      for (mme=a->line[i].first_elem; mme!=NULL; mme=nextmme)
        {
          nextmme=mme->RIGHT;
          ret=mtf(mme->x, mme->y, &mme->value, data);
          if (mme->value==0) matrix_remove(a, mme);
          if (ret!=0) return 1;
        }
    }
  return 0;
}

static int pp(int x, int y, double *val, void *data)
{
  if (*val!=0)
    fprintf((FILE *)data, "mbk_SetMatrixValue(u,%d,%d,%e);\n",x, y, *val);
  return 0;
}

void matdrive(mbk_matrix *a, char *name)
{
  FILE *f;
  f=fopen("toto.c","wt");
  fprintf(f,"u=mbk_CreateMatrix(%d, %d);\n",a->nbx, a->nby);
  fprintf(f,"l=mbk_CreateMatrix(%d, %d);\n",a->nbx, a->nby);
  mbk_MatrixTraverse(a, pp, f);
  fclose(f);
  name=NULL;
}
