/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_exp_VHDL.c                                        */
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
#include MUT_H
#include MLO_H
#include API_H
#include AVT_H
#include "gen_verif_utils.h"
#include "gen_verif_exp_VHDL.h"



/***************************************************************************/
/*          Control the expression of FOR in VHDL for later searching      */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Verif_HighBound_FOR(tree,env)
   tree_list *tree;
   ptype_list *env;
{  
  int var1,var2;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 208);
    //fprintf(stderr,"Verif_HighBound_FOR: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  case '+':  case '-':  case '*':  case '/':
      var1=Verif_HighBound_FOR(tree->NEXT,env);
      var2=Verif_HighBound_FOR(tree->NEXT->NEXT,env);
      if (var1 && var2) {
    avt_errmsg(GNS_ERRMSG, "134", AVT_ERROR, FILE_NAME(tree),LINE(tree));
   //      fprintf(stderr,"%s:%d: Only one variable authorized in a 'for' expression. Use Hierarchy!\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
         return 0;
      }
      return var1||var2;
      break;
  case '%':
      var1=Verif_HighBound_FOR(tree->NEXT,env);
      var2=Verif_HighBound_FOR(tree->NEXT->NEXT,env);
      if (var1 && var2) {
    avt_errmsg(GNS_ERRMSG, "134", AVT_ERROR, FILE_NAME(tree),LINE(tree));
  //       fprintf(stderr,"%s:%d: Only one variable authorized in a 'for' expression. Use Hierarchy!\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
         return 0;
      }
      if (var1||var2) {
    avt_errmsg(GNS_ERRMSG, "135", AVT_ERROR, FILE_NAME(tree),LINE(tree),"rem");
//         fprintf(stderr,"%s:%d: forbidden operator 'rem' on variable\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
      }
      return var1||var2;
      break;
  case GEN_TOKEN_MOD:
      var1=Verif_HighBound_FOR(tree->NEXT,env);
      var2=Verif_HighBound_FOR(tree->NEXT->NEXT,env);
      if (var1 && var2) {
    avt_errmsg(GNS_ERRMSG, "134", AVT_ERROR, FILE_NAME(tree),LINE(tree));
    //     fprintf(stderr,"%s:%d: Only one variable authorized in a 'for' expression. Use Hierarchy!\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
         return 0;
      }
      if (var1||var2) {
    avt_errmsg(GNS_ERRMSG, "135", AVT_ERROR, FILE_NAME(tree),LINE(tree),"mod");
  //       fprintf(stderr,"%s:%d: forbidden operator 'mod' on variable\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
      }
      return var1||var2;
      break;
  case GEN_TOKEN_POW:
      var1=Verif_HighBound_FOR(tree->NEXT,env);
      var2=Verif_HighBound_FOR(tree->NEXT->NEXT,env);
      if (var1 && var2) {
    avt_errmsg(GNS_ERRMSG, "134", AVT_ERROR, FILE_NAME(tree),LINE(tree));
    //     fprintf(stderr,"%s:%d: Only one variable authorized in a 'for' expression. Use Hierarchy!\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
         return 0;
      }
      if (var1||var2) {
    avt_errmsg(GNS_ERRMSG, "135", AVT_ERROR, FILE_NAME(tree),LINE(tree),"**");
    //     fprintf(stderr,"%s:%d: forbidden operator '**' on variable\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
      }
      return var1||var2;
      break;
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return Verif_HighBound_FOR(tree->DATA,env);
      break;
  case GEN_TOKEN_OPPOSITE: 
      return Verif_HighBound_FOR(tree->NEXT,env);
      break;
  case GEN_TOKEN_DIGIT:
      return 0;
      break;
  case GEN_TOKEN_IDENT:
      return 1;
      break;
  default:
    Error_Tree("Verif_HighBound_FOR",tree);
    EXIT(1); return 0;
  }
}


/***************************************************************************/
/*               eval and verify a scalar expression in VHDL               */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Verif_Exp_VHDL(tree_list *tree, ptype_list *env)
{  
  int i,ope,ope2;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 209);
