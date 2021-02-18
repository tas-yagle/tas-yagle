/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Generic                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_GENIUS.c                                          */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 23/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include MUT_H
#include MLO_H
#include API_H
#include "gen_model_utils.h"
#include "gen_corresp.h"
#include "gen_search_utils.h"
#include "gen_MBK_utils.h"
#include "gen_model_GENIUS.h"
#include "gen_model_global.h"


static lofig_list *FIG=NULL;                 /*current lofig in construction*/


/***************************************************************************/
/*                 change a tree of signals in a list                      */
/***************************************************************************/
static inline chain_list *SigTree2chain(tree_list *tree, chain_list *list) 
{ 
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 240);
//    fprintf(stderr,"SigTree2chain: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return SigTree2chain(tree->DATA,list);
    break;
  case ',':
    list=SigTree2chain(tree->NEXT->NEXT,list);       /*not to reverse order*/
    return SigTree2chain(tree->NEXT,list);
    break;
  case GEN_TOKEN_SIGNAL:
    return SigTree2chain(tree->NEXT,list);
    break;
  case GEN_TOKEN_IDENT: 
  case '(':
    return addchain(list,tree);
    break;  
  default:
    Error_Tree("SigTree2chain",tree);
    EXIT(2); return NULL;
  }
}


/****************************************************************************/
/*          put in lofigchain of signal the value of its LOCON              */
/****************************************************************************/
static inline void add_lofigchain(locon_list *locon, 
                                  chain_list *signals_tree)
{
   losig_list *sig;
   tree_list *tree;
   ptype_list *p,*q;
   
   
  if (!locon) return ;
  else add_lofigchain(locon->NEXT,signals_tree->NEXT);

  /*have value to copy in fields*/
  tree=signals_tree->DATA;
  sig=locon->SIG;
/*  p=getptype(sig->USER,GENSIGCHAIN);  
  if (!p) {
    sig->USER=addptype(sig->USER,GENSIGCHAIN,NULL);
    p=sig->USER;
  }  
  */
   
  /* search if already a chain list*/
  p=getptype(sig->USER,GENSIGCHAIN);  
  if (!p) {
    sig->USER=addptype(sig->USER,GENSIGCHAIN,NULL);
    p=sig->USER;
  }  
  q=getptype(locon->USER,GENCONCHAIN);  
  if (!q) {
    locon->USER=addptype(locon->USER,GENCONCHAIN,NULL);
    q=locon->USER;
  }  
  
    if (TOKEN(tree)=='(') {  /*vector*/
       tree=tree->NEXT->NEXT->DATA;     /*TOKEN=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO or TOKEN=exp */
       if (TOKEN(tree)==GEN_TOKEN_TO || TOKEN(tree)==GEN_TOKEN_DOWNTO) {/*it's an array*/
       /*copy only the array of signal and locon*/
         p->DATA=addgensigchain((gensigchain_list*)p->DATA,
                                Duplicate_Tree(tree),
                                locon,
                                Duplicate_Tree(tree));
         q->DATA=addgenconchain((genconchain_list*)q->DATA,
                                Duplicate_Tree(tree),
                                sig,
                                Duplicate_Tree(tree));
       }                   
       else { /* one bit of vector signal */                 
         int lineno=LINE(tree);
         char *file=FILE_NAME(tree);
         p->DATA=addgensigchain(p->DATA,Duplicate_Tree(tree),locon,
                                 PUT_ATO(GEN_TOKEN_DIGIT,-1));
         q->DATA=addgenconchain(q->DATA,PUT_ATO(GEN_TOKEN_DIGIT,-1),sig,
                                 Duplicate_Tree(tree));
       } 
    }
    else {/*it 's not a vector*/
         int lineno=LINE(tree);
         char *file=FILE_NAME(tree);
         p->DATA=addgensigchain(p->DATA,PUT_ATO(GEN_TOKEN_DIGIT,-1),locon,
                                 PUT_ATO(GEN_TOKEN_DIGIT,-1));
         q->DATA=addgenconchain(q->DATA,PUT_ATO(GEN_TOKEN_DIGIT,-1),sig,
                                 PUT_ATO(GEN_TOKEN_DIGIT,-1));
    }
}


