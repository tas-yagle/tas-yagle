/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_FCL.c                                             */
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
#include AVT_H
#include "gen_execute_VHDL.h"
#include "gen_model_utils.h"
#include "gen_model_FCL.h"



static lofig_list *FIG=NULL;                 /*current lofig in construction*/
static ptype_list *FOR_ENV=NULL;    /* local FOR variables with values */



/***************************************************************************/
/*                 change a tree of signals in a list                      */
/***************************************************************************/
static inline chain_list *SigTree2chain(tree_list *tree, chain_list *list) 
{ 
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 233);
    //fprintf(stderr,"SigTree2chain: NULL pointer\n");
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
  case GEN_TOKEN_IDENT: case '(':
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
static inline void add_lofigchain(locon_list *locon, chain_list *sigchain)
{
   losig_list *sig;
   ptype_list *p;

   
  if ((locon && !sigchain) || (!locon && sigchain)) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 234);
//      fprintf(stderr,"add_lofigchain: parameters discrepancy\n");
      EXIT(1);
  }    
  
  if (!locon || !sigchain) return ;
  else add_lofigchain(locon->NEXT,sigchain->NEXT);

   sig=(losig_list*) sigchain->DATA;
   
   /* lofigchain*/
   p=getptype(sig->USER,LOFIGCHAIN);  
   if (!p) {
        sig->USER=addptype(sig->USER,LOFIGCHAIN,NULL);
        p=sig->USER;
   }   
   p->DATA=addchain(p->DATA,locon);
}


/****************************************************************************/
/*   build and return the list of losig in tree put on top of sigchain      */
/* build also the generic of locon list and losig list returned             */
/* signal from tree                                                         */
/* signals are sorted in the same order than in locon list                  */
/****************************************************************************/
extern chain_list *Get_Losig_Alliance(locon_list *locon, chain_list *signal, 
                              chain_list *sigchain)
{
  losig_list *sig;
  chain_list *namechain;
  tree_list *tree;
  char *signame;
  char memo_char;
  long index;
  
  if ((locon && !signal) || (!locon && signal)) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 235);
//      fprintf(stderr,"Get_Losig_Alliance: parameters discrepancy\n");
      EXIT(1);
  }    
  
  if (!locon || !signal) return sigchain;
  else sigchain=Get_Losig_Alliance(locon->NEXT,signal->NEXT,sigchain);
  
  tree=(tree_list*)signal->DATA;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 236);
