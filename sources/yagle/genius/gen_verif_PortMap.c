
/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_PortMap.c                                         */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 10/06/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/


#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include MUT_H
#include MLO_H
#include API_H
#include AVT_H
#include "gen_verif_utils.h"
#include "gen_verif_exp_VHDL.h"
#include "gen_verif_vector.h"
#include "gen_verif_exclude.h"
#include "gen_verif_PortMap.h"




/***************************************************************************/
/*  change a tree of generic variables in a list of ptypeput on top of var */
/***************************************************************************/
extern ptype_list *GenTree2chain(tree_list *tree, ptype_list *var) 
{ 
  if (!tree) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 192);
//    fprintf(stderr,"GenTree2chain: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return GenTree2chain(tree->DATA,var);
    break;
  case GEN_TOKEN_GENERIC:
    return GenTree2chain(tree->NEXT,var);
    break;
  case GEN_TOKEN_NOP:
    return NULL;
    break;
  case ',':
    var=GenTree2chain(tree->NEXT->NEXT,var);       /*not to reverse order*/
    return GenTree2chain(tree->NEXT,var);
    break;
  case GEN_TOKEN_VARIABLE: case GEN_TOKEN_INTEGER:
    return GenTree2chain(tree->NEXT,var);
    break;
  case GEN_TOKEN_IDENT:
    return addptype(var,RAND_BYTE,tree);
    break;
  default:
    Error_Tree("GenTree2chain",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*    Verify if port map of instance matches with component model and      */
/* entity signals Sig and generic variables env                            */
/***************************************************************************/
static inline tree_list *Compare_PortMap(tree_list *instance, 
                                   tree_list *model, ptype_list *env_inst,
                                   ptype_list *env_model, chain_list *Sig) 
{ 
  int /*array_sig,array_mod,*/type;
  tree_list *sig;
  
  if (!instance || !model) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 193);
//    fprintf(stderr,"Compare_PortMap: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(instance)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    if (TOKEN(model)!=GEN_TOKEN_NODE) {
       Error_Tree("Compare_PortMap",model);
       EXIT(2);
    }
    Compare_PortMap(instance->DATA,model->DATA,env_inst,env_model,Sig);
    return instance;
    break;
  case ',':
    if (TOKEN(model)!=',') {
       Error_Tree("Compare_PortMap",model);
       EXIT(2);
    }
    /* no use to catch result because here it doesn't change*/
    Compare_PortMap(instance->NEXT,model->NEXT,env_inst,env_model,Sig);
    Compare_PortMap(instance->NEXT->NEXT,model->NEXT->NEXT,env_inst,env_model,Sig);
    return instance;
    break;

  /* PORT MAP(sig1,sig2,...,sigN) */
  case GEN_TOKEN_SIGNAL:
    if (TOKEN(model)!=GEN_TOKEN_SIGNAL) {
       Error_Tree("Compare_PortMap",model);
       EXIT(2);
    }
    /*result could change only here*/
    instance->NEXT=Compare_PortMap(instance->NEXT,model->NEXT,env_inst,env_model,Sig);
    return instance;
    break;
    
  case GEN_TOKEN_IDENT:  case '(':
    sig=fetch_inlist(instance,Sig);/*TOKEN(sig)=IN,OUT,INOUT...*/
    if (!sig) return instance;
    type=TOKEN(sig);
    if (!sig->NEXT) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 194);
