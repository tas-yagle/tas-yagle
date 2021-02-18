/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_execute_VHDL.c                                          */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 27/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <math.h>
#include MUT_H
#include MLO_H
#include API_H
#include "gen_model_utils.h"
#include "gen_corresp.h"
#include "gen_search_utils.h"
#include "gen_execute_VHDL.h"


int VAR_UNKNOWN=0;     /*flag if variable isn't found in environment*/
int VAR_UNDEF=0;       /*flag if var. with no value*/



/***************************************************************************/
/*                    Evaluate a scalar expression in VHDL                 */
/*                 env list of variables with their values                 */
/***************************************************************************/
static inline int __Eval_Exp_VHDL(tree_list *tree, ptype_list *env)
{  
  int ope,ope2;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 6);
//    fprintf(stderr,"__Eval_Exp_VHDL: NULL pointer\n");
    EXIT(1);
  }

  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return __Eval_Exp_VHDL(tree->DATA,env);
      break;
  case '+': 
    ope=__Eval_Exp_VHDL(tree->NEXT,env);
    ope2=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
    if (ope==UNDEF || ope2==UNDEF)
      return UNDEF;
    return ope + ope2;
    break;
  case '-': 
    ope=__Eval_Exp_VHDL(tree->NEXT,env);
    ope2=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
    if (ope==UNDEF || ope2==UNDEF)
      return UNDEF;
    return ope - ope2;
//      return __Eval_Exp_VHDL(tree->NEXT,env)-__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case '*': 
    ope=__Eval_Exp_VHDL(tree->NEXT,env);
    ope2=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
    if (ope==UNDEF || ope2==UNDEF)
      return UNDEF;
    return ope * ope2;
    //      return __Eval_Exp_VHDL(tree->NEXT,env)*__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case '/':
      ope2=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      if (ope2==0) {
         /*here LINE represents where the 2nd operand ends*/
         avt_errmsg(GNS_ERRMSG, "026", AVT_FATAL, FILE_NAME(tree),LINE(tree));
//         fprintf(stderr,"%s:%d: division by zero\n",FILE_NAME(tree),LINE(tree));
         EXIT(1);        
      }
      ope=__Eval_Exp_VHDL(tree->NEXT,env);
      if (ope==UNDEF || ope2==UNDEF)
	return UNDEF;
      return ope / ope2;
//      return __Eval_Exp_VHDL(tree->NEXT,env)/ope;
      break;
  case '%':
      ope=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      if (ope==0) {
         /*here LINE represents where the 2nd operand ends*/
         avt_errmsg(GNS_ERRMSG, "026", AVT_FATAL, FILE_NAME(tree),LINE(tree));
         //fprintf(stderr,"%s:%d: division by zero\n",FILE_NAME(tree),LINE(tree));
         EXIT(1);        
      }
      ope2=__Eval_Exp_VHDL(tree->NEXT,env);
      if (ope==UNDEF || ope2==UNDEF)
	return UNDEF;
      return ope2%ope;
      break;
  case GEN_TOKEN_MOD:
      ope=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      if (ope==0) {
         /*here LINE represents where the 2nd operand ends*/
         avt_errmsg(GNS_ERRMSG, "026", AVT_FATAL, FILE_NAME(tree),LINE(tree));
         //fprintf(stderr,"%s:%d: division by zero\n",FILE_NAME(tree),LINE(tree));
         EXIT(1);
      }
      ope2=__Eval_Exp_VHDL(tree->NEXT,env);
      if (ope==UNDEF || ope2==UNDEF)
	return UNDEF;
      ope=ope2%ope;
      return ope<0?-ope:ope;
      break;
  case GEN_TOKEN_POW:
      ope=__Eval_Exp_VHDL(tree->NEXT,env);
      ope2=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      if (ope==UNDEF || ope2==UNDEF)
	return UNDEF;
      return pow(ope,ope2);
      break;
  case GEN_TOKEN_OPPOSITE: 
    ope=__Eval_Exp_VHDL(tree->NEXT,env);
    if (ope==UNDEF)
      return UNDEF;
    return -ope;
    break;
  case GEN_TOKEN_DIGIT:
    return (int)(long)tree->DATA;
    break;
  case GEN_TOKEN_IDENT:
    {
      ptype_list *a;
      char *name=getname(tree);
      for (a=env;a;a=a->NEXT) 
	{
	  if (a->DATA==name) 
	    {
	      if (a->TYPE==UNDEF) VAR_UNDEF=1;
	      return a->TYPE;
	    }
	}
    }
    VAR_UNKNOWN=1;
    return UNDEF;
    break;
  default:
    Error_Tree("__Eval_Exp_VHDL",tree);
    EXIT(1); return 0;
  }
}


