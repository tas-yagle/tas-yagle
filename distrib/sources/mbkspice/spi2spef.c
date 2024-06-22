#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include EQT_H
#include MSL_H
#include AVT_H
#include "spi_int.h"
#include "spi_hash.h"
#include "spi_parse.h"

//#define CORRECT_CONNECT
//#define FULLVALUE
//#define DELAY_DEBUG_STAT
extern float             SPI_SCALE_RESIFACTOR ;
extern float             SPI_SCALE_CAPAFACTOR ;

static HeapAlloc ha_item;
static int freeitemspace;

#ifdef ENABLE_STATS
long ____nbequi=0;
#endif

extern int spi_get_xy (chain_list *ligne, float *com_x, float *com_y, int *Tx, int *Ty, int *R, int *A);

void _spispef_setnodename(subckt *cir, __node_ * n, char *ins, char *con)
{
  __names_ *p;

  CheckNodeSupInfo(cir, n);

  p=(__names_ *)AdvancedBlockAlloc(cir->aba, sizeof(ptype_list));
  p->con=con;
  p->ins=ins;
  p->next=n->SUPINFO->names;
  n->SUPINFO->names=p;
}

/*
static int countchain(chain_list *cl)
{
  int i=0;
  while (cl!=NULL) { i++; cl=cl->NEXT;}
  return i;
}
*/

void _spispef_complete_node(subckt *cir, char *name, __node_ *n, int mark, int justcreated)
{
  __equi_ *e;

#ifndef __ALL_WARNING__
  name	= NULL; 
#endif
  if (!justcreated)
    {
      e=n->signal;
      if (mark) 
        {
          if (e->firstline==-1) e->firstline=cir->TOTAL_LINE_COUNTER;
          if (e->lastline==-1 || e->lastline<cir->TOTAL_LINE_COUNTER) e->lastline=cir->TOTAL_LINE_COUNTER;
        }
      return;
    }

  e=AddHeapItem(&cir->ha_equi);  
#ifdef ENABLE_STATS
  ____nbequi++;
#endif
  n->signal=e;
  n->SUIV=NULL;
  e->nodes=n;
  e->lastnode=n;
  if (mark)
    e->firstline=e->lastline=cir->TOTAL_LINE_COUNTER;
  else
    e->firstline=e->lastline=-1;
  e->b.nbnodes=1;
  e->nbelem=0;
  e->next=cir->all_equi;
  if (cir->all_equi!=NULL) cir->all_equi->a.prev=e;
  e->a.prev=NULL;
//  e->tag=0;
  cir->all_equi=e;
}

void _spispef_mergenodes(subckt *cir, __node_ *a, __node_ *b)
{
  __node_ *n;
  if (a->signal==b->signal) return;
//  printf("merge %d %d\n",a->index,b->index);  
  if (a->signal->b.nbnodes>b->signal->b.nbnodes)
    {
      a->signal->nbelem+=b->signal->nbelem;
//      a->signal->tag|=b->signal->tag;
      n=b->signal->nodes;
      a->signal->b.nbnodes+=b->signal->b.nbnodes;

      a->signal->lastnode->SUIV=b->signal->nodes;
      a->signal->lastnode=b->signal->lastnode;
//      a->signal->nodes=(__node_ *)append((chain_list *)a->signal->nodes, (chain_list *)b->signal->nodes);
      if (a->signal->firstline==-1 ||
          b->signal->firstline<a->signal->firstline) a->signal->firstline=b->signal->firstline;
      if (a->signal->lastline==-1 ||
          b->signal->lastline>a->signal->lastline) a->signal->lastline=b->signal->lastline;

      if (b->signal->a.prev==NULL) cir->all_equi=b->signal->next;
      else b->signal->a.prev->next=b->signal->next;
      if (b->signal->next!=NULL) b->signal->next->a.prev=b->signal->a.prev;
      DelHeapItem(&cir->ha_equi, b->signal);
#ifdef ENABLE_STATS
      ____nbequi--;

#endif
      while (n!=NULL) { n->signal=a->signal; n=n->SUIV; }
    }
  else
    {
      b->signal->nbelem+=a->signal->nbelem;
//      b->signal->tag|=a->signal->tag;
      n=a->signal->nodes;
      b->signal->b.nbnodes+=a->signal->b.nbnodes;
      b->signal->lastnode->SUIV=a->signal->nodes;
      b->signal->lastnode=a->signal->lastnode;
      //b->signal->nodes=(__node_ *)append((chain_list *)b->signal->nodes, (chain_list *)a->signal->nodes);     
      if (b->signal->firstline==-1 ||
          a->signal->firstline<b->signal->firstline) b->signal->firstline=a->signal->firstline;
      if (b->signal->lastline==-1 ||
          a->signal->lastline>b->signal->lastline) b->signal->lastline=a->signal->lastline;
      if (a->signal->a.prev==NULL) cir->all_equi=a->signal->next;
      else a->signal->a.prev->next=a->signal->next;
      if (a->signal->next!=NULL) a->signal->next->a.prev=a->signal->a.prev;
      DelHeapItem(&cir->ha_equi, a->signal);
#ifdef ENABLE_STATS
      ____nbequi--;
#endif
      while (n!=NULL) { n->signal=b->signal; n=n->SUIV; }
    }
}

