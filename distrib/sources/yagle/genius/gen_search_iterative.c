/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_search_iterative.c                                      */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 12/08/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/


#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include MUT_H
#include MLO_H
#include FCL_H
#include API_H
#include "gen_env.h"
#include "gen_execute_VHDL.h"
#include "gen_model_utils.h"
#include "gen_model_transistor.h"
#include "gen_corresp.h"
#include "gen_search_utils.h"
#include "gen_search_recursive.h"
#include "gen_search_iterative.h"
#include "gen_expansion.h"
#include "gen_optimizations.h"
#include "gen_display.h"


static model_list* CURRENT_MODEL;
static chain_list* FOUNDINS_FOR;    /*list of list of foundins in for*/
static int LOOP_ERR; /*put to 1 if after phase 1 there is no head for a loop*/

char *current_variable_name;
chain_list *current_variable_values;
int isparal, isparal_count, smartselect;
static ptype_list *isparal_bound;
static tree_list *isparal_tree;

/*to avoid warning due to recursion*/
static  int recursive_FOR(tree_list* tree, foundins_list *begin_foundins, loins_list *modelins, ptype_list *offsets, ptype_list *env);


/****************************************************************************/
/*this is for a loop where the order is very important                      */
/****************************************************************************/
static int recursive_CHAIN(tree_list* tree, foundins_list *begin_foundins, loins_list *modelins, ptype_list *offsets, ptype_list *env)
{
   ptype_list *bound,*p;
   int match;
   char *local_ident,*bound_ident;
   foundins_list *which_link/*instance link to loop*/,*try_foundins;
   long sav_end_value,begin_value,end_value;
   long how_far;
   int fix_bound=0;   /*flag to know if bound 'n' is already defined*/
   foundins_list *sav_begin_foundins;

      Protect();
      match=0;
      sav_begin_foundins=begin_foundins;
      
      if (offsets) {
         how_far = offsets->TYPE;    /*distance between begin and circuit*/
         which_link = (foundins_list*) offsets->DATA;   /*just after*/
      }
      else {   /*no offset because the link is the head*/
         how_far = 0;    /*distance between head and link*/
         which_link = begin_foundins;   /*just after */
      }
      
      /*local environment  ==> 'i' */
      local_ident=getname(tree->NEXT);
      env=addptype(env,UNDEF,local_ident);
      
      /*entity environment ==> 'n' */
      bound_ident=Getname_Exp_VHDL(tree->NEXT->NEXT->NEXT);
      for (bound=env; bound; bound=bound->NEXT) 
         if (bound->DATA==bound_ident) break;
      if (!bound) {
         avt_errmsg(GNS_ERRMSG, "028", AVT_FATAL, bound_ident);
         //fprintf(stderr,"recursive_CHAIN: Variable %s not defined\n",bound_ident);
         EXIT(1);
      }  
      begin_value=Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      /* try the lowest predicate*/
      end_value=bound->TYPE;
      Solve_Equ_VHDL(tree->NEXT->NEXT->NEXT,
               begin_foundins->SIZE+begin_value-1,env);
      if (end_value==UNDEF) end_value=bound->TYPE;
      else if (bound->TYPE>end_value) {fix_bound=bound->TYPE; 
                                       bound->TYPE=end_value;}
      else if (bound->TYPE<end_value) {
         sav_end_value=end_value; 
         goto end_label;
      }
      
      sav_end_value=end_value;
      /*first test*/
      /*no link to first circuit_foundins--> no use to continue*/
      if (begin_value+how_far>Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env)) { 
         goto end_label;
      }   
            
     
      /*try a header: the highest to the lowest*/
      for ( ; begin_foundins!=which_link->AFTER/*no use to control all*/; 
           begin_foundins=begin_foundins->AFTER) {
         
         
         env->TYPE=begin_value;     /* 'i' */
         bound->TYPE=end_value;     /* 'n' */ 
         
         /*first test*/
         if (how_far<0) {
            avt_errmsg(GNS_ERRMSG, "081", AVT_FATAL, begin_foundins?begin_foundins->LOINS->INSNAME:NULL,which_link->LOINS->INSNAME,sav_begin_foundins->LOINS->INSNAME);
            /*fprintf(stderr,"recursive_CHAIN: out of bounds with %s and %s started from %s\n",
            begin_foundins?begin_foundins->LOINS->INSNAME:NULL,
            which_link->LOINS->INSNAME,sav_begin_foundins->LOINS->INSNAME);*/
            EXIT(3);
         }
         /*no link to first circuit_foundins--> no use to continue*/
         if (env->TYPE+how_far>Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env))
         {break;}
         
         /*search all chain*/
         for (try_foundins=begin_foundins; ; ) {
            if (try_foundins==NULL) {
             avt_errmsg(GNS_ERRMSG, "082", AVT_FATAL, env->TYPE,Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env));
             /*fprintf(stderr,"recursive_CHAIN: computing error on NULL pointer index=%ld end=%d\n",
             env->TYPE,Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env));*/
              EXIT(3);
            }
            
            /*recursive and iterative search(multiple for) */
            match=recursive_FOR(tree->NEXT->NEXT->NEXT->NEXT, 
                         try_foundins->LOWER?try_foundins->LOWER:try_foundins,
                         modelins, 
                         try_foundins!=which_link || !offsets?NULL:offsets->NEXT
                         , env);
            
            if (!match) {
               Backward();
               Protect();
               if (fix_bound) break;      /*move size forbidden*/
               bound->TYPE--;             /* 'n' = 'n' - 1 */
               /*no link to first circuit_foundins--> no use to continue*/
               if (env->TYPE+how_far>Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env))
               {break;}
               try_foundins=begin_foundins;  /*play again*/
               env->TYPE=begin_value;   /* 'i' put to zero */
               continue;                  /*go directly to for*/
            }   
            
            env->TYPE++;            /* 'i' = 'i' + 1 */
            /* 'i' > limit loop */
            if (env->TYPE>Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env)) {break;}
            try_foundins=try_foundins->AFTER;   /*play next stage*/
               
         }
         
         if (match) break;       /*Hurray! Generic match perfectly*/
         
         /*limit reached: change the beginnig of FOR*/
         if (fix_bound) {     /*forbidden to change the size*/
            fix_bound--;      /*if chain is longer than limit-->deplace window*/
            if (fix_bound<end_value) break;  /*window has reached limit*/
         }
         else end_value--;   /*try with a lower size*/    /* n = n - 1 */
         how_far--;     /*if you move the beginning you move also distances*/
     }   
     
     