/***************************************************************************/
/*                    Evaluate a scalar expression in VHDL                 */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Eval_Exp_VHDL(tree,env)
   tree_list *tree;
   ptype_list *env;
{
   int ret;
   
   VAR_UNKNOWN=0;
   VAR_UNDEF=0;
   ret = __Eval_Exp_VHDL(tree,env);
   if (VAR_UNDEF || VAR_UNKNOWN) return UNDEF;
   else return ret;
}


/***************************************************************************/
/*                   Evaluate a boolean expression in VHDL                 */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Eval_Bool_VHDL(tree,env)
   tree_list *tree;
   ptype_list *env;
{  
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 7);
//    fprintf(stderr,"Eval_Bool_VHDL: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return Eval_Bool_VHDL(tree->DATA,env);
      break;
  case GNS_TOKEN_AND: 
      return Eval_Bool_VHDL(tree->NEXT,env)&&Eval_Bool_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_OR: 
      return Eval_Bool_VHDL(tree->NEXT,env)||Eval_Bool_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_XOR: 
      return 0x1&&(Eval_Bool_VHDL(tree->NEXT,env)^Eval_Bool_VHDL(tree->NEXT->NEXT,env));
      break;
  case GEN_TOKEN_NOT: 
      return !Eval_Bool_VHDL(tree->NEXT,env);
      break;
  case '<': 
      return __Eval_Exp_VHDL(tree->NEXT,env)<__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case '>': 
      return __Eval_Exp_VHDL(tree->NEXT,env)>__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_EG: 
      return __Eval_Exp_VHDL(tree->NEXT,env)==__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_SUPEG: 
      return __Eval_Exp_VHDL(tree->NEXT,env)>=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_INFEG: 
      return __Eval_Exp_VHDL(tree->NEXT,env)<=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_NOTEG: 
      return __Eval_Exp_VHDL(tree->NEXT,env)!=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  default:
    Error_Tree("Eval_Bool_VHDL",tree);
    EXIT(1); return 0;
  }
}


/***************************************************************************/
/*               Solve an equation with one variable in VHDL               */
/*                 env list of variables with their values                 */
/***************************************************************************/
static int NbSolved;

void ResetNbSolved() { NbSolved=0;}
int GetNbSolved() { return NbSolved;}

extern void Solve_Equ_VHDL(tree,result,env)
   tree_list *tree;
   int result;
   ptype_list *env;
{  
  int res1=0,res2=0;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 8);
//    fprintf(stderr,"Solve_Equ_VHDL: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  case '+':  case '-':  case '*':  case '/':
      res1=__Eval_Exp_VHDL(tree->NEXT,env);
      res2=__Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case '%':  case GEN_TOKEN_MOD:  case GEN_TOKEN_POW:
      /*too difficult to solve*/
      /* with parser verification we shouldn't have this case to verify */
      return;
      break;
  }    
  
  switch (TOKEN(tree)) {
  case '%':  case GEN_TOKEN_MOD:  case GEN_TOKEN_POW:
      /* with parser verification we shouldn't have this case */
      avt_errmsg(GNS_ERRMSG, "027", AVT_FATAL);
//      fprintf(stderr,"Solve_Equ_VHDL: forbidden operators 'mod', 'rem', '**'\n");
      EXIT(1);
      break;
  /*nodes..*/
  case GEN_TOKEN_NODE:
      Solve_Equ_VHDL(tree->DATA,result,env);
      break;
  case '+': 
      Solve_Equ_VHDL(tree->NEXT,result-res2,env);
      Solve_Equ_VHDL(tree->NEXT->NEXT,result-res1,env);
      break;
  case '-': 
      Solve_Equ_VHDL(tree->NEXT,result+res2,env);
      Solve_Equ_VHDL(tree->NEXT->NEXT,res1-result,env);
      break;
  case '*': 
      Solve_Equ_VHDL(tree->NEXT,result/res2,env);
      Solve_Equ_VHDL(tree->NEXT->NEXT,result/res1,env);
      break;
  case '/':
      Solve_Equ_VHDL(tree->NEXT,result*res2,env);
      Solve_Equ_VHDL(tree->NEXT->NEXT,res1/result,env);
      break;
  case GEN_TOKEN_OPPOSITE: 
      Solve_Equ_VHDL(tree->NEXT,-result,env);
      break;
  case GEN_TOKEN_DIGIT:
      /*nothing to do*/
      break;
  case GEN_TOKEN_IDENT:
      {ptype_list *a;
      char *name=getname(tree);
      for (a=env;a;a=a->NEXT) {
         if (a->DATA==name) {
	   if (a->TYPE==UNDEF) { a->TYPE=result; NbSolved++; }
            return;
         }   
      } 
      avt_errmsg(GNS_ERRMSG, "028", AVT_FATAL, name);
//      fprintf(stderr,"Solve_Equ_VHDL: %s not found\n",name);
      EXIT(1);}    
      break;
  default:
    Error_Tree("Solve_Equ_VHDL",tree);
    EXIT(1);
  }
}