//      fprintf(stderr,"Get_Losig_Alliance: NULL pointer\n");
      EXIT(1);
  }

    /*search signal*/
    signame=getname(tree);    
    if (TOKEN(tree)=='(') { /*vector*/
      if (TOKEN(tree->NEXT->NEXT->DATA)==GEN_TOKEN_TO) { /*array*/
         char *name;
         int lim=Eval_Exp_VHDL(
                  (tree->NEXT->NEXT->DATA)->NEXT/*low bound*/,FOR_ENV);
         long i;
         for (i=Eval_Exp_VHDL(
                  (tree->NEXT->NEXT->DATA)->NEXT->NEXT/*high bound*/,FOR_ENV);
                  i>=lim; i--) {
            memo_char=SEPAR;       /* MBK variable it is for concatenation*/
            SEPAR=' ';/*nameindex() must be compatible with vectorradical()  */ 
            name=nameindex(signame,i);
            SEPAR=memo_char;        /* put last value in MBK environment */
            index=INDEX_START;
            /*search sig*/
            for (sig=FIG->LOSIG; sig; sig=sig->NEXT) {
              for (namechain=sig->NAMECHAIN; 
                                 namechain; 
                  namechain=namechain->NEXT) {
   	           if (((char*)namechain->DATA)==name) {break;}
              }   
              if (namechain) break;
              index++;
            }
            if (!sig) {
               chain_list *namechain;
               namechain=addchain(NULL,name);
               FIG->LOSIG=addlosig(FIG,index,namechain,INTERNAL);
               sig=FIG->LOSIG;
            }
            sigchain=addchain(sigchain,sig);
         }
      }
      else if (TOKEN(tree->NEXT->NEXT->DATA)==GEN_TOKEN_DOWNTO) { /*array*/
         char *name;
         int lim=Eval_Exp_VHDL(
                  (tree->NEXT->NEXT->DATA)->NEXT/*high bound*/,FOR_ENV);
         long i;
         for (i=Eval_Exp_VHDL(
                  (tree->NEXT->NEXT->DATA)->NEXT->NEXT/*low bound*/,FOR_ENV);
                  i<=lim; i++) {
            memo_char=SEPAR;       /* MBK variable it is for concatenation*/
            SEPAR=' ';/*nameindex() must be compatible with vectorradical()  */ 
            name=nameindex(signame,i);
            SEPAR=memo_char;        /* put last value in MBK environment */
            index=INDEX_START;
            /*search sig*/
            for (sig=FIG->LOSIG; sig; sig=sig->NEXT) {
              for (namechain=sig->NAMECHAIN; 
                                 namechain; 
                  namechain=namechain->NEXT) {
   	           if (((char*)namechain->DATA)==name) {break;}
              }   
              if (namechain) break;
              index++;
            }
            if (!sig) {
               chain_list *namechain;
               namechain=addchain(NULL,name);
               FIG->LOSIG=addlosig(FIG,index,namechain,INTERNAL);
               sig=FIG->LOSIG;
            }
            sigchain=addchain(sigchain,sig);
         }
      }

      else { /*bit vector*/
         long bitnum=Eval_Exp_VHDL(tree->NEXT->NEXT->DATA,FOR_ENV);
         char *name;
         memo_char=SEPAR;             /* MBK variable it is for concatenation*/
         SEPAR=' '; /* nameindex() must be compatible with vectorradical()  */ 
         name=nameindex(signame,bitnum);
         SEPAR=memo_char;              /* put last value in MBK environment */
         index=INDEX_START;
         /*search sig*/
         for (sig=FIG->LOSIG; sig; sig=sig->NEXT) {
          for (namechain=sig->NAMECHAIN; namechain; namechain=namechain->NEXT) {
           if (((char*)namechain->DATA)==name) {break;}
          }   
          if (namechain) break;   
          index++;
         }
         if (!sig) {
               chain_list *namechain;
               namechain=addchain(NULL,name);
               FIG->LOSIG=addlosig(FIG,index,namechain,INTERNAL);
               sig=FIG->LOSIG;
         }
         sigchain=addchain(sigchain,sig);
      }
    }

    else { /* a bit */
         index=INDEX_START;
         /*search sig*/
         for (sig=FIG->LOSIG; sig; sig=sig->NEXT) {
          for (namechain=sig->NAMECHAIN; namechain; namechain=namechain->NEXT) {
	        if (((char*)namechain->DATA)==signame) {break;}
          }   
          if (namechain) break;   
          index++;
         }
         if (!sig) {
               chain_list *namechain;
               namechain=addchain(NULL,signame);
               FIG->LOSIG=addlosig(FIG,index,namechain,INTERNAL);
               sig=FIG->LOSIG;
         }
         sigchain=addchain(sigchain,sig);
    }

    return sigchain;
}