end_label:
/*      
      if (sav_begin_foundins->LOWER) 
    gen_printf(0,"\non a higher level...\n");
      else gen_printf(0,"\t");
*/
      /*environment*/
      if (match) {
    chain_list *cl;
         Reduce();
         bound->TYPE=end_value;
     for (cl=current_variable_values;cl!=NULL && (long)cl->DATA!=end_value; cl=cl->NEXT) ;
     if (cl==NULL) current_variable_values=addchain(current_variable_values, (void *)(long)end_value);
     current_variable_name=(char *)bound->DATA;

         if (env->TYPE-begin_value==1)
       {
         gen_printf(0,"only %ld unit match from %s\n", env->TYPE-begin_value,sav_begin_foundins->LOINS->INSNAME);
       }
     else
       {
         gen_printf(0,"%ld units really match from %s\n", env->TYPE-begin_value,sav_begin_foundins->LOINS->INSNAME);
       }
      }   
      else {
         Backward();
         bound->TYPE=sav_end_value;
     gen_printf(0,"no unit match from %s\n", sav_begin_foundins->LOINS->INSNAME);
      }
      
//      if (sav_begin_foundins->LOWER) gen_printf(0,"\n");
         
      /*erase local var 'i' */
      p=env;
      env=env->NEXT;
      p->NEXT=NULL;/* break recursivity*/
      freeptype(p);
      return match;
}


/****************************************************************************/
/*this is for a loop where the order is without importance                  */
/****************************************************************************/
static int recursive_CHOICE(tree_list* tree, foundins_list *begin_foundins, loins_list *modelins, ptype_list *offsets, ptype_list *env)
{
   ptype_list *bound,*p;
   int match;
   char *local_ident,*bound_ident;
   foundins_list *which_link/*instance link to loop*/,*try_foundins;
   long sav_end_value,begin_value,end_value;
   int fix_bound=0;   /*flag to know if bound 'n' is already defined*/
   int one_time_match,is_link;

      match=0;
      Protect();
      
      if (offsets) {
         which_link = (foundins_list*) offsets->DATA;   /*just after*/
      }
      else {   /*no offset because the link is the head*/
         which_link = begin_foundins;   /*just after */
      }
      
      /*local environment  ==> 'i' */
      local_ident=getname(tree->NEXT);
      env=addptype(env,UNDEF,local_ident);
      
      /*entity environment ==> 'n' */
      bound_ident=Getname_Exp_VHDL(tree->NEXT->NEXT->NEXT);
      for (bound=env; bound; bound=bound->NEXT) 
         if (bound->DATA==bound_ident) break;
      if (!bound) {
         avt_errmsg(GNS_ERRMSG, "028", AVT_FATAL, bound_ident);
         //fprintf(stderr,"recursive_CHOICE: Variable %s not defined\n",bound_ident);
         EXIT(1);
      }  
      begin_value=Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      /* try the lowest predicate*/
      end_value=bound->TYPE;
      Solve_Equ_VHDL(tree->NEXT->NEXT->NEXT,
               begin_foundins->SIZE+begin_value-1,env);
      if (end_value==UNDEF) end_value=bound->TYPE;
      else if (bound->TYPE>end_value) {fix_bound=bound->TYPE; 
                                       bound->TYPE=end_value;}
      else if (bound->TYPE<end_value) {
         sav_end_value=end_value;
         goto label_end;
      }
      
      sav_end_value=end_value;
     
         one_time_match=0;
         is_link=0;
         
         env->TYPE=begin_value;     /* 'i' */
         bound->TYPE=end_value;     /* 'n' */ 
         
         /*no link to first circuit_foundins--> no use to continue*/
         if (env->TYPE>Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env))
         goto label_end;
         
         /*search all chain*/
         for (try_foundins=begin_foundins; ; ) {
            if (try_foundins!=NULL) {
               if (try_foundins->VISITED==modelins) 
               {
                  if (try_foundins==which_link) is_link=1;
                  try_foundins=try_foundins->AFTER; 
                  continue;
               }
            
               /*recursive and iterative search(multiple for) */
               match=recursive_FOR(tree->NEXT->NEXT->NEXT->NEXT, 
                         try_foundins->LOWER?try_foundins->LOWER:try_foundins,
                         modelins, 
                         which_link!=try_foundins || !offsets?NULL:offsets->NEXT
                         , env);
               if (match) {
                  if (try_foundins==which_link) is_link=1;
                  env->TYPE++;            /* 'i' = 'i' + 1 */
                  /* 'i' > limit loop */
                  if (env->TYPE>Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env)) 
                     {break;}
                  one_time_match=1;
               }
               try_foundins=try_foundins->AFTER;
               continue;
            }
            if (!match && !one_time_match) {
               
               if (fix_bound) break;      /*move size forbidden*/
               bound->TYPE--;             /* 'n' = 'n' - 1 */
               /*no link to first circuit_foundins--> no use to continue*/
               if (env->TYPE>Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env))
               {break;}
               try_foundins=begin_foundins;  /*play again*/
               env->TYPE=begin_value;   /* 'i' put to zero */
               Backward();
               Protect();
               continue;                  /*go directly to for*/
            }   
            is_link=0;
            one_time_match=0;
            try_foundins=begin_foundins;
         }

      if (!is_link) match=0;   

label_end:               

/*      if (begin_foundins->LOWER) 
    gen_printf(0,"\non a higher level...\n");
      else
    gen_printf(0,"\t");
*/
      /*environment*/
      if (match) {
    chain_list *cl;
         Reduce();
         bound->TYPE=end_value;
     for (cl=current_variable_values;cl!=NULL && (long)cl->DATA!=end_value; cl=cl->NEXT) ;
     if (cl==NULL) current_variable_values=addchain(current_variable_values, (void *)(long)end_value);
     current_variable_name=(char *)bound->DATA;

     if (env->TYPE-begin_value==1)
       gen_printf(2,"only %ld unit match from %s\n", env->TYPE-begin_value,begin_foundins->LOINS->INSNAME);
     else 
       gen_printf(2,"%ld units really match from %s\n", env->TYPE-begin_value,begin_foundins->LOINS->INSNAME);
      }   
      else {
         Backward();
         bound->TYPE=sav_end_value;
     gen_printf(0,"no unit match from %s\n", begin_foundins->LOINS->INSNAME);
      }

//      if (begin_foundins->LOWER) gen_printf(0,"\n");
         
      /*erase local var 'i' */
      p=env;
      env=env->NEXT;
      p->NEXT=NULL;/* break recursivity*/
      freeptype(p);
      return match;
}


