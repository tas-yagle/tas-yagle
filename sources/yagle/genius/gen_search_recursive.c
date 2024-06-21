/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_search_recursive.c                                      */
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
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include MUT_H
#include MLO_H
#include MSL_H
#include FCL_H
#include API_H
#include AVT_H
#include "gen_env.h"
#include "gen_model_transistor.h"
#include "gen_execute_VHDL.h"
#include "gen_model_utils.h"
#include "gen_corresp.h"
#include "gen_search_utils.h"
#include "gen_search_iterative.h"
#include "gen_search_recursive.h"
#include "gen_symmetric.h"
#include "gen_optimizations.h"
#include "gen_display.h"


extern int level;


char *reason=NULL;
/* very special treatment for EXTERNAL connector*/
/* we have to memorize it and later to do it */ 
static int EXTERNAL_FLAG;                      /*flag*/
/*to cut loop in cyclic graph (lofigchain) we memorize the callers*/
static locon_list* CIRCUIT_LOCON;
static locon_list* MODEL_LOCON;
static losig_list *LAST_LOSIG=NULL;
static int MODEL_BIT;
/*not to go 2 times in the same way, we have a copy of lofigchain. an element*/
/*is put off if visited*/
chain_list* CIRCUIT_LOFIGCHAIN;
/*name of loins in instruction FOR in phase1*/
static int FOR_FLAG, atleastFOR_FLAG=0;         /*flag*/
static chain_list* GENERIC_CIRCUIT_FOUNDINS;
static chain_list* STACK_CIRCUIT_FOUNDINS;
static int PHASE;
static char* FIRST_MODEL_FIGNAME;
static char* FIRST_MODEL_INSNAME;
static int ABORT_SYMMETRY=0; // Force the abort of symmetry in case of "no way to match"
static int EMERGENCY_ABORT=0; // CIRCUIT_LOFIGCHAIN n'est plus valide
static loins_list *PREVIOUS_INS=NULL;
static ptype_list *GLOBAL_CURFOUNDINS_VAR=NULL;

/*to avoid warning for multiple recursions*/
static  int Is_Lotrs_Match(lotrs_list* lotrs, loins_list* model_ins,
                           ptype_list *env);
static  int Put_Loins_Flags(loins_list* circuit_ins, 
                            loins_list* model_ins, ptype_list *env, locon_list *from_connector);
static  int Is_Gensig_Match(gensigchain_list* gen, int bit_wanted, 
                            ptype_list* env);
                          


ptype_list *CopyPtypeValues(ptype_list *source)
{
  if (source==NULL) return NULL;
  return addptype(CopyPtypeValues(source->NEXT),source->TYPE,source->DATA);
}

void ReplacePtypeValues(ptype_list *dest, ptype_list *source)
{
  for (;dest!=NULL && source!=NULL;dest=dest->NEXT,source=source->NEXT) {dest->TYPE=source->TYPE;}
}


char *getreason(int code)
{
  static char temp[200];
  if (code==0)
    return "";
  if (reason!=NULL)
    sprintf(temp,", reason:%s.",reason);
  else
    strcpy(temp,"");
  reason=NULL;
  return temp;
}
/****************************************************************************/
/* calculate the index of locon in model. (index is always an expression)   */
/****************************************************************************/
static  int BitNumber_Losig(locon_list* circuit_con, locon_list* model_con,
                            ptype_list *env)
{
  int numbit,bit_wanted,low_src,high_src,low_dest;
  genconchain_list *gen;
  ptype_list *p;

  if (!model_con || !circuit_con) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 69);
//    fprintf(stderr,"BitNumber_Losig: NULL pointer\n");
    EXIT(1);
  }   

  p=getptype(model_con->USER,GENCONCHAIN);
  if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 70);
//    fprintf(stderr,"BitNumber_Losig: NULL pointer\n");
    EXIT(1);
  }   
  gen=(genconchain_list*)p->DATA;

  /* get bit number for model */
  if (circuit_con->TYPE=='T') bit_wanted=-1;
  else bit_wanted=fastindex(circuit_con);

  /*several signals*/
  if (gen->NEXT) {
    avt_errmsg(GNS_ERRMSG, "087", AVT_FATAL, model_con->NAME, ((loins_list *)model_con->ROOT)->INSNAME);
    /*fprintf(stderr,"BitNumber_Losig: several signals connected to connector %s of instance '%s'\n",
            model_con->NAME, ((loins_list *)model_con->ROOT)->INSNAME);*/
    EXIT(3);
  }
   
  if (TOKEN(gen->LOCON_EXP)==GEN_TOKEN_TO) {
    if (TOKEN(gen->LOSIG_EXP)!=GEN_TOKEN_TO && TOKEN(gen->LOSIG_EXP)!=GEN_TOKEN_DOWNTO) {
      /* a part of vector is connected to only one signal*/
    avt_errmsg(GNS_ERRMSG, "088", AVT_FATAL, model_con->NAME,(char*)gen->LOSIG->NAMECHAIN->DATA);
 /*     fprintf(stderr,"BitNumber_Losig: locon vector %s connected to one signal %s\n",
              model_con->NAME,(char*)gen->LOSIG->NAMECHAIN->DATA);*/
      EXIT(3);
    }   
    /* locon is a vector */
    low_src=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT,GLOBAL_CURFOUNDINS_VAR);
    high_src=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT->NEXT,GLOBAL_CURFOUNDINS_VAR);
    if (low_src<=bit_wanted && (high_src==UNDEF || high_src>=bit_wanted)) {
      /*low_dest is not really the low bound of vector (it could be)*/
//      low_dest=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT,env);
      numbit=UNDEF;
      if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_TO)
        {
          low_dest=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT,env);
          numbit=low_dest+bit_wanted-low_src;
        }
      else if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_DOWNTO)
        {
          low_dest=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT->NEXT,env);
          numbit=low_dest+high_src-bit_wanted;
        }
      return numbit;
    }
  }
  else if (TOKEN(gen->LOCON_EXP)==GEN_TOKEN_DOWNTO) {
    if (TOKEN(gen->LOSIG_EXP)!=GEN_TOKEN_TO && TOKEN(gen->LOSIG_EXP)!=GEN_TOKEN_DOWNTO) {
      /* a part of vector is connected to only one signal*/
      avt_errmsg(GNS_ERRMSG, "088", AVT_FATAL, model_con->NAME,(char*)gen->LOSIG->NAMECHAIN->DATA);
/*      fprintf(stderr,"BitNumber_Losig: locon vector %s connected to one signal %s\n",
              model_con->NAME,(char*)gen->LOSIG->NAMECHAIN->DATA);*/
      EXIT(3);
    }   
    /* locon is a vector */
    low_src=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT->NEXT,GLOBAL_CURFOUNDINS_VAR);
    high_src=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT,GLOBAL_CURFOUNDINS_VAR);
    if (low_src<=bit_wanted && (high_src==UNDEF || high_src>=bit_wanted)) {
      /*low_dest is not really the low bound of vector (it could be)*/
//      low_dest=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT,env);
      numbit=UNDEF;
      if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_TO)
        {
          low_dest=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT,env);
          numbit=low_dest-bit_wanted+high_src;
        }
      else if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_DOWNTO)
        {
          low_dest=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT->NEXT,env);
          numbit=low_dest+bit_wanted-low_src;
        }
      return numbit;
    }
  }
  else {
    if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_TO || TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_DOWNTO) {
      /* one locon connected to several signals is forbidden */
      avt_errmsg(GNS_ERRMSG, "089", AVT_FATAL, model_con->NAME);
/*      fprintf(stderr,"BitNumber_Losig: Connector '%s' with several signals\n",
              model_con->NAME);*/
      EXIT(2);
    }   
    /* locon is one bit */
    numbit=Eval_Exp_VHDL(gen->LOCON_EXP,GLOBAL_CURFOUNDINS_VAR);
    if (numbit==bit_wanted) {
      numbit=Eval_Exp_VHDL(gen->LOSIG_EXP,env);
      /* look at the signal */
      return numbit;
    }
  }

  gen_printf(3,"%s[FAILED] Connector '%s(%d)' does not exist\n",tabs,model_con->NAME,bit_wanted);
  return -2;
  EXIT(3);
}


/****************************************************************************/
/*                LEX&YACC:  increase level of Stack                        */
/****************************************************************************/
static  void Generic_protect()
{
  STACK_CIRCUIT_FOUNDINS=addchain(STACK_CIRCUIT_FOUNDINS,
                                  GENERIC_CIRCUIT_FOUNDINS);
  GENERIC_CIRCUIT_FOUNDINS=NULL;   
}


/****************************************************************************/
/*                LEX&YACC:  remove last Stack level                        */
/****************************************************************************/
static  void Generic_backward()
{
  chain_list *l,*del;
  foundins_list *foundins;
   
  if (!STACK_CIRCUIT_FOUNDINS) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 71);
//    fprintf(stderr,"Generic_backward: NULL pointer\n");
    EXIT(1);
  }
  
  /*unshift*/
  for (l=GENERIC_CIRCUIT_FOUNDINS; l; l=l->NEXT) {
    foundins=(foundins_list*)l->DATA;
    if (foundins->FLAG==PRESEL) foundins->FLAG=0;
    else if (foundins->FLAG==PRESEL_HEAD) foundins->FLAG=HEAD;
  }
  freechain(GENERIC_CIRCUIT_FOUNDINS);
   
  GENERIC_CIRCUIT_FOUNDINS=STACK_CIRCUIT_FOUNDINS->DATA;
  del=STACK_CIRCUIT_FOUNDINS;
  STACK_CIRCUIT_FOUNDINS=STACK_CIRCUIT_FOUNDINS->NEXT;
  del->NEXT=NULL;/*break recursivity*/
  freechain(del);
}


/****************************************************************************/
/*        LEX&YACC: put curent stack level with last stack level            */
/****************************************************************************/
static  void Generic_reduce()
{
  chain_list *l,*del;
   
  if (!STACK_CIRCUIT_FOUNDINS) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 72);
//    fprintf(stderr,"Generic_reduce: NULL pointer\n");
    EXIT(1);
  }
   
  for (l=GENERIC_CIRCUIT_FOUNDINS; l; l=l->NEXT) if (!l->NEXT) break;
  if (l) l->NEXT=STACK_CIRCUIT_FOUNDINS->DATA;
  else GENERIC_CIRCUIT_FOUNDINS=STACK_CIRCUIT_FOUNDINS->DATA;
  del=STACK_CIRCUIT_FOUNDINS;
  STACK_CIRCUIT_FOUNDINS=STACK_CIRCUIT_FOUNDINS->NEXT;
  del->NEXT=NULL;/*break recursivity*/
  freechain(del);
}


/****************************************************************************/
/*             for phase1 we need to know wich loins is in loop             */
/****************************************************************************/
foundins_list *first_instance_in_for;
chain_list *signal_to_grab_instance_from=NULL;
int this_count_taken;
int nbget=0, nbhit=0;


static  void memorize_gen(loins_list *ins)
{
  foundins_list *found;
  ptype_list *l;
   
  l=getptype(ins->USER,GEN_FOUNDINS_PTYPE);
  if (!l) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 73);
//    fprintf(stderr,"Take_Generic: PTYPE not found\n");
    EXIT(1);
  }
  nbhit++;
  /*add to the list the new element*/
  found=l->DATA;

  if (found==first_instance_in_for) return; // to avoid including first instance

  if (found->FLAG==SELECT || found->FLAG==PRESEL || found->FLAG==PRESEL_HEAD || found->FLAG==SELECT_HEAD || found->FLAG==ALONE || found->FLAG==CHAIN || found->FLAG==CHOICE) return;

  if (found->FLAG==HEAD) found->FLAG=PRESEL_HEAD;
  else {found->FLAG=PRESEL;}
  this_count_taken=0;
  nbget++;
  GENERIC_CIRCUIT_FOUNDINS=addchain(GENERIC_CIRCUIT_FOUNDINS,found);
}

static inline void mark_linked_foundins(foundins_list *fil)
{
  while (fil!=NULL)
    {
      fil->LOINS->USER=addptype(fil->LOINS->USER, GEN_MARK_LOINS_GROUP, 0);
      fil=fil->AFTER;
    }
}

static inline void unmark_linked_foundins(foundins_list *fil)
{
  while (fil!=NULL)
    {
      fil->LOINS->USER=delptype(fil->LOINS->USER, GEN_MARK_LOINS_GROUP);
      fil=fil->AFTER;
    }
}


/****************************************************************************/
/*    to control the lofigchain in circuit: all connectors named wich match */
/*with model are erased and if PHASE 1 saved for later treatment            */
/****************************************************************************/
static inline int should_grab(losig_list *ls)
{
  int grab=0;
  chain_list *del;

  if (!atleastFOR_FLAG) return 0;

  if (PHASE==1 && ls!=NULL)
    {       
      for (del=signal_to_grab_instance_from; del!=NULL && ls!=del->DATA; del=del->NEXT) ;
      if (del!=NULL && (ls->FLAGS & GRAB_HAVE_BEEN_DONE)==0) 
        {
          grab=1;
          ls->FLAGS |= GRAB_HAVE_BEEN_DONE;
        }
    }

  if (signal_to_grab_instance_from==NULL) grab=1;
  return grab;
}

static  void Take_Generic()
{
  locon_list *con;
  loins_list *ins;
  chain_list *del,*l,*pred=NULL;
  int grab=0;
  losig_list *ls;

  /*if we wan't something very strict, we will put to 1.*/
  /*in some case FOR_FLAG has been put to 0 put it's rather 1*/
  /*the case is an generic loins already visited and with one of its own*/
  /*losig not completely visited, because it has been entered by another loins*/
  if (!atleastFOR_FLAG) return;

  if (mbk_LosigIsVDD(MODEL_LOCON->SIG) || mbk_LosigIsVSS(MODEL_LOCON->SIG)) 
    {
      //     for (l=CIRCUIT_LOFIGCHAIN; l; l=l->NEXT) remove_pending_con((locon_list *)l->DATA);
      freechain(CIRCUIT_LOFIGCHAIN);
      CIRCUIT_LOFIGCHAIN=NULL;
      return;
    }

  if (CIRCUIT_LOFIGCHAIN!=NULL) ls=((locon_list*)CIRCUIT_LOFIGCHAIN->DATA)->SIG;
  else ls=NULL;

  grab=should_grab(ls);
/*
  if (PHASE==1 && ls!=NULL)
    {       
      for (del=signal_to_grab_instance_from; del!=NULL && ((locon_list*)CIRCUIT_LOFIGCHAIN->DATA)->SIG!=del->DATA; del=del->NEXT) ;
      if (del!=NULL && (ls->FLAGS & GRAB_HAVE_BEEN_DONE)==0) 
        {
          grab=1;
          ls->FLAGS |= GRAB_HAVE_BEEN_DONE;
        }
    }

  if (signal_to_grab_instance_from==NULL) grab=1;
*/
  this_count_taken=0; 

  if (PHASE==2) mark_linked_foundins(first_instance_in_for);

  for (l=CIRCUIT_LOFIGCHAIN; l; ) {
    con=(locon_list*)l->DATA;
    ins=(loins_list*)con->ROOT;
    if (con->TYPE=='I' && FIRST_MODEL_FIGNAME==ins->FIGNAME)
      { 
        
        if (PHASE==1 && grab) memorize_gen(ins);
        if (PHASE!=2 || (PHASE==2 && getptype(ins->USER, GEN_MARK_LOINS_GROUP)!=NULL))
          {
            if (pred) pred->NEXT=l->NEXT;
            else CIRCUIT_LOFIGCHAIN=l->NEXT;
            del=l;
            l=l->NEXT;
            //      remove_pending_con((locon_list *)del->DATA);
            del->NEXT=NULL;    /*break recursivity of freechain*/
            freechain(del);
          }
        else
          {
            pred=l;
            l=l->NEXT;
          }
      }
    else
      {
        pred=l;
        l=l->NEXT;
      }  
  }   

  if (PHASE==2) unmark_linked_foundins(first_instance_in_for);

}