static char *vect(char *name, char *buf)
{
  if (vectorindex(name)==-1) return name;
  sprintf(buf,"%s[%d]",vectorradical(name),vectorindex(name));
  return buf;
}

static void _ParasiticDriveHeader(subckt *cir, char *name, chain_list *interf)
{
  time_t         secondes;
  struct tm     *jours;
  char tab[1024];
  __equi_ *e;
  chain_list *cl, *ch;
  __node_ *n, *n0;

  time( &secondes );
  jours = localtime( &secondes );
  
  sprintf( tab,
           "%d/%02d/%04d at %2d:%02d:%02d",
            jours->tm_mday,
            jours->tm_mon+1,
            jours->tm_year+1900,
            jours->tm_hour,
            jours->tm_min,
            jours->tm_sec
          );

  fprintf(cir->of, 
          "*SPEF \"IEEE 1481, 1998\"\n"
          "*DESIGN \"%s\"\n"
          "*DATE \"%s\"\n"
          "*VENDOR \"Avertec\"\n"
          "*PROGRAM \"Parasitic Driver\"\n"
          "*VERSION \"0.1b\"\n"
          "*DESIGN_FLOW \"EXTERNAL LOADS\"  \"EXTERNAL_SLEWS\"\n"
          "*DIVIDER .\n"
          "*DELIMITER :\n"
          "*BUS_DELIMITER []\n"
          "*T_UNIT 1 NS\n"
          "*C_UNIT 1.0 FF\n"
          "*R_UNIT 1.0 OHM\n"
          "*L_UNIT 1.0 HENRY\n",
          name, tab
          );

  if (cir->nbequi>0)
    {
      fprintf(cir->of, "\n*NAME_MAP\n");
      for (e=cir->all_equi; e!=NULL; e=e->next)
        {
          if (e->firstline!=-1)
            {
              fprintf(cir->of, "*%d %s\n", e->b.nbnodes, vect(e->a.nom, tab));
            }
        }
    }

  fprintf(cir->of, "\n*PORTS\n");
  for (cl=interf; cl!=NULL; cl=cl->NEXT)
    {
      if (cl->NEXT != NULL && strcmp ((char *)cl->NEXT->DATA, "=") == 0) break;
      n=ajoutenoeud( cir, (char *)cl->DATA, 0 );
      for (ch=interf; ch!=cl; ch=ch->NEXT)
        {
          n0=ajoutenoeud( cir, (char *)ch->DATA, 0 );
          if (n->signal->a.nom==n0->signal->a.nom) break;
        }
      if (ch==cl)
        {
          n=ajoutenoeud( cir, (char *)cl->DATA, 0 );
          fprintf(cir->of,"%s I\n", vect(n->signal->a.nom, tab));
        }
    }
  fprintf(cir->of,"\n");
}


static void namenode(__node_ *n, __names_ *name, char *buf)
{
  if (name!=NULL)
    {
      if (name->ins==NULL)
        sprintf(buf, "*%d", n->signal->b.nbnodes);
      else
        {
          sprintf(buf, "%s:%s", name->ins, name->con);
        }
    }
  else
    sprintf(buf, "*%d:%d", n->signal->b.nbnodes,n->index);
}

