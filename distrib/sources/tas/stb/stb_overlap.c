/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_overlap.c                                               */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                Gilles AUGUSTINS                                          */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include <limits.h>
#include <alloca.h>

#include MUT_H
#include STB_H

#include "stb_util.h"
#include "stb_transfer.h"
#include "stb_error.h"
#include "stb_overlap.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))  
#define NINCL     ((char)0)  
#define INCL      ((char)1)  

/*---------------------------------------------------------------------------\
|                                                                            |
|                               private variables                            |
|                                                                            |
\---------------------------------------------------------------------------*/

stbgap_list *HEAD_GAP;

/*---------------------------------------------------------------------------\
|                                                                            |
|                        manipulation des stbgap_list                        |
|                                                                            |
\---------------------------------------------------------------------------*/
/*****************************************************************************
*                               fonction stb_addgap()                        *
*****************************************************************************/

stbgap_list *stb_addgap (stbgap_list *head,  
                         long         start, 
                         char         sincl, 
                         long         end, 
                         char         eincl, 
                         chain_list  *signals)
{
    stbgap_list *ptgap;
    int           i;

    if (!HEAD_GAP) {
        #ifdef NOHEAPALLOC
        HEAD_GAP = (stbgap_list *)mbkalloc (sizeof (stbgap_list));
        HEAD_GAP->NEXT = NULL;
        #else
        HEAD_GAP = (stbgap_list *)mbkalloc (BUFSIZE * sizeof (stbgap_list));
        ptgap = HEAD_GAP;
        for (i = 1; i < BUFSIZE; i++) {
             ptgap->NEXT = ptgap + 1;
             ptgap++;
         }
         ptgap->NEXT = NULL;
         #endif
    }

    ptgap          = HEAD_GAP;
    HEAD_GAP       = HEAD_GAP->NEXT;
    ptgap->NEXT    = head;
    ptgap->START   = start;
    ptgap->SINCL   = sincl;
    ptgap->END     = end;
    ptgap->EINCL   = eincl;
    ptgap->SIGNALS = signals;
    
    return ptgap;
}

/*****************************************************************************
*                               fonction stb_freegap()                       *
*****************************************************************************/

void stb_freegap(stbgap_list *head)
{
    if (head->SIGNALS) {
        freechain (head->SIGNALS);
        head->SIGNALS = NULL;
    }
    if( head->NEXT ) {
        fprintf (stderr, "*** stb warning ***");
        fprintf (stderr, "  stb_freegap : not a single gap!\n");
        EXIT(1);
    }
    HEAD_GAP = (stbgap_list *)append ((chain_list *)head, (chain_list *)HEAD_GAP);
}

/*****************************************************************************
*                             fonction stb_freegaplist()                     *
*****************************************************************************/

void stb_freegaplist (stbgap_list *head)
{
    stbgap_list *pt;
    #ifdef NOHEAPALLOC
    stbgap_list *next;
    #endif
    
    for (pt = head; pt; pt = pt->NEXT)
        if (pt->SIGNALS) {
            freechain (pt->SIGNALS);
            pt->SIGNALS = NULL;
        }

    #ifdef NOHEAPALLOC
    for( pt = head ; pt ; pt = next ) {
      next = pt->NEXT;
      mbkfree( pt );
    }
    #else
    HEAD_GAP = (stbgap_list *)append ((chain_list *)head, (chain_list *)HEAD_GAP);
    #endif
}

/*****************************************************************************
*                             fonction stb_revgaplist()                      *
*****************************************************************************/

stbgap_list *stb_revgaplist (stbgap_list *head)
{
    stbgap_list *p;
    stbgap_list *q = NULL;
    
    if (!head) 
        return NULL; 
    
    while ((p = head->NEXT)) { 
        head->NEXT = q; 
        q = head; 
        head = p; 
    } 
    head->NEXT = q; 
    return head;
}

/*****************************************************************************
*                              fonction stb_dumpgaplist                      *
*****************************************************************************/

int stb_dumpgaplist (stbgap_list *list)
{
    stbgap_list *ptlist;
    chain_list  *ptch;

    for (ptlist = list; ptlist; ptlist = ptlist->NEXT) {
        if (ptlist->SINCL && ptlist->EINCL)
            printf (" * [%ld, %ld]\n", ptlist->START, ptlist->END);
        if (ptlist->SINCL && !ptlist->EINCL)
            printf (" * [%ld, %ld[\n", ptlist->START, ptlist->END);
        if (!ptlist->SINCL && ptlist->EINCL)
            printf (" * ]%ld, %ld]\n", ptlist->START, ptlist->END);
        if (!ptlist->SINCL && !ptlist->EINCL)
            printf (" * ]%ld, %ld[\n", ptlist->START, ptlist->END);
        for (ptch = ptlist->SIGNALS; ptch; ptch = ptch->NEXT)
            if (((ttvevent_list*)(ptch->DATA))->TYPE & TTV_NODE_UP)
                printf ("    - %s UP\n", ((ttvevent_list*)(ptch->DATA))->ROOT->NAME);
            else
                printf ("    - %s DOWN\n", ((ttvevent_list*)(ptch->DATA))->ROOT->NAME);
        printf ("\n");
    }

    return 1;
}