/****************************************************************************/
/*PHASE 2: REDUCE:  set environment for loop: local and bound values        */
/****************************************************************************/
static  int recursive_FOR(tree_list* tree, foundins_list *begin_foundins, loins_list *modelins, ptype_list *offsets, ptype_list *env)
{
   int match;


   switch(TOKEN(tree)) {
   case GEN_TOKEN_NODE:
      return recursive_FOR(tree->DATA,begin_foundins,modelins,offsets,env);
      break;
   case GEN_TOKEN_EXCLUDE:/*pointer on a loins in lofig*/
      return recursive_FOR(tree->NEXT,begin_foundins,modelins,offsets,env);
      break;
   case GEN_TOKEN_LOINS:/*pointer on a loins in lofig*/
      if ((loins_list*)tree->DATA==modelins) {
        first_instance_in_for=begin_foundins;
        return Search_Phase(2,begin_foundins,modelins,env);
      }   
      else return 1;
      break;
   case GEN_TOKEN_IF:
      if (Eval_Bool_VHDL(tree->NEXT,env)) return 
         recursive_FOR(tree->NEXT->NEXT,begin_foundins,modelins,offsets,env);
      else return 1;
      break;
   case GEN_TOKEN_FOR:
      if (!tree || !begin_foundins) {
        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 43);
//        fprintf(stderr,"recursive_FOR: NULL pointer\n");
        EXIT(1);
      }
/*
      if (begin_foundins->LOWER) 
    gen_printf(0,"\non a lower level...\n");
      else 
    gen_printf(0,"\t");
*/
      /*control the beginning*/
      if (begin_foundins->FLAG==CHOICE) 
         match=recursive_CHOICE(tree,begin_foundins,modelins,offsets,env);
      else if (begin_foundins->FLAG==CHAIN) 
         match=recursive_CHAIN(tree,begin_foundins,modelins,offsets,env);
      else {
         avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 44);
         /*fprintf(stderr,"recursive_FOR: phase1 error on %s for flag %X %s\n",
            begin_foundins->LOINS->INSNAME,begin_foundins->FLAG,begin_foundins->LOINS->FIGNAME);*/
         EXIT(3);
      }
      return match;
      break;
   case ';':
      return recursive_FOR(tree->NEXT,begin_foundins,modelins,offsets,env)
      && recursive_FOR(tree->NEXT->NEXT,begin_foundins,modelins,offsets,env);
      break;
   default:
      Error_Tree("recursive_FOR",tree);
      EXIT(2); return 0;
   }
}


/****************************************************************************/
/*   for loop model to change recursive algorithm to en iterative           */
/****************************************************************************/
extern int continue_search_on(tree_list* tree, foundins_list *circuit_foundins, loins_list *modelins, ptype_list *env)
{
   foundins_list *found;
   ptype_list *offsets=NULL;
   long distance;     /*relative distance between circuit_foundins and head*/
   int match;
   foundins_list* last;  /*the last to check*/
   
   /*search the highest in hierarchy*/
   distance = 0;
   last=circuit_foundins;
   for (found=circuit_foundins; found; found=found->BEFORE) {
      if (found->VISITED) {
         freeptype(offsets);
         if (found->VISITED==modelins) return 1;
         else return 0;
      }
      for ( ; found->HIGHER; found=found->HIGHER) {
         if (found->VISITED) {
            freeptype(offsets);
            if (found->VISITED==modelins) return 1;
            else return 0;
         }
         offsets=addptype(offsets,distance,found);
         distance = 0;
         last=found->HIGHER;
      }   
      if (!found->BEFORE) break;
      distance++;
   }

   /*for the last*/
   offsets=addptype(offsets,distance,last);
   if (!found) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 45);
//      fprintf(stderr,"continue_search_on: NULL pointer\n");
      EXIT(1);
   }
   match = recursive_FOR(tree, found, modelins, offsets, env);   
   freeptype(offsets);
   return match;
}


static int occur=0;
/****************************************************************************/
/*           non-zero if circuit_foundins match perfectly with model        */
/****************************************************************************/
static inline int begin_search_on(foundins_list *circuit_foundins, loins_list* model_ins, ptype_list *env)
{
   ptype_list *p;
   int match;
   tree_list *tree;
  
   if (!circuit_foundins || !model_ins) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 46);
      //fprintf(stderr,"begin_search_on: NULL pointer\n");
      EXIT(1);
   }
   
   if (circuit_foundins->LOINS->FIGNAME!=model_ins->FIGNAME) {
          avt_errmsg(GNS_ERRMSG, "083", AVT_FATAL, circuit_foundins->LOINS->FIGNAME,model_ins->FIGNAME);
/*    fprintf(stderr,"begin_search_on: parameter discrepancy between %s and %s\n",
            circuit_foundins->LOINS->FIGNAME,model_ins->FIGNAME);*/
    EXIT(1);
   }
   /* for comparison with model*/
   p=getptype(model_ins->USER,GENERIC_PTYPE);
   if (!p) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 47);
      //fprintf(stderr,"begin_search_on: PTYPE not found\n");
      EXIT(1);
   }
   tree=(tree_list*)p->DATA;  

   if (TOKEN(tree)!=GEN_TOKEN_FOR) {
      if (circuit_foundins->FLAG!=0) {
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 48);
/*          fprintf(stderr,"begin_search_on: phase1 error on %s with flag %X\n",
           circuit_foundins->LOINS->INSNAME,circuit_foundins->FLAG);*/
          EXIT(3);
      }

      if (!GOTO_MODE)
    gen_printf(0,"Starting '%s'%c",circuit_foundins->LOINS->INSNAME,GEN_DEBUG_LEVEL>2?'\n':' ');

      fflush(GENIUS_OUTPUT);
      match=Search_Phase(2,circuit_foundins,model_ins,env);   
      if (match) 
    {
      occur++;
/*      if (occur % 100==0) 
        gen_printf(-1,"\tmatch (#%d)\n",occur);*/
      gen_printf(0,"\tmatch (#%d)\n",occur);
//      if (occur>1100) EXIT(4);
    }
      else 
    {
      gen_printf(0,"\tnot match\n");
      if (!ThereAreDecisionToChange()) circuit_foundins->FLAG=UNSELECT;
    }   
      return match;
   }
   else {
      if (!circuit_foundins->FLAG || circuit_foundins->FLAG==ALONE) return 0;
      if (circuit_foundins->FLAG!=CHOICE && circuit_foundins->FLAG!=CHAIN) {
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 49);
/*          fprintf(stderr,"begin_search_on: phase1 error on %s with flag %X(v2)\n",
           circuit_foundins->LOINS->INSNAME,circuit_foundins->FLAG);*/
          EXIT(3);
      }
      
      match = continue_search_on(tree,circuit_foundins,
                                 model_ins,env);
      if (!match && !ThereAreDecisionToChange()) circuit_foundins->FLAG=UNSELECT; 
      return match;
   }   
} 