static void _ParasiticDriveNet(subckt *cir, __equi_ *e)
{
  __node_ *n;
  __item_ *i, *nexti;
  float totcapa;
  int num, done=0, hascon=0;
  char buf[1024], buf0[1024];
  __names_ *nam;

//  printf("ending %s\n",e->a.nom);
  for (i=e->items, totcapa=0; i!=NULL; i=i->next)
    {
      if (i->type=='c') totcapa+=i->value;
    }

  for (n=e->nodes; n!=NULL; n=n->SUIV) 
    if  (n->SUPINFO!=NULL && n->SUPINFO->names!=NULL) break;
  
  if (n!=NULL) hascon=1;
  
      done=1;
#ifdef FULLVALUE
      fprintf(cir->of, "\n*D_NET *%d %f\n",e->b.nbnodes,totcapa*1e15);
#else
      fprintf(cir->of, "\n*D_NET *%d %.3g\n",e->b.nbnodes,totcapa*1e15);
#endif
      if (hascon)
      {
      fprintf(cir->of, "*CONN\n");
      
//      for (n=e->nodes, num=0; n!=NULL; n=n->SUIV) n->index=num++;
      
      for (n=e->nodes; n!=NULL; n=n->SUIV)
        {
          for (nam=n->SUPINFO!=NULL?n->SUPINFO->names:NULL; nam!=NULL; nam=nam->next)
            {
              namenode(n, nam, buf);
              if (nam->ins==NULL)
                fprintf(cir->of, "*P %s I\n", buf);
              else
                fprintf(cir->of, "*I %s I\n", buf);
            }
        }
      }
      for (i=e->items; i!=NULL; i=i->next)
        {
          if (i->type=='c') break;
        }
      if (i!=NULL)
        {
          fprintf(cir->of, "\n*CAP\n");
      
          for (i=e->items, num=1; i!=NULL; i=i->next)
            {
              if (i->type=='c') 
                {
                  namenode(i->a, i->a->SUPINFO!=NULL?i->a->SUPINFO->names:NULL, buf);
                  namenode(i->b, i->b->SUPINFO!=NULL?i->b->SUPINFO->names:NULL, buf0);
#ifdef FULLVALUE
                  if (i->a->signal->tag & TAG_ALIM)
                    fprintf(cir->of, "%d %s %f\n", num++, buf0, i->value*1e15);
                  else if (i->b->signal->tag & TAG_ALIM)
                    fprintf(cir->of, "%d %s %f\n", num++, buf, i->value*1e15);
                  else 
                    fprintf(cir->of, "%d %s %s %f\n", num++, buf, buf0, i->value*1e15);
#else
                  if (i->a->signal->tag & TAG_ALIM)
                    fprintf(cir->of, "%d %s %.3f\n", num++, buf0, i->value*1e15);
                  else if (i->b->signal->tag & TAG_ALIM)
                    fprintf(cir->of, "%d %s %.3f\n", num++, buf, i->value*1e15);
                  else 
                    fprintf(cir->of, "%d %s %s %.3f\n", num++, buf, buf0, i->value*1e15);
#endif

                }
            }
        }

      for (i=e->items; i!=NULL; i=i->next)
        {
          if (i->type=='r') break;
        }

      num=1;
      if (i!=NULL)
        {
          fprintf(cir->of, "\n*RES\n");
      
          for (i=e->items; i!=NULL; i=i->next)
            {
              if (i->type=='r') 
                {
                  namenode(i->a, i->a->SUPINFO!=NULL?i->a->SUPINFO->names:NULL, buf);
                  namenode(i->b, i->b->SUPINFO!=NULL?i->b->SUPINFO->names:NULL, buf0);
#ifdef FULLVALUE
                  fprintf(cir->of, "%d %s %s %f\n", num++, buf, buf0, i->value);
#else
                  fprintf(cir->of, "%d %s %s %g\n", num++, buf, buf0, i->value);
#endif
                }
            }
          i++;
        }

#ifdef CORRECT_CONNECT
      for (n=e->nodes; n!=NULL; n=n->SUIV)
        {
          if (n->SUPINFO!=NULL && n->SUPINFO->names!=NULL && n->SUPINFO->names->next!=NULL)
            {
              if (i==NULL)
                {
                  i++;
                  fprintf(cir->of, "\n*RES\n");
                }

              namenode(n, n->SUPINFO->names, buf0);
              for (nam=n->SUPINFO->names->next; nam!=NULL; nam=nam->next)
                {
                  namenode(n, nam, buf);
                  fprintf(cir->of, "%d %s %s 0\n", num++, buf0, buf);
                }
            }
        }
#endif

  for (i=e->items; i!=NULL; i=nexti)
    {
      nexti=i->next;
      DelHeapItem(&ha_item, i);
    }
  freeitemspace+=e->nbelem;

  if (done) 
    fprintf(cir->of, "*END\n\n");
}