//       fprintf(stderr,"Compare_PortMap: NULL pointer\n");
       EXIT(1);
    }
    sig=sig->NEXT->DATA;/*TOKEN(sig)=IDENT or '('-->vector */

    /*compare size of vectors*/
    if (TOKEN(instance)=='(' && Bound_Vector(instance,sig,env_inst)) {
              avt_errmsg(GNS_ERRMSG, "124", AVT_ERROR, FILE_NAME(instance),LINE(instance),getname(instance),LINE(sig));
/*       fprintf(stderr,"%s:%d: array of signal '%s' out of bounds with model line %d\n",
         FILE_NAME(instance),LINE(instance),getname(instance),LINE(sig));*/
       Inc_Error();
    }
    else if (TOKEN(instance)==GEN_TOKEN_IDENT && TOKEN(sig)=='(') {  /*signal unsized*/
      int lineno=LINE(instance);
      char *file=FILE_NAME(instance);
      tree_list *size=Duplicate_Tree(sig->NEXT->NEXT);/*take sizeof model*/
      instance=PUT_BIN('(',instance,size);
    }
    /*
    array_mod=Verif_Vector(model->NEXT,env_model);
    array_sig=Size_Vector(instance,env_inst);
// zinaps : bug here
    if (array_sig!=array_mod) {
      if (Nb_Vector_Vars()>1)
	{
	  fprintf(stderr,"%s:%d: size of signal '%s' doesn't match with size of '%s' (%d!=%d)\n",
		  FILE_NAME(instance),LINE(instance),getname(instance),getname(model->NEXT),array_mod,array_sig);
	  Inc_Error();
	}
    } 
    */
    Compare_Type(model,instance,type);
    return instance;
  default:
    Error_Tree("Compare_PortMap",instance);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/* return the signal located at the right side of equipotential matching   */
/* the connector model,  env is list of variables                          */
/* return NULL if not found or error in size of model                      */
/***************************************************************************/
static inline tree_list *Seek_Equi(tree_list *tree, tree_list *model, 
                                    ptype_list *env, chain_list *Sig) 
{ 
  tree_list *con;
  
  if (!tree || !model) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 195);
//    fprintf(stderr,"Seek_Equi: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Seek_Equi(tree->DATA,model,env,Sig);
    break;
  case ',':
    con=Seek_Equi(tree->NEXT,model,env,Sig);
    if (!con) con=Seek_Equi(tree->NEXT->NEXT,model,env,Sig);
    return con;
    break;
  case GEN_TOKEN_EQUI:
    con=Seek_Equi(tree->NEXT,model,env,Sig);        /*TOKEN(con)=IDENT or '('*/
    if (con) {
       if (Compare_Vector(model/*TOKEN=IDENT or '('*/,con,env,env)) {
              avt_errmsg(GNS_ERRMSG, "125", AVT_ERROR, FILE_NAME(tree),LINE(tree),getname(model),LINE(model));
/*         fprintf(stderr,"%s:%d: array doesn't match for '%s' (line model %d)\n",
                  FILE_NAME(tree),LINE(tree),getname(model),LINE(model));*/
         Inc_Error();
       }

       /*if direction are inverted in port map and component-->invert signal*/
       else if (TOKEN(model)=='(' && TOKEN(con)=='('
       && ((TOKEN(model->NEXT->NEXT->DATA)==GEN_TOKEN_TO 
       && TOKEN(con->NEXT->NEXT->DATA)==GEN_TOKEN_DOWNTO)
       || (TOKEN(model->NEXT->NEXT->DATA)==GEN_TOKEN_DOWNTO 
       && TOKEN(con->NEXT->NEXT->DATA)==GEN_TOKEN_TO)))  {
         /*catch signal*/
         con=tree->NEXT->NEXT->DATA;            /*TOKEN(con)=IDENT or '('*/
         if (TOKEN(con/*catch signal*/)==GEN_TOKEN_IDENT) {
            tree_list *sig;
            char *file=FILE_NAME(con);               /*for PUT_BIN macro*/
            int lineno=LINE(con);               /*for PUT_BIN macro*/
            sig=fetch_inlist(con/*IDENT or '('*/,Sig);
            /*TOKEN(sig)=IN,OUT,INOUT...*/
            if (!sig || !sig->NEXT) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 196);
//                fprintf(stderr,"Seek_Equi: NULL pointer\n");
                EXIT(1);
            }
            sig=sig->NEXT->DATA; /*TOKEN(sig)=IDENT or '('-->vector */
            if (TOKEN(sig)=='(') /*put an array*/
              tree->NEXT->NEXT->DATA=PUT_BIN('(',tree->NEXT->NEXT->DATA,
                                     Duplicate_Tree(sig->NEXT->NEXT));
         }
         /*catch signal*/
         con=tree->NEXT->NEXT->DATA;            /*TOKEN(con)=IDENT or '('*/
         /*there is already an array*/
         if (TOKEN(con)=='(') {
            tree_list *save;
            con=con->NEXT->NEXT->DATA;   /*TOKEN(con)=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO*/
            /*inversion*/
            if (TOKEN(con)==GEN_TOKEN_TO) TOKEN(con)=GEN_TOKEN_DOWNTO;
            else if (TOKEN(con)==GEN_TOKEN_DOWNTO) TOKEN(con)=GEN_TOKEN_TO;
            else {
               Error_Tree("Seek_Equi",con);
               EXIT(2);
            }
            save=con->NEXT->DATA;                             /*expression*/
            con->NEXT->DATA=con->NEXT->NEXT->DATA;            /*expression*/
            con->NEXT->NEXT->DATA=save;                       /*expression*/
         }
         /*there is no array still*/
         else if (TOKEN(con)!=GEN_TOKEN_IDENT) {
            Error_Tree("Seek_Equi",con);
            EXIT(2);
         }
       }
       return tree->NEXT->NEXT->DATA;  /*we are interested in signals*/
    }
    return NULL;
    break;
  case '(':
    con=Seek_Equi(tree->NEXT,model,env,Sig);     /*TOKEN(con)=IDENT*/
    if (con) return tree;
    else return NULL;
  case GEN_TOKEN_IDENT:
    if (getname(tree)==getname(model)) return tree;
    else return NULL;
    break;
  default:
    Error_Tree("Seek_Equi",tree);
    EXIT(2); return NULL;
  }
}
  
    
    