/****************************************************************************/
/****************************************************************************/
static  void Sort_foundins(chain_list* for_foundins)
{
  foundins_list *first,*foundins,*low,*local,*local_high;
  foundins_list *begin_foundins,*top;
  
  if (!for_foundins) return;
  top=for_foundins->DATA;
  if (!top || !top->LOWER) return;
  
  for (begin_foundins=top; begin_foundins; begin_foundins=begin_foundins->NEXT) {
    if (!begin_foundins->LOWER || begin_foundins->FLAG!=CHOICE) continue;
    
    for (foundins=begin_foundins; foundins; foundins=foundins->AFTER) {
      if (!foundins->LOWER || foundins->FLAG!=CHOICE) {
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 50);
/*    fprintf(stderr,"Sort_foundins: computing error with flag %X on %s\n",
        foundins->FLAG,foundins->LOINS->INSNAME);*/
    EXIT(3);
      }   
      low=foundins->LOWER;
      /*change below*/
      for (local=low->AFTER; local; local=local->AFTER) {
    if (!local->HIGHER) {
      break;
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 51);
//      fprintf(stderr,"error %s\n",local->LOINS->INSNAME);
      EXIT(3);
    }
    local_high=local->HIGHER;
    local->HIGHER=NULL;
    if (local_high->AFTER) local_high->AFTER->BEFORE=local_high->BEFORE;
    if (local_high->BEFORE) local_high->BEFORE->AFTER=local_high->AFTER;
    /*local_high is now alone*/
    local_high->FLAG=UNSELECT;
    if (local_high->LOWER!=local) EXIT(4);
    local_high->LOWER=NULL;
    local_high->HIGHER=NULL;
    local_high->BEFORE=NULL;
    local_high->AFTER=NULL;
      }   
      
      first=NULL;
      /*change above*/
      for (local=low->BEFORE; local; local=local->BEFORE) {
    if (!local->HIGHER) {
      break;
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 52);
      //fprintf(stderr,"error %s\n",local->LOINS->INSNAME);
      EXIT(3);
    }
    local_high=local->HIGHER;
    local->HIGHER=NULL;
    if (local_high->AFTER) local_high->AFTER->BEFORE=local_high->BEFORE;
    if (local_high->BEFORE) local_high->BEFORE->AFTER=local_high->AFTER;
    /*local_high is now alone*/
    local_high->FLAG=UNSELECT;
    if (local_high->LOWER!=local) EXIT(4);
    local_high->LOWER=NULL;
    local_high->HIGHER=NULL;
    local_high->BEFORE=NULL;
    local_high->AFTER=NULL;
    /*memorize the first*/
    first=local;
      }
      
      /*change myself(my position particularly)*/
      if (first) {
    /*remove myself*/
    if (low->AFTER) low->AFTER->BEFORE=low->BEFORE;
    if (low->BEFORE) low->BEFORE->AFTER=low->AFTER;
    /*put at the first*/
    first->BEFORE=low;
    low->AFTER=first;
    low->BEFORE=NULL;
    low->SIZE=first->SIZE;
      }
    }
    
    for (foundins=begin_foundins->BEFORE; foundins; foundins=foundins->BEFORE) {
      if (!foundins->LOWER || foundins->FLAG!=CHOICE) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 53);
/*    fprintf(stderr,"Sort_foundins: computing error with flag %X on %s\n",
        foundins->FLAG,foundins->LOINS->INSNAME);*/
    EXIT(3);
      }   
      low=foundins->LOWER;
      /*change below*/
      for (local=low->AFTER; local; local=local->AFTER) {
    if (!local->HIGHER) {
      break;
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 54);
//      fprintf(stderr,"error %s\n",local->LOINS->INSNAME);
      EXIT(3);
    }
    local_high=local->HIGHER;
    local->HIGHER=NULL;
    if (local_high->AFTER) local_high->AFTER->BEFORE=local_high->BEFORE;
    if (local_high->BEFORE) local_high->BEFORE->AFTER=local_high->AFTER;
    /*local_high is now alone*/
    local_high->FLAG=UNSELECT;
    if (local_high->LOWER!=local) EXIT(4);
    local_high->LOWER=NULL;
    local_high->HIGHER=NULL;
    local_high->BEFORE=NULL;
    local_high->AFTER=NULL;
      }   
      
      first=NULL;
      /*change above*/
      for (local=low->BEFORE; local; local=local->BEFORE) {
    if (!local->HIGHER) {
      break;
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 55);
//      fprintf(stderr,"error %s\n",local->LOINS->INSNAME);
      EXIT(3);
    }
    local_high=local->HIGHER;
    local->HIGHER=NULL;
    if (local_high->AFTER) local_high->AFTER->BEFORE=local_high->BEFORE;
    if (local_high->BEFORE) local_high->BEFORE->AFTER=local_high->AFTER;
    /*local_high is now alone*/
    local_high->FLAG=UNSELECT;
    if (local_high->LOWER!=local) EXIT(4);
    local_high->LOWER=NULL;
    local_high->HIGHER=NULL;
    local_high->BEFORE=NULL;
    local_high->AFTER=NULL;
    /*memorize the first*/
    first=local;
      }
      
      /*change myself(my position particularly)*/
      if (first) {
    /*remove myself*/
    if (low->AFTER) low->AFTER->BEFORE=low->BEFORE;
    if (low->BEFORE) low->BEFORE->AFTER=low->AFTER;
    /*put at the first*/
    first->BEFORE=low;
    low->AFTER=first;
    low->BEFORE=NULL;
    low->SIZE=first->SIZE;
      }
    }
  }
  Sort_foundins(for_foundins->NEXT);
}
 
/****************************************************************************/
/* PHASe 1: SHIFT: take the maximum of instances in model if match the loop */
/****************************************************************************/
//#define LLEV -1
#define LLEV 2
extern int nbget, nbhit;
char *FOR_VARIABLE;
static inline void Search_Generic_FOR(foundins_list *foundins, loins_list *model_ins, ptype_list *env)
{
   chain_list *next_generics;
   int count=0,not_the_first;
   chain_list *l;
   foundins_list *join,*load,*pred,*new, *next;
   int orig_bound, bound, i;
   long oldenv[25];
   ptype_list *p;

   if (!foundins || !model_ins) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 56);
