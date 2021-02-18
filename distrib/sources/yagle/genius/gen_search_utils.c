/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_search_utils.c                                          */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 19/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>

#include AVT_H
#include MUT_H
#include MLO_H
#include RCN_H
#include FCL_H
#include CNS_H
#include MSL_H
#include API_H
#include GEN_H
#include "gen_execute_VHDL.h"
#include "gen_model_utils.h"
#include "gen_model_transistor.h"
#include "gen_corresp.h"
#include "gen_lofig.h"
#include "gen_search_utils.h"
#include "gen_env.h"
#include "gen_symmetric.h"
#include "gen_optimizations.h"
#include "gen_new_kind.h"
#include "gen_display.h"
#include "gen_model_global.h"

char tabs[200]="";

#define MAX_COUNT_PENDING 0 // deactivated

/*  internal type */
typedef struct foundmodel {
   struct foundmodel *NEXT;       /*an another model...*/
   foundins_list *INS;        /*list of instances found in netlist*/
   short CHECK;       /*flag to 1 if searched by an entire scope of netlist*/
   int COUNT;                 /*number of instance found listed in INS field*/
   model_list *MODEL;      /*model describtion*/
} foundmodel_list;

typedef struct
{
  locon_list *lc;
  int oldvisited;
} locon_mark;

foundmodel_list *FOUND_MODEL=NULL; /*models with their instances in circuit*/

/*list of loins recognized in model , type is the pointer of the matching model instance, DATA the list ofvisited  loins last time*/
static chain_list *MEMO_MARK;       /*list of last marks put on model signals*/
static chain_list *STACK_MARK;          /* stack of last searchings */
static chain_list *SHIFT_LOINS;           /*list of foundins*/
static chain_list *STACK_SHIFT_LOINS;
static chain_list *SHIFT_LOTRS;
static chain_list *STACK_SHIFT_LOTRS;

/* Zinaps: STACK,... not initialised at first, the programmer certainly knows what he was doing here */

/*to accelerate memory allocation*/
static foundins_list *HEAD_FOUNDINS=NULL;           /*list of allocated foundins*/

// Zinaps: for myself, i initialize the variables
static chain_list *MEMO_FIXED=NULL;       /*list of last fixed connectors*/
static chain_list *STACK_FIXED=NULL;      /* stack of last searchings */
/*
  A fixed connector is a connector that can't be swapped anymore
  MEMO_FIXED and STACK_FIXED keep track of the fixed connectors
*/

static chain_list *MEMO_LOCON_MARK=NULL;  /*list of last mark in connectors*/
static chain_list *STACK_LOCON_MARK=NULL; /* stack of last searchings */
/*
  A locon mark is a direct link from a connector to the corresponding
  model connector mark, it's there a accelerate the algorithm
  eg.
   model con X corresponding to circuit con Y is marked by signal S
   the mark is kept in the model signal linked to model con X
   with locon mark, we find the mark directly knowing Y
*/

static chain_list *MEMO_SWAP=NULL;        /*list of last mark in connectors*/
static chain_list *STACK_SWAP=NULL;       /* stack of last searchings */
/*
  MEMO_SWAP and STACK_SWAP keep track of the swapped connector
  so in case of failure we can go back to the way the connectors
  were linked
*/

static chain_list *MEMO_MARK_COUNT=NULL;
static chain_list *STACK_MARK_COUNT=NULL;
/*
  MEMO_MARK_COUNT and STACK_MARK_COUNT keep track of the swapped connector
  so in case of failure we can go back to the way the connectors
  were linked
*/

static ptype_list *MEMO_WITHOUT_COUNT=NULL;
static chain_list *STACK_WITHOUT_COUNT=NULL;

// Displays all found instances/models
void displaymodels()
{
  foundmodel_list *fm;
  int i;
  foundins_list *fi;

  gen_printf(0,"-*--------------------------------\n");
  for (fm=FOUND_MODEL;fm!=NULL;fm=fm->NEXT)
    {
      gen_printf(0,"\t%s [%d]\n",fm->MODEL->NAME,fm->COUNT);
      for (i=0,fi=fm->INS;i<20 && i<fm->COUNT;i++,fi=fi->NEXT)
        gen_printf(0,"\t\t%s\n",fi->LOINS->INSNAME);
      if (fi!=NULL && i>=20) gen_printf(0,"\t\t ... etc ...\n");
    }
  gen_printf(0,"-*--------------------------------\n");
}


void SearchInit()
{
  FOUND_MODEL=NULL;
  MEMO_MARK=NULL;
  STACK_MARK=NULL;
  MEMO_MARK_COUNT=NULL;
  STACK_MARK_COUNT=NULL;
  SHIFT_LOINS=NULL;
  STACK_SHIFT_LOINS=NULL;
  SHIFT_LOTRS=NULL;
  STACK_SHIFT_LOTRS=NULL;
  HEAD_FOUNDINS=NULL;
  MEMO_FIXED=NULL;
  STACK_FIXED=NULL;
  MEMO_LOCON_MARK=NULL;
  STACK_LOCON_MARK=NULL;
  MEMO_SWAP=NULL; 
  STACK_SWAP=NULL;
  MEMO_WITHOUT_COUNT=NULL;
  STACK_WITHOUT_COUNT=NULL;
}


void MarkEAUpdateFunc(void **arraypos)
{
  ((mark_list *)*arraypos)->mark_in_ea=arraypos;
}

/****************************************************************************/
/*               put a new elem in DATA field of head                       */
/****************************************************************************/
static inline foundins_list* addfoundins()
{
   foundins_list *pt;
   register int i;

   if (HEAD_FOUNDINS == NULL) {
      pt = (foundins_list *)mbkalloc(BUFSIZE0*sizeof(foundins_list));
      blocks_to_free=addchain(blocks_to_free,pt);
      HEAD_FOUNDINS = pt;
      for (i = 1; i < BUFSIZE0; i++) {
         pt->NEXT = pt + 1;
         pt++;
      }
      pt->NEXT = NULL;
   }

   pt = HEAD_FOUNDINS;
   HEAD_FOUNDINS = HEAD_FOUNDINS->NEXT;
   
   return pt;
}


/****************************************************************************/
/*              erase recursively from memory a list of marks               */
/****************************************************************************/
static inline void freefoundins(foundins_list *top)
{
   HEAD_FOUNDINS = (foundins_list*)append((chain_list*)top, (chain_list*)HEAD_FOUNDINS);
}


/****************************************************************************/
/*            erase all marks of a search put in a model                    */
/****************************************************************************/
static inline void freeallmarks(model_list* model)
{
   losig_list *losig;
   ptype_list *p,*mark;
   mark_list *load;
   
   
   if (STACK_MARK) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 111);
//      fprintf(stderr,"freeallmarks: Stack not empty\n");
      EXIT(1);
   }
   
   freechain(MEMO_MARK);
   MEMO_MARK=NULL;
   
   for (losig=model->LOFIG->LOSIG; losig; losig=losig->NEXT) {
      mark=getptype(losig->USER,GEN_MARK_PTYPE);
      if (mark) {
         /*free mark in circuit*/
         for (load=mark->DATA; load; load=load->NEXT) {
            /*erase mark in circuit*/
            p=getptype(load->LOSIG->USER,GEN_VISITED_PTYPE);
            if (p) {  /*no PTYPE means it has been erased before*/
               freeptype(p->DATA);
               /*there is also LOFIGCHAIN */
               load->LOSIG->USER=delptype(load->LOSIG->USER,GEN_VISITED_PTYPE);
               if (!load->LOSIG->USER) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 112);
//                  fprintf(stderr,"freeallmarks: no LOFIGCHAIN\n");
                  EXIT(3);
               }
            }
            if (load->mark_in_ea!=NULL) *load->mark_in_ea=NULL;
         }   
         freemark(mark->DATA);
         mark->DATA=NULL;
      }   
   }   
}

static inline void freeallmarkcounts()
{
   
   if (STACK_MARK_COUNT) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 113);
//      fprintf(stderr,"freeallmarkcounts: Stack not empty\n");
      EXIT(1);
   }
   
   freechain(MEMO_MARK_COUNT);
   MEMO_MARK_COUNT=NULL;
}

static inline void freeallwithoutcounts()
{
   
   if (STACK_WITHOUT_COUNT) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 114);
//      fprintf(stderr,"freeallmarkcounts: Stack not empty\n");
      EXIT(1);
   }
   
   freeptype(MEMO_WITHOUT_COUNT);
   MEMO_WITHOUT_COUNT=NULL;
}

void FinishCleanLofigMarks(lofig_list *lf)
{
  losig_list *losig;
  ptype_list *mark;
  mark_list *load;
  
  for (losig=lf->LOSIG; losig; losig=losig->NEXT) 
    {
      mark=getptype(losig->USER,GEN_MARK_PTYPE);
      if (mark) 
        {
          for (load=(mark_list *)mark->DATA; load!=NULL; load=load->NEXT)
            {
              if (load->mark_in_ea!=NULL) *load->mark_in_ea=NULL;
            }
          freemark(load);
          losig->USER=delptype(losig->USER,GEN_MARK_PTYPE);
        }   
    }   
}

void CleanUPMarkEA(lofig_list *lf)
{
  losig_list *losig;
  ptype_list *mark;

  for (losig=lf->LOSIG; losig; losig=losig->NEXT) 
    {
      mark=getptype(losig->USER,  GENIUS_EXP_ARRAY_PTYPE);
      if (mark) 
        {
          DeleteExpandableArray((ExpandableArray *)mark->DATA);
          losig->USER=delptype(losig->USER, GENIUS_EXP_ARRAY_PTYPE);
        }
    }   
}

mark_list *GetMarkEA(losig_list *ls, int index, mark_list *markl)
{
  ptype_list *pea;
  ExpandableArray *ea;
  mark_list *mark;
  if ((pea=getptype(ls->USER,  GENIUS_EXP_ARRAY_PTYPE))==NULL)
    {
      for (mark=markl; mark; mark=mark->NEXT)
        if (mark->BIT==index) break;
      return mark;
    }
  ea=(ExpandableArray *)pea->DATA;
  return *GetExpandableArrayItem(ea, index, MarkEAUpdateFunc);
}

static inline void freeallswaps()
{
   chain_list *cl;
   
   if (STACK_SWAP) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 115);
//      fprintf(stderr,"freeallswaps: Stack not empty\n");
      EXIT(1);
   }
   
   for (cl=MEMO_SWAP;cl!=NULL;cl=cl->NEXT)
     JustFreeSwap(cl->DATA);
   //mbkfree(cl->DATA);

   freechain(MEMO_SWAP);
   MEMO_SWAP=NULL;
}

HeapAlloc loconmark_heap;

void LoconMarkHeap_Manage(int mode)
{
  if (mode==0)
    CreateHeap(sizeof(locon_mark), 0, &loconmark_heap);
  else
    DeleteHeap(&loconmark_heap);
}

static inline void freeallfixedandallloconmarks()
{
   locon_list *lc;
   ptype_list *p;
   chain_list *cl;
   locon_mark *lcm;

   if (STACK_FIXED) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 116);
//      fprintf(stderr,"freeallfixed: Stack not empty\n");
      EXIT(1);
   }
   if (STACK_LOCON_MARK) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 117);
//      fprintf(stderr,"freeallloconmarks: Stack not empty\n");
      EXIT(1);
   }
   
   
   for (cl=MEMO_FIXED;cl!=NULL;cl=cl->NEXT)
     {
       lc=(locon_list *)cl->DATA;
       p=getptype(lc->USER,GEN_FIXED_PTYPE); 
       if (p==NULL) { avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 118);
//               fprintf(stderr,"freeallfixedandallloconmarks: ptype NULL\n");
               EXIT(5);}
       lc->USER=delptype(lc->USER, GEN_FIXED_PTYPE);
     }
   
   for (cl=MEMO_LOCON_MARK;cl!=NULL;cl=cl->NEXT)
     {
       lcm=(locon_mark *)cl->DATA;
       p=getptype(lcm->lc->USER,GEN_LOCON_MARK_PTYPE); 
       if (p==NULL) { avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 119);
               //fprintf(stderr,"freeallfixedandallloconmarks: ptype NULL\n");
               EXIT(6);}
       lcm->lc->USER=delptype(lcm->lc->USER, GEN_LOCON_MARK_PTYPE);
       DelHeapItem(&loconmark_heap, lcm);
       //mbkfree(lcm);
     }   
   freechain(MEMO_FIXED);
   MEMO_FIXED=NULL;
   freechain(MEMO_LOCON_MARK);
   MEMO_LOCON_MARK=NULL;
}

/****************************************************************************/
/*             free last marks contained in Stack                           */
/****************************************************************************/
static inline void freelastmarks()
{
   mark_list *mark,*load;
   chain_list *l;
   ptype_list *del,*pred;
   ptype_list *p;
   
   if (!STACK_MARK) {
       avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 120);
//      fprintf(stderr,"freelastmarks: No Stack\n");
      EXIT(1);
   }

//   gen_printf(5,"want to go  back to %p\n",STACK_MARK->DATA);
   
   for (l=MEMO_MARK;l;l=l->NEXT) {
      load=(mark_list*)l->DATA;
//      gen_printf(5,"- %s  next=%p current=%p\n",load->LOSIG->NAMECHAIN->DATA,l->NEXT,l);fflush(GENIUS_OUTPUT);
      /*erase mark in circuit*/
      p=getptype(load->LOSIG->USER,GEN_VISITED_PTYPE);
      if (!p) {
       avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 121);
//         fprintf(stderr,"freelastmarks: PTYPE not found\n");
         EXIT(1);
      }   
      pred=NULL;
      for (del=(ptype_list*)p->DATA; del; del=del->NEXT) { 
         if (del->DATA==load) break;
         pred=del;
      }   
      if (!del) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 122);
//         fprintf(stderr,"freelastmarks: mark not found in circuit\n");
         EXIT(3);
      }   
      if (pred) pred->NEXT=del->NEXT;
      else p->DATA=del->NEXT;
      del->NEXT=NULL;        /*cut recursivity of freeptype() */
      freeptype(del);
      
      /*erase mark in model*/
      p=load->ROOT;
      if (p->DATA==load) {
         p->DATA=load->NEXT;
      }
      else {
         for (mark=(mark_list*)p->DATA; mark; mark=mark->NEXT) 
            if (mark->NEXT==load) break;
         if (!mark) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 123);
//            fprintf(stderr,"freelastmarks: mark not found in model\n");
            EXIT(1);
         }
         mark->NEXT=load->NEXT;
      }
      if (load->mark_in_ea!=NULL) *load->mark_in_ea=NULL;
      load->NEXT=NULL;     /* cut recursivity of freemark() */
      freemark(load);
   }
   
   /* global var. */
   freechain(MEMO_MARK);
   MEMO_MARK=STACK_MARK->DATA;
   l=STACK_MARK;
   STACK_MARK=STACK_MARK->NEXT;
   l->NEXT=NULL;
   freechain(l);
//   gen_printf(5,"BAD back to %p\n",MEMO_MARK);
}

static inline void freelastmarkcounts()
{
   mark_list  *load;
   chain_list *l;

   if (!STACK_MARK_COUNT) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 124);
//      fprintf(stderr,"freelastmarks: No Stack\n");
      EXIT(1);
   }
   
   for (l=MEMO_MARK_COUNT;l;l=l->NEXT) {
      load=(mark_list*)l->DATA;
      load->COUNT--;
   }
   
   /* global var. */
   freechain(MEMO_MARK_COUNT);
   MEMO_MARK_COUNT=STACK_MARK_COUNT->DATA;
   l=STACK_MARK_COUNT;
   STACK_MARK_COUNT=STACK_MARK_COUNT->NEXT;
   l->NEXT=NULL;
   freechain(l);
}

static inline void freelastwithoutcounts()
{
   ptype_list *l, *p;
   chain_list *cl;

   if (!STACK_WITHOUT_COUNT) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 125);
//      fprintf(stderr,"freelastwithoutcounts: No Stack\n");
      EXIT(1);
   }
   
   for (l=MEMO_WITHOUT_COUNT;l;l=l->NEXT) 
     {
       if ((p=getptype(((loins_list *)l->DATA)->USER, GENIUS_LOINS_COUNT_PTYPE))==NULL) EXIT(122);
       p->DATA=(void *)l->TYPE;
//       printf("<count back to %d>",(int)p->DATA);
     }
   
   /* global var. */
   freeptype(MEMO_WITHOUT_COUNT);
   MEMO_WITHOUT_COUNT=STACK_WITHOUT_COUNT->DATA;
   cl=STACK_WITHOUT_COUNT;
   STACK_WITHOUT_COUNT=STACK_WITHOUT_COUNT->NEXT;
   cl->NEXT=NULL;
   freechain(cl);
}

static inline void freelastswaps()
{
   chain_list *l;
   
   if (!STACK_SWAP) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 126);
//      fprintf(stderr,"freelastswaps: No Stack\n");
      EXIT(1);
   }
   
   for (l=MEMO_SWAP;l;l=l->NEXT) 
     {
       UndoAndDeleteSwap(l);
     }   
   
   /* global var. */
   freechain(MEMO_SWAP);
   MEMO_SWAP=STACK_SWAP->DATA;
   l=STACK_SWAP;
   STACK_SWAP=STACK_SWAP->NEXT;
   l->NEXT=NULL;
   freechain(l);
}

/*             free last fixed contained in Stack                   */
static inline void freelastfixed()
{
//   mark_list *mark,*load;
   chain_list *l;
   ptype_list *p;
   locon_list *lc;

   if (!STACK_FIXED) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 127);
      //fprintf(stderr,"freelastfixed: No Stack\n");
      EXIT(1);
   }
   
   for (l=MEMO_FIXED;l;l=l->NEXT) {
      lc=(locon_list *)l->DATA;
      /*erase mark in circuit*/
      // a supprimer 
      p=getptype(lc->USER,GEN_FIXED_PTYPE); 
      if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 128);