/****************************************************************************/
/*     return the abl generic function of all created instances in fig      */
/****************************************************************************/
extern tree_list *Get_Loins_Alliance(tree_list *tree)
{
  tree_list *ret,*ret2;
  ptype_list *q;
  lofig_list *ins;                                
  chain_list *sigchain,*signals;
  char *modelname,*insname;
  int lineno=0,limit,i;                           /*hidden in macro PUT_BIN */
  char memo_char;
  char* file;

  if (!tree) {
    return NULL;
/*    fprintf(stderr,"Get_Loins_Alliance: NULL pointer\n");
    EXIT(1);*/
  }
  
  switch (TOKEN(tree)) {
  /*node*/
  case GEN_TOKEN_NODE:
     return Get_Loins_Alliance(tree->DATA);
     break;
  /*instance list connexions*/
  case ';':
    ret=Get_Loins_Alliance(tree->NEXT);
    ret2=Get_Loins_Alliance(tree->NEXT->NEXT);
    lineno=LINE(tree);
    file=FILE_NAME(tree);
    if (ret) {
      if (ret2) return PUT_BIN(';',ret,ret2);
      else return ret;
    }
    else return ret2;
    break;
  case GEN_TOKEN_FOR:
    limit=Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,FOR_ENV);
    ret=NULL;
    lineno=LINE(tree);
    file=FILE_NAME(tree);
    FOR_ENV=addptype(FOR_ENV,0,tree->NEXT);
    for (i=Eval_Exp_VHDL(tree->NEXT->NEXT,FOR_ENV); i<=limit; i++) {
      FOR_ENV->TYPE=i;
      ret2=Get_Loins_Alliance(tree->NEXT->NEXT->NEXT->NEXT);
      if (ret) {
         if (ret2) ret=PUT_BIN(';',ret,ret2);
      }
      else ret=ret2;
    }
    q=FOR_ENV;
    FOR_ENV=FOR_ENV->NEXT;
    q->NEXT=NULL;
    freeptype (q);
    return ret;
    break;
  case GEN_TOKEN_IF:
    i=Eval_Exp_VHDL(tree->NEXT,FOR_ENV);
    if (i) return Get_Loins_Alliance(tree->NEXT->NEXT);
    else return NULL;
    break;
  case GEN_TOKEN_MAP:
    insname=getname(tree->NEXT);
    /*if we are in a loop put an index */
    memo_char=SEPAR;               /* MBK variable it is for concatenation*/
    SEPAR='_';    /* compatible FCL  */ 
    if (FOR_ENV) insname=nameindex(insname,FOR_ENV->TYPE);
    SEPAR=memo_char;              /* put last value in MBK environment */
    modelname=getname_of(tree->NEXT);
    ins=getlofig(modelname,'P');       /* 'P' (partial) for only interface */
    if (!ins) {
    avt_errmsg(GNS_ERRMSG, "161", AVT_FATAL, modelname);
//       fprintf(stderr,"Get_Loins: no model %s found\n",modelname);
       EXIT(1);
    }
    signals=SigTree2chain(tree->NEXT->NEXT,NULL);
    sigchain=Get_Losig_Alliance(ins->LOCON,signals,NULL);
    FIG->LOINS=addloins(FIG,insname,ins,sigchain);
    add_lofigchain(FIG->LOINS->LOCON,sigchain);
    freechain(sigchain);
    freechain(signals);
    lineno=LINE(tree);
    file=FILE_NAME(tree);
    return PUT_ATO(GEN_TOKEN_LOINS,FIG->LOINS);
    break;
  default:
    Error_Tree("Get_Loins_Alliance",tree);
    EXIT(1); return NULL;
  }
}


/****************************************************************************/
/*   build all the locons of and its signals from tree pointing on port     */
/****************************************************************************/
extern void Get_Signals_Alliance(tree_list *tree)
{
  chain_list *namechain;
  int index;
  char *signame;
  losig_list *sig;
  char memo_char;
  
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 237);
//    fprintf(stderr,"Get_Signals_Alliance: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*node*/
  case GEN_TOKEN_NODE:
    Get_Signals_Alliance(tree->DATA);
    break;
  case ';':
    Get_Signals_Alliance(tree->NEXT->NEXT);
    Get_Signals_Alliance(tree->NEXT);
    break;
  case GEN_TOKEN_COMPONENT:
    /*nothing to do*/
    break;
  case GEN_TOKEN_INOUT: case GEN_TOKEN_SIGNAL:
    Get_Signals_Alliance(tree->NEXT);
    break;
  case GEN_TOKEN_IDENT: case '(':
    signame=getname(tree);
    /* search index number */
    index=INDEX_START;
    for (sig=FIG->LOSIG; sig; sig=sig->NEXT) index++;
    if (TOKEN(tree)=='(') {  /* vector */
     if (TOKEN(tree->NEXT->NEXT->DATA)==GEN_TOKEN_TO) {
      long i;      
      int lim=Eval_Exp_VHDL(tree->NEXT->NEXT->DATA->NEXT/*low bound*/,FOR_ENV);
      for(i=
      Eval_Exp_VHDL(tree->NEXT->NEXT->DATA->NEXT->NEXT/*high bound*/,FOR_ENV);
      i>=lim; i--) {
        char *name;
        memo_char=SEPAR;               /* MBK variable it is for concatenation*/
        SEPAR=' ';    /* nameindex() must be compatible with vectorradical()  */ 
        name=nameindex(signame,i);
        SEPAR=memo_char;              /* put last value in MBK environment */
        namechain=addchain(NULL,name);
        FIG->LOSIG=addlosig(FIG,index,namechain,INTERNAL);
        index++;
      }
     } 
     else if (TOKEN(tree->NEXT->NEXT->DATA)==GEN_TOKEN_DOWNTO) {
      long i;      
      int lim=Eval_Exp_VHDL(tree->NEXT->NEXT->DATA->NEXT/*high bound*/,FOR_ENV);
      for(i=
      Eval_Exp_VHDL(tree->NEXT->NEXT->DATA->NEXT->NEXT/*low bound*/,FOR_ENV);
      i<=lim; i++) {
        char *name;
        memo_char=SEPAR;               /* MBK variable it is for concatenation*/
        SEPAR=' ';    /* nameindex() must be compatible with vectorradical()  */ 
        name=nameindex(signame,i);
        SEPAR=memo_char;              /* put last value in MBK environment */
        namechain=addchain(NULL,name);
        FIG->LOSIG=addlosig(FIG,index,namechain,INTERNAL);
        index++;
      }
     } 
     else {
        Error_Tree("Get_Signals_Alliance",tree->NEXT->NEXT->DATA);
        EXIT(2);
     }   
    }
    else { /* one bit */
       namechain=addchain(NULL,signame);
       FIG->LOSIG=addlosig(FIG,index,namechain,INTERNAL);
    }
    break;
  default:
    Error_Tree("Get_Signals_Alliance",tree);
    EXIT(1);
  }
}


