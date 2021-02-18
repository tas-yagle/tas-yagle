/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_main.c                                                  */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 04/05/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <errno.h>
#include <tcl.h>

#include MUT_H
#include MLO_H
#include MLU_H
#include BEG_H

#include API_H

#ifdef AVERTEC
#include AVT_H
#endif


#include GEN_H
#include "gen_env.h"
#include "gen_model_utils.h"
#include "gen_model_global.h"
#include "gen_verif_global.h"
#include "gen_search_global.h"
#include "gen_search_utils.h"
//#include "gen_corresp.h"
//#include "gen_search_utils.h"
#include "gen_new_kind.h"
#include "gen_main.h"
#include "gen_display.h"
#include "gen_globals.h"
//#include "gen_corresp.h"

ht *gns_create_template_hierarchy(lib_entry *files_list);

void SaveContext(GeniusContext *GC)
{
    // gen_corresp.c
  GC->GEN_HEAD_LOFIG=GEN_HEAD_LOFIG; GEN_HEAD_LOFIG=NULL;
  GC->GEN_HT_LOFIG=GEN_HT_LOFIG; GEN_HT_LOFIG=NULL;

  memcpy(&GC->corresp_heap, &corresp_heap, sizeof(HeapAlloc));
  memcpy(&GC->subinst_heap, &subinst_heap, sizeof(HeapAlloc));

  GC->GENIUS_TO_SPY=GENIUS_TO_SPY; GENIUS_TO_SPY=NULL;
  GC->GENIUS_SPY=GENIUS_SPY; GENIUS_SPY=NULL;

  // gen_env.c
  GC->GENIUS_LIB_NAME=GENIUS_LIB_NAME; GENIUS_LIB_NAME=NULL;
  GC->GENIUS_LIB_PATH=GENIUS_LIB_PATH; GENIUS_LIB_PATH=NULL;
  GC->GENIUS_TREE=GENIUS_TREE; GENIUS_TREE=NULL;
  GC->GENIUS_PRIORITY=GENIUS_PRIORITY; GENIUS_PRIORITY=NULL;
  GC->GENIUS_OUTPUT=GENIUS_OUTPUT; GENIUS_OUTPUT=NULL;
  GC->GEN_DEBUG_LEVEL=GEN_DEBUG_LEVEL; GEN_DEBUG_LEVEL=0;
  GC->GEN_OPTIONS_PACK=GEN_OPTIONS_PACK; GEN_OPTIONS_PACK=0;
  GC->genius_ec=genius_ec; genius_ec=NULL;

  // gen_execute_VHDL.c
  GC->VAR_UNKNOWN=VAR_UNKNOWN; VAR_UNKNOWN=0;
  GC->VAR_UNDEF=VAR_UNDEF; VAR_UNDEF=0;

  // gen_model_utils.c
  GC->GEN_SEPAR=GEN_SEPAR; GEN_SEPAR='.';
  GC->blocks_to_free=blocks_to_free; blocks_to_free=NULL;
  GC->HEAD_MODEL=HEAD_MODEL; HEAD_MODEL=NULL;
  GC->HEAD_MARK=HEAD_MARK; HEAD_MARK=NULL;

  GC->GENIUS_HEAD_LOFIG=GENIUS_HEAD_LOFIG; GENIUS_HEAD_LOFIG=NULL;
  GC->GENIUS_HT_LOFIG=GENIUS_HT_LOFIG; GENIUS_HT_LOFIG=NULL;
  GC->ModelTrace=ModelTrace; ModelTrace=NULL;

  //gen_new_king.c
  memcpy(&GC->ea_heap, &ea_heap, sizeof(HeapAlloc));

  //gen_optimization.c
  memcpy(&GC->lcu_heap, &lcu_heap, sizeof(HeapAlloc));
  memcpy(&GC->stat_heap, &stat_heap, sizeof(HeapAlloc));

  //gen_search_global.c
  GC->GENIUS_GLOBAL_LOFIG=GENIUS_GLOBAL_LOFIG;

  // gen_search_utils.c
  memcpy(GC->tabs, tabs, 200); strcpy(tabs,"");
  GC->current_lofig=current_lofig;
  GC->ALL_ENV=ALL_ENV;
  GC->GEN_USER=GEN_USER;
  GC->NEW_LOINS_NAMES_HT=NEW_LOINS_NAMES_HT; NEW_LOINS_NAMES_HT=NULL;
  GC->LOINS_LIST_CONTROL=LOINS_LIST_CONTROL; LOINS_LIST_CONTROL=NULL;
  GC->ALL_KIND_OF_MODELS=ALL_KIND_OF_MODELS; ALL_KIND_OF_MODELS=NULL;
  GC->ALL_LOINS_FOUND=ALL_LOINS_FOUND; ALL_LOINS_FOUND=NULL;
  GC->CUR_HIER_LOFIG=CUR_HIER_LOFIG;
  GC->CUR_CORRESP_TABLE=CUR_CORRESP_TABLE;
  memcpy(&GC->all_loins_heap, &all_loins_heap, sizeof(HeapAlloc));
  memcpy(&GC->loconmark_heap, &loconmark_heap, sizeof(HeapAlloc));

  //gen_symmetric.c
  memcpy(&GC->swap_heap, &swap_heap, sizeof(HeapAlloc));
  GC->radindexht=radindexht;
  GC->radtable=radtable;
  GC->curradindex=curradindex;
  GC->scht=scht;
  GC->myallocs=myallocs;

  memcpy(GC->ALL_SYM_INFO, ALL_SYM_INFO, sizeof(biinfo *)*MAX_SYMLIST);
  GC->CUR_SYM_INDEX=CUR_SYM_INDEX;

  memcpy(GC->entry, entry, sizeof(struct localcash)*CASHS);
  GC->FOUND_MODEL=FOUND_MODEL;

  GC->TEMPLATE_HT=GNS_TEMPLATE_HT;
}