static  void Only_Take_Generic(chain_list *lfchain)
{
  locon_list *con;
  loins_list *ins;
  while (lfchain!=NULL)
    {
      con=(locon_list*)lfchain->DATA;
      ins=(loins_list*)con->ROOT;
      if (con->TYPE=='I' && FIRST_MODEL_FIGNAME==ins->FIGNAME && (((long)((loins_list *)con->ROOT)->NEXT) & 1)==0)
        {         
          memorize_gen(ins);
        }
      lfchain=lfchain->NEXT;
    }
}
/****************************************************************************/
/*search if there is a chance of matching and then try it recursively       */
/****************************************************************************/
static  int Are_Locon_Equal(locon_list** circuit_c, locon_list* model_con,
                            int bit_wanted, ptype_list *env)
{
  loins_list *circuit_ins;
  loins_list *model_ins;
  lotrs_list *lotrs;
  SymInfoItem *syms;
  int match;
  locon_list *circuit_con=*circuit_c;
  foundins_list *foundins;
  ptype_list *p, *lastfoundins_var;
  long flags;
  int temp;

  if (!circuit_con || !model_con) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 74);
//    fprintf(stderr,"Are_Locon_Equal: NULL pointer\n");
    EXIT(1);
  }

  /*could match only with a external in model, it will be solved later*/
  if (circuit_con->TYPE=='E') {
    reason="instance/transistor connector should not be linked to an external connector";
    return 0;
  }

  /* we shouldn't have this case because we have stop it earlier*/
  if (model_con->TYPE=='E') {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 104);
    //fprintf(stderr,"Are_Locon_Equal: connector should be internal\n");
    EXIT(1);
  }
   
  circuit_ins=(loins_list*)circuit_con->ROOT;
  model_ins=(loins_list*)model_con->ROOT;

  /*transistor*/
  if (circuit_con->TYPE=='T') {
    temp=level;
    lotrs=(lotrs_list*)circuit_ins;
    //      if (getptype(lotrs->USER, GENIUS_GHOST_PTYPE)!=NULL) return 0;
    if ((MLO_IS_TRANSN(lotrs->TYPE) && mbk_istransn(model_ins->FIGNAME))
        || (MLO_IS_TRANSP(lotrs->TYPE) && mbk_istransp(model_ins->FIGNAME))) {
      if (model_con->NAME==MBK_GRID_NAME) {/*it must be a grid*/
        if (lotrs->GRID!=circuit_con) { reason="transistor grid mismached"; return 0; } //zinaps: prendre en compte le bulk
      }   
      else {/*it must a source or drain*/
        if (lotrs->GRID==circuit_con) { reason="transistor grid mismached"; return 0;}
      }   
      /*recursivity!!!*/
      if (PHASE==1) Generic_protect();
      Protect();

      gen_printf(2,"%sGoing through signal '%s' in model - connector '%s'\n",tabs,(char *)circuit_con->SIG->NAMECHAIN->DATA,circuit_con->NAME);

      lastfoundins_var=GLOBAL_CURFOUNDINS_VAR;
      GLOBAL_CURFOUNDINS_VAR=NULL;

      match= Is_Lotrs_Match(lotrs, model_ins, env);

      GLOBAL_CURFOUNDINS_VAR=lastfoundins_var;

      flags=(long)getptype(model_ins->USER, GENIUS_LOINS_FLAGS_PTYPE)->DATA;
      // a mathed without loins can not reduce results
      if (match && (flags & LOINS_IS_WITHOUT))
        {
          if (PHASE==1) Generic_backward();
          Backward(); 
        }
      else
        {
          if (match) {
            if (PHASE==1) Generic_reduce();
            Reduce();
          }
          else {
            if (PHASE==1) Generic_backward();
            Backward(); 
          }
        }
      if (temp!=level) EXIT(15);
      return match;
    }   
    else { reason="transistor type mismached"; return 0;}   
  }
   
  /*instance*/
  if (circuit_ins->FIGNAME!=model_ins->FIGNAME ) { reason="instance model mismached"; return 0;}
  p=getptype(circuit_ins->USER,GEN_FOUNDINS_PTYPE);
  if (!p) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 105);
//    fprintf(stderr,"Is_Loins_Match: PTYPE not found\n");
    EXIT(1);
  }
  foundins=p->DATA;
  if (foundins->VISITED && foundins->VISITED!=model_ins) 
    {
      ABORT_SYMMETRY=1;
      return 0;
    }

  /* in circuit netlist, name of vector are compound with an area for their bit*/

  /* symmetry test */

  syms=GetSymmetricChain(circuit_con);
  if (syms==NULL)
    {
      // old behaviour
      //OLD:       if (vectorradical(circuit_con->NAME)!=model_con->NAME) return 0;
      if (fastradical(circuit_con)!=model_con->NAME) { reason="connector name mismached"; return 0; }
      /* instead of model, bit vector is defined by environment variablers */
      if (fastindex(circuit_con)!=bit_wanted) { reason="connector index mismached"; return 0;}
    }
  else
    {
      // we are carefull about the "fixed" tags, we use protect(), backward() and reduce()
      Protect();
      if (PHASE==1) Generic_protect();

      if (!TryToSwapWithAConnectorWithTheSameName(circuit_c, model_con, syms, bit_wanted)) 
        {
          if (PHASE==1) Generic_backward();
          Backward();
          return 0; 
        }

      if (PHASE==1) Generic_reduce();
      Reduce();
    }

  /* end of symmetry test */
   

  /*recursivity!!!*/
  if (PHASE==1) Generic_protect();
  Protect();

  match=Put_Loins_Flags(circuit_ins, model_ins, env, *circuit_c);

  if (match) {
    if (PHASE==1) Generic_reduce();
    Reduce();
  }
  else {
    if (PHASE==1) Generic_backward();
    Backward();    
  }
  return match;
}

static inline ptype_list *getfoundinsvar(locon_list *lc)
{
  ptype_list *p;
  if (lc->TYPE=='T') return NULL;

  if ((p=getptype(((loins_list *)lc->ROOT)->USER, GEN_VISITED_PTYPE))==NULL) return NULL;
  if ((p=getptype(((loins_list *)p->DATA)->USER, GEN_FOUNDINS_PTYPE))==NULL) return NULL;
  return ((foundins_list *)p->DATA)->VAR;
}
/****************************************************************************/
/*          search in LOFIGCHAIN of circuit the locon wanted                */
/****************************************************************************/
static  int Find_Locon(gensigchain_list *model_gensig, int bit_sig, int bit_con, 
                       ptype_list *env)
{
  locon_list *model_con;
  chain_list *l,*pred;
  locon_list *con;
  int ret, enteringDECISION_CHANGE_MODE=DECISION_CHANGE_MODE;
  long flags;
  loins_list *model_ins, *entering_previousins;
  int quickend=0, modelinsistran;
  long long modeltrkey=0, modeltrmask;
  ptype_list *p/*, *oldfoundins_var*/;
  
  if (!model_gensig) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 75);
//    fprintf(stderr,"Find_Locon: NULL pointer\n");
    EXIT(1);
  }
   
  model_con=model_gensig->LOCON;

  IncreaseBackTrackCounter();

  /*external are treated after because they match with all*/
  if (model_con->TYPE=='E') {
    /*nothing to do now, later yes*/
    EXTERNAL_FLAG=1;
    return Is_Gensig_Match(model_gensig->NEXT,bit_sig,env);
  }   

  model_ins=(loins_list *)model_con->ROOT;
  flags=(long)getptype(model_ins->USER, GENIUS_LOINS_FLAGS_PTYPE)->DATA;

  // partial without accelerator
  if (flags & LOINS_IS_WITHOUT)
    {
      ptype_list *p;
      long count;
      p=getptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE);
      if (!p || (long)p->DATA==-1)
        {
          //       printf(" new count %p %d\n",p,p!=NULL?(long)p->DATA:-1);
          count=CountFakeInstanceLocon(model_ins->LOCON, env);
          if (!p) model_ins->USER=addptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE, (void *)count);
          else p->DATA=(void *)count;
        }
      else count=(long)p->DATA;

      //       printf("count0=%d\n",count);
      count--;
       
      if (count!=0) quickend=1;
      else
        {
          int quieryresult;
          quieryresult=QuickMatchChance(model_ins, CIRCUIT_LOCON->SIG, env);
          gen_printf(2,"quickres=%d", quieryresult);
          switch(quieryresult)
            {
            case QUICK_NONE_MATCH: quickend=1; break;
            case QUICK_ONE_OR_MORE_MATCH: break; // check if really matches
            case QUICK_FAKE_MATCH:
              reason="there is already a fake on this signal"; 
              return 0; //this place is already taken by another fake, math=0
            }
        }
    }

  con=NULL;
  pred=NULL;

  modelinsistran=0;
  if (!quickend && (mbk_istransn(model_ins->FIGNAME) || mbk_istransp(model_ins->FIGNAME)))
    {
      ComputeModelTransistorKeyV2(model_ins->LOCON, env, &modeltrkey, &modeltrmask);
//             gen_printf(2, "modelins(%s) key=%llx, mask=%llx\n",model_ins->INSNAME,modeltrkey,modeltrmask);
      modelinsistran=1;
    }

  for (l=CIRCUIT_LOFIGCHAIN; !quickend && l!=NULL;) 
    {
      con=(locon_list*)l->DATA;

      if (model_con==MODEL_LOCON && bit_con==MODEL_BIT) 
        {
          if (con==CIRCUIT_LOCON) 
            {
              if (pred) pred->NEXT=l->NEXT;
              else CIRCUIT_LOFIGCHAIN=l->NEXT;
              /* it's not a choice, control the others */

/*              oldfoundins_var=GLOBAL_CURFOUNDINS_VAR;
              GLOBAL_CURFOUNDINS_VAR=getfoundinsvar(model_gensig->NEXT->LOCON);
  */             
              ret=Is_Gensig_Match(model_gensig->NEXT,bit_sig,env);

//              GLOBAL_CURFOUNDINS_VAR=oldfoundins_var;
              
              if (EMERGENCY_ABORT) 
                {
                  CIRCUIT_LOFIGCHAIN=addchain(CIRCUIT_LOFIGCHAIN, l->DATA);
                  l->NEXT=NULL;
                  freechain(l);
                  return 0;
                }
              
              if (ret) 
                {
                  
                  //           remove_pending_con((locon_list *)l->DATA);
          
                  /*           if (l!=CIRCUIT_LOFIGCHAIN) UpdateSignalStatistics(((locon_list *)l->DATA)->SIG, (locon_list *)l->DATA, STAT_COUNT_SUCESS);*/
                  l->NEXT=NULL;    /*break recursivity of freechain*/
                  freechain(l);
                  return 1;
                }  
              else 
                {
                  /*           if (l!=CIRCUIT_LOFIGCHAIN) UpdateSignalStatistics(((locon_list *)l->DATA)->SIG, (locon_list *)l->DATA, STAT_COUNT_FAILURE);*/
                  if (pred) pred->NEXT=l;
                  else CIRCUIT_LOFIGCHAIN=l;
                  return 0;
                }
            }
        }
      else
        {
          if (con->TYPE=='T' && modelinsistran)
            {
              long long key0, key1;
              TransistorKeyV2((lotrs_list *)con->ROOT, &key0, &key1);
//              gen_printf(2, "curtran key0=%llx, key1=%llx, mask=%llx\n",key0, key1,modeltrmask);
              if ((key0 & modeltrmask)!=modeltrkey && (key1 & modeltrmask)!=modeltrkey)
                {
                  pred=l;
                  l=l->NEXT;
//                             gen_printf(2, "cont\n");
                  continue;
                }
//                     gen_printf(2, "check\n");
            }
          else
            if (con->TYPE=='T' && modelinsistran==0)
              {
                pred=l;
                l=l->NEXT;
                continue;
              }

          /*control not to go backward in graph, it would be a vicious circle!!*/
  
          if (con!=CIRCUIT_LOCON)
            {
              /*if this choice match*/
              int good=0, failedtoswap;
              locon_list *last=NULL; // to search thru the symmetrics
        //      printf("2.%s %c.",con->NAME,con->TYPE);

              Protect();

              if (con->TYPE=='T' || ((loins_list *)con->ROOT)->FIGNAME==((loins_list *)model_con->ROOT)->FIGNAME)
                {
                  if (isfixed(con))
                    {
                      /*             if (GEN_DEBUG_LEVEL>4)
                                             gen_printf("(%s.%s)\n",((loins_list *)CIRCUIT_LOCON->ROOT)->INSNAME,CIRCUIT_LOCON->NAME);
                      */

                      entering_previousins=PREVIOUS_INS;
                      PREVIOUS_INS=model_ins;

                      if (Are_Locon_Equal(&con,model_con,bit_con,env)) good=1;

                      PREVIOUS_INS=entering_previousins;
                      ABORT_SYMMETRY=0;
                    }
                  else
                    {
                      Protect();
                      if (PHASE==1) Generic_protect();
                      do 
                        {

                          ABORT_SYMMETRY=0;

                          entering_previousins=PREVIOUS_INS;
                          PREVIOUS_INS=model_ins;

                          if (Are_Locon_Equal(&con,model_con,bit_con,env)) { good=1; break; }

                          PREVIOUS_INS=entering_previousins;

                          if (PHASE==1) Generic_backward();
                          Backward();
                          con=(locon_list*)l->DATA;
             

                          failedtoswap=0;
                          Protect();
                          if (PHASE==1) Generic_protect();

                          if (ABORT_SYMMETRY) break;

                          do 
                            {
                              // get next symmetric
                 
                              last=GetNextSymmetric(con, last);
                              if (last!=NULL)
                                {
                                  if (TryToSwapConnectors(con, last)==0) failedtoswap=1;
                                }
                            } while (last!=NULL && failedtoswap==1);
                        } while (last!=NULL);
                      if (PHASE==1) Generic_reduce();
                      Reduce();
                      ABORT_SYMMETRY=0;
                    }
                }
              //  printf("(%s.%s)\n",con->TYPE=='T'?((lotrs_list *)con->ROOT)->TRNAME:((loins_list *)con->ROOT)->INSNAME,con->NAME);
              if (good)
                {
                  /*extract from possibilities*/
                  if (pred) pred->NEXT=l->NEXT;
                  else CIRCUIT_LOFIGCHAIN=l->NEXT;
                  /* if this choice satisfy the others...*/

/*                  oldfoundins_var=GLOBAL_CURFOUNDINS_VAR;
                  GLOBAL_CURFOUNDINS_VAR=getfoundinsvar(model_gensig->NEXT->LOCON);*/

                  ret=Is_Gensig_Match(model_gensig->NEXT,bit_sig,env);

//                  GLOBAL_CURFOUNDINS_VAR=oldfoundins_var;

                  if (EMERGENCY_ABORT) // CIRCUIT_LOFIGCHAIN n'est plus coherent
                    {
                      CIRCUIT_LOFIGCHAIN=addchain(CIRCUIT_LOFIGCHAIN, l->DATA);
                      l->NEXT=NULL;
                      freechain(l);
                      Backward();
                      return 0;
                    }

                  if (ret && (flags & LOINS_IS_WITHOUT))
                    {
                      ptype_list *p;
                      long count;
                      p=getptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE);
                      if (!p || (long)p->DATA==-1)
                        {
                          count=CountFakeInstanceLocon(model_ins->LOCON, env);
                          //             printf("count1=%d\n",count);
                          if (!p) model_ins->USER=addptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE, (void *)count);
                          else p->DATA=(void *)count;
               
                        }
                      else count=(long)p->DATA;

                      //             printf(" count1=%ld ",count);
                      count--;
                      if (count==0)
                        {
                          //             gen_printf(2," <WITHOUT REJECT1> ");
                          ret=0;
                          /*             if (pred) pred->NEXT=l;
                                                 else CIRCUIT_LOFIGCHAIN=l;*/
                          if (l!=CIRCUIT_LOFIGCHAIN) UpdateSignalStatistics(((locon_list *)l->DATA)->SIG, (locon_list *)l->DATA, STAT_COUNT_FAILURE);
                          CIRCUIT_LOFIGCHAIN=addchain(CIRCUIT_LOFIGCHAIN, l->DATA);
                          l->NEXT=NULL;
                          freechain(l);
                          reason="a real transistor/instance was found";
                          EMERGENCY_ABORT=1;
                          Backward();
                          return 0;
                        }
                      else
                        {
                          //             gen_printf(2," <WITHOUT REJECT2> ");
                          ret=!ret;
                        }
                    }
    
                  if (ret) 
                    {
                      //             remove_pending_con((locon_list *)l->DATA);
                      // printf("|%d %d %d|",enteringDECISION_CHANGE_MODE,DECISION_CHANGE_MODE,GOTO_MODE);
                      if (!enteringDECISION_CHANGE_MODE && DECISION_CHANGE_MODE && !GOTO_MODE)
                        {
                          DECISION_CHANGE_MODE=0;
                          UpdateSignalStatistics(((locon_list *)l->DATA)->SIG, (locon_list *)l->DATA, STAT_FORCE_CHANGE);
                          //             gen_printf(2," <FORCE CHANGE %s> ",((locon_list *)l->DATA)->SIG->NAMECHAIN->DATA);
                        }
                      else
                        if (l!=CIRCUIT_LOFIGCHAIN) UpdateSignalStatistics(((locon_list *)l->DATA)->SIG, (locon_list *)l->DATA, STAT_COUNT_SUCESS);
                      l->NEXT=NULL;    /*break recursivity of freechain*/
                      freechain(l);
                      Reduce();
                      break;
                    }
                  else 
                    {    
                      if (l!=CIRCUIT_LOFIGCHAIN) UpdateSignalStatistics(((locon_list *)l->DATA)->SIG, (locon_list *)l->DATA, STAT_COUNT_FAILURE);
                      if (pred) pred->NEXT=l;
                      else CIRCUIT_LOFIGCHAIN=l;
                      Backward();
                    }
                }
              else
                {
                  if (l!=CIRCUIT_LOFIGCHAIN) UpdateSignalStatistics(((locon_list *)l->DATA)->SIG, (locon_list *)l->DATA, STAT_COUNT_FAILURE);
                  Backward();
                }
            }
        }   
      pred=l;
      l=l->NEXT;
    }   
  if (!l || quickend) con=NULL;

  /*verify there are the callers in lofigchain*/
  if ((model_con==MODEL_LOCON && bit_con==MODEL_BIT) || con==CIRCUIT_LOCON) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 76);