/****************************************************************************/
/*   build and return the list of losig in tree put on top of sigchain      */
/****************************************************************************/
extern chain_list *Get_Losig_Generic(locon_list *locon, chain_list *signal, 
                              chain_list *sigchain)
{
  losig_list *sig;
  chain_list *namechain;
  tree_list *tree;
  char *signame;
 
  if (locon && !signal) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 251);
//      fprintf(stderr,"Get_Losig_Generic: no signal for locon %s\n",locon->NAME);
      EXIT(1);
  }    
  if (!locon && signal) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 252);
//      fprintf(stderr,"Get_Losig_Generic: no locon for signal\n");
      EXIT(1);
  }    
  
  if (!locon || !signal) return sigchain;
  else {
/*if bit vector from FCL(alliance normalized) then group locon in a vector for GENIUS*/
     if (vectorradical(locon->NAME)!=locon->NAME) {
         locon_list* del;
         for (del=locon->NEXT; del && vectorradical(del->NAME)==vectorradical(locon->NAME); del=del->NEXT) ;     
     sigchain=Get_Losig_Generic(del,signal->NEXT,sigchain);
     }
     else sigchain=Get_Losig_Generic(locon->NEXT,signal->NEXT,sigchain);

  }
  
  tree=(tree_list*)signal->DATA;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 241);
//      fprintf(stderr,"Get_Losig_Generic: NULL pointer\n");
      EXIT(1);
  }

    /*search signal*/
    signame=getname(tree);      
    for (sig=FIG->LOSIG; sig; sig=sig->NEXT) {
      for (namechain=sig->NAMECHAIN; namechain; namechain=namechain->NEXT) {
              if (((char*)namechain->DATA)==signame) {break;}
      }   
      if (namechain) break;   
    }
    if (!sig) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 242);
//      fprintf(stderr,"Get_Losig_Generic: NULL pointer\n");
      EXIT(1);
    }  
    
    return addchain(sigchain,sig);
}


/****************************************************************************/
/*    put away locon without signal (as matter of fact, the aim is to get   */
/* away the locon bit vector and build a real vector for GENIUS             */
/****************************************************************************/
static inline void erase_bit_vector(loins_list* loins)
{
   locon_list *pred,*del,*locon;

   pred=NULL;
   
   for (locon=loins->LOCON; locon; ) {

     if (!locon->SIG) {/*not select in function Get_Losig_Generic() */
         del=locon;
         locon=locon->NEXT;
         mbkfree(del);
     }
     else {
         locon->NAME=vectorradical(locon->NAME);  /*erase bit field*/
         if (pred) pred->NEXT=locon;
         else loins->LOCON=locon;
         pred=locon;
         locon=locon->NEXT;
     }    
     
   }

}


/****************************************************************************/
/*     return the abl generic function of all created instances in fig      */
/****************************************************************************/
static inline tree_list *__Get_Loins_Generic(tree_list *tree, tree_list *archi)
{
  lofig_list *model;                                
  chain_list *sigchain,*signals;
  char *modelname,*insname;
  int lineno;                                   /*hidden in macro PUT_BIN */
  char* file;
  tree_list *temp0, *temp1;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 243);
