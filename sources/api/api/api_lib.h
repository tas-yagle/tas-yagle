/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : API Version 1.00                                             */
/*    Fichier : api100.h                                                     */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef API
#define API

#ifndef GRAB_IT
# define GRAB_IT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include MUT_H
/* ----- Types ------------------------------------------------------------- */

// ATTENTION: GARDER CES COMMENTAIRES
//GRAB_IT #define T_POINTER    0x1   /* pointer  */
//GRAB_IT #define T_INT        0x2   /* int      */
//GRAB_IT #define T_LONG       0x3   /* long     */
//GRAB_IT #define T_CHAR       0x4   /* char     */
//GRAB_IT #define T_DOUBLE     0x5   /* double   */
//GRAB_IT #define T_VOID       0x6   /* no type  */

#ifndef T_POINTER
# define T_POINTER    0x1   /* pointer  */
# define T_INT        0x2   /* int      */
# define T_LONG       0x3   /* long     */
# define T_CHAR       0x4   /* char     */
# define T_DOUBLE     0x5   /* double   */
# define T_VOID       0x6   /* no type  */
#endif

/* ----- Structures -------------------------------------------------------- */

GRAB_IT typedef struct t_arg {                       
GRAB_IT     char *NAME;
GRAB_IT     char *TYPE;
GRAB_IT     void *VALUE;
GRAB_IT     int   POINTER; /* pointer level, 0 1 2 3 ... */
GRAB_IT } t_arg;

/* ----- Functions --------------------------------------------------------- */

GRAB_IT extern t_arg *NewArg ();
GRAB_IT extern int TypeCheck (t_arg *arg, char *type);
GRAB_IT extern int GetPointer (t_arg *arg);
GRAB_IT extern void SetPointer (t_arg *arg, int p);
GRAB_IT extern void SetType (t_arg *arg, char *t);

void LoadDynamicLibraries(FILE *debug); // debug==NULL => pas de trace
void *GetDynamicFunction(char *function_name);

extern chain_list *API_DEFINES;
extern chain_list *API_ACTION_INIT;
extern chain_list *API_ACTION_END;
extern chain_list *API_ACTION_RESTART;
extern chain_list *API_ACTION_TOPLEVEL;



typedef struct tree 
{
  struct tree *NEXT;
  struct tree *DATA;
  int          TOKEN;
  int          LINE;
  char        *FILE_NAME;
} tree_list;

typedef struct SymInfoItem
{
  struct SymInfoItem *NEXT;
  char *ConnectorName;
  char *ConnectorRadical;
  int ConnectorIndex;
  int FLAGS;
} SymInfoItem;

typedef struct
{
  ptype_list *EXCLUDES, *WITHOUTS, *EXCLUDES_AT_END,*STOP_POWER,*FORCEMATCH;
} misc_stuffs;

extern SymInfoItem *AddSymInfoItem(SymInfoItem *head);

/*MACROs for the tree result*/
#define PUT_ATO(t,val) (PutAto_Tree(t,file,lineno,(void*)val))     /*create*/
#define PUT_UNI(t,branch) (PutUni_Tree(t,file,lineno,branch))      /*create*/
#define PUT_BIN(t,b1,b2) (PutBin_Tree(t,file,lineno,b1,b2))        /*create*/
#define PUT_TRI(t,b1,b2,b3) (PutTri_Tree(t,file,lineno,b1,b2,b3))  /*create*/
#define PUT_QUA(t,b1,b2,b3,b4) (PutQua_Tree(t,file,lineno,b1,b2,b3,b4))
#define LINE(pt) ((pt)->LINE)	                  /*to recover the line fault*/
#define TOKEN(pt) ((pt)->TOKEN)	                    /*to recover the token*/
#define FILE_NAME(pt) ((pt)->FILE_NAME)              /*to recover the token*/
#define VALUE(pt) ((int) pt->DATA)                     /* to recover value */
#define CHANGE_ATO(tr,val) (ChangeAto_Tree(tr, (void *)val))
#define GET_ATO(tr) (tr->DATA)

/****************************************************************************/
/*                   add a tree elem at the top                             */
/****************************************************************************/
extern tree_list* addtree(char* file, int lineno, int token, tree_list* data, tree_list* next);

/****************************************************************************/
/*              free recursively a tree_list from the top                   */
/****************************************************************************/
extern void freetree(tree_list* tree);

/****************************************************************************/
/*               build an atomic tree with a value                          */
/****************************************************************************/
extern tree_list *PutAto_Tree(int token, char* file, int lineno, void *value);
extern void ChangeAto_Tree(tree_list *tree, void *value);
/****************************************************************************/
/*         build an unary operator typed token with its branch              */
/****************************************************************************/
extern tree_list *PutUni_Tree(int token, char* file, int lineno, tree_list *b);