/*    fprintf(stderr,"Find_Locon: %s is missing in LOFIGCHAIN\n",
            CIRCUIT_LOCON->NAME);*/
    EXIT(2);
  }  

  if (con==NULL && (flags & LOINS_IS_WITHOUT))
    {
      int retcode;
/*      oldfoundins_var=GLOBAL_CURFOUNDINS_VAR;
      GLOBAL_CURFOUNDINS_VAR=getfoundinsvar(model_gensig->NEXT->LOCON);
*/
      retcode=Is_Gensig_Match(model_gensig->NEXT,bit_sig,env);

//      GLOBAL_CURFOUNDINS_VAR=oldfoundins_var;

      if (retcode)
        {
          // peut etre pas necessaire ici mais on ne sais jamais
          if (EMERGENCY_ABORT) { return 0;} // CIRCUIT_LOFIGCHAIN n'est plus valide

          if (!IMustChangeDecision())
            {
              long count;
              //           gen_printf(2," <WITHOUT0> ");
              // marquage special pour les instances "without"
              /*           p=getptype(model_ins->USER,GEN_VISITED_PTYPE);
                               if (p!=NULL) p->DATA=model_ins;
                               else model_ins->USER=addptype(model_ins->USER,GEN_VISITED_PTYPE, model_ins);
              */       
              p=getptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE);
              if (p && p->DATA!=(void *)-1) count=(long)p->DATA;
              else 
                {
                  count=CountFakeInstanceLocon(model_ins->LOCON, env);
                  //           printf("count=%d\n",count);
                  if (!p) p=model_ins->USER=addptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE, (void *)count);
                  else p->DATA=(void *)count;
                }       
              //           printf(" count2=%ld ",count);
              addwithoutcount(model_ins, count);
              count--;
              if (count!=0)
                {
                  p->DATA=(void *)count;
                  AddDecisionSnapshot(PREVIOUS_INS, model_ins);
                }
              else
                p->DATA=(void *)-1;
              /*model_ins->USER=delptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE);*/
              //           printf(" count=%ld ",count);
              gen_printf(2," %s marked ",model_ins->INSNAME);
              return 1;
            }
          else
            {
              //           gen_printf(2," <GREEEE> ");
              //           if ((p=getptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE))!=NULL)
              //         p->DATA=(void *)-1; /*model_ins->USER=delptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE);*/
              DECISION_CHANGE_MODE=1;
              return 0;
            }
        }
      // enlever count
      /*       if ((p=getptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE))!=NULL)
               p->DATA=(void *)-1;*/
      /*  if (getptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE)!=NULL)
          model_ins->USER=delptype(model_ins->USER, GENIUS_LOINS_COUNT_PTYPE);*/
      return 0;
    }

  /*not found*/
  if (!con) return 0;

  // set fixed?

  return 1;
}

#ifdef ZINAPS_DUMP
char *getmark(gensigchain_list* gen, int numbit)
{
  mark_list *mark;
  ptype_list *head;
   
  /* mark already exists ? */
  head=getptype(gen->LOCON->SIG->USER,GEN_MARK_PTYPE);
  if (!head) return "BUG";
   
  for (mark=(mark_list*)head->DATA; mark; mark=mark->NEXT) {
    if (mark->BIT==numbit) break;
  }
  if (mark==NULL) return ("BUG1");
  return mark->LOSIG->NAMECHAIN->DATA;
}
#endif


/****************************************************************************/
/*  look in GENIUS's LOFIGCHAIN one locon and try the next if success       */
/*  return -1 if not found                                                  */  
/****************************************************************************/

static  int Is_Gensig_Match(gensigchain_list* gen, int bit_wanted, ptype_list* env)
{
  int low_src,high_src,low_dest,numbit;
  int all_var_def;
  
  if (!gen) return -1;
  if (gen->ghost==1) 
    {
#if 0
#ifdef ZINAPS_DUMP0      
      if (gen->LOCON->TYPE=='E')
        gen_printf(2,"GHOST> %s.%s [%s] \n",((lofig_list *)gen->LOCON->ROOT)->NAME,gen->LOCON->NAME,(char *)gen->LOCON->SIG->NAMECHAIN->DATA);
      else
        gen_printf(2,"GHOST> %s.%s [%s] \n",((loins_list *)gen->LOCON->ROOT)->INSNAME,gen->LOCON->NAME,(char *)gen->LOCON->SIG->NAMECHAIN->DATA);
#endif       
#endif
      return Is_Gensig_Match(gen->NEXT,bit_wanted,env);
    }  

  if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_TO) {
    /* signal is a vector */
    if (TOKEN(gen->LOCON_EXP)!=GEN_TOKEN_TO && TOKEN(gen->LOCON_EXP)!=GEN_TOKEN_DOWNTO) {
      /*several signals connected to one connector is forbidden */
      avt_errmsg(GNS_ERRMSG, "089", AVT_FATAL, gen->LOCON->NAME);
/*      fprintf(stderr,"Is_Gensig_Match: Connector '%s' with several signals\n",
              gen->LOCON->NAME);*/
      EXIT(2);
    }
    /* losig is a vector */
    low_src=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT,env);
    high_src=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT->NEXT,env);
    all_var_def=!Exp_VHDL_Undef();

    /* an array match with another array*/
    if (all_var_def && low_src<=bit_wanted && (high_src==UNDEF || high_src>=bit_wanted)) {
      /*low_dest is not really the low bound of vector (it could be)*/
      numbit=UNDEF;
      if (TOKEN(gen->LOCON_EXP)==GEN_TOKEN_TO)
        {
          low_dest=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT, env);
          numbit=low_dest+bit_wanted-low_src;
        }
      else if (TOKEN(gen->LOCON_EXP)==GEN_TOKEN_DOWNTO)
        {
          if (high_src==UNDEF)
            {
              avt_errmsg(GNS_ERRMSG, "090", AVT_FATAL, gen->LOCON->NAME, gen_losigname(gen->LOCON->SIG));
/*              fprintf(stderr,"Can not compute destination connector index for '%s', the high bound of signal '%s' is not known yet\n",
                      gen->LOCON->NAME, gen_losigname(gen->LOCON->SIG));*/
              EXIT(2);
            }
          low_dest=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT->NEXT, env);
          numbit=low_dest-bit_wanted+high_src;
        }

      {
        int ret;

        ret=Find_Locon(gen, bit_wanted, numbit, env);
#ifdef ZINAPS_DUMP
        if (GEN_DEBUG_LEVEL>2)
          {
            if (gen->LOCON->TYPE=='E')
              gen_printf(2,"%s[%s] '%s' - '%s(%d)' in model - connected to '%s.%s(%d)'%s\n",tabs,ret!=0?"MATCHED":"FAILED",getmark(gen,bit_wanted),(char *)gen->LOCON->SIG->NAMECHAIN->DATA,bit_wanted,((lofig_list *)gen->LOCON->ROOT)->NAME,gen->LOCON->NAME,bit_wanted,getreason(ret));
            else
              gen_printf(2,"%s[%s] '%s' - '%s(%d)' in model - connected to '%s.%s(%d)'%s\n",tabs,ret!=0?"MATCHED":"FAILED",getmark(gen,bit_wanted),(char *)gen->LOCON->SIG->NAMECHAIN->DATA,bit_wanted,((loins_list *)gen->LOCON->ROOT)->INSNAME,gen->LOCON->NAME,bit_wanted,getreason(ret));
          }
#endif
        return ret;
      }
    }
  }
  else if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_DOWNTO) {
    /* signal is a vector */
    if (TOKEN(gen->LOCON_EXP)!=GEN_TOKEN_TO && TOKEN(gen->LOCON_EXP)!=GEN_TOKEN_DOWNTO) {
      /*several signals connected to one connector is forbidden */
      avt_errmsg(GNS_ERRMSG, "089", AVT_FATAL, gen->LOCON->NAME);
/*      fprintf(stderr,"Is_Gensig_Match: Connector '%s' with several signals\n",
              gen->LOCON->NAME);*/
      EXIT(2);
    }
    /* losig is a vector */
    low_src=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT->NEXT,env);
    high_src=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT,env);
    all_var_def=!Exp_VHDL_Undef();

    /* an array match with another array*/
    if (all_var_def && low_src<=bit_wanted && (high_src==UNDEF || high_src>=bit_wanted)) {
      /*low_dest is not really the low bound of vector (it could be)*/
//      low_dest=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT,env);
      numbit=UNDEF;
      if (TOKEN(gen->LOCON_EXP)==GEN_TOKEN_TO)
        {
          if (high_src==UNDEF)
            {
              avt_errmsg(GNS_ERRMSG, "090", AVT_FATAL, gen->LOCON->NAME, gen_losigname(gen->LOCON->SIG));
/*              fprintf(stderr,"Can not compute destination connector index for '%s', the high bound of signal '%s' is not known yet\n",
                      gen->LOCON->NAME, gen_losigname(gen->LOCON->SIG));*/
              EXIT(2);
            }
          low_dest=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT,env);
          numbit=low_dest-bit_wanted+high_src;
        }
      else if (TOKEN(gen->LOCON_EXP)==GEN_TOKEN_DOWNTO)
        {
          low_dest=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT->NEXT,env);
          numbit=low_dest+bit_wanted-low_src;
        }

      {
        int ret;
        ret=Find_Locon(gen, bit_wanted, numbit, env);
#ifdef ZINAPS_DUMP    
        if (GEN_DEBUG_LEVEL>2)
          {
            if (gen->LOCON->TYPE=='E')
              gen_printf(2,"%s[%s] '%s' - '%s(%d)' in model - connected to '%s.%s(%d)'%s\n",tabs,ret!=0?"MATCHED":"FAILED",getmark(gen,bit_wanted),(char *)gen->LOCON->SIG->NAMECHAIN->DATA,bit_wanted,((lofig_list *)gen->LOCON->ROOT)->NAME,gen->LOCON->NAME,bit_wanted,getreason(ret));
            else
              gen_printf(2,"%s[%s] '%s' - '%s(%d)' in model - connected to '%s.%s(%d)'%s\n",tabs,ret!=0?"MATCHED":"FAILED",getmark(gen,bit_wanted),(char *)gen->LOCON->SIG->NAMECHAIN->DATA,bit_wanted,((loins_list *)gen->LOCON->ROOT)->INSNAME,gen->LOCON->NAME,bit_wanted,getreason(ret));
          }
#endif
        return ret;
      }
    }
  }
  else {
    /* signal is a bit */
    if (TOKEN(gen->LOCON_EXP)==GEN_TOKEN_TO || TOKEN(gen->LOCON_EXP)==GEN_TOKEN_DOWNTO) {
      /*several signals connected to one connector is forbidden */
      avt_errmsg(GNS_ERRMSG, "091", AVT_FATAL, gen->LOCON->NAME);
/*      fprintf(stderr,"Is_Gensig_Match: All locon vector '%s' connected to one bit signal\n",
              gen->LOCON->NAME);*/
      EXIT(2);
    }
    low_src=Eval_Exp_VHDL(gen->LOSIG_EXP,env);
    all_var_def=!Exp_VHDL_Undef();
    if (all_var_def && bit_wanted==low_src) {
      /* bit connected to one bit of a connector */
      {
        int ret;

        ret=Find_Locon(gen, bit_wanted, Eval_Exp_VHDL(gen->LOCON_EXP,GLOBAL_CURFOUNDINS_VAR), env);
#ifdef ZINAPS_DUMP
        if (GEN_DEBUG_LEVEL>2)
          {
            if (gen->LOCON->TYPE=='E')
              gen_printf(2,"%s[%s] '%s' - '%s' in model - connected to '%s.%s'%s\n",tabs,ret!=0?"MATCHED":"FAILED",getmark(gen,bit_wanted),(char *)gen->LOCON->SIG->NAMECHAIN->DATA,((lofig_list *)gen->LOCON->ROOT)->NAME,gen->LOCON->NAME,getreason(ret));
            else
              gen_printf(2,"%s[%s] '%s' - '%s' in model - connected to '%s.%s'%s\n",tabs,ret!=0?"MATCHED":"FAILED",getmark(gen,bit_wanted),(char *)gen->LOCON->SIG->NAMECHAIN->DATA,((loins_list *)gen->LOCON->ROOT)->INSNAME,gen->LOCON->NAME,getreason(ret));
          }
#endif
        return ret;
      }
    }
  }

  return Is_Gensig_Match(gen->NEXT,bit_wanted,env);
}


