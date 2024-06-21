/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_vector.c                                          */
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
#include <limits.h>
#include MUT_H
#include MLO_H
#include API_H
#include AVT_H
#include "gen_verif_exp_VHDL.h"
#include "gen_verif_vector.h"
#include "gen_execute_VHDL.h"

#define DEFAULT_ARRAY 0x4   /* generic vectors have a size of 4 */


/***************************************************************************/
/* compare if arrays of instance and model are equal                       */
/* if different return 1 else 0                                            */
/* a signal without any dimension matches with all vector model (cf. VHDL) */
/***************************************************************************/
static inline int __Compare_Vector(tree_list *instance, tree_list *model, 
                           ptype_list *env_instance, ptype_list *env_model)
{ 
  int bound1,bound2;
  if (!instance || !model) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 225);
//    fprintf(stderr,"__Compare_Vector: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(instance)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    if (TOKEN(model)!=GEN_TOKEN_NODE) {
      Error_Tree("__Compare_Vector",model);
      EXIT(2); 
    }
    return __Compare_Vector(instance->DATA,model->DATA,env_instance,env_model);
    break;
  case GEN_TOKEN_TO:
    if (TOKEN(model)==GEN_TOKEN_TO) {
      bound1=Verif_Exp_VHDL(instance->NEXT/*low*/,env_instance);
      bound2=Verif_Exp_VHDL(model->NEXT/*low*/,env_model);
//      printf("HERE3 \n");
      if (bound1!=bound2) return 1;
      bound1=Verif_Exp_VHDL(instance->NEXT->NEXT/*high*/,env_instance);
      bound2=Verif_Exp_VHDL(model->NEXT->NEXT/*high*/,env_model);
//      printf("HERE4 \n");
      if (bound1!=bound2) return 1;
      return 0;
    }
    else if (TOKEN(model)==GEN_TOKEN_DOWNTO) {
      bound1=Verif_Exp_VHDL(instance->NEXT/*low*/,env_instance);
      bound2=Verif_Exp_VHDL(model->NEXT->NEXT/*low*/,env_model);
//      printf("HERE2 \n");
      if (bound1!=bound2) return 1;
      bound1=Verif_Exp_VHDL(instance->NEXT->NEXT/*high*/,env_instance);
      bound2=Verif_Exp_VHDL(model->NEXT/*high*/,env_model);
//      printf("HERE1 \n");
      if (bound1!=bound2) return 1;
      return 0;
    }
    else return 1;
    break;
  case GEN_TOKEN_DOWNTO:
    if (TOKEN(model)==GEN_TOKEN_TO) {
      bound1=Verif_Exp_VHDL(instance->NEXT->NEXT/*low*/,env_instance);
      bound2=Verif_Exp_VHDL(model->NEXT/*low*/,env_model);
      if (bound1!=bound2) return 1;
      bound1=Verif_Exp_VHDL(instance->NEXT/*high*/,env_instance);
      bound2=Verif_Exp_VHDL(model->NEXT->NEXT/*high*/,env_model);
      if (bound1!=bound2) return 1;
      return 0;
    }
    else if (TOKEN(model)==GEN_TOKEN_DOWNTO) {
      bound1=Verif_Exp_VHDL(instance->NEXT->NEXT/*low*/,env_instance);
      bound2=Verif_Exp_VHDL(model->NEXT->NEXT/*low*/,env_model);
      if (bound1!=bound2) return 1;
      bound1=Verif_Exp_VHDL(instance->NEXT/*high*/,env_instance);
      bound2=Verif_Exp_VHDL(model->NEXT/*high*/,env_model);
      if (bound1!=bound2) return 1;
      return 0;
    }
    else return 1;
    break;
  default:  /*expression*/
    if (TOKEN(model)!=GEN_TOKEN_TO && TOKEN(model)!=GEN_TOKEN_DOWNTO) {
      bound1=Verif_Exp_VHDL(instance,env_instance);
      bound2=Verif_Exp_VHDL(model,env_model);
      if (bound1!=bound2) return 1;
      return 0;
    }
    else return 1;
  }
}


