/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_exclude.c                                         */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 10/06/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include MUT_H
#include MLO_H
#include API_H
#include AVT_H
#include "gen_verif_exclude.h"


static ptype_list *EXCLUDE_INS; 
/*list of instances to exclude in current figure */
/* the TYPE field is either line number of C exclude or their negativ value*/
/* for already checked*/
static int EXCLUDED=0; /* flag put to 1 when current instance is excluded */


/****************************************************************************/
/*  return the list of string constant contained in tree and put on the top */
/* of exclude. verify double exclusion                                      */
/****************************************************************************/
static inline ptype_list *Verify_Exclude_In(tree_list *tree,
                                            ptype_list *exclude)
{
   ptype_list *p;
   char *name;
   if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 203);
    //fprintf(stderr,"Verify_Exclude_In: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
   /*nodes..*/
   case GEN_TOKEN_NODE:
      return Verify_Exclude_In(tree->DATA,exclude);
      break;
   case ',':
      exclude=Verify_Exclude_In(tree->NEXT,exclude);
      return Verify_Exclude_In(tree->NEXT->NEXT,exclude);
      break;
   case GEN_TOKEN_STRING:
      name=namealloc((char*)tree->DATA);
      for (p=exclude;p;p=p->NEXT) {
         if ((char*)p->DATA==name) {
    avt_errmsg(GNS_ERRMSG, "129", AVT_ERROR, FILE_NAME(tree),LINE(tree),name);
/*            fprintf(stderr,"%s:%d: %s already excluded\n",
                           FILE_NAME(tree),LINE(tree),name);*/
            Inc_Error();
            return exclude;
         }
      }    
      return addptype(exclude,LINE(tree),name);
      break;
   default:
      Error_Tree("Verify_Exclude_In",tree);
      EXIT(1); return NULL;
   }
}   


/****************************************************************************/
/*  return the list of instances excluded contained in tree and put on the  */
/* top of exclude.verify double exclusion                                   */
/****************************************************************************/
extern ptype_list *Verify_Exclude(tree_list *tree, 
                                           ptype_list *exclude) 
{
  if (!tree) { 
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 204);
//    fprintf(stderr,"Verify_Exclude: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
   /*nodes..*/
   case GEN_TOKEN_NODE:
      return Verify_Exclude(tree->DATA,exclude);
      break;
   case GEN_TOKEN_BLOCK:
      return Verify_Exclude(tree->NEXT,exclude);
      break;
   case ';':
      exclude=Verify_Exclude(tree->NEXT,exclude);
      return Verify_Exclude(tree->NEXT->NEXT,exclude);
      break;
   case GEN_TOKEN_EXCLUDE:
      exclude=Verify_Exclude_In(tree->NEXT,exclude);
      /*erase exclude*/
      Free_Tree(tree->NEXT);
      tree->TOKEN=GEN_TOKEN_NOP;
      return exclude;
      break;
   default:
      return exclude;
   }
}   


/****************************************************************************/
/* free the last list of exclude and build another from tree                */
/* must be done before all other functions below                            */
/****************************************************************************/
extern void Init_Exclude(tree_list *tree)
{
   freeptype(EXCLUDE_INS);
   EXCLUDE_INS=NULL;
   /* if we want only erase it --> call func. with NULL parameter*/
   if (tree) EXCLUDE_INS=Verify_Exclude(tree,NULL);
}   

extern void Expand_Exclude(ptype_list *pt)
{
  freeptype(EXCLUDE_INS);
  EXCLUDE_INS=NULL;
  if (pt==NULL) return;
  EXCLUDE_INS=(ptype_list *)append((chain_list *)EXCLUDE_INS, (chain_list *)pt);
}   

/***************************************************************************/
/*    control if loins is Excluded return line number if true else 0       */
/***************************************************************************/
extern int Is_Exclude(char *loins) {
   ptype_list *p;
   
   if (!loins) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 205);
//      fprintf(stderr,"Is_Exclude: NULL popinter\n");
      EXIT(1);
   }   
   
   for (p=EXCLUDE_INS;p;p=p->NEXT) {
      if ((char*)p->DATA==loins) {
         EXCLUDED=1;
         if (p->TYPE>0) p->TYPE=-p->TYPE;
         else return 0;
         return (int)-p->TYPE;
      }
   }
   EXCLUDED=0;
   return 0;
}