//    fprintf(stderr,"__Get_Loins_Generic: NULL pointer\n");
    EXIT(1);
  }
  
  lineno=LINE(tree);
  file=FILE_NAME(tree);

  switch (TOKEN(tree)) {
  /*node*/
  case GEN_TOKEN_NODE:
     return __Get_Loins_Generic(tree->DATA, archi);
     break;
  /*instance list connexions*/
  case ';':
    temp0=__Get_Loins_Generic(tree->NEXT,archi);
    temp1=__Get_Loins_Generic(tree->NEXT->NEXT,archi);
//    return PUT_BIN(';',__Get_Loins_Generic(tree->NEXT,archi),__Get_Loins_Generic(tree->NEXT->NEXT,archi));
    return PUT_BIN(';',temp0,temp1);
    break;
  case GEN_TOKEN_EXCLUDE:
    return PUT_UNI(GEN_TOKEN_EXCLUDE,__Get_Loins_Generic(tree->NEXT,archi));
    break;
  case GEN_TOKEN_FOR:
    return PUT_QUA(GEN_TOKEN_FOR,Duplicate_Tree(tree->NEXT),Duplicate_Tree(tree->NEXT->NEXT),Duplicate_Tree(tree->NEXT->NEXT->NEXT),__Get_Loins_Generic(tree->NEXT->NEXT->NEXT->NEXT,archi));
    break;
  case GEN_TOKEN_IF:
    return PUT_BIN(GEN_TOKEN_IF,Duplicate_Tree(tree->NEXT),__Get_Loins_Generic(tree->NEXT->NEXT,archi));
    break;
  case GEN_TOKEN_MAP:
    {
      misc_stuffs *ms;
      ptype_list *p;
      long flags;
      locon_list *lc, *lcm;

      insname=getname(tree->NEXT);
      modelname=getname_of(tree->NEXT);
      model=getlofig(modelname,'P');       /* 'P' (partial) for only interface */
      if (!model) {
    avt_errmsg(GNS_ERRMSG, "161", AVT_FATAL,modelname);
//        fprintf(stderr,"__Get_Loins: no model %s found\n",modelname);
        EXIT(1);
      }
      signals=SigTree2chain(tree->NEXT->NEXT,NULL);
      sigchain=Get_Losig_Generic(model->LOCON,signals,NULL);
      FIG->LOINS=gen_addloins(FIG,insname,model,sigchain,signals);
      
      // flags in loins
      ms=(misc_stuffs *)archi->DATA->NEXT->NEXT->NEXT->NEXT->DATA->DATA;

      for (lc=FIG->LOINS->LOCON, lcm=model->LOCON; lc!=NULL && lcm!=NULL; lc=lc->NEXT, lcm=lcm->NEXT)
        {
          if (gns_isunusedsig(lcm->SIG))
            if (!gns_isunusedsig(lc->SIG))
              {
                lc->SIG->USER=addptype(lc->SIG->USER, GEN_UNUSED_FLAG_PTYPE, NULL);
//                printf("i %s unu %s", insname, lc->NAME);
              }
          if (ms!=NULL) {
            for (p=ms->STOP_POWER; p!=NULL && p->DATA!=getsigname(lc->SIG); p=p->NEXT) ;
                if (p!=NULL) {
              if (getptype(lc->SIG->USER, GEN_STOP_POWER_FLAG_PTYPE)==NULL)
                  lc->SIG->USER=addptype(lc->SIG->USER, GEN_STOP_POWER_FLAG_PTYPE, NULL);
              p->TYPE=-p->TYPE;
            }
          }
        }


      flags=0;
      if (ms!=NULL)
        {
          for (p=ms->EXCLUDES; p!=NULL && p->DATA!=insname; p=p->NEXT) ;
          if (p!=NULL) { flags|=LOINS_IS_EXCLUDE; p->TYPE=-p->TYPE; }
          for (p=ms->WITHOUTS; p!=NULL && p->DATA!=insname; p=p->NEXT) ;
          if (p!=NULL) { flags|=LOINS_IS_WITHOUT; p->TYPE=-p->TYPE; }
          for (p=ms->EXCLUDES_AT_END; p!=NULL && p->DATA!=insname; p=p->NEXT) ;
          if (p!=NULL) { flags|=LOINS_IS_EXCLUDE_AT_END; p->TYPE=-p->TYPE; }
        }
      FIG->LOINS->USER=addptype(FIG->LOINS->USER, GENIUS_LOINS_FLAGS_PTYPE, (void *)flags);
//      printf("%s %s %ld\n",FIG->NAME, insname, flags);
      FIG->LOINS->USER=addptype(FIG->LOINS->USER, GEN_GENERIC_MAP_PTYPE, ((tree_list *)tree->NEXT->NEXT->NEXT->DATA)->DATA); // (<model_var>,<circuit_expr>)+ single chain list
      freechain(sigchain);
      freechain(signals);
      return PUT_ATO(GEN_TOKEN_LOINS,FIG->LOINS);
    }
    break;
  default:
    Error_Tree("__Get_Loins_Generic",tree);
    EXIT(1); return NULL;
  }
}


void verif_instance_flags(ptype_list *p, char *file, char *type)
{
  int failed=0;
  for (; p!=NULL; p=p->NEXT)
    {
      if (p->TYPE>0)
        {
          failed=1;
      avt_errmsg(GNS_ERRMSG, "162", AVT_ERROR, file, (int)p->TYPE, type, (char *)p->DATA);
//          fprintf(stderr,"%s:%d: %s instance '%s' can't be found\n", file, (int)p->TYPE, type, (char *)p->DATA);
        }
    }
  if (failed) EXIT(1);
}