/***************************************************************************/
/* compare if arrays of instance and model are equal                       */
/* if different return 1 else 0                                            */
/* a signal without any dimension matches with all vector model (cf. VHDL) */
/***************************************************************************/
extern int Compare_Vector(tree_list *instance, tree_list *model, 
                           ptype_list *env_instance, ptype_list *env_model)
{ 
  if (!instance || !model) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 226);
//    fprintf(stderr,"Compare_Vector: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(instance)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    if (TOKEN(model)!=GEN_TOKEN_NODE) {
      Error_Tree("Compare_Vector",model);
      EXIT(2); 
    }
    return Compare_Vector(instance->DATA,model->DATA,env_instance,env_model);
    break;
  case GEN_TOKEN_IDENT:
  /*a signal witout any array matches with all dimensions*/
    return 0;
    break;
  case '(':
    if (TOKEN(model)=='(')
      return __Compare_Vector(instance->NEXT->NEXT,model->NEXT->NEXT,env_instance,env_model);
    else 
      {
	if (TOKEN(model)==GEN_TOKEN_IDENT)
    avt_errmsg(GNS_ERRMSG, "158", AVT_ERROR, getname(model));
	  fprintf(stderr,"You must explicitly specify the vector range for '%s'\n",getname(model));
	return 1;
      }
    break;
  default:
    Error_Tree("Compare_Vector",instance);
    EXIT(2); 
    return 0;
  }
}


/***************************************************************************/
/* verify that instance vector is contained by model vector                */
/***************************************************************************/
static inline int __Bound_Vector(tree_list *instance, tree_list *model,
                        ptype_list *env)
{ 
  if (!instance || !model) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 227);
//    fprintf(stderr,"__Bound_Vector: NULL pointer\n");
    EXIT(1);
  }
  return 0;

  switch (TOKEN(instance)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    if (TOKEN(model)!=GEN_TOKEN_NODE) {
      Error_Tree("__Bound_Vector",model);
      EXIT(2); 
    }
    return __Bound_Vector(instance->DATA,model->DATA,env);
    break;
  case GEN_TOKEN_TO:
    if (TOKEN(model)==GEN_TOKEN_TO) {
      int bound, bound_up,bound_down;
      bound=Verif_Exp_VHDL(instance->NEXT/*low bound*/,env);
      bound_down=Verif_Exp_VHDL(model->NEXT/*low bound*/,env);
      if (bound<bound_down) return 1;
      bound_up=Verif_Exp_VHDL(model->NEXT->NEXT/*high bound*/,env);
      if (bound>bound_up) return 1;
      bound=Verif_Exp_VHDL(instance->NEXT->NEXT/*high bound*/,env);
      if (bound>bound_up) return 1;
      if (bound<bound_down) return 1;
      return 0;
    }
    else if (TOKEN(model)==GEN_TOKEN_DOWNTO) {
      int bound, bound_up,bound_down;
      bound=Verif_Exp_VHDL(instance->NEXT/*low bound*/,env);
      bound_down=Verif_Exp_VHDL(model->NEXT->NEXT/*low bound*/,env);
      if (bound<bound_down) return 1;
      bound_up=Verif_Exp_VHDL(model->NEXT/*high bound*/,env);
      if (bound>bound_up) return 1;
      bound=Verif_Exp_VHDL(instance->NEXT->NEXT/*high bound*/,env);
      if (bound>bound_up) return 1;
      if (bound<bound_down) return 1;
      return 0;
    }
    else return 1;
    break;
  case GEN_TOKEN_DOWNTO:
    if (TOKEN(model)==GEN_TOKEN_TO) {
      int bound, bound_up,bound_down;
      bound=Verif_Exp_VHDL(instance->NEXT->NEXT/*low bound*/,env);
      bound_down=Verif_Exp_VHDL(model->NEXT/*low bound*/,env);
      if (bound<bound_down) return 1;
      bound_up=Verif_Exp_VHDL(model->NEXT->NEXT/*high bound*/,env);
      if (bound>bound_up) return 1;
      bound=Verif_Exp_VHDL(instance->NEXT/*high bound*/,env);
      if (bound>bound_up) return 1;
      if (bound<bound_down) return 1;
      return 0;
    }
    else if (TOKEN(model)==GEN_TOKEN_DOWNTO) {
      int bound, bound_up,bound_down;
      bound=Verif_Exp_VHDL(instance->NEXT->NEXT/*low bound*/,env);
      bound_down=Verif_Exp_VHDL(model->NEXT->NEXT/*low bound*/,env);
      if (bound<bound_down) return 1;
      bound_up=Verif_Exp_VHDL(model->NEXT/*high bound*/,env);
      if (bound>bound_up) return 1;
      bound=Verif_Exp_VHDL(instance->NEXT/*high bound*/,env);
      if (bound>bound_up) return 1;
      if (bound<bound_down) return 1;
      return 0;
    }
    else return 1;
    break;
  default: /*it's surely an exp*/
    // zinaps: on n'a pas d'information sur les variables, on ne peut donc pas tester les bornes ici
#if 0
    if (TOKEN(model)==GEN_TOKEN_TO) {
      int bound, bound_up,bound_down;
      printf("here0\n");
      bound=Verif_Exp_VHDL(instance/*expression*/,env);
      bound_down=Verif_Exp_VHDL(model->NEXT/*low bound*/,env);
      if (bound<bound_down) return 1;
      bound_up=Verif_Exp_VHDL(model->NEXT->NEXT/*high bound*/,env);
      if (bound>bound_up) return 1;
      return 0;
    }
    else if (TOKEN(model)==GEN_TOKEN_DOWNTO) {
      int bound, bound_up,bound_down;
      bound=Verif_Exp_VHDL(instance/*expression*/,env);
      bound_down=Verif_Exp_VHDL(model->NEXT->NEXT/*low bound*/,env);
      printf("here1 %d %d\n",bound,bound_down);
      if (bound<bound_down) return 1;
      bound_up=Verif_Exp_VHDL(model->NEXT/*high bound*/,env);
      printf("here1 %d %d\n",bound,bound_up);
      if (bound>bound_up) return 1;
      return 0;
    }
    else return 1;
#endif
    return 0;
  }
}


