/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_C.c                                               */
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
#include <stdlib.h>
#include <errno.h>
#include MUT_H
#include AVT_H
#include "gen_tree_errors.h"
#include "gen_tree_utils.h"
#include "gen_tree_parser.h"
#include "gen_verif_C.h"

#define ASSIG_FLAG 0x10000000

static int BLOCK=0;                               /*number of current block*/
static int FORMAT_STOP=0;/*to memorize where mini-parser stopped to cut string*/


/***************************************************************************/
/*  change a tree of arguments in a list of ptype                          */
/***************************************************************************/
extern chain_list *args2chain(tree_list *tree, chain_list *var) 
{ 
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 75);
    //fprintf(stderr,"args2chain: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return args2chain(tree->DATA,var);
    break;
  case ',':
    var=args2chain(tree->NEXT->NEXT,var);       /*not to reverse order*/
    return args2chain(tree->NEXT,var);
    break;
  default:/*surely an expression*/
    return addchain(var,tree);
  }
}


/***************************************************************************/
/* return the element of list wich DATA field equals DATA field of ident   */
/* comparison is supposed to be on string                                  */
/* if not found NULL is returned and an error is dumped                    */
/***************************************************************************/
extern ptype_list *get_type(tree_list *ident, ptype_list *list) 
{
   ptype_list *p;
   char *name;
   if (!ident) {
      avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 76);
//      fprintf(stderr,"get_type: NULL pointer\n");
      EXIT(1);
   }  
   name=getname(ident);
   
   for (p=list;p;p=p->NEXT) {
      if (!p->DATA) {
         avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 77);
//         fprintf(stderr,"get_type: NULL pointer\n");
         EXIT(1);
      }  
      if (getname(p->DATA)==name) return p;   
   }
   
   avt_errmsg(API_ERRMSG, "005", AVT_ERROR, FILE_NAME(ident),LINE(ident), name);
//   fprintf(stderr,"%s:%d: '%s' not defined\n",FILE_NAME(ident),LINE(ident), name);
   Inc_Error();
   return NULL;
}


/****************************************************************************/
/* Build a list of declared variables listed in tree with the type type_exp */
/* Control double declaration. put on the top of env                        */
/****************************************************************************/
static inline ptype_list* Decl_Var(tree_list *tree, int type_exp, ptype_list *env)
{
  ptype_list *env2,*p;
  tree_list* elem;
  char *ident,*file;
  int lineno;
  if (!tree) { 
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 78);
    // fprintf(stderr,"Decl_Var: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return Decl_Var(tree->DATA,type_exp,env);
      break;
  /* left side is ignored and multitypes for the right one */
  case ',':
      env=Decl_Var(tree->NEXT,type_exp,env);
      return Decl_Var(tree->NEXT->NEXT,type_exp,env);
      break;
  /*affect*/  /*list of affect ?*/
  case '=':
      env2=Decl_Var(tree->NEXT,type_exp,env);
      if (env2!=env) {/*if new var (i.e no error)*/
         type_exp=TOKEN((tree_list*)env2->DATA);
         env2->TYPE |= ASSIG_FLAG;   /*mark variable has a value now*/
      }   
      return env2;
      break;
  case GEN_TOKEN_IDENT:
      if (type_exp==GEN_TOKEN_CHAR) type_exp=GEN_TOKEN_INTEGER;
      ident=getname(tree);
      for (p=env; p; p=p->NEXT) {
         if ((p->TYPE&(~ASSIG_FLAG))!=BLOCK) break;  /*control only in block*/
         if (getname((tree_list*)p->DATA)==ident) {
            avt_errmsg(API_ERRMSG, "006", AVT_ERROR, FILE_NAME(tree),LINE(tree),ident,LINE((tree_list*)p->DATA),FILE_NAME((tree_list*)p->DATA));
            //fprintf(stderr,"%s:%d: '%s' already used. primary declaration line %d in file %s\n",
            //   FILE_NAME(tree),LINE(tree),ident,LINE((tree_list*)p->DATA),FILE_NAME((tree_list*)p->DATA));
            Inc_Error();
            return env;
         }
      }
      lineno=LINE(tree);
      file=FILE_NAME(tree);
      elem=PUT_UNI(type_exp,Duplicate_Tree(tree));
      return addptype(env,BLOCK,elem);
      break;
  case GEN_TOKEN_ARRAY:
      if (type_exp==GEN_TOKEN_CHAR) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_CHAR,env);
      if (type_exp==GEN_TOKEN_FILE) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_FILE,env);
      if (type_exp==GEN_TOKEN_DOUBLE) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_DOUBLE,env);
      if (type_exp==GEN_TOKEN_VOIDTOKEN_D) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_VOID,env);
      if (type_exp==GEN_TOKEN_INTEGER) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_INT,env);
      if (type_exp==GEN_TOKEN_REF_CHAR) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_CHAR,env);
      if (type_exp==GEN_TOKEN_REF_FILE) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_FILE,env);
      if (type_exp==GEN_TOKEN_REF_DOUBLE) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_DOUBLE,env);
      if (type_exp==GEN_TOKEN_REF_VOID) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_VOID,env);
      if (type_exp==GEN_TOKEN_REF_INT) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_INT,env);
      avt_errmsg(API_ERRMSG, "007", AVT_ERROR, FILE_NAME(tree),LINE(tree));
    //  fprintf(stderr,"%s:%d: Only int*, void*, double *, char* and FILE* are accepted\n",FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return env;
      break;
  case GEN_TOKEN_REF:
      if (type_exp==GEN_TOKEN_CHAR) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_CHAR,env);
      if (type_exp==GEN_TOKEN_FILE) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_FILE,env);
      if (type_exp==GEN_TOKEN_DOUBLE) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_DOUBLE,env);
      if (type_exp==GEN_TOKEN_VOIDTOKEN_D) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_VOID,env);
      if (type_exp==GEN_TOKEN_INTEGER) return Decl_Var(tree->NEXT,GEN_TOKEN_REF_INT,env);
      avt_errmsg(API_ERRMSG, "007", AVT_ERROR, FILE_NAME(tree),LINE(tree));
      fprintf(stderr,"%s:%d: Only int*, void*, double *, char* and FILE* are accepted\n",FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return env;
      break;
  default:
      Error_Tree("Decl_Var",tree);
      EXIT(1);
	  return NULL; 
  }
}