/****************************************************************************/
/*         build an binary operator typed token with its branches           */
/****************************************************************************/
extern tree_list *PutBin_Tree(int token, char* file, int lineno, 
                               tree_list *b1, tree_list *b2);

/****************************************************************************/
/*            build an operator typed token with its 3 branches             */
/****************************************************************************/
extern tree_list *PutTri_Tree(int token, char* file, int lineno, 
                               tree_list *b1, tree_list *b2, tree_list *b3);

/****************************************************************************/
/*            build an operator typed token with its 4 branches             */
/****************************************************************************/
extern tree_list *PutQua_Tree(int token, char* file, int lineno, 
                               tree_list *b1, tree_list *b2,
                               tree_list *b3, tree_list *b4);

/***************************************************************************/
/*            return the string of the first ident met                     */
/***************************************************************************/
extern char *getname(tree_list *tree);

/***************************************************************************/
/*             return the string of the second ident met                   */
/***************************************************************************/
extern char *getname_of(tree_list *tree);

/***************************************************************************/
/*                      return the first ident met                         */
/***************************************************************************/
extern tree_list *getident(tree_list *tree);

/***************************************************************************/
/*                   return the second ident met                           */
/***************************************************************************/
extern tree_list *getident_of(tree_list *tree);

/***************************************************************************/
/*                       return size of vector                             */
/* tree is the vector and env the values of variables in expressions       */
/***************************************************************************/
extern int getsize(tree_list *tree, ptype_list *env);

/***************************************************************************/
/*              return a list of a tree structure                          */
/***************************************************************************/
extern chain_list *tree2chain(tree_list *tree, chain_list *pile);

/****************************************************************************/
/*  return a duplicate tree_list of tree.  Rq: strings aren't reallocated  */
/****************************************************************************/
extern tree_list *Duplicate_Tree(tree_list *tree);

/****************************************************************************/
/*  Dump on FILE *GENIUS_OUTPUT the syntaxical tree typed in file2tree.h    */
/****************************************************************************/
extern void Dump_Tree(tree_list *tree);

/****************************************************************************/
/*   Dump an error message on the uppest token of tree. Rq: the argument    */
/*               message should be a function name                          */
/*Rq: the number line dumped is just an information, it could be completely */
/* wrong... We only guaranty that for tokens _IDENT, _STRING and _DIGIT, it */
/* is the good one. Anyway mostly, the number line is around the error ;-)  */
/****************************************************************************/
extern void Error_Tree(char *message,tree_list *tree);

/****************************************************************************/
/*       Free all the tree, just stay strings in memory(namealloc)          */
/****************************************************************************/
extern void Free_Tree(tree_list *tree);

/****************************************************************************/
/*               return 1 if token is met in tree                           */
/****************************************************************************/
extern int SeekToken_Tree(tree_list* tree, int token);

void APIInit();
void APIExit();

typedef struct static_struct 
{
  struct static_struct *NEXT;
  t_arg *VARIABLE;
  tree_list *TREE;
  void *VALUE;
} static_list;

typedef struct ExecutionContext
{
  chain_list *globalvars;
  chain_list *BLOCKS;
  static_list *STATIC_LIST;
  chain_list *ALL_USED_FUNCTIONS;
} ExecutionContext;


typedef struct
{
  char *left;
  char type;
  union
  {
    char *name;
    double value;
  } right;
  char *FILE;
  int LINE;
} generic_map_info;

typedef int (*libfunc_type)(t_arg **ret, t_arg **prm, int n_params, char *errstr);
typedef int (*libgetargfunc_type)(t_arg ***prm, int *n_params);

ExecutionContext *APINewExecutionContext();
void APIFreeExecutionContext(ExecutionContext *ec);

tree_list *APIParseFile(FILE *f, char *filename, ExecutionContext *ec, chain_list *defines);
tree_list *APIVerify_C(tree_list *tree, ptype_list *Var);
int APICheckCFunctions(ExecutionContext *ec);
int APIExecute_C(tree_list *tree, chain_list *args, t_arg **ret_value, ExecutionContext *ec, int disablecore);
//int APIExecute_C(tree_list *tree, chain_list *vars/*ptype_list *env*/, ExecutionContext *ec, int disablecore);

int APIBlockIsC(tree_list *tree);
tree_list *APIGetCTree(tree_list *tree);
int APIBlockIsArchitecture(tree_list *tree);
char *APIGetCName(tree_list *tree);
tree_list *APIGetCFunction(ExecutionContext *ec, char *name, char sensi);

int APIBlockIsEntity(tree_list *tree);
char *APIEntityName(tree_list *tree);
int Inc_Error();
int Get_Error();

t_arg *APIGetGlobalVariable(ExecutionContext *ec, char *name);

chain_list *APIAddIntTARG(chain_list *head, char *name, int value);
chain_list *APIAddLongTARG(chain_list *head, char *name, long value);
chain_list *APIAddCharTARG(chain_list *head, char *name, char value);
chain_list *APIAddPointerTARG(chain_list *head, char *name, char *type, int level, void *value);
chain_list *APIAddDoubleTARG(chain_list *head, char *name, double value);

