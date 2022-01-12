/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_execute_C.c                                             */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 04/05/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <setjmp.h>
#include <tcl.h>
#undef STRINGIFY

#include MUT_H
//#include MLO_H
#include API_H
#include AVT_H
#include "gen_tree_utils.h"
//#include "gen_env.h"
#include "gen_tree_errors.h"
//#include "gen_tree_parser.h"
#include "gen_verif_C.h"
#include "gen_execute_C.h"



// maximum number of arguments in a function call, it should be sufficient, no?
#define MAX_ARGS 64

// intermediate variable types
#define INTERM "intermediate result"
#define CONDITION "condition result"
#define CONSTANTE "constant value"

static 
struct EXE_CTX
{
  int STATIC_FLAG;                /* are we static? */
  //static static_list* STATIC_LIST;        /*list of static variable with value*/
  int RETURN_FLAG;        /* to count loop for return */
  t_arg *RETURN_VALUE;        /* to count loop for return */
  tree_list *RETURN_TREE;
  int BREAK_FLAG;        /* to memorize a break*/
  int LOOP_COUNT;        /* to memorize a return */
  chain_list *UPDATE;        /*for special commande i++ and i-- */
  chain_list *TO_FREE;   /*allocation to free at the end of program*/
  tree_list *current_tree;
  chain_list *warnings/*, *globalvars=NULL*/;
  chain_list *BLOCK_ENV_LIMIT;
  ExecutionContext *GLOBAL_EC;
} CTX;

static ExecutionContext *CUR_CTX=NULL;
static int api_dupstringmode=0;
ExecutionContext *APIGetExecutionContext()
{
  return CUR_CTX;
}

chain_list *UNKNOWN_TYPES=NULL;

char *GetUnknownType(char *name)
{
  chain_list *cl;
  for (cl=UNKNOWN_TYPES;cl!=NULL && strcmp((char *)cl->DATA,name)!=0;cl=cl->NEXT) ;
  if (cl!=NULL) { mbkfree(name); return (char *)cl->DATA; }
  UNKNOWN_TYPES=addchain(UNKNOWN_TYPES, name);
  return name;
}


static t_arg *Eval_Exp_C(tree_list *tree, chain_list *env);

typedef struct update_s
{
  void *zone;
  int type;
  int inc;
} update_s;

void addupdate(chain_list **pt, void *zone, int type, int inc)
{
  update_s *s=(update_s *)mbkalloc(sizeof(update_s));
  s->zone=zone;
  s->type=type;
  s->inc=inc;
  *pt=addchain(*pt,s);
}



// gen_info() is accessible to the dynamic library functions
extern char *gen_info()
{
  static char r[255];
  if (CTX.current_tree!=NULL)
    sprintf(r,"%s:%d",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
  else
    sprintf(r,"somewhere:");
  return r;
}

// _________________________________________________

static int STR2TYPE(char *name)
{
  if (strcmp(name,"int")==0) return T_INT;
  if (strcmp(name,"long")==0) return T_LONG;
  if (strcmp(name,"char")==0) return T_CHAR;
  if (strcmp(name,"double")==0) return T_DOUBLE;
  return T_POINTER;
}

static int allocfortarg_multiplier=0; // for arrays
void *allocfortarg(char *type, int pointer)
{
  int tmp=allocfortarg_multiplier;
  if (tmp==0) tmp=1;

  if (pointer>0)
    return mbkalloc(sizeof(void *)*tmp);

  switch(STR2TYPE(type))
    {
    case T_LONG:
      return mbkalloc(sizeof(long)*tmp);
    case T_INT:
      return mbkalloc(sizeof(int)*tmp);
    case T_CHAR:
      return mbkalloc(sizeof(char)*tmp);
    case T_DOUBLE:
      return mbkalloc(sizeof(double)*tmp);
    default:
      avt_errmsg(API_ERRMSG, "020", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),type);
//      fprintf(stderr,"%s:%d: undefined type '%s'\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),type);
      EXIT(1); return NULL;
    }
}

int sizeoftarg(char* type, int pointer)
{
  if (pointer>0)
    return sizeof(void *);

  switch(STR2TYPE(type))
    {
    case T_LONG:
      return sizeof(long);
    case T_INT:
      return sizeof(int);
    case T_CHAR:
      return sizeof(char);
    case T_DOUBLE:
      return sizeof(double);
    default:
      avt_errmsg(API_ERRMSG, "020", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),type);
      //fprintf(stderr,"%s:%d: sizeoftarg: undefined type\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
      EXIT(1); return 0;
    }
}

int sizeofpointed(char* type, int pointer)
{
  if (pointer>1)
    return sizeof(void *);

  if (pointer==1)
    switch(STR2TYPE(type))
      {
      case T_LONG:
        return sizeof(long);
      case T_INT:
        return sizeof(int);
      case T_CHAR:
        return sizeof(char);
      case T_DOUBLE:
        return sizeof(double);
      default:
      avt_errmsg(API_ERRMSG, "021", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),type);
      //  fprintf(stderr,"%s:%d: sizeof(%s) is unknown\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),type);
        EXIT(1);
      }
  return 0;
}

char *deref(int number)
{
  char *t=mbkalloc(sizeof(char)*20);
  int i;
  for (i=0;i<number && i<16;i++) t[i]='*';
  if (number>=16) t[i]=t[i+1]=t[i+2]='.',t[i+3]='\0';
  else t[i]='\0';
  return t;
}

void assigntarg(t_arg *env, t_arg *p)
{
  if (strcmp(p->NAME,"NULL")==0 && env->POINTER>0)
    {
      memcpy(env->VALUE,p->VALUE,sizeoftarg(p->TYPE,p->POINTER));
      return;
    }
  if (env->POINTER>0 && p->POINTER>0 && strcmp(env->TYPE,p->TYPE)!=0 && strcmp(env->TYPE,"void")!=0 && strcmp(p->TYPE,"void")!=0)
    {
      chain_list *ch;
      for (ch=CTX.warnings;ch!=NULL && (long)ch->DATA!=LINE(CTX.current_tree); ch=ch->NEXT) ;
      if (ch==NULL)
        {
          avt_errmsg(API_ERRMSG, "022", AVT_WARNING, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), p->TYPE,deref(p->POINTER), env->TYPE,deref(env->POINTER));
/*          fprintf(stderr,"%s:%d: warning: assignment from %s%s to %s%s without a cast\n",
                  FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
                  p->TYPE,deref(p->POINTER),
                  env->TYPE,deref(env->POINTER)
                  );*/
          CTX.warnings=addchain(CTX.warnings,(void *)(long)LINE(CTX.current_tree));
        }
      memcpy(env->VALUE,p->VALUE,sizeoftarg(p->TYPE,p->POINTER));
      return;
    }

  if (env->POINTER==0 && p->POINTER==0)
    {
      if (strcmp(env->TYPE,"int")==0 && strcmp(p->TYPE,"char")==0)
        {
          *(int *)env->VALUE=(int)*(char *)p->VALUE;
          return;
        }
      if (strcmp(env->TYPE,"char")==0 && strcmp(p->TYPE,"int")==0)
        {
          *(char *)env->VALUE=(char)*(int *)p->VALUE;
          return;
        }
    }
  if ((strcmp(env->TYPE,p->TYPE)!=0 && !(strcmp(env->TYPE,"void")==0 && env->POINTER>0 && p->POINTER>0))
      || p->POINTER!=env->POINTER)
    {
      avt_errmsg(API_ERRMSG, "023", AVT_FATAL,  FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), env->TYPE,deref(env->POINTER), p->TYPE,deref(p->POINTER));
/*      fprintf(stderr,"%s:%d: incompatible type assignment %s%s != %s%s\n",
              FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
              env->TYPE,deref(env->POINTER),
              p->TYPE,deref(p->POINTER)
              );*/
      EXIT(1);
    }
  if (api_dupstringmode && strcmp(p->TYPE,"char")==0 && p->POINTER==1)
   *(char **)env->VALUE=sensitive_namealloc(*(char **)p->VALUE);
  else
   memcpy(env->VALUE,p->VALUE,sizeoftarg(p->TYPE,p->POINTER));
}

void assigntargtab(t_arg *env, t_arg *p, int index)
{
  char *elem;
  if (env->POINTER==0)
    {
      avt_errmsg(API_ERRMSG, "024", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), env->NAME);
/*      fprintf(stderr,"%s:%d: variable '%s' can not be indexed\n",
              FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
              env->NAME
              );*/
      EXIT(1);
    }
  if (strcmp(p->NAME,"NULL")==0 && env->POINTER>0)
    {
      memcpy(env->VALUE,p->VALUE,sizeoftarg(p->TYPE,p->POINTER));
      return;
    }

  if (env->POINTER>0 && p->POINTER-1>0 && strcmp(env->TYPE,p->TYPE)!=0 && strcmp(env->TYPE,"void")!=0 && strcmp(p->TYPE,"void")!=0)
    {
      chain_list *ch;
      for (ch=CTX.warnings;ch!=NULL && (long)ch->DATA!=LINE(CTX.current_tree); ch=ch->NEXT) ;
      if (ch==NULL)
        {
          avt_errmsg(API_ERRMSG, "022", AVT_WARNING,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), p->TYPE,deref(p->POINTER), env->TYPE,deref(env->POINTER));
/*
          fprintf(stderr,"%s:%d: warning: assignment from %s%s to %s%s without a cast\n",
                  FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
                  p->TYPE,deref(p->POINTER),
                  env->TYPE,deref(env->POINTER)
                  );*/
          CTX.warnings=addchain(CTX.warnings,(void *)(long)LINE(CTX.current_tree));
        }
      memcpy(env->VALUE,p->VALUE,sizeoftarg(p->TYPE,p->POINTER-1));
      return;
    }

  if (env->POINTER==0 && p->POINTER-1==0)
    {
      if (strcmp(env->TYPE,"int")==0 && strcmp(env->TYPE,"char")==0)
        {
          *(int *)env->VALUE=(int)*(char *)p->VALUE;
          return;
        }
      if (strcmp(env->TYPE,"char")==0 && strcmp(env->TYPE,"int")==0)
        {
          *(char *)env->VALUE=(char)*(int *)p->VALUE;
          return;
        }
    }

  if ((strcmp(env->TYPE,p->TYPE)!=0 && !(strcmp(env->TYPE,"void")==0 && env->POINTER>1 && p->POINTER>0))
      || p->POINTER!=env->POINTER-1)
    {
      avt_errmsg(API_ERRMSG, "023", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), env->TYPE,deref(env->POINTER),p->TYPE,deref(p->POINTER));
/*      
      fprintf(stderr,"%s:%d: incompatible type assignment %s%s != %s%s\n",
              FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
              env->TYPE,deref(env->POINTER),
              p->TYPE,deref(p->POINTER)
              );*/
      EXIT(1);
    }
  elem=*(char **)env->VALUE;
  
  memcpy(&elem[sizeoftarg(p->TYPE,p->POINTER)*index],p->VALUE,sizeoftarg(p->TYPE,p->POINTER));
}

// search for a t_arg defined in the block scope, the block is starting at limit
t_arg *blocklimited_gettarg(char *name, chain_list *env, chain_list *limit)
{
  chain_list *cl;
  for (cl=env; cl!=limit; cl=cl->NEXT)
    if (((t_arg *)cl->DATA)->NAME==name) break;
  if (cl==limit) return NULL;
  return (t_arg *)cl->DATA;
}

/****************************************************************************/
/*                   erase recursively a static chain                       */
/****************************************************************************/
static inline void freestatic(static_list *list)
{
  if (!list) return;

  freestatic(list->NEXT);
  mbkfree(list);
}


/****************************************************************************/
/*           return the last value of static variable                       */
/****************************************************************************/
static inline void *addstatic(tree_list* tree, t_arg *env)
{
  static_list *p;
  int line;
  char *filename,*name;
   
  if (!tree || !env) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 86);
    //fprintf(stderr,"addstatic: NULL pointer\n");
    EXIT(1);
  }
  line=LINE(tree);
  filename=FILE_NAME(tree);
  name=getname(tree);

   
  for (p=CTX.GLOBAL_EC->STATIC_LIST; p; p=p->NEXT) {
    /*case same acton*/
    if (p->TREE==tree) break;
    /*case action duplicated for double architecture*/
    if (LINE(p->TREE)==line && FILE_NAME(p->TREE)==filename) {
      if (TOKEN(p->TREE)!=GEN_TOKEN_IDENT) {
        Error_Tree("addstatic",p->TREE);
        EXIT(2);
      }
      if (getname(p->TREE)==name) break;
    }   
  }   

  if (!p) {
    p=(static_list*) mbkalloc(sizeof(static_list));
    p->NEXT=CTX.GLOBAL_EC->STATIC_LIST;
    p->VALUE=allocfortarg(env->TYPE, env->POINTER);
    memcpy(p->VALUE,env->VALUE,sizeoftarg(env->TYPE,env->POINTER));
    p->TREE=tree;
    p->VARIABLE=env;
    CTX.GLOBAL_EC->STATIC_LIST=p;
    return env->VALUE;
  }

  p->VARIABLE=env;
  memcpy(env->VALUE,p->VALUE,sizeoftarg(env->TYPE,env->POINTER));
  return p->VALUE;
}


/****************************************************************************/
/*            memorize last value to static variable                        */
/****************************************************************************/
static inline void refresh_static(t_arg /*ptype_list*/ *env)
{
  static_list *p;
   
  if (!env) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 87);
//    fprintf(stderr,"addstatic: NULL pointer\n");
    EXIT(1);
  }

  for (p=CTX.GLOBAL_EC->STATIC_LIST; p; p=p->NEXT) if (p->VARIABLE /*ENV*/==env) break;
  if (p) 
    {
      memcpy(p->VALUE,env->VALUE,sizeoftarg(p->VARIABLE->TYPE,p->VARIABLE->POINTER));
      p->VARIABLE=NULL;
    }   
}


