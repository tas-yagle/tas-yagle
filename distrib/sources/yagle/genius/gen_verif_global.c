/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_global.c                                          */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 08/04/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/


#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include MUT_H
#include MLO_H
#include API_H
#include AVT_H
#include "gen_model_transistor.h"
#include "gen_verif_utils.h"
#include "gen_verif_exp_VHDL.h"
#include "gen_verif_vector.h"
#include "gen_verif_exclude.h"
#include "gen_verif_PortMap.h"
#include "gen_verif_global.h"
#include "gen_env.h"
#include "gen_model_utils.h"


#define TRANS_NUM_INF 1         /*minimal number of trans. accepted in a rule*/
#define TRANS_NUM_SUP 150       /*maximal number of trans. accepted in a rule*/




/***************************************************************************/
/*        Verify if generic of instance and model are equal                */
/* return the new list of variables from component put on the top of Gen   */
/***************************************************************************/
static inline ptype_list *Compare_Generic(tree_list *instance, tree_list *model, 
                                   ptype_list *Gen)
{ 
  if (!instance || !model) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 211);
    //fprintf(stderr,"Compare_Generic: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(instance)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    if (TOKEN(model)!=GEN_TOKEN_NODE) {
      Error_Tree("Compare_Generic",model);
      EXIT(2); 
    }
    return Compare_Generic(instance->DATA,model->DATA,Gen);
    break;
  case GEN_TOKEN_GENERIC:
    if (TOKEN(model)!=GEN_TOKEN_GENERIC) {
    avt_errmsg(GNS_ERRMSG, "137", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(model));
/*      fprintf(stderr,"%s:%d: a generic isn't needed by model line %d\n",
              FILE_NAME(instance),LINE(instance),LINE(model));*/
      Inc_Error();
      return Gen;
    }
    return Compare_Generic(instance->NEXT,model->NEXT,Gen); 
    break;
  case GEN_TOKEN_NOP:
    if (TOKEN(model)!=GEN_TOKEN_NOP) {
    avt_errmsg(GNS_ERRMSG, "138", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(model));
/*      fprintf(stderr,"%s:%d: a Generic is needed by model line %d\n",
              FILE_NAME(instance),LINE(instance),LINE(model));*/
      Inc_Error();
    }
    return NULL;   
    break;
  case ',':
    if (TOKEN(model)!=',') {
    avt_errmsg(GNS_ERRMSG, "141", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(model),getname(model));
/*      fprintf(stderr, "%s:%d: too many variables in component. model ends at line %d with '%s'\n",
              FILE_NAME(instance),LINE(instance),LINE(model),getname(model));*/
      Inc_Error();
      return Gen;
    }
    Gen=Compare_Generic(instance->NEXT,model->NEXT,Gen);
    return Compare_Generic(instance->NEXT->NEXT,model->NEXT->NEXT,Gen);
    break;
  case GEN_TOKEN_VARIABLE: case GEN_TOKEN_INTEGER:
    if (TOKEN(model)==',') {
      tree_list *p;
      for (p=model;TOKEN(p)==',';p=p->NEXT) {}
    avt_errmsg(GNS_ERRMSG, "139", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(p),getname(p));
/*      fprintf(stderr,"%s:%d: not enough variables in component. model ends at line %d with '%s'\n",
              FILE_NAME(instance),LINE(instance),LINE(p),getname(p));*/
      Inc_Error();
      return Gen;
    }
    if (TOKEN(instance)!=TOKEN(model)) {
    avt_errmsg(GNS_ERRMSG, "140", AVT_ERROR, FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));
/*      fprintf(stderr,"%s:%d: type '%s' doesn't match with model line %d\n",
              FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));*/
      Inc_Error();
    }
    return Compare_Generic(instance->NEXT,model->NEXT,Gen);
    break;
  case GEN_TOKEN_IDENT:    
    if (TOKEN(model)!=GEN_TOKEN_IDENT) {
      Error_Tree("Compare_Generic",instance);
       EXIT(2); 
    }      
/*to alert if 2 times the same name is used, the value is without any importance*/
    return put_value(instance,RAND_BYTE,Gen);
    break;
  default:
    Error_Tree("Compare_Generic",instance);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*        Verify if port of instance and model are equal                   */
/* return the new list of variables put on the top of Con                  */
/* it doesn't check the size of vector!!!                                  */
/***************************************************************************/

static char dir;

typedef struct
{
  char dir;
  char *name;
} CONLIST;

static chain_list *CONLISTCL;

static chain_list *Compare_Port(tree_list *instance, tree_list *model, 
                                 ptype_list *env_inst, ptype_list *env_model, 
                                 chain_list *Con) 
{
  int ope1,ope2;
  if (!instance || !model) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 212);