/****************************************************************************/
/*           clean the tree for execution and send messages                 */
/* Var is a list of all generic variables from VHDL                         */
/* control double declaration of variables                                  */
/* useless statements as 3+4; is warned and erased                          */
/* uninitialised variables are warned                                       */
/* useless instructions are warned                                          */
/* illegal operations on types are counted as fatal errors                  */
/* fatal errors are counted                                                 */
/* return new tree                                                          */
/****************************************************************************/


extern tree_list* APIVerify_C(tree_list *tree, ptype_list *Var)
{
   ptype_list *p;
   ptype_list *q,*env=NULL;
   tree_list* elem;
   tree_list* entity_ident;
   int lineno;
   char* file;
   BLOCK=0;                               /*var. glob. number of block*/  

   if (TOKEN(tree)!=GEN_TOKEN_NODE) {
      Error_Tree("Verify_C",tree);
      EXIT(2);
   }
   if (TOKEN(tree->DATA)!=GEN_TOKEN_OF) {
      Error_Tree("Verify_C",tree->DATA);
      EXIT(2);
   }
   
   for (p=Var;p;p=p->NEXT) {
      elem=Duplicate_Tree(p->DATA);
/*      if (TOKEN(elem)!=GEN_TOKEN_INTEGER) {
         Error_Tree("Verify_C",elem);
         EXIT(2);
      }*/
      env=addptype(env,ASSIG_FLAG | BLOCK,elem);/*flag = var. already assigned*/
   }
   
   /*put name of entity as a defined char*  */
   entity_ident=getident_of(tree);
   lineno=LINE(entity_ident);
   file=FILE_NAME(tree);
   elem=PUT_UNI(GEN_TOKEN_REF_CHAR,Duplicate_Tree(entity_ident));
   env=addptype(env,ASSIG_FLAG | BLOCK,elem);/*flag = var. already assigned*/   
   
   /* Verify C */
   BLOCK=1;
   env=Verify_Type(tree->NEXT,env); 
   if (BLOCK!=1) {
      avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 79);
 //     fprintf(stderr,"Verify_C: computing error\n");
      EXIT(3);
   }
   
   for (q=env; q; q=q->NEXT) {Free_Tree((tree_list*)q->DATA);}
   freeptype(env);
   return tree;
}


