/**************************/
/*     eqt_util.c         */
/**************************/
/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include AVT_H
#include EQT_H
#include MUT_H

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

#ifndef __func__
#define __func__ "eqt_util"
#endif

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/
struct
{
  char *prefix;
  double value;
  char size;
} basic_units[]=
  {
    {"meg", 1e+6, 3},
    {"mil", 25.4e-6, 3},
    {"mi", 25.4e+6, 2},
    {"u", 1e-6, 1},
    {"n", 1e-9, 1},
    {"p", 1e-12, 1},
    {"m", 1e-3, 1},
    {"f", 1e-15, 1},
    {"v", 1, 1},
    {"s", 1, 1},
    {"k", 1e+3, 1},
    {"a", 1e-18, 1},
    {"g", 1e+9, 1},
    {"t", 1e+12, 1}
  };

eqt_ctx *EQT_CONTEXT_HIERARCHY[5]={NULL, NULL, NULL, NULL, NULL};
ht *eqt_ExprToNode=NULL;


extern int   eqtparse();
//extern int   eqtdebug;
eqt_ctx *EQT_CTX = NULL;

extern eqt_node     *eqt_NegNode           (eqt_node *node);
extern eqt_node     *eqt_InvNode           (eqt_node *node);
extern eqt_node     *eqt_DelMinDiv         (eqt_node *node);
extern eqt_node     *eqt_DelNeutral        (eqt_node *node);
extern char         *eqt_ToStr             (eqt_node *node);
extern void          eqt_drive             (eqt_node *node);
extern char         *eqt_operToStr         (long operator);

double               eqt_get_neutral       (long operator);
void                 eqt_delnode           (eqt_node *node);
eqt_node            *eqt_insertOperation   (eqt_node *node, eqt_node *operation);
int                  eqt_findSimpleAssoc   (eqt_node *node, char* var, double nb);
eqt_node            *eqt_associate         (eqt_node *node);
int                  eqt_isLeaf            (eqt_node *node);
int                  eqt_isOperand2        (eqt_node *node);
int                  eqt_isRealLeaf        (eqt_node *node);
void                 eqt_invertChild       (eqt_node *node);
eqt_node           **eqt_sortDoTable       (eqt_node *node,int cpt,int *nb);
int                  eqt_sortCompareNode   (const void *node1, const void *node2);
void                 eqt_sortGetSortedNode (eqt_node *node, eqt_node **table);
void                 eqt_sortBranch        (eqt_node *node);
void                 eqt_printTree         (eqt_node *node,int deep);
int                  eqt_linearise         (eqt_node *node);
int                  eqt_isZero            (eqt_node *node);
int                  eqt_isOne             (eqt_node *node);
int                  eqt_needParenthesis   (eqt_node *node);
int                  eqt_findAssoc         (eqt_node *node, char* var);
void                 eqt_assocVar          (eqt_node *node);
int                  eqt_addNeutral        (eqt_node *node);
void                 eqt_error             (char *text, ...);
void                 eqt_warning           (char *text, ...);


static eqt_node     *Oper1                 (eqt_node *node);
static eqt_node     *Oper2                 (eqt_node *node);
static eqt_node     *Oper3                 (eqt_node *node);
static eqt_node     *Oper                  (eqt_node *node);
static void          NewOper1              (eqt_node *node,eqt_node *n1);
static void          NewOper2              (eqt_node *node,eqt_node *n2);
static void          NewOper3              (eqt_node *node,eqt_node *n3);
static void          NewOper               (eqt_node *node,eqt_node *n);
static void          NewOperator           (eqt_node *node, long operator);
static char         *Varname               (eqt_node *node);
static double        Value                 (eqt_node *node);
static void          NewValue              (eqt_node *node, double value);
static long          Operator              (eqt_node *node);
static int           IsVarname             (eqt_node *node);
static int           IsValue               (eqt_node *node);
static int           IsNeg                 (eqt_node *node);
static eqt_node     *eqt_getNeutralNode    (int operator);
static chain_list   *eqt_assocGetVar       (eqt_node *node);
static void          eqt_assocPutVarTop    (eqt_node *node,char *var);
static void          eqt_assocFather       (eqt_node *node, char *var);
static void          NewValue              (eqt_node *node, double value);
static void          NewVarname            (eqt_node *node, char *varname);
static void          SubstN1ByN2           (eqt_node *n1,eqt_node *n2, char mode);
static void          EvalVarname           (eqt_ctx *ctx, eqt_node *node);
static void          eqt_addimportedvar    (eqt_ctx *ctx, char *var_name, double value);
inline double __eqt_getvar (eqt_ctx *ctx, char *var_name, int usenamealloc, int ctxonly);

/****************************************************************************/
/*     functions for fast allocation                                        */
/****************************************************************************/