/****************************************************************************/
/* return 1 if losig of netlist and losig of model have the same connexions */
/* bit_wanted is bit of signal model                                        */
/*  check also if internal, if all locon are present                        */
/****************************************************************************/
static  int Is_Losig_Match(losig_list* circuit_sig, losig_list* model_sig, int bit_wanted, ptype_list *env)
{
  int match, wasfast;
  int lofigchainsize;
  ptype_list *p;
  chain_list *l, *pred, *next;
  int sav_external_flag;
  chain_list *sav_circuit_lofigchain;
  locon_list *lc;
  losig_list *sav_lastlosig;
  chain_list *cl;

  if (circuit_sig==NULL)
    {
      // unused connector on instance
      // always matchs
      return 1;
    }

  if (!model_sig || !circuit_sig) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 77);
//    fprintf(stderr,"Is_Losig_Match: NULL pointer\n");
    EXIT(1);
  }

  if (getptype(model_sig->USER, GNS_FORCE_MATCH)!=NULL)
  {
    if (!gns_isforcematch(getsigname(circuit_sig), getsigname(model_sig))) {reason="signal name mismatch"; return 0;}
  }
  /*if not yet defined, maybe later we will reconsider this position*/
  if (bit_wanted==UNDEF) {
    if (PHASE!=2) return 1;
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 106);
//    fprintf(stderr,"Is_Losig_Match: undeterminate bit number\n");
    EXIT(3);
  }   
   
  /*control if already visited, it must match*/
  // printf(">>>> %s for %s(%d)\n",circuit_sig->NAMECHAIN->DATA,model_sig->NAMECHAIN->DATA, bit_wanted);
  match=controlmark(1,model_sig,bit_wanted,circuit_sig,NULL); // normally NULL should not matter here
  if (match==1) return 1;
  if (!match) { avt_errmsg(GNS_ERRMSG, "005", AVT_WARNING, 9);
//          printf("- visited but no match -\n");
          return 0;
  }

  /*no use to control vdd and vss*/
  if (mbk_LosigIsVDD(model_sig)) {
    if (mbk_LosigIsVDD(circuit_sig)) return 1;
    else { reason="both model and circuit signal should have been vdd"; return 0; }
  }   
  else if (mbk_LosigIsVSS(model_sig)) {
    if (mbk_LosigIsVSS(circuit_sig)) return 1;
    else { reason="both model and circuit signal should have been vss"; return 0; }
  }   

  if ((mbk_LosigIsVDD(circuit_sig) || mbk_LosigIsVSS(circuit_sig)) && getptype(model_sig->USER, GEN_STOP_POWER_FLAG_PTYPE)!=NULL) return 1;

  p=getptype(circuit_sig->USER,LOFIGCHAIN);
  if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 78);
//    fprintf(stderr,"Is_Losig_Match: No LOFIGCHAIN in circuit\n");
    EXIT(1);
  }

#if 1
  if (model_sig->FLAGS==2)
    {
      // uniquement interne et en l'air
      cl=(chain_list *)p->DATA;
      if (cl!=NULL && cl->NEXT==NULL)
        {
          if (GEN_DEBUG_LEVEL>3)
            {
              gen_printf(3,"QUICK MATCH: '%s(%d)'\n",(char *)model_sig->NAMECHAIN->DATA, bit_wanted);
            }
          return 1;
        }
    }
  else if (model_sig->FLAGS==1)
    {
      // uniquement connecte a l'interface et une instance
      // il suffit qu'il y ait plus de 2 elements dans le lofigchain pour que
      // ca marche
      cl=(chain_list *)p->DATA;
      if (cl!=NULL && cl->NEXT!=NULL)
        {
          if (GEN_DEBUG_LEVEL>1 && cl->NEXT->NEXT==NULL) 
            gen_printf(1,"WARNING: circuit signal '%s' matched but no other connector could be found outside the instance\n",(char *)circuit_sig->NAMECHAIN->DATA);
          if (GEN_DEBUG_LEVEL>3)
            {
              gen_printf(3,"QUICK MATCH: '%s(%d)'\n",(char *)model_sig->NAMECHAIN->DATA, bit_wanted);
            }
          if (PHASE==1 && should_grab(circuit_sig)) Only_Take_Generic(cl);
          return 1;
        }
    }
#endif

   
  /*environment for circuit*/
  sav_external_flag=EXTERNAL_FLAG;
  EXTERNAL_FLAG=0;

  sav_circuit_lofigchain=CIRCUIT_LOFIGCHAIN;
  sav_lastlosig=LAST_LOSIG;

  CIRCUIT_LOFIGCHAIN=NULL; lofigchainsize=0;

  wasfast=losigisspedup(circuit_sig);

  for (pred=NULL,l=p->DATA; l; l=next)
    {
      next=l->NEXT;
      lc=(locon_list *)l->DATA;
       
      if (lc->TYPE!='E' && ((long)((loins_list *)lc->ROOT)->NEXT) & 1)
        {
          lc->SIG->FLAGS |= SIGNAL_HAVE_MORE_INSTANCES_IN_LOFIGCHAIN;
          if (wasfast) fastremovestaycoherent(l, pred, &p->DATA);
          l->NEXT=NULL; freechain(l);
          if (pred==NULL) {  p->DATA=next; }
          else { pred->NEXT=next; }
          //       if (wasfast) checklofig(lc->SIG);
        }
      else
        {
          if (lc->FLAGS!=0) circuit_sig->FLAGS|=LOFIGCHAIN_HAS_SYMS;
          CIRCUIT_LOFIGCHAIN=addchain(CIRCUIT_LOFIGCHAIN,lc);
          pred=l;
          lofigchainsize++;
        }
    }

  if (!wasfast && lofigchainsize>50)
    {
      createlofigchainhash(circuit_sig);
    }

  PushPendingLofigChain(sav_circuit_lofigchain); 

  /*let look our model*/
  p=getptype(model_sig->USER,GENSIGCHAIN);
  if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 79);
//    fprintf(stderr,"Is_Losig_Match: NULL pointer\n");
    EXIT(1);
  }   

#ifdef ZINAPS_DUMP
  if (GEN_DEBUG_LEVEL>4)
    {
      chain_list *l;
      locon_list *con;
      loins_list *ins;
      int cnt=0;
      gen_printf(4,"- view >> '%s', '%s' in model >> -\n",(char *)circuit_sig->NAMECHAIN->DATA,(char *)model_sig->NAMECHAIN->DATA);
      for (l=CIRCUIT_LOFIGCHAIN; l; l=l->NEXT) 
        {
          con=(locon_list*)l->DATA;
          ins=(loins_list*)con->ROOT;
          if (con->TYPE=='E')
            gen_printf(4,"\t [ROOT] %s.%s\t%c\n",((lofig_list *)ins)->NAME,con->NAME,con->TYPE);
          else if (con->TYPE=='T')
            gen_printf(4,"\t [%s] %s.%s -> %s\t%c\n",MLO_IS_TRANSN(((lotrs_list *)ins)->TYPE)?"TN":"TP",((lotrs_list *)ins)->TRNAME!=NULL?((lotrs_list *)ins)->TRNAME:"??",con->NAME,(char *)con->SIG->NAMECHAIN->DATA,con->TYPE);
          else
            gen_printf(4,"\t [%s] %s.%s -> %s\t%c\n",ins->FIGNAME,ins->INSNAME!=NULL?ins->INSNAME:"??",con->NAME,(char *)con->SIG->NAMECHAIN->DATA,con->TYPE);
          cnt++;
          if (GEN_DEBUG_LEVEL==4 && cnt>=5) break;
        }
      //if (cnt>=5 && l!=NULL) gen_printf(4,"\t ...etc...\n");
      gen_printf(4,"- view <<<< -\n");
    }
#endif

  /* for all bits of signal do */
  LAST_LOSIG=circuit_sig;
  match=Is_Gensig_Match((gensigchain_list*)p->DATA, bit_wanted, env);
  if (EMERGENCY_ABORT) EMERGENCY_ABORT=0;
  if (match==-1) {
     avt_errmsg(GNS_ERRMSG, "092", AVT_FATAL, bit_wanted,(char*) model_sig->NAMECHAIN->DATA);
//    fprintf(stderr,"Is_Losig_Match: %d Out of bounds for signal %s\n",bit_wanted,(char*) model_sig->NAMECHAIN->DATA);
    EXIT(3);
  }   

  if (match && EXTERNAL_FLAG && !CIRCUIT_LOFIGCHAIN) {
    /*not enough connectors to assume one external connector in lofigchain*/
    if (GEN_DEBUG_LEVEL>1) gen_printf(1,"WARNING: circuit signal '%s' matched but no other connector could be found outside the instance\n",(char *)circuit_sig->NAMECHAIN->DATA);
    //      match=0;
  }

  if (match && !(PHASE==0 && EXTERNAL_FLAG)) Take_Generic();

  if (match && !EXTERNAL_FLAG && 
      (CIRCUIT_LOFIGCHAIN || (circuit_sig->FLAGS & SIGNAL_HAVE_MORE_INSTANCES_IN_LOFIGCHAIN)!=0))
    {
#ifdef ZINAPS_DUMP
      if (GEN_DEBUG_LEVEL>3)
        {
          /*too many connectors in lofigchain for model*/
          chain_list *l;
          locon_list *con;
          loins_list *ins;
          gen_printf(3,"WARNING: circuit signal '%s' is linked to too many connectors\n",(char *)circuit_sig->NAMECHAIN->DATA);
          for (l=CIRCUIT_LOFIGCHAIN; l; l=l->NEXT) 
            {
              con=(locon_list*)l->DATA;
              ins=(loins_list*)con->ROOT;
              if (con->TYPE=='E')
                gen_printf(1,"\t [ROOT] %s.%s\t%c\n",((lofig_list *)ins)->NAME,con->NAME,con->TYPE);
              else if (con->TYPE=='T')
                gen_printf(1,"\t [%s] %s.%s -> %s\t%c\n",MLO_IS_TRANSN(((lotrs_list *)ins)->TYPE)?"TN":"TP",((lotrs_list *)ins)->TRNAME!=NULL?((lotrs_list *)ins)->TRNAME:"??",con->NAME,(char *)con->SIG->NAMECHAIN->DATA,con->TYPE);
              else
                gen_printf(1,"\t [%s] %s.%s -> %s\t%c\n",ins->FIGNAME,ins->INSNAME!=NULL?ins->INSNAME:"??",con->NAME,(char *)con->SIG->NAMECHAIN->DATA,con->TYPE);
            }
        }
      if (CIRCUIT_LOFIGCHAIN==NULL)
        gen_printf(1,"\t future instances of this model\n");
#endif     
      reason="there should be no more connections with this signal";
      match=0;
    }

  PopPendingLofigChain();

  /*   for (l=CIRCUIT_LOFIGCHAIN; l; l=l->NEXT) 
       remove_pending_con((locon_list *)l->DATA);*/
  freechain(CIRCUIT_LOFIGCHAIN);
   
  /* replace last environment*/
  EXTERNAL_FLAG=sav_external_flag;
  CIRCUIT_LOFIGCHAIN=sav_circuit_lofigchain;
  LAST_LOSIG=sav_lastlosig;
  return match;
}


/****************************************************************************/
/* calculate the index of locon in model. (index is always an expression)   */
/*  then check this locon with its index                                    */
/****************************************************************************/
static  int Is_Locon_Match(locon_list* circuit_con, locon_list* model_con,
                           ptype_list *env)
{
  int match=-1,numbit,bit_wanted,low_src,high_src,low_dest;
  locon_list *sav_model_locon,*sav_circuit_locon;
  int sav_model_bit;
  genconchain_list *gen;
  ptype_list *p;

  if (!model_con || !circuit_con) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 80);
//    fprintf(stderr,"Is_Locon_Match: NULL pointer\n");
    EXIT(1);
  }   

  /*memorize the callers*/
  sav_model_locon=MODEL_LOCON;
  sav_model_bit=MODEL_BIT;
  sav_circuit_locon=CIRCUIT_LOCON;
  MODEL_LOCON=model_con;
  if (circuit_con->TYPE=='T') MODEL_BIT=-1;
  else MODEL_BIT=fastindex(circuit_con);
  CIRCUIT_LOCON=circuit_con;
   
  p=getptype(model_con->USER,GENCONCHAIN);
  if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 81);