/***************************************************************************/
/*           return the name of the first variable in expression           */
/***************************************************************************/
extern char* Getname_Exp_VHDL(tree)
   tree_list *tree;
{  
  char* var;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 9);
//    fprintf(stderr,"Getname_Exp_VHDL: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return Getname_Exp_VHDL(tree->DATA);
      break;
  case '+': case '-': case '%': case '/': case '*': case GEN_TOKEN_MOD: case GEN_TOKEN_POW:
      var=Getname_Exp_VHDL(tree->NEXT);
      if (!var) return Getname_Exp_VHDL(tree->NEXT->NEXT);
      else return var;
      break;
  case GEN_TOKEN_OPPOSITE: 
      return Getname_Exp_VHDL(tree->NEXT);
      break;
  case GEN_TOKEN_DIGIT:
      return NULL;
      break;
  case GEN_TOKEN_IDENT:
      return (char*) tree->DATA;
      break;
  default:
      Error_Tree("Getname_Exp_VHDL",tree);
      EXIT(1); return NULL;
  }
}

// zinaps
extern chain_list *GetnameS_Exp_VHDL(tree)
   tree_list *tree;
{  
  chain_list * var, *var1;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 10);
//    fprintf(stderr,"Getname_Exp_VHDL: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return GetnameS_Exp_VHDL(tree->DATA);
      break;
  case '+': case '-': case '%': case '/': case '*': case GEN_TOKEN_MOD: case GEN_TOKEN_POW:
      var=GetnameS_Exp_VHDL(tree->NEXT);
      var1=GetnameS_Exp_VHDL(tree->NEXT->NEXT);
      var=append(var, var1);
      return var;
      break;
  case GEN_TOKEN_OPPOSITE: 
      return GetnameS_Exp_VHDL(tree->NEXT);
      break;
  case GEN_TOKEN_DIGIT:
      return NULL;
      break;
  case GEN_TOKEN_IDENT:
      return addchain(NULL,(char*) tree->DATA);
      break;
  default:
      Error_Tree("GetnameS_Exp_VHDL",tree);
      EXIT(1); return NULL;
  }
}


/****************************************************************************/
/*  return non-zero if there is a variable not defined in Eval function     */
/*  put flag to zero for next use                                           */
/****************************************************************************/
extern int Exp_VHDL_Undef()
{
   int ret=VAR_UNKNOWN;
   VAR_UNKNOWN=0;
   return ret;
}


/***************************************************************************/
/*                    Count variables in a tree                            */
/***************************************************************************/

int __Count_Vars_Exp_VHDL(tree_list *tree)
{  
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 11);
    //fprintf(stderr,"__Count_Vars_Exp_VHDL: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return __Count_Vars_Exp_VHDL(tree->DATA);
  case '+': 
  case '-': 
  case '*': 
  case '/':
  case '%':
  case GEN_TOKEN_MOD:
  case GEN_TOKEN_POW:
    return  __Count_Vars_Exp_VHDL(tree->NEXT)+ __Count_Vars_Exp_VHDL(tree->NEXT->NEXT);
  case GEN_TOKEN_OPPOSITE: 
    return __Count_Vars_Exp_VHDL(tree->NEXT);
    break;
  case GEN_TOKEN_DIGIT:
    return 0;
  case GEN_TOKEN_IDENT:
    return 1;
  default:
    Error_Tree("__Count_Vars_Exp_VHDL",tree);
    EXIT(1); return 0;
  }
}


extern int getsize(tree,env)
   tree_list *tree;
   ptype_list* env;
{ 
  int res1,res2;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 12);
//    fprintf(stderr,"getsize: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return getsize(tree->DATA,env);
    break;
  case '(':
    if (TOKEN(tree->NEXT->NEXT)==GEN_TOKEN_TO || TOKEN(tree->NEXT->NEXT)==GEN_TOKEN_DOWNTO) return getsize(tree->NEXT->NEXT,env);
    else return 1;
    break;
  case GEN_TOKEN_DOWNTO:
    res1=Eval_Exp_VHDL(tree->NEXT,env);
    res2=Eval_Exp_VHDL(tree->NEXT->NEXT,env);
    return res1-res2;
    break;
  case GEN_TOKEN_TO:
    res1=Eval_Exp_VHDL(tree->NEXT,env);
    res2=Eval_Exp_VHDL(tree->NEXT->NEXT,env);
    return res2-res1;
    break;
  case GEN_TOKEN_IDENT:
    return 1;
    break;
  default:
    Error_Tree("getsize",tree);
    EXIT(2); return 0;
  }
}