static int EQT_GOT_RANDOM=0;
static HeapAlloc *eqt_heap[7]={NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/*char *eqt_lookup_expr(char *expr)
{
  long l;
  char *sn;
  if (eqt_ExprToNode==NULL)
    eqt_ExprToNode=addht(3000);
  sn=sensitive_namealloc(expr);
  return sn;
}*/

static eqt_node *eqt_loopup_node(eqt_ctx *ctx, char *expr)
{
  long l;
  eqt_node *node;
  if (eqt_ExprToNode==NULL) eqt_ExprToNode=addht(3000);
  if ((l=gethtitem(eqt_ExprToNode, expr))!=EMPTYHT)
    return (eqt_node *)l;
  node = eqt_create(ctx, expr);
  addhtitem(eqt_ExprToNode, expr, (long)node);
  return node;
}

static int eqt_getheap_num(char what)
{
  int num;
  if (eqt_heap[0]==NULL)
    {
      for (num=0; num<7; num++)
        eqt_heap[num]=(HeapAlloc *)mbkalloc(sizeof(HeapAlloc));
      CreateHeap(sizeof(double), 1024, eqt_heap[0]);
      CreateHeap(sizeof(struct eqt_ctx), 128, eqt_heap[1]);
      CreateHeap(sizeof(struct eqt_node), 1024, eqt_heap[2]);
      CreateHeap(sizeof(struct eqt_unary), 1024, eqt_heap[3]);
      CreateHeap(sizeof(struct eqt_binary), 1024, eqt_heap[4]);
      CreateHeap(sizeof(struct eqt_ternary), 1024, eqt_heap[5]);
      CreateHeap(sizeof(eqt_func_entry), 256, eqt_heap[6]);
    }
  switch(what)
    {
    case 'd' : num=0; break;
    case 'c' : num=1; break;
    case 'n' : num=2; break;
    case 'u' : num=3; break;
    case 'b' : num=4; break;
    case 't' : num=5; break;
    case 'f' : num=6; break;
    default:
      exit(1);
    }
  return num;
}
void *eqt_alloc(char what)
{
  int num;
  num=eqt_getheap_num(what);
  return AddHeapItem(eqt_heap[num]);
}

void eqt_free(char what, void *d)
{
  int num;
  num=eqt_getheap_num(what);
  DelHeapItem(eqt_heap[num], d);
}

static long eqt_my_update(int nouveau, long old_value, void *my_data)
{
  double    *var_tmp;
  if (nouveau==0)
    {
      *(double *)old_value=*(double *)my_data;
      return old_value;
    }
  var_tmp  = eqt_alloc('d');
  *var_tmp =*(double *)my_data;
  return (long)var_tmp;
}

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

/****************************************************************************/
/*{{{                    eqt_error()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_error(char *text, ...)
{
  va_list    arg;

  va_start(arg,text);
  fprintf(stderr,"[EQT_ERR]");
  if (text)
  {
    fprintf(stderr," ");
    vfprintf(stderr,text,arg);
  }
  fprintf(stderr,"\n");
  va_end(arg);

}

/*}}}************************************************************************/
/*{{{                    eqt_warning()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_warning(char *text, ...)
{
  va_list    arg;

  va_start(arg,text);
  fprintf(stderr,"[EQT_WAR]");
  if (text)
  {
    fprintf(stderr," ");
    vfprintf(stderr,text,arg);
  }
  fprintf(stderr,"\n");
  va_end(arg);
}

/*}}}************************************************************************/
/*{{{                    eqt_set_father()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_set_father(eqt_node *child, eqt_node* father)
{
  if (child)
    child->FATHER = father;
}

/*}}}************************************************************************/
/*{{{                    Value()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static double Value(eqt_node *node)
{
  return node->UNODE.VALUE;
}

/*}}}************************************************************************/
/*{{{                    IsValue()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int IsValue(eqt_node *node)
{
  return (node->TYPE == EQTVALUE_T);
}

/*}}}************************************************************************/
/*{{{                    IsVarname()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int IsVarname(eqt_node *node)
{
  return (node->TYPE == EQTVARNAME_T);
}

/*}}}************************************************************************/
/*{{{                    Varname()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *Varname(eqt_node *node)
{
  if (node->TYPE == EQTVARNAME_T)
    return node->UNODE.VARNAME;
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    Oper1()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static eqt_node *Oper1(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTUNOP_T :
         return node->UNODE.UNOP->OPERAND;
    case EQTBINOP_T :
         return node->UNODE.BINOP->OPERAND1;
    case EQTTERNOP_T :
         return node->UNODE.TERNOP->OPERAND1;
    default :
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    Oper2()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static eqt_node *Oper2(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTUNOP_T :
         return node->UNODE.UNOP->OPERAND;
    case EQTBINOP_T :
         return node->UNODE.BINOP->OPERAND2;
    case EQTTERNOP_T :
         return node->UNODE.TERNOP->OPERAND2;
    default :
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    Oper3()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static eqt_node *Oper3(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTUNOP_T :
         return node->UNODE.UNOP->OPERAND;
    case EQTTERNOP_T :
         return node->UNODE.TERNOP->OPERAND3;
    default :
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    Oper()                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static eqt_node *Oper(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTUNOP_T :
         return node->UNODE.UNOP->OPERAND;
    default :
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    Operator()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static long Operator(eqt_node *node)
{
  if (node)
    switch (node->TYPE)
    {
      case EQTUNOP_T :
           return node->UNODE.UNOP->OPERATOR;
      case EQTBINOP_T :
           return node->UNODE.BINOP->OPERATOR;
      case EQTTERNOP_T :
           return node->UNODE.TERNOP->OPERATOR;
      default :
           return 0;
    }
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    NewVarname()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void NewVarname(eqt_node *node, char *varname)
{
  if (node)
    switch (node->TYPE)
      case EQTVARNAME_T :
           node->UNODE.VARNAME = varname;
           return;
}

/*}}}************************************************************************/
/*{{{                    NewValue()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void NewValue(eqt_node *node, double value)
{
  if (node)
    switch (node->TYPE)
      case EQTVALUE_T : 
           node->UNODE.VALUE = value;
           return;
  avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//  eqt_error(__func__);
}

/*}}}************************************************************************/
/*{{{                    NewOperator()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void NewOperator(eqt_node *node, long operator)
{
  if (node)
    switch (node->TYPE)
    {
      case EQTUNOP_T :
           node->UNODE.UNOP->OPERATOR   = operator;
      case EQTBINOP_T :
           node->UNODE.BINOP->OPERATOR  = operator;
      case EQTTERNOP_T :
           node->UNODE.TERNOP->OPERATOR = operator;
      default :
           ;
    }
}

/*}}}************************************************************************/
/*{{{                    NewOper1()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void NewOper1(eqt_node *node,eqt_node *n1)
{
  switch (node->TYPE)
  {
    case EQTTERNOP_T :
         node->UNODE.TERNOP->OPERAND1 = n1;
         eqt_set_father(n1,node);
         break;
    case EQTBINOP_T :
         node->UNODE.BINOP->OPERAND1 = n1;
         eqt_set_father(n1,node);
         break;
    case EQTUNOP_T :
         NewOper(node,n1);
         break;
    default : ;
  }
}

/*}}}************************************************************************/
/*{{{                    NewOper2()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void NewOper2(eqt_node *node,eqt_node *n2)
{
  switch (node->TYPE)
  {
    case EQTTERNOP_T :
         node->UNODE.TERNOP->OPERAND2 = n2;
         eqt_set_father(n2,node);
         break;
    case EQTBINOP_T :
         node->UNODE.BINOP->OPERAND2 = n2;
         eqt_set_father(n2,node);
         break;
    case EQTUNOP_T :
         NewOper(node,n2);
         break;
    default : ;
  }
}

/*}}}************************************************************************/
/*{{{                    NewOper3()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void NewOper3(eqt_node *node,eqt_node *n3)
{
  switch (node->TYPE)
  {
    case EQTTERNOP_T :
         node->UNODE.TERNOP->OPERAND3 = n3;
         eqt_set_father(n3,node);
         break;
    case EQTUNOP_T :
         NewOper(node,n3);
         break;
    default : ;
  }
}

/*}}}************************************************************************/
/*{{{                    NewOper()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void NewOper(eqt_node *node,eqt_node *n)
{
  switch (node->TYPE)
  {
    case EQTUNOP_T :
         node->UNODE.UNOP->OPERAND = n;
         eqt_set_father(n,node);
         break;
    default : ;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_freenode()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_freenode(eqt_node *node)
{
  if (node)
    switch (node->TYPE)
    {
      case EQTVALUE_T :
           eqt_delnode (node);
           break;
      case EQTVARNAME_T :
           eqt_delnode (node);
           break;
      case EQTUNOP_T :
           eqt_freenode(Oper(node));
           eqt_delnode (node);
           break;
      case EQTBINOP_T :
           eqt_freenode(Oper1(node));
           eqt_freenode(Oper2(node));
           eqt_delnode (node);
           break;
      case EQTTERNOP_T :
           eqt_freenode(Oper1(node));
           eqt_freenode(Oper2(node));
           eqt_freenode(Oper3(node));
           eqt_delnode (node);
           break;
      case EQTDOTFUNC_T :
           {
             chain_list *cl;
             for (cl=node->UNODE.FUNCOP->ARGS; cl!=NULL; cl=cl->NEXT)
                eqt_freenode((eqt_node *)cl->DATA);
             eqt_delnode (node);
           }
    }
}

/*}}}************************************************************************/
/*{{{                    eqt_delnode()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_delnode(eqt_node *node)
{
  if (node)
    switch (node->TYPE)
    {
      case EQTVALUE_T :
           eqt_free('n',node);
           break;
      case EQTVARNAME_T :
           eqt_free('n',node);
           break;
      case EQTUNOP_T :
           eqt_free('u',node->UNODE.UNOP);
           eqt_free('n',node);
           break;
      case EQTBINOP_T :
           eqt_free('b',node->UNODE.BINOP);
           eqt_free('n',node);
           break;
      case EQTTERNOP_T :
           eqt_free('t',node->UNODE.TERNOP);
           eqt_free('n',node);
           break;
      case EQTDOTFUNC_T :
           freechain(node->UNODE.FUNCOP->ARGS);
           eqt_free('n',node);
           break;
    }
}

/*}}}************************************************************************/
/*{{{                    SubstN1ByN2()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void SubstN1ByN2(eqt_node *n1,eqt_node *n2, char mode)
{
  switch (n1->TYPE)
  {
    case EQTVALUE_T :
         break;
    case EQTVARNAME_T :
         break;
    case EQTUNOP_T :
         if (Oper(n1) != n2 && mode == 'Y')
           eqt_freenode(Oper(n1));
         eqt_free('u', n1->UNODE.UNOP);
         break;
    case EQTBINOP_T :
         if (Oper1(n1) != n2 && mode == 'Y')
           eqt_freenode(Oper1(n1));
         if (Oper2(n1) != n2 && mode == 'Y')
           eqt_freenode(Oper2(n1));
         eqt_free('b',n1->UNODE.BINOP);
         break;
    case EQTTERNOP_T :
         if (Oper1(n1) != n2 && mode == 'Y')
           eqt_freenode(Oper1(n1));
         if (Oper2(n1) != n2 && mode == 'Y')
           eqt_freenode(Oper2(n1));
         if (Oper3(n1) != n2 && mode == 'Y')
           eqt_freenode(Oper3(n1));
         eqt_free('t',n1->UNODE.TERNOP);
         break;
    case EQTDOTFUNC_T :
    default :
         avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//         eqt_error(__func__);
  }
  n1->TYPE  = n2->TYPE;
  switch (n2->TYPE)
  {
    case EQTVALUE_T :
         NewValue(n1,Value(n2));
         break;
    case EQTVARNAME_T :
         NewVarname(n1,Varname(n2));
         break;
    case EQTUNOP_T :
         n1->UNODE.UNOP = eqt_alloc('u');
         NewOper(n1,Oper(n2));
         break;
    case EQTBINOP_T :
         n1->UNODE.UNOP = eqt_alloc('b');
         NewOper1(n1,Oper1(n2));
         NewOper2(n1,Oper2(n2));
         break;
    case EQTTERNOP_T :
         n1->UNODE.UNOP = eqt_alloc('t');
         NewOper1(n1,Oper1(n2));
         NewOper2(n1,Oper2(n2));
         NewOper3(n1,Oper3(n2));
         break;
    case EQTDOTFUNC_T :
    default :
         avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//         eqt_error(__func__);
  }
  eqt_delnode(n2);
}

/*}}}************************************************************************/
/*{{{                    Basics allocations                                 */
/****************************************************************************/
/*{{{                    eqt_addvarname()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_addvarname (char *name)
{
  eqt_node  *new_node;

  new_node                  = (eqt_node*) eqt_alloc('n');

  new_node->TYPE            = EQTVARNAME_T;
  new_node->UNODE.VARNAME   = namealloc(name);
  new_node->FATHER          = NULL;

  return new_node;
}

/*}}}************************************************************************/
/*{{{                    eqt_addvalue()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_addvalue (double value)
{
  eqt_node  *new_node;

  new_node                  = (eqt_node*) eqt_alloc('n');

  new_node->TYPE            = EQTVALUE_T;
  new_node->UNODE.VALUE     = value;
  new_node->FATHER          = NULL;

  return new_node;
}

/*}}}************************************************************************/
/*{{{                    eqt_addunarynode()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_addunarynode (long operator, eqt_node *operand)
{
  eqt_unary     *unary_node;
  eqt_node      *new_node;

  unary_node                = (eqt_unary*) eqt_alloc('u');
  unary_node->OPERATOR      = operator;
  unary_node->OPERAND       = operand;

  new_node                  = (eqt_node*) eqt_alloc('n');
  new_node->TYPE            = EQTUNOP_T;
  new_node->UNODE.UNOP      = unary_node;

  new_node->FATHER          = NULL;
  eqt_set_father (operand,new_node);
  return new_node;
}

/*}}}************************************************************************/
/*{{{                    eqt_addbinarynode()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_addbinarynode (long operator, eqt_node *operand1,
                             eqt_node *operand2)
{
  eqt_binary    *binary_node;
  eqt_node      *new_node;

  binary_node               = (eqt_binary*) eqt_alloc('b');
  binary_node->OPERATOR     = operator;
  binary_node->OPERAND1     = operand1;
  binary_node->OPERAND2     = operand2;

  new_node                  = (eqt_node*) eqt_alloc('n');
  new_node->TYPE            = EQTBINOP_T;
  new_node->UNODE.BINOP     = binary_node;

  eqt_set_father (operand1,new_node);
  eqt_set_father (operand2,new_node);
  new_node->FATHER          = NULL;

  return new_node;
}

/*}}}************************************************************************/
/*{{{                    eqt_addternarynode()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_addternarynode (long operator, eqt_node *operand1,
                             eqt_node *operand2, eqt_node *operand3)
{
  eqt_ternary    *ternary_node;
  eqt_node      *new_node;

  ternary_node              = (eqt_ternary*) eqt_alloc('t');
  ternary_node->OPERATOR    = operator;
  ternary_node->OPERAND1    = operand1;
  ternary_node->OPERAND2    = operand2;
  ternary_node->OPERAND3    = operand3;

  new_node                  = (eqt_node*) eqt_alloc('n');
  new_node->TYPE            = EQTTERNOP_T;
  new_node->UNODE.TERNOP    = ternary_node;

  eqt_set_father (operand1,new_node);
  eqt_set_father (operand2,new_node);
  eqt_set_father (operand3,new_node);
  new_node->FATHER          = NULL;

  return new_node;
}

eqt_node *eqt_adddotfuncnode (char *name, chain_list *args)
{
  eqt_func   *ef;
  eqt_node   *new_node;
  chain_list *cl;

  ef              = (eqt_func*) eqt_alloc('t');
  ef->ARGS        = dupchainlst(args);
  ef->FUNCNAME    = namealloc(name);

  new_node                  = (eqt_node*) eqt_alloc('n');
  new_node->TYPE            = EQTDOTFUNC_T;
  new_node->UNODE.FUNCOP    = ef;

  for (cl=ef->ARGS; cl!=NULL; cl=cl->NEXT)
     eqt_set_father ((eqt_node *)cl->DATA,new_node);
  new_node->FATHER          = NULL;

  return new_node;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    eqt_dup()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_dup(eqt_node *n)
{
  eqt_node      *res;
  
  res->TYPE  = n->TYPE;
  switch (n->TYPE)
  {
    case EQTVALUE_T :
         res    = eqt_addvalue(Value(n));
         break;
    case EQTVARNAME_T :
         res    = eqt_addvarname(Varname(n));
         break;
    case EQTUNOP_T :
         res    = eqt_addunarynode(Operator(n),
                                   eqt_dup(Oper1(n)));
         break;
    case EQTBINOP_T :
         res    = eqt_addbinarynode(Operator(n),
                                    eqt_dup(Oper1(n)),
                                    eqt_dup(Oper2(n)));
         break;
    case EQTTERNOP_T :
         res    = eqt_addternarynode(Operator(n),
                                     eqt_dup(Oper1(n)),
                                     eqt_dup(Oper2(n)),
                                     eqt_dup(Oper3(n)));
         break;
    case EQTDOTFUNC_T :
         res = eqt_adddotfuncnode(n->UNODE.FUNCOP->FUNCNAME, n->UNODE.FUNCOP->ARGS);
         break;
    default :
         avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//         eqt_error(__func__);
  }
  
  return res;
}

/*}}}************************************************************************/

double eqt_calcval (eqt_ctx *ctx, eqt_node *node)
{
  double val;
  ctx->EQT_RES_CALC = EQT_COMPLETE;
  ctx->EQT_VAR_INVOLVED = 0;
  EQT_GOT_RANDOM=0;
  val=eqt_calcval_rec (ctx, node);
  ctx->EQT_GOT_RANDOM=EQT_GOT_RANDOM;
  return val;
}

eqt_func_entry *eqt_getdotfunc (eqt_ctx *ctx, char *name)
{
  chain_list *cl;
  name=namealloc(name);
  for (cl=ctx->FUNCTIONS; cl!=NULL && ((eqt_func_entry *)cl->DATA)->FUNCNAME!=name; cl=cl->NEXT) ;
  if (cl==NULL) return NULL;
  return (eqt_func_entry *)cl->DATA;
}

double eqt_execdotfunc(eqt_ctx *ctx, char *funcname, chain_list *args, int quick)
{
  eqt_func_entry *efe;
  struct
  {
    double oldval, newval;
    int set;
  } savevar[32];
  int i;
  chain_list *cl, *ch;
  long l;
  double value;
  
  if ((efe=eqt_getdotfunc(ctx, funcname))!=NULL)
   {
     if (countchain(args)!=countchain(efe->ARGS))
     {
        avt_errmsg(EQT_ERRMSG, "009", AVT_ERROR, efe->FUNCNAME);
        ctx->EQT_RES_CALC = EQT_INCOMPLETE;
        ctx->FAULTY_VAR = funcname;
        return 0;
     }
     for (cl=args, ch=efe->ARGS, i=0; cl!=NULL; cl=cl->NEXT, ch=ch->NEXT, i++)
      {
        if (!quick) 
          savevar[i].newval=eqt_calcval_rec(ctx, (eqt_node *)cl->DATA);
        else
          savevar[i].newval=*(double *)cl->DATA;
     
        if ((l=gethtitem (ctx->VAR_HT, ch->DATA))!=EMPTYHT)
          savevar[i].oldval=*(long *)l, savevar[i].set=1;
        else
          savevar[i].set=0;
      }
     for (ch=efe->ARGS, i=0; ch!=NULL; ch=ch->NEXT, i++)
      {
        eqt_addvar(ctx, (char *)ch->DATA, savevar[i].newval);
      }

     value=eqt_calcval_rec(ctx, efe->NODE_EXPR);

     for (ch=efe->ARGS, i=0; ch!=NULL; ch=ch->NEXT, i++)
      {
        if (savevar[i].set)
          eqt_addvar(ctx, (char *)ch->DATA, savevar[i].oldval);
        else
          delhtitem(ctx->VAR_HT, ch->DATA);
      }
     
     return value;
   }

  if (!quick) avt_errmsg(EQT_ERRMSG, "010", AVT_ERROR, funcname);
  ctx->EQT_RES_CALC = EQT_INCOMPLETE;
  ctx->FAULTY_VAR = funcname;
  return 0;
}

double eqt_execfunc123(eqt_ctx *ctx, int index, void *arg1, void *arg2, void *arg3, int quick)
{
  double res;
  if ((eqt_getdotfunc(ctx, ctx->NAMETAB[index]))!=NULL)
  {
    chain_list *cl=NULL;
    if (arg3!=NULL) cl=addchain(cl, arg3);
    if (arg2!=NULL) cl=addchain(cl, arg2);
    if (arg1!=NULL) cl=addchain(cl, arg1);
    res=eqt_execdotfunc(ctx, ctx->NAMETAB[index], cl, quick);
    freechain(cl);
  }
  else
  {
    if (arg3!=NULL)
      res= ctx->FUNCTAB3[index](*(double *)arg1, *(double *)arg2, *(double *)arg3) ;
    else if (arg2!=NULL)
      res= ctx->FUNCTAB2[index](*(double *)arg1, *(double *)arg2) ;
    else if (arg1!=NULL)
      res= ctx->FUNCTAB[index](*(double *)arg1) ;
    else
      avt_errmsg(EQT_ERRMSG, "008", AVT_FATAL, __func__);
  }
  return res;
}
/****************************************************************************/
/*{{{                    eqt_calcval_rec()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
double eqt_calcval_rec (eqt_ctx *ctx, eqt_node *node)
{
  double arg1, arg2, arg3;
  int op;
  if (!node)
    return 0.0;

  if (IsValue(node))
    return (Value(node));

  if (node->TYPE == EQTVARNAME_T)
    return (__eqt_getvar (ctx, node->UNODE.VARNAME, 0, 0));

  if (node->TYPE == EQTUNOP_T)
    switch (op=(Operator(node) & 0xff))
    {
      case EQTNOT :
           return  (double)(!(int)eqt_calcval_rec(ctx, Oper(node)) + 0.5);
      case EQTMIN :
           return  -eqt_calcval_rec(ctx, Oper(node));
      case EQTSQRT :
           return sqrt (eqt_calcval_rec(ctx, Oper(node)));
      case EQTEXP :
           return  exp (eqt_calcval_rec(ctx, Oper(node)));
      case EQTLOG :
           return  log (eqt_calcval_rec(ctx, Oper(node)));
      case EQTUSER :
           arg1=eqt_calcval_rec(ctx, Oper(node));
           return eqt_execfunc123(ctx, Operator(node) >> 8, &arg1, NULL, NULL, 1);
//           return  ctx->FUNCTAB[Operator(node) >> 8] (eqt_calcval_rec(ctx, Oper(node)));
      case EQTSIN :
           return  sin (eqt_calcval_rec(ctx, Oper(node)));
      case EQTCOS :
           return  cos (eqt_calcval_rec(ctx, Oper(node)));
      case EQTTAN :
           return  tan (eqt_calcval_rec(ctx, Oper(node)));
      case EQTASIN :
           return  asin (eqt_calcval_rec(ctx, Oper(node)));
      case EQTACOS :
           return  acos (eqt_calcval_rec(ctx, Oper(node)));
      case EQTATAN :
           return  atan (eqt_calcval_rec(ctx, Oper(node)));
      case EQTSINH :
           return  sinh (eqt_calcval_rec(ctx, Oper(node)));
      case EQTCOSH :
           return  cosh (eqt_calcval_rec(ctx, Oper(node)));
      case EQTTANH :
           return  tanh (eqt_calcval_rec(ctx, Oper(node)));
      case EQTLOG10:
           return  log10(eqt_calcval_rec(ctx, Oper(node)));
      case EQTCEIL :
           return  ceil (eqt_calcval_rec(ctx, Oper(node)));
      case EQTFLOOR:
           return  floor(eqt_calcval_rec(ctx, Oper(node)));
      case EQTFABS :
           return  fabs (eqt_calcval_rec(ctx, Oper(node)));
      case EQTABS :
           return  fabs (eqt_calcval_rec(ctx, Oper(node)));
      case EQTNOMDEV :
      case EQTNOMMOD :
      case EQTNOMSUB :
           if (ctx->SPECIAL_FUNC[(int)(op-EQTSPECFUNC_BASE)]!=NULL)
             return ctx->SPECIAL_FUNC[(int)(op-EQTSPECFUNC_BASE)](Varname(Oper(node)));
           avt_errmsg(EQT_ERRMSG, "010", AVT_FATAL, EQTSPECFUNC_NAME(op));
      case EQTINV :
           return  1.0 / (eqt_calcval_rec(ctx, Oper(node)));
    }

  if (node->TYPE == EQTBINOP_T)
    switch (Operator(node) & 0xff)
    {
      case EQTXOR :
           return (double)((int)(eqt_calcval_rec(ctx, Oper1(node)) + 0.5)
                   ^ (int)(eqt_calcval_rec(ctx, Oper2(node)) + 0.5));
      case EQTNOTEQ :
           return mbk_cmpdouble(eqt_calcval_rec(ctx, Oper1(node)), eqt_calcval_rec(ctx, Oper2(node)), EQT_PRECISION)!=0;
//           return eqt_calcval_rec(ctx, Oper1(node)) != eqt_calcval_rec(ctx, Oper2(node));
      case EQTEQ :
           return mbk_cmpdouble(eqt_calcval_rec(ctx, Oper1(node)), eqt_calcval_rec(ctx, Oper2(node)), EQT_PRECISION)==0;
//           return eqt_calcval_rec(ctx, Oper1(node)) == eqt_calcval_rec(ctx, Oper2(node));
      case EQTINFEQ :
           return mbk_cmpdouble(eqt_calcval_rec(ctx, Oper1(node)), eqt_calcval_rec(ctx, Oper2(node)), EQT_PRECISION)<=0;
//           return eqt_calcval_rec(ctx, Oper1(node)) <= eqt_calcval_rec(ctx, Oper2(node));
      case EQTSUPEQ :
           return mbk_cmpdouble(eqt_calcval_rec(ctx, Oper1(node)), eqt_calcval_rec(ctx, Oper2(node)), EQT_PRECISION)>=0;
//           return eqt_calcval_rec(ctx, Oper1(node)) >= eqt_calcval_rec(ctx, Oper2(node));
      case EQTINF :
           return mbk_cmpdouble(eqt_calcval_rec(ctx, Oper1(node)), eqt_calcval_rec(ctx, Oper2(node)), EQT_PRECISION)<0;
//           return eqt_calcval_rec(ctx, Oper1(node)) < eqt_calcval_rec(ctx, Oper2(node));
      case EQTSUP :
           return mbk_cmpdouble(eqt_calcval_rec(ctx, Oper1(node)), eqt_calcval_rec(ctx, Oper2(node)), EQT_PRECISION)>0;
//           return eqt_calcval_rec(ctx, Oper1(node)) > eqt_calcval_rec(ctx, Oper2(node));
      case EQTOR :
           return (double)((int)(eqt_calcval_rec(ctx, Oper1(node)) + 0.5)
                   || (int)(eqt_calcval_rec(ctx, Oper2(node)) + 0.5));
      case EQTAND :
           return (double)((int)(eqt_calcval_rec(ctx, Oper1(node)) + 0.5)
                   && (int)(eqt_calcval_rec(ctx, Oper2(node)) + 0.5));
      case EQTPLUS :
           return (eqt_calcval_rec(ctx, Oper1(node))
                   + eqt_calcval_rec(ctx, Oper2(node)));
      case EQTMIN :
           return (eqt_calcval_rec(ctx, Oper1(node))
                   - eqt_calcval_rec(ctx, Oper2(node)));
      case EQTMULT :
           return (eqt_calcval_rec(ctx, Oper1(node))
                   * eqt_calcval_rec(ctx, Oper2(node)));
      case EQTDIV :
           return (eqt_calcval_rec(ctx, Oper1(node))
                   / eqt_calcval_rec(ctx, Oper2(node)));
      case EQTPOW :
           return pow(eqt_calcval_rec(ctx, Oper1(node)),
                      eqt_calcval_rec(ctx, Oper2(node)));
      case EQTUSER :
           arg1=eqt_calcval_rec(ctx, Oper1(node));
           arg2=eqt_calcval_rec(ctx, Oper2(node));
           return eqt_execfunc123(ctx, Operator(node) >> 8, &arg1, &arg2, NULL, 1);
/*           return ctx->FUNCTAB2[Operator(node) >> 8]
             (eqt_calcval_rec(ctx, Oper1(node)),
              eqt_calcval_rec(ctx, Oper2(node)));*/
      case EQTATAN2 :
           return atan2(eqt_calcval_rec(ctx, Oper1(node)),
                        eqt_calcval_rec(ctx, Oper2(node)));
    }

  if (node->TYPE == EQTTERNOP_T)
    switch (node->UNODE.TERNOP->OPERATOR & 0xff)
    {
      case EQTIF :
           if (mbk_long_round(eqt_calcval_rec(ctx, node->UNODE.TERNOP->OPERAND1))!=0)
             return eqt_calcval_rec(ctx, node->UNODE.TERNOP->OPERAND2);
           else
             return eqt_calcval_rec(ctx, node->UNODE.TERNOP->OPERAND3);
      case EQTUSER :
           arg1=eqt_calcval_rec(ctx, node->UNODE.TERNOP->OPERAND1);
           arg2=eqt_calcval_rec(ctx, node->UNODE.TERNOP->OPERAND2);
           arg3=eqt_calcval_rec(ctx, node->UNODE.TERNOP->OPERAND3);
           return eqt_execfunc123(ctx, Operator(node) >> 8, &arg1, &arg2, &arg3, 1);
/*           return ctx->FUNCTAB3[node->UNODE.TERNOP->OPERATOR >> 8]
             (eqt_calcval_rec(ctx, node->UNODE.TERNOP->OPERAND1),
              eqt_calcval_rec(ctx, node->UNODE.TERNOP->OPERAND2),
              eqt_calcval_rec(ctx, node->UNODE.TERNOP->OPERAND3));*/
    }

  if (node->TYPE == EQTDOTFUNC_T)
  {
    return eqt_execdotfunc(ctx, node->UNODE.FUNCOP->FUNCNAME, node->UNODE.FUNCOP->ARGS, 0);
  }
  return 0.0;
}