//    fprintf(stderr,"Is_Locon_Match: NULL pointer\n");
    EXIT(1);
  }   
  /* get bit number for model */
  if (circuit_con->TYPE=='T') bit_wanted=-1;
  else bit_wanted=fastindex(circuit_con);

  /* for all bits of the connector */  
  for (gen=(genconchain_list *)p->DATA; gen && match; gen=gen->NEXT) {   
    if (TOKEN(gen->LOCON_EXP)==GEN_TOKEN_TO) {
      if (TOKEN(gen->LOSIG_EXP)!=GEN_TOKEN_TO && TOKEN(gen->LOSIG_EXP)!=GEN_TOKEN_DOWNTO) {
        /* a part of vector is connected to only one signal*/
        avt_errmsg(GNS_ERRMSG, "088", AVT_FATAL, model_con->NAME,(char*)gen->LOSIG->NAMECHAIN->DATA);
        fprintf(stderr,"Is_Locon_Match: locon vector %s connected to one signal %s\n",
                model_con->NAME,(char*)gen->LOSIG->NAMECHAIN->DATA);
        EXIT(3);
      }   
      /* locon is a vector */
      low_src=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT,GLOBAL_CURFOUNDINS_VAR);
      high_src=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT->NEXT,GLOBAL_CURFOUNDINS_VAR);
      if (low_src<=bit_wanted && (high_src==UNDEF || high_src>=bit_wanted)) {
        /*low_dest is not really the low bound of vector (it could be)*/
        low_dest=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT,env);
        numbit=UNDEF;
        if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_TO)     numbit=low_dest+bit_wanted-low_src;
        if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_DOWNTO) numbit=low_dest-bit_wanted+low_src;
        match=Is_Losig_Match(circuit_con->SIG,gen->LOSIG,numbit,env);
      }
    }
    else if (TOKEN(gen->LOCON_EXP)==GEN_TOKEN_DOWNTO) {
      if (TOKEN(gen->LOSIG_EXP)!=GEN_TOKEN_TO && TOKEN(gen->LOSIG_EXP)!=GEN_TOKEN_DOWNTO) {
        /* a part of vector is connected to only one signal*/
        avt_errmsg(GNS_ERRMSG, "088", AVT_FATAL, model_con->NAME,(char*)gen->LOSIG->NAMECHAIN->DATA);
        /*fprintf(stderr,"Is_Locon_Match: locon vector %s connected to one signal %s\n",
                model_con->NAME,(char*)gen->LOSIG->NAMECHAIN->DATA);*/
        EXIT(3);
      }   
      /* locon is a vector */
      low_src=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT->NEXT,GLOBAL_CURFOUNDINS_VAR);
      high_src=Eval_Exp_VHDL(gen->LOCON_EXP->NEXT,GLOBAL_CURFOUNDINS_VAR);
      if (low_src<=bit_wanted && (high_src==UNDEF || high_src>=bit_wanted)) {
        /*low_dest is not really the low bound of vector (it could be)*/
        low_dest=Eval_Exp_VHDL(gen->LOSIG_EXP->NEXT,env);
        numbit=UNDEF;
        if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_TO)    numbit=low_dest-bit_wanted+high_src;
        if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_DOWNTO)numbit=low_dest+bit_wanted-high_src;
        match=Is_Losig_Match(circuit_con->SIG,gen->LOSIG,numbit,env);
      }
    }
    else {
      if (TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_TO || TOKEN(gen->LOSIG_EXP)==GEN_TOKEN_DOWNTO) {
        /* one locon connected to several signals is forbidden */
        avt_errmsg(GNS_ERRMSG, "089", AVT_FATAL, model_con->NAME);
/*        fprintf(stderr,"Is_Locon_Match: Connector '%s' with several signals\n",
                model_con->NAME);*/
        EXIT(2);
      }   
      /* locon is one bit */
      numbit=Eval_Exp_VHDL(gen->LOCON_EXP,GLOBAL_CURFOUNDINS_VAR);
      if (numbit==bit_wanted) {
        numbit=Eval_Exp_VHDL(gen->LOSIG_EXP,env);
        /* look at the signal */
        match=Is_Losig_Match(circuit_con->SIG,gen->LOSIG,numbit,env);
      }
    }
  }  

  if (match==-1) {
        avt_errmsg(GNS_ERRMSG, "093", AVT_FATAL, bit_wanted,model_con->NAME);
//    fprintf(stderr,"Is_Locon_Match: %d Out of bounds for locon %s\n",bit_wanted,model_con->NAME);
    EXIT(3);
  }   

  MODEL_LOCON=sav_model_locon;
  MODEL_BIT=sav_model_bit;
  CIRCUIT_LOCON=sav_circuit_locon;
   
  return match;
}


/****************************************************************************/
/* look if all locons of lotrs match with model                             */
/* GRID and DRAIN can be inverted so there is two tries                     */
/****************************************************************************/

static void disptrace(int match, locon_list *lc)
{
  if (match==0)
    gen_printf(3,"%s'%s' could not be associated\n",tabs, lc->NAME);
  else
    if (match==1)
      gen_printf(3,"%s'%s' is associated to '%s'\n",tabs,(char *)GetFailedMark()->LOSIG->NAMECHAIN->DATA, lc->NAME);
    else
      gen_printf(3,"%s'%s' is already correctly associated to '%s'\n",tabs,(char *)GetFailedMark()->LOSIG->NAMECHAIN->DATA, lc->NAME);
}


static  int Is_Lotrs_Match(lotrs_list* lotrs, loins_list* model_ins,
                           ptype_list *env)
{
  locon_list *model_con;
  ptype_list *p;
  int match=1,check;
  int sav_for_flag;
  int bit_wanted;
  int ret;
  chain_list *cl;
  generic_map_info *gmi;
  char *param;

  if (!lotrs || !model_ins) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 82);
//    fprintf(stderr,"Is_Lotrs_Match: NULL pointer\n");
    EXIT(1);
  }
  
  if (!mbk_istransn(model_ins->FIGNAME) && !mbk_istransp(model_ins->FIGNAME)) {
     avt_errmsg(GNS_ERRMSG, "094", AVT_FATAL, model_ins->FIGNAME);
//    fprintf(stderr,"Is_Lotrs_Match: parameter discrepancy\n");
    EXIT(1);
  }
  /*search transistor*/
  if (MLO_IS_TRANSN(lotrs->TYPE) && !mbk_istransn(model_ins->FIGNAME)) {
     avt_errmsg(GNS_ERRMSG, "094", AVT_FATAL, model_ins->FIGNAME);
    //fprintf(stderr,"Is_Lotrs_Match: parameter discrepancy\n");
    EXIT(1);
  }      
  if (MLO_IS_TRANSP(lotrs->TYPE) && !mbk_istransp(model_ins->FIGNAME)) {
     avt_errmsg(GNS_ERRMSG, "094", AVT_FATAL, model_ins->FIGNAME);
    //fprintf(stderr,"Is_Lotrs_Match: parameter discrepancy\n");
    EXIT(1);
  }     

  /*if shifted yet return */
  check=Shift_lotrs(lotrs,model_ins);
  if (check==0) { reason="transistor used elsewhere"; return 0;}
  if (check==1) return 1;

  if (lotrs->TRNAME && getptype(model_ins->USER, GNS_FORCE_MATCH)!=NULL)
  {
    if (!gns_isforcematch(lotrs->TRNAME,model_ins->INSNAME)) {reason="transistor name mismatch"; return 0;}
  }

#ifdef ZINAPS_DUMP
  gen_printf(2,"%sTrying to match transistor '%s' ('%s') with '%s' in model\n", tabs,lotrs->TRNAME!=NULL?lotrs->TRNAME:"??",MLO_IS_TRANSN(lotrs->TYPE)?"N":"P", model_ins->INSNAME);
  strcat(tabs,"  ");
#endif

  if (getchain(FCL_ANY_PMOS, model_ins->FIGNAME)==NULL
      && getchain(FCL_ANY_NMOS, model_ins->FIGNAME)==NULL
      )
/*    !=FCL_ANY_PMOS && model_ins->FIGNAME!=FCL_ANY_NMOS
      && model_ins->FIGNAME!=FCL_ANY_PMOS2 && model_ins->FIGNAME!=FCL_ANY_NMOS2)*/
    {
      if (lotrs->MODINDEX!=addlotrsmodel(NULL, model_ins->FIGNAME)) 
        {
          if (GEN_DEBUG_LEVEL>2)
            gen_printf(2,"%s[FAILED] Specific transistor model (%s) is not %s\n", tabs, getlotrsmodel(lotrs), model_ins->FIGNAME);
          goto badthing;
        }
      else
        gen_printf(2,"%s[MATCHED] Specific transistor model (%s)\n", tabs, model_ins->FIGNAME);
    }

  // transistor parameter match
  p=getptype(model_ins->USER,GEN_GENERIC_MAP_PTYPE);
   
  for (cl=p->DATA, ret=0; cl!=NULL;cl=cl->NEXT, ret++)
    {
      gmi=(generic_map_info *)cl->DATA;
      if (ret>=NB_TRAN_PARAM)
        {
          avt_errmsg(GNS_ERRMSG, "095", AVT_FATAL, gmi->FILE, gmi->LINE,model_ins->INSNAME);
//          fprintf(stderr,"%s:%d: Too many parameters for transistor '%s'\n",gmi->FILE, gmi->LINE,model_ins->INSNAME);
          EXIT(1);
        }

      if (gmi->type!='v')
        {
          avt_errmsg(GNS_ERRMSG, "096", AVT_FATAL, gmi->FILE, gmi->LINE,model_ins->INSNAME);
          fprintf(stderr,"%s:%d: A number was expected for instance '%s', found a variable name\n",gmi->FILE, gmi->LINE, model_ins->INSNAME);
          EXIT(1);
        }
      if (gmi->right.value<=0.0)
        {
          avt_errmsg(GNS_ERRMSG, "097", AVT_FATAL, gmi->FILE, gmi->LINE,model_ins->INSNAME);
//          fprintf(stderr,"%s:%d: A positive non nul number was expected for instance '%s'\n",gmi->FILE, gmi->LINE, model_ins->INSNAME);
          EXIT(1);
        }
      // looking for explicit assignation
      if (gmi->left==NULL) param=transistor_params[ret];
      else param=gmi->left;

      if (param==transistor_params[0])
        {
          if (fabs(((double)lotrs->LENGTH/SCALE_X)*1e-6 - gmi->right.value) > ((FCL_SIZE_TOLERANCE * gmi->right.value) / 100.0)) 
            {
              gen_printf(2,"%s[FAILED] Specific transistor length (%g) is not %g\n", tabs, ((double)lotrs->LENGTH/SCALE_X)*1e-6, gmi->right.value);
              goto badthing;
            }
          else
            gen_printf(2,"%s[MATCHED] Specific transistor length (%g)\n", tabs, ((double)lotrs->LENGTH/SCALE_X)*1e-6);
        }
      else 
        if (param==transistor_params[1])
          {
            if (fabs(((double)lotrs->WIDTH/SCALE_X)*1e-6 - gmi->right.value) > ((FCL_SIZE_TOLERANCE * gmi->right.value) / 100.0)) 
              {
                gen_printf(2,"%s[FAILED] Specific transistor width (%g) is not %g\n", tabs, ((double)lotrs->WIDTH/SCALE_X)*1e-6, gmi->right.value);
                goto badthing;
              }
            else
              gen_printf(2,"%s[MATCHED] Specific transistor width (%g)\n", tabs, ((double)lotrs->WIDTH/SCALE_X)*1e-6);
          }
#if 0
        else
          if (param==transistor_params[2])
            {
              if (abs(((double) lotrs->XS * lotrs->WIDTH / ( SCALE_X * SCALE_X ))*1e-12 - gmi->right.value) > ((FCL_SIZE_TOLERANCE * gmi->right.value) / 100.0)) return 0;
            }
          else
            if (param==transistor_params[3])
              {
                if (abs(((double) lotrs->XD * lotrs->WIDTH / ( SCALE_X * SCALE_X ))*1e-12 - gmi->right.value) > ((FCL_SIZE_TOLERANCE * gmi->right.value) / 100.0)) return 0;
              }
            else
              if (param==transistor_params[4])
                {
                  if (abs(((double) lotrs->PS/SCALE_X)*1e-6 - gmi->right.value) > ((FCL_SIZE_TOLERANCE * gmi->right.value) / 100.0)) return 0;
                }
              else
                if (param==transistor_params[5])
                  {
                    if (abs(((double) lotrs->PD/SCALE_X)*1e-6 - gmi->right.value) > ((FCL_SIZE_TOLERANCE * gmi->right.value) / 100.0)) return 0;
                  }
#endif
                else
                  {
                    avt_errmsg(GNS_ERRMSG, "098", AVT_FATAL, gmi->FILE, gmi->LINE,param);
//                    fprintf(stderr,"%s:%d: unknown transistor parameter '%s'\n",gmi->FILE, gmi->LINE, param);
                    EXIT(1);             
                  }
    }
   
  /*set global var. */
  p=getptype(model_ins->USER,GENERIC_PTYPE);
  if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 83);
 //   fprintf(stderr,"Is_Lotrs_Match: NULL pointer\n");
    EXIT(1);
  }
  sav_for_flag=FOR_FLAG;
  if (TOKEN((tree_list*)p->DATA)==GEN_TOKEN_FOR) FOR_FLAG=1;
  else {FOR_FLAG=0;}
  if (FOR_FLAG) atleastFOR_FLAG++;

  model_con=model_ins->LOCON;
   
  if (model_con->NAME!=MBK_GRID_NAME) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 84);