/****************************************************************************/
/*   return the new env with modififications of UPDATE                      */
/*modif is list with field DATA is an elem of env and TYPE the number to add*/
/****************************************************************************/
static inline chain_list *Update_Var(chain_list /*ptype_list*/ *env) {
  chain_list *p;
  update_s *s;
  for (p=CTX.UPDATE;p;p=p->NEXT) {
    s=(update_s *)p->DATA;
    switch(s->type)
      {
      case T_LONG:
        *((long *)s->zone)+=s->inc; break;
      case T_INT:
        *((int *)s->zone)+=s->inc; break;
      case T_CHAR:
        *((char *)s->zone)+=s->inc; break;
      case T_DOUBLE:
        *((double *)s->zone)+=(double)s->inc; break;
      case T_POINTER:
        *((char **)s->zone)+=s->inc; break;
      default:
        avt_errmsg(API_ERRMSG, "025", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
/*        fprintf(stderr,"%s:%d: Update_Var: forbiden operation on this variable type\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));*/
        EXIT(1);
      }
    mbkfree(s);
  }   
  if (CTX.UPDATE) freechain(CTX.UPDATE);
  CTX.UPDATE=NULL;
  return env;
}

// Add a numeric value to a t_arg
static void addtarg(t_arg *ta, int value) 
{
  if (ta->POINTER>0)
    *((char **)ta->VALUE)+=value;
  else
    switch(STR2TYPE(ta->TYPE))
      {
      case T_LONG:
        *((long *)ta->VALUE)+=value; break;
      case T_INT:
        *((int *)ta->VALUE)+=value; break;
      case T_CHAR:
        *((char *)ta->VALUE)+=value; break;
      case T_DOUBLE:
        *((double *)ta->VALUE)+=(double)value; break;
      default:
        avt_errmsg(API_ERRMSG, "025", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
/*        fprintf(stderr,"%s:%d: Update_Var: forbiden operation on this variable type\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));*/
        EXIT(1);
      }
}

// create a t_arg given its type
t_arg *newtarg(char *type, int pointer, char *name)
{
  t_arg *tmp=(t_arg *)mbkalloc(sizeof(t_arg));
  if (allocfortarg_multiplier==0)
    tmp->VALUE=allocfortarg(type,pointer);
  else
    {
      void *tmp0=allocfortarg(type,pointer-1);
      tmp->VALUE=mbkalloc(sizeof(void *));
      *(void **)tmp->VALUE=tmp0;
      CTX.TO_FREE=addchain(CTX.TO_FREE, tmp0);
    }
  tmp->TYPE=type;
  tmp->NAME=name;
  tmp->POINTER=pointer;
  return tmp;
}

// if flag is not set, free only intermediate variable
void freetarg(t_arg *ta, int flag)
{
  if (flag || (!flag && (
          strcmp(ta->NAME, CONSTANTE)==0 ||
          strcmp(ta->NAME, INTERM)==0 ||
          strcmp(ta->NAME, CONDITION)==0 ||
          strcmp(ta->NAME,"NULL")==0 ||
          strcmp(ta->NAME,"?")==0)))
    {
      if (ta->VALUE!=NULL) mbkfree(ta->VALUE);
      mbkfree(ta);
    }
}

// pour le cast implicite des donnees

static char *getbesttype(t_arg *ta, t_arg *tb)
{
  int curtypea, curtypeb, besttypea=-1, besttypeb=-1;

  if (ta->POINTER>0 || tb->POINTER>0) return NULL;
  curtypea=STR2TYPE(ta->TYPE);
  curtypeb=STR2TYPE(tb->TYPE);
  if (curtypea==curtypeb) return NULL;

  switch(curtypea)
    {
    case T_CHAR: besttypea=1; break;
    case T_INT: besttypea=2; break;
    case T_LONG: besttypea=3; break;
    case T_DOUBLE: besttypea=4; break;
    }
  switch(curtypeb)
    {
    case T_CHAR: besttypeb=1; break;
    case T_INT: besttypeb=2; break;
    case T_LONG: besttypeb=3; break;
    case T_DOUBLE: besttypeb=4; break;
    }

  if (besttypeb>besttypea) besttypea=besttypeb;

  switch(besttypea)
    {
    case 1: return "char";
    case 2: return "int";
    case 3: return "long";
    case 4: return "double";
    }
  return NULL;
}

static double getval(t_arg *ta)
{
  switch(STR2TYPE(ta->TYPE))
    {
    case T_LONG:
      return (double)*((long *)ta->VALUE);
    case T_INT:
      return (double)*((int *)ta->VALUE);
    case T_CHAR:
      return (double)*((char *)ta->VALUE);
    case T_DOUBLE:
      return *((double *)ta->VALUE);
    }
  return 0.0;
}

static t_arg *setvalue(t_arg *ta, char *type)
{
  t_arg *tmp;
  double val;

  if (ta->POINTER>0 || strcmp(ta->TYPE,type)==0) return ta;

  tmp=newtarg(type, 0, INTERM);
  val=getval(ta);

  switch (STR2TYPE(type))
    {
    case T_CHAR:
      *(char *)tmp->VALUE=(char)val; break;
    case T_INT:
      *(int *)tmp->VALUE=(int)val; break;
    case T_LONG:
      *(long *)tmp->VALUE=(long)val; break;
    case T_DOUBLE:
      *(double *)tmp->VALUE=val; break;
    }
  freetarg(ta,0);
  return tmp;
}

static void setsametype(t_arg **ta, t_arg **tb)
{
  char *bt;
  if ((bt=getbesttype(*ta, *tb))==NULL) return;
  *ta=setvalue(*ta, bt);
  *tb=setvalue(*tb, bt);
}

// ----------------

// verify the type of a t_arg
void asserttargistype(t_arg *ta,int type, int pointerflag)
{
  if (STR2TYPE(ta->TYPE)!=type)
    {
      avt_errmsg(API_ERRMSG, "026", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME);
/*      fprintf(stderr,"%s:%d: '%s' has an unexpected type\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME);*/
      EXIT(1);
    }
  if (!pointerflag && ta->POINTER>0)
    {
      avt_errmsg(API_ERRMSG, "027", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME);
/*      fprintf(stderr,"%s:%d: '%s' should not be pointers\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME);*/
      EXIT(1);
    }
}

// verify 2 t_args are compatible (same type)
void asserttargsametype(t_arg *ta, t_arg *ta0, int pointerflag)
{
  if (strcmp(ta->TYPE,ta->TYPE)!=0 || ta->POINTER!=ta0->POINTER)
    {
      avt_errmsg(API_ERRMSG, "028", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME,ta0->NAME);
      /*fprintf(stderr,"%s:%d: type of '%s' and '%s' mismatch\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME,ta0->NAME);*/
      EXIT(1);
    }
  if (!pointerflag && ta->POINTER>0)
    {
      avt_errmsg(API_ERRMSG, "029", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME,ta0->NAME);
/*      fprintf(stderr,"%s:%d: '%s' and '%s' should not be pointers\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME,ta0->NAME);*/
      EXIT(1);
    }
}
// check if a t_arg is not of a certain type
void asserttargisnottype(t_arg *ta,int type)
{
  if (STR2TYPE(ta->TYPE)==type)
    {
      avt_errmsg(API_ERRMSG, "030", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME);
/*      fprintf(stderr,"%s:%d: '%s' has an unexpected type\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ta->NAME);*/
      EXIT(1);
    }
}



t_arg *gettarg(char *name, chain_list *env)
{
  chain_list *cl;
  t_arg *tmp;
 
  for (cl=env; cl!=NULL; cl=cl->NEXT)
    if (((t_arg *)cl->DATA)->NAME==name) break;

  if (cl==NULL) 
    {
      for (cl=API_DEFINES; cl!=NULL; cl=cl->NEXT)
        if (strcmp(((t_arg *)cl->DATA)->NAME,name)==0) break;

      if (cl!=NULL) 
        {
          tmp=(t_arg *)cl->DATA;
          tmp=newtarg(tmp->TYPE, tmp->POINTER, INTERM);
          assigntarg(tmp,(t_arg *)cl->DATA);
          return tmp;
        }
      return NULL;
    }
  return (t_arg *)cl->DATA;
}


// transparent IF condition testing of a t_arg (t_arg->value!=0)
int testtarg(t_arg *left)
{
  if (left->POINTER>0)
    {
      if (*((void **)left->VALUE)) return 1;
      return 0;
    }

  switch(STR2TYPE(left->TYPE))
    {
    case T_LONG:
      if (*((long *)left->VALUE)) return 1;
      return 0;
    case T_INT:
      if (*((int *)left->VALUE)) return 1;
      return 0;
    case T_CHAR:
      if (*((char *)left->VALUE)) return 1;
      return 0;
    default:
      avt_errmsg(API_ERRMSG, "031", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),left->TYPE);
/*      fprintf(stderr,"%s:%d: Unautorized test on type '%s'\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),left->TYPE);*/
      EXIT(1);
    }
  return 0;
}

t_arg *gen_docast(char *type, int pointer, t_arg *ta)
{
  t_arg *tmp = NULL;
  if (ta->POINTER>0 && pointer>0)
    {
      tmp=newtarg(type, pointer, INTERM);
      *(void **)tmp->VALUE=*(void **)ta->VALUE;
    }
  else
    if (ta->POINTER==0 && pointer==0)
      {
        if (STR2TYPE(type)==T_POINTER || STR2TYPE(ta->TYPE)==T_POINTER)
          {
            avt_errmsg(API_ERRMSG, "032", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), ta->TYPE, type);
                            
/*            fprintf(stderr,"%s:%d: interpreter can not cast %s to %s\n",
                    FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
                    ta->TYPE, type);*/
            EXIT(1);
          }
        tmp=newtarg(type, pointer, INTERM);
        switch (STR2TYPE(type))
          {
          case T_CHAR:
            switch (STR2TYPE(ta->TYPE))
              {
              case T_CHAR:
                *(char *)tmp->VALUE=*(char *)ta->VALUE; break;
              case T_INT:
                *(char *)tmp->VALUE=(char)*(int *)ta->VALUE; break;
              case T_LONG:
                *(char *)tmp->VALUE=(char)*(long *)ta->VALUE; break;
              case T_DOUBLE:
                *(char *)tmp->VALUE=(char)*(double *)ta->VALUE; break;
              }
            break;
          case T_INT:
            switch (STR2TYPE(ta->TYPE))
              {
              case T_CHAR:
                *(int *)tmp->VALUE=(int)*(char *)ta->VALUE; break;
              case T_INT:
                *(int *)tmp->VALUE=*(int *)ta->VALUE; break;
              case T_LONG:
                *(int *)tmp->VALUE=(int)*(long *)ta->VALUE; break;
              case T_DOUBLE:
                *(int *)tmp->VALUE=(int)*(double *)ta->VALUE; break;
              }
            break;
          case T_LONG:
            switch (STR2TYPE(ta->TYPE))
              {
              case T_CHAR:
                *(long *)tmp->VALUE=(long)*(char *)ta->VALUE; break;
              case T_INT:
                *(long *)tmp->VALUE=(long)*(int *)ta->VALUE; break;
              case T_LONG:
                *(long *)tmp->VALUE=*(long *)ta->VALUE; break;
              case T_DOUBLE:
                *(long *)tmp->VALUE=(long)*(double *)ta->VALUE; break;
              }
            break;
          case T_DOUBLE:
            switch (STR2TYPE(ta->TYPE))
              {
              case T_CHAR:
                *(double *)tmp->VALUE=(double)*(char *)ta->VALUE; break;
              case T_INT:
                *(double *)tmp->VALUE=(double)*(int *)ta->VALUE; break;
              case T_LONG:
                *(double *)tmp->VALUE=(double)*(long *)ta->VALUE; break;
              case T_DOUBLE:
                *(double *)tmp->VALUE=*(double *)ta->VALUE; break;
              }
            break;
          }
      }
    else
      {
        if (ta->POINTER==0 && STR2TYPE(ta->TYPE)==T_LONG && pointer>0)
          {
            tmp=newtarg(type, pointer, INTERM);
            *(void **)tmp->VALUE=(void *)*(long *)ta->VALUE;
          }
        else
          if (ta->POINTER>0 && STR2TYPE(type)==T_LONG && pointer==0)
            {
              tmp=newtarg(type, pointer, INTERM);
              *(long *)tmp->VALUE=(long)*(void **)ta->VALUE;
            }
          else
            {
              if (STR2TYPE(type)==T_POINTER || STR2TYPE(ta->TYPE)==T_POINTER)
                {
                  avt_errmsg(API_ERRMSG, "032", AVT_FATAL, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), ta->TYPE, type);
/*                  fprintf(stderr,"%s:%d: interpreter can not cast %s to %s\n",
                          FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
                          ta->TYPE, type);*/
                  EXIT(1);
                }
            }
      }
  freetarg(ta,0);
  return tmp;
}



/****************************************************************************/
/*                    return the value of tree                              */
/*   Rq:environment should be set                                           */
/* UPDATE and TO_FREE should be set                                         */
/****************************************************************************/