/****************************************************************************/
/*         build one locon of fig (and also its external signal)            */
/****************************************************************************/
extern void Get_Locon_Alliance(int dir, tree_list *tree)
{
  char *conname;
  chain_list *namechain;
  losig_list *sig;
  long index;
  char memo_char;

  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 238);
 //   fprintf(stderr,"Get_Locon_Alliance: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*node */
  case GEN_TOKEN_NODE:
     Get_Locon_Alliance(dir,tree->DATA);
     break;
  case '(': case GEN_TOKEN_IDENT:
    conname=getname(tree);
    /* search new index */
    index=INDEX_START;    
    for (sig=FIG->LOSIG; sig; sig=sig->NEXT) index++;
    if (TOKEN(tree)=='(') {/*order of bit vector is very important for GENIUS*/
     if (TOKEN(tree->NEXT->NEXT->DATA)==GEN_TOKEN_TO) {
      long i;              /*help for verif in Mark_Loins() */
      int lim;
      tree=tree->NEXT->NEXT->DATA;   /* TOKEN(tree)=GEN_TOKEN_TO */
      lim=Eval_Exp_VHDL(tree->NEXT/*low bound*/,FOR_ENV);
      memo_char=SEPAR;             /* MBK variable it is for concatenation*/
      SEPAR=' ';  /* nameindex() must be compatible with vectorradical()  */ 

      for (i=Eval_Exp_VHDL(tree->NEXT->NEXT/*high bound*/,FOR_ENV); i>=lim; i--) {
         char *name;
         name=nameindex(conname,i);
         namechain=addchain(NULL,name);
         FIG->LOSIG=addlosig(FIG,index,namechain,EXTERNAL);/*WARN:no more capa*/
         /*default addlocon set EXTERNAL*/
         FIG->LOCON=addlocon(FIG,name,FIG->LOSIG,dir); 
         /*used by GENIUS*/
         FIG->LOCON->USER=addptype(FIG->LOCON->USER,GENCONCHAIN,NULL);
         FIG->LOCON->USER->DATA=addgenconchain(NULL, Duplicate_Tree(tree),
                                             FIG->LOSIG, Duplicate_Tree(tree));
         /*used by FCL*/
         FIG->LOSIG->USER=addptype(FIG->LOSIG->USER,LOFIGCHAIN,NULL);
         FIG->LOSIG->USER->DATA=addchain(FIG->LOSIG->USER->DATA,FIG->LOCON);
         index++;
      }
      SEPAR=memo_char;              /* put last value in MBK environment */
     }
     else if (TOKEN(tree->NEXT->NEXT->DATA)==GEN_TOKEN_DOWNTO) {
      long i;              /*help for verif in Mark_Loins() */
      int lim;
      tree=tree->NEXT->NEXT->DATA;   /* TOKEN(tree)=GEN_TOKEN_DOWNTO */
      lim=Eval_Exp_VHDL(tree->NEXT/*high bound*/,FOR_ENV);
      memo_char=SEPAR;             /* MBK variable it is for concatenation*/
      SEPAR=' ';  /* nameindex() must be compatible with vectorradical()  */ 

      for (i=Eval_Exp_VHDL(tree->NEXT->NEXT/*low bound*/,FOR_ENV); i<=lim; i++) {
         char *name;
         name=nameindex(conname,i);
         namechain=addchain(NULL,name);
         FIG->LOSIG=addlosig(FIG,index,namechain,EXTERNAL);/*WARN:no more capa*/
         /*default addlocon set EXTERNAL*/
         FIG->LOCON=addlocon(FIG,name,FIG->LOSIG,dir); 
         /*used by GENIUS*/
         FIG->LOCON->USER=addptype(FIG->LOCON->USER,GENCONCHAIN,NULL);
         FIG->LOCON->USER->DATA=addgenconchain(NULL, Duplicate_Tree(tree),
                                             FIG->LOSIG, Duplicate_Tree(tree));
         /*used by FCL*/
         FIG->LOSIG->USER=addptype(FIG->LOSIG->USER,LOFIGCHAIN,NULL);
         FIG->LOSIG->USER->DATA=addchain(FIG->LOSIG->USER->DATA,FIG->LOCON);
         index++;
      }
      
      SEPAR=memo_char;              /* put last value in MBK environment */
    }
    else {
      Error_Tree("Get_Locon_Alliance",tree->NEXT->NEXT->DATA);
      EXIT(2);
    }}
    else {
       int lineno=LINE(tree);
       char* file=FILE_NAME(tree);
       namechain=addchain(NULL,conname);
       FIG->LOSIG=addlosig(FIG,index,namechain,EXTERNAL); /*WARN:no more capa*/
       /*default addlocon set EXTERNAL*/
       FIG->LOCON=addlocon(FIG,conname,FIG->LOSIG,dir); 
       /*used by GENIUS*/
       FIG->LOCON->USER=addptype(FIG->LOCON->USER,GENCONCHAIN,NULL);
       FIG->LOCON->USER->DATA=addgenconchain(NULL, PUT_ATO(GEN_TOKEN_DIGIT,-1),
                                             FIG->LOSIG,  PUT_ATO(GEN_TOKEN_DIGIT,-1));
       /*used by FCL*/
       FIG->LOSIG->USER=addptype(FIG->LOSIG->USER,LOFIGCHAIN,NULL);
       FIG->LOSIG->USER->DATA=addchain(FIG->LOSIG->USER->DATA,FIG->LOCON);
    }
    break;
  default:
    Error_Tree("Get_Locon_Alliance",tree);
    EXIT(1);
  }
}