/*****************************************************************************
*                           fonction stb_creategaplist                       *
*****************************************************************************/

stbgap_list *stb_creategaplist (stbpair_list  *stbpair, ttvevent_list *event)
{
    stbpair_list  *ptpair;
    stbgap_list   *gaplist  = NULL;
    
    if (stbpair)  
        for (ptpair = stbpair; ptpair; ptpair = ptpair->NEXT) {
            gaplist = stb_addgap (gaplist, ptpair->D, INCL, ptpair->U, INCL, NULL);
            if (event)
                gaplist->SIGNALS = addchain (gaplist->SIGNALS, event);
        }
    return stb_revgaplist (gaplist);
}

/*****************************************************************************
*                           fonction stb_dupchain                            *
*****************************************************************************/

chain_list *stb_dupchain (chain_list *ch)
{
    chain_list *ptch;
    chain_list *dupch = NULL;
    for (ptch = ch; ptch; ptch = ptch->NEXT)
        dupch = addchain (dupch, ptch->DATA);
    return reverse (dupch);
}

/*****************************************************************************
*                           fonction stb_union                               *
*****************************************************************************/
/* realise l'union de gap1 dans gap0. gap1 et gap0 sont détruites           */
/****************************************************************************/

stbgap_list *stb_union (stbgap_list *gap0, stbgap_list *gap1)
{
  stbgap_list *headgap ;
  stbgap_list *endgap ;
  stbgap_list *gap ;
  stbgap_list *sgap0 ;
  stbgap_list *sgap1 ;
  int          alone ;

  sgap0   = gap0 ;
  sgap1   = gap1 ;
  headgap = NULL ;
  endgap  = NULL ;

  while( sgap0 || sgap1 ) {

    /* détermine si il y a un gap non recouvrant */
    alone = -1 ;
    
    if( !sgap0 ) 
      alone = 1 ;
      
    if( !sgap1 )
      alone = 0 ;

    if( alone == -1 ) {
    
      if( sgap0->END <= sgap1->START )
        alone = 0 ;
        
      if( sgap1->END <= sgap0->START )
        alone = 1 ;
    }

    if( alone != -1 ) {

      /* traitement du gap non recouvrant : pas de recopie de la liste
         des losig */

      if( alone == 0 ) {
        gap = sgap0 ;
        sgap0 = sgap0->NEXT ;
      }
      else {
        gap = sgap1 ;
        sgap1 = sgap1->NEXT ;
      }
     
      if( endgap ) 
        endgap->NEXT = gap ;
      else 
        headgap = gap ;

      endgap = gap ;
      gap->NEXT = NULL ;
      
    }
    else { /* ici, nous sommes sûr d'avoir sgap0 et sgap1 */

      if( sgap0->START == sgap1->START ) {

        if( sgap0->END  == sgap1->END ) {
            
          /* traitement de deux gap égaux : merge de deux liste, dont l'une ne
             contient qu'un seul élément (c'est sgap1) */

          gap = sgap0 ;
          sgap0 = sgap0->NEXT ;

          if( endgap )
            endgap->NEXT = gap ;
          else
            headgap = gap ;
          endgap = gap ;
          gap->NEXT = NULL ;

          gap->SIGNALS  = append( sgap1->SIGNALS, gap->SIGNALS );
          sgap1->SIGNALS = NULL ;
          
          gap = sgap1 ;
          sgap1 = sgap1->NEXT ;
          gap->NEXT = NULL ;
          stb_freegap( gap );
        }
        else {

          /* le plus long des deux gap est séparé en deux à la fin du gap
             le plus court */
          if( sgap0->END > sgap1->END ) {
            gap = stb_addgap( NULL,
                              sgap0->START,
                              1,
                              sgap1->END,
                              1,
                              dupchainlst( sgap0->SIGNALS )
                            );
            sgap0->START = sgap1->END ;
            gap->NEXT = sgap0 ;
            sgap0 = gap ;
          }
          else {
            gap = stb_addgap( NULL,
                              sgap1->START,
                              1,
                              sgap0->END,
                              1,
                              dupchainlst( sgap1->SIGNALS )
                            );
            sgap1->START = sgap0->END ;
            gap->NEXT = sgap1 ;
            sgap1 = gap ;
          }
        }
      }
      else {

        /* le gap qui commence le plus tôt est séparé en deux au début du 
           second gap */
        if( sgap0->START < sgap1->START ) {
          gap = stb_addgap( NULL,
                            sgap0->START,
                            1,
                            sgap1->START,
                            1,
                            dupchainlst( sgap0->SIGNALS )
                          );
          sgap0->START = sgap1->START ;
          gap->NEXT = sgap0 ;
          sgap0 = gap ;
        }
        else {
          gap = stb_addgap( NULL,
                            sgap1->START,
                            1,
                            sgap0->START,
                            1,
                            dupchainlst( sgap1->SIGNALS )
                          );
          sgap1->START = sgap0->START ;
          gap->NEXT = sgap1 ;
          sgap1 = gap ;
        }
      }
    }
  }
  
  return headgap;
}