void RestoreContext(GeniusContext *GC)
{
    // gen_corresp.c
  GEN_HEAD_LOFIG=GC->GEN_HEAD_LOFIG;
  GEN_HT_LOFIG=GC->GEN_HT_LOFIG;
//  TRANS_TO_SUPPRESS=GC->TRANS_TO_SUPPRESS;

  memcpy(&corresp_heap, &GC->corresp_heap, sizeof(HeapAlloc));
  memcpy(&subinst_heap, &GC->subinst_heap, sizeof(HeapAlloc));

  GENIUS_TO_SPY=GC->GENIUS_TO_SPY;
  GENIUS_SPY=GC->GENIUS_SPY;

  // gen_env.c
  GENIUS_LIB_NAME=GC->GENIUS_LIB_NAME;
  GENIUS_LIB_PATH=GC->GENIUS_LIB_PATH;
  GENIUS_TREE=GC->GENIUS_TREE;
  GENIUS_PRIORITY=GC->GENIUS_PRIORITY;
  GENIUS_OUTPUT=GC->GENIUS_OUTPUT;
  GEN_DEBUG_LEVEL=GC->GEN_DEBUG_LEVEL;
  GEN_OPTIONS_PACK=GC->GEN_OPTIONS_PACK;
  genius_ec=GC->genius_ec;

  // gen_execute_VHDL.c
  VAR_UNKNOWN=GC->VAR_UNKNOWN;
  VAR_UNDEF=GC->VAR_UNDEF;

  // gen_model_utils.c
  GEN_SEPAR=GC->GEN_SEPAR;
  blocks_to_free=GC->blocks_to_free;
  HEAD_MODEL=GC->HEAD_MODEL;
  HEAD_MARK=GC->HEAD_MARK;

  GENIUS_HEAD_LOFIG=GC->GENIUS_HEAD_LOFIG;
  GENIUS_HT_LOFIG=GC->GENIUS_HT_LOFIG;
  ModelTrace=GC->ModelTrace;

  //gen_new_king.c
  memcpy(&ea_heap, &GC->ea_heap, sizeof(HeapAlloc));

  //gen_optimization.c
  memcpy(&lcu_heap, &GC->lcu_heap, sizeof(HeapAlloc));
  memcpy(&stat_heap, &GC->stat_heap, sizeof(HeapAlloc));

  //gen_search_global.c
  GENIUS_GLOBAL_LOFIG=GC->GENIUS_GLOBAL_LOFIG;

  // gen_search_utils.c
  memcpy(tabs, GC->tabs, 200);
  current_lofig=GC->current_lofig;
  ALL_ENV=GC->ALL_ENV;
  GEN_USER=GC->GEN_USER;
  NEW_LOINS_NAMES_HT=GC->NEW_LOINS_NAMES_HT;
  LOINS_LIST_CONTROL=GC->LOINS_LIST_CONTROL;
  ALL_KIND_OF_MODELS=GC->ALL_KIND_OF_MODELS;
  ALL_LOINS_FOUND=GC->ALL_LOINS_FOUND;
  CUR_HIER_LOFIG=GC->CUR_HIER_LOFIG;
  CUR_CORRESP_TABLE=GC->CUR_CORRESP_TABLE;
  memcpy(&all_loins_heap, &GC->all_loins_heap, sizeof(HeapAlloc));
  memcpy(&loconmark_heap, &GC->loconmark_heap, sizeof(HeapAlloc));

  //gen_symmetric.c
  memcpy(&swap_heap, &GC->swap_heap, sizeof(HeapAlloc));
  radindexht=GC->radindexht;
  radtable=GC->radtable;
  curradindex=GC->curradindex;
  scht=GC->scht;
  myallocs=GC->myallocs;

  memcpy(ALL_SYM_INFO, GC->ALL_SYM_INFO, sizeof(biinfo *)*MAX_SYMLIST);
  CUR_SYM_INDEX=GC->CUR_SYM_INDEX;

  memcpy(entry, GC->entry, sizeof(struct localcash)*CASHS);
  FOUND_MODEL=GC->FOUND_MODEL;

  GNS_TEMPLATE_HT=GC->TEMPLATE_HT;
}