//    fprintf(stderr,"Compare_Port: NULL pointer\n");
    EXIT(1);
  }
 switch (TOKEN(instance)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    if (TOKEN(model)!=GEN_TOKEN_NODE) {
      Error_Tree("Compare_Port",model);
      EXIT(2); 
    }
    return Compare_Port(instance->DATA,model->DATA,env_inst,env_model,Con);
    break;
  case GEN_TOKEN_PORT:
    if (TOKEN(model)!=GEN_TOKEN_PORT) {
    avt_errmsg(GNS_ERRMSG, "142", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(model));
/*
      fprintf(stderr, "%s:%d: a port isn't needed by model line %d\n",
              FILE_NAME(instance),LINE(instance),LINE(model));*/
      Inc_Error();
      return Con;
    }
    return Compare_Port(instance->NEXT,model->NEXT,env_inst,env_model,Con); 
    break;
  case GEN_TOKEN_NOP:
    if (TOKEN(model)!=GEN_TOKEN_NOP) {

    avt_errmsg(GNS_ERRMSG, "143", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(model));
/*      fprintf(stderr, "%s:%d: a Port is needed by model line %d\n",
              FILE_NAME(instance),LINE(instance),LINE(model));*/
      Inc_Error();
    }
    return NULL;   
    break;
  case ',':
    if (TOKEN(model)!=',') {
    avt_errmsg(GNS_ERRMSG, "141", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(model),getname(model));
/*      fprintf(stderr, "%s:%d: too many connectors in component. model ends at line %d with '%s'\n",
              FILE_NAME(instance),LINE(instance),LINE(model),getname(model));*/
      Inc_Error();
      return Con;
    }
    Con=Compare_Port(instance->NEXT,model->NEXT,env_inst,env_model,Con);
    return Compare_Port(instance->NEXT->NEXT,model->NEXT->NEXT,env_inst,env_model,Con);
    break;
  case GEN_TOKEN_SIGNAL: 
 case GEN_TOKEN_OUT: case GEN_TOKEN_IN: case GEN_TOKEN_INOUT: case GEN_TOKEN_TRANSCV: case GEN_TOKEN_TRISTATE: case GEN_TOKEN_LINKAGE:
    if (TOKEN(model)==',') {
      tree_list *p;
      for (p=model;TOKEN(p)==',';p=p->NEXT->NEXT) {}
    avt_errmsg(GNS_ERRMSG, "141", AVT_ERROR, FILE_NAME(instance),LINE(instance),LINE(model),getname(p));
/*      fprintf(stderr, "%s:%d: not enough connectors in component. model ends at line %d with '%s'\n",
              FILE_NAME(instance),LINE(instance),LINE(p),getname(p));*/
      Inc_Error();
      return Con;
    }
    if (TOKEN(instance)!=TOKEN(model) && TOKEN(model)!=GEN_TOKEN_LINKAGE) {
    avt_errmsg(GNS_ERRMSG, "140", AVT_ERROR, FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));
/*      fprintf(stderr,"%s:%d: type '%s' doesn't match with model line %d\n",
              FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));*/
      Inc_Error();
    }

    dir=' ';
    switch(TOKEN(instance))
      {
      case GEN_TOKEN_OUT : dir=OUT; break;
      case GEN_TOKEN_IN : dir=IN; break;
      case GEN_TOKEN_INOUT : dir=INOUT; break;
      case GEN_TOKEN_TRANSCV : dir=TRANSCV; break;
      case GEN_TOKEN_TRISTATE : dir=TRISTATE; break;
      case GEN_TOKEN_LINKAGE : dir=UNKNOWN; break;
      }
    
    return Compare_Port(instance->NEXT,model->NEXT,env_inst,env_model,Con);
    break;
  case '(':
    if (TOKEN(model)!='(') {
    avt_errmsg(GNS_ERRMSG, "144", AVT_ERROR, FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));
/*       fprintf(stderr,"%s:%d: a bit is expected for '%s' line %d of model\n",
                     FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));*/
       Inc_Error();
       Con=Compare_Port(instance->NEXT->DATA,model,env_inst,env_model,Con);
       return Con;
    }   
    Con=Compare_Port(instance->NEXT,model->NEXT,env_inst,env_model,Con);
    ope1=Verif_Vector(instance,env_inst);
    ope2=Verif_Vector(model,env_model);
    if (ope1!=ope2) {
    avt_errmsg(GNS_ERRMSG, "145", AVT_ERROR, FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));
/*       fprintf(stderr,"%s:%d: size of '%s' mismatches with model line %d\n",
                     FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));*/
       Inc_Error();    
    }
    return Con;
    break;
  case GEN_TOKEN_IDENT:
    if (TOKEN(model)=='(') {
    avt_errmsg(GNS_ERRMSG, "146", AVT_ERROR, FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));
/*       fprintf(stderr,"%s:%d: a vector is expected for '%s' line %d of model\n",
                     FILE_NAME(instance),LINE(instance),getname(instance),LINE(model));*/
       Inc_Error();
       Con=Compare_Port(instance,model->NEXT->DATA,env_inst,env_model,Con);
       return Con;
    }   
/*to alert if 2 times the same name is used*/
    {
      CONLIST *clt;
      clt=(CONLIST *)mbkalloc(sizeof(CONLIST));
      clt->dir=dir;
      clt->name=getname(instance);
      
      CONLISTCL=addchain(CONLISTCL, clt);
    }
    return put_inlist(instance,Con);
    break;
  default:
    Error_Tree("Compare_Port",instance);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*give an index to entity and entity architecture to escape duplicate archi*/