int GenCatchFunction(t_arg **ret, t_arg **tab, int nb, char *name)
{
  tree_list *tree;
  chain_list *ch;
#if 0
  chain_list *cl;
#endif // ..anto..
  t_arg *ret_value;

  if (strcmp(name,"malloc")==0)
    {
      if (nb<1)
        {
          avt_errmsg(API_ERRMSG, "033", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
          // fprintf(stderr,"%s:%d: too few arguments for function 'malloc'\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
          EXIT(1);
        }
      if (nb>1)
        {
          avt_errmsg(API_ERRMSG, "034", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
//          fprintf(stderr,"%s:%d: too many arguments in function 'malloc'\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
          EXIT(1);
        }
      tab[0]=setvalue(tab[0], "int");
      asserttargistype(tab[0], T_INT, 0);
      *ret=newtarg("void", 1, INTERM);
      *(void **)(*ret)->VALUE=malloc(*(int *)tab[0]->VALUE);
      return 1;
    } 
  else if (strcmp(name,"callfunc")==0)
    {
      APICallFunc *acf;
      if (nb<1)
        {
          avt_errmsg(API_ERRMSG, "035", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
          //fprintf(stderr,"%s:%d: too few arguments for function 'callfunc'\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
          EXIT(1);
        }
      asserttargistype(tab[0], T_CHAR, 1);
/*
      for (i = 1; i < nb; i++) 
        {
          if (tab[i]->POINTER>0)
            {
              if (tab[i]->POINTER==1 && strcmp(tab[i]->TYPE,"char")==0)
              {
                    printf("%p %s\n", *(char **)tab[i]->VALUE, *(char **)tab[i]->VALUE);
                *(void **)tab[i]->VALUE = sensitive_namealloc(*(char **)tab[i]->VALUE);
              }
              else
                {
                  avt_errmsg(API_ERRMSG, "036", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
                  //fprintf(stderr,"%s:%d: callfunc: only 'char *' pointer type can be used in function call\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
                  EXIT(3);
                }
            }
        }
*/        
      acf=(APICallFunc *)mbkalloc(sizeof(APICallFunc));
      acf->NAME=sensitive_namealloc(*(char **)tab[0]->VALUE);
      api_dupstringmode=1;
      acf->ARGS=APIPrepareFunctionARGS(NULL, &tab[1], nb-1, NULL);      
      api_dupstringmode=0;
      *ret=newtarg("void", 1, INTERM);
      *(void **)(*ret)->VALUE=acf;
      return 1;
    }
  else if ((tree=APIGetCFunction(CTX.GLOBAL_EC, name, 'y'))!=NULL)
    {
#if 0
      int count;
#endif // ..anto..
      //      printf("found tree for '%s'\n",name);
      ch=APIPrepareFunctionARGS(tree, tab, nb, CTX.current_tree);
      
      if (APIExecute_C(tree, ch, &ret_value, CTX.GLOBAL_EC, 1)) EXIT(1);
      
      if (ret_value==NULL)
        *ret=newtarg("<no return>", 1, INTERM);
      else
        *ret=ret_value;

      return 1;
    }
  return 0;
}

void *GetIndexedZone(t_arg *elem, tree_list *tree, chain_list *env)
{
  int index;
  char *elm;
  t_arg *tmp;

  if (TOKEN(tree->NEXT->DATA)!=GEN_TOKEN_ARRAY) return NULL;
  if (elem->POINTER==0)
    {
      avt_errmsg(API_ERRMSG, "024", AVT_FATAL,FILE_NAME(tree),LINE(tree),elem->NAME);
/*      fprintf(stderr,"%s:%d: variable '%s' can not be indexed\n",
              FILE_NAME(tree),LINE(tree),
              elem->NAME
              );*/
      EXIT(1);
    }

  tmp=Eval_Exp_C(tree->NEXT->DATA->NEXT->NEXT,env);
  asserttargistype(tmp, T_INT, 0);
  index=*(int *)tmp->VALUE;
  freetarg(tmp,0);
  elm=*(char **)elem->VALUE;
  return &elm[sizeoftarg(elem->TYPE,elem->POINTER-1)*index];
}

void ZoneAdd(void *zone, char *type, int pointer, int inc)
{
  if (pointer>0)
    *((char **)zone)+=inc;
  else
    switch(STR2TYPE(type))
      {
      case T_INT:
        *((int *)zone)+=inc; break;
      case T_LONG:
        *((long *)zone)+=inc; break;
      case T_CHAR:
        *((char *)zone)+=inc; break;
      case T_DOUBLE:
        *((double *)zone)+=(double)inc; break;
      default:
        avt_errmsg(API_ERRMSG, "025", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
        //fprintf(stderr,"%s:%d: Update_Var: forbiden operation on this variable type\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
        EXIT(1);
      }
}

void AffectZone(t_arg *tmp, void *zone)
{
  memcpy(tmp->VALUE,zone,sizeoftarg(tmp->TYPE,tmp->POINTER));
}

void findandresettype(t_arg *ret)
{
  if (strcmp(ret->TYPE,"int")==0) { free(ret->TYPE); ret->TYPE="int"; }
  else if (strcmp(ret->TYPE,"void")==0) { free(ret->TYPE); ret->TYPE="void"; }
  else if (strcmp(ret->TYPE,"double")==0) { free(ret->TYPE); ret->TYPE="double"; }
  else if (strcmp(ret->TYPE,"char")==0) { free(ret->TYPE); ret->TYPE="char"; }
  else if (strcmp(ret->TYPE,"FILE")==0) { free(ret->TYPE); ret->TYPE="FILE"; }
  else { ret->TYPE=GetUnknownType(ret->TYPE); }
}

static t_arg *Eval_Exp_C(tree_list *tree, chain_list *env)
{ 
  t_arg *elem, *tmp, *left, *right;
  int error, size;
  char *ident;
  void *indexed;
  
  CTX.current_tree=tree; // to specify file:line

  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 87);
            //fprintf(stderr,"Eval_Exp_C: NULL pointer\n");
    EXIT(1);
  }
  
  if (CTX.RETURN_FLAG) return NULL;   /* boolean value :false */
  if (CTX.BREAK_FLAG) return NULL;   /* boolean value :false */
  
  switch (TOKEN(tree)) {
    /*nodes..*/
  case GEN_TOKEN_NODE:
    return Eval_Exp_C(tree->DATA,env);
    break;
  case GEN_TOKEN_INC_BEFORE:
    ident=getname(tree->NEXT);
    elem=gettarg(ident, env);
    if (!elem) {
      avt_errmsg(API_ERRMSG, "038", AVT_FATAL, FILE_NAME(tree),LINE(tree),ident);
      //fprintf(stderr,"%s:%d: Eval_Exp_C: unknown variable '%s'\n",FILE_NAME(tree),LINE(tree),ident);
      EXIT(2);
    }
    if ((indexed=GetIndexedZone(elem,tree,env))==NULL)
      {
        size=sizeofpointed(elem->TYPE, elem->POINTER);
        if (size!=0)
          addtarg(elem,size);
        else
          addtarg(elem,+1);
        return elem;
      }
    else
      {
        tmp=newtarg(elem->TYPE, elem->POINTER-1, INTERM);
        size=sizeofpointed(elem->TYPE, elem->POINTER-1);
        if (size!=0)
          {
            ZoneAdd(indexed,elem->TYPE, elem->POINTER-1, size);
            AffectZone(tmp, indexed);
          }
        else
          {
            ZoneAdd(indexed,elem->TYPE, elem->POINTER-1, +1);
            AffectZone(tmp, indexed);
          }
        return tmp;
      }
  case GEN_TOKEN_DEC_BEFORE:
    ident=getname(tree->NEXT);
    elem=gettarg(ident, env);
    if (!elem) {
      avt_errmsg(API_ERRMSG, "038", AVT_FATAL, FILE_NAME(tree),LINE(tree),ident);
      //fprintf(stderr,"%s:%d: Eval_Exp_C: unknown variable '%s'\n",FILE_NAME(tree),LINE(tree),ident);
      EXIT(2);
    }
    if ((indexed=GetIndexedZone(elem,tree,env))==NULL)
      {
        size=sizeofpointed(elem->TYPE, elem->POINTER);
        if (size!=0)
          addtarg(elem,-size);
        else
          addtarg(elem,-1);
        return elem;
      }
    else
      {
        tmp=newtarg(elem->TYPE, elem->POINTER-1, INTERM);
        size=sizeofpointed(elem->TYPE, elem->POINTER-1);
        if (size!=0)
          {
            ZoneAdd(indexed,elem->TYPE, elem->POINTER-1, -size);
            AffectZone(tmp, indexed);
          }
        else
          {
            ZoneAdd(indexed,elem->TYPE, elem->POINTER-1, -1);
            AffectZone(tmp, indexed);
          }
        return tmp;
      }
  case GEN_TOKEN_INC_AFTER:
    ident=getname(tree->NEXT);
    elem=gettarg(ident, env);
    if (!elem) {
      avt_errmsg(API_ERRMSG, "038", AVT_FATAL, FILE_NAME(tree),LINE(tree),ident);
      //fprintf(stderr,"%s:%d: Eval_Exp_C: unknown variable '%s'\n",FILE_NAME(tree),LINE(tree),ident);
      EXIT(2);
    }  
    if ((indexed=GetIndexedZone(elem,tree,env))==NULL)
      {
        size=sizeofpointed(elem->TYPE, elem->POINTER);
        if (size!=0)
          addupdate(&CTX.UPDATE,
                    elem->VALUE,
                    elem->POINTER>0?T_POINTER:STR2TYPE(elem->TYPE),
                    size);
        else
          addupdate(&CTX.UPDATE,
                    elem->VALUE,
                    elem->POINTER>0?T_POINTER:STR2TYPE(elem->TYPE),
                    +1);
        tmp=newtarg(elem->TYPE, elem->POINTER, INTERM);
        assigntarg(tmp,elem);
      }
    else
      {
        tmp=newtarg(elem->TYPE, elem->POINTER-1, INTERM);
        size=sizeofpointed(elem->TYPE, elem->POINTER-1);
        if (size!=0)
          addupdate(&CTX.UPDATE,
                    indexed,
                    elem->POINTER-1>0?T_POINTER:STR2TYPE(elem->TYPE),
                    size);
        else
          addupdate(&CTX.UPDATE,
                    indexed,
                    elem->POINTER-1>0?T_POINTER:STR2TYPE(elem->TYPE),
                    +1);
        AffectZone(tmp, indexed);
      }
    return tmp;
  case GEN_TOKEN_DEC_AFTER:
    ident=getname(tree->NEXT);
    elem=gettarg(ident, env);
    if (!elem) {
      avt_errmsg(API_ERRMSG, "038", AVT_FATAL, FILE_NAME(tree),LINE(tree),ident);
      //fprintf(stderr,"%s:%d: Eval_Exp_C: unknown variable '%s'\n",FILE_NAME(tree),LINE(tree),ident);
      EXIT(2);
    }  
      
    if ((indexed=GetIndexedZone(elem,tree,env))==NULL)
      {
        size=sizeofpointed(elem->TYPE, elem->POINTER);
        if (size!=0)
          addupdate(&CTX.UPDATE,
                    elem->VALUE,
                    elem->POINTER>0?T_POINTER:STR2TYPE(elem->TYPE),
                    -size);
        else
          addupdate(&CTX.UPDATE,
                    elem->VALUE,
                    elem->POINTER>0?T_POINTER:STR2TYPE(elem->TYPE),
                    -1);
        tmp=newtarg(elem->TYPE, elem->POINTER, INTERM);
        assigntarg(tmp,elem);
      }
    else
      {
        tmp=newtarg(elem->TYPE, elem->POINTER-1, INTERM);
        size=sizeofpointed(elem->TYPE, elem->POINTER-1);
        if (size!=0)
          addupdate(&CTX.UPDATE,
                    indexed,
                    elem->POINTER-1>0?T_POINTER:STR2TYPE(elem->TYPE),
                    -size);
        else
          addupdate(&CTX.UPDATE,
                    indexed,
                    elem->POINTER-1>0?T_POINTER:STR2TYPE(elem->TYPE),
                    -1);
        AffectZone(tmp, indexed);
      }
    return tmp;
  case '+':
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);
    
    if (left->POINTER==0 && right->POINTER>0)
      {
        // type* a + int
        asserttargistype(left, T_INT, 0);
        tmp=newtarg(right->TYPE, right->POINTER, INTERM);
        *((char **)tmp->VALUE) = *((char **)right->VALUE) + sizeofpointed(right->TYPE, right->POINTER)* *((int *)left->VALUE);
        freetarg(left,0); freetarg(right,0);
        return tmp;
      }
    else
      if (left->POINTER>0 && right->POINTER==0)
        {
          // type* a + int
          asserttargistype(right, T_INT, 0);
          tmp=newtarg(left->TYPE, left->POINTER, INTERM);
          *((char **)tmp->VALUE) = *((char **)left->VALUE) + sizeofpointed(left->TYPE, left->POINTER)* *((int *)right->VALUE);
          freetarg(left,0); freetarg(right,0);
          return tmp;
        }
      else
        if (left->POINTER>0 && right->POINTER>0)
          {
            avt_errmsg(API_ERRMSG, "039", AVT_FATAL, FILE_NAME(tree),LINE(tree));
            //fprintf(stderr,"%s:%d: Unautorized operation on pointers\n",FILE_NAME(tree),LINE(tree));
            EXIT(1);
          }
    setsametype(&left, &right);
    asserttargsametype(left, right,0);

    tmp=newtarg(left->TYPE, left->POINTER, INTERM);

    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) + *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) + *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) + *((char *)right->VALUE); break;
      case T_DOUBLE:
        *((double *)tmp->VALUE) = *((double *)left->VALUE) + *((double *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL, FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp; //Eval_Exp_C(tree->NEXT,env)+Eval_Exp_C(tree->NEXT->NEXT,env);
  case '-': 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);
  
    if (left->POINTER==0 && right->POINTER>0)
      {
        avt_errmsg(API_ERRMSG, "039", AVT_FATAL, FILE_NAME(tree),LINE(tree));
        //fprintf(stderr,"%s:%d: Unautorized operation on pointers\n",FILE_NAME(tree),LINE(tree));
        EXIT(1);
      }
    else
      if (left->POINTER>0 && right->POINTER==0)
        {
          // type* a + int
          asserttargisnottype(right, T_CHAR);
          asserttargisnottype(right, T_DOUBLE);
          tmp=newtarg(left->TYPE, left->POINTER, INTERM);
          if (STR2TYPE(tmp->TYPE)==T_INT)
            {
              *((char **)tmp->VALUE) = *((char **)left->VALUE) - sizeofpointed(left->TYPE, left->POINTER)* *((int *)right->VALUE);
            }
          else
            {
              // LONG
              *((char **)tmp->VALUE) = *((char **)left->VALUE) - sizeofpointed(left->TYPE, left->POINTER)* *((long *)right->VALUE);
            }
          freetarg(left,0); freetarg(right,0);
          return tmp;
        }
      else
        if (left->POINTER>0 && right->POINTER>0)
          {
            asserttargsametype(left, right,1);
            tmp=newtarg("int", 0, INTERM);
            *((int *)tmp->VALUE) = (*((char **)left->VALUE) - *((char **)right->VALUE))/sizeofpointed(left->TYPE, left->POINTER);
            freetarg(left,0); freetarg(right,0);
            return tmp;
          }

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    
    tmp=newtarg(left->TYPE, left->POINTER, INTERM);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) - *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) - *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) - *((char *)right->VALUE); break;
      case T_DOUBLE:
        *((double *)tmp->VALUE) = *((double *)left->VALUE) - *((double *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case '*': 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    
    tmp=newtarg(left->TYPE, left->POINTER, INTERM);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) * *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) * *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) * *((char *)right->VALUE); break;
      case T_DOUBLE:
        *((double *)tmp->VALUE) = *((double *)left->VALUE) * *((double *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        // fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case '/':
    right=Eval_Exp_C(tree->NEXT->NEXT,env);
    
    error=0;
    switch(STR2TYPE(right->TYPE))
      {
      case T_INT:
        if (*((int *)right->VALUE) == 0) error=1;
        break;
      case T_LONG:
        if (*((long *)right->VALUE) == 0) error=1;
        break;
      case T_CHAR:
        if (*((char *)right->VALUE) == 0) error=1;
        break;
      case T_DOUBLE:
        if (*((double *)right->VALUE) == 0.0) error=1;
        break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),right->TYPE,deref(right->POINTER));
        // fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }

    if (error)
      {
        avt_errmsg(API_ERRMSG, "041", AVT_FATAL,FILE_NAME(tree),LINE(tree));
//        fprintf(stderr,"%s:%d: division by zero\n",FILE_NAME(tree),LINE(tree));
        EXIT(1);
      }

    left=Eval_Exp_C(tree->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    
    tmp=newtarg(left->TYPE, left->POINTER, INTERM);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) / *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) / *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) / *((char *)right->VALUE); break;
      case T_DOUBLE:
        *((double *)tmp->VALUE) = *((double *)left->VALUE) / *((double *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "042", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),left->TYPE);
        //fprintf(stderr,"%s:%d: Unautorized operation on type '%s'\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),left->TYPE);
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case '%':
    right=Eval_Exp_C(tree->NEXT->NEXT,env);
    
    error=0;
    switch(STR2TYPE(right->TYPE))
      {
      case T_INT:
        if (*((int *)right->VALUE) == 0) error=1;
        break;
      case T_LONG:
        if (*((long *)right->VALUE) == 0) error=1;
        break;
      case T_CHAR:
        if (*((char *)right->VALUE) == 0) error=1;
        break;
      case T_DOUBLE:
        if (*((double *)right->VALUE) == 0.0) error=1;
        break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),right->TYPE,deref(right->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }

    if (error)
      {
        avt_errmsg(API_ERRMSG, "041", AVT_FATAL,FILE_NAME(tree),LINE(tree));
        //fprintf(stderr,"%s:%d: division by zero\n",FILE_NAME(tree),LINE(tree));
        EXIT(1);
      }

    left=Eval_Exp_C(tree->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    asserttargisnottype(left, T_DOUBLE);
    
    tmp=newtarg(left->TYPE, left->POINTER, INTERM);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) % *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) % *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) % *((char *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case '<': 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    
    tmp=newtarg("int", 0, CONDITION);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) < *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) < *((long *)right->VALUE); break;
      case T_CHAR:
        *((int *)tmp->VALUE) = *((char *)left->VALUE) < *((char *)right->VALUE); break;
      case T_DOUBLE:
        *((int *)tmp->VALUE) = *((double *)left->VALUE) < *((double *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
//        fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case '>': 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    
    tmp=newtarg("int", 0, CONDITION);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) > *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) > *((long *)right->VALUE); break;
      case T_CHAR:
        *((int *)tmp->VALUE) = *((char *)left->VALUE) > *((char *)right->VALUE); break;
      case T_DOUBLE:
        *((int *)tmp->VALUE) = *((double *)left->VALUE) > *((double *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case '&': 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    asserttargisnottype(left, T_DOUBLE);

    tmp=newtarg(left->TYPE, left->POINTER, INTERM);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) & *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) & *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) & *((char *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case '|': 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    asserttargisnottype(left, T_DOUBLE);

    tmp=newtarg(left->TYPE, left->POINTER, INTERM);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) | *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) | *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) | *((char *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case '^': 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    asserttargisnottype(left, T_DOUBLE);

    tmp=newtarg(left->TYPE, left->POINTER, INTERM);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) ^ *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) ^ *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) ^ *((char *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case GNS_TOKEN_AND: 

    tmp=newtarg("int", 0, CONDITION);

    left=Eval_Exp_C(tree->NEXT,env);
    if (left->POINTER==0) asserttargisnottype(left, T_DOUBLE);

    if (!testtarg(left))
      *((int *)tmp->VALUE) = 0;
    else
      {
        right=Eval_Exp_C(tree->NEXT->NEXT,env);
        if (right->POINTER==0) asserttargisnottype(right, T_DOUBLE);
            
        *((int *)tmp->VALUE) = testtarg(right);
        freetarg(right,0);
      }

    freetarg(left,0);
    return tmp;
  case GEN_TOKEN_OR: 
    tmp=newtarg("int", 0, CONDITION);

    left=Eval_Exp_C(tree->NEXT,env);
    if (left->POINTER==0) asserttargisnottype(left, T_DOUBLE);

    if (testtarg(left))
      *((int *)tmp->VALUE) = 1;
    else
      {
        right=Eval_Exp_C(tree->NEXT->NEXT,env);
        if (right->POINTER==0) asserttargisnottype(right, T_DOUBLE);
        
        *((int *)tmp->VALUE) = testtarg(right);
        freetarg(right,0);
      }

    freetarg(left,0);
    return tmp;
  case GEN_TOKEN_EG: 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,1);
    
    tmp=newtarg("int", 0, CONDITION);
    
    if (left->POINTER>0) 
      *((int *)tmp->VALUE) = *((void **)left->VALUE) == *((void **)right->VALUE);
    else
      switch(STR2TYPE(left->TYPE))
        {
        case T_INT:
          *((int *)tmp->VALUE) = *((int *)left->VALUE) == *((int *)right->VALUE); break;
        case T_LONG:
          *((long *)tmp->VALUE) = *((long *)left->VALUE) == *((long *)right->VALUE); break;
        case T_CHAR:
          *((int *)tmp->VALUE) = *((char *)left->VALUE) == *((char *)right->VALUE); break;
        case T_DOUBLE:
          *((int *)tmp->VALUE) = *((double *)left->VALUE) == *((double *)right->VALUE); break;
        default:
          avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //  fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          EXIT(1);
        }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case GEN_TOKEN_SUPEG: 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,1);
    
    tmp=newtarg("int", 0, CONDITION);
    
    if (left->POINTER>0) 
      *((int *)tmp->VALUE) = *((void **)left->VALUE) >= *((void **)right->VALUE);
    else
      switch(STR2TYPE(left->TYPE))
        {
        case T_INT:
          *((int *)tmp->VALUE) = *((int *)left->VALUE) >= *((int *)right->VALUE); break;
        case T_LONG:
          *((long *)tmp->VALUE) = *((long *)left->VALUE) >= *((long *)right->VALUE); break;
        case T_CHAR:
          *((int *)tmp->VALUE) = *((char *)left->VALUE) >= *((char *)right->VALUE); break;
        case T_DOUBLE:
          *((int *)tmp->VALUE) = *((double *)left->VALUE) >= *((double *)right->VALUE); break;
        default:
          avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          EXIT(1);
        }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case GEN_TOKEN_INFEG: 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,1);
    
    tmp=newtarg("int", 0, CONDITION);
    
    if (left->POINTER>0) 
      *((int *)tmp->VALUE) = *((void **)left->VALUE) <= *((void **)right->VALUE);
    else
      switch(STR2TYPE(left->TYPE))
        {
        case T_INT:
          *((int *)tmp->VALUE) = *((int *)left->VALUE) <= *((int *)right->VALUE); break;
        case T_LONG:
          *((long *)tmp->VALUE) = *((long *)left->VALUE) <= *((long *)right->VALUE); break;
        case T_CHAR:
          *((int *)tmp->VALUE) = *((char *)left->VALUE) <= *((char *)right->VALUE); break;
        case T_DOUBLE:
          *((int *)tmp->VALUE) = *((double *)left->VALUE) <= *((double *)right->VALUE); break;
        default:
          avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          // fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          EXIT(1);
        }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case GEN_TOKEN_NOTEG: 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,1);
    
    tmp=newtarg("int", 0, CONDITION);
    
    if (left->POINTER>0) 
      *((int *)tmp->VALUE) = *((void **)left->VALUE) != *((void **)right->VALUE);
    else
      switch(STR2TYPE(left->TYPE))
        {
        case T_INT:
          *((int *)tmp->VALUE) = *((int *)left->VALUE) != *((int *)right->VALUE); break;
        case T_LONG:
          *((long *)tmp->VALUE) = *((long *)left->VALUE) != *((long *)right->VALUE); break;
        case T_CHAR:
          *((int *)tmp->VALUE) = *((char *)left->VALUE) != *((char *)right->VALUE); break;
        case T_DOUBLE:
          *((int *)tmp->VALUE) = *((double *)left->VALUE) != *((double *)right->VALUE); break;
        default:
          avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          // fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          EXIT(1);
        }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case GEN_TOKEN_SHR: 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    asserttargisnottype(left, T_DOUBLE);

    tmp=newtarg(left->TYPE, left->POINTER, INTERM);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) >> *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) >> *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) >> *((char *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case GEN_TOKEN_SHL: 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);
    asserttargsametype(left, right,0);
    asserttargisnottype(left, T_DOUBLE);

    tmp=newtarg(left->TYPE, left->POINTER, INTERM);
    
    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = *((int *)left->VALUE) << *((int *)right->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = *((long *)left->VALUE) << *((long *)right->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = *((char *)left->VALUE) << *((char *)right->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type '%s'\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),left->TYPE);
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp;
  case GEN_TOKEN_OPPOSITE: 
    left=Eval_Exp_C(tree->NEXT,env);

    tmp=newtarg(left->TYPE, left->POINTER, INTERM);

    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = - *((int *)left->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = - *((long *)left->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = - *((char *)left->VALUE); break;
      case T_DOUBLE:
        *((double *)tmp->VALUE) = - *((double *)left->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE, deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0);
    return tmp; 
  case '~': 
    left=Eval_Exp_C(tree->NEXT,env);
    right=Eval_Exp_C(tree->NEXT->NEXT,env);

    setsametype(&left, &right);    
    asserttargsametype(left, right,0);
    asserttargisnottype(left, T_DOUBLE);

    tmp=newtarg(left->TYPE, left->POINTER, INTERM);

    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = ~ *((int *)left->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = ~ *((long *)left->VALUE); break;
      case T_CHAR:
        *((char *)tmp->VALUE) = ~ *((char *)left->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0); freetarg(right,0);
    return tmp; 
  case GEN_TOKEN_NOT: 
    left=Eval_Exp_C(tree->NEXT,env);

    tmp=newtarg("int", 0, CONDITION); //tmp=newtarg(left->TYPE, left->POINTER, INTERM);

    switch(STR2TYPE(left->TYPE))
      {
      case T_INT:
        *((int *)tmp->VALUE) = ! *((int *)left->VALUE); break;
      case T_LONG:
        *((long *)tmp->VALUE) = ! *((long *)left->VALUE); break;
      case T_CHAR:
        *((int *)tmp->VALUE) = ! *((char *)left->VALUE); break;
      case T_POINTER:
        *((int *)tmp->VALUE) = ! *((void **)left->VALUE); break;
      default:
        avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
        EXIT(1);
      }
    freetarg(left,0);
    return tmp; 
  case GEN_TOKEN_DIGIT: 
    tmp=newtarg("int", 0, CONSTANTE);
    *((int *)tmp->VALUE) = (int)(long)tree->DATA;
    return tmp;
  case GEN_TOKEN_DIGIT_CHAR: 
    tmp=newtarg("char", 0, CONSTANTE);
    *((char *)tmp->VALUE) = (char)(long)tree->DATA;
    return tmp;
  case GEN_TOKEN_DIGIT_DOUBLE: 
    tmp=newtarg("double", 0, CONSTANTE);
    *((double *)tmp->VALUE) = *(double *)tree->DATA;
    return tmp;
  case GEN_TOKEN_STRING:
    tmp=newtarg("char", 1, CONSTANTE);
    *((char **)tmp->VALUE) = sensitive_namealloc((char *)tree->DATA);
    return tmp;
  case GEN_TOKEN_REF:
    {
      void **ref;
      int i;
      left=Eval_Exp_C(tree->NEXT,env);
      i=STR2TYPE(left->TYPE);
      if (left->POINTER==0 || left->POINTER>(int)(long)tree->NEXT->NEXT->DATA || (left->POINTER==(int)(long)tree->NEXT->NEXT->DATA && i!=T_LONG && i!=T_INT && i!=T_DOUBLE && i!=T_CHAR))
        {
          avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
         // fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          EXIT(1);
        }
      
      ref=(void **)left->VALUE;
      for (i=1;i<=(int)(long)tree->NEXT->NEXT->DATA;i++)
        ref=(void **)*ref;
    
      
      tmp=newtarg(left->TYPE, left->POINTER-(int)(long)tree->NEXT->NEXT->DATA, INTERM);
      
      switch(STR2TYPE(left->TYPE))
        {
        case T_INT:
          if (tmp->POINTER==0)
            *((int *)tmp->VALUE) = *((int *)ref);
          else
            *((int **)tmp->VALUE) = (int *)ref;
          break;
        case T_LONG:
          if (tmp->POINTER==0)
            *((long *)tmp->VALUE) = *((long *)ref);
          else
            *((long **)tmp->VALUE) = (long *)ref;
          break;
        case T_CHAR:
          if (tmp->POINTER==0)
            *((char *)tmp->VALUE) = *((char *)ref);
          else
            *((char **)tmp->VALUE) = (char *)ref;
          break;
        case T_DOUBLE:
          if (tmp->POINTER==0)
            *((double *)tmp->VALUE) = *((double *)ref);
          else
            *((double **)tmp->VALUE) = (double *)ref;
          break;
        case T_POINTER:
          *((void **)tmp->VALUE) = (void *)ref;
          break;
        default:
          avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
          EXIT(1);
        }
      freetarg(left,0);
      return tmp; 
    }
  case GEN_TOKEN_ADR:
    ident=getname(tree->NEXT);
    elem=gettarg(ident, env);
    if (!elem) {
      avt_errmsg(API_ERRMSG, "038", AVT_FATAL,FILE_NAME(tree),LINE(tree),ident);
      //fprintf(stderr,"%s:%d: Eval_Exp_C: unknown variable '%s'\n",FILE_NAME(tree),LINE(tree),ident);
      EXIT(2);
    }
    if (TOKEN(tree->NEXT->DATA)!=GEN_TOKEN_ARRAY)
      {
        tmp=newtarg(elem->TYPE, elem->POINTER+1, INTERM);
        *(void **)tmp->VALUE=elem->VALUE;
        return tmp;
      }
    else
      {
        char *elm;
        if (elem->POINTER==0)
          {
            avt_errmsg(API_ERRMSG, "024", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), elem->NAME);
      /*      fprintf(stderr,"%s:%d: variable '%s' can not be indexed\n",
                    FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
                    elem->NAME
                    );*/
            EXIT(1);
          }
        tmp=Eval_Exp_C(tree->NEXT->DATA->NEXT->NEXT,env);
        asserttargisnottype(tmp, T_CHAR);
        asserttargisnottype(tmp, T_DOUBLE);
        if (tmp->POINTER==0)
          {
            if (STR2TYPE(tmp->TYPE)==T_INT)
              {
                int index;
                index=*(int *)tmp->VALUE;
                freetarg(tmp,0);
                tmp=newtarg(elem->TYPE, elem->POINTER, INTERM);
                elm=*(char **)elem->VALUE;
                *(void **)tmp->VALUE=&elm[sizeoftarg(elem->TYPE,elem->POINTER-1)*index];
              }
            else
              {
                // LONG
                long index;
                index=*(long *)tmp->VALUE;
                freetarg(tmp,0);
                tmp=newtarg(elem->TYPE, elem->POINTER, INTERM);
                elm=*(char **)elem->VALUE;
                *(void **)tmp->VALUE=&elm[sizeoftarg(elem->TYPE,elem->POINTER-1)*index];
              }
          }
        else
          {
            avt_errmsg(API_ERRMSG, "027", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),tmp->NAME);
//            fprintf(stderr,"%s:%d: '%s' should not be pointers\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),tmp->NAME);
            EXIT(1);
          }
        return tmp;
      }
  case GEN_TOKEN_NULL:
    tmp=newtarg("void", 1, "NULL");
    *(void **)tmp->VALUE=NULL;
    return tmp;
  case GEN_TOKEN_IDENT:
    ident=getname(tree);
    elem=gettarg(ident, env);
    if (!elem) {
      avt_errmsg(API_ERRMSG, "038", AVT_FATAL,FILE_NAME(tree),LINE(tree),ident);
      //fprintf(stderr,"%s:%d: Eval_Exp_C: unknown variable '%s'\n",FILE_NAME(tree),LINE(tree),ident);
      EXIT(2);
    }   
    return elem;
  case GEN_TOKEN_ARRAY:
    {
      char *elm;
      ident=getname(tree);
      elem=gettarg(ident, env);
      if (!elem) {
        avt_errmsg(API_ERRMSG, "038", AVT_FATAL,FILE_NAME(tree),LINE(tree),ident);
        //fprintf(stderr,"%s:%d: Eval_Exp_C: unknown variable '%s'\n",FILE_NAME(tree),LINE(tree),ident);
        EXIT(2);
      }   
      if (elem->POINTER==0)
        {
          avt_errmsg(API_ERRMSG, "024", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), elem->NAME);
/*          fprintf(stderr,"%s:%d: variable '%s' can not be indexed\n",
                  FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
                  elem->NAME
                  );*/
          EXIT(1);
        }
      tmp=Eval_Exp_C(tree->NEXT->NEXT,env);
      asserttargisnottype(tmp, T_CHAR);
      asserttargisnottype(tmp, T_DOUBLE);
      if (tmp->POINTER==0)
        {
          if (STR2TYPE(tmp->TYPE)==T_INT)
            { 
              int index;
              index=*(int *)tmp->VALUE;
              freetarg(tmp,0);
              tmp=newtarg(elem->TYPE, elem->POINTER-1, INTERM);
              elm=*(char **)elem->VALUE;
              elm=&elm[sizeoftarg(elem->TYPE,elem->POINTER-1)*index];
            }
          else
            {
              // LONG
              long index;
              index=*(long *)tmp->VALUE;
              freetarg(tmp,0);
              tmp=newtarg(elem->TYPE, elem->POINTER-1, INTERM);
              elm=*(char **)elem->VALUE;
              elm=&elm[sizeoftarg(elem->TYPE,elem->POINTER-1)*index];
            }
        }
      else
        {
          avt_errmsg(API_ERRMSG, "027", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),tmp->NAME);
          //fprintf(stderr,"%s:%d: '%s' should not be pointers\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),tmp->NAME);
          EXIT(1);
        }

      if (tmp->POINTER>0) 
        *((void **)tmp->VALUE) = *(void **)elm;
      else
        switch(STR2TYPE(tmp->TYPE))
          {
          case T_INT:
            *((int *)tmp->VALUE) = *(int *)elm; break;
          case T_LONG:
            *((long *)tmp->VALUE) = *(long *)elm; break;
          case T_CHAR:
            *((char *)tmp->VALUE) = *(char *)elm; break;
          case T_DOUBLE:
            *((double *)tmp->VALUE) = *(double *)elm; break;
          default:
            avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),elem->TYPE,deref(elem->POINTER));
            //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),elem->TYPE,deref(elem->POINTER));
            EXIT(1);
          }
      return tmp;
    }
  case GEN_TOKEN_STDOUT:
    tmp=newtarg("FILE", 1, INTERM);
    *((FILE **)tmp->VALUE) = stdout;
    return tmp;
    break;
  case GEN_TOKEN_STDERR:
    tmp=newtarg("FILE", 1, INTERM);
    *((FILE **)tmp->VALUE) = stderr;
    return tmp;
  case GEN_TOKEN_CAST:
    {
      return gen_docast((char *)tree->NEXT->DATA, (int)(long)tree->NEXT->NEXT->DATA, Eval_Exp_C(tree->NEXT->NEXT->NEXT,env));
    }
  case '=':
    if (TOKEN(tree->NEXT->DATA)!=GEN_TOKEN_REF)
      {
        ident=getname(tree->NEXT);
        elem=gettarg(ident, env);
        if (!elem) {
          avt_errmsg(API_ERRMSG, "038", AVT_FATAL,FILE_NAME(tree),LINE(tree),ident);
          //fprintf(stderr,"%s:%d: unknown variable '%s'\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ident);
          EXIT(2);
        }   
        right=Eval_Exp_C(tree->NEXT->NEXT,env);
    
        if (TOKEN(tree->NEXT->DATA)!=GEN_TOKEN_ARRAY)
          {
            if (elem->POINTER==0) right=setvalue(right, elem->TYPE);
            assigntarg(elem,right);
            freetarg(right,0);
            return elem;
          }
        else
          {
            tmp=Eval_Exp_C(tree->NEXT->DATA->NEXT->NEXT,env);
            asserttargisnottype(tmp, T_CHAR);
            asserttargisnottype(tmp, T_DOUBLE);
            if (tmp->POINTER==0)
              {
                if (STR2TYPE(tmp->TYPE)==T_INT)
                  { 
                    int index;
                    index=*(int *)tmp->VALUE;
                    freetarg(tmp,0);
                    if (elem->POINTER==1) right=setvalue(right, elem->TYPE);
                    assigntargtab(elem, right,index);
                    tmp=newtarg(elem->TYPE, elem->POINTER-1, INTERM);
                    assigntarg(tmp,right);
                    freetarg(right,0);
                  }
                else
                  {
                    // LONG
                    long index;
                    index=*(long *)tmp->VALUE;
                    freetarg(tmp,0);
                    if (elem->POINTER==1) right=setvalue(right, elem->TYPE);
                    assigntargtab(elem, right,index);
                    tmp=newtarg(elem->TYPE, elem->POINTER-1, INTERM);
                    assigntarg(tmp,right);
                    freetarg(right,0);
                  }
              }
            else
              {
                avt_errmsg(API_ERRMSG, "027", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),tmp->NAME);
                //fprintf(stderr,"%s:%d: '%s' should not be pointers\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),tmp->NAME);
                EXIT(1);
              }
            return tmp;
          }
      }
    else
      {
        int nbref;
        void **ref;
        int i,red;
        ident=getname(tree->NEXT);
        elem=gettarg(ident, env);
        if (!elem) {
          avt_errmsg(API_ERRMSG, "038", AVT_FATAL,FILE_NAME(tree),LINE(tree),ident);
          //fprintf(stderr,"%s:%d: unknown variable '%s'\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),ident);
        }   
        left=Eval_Exp_C(tree->NEXT->NEXT,env);
        if (TOKEN(tree->NEXT->DATA->NEXT->DATA)==GEN_TOKEN_ARRAY)
          {
            if (elem->POINTER==0)
              {
                avt_errmsg(API_ERRMSG, "024", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree), elem->NAME);
/*                fprintf(stderr,"%s:%d: variable '%s' can not be indexed\n",
                        FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),
                        elem->NAME
                        );*/
                EXIT(1);
              }
            red=1;
          }
        else 
          {
            red=0;
          }

        nbref=(int)(long)tree->NEXT->DATA->NEXT->NEXT->DATA;
    
        if (elem->POINTER-red<nbref)
          {
            avt_errmsg(API_ERRMSG, "043", AVT_FATAL,FILE_NAME(tree),LINE(tree),elem->TYPE,deref(elem->POINTER));
//            fprintf(stderr,"%s:%d: can not make the requested dereference of '%s%s'\n",FILE_NAME(tree),LINE(tree),elem->TYPE,deref(elem->POINTER));
            EXIT(1);
          }
        i=STR2TYPE(left->TYPE);
        if (left->POINTER>nbref || (left->POINTER==nbref && (i!=T_LONG || i!=T_INT || i!=T_DOUBLE || i!=T_CHAR)))
          {
            avt_errmsg(API_ERRMSG, "044", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE);
            // fprintf(stderr,"%s:%d: can not make the dereference of '%s'\n",FILE_NAME(tree),LINE(tree),left->TYPE);
            EXIT(1);
          }
        if (elem->POINTER-red-nbref!=left->POINTER)
          {
            avt_errmsg(API_ERRMSG, "037", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER),elem->TYPE,deref(elem->POINTER-red-nbref));