/****************************************************************************/
/* search if there is a % parameter in format*/
/****************************************************************************/
static inline void Quick_Format_Exp(tree_list *tree)
{
  char *format=(char*) tree->DATA;
  int taille=strlen(format)-1;
  int integer=0,string=0,i;

  if (!format) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 80);
//    fprintf(stderr,"Quick_Format_Exp: NULL pointer\n");
    EXIT(1);
  }
  for (i=0;i<=taille;i++) {   
     /*nothing to do here*/
     /*format*/
     if (format[i]=='%') {
       i++;
       if (i>taille) {i--; goto label_error;}
       /*to put a '%' in flow*/
       if (format[i]=='%') {
         /*nothing to do here*/ 
         goto label_end_format;}
       /*flag*/
        switch (format[i]) {
          case '-': case '+': case ' ': case '0':  
               i++; break;
        }  
        if (i>taille) {i--; goto label_error;}
        /*size*/
        for (;i<=taille;i++) 
            if (format[i]>='0' && format[i]<='9') /*nothing to do*/; 
            else break;
        if (i>taille) {i--; goto label_error;}
      /*precision*/
        if (format[i]=='.') for (;i<=taille;i++) {
            if (format[i]>='0' && format[i]<='9') /*nothing to do*/; 
            else break;
        }    
        if (i>taille) {i--; goto label_error;}
        /*conversion*/
        if (format[i]=='h' || format[i]=='l' || format[i]=='L') 
            i++;
        if (i>taille) {i--; goto label_error;}
        /*argument*/
        switch (format[i]) {
          /*integer*/case 'd': case 'i': case 'o': case 'u': case 'x': 
          case 'X': /*floating point*/case 'f': case 'e': case 'E': 
          case 'g': case 'G': /*a char*/case 'c': 
            integer=1;
            i++;
            goto label_end;
            break;
          /*string*/case 's': 
          /*pointer on int*/case 'n': 
          /*pointer*/case 'p': 
            string=1;
            i++;
            goto label_end;
            break;
        }  
        label_error:
        avt_errmsg(API_ERRMSG, "008", AVT_ERROR, FILE_NAME(tree),LINE(tree),format[i]);
        //fprintf(stderr,"%s:%d: illegal format string '%%%c'\n",
        //          FILE_NAME(tree),LINE(tree),format[i]);
        Inc_Error();
      }/*end of format*/

      label_end_format:
      continue;
   }/*end of loop*/
 
   label_end:
   if (string || integer) {/*error*/
        avt_errmsg(API_ERRMSG, "009", AVT_ERROR, FILE_NAME(tree),LINE(tree));
//        fprintf(stderr,"%s:%d: not enough arguments for format\n",FILE_NAME(tree),LINE(tree));
        Inc_Error();
   }   
}