/* and action                                                              */
/***************************************************************************/
static inline void Change_Name(tree_list *entity, tree_list *architecture,
                               tree_list *action)
{
  static long count=0; /*not to give the same index 2 times*/
  char memo_char;
  static char *archi_name=NULL;

  if (!entity || !architecture) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 213);
//    fprintf(stderr,"Change_Name: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(architecture)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      if (TOKEN(entity)!=GEN_TOKEN_NODE) {
         Error_Tree("Change_Name",entity);
         EXIT(1);
      }
      if (action && TOKEN(action)!=GEN_TOKEN_NODE) {
         Error_Tree("Change_Name",action);
         EXIT(1);
      }
      Change_Name(entity->DATA,architecture->DATA,action?action->DATA:NULL);
      break;
  case GEN_TOKEN_ARCHITECTURE:
      if (TOKEN(entity)!=GEN_TOKEN_ENTITY) {
         Error_Tree("Change_Name",entity);
         EXIT(1);
      }
      if (action && TOKEN(action)!=GEN_TOKEN_ACTION) {
         Error_Tree("Change_Name",action);
         EXIT(1);
      }
      Change_Name(entity->NEXT,architecture->NEXT,action?action->NEXT:NULL);
      break;
  case GEN_TOKEN_OF:
      if (action && TOKEN(action)!=GEN_TOKEN_OF) {
         Error_Tree("Change_Name",action);
         EXIT(1);
      }
      archi_name=getname(architecture->NEXT);
      Change_Name(entity,architecture->NEXT->NEXT->DATA,
                     action?action->NEXT->NEXT->DATA:NULL);
      break;
  case GEN_TOKEN_IDENT:
      if (action && TOKEN(action)!=GEN_TOKEN_IDENT) {
         Error_Tree("Change_Name",action);
         EXIT(1);
      }
      if (TOKEN(entity)!=GEN_TOKEN_IDENT) {
         Error_Tree("Change_Name",entity);
         EXIT(1);
      }
      memo_char=SEPAR;               /* MBK variable it is for concatenation*/
      SEPAR=' ';    /* nameindex() must be compatible with vectorradical()  */ 
      {
        char temp[255];
        sprintf(temp,"%s %s",(char*)entity->DATA, archi_name);
//        printf("> %s\n",archi_name);
        entity->DATA=(tree_list*)(void*) namealloc(temp); //nameindex((char*)entity->DATA,count);
        SEPAR=memo_char;              /* put last value in MBK environment */
        count++;
      }
      architecture->DATA=entity->DATA;
      if (action) action->DATA=entity->DATA;
      break;
  default:
    Error_Tree("Change_Name",architecture);
    EXIT(1); return;
  }
}
   