//       fprintf(stderr,"freelastfixed: PTYPE not found\n");
         EXIT(1);
      }
      // jusqu'ici
      lc->USER=delptype(lc->USER, GEN_FIXED_PTYPE);
    }
   
   /* global var. */
   freechain(MEMO_FIXED);
   MEMO_FIXED=STACK_FIXED->DATA;
   l=STACK_FIXED;
   STACK_FIXED=STACK_FIXED->NEXT;
   l->NEXT=NULL;
   freechain(l);
}

static inline void freelastloconmark()
{
   chain_list *l;
   ptype_list *p;
   locon_mark *lcm;

   if (!STACK_LOCON_MARK) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 129);
//      fprintf(stderr,"freelastloconmark: No Stack\n");
      EXIT(1);
   }

   for (l=MEMO_LOCON_MARK;l;l=l->NEXT) {
      lcm=(locon_mark *)l->DATA;
      /*erase mark in circuit*/
      // a supprimer 
      p=getptype(lcm->lc->USER,GEN_LOCON_MARK_PTYPE); 
      if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 130);
//         fprintf(stderr,"freelastloconmark: PTYPE not found\n");
         EXIT(1);
      }
      ((mark_list *)p->DATA)->VISITED=lcm->oldvisited;
      // jusqu'ici
      lcm->lc->USER=delptype(lcm->lc->USER, GEN_LOCON_MARK_PTYPE);
      DelHeapItem(&loconmark_heap, lcm);
      //mbkfree(lcm);
   }
   
   /* global var. */
   freechain(MEMO_LOCON_MARK);
   MEMO_LOCON_MARK=STACK_LOCON_MARK->DATA;
   l=STACK_LOCON_MARK;
   STACK_LOCON_MARK=STACK_LOCON_MARK->NEXT;
   l->NEXT=NULL;
   freechain(l);
}

/****************************************************************************/
/*               increase level of stack for marks                          */
/****************************************************************************/
int tm=0;

static inline void certifylastmarks()
{
  tm++;
  STACK_MARK=addchain(STACK_MARK,MEMO_MARK);
// gen_printf(5,"%d: SAVE %p (%p)\n",tm,STACK_MARK->DATA,&(STACK_MARK->DATA));
/*  {
    chain_list *cl;
    gen_printf(0,"STACK: ");
    for (cl=STACK_MARK; cl!=NULL;cl=cl->NEXT)
      gen_printf(0,"[%p: %p]",cl, cl->DATA);
      gen_printf(0,"\n");
  }*/
  MEMO_MARK=NULL;
}

static inline void certifylastmarkcounts()
{
   STACK_MARK_COUNT=addchain(STACK_MARK_COUNT,MEMO_MARK_COUNT);
   MEMO_MARK_COUNT=NULL;
}

static inline void certifylastwithoutcounts()
{
   STACK_WITHOUT_COUNT=addchain(STACK_WITHOUT_COUNT,MEMO_WITHOUT_COUNT);
   MEMO_WITHOUT_COUNT=NULL;
}

/*               increase level of stack for fixed connectors               */
static inline void certifylastfixed()
{
   STACK_FIXED=addchain(STACK_FIXED,MEMO_FIXED);
   MEMO_FIXED=NULL;
}

static inline void certifylastloconmark()
{
   STACK_LOCON_MARK=addchain(STACK_LOCON_MARK,MEMO_LOCON_MARK);
   MEMO_LOCON_MARK=NULL;
}

static inline void certifylastswap()
{
   STACK_SWAP=addchain(STACK_SWAP,MEMO_SWAP);
   MEMO_SWAP=NULL;
}

/****************************************************************************/
/*          reduce current level with last level for marks                  */
/****************************************************************************/
static inline void flattenlastmarks()
{
   chain_list *l,*del;
   
   if (!STACK_MARK) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 131);
//      fprintf(stderr,"flattenlastmarks: No Stack\n");
      EXIT(1);
   }
   
   for (l=MEMO_MARK; l; l=l->NEXT) if (!l->NEXT) break;
//   gen_printf(5,"FLAT %p with %p\n",MEMO_MARK,STACK_MARK->DATA);
   if (l) l->NEXT=STACK_MARK->DATA;
   else MEMO_MARK=STACK_MARK->DATA;
   del=STACK_MARK;
   STACK_MARK=STACK_MARK->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);
}
static inline void flattenlastmarkcounts()
{
   chain_list *l,*del;
   
   if (!STACK_MARK_COUNT) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 132);
//      fprintf(stderr,"flattenlastmarkcounts: No Stack\n");
      EXIT(1);
   }
   
   for (l=MEMO_MARK_COUNT; l; l=l->NEXT) if (!l->NEXT) break;
   if (l) l->NEXT=STACK_MARK_COUNT->DATA;
   else MEMO_MARK_COUNT=STACK_MARK_COUNT->DATA;
   del=STACK_MARK_COUNT;
   STACK_MARK_COUNT=STACK_MARK_COUNT->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);
}
static inline void flattenlastwithoutcounts()
{
   chain_list *del;
   ptype_list *l;
   
   if (!STACK_WITHOUT_COUNT) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 133);
//      fprintf(stderr,"flattenlastmarkcounts: No Stack\n");
      EXIT(1);
   }
   
   for (l=MEMO_WITHOUT_COUNT; l; l=l->NEXT) if (!l->NEXT) break;
   if (l) l->NEXT=STACK_WITHOUT_COUNT->DATA;
   else MEMO_WITHOUT_COUNT=STACK_WITHOUT_COUNT->DATA;
   del=STACK_WITHOUT_COUNT;
   STACK_WITHOUT_COUNT=STACK_WITHOUT_COUNT->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);
}
/*          reduce current level with last level for fixed                  */
static inline void flattenlastfixed()
{
   chain_list *l,*del;
   
   if (!STACK_FIXED) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 134);
//      fprintf(stderr,"flattenlastfixed: No Stack\n");
      EXIT(1);
   }
   
   for (l=MEMO_FIXED; l; l=l->NEXT) if (!l->NEXT) break;
   if (l) l->NEXT=STACK_FIXED->DATA;
   else MEMO_FIXED=STACK_FIXED->DATA;
   del=STACK_FIXED;
   STACK_FIXED=STACK_FIXED->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);
}

static inline void flattenlastswap()
{
   chain_list *l,*del;
   
   if (!STACK_SWAP) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 135);
//      fprintf(stderr,"flattenlastswap: No Stack\n");
      EXIT(1);
   }
   
   for (l=MEMO_SWAP; l; l=l->NEXT) if (!l->NEXT) break;
   if (l) l->NEXT=STACK_SWAP->DATA;
   else MEMO_SWAP=STACK_SWAP->DATA;
   del=STACK_SWAP;
   STACK_SWAP=STACK_SWAP->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);
}

static inline void flattenlastloconmark()
{
   chain_list *l,*del;
   
   if (!STACK_LOCON_MARK) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 136);
//      fprintf(stderr,"flattenlastloconmark: No Stack\n");
      EXIT(1);
   }

   for (l=MEMO_LOCON_MARK; l; l=l->NEXT) if (!l->NEXT) break;
   if (l) l->NEXT=STACK_LOCON_MARK->DATA;
   else MEMO_LOCON_MARK=STACK_LOCON_MARK->DATA;
   del=STACK_LOCON_MARK;
   STACK_LOCON_MARK=STACK_LOCON_MARK->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);
}

void setfixed(locon_list *lc)
{
  ptype_list *p;
  if ((p=getptype(lc->USER,GEN_FIXED_PTYPE))==NULL)
    {
      lc->USER=addptype(lc->USER,GEN_FIXED_PTYPE, lc);
      MEMO_FIXED=addchain(MEMO_FIXED, lc);
    }
}

void addloconmark(locon_list *lc, mark_list *mark)
{
  locon_mark *lcm;
  ptype_list *p;
  if ((p=getptype(lc->USER,GEN_LOCON_MARK_PTYPE))==NULL)
    {
//      lcm=(locon_mark *)mbkalloc(sizeof(locon_mark));
      lcm=(locon_mark *)AddHeapItem(&loconmark_heap);
      lcm->lc=lc;
      lcm->oldvisited=mark->VISITED;
      lc->USER=addptype(lc->USER,GEN_LOCON_MARK_PTYPE, mark);
      MEMO_LOCON_MARK=addchain(MEMO_LOCON_MARK, lcm);
    }
}

void addswap(locon_list *lc, locon_list *sym)
{
  MEMO_SWAP=addchain(MEMO_SWAP, AddSwap(lc, sym));
}

void addwithoutcount(loins_list *li, long oldcount)
{
  MEMO_WITHOUT_COUNT=addptype(MEMO_WITHOUT_COUNT, oldcount, li);
}

mark_list *getloconmark(locon_list *lc)
{
  ptype_list *p;
  if ((p=getptype(lc->USER,GEN_LOCON_MARK_PTYPE))==NULL) return NULL;
  return (mark_list *)p->DATA;
}

int isfixed(locon_list *lc)
{
  mark_list *m;
  if (getptype(lc->USER,GEN_FIXED_PTYPE)!=NULL) return 1;
  if ((m=getloconmark(lc))!=NULL && m->COUNT>1) return 1;

/*  if (getptype(lc->USER,GEN_FIXED_PTYPE)==NULL) return 0;
  return 1;
  */
  return 0;
}

/****************************************************************************/
/*       return a list of lotrs contained in loins erased from all Stack    */
/****************************************************************************/
static void Extract_Transistor(model_list *ml, chain_list **tranin, chain_list **insin)
{
   chain_list *l,*q,*transistor_list=NULL, *instance_list=NULL;
   ptype_list *p;
   foundins_list *foundins;
   lotrs_list *lotrs;
   loins_list* loins;
   
   if (STACK_SHIFT_LOINS || STACK_SHIFT_LOTRS) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 137);
//      fprintf(stderr,"Extract_Transistor: Stack not empty\n");
      EXIT(1);
   }
   
   for (l=SHIFT_LOINS; l; l=l->NEXT) {
      foundins=l->DATA;

      p=delptype(foundins->VISITED->USER,GEN_VISITED_PTYPE);
      if (p==NULL) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 138);
//        fprintf(stderr,"Extract_Transistor: NULL pointer in model visited ptype(v2)\n");
        EXIT(1);
      }
      foundins->VISITED->USER=p;

      /*is it exclude?*/
      p=getptype(foundins->VISITED->USER,GENIUS_LOINS_FLAGS_PTYPE);
      if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 139);
//         fprintf(stderr,"Extract_Transistor: PTYPE error 0\n");
         EXIT(1);
      }
      /*if loins exclude in model skip it in build*/
      if (((long)p->DATA) & LOINS_IS_EXCLUDE) {
         foundins->VISITED=NULL;    /*to be used again*/
         continue;
      }
      if (((long)p->DATA) & LOINS_IS_EXCLUDE_AT_END) {
         foundins->LOCKED=1;    /*to be used again*/
      }
      else
      {
      
      /*work for YAGLE  --> not to loose transistors*/
      p=getptype(foundins->LOINS->USER,FCL_TRANSLIST_PTYPE);
      if (!p/* || !p->DATA*/) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 140);
//         fprintf(stderr,"Extract_Transistor: PTYPE error 1\n");
         EXIT(1);
      }

      for (q=p->DATA; q; q=q->NEXT) if (!q->NEXT) break;
      if (q) {
         q->NEXT=transistor_list;
         transistor_list=p->DATA;
      }   
      /*logfigchain coherence--> put off connectors*/
      p->DATA=NULL; /*to cut effect in erase_foundins wich erase this array*/

      p=getptype(foundins->LOINS->USER,FCL_INSLIST_PTYPE);
      if (p!=NULL) 
        {
          for (q=p->DATA; q; q=q->NEXT) if (!q->NEXT) break;
          if (q) { q->NEXT=instance_list; instance_list=p->DATA; }   
          p->DATA=NULL;
        }
      }
      erase_foundins(NULL, foundins->LOINS);
   }

   freechain(SHIFT_LOINS);
   SHIFT_LOINS=NULL;   

   for (l=SHIFT_LOTRS; l; l=l->NEXT) {
      lotrs=l->DATA;
      if (!lotrs->USER || lotrs->USER->TYPE!=GEN_VISITED_PTYPE) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 141);
         //fprintf(stderr,"Extract_Transistor: PTYPE error 2\n");
         EXIT(1);
      }
      loins=lotrs->USER->DATA;
      /*is it exclude?*/
      p=getptype(loins->USER,GENIUS_LOINS_FLAGS_PTYPE);
      if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 142);
//         fprintf(stderr,"Extract_Transistor: PTYPE error 3\n");
         EXIT(1);
      }
      /*if erase mark, it can be used in another model. fortunately we modify 
      also the lofigchain, so GENIUS and YAGLE can't use it again*/
      lotrs->USER=delptype(lotrs->USER,GEN_VISITED_PTYPE);
//      freeptype(lotrs->USER); /*there is only ptype GEN_VISITED_PTYPE here*/
//      lotrs->USER=NULL;   

      /*if loins exclude in model skip it in build: do not modify lofigchain*/
      if (((long)p->DATA) & LOINS_IS_EXCLUDE) continue;
      if (((long)p->DATA) & LOINS_IS_EXCLUDE_AT_END) continue;
      
      /*work for YAGLE  --> not to loose transistor*/
      transistor_list=addchain(transistor_list,lotrs);

      /*lofigchain coherence--> put off connectors*/
      /*GRID*/

      if (!mbk_LosigIsVDD(lotrs->GRID->SIG) && !mbk_LosigIsVSS(lotrs->GRID->SIG))
        {
          SignalStatisticsRemove(lotrs->GRID->SIG, lotrs->GRID);
          if (!fastremovelofigchainconnector(lotrs->GRID->SIG, lotrs->GRID))
            {
              p=getptype(lotrs->GRID->SIG->USER,LOFIGCHAIN);
              if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 143);
//                fprintf(stderr,"Extract_Transistor: no LOFIGCHAIN\n");
                EXIT(1);
              }
              p->DATA=RemoveLofigChainLocon((chain_list *)p->DATA, lotrs->GRID);
/*              new=NULL;

              for (lofigchain=p->DATA; lofigchain; lofigchain=lofigchain->NEXT) 
                if (lofigchain->DATA!=lotrs->GRID) new=addchain(new,lofigchain->DATA);
              freechain(p->DATA);
              p->DATA=reverse(new);*/
            }
          
        }

      /*SOURCE*/
      if (!mbk_LosigIsVDD(lotrs->SOURCE->SIG) && !mbk_LosigIsVSS(lotrs->SOURCE->SIG))
        {
          SignalStatisticsRemove(lotrs->SOURCE->SIG, lotrs->SOURCE);
          if (!fastremovelofigchainconnector(lotrs->SOURCE->SIG, lotrs->SOURCE))
            {
              p=getptype(lotrs->SOURCE->SIG->USER,LOFIGCHAIN);
              if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 144);
//                fprintf(stderr,"Extract_Transistor: no LOFIGCHAIN\n");
                EXIT(1);
              }
//              new=NULL;
               p->DATA=RemoveLofigChainLocon((chain_list *)p->DATA, lotrs->SOURCE);
/*              for (lofigchain=p->DATA; lofigchain; lofigchain=lofigchain->NEXT) 
                if (lofigchain->DATA!=lotrs->SOURCE) new=addchain(new,lofigchain->DATA);
              freechain(p->DATA);
              p->DATA=reverse(new);*/
            }
        }
      /*DRAIN*/
      if (!mbk_LosigIsVDD(lotrs->DRAIN->SIG) && !mbk_LosigIsVSS(lotrs->DRAIN->SIG))
        {
          SignalStatisticsRemove(lotrs->DRAIN->SIG, lotrs->DRAIN);
          if (!fastremovelofigchainconnector(lotrs->DRAIN->SIG, lotrs->DRAIN))
            {
              p=getptype(lotrs->DRAIN->SIG->USER,LOFIGCHAIN);
              if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 145);
//                fprintf(stderr,"Extract_Transistor: no LOFIGCHAIN\n");
                EXIT(1);
              }
              p->DATA=RemoveLofigChainLocon((chain_list *)p->DATA, lotrs->DRAIN);
/*              new=NULL;
              for (lofigchain=p->DATA; lofigchain; lofigchain=lofigchain->NEXT) 
                if (lofigchain->DATA!=lotrs->DRAIN) new=addchain(new,lofigchain->DATA);
              freechain(p->DATA);
              p->DATA=reverse(new);*/
            }
        }

      /*BULK*/
      if (SPI_IGNORE_BULK=='N' && lotrs->BULK!=NULL && lotrs->BULK->SIG!=NULL)
        {
          if (!mbk_LosigIsVDD(lotrs->BULK->SIG) && !mbk_LosigIsVSS(lotrs->BULK->SIG))
            {
              SignalStatisticsRemove(lotrs->BULK->SIG, lotrs->BULK);
              if (!fastremovelofigchainconnector(lotrs->BULK->SIG, lotrs->BULK))
                {
                  p=getptype(lotrs->BULK->SIG->USER,LOFIGCHAIN);
                  if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 146);
//                    fprintf(stderr,"Extract_Transistor: no LOFIGCHAIN\n");
                    EXIT(1);
                  }
                  p->DATA=RemoveLofigChainLocon((chain_list *)p->DATA, lotrs->BULK);
                }
            }
        }

   }
   freechain(SHIFT_LOTRS);
   SHIFT_LOTRS=NULL;

   *tranin=transistor_list;
   *insin=instance_list; 
   ml = NULL;
}


static void Extract_Only_Transistor(chain_list **tranin, chain_list **insin)
{
   chain_list *l;
   ptype_list *p;
   foundins_list *foundins;
   lotrs_list *lotrs;
   chain_list *new=NULL, *tmp, *newi=NULL; 
   
   if (STACK_SHIFT_LOINS || STACK_SHIFT_LOTRS) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 147);