static ht *MAIN_HT_LOFIG=NULL;
static int GNS_RECURSION=0;

lofig_list *genius_getmainheadloadedlofig(char *name)
{
  long l;
  if ((l=gethtitem(MAIN_HT_LOFIG, name))==EMPTYHT) return NULL;
  return (lofig_list *)l;
}



#define PRIME 211 /* undivisable number */

/* LEX&YACC functions parser */
/* from gen_tree_parser.yac */
extern tree_list *Read_All(lib_entry *);

extern void SearchInit();
extern void StatHeap_Manage(int mode);
extern void SwapHeap_Manage(int mode);
extern void LoconMarkHeap_Manage(int mode);
extern void EAHeap_Manage(int mode);
extern void LofigChainUpgradeHeap_Manage(int mode);
extern void AllLoinsHeap_Manage(int mode);
extern void CorrespHeap_Manage(int mode);

typedef void (*heapfunc)(int);

static heapfunc heapfuncs[]=
  {
    StatHeap_Manage,
    SwapHeap_Manage, 
    LoconMarkHeap_Manage,
    EAHeap_Manage,
    LofigChainUpgradeHeap_Manage,
    AllLoinsHeap_Manage,
    CorrespHeap_Manage
  };

static heapfunc heapfreefuncs[]=
  {
    StatHeap_Manage,
    SwapHeap_Manage, 
    LoconMarkHeap_Manage,
    EAHeap_Manage,
    LofigChainUpgradeHeap_Manage,
    AllLoinsHeap_Manage
  };

/****************************************************************************/
/* Parse from a LIBRARY files user's models and extract them from netlist   */
/****************************************************************************/