/***************************************************************************/
/* return number of instances in tree                                      */  
/***************************************************************************/
static inline int Count_Instances(tree_list *tree) 
{ 
  if (!tree) {
    return 0;
/*    fprintf(stderr,"Count_Instances: NULL pointer\n");
    EXIT(1);*/
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Count_Instances(tree->DATA);
    break;
  case ';':
    return Count_Instances(tree->NEXT)+Count_Instances(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_MAP:
    return 1;
    break;
  case GEN_TOKEN_IF:
    return Count_Instances(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_FOR:
    return Count_Instances(tree->NEXT->NEXT->NEXT->NEXT);
    break;
  default:
    Error_Tree("Count_Instances",tree);
    EXIT(2); return 0;
  }
}


/***************************************************************************/
/* return 1 if tree contains some some generics                            */  
/***************************************************************************/
static inline int Detect_Exclude(tree_list *tree) 
{ 
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 214);
//    fprintf(stderr,"Detect_Exclude: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Detect_Exclude(tree->DATA);
    break;
  case ';':
    return Detect_Exclude(tree->NEXT)
      || Detect_Exclude(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_MAP:
    return 0;
    break;
  case GEN_TOKEN_IF:
    return Detect_Exclude(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_EXCLUDE:
    return 1;
    break;
  case GEN_TOKEN_FOR:
    return Detect_Exclude(tree->NEXT->NEXT->NEXT->NEXT);
    break;
  default:
    Error_Tree("Detect_Exclude",tree);
    EXIT(2); return 0;
  }
}


/***************************************************************************/
/* verify connectors are (GEN_TOKEN_IN, GEN_TOKEN_INOUT, GEN_TOKEN_INOUT) so num should be 0          */
/* and Con=NULL                                                            */
/***************************************************************************/
static inline chain_list *Check_Transistor(tree_list *tree, int *num, char *rule,
                                    chain_list *Con)
{ 
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 215);
//    fprintf(stderr,"Check_Transistor: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Check_Transistor(tree->DATA,num,rule,Con);
    break;
  case ',':
    Con=Check_Transistor(tree->NEXT,num,rule,Con);
    if (*num==4) {
    avt_errmsg(GNS_ERRMSG, "147", AVT_ERROR, FILE_NAME(tree),LINE(tree),rule);
/*      fprintf(stderr,
      "%s:%d: predefined rule '%s': IN Grid, INOUT Source, INOUT Drain, IN Bulk\n",
               FILE_NAME(tree),LINE(tree),rule);*/
      Inc_Error();
      return Con;
    }  
    return Check_Transistor(tree->NEXT->NEXT,num,rule,Con);
    break;
  case GEN_TOKEN_SIGNAL:
    *num=*num+1;
    return Check_Transistor(tree->NEXT,num,rule,Con);
    break;
  case GEN_TOKEN_IN: 
    if (*num!=1 && *num!=4) { 
    avt_errmsg(GNS_ERRMSG, "147", AVT_ERROR, FILE_NAME(tree),LINE(tree),rule);
    /*  fprintf(stderr, "%s:%d: predefined rule '%s': IN Grid, INOUT Source, INOUT Drain, IN Bulk\n",
               FILE_NAME(tree),LINE(tree),rule);*/
      Inc_Error();
    }
    return Check_Transistor(tree->NEXT,num,rule,Con);
    break;
  case GEN_TOKEN_INOUT:
    if (*num!=2 && *num!=3) { 
    avt_errmsg(GNS_ERRMSG, "147", AVT_ERROR, FILE_NAME(tree),LINE(tree),rule);
    /*  fprintf(stderr, "%s:%d: predefined rule '%s': IN Grid, INOUT Source, INOUT Drain\n",
               FILE_NAME(tree),LINE(tree),rule);*/
      Inc_Error();
    }
    return Check_Transistor(tree->NEXT,num,rule,Con);
    break;
  case GEN_TOKEN_OUT: case GEN_TOKEN_TRANSCV: case GEN_TOKEN_TRISTATE:
    avt_errmsg(GNS_ERRMSG, "147", AVT_ERROR, FILE_NAME(tree),LINE(tree),rule);
/*    fprintf(stderr, "%s:%d: predefined rule '%s': IN Grid, INOUT Source, INOUT Drain, IN Bulk\n",
               FILE_NAME(tree),LINE(tree),rule);*/
    Inc_Error();
    return Check_Transistor(tree->NEXT,num,rule,Con);
    break;
  case '(':
    avt_errmsg(GNS_ERRMSG, "148", AVT_ERROR, FILE_NAME(tree),LINE(tree),rule);
/*    fprintf(stderr,"%s:%d: There is no vector in predefined rule '%s'\n",
            FILE_NAME(tree),LINE(tree),rule);*/
    Inc_Error();
    return Check_Transistor(tree->NEXT,num,rule,Con);
    break;
  case GEN_TOKEN_IDENT:
    /*control not the same name*/
    return put_inlist(tree,Con);
    break;
  default:
    Error_Tree("Check_Transistor",tree);
    EXIT(2); return NULL;
  }
}

   
/***************************************************************************/
/*     Verify that tree is component of pre-defined rule TN or TP          */
/***************************************************************************/
extern void Verif_Transistor(tree)
   tree_list *tree;
{
  static char *name=NULL; 
  int num;
  chain_list *Con;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 216);
//    fprintf(stderr,"Verif_Transistor: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    Verif_Transistor(tree->DATA);
    break;
  case GEN_TOKEN_COMPONENT:
    if (TOKEN(tree->NEXT->NEXT)==GEN_TOKEN_GENERIC) {
    avt_errmsg(GNS_ERRMSG, "149", AVT_ERROR, FILE_NAME(tree),LINE(tree),getname(tree->NEXT));
/*      fprintf(stderr,"%s:%d: The predefined entity '%s' don't have a generic\n",
               FILE_NAME(tree),LINE(tree),getname(tree->NEXT));*/
      Inc_Error();
    }
    name=getname(tree->NEXT);
    Verif_Transistor(tree->NEXT->NEXT->NEXT);
    break;
  case GEN_TOKEN_PORT:
    num=0;
    Con=Check_Transistor(tree->NEXT,&num,name,NULL);
    if (num!=4) {
    avt_errmsg(GNS_ERRMSG, "147", AVT_ERROR, FILE_NAME(tree),LINE(tree),name);
/*      fprintf(stderr, "%s:%d: predefined rule '%s': IN Grid, INOUT Source, INOUT Drain, IN Bulk\n",
               FILE_NAME(tree),LINE(tree),name);*/
      Inc_Error();
    }
    freechain(Con);
    break;
  default:
    Error_Tree("Verif_Transistor",tree);
    EXIT(2); return;
  }
}
  