//      fprintf(stderr,"Search_Generic_FOR: NULL pointer\n");
      EXIT(1);
   }
   
   /*we considere the loins in circuit as if it was the first of the loop*/
   /*anyway if it isn't another will take for itself the result of below*/
   orig_bound=bound=env->TYPE;
   FOR_VARIABLE=(char *)env->DATA;
/*   for (p=env;p!=NULL; p=p->NEXT)
     {
       printf("- %s=%d \n",p->DATA,p->TYPE);
     }*/
   isparal_count=0;   
   isparal=IsFORparallal(model_ins, env, CURRENT_MODEL->LOFIG, &smartselect);

   if (isparal) gen_printf(3,"%sModel is a parallel search : using GENIUS fastmode\n",tabs);
   /*iterative method not to fulfill the function stack during computing*/
   for (load=foundins; load; load=next)
     {
       next=load->NEXT;
       env->TYPE=bound;
       for (p=env, i=0; p!=NULL; p=p->NEXT, i++) oldenv[i]=p->TYPE;
       nbget=nbhit=0;
       gen_printf(LLEV,"%s __ New Loop Search: start __ %s\n",tabs,load->LOINS->INSNAME);
//       printf("%d\n",load->FLAG);
       first_instance_in_for=load;
       signal_to_grab_instance_from=NULL;
       
       if (!load->ghost && load->FLAG!=CHAIN && load->FLAG!=CHOICE &&
       load->FLAG!=ALONE && 
       (!isparal || (isparal && load->FLAG!=SELECT)) &&
       Search_Phase(1,load,model_ins,env)) 
         {
           /*this instance match*/
     
           //     printf("is %d %s\n",load->FLAG,load->LOINS->INSNAME);
           if (!load->FLAG) load->FLAG=HEAD;
           /*so the next instance linked to this one isn't the beginning*/
           //     printf(" env->TYPE=%d ",env->TYPE);
           next_generics=get_Generic_foundins();

           if (isparal)
             {
               ptype_list *p;
               if ((p=getptype(load->LOINS->USER, GEN_LOOP_INDEX_PTYPE))==NULL)
                 p=load->LOINS->USER=addptype(load->LOINS->USER, GEN_LOOP_INDEX_PTYPE, NULL);
               p->DATA=(void *)(long)env->TYPE;
             }

           env->TYPE++;
           pred=load;
           not_the_first=0;
           //     printf("hit=%d get=%d ",nbhit,nbget);
           nbhit=0;

           gen_printf(LLEV,"%s __ New Loop Search: join __\n",tabs);
           for (l=next_generics; l; l=l->NEXT) 
             {
               join=(foundins_list*) l->DATA;
               if (join!=load) 
                 {
                   // printf("%d -> %d",load->FLAG, join->FLAG);
                   if (join->FLAG==SELECT) 
                     {
                       avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 57);
                //       fprintf(stderr,"Search_Generic_FOR: %s already flag SELECT\n", join->LOINS->INSNAME);
                       EXIT(2);
                     } 
                   if (Search_Phase(0,join,model_ins,env)) 
                     {
                       //gen_printf(LLEV,"*",tabs); fflush(GENIUS_OUTPUT);
                       for ( ; pred->AFTER; pred=pred->AFTER) if (load->BEFORE==pred) break;
                       join->BEFORE=pred;
                       pred->AFTER=join;
                       if (load->BEFORE==pred) 
                         {
                           join->AFTER=load;
                           load->BEFORE=join;
                         }   
                       pred=join;
                       //             printf("%d\n",join->FLAG);
                       if (join->FLAG==PRESEL) 
                         {
                           join->FLAG=CHOICE;
                           /*this one or another*/

                           if (!isparal && not_the_first) {join->FLAG=CHOICE;}
                           else 
                             {
                               join->FLAG=SELECT;            /*we don't know now*/
                               not_the_first=1; /*but the next one we have the choice*/
                             }   
                         }
                       else 
                         if (join->FLAG==PRESEL_HEAD) join->FLAG=SELECT_HEAD;
                         else
                           {
                             avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 58);
/*                             fprintf(stderr,"Search_Generic_FOR: forbidden flag %X for %s\n",
                                     join->FLAG, join->LOINS->INSNAME);*/
                             EXIT(3);
                           }
                       if (isparal)
                         {
                           ptype_list *p;
                           if ((p=getptype(join->LOINS->USER, GEN_LOOP_INDEX_PTYPE))==NULL)
                             p=join->LOINS->USER=addptype(join->LOINS->USER, GEN_LOOP_INDEX_PTYPE, NULL);
                           p->DATA=(void *)(long)env->TYPE;
                           env->TYPE++;
                           count++;
                         }
                       nbhit++;
                       //             printf("->%d\n",join->FLAG);
                     }
                   else 
                     {
                       //             gen_printf(LLEV,".",tabs); fflush(GENIUS_OUTPUT);
                       if (join->FLAG==PRESEL) {join->FLAG=0;}
                       else if (join->FLAG==PRESEL_HEAD) join->FLAG=HEAD;
                       else 
                         {
                           avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 59);
/*                           fprintf(stderr,"Search_Generic_FOR: forbidden flag %X for %s\n",
                                   join->FLAG, join->LOINS->INSNAME);*/
                           EXIT(3);
                         }
                     }
                 }
             }
           //     printf(" phit=%d\n",nbhit);
           if (isparal)
             {
               int end_value=env->TYPE-1, old=isparal_bound->TYPE;
               ptype_list *hider;
               chain_list *cl;
               isparal_bound->TYPE=UNDEF;
               Solve_Equ_VHDL(isparal_tree->NEXT->NEXT->NEXT, end_value/*-orig_bound*/,env);
               if (isparal_bound->TYPE==UNDEF) isparal_bound->TYPE=env->TYPE-1;

               hider=env; 
               env=env->NEXT; // remove local variable
               gen_printf(0,"HEAD %s = ",load->LOINS->INSNAME);
               count=end_value-orig_bound+1;
               if (count==1) gen_printf(0,"%d unit\n",count);
               else gen_printf(0,"%d units\n",count);
               Build_loins(CURRENT_MODEL, env);
               env=hider;
               isparal_count++;
               next=getfoundins(model_ins->FIGNAME);
               if (next && next->ghost) EXIT(54);
               isparal_bound->TYPE=old;

               for (cl=current_variable_values;cl!=NULL && (long)cl->DATA!=count; cl=cl->NEXT) ;
               if (cl==NULL) current_variable_values=addchain(current_variable_values, (void *)(long)(end_value-bound+1));
               current_variable_name=(char *)isparal_bound->DATA;
             }

           Remove(CURRENT_MODEL);
           free_Generic_foundins();
         }
       freechain(signal_to_grab_instance_from);
       for (p=env, i=0; p!=NULL; p=p->NEXT, i++) p->TYPE=oldenv[i];
     }   

   if (!isparal)
     {
       /*now we count the generic line*/
       for (load=foundins ; load; load=load->NEXT) {
     if (load->FLAG==HEAD) {
       gen_printf(0,"HEAD %s = ",load->LOINS->INSNAME);
       fflush(GENIUS_OUTPUT);
       
       count=0;
       if (load->BEFORE) { 
         avt_errmsg(GNS_ERRMSG, "084", AVT_FATAL, load->LOINS->INSNAME);
         /*fprintf(stderr,"Search_Generic_FOR: infinite loop on %s\n",
             load->LOINS->INSNAME);*/
         EXIT(1);
       }       
       for (join=load, i=orig_bound; join; join=join->AFTER, i++) {
         if (join->FLAG!=SELECT && join->FLAG!=HEAD) {
           avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 60);
/*           fprintf(stderr,"Search_Generic_FOR: illegal flag %X on %s\n",
               join->FLAG,join->LOINS->INSNAME);*/
           EXIT(3);
         }
         join->FLAG=CHAIN;
         count++;
         
         if ((p=getptype(join->LOINS->USER, GEN_LOOP_INDEX_PTYPE))==NULL)
           p=join->LOINS->USER=addptype(join->LOINS->USER, GEN_LOOP_INDEX_PTYPE, NULL);
         p->DATA=(void *)(long)i;
       }
       load->SIZE=count;
       
       if (count==1) gen_printf(0,"%d unit\n",count);
       else gen_printf(0,"%d units\n",count);
       
       /*create a higher level*/
       new=new_foundins(FOUNDINS_FOR->DATA,load->LOINS,NULL);
       FOUNDINS_FOR->DATA=new;/*save the head*/
       load->HIGHER=new;
       new->LOWER=load;
     }
       } 
       
       /*we count the generic cycle*/
       for (load=foundins; load; load=load->NEXT) {
     if (load->FLAG==SELECT_HEAD) {
       /*cut the loop*/
       load->BEFORE->AFTER=NULL;
       
       gen_printf(0,"CYCLE %s = ",load->LOINS->INSNAME);
       fflush(GENIUS_OUTPUT);
       
       count=0;
       for (join=load,i=orig_bound; join; join=join->AFTER, i++) {
         if (join->FLAG!=CHOICE && join->FLAG!=SELECT && 
         join->FLAG!=SELECT_HEAD) {
           avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 61);
/*           fprintf(stderr,"Search_Generic_FOR: illegal flag %X on %s\n",
               join->FLAG,join->LOINS->INSNAME);*/
           EXIT(3);
         }
         join->FLAG=CHOICE;
         count++;
         
         if ((p=getptype(join->LOINS->USER, GEN_LOOP_INDEX_PTYPE))==NULL)
           p=join->LOINS->USER=addptype(join->LOINS->USER, GEN_LOOP_INDEX_PTYPE, NULL);
         p->DATA=(void *)(long)i;
         
         /*create a higher level*/
         new=new_foundins(FOUNDINS_FOR->DATA,join->LOINS,NULL);
         FOUNDINS_FOR->DATA=new;/*save the head*/
         new->LOWER=join;
         join->HIGHER=new;
       }
       /*cut the loop*/
       load->BEFORE=NULL;
       load->SIZE=count;
       
       if (count==1) gen_printf(0,"%d unit\n",count);
       else gen_printf(0,"%d units\n",count);
     }
       } 
     }       
}   
   