//      fprintf(stderr,"Extract_Transistor: Stack not empty\n");
      EXIT(1);
   }
   
   for (l=SHIFT_LOINS; l; l=l->NEXT) {
      foundins=l->DATA;
      
      /*work for YAGLE  --> not to loose transistors*/
      p=getptype(foundins->LOINS->USER,FCL_TRANSLIST_PTYPE);
      if (!p) 
        {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 148);
//          fprintf(stderr,"Extract_Transistor: PTYPE error 4\n");
          EXIT(1);
        }

      tmp=dupchainlst((chain_list *)p->DATA);
      new=append(tmp, new);

      p=getptype(foundins->LOINS->USER,FCL_INSLIST_PTYPE);
      if (p!=NULL) 
        {
          tmp=dupchainlst((chain_list *)p->DATA);
          newi=append(tmp, newi);
        }
   }
   
   for (l=SHIFT_LOTRS; l; l=l->NEXT) {
      lotrs=l->DATA;
      if (!lotrs->USER || lotrs->USER->TYPE!=GEN_VISITED_PTYPE) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 149);
//         fprintf(stderr,"Extract_Transistor: PTYPE error 5\n");
         EXIT(1);
      }

      /*work for YAGLE  --> not to loose transistor*/
      new=addchain(new, lotrs);
   }
   
   *insin=newi;
   *tranin=new;
}

int CountFakeInstanceLocon(locon_list *ptcon, ptype_list *env)
{
  char *insname;
  ptype_list *p;
  genconchain_list *gencon;
  tree_list *array_con;
  long begin_con,end_con;
  int total=0;

  while (ptcon!=NULL)
    {
      p=getptype(ptcon->USER,GENCONCHAIN); 
      if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 150);
//        fprintf(stderr,"Build_loins: NULL pointer\n");
        EXIT(1);
      }   
      gencon=p->DATA;
      if (!gencon) 
        {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 151);
//          fprintf(stderr,"Build_loins: NULL pointer\n");
          EXIT(1);
        }   
      if (gencon->NEXT) {
        avt_errmsg(GNS_ERRMSG, "106", AVT_FATAL, ptcon->NAME,insname);
/*        fprintf(stderr,"Build_Loins: several signals connected to %s in loins %s\n",
                ptcon->NAME,insname);*/
        EXIT(1);
      }   
   
      array_con=gencon->LOCON_EXP;      /*TOKEN=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/
      if (TOKEN(array_con)==GEN_TOKEN_TO) {
        /*low first*/
        begin_con=Eval_Exp_VHDL(array_con->NEXT->DATA/*low bound*/,env); 
        /*high bound after*/
        end_con=Eval_Exp_VHDL(array_con->NEXT->NEXT->DATA/*high bound*/,env);
      }
      else if (TOKEN(array_con)==GEN_TOKEN_DOWNTO) {
        /*high first*/
        begin_con=Eval_Exp_VHDL(array_con->NEXT->DATA/*high bound*/,env);
        /*low bound after*/
        end_con=Eval_Exp_VHDL(array_con->NEXT->NEXT->DATA/*low bound*/,env); 
      }
      else {
        begin_con=-1;
        end_con=-1;
      }
      if (begin_con==UNDEF || end_con==UNDEF)
        {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 189);
//          fprintf(stderr,"CountLocon: can not determine the number of connectors\n");
          EXIT(1);
        }
      begin_con-=end_con;
      if (begin_con<0) begin_con=-begin_con;
      if (!mbk_LosigIsVDD(gencon->LOSIG) && !mbk_LosigIsVSS(gencon->LOSIG))
        {
          total+=begin_con+1;
        }
      ptcon=ptcon->NEXT;
    }

  return total;
}

long KeyContribValueFor(losig_list *ls)
{
  if (mbk_LosigIsVSS(ls))
    return 12456;
  else
    if (mbk_LosigIsVDD(ls))
      return 54321;
  return (long)ls;
}

long ComputeModelInstanceKey(locon_list *ptcon, ptype_list *env)
{
  losig_list *losig;
  char *insname;
  ptype_list *p;
  genconchain_list *gencon;
  mark_list* mark;
  tree_list *array_sig;
  ptype_list *head;
//  ptype_list *gensigchain_ptype;
  long begin_sig,end_sig;
  long key=0;
  int isfake;

  while (ptcon != NULL)
    {
      isfake=0;
      /*search genericity */
      p=getptype(ptcon->USER,GENCONCHAIN); 
      if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 152);
//        fprintf(stderr,"ComputeModelInstanceKey: NULL pointer\n");
        EXIT(1);
      }   
      gencon=p->DATA;
      if (!gencon) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 153);
//        fprintf(stderr,"ComputeModelInstanceKey: NULL pointer\n");
        EXIT(1);
      }   
      if (gencon->NEXT) {
        avt_errmsg(GNS_ERRMSG, "106", AVT_FATAL, ptcon->NAME,insname);
        fprintf(stderr,"ComputeModelInstanceKey: several signals connected to %s in loins %s\n",
                ptcon->NAME,insname);
        EXIT(1);
      }   
      
      losig=gencon->LOSIG;
      head=getptype(losig->USER,GEN_MARK_PTYPE);
      if (!head) {
        if (gns_isunusedsig(losig))
          isfake=1;
        else
          {
            avt_errmsg(GNS_ERRMSG, "107", AVT_FATAL, ptcon->NAME,(char *)losig->NAMECHAIN->DATA);
/*            fprintf(stderr,"ComputeModelInstanceKey: No search done on connector '%s' signal '%s', model must be a connexe graph\n",
                    ptcon->NAME,(char *)losig->NAMECHAIN->DATA);*/
            EXIT(1);
          }
      }
/*      gensigchain_ptype=getptype(losig->USER,GENSIGCHAIN); 
      if (!gensigchain_ptype) {
        fprintf(stderr,"ComputeModelInstanceKey: NULL pointer\n");
        EXIT(1);
      }   
  */    
      array_sig=gencon->LOSIG_EXP;      /*TOKEN(array_sig)=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/
      if (TOKEN(array_sig)==GEN_TOKEN_TO) {
        /*low first*/
        begin_sig=Eval_Exp_VHDL(array_sig->NEXT->DATA/*low bound*/,env); 
        /*high bound after*/
        end_sig=Eval_Exp_VHDL(array_sig->NEXT->NEXT->DATA/*high bound*/,env);
      }
      else if (TOKEN(array_sig)==GEN_TOKEN_DOWNTO) {
        /*high first*/
        begin_sig=Eval_Exp_VHDL(array_sig->NEXT->DATA/*high bound*/,env);
        /*low bound after*/
        end_sig=Eval_Exp_VHDL(array_sig->NEXT->NEXT->DATA/*low bound*/,env); 
      }
      else {
        begin_sig=end_sig=Eval_Exp_VHDL(array_sig, env);
//        end_sig=-1;
      }   
      for ( ;/*test on begin_con==end_con at the end of loop*/ ; ) {
        if (!isfake)
          {
            mark=GetMarkEA(gencon->LOSIG, begin_sig, (mark_list*)head->DATA);
            /*        for (mark=(mark_list*)head->DATA; mark; mark=mark->NEXT)
                if (mark->BIT==begin_sig) break;*/
            if (!mark) {
        avt_errmsg(GNS_ERRMSG, "108", AVT_FATAL, (char *)losig->NAMECHAIN->DATA, begin_sig);
/*              fprintf(stderr,"ComputeModelInstanceKey: No search done on signal '%s %ld', model must be a connexe graph(v2)\n",
                      (char *)losig->NAMECHAIN->DATA, begin_sig);*/
              EXIT(3);
            }
          
        //        printf("<%s>",mark->LOSIG->NAMECHAIN->DATA);
            if (!(SPI_IGNORE_BULK=='Y' && ptcon->NAME==MBK_BULK_NAME))
              key+=KeyContribValueFor(mark->LOSIG);
          }
        /*test end of loop*/
        if (begin_sig==end_sig) break;
        /*move counter*/

        if (begin_sig<end_sig) begin_sig++;
        else begin_sig--;
      }
      ptcon=ptcon->NEXT;
    }

  return key;
}

void ComputeModelTransistorKeyV2(locon_list *ptcon, ptype_list *env, long long *key, long long *mask)
{
  losig_list *losig;
  ptype_list *p;
  genconchain_list *gencon;
  mark_list* mark;
  tree_list *array_sig;
  ptype_list *head;
//  ptype_list *gensigchain_ptype;
  long begin_sig;
  long long _key=0, _mask=0;


  while (ptcon != NULL)
    {
      /*search genericity */
      p=getptype(ptcon->USER,GENCONCHAIN); 
      if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 154);
//        fprintf(stderr,"ComputeModelInstanceKeyv2: NULL pointer\n");
        EXIT(1);
      }   
      gencon=p->DATA;
/*      if (!gencon) {
        fprintf(stderr,"ComputeModelInstanceKey: NULL pointer\n");
        EXIT(1);
      }   
      if (gencon->NEXT) {
        fprintf(stderr,"ComputeModelInstanceKey: several signals connected to %s in loins %s\n",
                ptcon->NAME,insname);
        EXIT(1);
      }   
  */    
      _mask<<=16;
      _key<<=16;
      losig=gencon->LOSIG;
      head=getptype(losig->USER,GEN_MARK_PTYPE);
      if (!head) 
        {
          ptcon=ptcon->NEXT;
          continue;
        }
/*        
      gensigchain_ptype=getptype(losig->USER,GENSIGCHAIN); 
      if (!gensigchain_ptype) {
        fprintf(stderr,"ComputeModelInstanceKey: NULL pointer\n");
        EXIT(1);
      }   
  */    
      array_sig=gencon->LOSIG_EXP;      /*TOKEN(array_sig)=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/
      begin_sig=Eval_Exp_VHDL(array_sig, env);

      mark=GetMarkEA(gencon->LOSIG, begin_sig, (mark_list*)head->DATA);
/*      for (mark=(mark_list*)head->DATA; mark; mark=mark->NEXT)
        if (mark->BIT==begin_sig) break;
  */    
/*      gen_printf(2,"(%s %d)",ptcon->NAME,begin_sig);
      if (mark)
        gen_printf(2,"/%d)",mark->COUNT);*/
      if (mark && mark->COUNT>1)
        {
          //        printf("<%s>",mark->LOSIG->NAMECHAIN->DATA);
          if (!(SPI_IGNORE_BULK=='Y' && ptcon->NAME==MBK_BULK_NAME))
            {
//              printf("%s %s %c\n",ptcon->NAME,mark->LOSIG->NAMECHAIN->DATA,mark->LOSIG->TYPE);
              _mask|=0x0ffff;
              _key|=KeyContrib2ValueFor(mark->LOSIG);
            }
        }
      ptcon=ptcon->NEXT;
    }
  *key=_key;
  *mask=_mask;
}

chain_list *GetMarkFromCON(locon_list *ptcon, int *first, int *last, model_list *model, ptype_list *env, ptype_list *livar)
{
  losig_list *losig;
//  lofig_list *ptnewfig;
  char *insname;
  ptype_list *p;
  genconchain_list *gencon;
  mark_list* mark;
  tree_list *array_sig,*array_con;
  ptype_list *head;
//  ptype_list *gensigchain_ptype;
  long begin_con,end_con,begin_sig,end_sig;
  chain_list *cl;
  int isfake=0, val;

  p=getptype(ptcon->USER,GENCONCHAIN); 
  if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 155);
//    fprintf(stderr,"GetMarkFromCON: NULL pointer\n");
    EXIT(1);
  }   
  gencon=p->DATA;
  if (!gencon) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 156);
//    fprintf(stderr,"GetMarkFromCON: NULL pointer\n");
    EXIT(1);
  }   
  if (gencon->NEXT) {
        avt_errmsg(GNS_ERRMSG, "106", AVT_FATAL, ptcon->NAME,insname);
/*  fprintf(stderr,"GetMarkFromCON: several signals connected to %s in loins %s\n",
            ptcon->NAME,insname);*/
    EXIT(1);
  }   
  losig=gencon->LOSIG;
  head=getptype(losig->USER,GEN_MARK_PTYPE);
  if (!head) {
    if (gns_isunusedsig(losig))
      isfake=1;
    else
      {
        avt_errmsg(GNS_ERRMSG, "107", AVT_FATAL, ptcon->NAME,(char *)losig->NAMECHAIN->DATA);
/*        fprintf(stderr,"GetMarkFromCON: No search done on connector '%s' signal '%s', model '%s' must be a connexe graph\n",
                ptcon->NAME,(char *)losig->NAMECHAIN->DATA, model!=NULL?model->NAME:"??");*/
        EXIT(1);
      }
  }

/*  gensigchain_ptype=getptype(losig->USER,GENSIGCHAIN); 
  if (!gensigchain_ptype) {
    fprintf(stderr,"GetMarkFromCON: NULL pointer\n");
    EXIT(1);
  }   
*/
  array_con=gencon->LOCON_EXP;      /*TOKEN=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/
  if (TOKEN(array_con)==GEN_TOKEN_TO) {
    /*low first*/
    begin_con=Eval_Exp_VHDL(array_con->NEXT->DATA/*low bound*/,livar); 
    /*high bound after*/
    end_con=Eval_Exp_VHDL(array_con->NEXT->NEXT->DATA/*high bound*/,livar);
  }
  else if (TOKEN(array_con)==GEN_TOKEN_DOWNTO) {
    /*high first*/
    begin_con=Eval_Exp_VHDL(array_con->NEXT->DATA/*high bound*/,livar);
    /*low bound after*/
    end_con=Eval_Exp_VHDL(array_con->NEXT->NEXT->DATA/*low bound*/,livar); 
  }
  else {
    begin_con=-1;
    end_con=-1;
  }   
  *first=begin_con;
  *last=end_con;
  if (*first==UNDEF || *last==UNDEF)
    {
        avt_errmsg(GNS_ERRMSG, "109", AVT_FATAL, ptcon->NAME);
//      fprintf(stderr,"GetMarkFromCON: can't compute connector bound for connector '%s'\nCertainly while building a fake instance or transistor\n", ptcon->NAME);
      EXIT(1);
    }
  array_sig=gencon->LOSIG_EXP;      /*TOKEN(array_sig)=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/

  if (TOKEN(array_sig)==GEN_TOKEN_TO) {
    /*low first*/
    begin_sig=Eval_Exp_VHDL(array_sig->NEXT->DATA/*low bound*/,env); 
    /*high bound after*/
    end_sig=Eval_Exp_VHDL(array_sig->NEXT->NEXT->DATA/*high bound*/,env);
  }
  else if (TOKEN(array_sig)==GEN_TOKEN_DOWNTO) {
    /*high first*/
    begin_sig=Eval_Exp_VHDL(array_sig->NEXT->DATA/*high bound*/,env);
    /*low bound after*/
    end_sig=Eval_Exp_VHDL(array_sig->NEXT->NEXT->DATA/*low bound*/,env); 
  }
  else {
    begin_sig=end_sig=-1; //Eval_Exp_VHDL(array_sig, env);
  }
  if ((begin_sig==-1 && end_sig==-1 && (val=Eval_Exp_VHDL(array_sig,env))==UNDEF)
      || begin_sig==UNDEF || end_sig==UNDEF)
    {
      *first=UNDEF;
      *last=UNDEF;
      return NULL;
    }
  else if (begin_sig==-1 && end_sig==-1) begin_sig=end_sig=val;

  cl=NULL;
  for ( ;/*test on begin_con==end_con at the end of loop*/ ; ) {
    if (!isfake)
      {
        mark=GetMarkEA(gencon->LOSIG, begin_sig, (mark_list*)head->DATA);
        /*    for (mark=(mark_list*)head->DATA; mark; mark=mark->NEXT)
              if (mark->BIT==begin_sig) break;
        */
        
        if (!mark) {
        avt_errmsg(GNS_ERRMSG, "107", AVT_FATAL, ptcon->NAME,begin_con);
/*          fprintf(stderr,"GetMarkFromCON: No search done on connector '%s %ld', model '%s' must be a connexe graph(v2)\n",
                  ptcon->NAME,begin_con,model!=NULL?model->NAME:"??");*/
          EXIT(3);
        }
        cl=addchain(cl, mark->LOSIG);
      }
    if (begin_con==end_con && begin_sig==end_sig) break;
    /*now error*/
    if (begin_con==end_con || begin_sig==end_sig) {
     // fprintf(stderr,"locon=%s %ld %ld / %ld %ld\n",ptcon->NAME, begin_con, end_con, begin_sig, end_sig);
      avt_error("gns", -1, AVT_ERR, "width mismatch on connector '%s(%d..%d)' and signal '%s(%d..%d)'\n",ptcon->NAME, begin_con, end_con, gen_losigname(ptcon->SIG), begin_sig, end_sig);
      EXIT(3);
    }
    /*move counter*/
    if (begin_con<end_con) begin_con++;
    else begin_con--;
    if (begin_sig<end_sig) begin_sig++;
    else begin_sig--;
  }
  return cl;
  model = NULL;
}

chain_list *GetModelSignalFromCON(locon_list *ptcon, int *first, int *last, model_list *model, ptype_list *env, ptype_list *livar)
{
  losig_list *losig;
//  lofig_list *ptnewfig;
  char *insname;
  ptype_list *p;
  genconchain_list *gencon;
//  mark_list* mark;
  tree_list *array_sig,*array_con;
//  ptype_list *head;
//  ptype_list *gensigchain_ptype;
  int begin_con,end_con,begin_sig,end_sig;
  chain_list *cl;
  char temp[1000];
  
  p=getptype(ptcon->USER,GENCONCHAIN); 
  if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 157);
//    fprintf(stderr,"GetMarkFromCON: NULL pointer\n");
    EXIT(1);
  }   
  gencon=p->DATA;
  if (!gencon) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 158);
//    fprintf(stderr,"GetMarkFromCON: NULL pointer\n");
    EXIT(1);
  }   
  if (gencon->NEXT) {
        avt_errmsg(GNS_ERRMSG, "106", AVT_FATAL, ptcon->NAME,insname);

/*    fprintf(stderr,"GetMarkFromCON: several signals connected to %s in loins %s\n",
            ptcon->NAME,insname);*/
    EXIT(1);
  }   
  losig=gencon->LOSIG;