/***************************************************************************/
/*              return the ident of a token signal                         */
/* if in a vector a variable isn't listed in env NULL is returned          */
/***************************************************************************/
static inline tree_list *Verif_Signal(tree_list *tree, ptype_list *env) 
{ 
  int Error;
  
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 217);
//    fprintf(stderr,"Verif_Signal: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_Signal(tree->DATA,env);
    break;
   case GEN_TOKEN_SIGNAL: 
    return Verif_Signal(tree->NEXT,env);
    break;
  case GEN_TOKEN_IN: case GEN_TOKEN_OUT: case GEN_TOKEN_INOUT: case GEN_TOKEN_TRANSCV: case GEN_TOKEN_TRISTATE: case GEN_TOKEN_LINKAGE:
    if (Verif_Signal(tree->NEXT,env)) return tree;
    else return NULL;
    break;
  case '(':
    Error=Get_Error();
    Verif_Vector(tree,env);
    if (Error==Get_Error()) return tree;
    else return NULL;
    break;
  case GEN_TOKEN_IDENT:
    return tree;
    break;
  default:
    Error_Tree("Verif_Signal",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*                     Verify if port is correct                           */
/* return list of connectors put on the top of Con                         */
/* env is environment where are listed known variables                     */
/***************************************************************************/
extern chain_list *Verif_Port(tree,env,Con) 
   tree_list *tree;
   ptype_list* env;
   chain_list *Con;
{ 
  tree_list *Sig;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 218);
//    fprintf(stderr,"Verif_Port: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_Port(tree->DATA,env,Con);
    break;
  case GEN_TOKEN_PORT:
    return Verif_Port(tree->NEXT,env,Con);
    break;
  case GEN_TOKEN_NOP:
    return NULL;
    break;
  case ',':
    Con=Verif_Port(tree->NEXT,env,Con);
    return Verif_Port(tree->NEXT->NEXT,env,Con);
    break;
  case GEN_TOKEN_SIGNAL:
    if (!tree->NEXT) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 219);