/****************************************************************************/
/* take the first '%' of format and return singlet build from the cut format*/
/* and the parameter tree. The type of tree is checked to match with format */
/* GLOBAL VARIABLE:  FORMAT_STOP  which is counter of parser on string      */
/****************************************************************************/
static inline tree_list *Format_Exp(char *format, tree_list *tree, char* file_name, int line)
{
  int taille=strlen(format)-1;
  char *buf;
  int integer=0,string=0,i,lineno;
  char *extract;
  char *file;

  buf=(char*)mbkalloc(taille+2);
  
  if (!format) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 81);
    // fprintf(stderr,"Format_Exp: NULL pointer\n");
    EXIT(1);
  }
  for (i=FORMAT_STOP;i<=taille;i++) {  /*FORMAT_STOP:where last parse stopped*/ 
     buf[i-FORMAT_STOP]=format[i];
     /*format*/
     if (format[i]=='%') {
       i++;
       if (i>taille) {i--; goto label_error;}
       /*to put a '%' in flow*/
       if (format[i]=='%') {
         buf[i-FORMAT_STOP]=format[i]; 
         goto label_end_format;}
       /*flag*/
        switch (format[i]) {
          case '-': case '+': case ' ': case '0': 
               buf[i-FORMAT_STOP]=format[i]; i++; break;
        }  
        if (i>taille) {i--; goto label_error;}
        /*size*/
        for (;i<=taille;i++) 
            if (format[i]>='0' && format[i]<='9') buf[i-FORMAT_STOP]=format[i]; 
            else break;
        if (i>taille) {i--; goto label_error;}
      /*precision*/
        if (format[i]=='.') for (;i<=taille;i++) {
            if (format[i]>='0' && format[i]<='9') buf[i-FORMAT_STOP]=format[i]; 
            else break;
        }    
        if (i>taille) {i--; goto label_error;}
        /*conversion*/
        if (format[i]=='h' || format[i]=='l' || format[i]=='L') {
            buf[i-FORMAT_STOP]=format[i]; i++;}
        if (i>taille) {i--; goto label_error;}
        /*argument*/
        switch (format[i]) {
          /*integer*/case 'd': case 'i': case 'o': case 'u': case 'x': 
          case 'X': /*floating point*/case 'f': case 'e': case 'E': 
          case 'g': case 'G': /*a char*/case 'c': 
            integer=1;
            buf[i-FORMAT_STOP]=format[i];
            i++;
            goto label_end;
            break;
          /*string*/case 's': 
            string=1;
            buf[i-FORMAT_STOP]=format[i];
            i++;
            goto label_end;
            break;
          /*pointer on int*/case 'n': 
            string=1;
            buf[i-FORMAT_STOP]=format[i];
			i++;
            goto label_end;
            break;
          /*pointer*/case 'p': 
            buf[i-FORMAT_STOP]=format[i];
            i++;
	    integer=1;
            goto label_end;
            break;
        }  
        label_error:
        avt_errmsg(API_ERRMSG, "008", AVT_ERROR, tree?FILE_NAME(tree):file_name, tree?LINE(tree):line,format[i]);
/*        fprintf(stderr,"%s:%d: illegal format string '%%%c'\n",
                  tree?FILE_NAME(tree):file_name,
                  tree?LINE(tree):line,format[i]);*/
        Inc_Error();
      }/*end of format*/

      label_end_format:
      continue;
   }/*end of loop*/
 
   label_end:
   buf[i-FORMAT_STOP]='\0';
   FORMAT_STOP=i;      /*var. glob. to memorize where mini-parser stopped*/
   extract=mbkalloc(strlen(buf)+1);
   strcpy(extract,buf);
   mbkfree(buf);
   file=tree?FILE_NAME(tree):file_name;
   lineno=tree?LINE(tree):line;                /*variable hidden in PUT_BIN*/
   if (string || integer) {
      if (tree) 
         return PUT_BIN(',', PUT_ATO(GEN_TOKEN_STRING,extract), Duplicate_Tree(tree));
      else /*error*/{
        avt_errmsg(API_ERRMSG, "009", AVT_ERROR, file_name, line);
//        fprintf(stderr,"%s:%d: not enough arguments for format\n",file_name,line);
        Inc_Error();
        return PUT_ATO(GEN_TOKEN_STRING,extract);
      }
   }   
   else if (tree) /*error*/{
        avt_errmsg(API_ERRMSG, "009", AVT_ERROR, FILE_NAME(tree),LINE(tree));
 //       fprintf(stderr,"%s:%d: too many arguments for format\n",FILE_NAME(tree),LINE(tree));
        Inc_Error();
        return  PUT_ATO(GEN_TOKEN_STRING,extract);
      }
   else return PUT_ATO(GEN_TOKEN_STRING,extract); 
}