static void gns_ordersignalnames(lofig_list *lf)
{
  losig_list *ls;
  char *signame;
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
  {
    if (ls->NAMECHAIN!=NULL && ls->NAMECHAIN->NEXT!=NULL)
    {
      signame=getsigname(ls);
      ls->NAMECHAIN=delchaindata(ls->NAMECHAIN, signame);
      ls->NAMECHAIN=addchain(ls->NAMECHAIN, signame);
    }
  }
}

extern chain_list* genius_main(lofig_list *netlist, char *cellib, char *libname, char *outname)
{
   int err;
   ptype_list *p,*bi_list;
   chain_list *fcl=NULL,*genius=NULL;
   chain_list *ret, *ret2;
   GeniusContext GC;
   lofig_list *(*tmpfunc)(char *name);
   mbkContext *oldctx;
   mbkContext newcontext;
   loins_list *bbox;


#ifdef AVERTEC_LICENSE
   if(avt_givetoken("YAGLE_LICENSE_SERVER", "gns")!=AVT_VALID_TOKEN) EXIT(1) ;
#endif
   
   gns_ordersignalnames(netlist);

   if (GNS_RECURSION==0)
     {
       MAIN_HT_LOFIG=HT_LOFIG;
       HOOK_GETLOADEDLOFIG=genius_getmainheadloadedlofig;
     }
   GNS_RECURSION++;

   SaveContext(&GC);
   oldctx=genius_external_getcontext();
   newcontext.HT_LOFIG=HT_LOFIG;
   newcontext.HEAD_LOFIG=HEAD_LOFIG;
   genius_external_setcontext(&newcontext);

   /* init environment */
   genius_env(netlist, cellib, libname);

   beg_init();
   APIInit();
   genius_ec=APINewExecutionContext();

   // initialisation des heaps
   for (err=0; err<(signed)(sizeof(heapfuncs)/sizeof(*heapfuncs)); err++)
     heapfuncs[err](0);

   GENIUS_HEAD_LOFIG=GEN_HEAD_LOFIG; GENIUS_HT_LOFIG=GEN_HT_LOFIG;
   SwitchLOFIGContext(); // => genius
//   for (lf=HEAD_LOFIG; lf!=NULL; lf=lf->NEXT) unlocklofig(lf) ;
   delalllofig();
   SwitchLOFIGContext(); // => normal

   GENIUS_HEAD_LOFIG=NULL; GENIUS_HT_LOFIG=NULL; NEW_LOINS_NAMES_HT=addht(10);
   LOINS_LIST_CONTROL=addht(10);
   GEN_HEAD_LOFIG=NULL; GEN_HT_LOFIG=NULL;

   SwitchLOFIGContext(); // => genius

   SearchInit();
   InitModels();

   /* ----> GENIUS_PRIORITY */

   GENIUS_PRIORITY = APIReadLibrary(GENIUS_LIB_NAME,GENIUS_LIB_PATH, GEN_DEBUG_LEVEL>0?GENIUS_OUTPUT:NULL);
   err = Get_Error();
   if (err) {
        avt_errmsg(GNS_ERRMSG, "039", AVT_ERR, err);
       //fprintf(stderr,"*** %d error(s) detected,  I can't get farther!! ***\n",err);
       EXIT(err);
   }
   if (!GENIUS_PRIORITY) return NULL;
   
   Build_All_Transistor_Models();

   LoadSpiceFCLFiles(GENIUS_PRIORITY);

   GNS_TEMPLATE_HT=gns_create_template_hierarchy(GENIUS_PRIORITY);

   APIParseFile_SetTemplateInfo(GNS_TEMPLATE_HT);

   GENIUS_TREE = Read_All(GENIUS_PRIORITY);  /* ----> GENIUS_TREE */

   APIParseFile_SetTemplateInfo(NULL);

   if (!GENIUS_TREE) return NULL;
   APIVerify_C_Functions(genius_ec);

   err = Get_Error();
   if (err) {
       avt_errmsg(GNS_ERRMSG, "039", AVT_ERR, err);
       //fprintf(stderr,"*** %d error(s) detected,  I can't get farther!! ***\n",err);
       EXIT(err);
   }

   // call user init function
   {
     APICallFunc cf;
     t_arg *ret;
     cf.NAME="GnsInit";
     cf.ARGS=NULL;
     if (APIExecAPICallFunc(genius_ec, &cf, &ret, 1)==0)
        APIFreeTARG(ret);
   } 

   
   /*get a tree_list for GENIUS and FCL */
   bi_list=Verif_All(GENIUS_TREE);
   for (p=bi_list; p; p=p->NEXT) {
      if (p->TYPE==FCL) fcl=(chain_list*)p->DATA;
      if (p->TYPE==GENIUS) genius=(chain_list*)p->DATA;
   }
   freeptype(bi_list);
   err = Get_Error();   
   if (err) {
       avt_errmsg(GNS_ERRMSG, "039", AVT_ERR, err);
       //fprintf(stderr,"\n%d error(s) detected,  I can't get farther!!\n",err);
       EXIT(err);
   }



   if (GEN_DEBUG_LEVEL>0) LoadDynamicLibraries(GENIUS_OUTPUT);
   else LoadDynamicLibraries(NULL);
   
   if (genius_ec->ALL_USED_FUNCTIONS!=NULL && APICheckCFunctions(genius_ec)) EXIT(1);

   {
     t_arg *ret;
     chain_list *p;
     libfunc_type func;
     char buf[128];
     for (p=API_ACTION_RESTART; p!=NULL; p=p->NEXT)
       {
         func=(libfunc_type)p->DATA;
         func(&ret, NULL, 0 , buf);
         mbkfree(ret->TYPE); mbkfree(ret);
       }
   }


#ifdef AVERTEC_LICENSE
   if(avt_givetoken("YAGLE_LICENSE_SERVER", "gns")!=AVT_VALID_TOKEN) EXIT(1) ;
#endif
   Build_All_Models(GENIUS_TREE,fcl);

   /*search*/
   ret=NULL;

   SwitchLOFIGContext(); // => normal


   LATEST_GNS_RUN=(ALL_FOR_GNS *)mbkalloc(sizeof(ALL_FOR_GNS));
   LATEST_GNS_RUN->ALL_INSTANCES=NULL;
   LATEST_GNS_RUN->FIGNAME=netlist->NAME;
   LATEST_GNS_RUN->GLOBAL_LOFIG=netlist;
   LATEST_GNS_RUN->TEMPLATE_HT=GNS_TEMPLATE_HT;
   tmpfunc=external_getlofig;
   external_getlofig=genius_external_getlofig;

   if (netlist)
      ret=FindInstances (netlist,genius,fcl);

   LATEST_GNS_RUN->RETURN=ret;

   external_getlofig=tmpfunc;

   SwitchLOFIGContext(); // => genius

   freechain(genius_ec->BLOCKS);
   genius_ec->BLOCKS=NULL;
   Free_Tree(GENIUS_TREE);
   GENIUS_TREE=NULL;
   freemodel();
   freechain(fcl);
   freechain(genius);

   DelContext();

   SwitchLOFIGContext(); // => normal

   /*close file report .gen*/
   if (GENIUS_OUTPUT!=stdout && GENIUS_OUTPUT!=stderr) fclose(GENIUS_OUTPUT);

   /*file names are used for message and idetification in GENIUS*/
   APIFreeLibrary(GENIUS_PRIORITY);
   GENIUS_PRIORITY=NULL; delht(NEW_LOINS_NAMES_HT);
   delht(LOINS_LIST_CONTROL);
   free_all_blocks_to_free();

   APIFreeExecutionContext(genius_ec);
   APIExit();

   {
     t_arg *rett;
     t_arg param, *tab[1];
     chain_list *p;
     libfunc_type func;
     char buf[128];

     ret2=NULL;
     for (bbox=netlist->LOINS; bbox!=NULL; bbox=bbox->NEXT)
       ret2=addchain(ret2, bbox);
     for (p=ret; p!=NULL; p=p->NEXT)
       ret2=addchain(ret2, p->DATA);
     
     for (p=API_ACTION_TOPLEVEL; p!=NULL; p=p->NEXT)
       {
         int j;
         param.NAME="param";
         param.POINTER=0;
         param.VALUE=mbkalloc(sizeof(long));
         *(long *)param.VALUE=(long)ret2;
         param.TYPE="long";
         func=(libfunc_type)p->DATA;
         tab[0]=&param;
         j=func(&rett, tab, 1 , buf);
         if (j!=0)
           {
             avt_errmsg(GNS_ERRMSG, "042", AVT_FATAL, buf);
             //fprintf(stderr,"Error: %s\n",buf);
             EXIT(2);
           }

         mbkfree(param.VALUE);
         mbkfree(rett->TYPE); mbkfree(rett);
       }
     freechain(ret2);
   }

   if (outname!=NULL) LATEST_GNS_RUN->FIGNAME=namealloc(outname);

   gnsApplyConnectorOrientation(LATEST_GNS_RUN);
   
   if (!(GEN_OPTIONS_PACK & GEN_NO_GNS))
     {
       char mode='y';
       if (GEN_OPTIONS_PACK & GEN_VERBOSE_GNS) mode='n';
       gnsDriveCorrespondanceTables(LATEST_GNS_RUN, mode);
     }

   // destruction des heaps sauf pour les tables de correspondances
   for (err=0; err<(signed)(sizeof(heapfreefuncs)/sizeof(*heapfreefuncs)); err++)
     heapfuncs[err](1);
   clean_gen_env();
   memcpy(&LATEST_GNS_RUN->external_ctx, &newcontext, sizeof(mbkContext));
   genius_external_setcontext(oldctx);
   RestoreContext(&GC);

   GNS_RECURSION--;
   if (GNS_RECURSION==0)
     {
       MAIN_HT_LOFIG=NULL;
       HOOK_GETLOADEDLOFIG=NULL;
     }

   return ret;
}