/****************************************************************************/
/* return a list of instances contained in FOR instructions, list put on top*/
/****************************************************************************/
static inline loins_list* iterative_FOR(tree_list* tree, ptype_list *env)
{
  int index_value,level;
  char *index_ident,*bound_ident;
  foundins_list *foundins,*begin_foundins;
  static int inloop=0;
//  const int X=10;
  ptype_list *bound,*q;
  loins_list *ins_model;

   if (!tree) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 62);
//     fprintf(stderr,"iterative_FOR: NULL pointer\n");
     EXIT(1);
   }
   
   switch(TOKEN(tree)) {
   case GEN_TOKEN_NODE:
      return iterative_FOR(tree->DATA,env);
      break;
   case GEN_TOKEN_EXCLUDE:/*pointer on a loins in lofig*/
      return iterative_FOR(tree->NEXT,env);
      break;
   case GEN_TOKEN_LOINS:/*pointer on a loins in lofig*/
      /*if we are in a loop, memorize*/
      ins_model=(loins_list*)tree->DATA;
      if (inloop) {
         ptype_list *var,*del;
         if (mbk_istransn(ins_model->FIGNAME) || mbk_istransp(ins_model->FIGNAME)) {
            avt_errmsg(GNS_ERRMSG, "085", AVT_FATAL);
            //fprintf(stderr,"iterative_FOR: transistor in center of loop forbidden\n");
            EXIT(3);
         }
         var=env;

         begin_foundins=getfoundins(ins_model->FIGNAME);

         FOUNDINS_FOR=addchain(FOUNDINS_FOR,NULL);
         if (begin_foundins) Search_Generic_FOR(begin_foundins,ins_model,var);

         if (!FOUNDINS_FOR->DATA && !isparal) { 
       gen_printf(1,"No unit match\n");
       LOOP_ERR=1;
         }   
         else for (level=inloop-1; level>0; level--) {
            var=var->NEXT;
            env=addptype(env,var->TYPE,var->DATA);
            FOUNDINS_FOR=addchain(FOUNDINS_FOR,NULL);
            if (FOUNDINS_FOR->NEXT->DATA) {
          gen_printf(1,"on a higher level...\n");
          Search_Generic_FOR(FOUNDINS_FOR->NEXT->DATA,ins_model,env);
          if (!FOUNDINS_FOR->DATA) { 
        gen_printf(1,"No unit match\n");
        LOOP_ERR=1;
        break;
          }   
            }
            del=env;
            env=env->NEXT;
            del->NEXT=NULL;
            freeptype(del);
         }
         /* do not worry about the last level*/
         for (foundins=FOUNDINS_FOR->DATA; foundins; foundins=foundins->NEXT) {
            foundins->LOWER->HIGHER=NULL;
            foundins->LOWER=NULL;
         }
         for (foundins=begin_foundins; foundins; foundins=foundins->NEXT) {
            q=getptype(foundins->LOINS->USER,GEN_FOUNDINS_PTYPE);
            if (!q) {
               avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 63);
//               fprintf(stderr,"iterative_FOR: PTYPE not found\n");
               EXIT(1);
            }
            q->DATA=foundins;
         }   

         /*sort now as best as you can*/
         Sort_foundins(FOUNDINS_FOR->NEXT);

         return ins_model;
      }
      return NULL;
      break;
   case GEN_TOKEN_IF:
      return iterative_FOR(tree->NEXT->NEXT,env);
      break;
   case GEN_TOKEN_FOR:
      inloop++;
      /*entity environment ==> 'n' */
      bound_ident=Getname_Exp_VHDL(tree->NEXT->NEXT->NEXT);
      if (Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env)!=UNDEF ) 
    {
       avt_errmsg(GNS_ERRMSG, "086", AVT_FATAL, FILE_NAME(tree),LINE(tree),getname(tree->NEXT));
/*      fprintf(stderr,"%s:%d: GENIUS ignored expansed For driven by '%s'\n",
          FILE_NAME(tree),LINE(tree),getname(tree->NEXT));*/
      /*break;*/EXIT(1);   
    }
      for (bound=env; bound; bound=bound->NEXT) 
         if (bound->DATA==bound_ident) break;
      if (!bound) {
         avt_errmsg(GNS_ERRMSG, "035", AVT_FATAL, bound_ident);
         //fprintf(stderr,"iterative_FOR: Variable %s not defined\n",bound_ident);
         EXIT(1);
      }  
      /*local environment 'i' */
      index_value=Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      index_ident=getname(tree->NEXT);
      env=addptype(env,index_value,index_ident);
      /*value of 'n' */
      bound->TYPE=100000;/*(if index_value is 0) bound != index_value*/
      isparal_bound=bound;
      isparal_tree=tree;
      /*iterations*/