static void checkstart(__equi_ *e, int line)
{
  if ((e->tag & TAG_MASK)!=0) return;
  if (e->firstline==line && freeitemspace>=e->nbelem)
    {      
      e->tag|=1;
      e->items=NULL;
      freeitemspace-=e->nbelem;

//      printf("starting %s , %d, f=%d\n",e->a.nom,e->nbelem, freeitemspace);
      if (e->tag & TAG_ALIM) EXIT(6);
    }
/*  else  if (e->firstline==line && freeitemspace<e->nbelem)
    printf("[NOT STARTING] %s , %d, f=%d\n",e->a.nom,e->nbelem, freeitemspace);*/
}

static int checkend(__equi_ *e, int line)
{
  if ((e->tag & TAG_MASK)!=1) return 0;
  if (e->lastline==line)
    {
      e->tag|=2;
      return 1;
    }
  return 0;
}

static void CreateSpeforAnyOther(spifile *sf, chain_list *cl)
{
  chain_list *l, *e, *ch;
  subckt *cir=NULL;
  char *tmp;
  __equi_ *run_equi;
  int forcedexit=0;
  float       com_x, com_y;
  int         Tx, Ty, R, A;
  chain_list *fifodf=NULL, *elem;
#ifdef ENABLE_STATS
  // -----
  long df_temp0;
  long df_time=time(NULL), df_starttime=df_time;
  long df_temp=0;
  // -----
#endif

  l=lireligne(sf, cir);
            
  if (!spi_get_xy (l, &com_x, &com_y, &Tx, &Ty, &R, &A)) l = NULL;
  while (l!=NULL && forcedexit==0)
    {
#ifdef ENABLE_STATS
      if (++df_temp>=1024)
        {
          if ((df_temp0=time(NULL))!=df_time)
            {
              char temp[20];
              df_time=df_temp0;
              df_temp=0;
              if (avt_terminal(stdout))
                {
                  sprintf(temp,"[¤+%08d¤.]",sf->msl_line);
                  avt_fprintf(stdout,temp);
                  fflush(stdout);
                  avt_back_fprintf(stdout, strlen(temp)-4);
                }
            }
          df_temp=0;
        }
#endif
      if( strcasecmp( (char*)l->DATA, ".SUBCKT" ) == 0 )
        {
          if(cir)  
            avt_errmsg (SPI_ERRMSG, "015", AVT_FATAL, sf->filename, sf->msl_line );

          e=l->NEXT;
          tmp=namealloc((char *)e->DATA);

          for (ch=cl; ch!=NULL && strcasecmp(((subckt *)ch->DATA)->NOM,tmp)!=0; ch=ch->NEXT) /*printf(">>%s\n",((subckt *)ch->DATA)->NOM);*/;
          if (ch==NULL) EXIT(124);
          cir=(subckt *)ch->DATA;
          cir->TOTAL_LINE_COUNTER=0;
          if (cir->pass==0)
            {
              int i=1;
              char *where;
              if ((where=filepath(cir->NOM, "spef"))!=NULL)
                {
                  avt_log(-1, 0, "File '%s.spef' already exists, no spef generation done\n",where);
//                  printf("[info] using existing parasitic file '%s.spef'\n",cir->NOM);
//                  fclose(cir->of);
                  cir->of=NULL;
                  cir->nbequi=0;
                  
                  for (ch=cl; ch!=NULL; ch=ch->NEXT)
                    {
                      cir=(subckt *)ch->DATA;
                      if (((subckt *)ch->DATA)->nbequi!=0) break;
                    }
                  if (ch==NULL) // no more circuit to complete
                    forcedexit=1;
                }
              else
                {
                  if ((cir->of=mbkfopen(cir->NOM, "spef", WRITE_TEXT))==NULL)
                    EXIT(142);
                  for (run_equi=cir->all_equi; run_equi!=NULL; run_equi=run_equi->next)
                    if (run_equi->firstline!=-1) run_equi->b.nbnodes=i++;
                  // drive header
                  _ParasiticDriveHeader(cir, cir->NOM, e->NEXT);
                }
            }
          cir->pass++;
#ifdef ENABLE_STATS
          if (cir->of!=NULL) printf("%s pass %d\n",cir->NOM, cir->pass);
#endif
        }
      else if (strcasecmp( (char*)(l->DATA), ".ENDS" ) == 0 )
        {
//-----------------
//          cir->nbequi=0;
          if (cir->of!=NULL)
            {
              if (cir->nbequi==0) 
                {
                  fclose(cir->of);
//                  printf("closed %s\n",cir->NOM);
                }
#ifdef ENABLE_STATS
              else
                printf("%s, %d equi remaining\n-----------------\n",cir->NOM,cir->nbequi);
#endif
            }
          cir=NULL;
        }
      else if (cir!=NULL && cir->of!=NULL && (*(char*)l->DATA=='R' || *(char*)l->DATA=='r'))
        {
          __node_ *a, *b;
          __item_ *i;
          if (countchain(l)<4) 
              avt_errmsg (SPI_ERRMSG, "036", AVT_FATAL, sf->filename, sf->msl_line );
          a=ajoutenoeud( cir, (char *)l->NEXT->DATA, 0 ); //getnode(cir, (char *)l->NEXT->DATA, 0); // node0
          b=ajoutenoeud( cir, (char *)l->NEXT->NEXT->DATA, 0 ); //b=getnode(cir, (char *)l->NEXT->NEXT->DATA, 0); // node1
          // ---------
          checkstart(a->signal, cir->TOTAL_LINE_COUNTER/*sf->msl_line*/);
          if ((a->signal->tag & TAG_MASK)==1)
            {
              i=AddHeapItem(&ha_item);
              i->next=a->signal->items; a->signal->items=i;
              i->type='r';
              i->a=a; i->b=b;
              i->value = eqt_eval( GLOBAL_CTX, namealloc((char*)(l->NEXT->NEXT->NEXT->DATA)) ,EQTFAST ) * SPI_SCALE_RESIFACTOR ;
              if( !eqt_resistrue(GLOBAL_CTX) ) 
                avt_errmsg (SPI_ERRMSG, "034", AVT_FATAL, sf->filename, sf->msl_line );
            }
          if (checkend(a->signal, cir->TOTAL_LINE_COUNTER/*sf->msl_line*/))
            {
              _ParasiticDriveNet(cir, a->signal);
              cir->nbequi--;
            }
        }
      else if (cir!=NULL && cir->of!=NULL && (*(char*)l->DATA=='C' || *(char*)l->DATA=='c'))
        {
          __node_ *a, *b;
          __item_ *i;
          if (countchain(l)<4) 
                avt_errmsg (SPI_ERRMSG, "038", AVT_FATAL, sf->filename, sf->msl_line );
          a=ajoutenoeud( cir, (char *)l->NEXT->DATA, 0 ); //a=getnode(cir, (char *)l->NEXT->DATA, 0); // node0
          b=ajoutenoeud( cir, (char *)l->NEXT->NEXT->DATA, 0 ); //b=getnode(cir, (char *)l->NEXT->NEXT->DATA, 0); // node1
// ---------
          checkstart(a->signal, cir->TOTAL_LINE_COUNTER/*sf->msl_line*/);
          if ((a->signal->tag & TAG_MASK)==1)
            {
              i=AddHeapItem(&ha_item);
              i->next=a->signal->items; a->signal->items=i;
              i->type='c';
              i->a=a; i->b=b;
              i->value = eqt_eval( GLOBAL_CTX, namealloc((char*)(l->NEXT->NEXT->NEXT->DATA)) ,EQTFAST ) * SPI_SCALE_CAPAFACTOR ;
              if( !eqt_resistrue(GLOBAL_CTX) ) 
                avt_errmsg (SPI_ERRMSG, "034", AVT_FATAL, sf->filename, sf->msl_line );
            }
          if (checkend(a->signal, cir->TOTAL_LINE_COUNTER/*sf->msl_line*/))
            {
              _ParasiticDriveNet(cir, a->signal);
              cir->nbequi--;
            }

// ---------
          if (b->signal!=a->signal)
            {
              checkstart(b->signal, cir->TOTAL_LINE_COUNTER/*sf->msl_line*/);
              if ((b->signal->tag & TAG_MASK)==1)
                {
                  i=AddHeapItem(&ha_item);
                  i->next=b->signal->items; b->signal->items=i;
                  i->type='c';
                  i->a=a; i->b=b;
                  i->value = eqt_eval( GLOBAL_CTX, namealloc((char*)(l->NEXT->NEXT->NEXT->DATA)) ,EQTFAST ) * SPI_SCALE_CAPAFACTOR ;
                  if( !eqt_resistrue(GLOBAL_CTX) ) 
                        avt_errmsg (SPI_ERRMSG, "034", AVT_FATAL, sf->filename, sf->msl_line );
                }
              if (checkend(b->signal, cir->TOTAL_LINE_COUNTER/*sf->msl_line*/))
                {
                  _ParasiticDriveNet(cir, b->signal);
                  cir->nbequi--;
                }
            }
        }
      else if (strcasecmp ((char *)l->DATA, ".INCLUDE") == 0 || strcasecmp ((char *)l->DATA, ".INC") == 0 || strcasecmp ((char *)l->DATA, ".INCL") == 0) 
        {
          elem = l->NEXT;
          tmp = (char *)elem->DATA;
          if (*tmp == '\'')
            tmp++;
          if (tmp[strlen (tmp) - 1] == '\'')
            tmp[strlen (tmp) - 1] = 0;
          fifodf = addchain( fifodf, sf );
          sf = spifileopen(tmp,NULL,( SPI_PARSE_FIRSTLINE == PARSEFIRSTLINE_YES || SPI_PARSE_FIRSTLINE == PARSEFIRSTLINE_INC ) ? 1 : 0 );          
        }

      freechain(l);
      if ((l=lireligne(sf, cir))==NULL)
        {
          while (fifodf!=NULL && l==NULL)
            {
              spifileclose(sf) ;
              elem = fifodf;
              sf=(spifile*)fifodf->DATA;
              fifodf=fifodf->NEXT;
              elem->NEXT = NULL;
              freechain( elem );
              l=lireligne(sf, cir);
            }
        }
      if (!spi_get_xy (l, &com_x, &com_y, &Tx, &Ty, &R, &A)) l = NULL;
    }
  if (l!=NULL) freechain(l);
}