void verif_instance_flags_chainlist(chain_list *p, char *file, char *type)
{
  int failed=0;
  for (; p!=NULL; p=p->NEXT)
    {
      if (p->DATA!=NULL)
        {
          failed=1;
      avt_errmsg(GNS_ERRMSG, "162", AVT_ERROR, file, 0, type, (char *)p->DATA);
//          fprintf(stderr,"%s:%d: %s instance '%s' can't be found\n", file, (int)p->TYPE, type, (char *)p->DATA);
        }
    }
  if (failed) EXIT(1);
}


/****************************************************************************/
/* return non NULL if contained loins, erase the other part of tree from ';'*/
/****************************************************************************/
static inline tree_list* clean_tree(tree_list *tree, loins_list* loins)
{
   tree_list* ret;
  
  if (!tree || !loins) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 244);
//    fprintf(stderr,"clean_tree: NULL pointer\n");
    EXIT(1);
  }
  
  switch (TOKEN(tree)) {
  /*node*/
  case GEN_TOKEN_NODE:
    ret= clean_tree(tree->DATA,loins);
    if (ret) {
      tree->DATA=ret;
      return tree;
    }
    else return NULL;
    break;
  /*instance list connexions*/
  case ';':
    ret=clean_tree(tree->NEXT,loins);
    if (ret) {
      ret=Duplicate_Tree(tree->NEXT->DATA);
      Free_Tree(tree);
      return ret;
    }
    ret=clean_tree(tree->NEXT->NEXT,loins);
    if (ret) {
      ret=Duplicate_Tree(tree->NEXT->NEXT->DATA);
      Free_Tree(tree);
      return ret;
    }
    return NULL;
    break;
  case GEN_TOKEN_EXCLUDE:
    ret=clean_tree(tree->NEXT,loins);
    if (ret) {
      tree->NEXT=ret;
      return tree;
    }  
    else return NULL;
    break;
  case GEN_TOKEN_FOR:
    ret=clean_tree(tree->NEXT->NEXT->NEXT->NEXT,loins);
    if (ret) {
      tree->NEXT->NEXT->NEXT->NEXT=ret;
      return tree;
    }
    else return NULL;
    break;
  case GEN_TOKEN_IF:
    ret=clean_tree(tree->NEXT->NEXT,loins);
    if (ret) {
      tree->NEXT->NEXT=ret;
      return tree;
    }
    else return NULL;
    break;
  case GEN_TOKEN_LOINS:
    if ((loins_list*)tree->DATA==loins) return tree;
    else return NULL;
    break;
  default:
    Error_Tree("clean_tree",tree);
    EXIT(1); return NULL;
  }
}

/****************************************************************************/
/*    add reference in loins USER field of a pointer to its genericity      */
/****************************************************************************/
static inline void Add_Reference_Generic(tree_list *generic_tree, tree_list *begin_tree)
{
  loins_list *loins;
  
  if (!generic_tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 245);
//    fprintf(stderr,"Add_Reference_Generic: NULL pointer\n");
    EXIT(1);
  }
  
  switch (TOKEN(generic_tree)) {
  /*node*/
  case GEN_TOKEN_NODE:
     Add_Reference_Generic(generic_tree->DATA,begin_tree);
     break;
  /*instance list connexions*/
  case ';':
    Add_Reference_Generic(generic_tree->NEXT,begin_tree);
    Add_Reference_Generic(generic_tree->NEXT->NEXT,begin_tree);
    break;
  case GEN_TOKEN_EXCLUDE:
    if (!begin_tree) begin_tree=generic_tree;
    Add_Reference_Generic(generic_tree->NEXT,begin_tree);
    break;
  case GEN_TOKEN_FOR:
    if (!begin_tree) begin_tree=generic_tree;
    Add_Reference_Generic(generic_tree->NEXT->NEXT->NEXT->NEXT,begin_tree);
    break;
  case GEN_TOKEN_IF:
    if (!begin_tree) begin_tree=generic_tree;
    Add_Reference_Generic(generic_tree->NEXT->NEXT,begin_tree);
    break;
  case GEN_TOKEN_LOINS:
    if (!begin_tree) begin_tree=generic_tree; /*if not complex put simple ref*/
    loins=(loins_list*)generic_tree->DATA;
    loins->USER=addptype(loins->USER,GENERIC_PTYPE,Duplicate_Tree(begin_tree));
    clean_tree(loins->USER->DATA,loins);     /*erase foreign loins and ';'*/
    break;
  default:
    Error_Tree("Add_Reference_Generic",generic_tree);
    EXIT(1);
  }
}