/*---------------------------------------------------------------------------\
|                                                                            |
|                         manipulation des stb_pairlist                      |
|                                                                            |
\---------------------------------------------------------------------------*/
/*****************************************************************************
*                           fonction stb_dumppairlist                        *
*****************************************************************************/

int stb_dumppairlist(stbpair_list *list)
{
    stbpair_list *ptlist;

    for (ptlist = list; ptlist; ptlist = ptlist->NEXT)
        printf (" * [%ld, %ld] \n", ptlist->D, ptlist->U);

    return 1;
}

/*****************************************************************************
*                           fonction stb_beforedstbpair()                    *
*****************************************************************************/
/* renvoie le stbpair telque stbpair->D < d, NULL si il n'existe pas        */
/****************************************************************************/

stbpair_list *stb_beforedstbpair (stbpair_list *list, long d)
{
    stbpair_list *ptstbpair;
    stbpair_list *prev = NULL;

    ptstbpair = list;
    while (ptstbpair->D < d) {
        prev = ptstbpair;
        ptstbpair = ptstbpair->NEXT;
        if (!ptstbpair)
            break;
    }
    return prev;
}

/*****************************************************************************
*                           fonction stb_insertstbpair                       *
*****************************************************************************/
/* insere un stbpair dans l'ordre croissant des D                           */
/****************************************************************************/

stbpair_list *stb_insertstbpair ( stbpair_list *head, long d, long u)
{
    stbpair_list *ptprev, *ptbefore = NULL;

    if (head)
        ptbefore = stb_beforedstbpair(head, d);
    
    if (ptbefore) {
        ptprev = ptbefore;
        ptbefore = stb_addstbpair (ptbefore->NEXT, d, u);
        ptprev->NEXT = ptbefore;
    } else {
        head = stb_addstbpair (head, d, u);
    }
    
    return head;
}

/*****************************************************************************
*                           fonction stb_packstbpair                         *
*****************************************************************************/
/* Fusionne tous les intervalles d'un stbpair_list ayant une intersection.  */
/* Les intervalles sont supposes tries dans l'ordre croissant des D.        */
/****************************************************************************/

stbpair_list *stb_packstbpair (stbpair_list *list)
{
    stbpair_list *ptstbpair;
    stbpair_list *ptsav;

    ptstbpair = list; 

    while (ptstbpair) {
        if (ptstbpair->NEXT) {
            if (ptstbpair->U >= ptstbpair->NEXT->D) {
                ptstbpair->U    = MAX (ptstbpair->U, ptstbpair->NEXT->U);
                ptsav           = ptstbpair->NEXT;
                ptstbpair->NEXT = ptsav->NEXT;
                ptsav->NEXT     = NULL;
                stb_freestbpair (ptsav);
            } else
              ptstbpair = ptstbpair->NEXT; 
        } else 
        break; 
    }
    return list;
}

/*****************************************************************************
*                           fonction stb_periodpack                          *
*****************************************************************************/
/* cree une liste dans laquelle on a ramene tous les intervalles dans la    */
/* periode [0, T]                                                           */
/****************************************************************************/

stbpair_list *stb_periodpack (stbpair_list *list, long ckperiod)
{
    stbpair_list *elm, *reslist = NULL;
    long          d, u;

    if (ckperiod <= 0) {
        fflush (stdout);
        fprintf (stderr, "*** stb warning ***");
        fprintf (stderr, "  clock period < 0\n");
        return list;
    }
    
    for (elm = list; elm; elm = elm->NEXT) {
        if (elm->U - elm->D >= ckperiod) 
            reslist = stb_insertstbpair (reslist, 0, ckperiod);
        else {
            d = elm->D % ckperiod;
            u = elm->U % ckperiod;
            if (d <= u) {
                reslist = stb_insertstbpair (reslist, d, u);
            } else {
                reslist = stb_insertstbpair (reslist, 0, u);
                reslist = stb_insertstbpair (reslist, d, ckperiod);
            }
        }
    }
    stb_freestbpair (list);

    return stb_packstbpair (reslist);
}

/*****************************************************************************
*                           fonction stb_insertckpair                        *
*****************************************************************************/