/*}}}************************************************************************/
/*{{{                    eqt_reduce()                                       */
/*                                                                          */
/*                                                                          */
/* Warning node destroyed after effect                                      */
/****************************************************************************/
void eqt_reduce(eqt_ctx *ctx, eqt_node *node)
{
  eqt_node      *n, *n1, *n2;
  double         res, arg1, arg2;

  if (node)
    switch (node->TYPE)
    {
      case EQTVALUE_T :
           break;
      case EQTVARNAME_T :
           EvalVarname(ctx, node);
           break;
      case EQTUNOP_T :
           eqt_reduce (ctx, Oper(node));

           if (IsValue(n = Oper(node)))
           {
             switch (Operator(node) & 0xff)
             {
               case EQTMIN  : res =      -Value(n)  ;break;
               case EQTSQRT : res =  sqrt(Value(n)) ;break;
               case EQTEXP  : res =   exp(Value(n)) ;break;
               case EQTLOG  : res =   log(Value(n)) ;break;
               case EQTUSER :
                             arg1=Value(n);
                             res=eqt_execfunc123(ctx, Operator(node) >> 8, &arg1, NULL, NULL, 1);
                             break;
//                              res = ctx->FUNCTAB[Operator(node) >> 8](Value(n));break;
               case EQTSIN  : res =   sin(Value(n)) ;break;
               case EQTCOS  : res =   cos(Value(n)) ;break;
               case EQTTAN  : res =   tan(Value(n)) ;break;
               case EQTASIN : res =  asin(Value(n)) ;break;
               case EQTACOS : res =  acos(Value(n)) ;break;
               case EQTATAN : res =  atan(Value(n)) ;break;
               case EQTSINH : res =  sinh(Value(n)) ;break;
               case EQTCOSH : res =  cosh(Value(n)) ;break;
               case EQTTANH : res =  tanh(Value(n)) ;break;
               case EQTLOG10: res = log10(Value(n)) ;break;
               case EQTCEIL : res =  ceil(Value(n)) ;break;
               case EQTFLOOR: res = floor(Value(n)) ;break;
               case EQTFABS : res =  fabs(Value(n)) ;break;
               case EQTABS  : res =  fabs(Value(n)) ;break;
               case EQTNOMDEV :
               case EQTNOMMOD :
               case EQTNOMSUB :
                    avt_errmsg(EQT_ERRMSG, "008", AVT_FATAL, __func__);
             }
             NewValue(n,res);
           }
           break;
      case EQTBINOP_T :
           eqt_reduce (ctx, Oper1(node));
           eqt_reduce (ctx, Oper2(node));
           n1       = Oper1(node);
           n2       = Oper2(node);

           if ((IsValue(n1)) && (IsValue(n2)))
           {
             switch (Operator(node) & 0xff)
             {
               case EQTPLUS : res = Value(n1)+Value(n2)            ;break;
               case EQTMIN  : res = Value(n1)-Value(n2)            ;break;
               case EQTMULT : res = Value(n1)*Value(n2)            ;break;
               case EQTDIV  : res = Value(n1)/Value(n2)            ;break;
               case EQTPOW  : res = pow((Value(n1)),(Value(n2)))   ;break;
               case EQTUSER :
                             arg1=Value(n1);
                             arg2=Value(n2);
                             res=eqt_execfunc123(ctx, Operator(node) >> 8, &arg1, &arg2, NULL, 1);
                             break;
//                              res = ctx->FUNCTAB2[Operator(node) >> 8](Value(n1),Value(n2));break;
               case EQTATAN2: res = atan2((Value(n1)), (Value(n2)));break;
             }
             NewValue(n1,res);
             SubstN1ByN2(node,n1,'Y');
           }
           else
             switch (Operator(node))
             {
               case EQTMULT :
                    if (eqt_isOne(n1))
                      SubstN1ByN2(node,n2,'Y');
                    else if (eqt_isOne(n2))
                      SubstN1ByN2(node,n1,'Y');
                    else if (eqt_isZero(n1) || eqt_isZero(n2))
                      SubstN1ByN2(node,eqt_addvalue(0.0),'Y');
                    break;
               case EQTDIV :
                    if (eqt_isOne(n1))
                    {
                      n    = eqt_addunarynode(EQTINV,n2);
                      NewOper2(node,NULL);
                      SubstN1ByN2(node,n,'Y');
                    }
                    else if (eqt_isOne(n2))
                      SubstN1ByN2(node,n1,'Y');
                    break;
               case EQTPLUS :
                    if ( eqt_isRealLeaf(Oper1(node))
                         && Operator(n = Oper2(node)) == EQTPLUS
                         && eqt_isLeaf(Oper1(n))
                         && Operator(n = Oper2(n)) == EQTMIN
                         && IsVarname(Oper(n))
                         && strcmp(Varname(Oper(n)),Varname(Oper1(node))))
                    {
                      n     = Oper2(Oper2(node));
                      NewOper2(Oper2(node),NULL);
                      SubstN1ByN2(node,n2,'Y');
                    }
                    break;
             }
           break;
      case EQTTERNOP_T :
      case EQTDOTFUNC_T :
           avt_errmsg(EQT_ERRMSG, "004", AVT_ERROR, __func__);
//           eqt_error("%s : can't be done yet on ternary operators",__func__);
    }
}

/*}}}************************************************************************/
/*{{{                    EvalVarname()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void EvalVarname(eqt_ctx *ctx, eqt_node *node)
{
  long       value;
  
  if (node && IsVarname(node))
  {
    value   = gethtitem(ctx->VAR_HT, /*namealloc (*/Varname(node)/*)*/);
    if ((value != EMPTYHT) && (value != DELETEHT))
    {
      node->TYPE    = EQTVALUE_T;
      NewValue(node,*((double*)value));
    }
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_isZero()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int eqt_isZero(eqt_node *node)
{
  return (IsValue(node) && Value(node) == 0.0);
}