/***************************************************************************/
/* verify that instance vector is contained by model vector                */
/***************************************************************************/
extern int Bound_Vector(tree_list *instance, tree_list *model,
                        ptype_list *env)
{ 
  if (!instance || !model) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 228);
//    fprintf(stderr,"Bound_Vector: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(instance)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    if (TOKEN(model)!=GEN_TOKEN_NODE) {
      Error_Tree("Bound_Vector",model);
      EXIT(2); 
    }
    return Bound_Vector(instance->DATA,model->DATA,env);
    break;
  case GEN_TOKEN_IDENT:
  /*a signal witout any array matches with all dimensions*/
    return 0;
    break;
  case '(':
    if (TOKEN(model)=='(') {
      return __Bound_Vector(instance->NEXT->NEXT,model->NEXT->NEXT,env);
    }
    else return 1;
    break;
  default:
    Error_Tree("Bound_Vector",instance);
    EXIT(2); 
    return 0;
  }
}


/***************************************************************************/
/*               verify the direction of a vector array                    */
/* return size of vector                                                   */
/* env is the list of known variables                                      */
/***************************************************************************/
static inline int __Verif_Vector(tree_list *tree, ptype_list *env)
{ 
  int res1,res2;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 229);
//    fprintf(stderr,"__Verif_Vector: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return __Verif_Vector(tree->DATA,env);
    break;
  case GEN_TOKEN_TO:
    res1=Verif_Exp_VHDL(tree->NEXT/*low bound*/,NULL);
    res2=Verif_Exp_VHDL(tree->NEXT->NEXT/*high bound*/,env);
/*
    if (res1<0 || res2<0) {
      fprintf(stderr,"%s:%d: negative vector index\n", FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return (res1<res2)?res2-res1+1:res1-res2+1;
    }
    if (res1>res2) {
      fprintf(stderr,"%s:%d: vector direction error\n", FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return res1-res2+1;
    }
    */
    return res2-res1+1;
    break;
  case GEN_TOKEN_DOWNTO:
    res1=Verif_Exp_VHDL(tree->NEXT->NEXT/*low bound*/,NULL);
    res2=Verif_Exp_VHDL(tree->NEXT/*high bound*/,env);
/*    if (res1<0 || res2<0) {
      fprintf(stderr,"%s:%d: negative vector index\n", FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return (res1<res2)?res2-res1+1:res1-res2+1;
    }
    if (res1>res2) {
      fprintf(stderr,"%s:%d: vector direction error\n", FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return res1-res2+1;
    }
    */
    return res2-res1+1;
    break;
  default:  /*it's surely an expression*/
    Verif_Exp_VHDL(tree,env);
    return 1;
  }
}