stbpair_list *stb_insertckpair (stbpair_list *pair, stbnode *node, long type)
{
    char index ;
    
    if (node->CK ) {
        index = stb_getckindex( node->CK );
        if( index != STB_NO_INDEX ) {
            if (node->CK->TYPE == STB_TYPE_CLOCK ||
                node->CK->TYPE == STB_TYPE_COMMAND ||
                node->CK->TYPE == STB_TYPE_CLOCKPATH) {
                if (type & TTV_NODE_UP) {
                    pair = stb_insertstbpair (pair, node->CK->SUPMIN, node->CK->SUPMAX);
                    pair = stb_fusestbpair (pair);
                }
                else {
                    pair = stb_insertstbpair (pair, node->CK->SDNMIN, node->CK->SDNMAX);
                    pair = stb_fusestbpair (pair);
                }
            }
        }
    }
    
    return pair;
}

/****************************************************************************
*                           fonction stb_intersect                          *
*****************************************************************************/

int stb_intersect (stbpair_list *victim_pair, stbpair_list *aggr_pair)
{
    stbpair_list *v_pair, *a_pair;
    int           Dv, Uv;
    int           Da, Ua;

    for (v_pair = victim_pair; v_pair; v_pair = v_pair->NEXT) {
        Dv = v_pair->D;
        Uv = v_pair->U;
        for (a_pair = aggr_pair; a_pair; a_pair = a_pair->NEXT) {
            Da = a_pair->D;
            Ua = a_pair->U;
            if (Da >= Dv && Da < Uv)
                return 1;
            if (Ua > Dv && Ua <= Uv)
                return 1;
            if (Da <= Dv && Ua >= Uv)
                return 1;
            
        }
    }

    return 0;
}
/*---------------------------------------------------------------------------\
|                                                                            |
|                              fonctions overlap                             |
|                                                                            |
\---------------------------------------------------------------------------*/
/****************************************************************************\
*                           fonction stb_overlapdev                          * 
* Renvoie une liste d'intervales correspondant aux différents contextes      *
* (gap) d'agression. Le refevent et les event sont supposés n'appartenir     *
* qu'à un seul et meme domaine d'horloge.                                    *
\****************************************************************************/

stbgap_list *stb_overlapdev_old (stbfig_list *stbfig, ttvevent_list *refevent, chain_list *eventlist, long margin )
{
    stbpair_list  *pair_agr;
    chain_list    *scan_agr;
    ttvevent_list *agr;
    stbgap_list   *gap;
    stbgap_list   *resgap=NULL;
    int            n;

    n = stb_numberofdomain( stbfig, stb_getstbnode( refevent ) ) ;
    
    if( n == 1 || n==0 ) {

      for (scan_agr = eventlist; scan_agr; scan_agr = scan_agr->NEXT) {

          agr = (ttvevent_list*)scan_agr->DATA;
          pair_agr = stb_getpairnode( stbfig, agr, margin );

          gap   = stb_creategaplist( pair_agr, agr );
      
          resgap = stb_union( resgap, gap );

          stb_freestbpair( pair_agr );
      }
    }

    return resgap;
}

#define STB_MAX_DATE 50000

typedef struct
{
  ttvevent_list *tve;
  long date;
  int start;
} stb_date_info;

static int stb_sort_by_date(const void *a, const void *b)
{
  stb_date_info *sdia=(stb_date_info *)a, *sdib=(stb_date_info *)b;
  if (sdia->date<sdib->date) return -1;
  if (sdia->date>sdib->date) return 1;
  return 0;
}
stbgap_list *stb_overlapdev (stbfig_list *stbfig, ttvevent_list *refevent, chain_list *eventlist, long margin )
{
  stb_date_info sdi[STB_MAX_DATE];
  int nb, i, n;
  stbpair_list *pair_agr, *pair;
  ttvevent_list *agr;
  chain_list *cur_ag_list=NULL;
  stbgap_list   *gap=NULL;
  long startd=TTV_NOTIME;
  chain_list    *scan_agr;

  n = stb_numberofdomain( stbfig, stb_getstbnode( refevent ) ) ;
  
  if( n == 1 || n==0 ) 
    {
//      sdi=mbkalloc(sizeof(stb_date_info)*STB_MAX_DATE);
      nb=0;
      for (scan_agr = eventlist; scan_agr; scan_agr = scan_agr->NEXT)
        {
          agr = (ttvevent_list*)scan_agr->DATA;
          pair_agr = stb_getpairnode( stbfig, agr, margin );
          for (pair=pair_agr; pair!=NULL; pair=pair->NEXT)
            {
              sdi[nb].date=pair->D;
              sdi[nb].tve=agr;
              sdi[nb].start=1;
              nb++;
              sdi[nb].date=pair->U;
              sdi[nb].tve=agr;
              sdi[nb].start=0;
              nb++;
            }
          stb_freestbpair( pair_agr );
        }

      qsort(sdi, nb, sizeof(stb_date_info), stb_sort_by_date);

      for (i=0; i<nb; i++)
        {
          if (sdi[i].start)
            {
              startd=sdi[i].date;
              cur_ag_list=addchain(cur_ag_list, sdi[i].tve);
            }
          else
            {
              if (startd!=TTV_NOTIME)
                {
                  gap=stb_addgap (gap, startd, 0, sdi[i].date, 0, dupchainlst(cur_ag_list));
                  startd=TTV_NOTIME;
                }
              cur_ag_list=delchaindata(cur_ag_list, sdi[i].tve);
            }
        }
    }
  return stb_revgaplist (gap);
}