/****************************************************************************/
/* for a complexe instruction (echo,print) with a format and n arguments    */
/* flatten in several instructions with a simple format and one argument    */
/****************************************************************************/
static inline void flatten_inst(tree_list *tree, ptype_list *env) {
   chain_list *p,*args;
   tree_list *cut_tree=NULL,*res;
   char *string,*file;
   int lineno,size;
   
   if (!tree) {
      avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 82);
      //fprintf(stderr,"flatten_inst: NULL pointer\n");
      EXIT(1);
   }

   /*control of tree*/
   if (TOKEN(tree)!=GEN_TOKEN_FPRINTF && TOKEN(tree)!=GEN_TOKEN_SPRINTF) {
      Error_Tree("flatten_inst",tree);
      EXIT(2);
   }  
   /*only one argument no need to flatten*/
   if (TOKEN(tree->NEXT->NEXT->DATA)!=',') {
      Quick_Format_Exp(tree->NEXT->NEXT->DATA);
      if (TOKEN(tree)==GEN_TOKEN_SPRINTF) TOKEN(tree)=GEN_TOKEN_BSPRINTF;
      return ;
   }   
   
   lineno=LINE(tree);
   file=FILE_NAME(tree);
   args = args2chain(tree->NEXT->NEXT->DATA,NULL);
   
   if (TOKEN((tree_list*)args->DATA)!=GEN_TOKEN_STRING) {
      avt_errmsg(API_ERRMSG, "010", AVT_ERROR, FILE_NAME(tree), LINE(tree));
//      fprintf(stderr,"%s:%d: only const char* accepted\n",FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return;
   }
   
   string = (char*)((chain_list*)args->DATA)->DATA;/*TOKEN(args->DATA)=STRING*/
   size=strlen(string)-1;
   FORMAT_STOP=0;           /*var. glob. for mini parser on variable string*/

/*flatten and build*/
   for (p=args->NEXT; p && FORMAT_STOP<=size; p=p->NEXT) {
      res=Format_Exp(string,p->DATA,file,lineno);
      if (cut_tree)  cut_tree=PUT_BIN(';',cut_tree,PUT_BIN(TOKEN(tree),Duplicate_Tree(tree->NEXT->DATA),res));
      else cut_tree=PUT_BIN(TOKEN(tree),Duplicate_Tree(tree->NEXT->DATA),res);
   }
   if (FORMAT_STOP<=size) {
      res=Format_Exp(string, p?p->DATA:NULL, file, lineno);
      if (cut_tree)  cut_tree=PUT_BIN(';',cut_tree,PUT_BIN(TOKEN(tree),Duplicate_Tree(tree->NEXT->DATA),res));
      else cut_tree=PUT_BIN(TOKEN(tree),Duplicate_Tree(tree->NEXT->DATA),res);
   } 
   
   else if (p) {
        avt_errmsg(API_ERRMSG, "011", AVT_ERROR, FILE_NAME(tree),LINE(tree));
        // fprintf(stderr,"%s:%d: too many arguments for format\n",FILE_NAME(tree),LINE(tree));
        Inc_Error();
   }

   cut_tree=PUT_BIN(';',PUT_ATO(GEN_TOKEN_NOP,0),cut_tree);

   freechain(args);
   /*replace one fprintf by several*/
   Free_Tree(tree->NEXT->NEXT);
   Free_Tree(tree->NEXT);
   FILE_NAME(tree)=FILE_NAME(cut_tree);       /*keep the head*/
   TOKEN(tree)=TOKEN(cut_tree);       /*keep the head*/
   LINE(tree)=LINE(cut_tree);       /*keep the head*/
   tree->NEXT=cut_tree->NEXT;       /* because there is no return */
   tree->DATA=cut_tree->DATA;
   cut_tree->NEXT=NULL;   /*to break recursivity of freetree*/
   freetree(cut_tree);
   if (TOKEN(tree)==GEN_TOKEN_SPRINTF) TOKEN(tree)=GEN_TOKEN_BSPRINTF;

   env  = NULL;
}