void APIFreeTARGS(chain_list *head);
chain_list *APIAddTARG(chain_list *head, char *type, int pointer, char *name);
void APIFreeTARG(t_arg *ta);
void APIVerify_C_Functions(ExecutionContext *ec);
t_arg *APINewTARG(char *type, int pointer, char *name);
chain_list *APIGetCFunctionARGS(tree_list *tree);
chain_list *APIGetCFunctionReturnARG(tree_list *tree);
chain_list *APIPrepareFunctionARGS(tree_list *func, t_arg **tab, int nb, tree_list *where);


t_arg *APICreateIntTARG(char *name, int value);
t_arg *APICreateLongTARG(char *name, long value);
t_arg *APICreateCharTARG(char *name, char value);
t_arg *APICreatePointerTARG(char *name, char *type, int level, void *value);
t_arg *APICreateDoubleTARG(char *name, double value);


typedef struct
{
  char *NAME;
  chain_list *ARGS;
} APICallFunc;

chain_list *APIDupARGS(chain_list *cl);
ExecutionContext *APIGetExecutionContext();
int APIExecAPICallFunc(ExecutionContext *ec, APICallFunc *cf, t_arg **ret, int noerr);
void APIPrintCallFunc(APICallFunc *cf, char *buf);
void APIFreeCallFunc(APICallFunc *cf);

typedef struct lib_entry
{
  struct lib_entry *NEXT;
  char *name, *entity, *model, *subfigurename;
  char *match;
  int priority, format, keep;
  chain_list *paramset;
  tree_list *tree;
} lib_entry;

lib_entry *APIReadLibrary(char *library, char *path, FILE *debug);
void APIFreeLibrary(lib_entry *le);
int ReadAllCorVHDL(ExecutionContext *ec, lib_entry *files_list, FILE *debug);

void APICallApiInitFunctions();
void APICallApiTerminateFunctions();
int APIInParser();
FILE *APIFindFile(lib_entry *p);
void APIPreprocess(char *src, char *dest, chain_list *defines);

#define MAX_VHD_RULE_SIZE 32*1024

typedef struct
{
  char *ORIG;
  char *DEST;
} api_define_type;

typedef struct
{
  char *new_name;
  char *orig_name;
  chain_list *defines;
} template_corresp;

chain_list *dup_adt_list(chain_list *cl);
api_define_type *create_adt(char * orig, char *dest);
void free_adt_list(chain_list *cl);
int gen_find_template_corresp(ht *base_ht, chain_list *defines, char *name, char *result);
int gen_find_reverse_template_corresp(chain_list *defines, char *name, char *result);
template_corresp *gen_add_template_corresp(ht *base_ht, char *new_name, char *orig_name, chain_list *defines);
template_corresp *gen_get_template_corresp(ht *base_ht, char *name);
void APIParseFile_SetTemplateInfo(ht *head);
char *getdistpath_2(char *temp);
int api_has_tcl_func(char *name);



#define BASIC_API  1
#define SPI_API    2
#define TIMING_API 4
#define FUNC_API   8
#define CTK_API    16
#define YAGLE_API  32
#define TMA_API    64

#ifdef BASIC_TOKEN
#define TOKEN_MASK BASIC_API
#define TOKEN_NAME "basic_api"
#else
#if SPI_TOKEN
#define TOKEN_MASK SPI_API
#define TOKEN_NAME "spi_api"
#else
#if TIMING_TOKEN
#define TOKEN_MASK TIMING_API
#define TOKEN_NAME "timing_api"
#else
#if FUNC_TOKEN
#define TOKEN_MASK FUNC_API
#define TOKEN_NAME "func_api"
#else
#if CTK_TOKEN
#define TOKEN_MASK CTK_API
#define TOKEN_NAME "ctk_api"
#else
#if YAGLE_TOKEN
#define TOKEN_MASK YAGLE_API
#define TOKEN_NAME "yagle_api"
#endif // yagle_token
#endif // ctk_token
#endif // func_token
#endif // timing_token
#endif // spi_token
#endif // basic_token
extern int token_already_taken;

#ifdef AVERTEC_LICENSE
#define API_TEST_TOKEN_SUB(x,y) if ((token_already_taken & x)==0) \
                                { \
                                  token_already_taken|=x; \
                                  if(avt_givetoken("AVT_LICENSE_SERVER", y)!=AVT_VALID_TOKEN) EXIT(1); \
                                }
#define API_TEST_TOKEN API_TEST_TOKEN_SUB(TOKEN_MASK, TOKEN_NAME)
#else
#define API_TEST_TOKEN
#define API_TEST_TOKEN_SUB(x,y)
#endif


#endif //TOBECUT