/*  head=getptype(losig->USER,GEN_MARK_PTYPE);
  if (!head) {
     if (gns_isunusedsig(losig))
       return NULL;

     fprintf(stderr,"GetMarkFromCON: No search done on connector '%s' signal '%s', model '%s' must be a connexe graph\n",
             ptcon->NAME,(char *)losig->NAMECHAIN->DATA, model!=NULL?model->NAME:"??");
     EXIT(1);
  }
*/
/*
  gensigchain_ptype=getptype(losig->USER,GENSIGCHAIN); 
  if (!gensigchain_ptype) {
    fprintf(stderr,"GetMarkFromCON: NULL pointer\n");
    EXIT(1);
  }   */
  array_con=gencon->LOCON_EXP;      /*TOKEN=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/
  if (TOKEN(array_con)==GEN_TOKEN_TO) {
    /*low first*/
    begin_con=Eval_Exp_VHDL(array_con->NEXT->DATA/*low bound*/,livar); 
    /*high bound after*/
    end_con=Eval_Exp_VHDL(array_con->NEXT->NEXT->DATA/*high bound*/,livar);
  }
  else if (TOKEN(array_con)==GEN_TOKEN_DOWNTO) {
    /*high first*/
    begin_con=Eval_Exp_VHDL(array_con->NEXT->DATA/*high bound*/,livar);
    /*low bound after*/
    end_con=Eval_Exp_VHDL(array_con->NEXT->NEXT->DATA/*low bound*/,livar); 
  }
  else {
    begin_con=-1;
    end_con=-1;
  }   
  *first=begin_con;
  *last=end_con;
  if (*first==UNDEF || *last==UNDEF)
    {
        avt_errmsg(GNS_ERRMSG, "109", AVT_FATAL, ptcon->NAME);
//      fprintf(stderr,"GetMarkFromCON: can't compute connector bound for connector '%s'\nCertainly while building a fake instance or transistor\n", ptcon->NAME);
      EXIT(1);
    }
  array_sig=gencon->LOSIG_EXP;      /*TOKEN(array_sig)=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/

  if (TOKEN(array_sig)==GEN_TOKEN_TO) {
    /*low first*/
    begin_sig=Eval_Exp_VHDL(array_sig->NEXT->DATA/*low bound*/,env); 
    /*high bound after*/
    end_sig=Eval_Exp_VHDL(array_sig->NEXT->NEXT->DATA/*high bound*/,env);
  }
  else if (TOKEN(array_sig)==GEN_TOKEN_DOWNTO) {
    /*high first*/
    begin_sig=Eval_Exp_VHDL(array_sig->NEXT->DATA/*high bound*/,env);
    /*low bound after*/
    end_sig=Eval_Exp_VHDL(array_sig->NEXT->NEXT->DATA/*low bound*/,env); 
  }
  else {
    begin_sig=end_sig=Eval_Exp_VHDL(array_sig, env);
  }
  if ((begin_sig==-1 && end_sig==-1 && Eval_Exp_VHDL(array_sig,env)==UNDEF)
      || begin_sig==UNDEF || end_sig==UNDEF)
    {
      *first=UNDEF;
      *last=UNDEF;
      return NULL;
    }
  cl=NULL;
  for ( ;/*test on begin_con==end_con at the end of loop*/ ; ) {

//    mark=GetMarkEA(gencon->LOSIG, begin_sig, (mark_list*)head->DATA);

/*
    if (!mark) {
      fprintf(stderr,"GetMarkFromCON: No search done on connector '%s %ld', model '%s' must be a connexe graph(v2)\n",
              ptcon->NAME,begin_con,model!=NULL?model->NAME:"??");
      EXIT(3);
    }*/
    if (begin_sig!=-1)
      sprintf(temp,"%s %d", (char *)gencon->LOSIG->NAMECHAIN->DATA, begin_sig);
    else
      strcpy(temp,(char *)gencon->LOSIG->NAMECHAIN->DATA);
    cl=addchain(cl, namealloc(temp));
    if (begin_con==end_con && begin_sig==end_sig) break;
    /*now error*/
    if (begin_con==end_con || begin_sig==end_sig) {
/*      fprintf(stderr,"locon=%s %d %d / %d %d\n",ptcon->NAME, begin_con, end_con, begin_sig, end_sig);
      fprintf(stderr,"GetMarkFromCON: computing error\n");*/
        avt_errmsg(GNS_ERRMSG, "110", AVT_FATAL, ptcon->NAME, begin_con, end_con, gen_losigname(ptcon->SIG), begin_sig, end_sig);
//      avt_error("gns", -1, AVT_ERR, "width mismatch on connector '%s(%d..%d)' and signal '%s(%d..%d)'\n",ptcon->NAME, begin_con, end_con, gen_losigname(ptcon->SIG), begin_sig, end_sig);
      EXIT(3);
    }
    /*move counter*/
    if (begin_con<end_con) begin_con++;
    else begin_con--;
    if (begin_sig<end_sig) begin_sig++;
    else begin_sig--;
  }
  return cl;
  model = NULL;
}

ptype_list *GetFORdependantSingleSignalExpressions(locon_list *ptcon, ptype_list *env, ptype_list *lastlist)
{
  losig_list *losig;
//  lofig_list *ptnewfig;
  char *insname;
  ptype_list *p;
  genconchain_list *gencon;
  tree_list *array_sig;

  p=getptype(ptcon->USER,GENCONCHAIN); 
  if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 159);
//    fprintf(stderr,"Build_loins: NULL pointer\n");
    EXIT(1);
  }   
  gencon=p->DATA;
  if (!gencon) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 160);
//    fprintf(stderr,"Build_loins: NULL pointer\n");
    EXIT(1);
  }   
  if (gencon->NEXT) {
        avt_errmsg(GNS_ERRMSG, "106", AVT_FATAL, ptcon->NAME,insname);
/*  fprintf(stderr,"Build_Loins: several signals connected to %s in loins %s\n",
            ptcon->NAME,insname);*/
    EXIT(1);
  }   
  losig=gencon->LOSIG;

  array_sig=gencon->LOSIG_EXP;      /*TOKEN(array_sig)=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/

  if (TOKEN(array_sig)==GEN_TOKEN_TO || TOKEN(array_sig)==GEN_TOKEN_DOWNTO) return lastlist;

  if (Eval_Exp_VHDL(array_sig,env)==UNDEF)
    {
      ptype_list *p;
//      printf("add %s\n",losig->NAMECHAIN->DATA);
      for (p=lastlist; p!=NULL && p->TYPE!=(long)losig; p=p->NEXT) ;
      if (p!=NULL)
        p->DATA=addchain((chain_list *)p->DATA, array_sig);
      else
        lastlist=addptype(lastlist, (long)losig, addchain(NULL, array_sig));
    }

  return lastlist;
}

static chain_list *FakeLOINS=NULL;
static chain_list *FakeLOTRS=NULL;

static void CreateFake(model_list *model, ptype_list *env)
{
   ptype_list *p;
   lotrs_list *tr;
   loins_list *li;
   int f,l;
   lofig_list *lf;
   long key;

   lf=model->LOFIG;
   for (li=lf->LOINS;li!=NULL;li=li->NEXT)
     {
       p=getptype(li->USER,GENIUS_LOINS_FLAGS_PTYPE);
       if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 161);
//         fprintf(stderr,"CreateFake: PTYPE error 6\n");
         EXIT(1);
       }
       if (!(((long)p->DATA) & LOINS_IS_WITHOUT)) continue;

       if (mbk_istransn(li->FIGNAME) || mbk_istransp(li->FIGNAME))
         {
           chain_list *cl[4];
           cl[0]=GetMarkFromCON(li->LOCON, &f, &l, model, env, NULL); // grille
           if (cl[0]->NEXT!=NULL) EXIT(5);
           cl[1]=GetMarkFromCON(li->LOCON->NEXT, &f, &l,model, env, NULL); //drain
           if (cl[1]->NEXT!=NULL) EXIT(6);
           cl[2]=GetMarkFromCON(li->LOCON->NEXT->NEXT, &f, &l, model, env, NULL); // source
           if (cl[2]->NEXT!=NULL) EXIT(7);
           if (SPI_IGNORE_BULK=='Y')
             cl[3]=NULL;
           else
             cl[3]=GetMarkFromCON(li->LOCON->NEXT->NEXT->NEXT, &f, &l, model, env, NULL); // source
           
           tr=BuildFakeTransistor(mbk_istransn(li->FIGNAME)?TRANSN:TRANSP,
                                  (losig_list *)cl[0]->DATA,
                                  (losig_list *)cl[1]->DATA,
                                  (losig_list *)cl[2]->DATA,
                                  cl[3]==NULL?NULL:(losig_list *)cl[3]->DATA
                                  );
           addlotrsmodel(tr, li->FIGNAME) ;
           fastaddlofigchainconnector(tr->GRID->SIG, tr->GRID);
           fastaddlofigchainconnector(tr->DRAIN->SIG, tr->DRAIN);
           fastaddlofigchainconnector(tr->SOURCE->SIG, tr->SOURCE);
           if (SPI_IGNORE_BULK=='N' && tr->BULK->SIG!=NULL) fastaddlofigchainconnector(tr->BULK->SIG, tr->BULK);
           key=TransistorKey(tr);
           AddQuickMatchChance(tr->GRID->SIG, key);
           AddQuickMatchChance(tr->DRAIN->SIG, key);
           AddQuickMatchChance(tr->SOURCE->SIG, key);
           if (SPI_IGNORE_BULK=='N' && tr->BULK->SIG!=NULL) AddQuickMatchChance(tr->BULK->SIG, key);
           FakeLOTRS=addchain(FakeLOTRS, tr);
           freechain(cl[0]);
           freechain(cl[1]);
           freechain(cl[2]);
           freechain(cl[3]);
         }
       else
         {
           locon_list *lc, *ptnewcon;
           loins_list *newli;
           chain_list *cl, *ch;
           char oldSEPAR;
           int dir, ri;
           newli=(loins_list *)mbkalloc(sizeof(loins_list));
           newli->INSNAME=FAKE_INS_NAME;
           newli->FIGNAME=li->FIGNAME;
           oldSEPAR=SEPAR;
           SEPAR=' ';
           newli->LOCON=NULL;
           newli->NEXT=NULL;
           for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
             {
               ch=cl=GetMarkFromCON(lc, &f, &l, model, env, NULL);
               if (f<l) dir=1;
               else dir=-1;
               ri=getradindex(lc->NAME);
               while (f!=l+dir)
                 {
                   ptnewcon = (locon_list *)mbkalloc(sizeof(locon_list));
                   if (l==-1) ptnewcon->NAME = lc->NAME;
                   else ptnewcon->NAME = nameindex(lc->NAME,f);
                   
                   ptnewcon->SIG  = (losig_list *)ch->DATA;
                   ptnewcon->DIRECTION = lc->DIRECTION;
                   ptnewcon->TYPE = 'I';
                   ptnewcon->ROOT = newli;      
//                   ptnewcon->PNODE= NULL;
                   AddRadicalInfoInLOCON(ptnewcon, ri, f);
                   ptnewcon->NEXT = newli->LOCON;
                   ptnewcon->USER=NULL;
                   newli->LOCON  = ptnewcon;
                   fastaddlofigchainconnector(ptnewcon->SIG, ptnewcon);
                   f+=dir;
                   ch=ch->NEXT;
                 }
               freechain(cl);
             }
           newli->LOCON=(locon_list *)reverse((chain_list *)newli->LOCON);
           newli->USER=addptype(NULL,FCL_TRANSLIST_PTYPE,NULL);
           SEPAR=oldSEPAR;
           key=InstanceKey(newli);
           for (lc=newli->LOCON;lc!=NULL;lc=lc->NEXT)
             AddQuickMatchChance(lc->SIG, key);
           FakeLOINS=addchain(FakeLOINS, newli);
           AddSymsFlagInLoinsLocon(newli);
/*           fprintf(stderr,"feature not implemented yet\n");
           EXIT(10);
*/
         }

     }
   
}


void CleanLofigForWithouts(model_list *model)
{
  loins_list *li;
  lofig_list *lf;
  lf=model->LOFIG;
  for (li=lf->LOINS;li!=NULL;li=li->NEXT)
    {
      if (getptype(li->USER, GENIUS_LOINS_COUNT_PTYPE)!=NULL)
        li->USER=delptype(li->USER, GENIUS_LOINS_COUNT_PTYPE);
    }
}

int level=0;

/****************************************************************************/
/*  LEX&YAC searching:  Concate this level of counter with the last one     */
/****************************************************************************/
extern void Reduce()
{
   chain_list *l,*del;
   
   
//   gen_printf(2,"(r)LEVEL=%d",--level);

   if (!STACK_SHIFT_LOINS || !STACK_SHIFT_LOTRS || !STACK_MARK || !STACK_MARK_COUNT) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 162);
//      fprintf(stderr,"Reduce: NULL pointer\n");
      EXIT(1);
   }

   flattenlastmarks();
   flattenlastmarkcounts();
   flattenlastwithoutcounts();
   flattenlastfixed();
   flattenlastloconmark();
   flattenlastswap();

   for (l=SHIFT_LOINS; l; l=l->NEXT) if (!l->NEXT) break;
   if (l) l->NEXT=STACK_SHIFT_LOINS->DATA;
   else SHIFT_LOINS=STACK_SHIFT_LOINS->DATA;
   del=STACK_SHIFT_LOINS;
   STACK_SHIFT_LOINS=STACK_SHIFT_LOINS->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);

   for (l=SHIFT_LOTRS; l; l=l->NEXT) if (!l->NEXT) break;
   if (l) l->NEXT=STACK_SHIFT_LOTRS->DATA;
   else SHIFT_LOTRS=STACK_SHIFT_LOTRS->DATA;
   del=STACK_SHIFT_LOTRS;
   STACK_SHIFT_LOTRS=STACK_SHIFT_LOTRS->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);
}

/****************************************************************************/
/*           LEX&YAC searching:  Erase last level of Stack                  */
/****************************************************************************/
extern void Backward()
{
   chain_list *l,*del;
   foundins_list *circuit_ins;
   lotrs_list *lotrs;
   ptype_list *p;
   loins_list *model_ins;

//   gen_printf(2,"(-)LEVEL=%d",--level);
   if (!STACK_SHIFT_LOINS || !STACK_SHIFT_LOTRS || !STACK_MARK || !STACK_MARK_COUNT) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 163);
//      fprintf(stderr,"Backward: NULL pointer\n");
      EXIT(1);
   }
   
   freelastloconmark();
   freelastfixed();
   freelastmarkcounts();
   freelastwithoutcounts();
   freelastswaps(); // <- before freelastmarks();
   freelastmarks();

   /*unshift*/
   for (l=SHIFT_LOINS; l; l=l->NEXT) {
      circuit_ins=(foundins_list*)l->DATA;
      model_ins=circuit_ins->VISITED;
      p=delptype(model_ins->USER,GEN_VISITED_PTYPE);
      if (p==NULL) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 164);
//        fprintf(stderr,"Backward: NULL pointer in model visited ptype(v2)\n");
        EXIT(1);
      }
      model_ins->USER=p;
      circuit_ins->VISITED=NULL;
   }
   freechain(SHIFT_LOINS);
   
   SHIFT_LOINS=STACK_SHIFT_LOINS->DATA;
   del=STACK_SHIFT_LOINS;
   STACK_SHIFT_LOINS=STACK_SHIFT_LOINS->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);

   /*unshift*/
   for (l=SHIFT_LOTRS; l; l=l->NEXT) {
      lotrs=(lotrs_list*)l->DATA;
      if (!lotrs->USER) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 165);
//         fprintf(stderr,"Backward: ptype not found\n");
         EXIT(1);
      }
      lotrs->USER=delptype(lotrs->USER, GEN_VISITED_PTYPE);
//      freeptype(lotrs->USER);       /*erase only ptype GEN_VISITED_PTYPE*/
//      lotrs->USER=NULL;
   }
   freechain(SHIFT_LOTRS);
   
   SHIFT_LOTRS=STACK_SHIFT_LOTRS->DATA;
   del=STACK_SHIFT_LOTRS;
   STACK_SHIFT_LOTRS=STACK_SHIFT_LOTRS->NEXT;
   del->NEXT=NULL;/*break recursivity*/
   freechain(del);
}

/****************************************************************************/
/*           LEX&YAC searching:  increase the counter of Stack              */
/****************************************************************************/
extern void Protect()
{
//   gen_printf(2,"(+)LEVEL=%d",level++);
  certifylastmarkcounts();
  certifylastwithoutcounts();
  certifylastmarks();
  certifylastfixed();
  certifylastloconmark();
  certifylastswap();
  
  STACK_SHIFT_LOINS=addchain(STACK_SHIFT_LOINS,SHIFT_LOINS);
  SHIFT_LOINS=NULL;   
  STACK_SHIFT_LOTRS=addchain(STACK_SHIFT_LOTRS,SHIFT_LOTRS);
  SHIFT_LOTRS=NULL;   
}


/****************************************************************************/
/* LEX&YAC searching:  Put at this level of counter this foundins(see above)*/
/****************************************************************************/
extern int Shift(foundins_list* foundins, loins_list* model_ins)
{
   if (foundins->VISITED) {
      if (foundins->VISITED==model_ins) return 1;
      else return 0;
   }   
   
   foundins->VISITED=model_ins;
   model_ins->USER=addptype(model_ins->USER,GEN_VISITED_PTYPE,foundins->LOINS);
   SHIFT_LOINS=addchain(SHIFT_LOINS,foundins);   
   return 2;
}


/****************************************************************************/
/*    LEX&YAC searching:  Put at this level of counter this lotrs in Stack  */
/****************************************************************************/
extern int Shift_lotrs(lotrs_list* circuit_ins, loins_list* model_ins)
{
   ptype_list *p;
   
   p=getptype(circuit_ins->USER,GEN_VISITED_PTYPE);
   if (p) {
      if (p->DATA==model_ins) return 1;
      else return 0;
   }   
   
   circuit_ins->USER=addptype(circuit_ins->USER,GEN_VISITED_PTYPE,model_ins);
   SHIFT_LOTRS=addchain(SHIFT_LOTRS,circuit_ins);   
   return 2;
}