//            fprintf(stderr,"%s:%d: can not assign %s%s to %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER),elem->TYPE,deref(elem->POINTER-red-nbref));
            EXIT(1);
          }
    
        if (TOKEN(tree->NEXT->DATA->NEXT->DATA)!=GEN_TOKEN_ARRAY)
          {
            ref=(void **)elem->VALUE;
          }
        else
          {
            char *elm;
            tmp=Eval_Exp_C(tree->NEXT->DATA->NEXT->DATA->NEXT->NEXT,env);
            asserttargisnottype(tmp, T_CHAR);
            asserttargisnottype(tmp, T_DOUBLE);
            if (tmp->POINTER==0)
              {
                if (STR2TYPE(tmp->TYPE)==T_INT)
                  {
                    int index;
                    index=*(int *)tmp->VALUE;
                    freetarg(tmp,0);
                    elm=*(char **)elem->VALUE;
                    ref=(void **)&elm[sizeoftarg(elem->TYPE,elem->POINTER-1)*index];
                  }
                else
                  {
                    // LONG
                    long index;
                    index=*(long *)tmp->VALUE;
                    freetarg(tmp,0);
                    elm=*(char **)elem->VALUE;
                    ref=(void **)&elm[sizeoftarg(elem->TYPE,elem->POINTER-1)*index];
                  }
              }
            else
              {
                avt_errmsg(API_ERRMSG, "027", AVT_FATAL,FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),tmp->NAME);
                // fprintf(stderr,"%s:%d: '%s' should not be pointers\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree),tmp->NAME);
                EXIT(1);
              }
          }

        for (i=1;i<=nbref;i++)
          {
            ref=(void **)*ref;
          }

        i=left->POINTER;
        switch(STR2TYPE(elem->TYPE))
          {
          case T_INT:
            if (i==0)
              *((int *)ref) = *(int *)left->VALUE;
            else
              *((int **)ref) = (int *)left->VALUE;
            break;
          case T_LONG:
            if (i==0)
              *((long *)ref) = *(long *)left->VALUE;
            else
              *((long **)ref) = (long *)left->VALUE;
            break;
          case T_CHAR:
            if (i==0)
              *((char *)ref) = *(char *)left->VALUE;
            else
              *((char **)ref) = (char *)left->VALUE;
            break;
          case T_DOUBLE:
            if (i==0)
              *((double *)ref) = *(double *)left->VALUE;
            else
              *((double **)ref) = (double *)left->VALUE;
            break;
          case T_POINTER:
            *((void **)ref) = (void *)left->VALUE;
            break;
          default:
            avt_errmsg(API_ERRMSG, "040", AVT_FATAL,FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
            //fprintf(stderr,"%s:%d: Unautorized operation on type %s%s\n",FILE_NAME(tree),LINE(tree),left->TYPE,deref(left->POINTER));
            EXIT(1);
          }
        return left; 
      }
    /*exp list*/
  case ',':
    left=Eval_Exp_C(tree->NEXT,env);
    env=Update_Var(env);
    freetarg(left,0);
    return Eval_Exp_C(tree->NEXT->NEXT,env);
  case '?':
    left=Eval_Exp_C(tree->NEXT,env);
    if (left->POINTER==0) asserttargisnottype(left, T_DOUBLE);
   
    if (testtarg(left))
      {
        freetarg(left,0);
        return Eval_Exp_C(tree->NEXT->NEXT,env);
      }
    freetarg(left,0);
    return Eval_Exp_C(tree->NEXT->NEXT->NEXT,env);

  case GEN_TOKEN_FUNCTION:
    {
      int i,j;
      chain_list *args, *ch;
      char function_name[255];
      t_arg *ret;
      libfunc_type libfunc;
      t_arg *TAB_ARG[MAX_ARGS];

      if (tree->NEXT->NEXT->DATA!=NULL)
        args = args2chain(tree->NEXT->NEXT,NULL);
      else
        args = NULL;

      for (ch=args,i=0;ch!=NULL && i<MAX_ARGS;ch=ch->NEXT, i++)
        {
          TAB_ARG[i]=Eval_Exp_C((tree_list *)ch->DATA, env);
        }

      if (i>=MAX_ARGS)
        {
          avt_errmsg(API_ERRMSG, "045", AVT_FATAL,FILE_NAME(tree),LINE(tree),MAX_ARGS);
          //fprintf(stderr,"%s:%d: Can not call functions with more then %d arguments\n",FILE_NAME(tree),LINE(tree),MAX_ARGS);
          EXIT(1);
        }
    
      if (GenCatchFunction(&ret, TAB_ARG, i, getname(tree->NEXT->DATA))==0)
        {
          t_arg **functab;
          libgetargfunc_type libfuncarg;
          int funcnb;

          sprintf(function_name,"wrap_getargs_%s",getname(tree->NEXT->DATA));
      
          libfuncarg=(libgetargfunc_type)GetDynamicFunction(function_name);
      
          if (libfuncarg!=NULL)
            {
              libfuncarg(&functab, &funcnb);
              if (funcnb==i)
                {
                  for (j=0;j<i;j++)
                    {
                      if (functab[j]->POINTER==0) TAB_ARG[j]=setvalue(TAB_ARG[j], functab[j]->TYPE);
                    }
                }
              for (j=0;j<funcnb;j++) 
                {
                  findandresettype(functab[j]);
                  freetarg(functab[j],0);
                }
              mbkfree(functab);
            }

          sprintf(function_name,"wrap_%s",getname(tree->NEXT->DATA));
      
          libfunc=(libfunc_type)GetDynamicFunction(function_name);

          if (libfunc==NULL)
            {
              avt_errmsg(API_ERRMSG, "046", AVT_FATAL,FILE_NAME(tree),LINE(tree),function_name);
              //fprintf(stderr,"%s:%d: function '%s' can't be found in the dynamic libraries\n",FILE_NAME(tree),LINE(tree),function_name);
              EXIT(3);
            }
          
          j=libfunc(&ret, TAB_ARG, i, function_name);
          switch(j)
            {
            case 1:
              avt_errmsg(API_ERRMSG, "047", AVT_FATAL,FILE_NAME(tree),LINE(tree),function_name);
              //fprintf(stderr,"%s:%d: %s\n",FILE_NAME(tree),LINE(tree),function_name);
              EXIT(2);
            case 2:
            case 3:
              avt_errmsg(API_ERRMSG, "048", AVT_FATAL,FILE_NAME(tree),LINE(tree));
              //fprintf(stderr," error happens at %s:%d\n",FILE_NAME(tree),LINE(tree));
              EXIT(2);
            }

          ret->NAME=INTERM;
          // free the known types
          findandresettype(ret);
        }
      for (j=0;j<i;j++)
        freetarg(TAB_ARG[j],0);

      freechain(args);
      return ret;
    }
  default:
    Error_Tree("Eval_Exp_C",tree);
    EXIT(1); return NULL;
  }
}