/***************************************************************************/
/*       control if all loins named in Excluded are in architecture        */
/***************************************************************************/
extern void Verif_Alone_Exclude(tree_list *archi) {
   ptype_list *p;

   if (!archi) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 206);
//      fprintf(stderr,"Verif_Alone_Exclude: NULL popinter\n");
      EXIT(1);
   }   
   
   for (p=EXCLUDE_INS;p;p=p->NEXT) {
      if (p->TYPE<0) p->TYPE=-p->TYPE;
      else {
    avt_errmsg(GNS_ERRMSG, "130", AVT_ERROR, FILE_NAME(archi),(int)p->TYPE,(char*)p->DATA,getname(archi),getname_of(archi));
/*         fprintf(stderr,
               "%s:%d: instance %s doesn't exist in architecture %s of %s\n",
                FILE_NAME(archi),(int)p->TYPE,(char*)p->DATA,getname(archi),getname_of(archi));*/
         Inc_Error();
      }   
   }
}

   
/***************************************************************************/
/*             comparison between type of tree and type of instance        */
/* an error is dumped if discrepancy                                       */
/***************************************************************************/
extern void Compare_Type(tree_list *tree,
                         tree_list *instance, int type_instance) 
{
  if (!tree || !instance) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 207);
//    fprintf(stderr,"Compare_Type: NULL pointer\n");
    EXIT(1);
  }
  
  /* for inout no error is possible*/
  if (type_instance==GEN_TOKEN_INOUT || type_instance==GEN_TOKEN_TRANSCV) return;
  
  /*comparison of type_instance component and signal used*/
  switch (TOKEN(tree)) {
     case GEN_TOKEN_INOUT: case GEN_TOKEN_TRANSCV:
       /*no error is possible*/
       break;
     case GEN_TOKEN_IN:
       /* if instance is outside the model (exclude); */
       /*type_instance must be the opposite */
       /*but they are exceptions*/
       if (EXCLUDED) {
         /*instance is outside the model, we can have: 1 IN and the other OUT*/
         /*or both IN and another OUT not mentionned completes the driver*/ 
       }
       else if (type_instance!=GEN_TOKEN_IN) {
    avt_errmsg(GNS_ERRMSG, "131", AVT_ERROR, FILE_NAME(instance),LINE(instance),getname(tree),getname(instance));
/*          fprintf(stderr,
           "%s:%d: IN '%s' cannot be connected with OUT '%s'\n",
            FILE_NAME(instance),LINE(instance),getname(tree),getname(instance));*/
          Inc_Error();
       }
       break;
     case GEN_TOKEN_OUT: case GEN_TOKEN_TRISTATE:
       /* if instance is outside the model (exclude); */
       /*type_instance must be the opposite */
       if (EXCLUDED) {
         /*we are outside the model*/
         /*we can have an IN and an OUT... but NOT 2 OUT(multiple driver)*/
         if (type_instance==GEN_TOKEN_OUT) {
    avt_errmsg(GNS_ERRMSG, "132", AVT_ERROR, FILE_NAME(instance), LINE(instance),getname(tree),getname(instance));
/*             fprintf(stderr,
              "%s:%d: exclude(), OUT '%s' cannot be connected with OUT '%s'\n",
           FILE_NAME(instance), LINE(instance),getname(tree),getname(instance));*/
             Inc_Error();
         }
       }
       else if (type_instance!=GEN_TOKEN_OUT && type_instance!=GEN_TOKEN_TRISTATE) {
    avt_errmsg(GNS_ERRMSG, "132", AVT_ERROR, FILE_NAME(instance),LINE(instance),getname(tree),getname(instance));
/*          fprintf(stderr,
          "%s:%d: OUT '%s' cannot be connected with IN '%s'\n",
            FILE_NAME(instance),LINE(instance),getname(tree),getname(instance));*/
          Inc_Error();
       }
       break;
     default:
       Error_Tree("Compare_Type",tree);
       EXIT(2);  return ;
  }
}