/****************************************************************************/
/*     return the abl generic function of all created instances in fig      */
/****************************************************************************/
extern tree_list *Get_Loins_Generic(tree_list *tree, tree_list *archi)
{
  tree_list *tree_generic;
  
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 246);
//    fprintf(stderr,"Get_Loins_Generic: NULL pointer\n");
    EXIT(1);
  }
  tree_generic=__Get_Loins_Generic(tree, archi);
  Add_Reference_Generic(tree_generic,NULL);
  return tree_generic;
}


/****************************************************************************/
/*   build all the locons of and its signals from tree pointing on port     */
/****************************************************************************/
static tree_list *sigrangetree;
static chain_list *lst;

extern void Get_Signals_Generic(tree_list *tree)
{
  chain_list *namechain;
  int index;
  chain_list *cl;
  char *signame;
  losig_list *sig;

  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 247);
//    fprintf(stderr,"Get_Signals_Generic: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*node*/
  case GEN_TOKEN_NODE:
    Get_Signals_Generic(tree->DATA);
    break;
  case ';':
    Get_Signals_Generic(tree->NEXT->NEXT);
    Get_Signals_Generic(tree->NEXT);
    break;
  case GEN_TOKEN_COMPONENT:
    /*nothing to do*/
    break;
  case GEN_TOKEN_INOUT:
    Get_Signals_Generic(tree->NEXT);
    break;
  case GEN_TOKEN_SIGNAL:
    lst=NULL;
    Get_Signals_Generic(tree->NEXT);
    for (cl=lst; cl!=NULL; cl=cl->NEXT)
      {
        sig=(losig_list *)cl->DATA;
        //printf("%s : range %p (%d)\n",sig->NAMECHAIN->DATA, sigrangetree,sigrangetree==NULL?0:TOKEN(sigrangetree));
        sig->USER=addptype(sig->USER, GEN_REAL_RANGE_PTYPE, Duplicate_Tree(sigrangetree));
      }
    freechain(lst);
    break;
  case GEN_TOKEN_IDENT: case '(':
    signame=getname(tree);
    /* search index number */
    index=INDEX_START;
    for (sig=FIG->LOSIG; sig; sig=sig->NEXT) index++;
    namechain=addchain(NULL,signame);
    FIG->LOSIG=addlosig(FIG,index,namechain,INTERNAL);

    lst=addchain(lst, FIG->LOSIG);

    if (TOKEN(tree)=='(') {
      sigrangetree=tree->NEXT->NEXT->DATA;      /* TOKEN=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO */
    }
    else {
      sigrangetree=NULL;
    }

//    FIG->LOSIG->USER=addptype(FIG->LOSIG->USER, GEN_REAL_RANGE_PTYPE, tree);

    break;
  default:
    Error_Tree("Get_Signals_Generic",tree);
    EXIT(1);
  }
}


/****************************************************************************/
/*         build one locon of fig (and also its external signal)            */
/****************************************************************************/
extern void Get_Locon_Generic(int dir, tree_list *tree)
{
  char *conname;
  chain_list *namechain;
  losig_list *sig;
  long index;

  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 248);