/****************************************************************************/
/* Build a list of declared variables listed in tree                        */
/* put on the top of env                                                    */
/****************************************************************************/


static inline chain_list* Decl_Var(tree_list *tree, chain_list *env, char *type, int pointer)
{
  char *ident;
  t_arg *tmp, *right;
  static long index=-1;

  if (APIInParser()==0) CTX.current_tree=tree;

  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 88);
//    fprintf(stderr,"Decl_Var: NULL pointer\n");
    EXIT(1);
  }
  
  if (APIInParser()==0 && CTX.RETURN_FLAG) return NULL;   /* boolean value :false */
  if (APIInParser()==0 && CTX.BREAK_FLAG) return NULL;
  
  switch (TOKEN(tree)) {
    /*nodes..*/
  case GEN_TOKEN_NODE:
    return Decl_Var(tree->DATA,env,type,pointer);
    break;
  case ',': 
    env=Decl_Var(tree->NEXT,env,type,pointer);
    return Decl_Var(tree->NEXT->NEXT,env,type,pointer);
    break;
    /*affect*/  /*list of affect ?*/
  case '=':
    {
      int osf=CTX.STATIC_FLAG;
      CTX.STATIC_FLAG=0;
      env=Decl_Var(tree->NEXT,env,type,pointer);
      CTX.STATIC_FLAG=osf;

      ident=getname(tree->NEXT);
      tmp=blocklimited_gettarg(ident, env, CTX.BLOCK_ENV_LIMIT);
      
      right=Eval_Exp_C(tree->NEXT->NEXT,env);
      
      assigntarg(tmp,right);
      
      freetarg(right,0);
      
      if (APIInParser()==0 && CTX.STATIC_FLAG)
        {
          addstatic(tree->NEXT->DATA,tmp);
        }   
      
      return env;
    }
    /* declaration witout initialization */
  case GEN_TOKEN_ARRAY:
    tmp=Eval_Exp_C(tree->NEXT->NEXT,env);
    asserttargisnottype(tmp, T_CHAR);
    asserttargisnottype(tmp, T_DOUBLE);
    if (tmp->POINTER==0)
      {
        if (STR2TYPE(tmp->TYPE)==T_INT)
          {
            index=(long)*(int *)tmp->VALUE;
            freetarg(tmp,0);
          }
        else
          {
            index=*(long *)tmp->VALUE;
            freetarg(tmp,0);
          }
      }
    tree=tree->NEXT;
  case GEN_TOKEN_IDENT:
    ident=getname(tree);
    tmp=blocklimited_gettarg(ident, env, APIInParser()!=0?NULL:CTX.BLOCK_ENV_LIMIT);
    if (tmp!=NULL) 
      {
        avt_errmsg(API_ERRMSG, "049", AVT_FATAL,FILE_NAME(tree),LINE(tree),ident);
        //fprintf(stderr,"%s:%d: variable '%s' already declared in this scope\n",FILE_NAME(tree),LINE(tree),ident);
        EXIT(2);
      }   
    if (STR2TYPE(type)==T_POINTER && pointer==0)
      {
        avt_errmsg(API_ERRMSG, "050", AVT_FATAL,FILE_NAME(tree),LINE(tree),type);
        //fprintf(stderr,"%s:%d: type '%s' must be used as pointer\n",FILE_NAME(tree),LINE(tree),type);
        EXIT(2);
      }
 
    if (index!=-1) { pointer++; allocfortarg_multiplier=index; /*printf("t=%s p=%d n=%d\n",type,pointer,index);*/}
    tmp=newtarg(type, pointer, ident);
    if (index!=-1) { allocfortarg_multiplier=0; index=-1; }
    
    env=addchain(env, tmp);
    if (APIInParser()==0 && CTX.STATIC_FLAG) 
      {
        addstatic(tree, tmp);
      }   
    return env;
    break;
  case GEN_TOKEN_REF:
    return Decl_Var(tree->NEXT,env,type,(int)(long)tree->NEXT->NEXT->DATA);
  default:
    Error_Tree("Decl_Var",tree);
    EXIT(1); return NULL;
  }
}