/*}}}************************************************************************/
/*{{{                    eqt_isOne()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int eqt_isOne(eqt_node *node)
{
  return (IsValue(node) && Value(node) == 1.0);
}

/*}}}************************************************************************/
/*{{{                    print_space()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_space(int space)
{
  int     i;

  for (i = 0; i < space; i++)
    fprintf(stdout,"¦ ");
}

/*}}}************************************************************************/
/*{{{                    eqt_initSpaceII()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_initSpaceII(char *EQT_PRINTYES)
{
  int    i;

  for (i = 0; i < 256; i ++ )
    EQT_PRINTYES[i] = 0;
}

/*}}}************************************************************************/
/*{{{                    print_spaceII()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_spaceII(char *EQT_PRINTYES, int space)
{
  int i;

  for (i = 0; i < space; i++)
    fprintf(stdout,"%s ", EQT_PRINTYES[i] ? " " : "¦");
}

/*}}}************************************************************************/
/*{{{                    eqt_print()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_print(eqt_ctx *ctx, eqt_node *node)
{
  int nb_space = 0, op;

  if (!node)
    avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//    eqt_error("%s : Noeud vide",__func__);
  else
    switch (node->TYPE)
    {
      case EQTVALUE_T :
           print_space(nb_space);
           fprintf (stdout," %5g\n", (Value(node))); nb_space++;
           break;
      case  EQTVARNAME_T :
           print_space(nb_space);
           fprintf (stdout," %5s\n", (node->UNODE.VARNAME));
           break;
      case  EQTUNOP_T :
           switch (op=(Operator(node) & 0xff))
           {
             case EQTNOT :
                  print_space(nb_space);
                  fprintf(stdout, "! \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTMIN :
                  print_space(nb_space);
                  fprintf(stdout, "- \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTSQRT:
                  print_space(nb_space);
                  fprintf(stdout, "SQRT \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTEXP :
                  print_space(nb_space);
                  fprintf(stdout, "EXP \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTLOG :
                  print_space(nb_space);
                  fprintf(stdout, "LOG \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTUSER :
                  print_space(nb_space);
                  fprintf(stdout, "%5s \n", ctx->NAMETAB[Operator(node) >> 8]);
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTSIN :
                  print_space(nb_space);
                  fprintf(stdout, "SIN \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTCOS :
                  print_space(nb_space);
                  fprintf(stdout, "COS \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTTAN :
                  print_space(nb_space);
                  fprintf(stdout, "TAN \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTASIN :
                  print_space(nb_space);
                  fprintf(stdout, "ASIN \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTACOS :
                  print_space(nb_space);
                  fprintf(stdout, "ACOS \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTATAN :
                  print_space(nb_space);
                  fprintf(stdout, "ATAN \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTSINH :
                  print_space(nb_space);
                  fprintf(stdout, "SINH \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTCOSH :
                  print_space(nb_space);
                  fprintf(stdout, "COSH \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTTANH :
                  print_space(nb_space);
                  fprintf(stdout, "TANH \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTLOG10 :
                  print_space(nb_space);
                  fprintf(stdout, "LOG10 \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTCEIL :
                  print_space(nb_space);
                  fprintf(stdout, "CEIL \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTFLOOR :
                  print_space(nb_space);
                  fprintf(stdout, "FLOOR \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTFABS :
                  print_space(nb_space);
                  fprintf(stdout, "FABS \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTABS :
                  print_space(nb_space);
                  fprintf(stdout, "ABS \n");
                  nb_space++;
                  eqt_print(ctx, Oper(node));
                  break;
             case EQTNOMDEV :
             case EQTNOMMOD :
             case EQTNOMSUB :
                  print_space(nb_space);
                  fprintf(stdout, "%s(%s) \n", EQTSPECFUNC_NAME(op), Varname(Oper(node)));

           }
           break;
      case  EQTBINOP_T :
           switch (Operator(node) & 0xff)
           {
             case EQTPLUS:
                  print_space(nb_space);
                  fprintf(stdout, "     + \n");
                  eqt_print(ctx, Oper1(node));
                  nb_space++;
                  eqt_print(ctx, Oper2(node));
                  break;
             case EQTMIN :
                  print_space(nb_space);
                  fprintf(stdout, "     - \n");
                  nb_space++;
                  eqt_print(ctx, Oper1(node));
                  eqt_print(ctx, Oper2(node));
                  break;
             case EQTMULT:
                  print_space(nb_space);
                  fprintf(stdout, "     * \n");
                  nb_space++ ; eqt_print(ctx, Oper1(node));
                  eqt_print(ctx, Oper2(node));
                  break;
             case EQTDIV :
                  print_space(nb_space);
                  fprintf(stdout, "     / \n");
                  nb_space++;
                  eqt_print(ctx, Oper1(node));
                  eqt_print(ctx, Oper2(node));
                  break;
             case EQTPOW :
                  print_space(nb_space);
                  fprintf(stdout, "     ^ \n");
                  nb_space++;
                  eqt_print(ctx, Oper1(node)),
                  eqt_print(ctx, Oper2(node));
                  break;
             case EQTUSER :
                  print_space(nb_space);
                  fprintf(stdout, "%5s \n", ctx->NAMETAB[Operator(node) >> 8]);
                  nb_space++;
                  eqt_print(ctx, Oper1(node)) ,
                  eqt_print(ctx, Oper2(node));
                  break;
             case EQTATAN2 :
                  print_space(nb_space);
                  fprintf(stdout, "     ATAN2 \n");
                  nb_space++;
                  eqt_print(ctx, Oper1(node));
                  eqt_print(ctx, Oper2(node));
                  break;
           }
      case  EQTTERNOP_T :
           switch (node->UNODE.TERNOP->OPERATOR & 0xff)
           {
             case EQTUSER :
                  print_space(nb_space);
                  fprintf(stdout, "%5s \n", ctx->NAMETAB[node->UNODE.TERNOP->OPERATOR >> 8]);
                  nb_space++;
                  eqt_print(ctx, node->UNODE.TERNOP->OPERAND1) ,
                  eqt_print(ctx, node->UNODE.TERNOP->OPERAND2) ,
                  eqt_print(ctx, node->UNODE.TERNOP->OPERAND3);
                  break;
           }
           break;
      case  EQTDOTFUNC_T :
           {
             chain_list *cl;
             print_space(nb_space);
             fprintf(stdout, "%5s \n",node->UNODE.FUNCOP->FUNCNAME);
             nb_space++;
             for (cl=node->UNODE.FUNCOP->ARGS; cl!=NULL; cl=cl->NEXT)
                eqt_print(ctx, (eqt_node *)cl->DATA);
             break;
           }

    }
}

/*}}}************************************************************************/
/****************************************************************************/

int eqt_parse (eqt_ctx *ctx)
{
    int res;
    
    EQT_CTX = ctx;
//    eqtrestart(NULL);
//    eqtdebug = 1;
    EQT_CTX->EQUA_NODE=NULL;
    res = eqtparse ();
    ctx = EQT_CTX;
    EQT_CTX = NULL;

    return res;
}

eqt_node *eqt_create (eqt_ctx *ctx, char *equa)
{
  ctx->EQT_MODE_CHOICE = EQTNORMAL;

  if (!equa) {
    avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//    eqt_error(__func__);
    return NULL;
  }

  ctx->EQUA_STR = equa;
  eqt_parse (ctx);

  return ctx->EQUA_NODE;
}

/****************************************************************************/
void eqt_resetvars(eqt_ctx *ctx, int nbvars)
{
  long        nextitem ;
  void       *nextkey ;
  if (ctx->VAR_HT) {
        scanhtkey( ctx->VAR_HT, 1, &nextkey, &nextitem );
        while( nextitem != EMPTYHT ) {
          eqt_free( 'd', (void *)nextitem );
          scanhtkey( ctx->VAR_HT, 0, &nextkey, &nextitem );
        }
        if (nbvars>0)
        {
          delht (ctx->VAR_HT);
          ctx->VAR_HT=addht(nbvars);
        }
        else
        {
          resetht(ctx->VAR_HT);
        }
        
    }
}

void eqt_term (eqt_ctx *ctx)
{
    long        nextitem ;
    void       *nextkey ;
    chain_list *cl;
    eqt_func_entry *efe;
    
    if ( !ctx ) return;
    if (ctx->VAR_HT) {
        scanhtkey( ctx->VAR_HT, 1, &nextkey, &nextitem );
        while( nextitem != EMPTYHT ) {
          eqt_free( 'd', (void *)nextitem );
          scanhtkey( ctx->VAR_HT, 0, &nextkey, &nextitem );
        }
        delht (ctx->VAR_HT);
    }
    if (ctx->UNIT_HT) {
        scanhtkey( ctx->UNIT_HT, 1, &nextkey, &nextitem );
        while( nextitem != EMPTYHT ) {
          eqt_free('d', (void *)nextitem );
          scanhtkey( ctx->UNIT_HT, 0, &nextkey, &nextitem );
        }
        delht (ctx->UNIT_HT);
    }
    for (cl=ctx->FUNCTIONS; cl!=NULL; cl=delchain(cl, cl)) {
      efe=(eqt_func_entry *)cl->DATA;
      eqt_freenode(efe->NODE_EXPR);
      freechain(efe->ARGS);
      eqt_free('f',efe);
    }

    eqt_free('c',ctx);
}

/****************************************************************************/

eqt_ctx *eqt_init (int nbvars)
{
    eqt_ctx *ctx;
    int i;
    
    ctx = (eqt_ctx*) eqt_alloc('c');
    ctx->VAR_HT    = addht (nbvars);
    ctx->UNIT_HT   = NULL; //addht (25);
    ctx->EQUA_NODE = NULL;
    ctx->EQTINDEX = 0;
    ctx->USE_SPICE_UNITS = 0;
    for (i = 0; i < EQTMAXNFUNC; i++) {
        ctx->NAMETAB[i] = NULL;
        ctx->FUNCTAB[i] = NULL;
        ctx->FUNCTAB2[i] = NULL;
        ctx->FUNCTAB3[i] = NULL;
    }
    ctx->FUNCTIONS=NULL;

    return ctx;
}

/****************************************************************************/

long eqt_dup_ctx_fn( long value, void *unused )
{
  double *pt ;
  
  pt = eqt_alloc('d');
  *pt = *((double*)value);

  unused=NULL;
  return (long)pt ;
}

eqt_ctx *eqt_dup_ctx (eqt_ctx *ctx)
{
    eqt_ctx *dupctx;
    int i;
    chain_list *cl;
    eqt_func_entry *efe;

    if (!ctx) return NULL;
    
    dupctx = (eqt_ctx*) eqt_alloc('c');

    dupctx->EQUA_NODE = NULL;

    for (i = 0; i < EQTMAXNFUNC; i++) {
        dupctx->NAMETAB[i] = NULL;
        dupctx->FUNCTAB[i] = NULL;
        dupctx->FUNCTAB2[i] = NULL;
        dupctx->FUNCTAB3[i] = NULL;
    }

    for (i = 0; i < ctx->EQTINDEX; i++) {
        dupctx->NAMETAB[i] = ctx->NAMETAB[i];
        dupctx->FUNCTAB[i] = ctx->FUNCTAB[i];
        dupctx->FUNCTAB2[i] = ctx->FUNCTAB2[i];
        dupctx->FUNCTAB3[i] = ctx->FUNCTAB3[i];
    }

    dupctx->EQTINDEX = ctx->EQTINDEX;
    dupctx->FUNCTIONS=NULL;
    for (cl=ctx->FUNCTIONS; cl!=NULL; cl=cl->NEXT) {
        dupctx->FUNCTIONS=addchain(dupctx->FUNCTIONS, efe=(eqt_func_entry *)eqt_alloc('f'));
        memcpy(efe, cl->DATA, sizeof(eqt_func_entry));
        efe->NODE_EXPR=eqt_create(dupctx, efe->EXPR);
        efe->ARGS=dupchainlst(efe->ARGS);
    }

    dupctx->VAR_HT  = controlled_dupht( ctx->VAR_HT,  eqt_dup_ctx_fn, NULL );
    if (ctx->UNIT_HT!=NULL)
      dupctx->UNIT_HT = controlled_dupht( ctx->UNIT_HT, eqt_dup_ctx_fn, NULL );
    else
      dupctx->UNIT_HT = NULL;

    dupctx->USE_SPICE_UNITS=ctx->USE_SPICE_UNITS;

    return dupctx;
}

/****************************************************************************/

extern void eqt_free_param (eqt_param *param)
{
  if ( param ) {
    mbkfree (param->EBI);
//    mbkfree (param->VAL);
    mbkfree (param);
  }
}

/****************************************************************************/
chain_list *eqt_export_func(chain_list *old, eqt_ctx *ctx)
{ 
  chain_list *cl, *ch, *pold=old;
  eqt_func_entry *efe;
  
  for (cl=ctx->FUNCTIONS; cl!=NULL; cl=cl->NEXT) {
    for (ch=pold; ch!=NULL && ((eqt_func_entry *)ch->DATA)->FUNCNAME!=((eqt_func_entry *)cl->DATA)->FUNCNAME; ch=ch->NEXT) ;
    if (ch==NULL)
    {
      old=addchain(old, efe=(eqt_func_entry *)eqt_alloc('f'));
      memcpy(efe, cl->DATA, sizeof(eqt_func_entry));
      efe->ARGS=dupchainlst(efe->ARGS);
    }
  }
  return old;
}

void eqt_import_func(chain_list *old, eqt_ctx *ctx)
{ 
  chain_list *cl, *ch;
  eqt_func_entry *efe;
  
  for (cl=old; cl!=NULL; cl=cl->NEXT) {
    for (ch=ctx->FUNCTIONS; ch!=NULL && ((eqt_func_entry *)ch->DATA)->FUNCNAME!=((eqt_func_entry *)cl->DATA)->FUNCNAME; ch=ch->NEXT) ;
    if (ch==NULL)
    {
      ctx->FUNCTIONS=addchain(ctx->FUNCTIONS, efe=(eqt_func_entry *)eqt_alloc('f'));
      memcpy(efe, cl->DATA, sizeof(eqt_func_entry));
      efe->NODE_EXPR=eqt_create(ctx, efe->EXPR);
      efe->ARGS=dupchainlst(efe->ARGS);
    }
  }
}