/***************************************************************************/
/*               verify the direction of a vector array                    */
/* return size of vector                                                   */
/* env is the list of known variables                                      */
/***************************************************************************/
extern int Verif_Vector(tree_list *tree, ptype_list *env)
{ 
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 230);
//    fprintf(stderr,"Verif_Vector: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_Vector(tree->DATA,env);
    break;
  case GEN_TOKEN_IDENT:
    return 1;
    break;
  case '(':
    return __Verif_Vector(tree->NEXT->NEXT,env);
    break;
  default:  /*it's surely an expression*/
    Error_Tree("Verif_Vector",tree);
    EXIT(2); 
    return 0;
  }
}


/***************************************************************************/
/*               verify the direction of a vector array                    */
/* return size of vector                                                   */
/* env is the list of known variables                                      */
/***************************************************************************/
static int nbvars;

static inline int __Size_Vector(tree_list *tree, ptype_list *env)
{ 
  int res1,res2;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 231);
//    fprintf(stderr,"__Size_Vector: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return __Size_Vector(tree->DATA,env);
    break;
  case GEN_TOKEN_TO:
    /* Different from Size_Vector()  !! */
    res1=Verif_Exp_VHDL(tree->NEXT/*low*/,env); 
    res2=Verif_Exp_VHDL(tree->NEXT->NEXT/*high*/,env);
    nbvars+=__Count_Vars_Exp_VHDL(tree->NEXT)+__Count_Vars_Exp_VHDL(tree->NEXT->NEXT);
    if (res1<0 || res2<0) {
    avt_errmsg(GNS_ERRMSG, "159", AVT_ERROR, FILE_NAME(tree),LINE(tree));
//      fprintf(stderr,"%s:%d: negative vector index\n", FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return (res1<res2)?res2-res1+1:res1-res2+1;
    }
    if (res1>res2) {
    avt_errmsg(GNS_ERRMSG, "160", AVT_ERROR, FILE_NAME(tree),LINE(tree));
//      fprintf(stderr,"%s:%d: vector direction error\n", FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return res1-res2+1;
    }
    return res2-res1+1;
    break;
  case GEN_TOKEN_DOWNTO:
    /* Different from Size_Vector()  !! */
    res1=Verif_Exp_VHDL(tree->NEXT->NEXT/*low*/,env);
    res2=Verif_Exp_VHDL(tree->NEXT/*high*/,env);
    nbvars+=__Count_Vars_Exp_VHDL(tree->NEXT)+__Count_Vars_Exp_VHDL(tree->NEXT->NEXT);
    if (res1<0 || res2<0) {
    avt_errmsg(GNS_ERRMSG, "159", AVT_ERROR, FILE_NAME(tree),LINE(tree));
    //  fprintf(stderr,"%s:%d: negative vector index\n", FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return (res1<res2)?res2-res1+1:res1-res2+1;
    }
    if (res1>res2) {
    avt_errmsg(GNS_ERRMSG, "160", AVT_ERROR, FILE_NAME(tree),LINE(tree));
    //  fprintf(stderr,"%s:%d: vector direction error\n", FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return res1-res2+1;
    }
    return res2-res1+1;
    break;
  default:  /*it's surely an expression*/
    Verif_Exp_VHDL(tree,env);
    return 1;
  }
}


/***************************************************************************/
/*               verify the direction of a vector array                    */
/* return size of vector                                                   */
/* env is the list of known variables                                      */
/***************************************************************************/
extern int Size_Vector(tree_list *tree, ptype_list *env)
{ 
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 232);
//    fprintf(stderr,"Size_Vector: NULL pointer\n");
    EXIT(1);
  }
  nbvars=0;
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Size_Vector(tree->DATA,env);
    break;
  case GEN_TOKEN_IDENT:
    return 1;
    break;
  case '(':
    return __Size_Vector(tree->NEXT->NEXT,env);
    break;
  default:  /*it's surely an expression*/
    Error_Tree("Size_Vector",tree);
    EXIT(2); 
    return 0;
  }
}


int Nb_Vector_Vars()
{
  return nbvars;
}