/****************************************************************************/
/*           LEX&YAC searching:  Refresh the Stack                          */
/****************************************************************************/
extern void Remove(model_list* model)
{
   chain_list *l;
   foundins_list *circuit_ins;
   lotrs_list *lotrs;
   loins_list *model_ins;
   ptype_list *p;

   if (STACK_SHIFT_LOINS || STACK_SHIFT_LOTRS || STACK_MARK) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 166);
//      fprintf(stderr,"Remove: Stack not empty\n");
      EXIT(1);
   }

   freeallswaps();
   freeallfixedandallloconmarks();
   freeallwithoutcounts();
   freeallmarkcounts();
   freeallmarks(model);

   for (l=SHIFT_LOINS; l; l=l->NEXT) {
      circuit_ins=(foundins_list*)l->DATA;
      model_ins=circuit_ins->VISITED;
      p=delptype(model_ins->USER,GEN_VISITED_PTYPE);
      if (p==NULL) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 167);
//        fprintf(stderr,"Remove: NULL pointer in model visited ptype(v2)\n");
        EXIT(1);
      }
      model_ins->USER=p;
      circuit_ins->VISITED=NULL;
      /*do not erase field circuit_ins->FLAG*/
   }
   freechain(SHIFT_LOINS);
   SHIFT_LOINS=NULL;
   
   for (l=SHIFT_LOTRS; l; l=l->NEXT) {
      lotrs=(lotrs_list*)l->DATA;
      if (!lotrs->USER) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 168);
//         fprintf(stderr,"Remove: ptype not found\n");
         EXIT(1);
      }
      lotrs->USER=delptype(lotrs->USER, GEN_VISITED_PTYPE);
      //freeptype(lotrs->USER);
      //      lotrs->USER=NULL;
   }
   freechain(SHIFT_LOTRS);
   SHIFT_LOTRS=NULL;
}


/****************************************************************************/
/*     return 0 if not match, 1 if already visited, 2 if just visited       */
/****************************************************************************/
static mark_list *failedmark;

mark_list *GetFailedMark() { return failedmark; }


void testranges(lofig_list *lf, ptype_list *env)
{
  losig_list *ls;
  ptype_list *range, *head;
  tree_list *tree;
  int left, right;
  mark_list *mark;

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      if (ls->TYPE=='I')
        {
//          printf("%s \n",(char *)ls->NAMECHAIN->DATA);
          range=getptype(ls->USER, GEN_REAL_RANGE_PTYPE);
          head=getptype(ls->USER, GEN_MARK_PTYPE);
          if (!head) continue;
          if (range==NULL || (tree=(tree_list *)range->DATA)==NULL)
            left=right=-1;
          else
            {
              if (gen_is_token_to(TOKEN(tree)))
                {
                  left=Eval_Exp_VHDL(tree->NEXT->DATA, env);
                  right=Eval_Exp_VHDL(tree->NEXT->NEXT->DATA, env);
                }
              else
                if (gen_is_token_downto(TOKEN(tree)))
                  {
                    right=Eval_Exp_VHDL(tree->NEXT->DATA, env);
                    left=Eval_Exp_VHDL(tree->NEXT->NEXT->DATA, env);
                  }
                else left=right=-1;
            }
          
          for (mark=(mark_list*)head->DATA; mark!=NULL; mark=mark->NEXT) 
            {
              if (mark->BIT<left || mark->BIT>right)
                {
                  if (left!=-1) 
        avt_errmsg(GNS_ERRMSG, "111", AVT_FATAL, mark->BIT, (char *)ls->NAMECHAIN->DATA, left,right);
//                  fprintf(stderr,"Index %d is out of range for signal %s (%d..%d)\n",mark->BIT, (char *)ls->NAMECHAIN->DATA, left,right);
                  else
        avt_errmsg(GNS_ERRMSG, "112", AVT_FATAL, mark->BIT, (char *)ls->NAMECHAIN->DATA);
        //             fprintf(stderr,"Index %d is out of range for signal %s\n",mark->BIT, (char *)ls->NAMECHAIN->DATA);
                  EXIT(1);
                }
            }   
        }

    }
}

losig_list *getlosigfrommark(gensigchain_list* gen, int numbit)
{
  mark_list *mark;
  ptype_list *head;
  ExpandableArray *ea;
  ptype_list *pea;
  void **mark_in_ea;

  /* mark already exists ? */

  if (numbit!=-1 && (pea=getptype(gen->LOCON->SIG->USER,  GENIUS_EXP_ARRAY_PTYPE))!=NULL)
    {
      ea=(ExpandableArray *)pea->DATA;
      mark_in_ea=GetExpandableArrayItem(ea, numbit, MarkEAUpdateFunc);
      mark=(mark_list *)*mark_in_ea;
    }
  else
    {
      head=getptype(gen->LOCON->SIG->USER,GEN_MARK_PTYPE);
      if (!head) return NULL;
      for (mark=(mark_list*)head->DATA; mark; mark=mark->NEXT) {
        if (mark->BIT==numbit) break;
      }
    }
  if (mark==NULL) return NULL;
  return mark->LOSIG;
}

extern int controlmark(int visited, losig_list *model_sig, int numbit, losig_list *circuit_sig, locon_list *me)
{
   mark_list *mark,*last_mark, *cmark;
   ptype_list *head,*top, *pea;
   ptype_list *model_sig_last;
   ExpandableArray *ea;
   void **mark_in_ea=NULL;
   
   /* mark already exists ? */
   head=getptype(model_sig->USER,GEN_MARK_PTYPE);
   if (!head) {
      model_sig->USER=addptype(model_sig->USER,GEN_MARK_PTYPE,NULL);
      head=model_sig->USER;
   }
   
   if (numbit==-1)
     {
       for (mark=(mark_list*)head->DATA; mark; mark=mark->NEXT) {
         if (mark->BIT==numbit) break;
       }   
     }
   else
     {
       if ((pea=getptype(model_sig->USER,  GENIUS_EXP_ARRAY_PTYPE))==NULL)
         {
           model_sig->USER=addptype(model_sig->USER,  GENIUS_EXP_ARRAY_PTYPE, ea=NewExpandableArray());
         }
       else
         ea=(ExpandableArray *)pea->DATA;
       mark_in_ea=GetExpandableArrayItem(ea, numbit, MarkEAUpdateFunc);
       mark=(mark_list *)*mark_in_ea;
     }

   failedmark=mark;
   
   if (circuit_sig->TYPE!=CNS_SIGVDD && circuit_sig->TYPE!=CNS_SIGVSS) 
     {
       if (mark && mark->LOSIG!=circuit_sig) 
         {
#ifdef ZINAPS_DUMP
             gen_printf(3,"%s'%s' != '%s'\n",tabs,(char *)mark->LOSIG->NAMECHAIN->DATA,(char *)circuit_sig->NAMECHAIN->DATA);
#endif
           if (!TryToSwapWithAConnectorWithTheSameSignalAndWithOneMark(mark->one_locon,circuit_sig,&cmark))
             return 0;
           if (cmark->BIT!=numbit || cmark->LOSIG!=circuit_sig) 
           {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 169);
//                   fprintf(stderr,"Internal severe warning: Bit mismatch %d!=%d or %s!=%s\n",cmark->BIT,numbit,(char *)cmark->LOSIG->NAMECHAIN->DATA,(char *)circuit_sig->NAMECHAIN->DATA);
                   EXIT(45);
           }
           mark=cmark;
         }
     }
   else
     {
       if (mark && mark->LOSIG->TYPE!=circuit_sig->TYPE)
         {
#ifdef ZINAPS_DUMP
             gen_printf(3,"%s'%s' != '%s'\n",tabs,(char *)mark->LOSIG->NAMECHAIN->DATA,(char *)circuit_sig->NAMECHAIN->DATA);
#endif
           return 0;
         }
     }


   /* losig in circuit already marked by another? */
   top=getptype(circuit_sig->USER,GEN_VISITED_PTYPE);
   if (!top) {
      circuit_sig->USER=addptype(circuit_sig->USER,GEN_VISITED_PTYPE,NULL);
      top=circuit_sig->USER;
   }
   for (model_sig_last=(ptype_list*)top->DATA; model_sig_last; model_sig_last=model_sig_last->NEXT) 
     {
       last_mark=(mark_list*)model_sig_last->DATA;
       if (last_mark==mark) break;
       /*if signal is internal in model, it doesn't accept another one connected*/
       /* case the new one is internal */
       else
         if (model_sig->TYPE==INTERNAL) {return 0;}
       /* case the last one is internal */
         else 
           if (model_sig_last->TYPE==INTERNAL) {return 0;}
     }   
   
   if (circuit_sig->TYPE!=CNS_SIGVDD && circuit_sig->TYPE!=CNS_SIGVSS) 
    {
      if ((mark && !model_sig_last) || (!mark && model_sig_last)) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 170);
//        fprintf(stderr,"controlmark: computing error\n");
        EXIT(3);
      }
    }
  /*if it wasn't marked*/
  if (!mark) {
/*#ifdef ZINAPS_DUMP
    gen_printf(4,"%s marks %s\n",me!=NULL?me->NAME:"??",(char *)circuit_sig->NAMECHAIN->DATA);
#endif*/
    mark=addmark(head,numbit,circuit_sig,visited,me);
    if (mark_in_ea!=NULL) *mark_in_ea=mark;
    mark->mark_in_ea=mark_in_ea;
//    gen_printf(5,"+ %s next=%p",mark->LOSIG->NAMECHAIN->DATA,MEMO_MARK);
    MEMO_MARK=addchain(MEMO_MARK, mark);
//    gen_printf(5,"cur=%p\n",MEMO_MARK);
    top->DATA=addptype(top->DATA,model_sig->TYPE,mark);
    if (me!=NULL) addloconmark(me, mark);
    failedmark=mark;
    return 2;   
  }

  if (visited==0)
    {
      mark->COUNT++;
      MEMO_MARK_COUNT=addchain(MEMO_MARK_COUNT, mark);
    }
/*  if (GEN_DEBUG_LEVEL>3)
    printf("(%d hits)",mark->COUNT);*/

  if (me!=NULL) addloconmark(me, mark);
  failedmark=mark;

  /*if it was marked */
  if (mark->VISITED) {return 1;}
  else {mark->VISITED=visited; return 2;}
}


/****************************************************************************/
/*   put on top a new element of searching, return the new foundins         */
/****************************************************************************/
extern foundins_list* new_foundins(foundins_list *top, loins_list *loins, ptype_list *var)
{
   foundins_list *new;
   ptype_list* p;
   
   if (!loins) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 171);
//      fprintf(stderr,"new_foundins: NULL pointer\n");
      EXIT(1);
   }

   loins->FIGNAME=modelradical(loins->FIGNAME);

   new=addfoundins();
   new->NEXT=top;
   new->VAR=var;
   new->FLAG=0;
   new->SAVEDFLAG=0;
   new->LOINS=loins;
   new->AFTER=NULL;
   new->BEFORE=NULL;
   new->SIZE=0;
   new->VISITED=NULL;
   new->ghost = 0;
   new->LOCKED = 0;
   new->LOWER=NULL;
   new->HIGHER=NULL;
   new->NOT_MATCH=0;
   new->PREVIOUS=NULL;
   if (top!=NULL) top->PREVIOUS=new;

   p=getptype(loins->USER,GEN_FOUNDINS_PTYPE);
   if (!p) loins->USER=addptype(loins->USER,GEN_FOUNDINS_PTYPE,new);
   else p->DATA=new;
   
   return new;
}


/****************************************************************************/
/*                put a new element of searching in a global variable       */
/****************************************************************************/
static foundins_list *lastfoundinsadded;

foundins_list *GetLastAddedFoundins()
{
  return lastfoundinsadded;
}
extern void add_foundins(model_list *ml, loins_list *loins, ptype_list *var)
{
   foundmodel_list* p;
   char *mlname;


   if (!loins) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 172);
//      fprintf(stderr,"add_foundins: NULL pointer\n");
      EXIT(1);
   }

   loins->FIGNAME=modelradical(loins->FIGNAME);
   mlname=modelradical(ml->NAME);
   for (p=FOUND_MODEL;p;p=p->NEXT) 
      if (p->MODEL->NAME==mlname/*loins->FIGNAME*/) break;
   if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 173);
//      fprintf(stderr,"add_foundins: model '%s' not found\n",loins->FIGNAME);
      EXIT(1);
   }   

   lastfoundinsadded=p->INS=new_foundins(p->INS,loins,var);
}


/****************************************************************************/
/*                put a new element of searching in a global variable       */
/****************************************************************************/
extern void add_foundmodel(foundins_list *liste_Ins, 
                                 model_list *model)
{
   foundins_list *p;
   foundmodel_list *new;
   
   int i=0;
   
   if (!model) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 174);
//      fprintf(stderr,"add_foundmodel: NULL pointer\n");
      EXIT(1);
   }

   new=(foundmodel_list*) mbkalloc(sizeof(foundmodel_list));
   new->NEXT=FOUND_MODEL;
   new->MODEL=model;
   new->INS=liste_Ins;
   for (p=liste_Ins;p;p=p->NEXT) i++;
   new->COUNT=i;
   new->CHECK=0; 
   FOUND_MODEL=new;
}


/****************************************************************************/
/*  free recursively from top a list of foundins, all non zero if you want  */
/*  to erase also loins attached                                            */
/****************************************************************************/
//#define LLEV -1
#define LLEV 5

chain_list *GrabExistingLoins(lofig_list *lf, char *modelname, char *regexp)
{
  loins_list *li;
  chain_list *cl=NULL, *ch;
  locon_list *lc;
  regex_t regular_expression;
  regmatch_t pmatch[1];
  int failedregexp=1;
  
  if (regexp!=NULL)
    failedregexp=regcomp(&regular_expression, regexp, REG_EXTENDED | REG_NOSUB | REG_ICASE);

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      if (li->FIGNAME==modelname
          || (failedregexp==0 && regexec(&regular_expression, li->FIGNAME, 0, pmatch, 0)!=REG_NOMATCH)
          )
        {
          li->LOCON=(locon_list *)reverse((chain_list *)li->LOCON);
          cl=addchain(cl, li);
          for (lc=li->LOCON, ch=NULL; lc!=NULL; lc=lc->NEXT) ch=addchain(ch, lc->PNODE);
          ch=reverse(ch);
          li->USER=addptype(li->USER, GENIUS_SAVED_PNODE, ch);
          li->USER=addptype(li->USER, FCL_INSLIST_PTYPE, addchain(NULL, li));
          li->USER=addptype(li->USER, FCL_TRANSLIST_PTYPE, NULL);
          if (modelname!=li->FIGNAME)
            li->USER=addptype(li->USER, GEN_BBOX_LOINS_MODEL, modelname);
        }
    }
  return cl;
}

void CleanLoconFlags(loins_list *li)
{
  locon_list *lc;
  for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT) lc->FLAGS=0;
}

void RestoreRealLoinsPnodes(loins_list *li)
{
  ptype_list *p;
  chain_list *cl;
  locon_list *lc;

  p=getptype(li->USER, GENIUS_SAVED_PNODE);
  if (p!=NULL)
  {
    for (lc=li->LOCON, cl=(chain_list *)p->DATA; lc!=NULL ; lc=lc->NEXT, cl=cl->NEXT)
      {
        lc->PNODE=cl->DATA;
      }
    freechain((chain_list *)p->DATA);
    li->USER=delptype(li->USER, GENIUS_SAVED_PNODE);
  }
}

void PutBackRealLoins(loins_list *li)
{
  RestoreRealLoinsPnodes(li);
  li->NEXT=(loins_list*)(~1&(long)li->NEXT);
}

static foundins_list *GNS_RESTORE=NULL;