extern eqt_param *eqt_export_vars (eqt_ctx *ctx)
{
  int j = 0;
  eqt_param *param = NULL;
  chain_list *head, *chain ;
  void* nextkey ;
  long nextitem ;

  head = GetAllHTElems( ctx->VAR_HT );
  for( chain = head ; chain ; chain = chain->NEXT ) j++ ;
  freechain( head );
    
  if( j ) {
    param = (eqt_param*)mbkalloc (sizeof (struct eqt_param));
    param->EBI= mbkalloc ( j * sizeof (eqt_biinfo));
    param->NUMBER = j;
    
    j = 0;
    scanhtkey( ctx->VAR_HT, 1, &nextkey, &nextitem );
    while( nextitem != EMPTYHT ) {
      param->EBI[j].VAL = *(double*)(nextitem);
      param->EBI[j].NAME = nextkey;
      j++ ;
      scanhtkey( ctx->VAR_HT, 0, &nextkey, &nextitem );
    }
  }

  return param;
}
eqt_param *eqt_dupvars (eqt_param *sourceparam)
{
  eqt_param *param=NULL;

  if (sourceparam!=NULL && sourceparam->NUMBER!=0)
  {
    param = (eqt_param*)mbkalloc (sizeof (struct eqt_param));
    param->EBI= mbkalloc ( sourceparam->NUMBER * sizeof (eqt_biinfo));  
    param->NUMBER = sourceparam->NUMBER;
    memcpy(param->EBI, sourceparam->EBI, sourceparam->NUMBER * sizeof (eqt_biinfo));
  }

  return param;
}

/****************************************************************************/

void eqt_import_vars (eqt_ctx *to, eqt_param *from)
{
    int i;

    if (!from) return;
    for (i = 0; i < from->NUMBER; i++) eqt_addimportedvar (to, from->EBI[i].NAME, from->EBI[i].VAL);
}

void eqt_display_vars (eqt_param *params)
{
    int i;

    if (!params) return;
    for (i = 0; i < params->NUMBER; i++) printf("%s = %g\n", params->EBI[i].NAME, params->EBI[i].VAL);
}

/****************************************************************************/

void eqt_addunit (eqt_ctx *ctx, char *unit_name, double factor)
{
  char    buf[1024], *c;
  
  downstr (unit_name, buf);
  c=namealloc (buf);
  controlled_addhtitem(ctx->UNIT_HT, c, eqt_my_update, (void *)&factor);
}


/****************************************************************************/

double eqt_getunit (eqt_ctx *ctx, char *unit_name)
{
  long htfactor;
  char buf[1024];
  unsigned int i;

  if (!unit_name) return 1;
    
  htfactor=EMPTYHT;
  downstr (unit_name, buf);
  if (ctx->UNIT_HT)
    htfactor = gethtitem (ctx->UNIT_HT, namealloc (buf));
    
  if (ctx->USE_SPICE_UNITS && htfactor==EMPTYHT)
    {
      for (i=0; i<sizeof(basic_units)/sizeof(*basic_units); i++)
        if (strncmp(buf, basic_units[i].prefix, basic_units[i].size)==0)
          return basic_units[i].value;
    }

  if ((htfactor != EMPTYHT) && (htfactor != DELETEHT))
    return *(double*)htfactor;
  else {
    avt_errmsg(EQT_ERRMSG, "001", AVT_ERROR, unit_name);
    //fprintf (stderr, "[EQT WAR] Unknown unit '%s'\n", unit_name);
    return 1;
  }
}

/****************************************************************************/

void eqt_addvar (eqt_ctx *ctx, char *var_name, double value)
{
  char      *name;
  
  name=var_name;
  controlled_addhtitem(ctx->VAR_HT, name, eqt_my_update, (void *)&value);
}

/****************************************************************************/
static void eqt_addimportedvar (eqt_ctx *ctx, char *var_name, double value)
{
  controlled_addhtitem(ctx->VAR_HT, var_name, eqt_my_update, (void *)&value);
}

/****************************************************************************/
inline double __eqt_getvar (eqt_ctx *ctx, char *var_name, int usenamealloc, int ctxonly)
{
  long htvalue;
  int i;

  ctx->EQT_VAR_INVOLVED = 1;

  if (usenamealloc) var_name=namealloc(var_name);

  if (!ctxonly)
  {
    for (i=0; i<5 && EQT_CONTEXT_HIERARCHY[i]!=NULL; i++)
    {
      htvalue = gethtitem (EQT_CONTEXT_HIERARCHY[i]->VAR_HT, var_name);
      if ((htvalue != EMPTYHT) && (htvalue != DELETEHT))
        return(*(double*)htvalue);
    }
  }
  htvalue = gethtitem (ctx->VAR_HT, var_name);

  if ((htvalue != EMPTYHT) && (htvalue != DELETEHT))
    return(*(double*)htvalue);

  ctx->EQT_RES_CALC = EQT_INCOMPLETE;
  ctx->FAULTY_VAR = var_name;
  return 0.0;
}

int eqt_isdefined(eqt_ctx *ctx, char *varname, int global)
{
  int i;
  long htvalue;
  if (global)
  {
    for (i=0; i<5 && EQT_CONTEXT_HIERARCHY[i]!=NULL; i++)
    {
      htvalue = gethtitem (EQT_CONTEXT_HIERARCHY[i]->VAR_HT, varname);
      if ((htvalue != EMPTYHT) && (htvalue != DELETEHT))
        return 1;
    }
  }
  if (ctx==NULL) return 0;
  return gethtitem(ctx->VAR_HT, varname)!=EMPTYHT;
}

double eqt_getvar (eqt_ctx *ctx, char *var_name)
{
  return __eqt_getvar (ctx, var_name, 1, 0);
}
double eqt_getvar_in_context_only (eqt_ctx *ctx, char *var_name)
{
  return __eqt_getvar (ctx, var_name, 1, 1);
}

/****************************************************************************/

double eqt_eval (eqt_ctx *ctx, char *equa, int choix)
{
  double       d = 0.0;
  eqt_node    *n = NULL;

  ctx->EQT_MODE_CHOICE = choix;
  n = eqt_loopup_node(ctx, equa);
  if (n!=NULL)
    d = eqt_calcval (ctx, n);
  else
    avt_errmsg(EQT_ERRMSG, "002", AVT_ERROR, equa);

#if 0
  switch (ctx->EQT_MODE_CHOICE) {
    case EQTNORMAL :
//         n = eqt_create (ctx, equa);
         n = eqt_loopup_node(equa);
         if (n) {
           d = eqt_calcval (ctx, n);
//           eqt_freenode (n);
         }
         else
          avt_errmsg(EQT_ERRMSG, "002", AVT_ERROR, equa);
//           eqt_error("%s : can't evaluate '%s'",__func__,equa);
         break;
    case EQTFAST :
         ctx->EQUA_STR = equa;
         ctx->EQT_RES_CALC = EQT_COMPLETE;
         ctx->EQT_VAR_INVOLVED = 0;
         EQT_GOT_RANDOM=0;
         if (eqt_parse (ctx) == 0)
           d = ctx->EQUA_VAL;
         else
          avt_errmsg(EQT_ERRMSG, "002", AVT_ERROR, equa);
         ctx->EQT_GOT_RANDOM=EQT_GOT_RANDOM;
//           eqt_error("%s : can't evaluate '%s'",__func__,equa);
         break;

  }
#endif
  return d;
}

/****************************************************************************/

int eqt_resistrue (eqt_ctx *ctx)
{
  return ctx->EQT_RES_CALC == EQT_COMPLETE ? 1 : 0;
}

int eqt_resisrandom (eqt_ctx *ctx)
{
  return ctx->EQT_GOT_RANDOM!=0;
}
void eqt_set_failedres (eqt_ctx *ctx)
{
  ctx->EQT_RES_CALC=EQT_INCOMPLETE;
}

/****************************************************************************/

int eqt_var_involved (eqt_ctx *ctx)
{
  return ctx->EQT_VAR_INVOLVED;
}

/****************************************************************************/

void eqt_addfunction (eqt_ctx *ctx, char *name, double (*function) (double))
{
  if (ctx->EQTINDEX >= EQTMAXNFUNC)
  {
    avt_errmsg(EQT_ERRMSG, "003", AVT_WARNING);
//    eqt_warning("too much user defined functions");
    return;
  }
  ctx->NAMETAB[ctx->EQTINDEX] = namealloc (name);
  ctx->FUNCTAB[ctx->EQTINDEX] = function;
  ctx->EQTINDEX++;
}

/****************************************************************************/

int eqt_getindex (eqt_ctx *ctx, char *name)
{
  int        i;

  for (i = 0 ; i < EQTMAXNFUNC ; i++)
  {
    if (ctx->NAMETAB[i] == name)
      return i;
  }

  return -1;
}


/****************************************************************************/

void eqt_addfunction2 (eqt_ctx *ctx, char *name, double (*function) (double, double))
{
  if (ctx->EQTINDEX >= EQTMAXNFUNC)
  {
    avt_errmsg(EQT_ERRMSG, "003", AVT_WARNING);
//    eqt_warning("too much user defined functions");
    return;
  }
  ctx->NAMETAB [ctx->EQTINDEX] = namealloc (name);
  ctx->FUNCTAB2[ctx->EQTINDEX] = function;
  ctx->EQTINDEX++;
}

/****************************************************************************/

void eqt_addfunction3 (eqt_ctx *ctx, char *name, double (*function) (double, double, double))
{
  if (ctx->EQTINDEX >= EQTMAXNFUNC)
  {
    avt_errmsg(EQT_ERRMSG, "003", AVT_WARNING);
//    eqt_warning("too much user defined functions");
    return;
  }
  ctx->NAMETAB [ctx->EQTINDEX] = namealloc (name);
  ctx->FUNCTAB3[ctx->EQTINDEX] = function;
  ctx->EQTINDEX++;
}

int eqt_adddotfunc (eqt_ctx *ctx, char *name, chain_list *args, char *expr)
{
  eqt_func_entry *efe;
  chain_list *cl;

  name=namealloc(name);
  for (cl=ctx->FUNCTIONS; cl!=NULL && ((eqt_func_entry *)cl->DATA)->FUNCNAME!=name; cl=cl->NEXT) ;
  if (cl!=NULL)
  {
    efe=(eqt_func_entry *)cl->DATA;
    eqt_freenode(efe->NODE_EXPR);
    freechain(efe->ARGS);
  }
  else
  {
    efe=(eqt_func_entry *)eqt_alloc('f');
    ctx->FUNCTIONS=addchain(ctx->FUNCTIONS, efe);
  }
  efe->FUNCNAME=name;
  efe->ARGS=dupchainlst(args);
  for (cl=efe->ARGS; cl!=NULL; cl=cl->NEXT) cl->DATA=namealloc((char *)cl->DATA);
  efe->EXPR=namealloc(expr); //?
  efe->NODE_EXPR=eqt_create(ctx, efe->EXPR);  
  return 1;
}

/****************************************************************************/
chain_list *eqt_NodeToAbl(eqt_node *node)
{
  chain_list *abl;
  int value;

  if (!node)
  {
    avt_errmsg(EQT_ERRMSG, "005", AVT_ERROR);
//    eqt_error("empty node can't make ABL");
    return NULL;
  }

  if (IsValue(node)) {
    value = (int)(Value(node));
    if ( value == 0 )
      abl = createAtom ( "'0'" );
    else if ( value == 1 )
      abl = createAtom ( "'1'" );
    else {
      avt_errmsg(EQT_ERRMSG, "006", AVT_ERROR);
//      eqt_error("eqt error: integer value in ABL to make");
      abl = NULL;
    }
    return abl;
  }

  if (node->TYPE == EQTVARNAME_T) {
    abl = createAtom ( node->UNODE.VARNAME );
    return abl;
  }

  if (node->TYPE == EQTUNOP_T)
    switch (Operator(node) & 0xff) {
      case EQTNOT :
           abl = notExpr ( eqt_NodeToAbl (Oper(node) ) );
           return abl;
           break;
    }

  if (node->TYPE == EQTBINOP_T)
    switch (Operator(node) & 0xff) {
      case EQTOR:
      case EQTPLUS:
           abl = createBinExpr ( OR ,
                                 eqt_NodeToAbl (Oper1(node)) ,
                                 eqt_NodeToAbl (Oper2(node)) );
           return abl;
           break;
      case EQTAND:
      case EQTMULT: 
           abl = createBinExpr ( AND,
                                 eqt_NodeToAbl ( Oper1(node) ),
                                 eqt_NodeToAbl ( Oper2(node) ));
           return abl;
           break;
      case EQTXOR:
      case EQTPOW:
           abl = createBinExpr ( XOR,
                                 eqt_NodeToAbl ( Oper1(node) ),
                                 eqt_NodeToAbl ( Oper2(node) ));
           return abl;
           break;
    }
  if (node->TYPE == EQTTERNOP_T || node->TYPE==EQTDOTFUNC_T)
    avt_errmsg(EQT_ERRMSG, "004", AVT_WARNING, __func__);
    //  eqt_warning("%s can't be done yet on ternary operators",__func__);

  return NULL;
}

/****************************************************************************/
chain_list *eqt_StrToAbl(eqt_ctx *ctx, char *str)
{
  eqt_node *node;
  chain_list *abl;

  node = eqt_create (ctx, str);
  if ( !node ) {
    avt_errmsg(EQT_ERRMSG, "007", AVT_ERROR);
//    eqt_error("node NULL");
    return NULL;
  }
  else {
    abl = eqt_NodeToAbl ( node );
    eqt_freenode( node );
    return abl;
  }
}

/****************************************************************************/
/* FUNCTION : eqt_ReduceSpace                                               */
/* Object   : reduce all consecutive white space in a string                */
/****************************************************************************/
char *eqt_ReduceSpace(char *str)
{
  char   initbuf[1024];
  char   finalbuf[1024];
  char   flag = 'N' ; /* space flag */
  int    length;
  int    i, j;

  sprintf(initbuf,"%s",str);
  length = strlen(str);
  j = 0;
  for (i = 0; i < length ; i++)
  {
    if ((initbuf[i]) != ' ')
    {
      finalbuf[j] = initbuf[i];
      flag = 'N';
      j++;
    }
    else if ((initbuf[i]) == ' ' && flag == 'N')
    {
      finalbuf[j] = initbuf[i];
      flag = 'Y';
      j++;
    }
  }
  finalbuf[j] = '\0';

  return mbkstrdup(finalbuf);
}