/****************************************************************************/
/*                    execute instructions in tree                          */
/* return the new environment                                               */
/****************************************************************************/
static char *lastflow=NULL; // about sprintf, to know the first cut_tree

extern chain_list *Eval_Inst_C(tree_list *tree,chain_list *env)
{
  chain_list *oldlimit, *env2, *p;
  t_arg *tmp, *tmp1;
  int i;

  CTX.current_tree=tree;

  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 89);
//    fprintf(stderr,"Eval_Inst_C: NULL pointer\n");
    EXIT(1);
  }
  
  if (CTX.RETURN_FLAG) return env;
  if (CTX.BREAK_FLAG) return env;
  
  switch (TOKEN(tree)) {
    /*nodes..*/
  case GEN_TOKEN_NODE:
    env=Eval_Inst_C(tree->DATA,env);
    break;
  case GEN_TOKEN_NOP:
    lastflow=NULL;
    break;
  case ';': 
    env=Eval_Inst_C(tree->NEXT,env);
    if (CTX.RETURN_FLAG) return env;
    if (CTX.BREAK_FLAG) return env;
    env=Eval_Inst_C(tree->NEXT->NEXT,env);
    break;
  case GEN_TOKEN_BLOCK:
    oldlimit=CTX.BLOCK_ENV_LIMIT;
    CTX.BLOCK_ENV_LIMIT=env;
    env2=Eval_Inst_C(tree->NEXT,env);
    if (env!=env2) {
      for (p=env2; p; p=p->NEXT) {
        refresh_static((t_arg *)p->DATA);
        freetarg((t_arg *)p->DATA,1);
        if (p->NEXT==env) break;
      }
      if (!p) {
        avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 90);
//        fprintf(stderr,"Eval_Inst_C: NULL pointer\n");
        EXIT(1);
      }
      p->NEXT=NULL;
      freechain(env2);
    }   
    CTX.BLOCK_ENV_LIMIT=oldlimit;
    break;
  case GEN_TOKEN_STATIC:
    CTX.STATIC_FLAG=1;
    env=Eval_Inst_C(tree->NEXT,env);
    CTX.STATIC_FLAG=0;
    break;
  case GEN_TOKEN_FILE:
    env=Decl_Var(tree->NEXT,env,"FILE",0);
    break;
  case GEN_TOKEN_INTEGER:
    env=Decl_Var(tree->NEXT,env,"int",0);
    break;
  case GEN_TOKEN_LONG:
    env=Decl_Var(tree->NEXT,env,"long",0);
    break;
  case GEN_TOKEN_DOUBLE:
    env=Decl_Var(tree->NEXT,env,"double",0);
    break;
  case GEN_TOKEN_CHAR:
    env=Decl_Var(tree->NEXT,env,"char",0);
    break;
  case GEN_TOKEN_VOIDTOKEN_D:
    env=Decl_Var(tree->NEXT,env,"void",0);
    break;
  case GEN_TOKEN_OTHER_TYPES:
    env=Decl_Var(tree->NEXT->NEXT,env,getname(tree->NEXT),0);
    break;
  case GEN_TOKEN_EXCLUDE:
    /*nothing to do --> all is in ABL*/
    break;
  case GEN_TOKEN_EXIT:
    {
      t_arg *tmp;
      tmp=Eval_Exp_C(tree->NEXT,env);
      EXIT(*(int *)tmp->VALUE);
    }
  case GEN_TOKEN_BREAK:
    if (CTX.LOOP_COUNT) CTX.BREAK_FLAG=1;
    break;
  case GEN_TOKEN_RETURN:
    {
      t_arg *tmp;
      if (tree->DATA!=NULL)
        {
          tmp=Eval_Exp_C(tree->DATA,env);
          CTX.RETURN_VALUE=newtarg(tmp->TYPE, tmp->POINTER, INTERM);
          assigntarg(CTX.RETURN_VALUE,tmp);
          freetarg(tmp,0);
          CTX.RETURN_TREE=tree;
        }
      CTX.RETURN_FLAG=1;
      break;
    }
  case GEN_TOKEN_FPRINTF:           
    {
      FILE* flow;
      char *format;
      tmp=Eval_Exp_C(tree->NEXT,env);
      asserttargistype(tmp, T_POINTER, 1);
      if (strcmp(tmp->TYPE,"FILE")!=0)
        {
          avt_errmsg(API_ERRMSG, "051", AVT_FATAL, FILE_NAME(tree),LINE(tree),tmp->NAME);
          //fprintf(stderr,"%s:%d: Eval_Inst_C: type of '%s' must be 'FILE *'\n",FILE_NAME(tree),LINE(tree),tmp->NAME);
          EXIT(2);
        }
      flow=*(FILE **)tmp->VALUE;
      freetarg(tmp,0);

      tree=tree->NEXT->NEXT->DATA;
    
      if (TOKEN(tree)==',') 
        {
          tmp=Eval_Exp_C(tree->NEXT->DATA,env);
          asserttargistype(tmp, T_CHAR, 1);
          format=*(char **)tmp->VALUE;

          tmp1=Eval_Exp_C(tree->NEXT->NEXT->DATA,env);
        
          if (tmp1->POINTER>0)
            fprintf(flow,format,*(void **)tmp1->VALUE);
          else
            switch(STR2TYPE(tmp1->TYPE))
              {
              case T_INT: fprintf(flow,format,*(int *)tmp1->VALUE); break;
              case T_LONG: fprintf(flow,format,*(long *)tmp1->VALUE); break;
              case T_CHAR: fprintf(flow,format,*(char *)tmp1->VALUE); break;
              case T_DOUBLE: fprintf(flow,format,*(double *)tmp1->VALUE); break;
              }
          freetarg(tmp,0);
          freetarg(tmp1,0);
        }
      else 
        {
          tmp=Eval_Exp_C(tree,env);
          asserttargistype(tmp, T_CHAR, 1);
          format=*(char **)tmp->VALUE;
          fprintf(flow, "%s", format);
          freetarg(tmp,0);
        }
      if (flow==stdout || flow==stderr) fflush(flow);
    }   
    break;
  case GEN_TOKEN_BSPRINTF:           
    lastflow=NULL;
  case GEN_TOKEN_SPRINTF:           
    {
      char* flow;
      char *format;
      tmp=Eval_Exp_C(tree->NEXT,env);

      asserttargistype(tmp, T_CHAR, 1);
      /*    if (strcmp(tmp->TYPE,"char")!=0)
            {
            fprintf(stderr,"Eval_Inst_C: type of '%s' must be 'char *'\n",tmp->NAME);
            EXIT(2);
            }*/
      flow=*(char **)tmp->VALUE;
      freetarg(tmp,0);

      tree=tree->NEXT->NEXT->DATA;
    
      if (TOKEN(tree)==',') 
        {
          char temp_c[2048];
          tmp=Eval_Exp_C(tree->NEXT->DATA,env);
          asserttargistype(tmp, T_CHAR, 1);
          format=*(char **)tmp->VALUE;

          tmp1=Eval_Exp_C(tree->NEXT->NEXT->DATA,env);
        
          if (tmp1->POINTER>0)
            sprintf(temp_c,format,*(void **)tmp1->VALUE);
          else
            switch(STR2TYPE(tmp1->TYPE))
              {
              case T_INT: sprintf(temp_c,format,*(int *)tmp1->VALUE); break;
              case T_LONG: sprintf(temp_c,format,*(long *)tmp1->VALUE); break;
              case T_CHAR: sprintf(temp_c,format,*(char *)tmp1->VALUE); break;
              case T_DOUBLE: sprintf(temp_c,format,*(double *)tmp1->VALUE); break;
              }
          if (lastflow==flow) strcat(flow,temp_c);
          else strcpy(flow,temp_c);
          freetarg(tmp,0);
          freetarg(tmp1,0);
        }
      else 
        {
          tmp=Eval_Exp_C(tree,env);
          asserttargistype(tmp, T_CHAR, 1);
          format=*(char **)tmp->VALUE;
          if (lastflow==flow) strcat(flow,format);
          else strcpy(flow,format);
          freetarg(tmp,0);
        }
      lastflow=flow;
    }   
    break;
  case GEN_TOKEN_IF:
    tmp=Eval_Exp_C(tree->NEXT,env);
    asserttargisnottype(tmp, T_DOUBLE);

    if (testtarg(tmp))
      {
        /* for effect of Eval_Exp_C  */ 
        env=Update_Var(env);
        env=Eval_Inst_C(tree->NEXT->NEXT,env);
      }
    freetarg(tmp,0);
    break;
  case GEN_TOKEN_ELSE:
    tmp=Eval_Exp_C(tree->NEXT,env);
    asserttargisnottype(tmp, T_DOUBLE);

    if (testtarg(tmp)) 
      {
        /* for effect of Eval_Exp_C  */ 
        env=Update_Var(env);
        env=Eval_Inst_C(tree->NEXT->NEXT,env);
      }   
    else
      {
        /* for effect of Eval_Exp_C  */ 
        env=Update_Var(env);
        env=Eval_Inst_C(tree->NEXT->NEXT->NEXT,env);
      }
    freetarg(tmp,0);
    break;
  case GEN_TOKEN_DO:
    CTX.LOOP_COUNT++;
    do {
      /* for effect of Eval_Exp_C  */ 
      env=Update_Var(env);
      if (CTX.RETURN_FLAG) return env;
      if (CTX.BREAK_FLAG) {
        CTX.BREAK_FLAG=0;
        CTX.LOOP_COUNT--;
        return env;
      }   
      env=Eval_Inst_C(tree->NEXT,env);
      if (CTX.RETURN_FLAG) return env;
      if (CTX.BREAK_FLAG) {
        CTX.BREAK_FLAG=0;
        CTX.LOOP_COUNT--;
        return env;
      } 
      tmp=Eval_Exp_C(tree->NEXT->NEXT,env);
      asserttargisnottype(tmp, T_DOUBLE);
      i=testtarg(tmp);
      freetarg(tmp,0);
    } while (i);
    CTX.LOOP_COUNT--;
    break;
  case GEN_TOKEN_WHILE:
    CTX.LOOP_COUNT++;

    tmp=Eval_Exp_C(tree->NEXT,env);
    asserttargisnottype(tmp, T_DOUBLE);
    i=testtarg(tmp);
    freetarg(tmp,0);

    while (i) {
      /* for effect of Eval_Exp_C  */ 
      env=Update_Var(env);
      if (CTX.RETURN_FLAG) return env;
      if (CTX.BREAK_FLAG) {
        CTX.BREAK_FLAG=0;
        CTX.LOOP_COUNT--;
        return env;
      }   
      env=Eval_Inst_C(tree->NEXT->NEXT,env);
      if (CTX.RETURN_FLAG) return env;
      if (CTX.BREAK_FLAG) {
        CTX.BREAK_FLAG=0;
        CTX.LOOP_COUNT--;
        return env;
      }
      tmp=Eval_Exp_C(tree->NEXT,env);
      asserttargisnottype(tmp, T_DOUBLE);
      i=testtarg(tmp);
      freetarg(tmp,0);
    }
    CTX.LOOP_COUNT--;
    break;
  case GEN_TOKEN_FOR:
    CTX.LOOP_COUNT++;
    for(env=Eval_Inst_C(tree->NEXT,env);
        testtarg(tmp=Eval_Exp_C(tree->NEXT->NEXT,env));
        env=Eval_Inst_C(tree->NEXT->NEXT->NEXT,env)) 
      {
        freetarg(tmp,0);
        /* for effect of Eval_Exp_C  */ 
        env=Update_Var(env);
        if (CTX.RETURN_FLAG) return env;
        if (CTX.BREAK_FLAG) {
          CTX.BREAK_FLAG=0;
          CTX.LOOP_COUNT--;
          return env;
        }   
        env=Eval_Inst_C(tree->NEXT->NEXT->NEXT->NEXT,env);
        if (CTX.RETURN_FLAG) return env;
        if (CTX.BREAK_FLAG) {
          CTX.BREAK_FLAG=0;
          CTX.LOOP_COUNT--;
          return env;
        }   
      }
    freetarg(tmp,0);
       
    CTX.LOOP_COUNT--;
    break;
    /*other cases are expressions*/
  default:
    freetarg(Eval_Exp_C(tree,env),0);
     
  }

  /* to get effect for i++ and i--   */ 
  env=Update_Var(env);
  return env;
}   