static void put_back_locked_loins()
{
  foundins_list *gr, *next;
  foundmodel_list *p;
  locon_list *lc;
  ptype_list *pt;
  for (gr=GNS_RESTORE; gr!=NULL; gr=next)
  {
    next=gr->NEXT;
    for (lc=gr->LOINS->LOCON; lc!=NULL; lc=lc->NEXT)
     {
      if ((pt=getptype(lc->SIG->USER, LOFIGCHAIN))!=NULL)
         pt->DATA=addchain((chain_list *)pt->DATA, lc);
     }
    for (p=FOUND_MODEL;p;p=p->NEXT)
      if (p->MODEL->NAME==gr->LOINS->FIGNAME) break;

    if (p->INS!=NULL) p->INS->PREVIOUS=gr;
    gr->PREVIOUS=NULL;
    gr->NEXT=p->INS;
    p->INS=gr;
    gr->FLAG=0;
    gr->AFTER=NULL;
    gr->BEFORE=NULL;
    gr->SIZE=0;
    gr->VISITED=NULL;
    gr->ghost = 0;
    gr->LOCKED = 0;
    gr->LOWER=NULL;
    gr->HIGHER=NULL;
    gr->NOT_MATCH=0;
    gr->LOINS->NEXT=(loins_list*)(~1 & (long)gr->LOINS->NEXT); // [!] very dangerous
  }
  GNS_RESTORE=NULL;
}
extern void free_foundins(foundins_list *top, int all)
{
  ptype_list *p,*lofigchain;
  foundins_list *fl, *nextfl=NULL;
  chain_list *new,*l,*signals_to_check,*l1;
  losig_list *losig;
  locon_list *ptcon, *next;
  int nbcon=0,nbsig=0,avg=0,nbins=0,nbins2=0, j;
  chain_list *cl;
  lotrs_list *tr;
  loins_list *li;

  signals_to_check=NULL;

  cleanalllofigchainhash();

  for (fl=top;fl!=NULL;fl=fl->NEXT)
    {
      if (!fl->LOCKED)
        freeptype(fl->VAR);
      nbins++;
      if (all && fl->LOINS) 
        {
          nbins2++;
          for (ptcon = fl->LOINS->LOCON; ptcon!=NULL; ptcon = ptcon->NEXT) 
            {
              // I am to be deleted later
              if (ptcon->SIG==NULL) continue;
              ptcon->USER=addptype(ptcon->USER,TO_DELETE_MARK_PTYPE,0);
              nbcon++;
              // signal already added?
              if (getptype(ptcon->SIG->USER,TO_DELETE_MARK_PTYPE)==NULL)
                {
                  // no. Add it to the list
                  nbsig++;
                  signals_to_check=addchain(signals_to_check, ptcon->SIG);
                  ptcon->SIG->USER=addptype(ptcon->SIG->USER,TO_DELETE_MARK_PTYPE,0);
                }
            }
        }
    }

    for (cl=FakeLOTRS;cl!=NULL;cl=cl->NEXT)
    {
      locon_list *lc[4];
      int i;
      tr=(lotrs_list *)cl->DATA;
      lc[0]=tr->GRID;
      lc[1]=tr->SOURCE;
      lc[2]=tr->DRAIN;
      j=3;
      if (SPI_IGNORE_BULK=='N' && tr->BULK->SIG!=NULL) lc[3]=tr->BULK,j++;
      for (i=0;i<j;i++)
        {
          lc[i]->USER=addptype(lc[i]->USER,TO_DELETE_MARK_PTYPE,0);
          nbcon++;
          // signal already added?
          if (getptype(lc[i]->SIG->USER,TO_DELETE_MARK_PTYPE)==NULL)
            {
              // no. Add it to the list
              nbsig++;
              signals_to_check=addchain(signals_to_check, lc[i]->SIG);
              lc[i]->SIG->USER=addptype(lc[i]->SIG->USER,TO_DELETE_MARK_PTYPE,0);
            }
        }
    }

  for (cl=FakeLOINS;cl!=NULL;cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      for (ptcon = li->LOCON; ptcon!=NULL; ptcon = ptcon->NEXT) 
        {
          // I am to be deleted later
          if (ptcon->SIG==NULL) continue;
          ptcon->USER=addptype(ptcon->USER,TO_DELETE_MARK_PTYPE,0);
          nbcon++;
          // signal already added?
          if (getptype(ptcon->SIG->USER,TO_DELETE_MARK_PTYPE)==NULL)
            {
              // no. Add it to the list
              nbsig++;
              signals_to_check=addchain(signals_to_check, ptcon->SIG);
              ptcon->SIG->USER=addptype(ptcon->SIG->USER,TO_DELETE_MARK_PTYPE,0);
            }
        }
    }

#ifdef ZINAPS_DUMP
    if (nbins>0) 
      {
        gen_printf(LLEV,"Freeing loins\n");
        gen_printf(LLEV," |__ Connectors:%d Signals:%d, Instances %d/%d\n",nbcon,nbsig,nbins2,nbins);
      }
#endif
  for (l=signals_to_check;l!=NULL;l=l->NEXT)
    {
      losig=(losig_list *)l->DATA;
      losig->FLAGS &= ~LOFIGCHAIN_HAS_SYMS;
      losig->FLAGS &= ~SIGNAL_HAVE_MORE_INSTANCES_IN_LOFIGCHAIN;
      losig->USER=delptype(losig->USER,TO_DELETE_MARK_PTYPE);
      lofigchain=getptype(losig->USER,LOFIGCHAIN);

      if (!lofigchain) 
        {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 174);
//          fprintf(stderr,"free_foundins: no LOFIGCHAIN\n");
          EXIT(1);
        }

      new=NULL;
      for (l1=lofigchain->DATA; l1!=NULL; l1=l1->NEXT) 
        {
          avg++;
          ptcon=(locon_list *)l1->DATA;
          
          if (getptype(ptcon->USER,TO_DELETE_MARK_PTYPE)==NULL)
            {
              new=addchain(new,ptcon);
            }
        }
      
      freechain(lofigchain->DATA);
      lofigchain->DATA=new;    
    }

  freechain(signals_to_check);

#ifdef ZINAPS_DUMP
  if (nbsig>0) gen_printf(LLEV," |__ LOFIGCHAIN size: %g connectors/signal\n",(float)avg/(float)nbsig);
#endif

  for (fl=top, top=NULL;fl!=NULL;fl=nextfl)
    {
      nextfl=fl->NEXT;
      if (!fl->LOCKED)
      {
        fl->NEXT=top;
        top=fl;
      }
      else
      {
        fl->NEXT=GNS_RESTORE;
        GNS_RESTORE=fl;
        continue;
      }

      if (all && fl->LOINS) 
        {
          if (fl->SAVEDFLAG==0)
            {
              for (ptcon = fl->LOINS->LOCON; ptcon!=NULL; ptcon = next) 
                {
                  next=ptcon->NEXT;
                  freeptype(ptcon->USER);
                  
                  /*              if( ptcon->PNODE )
                              delrcnlocon( ptcon );
                  */              
                  mbkfree(ptcon);
                }
              p=getptype(fl->LOINS->USER,FCL_TRANSLIST_PTYPE); // 2 delete?
              if (p) freechain(p->DATA);

              p=getptype(fl->LOINS->USER,FCL_INSLIST_PTYPE); // 2 delete?
              if (p) freechain(p->DATA);

              freeptype(fl->LOINS->USER);
              mbkfree(fl->LOINS);
            }
          else
            {
              p=getptype(fl->LOINS->USER,FCL_INSLIST_PTYPE);
              if (p) freechain(p->DATA);

              PutBackRealLoins(fl->LOINS);
              fl->LOINS->LOCON=(locon_list *)reverse((chain_list *)fl->LOINS->LOCON);
            }
        }
    }

  // destruction des faux transistors
  for (cl=FakeLOTRS;cl!=NULL;cl=cl->NEXT)
    {
      tr=(lotrs_list *)cl->DATA;
      freeptype(tr->GRID->USER);
      freeptype(tr->SOURCE->USER);
      freeptype(tr->DRAIN->USER);
      mbkfree(tr->GRID);
      mbkfree(tr->SOURCE);
      mbkfree(tr->DRAIN);
      if (SPI_IGNORE_BULK=='N' && tr->BULK->SIG!=NULL) 
        {
          freeptype(tr->BULK->USER);
          mbkfree(tr->BULK);
        }
      freeptype(tr->USER);
      mbkfree(tr);
    }
  freechain(FakeLOTRS);
  FakeLOTRS=NULL;

  // destruction des fausses instances
  for (cl=FakeLOINS;cl!=NULL;cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      for (ptcon = li->LOCON; ptcon!=NULL; ptcon = next) 
        {
          next=ptcon->NEXT;
          freeptype(ptcon->USER);
          mbkfree(ptcon);
        }
      freeptype(li->USER);
      mbkfree(li);
    }

  freechain(FakeLOINS);
  FakeLOINS=NULL;

  freefoundins(top); // [!] fait un append
  EraseSignalStatistics();
}


//* test Zinaps
static foundins_list *PENDING_DELETION=NULL;
static long count_pending=0;

void finish_erase_foundins()
{
  free_foundins(PENDING_DELETION,1);
  PENDING_DELETION=NULL;
  count_pending=0;
  put_back_locked_loins();
}


void intermediate_finish_erase_foundins()
{
  if (MAX_COUNT_PENDING!=0 && count_pending>MAX_COUNT_PENDING) finish_erase_foundins();
}

extern void reset_existing_loins_pnodes()
{
  foundins_list *fl;
  foundmodel_list* fmodel;

  for (fmodel=FOUND_MODEL; fmodel; fmodel=fmodel->NEXT)
   {
    for (fl=fmodel->INS;fl!=NULL;fl=fl->NEXT)
      {
        if (fl->LOINS) 
          {
            if (fl->SAVEDFLAG)
              {
                RestoreRealLoinsPnodes(fl->LOINS);
              }
          }
      }
   }
}

/****************************************************************************/
/*                 erase one foundins which point to elem                   */
/****************************************************************************/
extern void erase_foundins(model_list *ml, loins_list *elem)
{
  char *name=elem->FIGNAME;

  ptype_list *pl;
  foundins_list *fil;
  foundmodel_list *p;
  
  if (ml==NULL)
    {
      for (p=FOUND_MODEL;p;p=p->NEXT) 
        if (p->MODEL->NAME==modelradical(name)) break;
      if (p==NULL && (pl=getptype(elem->USER,  GEN_BBOX_LOINS_MODEL))!=NULL)
        {
          for (p=FOUND_MODEL;p;p=p->NEXT) 
            if (p->MODEL->NAME==pl->DATA) break;
        }
    }
  else
    {
      for (p=FOUND_MODEL;p;p=p->NEXT) 
        if (p->MODEL->NAME==ml->NAME) break;
    }
        
  
  if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 190);
//    fprintf(stderr,"erase_foundins: model %s not found\n",name);
    EXIT(2);
  }

  if (p->INS->LOINS==elem) {
    fil=p->INS;
    p->INS=fil->NEXT;
    //found->NEXT=NULL;
    fil->NEXT=PENDING_DELETION;
    PENDING_DELETION=fil;
    fil->ghost=1;
    p->COUNT--;
    count_pending++;
    fil->LOINS->NEXT=(loins_list*)(1|(long)fil->LOINS->NEXT); // [!] very dangerous
    return;
  }   

  pl=getptype(elem->USER,GEN_FOUNDINS_PTYPE);
  if (!pl)
    {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 175);
//      fprintf(stderr,"erase_foundins: PTYPE not found\n");
      EXIT(1);
    }

  fil=(foundins_list *)pl->DATA;
  if (((long)fil->LOINS->NEXT) & 1)
    {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 176);
//      fprintf(stderr,"erase_foundins: loins %s already erased\n",elem->INSNAME);
      EXIT(1);
    }
  
  if (fil->PREVIOUS!=NULL) { fil->PREVIOUS->NEXT=fil->NEXT; }
  if (fil->NEXT!=NULL) { fil->NEXT->PREVIOUS=fil->PREVIOUS; }

  if (fil->AFTER) fil->AFTER->BEFORE=fil->BEFORE;
  if (fil->BEFORE) fil->BEFORE->AFTER=fil->AFTER;
  if (fil->LOWER) fil->LOWER->HIGHER=NULL;
  if (fil->HIGHER) fil->HIGHER->LOWER=NULL;
  
  fil->NEXT=PENDING_DELETION;
  PENDING_DELETION=fil;
  fil->ghost=1;
  p->COUNT--;
  count_pending++;
  fil->LOINS->NEXT=(loins_list*)(1|(long)fil->LOINS->NEXT); // [!] very dangerous
}


/****************************************************************************/
/*             search in a global variable a foundins                       */
/****************************************************************************/
extern foundins_list* getfoundins(char *name)
{
   foundmodel_list *p;
   
   for (p=FOUND_MODEL;p;p=p->NEXT) 
      if (p->MODEL->NAME==modelradical(name)) return p->INS;
   
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 177);
//   fprintf(stderr,"getfoundins: model %s not found\n",name);
   p=NULL;
   p->NEXT=p->NEXT;
   EXIT(2);
   return NULL;
}


/****************************************************************************/
/*       free global variable, set all!=0 if you to erase loins           */
/****************************************************************************/
extern void free_foundmodel(int all)
{
   foundmodel_list *p,*q;
   for (p=FOUND_MODEL;p;p=q)  {
      free_foundins(p->INS,all);   
      q=p->NEXT;
      mbkfree(p);
   }
   if (all) 
     {
       FOUND_MODEL=NULL;
       HEAD_FOUNDINS=NULL;
     }
}


/****************************************************************************/
/*            return a model from a variable global list                    */
/****************************************************************************/
static inline foundmodel_list* Found_Model(char *model)
{
   foundmodel_list *found;
   model=modelradical(model);
   
   for (found=FOUND_MODEL; found; found=found->NEXT) 
      if (found->MODEL->NAME==model) break;
   if (!found) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 178);
//      fprintf(stderr,"Found_Model: %s not found\n",model);
      EXIT(1);
   }
   return found;
}


/****************************************************************************/
/*               return non zero if model as been already searched          */
/****************************************************************************/
extern int Check_Model(char *model)
{
   foundmodel_list *foundmodel;
   foundmodel=Found_Model(model);
   if (!foundmodel) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 179);
//      fprintf(stderr,"Check_Model: %s not found\n",model);
      EXIT(2);   
   }
   return foundmodel->CHECK;
}


/****************************************************************************/
/*           return the number of loins model found in circuit              */
/****************************************************************************/
extern int Count_Model(char *model)
{
   foundmodel_list *foundmodel=Found_Model(model);
   if (!foundmodel) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 180);
//      fprintf(stderr,"Count_Model: %s not found\n",model);
      EXIT(2);   
   }
   return foundmodel->COUNT;
}


/****************************************************************************/
/*             increase the counter of instance from count                  */
/****************************************************************************/
extern void  AddCount_Model(char *model, int count)
{
   foundmodel_list *found;
   found=Found_Model(model);
   if (!found) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 181);
//      fprintf(stderr,"PutCount_Model: %s not found\n",model);
      EXIT(2);   
   }
   found->COUNT+=count;
}


/****************************************************************************/
/*                   put model flag to check                                */
/****************************************************************************/
extern void PutCheck_Model(char *model)
{
   foundmodel_list *found;
   found=Found_Model(model);
   found->CHECK=1;
}


/****************************************************************************/
/* build a new loins and delete all marks and all loins contained inside    */
/****************************************************************************/

HeapAlloc all_loins_heap;
chain_list *ALL_KIND_OF_MODELS;
all_loins_heap_struct *ALL_LOINS_FOUND;
ht *NEW_LOINS_NAMES_HT;
ht *LOINS_LIST_CONTROL;


void AllLoinsHeap_Manage(int mode)
{
  if (mode==0)
    {
      CreateHeap(sizeof(all_loins_heap_struct), 8192, &all_loins_heap);
      ALL_LOINS_FOUND=NULL;
    }
  else
    DeleteHeap(&all_loins_heap);
}

void add_loins_in_all_loins_found(char *name, model_list *model, corresp_t *table)
{
  all_loins_heap_struct *r;
  r=(all_loins_heap_struct *)AddHeapItem(&all_loins_heap);
  r->model=model;
  r->instance_name=name;
  r->CorrespondanceTable=table;
  r->NEXT=ALL_LOINS_FOUND;
  ALL_LOINS_FOUND=r;
}

void setglobalvariables(lofig_list *lf, chain_list *loins, chain_list *lotrs, ptype_list *env, chain_list *h_lotrs, chain_list *swaps)
{
  ALL_LOINS=loins;
  ALL_LOTRS=lotrs;
  current_lofig=lf;
  ALL_ENV=env;
  ALL_HIERARCHY_LOTRS=h_lotrs;
  ALL_SWAPS=swaps;
}

chain_list *ALL_NEW_INSTANCES=NULL;

void UpdateNewInstanceLofigChain()
{
  chain_list *cl;
  loins_list *li;
  locon_list *lc;
//  ptype_list *p;
  for (cl=ALL_NEW_INSTANCES; cl!=NULL; cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
        {
          /*change for future search the lofigchain*/
          fastaddlofigchainconnector(lc->SIG, lc);
        }
    }
  freechain(ALL_NEW_INSTANCES);
  ALL_NEW_INSTANCES=NULL;
}

static void checkrange(int a, int b, tree_list *t)
{
  if (a<0 || b<0)
    {
      avt_errmsg(GNS_ERRMSG, "113", AVT_ERROR, FILE_NAME(t), LINE(t), a, b);
      
      //fprintf(stderr,"%S:%d: negative vector bound computed for expression: \"");
      Dump_Tree(t);
      //fprintf(stderr,"\", values are %d and %d at %s:%d\n", a, b, FILE_NAME(t), LINE(t));
      EXIT(1);
    }
}

extern void Build_loins(model_list *model, ptype_list *env) 
{
   locon_list *ptcon = NULL;
   locon_list *ptnewcon = NULL;
   loins_list *ptins = NULL;
   losig_list *losig;
   lofig_list *ptnewfig;
   chain_list *translist, *inslist, *cl;
   char *figname,*insname, *temp, *c;
   ptype_list *p;
   genconchain_list *gencon;
   mark_list* mark;
   tree_list *array_sig,*array_con;
   ptype_list *var,*head;
//   ptype_list *gensigchain_ptype;
   static int count_ins=1;
   static char* last_model=NULL;
   long begin_con,end_con,begin_sig,end_sig;
   char memo_char;
   int ri;
   int isfake;
   corresp_t *table;
   char temp0[256],num[256];

   if (!model) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 182);
//      fprintf(stderr,"Build_loins: NULL pointer\n");
      EXIT(1);
   }

   testranges(model->LOFIG, env);

   if (modelradical(model->NAME)==last_model) count_ins++;
   else {
      last_model=modelradical(model->NAME);
      count_ins=1;
   }

   // creation des instances non existantes
   CreateFake(model, env);

   ptnewfig=model->LOFIG;
   figname = modelradical(model->NAME);
   insname=modelradical(figname);
   
   memo_char=SEPAR;               /* MBK variable it is for concatenation*/
   SEPAR='_';    /* nameindex() must be compatible with FCL  */ 
#if 1
   strcpy(temp0,modelradical(figname));
   for (var=env; var; var=var->NEXT)  
     {
       if (var->TYPE!=UNDEF) 
         {
           sprintf(num,"_%ld", var->TYPE);
           strcat(temp0, num);
         }
     }
   sprintf(num,"_%d", count_ins);
   strcat(temp0, num);
   insname=namealloc(temp0);
#else
   for (var=env; var; var=var->NEXT)  
     if (var->TYPE!=UNDEF) 
       insname=nameindex(insname,var->TYPE);

   insname = nameindex(insname,count_ins);