/***************************************************************************/
/*        Sort the equipotentials of instance in order of model            */
/* env contains the list of generic variables                              */
/***************************************************************************/
static inline tree_list *Sort_Equi(tree_list *instance, tree_list *model,
                              ptype_list *env, chain_list *Sig) 
{ 
  int lineno;
  char* file;
  tree_list *res1,*res2,*found;

  if (!instance || !model) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 197);
//    fprintf(stderr,"Sort_Equi: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(model)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Sort_Equi(instance,model->DATA,env,Sig);
    break;
  case ',':
    res1=Sort_Equi(instance,model->NEXT,env,Sig);
    res2=Sort_Equi(instance,model->NEXT->NEXT,env,Sig);
    lineno=LINE(res1);/* used in PUT_BIN()*/
    file=FILE_NAME(res1);
    return PUT_BIN(',',res1,res2);
    break;
  case GEN_TOKEN_SIGNAL:
    found=Seek_Equi(instance,model->NEXT->DATA->NEXT->DATA/*TOKEN=IDENT or '('*/,env,Sig);
    if (!found) {
              avt_errmsg(GNS_ERRMSG, "126", AVT_ERROR, FILE_NAME(instance),LINE(instance),getname(model->NEXT));
/*      fprintf(stderr,"%s:%d: connector '%s' is missing in left side of instance\n",
         FILE_NAME(instance),LINE(instance),getname(model->NEXT));*/
      Inc_Error();
      return Duplicate_Tree(model);
    }
    lineno=LINE(found);/* used in PUT_UNI()*/
    file=FILE_NAME(found);
    return PUT_UNI(GEN_TOKEN_SIGNAL,Duplicate_Tree(found));
    break;
  default:
    Error_Tree("Sort_Equi",model);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*        Compare the number of argument between instance and model        */
/* return 1 if different else return 0                                     */
/***************************************************************************/
static inline int Compare_NumArgs(tree_list *instance, tree_list *model) 
{ 
  if (!instance || !model) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 198);
//    fprintf(stderr,"Compare_NumArgs: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(instance)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    if (TOKEN(model)!=GEN_TOKEN_NODE) {
       Error_Tree("Compare_NumArgs",model);
       EXIT(2);
    }    
    return Compare_NumArgs(instance->DATA,model->DATA);
    break;
  case ',':
    if (TOKEN(model)!=',') {
              avt_errmsg(GNS_ERRMSG, "127", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(model));
/*    fprintf(stderr,"%s:%d: too many connections in Port Map. Component line %d\n",
              FILE_NAME(instance),LINE(instance),LINE(model));*/
      Inc_Error();
      return 1;
    }
    if (Compare_NumArgs(instance->NEXT,model->NEXT)) return 1;
    else return Compare_NumArgs(instance->NEXT->NEXT,model->NEXT->NEXT);
    break;

  /* PORT MAP(sig1,sig2,...,sigN)  or PORT MAP(con1=>sig1,...,conN=>sigN) */
  case GEN_TOKEN_SIGNAL: case GEN_TOKEN_EQUI:
    if (TOKEN(model)==',') {
              avt_errmsg(GNS_ERRMSG, "128", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(model));
/*      fprintf(stderr,"%s:%d: not enough connections in Port Map. Component line %d\n",
              FILE_NAME(instance),LINE(instance),LINE(model));*/
      Inc_Error();
      return 1;
    }
    return 0;
    break;
  default:
    Error_Tree("Compare_NumArgs",instance);
    EXIT(2); return 0;
  }
}