/* display system and user time */

void
genius_print_time(struct rusage *start, struct rusage *end, time_t rstart, time_t rend)
{
    unsigned long   temps;
    unsigned long   user, syst;
    unsigned long   userM, userS, userD;
    unsigned long   systM, systS, systD;
    unsigned long   bytes;

    temps = rend - rstart;
    user = (100 * end->ru_utime.tv_sec + (end->ru_utime.tv_usec / 10000))
        - (100 * start->ru_utime.tv_sec + (start->ru_utime.tv_usec / 10000));
    syst = (100 * end->ru_stime.tv_sec + (end->ru_stime.tv_usec / 10000))
        - (100 * start->ru_stime.tv_sec + (start->ru_stime.tv_usec / 10000));

    userM = user / 6000;
    userS = (user % 6000) / 100;
    userD = (user % 100) / 10;

    systM = syst / 6000;
    systS = (syst % 6000) / 100;
    systD = (syst % 100) / 10;

    avt_fprintf(GENIUS_OUTPUT, "¤+ %02ldm%02lds", (long) (temps / 60), (long) (temps % 60));
    avt_fprintf(GENIUS_OUTPUT, "¤+  u:%02ldm%02ld.%ld", userM, userS, userD);
    bytes = mbkprocessmemoryusage();
    avt_fprintf(GENIUS_OUTPUT, "¤+  M:%ldKb\n", bytes / 1024);
 
    fflush(GENIUS_OUTPUT);
}
char *genius_getutime(struct rusage *start, struct rusage *end)
{
    unsigned long   user;
    unsigned long   userM, userS, userD;
    static char temp[64];

    user = (100 * end->ru_utime.tv_sec + (end->ru_utime.tv_usec / 10000))
        - (100 * start->ru_utime.tv_sec + (start->ru_utime.tv_usec / 10000));

    userM = user / 6000;
    userS = (user % 6000) / 100;
    userD = (user % 100) / 10;

    sprintf(temp, "%02ldm%02ld.%ld", userM, userS, userD);
 
    return temp;
}