#endif

   {       
     chain_list *h_trs, *h_ins;
     temp=figname;
     env=(ptype_list *)reverse((chain_list *)env);
     for (var=env; var; var=var->NEXT)  
       if (var->TYPE!=UNDEF) temp=nameindex(temp, var->TYPE);
     /*execute action*/
     env=(ptype_list *)reverse((chain_list *)env);
     var=env;

     var=addptype(var, (long)temp, namealloc("model"));
     var=addptype(var, (long)insname, namealloc("instance"));
     if (modelradical(model->NAME)!=model->NAME)
       var=addptype(var, (long)namealloc(strchr(model->NAME,' ')+1), namealloc("archi"));
     else
       var=addptype(var, (long)model->NAME, namealloc("archi"));

     Extract_Only_Transistor(&h_trs, &h_ins);
     setglobalvariables(model->LOFIG, SHIFT_LOINS, SHIFT_LOTRS, var, h_trs,MEMO_SWAP);
     GEN_USER=NULL;

     table=gen_build_corresp_table(model->LOFIG, SHIFT_LOINS, SHIFT_LOTRS);
     table->VAR=dupptypelst(env);
     if ((c=strchr(temp,' '))!=NULL)
       {
         table->ARCHISTART=c+1-temp;
         table->ARCHILENGTH=strlen(temp)-table->ARCHISTART;
       }
     else
       table->ARCHISTART=table->ARCHILENGTH=0;

     table->GENIUS_FIGNAME=completename(temp, model->LOFIG->NAME, (char *)var->TYPE);
     add_loins_in_all_loins_found(insname, model, table);
     for (cl=ALL_KIND_OF_MODELS; cl!=NULL && cl->DATA!=table->GENIUS_FIGNAME; cl=cl->NEXT) ;
     if (cl==NULL) 
       { 
         gen_build_netlist(table->GENIUS_FIGNAME, table);
         ALL_KIND_OF_MODELS=addchain(ALL_KIND_OF_MODELS, table->GENIUS_FIGNAME);
       }
     
     //           Exec_C(model->C, var);
     var->NEXT->NEXT->NEXT=NULL;
     freeptype(var);
     
     setglobalvariables(NULL, NULL, NULL, NULL, NULL, NULL);
     freechain(h_trs);
     freechain(h_ins);
   }


   /*first take transistors contained in all loins marked*/
   Extract_Transistor(model, &translist, &inslist);  

   ptins = (loins_list *)mbkalloc(sizeof(loins_list));
   
   ptins->INSNAME  = insname;
   ptins->FIGNAME  = figname;
   ptins->LOCON   = NULL;
   ptins->USER    = NULL;
   ptins->NEXT    = NULL;

   SEPAR=' ';    /* nameindex() must be compatible with vectorradical()  */ 
   for (ptcon = ptnewfig->LOCON; ptcon; ptcon = ptcon->NEXT) {
     /*search genericity */
     isfake=0;
     p=getptype(ptcon->USER,GENCONCHAIN); 
     if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 183);
//       fprintf(stderr,"Build_loins: NULL pointer\n");
       EXIT(1);
     }   
     gencon=p->DATA;
     if (!gencon) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 184);
//       fprintf(stderr,"Build_loins: NULL pointer\n");
       EXIT(1);
     }   
     if (gencon->NEXT) { 
        avt_errmsg(GNS_ERRMSG, "106", AVT_FATAL, ptcon->NAME,insname);
/*       fprintf(stderr,"Build_Loins: several signals connected to %s in loins %s\n",
               ptcon->NAME,insname);*/
       EXIT(1);
     }   
     losig=gencon->LOSIG;
     head=getptype(losig->USER,GEN_MARK_PTYPE);
     if (!head) {

       if (gns_isunusedsig(losig))
         isfake=1;
       else
         {
        avt_errmsg(GNS_ERRMSG, "107", AVT_FATAL, ptcon->NAME,(char *)losig->NAMECHAIN->DATA);
/*           fprintf(stderr,"No search done on connector '%s' signal '%s', model '%s' must be a connexe graph\n",
                   ptcon->NAME,(char *)losig->NAMECHAIN->DATA, model->NAME);*/
           EXIT(1);
         }
     }
/*     gensigchain_ptype=getptype(losig->USER,GENSIGCHAIN); 
     if (!gensigchain_ptype) {
       fprintf(stderr,"Build_loins: NULL pointer\n");
       EXIT(1);
     }   */
     array_con=gencon->LOCON_EXP;      /*TOKEN=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/
     if (TOKEN(array_con)==GEN_TOKEN_TO) {
       /*low first*/
       begin_con=Eval_Exp_VHDL(array_con->NEXT->DATA/*low bound*/,env); 
       /*high bound after*/
       end_con=Eval_Exp_VHDL(array_con->NEXT->NEXT->DATA/*high bound*/,env);
       checkrange(begin_con, end_con, array_con);
     }
     else if (TOKEN(array_con)==GEN_TOKEN_DOWNTO) {
       /*high first*/
       begin_con=Eval_Exp_VHDL(array_con->NEXT->DATA/*high bound*/,env);
       /*low bound after*/
       end_con=Eval_Exp_VHDL(array_con->NEXT->NEXT->DATA/*low bound*/,env); 
       checkrange(end_con, begin_con, array_con);
     }
     else {
       begin_con=-1;
       end_con=-1;
     }   
     array_sig=gencon->LOSIG_EXP;      /*TOKEN(array_sig)=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/
     if (TOKEN(array_sig)==GEN_TOKEN_TO) {
       /*low first*/
       begin_sig=Eval_Exp_VHDL(array_sig->NEXT->DATA/*low bound*/,env); 
       /*high bound after*/
       end_sig=Eval_Exp_VHDL(array_sig->NEXT->NEXT->DATA/*high bound*/,env);
       checkrange(begin_sig, end_sig, array_sig);
     }
     else if (TOKEN(array_sig)==GEN_TOKEN_DOWNTO) {
       /*high first*/
       begin_sig=Eval_Exp_VHDL(array_sig->NEXT->DATA/*high bound*/,env);
       /*low bound after*/
       end_sig=Eval_Exp_VHDL(array_sig->NEXT->NEXT->DATA/*low bound*/,env); 
       checkrange(end_sig, begin_sig, array_sig);
     }
     else {
       begin_sig=-1;
       end_sig=-1;
     } 
     ri=getradindex(ptcon->NAME);
     for ( ;/*test on begin_con==end_con at the end of loop*/ ; ) {
       if (!isfake)
         {
           mark=GetMarkEA(gencon->LOSIG, begin_sig, (mark_list*)head->DATA);
           /*       for (mark=(mark_list*)head->DATA; mark; mark=mark->NEXT)
                    if (mark->BIT==begin_sig) break;*/
           if (!mark) {
        avt_errmsg(GNS_ERRMSG, "114", AVT_FATAL, ptcon->NAME,begin_con,model->NAME);
/*             fprintf(stderr,"No search done on connector '%s %ld', model '%s' must be a connexe graph(v2)\n",
                     ptcon->NAME,begin_con,model->NAME);*/
             EXIT(3);
           }
         }
       ptnewcon = (locon_list *)mbkalloc(sizeof(locon_list));
       if (end_con==-1) ptnewcon->NAME = ptcon->NAME;
       else ptnewcon->NAME = nameindex(ptcon->NAME,begin_con);

       if (isfake)
         ptnewcon->SIG=NULL;
       else
         ptnewcon->SIG  = mark->LOSIG;
#if 0
       /*change for future search the lofigchain*/
       printf("update lofigchain 0\n");
       p=getptype(mark->LOSIG->USER,LOFIGCHAIN);
       if (!p) {
         fprintf(stderr,"Build_loins: PTYPE not found\n");
         EXIT(1);
       }
       p->DATA=addchain(p->DATA,ptnewcon);
#endif
       /*normal fields*/
       ptnewcon->DIRECTION = ptcon->DIRECTION;
       ptnewcon->TYPE = 'I';
       ptnewcon->ROOT = (void *)ptins;      
//       ptnewcon->PNODE= NULL;
       AddRadicalInfoInLOCON(ptnewcon, ri, begin_con);
       ptnewcon->NEXT = ptins->LOCON;
       ptnewcon->USER=NULL;
       ptins->LOCON  = ptnewcon;
       
       /*test end of loop*/
       if (begin_con==end_con && begin_sig==end_sig) break;
       /*now error*/
       if (begin_con==end_con || begin_sig==end_sig) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 185);
//         fprintf(stderr,"Build_loins: computing error\n");
         EXIT(3);
       }
       /*move counter*/
       if (begin_con<end_con) begin_con++;
       else begin_con--;
       if (begin_sig<end_sig) begin_sig++;
       else begin_sig--;
     }
   }
   SEPAR=memo_char;              /* put last value in MBK environment */
   
   ptins->LOCON = (locon_list *)reverse((chain_list *)ptins->LOCON);
   ptins->USER  = addptype(ptins->USER,FCL_TRANSLIST_PTYPE,translist);
   ptins->USER  = addptype(ptins->USER,FCL_INSLIST_PTYPE,inslist);
   ptins->USER = (ptype_list *)append((chain_list *)ptins->USER, (chain_list *)GEN_USER);
//   GenPostOp(ptins);

   if (TRACE_MODE == 'Y')
     (void)fprintf(stdout, "--- mbk --- addloins  : %s of figure %s\n", 
                   insname, figname);
   
   /*build foundloins for next use*/
   var=NULL;
   for (p=env; p; p=p->NEXT) var=addptype(var,p->TYPE,p->DATA);
   // zinaps
   AddSymsFlagInLoinsLocon(ptins);

//   TrytoArrangeSymmetricConnectors(ptins);


//   AddRadicalInfoInPNODE(ptins);
/*   AddSymsToInstanceConnectors(ptins);
   AddCoupledToInstanceConnectors(ptins);*/
   add_foundins(model, ptins, var);
   ALL_NEW_INSTANCES=addchain(ALL_NEW_INSTANCES, ptins);
   /*erase marks in model*/
   freeallswaps();
   freeallfixedandallloconmarks();
   freeallmarkcounts();
   freeallmarks(model);
}


/****************************************************************************/
/*   final loins list for FCL  almost change names and execute C program    */
/****************************************************************************/
extern chain_list* Build_loins_list()
{
   chain_list *ret;
   char memo_char;
   foundmodel_list* fmodel;
   model_list *model;
   foundins_list *foundins, *next;
   ptype_list* var;
   int how_many, keep;
   lib_entry *le;
   long k;
   char *finalmodel;

   ret=NULL;
   for (fmodel=FOUND_MODEL; fmodel; fmodel=fmodel->NEXT) {
      model=fmodel->MODEL;  
      how_many=1;
      for (le=GENIUS_PRIORITY;le!=NULL && le->entity!=modelradical(model->NAME);le=le->NEXT) ;
      if (le==NULL) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 186);
//              fprintf(stderr,"model %s not found\n",modelradical(model->NAME));
              EXIT(64);
      }

      for (foundins=fmodel->INS ; foundins; foundins=next)  
        { 
          next=foundins->NEXT;
          
          if (le->keep || V_BOOL_TAB[__GEN_KEEP_ALL].VALUE) keep=1;
          else keep=0;

          memo_char=SEPAR;       /* MBK variable it is for concatenation*/
          SEPAR='_';              /* name for FCL */ 
          finalmodel=foundins->LOINS->FIGNAME;
          for (var=foundins->VAR; var; var=var->NEXT)  
            if (var->TYPE!=UNDEF) 
              finalmodel=nameindex(finalmodel, var->TYPE);
          SEPAR=memo_char;

          if ((k=gethtitem(LOINS_LIST_CONTROL, finalmodel))!=EMPTYHT)
            {
              // CONTROLED BY THE API
              if (k & 1) keep=1;
              else keep=0;
            }
          if ((k=gethtitem(LOINS_LIST_CONTROL, foundins->LOINS->INSNAME))!=EMPTYHT)
            {
              // CONTROLED BY THE API
              if (k & 1) keep=1;
              else keep=0;
            }

          if (keep)
            {
              if (foundins->SAVEDFLAG!=0)
                {
                  //                 PutBackRealLoins(foundins->LOINS);
                  erase_foundins(model, foundins->LOINS);
                  continue;
                }
              
              SetPNODEtoNULL(foundins->LOINS);
              
              foundins->LOINS->LOCON=(locon_list *)reverse((chain_list *)foundins->LOINS->LOCON);
              ret=addchain(ret,foundins->LOINS);
              foundins->LOINS->USER=delptype(foundins->LOINS->USER,GEN_FOUNDINS_PTYPE);
              foundins->LOINS->FIGNAME=finalmodel;
              /*name of instance*/
              /*             foundins->LOINS->INSNAME=
                             nameindex(foundins->LOINS->FIGNAME,how_many);               
              */
              k=gethtitem(NEW_LOINS_NAMES_HT, foundins->LOINS->INSNAME);
              if (k!=EMPTYHT)
                foundins->LOINS->FIGNAME=(char *)k;
              
              foundins->LOINS=NULL;         /*extract -> not erase later*/
              how_many++;
            }
          else
            {
              erase_foundins(model, foundins->LOINS);
            }
        }
   }  
   finish_erase_foundins();
   
   return ret;
}   

char *modelradical(char *name)
{
  char *s;
  char str[1000];

  s = strchr(name, ' ');
  
  if (!s)
    return name;
  
  strcpy(str, name);
  s=&str[s-name];
  
  *s = '\0';
  return namealloc(str);
} 

chain_list *GetFixedSignalList(loins_list *li, ptype_list *env, ptype_list *livar)
{
  int total=0, taken=0;
  chain_list *cl, *ch=NULL, *cl1;
  SymInfoItem *cl0;
  chain_list *radics=NULL;
  int f, l;
  long old_env_value;
  locon_list *lc;
  losig_list *ls;

//  printf("changing '%s' from %d to UNDEF\n", env->DATA, env->TYPE);
  old_env_value=env->TYPE;
/*  {
    ptype_list *p;
   for (p=env;p!=NULL; p=p->NEXT)
     {
       printf("- %s=%d \n",p->DATA,p->TYPE);
     }
  }*/
  env->TYPE=UNDEF;
  for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      // depends on loop variable?
     // printf("here\n");
      cl=GetMarkFromCON(lc, &f, &l, NULL, env, livar); // model=NULL, no error should happen
     // printf("hereend\n");
//      printf("(%d %d)",f,l);
      if (l!=UNDEF && f!=UNDEF)
        {
          taken++;
          ch=append(cl, ch);
          radics=addchain(radics, lc->NAME);
        }
      else
        freechain(cl);
      total++;
    }
  
  // looking for dependant, who are non dependant symmetric
//  printf("total=%d taken=%d | ", total, taken);

  for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      // depends on loop variable?
      cl=GetMarkFromCON(lc, &f, &l, NULL, env, livar); // model=NULL, no error should happen
      freechain(cl);
      if (l==UNDEF || f==UNDEF)
        {
          cl0=OldFashionGetSymmetricChain(lc);
          while (cl0!=NULL)
            {
              for (cl1=radics; cl1!=NULL && cl0->ConnectorRadical!=cl1->DATA; cl1=cl1->NEXT) ;
              if (cl1!=NULL) break;
              cl0=cl0->NEXT;
            }
          if (cl0!=NULL)
            {
              taken++;
              env->TYPE=old_env_value;
              cl=GetMarkFromCON(lc, &f, &l, NULL, env, livar);
              env->TYPE=UNDEF;
              ch=append(cl, ch);
              radics=addchain(radics, lc->NAME);
            }
        }
    }

  env->TYPE=old_env_value;
  freechain(radics);
//  printf("total=%d taken=%d\n", total, taken);
  if (total==taken)
    {
      freechain(ch);
      return NULL;
    }

  if (isparal || smartselect)
    {
      losig_list *bestsig=NULL;
      int bestcount=1000000, cnt;
      loins_list *li0;
//      ptype_list *p;
      for (cl=ch; cl!=NULL; cl=cl->NEXT)
        {
          ls=(losig_list *)cl->DATA;
          if (mbk_LosigIsVSS(ls) || mbk_LosigIsVDD(ls)) continue;
          cl1=(chain_list *)getptype(ls->USER, LOFIGCHAIN)->DATA;
          cnt=0;
          while (cl1!=NULL)
            {
              lc=(locon_list *)cl1->DATA;
              if (lc->TYPE!='T')
                {
                  li0=((loins_list *)lc->ROOT);
                  if ((((long)li0->NEXT) & 1)==0 && li0->FIGNAME==li->FIGNAME) cnt++;
                }
              cl1=cl1->NEXT;
            }
          if (cnt<bestcount)
            {
              bestcount=cnt;
              bestsig=(losig_list *)cl->DATA;
            }
        }
      
      if (bestsig==NULL) 
        {
        //  fprintf(stderr,"Could not find a way to handle loop on instance with figure '%s', using default behaviour\n",  li->FIGNAME);
        }
      else
        {
      //    printf("bestsig='%s', count=%d\n",bestsig->NAMECHAIN->DATA, bestcount);
          freechain(ch);
          ch=addchain(NULL , bestsig);
        }
    }


  for (cl=ch; cl!=NULL; cl=cl->NEXT)
    ((losig_list *)cl->DATA)->FLAGS &= ~GRAB_HAVE_BEEN_DONE;
  return ch;
}

int IsFORparallal(loins_list *li, ptype_list *env, lofig_list *model_lofig, int *smartselect)
{
  chain_list *cl;
  long old_env_value;
  locon_list *lc;
  ptype_list *allsigs=NULL, *p;
  int oldval, val, notparal=0;
  loins_list *loins;
  genconchain_list *gencon;
  losig_list *losig;

  old_env_value=env->TYPE;
  env->TYPE=UNDEF;
  
  *smartselect=0;

  for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      allsigs=GetFORdependantSingleSignalExpressions(lc, env, allsigs);
    }
  
  // les autres instances sont elles reliees par des signaux dependant de la boucle
  for (loins=model_lofig->LOINS; notparal==0 && loins!=NULL; loins=loins->NEXT)
    {
      if (loins==li) continue;
      for (lc=loins->LOCON; notparal==0 && lc!=NULL; lc=lc->NEXT)
        {
          p=getptype(lc->USER,GENCONCHAIN); 
          if (!p) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 187);
//            fprintf(stderr,"Build_loins: NULL pointer\n");
            EXIT(1);
          }   
          gencon=p->DATA;
          if (!gencon) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 188);