/***************************************************************************/
/*           return 1 if tree contains an equi else return 0               */
/***************************************************************************/
static inline int Detect_Equi(tree_list *tree) 
{ 
  if (!tree) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 199);
//    fprintf(stderr,"Detect_Equi: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Detect_Equi(tree->DATA);
    break;
  case ',':
    return Detect_Equi(tree->NEXT);
    break;
 case GEN_TOKEN_SIGNAL:
    return 0;
    break;
  case GEN_TOKEN_EQUI:
    return 1;
    break;
  default:
    Error_Tree("Detect_Equi",tree);
    EXIT(2); return 0;
  }
}


/***************************************************************************/
/*              Give real dimension to signal unsized                      */
/***************************************************************************/
static inline tree_list* Give_Dimension(tree_list *tree,chain_list *Sig) 
{ 
  tree_list* ret;

  if (!tree) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 200);
//    fprintf(stderr,"Give_Dimension: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    tree->DATA=Give_Dimension(tree->DATA,Sig);
    return tree;
    break;
  case ',':
    Give_Dimension(tree->NEXT,Sig);
    Give_Dimension(tree->NEXT->NEXT,Sig);
    return tree;
    break;
  case GEN_TOKEN_SIGNAL:
    Give_Dimension(tree->NEXT,Sig);
    return tree;
    break;
  case GEN_TOKEN_EQUI:
    Give_Dimension(tree->NEXT->NEXT,Sig);
    return tree;
    break;
  case '(':
    /*nothing to do*/
    return tree;
    break;
  case GEN_TOKEN_IDENT:
    ret=fetch_inlist(tree, Sig);    /* TOKEN(ret)=SIGNAL */
    if (ret) {
      ret=ret->NEXT->DATA;            /* TOKEN(ret)='(' or TOKEN(ret)=IDENT */
      if (TOKEN(ret)=='(') {
         ret=Duplicate_Tree(ret);
         Free_Tree(tree);
         return ret;
      }
    }
    return tree;
    break;
  default:
    Error_Tree("Give_Dimension",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/* Verify that all instances of tree match with components Comp and generic*/
/* variables and signals Sig                                               */
/* result put on top of Ins                                                */  
/***************************************************************************/
extern chain_list *Verif_Instance(tree,Sig,env,Comp,Ins) 
   tree_list *tree;
   ptype_list* env;
   chain_list *Sig,*Comp,*Ins;
{ 
  int ope1,ope2,error;
  chain_list *Ins2;     /*garbage just to free mem*/
  tree_list *component,*port;
  ptype_list *env_component,*p,*q;

  if (!tree) {
    return Ins;
/*    fprintf(stderr,"Verif_Instance: NULL pointer\n");
    EXIT(1);*/
  }

  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_Instance(tree->DATA,Sig,env,Comp,Ins);
    break;
  case GEN_TOKEN_EXCLUDE:
    Ins= Verif_Instance(tree->NEXT,Sig,env,Comp,Ins);
    return Ins;
    break;
  case ';':
    Ins=Verif_Instance(tree->NEXT,Sig,env,Comp,Ins);
    return Verif_Instance(tree->NEXT->NEXT,Sig,env,Comp,Ins);
    break;
  case GEN_TOKEN_OF:
    return Verif_Instance(tree->NEXT,Sig,env,Comp,Ins);
    break;
  case GEN_TOKEN_IDENT:
    return put_inlist(tree,Ins);
    break;
  case GEN_TOKEN_MAP:
    Ins=Verif_Instance(tree->NEXT,Sig,env,Comp,Ins);
#if 0
    /*to alert if 2 times the same name is used*/
    if ((ope1=Is_Exclude(getname(tree->NEXT)))) {
      /*copy MAP and a NODE*/
      tree_list *map=addtree(FILE_NAME(tree),LINE(tree),GEN_TOKEN_MAP,NULL,tree->NEXT);
      tree_list *node=addtree(FILE_NAME(tree),LINE(tree),GEN_TOKEN_NODE,map,NULL);
      /*scratch and insertion of exclude in tree*/
      tree->LINE=ope1;
      tree->TOKEN=GEN_TOKEN_EXCLUDE;
      tree->DATA=NULL;
      tree->NEXT=node;
      /*we were at MAP level*/
      tree=map;
    }
#endif
    error=Get_Error();
    component=fetch_inlist(getident_of(tree->NEXT),Comp);/*TOKEN(component)=NODE*/
    /* error from user*/
    if (!component) return Ins;
    /* test for BUG!!! */
    if (!component->NEXT || !component->NEXT->NEXT || 
        !component->NEXT->NEXT->DATA) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 201);
//         fprintf(stderr,"Verif_Instance: NULL pointer in component list\n");
         EXIT(1);
    }     
    port=component->NEXT->NEXT->DATA;
    if (TOKEN(port)!=GEN_TOKEN_PORT) {
         Error_Tree("Verif_Instance",port);
         EXIT(2);
    }         

    if (Compare_NumArgs(tree->NEXT->NEXT,port->NEXT)) return Ins;
    /*no use to continue because number of arguments are different*/
    env_component=GenTree2chain(component->NEXT,NULL);
    /* to accord values with global environment */
    for (q=env_component;q;q=q->NEXT) {
      char *name=getname(q->DATA);
      for (p=env;p;p=p->NEXT) if (getname(p->DATA)==name) break;
      if (p) q->TYPE=p->TYPE;
    }
    /* give a size to signal vector without */
    Give_Dimension(tree->NEXT->NEXT,Sig);
    if (Detect_Equi(tree->NEXT->NEXT)) {
  /*  PORT MAP(con3=>sig3,...,con1=>sig1) ---> PORT MAP(sig1,sig2,...,sigN)  */
       tree_list *res;
       res=Sort_Equi(tree->NEXT->NEXT,port->NEXT,env_component,Sig);
       Free_Tree(tree->NEXT->NEXT->DATA);
       tree->NEXT->NEXT->DATA=res;
    }
    if (error==Get_Error()) { 
         Compare_PortMap(tree->NEXT->NEXT,port->NEXT,env,env_component,Sig);
    }
    freeptype(env_component);
    return Ins;
    break;
  case GEN_TOKEN_IF:
    Verif_Bool_VHDL(tree->NEXT,env);
    return Verif_Instance(tree->NEXT->NEXT,Sig,env,Comp,Ins);
    break;
  case GEN_TOKEN_FOR:
    ope1=Verif_Exp_VHDL(tree->NEXT->NEXT,env);
    ope2=Verif_Exp_VHDL(tree->NEXT->NEXT->NEXT,env);
    /*if (ope1>ope2) {
      fprintf(stderr,"%s:%d: bounds of FOR are inverted\n",FILE_NAME(tree),LINE(tree));
      Inc_Error();
    } 
    */
    if (!tree->NEXT) {
              avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 202);
//      fprintf(stderr,"Verif_Instance: NULL pointer in FOR\n");
      EXIT(1);
    }
   // zinaps: removed Verif_HighBound_FOR(tree->NEXT->NEXT->NEXT,env);
    /*to check if variable isn't already used and for is in bounds*/
    /*lower bound of for*/
    env=put_value(tree->NEXT->DATA,ope1,env); 
    Ins=Verif_Instance(tree->NEXT->NEXT->NEXT->NEXT,Sig,env,Comp,Ins);
    /*upper bound of for*/
    env->TYPE=ope2; 
    Ins2=Verif_Instance(tree->NEXT->NEXT->NEXT->NEXT,Sig,env,Comp,NULL);
    /*no use to control 2 times name of instances*/
    freechain(Ins2);
    /*remove variable of for*/
    env->NEXT=NULL;  
    freeptype(env);
    return Ins;
    break;
  default:
    Error_Tree("Verif_Instance",tree);
    EXIT(2); return NULL;
  }
}