extern chain_list *Eval_Global_Decl(tree_list *tree,chain_list *env)
{
  CTX.current_tree=tree;

  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 91);
//    fprintf(stderr,"Eval_Global_Decl: NULL pointer\n");
    EXIT(1);
  }
    
  switch (TOKEN(tree)) {
    /*nodes..*/
  case GEN_TOKEN_NODE:
    env=Eval_Global_Decl(tree->DATA,env);
    break;
  case GEN_TOKEN_NOP:
    lastflow=NULL;
    break;
  case ';': 
    env=Eval_Global_Decl(tree->NEXT,env);
    env=Eval_Global_Decl(tree->NEXT->NEXT,env);
    break;
  case GEN_TOKEN_FILE:
    env=Decl_Var(tree->NEXT,env,"FILE",0);
    break;
  case GEN_TOKEN_INTEGER:
    env=Decl_Var(tree->NEXT,env,"int",0);
    break;
  case GEN_TOKEN_LONG:
    env=Decl_Var(tree->NEXT,env,"long",0);
    break;
  case GEN_TOKEN_DOUBLE:
    env=Decl_Var(tree->NEXT,env,"double",0);
    break;
  case GEN_TOKEN_CHAR:
    env=Decl_Var(tree->NEXT,env,"char",0);
    break;
  case GEN_TOKEN_VOIDTOKEN_D:
    env=Decl_Var(tree->NEXT,env,"void",0);
    break;
  case GEN_TOKEN_OTHER_TYPES:
    env=Decl_Var(tree->NEXT->NEXT,env,getname(tree->NEXT),0);
    break;
    /*other cases are expressions*/
  default:
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 92);
    //fprintf(stderr,"Eval_Global_Decl: unknown token %d\n",TOKEN(tree));
    EXIT(1);
     
  }
  env=Update_Var(env);
  return env;
}   

void Add_Global_Decl(tree_list *tree, ExecutionContext *ec)
{
  ec->globalvars=Eval_Global_Decl(tree, ec->globalvars);
}

/****************************************************************************/
/*           execute instructions in tree and set environment               */
/* env contains variables defined by netlist occurencies                    */
/* tree should be verified first...                                         */
/****************************************************************************/

static jmp_buf sj_env;

void genius_fatal(int code)
{
  longjmp(sj_env, code);
}