/****************************************************************************/
/* FUNCTION : eqt_IsSpecialCar                                              */
/* Object   : Test if the car is special                                    */
/****************************************************************************/
int  eqt_IsSpecialCar(char str, char pos)
{
  /* pos : D = Droite de l espace */
  /* pos : G = Gauche de l espace */

  int res = 0;

  if ((str == '(') && (pos == 'D'))
    res = 1;
  else if ((str == ')') && (pos == 'G'))
    res = 2;
  else if ((str == '!') && (pos == 'D'))
    res = 3;
  else if ((str == '~') && (pos == 'D'))
    res = 3;
  else if ((str == '\'') && (pos == 'G'))
    res = 3;
  else if (isalnum((int)str) != 0) {
    res = 4;
  }

  return res;
}

/****************************************************************************/
/* FUNCTION : eqt_ConvertStr                                                */
/* Object   : Replace the good white space by a '&' car                     */
/****************************************************************************/
char *eqt_ConvertStr(char *str)
{
  char *string, *sreduce;
  char *pt, *ptres;
  int   res1 = 0;
  int   res2 = 0;

  sreduce = eqt_ReduceSpace ( str );
  pt = sreduce;
  string = (char*)mbkalloc ((2*strlen(pt))+1);
  ptres = string;
  while ( *pt != '\0' ) {
      switch (*pt) {
        case ' '    :   res1 = eqt_IsSpecialCar(*(pt-1),'G');
                        res2 = eqt_IsSpecialCar(*(pt+1),'D');
                        if ((res1 && res2)) 
                            *ptres = '&';
                        else *ptres = *pt; 
                        break;
        case '!'    :   if (pt>sreduce) {
                            res1 = eqt_IsSpecialCar(*(pt-1),'G');
                            res2 = eqt_IsSpecialCar(*(pt+1),'D');
                        }
                        if (((res1==4)||(res1==2)) && ((res2==4)||(res2==1))) {
                            *ptres = '&'; ptres++;
                            *ptres = *pt;
                        }
                        else *ptres = *pt; 
                        break;
        case '\''    :  if (*(pt+1) != '\0') {
                            res1 = eqt_IsSpecialCar(*(pt-1),'G');
                            res2 = eqt_IsSpecialCar(*(pt+1),'D');
                        }
                        if (((res1==4)||(res1==2)) && ((res2==4)||(res2==1))) {
                            *ptres = *pt; ptres++;
                            *ptres = '&';
                        }
                        else *ptres = *pt; 
                        break;
        default     :   *ptres = *pt;                
                        break;
      }
      pt++;
      ptres++;
  }
  *ptres = '\0';
  mbkfree (sreduce);
  return string;
}