char stb_getckindex( stbck *cklist )
{
  char index ;
  
  if( cklist->CKINDEX == STB_NO_INDEX )
    index = cklist->CTKCKINDEX ;
  else
    index = cklist->CKINDEX ;

  return index ;
}

/****************************************************************************\
Renvoie le nb de domaines d'horloge auquel appartient le signal.
La valeur -1 est renvoyée si on est pas en mesure de déterminer à quelle 
domaine d'horloge appartient ce signal.
\****************************************************************************/
int stb_numberofdomain( stbfig_list *stbfig, stbnode *node )
{
  int              n ;
  int              i ;
  int              phase ;
  stbdomain_list  *ptdomain ;
  stbdomain_list **tabdomain ;
  stbck           *cklist ;
  char             index ;
  
  if( !node || node->NBINDEX == 0 )
    return -1 ;

  tabdomain = (stbdomain_list**)alloca( sizeof( stbdomain_list* ) * node->NBINDEX );
  for( phase = 0 ; phase < node->NBINDEX ; phase++ )
    tabdomain[ phase ] = NULL ;
  
  n = 0;

  for( cklist = node->CK ; cklist ; cklist = cklist->NEXT ) {
  
    index = stb_getckindex( cklist );
    if( index == STB_NO_INDEX )
      return -1 ;

    ptdomain = stb_getstbdomain( stbfig->CKDOMAIN, index );
   
    if( ptdomain ) {
      for( i = 0 ; i < n ; i++ ) {
        if( tabdomain[i] == ptdomain )
          break;
      }
      if( i == n ) {
        tabdomain[n] = ptdomain ;
        n++;
      }
    }
  }

  for( phase = 0 ; phase < node->NBINDEX ; phase++ ) {
  
    if( node->STBTAB[phase] ) {
    
      ptdomain = stb_getstbdomain( stbfig->CKDOMAIN, phase );
     
      if( ptdomain ) {
        for( i = 0 ; i < n ; i++ ) {
          if( tabdomain[i] == ptdomain )
            break;
        }
        if( i == n ) {
          tabdomain[n] = ptdomain ;
          n++;
        }
      }
    }
  }
  
  stbfig = NULL;
  return n;
}