void
genius_chrono(struct rusage *t, time_t *rt)
{
    getrusage(RUSAGE_SELF, t);
    time(rt);
}

tree_list *Read_All(lib_entry *files_list)
{
  int done;
  lib_entry *p, *p0;
  tree_list *res=NULL,*sav=NULL;
  FILE *f;
  chain_list *old, *cl;
  char *r;

  if (!files_list) {
       avt_errmsg(GNS_ERRMSG, "043", AVT_WARNING);
       //gen_printf(0, "warning: No model file in library\n");
       return NULL;
   }
   /* read all files listed */
   for (p=files_list;p;p=p->NEXT) 
     {
       if (p->format==0)
         {
           if (strcasecmp(p->name,"none")!=0)
             {
               // VHDL
               f = APIFindFile(p);
               if (f) 
                 {
                   gen_printf(0, "Opening model file %s....\n",p->name);
                   
                   r=strrchr(p->name, '.');
                   if (r!=NULL && strcasecmp(r, ".tcl")==0)
                   {
                    int nb;
                    r=mbkalloc(1000000);
                    nb=fread(r,sizeof(char),1000000, f);
                    r[nb]='\0';
                    if (Tcl_EvalEx((Tcl_Interp *)TCL_INTERPRETER, r, -1, TCL_EVAL_GLOBAL)==TCL_ERROR)
                        avt_errmsg(GNS_ERRMSG, "165", AVT_ERROR, p->name, ((Tcl_Interp *)TCL_INTERPRETER)->errorLine, Tcl_GetStringResult((Tcl_Interp *)TCL_INTERPRETER));
                    mbkfree(r);
                   }
                   else
                   {
                   old=genius_ec->BLOCKS;
                   res=APIParseFile(f, p->name, genius_ec, p->paramset);
                   
                   for (done=0, cl=genius_ec->BLOCKS; cl!=old; cl=cl->NEXT)
                     {
                       if (APIBlockIsEntity((tree_list *)cl->DATA)) 
                         {
                           if (!done)
                             {
                               p->entity=APIEntityName((tree_list *)cl->DATA);
                               done=1;
                             }
                           else
                             {
                               p0=(lib_entry *)mbkalloc(sizeof(lib_entry));
                               memcpy(p0, p, sizeof(lib_entry));
                               p0->entity=APIEntityName((tree_list *)cl->DATA);
                               p0->paramset=dup_adt_list(p0->paramset);
                               p0->NEXT=p->NEXT;
                               p->NEXT=p0;
                               p=p0;
                             }
                         }
                     }
                   
                   if (res) 
                     {
                       if (sav) sav=PutBin_Tree(';',"",0,sav,res); /*PUT_BIN(';',sav,res);*/
                       else sav=res;
                     }   
                   }
                   fclose(f);
                 }
               else 
                 {
                  avt_errmsg(GNS_ERRMSG, "166", AVT_ERROR, p->name);
                   Inc_Error();
                 }   
             }
         }
       else
         {
           // spice FCL, order is not modified
           if (sav) sav=PutBin_Tree(';',"",0,sav,p->tree);
           else sav=p->tree;
         }
       
     } /* end of loop on files list */
   return sav;
}

ht *gns_create_template_hierarchy(lib_entry *files_list)
{
  lib_entry *p;
  ht *t_h;
  char res[1024];

  t_h=addht(128);

  for (p=files_list;p;p=p->NEXT) 
    {
      if (p->model!=NULL)
        {
          if (gen_find_template_corresp(NULL, p->paramset, p->model, res)==0)
            {
              // c'est un template
              if (gen_get_template_corresp(t_h, p->model)==NULL)
                {
                  gen_printf(1,"Adding template '%s' from model '%s'\n",res,p->model);
                  gen_add_template_corresp(t_h, res, p->model, p->paramset);
                }
            }
        }
    }
  return t_h;
}