/****************************************************************************/
/*           clean the tree for execution and send messages                 */
/* Var is the list of all declared variables with their initialization      */
/*  unused statements as 3+4; is warned and erased                          */
/* uninitialised variables are warned                                       */
/* useless instructions are warned                                          */
/* verify number and type of arguments                                      */
/* illegal operations on types are counted as fatal errors                  */
/* fatal errors are counted but not returned                                */
/* return the new environment                                               */
/****************************************************************************/
extern ptype_list *Verify_Type(tree_list *tree, ptype_list *env)
{
  ptype_list *p,*env2;
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 83);
    // fprintf(stderr,"Verify_Type: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return Verify_Type(tree->DATA,env);
      break;
  case GEN_TOKEN_EXIT:
      return env;
      break;
  case GEN_TOKEN_BREAK:
      return env;
      break;
  case GEN_TOKEN_RETURN:
      return env;
      break;
  case GEN_TOKEN_BLOCK:
      BLOCK++;
      env2=Verify_Type(tree->NEXT,env);
      if (env!=env2) {
         for (p=env2; p; p=p->NEXT) {
            Free_Tree(p->DATA);
            if (p->NEXT==env) break;
         }
         if (!p) {
            avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 84);
            // fprintf(stderr,"Verify_Type: NULL pointer\n");
            EXIT(1);
         }
         p->NEXT=NULL;
         freeptype(env2);
      }   
      BLOCK--;
      return env;   /*return old variables*/
      break;
  case ';':
      env=Verify_Type(tree->NEXT,env);
      return Verify_Type(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_FCLOSE:
      return env;
      break;
  case GEN_TOKEN_STATIC:
      return Verify_Type(tree->NEXT,env);
      break;
  case GEN_TOKEN_INTEGER:
      return Decl_Var(tree->NEXT,GEN_TOKEN_INTEGER,env);
      break;
  case GEN_TOKEN_VOIDTOKEN_D:
      return Decl_Var(tree->NEXT,GEN_TOKEN_VOIDTOKEN_D,env);
      break;
  case GEN_TOKEN_DOUBLE:
      return Decl_Var(tree->NEXT,GEN_TOKEN_DOUBLE,env);
      break;
  case GEN_TOKEN_CHAR:
      return Decl_Var(tree->NEXT,GEN_TOKEN_CHAR,env);
      break;
  case GEN_TOKEN_FILE:
      return Decl_Var(tree->NEXT,GEN_TOKEN_FILE,env);
      break;
  case GEN_TOKEN_FPRINTF: 
  case GEN_TOKEN_SPRINTF: 
      flatten_inst(tree,env);   /*verify coherence with const char* argument*/
      return env;
      break;
  case GEN_TOKEN_EXCLUDE:
    /*a specialized function Verify_Exclude() has erased them from root block*/
      /*here are some contain in condition block as if , for ... non-sense*/
      avt_errmsg(API_ERRMSG, "012", AVT_ERROR, FILE_NAME(tree),LINE(tree));
      /*fprintf(stderr,"%s:%d: exclude can't be in a conditional block\n",
               FILE_NAME(tree),LINE(tree));*/
      Inc_Error();
      return env;
      break;
  case GEN_TOKEN_IF:
      return Verify_Type(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_ELSE:
      env=Verify_Type(tree->NEXT->NEXT,env);
      return Verify_Type(tree->NEXT->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_DO:
      env=Verify_Type(tree->NEXT,env);
      return env;
      break;
  case GEN_TOKEN_WHILE:
      return Verify_Type(tree->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_FOR:
      env=Verify_Type(tree->NEXT,env);
      env=Verify_Type(tree->NEXT->NEXT->NEXT,env);
      return Verify_Type(tree->NEXT->NEXT->NEXT->NEXT,env);
      break;
  case GEN_TOKEN_INC_BEFORE: case GEN_TOKEN_INC_AFTER:
  case GEN_TOKEN_DEC_BEFORE: case GEN_TOKEN_DEC_AFTER:
      return env;
      break;
  /*exp with affect --> look at this exp*/
  case '=':
      return env;
      break;
  /*several exp*/
  case ',':
      env=Verify_Type(tree->NEXT,env);
      return Verify_Type(tree->NEXT->NEXT,env);
      break;
  /*nothing to execute*/
  case GEN_TOKEN_NOP:
      return env;
      break;
  /*other cases are expressions alone*/
  default:
      return env;
  }
}


/****************************************************************************/
/*          count the number of arguments of a function                     */
/****************************************************************************/
extern int Count_Args(tree)
   tree_list *tree;
{  
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 84);
    //fprintf(stderr,"Count_Args: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return Count_Args(tree->DATA);
      break;
  case ',':
      return Count_Args(tree->NEXT)+Count_Args(tree->NEXT->NEXT);
      break;
  default: /* it's an expression*/       return 1;
  }
}