/*****************************************************************************
* Renvoie la stabilité du premier domaine d'horloge d'un event en prenant en *
* compte :                                                                   *
* - La stabilité originale déterminée à vdd/2                                *
* - Les fronts                                                               *
* - Le réseau RC                                                             *
* - Une marge                                                                *
*****************************************************************************/
stbpair_list* stb_getpairnode( stbfig_list   *stbfig,
                               ttvevent_list *event,
                               long           margin
                             )
{
    stbpair_list   *pair;
    stbpair_list   *duppair;
    stbpair_list   *pairbefore;
    stbpair_list   *pairafter;
    long            period;
    long            search_type = TTV_FIND_MAX | TTV_FIND_RC ;
    stbnode        *stbnode;
    int             phase, i;
    long            rcmax;
    long            smax;
    stbdomain_list *ptdomain;
    stbdomain_list *tstdomain;
    char           domain;
    ptype_list *pt;
    stb_fastslope *sfs;

    if (event->TYPE & TTV_NODE_UP) i=1; else i=0;
    if ((pt=getptype(event->ROOT->USER, STB_CTK_FAST_SLOPE))!=NULL)
    {
      sfs=(stb_fastslope *)pt->DATA;
      if (sfs->ev[i].pairnode!=NULL)
         return stb_dupstbpairlist( sfs->ev[i].pairnode );
    }
    
    if (stbfig->GRAPH == STB_DET_GRAPH)
        search_type |= TTV_FIND_LINE;
    else
    if (stbfig->GRAPH == STB_RED_GRAPH)
        search_type |= TTV_FIND_PATH;

    stbnode = stb_getstbnode( event ); 
    
    ptdomain = NULL;
    pair     = NULL;
    
    for( phase=0 ; phase < stbnode->NBINDEX ; phase++ ) {
    
      tstdomain = NULL;
      domain = 'Y';
      
      if( stbnode->STBTAB[phase] ) {
        
        if( ptdomain == NULL ) {
          ptdomain = stb_getstbdomain( stbfig->CKDOMAIN, phase );
          if( !ptdomain )
            domain = 'N' ;
          tstdomain = ptdomain ;
        }
        else {
          tstdomain = stb_getstbdomain( stbfig->CKDOMAIN, phase );
          if( !tstdomain )
            domain = 'N' ;
          if( tstdomain != ptdomain )
            tstdomain = NULL;
        }
      }
  
      if( tstdomain || domain == 'N' ) {
        duppair = stb_dupstbpairlist( stbnode->STBTAB[phase] );
        pair    = stb_mergestbpairlist( pair, duppair );
      }
    }
    
    pair    = stb_insertckpair( pair, stbnode,  event ->TYPE );
    
    /* ajout delai RC max */
    rcmax = ttv_getdelaynode( stbfig->FIG, 
                              stbfig->FIG->INFO->LEVEL, 
                              event , 
                              search_type
                            );
    if( rcmax )
      pair = stb_adddelaystbpair( pair, rcmax, 0, 0, 0 );
   
    /* ajout des fronts */
    smax = ttv_getslopenode( stbfig->FIG,
                             stbfig->FIG->INFO->LEVEL,
                             event ,
                             search_type | TTV_FIND_GATE,
                             TTV_MODE_DELAY
                           );
    
    if( smax ) {
      if( ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) {
        if( STM_DEFAULT_SMAXR >= 0.0 && STM_DEFAULT_SMINR >= 0.0 )
          smax = (long)(((float)smax)/( STM_DEFAULT_SMAXR-STM_DEFAULT_SMINR ));
        else
          smax = (long)(((float)smax)/0.6);
      }
      else {
        if( STM_DEFAULT_SMAXF >= 0.0 && STM_DEFAULT_SMINF >= 0.0 )
          smax = (long)(((float)smax)/( STM_DEFAULT_SMAXF-STM_DEFAULT_SMINF ));
        else
          smax = (long)(((float)smax)/0.6);
      }
      pair = stb_adddelaystbpair( pair, smax/2.0, -smax/2.0, 0, 0 );
    }

    /* Ajout de la marge */
    if( margin )
      pair = stb_adddelaystbpair( pair, margin, -margin, 0, 0 );

    /* Duplication sur la période précédente et suivante */
    period = stb_getperiod( stbfig, phase );
    if( period != STB_NO_TIME ) {
      pairbefore = stb_dupstbpairlist( pair );
      pairbefore = stb_adddelaystbpair( pairbefore, 0, 0, period, period );
      pairafter  = stb_dupstbpairlist( pair );
      pairafter  = stb_adddelaystbpair( pairafter, 0, 0, -period, -period );
      pair       = stb_mergestbpairlist( pair, pairbefore );
      pair       = stb_mergestbpairlist( pair, pairafter );
    }

   if ((pt=getptype(event->ROOT->USER, STB_CTK_FAST_SLOPE))!=NULL)
    {
      sfs=(stb_fastslope *)pt->DATA;
      sfs->ev[i].pairnode=stb_dupstbpairlist( pair );
    }

    return pair;
}

/*****************************************************************************
* Renvoie la stabilité correspondant aux limites extrèmes des intervalles.   *
* - La stabilité originale déterminée à vdd/2                                *
* - Les fronts                                                               *
* - Le réseau RC                                                             *
* - Une marge                                                                *
*****************************************************************************/
stbpair_list* stb_getlimitpairnode( stbfig_list   *stbfig,
                                    ttvevent_list *event,
                                    long           margin
                                  )
{
    stbpair_list *pair=NULL;
    stbpair_list *scanpair;
    stbpair_list *pairbefore;
    stbpair_list *pairafter;
    long          period;
    long          search_type = TTV_FIND_MAX | TTV_FIND_RC;
    stbnode      *stbnode;
    int           phase;
    long          rcmax;
    long          smax;
    stbdomain_list *ptdomain;
    int i;
    ptype_list *pt;
    stb_fastslope *sfs;

    /*
    stbpair_list *allphasepair[256];
    */

    if (stbfig->GRAPH == STB_DET_GRAPH)
        search_type |= TTV_FIND_LINE;
    else
    if (stbfig->GRAPH == STB_RED_GRAPH)
        search_type |= TTV_FIND_PATH;

    stbnode  = stb_getstbnode( event ); 
    ptdomain = NULL ;

    for( phase=0 ; phase < stbnode->NBINDEX ; phase++ ) {
      if( stbnode->STBTAB[phase] ) {
        ptdomain = stb_getstbdomain( stbfig->CKDOMAIN, phase );
        break;
      }
    }
   
    period = stb_getperiod( stbfig, phase );
    rcmax = ttv_getdelaynode( stbfig->FIG, 
                              stbfig->FIG->INFO->LEVEL, 
                              event , 
                              search_type
                            );
    smax = ttv_getslopenode( stbfig->FIG,
                             stbfig->FIG->INFO->LEVEL,
                             event ,
                             search_type | TTV_FIND_GATE,
                             TTV_MODE_DELAY
                           );

    for( phase=0 ; phase < stbnode->NBINDEX ; phase++ ) {
      
      /*
      allphasepair[phase]=NULL;
      */

      if( !stbnode->STBTAB[phase] )
        continue ;
      if( stb_getstbdomain( stbfig->CKDOMAIN, phase ) != ptdomain )
        continue ;
      
      for( scanpair = stbnode->STBTAB[phase] ; 
           scanpair ; 
           scanpair = scanpair->NEXT ) {

        /*
        allphasepair[phase] = stb_addstbpair( allphasepair[phase],
                               scanpair->U - smax/2 - margin,
                               scanpair->U + smax/2 + margin + rcmax
                             );

        allphasepair[phase] = stb_addstbpair( allphasepair[phase],
                               scanpair->D - smax/2 - margin,
                               scanpair->D + smax/2 + margin + rcmax
                             );
        */
        pair = stb_insertstbpair( pair,
                               scanpair->U - smax/2 - margin,
                               scanpair->U + smax/2 + margin + rcmax
                             );

        pair = stb_insertstbpair( pair,
                               scanpair->D - smax/2 - margin,
                               scanpair->D + smax/2 + margin + rcmax
                             );
      }
      /*
      allphasepair[phase]=stb_fusestbpair(allphasepair[phase]);
      */
    }

    pair = stb_packstbpair( pair );

    /*
    pair = stb_globalstbtab(allphasepair, stbnode->NBINDEX);

    stb_freestbtabpair(allphasepair, stbnode->NBINDEX);
    */
      
    pair   = stb_insertckpair( pair, stbnode,  event ->TYPE );

    /* Duplication sur la période précédente et suivante */
    if( period != STB_NO_TIME ) {
      pairbefore = stb_dupstbpairlist( pair );
      pairbefore = stb_adddelaystbpair( pairbefore, 0, 0, period, period );
      pairafter  = stb_dupstbpairlist( pair );
      pairafter  = stb_adddelaystbpair( pairafter, 0, 0, -period, -period );
      pair       = stb_mergestbpairlist( pair, pairbefore );
      pair       = stb_mergestbpairlist( pair, pairafter );
    }

    return pair;
}