int APIExecute_C(tree_list *tree, chain_list *args, t_arg **ret_value, ExecutionContext *ec, int disablecore)
{
  int fatal_err;
  chain_list *basevar,*env2,*p;
  chain_list *tmp;
  t_arg *ta, *ta0;
#if 0
  chain_list *p0;
  libfunc_type func;
  t_arg *ret;
  char buf[128];
#endif // ..anto..
  struct EXE_CTX LASTCTX;
  ExecutionContext *prev=CUR_CTX;

  CUR_CTX=ec;

  memcpy(&LASTCTX, &CTX, sizeof(struct EXE_CTX));

  CTX.current_tree=tree;
  CTX.GLOBAL_EC=ec;
  CTX.TO_FREE=NULL;
  CTX.UPDATE=NULL;

  if (disablecore)
    {
      avt_TrapSegV();
      avt_PushSegVExit(genius_fatal,1);
      avt_PushSegVMessage("");
    }

  if (setjmp(sj_env))
    {
      avt_errmsg(API_ERRMSG, "052", AVT_ERROR, FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
      //fprintf(stderr,"%s:%d: Fatal error while executing program\n",FILE_NAME(CTX.current_tree),LINE(CTX.current_tree));
      memcpy(&CTX, &LASTCTX, sizeof(struct EXE_CTX));
      CUR_CTX=prev;
      return 1;
    }

  if (!tree)
    {
      //fprintf(stderr,"Exec_C: NULL pointer\n");
      memcpy(&CTX, &LASTCTX, sizeof(struct EXE_CTX));
      CUR_CTX=prev;
      return 1;
    }
  if ((fatal_err=Get_Error())!=0)
    {
      avt_errmsg(API_ERRMSG, "053", AVT_ERROR, fatal_err);
      //fprintf(stderr,"Exec_C: %d errors. Cannot execute\n",fatal_err);
      memcpy(&CTX, &LASTCTX, sizeof(struct EXE_CTX));
      CUR_CTX=prev;
      return 1;
    }

  CTX.RETURN_FLAG=0;    /* escape from all Exec_C() */
  CTX.BREAK_FLAG=0;     /* escape loop*/
  CTX.LOOP_COUNT=0;     /*counter for break*/
  CTX.UPDATE=NULL;
  CTX.TO_FREE=NULL;
  CTX.warnings=NULL;
  CTX.BLOCK_ENV_LIMIT=NULL;
  CTX.RETURN_VALUE=NULL;
   
  //  basevar=NULL;
  basevar=ec->globalvars;

  for (tmp=APIGetCFunctionARGS(tree);tmp!=NULL;tmp=tmp->NEXT)
    {
      ta0=(t_arg *)tmp->DATA;
      for (p=args;p!=NULL;p=p->NEXT) 
        {
          ta=(t_arg *)p->DATA;
          if (strcasecmp(ta->NAME, ta0->NAME)==0) break;
        }
      //      for (p0=env;p0!=NULL && p0->DATA!=tmp->DATA;p0=p0->NEXT) ;
      if (p==NULL)
        {
          avt_errmsg(API_ERRMSG, "054", AVT_WARNING, FILE_NAME(tree), LINE(tree), ta0->NAME);
          //fprintf(stderr,"%s:%d: parameter '%s' is uninitialised\n",FILE_NAME(tree), LINE(tree), ta0->NAME);
          ta=newtarg(ta0->TYPE, ta0->POINTER, sensitive_namealloc(ta0->NAME));
          basevar=addchain(basevar,ta);
        }
      else
        {
          // check types
          if (strcmp(ta0->TYPE, ta->TYPE)!=0 || ta0->POINTER!=ta->POINTER)
            {
              avt_errmsg(API_ERRMSG, "055", AVT_WARNING, FILE_NAME(tree), LINE(tree),ta0->NAME, ta->TYPE,deref(ta->POINTER), ta0->TYPE, deref(ta0->POINTER));
              //fprintf(stderr,"%s:%d: conflicting type for parameter '%s' : '%s%s'!='%s%s'\n",FILE_NAME(tree), LINE(tree),ta0->NAME, ta->TYPE,deref(ta->POINTER), ta0->TYPE, deref(ta0->POINTER));
            }
        }
    }

  basevar=append(args, basevar);
  
  env2=Eval_Inst_C(tree->NEXT->NEXT->DATA,basevar);


  tmp=APIGetCFunctionReturnARG(tree);
  ta=(t_arg *)tmp->DATA;
  if (strcmp(ta->TYPE, "void")==0 && ta->POINTER==0 && CTX.RETURN_VALUE!=NULL)
    avt_errmsg(API_ERRMSG, "056", AVT_WARNING, FILE_NAME(CTX.RETURN_TREE), LINE(CTX.RETURN_TREE));
    //fprintf(stderr,"%s:%d: return value for void function\n",FILE_NAME(CTX.RETURN_TREE), LINE(CTX.RETURN_TREE));
  else
    if (!(strcmp(ta->TYPE, "void")==0 && ta->POINTER==0) && CTX.RETURN_VALUE==NULL)
      avt_errmsg(API_ERRMSG, "057", AVT_WARNING, FILE_NAME(tree), LINE(tree));
      //fprintf(stderr,"%s:%d: no return value for function\n",FILE_NAME(tree), LINE(tree));
    else
      {
        if (CTX.RETURN_VALUE!=NULL)
          {
            if (!(ta->POINTER!=0 && CTX.RETURN_VALUE->POINTER!=0 && (strcmp(ta->TYPE,"void")==0 || strcmp(CTX.RETURN_VALUE->TYPE,"void")==0)) &&
                (strcmp(ta->TYPE, CTX.RETURN_VALUE->TYPE)!=0 || ta->POINTER!=CTX.RETURN_VALUE->POINTER))
              avt_errmsg(API_ERRMSG, "058", AVT_WARNING, FILE_NAME(CTX.RETURN_TREE), LINE(CTX.RETURN_TREE), ta->TYPE,deref(ta->POINTER), CTX.RETURN_VALUE->TYPE, deref(CTX.RETURN_VALUE->POINTER));
              //fprintf(stderr,"%s:%d: conflicting type for return value : '%s%s'!='%s%s'\n",FILE_NAME(CTX.RETURN_TREE), LINE(CTX.RETURN_TREE), ta->TYPE,deref(ta->POINTER), CTX.RETURN_VALUE->TYPE, deref(CTX.RETURN_VALUE->POINTER));
          }
      }

  *ret_value=CTX.RETURN_VALUE;
  CTX.RETURN_VALUE=NULL;

  if (basevar!=env2) {
    for (p=env2; p; p=p->NEXT) {
      refresh_static((t_arg *)p->DATA);
      freetarg((t_arg *)p->DATA,1);
      if (p->NEXT==basevar) break;
    }
    if (!p) {
//      fprintf(stderr,"Eval_Inst_C: NULL pointer\n");
      memcpy(&CTX, &LASTCTX, sizeof(struct EXE_CTX));
      return 1;
    }
    p->NEXT=NULL;
    freechain(env2);
  }   
  
  env2=NULL;
  for (p=basevar; p!=ec->globalvars; env2=p, p=p->NEXT) freetarg((t_arg *)p->DATA,1);
  if (env2!=NULL)
    {
      env2->NEXT=NULL;
      freechain(basevar);
    }

  /*free mem for strings */
  for (p=CTX.TO_FREE;p!=NULL;p=p->NEXT)
    mbkfree(p->DATA);

  freechain(CTX.TO_FREE);
  freechain(CTX.warnings);

  if (disablecore)
    {
      avt_PopSegVMessage();
      avt_PopSegVExit();
      avt_UnTrapSegV();
    }

  memcpy(&CTX, &LASTCTX, sizeof(struct EXE_CTX));
  CUR_CTX=prev;
  return 0;
}


/****************************************************************************/
/*           free in mem the effect of execution of program C               */
/****************************************************************************/
extern void Free_Exec_C(ExecutionContext *ec)
{
  static_list *st;
  chain_list *p;

  for (st=ec->STATIC_LIST;st!=NULL;st=st->NEXT) mbkfree(st->VALUE);

  freestatic(ec->STATIC_LIST);
  //  freechain(ModelTrace);
  //  ModelTrace=NULL;
  for (p=ec->globalvars; p!=NULL; p=p->NEXT) freetarg((t_arg *)p->DATA,1);
  freechain(ec->globalvars);
}

ExecutionContext *APINewExecutionContext()
{
  ExecutionContext *ec;
  t_arg *ta;

  ec=(ExecutionContext *)mbkalloc(sizeof(ExecutionContext));
  ec->BLOCKS=NULL;
  ec->globalvars=NULL;
  ec->STATIC_LIST=NULL;
  ec->ALL_USED_FUNCTIONS=NULL;

  ta=newtarg("FILE", 1, sensitive_namealloc("stdout"));
  *(FILE **)ta->VALUE=stdout;
  ec->globalvars=addchain(ec->globalvars,ta);

  ta=newtarg("FILE", 1, sensitive_namealloc("stderr"));
  *(FILE **)ta->VALUE=stderr;
  ec->globalvars=addchain(ec->globalvars,ta);

  return ec;
}

t_arg *APIGetGlobalVariable(ExecutionContext *ec, char *name)
{
  t_arg *ta;
  chain_list *cl;

  for (cl=ec->globalvars; cl!=NULL; cl=cl->NEXT)
    {
      ta=(t_arg *)cl->DATA;
      if (strcmp(ta->NAME,name)==0) return ta;
    }
  return ta;
}


void APIFreeExecutionContext(ExecutionContext *ec)
{
  chain_list *cl, *ch;
  used_func *uf;
  
  for (cl=ec->BLOCKS; cl!=NULL; cl=cl->NEXT)
    Free_Tree((tree_list *)cl->DATA);

  for (cl=ec->ALL_USED_FUNCTIONS; cl!=NULL; cl=cl->NEXT)
  {
    uf=(used_func *)cl->DATA;
    for (ch=uf->where; ch!=NULL; ch=ch->NEXT)
      Free_Tree((tree_list *)ch->DATA);
    freechain(uf->where);
    mbkfree(uf);
  }
  freechain(ec->ALL_USED_FUNCTIONS);
  Free_Exec_C(ec);
  mbkfree(ec);
}

int APICheckCFunctions(ExecutionContext *ec)
{
  if (ec->ALL_USED_FUNCTIONS!=NULL)
    {
      chain_list *cl, *ch;
      used_func *uf;
      char temp[1024];
      int notfound=0;
      
      for (cl=ec->ALL_USED_FUNCTIONS; cl!=NULL; cl=cl->NEXT)
        {
          uf=(used_func *)cl->DATA;
          if (strcmp(uf->name,"malloc")!=0 && 
              strcmp(uf->name,"callfunc")!=0 && 
              APIGetCFunction(ec, uf->name, 'y')==NULL)
            {
              sprintf(temp,"wrap_%s",uf->name);
              if (GetDynamicFunction(temp)==NULL)
                {
                  notfound++;
                  if (notfound==1) fprintf(stderr,"\n");
                  avt_errmsg(API_ERRMSG, "059", AVT_ERROR, uf->name);
//                  fprintf(stderr,"Function '%s' used in action was not found\nLocation(s):\n", uf->name);
                  for (ch=uf->where; ch!=NULL; ch=ch->NEXT)
                    {
                      fprintf(stderr,"\t%s:%d\n",FILE_NAME((tree_list *)ch->DATA), LINE((tree_list *)ch->DATA));
                    }
                }
            }
          for (ch=uf->where; ch!=NULL; ch=ch->NEXT)
             Free_Tree((tree_list *)ch->DATA);

          freechain(uf->where);
          mbkfree(uf);
        }
      if (notfound) return 1;
      freechain(ec->ALL_USED_FUNCTIONS);
      ec->ALL_USED_FUNCTIONS=NULL;
    }
  return 0;
}

void APIVerify_C_Functions(ExecutionContext *ec)
{
  chain_list *cl;
  tree_list *tl;
  for (cl=ec->BLOCKS; cl!=NULL; cl=cl->NEXT)
    {
      tl=(tree_list *)cl->DATA;
      if (APIBlockIsC(tl))
        {
          APIVerify_C(tl->NEXT, NULL);
        }
    }
}

chain_list *APIGetCFunctionARGS(tree_list *tree)
{
  return (chain_list *)tree->NEXT->DATA->NEXT->NEXT->NEXT->DATA->DATA;
}

chain_list *APIGetCFunctionReturnARG(tree_list *tree)
{
  return (chain_list *)tree->NEXT->DATA->NEXT->NEXT->NEXT->NEXT->DATA->DATA;
}
tree_list *APIGetCFunction(ExecutionContext *ec, char *name, char sensi)
{
  chain_list *cl;
  tree_list *tl;
  for (cl=ec->BLOCKS; cl!=NULL; cl=cl->NEXT)
    {
      tl=(tree_list *)cl->DATA;
      if (APIBlockIsC(tl) && 
          ((sensi=='y' && strcmp(APIGetCName(tl), name)==0) ||
           (sensi=='n' && strcasecmp(APIGetCName(tl), name)==0)))
        {
          return tl;
        }
    }
  return NULL;
}

chain_list *APIAddIntTARG(chain_list *head, char *name, int value)
{  
  t_arg *ta;
  ta=newtarg("int", 0, name);
  *(int *)ta->VALUE=value;
  return addchain(head, ta);
}

chain_list *APIAddLongTARG(chain_list *head, char *name, long value)
{  
  t_arg *ta;
  ta=newtarg("long", 0, name);
  *(long *)ta->VALUE=value;
  return addchain(head, ta);
}

chain_list *APIAddDoubleTARG(chain_list *head, char *name, double value)
{  
  t_arg *ta;
  ta=newtarg("double", 0, name);
  *(double *)ta->VALUE=value;
  return addchain(head, ta);
}

chain_list *APIAddCharTARG(chain_list *head, char *name, char value)
{  
  t_arg *ta;
  ta=newtarg("char", 0, name);
  *(char *)ta->VALUE=value;
  return addchain(head, ta);
}

chain_list *APIAddPointerTARG(chain_list *head, char *name, char *type, int level, void *value)
{  
  t_arg *ta;
  ta=newtarg(type, level, name);
  *(void **)ta->VALUE=value;
  return addchain(head, ta);
}


t_arg *APICreateIntTARG(char *name, int value)
{  
  t_arg *ta;
  ta=newtarg("int", 0, name);
  *(int *)ta->VALUE=value;
  return ta;
}

t_arg *APICreateLongTARG(char *name, long value)
{  
  t_arg *ta;
  ta=newtarg("long", 0, name);
  *(long *)ta->VALUE=value;
  return ta;
}

t_arg *APICreateDoubleTARG(char *name, double value)
{  
  t_arg *ta;
  ta=newtarg("double", 0, name);
  *(double *)ta->VALUE=value;
  return ta;
}

t_arg *APICreateCharTARG(char *name, char value)
{  
  t_arg *ta;
  ta=newtarg("long", 0, name);
  *(char *)ta->VALUE=value;
  return ta;
}

t_arg *APICreatePointerTARG(char *name, char *type, int level, void *value)
{  
  t_arg *ta;
  ta=newtarg(type, level, name);
  *(void **)ta->VALUE=value;
  return ta;
}


void APIFreeTARGS(chain_list *head)
{
  chain_list *cl;
  for (cl=head; cl!=NULL; cl=cl->NEXT)
    freetarg((t_arg *)cl->DATA,1);
  freechain(head);
}

void APIFreeTARG(t_arg *ta)
{
  freetarg(ta,1);
}

chain_list *APIAddTARG(chain_list *head, char *type, int pointer, char *name)
{
  t_arg *ta;
  ta=newtarg(type, pointer, name);
  return addchain(head, ta);
}

t_arg *APINewTARG(char *type, int pointer, char *name)
{
  return newtarg(type, pointer, name);
}

chain_list *APIPrepareFunctionARGS(tree_list *func, t_arg **tab, int nb, tree_list *where)
{
  chain_list *cl, *ch;
  int count, i;
  t_arg *ta;

  if (func!=NULL)
    {
      cl=APIGetCFunctionARGS(func);
      for (ch=cl, count=0; ch!=NULL; ch=ch->NEXT) count++;
      if (count<nb)
        {
          avt_errmsg(API_ERRMSG, "060", AVT_FATAL, FILE_NAME(where!=NULL?where:func),LINE(where!=NULL?where:func),APIGetCName(func));
//          fprintf(stderr,"%s:%d: too many arguments in function '%s'\n",FILE_NAME(where!=NULL?where:func),LINE(where!=NULL?where:func),APIGetCName(func));
          EXIT(1);
        }
      else
        if (count>nb)
          {
            avt_errmsg(API_ERRMSG, "061", AVT_FATAL, FILE_NAME(where!=NULL?where:func),LINE(where!=NULL?where:func),APIGetCName(func));
            //fprintf(stderr,"%s:%d: too few arguments for function '%s'\n",FILE_NAME(where!=NULL?where:func),LINE(where!=NULL?where:func),APIGetCName(func));
            EXIT(1);
          }
    }

  i=0; ch=NULL;
  while (i<nb)
    {
      if (func!=NULL)
        {
          ta=(t_arg *)cl->DATA;
          if (ta->POINTER==0) tab[i]=setvalue(tab[i], ta->TYPE);
        }
      ch=APIAddTARG(ch, tab[i]->TYPE, tab[i]->POINTER, func!=NULL?((t_arg *)cl->DATA)->NAME:tab[i]->NAME);
      ta=(t_arg *)ch->DATA;
      assigntarg((t_arg *)ch->DATA,tab[i]);
      if (func!=NULL) cl=cl->NEXT;
      i++;
    }

  return reverse(ch);
}

chain_list *APIDupARGS(chain_list *cl)
{
  chain_list *ch;
  t_arg *ta;

  ch=NULL;
  while (cl!=NULL)
    {
      ta=(t_arg *)cl->DATA;
      ch=APIAddTARG(ch, ta->TYPE, ta->POINTER, ta->NAME);
      assigntarg((t_arg *)ch->DATA,ta);
      cl=cl->NEXT;
    }
  return reverse(ch);
}

int APIExecAPICallFunc(ExecutionContext *ec, APICallFunc *cf, t_arg **ret, int noerr)
{
  tree_list *func;
  chain_list *ch;
  t_arg *tab[100], *ret_value;
  int nb, j;
  char function_name[200];
  libfunc_type libfunc;
  char buf1[1024];

  if (ec!=NULL && (func = APIGetCFunction (ec, cf->NAME, 'y'))!=NULL) 
    {
      //      printf("found tree for '%s'\n",cf->NAME);
      for (nb=0, ch=cf->ARGS; ch!=NULL; nb++, ch=ch->NEXT) tab[nb]=(t_arg *)ch->DATA;

      ch=APIPrepareFunctionARGS(func, tab, nb, NULL);

      if (APIExecute_C(func, ch, &ret_value, ec, 1)) EXIT(1);
      
      if (ret_value==NULL)
        *ret=newtarg("<no return>", 1, INTERM);
      else
        *ret=ret_value;
      return 0;
    }

  if (api_has_tcl_func(cf->NAME))
  {
     sprintf(buf1,"if {[catch {%s", cf->NAME);
     for (ch=cf->ARGS; ch!=NULL; ch=ch->NEXT)
      {
         tab[0]=(t_arg *)ch->DATA;
         if (tab[0]->POINTER!=1 && strcmp(tab[0]->TYPE,"char")!=0) break;
         strcat(buf1, " ");
         strcat(buf1, *(char **)tab[0]->VALUE);
      }
     if (ch==NULL)
     {
        strcat(buf1,"} msg options]==1} {\n"
                      "  set gns_tcl_error [dict get $options -errorinfo]\n"
                      "  0\n"
                      "}\n"
              );
        if (Tcl_EvalEx((Tcl_Interp *)TCL_INTERPRETER, buf1, -1, TCL_EVAL_GLOBAL)!=TCL_OK)
           {
             const char *res;
             res=Tcl_GetVar((Tcl_Interp *)TCL_INTERPRETER, "gns_tcl_error", TCL_GLOBAL_ONLY);
             avt_errmsg(GNS_ERRMSG, "167", AVT_ERROR, cf->NAME, /*Tcl_GetStringResult((Tcl_Interp *)TCL_INTERPRETER)*/res!=NULL?res:"?");
           }
        *ret=newtarg("<no return>", 1, INTERM);
        return 0;
     }
  }
  
  // .so
  sprintf(function_name,"wrap_%s",cf->NAME);
  
  libfunc=(libfunc_type)GetDynamicFunction(function_name);
  
  if (libfunc==NULL)
    {
      if (noerr) return 1;
      avt_errmsg(API_ERRMSG, "062", AVT_FATAL, function_name);
     // fprintf(stderr,"somewhere: function '%s' can't neither be found in interpreter nor in the dynamic libraries\n", function_name);
      EXIT(3);
    }
  
  for (nb=0, ch=cf->ARGS; ch!=NULL; nb++, ch=ch->NEXT) tab[nb]=(t_arg *)ch->DATA;
  j=libfunc(&ret_value, tab, nb, function_name);
  switch(j)
    {
    case 1:
      avt_errmsg(API_ERRMSG, "063", AVT_FATAL, function_name);
//      fprintf(stderr,"somewhere executing %s\n",function_name);
      EXIT(2);
    case 2:
    case 3:
      avt_errmsg(API_ERRMSG, "064", AVT_FATAL, function_name);
//      fprintf(stderr," error happens at somewhere when executing '%s'\n",function_name);
      EXIT(2);
    }
  
  ret_value->NAME=INTERM;
  // free the known types
  if (strcmp(ret_value->TYPE,"int")==0) { mbkfree(ret_value->TYPE); ret_value->TYPE="int"; }
  else if (strcmp(ret_value->TYPE,"void")==0) { mbkfree(ret_value->TYPE); ret_value->TYPE="void"; }
  else if (strcmp(ret_value->TYPE,"double")==0) { mbkfree(ret_value->TYPE); ret_value->TYPE="double"; }
  else if (strcmp(ret_value->TYPE,"char")==0) { mbkfree(ret_value->TYPE); ret_value->TYPE="char"; }
  else if (strcmp(ret_value->TYPE,"FILE")==0) { mbkfree(ret_value->TYPE); ret_value->TYPE="FILE"; }
  else { ret_value->TYPE=GetUnknownType(ret_value->TYPE); }
  *ret=ret_value;
  return 0;
}  

void APIPrintCallFunc(APICallFunc *cf, char *buf)
{
  chain_list *ch;
  t_arg *ta;

  strcpy(buf, cf->NAME);
  
  for (ch=cf->ARGS; ch!=NULL; ch=ch->NEXT)
    {
      ta=(t_arg *)ch->DATA;
      if (ta->POINTER>0)
        {
          if (strcmp(ta->TYPE, "char")==0)
            sprintf(&buf[strlen(buf)]," \"%s\"",*(char **)ta->VALUE);
          else
            sprintf(&buf[strlen(buf)]," %p",*(void **)ta->VALUE);
        }
      else if (strcmp(ta->TYPE,"int")==0)
        sprintf(&buf[strlen(buf)]," %d",*(int *)ta->VALUE);
      else if (strcmp(ta->TYPE,"char")==0)
        sprintf(&buf[strlen(buf)]," '%c'",*(char *)ta->VALUE);
      else if (strcmp(ta->TYPE,"double")==0)
        sprintf(&buf[strlen(buf)]," %e",*(double *)ta->VALUE);
      else if (strcmp(ta->TYPE,"long")==0)
        sprintf(&buf[strlen(buf)]," %ld",*(long *)ta->VALUE);
      else
        sprintf(&buf[strlen(buf)]," ?");
    }
}  

void APIFreeCallFunc(APICallFunc *cf)
{
  APIFreeTARGS(cf->ARGS);      
  mbkfree(cf);
}