//      fprintf(stderr,"Verif_Port: NULL pointer\n");
      EXIT(1);
    }
    Sig=Verif_Signal(tree->NEXT,env);
    /*put Sig at the end of Con*/
    if (Sig) return put_inlist(Sig,Con);  
    else return Con;
  default:
    Error_Tree("Verif_Port",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*           return the token ident from a token variable                  */
/***************************************************************************/
static inline tree_list *Verif_Variable(tree_list *tree) 
{ 
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 220);
//    fprintf(stderr,"Verif_Variable: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_Variable(tree->DATA);
    break;
  case GEN_TOKEN_VARIABLE: 
    return Verif_Variable(tree->NEXT);
    break; 
  case GEN_TOKEN_INTEGER: 
    return tree;
    break;
  default:
    Error_Tree("Verif_Variable",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*        Verify if generic of component and model are equal               */
/* the list of variables is returned on top of Gen                         */
/***************************************************************************/
extern ptype_list *Verif_Generic(tree,Gen) 
   tree_list *tree;
   ptype_list *Gen;
{ 
  tree_list *Var;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 221);
//    fprintf(stderr,"Verif_Generic: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_Generic(tree->DATA,Gen);
    break;
  case GEN_TOKEN_GENERIC:
    return Verif_Generic(tree->NEXT,Gen);
    break;
  case GEN_TOKEN_NOP:
    return NULL;
    break;
  case ',':
    Gen=Verif_Generic(tree->NEXT,Gen);
    return Verif_Generic(tree->NEXT->NEXT,Gen);
    break;
  case GEN_TOKEN_VARIABLE:
    Var=Verif_Variable(tree->NEXT);
    if (Var) return put_value(Var,RAND_BYTE,Gen); /*put Var in Gen with value*/
    else return Gen;
    break;
  default:
    Error_Tree("Verif_Generic",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*         Verify if all signals in tree use variables listed in env       */
/* result is put on top of Sig                                             */
/***************************************************************************/
extern chain_list *Verif_All_Signals(tree,env,Sig) 
   tree_list *tree;
   ptype_list *env;
   chain_list *Sig;
{ 
  tree_list *new;
  /*char* name;*/

  if (!tree) {
    return Sig;
/*    fprintf(stderr,"Verif_All_Signals: NULL pointer\n");
    EXIT(1);*/
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_All_Signals(tree->DATA,env,Sig);
    break;
  case ';':
    Sig=Verif_All_Signals(tree->NEXT,env,Sig);
    return Verif_All_Signals(tree->NEXT->NEXT,env,Sig);
    break;
  case GEN_TOKEN_SIGNAL: 
    new=Verif_Signal(tree->NEXT,env);
    if (new) {
      return put_inlist(new,Sig);
    }  
    else return Sig;
    break;
  case GEN_TOKEN_COMPONENT:
    /*don't care*/
    return Sig;
    break;
  default:
    Error_Tree("Verif_All_Signals",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*   Verify if all components in tree match with their models in Entity    */
/* result is put on top of Comp                                            */
/***************************************************************************/
extern chain_list *Verif_All_Components(tree,Entity,Comp) 
   tree_list *tree;
   chain_list *Entity,*Comp;
{
  int Error;
  char *name;
  
  if (!tree) {
    return Comp; // there can be no components
/*    fprintf(stderr,"Verif_All_Components: NULL pointer\n");
    EXIT(1);*/
  }
  
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_All_Components(tree->DATA,Entity,Comp);
    break;
  case ';':
    Comp=Verif_All_Components(tree->NEXT,Entity,Comp);
    return Verif_All_Components(tree->NEXT->NEXT,Entity,Comp);
    break;
  case GEN_TOKEN_COMPONENT: 
    Error=Get_Error();              /*to detect new error */
    name=getname(tree->NEXT);
    if (mbk_istransn(name) || mbk_istransp(name)) {
      Verif_Transistor(tree);
    }
    else {
      tree_list *ent;
      ptype_list *env_instance=NULL,*env_model=NULL,*q,*p;
      chain_list *Con;
      ent=fetch_inlist(tree->NEXT,Entity);/*TOKEN(ent)=NODE*/
      if (!ent) return Comp;
      env_instance=Compare_Generic(tree->NEXT->NEXT,ent->NEXT,NULL);
/*sort between variables belonging to model and instance*/
      env_model=(ptype_list*)reverse((chain_list*)GenTree2chain(ent->NEXT,NULL));
      q=env_instance;
      /*to give same values to var of models*/
      for (p=env_model;p&&q;p=p->NEXT) {
         p->TYPE=q->TYPE;
         q=q->NEXT;
      }   
      {
        lofig_list *lf;
        chain_list *cl;
        CONLISTCL=NULL;
        
        Con=Compare_Port(tree->NEXT->NEXT->NEXT,ent->NEXT->NEXT,env_instance,env_model,NULL);

        if ((lf=getloadedlofig(name))!=NULL)
          {
            locon_list *lc;
//            printf("%s\n",name);
            for (lc=lf->LOCON; lc!=NULL; lc=lc->NEXT)
              {
                if (lc->DIRECTION!=UNKNOWN) continue;
                for (cl=CONLISTCL; cl!=NULL && lc->NAME!=((CONLIST *)cl->DATA)->name; cl=cl->NEXT) ;
                if (cl!=NULL && ((CONLIST *)cl->DATA)->dir!=' ')
                  {
                    lc->DIRECTION=((CONLIST *)cl->DATA)->dir;
//                    printf("=> %s '%c'\n",lc->NAME, lc->DIRECTION);
                  }
              }
          }
        for (cl=CONLISTCL; cl!=NULL; cl=cl->NEXT) mbkfree(cl->DATA);
        freechain(CONLISTCL);
      }
      freechain(Con);
      freeptype(env_model);    
      freeptype(env_instance);    
    }  
    if (Error==Get_Error()) return put_inlist(tree->NEXT/*I want NODE*/,Comp);
    else return Comp;
    break;
  case GEN_TOKEN_SIGNAL:
    /*don't care*/
    return Comp;
    break;
  default:
    Error_Tree("Verif_All_Components",tree);
    EXIT(2); return NULL;
  }
}

   
/***************************************************************************/
/*        Verify in tree all architecture modules attached to entity       */
/* result is put on top of Archi                                           */
/***************************************************************************/
extern chain_list *Verif_Architecture(tree,entity,Archi) 
   tree_list *tree;
   char *entity;
   chain_list *Archi;
{
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 222);
//    fprintf(stderr,"Verif_Architecture: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_Architecture(tree->DATA,entity,Archi);
    break;
  case ';':
    Archi=Verif_Architecture(tree->NEXT,entity,Archi);
    return Verif_Architecture(tree->NEXT->NEXT,entity,Archi);
    break;
  case GEN_TOKEN_ARCHITECTURE: 
    if (getname_of(tree->NEXT)!=entity) return Archi;
    else return put_inlist(tree->NEXT/*we want the NODE!!!*/,Archi);
  case GEN_TOKEN_ACTION: case GEN_TOKEN_ENTITY:
    /*not concerned*/
    return Archi;
    break;
  default:
    Error_Tree("Verif_Architecture",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*        Verify in tree all action modules attached to entity             */
/* result is put on top of Action                                          */
/***************************************************************************/
extern chain_list *Verif_Action(tree,entity,Action) 
   tree_list *tree;
   char *entity;
   chain_list *Action;
{
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 223);
//    fprintf(stderr,"Verif_Action: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_Action(tree->DATA,entity,Action);
    break;
  case ';':
    Action=Verif_Action(tree->NEXT,entity,Action);
    return Verif_Action(tree->NEXT->NEXT,entity,Action);
    break;
  case GEN_TOKEN_ACTION: 
    if (getname_of(tree->NEXT)!=entity) return Action;
    else return addchain(Action, tree); 
      //return put_inlist(tree->NEXT/* we want the NODE!!!*/,Action);
  case GEN_TOKEN_ARCHITECTURE: case GEN_TOKEN_ENTITY:
    /*not concerned*/
    return Action;
    break;
  default:
    Error_Tree("Verif_Action",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/*    Verify all entity declarations in tree and return the list of entity */
/* result is put on top of Ent                                             */
/***************************************************************************/
extern chain_list *Verif_Entity(tree,Ent) 
   tree_list *tree;
   chain_list *Ent;
{ 
  char *entity_name;
  int before;
  ptype_list *env;
  chain_list *Sig;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 223);
//    fprintf(stderr,"Verif_Entity: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return Verif_Entity(tree->DATA,Ent);
    break;
  case ';':
    Ent=Verif_Entity(tree->NEXT,Ent);
    return Verif_Entity(tree->NEXT->NEXT,Ent);
    break;
  case GEN_TOKEN_ENTITY:
    before=Get_Error();/*to detect new error*/
    entity_name=getname(tree->NEXT);
    if (mbk_istransp(entity_name) || mbk_istransn(entity_name)) {
    avt_errmsg(GNS_ERRMSG, "151", AVT_ERROR, FILE_NAME(tree->NEXT),LINE(tree->NEXT), entity_name);
/*         fprintf(stderr,
             "%s:%d: predefined rule '%s' impossible to use as a transistor name as an entity name\n",
             FILE_NAME(tree->NEXT),LINE(tree->NEXT), entity_name);*/
         Inc_Error();
         return Ent;
    }     
    env=Verif_Generic(tree->NEXT->NEXT,NULL);
    Sig=Verif_Port(tree->NEXT->NEXT->NEXT,env,NULL);
    freechain(Sig);
    freeptype(env);
    if (before==Get_Error()) return put_inlist(tree->NEXT/*I want NODE*/,Ent);
    return Ent;/*put in list only if correct*/
    break;
  case GEN_TOKEN_ACTION: case GEN_TOKEN_ARCHITECTURE:
    /* not yet */
    return Ent;
    break;
  default:
    Error_Tree("Verif_Entity",tree);
    EXIT(2); return NULL;
  }
}


/***************************************************************************/
/* verify if all actions and architectures in tree have their entity named */
/* in Ent                                                                  */
/* Ent is a list of tree                                                  */
/***************************************************************************/
extern void Verif_Alone(tree,Ent) 
   tree_list *tree;
   chain_list *Ent;
{
  tree_list *entity;
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 224);
//    fprintf(stderr,"Verif_Alone: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    Verif_Alone(tree->DATA,Ent);
    break;
  case ';':
    Verif_Alone(tree->NEXT,Ent);
    Verif_Alone(tree->NEXT->NEXT,Ent);
    break;
  case GEN_TOKEN_ARCHITECTURE: case GEN_TOKEN_ACTION: 
    entity=getident_of(tree->NEXT);
    if (TOKEN(tree)!=GEN_TOKEN_ACTION) fetch_inlist(entity,Ent);
    break;
  case GEN_TOKEN_ENTITY:
    /*still defined*/
    break;
  default:
    Error_Tree("Verif_Alone",tree);
    EXIT(2); return;
  }
}


/***************************************************************************/
static inline int Pure_Transistor(chain_list *Compo)
{
   chain_list *l;
   char *name;
   tree_list *component;
   
         for (l=Compo;l;l=l->NEXT) {
            component=(tree_list*)l->DATA;
            name=getname(component);
            if (!mbk_istransn(name) && !mbk_istransp(name)) return 0;
         }
         return 1;
}

//ht *architomodel=NULL;

/***************************************************************************/
/*              Verify coherence of VHDL and C describtion                 */
/* return 2 lists typed GENIUS and FCL with the name of built models       */
/***************************************************************************/
extern ptype_list *Verif_All(tree) 
   tree_list *tree;
{
   chain_list *Ent,*p,*q,*l,*Archi,*Act,*Sig,*Compo,*Inst;
   ptype_list *env;  /*values of generic variables*/
   tree_list  *entity/*,*action*/,*architecture;/*ponting on GEN_TOKEN_NODEs ent,act,arch*/
   chain_list *fcl_names=NULL,*genius_names=NULL;
                                       /*model for genius and fcl algorithms */
   ptype_list *ret=NULL;  /* return value */
   char *name,*newname;
   int several_Archi;
   lib_entry *le;

   if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 225);
      EXIT(1);   
   }
   Ent=Verif_Entity(tree,NULL);
   Verif_Alone(tree,Ent); /*which is without entity?*/

   for (p=Ent;p;p=p->NEXT) {
      entity=(tree_list*)p->DATA;
      name=getname(entity);
    
      env=Verif_Generic(entity->NEXT,NULL);
      Sig=Verif_Port(entity->NEXT->NEXT,env,NULL);
    
      /*action to extract Exclude*/
      Act=Verif_Action(tree,name,NULL);
      if (Act && Act->NEXT) {
    avt_errmsg(GNS_ERRMSG, "152", AVT_ERROR, FILE_NAME((tree_list*)Act->NEXT->DATA),LINE((tree_list*)Act->NEXT->DATA),name);
         Inc_Error();
      }
      /*architecture*/
      le=NULL;
      Archi=Verif_Architecture(tree,name,NULL);
      if (!Archi) {
        for (le=GENIUS_PRIORITY;le!=NULL && le->entity!=name; le=le->NEXT) ;
        if (le==NULL)
          {
    avt_errmsg(GNS_ERRMSG, "153", AVT_ERROR, FILE_NAME(entity),LINE(entity),name);
            Inc_Error();
          }
      }     
      
      if (le==NULL)
        {
          several_Archi=0;
          for (q=Archi;q;q=q->NEXT) 
            {
              chain_list *Sig_aux=NULL;
//              misc_stuffs *ms;
              architecture=(tree_list*)q->DATA;

          /*    ms=(misc_stuffs *)architecture->DATA->NEXT->NEXT->NEXT->NEXT->DATA->DATA;
              if (ms!=NULL)
                Expand_Exclude(ms->EXCLUDES);*/
         
              Compo=Verif_All_Components(architecture->NEXT,Ent,NULL);         
              for (l=Sig;l;l=l->NEXT) Sig_aux=addchain(Sig_aux,l->DATA);
              Sig_aux=Verif_All_Signals(architecture->NEXT,env,Sig_aux);
              Inst=Verif_Instance(architecture->NEXT->NEXT,Sig_aux,env,Compo,NULL);
//              Verif_Alone_Exclude(architecture);
              freechain(Inst);
              freechain(Sig_aux);
              
              /*if several architecture for the same entity --> duplicate entity*/
              if (several_Archi && Get_Error()==0) {
                int lineno=LINE(entity);
                char *file=FILE_NAME(entity);
                //            tree_list *new_entity=PUT_TRI(GEN_TOKEN_ENTITY,Duplicate_Tree(entity->DATA),Duplicate_Tree(entity->NEXT->DATA),Duplicate_Tree(entity->NEXT->NEXT->DATA));
                tree_list *new_entity=PUT_QUA(GEN_TOKEN_ENTITY,Duplicate_Tree(entity->DATA),Duplicate_Tree(entity->NEXT->DATA),Duplicate_Tree(entity->NEXT->NEXT->DATA),Duplicate_Tree(entity->NEXT->NEXT->NEXT->DATA));
                // [!] zinaps: les informartions de symétrie ne sont pas dupliquées
//                tree_list *new_action=NULL;
                if (Act) {
#if 0
                  action=(tree_list*)Act->DATA;  /*TOKEN=GEN_TOKEN_NODE*/
                  new_action=PUT_BIN(GEN_TOKEN_ACTION,Duplicate_Tree(action->DATA),
                                     Duplicate_Tree(action->NEXT->DATA));
                  tree->NEXT->NEXT->DATA=
                    PUT_BIN(';',new_action,tree->NEXT->NEXT->DATA);
#endif
                }
                Change_Name(new_entity->NEXT,architecture,
                            /*new_action?new_action->NEXT:*/NULL);
                tree->NEXT->NEXT->DATA=
                  PUT_BIN(';',new_entity,tree->NEXT->NEXT->DATA);
                newname=getname(new_entity->NEXT);
//                if (Act) addhtitem(architomodel, newname, (long)Act->DATA);
              }
              else newname=name;
              /*flag*/
              if (Archi->NEXT) several_Archi=1;
              
              /* to know if this model could be searched by FCL algorithm */
              if (!env) { /* it means that there is no generic */
                if (Pure_Transistor(Compo)) {/*transistor netlist only for fcl*/
                  if (Count_Instances(architecture->NEXT->NEXT)>TRANS_NUM_SUP) {
    avt_errmsg(GNS_ERRMSG, "154", AVT_ERROR, FILE_NAME(architecture),LINE(architecture),TRANS_NUM_SUP,name);
                    Inc_Error();
                  }
                  if (Count_Instances(architecture->NEXT->NEXT)!=0 && Detect_Exclude(architecture->NEXT->NEXT)) {
    avt_errmsg(GNS_ERRMSG, "155", AVT_ERROR, FILE_NAME(architecture),LINE(architecture),name);
                    Inc_Error();
                  }
                  /* memorize model for fcl algorithm */
                  if (Get_Error()==0) fcl_names=addchain(fcl_names,newname);    
                }   
                else {/* there is a hierarchy but could be flattened */
                  /* but we don't do it to have a hierarchy */
                  if (!Detect_Exclude(architecture->NEXT->NEXT)) {
                    /* it is good for fcl. BUT we prefer to memorize it for genius*/
                    if (Get_Error()==0) genius_names=addchain(genius_names,newname);
                  }
                  else /* it is an exclude so it must be for genius */
                    if (Get_Error()==0) genius_names=addchain(genius_names,newname);
                }                     
              }
              else {  /* there are generic variables --> for Genius algorithm*/
                if (Pure_Transistor(Compo)) {/* I can't putre transistor netlist */

                  if (Compo==NULL)
                    {
                      // test blackbox, ajout dans fcl
    avt_errmsg(GNS_ERRMSG, "156", AVT_ERROR, FILE_NAME(architecture),LINE(architecture));
                      if (Get_Error()==0) fcl_names=addchain(fcl_names,newname);
                    }
                  else
                    {
                      
    avt_errmsg(GNS_ERRMSG, "157", AVT_ERROR, FILE_NAME(architecture),LINE(architecture),name);
                      Inc_Error();
                    }
                }
                else
                  /* memorize model for genius algorithm */
                  if (Get_Error()==0) genius_names=addchain(genius_names,newname);
              }   
            }
          
          freechain(Compo);
        }
      else if (le->format==2)
        genius_names=addchain(genius_names,le->entity);
      else
        fcl_names=addchain(fcl_names,le->entity);

      freechain(Archi);
      freechain(Act);
      Init_Exclude(NULL);  /*free memory*/ 
      freeptype(env);
      freechain(Sig);
   }/*end of loop on Ent*/
   freechain(Ent);
   
   if (genius_names) ret=addptype(NULL,GENIUS,reverse(genius_names));
   if (fcl_names) ret=addptype(ret,FCL,reverse(fcl_names));
   return ret;   
}