//      printf("%s=%d %s=%d\n",index_ident, index_value, bound_ident, bound->TYPE);
      ins_model=iterative_FOR(tree->NEXT->NEXT->NEXT->NEXT,env);
      /*environment*/
      bound->TYPE=UNDEF;
      q=env;
      env=env->NEXT;
      q->NEXT=NULL;/* break recursivity*/
      freeptype(q);
      inloop--;
      return ins_model;
      break;
   case ';':
      ins_model=iterative_FOR(tree->NEXT,env);
      if (!ins_model) return iterative_FOR(tree->NEXT->NEXT,env);
      iterative_FOR(tree->NEXT->NEXT,env);
      return ins_model;
      break;
   default:
      Error_Tree("iterative_FOR",tree);
      EXIT(2); return NULL;
   }
}


/****************************************************************************/
/*             return the loins which is the deepest in loops               */
/****************************************************************************/
static loins_list* choose_loins(tree_list* tree, char *rarest, ptype_list *env)
{
   loins_list *loins;
   int for_end;

   if (!tree) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 64);
//     fprintf(stderr,"choose_loins: NULL pointer\n");
     EXIT(1);
   }
   switch(TOKEN(tree)) {
   case GEN_TOKEN_NODE:
      return choose_loins(tree->DATA,rarest,env);
      break;
   case GEN_TOKEN_EXCLUDE:/*pointer on a loins in lofig*/
      return choose_loins(tree->NEXT,rarest,env);
      break;
   case GEN_TOKEN_LOINS:/*pointer on a loins in lofig*/
     loins=(loins_list*)tree->DATA;
     if (((long)getptype(loins->USER, GENIUS_LOINS_FLAGS_PTYPE)->DATA & LOINS_IS_WITHOUT)!=0)
       return NULL;
     // a mathed without loins can not reduce results
     return (loins_list*)tree->DATA;
     break;
   case GEN_TOKEN_IF:  /*at this state of development if aren't supported by searching*/
      avt_errmsg(GNS_ERRMSG, "029", AVT_FATAL, FILE_NAME(tree),LINE(tree));
      //fprintf(stderr,"%s:%d: IF forbidden for GENIUS\n",FILE_NAME(tree),LINE(tree));
      EXIT(1);
      return choose_loins(tree->NEXT->NEXT,rarest,env);
      break;
   case GEN_TOKEN_FOR:
     for_end=Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env);
     if (for_end==UNDEF)
       {
     return choose_loins(tree->NEXT->NEXT->NEXT->NEXT,rarest,env);
       }
     return NULL;
      break;
   case ';':
      if (SeekToken_Tree(tree->NEXT,GEN_TOKEN_FOR)) {
/*         if (SeekToken_Tree(tree->NEXT->NEXT,GEN_TOKEN_FOR)) {
            fprintf(stderr,"%s:%d: put this FOR in another model\n",
            FILE_NAME(tree->NEXT->NEXT),LINE(tree->NEXT->NEXT));
            EXIT(3);
         }*/
         loins=choose_loins(tree->NEXT,rarest,env);
         if (loins!=NULL) return loins;
      }   
      if (SeekToken_Tree(tree->NEXT->NEXT,GEN_TOKEN_FOR)) 
        {
          loins=choose_loins(tree->NEXT->NEXT,rarest,env);
          if (loins!=NULL) return loins;
        }
      loins=choose_loins(tree->NEXT->NEXT,rarest,env);
      if (loins!=NULL && loins->FIGNAME==rarest) return loins;
      return choose_loins(tree->NEXT,rarest,env);
      break;
   default:
      Error_Tree("choose_loins",tree);
      EXIT(2); return NULL;
   }
}


/****************************************************************************/
/*    Search occurences of model contained in circuit and maybe begin from  */
/*   the rarest                                                             */
/****************************************************************************/

chain_list *expansedloins, *originalloins;

extern int genFindInstances(lofig_list *circuit, model_list *model, char *select_model)
{
   int count,countFOR,launchexpansion=0,oldcount;
   lofig_list *lofig;
   foundins_list *foundins,*foundins_start;
   ptype_list *p,*env;
   chain_list *l,*allforvars, *cl;
   loins_list *model_ins,*best_loins;
   tree_list *tree;
   int done;

   chain_list *allloins;
   
   if (!model || !select_model || !circuit) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 65);
//      fprintf(stderr,"genFindInstances: NULL pointer\n");
      EXIT(1);
   }   

   CURRENT_MODEL=model;
   lofig=model->LOFIG;

   GENIUS_TO_SPY=modelradical(model->NAME);

   env=NULL;
   for (l=model->VAR;l;l=l->NEXT) env=addptype(env,UNDEF,l->DATA);  

   /* assign the generic variables */
   allforvars=NULL;
   search_FOR(model->GENERIC, &allforvars, &launchexpansion);