//    fprintf(stderr,"Verif_Exp_VHDL: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return Verif_Exp_VHDL(tree->DATA,env);
      break;
  case '+': 
      return Verif_Exp_VHDL(tree->NEXT,env)+Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case '-': 
      return Verif_Exp_VHDL(tree->NEXT,env)-Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case '*': 
      return Verif_Exp_VHDL(tree->NEXT,env)*Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case '/':
      ope=Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      if (ope==0) {
         /*here LINE represents where the 2nd operand ends*/
    avt_errmsg(GNS_ERRMSG, "136", AVT_FATAL, FILE_NAME(tree),LINE(tree));
    //     fprintf(stderr,"%s:%d: division by zero could appear\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
         return Verif_Exp_VHDL(tree->NEXT,env);         
      }
      return Verif_Exp_VHDL(tree->NEXT,env)/ope;
      break;
  case '%':
      ope=Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      if (ope==0) {
         /*here LINE represents where the 2nd operand ends*/
    avt_errmsg(GNS_ERRMSG, "136", AVT_FATAL, FILE_NAME(tree),LINE(tree));
    //     fprintf(stderr,"%s:%d: division by zero could appear\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
         return Verif_Exp_VHDL(tree->NEXT,env);
      }
      return Verif_Exp_VHDL(tree->NEXT,env)%ope;
      break;
  case GEN_TOKEN_MOD:
      ope=Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      if (ope==0) {
         /*here LINE represents where the 2nd operand ends*/
    avt_errmsg(GNS_ERRMSG, "136", AVT_FATAL, FILE_NAME(tree),LINE(tree));
    //     fprintf(stderr,"%s:%d: division by zero could appear\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
         return Verif_Exp_VHDL(tree->NEXT,env);
      }
      ope=Verif_Exp_VHDL(tree->NEXT,env)%ope;
      return ope<0?-ope:ope;
      break;
  case GEN_TOKEN_POW:
      ope2=1;
      ope=Verif_Exp_VHDL(tree->NEXT,env);
      for (i=Verif_Exp_VHDL(tree->NEXT->NEXT,env);i>0;i--) ope2*=ope;
      return ope2;
      break;
  case GEN_TOKEN_OPPOSITE: 
      return -Verif_Exp_VHDL(tree->NEXT,env);
      break;
  case GEN_TOKEN_DIGIT:
      return (int)(long)tree->DATA;
      break;
  case GEN_TOKEN_IDENT:
      ope=fetch_value(tree,env); /*check if exists and take the value*/
      return ope;
      break;
  default:
    Error_Tree("Verif_Exp_VHDL",tree);
    EXIT(1); return 0;
  }
}


/***************************************************************************/
/*            Verify and Evaluate a boolean expression in VHDL             */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Verif_Bool_VHDL(tree, env)
   tree_list *tree;
   ptype_list *env;
{  
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 210);
//    fprintf(stderr,"Verif_Bool_VHDL: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return Verif_Bool_VHDL(tree->DATA,env);
      break;
  case GNS_TOKEN_AND: 
      return 
      Verif_Bool_VHDL(tree->NEXT,env)&&Verif_Bool_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_OR: 
      return
      Verif_Bool_VHDL(tree->NEXT,env)||Verif_Bool_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_XOR: 
      return 0x1&&
      (Verif_Bool_VHDL(tree->NEXT,env)^Verif_Bool_VHDL(tree->NEXT->NEXT,env));
      break;
  case GEN_TOKEN_NOT: 
      return !Verif_Bool_VHDL(tree->NEXT,env);
      break;
  case '<': 
      return 
      Verif_Exp_VHDL(tree->NEXT,env)<Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case '>': 
      return 
      Verif_Exp_VHDL(tree->NEXT,env)>Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_EG: 
      return 
      Verif_Exp_VHDL(tree->NEXT,env)==Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_SUPEG: 
      return 
      Verif_Exp_VHDL(tree->NEXT,env)>=Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_INFEG: 
      return 
      Verif_Exp_VHDL(tree->NEXT,env)<=Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_NOTEG: 
      return 
      Verif_Exp_VHDL(tree->NEXT,env)!=Verif_Exp_VHDL(tree->NEXT->NEXT,env);
      break;
  default:
    Error_Tree("Verif_Bool_VHDL",tree);
    EXIT(1); return 0;
  }
}