/*{{{                    eqt_get_brother()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_get_brother(eqt_node *node)
{
  eqt_node *father = node->FATHER;

  if (father->TYPE == EQTBINOP_T)
  {
    if (Oper2(father) == node)
      return Oper1(father);
    else
      return Oper2(father);
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    eqt_adopt_brother()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_adopt_brother(eqt_node *node,eqt_node *brother)
{
  eqt_node *father = node->FATHER;

  if (father->TYPE == EQTBINOP_T)
  {
    if (Oper2(father) == node)
      NewOper1(father,brother);
    else
      NewOper2(father,brother);
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_DelMinDiv()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_DelMinDiv(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTVALUE_T : case EQTVARNAME_T :
         return node;
    case EQTUNOP_T :
         if (IsValue(Oper(node)) && Operator(node) == EQTMIN)
         {
           eqt_node *res;
           
           res  = eqt_NegNode(Oper(node));
           eqt_delnode(node);
           node = res;
         }
         else
           NewOper(node,eqt_DelMinDiv(Oper(node)));
         return node;
    default :
         if (Operator(node) == EQTMIN)
         {
           NewOperator(node,EQTPLUS);
           NewOper2(node,eqt_NegNode(Oper2(node)));
         }
         else if (Operator(node) == EQTDIV)
         {
           NewOperator(node,EQTMULT);
           NewOper2(node,eqt_InvNode(Oper2(node)));
         }
         NewOper1(node,eqt_DelMinDiv(Oper1(node)));
         NewOper2(node,eqt_DelMinDiv(Oper2(node)));
         return node;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_NegNode()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_NegNode(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTVALUE_T :
         node->UNODE.VALUE = -(Value(node));
         return node;
    case EQTVARNAME_T :
         return eqt_addunarynode(EQTMIN,node);
    case EQTUNOP_T :
         if (Operator(node) == EQTMIN)
         {
           eqt_node     *res;

           res      = Oper(node);
           eqt_delnode(node);
           return res;
         }
         else
         {
           NewOper(node,eqt_NegNode(Oper(node)));
           return node;
         }
    default :
         switch (Operator(node))
         {
           case EQTMIN :
                NewOperator(node,EQTPLUS);
                NewOper1(node,eqt_NegNode(Oper1(node)));
                break;
           case EQTPLUS :
                NewOper2(node,eqt_NegNode(Oper2(node)));
                NewOper1(node,eqt_NegNode(Oper1(node)));
                break;
           case EQTMULT :
                if (IsValue(Oper1(node)))
                  NewOper1(node,eqt_NegNode(Oper1(node)));
                else
                  NewOper2(node,eqt_NegNode(Oper2(node))); 
                break;
           default :
                NewOper1(node,eqt_NegNode(Oper1(node)));
         }
         return node;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_InvNode()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_InvNode(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTVALUE_T :
         if (Value(node) == 1.0)
           return node;
    case EQTVARNAME_T :
         return eqt_addunarynode(EQTINV,node);
    case EQTUNOP_T :
         if (Operator(node) == EQTINV)
         {
           eqt_node     *res;

           res      = Oper(node);
           eqt_delnode(node);
           return res;
         }
         else
         {
           NewOper(node,eqt_InvNode(Oper(node)));
           return node;
         }
    default :
         if (Operator(node) == EQTDIV)
         {
           NewOper1(node,eqt_InvNode(Oper1(node)));
           NewOperator(node,EQTMULT);
         }
         else if (Operator(node) == EQTMULT)
         {
           NewOper1(node,eqt_InvNode(Oper1(node)));
           NewOper2(node,eqt_InvNode(Oper2(node)));
         }
         else
           node = eqt_addunarynode(EQTINV,node);
         return node;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_DelNeutral()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_DelNeutral(eqt_node *node)
{
  double     neutral;
  eqt_node  *res, *father;

  father    = node->FATHER;
  switch (node->TYPE)
  {
    case EQTVALUE_T : case EQTVARNAME_T :
         return node;
    case EQTUNOP_T :
         if (Operator(node) == EQTMIN)
           neutral  = 0.0;
         else if (Operator(node) == EQTINV)
           neutral  = 1.0;
         else
           neutral  = -1.0;
         if ( neutral >= 0.0
              && IsValue(Oper(node)) && Value(Oper(node)) == neutral)
         {
           res          = Oper(node);
           res->FATHER  = father;
           eqt_delnode(node);
           
           return eqt_DelNeutral(res);
         }
         NewOper(node,eqt_DelNeutral(Oper(node)));
         
         return  node;
    default :
         if (Operator(node) == EQTPLUS)
           neutral  = 0.0;
         else if (Operator(node) == EQTMULT)
           neutral  = 1.0;
         else
           neutral  = -1.0;
         if (neutral >= 0.0)
         {
           if ( IsValue(Oper1(node)) && Value(Oper1(node)) == neutral)
           {
             res            = Oper2(node);
             res->FATHER    = father;
             eqt_delnode(Oper1(node));
             eqt_delnode(node);

             return eqt_DelNeutral(res);
             //             NewOper2(node,NULL);
             //             SubstN1ByN2(node,eqt_DelNeutral(res),'Y');
             //             
             //             return node;
           }
           // negative form of neutral element
           else if ( IsValue(Oper1(node)) && Value(Oper1(node)) == -neutral)
           {
             res            = eqt_NegNode(Oper2(node));
             res->FATHER    = father;
             eqt_delnode(Oper1(node));
             eqt_delnode(node);

             return eqt_DelNeutral(res);
//             NewOper2(node,NULL);
//             SubstN1ByN2(node,eqt_DelNeutral(res),'Y');
//
//             return node;
           }
           else if ( IsValue(Oper2(node)) && Value(Oper2(node)) == neutral)
           {
             res            = Oper1(node);
             res->FATHER    = father;
             eqt_delnode(Oper2(node));
             eqt_delnode(node);

             return eqt_DelNeutral(res);
//             NewOper1(node,NULL);
//             SubstN1ByN2(node,eqt_DelNeutral(res),'Y');
//
//             return node;
           }
           // negative form of neutral element
           else if ( IsValue(Oper2(node)) && Value(Oper2(node)) == -neutral)
           {
             res            = eqt_NegNode(Oper1(node));
             res->FATHER    = father;
             eqt_delnode(Oper2(node));
             eqt_delnode(node);

             return eqt_DelNeutral(res);
//             NewOper1(node,NULL);
//             SubstN1ByN2(node,eqt_DelNeutral(res),'Y');
//
//             return node;
           }
         }
         NewOper1(node,eqt_DelNeutral(Oper1(node)));
         NewOper2(node,eqt_DelNeutral(Oper2(node)));
         
         return node;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_ToStr()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *eqt_ToStr(eqt_node *node)
{
  char       buf[4096];
  char      *operand1, *operand2, *operator;
  int        par;

  par = eqt_needParenthesis(node);
  switch (node->TYPE)
  {
    case EQTVALUE_T :
         if (par)
           sprintf(buf,"(%g)",Value(node));
         else
           sprintf(buf,"%g",Value(node));
         return mbkstrdup(buf);
    case EQTVARNAME_T :
         sprintf(buf,"%s",node->UNODE.VARNAME);
         return mbkstrdup(buf);
    case EQTUNOP_T :
         operator = eqt_operToStr(Operator(node));
         operand1 = eqt_ToStr(Oper(node));
         switch (par)
         {
           case 1 :
                sprintf(buf,"%s(%s)",operator,operand1);
                break;
           case 2 :
                sprintf(buf,"(%s%s)",operator,operand1);
                break;
           default :
                sprintf(buf,"%s%s",operator,operand1);
                break;
         }
         mbkfree(operator);
         mbkfree(operand1);
         return mbkstrdup(buf);
    default :
         if (Operator(node) == EQTPLUS && IsNeg(Oper2(node)))
           operator = mbkstrdup("");
         else
           operator = eqt_operToStr(Operator(node));
         operand2   = eqt_ToStr(Oper2(node));
         operand1   = eqt_ToStr(Oper1(node));
         if (par)
           sprintf(buf,"(%s%s%s)",operand1,operator,operand2);
         else
           sprintf(buf,"%s%s%s",operand1,operator,operand2);
         mbkfree(operator);
         mbkfree(operand1);
         mbkfree(operand2);
         return mbkstrdup(buf);
  }
}


static chain_list *__eqt_GetVariables(eqt_ctx *ctx, eqt_node *node, chain_list *list, int all)
{
  chain_list *cl;

  switch (node->TYPE)
  {
    case EQTVALUE_T :
      return list;
    case EQTVARNAME_T :
      for (cl=list; cl!=NULL && cl->DATA!=node->UNODE.VARNAME; cl=cl->NEXT) ;
      if (cl==NULL)
        {
          if (all || !eqt_isdefined(ctx, node->UNODE.VARNAME, 1))
            list=addchain(list, node->UNODE.VARNAME);
        }
      return list;
    case EQTUNOP_T :
      list = __eqt_GetVariables(ctx, Oper(node), list, all);
      return list;
    case EQTDOTFUNC_T:
      {
        if (eqt_getdotfunc(ctx, node->UNODE.FUNCOP->FUNCNAME)==NULL)
        {
          for (cl=list; cl!=NULL && cl->DATA!=node->UNODE.FUNCOP->FUNCNAME; cl=cl->NEXT) ;
          if (cl==NULL) list=addchain(list, node->UNODE.FUNCOP->FUNCNAME);
        }
        for (cl=node->UNODE.FUNCOP->ARGS; cl!=NULL; cl=cl->NEXT)
          list=__eqt_GetVariables(ctx, (eqt_node *)cl->DATA, list, all);
        return list;
      }
    case EQTTERNOP_T :
      list = __eqt_GetVariables(ctx, Oper3(node), list, all);
    case EQTBINOP_T :
      list = __eqt_GetVariables(ctx, Oper2(node), list, all);
      list = __eqt_GetVariables(ctx, Oper1(node), list, all);
    default :
      return list;
  }
}

chain_list *eqt_GetVariables(eqt_ctx *ctx, char *equa, int all)
{
  eqt_node    *n;
  chain_list *cl=NULL;

  ctx->EQT_MODE_CHOICE = EQTNORMAL;
  //n = eqt_create (ctx, equa);
  n = eqt_loopup_node(ctx, equa);
  if (n!=NULL) cl=__eqt_GetVariables(ctx, n, NULL, all);
//  eqt_freenode (n);
  return cl;
}

/*}}}************************************************************************/
/*{{{                    eqt_ToIntStr()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *eqt_ToIntStr(eqt_node *node)
{
  char       buf[4096];
  char      *operand1, *operand2, *operator;
  int        par;

  par = eqt_needParenthesis(node);
  switch (node->TYPE)
  {
    case EQTVALUE_T :
         if (par)
           sprintf(buf,"(%ld)",(long)Value(node));
         else
           sprintf(buf,"%ld",(long)Value(node));
         return mbkstrdup(buf);
    case EQTVARNAME_T :
         sprintf(buf,"%s",node->UNODE.VARNAME);
         return mbkstrdup(buf);
    case EQTUNOP_T :
         operator = eqt_operToStr(Operator(node));
         operand1 = eqt_ToIntStr(Oper(node));
         switch (par)
         {
           case 1 :
                sprintf(buf,"%s(%s)",operator,operand1);
                break;
           case 2 :
                sprintf(buf,"(%s%s)",operator,operand1);
                break;
           default :
                sprintf(buf,"%s%s",operator,operand1);
                break;
         }
         mbkfree(operator);
         mbkfree(operand1);
         return mbkstrdup(buf);
    default :
         if (Operator(node) == EQTPLUS && IsNeg(Oper2(node)))
           operator = mbkstrdup("");
         else
           operator = eqt_operToStr(Operator(node));
         operand2   = eqt_ToIntStr(Oper2(node));
         operand1   = eqt_ToIntStr(Oper1(node));
         if (par)
           sprintf(buf,"(%s%s%s)",operand1,operator,operand2);
         else
           sprintf(buf,"%s%s%s",operand1,operator,operand2);
         mbkfree(operator);
         mbkfree(operand1);
         mbkfree(operand2);
         return mbkstrdup(buf);
  }
}

/*}}}************************************************************************/
/*{{{                    IsNeg()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int IsNeg(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTVALUE_T :
         return (Value(node) < 0.0);
    case EQTUNOP_T :
         return (Operator(node) == EQTMIN);
    default :
         return 0;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_needParenthesis()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int eqt_needParenthesis(eqt_node *node)
{
  eqt_node  *father;

  father    = node->FATHER;
  switch (node->TYPE)
  {
    case EQTUNOP_T :
         switch (Operator(node))
         {
           case EQTMIN :
                if (Operator(father) == EQTPLUS)
                  return 0;
                else
                  return 2;
           default :
                return 0;
         }
    case EQTBINOP_T :
         switch (Operator(node))
         {
           case EQTMULT : case EQTDIV :
                return 0;
           default :;
         }
         if (father)
           switch (Operator(father))
           {
             case EQTMIN :
                  return eqt_isOperand2(node);
             case EQTDIV :
                  return 1;
             default :
                  return (Operator(father) != Operator(node));
           }
         else
           return 0;
    case EQTVALUE_T :
         if (father)
           if (Operator(father) == EQTPLUS)
             return 0;
           else
             return (Value(node) < 0.0);
         else
           return 0;
    default :
         return 0;
  }
  return (node == NULL);
}

/*}}}************************************************************************/
/*{{{                    eqt_drive()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_drive(eqt_node *node)
{
  char      *str;

  str = eqt_ToStr(node);
  fprintf(stdout,"%s\n",str);
  mbkfree(str);
}

/*}}}************************************************************************/
/*{{{                    eqt_operToStr()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *eqt_operToStr(long operator)
{
  switch (operator)
  {
    case EQTPLUS  : return mbkstrdup("+");
    case EQTMIN   : return mbkstrdup("-");
    case EQTMULT  : return mbkstrdup("*");
    case EQTDIV   : return mbkstrdup("/");
    case EQTEXP   : return mbkstrdup("e");
    case EQTSQRT  : return mbkstrdup("sqrt");
    case EQTPOW   : return mbkstrdup("^");
    case EQTLOG   : return mbkstrdup("log");
    case EQTINV   : return mbkstrdup("inv");

    case EQTSIN   : return mbkstrdup("sin");
    case EQTCOS   : return mbkstrdup("cos");
    case EQTTAN   : return mbkstrdup("tan");
    case EQTASIN  : return mbkstrdup("");
    case EQTACOS  : return mbkstrdup("");
    case EQTATAN  : return mbkstrdup("");
    case EQTATAN2 : return mbkstrdup("");
    case EQTSINH  : return mbkstrdup("");
    case EQTCOSH  : return mbkstrdup("");
    case EQTTANH  : return mbkstrdup("");
    case EQTLOG10 : return mbkstrdup("log10");
    case EQTCEIL  : return mbkstrdup("");
    case EQTFLOOR : return mbkstrdup("");
    case EQTFABS  : return mbkstrdup("fabs");
    case EQTABS   : return mbkstrdup("abs");
    default : return mbkstrdup("");
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_linearise()                                    */
/*                                                                          */
/* return invert child                                                      */
/****************************************************************************/
int eqt_linearise(eqt_node *node)
{
  eqt_node      *father;

  switch (node->TYPE)
  {
    case EQTVALUE_T : case EQTVARNAME_T : case EQTUNOP_T :
         return 0;
    case EQTBINOP_T :
         eqt_linearise(Oper1(node));
         if ( (father = node->FATHER) )
         {
           if ( Operator(father) == Operator(node) )
           {
             if (! eqt_isOperand2(node) )
             {                 
               NewOper1(father, Oper1(node));
               NewOper1(node, Oper2(father));
               eqt_invertChild(node);
               NewOper2(father,node);
               eqt_linearise(Oper1(node));
               return 0;
             }
           }
           else if ( eqt_isOperand2(node) )
           {
             eqt_invertChild(father);
             return 1;
           }
           else
             return 0;
         }
         eqt_linearise(Oper2(node));
         return 0;
    default :
         avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//         eqt_error(__func__);
         return 0;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_addNeutral()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int eqt_addNeutral(eqt_node *node)
{
  eqt_node  *nodex, *father, *neutral;
  int        operator;
  
  switch (node->TYPE)
  {
    case EQTVALUE_T : case EQTVARNAME_T : case EQTUNOP_T :
         return 1;
    case EQTBINOP_T :
         operator   = Operator(node);
         for (nodex = node; Operator(nodex) == operator;
              nodex = Oper2(nodex))
           eqt_addNeutral(Oper1(nodex));
         if (!IsValue(nodex))
         {
           neutral    = eqt_getNeutralNode(operator);
           father     = nodex->FATHER;
           if (father)
             NewOper2(father,eqt_addbinarynode(operator,nodex,neutral));
         }
         return 1;
    default :
         return 0;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_getNeutralNode()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static eqt_node *eqt_getNeutralNode(int operator)
{
  return eqt_addvalue(eqt_get_neutral(operator));
}

/*}}}************************************************************************/
/*{{{                    eqt_printTree()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_printTree(eqt_node *node,int deep)
{
  char   *operator;
  char EQT_PRINTYES[255];

  if (deep == 0)
    eqt_initSpaceII(EQT_PRINTYES);
  switch (node->TYPE)
  {
    case EQTVALUE_T :
         fprintf(stdout," %g\n",Value(node));
         break;
    case EQTVARNAME_T :
         fprintf(stdout," %s\n",Varname(node));
         break;
    case EQTUNOP_T :
         operator = eqt_operToStr(Operator(node));
         fprintf(stdout,"(%s)\n",operator);
         mbkfree(operator);
         print_spaceII(EQT_PRINTYES, deep);
         fprintf(stdout,"`---:");
         EQT_PRINTYES[deep] = 1;
         eqt_printTree(Oper(node),deep+1);
         EQT_PRINTYES[deep] = 0;
         break;
    default :
         operator = eqt_operToStr(Operator(node));
         fprintf(stdout,"(%s)\n",operator);
         mbkfree(operator);
         print_spaceII(EQT_PRINTYES, deep);
         if (eqt_isRealLeaf(Oper1(node)))
           fprintf(stdout,"+--1:");
         else
           fprintf(stdout,"+-+1:");
         eqt_printTree(Oper1(node),deep+1);
         print_spaceII(EQT_PRINTYES, deep);
         EQT_PRINTYES[deep] = 1;
         if (eqt_isRealLeaf(Oper2(node)))
           fprintf(stdout,"`--2:");
         else
           fprintf(stdout,"`-+2:");
         eqt_printTree(Oper2(node),deep+1);
         EQT_PRINTYES[deep] = 0;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_sortBranch()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_sortBranch(eqt_node *node)
{
  eqt_node  **table;
  int        size;

  switch (node->TYPE)
  {
    case EQTBINOP_T :
         table      = eqt_sortDoTable(node,0,&size);
         qsort(table,size,sizeof(eqt_node *),eqt_sortCompareNode);
         eqt_sortGetSortedNode(node,table);
    case EQTVARNAME_T : case EQTVALUE_T : case EQTUNOP_T :
         break;
    default :
         avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//         eqt_error(__func__);
         EXIT(0);
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_sortGetSortedNode()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_sortGetSortedNode(eqt_node *node, eqt_node **table)
{
  eqt_node   *nodex;
  int         i;

  for (nodex = node, i = 0; nodex; nodex = Oper2(nodex), i ++)
  {
    if (!eqt_isLeaf(Oper2(nodex)))
      NewOper1(nodex,table[i]);
    else
    {
      long       operator;
      double     neutral;

      NewOper1(nodex,table[i]);
      operator  = Operator(nodex);
      neutral   = eqt_get_neutral(operator);
      if (neutral >= 0.0 && !IsValue(table[i+1]))
      {
        eqt_node    *tmp;

        tmp = eqt_addbinarynode(operator,table[i+1],eqt_addvalue(neutral));
        NewOper2(nodex,tmp);
      }
      else
        NewOper2(nodex,table[i+1]);
      break;
    }
  }
  mbkfree(table);
}

/*}}}************************************************************************/
/*{{{                    eqt_get_neutral()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
double eqt_get_neutral(long operator)
{
  switch (operator)
  {
    case EQTPLUS : return 0.0;
    case EQTMULT : return 1.0;
    default      : return -1.0;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_sortDoTable()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node **eqt_sortDoTable(eqt_node *node,int cpt,int *nb)
{
  eqt_node  **table;

  if (eqt_isLeaf(node))
    if (cpt)
    {
      int     eltsize = sizeof(eqt_node *);

      table           = (eqt_node**)mbkalloc((cpt+1)*eltsize);
      table[cpt]      = node;
      *nb             = cpt + 1;
      return table;
    }
    else
      return NULL;
  else
  {
    table      = eqt_sortDoTable(Oper2(node),cpt+1,nb);
    eqt_sortBranch(Oper1(node));
    table[cpt] = Oper1(node);
    return table;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_sortCompareNode()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int eqt_sortCompareNode(const void *node1, const void *node2)
{
  eqt_node  *n1 = *(eqt_node **) node1;
  eqt_node  *n2 = *(eqt_node **) node2;
  int        leafres;

  if (n1->TYPE == n2->TYPE)
    if (IsVarname(n1))
      return strcmp(Varname(n1),Varname(n2));
    else if (Operator (n1) != 0 && Operator(n1) == Operator(n2))
      if (IsVarname(Oper1(n1)))
        if (IsVarname(Oper1(n2)))
          return strcmp(Varname(Oper1(n1)),Varname(Oper1(n2)));
        else
          return -1;
      else if (IsVarname(Oper1(n2)))
        return 1;
      else
        return 0;
    else
      return 0;
  else if (IsValue(n1))
    return 1;
  else if (IsVarname(n1))
    return -1;
  else if ((leafres = eqt_isLeaf(n1)))
    if (IsVarname(n2))
      return 1;
    else
      return leafres;
  else if (IsVarname(n2))
    return 1;
  else if (IsValue(n2))
    return -1;
  else if ((leafres = -eqt_isLeaf(n2)))
    return leafres;
  else
    return (Operator(n1) - Operator(n2));
}

/*}}}************************************************************************/
/*{{{                    eqt_associate()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_associate(eqt_node *node)
{
  eqt_node  *res, *father;
  
  father    = node->FATHER;
  switch (node->TYPE)
  {
    case EQTVALUE_T : case EQTVARNAME_T :
         return node;
    case EQTUNOP_T :
         NewOper(node,eqt_associate(Oper(node)));
         return node;
    case EQTBINOP_T:
         if (Operator(node) == EQTPLUS)
         {
           if (IsVarname(Oper1(node)))
             if (eqt_findSimpleAssoc(Oper2(node),Varname(Oper1(node)),1.0))
             {
               res          = Oper2(node);
               res->FATHER  = father;
               eqt_delnode(Oper1(node));
               eqt_delnode(node);
               node         = eqt_associate(res);
             }
             else
               NewOper2(node,eqt_associate(Oper2(node)));
           else if (Operator(Oper1(node)) == EQTMIN 
                    && IsVarname(Oper(Oper1(node))))
             if (eqt_findSimpleAssoc(Oper2(node),
                                     Varname(Oper(Oper1(node))),-1.0))
             {
               res          = Oper2(node);
               res->FATHER  = father;
               eqt_delnode(Oper(Oper1(node)));
               eqt_delnode(Oper1(node));
               eqt_delnode(node);
               node         = eqt_associate(res);
             }
             else
             {
               eqt_assocVar(node);
               NewOper2(node,eqt_associate(Oper2(node)));
             }
           else
             NewOper2(node,eqt_associate(Oper2(node)));
         }
         return node;
    default :
         avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//         eqt_error(__func__);
         return NULL;
  }
}


/*}}}************************************************************************/
/*{{{                    eqt_assocVar()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_assocVar(eqt_node *node)
{
  chain_list    *var, *lastvar, *chainx;
  eqt_node      *nodex, *next;

  var       = NULL;
  if (Operator(node) == EQTPLUS && Operator(Oper1(node)) == EQTMULT)
  {
    lastvar   = NULL;
    for (nodex = node; Operator(nodex) == EQTPLUS; nodex = Oper2(nodex))
    {
      var     = eqt_assocGetVar(Oper1(nodex));
      if (lastvar)
        for ( ; lastvar; lastvar = delchain(lastvar,lastvar))
        {
          for ( chainx = var; chainx; chainx = chainx->NEXT)
            if (lastvar->DATA == chainx->DATA)
            {
              next    = nodex->FATHER;
              eqt_assocFather(nodex,chainx->DATA);
              nodex   = next;
              var     = delchain(var,chainx);
              break;
            }
          if (chainx)
          {
            freechain(lastvar);
            break;
          }
        }
      lastvar = var;
    }
  }
  freechain(var);
}

/*}}}************************************************************************/
/*{{{                    eqt_assocFather()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void eqt_assocFather(eqt_node *node, char *var)
{
  eqt_node  *father;
  
  father    = node->FATHER;
  eqt_assocPutVarTop(node,var);
  eqt_assocPutVarTop(father,var);
  NewOper2(father,Oper2(node));
  NewOper2(node,Oper2(Oper1(node)));
  eqt_delnode(Oper1(Oper1(node)));
  eqt_delnode(Oper1(node));
  NewOper1(node,Oper2(Oper1(father)));
  NewOper2(Oper1(father),
           eqt_addbinarynode(EQTMULT,node,eqt_addvalue(1.0)));
}

/*}}}************************************************************************/
/*{{{                    eqt_assocPutVarTop()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void eqt_assocPutVarTop(eqt_node *node,char *var)
{
  eqt_node  *nodex;

  for (nodex = Oper1(node); Varname(Oper1(nodex))!= var; nodex = Oper2(nodex))
    ;
  if (nodex != Oper1(node))
  {
    NewOper2(nodex->FATHER,Oper2(nodex));
    NewOper2(nodex,Oper1(node));
    NewOper1(node,nodex);
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_assocGetVar()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static chain_list *eqt_assocGetVar(eqt_node *node)
{
  if (IsValue(node) || !IsVarname(Oper1(node)))
    return NULL;
  else
    return addchain(eqt_assocGetVar(Oper2(node)),Varname(Oper1(node)));
}

/*}}}************************************************************************/
/*{{{                    eqt_findSimpleAssoc()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int eqt_findSimpleAssoc(eqt_node *node, char* var, double nb)
{
  int        res;

  if (IsValue(node))
    return 0;
  else if (eqt_isLeaf(Oper1(node)))
  {
    if (IsVarname(Oper1(node)))
    {
      if (Varname(Oper1(node)) == var)
      {
        NewOper2(node->FATHER,Oper2(node));
        eqt_delnode(Oper1(node));
        res     =  1 + eqt_findSimpleAssoc(Oper2(node),var,nb+1.0);
        eqt_delnode(node);
        return res;
      }
      else
        return eqt_findSimpleAssoc(Oper2(node),var,nb);
    }
    else if (Operator(Oper1(node)) == EQTMIN)
    {
      if (IsVarname(Oper(Oper1(node)))
          && Varname(Oper(Oper1(node))) == var)
      {
        NewOper2(node->FATHER,Oper2(node));
        eqt_delnode(Oper(Oper1(node)));
        eqt_delnode(Oper1(node));
        res     =  1 + eqt_findSimpleAssoc(Oper2(node),var,nb-1.0);
        eqt_delnode(node);
        return res;
      }
      else
        return eqt_findSimpleAssoc(Oper2(node),var,nb);
    }
    else
      return eqt_findSimpleAssoc(Oper2(node),var,nb);
  }
  else
  {
    eqt_node    *tmp;
    
    tmp = eqt_addbinarynode(EQTMULT,eqt_addvarname(var),eqt_addvalue(nb));
    eqt_insertOperation(node,tmp);
    
    return 1;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_insertOperation()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
eqt_node *eqt_insertOperation(eqt_node *node, eqt_node *operation)
{
  eqt_node      *tmp;
  eqt_node      *father;
  
  father    = node->FATHER;
  if (IsVarname(Oper1(node)))
    NewOper2(node,eqt_insertOperation(Oper2(node),operation));
  else if (IsValue(node))
    node    = eqt_addbinarynode(Operator(father),operation,node);
  else
  {
    tmp     = eqt_addbinarynode(Operator(father),Oper1(node),Oper2(node));
    NewOper2(node,tmp);
    NewOper1(node,operation);
  }
  
  return node;
}

/*}}}************************************************************************/
/*{{{                    eqt_isRealLeaf()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int eqt_isRealLeaf(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTVALUE_T : case EQTVARNAME_T :
         return 1;
    default :
         return 0;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_invertChild()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eqt_invertChild(eqt_node *node)
{
  eqt_node  *node1;

  switch (node->TYPE)
  {
    case EQTBINOP_T :
         node1 = Oper1(node);
         NewOper1(node,Oper2(node));
         NewOper2(node,node1);
         break;
    default :
         ;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_isLeaf()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int eqt_isLeaf(eqt_node *node)
{
  switch (node->TYPE)
  {
    case EQTVALUE_T :
         return 1;
    case EQTVARNAME_T :
         return -1;
    case EQTUNOP_T :
         return eqt_isLeaf(Oper(node));
    default :
         return 0;
  }
}

/*}}}************************************************************************/
/*{{{                    eqt_isOperand2()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int eqt_isOperand2(eqt_node *node)
{
  eqt_node  *father = node->FATHER;

  if (father)
    switch (node->TYPE)
    {
      case EQTUNOP_T :
           return 1;
      case EQTBINOP_T :
           return (Oper2(father) == node);
      default :
           return 0;
    }
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    eqt_getEvaluedEquation()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *eqt_getEvaluedEquation(eqt_ctx *ctx, char *equation)
{
  eqt_node  *node   = NULL;
  char      *res    = NULL;

  node = eqt_create(ctx, equation);
  if (!node)
    avt_errmsg(EQT_ERRMSG, "008", AVT_ERROR, __func__);
//    eqt_error(__func__);
  else
  {
    node = eqt_DelMinDiv(node);
    eqt_linearise(node);
    eqt_addNeutral(node);
    eqt_sortBranch(node);
    node = eqt_associate(node);
    eqt_assocVar(node);
    eqt_reduce(ctx, node);
    node = eqt_DelNeutral(node);
    res  = eqt_ToIntStr(node);
    eqt_freenode(node);
  }
  return res;
}

/*}}}************************************************************************/
/*{{{                    eqt_getSimpleEquation()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *eqt_getSimpleEquation(char *equation)
{
  char    *res    = NULL;
  eqt_ctx *ctx;

  ctx = eqt_init(EQT_NB_VARS);
  res   = eqt_getEvaluedEquation(ctx, equation);
  eqt_term (ctx);

  return res;
}


/*}}}************************************************************************/

static double sum (double p, double q)
{
  return p + q ;
}


static double dmax(double p, double q)
{
 double r ;
 if ( p >= q )
    r = p ;
 else
    r = q ;
 return r ;
}

/******************************************************************************/
/******************************************************************************/
static double dmin(double p, double q)
{
 double r ;
 if ( p <= q )
    r = p ;
 else
    r = q ;
 return r ;
}

/******************************************************************************/
/******************************************************************************/
static double v2(double p, double q)
{
 double r ;
 r = p - q ;
 return r ;
}

/******************************************************************************/
static double valif(double a, double b, double c)
{
 return (int)(a + 0.5) ? b : c ;
}

static double eqt_trunc(double val)
{
  if (val>=0) return floor(val);
  return ceil(val);
}

static double spi_trunc (double val)
{
  return (double)(int)val;
}

static double ppar (double val)
{
  return val;
}

static double eqt_sgn(double x)
{
  if (x<0) return -1;
  if (x>0) return 1;
  return 0;
}

static double eqt_pow(double x, double y)
{
  return pow(x, eqt_trunc(y));
}

static double eqt_pwr(double x, double y)
{
  return copysign(pow(fabs(x), y), x);
}

static double eqt_sign(double x, double y)
{
  return copysign(x, y);
}
static double eqt_e(double a, double b)
{
  return a*pow(10, b);
}

static int doneseed=0, displayseed=1 ;
static unsigned int eqt_seedp=0, eqt_seedp_save=0;

void eqt_srand(unsigned int seed)
{
  eqt_seedp=seed;
  doneseed=1;
}

void eqt_initseed(int mode, unsigned int value)
{
  if (mode) eqt_seedp=mbk_get_a_seed();
  else eqt_seedp=value;
  displayseed=1;
  doneseed=1;
  eqt_seedp_save=eqt_seedp;
}
unsigned int eqt_getinitseed()
{
  return eqt_seedp_save;
}

static double ranf()         /* ranf() is uniform in 0..1 */
{
  if( !doneseed ) {
    eqt_initseed(1, 0);
  }
  if (displayseed)
   {
     char buf[256];
     sprintf(buf, "info:Main monte-carlo seed = %u", eqt_seedp);
     avt_log(-1, 0, "%s\n", &buf[5]);
     if (MBK_SOCK!=-1)
       mbk_sendmsg( MBK_COM_DATA, buf, strlen(buf)+1);
     displayseed=0;
   }
  EQT_GOT_RANDOM=1;
  return ((double)mbk_rand_r(&eqt_seedp) / (double)MBK_RAND_MAX);
}

unsigned int eqt_get_current_srand_seed()
{
  if (!doneseed) 
  {
    eqt_initseed(1, 0) ;
  }
  return eqt_seedp;
}

static double box_muller(double m, double s/*, double sigma_level*/)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
	double x1, x2, w, y1;
        double d ;
	static double y2;
	static int use_last = 0;
        double r ;

   if( V_BOOL_TAB[ __AVT_ENABLE_STAT ].VALUE ) {
//     s/=sigma_level;
     do 
     {
          if (use_last)		        /* use value from previous call */
          {
                  y1 = y2;
                  use_last = 0;
          }
          else
          {
                  do {
                          x1 = 2.0 * ranf() - 1.0;
                          x2 = 2.0 * ranf() - 1.0;
                          w = x1 * x1 + x2 * x2;
                  } while ( w >= 1.0 );

                  w = sqrt( (-2.0 * log( w ) ) / w );
                  y1 = x1 * w;
                  y2 = x2 * w;
                  // commente pour que le seed fonctionne bien
                  //use_last = 1;
          }
     } while (fabs(y1)>6);
    r = m + y1 * s ;
    if( V_INT_TAB[ __EQT_STATISTICAL_DISCRETISATION ].VALUE > 0 ) {
      d = 2*s/((double)V_INT_TAB[ __EQT_STATISTICAL_DISCRETISATION ].VALUE) ;
      r = d*floor( (r-(m-d/2.0))/d ) + m;
      /*
      if( r >= m-d/2 && r <= m+d/2 )
        r = m ;
      else {
        if( r>m )
          r = floor( ( r-(m+d/2) )/d )*d + (m+d/2.0) ;
        else
          r = ceil( ( r-(m-d/2) )/d )*d + (m-d/2.0) ;
      }
      */
    }
  }
  else
    r = m;

  return r ;
}

static double eqt_limit(double nom, double var)
{
  if( V_BOOL_TAB[ __AVT_ENABLE_STAT ].VALUE ) {
    EQT_GOT_RANDOM=1;
    if (mbk_rand_r(&eqt_seedp)>MBK_RAND_MAX/2)
       return nom+var;
    else
       return nom-var;
  }
  return nom;
}

static double eqt_unif(double nom, double rel)
{
  if( V_BOOL_TAB[ __AVT_ENABLE_STAT ].VALUE ) {
    double a=nom*(1-rel), b=nom*(1+rel);
    return a+ranf()*(b-a);
  }
  return nom;
}

static double eqt_aunif(double nom, double rel)
{
  if( V_BOOL_TAB[ __AVT_ENABLE_STAT ].VALUE ) {
    double a=nom-rel, b=nom+rel;
    return a+ranf()*(b-a);
  }
  return nom;
}

static double eqt_agauss(double m, double s, double sigma_level)
{
  return box_muller(m, s/sigma_level);
}

static double eqt_gauss(double m, double s, double sigma_level)
{
  return box_muller(m, s*m/sigma_level);
}

static double eqt_lognorm(double nom, double var, double sigma_level)
{
  double m=log(nom), s=var*m/sigma_level;
  double u=nom*exp(s*s/2), sig=nom*sqrt(exp(2*s*s)-exp(s*s));
  return box_muller(u, sig);
}
static double eqt_alognorm(double nom, double var, double sigma_level)
{
  double /*m=log(nom),*/ s=var/sigma_level;
  double u=nom*exp(s*s/2), sig=nom*sqrt(exp(2*s*s)-exp(s*s));
  return box_muller(u, sig);
}

void eqt_add_spice_extension(eqt_ctx *ctx)
{
   chain_list *cl;

   eqt_addfunction3(ctx, "valif",&valif) ;
   eqt_addfunction2(ctx, "dmin",&dmin) ;
   eqt_addfunction2(ctx, "dmax",&dmax) ;
   eqt_addfunction2(ctx, "max",&dmax) ;
   eqt_addfunction2(ctx, "min",&dmin) ;
   eqt_addfunction2(ctx, "sum",&sum) ;
//   eqt_addfunction2(ctx, "v",&v2) ;
   eqt_addfunction (ctx, "trunc", &spi_trunc);
   eqt_addfunction (ctx, "int", &spi_trunc);
   eqt_addfunction (ctx, "ppar", &ppar);

   eqt_addfunction(ctx, "sgn",&eqt_sgn) ;
   eqt_addfunction2(ctx, "pow",&eqt_pow) ;
   eqt_addfunction2(ctx, "pwr",&eqt_pwr) ;
   eqt_addfunction2(ctx, "sign",&eqt_sign) ;

   cl=addchain(NULL, "c");
   cl=addchain(cl, "b");
   cl=addchain(cl, "a");
   eqt_adddotfunc(ctx, "symdistr", cl, "a-c*(b-a)");
   freechain(cl);

   cl=addchain(NULL, "d");
   cl=addchain(cl, "c");
   cl=addchain(cl, "b");
   cl=addchain(cl, "a");

   eqt_adddotfunc(ctx, "asymdistr", cl, "b+(1/(1+d*(c+a-2*b)/(c-a))-1)*2*(c-b)*(b-a)/(c+a-2*b)");
   eqt_adddotfunc(ctx, "skewcor", cl, "(d>0?1:0)*(b-a)*d+(d<=0?1:0)*(a-c)*d");
   eqt_adddotfunc(ctx, "distr", cl, "b+d*2*(c-b)*(a-b)/((c==b)||(b==a)||(c==a)?1:(c-a)+d*(c+a-2*b))");
   eqt_addfunction2(ctx, "limit",&eqt_limit) ;
   eqt_addfunction2(ctx, "e",&eqt_e) ;
   eqt_addfunction2(ctx, "aunif",&eqt_aunif) ;
   eqt_addfunction2(ctx, "unif",&eqt_unif) ;
   eqt_addfunction3(ctx, "gauss",&eqt_gauss) ;
   eqt_addfunction3(ctx, "agauss",&eqt_agauss) ;
   eqt_addfunction3(ctx, "alognorm",&eqt_alognorm) ;
   eqt_addfunction3(ctx, "lognorm",&eqt_lognorm) ;
   
   freechain(cl);

   ctx->USE_SPICE_UNITS = 1;
}

void eqt_setspecialfunc(eqt_ctx *ctx, int index, double (*func)(char *var))
{
  ctx->SPECIAL_FUNC[index]=func;
}