//            fprintf(stderr,"Build_loins: NULL pointer\n");
            EXIT(1);
          }   
          if (gencon->NEXT) {
        avt_errmsg(GNS_ERRMSG, "106", AVT_FATAL, lc->NAME,loins->INSNAME);
/*          fprintf(stderr,"Build_Loins: several signals connected to %s in loins %s\n",
                    lc->NAME,loins->INSNAME);*/
            EXIT(1);
          }   
          losig=gencon->LOSIG;
          for (p=allsigs; p!=NULL; p=p->NEXT)
            {
              if ((losig_list *)p->TYPE==losig)
                {
                  gen_printf(3,"instance '%s' depends on loop search thru signal '%s', fastmode disabled\n",loins->INSNAME,losig->NAMECHAIN->DATA);
                  notparal=1;
                  break;
                }
            }
        }
    }

  env->TYPE=10007;

  for (p=allsigs; p!=NULL; p=p->NEXT)
    {
      if (notparal==0)
        {
//          printf("testing : %s ",((losig_list *)p->TYPE)->NAMECHAIN->DATA);
          oldval=UNDEF;      
          for (cl=(chain_list *)p->DATA; cl!=NULL; cl=cl->NEXT)
            {
              val=Eval_Exp_VHDL((tree_list *)cl->DATA,env);
              if (oldval!=UNDEF && val!=oldval) { notparal=1; break; }
              oldval=val;
            }
//          printf("%s\n",notparal==0?"PARAL":"NOT PARAL");
        }
      freechain((chain_list *)p->DATA);
    }

  env->TYPE=old_env_value;
  freeptype(allsigs);

  for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      gensigchain_list *gensig;
      if ((p=getptype(lc->SIG->USER,GENSIGCHAIN))!=NULL)
        {
          for (gensig=p->DATA; gensig!=NULL; gensig=gensig->NEXT)
            {
              if (gensig->LOCON->TYPE=='I' && gensig->LOCON->ROOT!=li) break;
            }
          if (gensig!=NULL && OldFashionGetSymmetricChain(gensig->LOCON)!=NULL) break;
        } 
    }

  if (lc!=NULL)
    {
      gen_printf(3,"instance '%s' is linked to switchable internal signal, fastmode disabled\n",li->INSNAME);
      notparal=1;
      *smartselect=1;
      return !notparal;
    }

  if (!notparal) *smartselect=1;

  return !notparal;
}

void UpdateTOPInstancesCorrespondanceTable()
{
   chain_list *ret;
   foundmodel_list* fmodel;
   model_list *model;
   foundins_list *foundins;
   lofig_list *gilles_lofig;
   ptype_list *p;
   corresp_t *crt;
   subinst_t *sins=NULL;

   ret=NULL;
   
   gen_add_all_lofig_weight();

   for (fmodel=FOUND_MODEL; fmodel; fmodel=fmodel->NEXT) 
     {
       model=fmodel->MODEL;

       for (foundins=fmodel->INS ; foundins; foundins=foundins->NEXT)  
         { 
           p=getptype(foundins->LOINS->USER, GEN_USER_PTYPE);
           if (p==NULL) EXIT(14);
           crt=(corresp_t *)p->DATA;
           crt->FLAGS|=LOINS_IS_TOPLEVEL;
           if (foundins->SAVEDFLAG==0)
             {

               if ((GEN_OPTIONS_PACK & GEN_NO_ORDERING)==0)
                 ArrangeInstanceConnectors(foundins->LOINS);

               gilles_lofig=gen_getlofig(crt->GENIUS_FIGNAME);
//               printf("update %s\n",crt->GENIUS_FIGNAME);
               gen_update_alims (gilles_lofig, crt, NULL);
               gen_update_corresp_table(gilles_lofig, crt, foundins->LOINS);
             }
//               flags = (long)getptype (foundins->VISITED->USER, GENIUS_LOINS_FLAGS_PTYPE)->DATA;
           sins=Add_subinst(sins, foundins->LOINS->INSNAME, crt, crt->FLAGS);
         }
     }  
   LATEST_GNS_RUN->TOP_LEVEL_SUBINST=sins;
}   



int gns_GetInstanceLoopIndex(loins_list *ins, char **ptptname)
{
  ptype_list *p;
  if (ptptname!=NULL)
    {
      if ((p=getptype(ins->USER, GEN_ORIG_LOINS_PTYPE))==NULL)
        *ptptname=ins->INSNAME;
      else 
        *ptptname=((loins_list *)p->DATA)->INSNAME;
    }
  if ((p=getptype(ins->USER,GEN_LOOP_INDEX_PTYPE))!=NULL)
    return (int)(long)p->DATA;
  return -1;
}

void gns_GetInstanceLoopRange(lofig_list *lf, loins_list *ins, int *left, int *right)
{
  ptype_list *p;
  loins_list *li;
  char *ptptname, *ptptname0;
  int min=1000000, max=-1, idx;
  if ((p=getptype(ins->USER, GEN_ORIG_LOINS_PTYPE))==NULL)
    ptptname=ins->INSNAME;
  else 
    ptptname=((loins_list *)p->DATA)->INSNAME;

  if ((p=getptype(ins->USER,GEN_LOOP_INDEX_PTYPE))==NULL)
    {
      *left=*right=-1;
      return;
    }

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      if ((p=getptype(li->USER, GEN_ORIG_LOINS_PTYPE))==NULL)
        ptptname0=li->INSNAME;
      else 
        ptptname0=((loins_list *)p->DATA)->INSNAME;

      if (ptptname0==ptptname)
        {
          if ((p=getptype(li->USER,GEN_LOOP_INDEX_PTYPE))==NULL) EXIT(102);
          idx=(int)(long)p->DATA;
          if (idx<min) min=idx;
          if (idx>max) max=idx;
        }
    }
  *left=min;
  *right=max;
}

chain_list *gen_GetInstanceWithSameRadical(lofig_list *lf, char *radical)
{
  loins_list *ins;
  lotrs_list *tr;
  ptype_list *p;
  chain_list *names=NULL;
  char *name;

  for (ins=lf->LOINS; ins!=NULL; ins=ins->NEXT)
    {
      if ((p=getptype(ins->USER, GEN_ORIG_LOINS_PTYPE))==NULL)
        {
          if (strcmp(ins->INSNAME, radical)==0) names=addchain(names, ins->INSNAME);
        }
      else 
        {
          name=((loins_list *)p->DATA)->INSNAME;
          if (strcmp(name, radical)==0) names=addchain(names, ins->INSNAME);
        }
    }
  for (tr=lf->LOTRS; tr!=NULL; tr=tr->NEXT)
    {
      if ((p=getptype(tr->USER, GEN_ORIG_LOINS_PTYPE))==NULL)
        {
          if (strcmp(tr->TRNAME, radical)==0) names=addchain(names, tr->TRNAME);
        }
      else 
        {
          name=((loins_list *)p->DATA)->INSNAME;
          if (strcmp(name, radical)==0) names=addchain(names, name);
        }
    }
  return names;
}

int gns_isforcematch(char *cirname, char *modelname)
{
  int l1, l2, i;
  int cc;
  l2=strlen(cirname);
  l1=strlen(modelname);
  if (l1>l2) return 0;
  if (l2!=l1 && cirname[l2-l1-1]!=SEPAR) return 0;
  for (i=0, cc=l2-l1; i<l1; i++, cc++)
    {
      if (!(mbk_charcmp(cirname[cc], modelname[i]) || (modelname[i]=='_' && cirname[cc]==SEPAR))) return 0;
    }
  return 1;
}

typedef struct
{
  lotrs_list *mtrs, *ctrs;
} match;

static int sorttrs(const void *a, const void *b)
{
  return mbk_casestrcmp(((match *)a)->mtrs->TRNAME, ((match *)b)->mtrs->TRNAME);
}

#define TEMP_MATCHCODE 0xfab90529

static void addmatchinfo(locon_list *lc, char *name)
{
  ptype_list *p;
  if ((p=getptype(lc->SIG->USER, TEMP_MATCHCODE))==NULL)
    p=lc->SIG->USER=addptype(lc->SIG->USER, TEMP_MATCHCODE, NULL);
  p->DATA=addchain((chain_list *)p->DATA, name);
}

static void freematchinfo(locon_list *lc)
{
  ptype_list *p;
  if ((p=getptype(lc->SIG->USER, TEMP_MATCHCODE))!=NULL)
    {
      freechain((chain_list *)p->DATA);
      lc->SIG->USER=delptype(lc->SIG->USER, TEMP_MATCHCODE);
    }
}

static int matchedinfo(locon_list *a, locon_list *b)
{
  ptype_list *p;
  chain_list *la, *lb;
  if ((p=getptype(a->SIG->USER, TEMP_MATCHCODE))==NULL) return 0;
  la=(chain_list *)p->DATA;
  if ((p=getptype(b->SIG->USER, TEMP_MATCHCODE))==NULL) return 0;
  lb=(chain_list *)p->DATA;
  while (la!=NULL && lb!=NULL && la->DATA==lb->DATA) la=la->NEXT, lb=lb->NEXT;
  return la==NULL && lb==NULL;
}

static int fillcorresp(ptype_list **p, void *val)
{
  ptype_list *pt;
  if ((pt=getptype(*p, FCL_CORRESP_PTYPE))==NULL)
  {
   *p=addptype(*p, FCL_CORRESP_PTYPE, val);
   return 0;
  }
  return pt->DATA!=val;
}

static void *getcorresp(ptype_list *p)
{
  ptype_list *pt;
  if ((pt=getptype(p, FCL_CORRESP_PTYPE))!=NULL) return pt->DATA;
  return NULL;
}

static void removecorresp(ptype_list **p)
{
  *p=testanddelptype(*p, FCL_CORRESP_PTYPE);
}

chain_list *GrabBlackboxAsUnused(char *modelname, lofig_list *model, lofig_list *netlist)
{
  lotrs_list *tr, *trm;
  int nb, i, count, failed;
  match *trstab;
  ht *inst;
  ptype_list *p;
  char *c, *found, *instname;
  long l;
  chain_list *cl, *ch, *ret, *tmp;
  losig_list *ls;
  
  ret=NULL;
  
  nb=countchain((chain_list *)model->LOTRS);
  trstab=(match *)mbkalloc(sizeof(match)*nb);
  for (i=0, tr=model->LOTRS; tr; tr=tr->NEXT, i++)
    trstab[i].mtrs=tr;
  qsort(trstab, nb, sizeof (match), sorttrs);

  inst=addht(1000);
  for (tr=netlist->LOTRS; tr; tr=tr->NEXT)
  {
    if ((p=getptype(tr->USER, BBOX_AS_UNUSED))!=NULL)
    {
      c=(char *)p->DATA;
      found=strchr(c, '`');
      if (strncmp(c, modelname, strlen(modelname))==0)
      {
        c=namealloc(found+1);
        if ((l=gethtitem(inst, c))==EMPTYHT) l=(long)NULL;
        addhtitem(inst, c, (long)addchain((chain_list *)l, tr));
      }
    }
  }
  
  for (i=0; i<nb; i++)
  {
    tr=trstab[i].mtrs;
    addmatchinfo(tr->GRID, tr->TRNAME);
    addmatchinfo(tr->SOURCE, tr->TRNAME);
    addmatchinfo(tr->DRAIN, tr->TRNAME);
    if (tr->BULK) addmatchinfo(tr->BULK, tr->TRNAME);
  }
 
  cl=GetAllHTKeys(inst);
  while (cl!=NULL)
  {
    ch=(chain_list *)gethtitem(inst, instname=(char *)cl->DATA);
    count=0;
    for (i=0; i<nb; i++) trstab[i].ctrs=NULL;
    while (ch!=NULL)
    {
      tr=ch->DATA;
      for (i=0; i<nb; i++)
        if (trstab[i].ctrs==NULL && gns_isforcematch(tr->TRNAME, trstab[i].mtrs->TRNAME)) break;
      if (i<nb)
      {
        trstab[i].ctrs=tr;
        count++;
      }
      else
      {
        printf("unmatched transistor in unused blackbox: instance=%s trs=%s\n",instname,tr->TRNAME);
      }
      ch=delchain(ch, ch);
    } 

    if (count==nb)
    {
      for (i=0; i<nb; i++)
      {
        tr=trstab[i].ctrs;
        addmatchinfo(tr->GRID, trstab[i].mtrs->TRNAME);
        addmatchinfo(tr->SOURCE, trstab[i].mtrs->TRNAME);
        addmatchinfo(tr->DRAIN, trstab[i].mtrs->TRNAME);
        if (tr->BULK) addmatchinfo(tr->BULK, trstab[i].mtrs->TRNAME);
        fillcorresp(&trstab[i].mtrs->USER, tr);        
      }

      failed=0;
      for (i=0; i<nb && !failed; i++)
      {
        trm=trstab[i].mtrs;
        tr=trstab[i].ctrs;
        if (!matchedinfo(tr->GRID, trm->GRID) || !matchedinfo(tr->BULK, trm->BULK)) failed=1;
        else if (fillcorresp(&trm->GRID->SIG->USER, tr->GRID->SIG) || fillcorresp(&trm->BULK->SIG->USER, tr->BULK->SIG)) failed=1;
        if (!failed)
        {
         if (matchedinfo(tr->SOURCE, trm->SOURCE) && matchedinfo(tr->DRAIN, trm->DRAIN))
          {
            if (fillcorresp(&trm->SOURCE->SIG->USER, tr->SOURCE->SIG)
                || fillcorresp(&trm->DRAIN->SIG->USER, tr->DRAIN->SIG)) failed=1;
          }
         if (matchedinfo(tr->SOURCE, trm->DRAIN) && matchedinfo(tr->DRAIN, trm->SOURCE))
          {
            if (fillcorresp(&trm->SOURCE->SIG->USER, tr->DRAIN->SIG)
                || fillcorresp(&trm->DRAIN->SIG->USER, tr->SOURCE->SIG)) failed=1;
          }
        }
      }

      if (!failed)
      {
        loins_list *ptloins;
        locon_list *ptlocon, *ptnewcon;
        losig_list *ptlosig;
        ptloins = (loins_list *)mbkalloc(sizeof(loins_list));
        ptloins->INSNAME = instname;
        ptloins->FIGNAME = model->NAME;
        ptloins->LOCON = NULL;
        ptloins->USER = NULL;
        ptloins->NEXT = NULL;

        for (ptlocon = model->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            ptlosig = (losig_list *)getcorresp(ptlocon->SIG->USER);
            if (ptlosig == NULL) {
                fprintf(stderr, "[FCL ERR] connector '%s' in model '%s' has no correspondance in circuit\n", ptlocon->NAME, model->NAME);
                continue;
            }
            ptnewcon = (locon_list *)mbkalloc(sizeof(locon_list));
            ptnewcon->NAME = ptlocon->NAME;
            ptnewcon->DIRECTION = ptlocon->DIRECTION;
            ptnewcon->TYPE = 'I';
            ptnewcon->SIG  = ptlosig;
            ptnewcon->ROOT = ptloins;
            ptnewcon->USER = NULL;
            ptnewcon->PNODE= NULL;
            ptnewcon->NEXT = ptloins->LOCON;
            ptloins->LOCON  = ptnewcon;
        }
        ptloins->LOCON = (locon_list *)reverse((chain_list *)ptloins->LOCON);
        tmp=NULL;
        for (i=0; i<nb; i++) tmp=addchain(tmp, trstab[i].ctrs);
        ptloins->USER = addptype(ptloins->USER, FCL_TRANSLIST_PTYPE, tmp);
        geniusExecuteAction(ptloins);
        ret=addchain(ret, ptloins);
      }

      for (i=0; i<nb; i++)
      {
        tr=trstab[i].ctrs;
        freematchinfo(tr->GRID);
        freematchinfo(tr->SOURCE);
        freematchinfo(tr->DRAIN);
        if (tr->BULK) freematchinfo(tr->BULK);
        tr=trstab[i].mtrs;
        removecorresp(&tr->USER);
      }
      for (ls=model->LOSIG; ls; ls=ls->NEXT)
        removecorresp(&ls->USER);
    }

    cl=delchain(cl, cl);
  }

  for (i=0; i<nb; i++)
  {
    tr=trstab[i].mtrs;
    freematchinfo(tr->GRID);
    freematchinfo(tr->SOURCE);
    freematchinfo(tr->DRAIN);
    if (tr->BULK) freematchinfo(tr->BULK);
  }

  delht(inst);
  mbkfree(trstab);
  fclUpdateLofigchain(ret, model->NAME);
  return ret;
}

static void recur_IsModelFullyConnected(lotrs_list *lt);

static void recur_goto(losig_list *ls)
{
  ptype_list *pt;
  locon_list *lc;
  chain_list *cl;
  if (getptype(ls->USER, TEMP_MATCHCODE)!=NULL) return;
  ls->USER=addptype(ls->USER, TEMP_MATCHCODE, NULL);
  if (mbk_LosigIsVDD(ls) || mbk_LosigIsVSS(ls)) return;
  if ((pt=getptype(ls->USER, LOFIGCHAIN))!=NULL)
  {
    for (cl=(chain_list *)pt->DATA; cl; cl=cl->NEXT)
    {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='T') recur_IsModelFullyConnected((lotrs_list *)lc->ROOT);
    }
  }
}

static void recur_IsModelFullyConnected(lotrs_list *lt)
{
  if (getptype(lt->USER, TEMP_MATCHCODE)!=NULL) return;
  lt->USER=addptype(lt->USER, TEMP_MATCHCODE, NULL);
  recur_goto(lt->GRID->SIG);
  recur_goto(lt->DRAIN->SIG);
  recur_goto(lt->SOURCE->SIG);
  if (lt->BULK) recur_goto(lt->BULK->SIG);
}

int IsModelFullyConnected(lofig_list *lf)
{
  lotrs_list *lt;
  losig_list *ls;
  int ret=0;
  if (lf->LOTRS!=NULL)
   recur_IsModelFullyConnected(lf->LOTRS);
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
  {
    if (getptype(ls->USER, TEMP_MATCHCODE)==NULL) ret=1;
    ls->USER=testanddelptype(ls->USER, TEMP_MATCHCODE);
  }
  for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
  {
    if (getptype(lt->USER, TEMP_MATCHCODE)==NULL) ret=1;
    lt->USER=testanddelptype(lt->USER, TEMP_MATCHCODE);
  }
  return !ret;
}