/****************************************************************************/
/*   build all the locons of and its signals from tree pointing on port     */
/****************************************************************************/
extern void Get_Port_Alliance(tree_list *tree)
{
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 239);
//    fprintf(stderr,"Get_Port_Alliance: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*node*/
  case GEN_TOKEN_NODE:
    Get_Port_Alliance(tree->DATA);
    break;
  case GEN_TOKEN_PORT:
    Get_Port_Alliance(tree->NEXT);
    break;
  case ',':                          /*it's a list of different connectors*/
    Get_Port_Alliance(tree->NEXT->NEXT); /*to avoid reversion of addchain*/
    Get_Port_Alliance(tree->NEXT);
    break;
  case GEN_TOKEN_SIGNAL:
    Get_Port_Alliance(tree->NEXT);
    break;
  case GEN_TOKEN_IN:
    Get_Locon_Alliance(IN,tree->NEXT);
    break;
  case GEN_TOKEN_OUT:
    Get_Locon_Alliance(OUT,tree->NEXT);
    break;
  case GEN_TOKEN_INOUT:
    Get_Locon_Alliance(INOUT,tree->NEXT);
    break;
  case GEN_TOKEN_TRISTATE:
    Get_Locon_Alliance(TRISTATE,tree->NEXT);
    break;
  case GEN_TOKEN_TRANSCV:
    Get_Locon_Alliance(TRANSCV,tree->NEXT);
    break;
  default:
    Error_Tree("Get_Port_Alliance",tree);
    EXIT(1);
  }
}


/****************************************************************************/
/*                 put environment to good values                           */
/****************************************************************************/
extern void Env_Alliance(lofig_list *lofig)
{
FIG=lofig;
}