static void ParseAndReparseUntilTheEnd(char *name, char *ext, chain_list *cl)
{ 
  spifile *sf;
  int cnt;
  chain_list *ch;

  for (ch=cl, cnt=0; ch!=NULL; cnt+=((subckt *)ch->DATA)->nbequi, ch=ch->NEXT) ;
  while (cnt!=0)
    {
      sf=spifileopen(name, ext, ( SPI_PARSE_FIRSTLINE == PARSEFIRSTLINE_YES ) ? 1 : 0 );

      CreateSpeforAnyOther(sf, cl);
      
      spifileclose( sf );
      for (ch=cl, cnt=0; ch!=NULL; cnt+=((subckt *)ch->DATA)->nbequi, ch=ch->NEXT) ;
    }
}


void SpiceToSpef(char *name, char *ext, chain_list *cirs, long freespace)
{
  chain_list *cl=NULL;
  int maxelem=0;
  subckt *cir;
  __equi_ *e;
  __node_ *n;
  int cnt, cnt0, num;
  unsigned long freespaceUL;

  freespaceUL=((unsigned long )freespace)*1024*1024;

  for (cl=cirs; cl!=NULL; cl=cl->NEXT)
    {
      cir=(subckt *)cl->DATA;
      for (e=cir->all_equi, cnt0=0, cnt=0; e!=NULL; e=e->next, cnt++)
        {
          if (isvss(e->a.nom) || isglobalvss(e->a.nom) || isvdd(e->a.nom) || isglobalvdd(e->a.nom))
            e->tag = TAG_ALIM;
          else
            e->tag = 0;
          if (e->firstline==-1 || (e->tag & TAG_ALIM)!=0) { cnt0++; e->firstline=e->lastline=-1; }
          else if (e->nbelem>maxelem) maxelem=e->nbelem;

          for (n=e->nodes, num=1; n!=NULL; n=n->SUIV) n->index=num++;          

          //                            printf("[%d %d] %d elem, %s\n",e->firstline,e->lastline,e->nbelem, nom);
        }
      cir->nbequi=cnt-cnt0;
#ifdef ENABLE_STATS
      if (cir->nbequi>0)
        printf("%d nodes, %d equis (%d sans RC, max=%d)\n", cir->nbnodes, cnt,cnt0,maxelem);
#endif
    }

  freeitemspace=freespaceUL/sizeof(__item_);

  if (maxelem>freeitemspace)
    {
      fprintf(stderr,"Can not drive spef with the current space restriction (maximum: %d rc), needed %d\n",freeitemspace,maxelem);
      return; // free!!!
    }


  CreateHeap(sizeof(__item_), 8192, &ha_item);

#ifdef ENABLE_STATS
  printf("tempspace=%ldKo, %d rc elems\n",freespaceUL/1024,freeitemspace);
#endif
  ParseAndReparseUntilTheEnd(name, ext, cirs);
  DeleteHeap(&ha_item);
}