/*****************************************************************************
*                           fonction stb_overlap                             *
* Renvoie la liste des agresseurs d'un signal.                               *
* Arguments : stbfig : la vue stb.                                           *
*             victime : l'event victime.                                     *
*             aggr_list : une liste d'event agresseurs dont on veut tester   *
*                         si il sont agresseurs ou non. Il doivent tous etre *
*                         du meme domaine d'horloge que victim.              *
*****************************************************************************/

chain_list *stb_overlap ( stbfig_list   *stbfig, 
                          ttvevent_list *victim, 
                          chain_list    *aggr_list,
                          long           margin,
                          long           mode
                        )
{
    stbpair_list  *pair_vic;
    stbpair_list  *pair_agr;
    chain_list    *active_agr=NULL;
    chain_list    *scan_agr;
    ttvevent_list *agr;
    
    if( !aggr_list )
      return NULL;

    switch( stb_numberofdomain( stbfig, stb_getstbnode( victim ) ) ) {
    
    case -1 :
      active_agr = dupchainlst( aggr_list );
      break;
      
    case 0 :
    case 1 :
      switch( mode ) {
       case STB_STD:
         pair_vic = stb_getpairnode( stbfig, victim, margin );
         break;
       case STB_OBS:
         pair_vic = stb_getlimitpairnode( stbfig, victim, margin );
         break;
       }
      
      for (scan_agr = aggr_list; scan_agr; scan_agr = scan_agr->NEXT) {

          agr = (ttvevent_list*)scan_agr->DATA;

          pair_agr = stb_getpairnode( stbfig, agr, margin );

          if (stb_intersect( pair_vic, pair_agr ) )
                  active_agr = addchain( active_agr, agr );

          stb_freestbpair( pair_agr );
      }
      stb_freestbpair( pair_vic );
      break;
      
    default :
      active_agr = dupchainlst( aggr_list );
    }

    return active_agr;
}