//    fprintf(stderr,"Get_Locon_Generic: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*node */
  case GEN_TOKEN_NODE:
     Get_Locon_Generic(dir,tree->DATA);
     break;
  case '(': case GEN_TOKEN_IDENT:
    conname=getname(tree);
    /* search new index */
    index=INDEX_START;    
    for (sig=FIG->LOSIG; sig; sig=sig->NEXT) index++;
    namechain=addchain(NULL,conname);
    /*WARN:no more capa*/
    FIG->LOSIG=addlosig(FIG,index,namechain,EXTERNAL);

    /*default addlocon set EXTERNAL*/
    FIG->LOCON=addlocon(FIG,conname,FIG->LOSIG,dir);
    if (TOKEN(tree)=='(') {
       tree=tree->NEXT->NEXT->DATA;
       FIG->LOSIG->USER=addptype(FIG->LOSIG->USER,GENSIGCHAIN,NULL);
       FIG->LOCON->USER=addptype(FIG->LOCON->USER,GENCONCHAIN,NULL);
       FIG->LOSIG->USER->DATA=addgensigchain(FIG->LOSIG->USER->DATA,
              Duplicate_Tree(tree), FIG->LOCON,
              Duplicate_Tree(tree)); /*copy only array*/
       FIG->LOCON->USER->DATA=addgenconchain(FIG->LOCON->USER->DATA,
              Duplicate_Tree(tree), FIG->LOSIG,
              Duplicate_Tree(tree)); /*copy only array*/


       FIG->LOSIG->USER=addptype(FIG->LOSIG->USER, GEN_REAL_RANGE_PTYPE, Duplicate_Tree(tree));
    } 
    else {/*it 's not a vector*/
       int lineno=LINE(tree);
       char* file=FILE_NAME(tree);
       FIG->LOSIG->USER=addptype(FIG->LOSIG->USER,GENSIGCHAIN,NULL);
       FIG->LOCON->USER=addptype(FIG->LOCON->USER,GENCONCHAIN,NULL);
       FIG->LOSIG->USER->DATA=addgensigchain(FIG->LOSIG->USER->DATA,
              PUT_ATO(GEN_TOKEN_DIGIT,-1), FIG->LOCON, PUT_ATO(GEN_TOKEN_DIGIT,-1)); 
       FIG->LOCON->USER->DATA=addgenconchain(FIG->LOCON->USER->DATA,
              PUT_ATO(GEN_TOKEN_DIGIT,-1), FIG->LOSIG, PUT_ATO(GEN_TOKEN_DIGIT,-1)); 

       FIG->LOSIG->USER=addptype(FIG->LOSIG->USER, GEN_REAL_RANGE_PTYPE, NULL);
    }
    break;
  default:
    Error_Tree("Get_Locon_Generic",tree);
    EXIT(1);
  }
}


/****************************************************************************/
/*   build all the locons of and its signals from tree pointing on port     */
/****************************************************************************/
extern void Get_Port_Generic(tree_list *tree)
{
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 249);
//    fprintf(stderr,"Get_Port_Generic: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*node*/
  case GEN_TOKEN_NODE:
    Get_Port_Generic(tree->DATA);
    break;
  case GEN_TOKEN_PORT:
    Get_Port_Generic(tree->NEXT);
    break;
  case ',':                          /*it's a list of different connectors*/
    Get_Port_Generic(tree->NEXT->NEXT); /*to avoid reversion of addchain*/
    Get_Port_Generic(tree->NEXT);
    break;
  case GEN_TOKEN_SIGNAL:
    Get_Port_Generic(tree->NEXT);
    break;
  case GEN_TOKEN_IN:
    Get_Locon_Generic(IN,tree->NEXT);
    break;
  case GEN_TOKEN_OUT:
    Get_Locon_Generic(OUT,tree->NEXT);
    break;
  case GEN_TOKEN_INOUT:
    Get_Locon_Generic(INOUT,tree->NEXT);
    break;
  case GEN_TOKEN_TRISTATE:
    Get_Locon_Generic(TRISTATE,tree->NEXT);
    break;
  case GEN_TOKEN_TRANSCV:
    Get_Locon_Generic(TRANSCV,tree->NEXT);
    break;
  default:
    Error_Tree("Get_Port_Generic",tree);
    EXIT(1);
  }
}


/****************************************************************************/
/*           return list of variables contained in tree                     */
/*  result is put on top of var                                             */
/****************************************************************************/
extern chain_list* Get_Generic(tree_list* tree, chain_list *var)
{
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 250);
//    fprintf(stderr,"Get_Generic: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  case GEN_TOKEN_NOP:
   return var;
   break;
  /*node*/
  case GEN_TOKEN_NODE:
    return Get_Generic(tree->DATA,var);
    break;
  case GEN_TOKEN_GENERIC:
    return Get_Generic(tree->NEXT,var);
    break;
  case ',':                          
    var=Get_Generic(tree->NEXT->NEXT,var); /*to avoid reversion of addchain*/
    return Get_Generic(tree->NEXT,var);
    break;
  case GEN_TOKEN_INTEGER: case GEN_TOKEN_VARIABLE:
    return Get_Generic(tree->NEXT,var);
    break;
  case GEN_TOKEN_IDENT:
    return addchain(var,getname(tree));
    break;
  default:
    Error_Tree("Get_Generic",tree);
    EXIT(1); return NULL;
  }
}


/****************************************************************************/
/*                 put environment to good values                           */
/****************************************************************************/
extern void Env_Generic(lofig_list *lofig)
{
   FIG=lofig;
}