//    fprintf(stderr,"Is_Lotrs_Match: computing error\n");
    EXIT(3);
  }   

  /*first verif*/
  bit_wanted=BitNumber_Losig(lotrs->GRID,model_con/*grid*/,env);
  if (bit_wanted==-2) match=0;
  else 
    {
      match=controlmark(0,model_con->SIG,bit_wanted,lotrs->GRID->SIG,lotrs->GRID);
      disptrace(match, lotrs->GRID);
    }
  if (!match) { 
    if (FOR_FLAG) atleastFOR_FLAG--;
    FOR_FLAG=sav_for_flag;
    goto badthing;
  }

  if (SPI_IGNORE_BULK=='N' || (lotrs->BULK!=NULL && lotrs->BULK->SIG!=NULL))
    {
      // bulk
      bit_wanted=BitNumber_Losig(lotrs->BULK,model_con->NEXT->NEXT->NEXT/*bulk*/,env);
      if (bit_wanted==-2) match=0;
      else 
        {
          match=controlmark(0,model_con->NEXT->NEXT->NEXT->SIG,bit_wanted,lotrs->BULK->SIG,lotrs->BULK);
          disptrace(match, lotrs->BULK);
        }
      if (!match) { 
        if (FOR_FLAG) atleastFOR_FLAG--;
        FOR_FLAG=sav_for_flag;
        goto badthing;
      }
    }
   
  /*** 2 tries ***/
  if (PHASE==1) Generic_protect();
  Protect();

  bit_wanted=BitNumber_Losig(lotrs->DRAIN,model_con->NEXT/*drain*/,env);  
  if (bit_wanted==-2) match=0;
  else 
    {
      match=controlmark(0,model_con->NEXT->SIG,bit_wanted,lotrs->DRAIN->SIG,lotrs->DRAIN);
      disptrace(match, lotrs->DRAIN);
    }

  if (match) {
    bit_wanted=BitNumber_Losig(lotrs->SOURCE,model_con->NEXT->NEXT/*source*/,
                               env);  
    if (bit_wanted==-2) match=0;
    else 
      {
        match=controlmark(0,model_con->NEXT->NEXT->SIG,bit_wanted,lotrs->SOURCE->SIG,lotrs->SOURCE);
        disptrace(match, lotrs->SOURCE);
      }
    /*real search!*/
    if (match) {
     
      match=Is_Locon_Match(lotrs->GRID,model_con/*grid*/,env);   
      if (!match) {
        /*replace environment*/
        if (FOR_FLAG) atleastFOR_FLAG--;
        FOR_FLAG=sav_for_flag;
        if (PHASE==1) Generic_backward();
        Backward();   
        goto badthing;
      }
     
      if (SPI_IGNORE_BULK=='N' || (lotrs->BULK!=NULL && lotrs->BULK->SIG!=NULL))
        {
          match=Is_Locon_Match(lotrs->BULK,model_con->NEXT->NEXT->NEXT/*bulk*/,env);   
          if (!match) {
            /*replace environment*/
            if (FOR_FLAG) atleastFOR_FLAG--;
            FOR_FLAG=sav_for_flag;
            if (PHASE==1) Generic_backward();
            Backward();   
            goto badthing;
          }
        }

      match=Is_Locon_Match(lotrs->DRAIN,model_con->NEXT/*drain*/,env);

      if (match) {
        match=Is_Locon_Match(lotrs->SOURCE,model_con->NEXT->NEXT/*source*/,
                             env);
        if (match) {
          if (PHASE==1) Generic_reduce();
          Reduce();
          /*replace environment*/
          if (FOR_FLAG) atleastFOR_FLAG--;
          FOR_FLAG=sav_for_flag;

#ifdef ZINAPS_DUMP
          tabs[strlen(tabs)-2]='\0';
          gen_printf(2,"%s[%s] Matching transistor '%s' - '%s' in model\n",tabs,match!=0?"SUCESS":"FAILED",lotrs->TRNAME!=NULL?lotrs->TRNAME:"??",model_ins->INSNAME);
#endif       
          lotrs->FLAGS=1; // reconnu a l'envers
          return match;
        }
      }   
    }    
  }

  /*** last try ***/
  if (PHASE==1) Generic_backward();
  Backward();   
  
  bit_wanted=BitNumber_Losig(lotrs->SOURCE,model_con->NEXT/*drain*/,env);  
  if (bit_wanted==-2) match=0;
  else 
    {
      match=controlmark(0,model_con->NEXT->SIG,bit_wanted,lotrs->SOURCE->SIG,lotrs->SOURCE);
      disptrace(match, lotrs->SOURCE);
    }
  if (!match) {
    /*replace environment*/
    if (FOR_FLAG) atleastFOR_FLAG--;
    FOR_FLAG=sav_for_flag;
    goto badthing;
  }

  bit_wanted=BitNumber_Losig(lotrs->DRAIN,model_con->NEXT->NEXT/*source*/,env);
  if (bit_wanted==-2) match=0;
  else 
    {
      match=controlmark(0,model_con->NEXT->NEXT->SIG,bit_wanted,lotrs->DRAIN->SIG,lotrs->DRAIN);
      disptrace(match, lotrs->DRAIN);
    }
  if (!match) {
    /*replace environment*/
    if (FOR_FLAG) atleastFOR_FLAG--;
    FOR_FLAG=sav_for_flag;
    goto badthing;
  }
   
  match=Is_Locon_Match(lotrs->GRID,model_con/*grid*/,env);
  if (!match) { 
    /*replace environment*/
    if (FOR_FLAG) atleastFOR_FLAG--;
    FOR_FLAG=sav_for_flag;
    goto badthing;
  }
  if (SPI_IGNORE_BULK=='N' || (lotrs->BULK!=NULL && lotrs->BULK->SIG!=NULL))
    {
      match=Is_Locon_Match(lotrs->BULK,model_con->NEXT->NEXT->NEXT/*bulk*/,env);
      if (!match) { 
        /*replace environment*/
        if (FOR_FLAG) atleastFOR_FLAG--;
        FOR_FLAG=sav_for_flag;
        goto badthing;
      }
    }

  match=Is_Locon_Match(lotrs->SOURCE,model_con->NEXT/*drain*/,env);
  if (!match) { 
    /*replace environment*/
    if (FOR_FLAG) atleastFOR_FLAG--;
    FOR_FLAG=sav_for_flag;
    goto badthing;
  }
   
  match=Is_Locon_Match(lotrs->DRAIN,model_con->NEXT->NEXT/*source*/,env);

  /*replace environment*/
  if (FOR_FLAG) atleastFOR_FLAG--;
  FOR_FLAG=sav_for_flag;

#ifdef ZINAPS_DUMP
  tabs[strlen(tabs)-2]='\0';
  gen_printf(2,"%s[%s] Matching transistor '%s' - '%s' in model\n",tabs,match!=0?"SUCESS":"FAILED",lotrs->TRNAME!=NULL?lotrs->TRNAME:"??",model_ins->INSNAME);
#endif
  lotrs->FLAGS=0; // reconnu a l'endroit
  return match;

 badthing:

  match=0;
   
#ifdef ZINAPS_DUMP
  tabs[strlen(tabs)-2]='\0';
  gen_printf(2,"%s[%s] Matching transistor '%s' - '%s' in model\n",tabs,match!=0?"SUCESS":"FAILED",lotrs->TRNAME!=NULL?lotrs->TRNAME:"??",model_ins->INSNAME);
#endif
  return match;
}



/****************************************************************************/
/*   just look without recursion if this instance has the good connexion    */
/****************************************************************************/
static  int Mark_Loins(loins_list* circuit_ins, loins_list* model_ins, ptype_list *env, locon_list **lastlocon)
{
  locon_list *circuit_con,*model_con;
  ptype_list *p;
  int match=1;
  tree_list *tree=NULL;
  int bit_wanted, match0; 
  int external_swaps=0;

  if (!circuit_ins || !model_ins) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 85);
    //fprintf(stderr,"Mark_Loins: NULL pointer\n");
    EXIT(1);
  }
   
  if (circuit_ins->FIGNAME!=model_ins->FIGNAME) {
     avt_errmsg(GNS_ERRMSG, "099", AVT_FATAL, model_ins->FIGNAME);
//    fprintf(stderr,"Mark_Loins: parameter discrepancy\n");
    EXIT(1);
  }

  /*just mark signals!*/
  /*search connectors*/
  model_con=model_ins->LOCON;
  for (circuit_con=circuit_ins->LOCON; match && circuit_con; circuit_con=circuit_con->NEXT) {
    /*not enough connector*/
    if (lastlocon!=NULL) *lastlocon=circuit_con;
    if (!model_con) {
     avt_errmsg(GNS_ERRMSG, "100", AVT_FATAL, circuit_ins->INSNAME);
/*      fprintf(stderr,"Mark_Loins: too many connectors in loins '%s'\n",
              circuit_ins->INSNAME);*/
      EXIT(3);
    }
    //OLD:      if (vectorradical(circuit_con->NAME)!=model_con->NAME) {
    if (fastradical(circuit_con)!=model_con->NAME) {
     avt_errmsg(GNS_ERRMSG, "101", AVT_FATAL, circuit_con->NAME,model_con->NAME,circuit_ins->INSNAME, model_ins->INSNAME);
    
/*      fprintf(stderr,"Mark_Loins: connectors %s and %s mismatch in loins %s and %s\n",
              circuit_con->NAME,model_con->NAME,circuit_ins->INSNAME,
              model_ins->INSNAME);*/
      EXIT(3);
    }

    bit_wanted=BitNumber_Losig(circuit_con,model_con,env);
    if (bit_wanted==-2) 
      {
        match=0;
        break;
      }


    if (circuit_con->SIG!=NULL)
      {
        if (bit_wanted!=UNDEF)
          {
            int abort=1, tryagain=2;
            locon_list *last=circuit_con;
            mark_list *failedmark;
            SymInfoItem *syms;
            
            do 
              {
                
                tryagain=0;
                Protect();
                
                //          gen_printf(3,"[%s]",circuit_con->NAME);
                if ((match0=controlmark(0,model_con->SIG,bit_wanted,circuit_con->SIG,circuit_con))!=0) { abort=0; Reduce(); break; }
                Backward();
                
                syms=GetSymmetricChain(circuit_con);
                if (syms!=NULL) 
                  {
                    failedmark=GetFailedMark();
                    external_swaps++;
                    tryagain=TryToSwapWithAConnectorWithTheMarkedSignal(circuit_con, failedmark, &last, syms);
                  }
              } while (tryagain==1);
            
            if (abort==1) 
              {
                //          gen_printf(3,"failed(%s)",circuit_con->NAME);
                gen_printf(3,"%s'%s' could not be associated\n",tabs, circuit_con->NAME);
                //          gen_printf(2,"-- %d ++\n",external_swaps);
                if (external_swaps==0) ABORT_SYMMETRY=1;
                match=0; 
                break; 
              }
            else
              {
#ifdef ZINAPS_DUMP
                if (match0==1)
                  gen_printf(3,"%s'%s' is associated to '%s'\n",tabs,(char *)GetFailedMark()->LOSIG->NAMECHAIN->DATA, circuit_con->NAME);
                else
                  gen_printf(3,"%s'%s' is already correctly associated to '%s'\n",tabs,(char *)GetFailedMark()->LOSIG->NAMECHAIN->DATA, circuit_con->NAME);
#endif
              }
          }
      }

    if (!circuit_con->NEXT) {
      /* if there is some connectors left */
      if (model_con->NEXT) { 
     avt_errmsg(GNS_ERRMSG, "102", AVT_FATAL, circuit_ins->INSNAME);
/*        fprintf(stderr,"Mark_Loins: not enough connectors in loins '%s'\n",
                circuit_ins->INSNAME);*/
        EXIT(3);
      } 
    } 
    else//OLD: if (vectorradical(circuit_con->NAME)!=vectorradical(circuit_con->NEXT->NAME)) {
      if (fastradical(circuit_con)!=fastradical(circuit_con->NEXT)) {
        /* change of name vector*/
        model_con=model_con->NEXT;
        /*not enough connector*/
        if (!model_con) {
     avt_errmsg(GNS_ERRMSG, "100", AVT_FATAL, circuit_ins->INSNAME);
/*          fprintf(stderr,"Mark_Loins: too many connectors in loins '%s'\n",
                  circuit_ins->INSNAME);*/
          EXIT(3);
        }
        p=getptype(model_con->USER,GENCONCHAIN);
        if (!p) {
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 86);
//          fprintf(stderr,"Mark_Loins: ptype not found\n");
          EXIT(1);
        }
        tree=((genconchain_list*)p->DATA)->LOSIG_EXP;
        if (TOKEN(tree)==GEN_TOKEN_TO) {
          int model_end,circuit_end,model_begin,circuit_begin;
          model_begin=Eval_Exp_VHDL(tree->NEXT,env);
          model_end=Eval_Exp_VHDL(tree->NEXT->NEXT,env);
          if (model_end==UNDEF) {
            if (TOKEN(tree)==GEN_TOKEN_TO) {
              locon_list *locon,*pred=NULL;
              /*search last*/
              for (locon=circuit_con->NEXT;
                   locon && //OLD: vectorradical(locon->NAME)==vectorradical(circuit_con->NEXT->NAME); 
                     fastradical(locon)==fastradical(circuit_con->NEXT); 
                   locon=locon->NEXT) pred=locon;
              /*keep higher index*/
              circuit_end=fastindex(pred);
              circuit_begin=Eval_Exp_VHDL(tree->NEXT/*low bound*/,env);
            }   
            else {/*TOKEN(tree)=DOWNTO*/
              circuit_end=fastindex(circuit_con->NEXT);
              circuit_begin=Eval_Exp_VHDL(tree->NEXT->NEXT/*low*/,env);
            }   
            tree=tree->NEXT->NEXT->DATA;/*TOKEN(tree)=high bound of vector*/
            Solve_Equ_VHDL(tree,circuit_end-circuit_begin+model_begin,env);
          }
        }   
        else if (TOKEN(tree)==GEN_TOKEN_DOWNTO) {
          int model_end,circuit_end,model_begin,circuit_begin;
          model_begin=Eval_Exp_VHDL(tree->NEXT->NEXT,env);
          model_end=Eval_Exp_VHDL(tree->NEXT,env);
          if (model_end==UNDEF) {
            if (TOKEN(tree)==GEN_TOKEN_TO) {
              locon_list *locon,*pred=NULL;
              /*search last*/
              for (locon=circuit_con->NEXT;
                   locon && // OLD: vectorradical(locon->NAME)==vectorradical(circuit_con->NEXT->NAME); 
                     fastradical(locon)==fastradical(circuit_con->NEXT); 
                   locon=locon->NEXT) pred=locon;
              /*keep higher index*/
              circuit_end=fastindex(pred);
              circuit_begin=Eval_Exp_VHDL(tree->NEXT/*low bound*/,env);
            }   
            else {/*TOKEN(tree)=DOWNTO*/
              circuit_end=fastindex(circuit_con->NEXT);
              circuit_begin=Eval_Exp_VHDL(tree->NEXT->NEXT/*low*/,env);
            }   
            tree=((genconchain_list*)p->DATA)->LOSIG_EXP;
            tree=tree->NEXT->DATA;/*TOKEN(tree)=high bound of vector*/
            //           printf("(?==%d)",circuit_end-circuit_begin+model_begin);
            Solve_Equ_VHDL(tree,circuit_end-circuit_begin+model_begin,env);
          }
        }   
      }
      else {/* change bit vector --> nothing to do*/}      
  }
  
  return match;
}

/****************************************************************************/
/*          check for each locon of circuit if it match with model          */
/****************************************************************************/
static  int Is_Loins_Match(foundins_list* circuit_foundins, 
                           loins_list* model_ins, ptype_list *env, locon_list **lastlocon)
{
  locon_list *circuit_con,*model_con;
  int match=1;
  loins_list *circuit_ins;
  ptype_list *lastfoundins_var;

  *lastlocon=NULL;
   
  if (!circuit_foundins || !model_ins) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 87);