/****************************************************************************/
/*return the type of tree and verify that tree match with type_exp          */
/* env is the declared variables in field DATA and initialization in TYPE   */
/****************************************************************************/
extern int Type_Exp(tree,type_exp,env)
   tree_list *tree;
   int type_exp;
   ptype_list *env;
{
  ptype_list *elem;
  tree_list *var;
  int t;
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 85);
    // fprintf(stderr,"Type_Exp: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return Type_Exp(tree->DATA,type_exp,env);
      break;
  /*very special cases*/
  case GEN_TOKEN_INC_BEFORE: case GEN_TOKEN_INC_AFTER: case GEN_TOKEN_DEC_BEFORE: case GEN_TOKEN_DEC_AFTER:
      Type_Exp(tree->NEXT,GEN_TOKEN_INTEGER,env);
      return GEN_TOKEN_INTEGER;
      break;
  /*affect*/  /*list of affect ?*/
  case '=':
      elem=get_type(tree->NEXT,env);
      if (!elem) return type_exp;
      elem->TYPE|=ASSIG_FLAG;
      Type_Exp(tree->NEXT,type_exp,env);
      return Type_Exp(tree->NEXT->NEXT,type_exp,env);  /*polymorph type*/
      break;
  /*tri*/
  case '?':
      Type_Exp(tree->NEXT,GEN_TOKEN_INTEGER,env);
      Type_Exp(tree->NEXT->NEXT,type_exp,env);
      return Type_Exp(tree->NEXT->NEXT->NEXT,type_exp,env);  /*polymorph type*/
      break;
  /* left side is ignored and multitypes for the right one */
  case ',':
      Type_Exp(tree->NEXT,0,env);
      return Type_Exp(tree->NEXT->NEXT,type_exp,env);       /*polymorph type*/
      break;
  case '+': case '-': case '*': case '%': case '/': case '&': case '|': 
  case GNS_TOKEN_AND: case GEN_TOKEN_OR: case GEN_TOKEN_SHL: case GEN_TOKEN_SHR: case '^': 
      Type_Exp(tree->NEXT,GEN_TOKEN_INTEGER,env);
      Type_Exp(tree->NEXT->NEXT,GEN_TOKEN_INTEGER,env);
      return GEN_TOKEN_INTEGER;
      break;
  case '>': case '<': case GEN_TOKEN_SUPEG: case GEN_TOKEN_INFEG: case GEN_TOKEN_EG: case GEN_TOKEN_NOTEG:
      type_exp=Type_Exp(tree->NEXT,0,env);
      Type_Exp(tree->NEXT->NEXT,type_exp,env);  /*polymorph type*/
      return GEN_TOKEN_INTEGER;
      break;
  /*unary*/
  case GEN_TOKEN_OPPOSITE: case '~': case GEN_TOKEN_NOT:
      Type_Exp(tree->NEXT,GEN_TOKEN_INTEGER,env);
      return GEN_TOKEN_INTEGER;
      break;
  case GEN_TOKEN_REF:
      t=Type_Exp(tree->NEXT,0,env);
      if ((!type_exp || type_exp==GEN_TOKEN_INTEGER) && t==GEN_TOKEN_REF_CHAR) return GEN_TOKEN_INTEGER;
      if ((!type_exp || type_exp==GEN_TOKEN_FILE) && t==GEN_TOKEN_REF_FILE) return GEN_TOKEN_FILE;
      if ((!type_exp || type_exp==GEN_TOKEN_DOUBLE) && t==GEN_TOKEN_REF_DOUBLE) return GEN_TOKEN_DOUBLE;
      if ((!type_exp || type_exp==GEN_TOKEN_VOIDTOKEN_D) && t==GEN_TOKEN_REF_VOID) return GEN_TOKEN_VOIDTOKEN_D;
      return GEN_TOKEN_REF;
      avt_errmsg(API_ERRMSG, "013", AVT_ERROR, FILE_NAME(tree),LINE(tree));
      // fprintf(stderr,"%s:%d: '*' doesn't match with type\n",FILE_NAME(tree),LINE(tree));
      Inc_Error();
      return type_exp;
      break;   
   /*function*/
  case GEN_TOKEN_FUNCTION:
      return GEN_TOKEN_FUNCTION; 
  case GEN_TOKEN_MBKFOPEN:
      if (type_exp && GEN_TOKEN_REF_FILE!=type_exp) {
         avt_errmsg(API_ERRMSG, "014", AVT_ERROR, FILE_NAME(tree),LINE(tree));
         //fprintf(stderr,"%s:%d: function type FILE* doesn't match\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
      }
      Type_Exp(tree->NEXT,GEN_TOKEN_REF_CHAR,env);
      Type_Exp(tree->NEXT->NEXT,GEN_TOKEN_REF_CHAR,env);
      Type_Exp(tree->NEXT->NEXT->NEXT,GEN_TOKEN_REF_CHAR,env);
      return GEN_TOKEN_REF_FILE;
      break;
  case GEN_TOKEN_FOPEN:
      if (type_exp && GEN_TOKEN_REF_FILE!=type_exp) {
         avt_errmsg(API_ERRMSG, "014", AVT_ERROR, FILE_NAME(tree),LINE(tree));
         //fprintf(stderr,"%s:%d: function type FILE* doesn't match\n",FILE_NAME(tree),LINE(tree));
         Inc_Error();
      }
      Type_Exp(tree->NEXT,GEN_TOKEN_REF_CHAR,env);
      Type_Exp(tree->NEXT->NEXT,GEN_TOKEN_REF_CHAR,env);
      return GEN_TOKEN_REF_FILE;
      break;
  /*terminal*/
  case GEN_TOKEN_IDENT:
      elem=get_type(tree,env);
      if (!elem) return type_exp;
      var=(tree_list*)elem->DATA;
      t=TOKEN(var);
      if (type_exp && t!=type_exp) {
         avt_errmsg(API_ERRMSG, "015", AVT_ERROR, FILE_NAME(tree),LINE(tree),getname(var));
         /*fprintf(stderr,"%s:%d: type of variable '%s' doesn't match\n",
                 FILE_NAME(tree),LINE(tree),getname(var));*/
         Inc_Error();
      }
      else if (!(elem->TYPE & ASSIG_FLAG)) {
         avt_errmsg(API_ERRMSG, "016", AVT_ERROR, FILE_NAME(tree),LINE(tree),getname(var));
         /*fprintf(stderr,"%s:%d: warning: '%s' might be used uninitialized\n",
                 FILE_NAME(tree),LINE(tree),getname(var));*/
      }
      return t;                                    /* polymorph type */
      break;
  case GEN_TOKEN_DIGIT:
      if (type_exp && GEN_TOKEN_INTEGER!=type_exp) {
         avt_errmsg(API_ERRMSG, "017", AVT_ERROR, FILE_NAME(tree),LINE(tree),(int)(long)tree->DATA);
         /*fprintf(stderr,"%s:%d: digit '%d' doesn't match\n",
                 FILE_NAME(tree),LINE(tree),(int)(long)tree->DATA);*/
         Inc_Error();
      }
      return GEN_TOKEN_INTEGER;
      break;
  case GEN_TOKEN_DIGIT_DOUBLE:
      if (type_exp && GEN_TOKEN_DOUBLE!=type_exp) {
         avt_errmsg(API_ERRMSG, "017", AVT_ERROR, FILE_NAME(tree),LINE(tree),(int)(long)tree->DATA);
         /*fprintf(stderr,"%s:%d: digit '%e' doesn't match\n",
                 FILE_NAME(tree),LINE(tree),*(double *)tree->DATA);*/
         Inc_Error();
      }
      return GEN_TOKEN_DOUBLE;
      break;
  case GEN_TOKEN_STRING:
      if (type_exp && GEN_TOKEN_REF_CHAR!=type_exp) {
         avt_errmsg(API_ERRMSG, "018", AVT_ERROR, FILE_NAME(tree),LINE(tree),(char*)tree->DATA);
          /*fprintf(stderr,"%s:%d: string '%s' doesn't match\n",
                 FILE_NAME(tree),LINE(tree),(char*)tree->DATA);*/
         Inc_Error();
      }
      return GEN_TOKEN_REF_CHAR;
      break;
  case GEN_TOKEN_STDERR: case GEN_TOKEN_STDOUT:
      if (type_exp && GEN_TOKEN_REF_FILE!=type_exp) {
         avt_errmsg(API_ERRMSG, "019", AVT_ERROR, FILE_NAME(tree),LINE(tree),(char*)tree->DATA);
         /*fprintf(stderr,"%s:%d: flow '%s' doesn't match\n",
                 FILE_NAME(tree),LINE(tree),(char*)tree->DATA);*/
         Inc_Error();
      }
      return GEN_TOKEN_REF_FILE;
      break;
  default:
      Error_Tree("Type_Exp",tree);
      EXIT(1);

	  return 0;
  }
}