/*   gen_printf(1,"for vars :");
   for (l=allforvars;l!=NULL;l=l->NEXT)
     gen_printf(1,"%s ",(char *)l->DATA);
   gen_printf(1,"\n");
*/
   allloins=NULL;
   getallloins(model->GENERIC, &allloins);
   countFOR=CheckGenericValues(allloins,allforvars,env);

   freechain(allloins);
   
   count=oldcount=0;

   while (GenerateNextValueConfiguration(env,allforvars))
     {

       expansedloins=originalloins=NULL;
       
       if (countFOR || launchexpansion)
     {
       ExpandFOR(model->GENERIC, circuit, env, &expansedloins, &originalloins, "", 0, 1);
       
       // now we really add the new loins to the lofig
       FinishExpansion(originalloins);
     }
       
       best_loins=choose_loins(model->GENERIC, select_model, env);
       // prechoose
       if (best_loins==NULL)
     for (cl=expansedloins;cl!=NULL;cl=cl->NEXT)
       {
         if ((best_loins=(loins_list *)cl->DATA)->FIGNAME==select_model) break;
       }
       
       /*beginning of real search*/
       p=getptype(best_loins->USER,GENERIC_PTYPE);
       if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 66);
//     fprintf(stderr,"genFindInstances: PTYPE not found\n");
     EXIT(1);
       }
       tree=(tree_list*) p->DATA;

       for (foundins=getfoundins(best_loins->FIGNAME); foundins; foundins=foundins->NEXT) 
     {
       foundins->FLAG=0;
       foundins->SIZE=0;
       foundins->NOT_MATCH=0;
       foundins->AFTER=NULL;
       foundins->BEFORE=NULL;
       foundins->LOWER=NULL;
       if (foundins->HIGHER) 
         {
           p=getptype(foundins->LOINS->USER,GEN_FOUNDINS_PTYPE);
           if (!p) 
         {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 67);
//           fprintf(stderr,"genFindInstances: NULL pointer\n");
           EXIT(1);
         }
           p->DATA=foundins;
           foundins->HIGHER=NULL;
         }
       foundins->VISITED=0;
     }
       
       gen_printf(0,"\nPhase 1\n");
       LOOP_ERR=0;

       isparal=0;
       smartselect=0;

       model_ins=iterative_FOR(tree, env);             

       if (LOOP_ERR) 
     {
       /*clean memory*/
       foundins_list *tmp=NULL;
       //       freeptype(env);
       
       for (l=FOUNDINS_FOR; l; l=l->NEXT) 
         {
           tmp=(foundins_list*)append((chain_list *)l->DATA,(chain_list *)tmp);
         }
       
       free_foundins(tmp,0);
       freechain(FOUNDINS_FOR);
       FOUNDINS_FOR=NULL;
       //       return 0;
     }
       else
     {
       if (!isparal)
         {
           if (best_loins!=NULL)
         {
           if (best_loins->FIGNAME==select_model)
             model_ins=best_loins;
         }
           if (!model_ins) {
         for (model_ins=lofig->LOINS; model_ins; model_ins=model_ins->NEXT) 
           if (model_ins->FIGNAME==select_model) break;
           }
           if (!model_ins) {
            avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 68);
/*         fprintf(stderr,"genFindInstances: start loins not found for model %s\n",
             model->NAME);*/
         EXIT(1);
           }   
           
           gen_printf(0,"\nPhase 2\n");
           ClearAllDecisions();
           ResetBackTrackCounter();
           occur=0;
           /*search around found instances for the others loins of lofig*/
           for (foundins=getfoundins(model_ins->FIGNAME); foundins; ){
         if (!foundins->NOT_MATCH && foundins->FLAG!=UNSELECT) {
           //SetVariableToFindtoUNDEF(env);
           
           done=0;
           do 
             {
               for (p=env; p; p=p->NEXT) p->TYPE=UNDEF;
               //extern            int eeadd, eeremove;

               if (begin_search_on(foundins, model_ins, env)) {
             /*erase all marks and lower loins and build the new one*/
             ProcessSignalStatistics();
             Build_loins(model, env/*generic values*/);   
             count++;   /*count occurrences of lofig*/
             foundins=getfoundins(model_ins->FIGNAME); /*it is a new beginning*/ 
             if (foundins && foundins->ghost) EXIT(54);
             Remove(model);
             intermediate_finish_erase_foundins();
             done=1;
               }
               else
             {
               if (!ThereAreDecisionToChange()) break;
               ResetBackTrackCounter();
               CleanLofigForWithouts(model);
               gen_printf(0,"Back to last wrong decision ...");
               GOTO_MODE=1;
             }
               /*           printf("add=%d remove=%d\n",eeadd,eeremove);
                       if (eeadd!=eeremove) EXIT(5);
                       eeadd=0;eeremove=0;*/
             } while (done==0);
           
           
           ClearAllDecisions();
           ResetBackTrackCounter();
           CleanLofigForWithouts(model);
           
           if (!done)
             {
               foundins_start=foundins;              /*for the loop*/
               Remove(model);
               for (foundins=foundins_start; foundins; foundins=foundins->BEFORE)
             foundins->NOT_MATCH=1;
               for (foundins=foundins_start; foundins; foundins=foundins->AFTER)
             foundins->NOT_MATCH=1;
               foundins=foundins_start->NEXT;   
             }   
         }
         else foundins=foundins->NEXT;
           }
         }
       else
         {
           gen_printf(0,"\nPhase 2 skipped\n");
           count=isparal_count;
         }

       ClearQuickMatchChanceHashTables();
       finish_erase_foundins(); // Zinaps optim 2
       
       /*clean memory*/
       {
         foundins_list *tmp=NULL;
         
         for (l=FOUNDINS_FOR; l; l=l->NEXT) 
           {
         tmp=(foundins_list*)append((chain_list *)l->DATA,(chain_list *)tmp);
           }
         
         free_foundins(tmp,0);
       }
       
       freechain(FOUNDINS_FOR);
       FOUNDINS_FOR=NULL;
     }
       
       if (countFOR || launchexpansion)
     {
       // we put back the lofig as it was before the expansion
       UndoExpansion(expansedloins);
     }

       freechain(originalloins);
       freechain(expansedloins);
       gen_printf(3,"----> %d units\n",count-oldcount);
       oldcount=count;
     }


   CleanUPMarkEA(model->LOFIG);

   freechain(allforvars);
   FreeConfigurationsData();
   freeptype(env);

   return count; 
   
}