//    fprintf(stderr,"Is_Loins_Match: NULL pointer\n");
    EXIT(1);
  }
  circuit_ins=circuit_foundins->LOINS;
  
  if (circuit_ins->FIGNAME!=model_ins->FIGNAME) {
     avt_errmsg(GNS_ERRMSG, "103", AVT_FATAL, circuit_ins->FIGNAME,model_ins->FIGNAME);
//    fprintf(stderr,"Is_Loins_Match: parameter discrepancy with %s and %s\n",circuit_ins->FIGNAME,model_ins->FIGNAME);
    EXIT(1);
  }
  if (getptype(model_ins->USER, GNS_FORCE_MATCH)!=NULL)
  {
    if (!gns_isforcematch(circuit_ins->INSNAME,model_ins->INSNAME)) {reason="instance name mismatch"; return 0;}
  }

#ifdef ZINAPS_DUMP
  gen_printf(2,"%sTrying to match instance '%s' ('%s') with '%s' in model\n", tabs, circuit_ins->INSNAME,circuit_ins->FIGNAME, model_ins->INSNAME);
  strcat(tabs,"  ");
#endif
  /*just mark instance and control*/
  match=Shift(circuit_foundins,model_ins);
  if (match==0) return 0;   /*already another model*/
  if (match==1) return 1;   /*already visited*/

  lastfoundins_var=GLOBAL_CURFOUNDINS_VAR;
  GLOBAL_CURFOUNDINS_VAR=circuit_foundins->VAR;

  /*just mark signals!*/
  match = Mark_Loins(circuit_ins, model_ins, env, lastlocon);  

  if (match && PHASE==1 && circuit_foundins==first_instance_in_for)
    {
      signal_to_grab_instance_from=GetFixedSignalList(model_ins, env, circuit_foundins->VAR);
      /*       while (signal_to_grab_instance_from!=NULL)
               {
               printf("%s\n",((losig_list *)signal_to_grab_instance_from->DATA)->NAMECHAIN->DATA);
               signal_to_grab_instance_from=signal_to_grab_instance_from->NEXT;
               }
               EXIT(1);*/
    }
   

  /*search connectors*/
  model_con=model_ins->LOCON;
  for (circuit_con=circuit_ins->LOCON; match && circuit_con; circuit_con=circuit_con->NEXT) 
    {
      *lastlocon=circuit_con;
      /* we mark the connector as fixed */
      //       setfixed(circuit_con);
      /*       if (GEN_DEBUG_LEVEL>4)
               printf(">>> set >>> %s.%s\n",((loins_list *)circuit_con->ROOT)->INSNAME, circuit_con->NAME);
      */
      /*not enough connector*/
       
      if (!model_con) {
     avt_errmsg(GNS_ERRMSG, "100", AVT_FATAL, circuit_ins->INSNAME);
/*        fprintf(stderr,"Is_Loins_Match: too many connectors in loins '%s'\n",
                circuit_ins->INSNAME);*/
        EXIT(3);
      }
      //OLD:       if (vectorradical(circuit_con->NAME)!=model_con->NAME) {
      if (fastradical(circuit_con)!=model_con->NAME) {
     avt_errmsg(GNS_ERRMSG, "101", AVT_FATAL, circuit_con->NAME,model_con->NAME,circuit_ins->INSNAME, model_ins->INSNAME);
/*        fprintf(stderr, "Is_Loins_Match: connectors %s and %s mismatch in loins %s and %s\n",
                circuit_con->NAME,model_con->NAME,circuit_ins->INSNAME,
                model_ins->INSNAME);*/
        EXIT(3);
      }   
       
      if (!Is_Locon_Match(circuit_con,model_con,env)) {match = 0; break;}
       
      if (!circuit_con->NEXT) {
        /* if there is some connectors left */
        if (model_con->NEXT) { 
          avt_errmsg(GNS_ERRMSG, "102", AVT_FATAL, circuit_ins->INSNAME);
/*          fprintf(stderr,"Is_Loins_Match: not enough connectors in loins '%s'\n",
                  circuit_ins->INSNAME);*/
          EXIT(3);
        } 
      } 
      else // OLD: if (vectorradical(circuit_con->NAME)!=vectorradical(circuit_con->NEXT->NAME)) {
        if (fastradical(circuit_con)!=fastradical(circuit_con->NEXT)) {
          /* change of vector name*/
          model_con=model_con->NEXT;
          /*not enough connector*/
          if (!model_con) {
            avt_errmsg(GNS_ERRMSG, "100", AVT_FATAL, circuit_ins->INSNAME);
/*            fprintf(stderr,"Is_Loins_Match: too many connectors in loins '%s'\n",
                    circuit_ins->INSNAME);*/
            EXIT(3);
          }
        }
        else {/* change bit vector --> nothing to do*/}      
    }

#ifdef ZINAPS_DUMP
  tabs[strlen(tabs)-2]='\0';
  gen_printf(2,"%s[%s] Matching instance '%s' - '%s' in model\n",tabs,match!=0?"SUCESS":"FAILED",circuit_ins->INSNAME,model_ins->INSNAME);
#endif
  GLOBAL_CURFOUNDINS_VAR=lastfoundins_var;
  return match;
}




// zinaps : a commenter
static  int Is_Loins_Match_Syms(foundins_list* circuit_foundins, loins_list* model_ins, ptype_list *env)
{
  locon_list *lastupto=NULL, *lastlocon, *tmpcon, *last=NULL;
  int match=0, i;
  ptype_list *p,*p1,*p2, *env0;
  chain_list *cl;
  long oldenv[25];
  generic_map_info *gmi;

  Protect();
  if (PHASE==1) Generic_protect();

  // Saving old env values
  for (p=env, i=0; p!=NULL; p=p->NEXT, i++) oldenv[i]=p->TYPE;

  do 
    {
      // ------------------------------------
      // Assigning generic map values
      // ------------------------------------
#if 0     
#ifdef ZINAPS_DUMP
      gen_printf(3,"circuit_foundins->var: ");
      for (p=circuit_foundins->VAR;p!=NULL;p=p->NEXT)
    gen_printf(3,"%s=%ld ",(char *)p->DATA,p->TYPE);
      gen_printf(3,"\n");
#endif
#endif

      p=getptype(model_ins->USER,GEN_GENERIC_MAP_PTYPE);
     
      for (cl=p->DATA,p2=circuit_foundins->VAR; cl!=NULL;cl=cl->NEXT/*->NEXT*/,p2=p2->NEXT)
    {
      gmi=(generic_map_info *)cl->DATA;

          if (gmi->type!='n')
        {
          avt_errmsg(GNS_ERRMSG, "104", AVT_FATAL, gmi->FILE, gmi->LINE,model_ins->INSNAME);
//          fprintf(stderr,"%s:%d: A variable name was expected for instance '%s', found a number\n",gmi->FILE, gmi->LINE,model_ins->INSNAME);
          EXIT(1);
        }

      // looking for explicit assignation
      if (gmi->left/*cl->NEXT->DATA*/!=NULL)
        {
          // searching for this generic in the circuit instance // explicit assignment
          for (p1=circuit_foundins->VAR;p1!=NULL && p1->DATA!=gmi->left/*cl->NEXT->DATA*/;p1=p1->NEXT) ;
        }
      else p1=p2; // implicit assignment
      
      if (p1==NULL)
        {
          // generic not found
          avt_errmsg(GNS_ERRMSG, "031", AVT_FATAL, (char *)cl->NEXT->DATA);
//          fprintf(stderr,"Is_Loins_Match: Generic variable '%s' not define\n",(char *)cl->NEXT->DATA);
          EXIT(1);
        }
      
      for (env0=env;env0!=NULL && env0->DATA!=gmi->right.name /*cl->DATA*/;env0=env0->NEXT) ;

      if (env0==NULL)
        {
          avt_errmsg(GNS_ERRMSG, "105", AVT_FATAL, gmi->right.name);
//          avt_error("gns", 20, AVT_ERR,"Could not find generic variable '%s' in entity variable list\n",gmi->right.name);
          EXIT(1);
        }

          if (env0->TYPE!=UNDEF && p1->TYPE!=env0->TYPE)
            {
              for (p=env, i=0; p!=NULL; p=p->NEXT, i++) p->TYPE=oldenv[i];
              if (PHASE==1) Generic_backward();
              Backward();          
              return 0; // no match
            }
      env0->TYPE=p1->TYPE;
          
    }

      ABORT_SYMMETRY=0;

      /*recursive search*/
      if ((match=Is_Loins_Match(circuit_foundins, model_ins, env,&lastlocon))!=0) break;
      
      // will never match, we put back the the old env values
      for (p=env, i=0; p!=NULL; p=p->NEXT, i++) p->TYPE=oldenv[i];

      if (PHASE==1) Generic_backward();
      Backward();          

      if (ABORT_SYMMETRY==1)
    {
      break;
    }
      
      if (!match && lastlocon!=NULL)
    {
      // if the instance does not match, we try to swap the different connectors
      // with their symmetric one by one
      // for each swap, we try the modified instance
      
      // has the faulty connector got a sym?
          //      printf("here %s\n",lastlocon->NAME);
      if (GetSymmetricChain(lastlocon)==NULL) break;

      if (last!=NULL && lastlocon!=lastupto)
        {
          last=NULL;
          // have we gone backward?
          for (tmpcon=circuit_foundins->LOINS->LOCON;tmpcon!=lastlocon && tmpcon!=lastupto; tmpcon=tmpcon->NEXT) ;
              //          printf("%p ; %p ; %p\n",tmpcon, lastlocon,lastupto);
          if (tmpcon==lastlocon) break; // break the dead loop
              //          lastupto=lastlocon;
        }
      lastupto=lastlocon;
      do 
        {
          
          last=GetNextSymmetric(lastlocon, last);
        } while (last!=NULL && !TryToSwapConnectors(lastlocon,last));
      if (last==NULL) break;
      Protect();
      if (PHASE==1) Generic_protect();
    }
    } while (!match && last!=NULL && lastlocon!=NULL);

  if (match)
    {
      if (PHASE==1) Generic_reduce();
      Reduce();
    }

  return match;
}

/****************************************************************************/
/* look if flags put on last turn match with this one, it is very important */
/* for cohrence between recursive and iterative algorithm                   */
/****************************************************************************/
static  int Put_Loins_Flags(loins_list* circuit_ins, loins_list* model_ins,
                            ptype_list *env, locon_list *from_connector)
{
  ptype_list *p, *p1;
  int match;
  tree_list *tree=NULL;
  int sav_for_flag;
  foundins_list *circuit_foundins;

  if (!circuit_ins || !model_ins) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 88);
//    fprintf(stderr,"Is_Loins_Match: NULL pointer\n");
    EXIT(1);
  }
   
  p=getptype(circuit_ins->USER,GEN_FOUNDINS_PTYPE);
  if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 89);
//    fprintf(stderr,"Is_Loins_Match: PTYPE not found\n");
    EXIT(1);
  }
  circuit_foundins=p->DATA;


  /*test already check to accelerate and not to interfer with HEAD FLAG*/
  if (circuit_foundins->VISITED) {
    if (circuit_foundins->VISITED==model_ins && !circuit_foundins->ghost) {return 1;}
    else { ABORT_SYMMETRY=1; return 0;}
  }

  // fake instance : visited = self
  p1=getptype(model_ins->USER,GEN_VISITED_PTYPE);

  if (p1!=NULL && p1->DATA!=circuit_foundins->LOINS) { ABORT_SYMMETRY=1; return 0;}


  if (PHASE==0) {
    /*this instance belongs to loop we are studying?*/
    /*we will catch this instance*/
    if (FIRST_MODEL_INSNAME==model_ins->INSNAME) 
      switch (circuit_foundins->FLAG)  {
      case ALONE: return 0;break;      /*belongs to an element alone*/
      case CHOICE: case CHAIN: return 0;break;/*belongs to another loop*/
      case SELECT: case SELECT_HEAD: case HEAD: case PRESEL:
      case PRESEL_HEAD: return 1;break; /*we have already caught it, stop*/
      case 0: case UNSELECT: return 0;break;/*not in this loop*/
      default:
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 107);
/*        fprintf(stderr,"Put_Loins_Flags: flag %X unknown for loins %s in phase %d\n",
                circuit_foundins->FLAG,circuit_foundins->LOINS->INSNAME,PHASE);*/
        EXIT(3);
        break;
      }       

    /*now normal recursion*/
    p=getptype(model_ins->USER,GENERIC_PTYPE);
    if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 90);
//      fprintf(stderr,"Is_Loins_Match: PTYPE not found\n");
      EXIT(1);
    }

    if (TOKEN((tree_list*)p->DATA)!=GEN_TOKEN_FOR) /*an element alone*/
      switch (circuit_foundins->FLAG)  {
      case ALONE: break;      /*belongs to an element alone*/
      case SELECT: case SELECT_HEAD: case HEAD: case PRESEL: 
      case PRESEL_HEAD: return 0;break; /*belongs to loop, can't be alone*/
      case CHOICE: case CHAIN: return 0;break;/*belongs to another loop*/
      case 0: case UNSELECT: break;/*try it alone*/
      default:
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 108);
/*        fprintf(stderr,"Put_Loins_Flags: flag %X unknown for loins %s in phase %d\n",
                circuit_foundins->FLAG,circuit_foundins->LOINS->INSNAME,PHASE);*/
        EXIT(3);
        break;
      }       
    else /*belongs to another loop*/
      switch (circuit_foundins->FLAG)  {
        /*belongs to an element alone but maybe it will belong to a loop later*/
      case ALONE: break;
      case SELECT: case SELECT_HEAD: case HEAD: case PRESEL: 
      case PRESEL_HEAD: return 0;break; /*can't be in another loop*/
      case CHOICE: case CHAIN: break;/*belongs to another loop*/
        /*try it alone maybe it will be driven later*/
      case 0: case UNSELECT: break;
      default:
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 109);
/*        fprintf(stderr,"Put_Loins_Flags: flag %X unknown for loins %s in phase %d\n",
                circuit_foundins->FLAG,circuit_foundins->LOINS->INSNAME,PHASE);*/
        EXIT(3);
        break;
      }       
  }   

  else if (PHASE==1) { 
    /*this instance belongs to loop we are studying?*/
    /*we will catch this instance*/
    if (FIRST_MODEL_INSNAME==model_ins->INSNAME) 
      switch (circuit_foundins->FLAG)  {
      case ALONE: return 0;break;      /*belongs to an element alone*/
      case CHOICE: case CHAIN: return 0;break;/*belongs to another loop*/
      case PRESEL: return 1;
      case SELECT: case PRESEL_HEAD:     
      case SELECT_HEAD:
        if (circuit_foundins->VISITED==NULL) { ABORT_SYMMETRY=1; return 0; }// it's a HEAD of another loop
        return 1; /*we have already caught it, stop*/
      case 0: case UNSELECT: 
        circuit_foundins->FLAG=PRESEL; /*try it in this loop*/   
        GENERIC_CIRCUIT_FOUNDINS=
          addchain(GENERIC_CIRCUIT_FOUNDINS,circuit_foundins);
        /*now normal recursion*/
        p=getptype(model_ins->USER,GENERIC_PTYPE);
        if (!p) {
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 91);
     //     fprintf(stderr,"Is_Loins_Match: PTYPE not found\n");
          EXIT(1);
        }
        goto check_label;
        break;
      case HEAD:
        if (circuit_foundins->VISITED==NULL)
          {
            ABORT_SYMMETRY=1;
            return 0; // it's a HEAD of another loop
          }

        circuit_foundins->FLAG=PRESEL_HEAD; /*try it in this loop*/   
        GENERIC_CIRCUIT_FOUNDINS=
          addchain(GENERIC_CIRCUIT_FOUNDINS,circuit_foundins);
        /*now normal recursion*/
        p=getptype(model_ins->USER,GENERIC_PTYPE);
        if (!p) {
           avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 92);
//          fprintf(stderr,"Is_Loins_Match: PTYPE not found\n");
          EXIT(1);
        }
        goto check_label;
        break;
      default:
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 93);
/*        fprintf(stderr,"Put_Loins_Flags: flag %X unknown for loins %s in phase %d\n",
                circuit_foundins->FLAG,circuit_foundins->LOINS->INSNAME,PHASE);*/
        EXIT(3);
        break;
      }       

    /*now normal recursion*/
    p=getptype(model_ins->USER,GENERIC_PTYPE);
    if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 94);