/*****************************************************************************
* Renvoie la liste des signaux qui ne sont pas sur le meme domaine que le    *
* signal de référence. Si la phase de reference comporte plusieurs domaines, *
* tous les signaux sont supposés sur une phase différente.                   *
*****************************************************************************/
chain_list *stb_diftdomain( stbfig_list   *stbfig, 
                            ttvevent_list *ref,
                            chain_list    *testlist
                          )
{
  int              phase;
  stbnode         *node;
  stbnode         *teststb;
  ttvevent_list   *testevt;
  int              nbdomain;
  chain_list      *scan;
  chain_list      *retchain;
  stbdomain_list  *ptdomain;
  stbdomain_list  *tstdomain;
  int              n;
  char             index;

  node = stb_getstbnode( ref );
  nbdomain = stb_numberofdomain( stbfig, node );
  
  switch( nbdomain ) {
 
  case -1 :
    /* Cas tordu, le noeud n'est sur aucun domaine d'horloge. On suppose dans
       ce cas que tous les agresseurs sont sur un domaine différent. */
    retchain = dupchainlst( testlist );
    break;
    
  case 0 : 
    /* cas ou il n'y a pas d'horloge : on considère que les agresseurs qui
       n'ont pas d'horloge sont sur le même domaine. */
       
    retchain = NULL ;
    
    for( phase=0 ; phase < node->NBINDEX ; phase++ )
      if( node->STBTAB[phase] )
        break;
    
    for( scan = testlist ; scan ; scan = scan->NEXT ) {
    
      testevt = ((ttvevent_list*)scan->DATA);
      teststb = stb_getstbnode( testevt );
      tstdomain = NULL;
      
      for( n=0 ; n<teststb->NBINDEX ; n++ ) {
      
        if( teststb->STBTAB[n] && n != phase ) 
          break ;
      }
      if( n < teststb->NBINDEX ) {
        retchain = addchain( retchain, testevt );
      }
    }
    retchain = reverse( retchain );

    break;

  case 1 :
    /* Classique. On doit trouver parmis les agresseurs ceux qui sont sur un
       domaine différent. */
       
    retchain = NULL ;
    ptdomain = NULL ;
    
    if( node->CK ) {
      index = stb_getckindex( node->CK );
      if( index != STB_NO_INDEX )  
        ptdomain = stb_getstbdomain( stbfig->CKDOMAIN, index );
    }
    else {
      for( phase=0 ; phase < node->NBINDEX ; phase++ )
        if( node->STBTAB[phase] )
          break;
      if( phase < node->NBINDEX )
        ptdomain = stb_getstbdomain( stbfig->CKDOMAIN, phase );
    }
  
    if( ptdomain ) {
      for( scan = testlist ; scan ; scan = scan->NEXT ) {
      
        testevt = ((ttvevent_list*)scan->DATA);
        teststb = stb_getstbnode( testevt );
        tstdomain = NULL;
        
        for( n=0 ; n<teststb->NBINDEX ; n++ ) {
        
          if( teststb->STBTAB[n] ) {
          
            tstdomain = stb_getstbdomain( stbfig->CKDOMAIN, n );
            
            if( tstdomain != ptdomain ) {
              retchain = addchain( retchain, testevt );
              break;
            }
          }
        }
        if( !tstdomain ) {
          if( teststb->CK ) {
            index = stb_getckindex( teststb->CK );
            if( index < ptdomain->CKMIN ||
                index > ptdomain->CKMAX    )
              retchain = addchain( retchain, testevt );
          }
        }
      }
      retchain = reverse( retchain );
    }

    break;
    
  default :
    /* Cas rare : Le signal appartient à au moins deux domaines différents. On
       suppose dans ce cas que tous les agresseurs sont sur un domaine
       différent. */
    retchain = dupchainlst( testlist );
    break;
  }

  return retchain;
}

/*****************************************************************************
*                           fonction stb_makeanalyselist                     *
*****************************************************************************/
                           
chain_list *stb_mkanalyselist (ttvfig_list *fig)
{
    chain_list  *siglist = NULL;
    long         i;
    
    // les commandes internes 
    for (i = 0; i < fig->NBILCMDSIG; i++) {
        siglist = addchain (siglist, &fig->ILCMDSIG[i]->NODE[0]);
        siglist = addchain (siglist, &fig->ILCMDSIG[i]->NODE[1]);
    }
    // les signaux externes
    for (i = 1; i < fig->NBESIG; i++) {
        siglist = addchain (siglist, &fig->ESIG[i]->NODE[0]);
        siglist = addchain (siglist, &fig->ESIG[i]->NODE[1]);
    }
    // les connecteurs externes
    //for (i = 0; i < fig->NBNCSIG; i++) {
    //  siglist = addchain (siglist, &fig->NCSIG[i]->NODE[0]);
    //  siglist = addchain (siglist, &fig->NCSIG[i]->NODE[1]);
    //}
    // les connecteurs externes
    for (i = 0; i < fig->NBCONSIG; i++) {
        siglist = addchain (siglist, &fig->CONSIG[i]->NODE[0]);
        siglist = addchain (siglist, &fig->CONSIG[i]->NODE[1]);
    }
    // les commandes externes 
    for (i = 0; i < fig->NBELCMDSIG; i++) {
        siglist = addchain (siglist, &fig->ELCMDSIG[i]->NODE[0]);
        siglist = addchain (siglist, &fig->ELCMDSIG[i]->NODE[1]);
    }

    // les latchs 
    for (i = 1; i < fig->NBELATCHSIG; i++) {
        siglist = addchain (siglist, &fig->ELATCHSIG[i]->NODE[0]);
        siglist = addchain (siglist, &fig->ELATCHSIG[i]->NODE[1]);
    }

    // les latchs 
    //for (i = 0; i < fig->NBILATCHSIG; i++) {
    //  siglist = addchain (siglist, &fig->ILATCHSIG[i]->NODE[0]);
    //  siglist = addchain (siglist, &fig->ILATCHSIG[i]->NODE[1]);
    //}

    return siglist;
}