//      fprintf(stderr,"Is_Loins_Match: PTYPE not found\n");
      EXIT(1);
    }

    if (TOKEN((tree_list*)p->DATA)!=GEN_TOKEN_FOR) /*an element alone*/
      switch (circuit_foundins->FLAG)  {
      case ALONE: break;      /*belongs to an element alone*/
      case SELECT: case SELECT_HEAD: case HEAD: case PRESEL: 
      case PRESEL_HEAD: return 0;break; /*belongs to loop, can't be alone*/
      case CHOICE: case CHAIN: return 0;break;/*belongs to another loop*/
        /*try it alone*/
      case 0: case UNSELECT: circuit_foundins->FLAG=ALONE;break;
      default:
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 95);
/*        fprintf(stderr,"Put_Loins_Flags: flag %X unknown for loins %s in phase %d\n",
                circuit_foundins->FLAG,circuit_foundins->LOINS->INSNAME,PHASE);*/
        EXIT(3);
        break;
      }       
    else
      {
        /*belongs to another loop*/
        switch (circuit_foundins->FLAG)  {
          /*belongs to an element alone but maybe later it will be driven by*/
          /*a loop*/
        case ALONE: break; 
        case SELECT: case SELECT_HEAD: case HEAD: case PRESEL: 
        case PRESEL_HEAD: return 0;break; /*can't be in another loop*/
        case CHOICE: case CHAIN: break;/*belongs to another loop*/
          /*try it alone, maybe it will be driven later*/
        case 0: case UNSELECT: circuit_foundins->FLAG=ALONE;break;
        default:
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 96);
/*          fprintf(stderr,"Put_Loins_Flags: flag %X unknown for loins %s in phase %d\n",
                  circuit_foundins->FLAG,circuit_foundins->LOINS->INSNAME,PHASE);*/
          EXIT(3);
          break;
        }
      }       
  }   
   
  else if (PHASE==2) {
    /*this instance belongs to loop we are studying?*/
    /*we will catch this instance*/
    if (FIRST_MODEL_INSNAME==model_ins->INSNAME) 
      switch (circuit_foundins->FLAG)  {
      case ALONE: return 0;break;      /*belongs to an element alone*/
      case CHOICE: case CHAIN: return 1;break;/*belongs to this loop*/
      case 0: case UNSELECT: return 0;break;/*not select by last phase*/
      default:
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 97);
/*        fprintf(stderr,"Put_Loins_Flags: flag %X unknown for loins %s in phase %d\n",
                circuit_foundins->FLAG,circuit_foundins->LOINS->INSNAME,PHASE);*/
        EXIT(3);
        break;
      }       

    /*now normal recursion*/
    p=getptype(model_ins->USER,GENERIC_PTYPE);
    if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 98);
//      fprintf(stderr,"Is_Loins_Match: PTYPE not found\n");
      EXIT(1);
    }

    if (TOKEN((tree_list*)p->DATA)!=GEN_TOKEN_FOR) /*an element alone*/
      switch (circuit_foundins->FLAG)  {
      case ALONE: break;      /*belongs to an element alone*/
      case CHOICE: return 0;break; /*maybe it's another: we have the choice*/
      case CHAIN: break;/*let's try it...*/
      case 0: case UNSELECT: break;/*let's try it...*/
      default:
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 99);
/*        fprintf(stderr,"Put_Loins_Flags: flag %X unknown for loins %s in phase %d\n",
                circuit_foundins->FLAG,circuit_foundins->LOINS->INSNAME,PHASE);*/
        EXIT(3);
        break;
      }       
    else /*belongs to another loop*/
      switch (circuit_foundins->FLAG)  {
      case ALONE: break;      /*let's try it like this maybe it is an */
        /*instance in a loop which has been driven by another loop*/
      case CHOICE: case CHAIN:  /*belongs to another loop*/
        setfixed(CIRCUIT_LOCON); // ??? really put this here, i don't know
        match = Mark_Loins(circuit_ins, model_ins, env, NULL);
        if (!match) { return 0;}
        gen_printf(0,"\n\t"); /*to be clean in display*/
        /*give this occurrence to an iterative algo(faster)*/
//        printf("recursion!!!!!!!!!!!!!\n");
        return continue_search_on(tree,circuit_foundins,model_ins,env);
        break;
      case 0: case UNSELECT: return 0;break;/*not selected in a loop*/
      default:
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 100);
/*        fprintf(stderr,"Put_Loins_Flags: flag %X unknown for loins %s in phase %d\n",
                circuit_foundins->FLAG,circuit_foundins->LOINS->INSNAME,PHASE);*/
        EXIT(3);
        break;
      }       
  }   
  else {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 101);
//    fprintf(stderr,"Is_Loins_Match: phase %d error\n",PHASE);
    EXIT(3);
  }


 check_label:   

  sav_for_flag=FOR_FLAG;
  if (TOKEN((tree_list*)p->DATA)==GEN_TOKEN_FOR) FOR_FLAG=1;
  else FOR_FLAG=0;
  if (FOR_FLAG) atleastFOR_FLAG++;
  
  //   setfixed(from_connector); // when jumping to another instance, we block the connector
  setfixed(CIRCUIT_LOCON); // when jumping to another instance, we block the connector
  gen_printf(4,"1. %s (%s) is now fixed\n",CIRCUIT_LOCON->NAME, (char *)CIRCUIT_LOCON->SIG->NAMECHAIN->DATA);
  gen_printf(2,"%sGoing through signal '%s' in model - connector '%s'\n",tabs,(char *)from_connector->SIG->NAMECHAIN->DATA,from_connector->NAME);

  match=Is_Loins_Match_Syms(circuit_foundins,model_ins,env);

  /*replace environment*/
  if (FOR_FLAG) atleastFOR_FLAG--;
  FOR_FLAG=sav_for_flag;
   
  return match;
}




/****************************************************************************/
/*  begin of recursive search on circuit_ins, if phase=1 a generic          */
/* instances list is generate wich can be read by get_Generic_foundins()    */
/* phase=0 environment stays at the same place, just look if match          */
/* phase=2 environment moves                                                */
/****************************************************************************/
extern int Search_Phase(int phase, foundins_list* circuit_foundins, 
                        loins_list* model_ins, ptype_list *env)
{
  int match;
  int sav_phase,sav_for_flag;
  char* sav_first_model_insname, *sav_first_model_figname;
  ptype_list* p;
   
   
  /*to encapsulate model*/
  sav_first_model_insname=FIRST_MODEL_INSNAME;
  sav_first_model_figname=FIRST_MODEL_FIGNAME;
  sav_phase=PHASE;
  sav_for_flag=FOR_FLAG;

  /*new values for static variables*/
  FIRST_MODEL_INSNAME=model_ins->INSNAME;
  FIRST_MODEL_FIGNAME=model_ins->FIGNAME;
  PHASE=phase;
  if (PHASE==1 || PHASE==0) FOR_FLAG=1;
  else {
    p=getptype(model_ins->USER,GENERIC_PTYPE);
    if (!p) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 102);
//      fprintf(stderr,"Is_Loins_Match: PTYPE not found\n");
      EXIT(1);
    }
    if (TOKEN((tree_list*)p->DATA)==GEN_TOKEN_FOR) FOR_FLAG=1;
    else FOR_FLAG=0;
  }   

  if (FOR_FLAG) atleastFOR_FLAG++;

  /*if shifted yet return */
  Protect();
  if (PHASE==1) Generic_protect();

  /*recursive search*/
  match=Is_Loins_Match_Syms(circuit_foundins, model_ins, env);
   
  /*aim of this search: reduce or go backward*/
  if (!match || (!isparal && PHASE==0)) {
    Backward();          
    if (PHASE==1) Generic_backward();
  }
  else {
    if (PHASE==1) Generic_reduce();
    Reduce();
  }
   
  /*give last environment for the caller. */
  /*notice that we could be in the middle of a recursion*/
  FIRST_MODEL_INSNAME=sav_first_model_insname;
  FIRST_MODEL_FIGNAME=sav_first_model_figname;
  PHASE=sav_phase;
  if (FOR_FLAG) atleastFOR_FLAG--;
  FOR_FLAG=sav_for_flag;
  return match;
}   


/****************************************************************************/
/* take the generic instances produced by searching for PHASE 1             */
/****************************************************************************/
extern chain_list* get_Generic_foundins()
{
  return GENERIC_CIRCUIT_FOUNDINS;
}


/****************************************************************************/
/*  erase all generic instances from an internal list for PHASE 1           */
/****************************************************************************/
extern void free_Generic_foundins()
{
  chain_list *l;
   
  if (STACK_CIRCUIT_FOUNDINS) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 103);
//    fprintf(stderr,"free_Generic_foundins: Stack not empty\n");
    EXIT(1);
  }
  
  for (l=STACK_CIRCUIT_FOUNDINS; l; l=l->NEXT) 
    freechain(l->DATA);
  freechain(STACK_CIRCUIT_FOUNDINS);
  STACK_CIRCUIT_FOUNDINS=NULL;
  freechain(GENERIC_CIRCUIT_FOUNDINS);
  GENERIC_CIRCUIT_FOUNDINS=NULL;
}


// handle of non existant instances
// backtracking

typedef struct
{
  long counter;
  loins_list *ins;
  loins_list *forwho;
  chain_list *path;
} backtrack;

static chain_list *BackTrackStack=NULL;
static long BackTrackCounter=0;
int GOTO_MODE=0,DECISION_CHANGE_MODE=0;
static chain_list *GOTO_TRACK;

void AddDecisionSnapshot(loins_list *li, loins_list *forwho)
{
  chain_list *cl;
  backtrack *bk, *bk1;

  if (GOTO_MODE) return;

  for (cl=BackTrackStack; cl!=NULL; cl=cl->NEXT)
    {
      bk1=(backtrack *)cl->DATA;
      if (bk1->forwho==forwho && bk1->ins==li) return;
    }
  if (BackTrackStack!=NULL)
    bk1=(backtrack *)BackTrackStack->DATA;
  else
    bk1=NULL;

  //  printf("/bts:%p %p/",BackTrackStack,bk1);

  if (bk1==NULL || bk1->ins!=li || bk1->forwho!=forwho)
    {
      bk=(backtrack *)mbkalloc(sizeof(backtrack));
      bk->ins=li;
      bk->forwho=forwho;
      bk->counter=-1;
      if (bk1==NULL)
    bk->path=NULL;
      else
    bk->path=dupchainlst(bk1->path);
      BackTrackStack=addchain(BackTrackStack, bk);
    }
  else
    bk=bk1;

  //  printf("ins=%s %p\n",bk->ins->INSNAME,bk->path);
  if (bk->counter!=-1)
    EXIT(24);

  bk->counter=BackTrackCounter;

  // printf("add snapshot at <%s->%s> %ld\t",li->INSNAME,forwho->INSNAME,BackTrackCounter);
}

int IMustChangeDecision()
{
  backtrack *bk;
  chain_list *cl;
  if (!GOTO_MODE || BackTrackStack==NULL) return 0;
  bk=(backtrack *)BackTrackStack->DATA;

  /*  printf("<{");
      for (cl=GOTO_TRACK;cl!=NULL;cl=cl->NEXT)
      printf("%ld,",(long)cl->DATA);
      printf("} %ld %ld>",bk->counter,BackTrackCounter);*/
  fflush(stdout);
  if (GOTO_TRACK==NULL && bk->counter==BackTrackCounter)
    {
      for (cl=bk->path; cl!=NULL && cl->NEXT!=NULL; cl=cl->NEXT) ;
      if (cl==NULL)
    bk->path=addchain(NULL, (void *)bk->counter);
      else
    cl->NEXT=addchain(NULL, (void *)bk->counter);
      GOTO_MODE=0;
      bk->counter=-1;
      //      printf("final decision change at snapshot at %ld\n",BackTrackCounter);
      return 1;
    }
  else
    if (GOTO_TRACK!=NULL && (long)GOTO_TRACK->DATA==BackTrackCounter)
      {
        //      printf("decision change at snapshot at %ld\n",BackTrackCounter);
        GOTO_TRACK=GOTO_TRACK->NEXT;
        return 1;
      }
  return 0;
}

void IncreaseBackTrackCounter()
{
  BackTrackCounter++;
  //  printf("<-(%ld)->",BackTrackCounter);
}

int ThereAreDecisionToChange()
{
  backtrack *bk;
  chain_list *cl, *next;
  for (cl=BackTrackStack; cl!=NULL; cl=cl->NEXT)
    {
      next=cl->NEXT;
      bk=(backtrack *)cl->DATA;
      if (bk->counter==-1) 
    {
      freechain(bk->path);
      mbkfree(bk);
    }
      else
        break;
    }
  BackTrackStack=cl;
  return BackTrackStack!=NULL;
}

void ClearAllDecisions()
{
  chain_list *cl;
  backtrack *bk;
  for (cl=BackTrackStack; cl!=NULL; cl=cl->NEXT)
    {
      bk=(backtrack *)cl->DATA;
      freechain(bk->path);
      mbkfree(bk);
    }
  freechain(BackTrackStack);
  BackTrackStack=NULL;
}

void ResetBackTrackCounter()
{
  BackTrackCounter=0;
  GOTO_TRACK=NULL;
  DECISION_CHANGE_MODE=0;
  if (BackTrackStack!=NULL)
    {
      backtrack *bk;
      bk=(backtrack *)BackTrackStack->DATA;
      GOTO_TRACK=bk->path;
    }
}
