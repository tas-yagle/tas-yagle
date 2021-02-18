/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_tree_utils.c                                            */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 08/04/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include MUT_H
#include AVT_H
#include "gen_tree_utils.h"
#include "gen_tree_parser.h"

static HeapAlloc tree_heap;
static HeapAlloc syminfo_heap;

static int initcount=0;
void APIInit()
{
  if (initcount==0)
    {
      CreateHeap(sizeof(tree_list), 0, &tree_heap);
      CreateHeap(sizeof(SymInfoItem), 0, &syminfo_heap);
    }
  initcount++;
}

void APIExit()
{
  initcount--;
  if (initcount==0)
    {
      DeleteHeap(&tree_heap);
      DeleteHeap(&syminfo_heap);
    }
}

int APIBlockIsC(tree_list *tree)
{
  return TOKEN(tree)==GEN_TOKEN_ACTION;
}

char *APIGetCName(tree_list *tree)
{
  return getname_of(tree->NEXT);
}

tree_list *APIGetCTree(tree_list *tree)
{
  return tree->NEXT;
}

int APIBlockIsArchitecture(tree_list *tree)
{
  return TOKEN(tree)==GEN_TOKEN_ARCHITECTURE;
}

int APIBlockIsEntity(tree_list *tree)
{
  return TOKEN(tree)==GEN_TOKEN_ENTITY;
}

char *APIEntityName(tree_list *tree)
{
  return (char *)tree->NEXT->DATA->DATA;
}

void TreeHeap_Manage(int mode)
{
  if (mode==0)
    {
      CreateHeap(sizeof(tree_list), 0, &tree_heap);
    }
  else
    {
      DeleteHeap(&tree_heap);
    }
}

/****************************************************************************/
/*                   add a tree elem at the top                             */
/****************************************************************************/
extern tree_list* addtree(char* file, int lineno, int token, tree_list* data, tree_list* next)
{
   tree_list* pt;
/*   register int i;

   if (HEAD_TREE == NULL) {
      pt = (tree_list *)mbkalloc(BUFSIZE0*sizeof(tree_list));
      blocks_to_free=addchain(blocks_to_free,pt);
      HEAD_TREE = pt;
      for (i = 1; i < BUFSIZE0; i++) {
         pt->NEXT = pt + 1;
         pt++;
      }
      pt->NEXT = NULL;
   }

   pt = HEAD_TREE;
   HEAD_TREE = HEAD_TREE->NEXT;
  */
   pt=(tree_list *)AddHeapItem(&tree_heap);
   /*change fields*/
   pt->NEXT=next;
   pt->DATA=data;
   LINE(pt)=lineno;
   FILE_NAME(pt)=file;
   TOKEN(pt)=token;
   return pt;
}


/****************************************************************************/
/*              free recursively a tree_list from the top                   */
/****************************************************************************/

extern void freetree(tree_list* tree)
{
  tree_list *tr, *ntr;
  for (tr=tree; tr!=NULL; tr=ntr) { ntr=tr->NEXT; DelHeapItem(&tree_heap, tr); }
    
//   HEAD_TREE=(tree_list*) append((chain_list*)tree, (chain_list*)HEAD_TREE);
}


/****************************************************************************/
/*               build an atomic tree with a value                          */
/****************************************************************************/
extern tree_list *PutAto_Tree(token,file,lineno,value)
  char* file;
  int token,lineno;
  void* value;
{
return addtree(file,lineno,token,value,NULL);
}
  
extern void ChangeAto_Tree(tree_list *tree, void *value)
{  
  tree->DATA=value;
}

/****************************************************************************/
/*         build an unary operator typed token with its branch              */
/****************************************************************************/
extern tree_list *PutUni_Tree(token,file,lineno,branch)
  char* file;
  int token,lineno;
  tree_list *branch;
{
  branch=addtree(file,lineno,GEN_TOKEN_NODE,branch,NULL);
return addtree(file,lineno,token,NULL,branch);
}
  

/****************************************************************************/
/*         build an binary operator typed token with its branches           */
/****************************************************************************/
extern tree_list *PutBin_Tree(token,file,lineno,branch1,branch2)
  char* file;
  int token,lineno;
  tree_list *branch1,*branch2;
{
  branch2=addtree(file,lineno,GEN_TOKEN_NODE,branch2,NULL);
  branch1=addtree(file,lineno,GEN_TOKEN_NODE,branch1,branch2);
return addtree(file,lineno,token,NULL,branch1);
}
  
  
/****************************************************************************/
/*            build an operator typed token with its 3 branches             */
/****************************************************************************/
extern tree_list *PutTri_Tree(token,file,lineno,branch1,branch2,branch3)
  char* file;
  int token,lineno;
  tree_list *branch1,*branch2,*branch3;
{
  branch3=addtree(file,lineno,GEN_TOKEN_NODE,branch3,NULL);
  branch2=addtree(file,lineno,GEN_TOKEN_NODE,branch2,branch3);
  branch1=addtree(file,lineno,GEN_TOKEN_NODE,branch1,branch2);
return addtree(file,lineno,token,NULL,branch1);
}
  

/****************************************************************************/
/*            build an operator typed token with its 4 branches             */
/****************************************************************************/
extern tree_list *PutQua_Tree(token,file,lineno,branch1,branch2,branch3,branch4)
  char* file;
  int token,lineno;
  tree_list *branch1,*branch2,*branch3,*branch4;
{
  branch4=addtree(file,lineno,GEN_TOKEN_NODE,branch4,NULL);
  branch3=addtree(file,lineno,GEN_TOKEN_NODE,branch3,branch4);
  branch2=addtree(file,lineno,GEN_TOKEN_NODE,branch2,branch3);
  branch1=addtree(file,lineno,GEN_TOKEN_NODE,branch1,branch2);
return addtree(file,lineno,token,NULL,branch1);
}
 
 
/***************************************************************************/
/*            return the string of the first ident met                     */
/***************************************************************************/
extern char *getname(tree) 
   tree_list *tree;
{ 
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 1);
    //fprintf(stderr,"getname: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return getname(tree->DATA);
    break;
  case '(':/*vector*/
    return getname(tree->NEXT);
    break;
  case GEN_TOKEN_SIGNAL: case GEN_TOKEN_IN: case GEN_TOKEN_OUT: case GEN_TOKEN_INOUT: case GEN_TOKEN_TRANSCV: case GEN_TOKEN_TRISTATE: case GEN_TOKEN_LINKAGE:
  case GEN_TOKEN_VARIABLE: case GEN_TOKEN_INTEGER: case GEN_TOKEN_CHAR: case GEN_TOKEN_FILE: case GEN_TOKEN_REF: 
  case GEN_TOKEN_REF_CHAR: case GEN_TOKEN_REF_FILE: case GEN_TOKEN_OF: case GEN_TOKEN_STATIC: case GEN_TOKEN_DOUBLE: case GEN_TOKEN_VOIDTOKEN_D: case GEN_TOKEN_LONG:
  case GEN_TOKEN_REF_INT: case GEN_TOKEN_REF_DOUBLE: case GEN_TOKEN_REF_VOID: case GEN_TOKEN_ARRAY:
    return getname(tree->NEXT);
    break;
  case GEN_TOKEN_IDENT:
    return (char*) tree->DATA;
    break;
  default:
    Error_Tree("getname",tree);
    EXIT(2); 
	return NULL;
  }
}


/***************************************************************************/
/*             return the string of the second ident met                   */
/***************************************************************************/
extern char *getname_of(tree) 
   tree_list *tree;
{ 
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 2);
    //fprintf(stderr,"getname_of: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return getname_of(tree->DATA);
    break;
  case GEN_TOKEN_OF:
    return getname(tree->NEXT->NEXT);
    break;
  default:
    Error_Tree("getname_of",tree);
    EXIT(2); 
	return NULL;
  }
}

  
/***************************************************************************/
/*                      return the first ident met                         */
/***************************************************************************/
extern tree_list *getident(tree) 
   tree_list *tree;
{ 
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 3);
    //fprintf(stderr,"getident: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return getident(tree->DATA);
    break;
  case GEN_TOKEN_SIGNAL: case GEN_TOKEN_IN: case GEN_TOKEN_OUT: case GEN_TOKEN_INOUT: case GEN_TOKEN_TRANSCV: case GEN_TOKEN_TRISTATE: case GEN_TOKEN_LINKAGE:
  case GEN_TOKEN_VARIABLE: case GEN_TOKEN_INTEGER: case GEN_TOKEN_CHAR: case GEN_TOKEN_FILE:  case GEN_TOKEN_REF:
  case GEN_TOKEN_REF_CHAR: case GEN_TOKEN_REF_FILE: case GEN_TOKEN_OF: case GEN_TOKEN_STATIC:
    return getident(tree->NEXT);
    break;
  case GEN_TOKEN_IDENT:
    return tree;
    break;
  default:
    Error_Tree("getident",tree);
    EXIT(2); 
	return NULL;
  }
}


/***************************************************************************/
/*                   return the second ident met                           */
/***************************************************************************/
extern tree_list *getident_of(tree) 
   tree_list *tree;
{ 
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 4);
    //fprintf(stderr,"getident_of: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return getident_of(tree->DATA);
    break;
  case GEN_TOKEN_OF:
    return getident(tree->NEXT->NEXT);
    break;
  default:
    Error_Tree("getident_of",tree);
    EXIT(2); 
	return NULL;
  }
}


/***************************************************************************/
/*                       return size of vector                             */
/* tree is the vector and env the values of variables in expressions       */
/***************************************************************************/


/***************************************************************************/
/*              return a list of a tree structure                          */
/***************************************************************************/
extern chain_list *tree2chain(tree_list *tree, chain_list *pile)
{
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 5);
    //fprintf(stderr,"tree2chain: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /*nodes..*/
  case GEN_TOKEN_NODE:
      return tree2chain(tree->DATA,pile);
      break;
  case ',': case ';': 
      /*to revert effect of addchain*/
      pile=tree2chain(tree->NEXT->NEXT,pile);
      return tree2chain(tree->NEXT,pile);
      break;
  default:
      return addchain(pile,tree);
  }
}


/****************************************************************************/
/*  return a duplicate  of tree.  Rq: strings aren't reallocated  */
/****************************************************************************/
extern tree_list *Duplicate_Tree(tree)
   tree_list *tree;
{ 
  tree_list *inter;
  if (!tree) {
    return NULL;
    fprintf(stderr,"Duplicate_Tree: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* as an ABL this token is a node */
  case GEN_TOKEN_NODE:
    return addtree(FILE_NAME(tree),LINE(tree),tree->TOKEN,Duplicate_Tree(tree->DATA),NULL);
    break;
  /*fourth class*/
  case GEN_TOKEN_PRAGMA_SYM:
    return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),(tree_list *)dupchainlst((chain_list *)tree->DATA),NULL);
  case GEN_TOKEN_FOR:  case GEN_TOKEN_ENTITY: case GEN_TOKEN_OF:
    inter=Duplicate_Tree(tree->NEXT);
    inter->NEXT=Duplicate_Tree(tree->NEXT->NEXT);
    inter->NEXT->NEXT=Duplicate_Tree(tree->NEXT->NEXT->NEXT);
    inter->NEXT->NEXT->NEXT=Duplicate_Tree(tree->NEXT->NEXT->NEXT->NEXT);
    return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),NULL,inter);    
    break;
  /*third class*/
  case GEN_TOKEN_ELSE: case '?':  case GEN_TOKEN_ARCHITECTURE:
  case GEN_TOKEN_COMPONENT: case GEN_TOKEN_MBKFOPEN:
    inter=Duplicate_Tree(tree->NEXT);
    inter->NEXT=Duplicate_Tree(tree->NEXT->NEXT);
    inter->NEXT->NEXT=Duplicate_Tree(tree->NEXT->NEXT->NEXT);
    return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),NULL,inter);    
    break;
  case GEN_TOKEN_CAST:
    inter=Duplicate_Tree(tree->NEXT->NEXT->NEXT); // expr
    inter=addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree->NEXT->NEXT),tree->NEXT->NEXT->DATA,inter);
    inter=addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree->NEXT),tree->NEXT->DATA,inter);
    return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),NULL,inter);
  /*binary class*/
  case GEN_TOKEN_FPRINTF: case GEN_TOKEN_SPRINTF: case GEN_TOKEN_BSPRINTF: case GEN_TOKEN_EQUI: case GEN_TOKEN_MAP:
  case '+': case '-': case '*': case '/': case '%': case '>': case '<':
  case '&': case '|': case '^': case GEN_TOKEN_EG:
  case GEN_TOKEN_SUPEG: case GEN_TOKEN_INFEG: case GEN_TOKEN_NOTEG: case GEN_TOKEN_SHR: case GEN_TOKEN_SHL:
  case ';': case ',': case GNS_TOKEN_AND: case GEN_TOKEN_XOR: case GEN_TOKEN_MOD: case GEN_TOKEN_POW:
  case '=': case GEN_TOKEN_IF: case GEN_TOKEN_DO: case GEN_TOKEN_WHILE: case '(': case GEN_TOKEN_ACTION:
  case GEN_TOKEN_OR: case GEN_TOKEN_FOPEN: case GEN_TOKEN_TO: case GEN_TOKEN_DOWNTO:  case GEN_TOKEN_FUNCTION: 
  case GEN_TOKEN_OTHER_TYPES: case GEN_TOKEN_ARRAY:
    inter=Duplicate_Tree(tree->NEXT);
    inter->NEXT=Duplicate_Tree(tree->NEXT->NEXT);
    return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),NULL,inter);
    break;
  /*unary class*/
  case GEN_TOKEN_OPPOSITE: case '~':
  case GEN_TOKEN_INC_BEFORE: case GEN_TOKEN_INC_AFTER: case GEN_TOKEN_DEC_BEFORE: case GEN_TOKEN_DEC_AFTER:
  case GEN_TOKEN_IN: case GEN_TOKEN_OUT: case GEN_TOKEN_INOUT: case GEN_TOKEN_TRANSCV: case GEN_TOKEN_TRISTATE: case GEN_TOKEN_LINKAGE:
  case GEN_TOKEN_GENERIC: case GEN_TOKEN_PORT: case GEN_TOKEN_EXCLUDE:
  case GEN_TOKEN_NOT:
  case GEN_TOKEN_VARIABLE: case GEN_TOKEN_SIGNAL: case GEN_TOKEN_INTEGER: case GEN_TOKEN_FCLOSE: case GEN_TOKEN_VOIDTOKEN_D:
  case GEN_TOKEN_CHAR: case GEN_TOKEN_FILE: case GEN_TOKEN_REF_CHAR: case GEN_TOKEN_REF_FILE: case GEN_TOKEN_DOUBLE: case GEN_TOKEN_LONG:
  case GEN_TOKEN_BLOCK: case GEN_TOKEN_EXIT: case GEN_TOKEN_STATIC: case GEN_TOKEN_ADR: 
    inter=Duplicate_Tree(tree->NEXT);
    return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),NULL,inter);
    break;
  /*terminal*/
  case GEN_TOKEN_REF:
    inter=Duplicate_Tree(tree->NEXT);
    inter->NEXT=tree->NEXT->NEXT; // ref depth
    return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),NULL,inter);
  case GEN_TOKEN_STDOUT: case GEN_TOKEN_STDERR: case GEN_TOKEN_RETURN: case GEN_TOKEN_BREAK:
  case GEN_TOKEN_IDENT: case GEN_TOKEN_DIGIT: case GEN_TOKEN_DIGIT_CHAR: case GEN_TOKEN_NOP: case GEN_TOKEN_LOINS:/*pointer on a loins*/
  case GEN_TOKEN_NULL:
    return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),tree->DATA,NULL);
    /*namealloc or no data pointer*/
    break;
  case GEN_TOKEN_DIGIT_DOUBLE:
    {
      double *a=(double *)mbkalloc(sizeof(double));
      *a=*(double *)tree->DATA;
      return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),(tree_list *)a,NULL);
    }
  case GEN_TOKEN_VOIDTOKEN:
     return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),(tree_list *)dupptypelst((ptype_list *)tree->DATA),NULL);
  case GEN_TOKEN_VOIDTOKEN_CL:
     return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),(tree_list *)dupchainlst((chain_list *)tree->DATA),NULL);
  case GEN_TOKEN_VOIDTOKEN_CL_TA:
     return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),(tree_list *)APIDupARGS((chain_list *)tree->DATA),NULL);
  case GEN_TOKEN_STRING:
    {char *a=(char *)mbkalloc(strlen((char*)tree->DATA)+1);
    strcpy(a,(char*)tree->DATA);
    return addtree(FILE_NAME(tree),LINE(tree),TOKEN(tree),(tree_list*)a,NULL);
    break;}
  default:
    Error_Tree("Duplicate_Tree",tree);
    EXIT(2); 
	return NULL;
  }
}



/****************************************************************************/
/*  Dump on FILE *stdout the syntaxical tree typed in file2tree.h     */
/****************************************************************************/
extern void Dump_Tree(tree_list *tree)
{
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 6);
    //fprintf(stderr,"Dump_Tree: NULL pointer\n");
    EXIT(1); 
  }

  switch (TOKEN(tree)) {
  /* as an ABL token is a node */
  case GEN_TOKEN_NODE:
    Dump_Tree(tree->DATA);
    break;
  case GEN_TOKEN_ENTITY:
    fprintf(stdout,"\n******************************************************\n");
    fprintf(stdout," ENTITY ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," IS\n");
    Dump_Tree(tree->NEXT->NEXT);
    Dump_Tree(tree->NEXT->NEXT->NEXT);
    fprintf(stdout,"\n");
    break;
  case GEN_TOKEN_ARCHITECTURE:
    fprintf(stdout,"\n******************************************************\n");
    fprintf(stdout," ARCHITECTURE ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," IS\n");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout," BEGIN\n");
    Dump_Tree(tree->NEXT->NEXT->NEXT);
    fprintf(stdout," END\n");
    break;
  case GEN_TOKEN_ACTION:
    fprintf(stdout,"\n******************************************************\n");
    fprintf(stdout," ACTION ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," IS\n BEGIN\n");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout," END\n");
    break;
  case GEN_TOKEN_GENERIC:
    fprintf(stdout," GENERIC (");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," )\n");
    break;
  case GEN_TOKEN_PORT:
    fprintf(stdout," PORT (");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," )\n");
    break;
  case GEN_TOKEN_MAP:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," PORT MAP (");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout," )\n");
    break;
  case GEN_TOKEN_COMPONENT:
    fprintf(stdout," COMPONENT ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," IS\n ");
    Dump_Tree(tree->NEXT->NEXT);
    Dump_Tree(tree->NEXT->NEXT->NEXT);
    fprintf(stdout,"\n");
    break;
  case GEN_TOKEN_OF:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," OF ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case '+': case '-': case '*': case '/': case '%': case '>': case '<':
  case '&': case '|': case '^': case ';': case ',': case '=':
    Dump_Tree(tree->NEXT);
    fprintf(stdout," %c ",(char)TOKEN(tree));
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case '?':
    Dump_Tree(tree->NEXT);
    fprintf(stdout," ? ");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout," : ");
    Dump_Tree(tree->NEXT->NEXT->NEXT);
    break;
  case '~':
    fprintf(stdout," ~");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_MOD:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," mod ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_POW:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," ** ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GNS_TOKEN_AND:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," && ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_OR:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," || ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_XOR:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," xor ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_NOT:
    fprintf(stdout," !");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_OPPOSITE: 
    fprintf(stdout," -");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_EG:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," == ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_NOTEG:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," != ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_SUPEG:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," >= ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_INFEG:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," <= ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_SHL:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," << ");
    Dump_Tree(tree->NEXT->NEXT);    
    break;
  case GEN_TOKEN_SHR:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," >> ");
    Dump_Tree(tree->NEXT->NEXT);    
    break;
  case GEN_TOKEN_FPRINTF:
  case GEN_TOKEN_SPRINTF:
  case GEN_TOKEN_BSPRINTF:
    fprintf(stdout,"FPRINTF ( ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout,", ");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout,") ");
    fflush(stdout);
    break;
  case GEN_TOKEN_EXCLUDE:
    fprintf(stdout,"EXCLUDE (");
    Dump_Tree(tree->NEXT);
    fprintf(stdout,") ");
    fflush(stdout);
    break;
  case GEN_TOKEN_FOPEN:
    fprintf(stdout,"FOPEN (");
    Dump_Tree(tree->NEXT);
    fprintf(stdout,", ");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout,") ");
    fflush(stdout);
    break;
  case GEN_TOKEN_MBKFOPEN:
    fprintf(stdout,"MBKFOPEN (");
    Dump_Tree(tree->NEXT);
    fprintf(stdout,", ");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout,", ");
    Dump_Tree(tree->NEXT->NEXT->NEXT);
    fprintf(stdout,") ");
    fflush(stdout);
    break;
  case GEN_TOKEN_FCLOSE:
    fprintf(stdout,"FCLOSE (");
    Dump_Tree(tree->NEXT);
    fprintf(stdout,") ");
    fflush(stdout);
    break;
  case GEN_TOKEN_FOR:
    fprintf(stdout,"FOR( ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," ; ");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout," ; ");
    Dump_Tree(tree->NEXT->NEXT->NEXT);
    fprintf(stdout," ) {\n");
    Dump_Tree(tree->NEXT->NEXT->NEXT->NEXT);
    fprintf(stdout,"}\n");
    break;
  case GEN_TOKEN_IF:
    fprintf(stdout,"IF( ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," ) {\n");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout,"}\n");
    break;
  case GEN_TOKEN_DO:
    fprintf(stdout,"DO{ ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," } \nWHILE(");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout,")\n");
    break;
  case GEN_TOKEN_WHILE:
    fprintf(stdout,"WHILE( ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," ) {\n");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout,"}\n");
    break;
  case GEN_TOKEN_ELSE:
    fprintf(stdout,"IF( ");
    Dump_Tree(tree->NEXT);
    fprintf(stdout," ) {\n");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout,"}\nELSE{\n");
    Dump_Tree(tree->NEXT->NEXT->NEXT);
    fprintf(stdout,"}\n");
    break;
  case '(':
    Dump_Tree(tree->NEXT);
    fprintf(stdout,"(");
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout,")");
    fflush(stdout);
    break;
  case GEN_TOKEN_TO:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," to ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_DOWNTO:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," downto ");
    Dump_Tree(tree->NEXT->NEXT);
    break;
  case GEN_TOKEN_IN:
    fprintf(stdout,"IN ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_OUT:
    fprintf(stdout,"OUT ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_INOUT:
    fprintf(stdout,"INOUT ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_TRISTATE:
    fprintf(stdout,"TRISTATE ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_TRANSCV:
    fprintf(stdout,"TRANSCV ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_LINKAGE:
    fprintf(stdout,"LINKAGE ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_INTEGER:
    fprintf(stdout,"INTEGER ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_BLOCK:
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_CHAR:
    fprintf(stdout,"CHAR ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_FILE:
    fprintf(stdout,"FILE ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_REF:
    fprintf(stdout,"*");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_VARIABLE:
    fprintf(stdout,"VARIABLE ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_SIGNAL:
    fprintf(stdout,"SIGNAL ");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_EQUI:
    Dump_Tree(tree->NEXT);
    fprintf(stdout," => ");
    Dump_Tree(tree->NEXT->NEXT);    
    break;
  case GEN_TOKEN_DEC_BEFORE:
    fprintf(stdout," --");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_DEC_AFTER:
    Dump_Tree(tree->NEXT);
    fprintf(stdout,"-- ");
    fflush(stdout);
    break;
  case GEN_TOKEN_INC_BEFORE:
    fprintf(stdout," ++");
    Dump_Tree(tree->NEXT);
    break;
  case GEN_TOKEN_INC_AFTER:
    Dump_Tree(tree->NEXT);
    fprintf(stdout,"++ ");
    fflush(stdout);
    break;
  case GEN_TOKEN_FUNCTION:
    fprintf(stdout," %s (",(char *)tree->NEXT->DATA);
    Dump_Tree(tree->NEXT->NEXT);
    fprintf(stdout,") ");
    fflush(stdout);
    break;
  case GEN_TOKEN_EXIT:
    fprintf(stdout,"exit(");
    Dump_Tree(tree->NEXT);
    fprintf(stdout,")");
    fflush(stdout);
    break;
  case GEN_TOKEN_BREAK:
    fprintf(stdout,"break");
    fflush(stdout);
    break;
  case GEN_TOKEN_RETURN:
    fprintf(stdout,"return");
    fflush(stdout);
    break;
  case GEN_TOKEN_STRING:
    fprintf(stdout,"\"%s\"",(char*)tree->DATA);
    fflush(stdout);
    break;
  case GEN_TOKEN_IDENT:
    fprintf(stdout,"%s",(char*)tree->DATA);
    fflush(stdout);
    break;
  case GEN_TOKEN_DIGIT:
    fprintf(stdout,"%d",(int)(long)tree->DATA);
    fflush(stdout);
    break;
   case GEN_TOKEN_LOINS:/*pointer on a loins in lofig*/
    if (tree->DATA) fprintf(stdout,"LOINS");
    else fprintf(stdout,"!!!no access to loins!!!");
    fflush(stdout);
    break;
  case GEN_TOKEN_STATIC:
    fprintf(stdout,"static ");
    fflush(stdout);
    break;
  case GEN_TOKEN_STDOUT:
    fprintf(stdout,"stdout");
    fflush(stdout);
    break;
  case GEN_TOKEN_STDERR:
    fprintf(stdout,"stderr");
    fflush(stdout);
    break;
  case GEN_TOKEN_NOP:
    fprintf(stdout,"N.O.P\n"); /*nothing to dump*/
    break;
  default:
    Error_Tree("Dump_Tree",tree);
  }
}



/****************************************************************************/
/*   Dump an error message on the uppest token of tree. Rq: the argument    */
/*               message should be a function name                          */
/*Rq: the number line dumped is just an information, it could be completely */
/* wrong... We only guaranty that for tokens GEN_TOKEN_IDENT, GEN_TOKEN_STRING and GEN_TOKEN_DIGIT, it */
/* is the good one. Anyway mostly, the number line is around the error ;-)  */
/****************************************************************************/
extern void Error_Tree(char *message,tree_list *tree)
{
  if (!tree) {
    avt_errmsg(API_ERRMSG, "004", AVT_FATAL, 7);
//    fprintf(stderr,"Error_Tree: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* node of ABL */
  case GEN_TOKEN_NODE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 8);
//    fprintf(stderr,"(%s): unexpected node in file %s:%d\n",message,FILE_NAME(tree),LINE(tree));
    break;
  case '(': case '?':
  case '+': case '-': case '*': case '/': case '%': case '>': case '<':
  case '&': case '|': case '^': case ';': case ',': case '=': case '~':
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 9);
//    fprintf(stderr,"(%s) %d: unexpected token '%c' in file %s\n",message,
//	    LINE(tree),TOKEN(tree),FILE_NAME(tree));
    break;
  case GEN_TOKEN_ARRAY:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 10);
//    fprintf(stderr,"(%s) %d: unexpected ARRAY in file %s\n",message,LINE(tree),FILE_NAME(tree));
    break;
  case GEN_TOKEN_TO:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 11);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"to",FILE_NAME(tree));
    break;
  case GEN_TOKEN_DOWNTO:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 12);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"downto",FILE_NAME(tree));
    break;
  case GEN_TOKEN_ENTITY:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 13);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"entity",FILE_NAME(tree));
    break;
  case GEN_TOKEN_ARCHITECTURE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 14);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,
//	    LINE(tree),"structure",FILE_NAME(tree));
    break;
  case GEN_TOKEN_ACTION:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 15);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"action",FILE_NAME(tree));
    break;
  case GEN_TOKEN_MOD:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 16);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"mod",FILE_NAME(tree));
    break;
  case GEN_TOKEN_POW:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 17);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"pow",FILE_NAME(tree));
    break;
  case GEN_TOKEN_MAP:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 18);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"port map",FILE_NAME(tree));
    break;
  case GEN_TOKEN_COMPONENT:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 19);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"component",FILE_NAME(tree));
    break;
  case GEN_TOKEN_OF:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 20);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"of",FILE_NAME(tree));
    break;
  case GEN_TOKEN_GENERIC:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 21);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"generic",FILE_NAME(tree));
    break;
  case GEN_TOKEN_PORT:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 22);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"port",FILE_NAME(tree));
    break;
  case GEN_TOKEN_VARIABLE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 23);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"variable",FILE_NAME(tree));
    break;
  case GEN_TOKEN_INTEGER:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 24);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"integer",FILE_NAME(tree));
    break;
  case GEN_TOKEN_BLOCK:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 25);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"block",FILE_NAME(tree));
    break;
  case GEN_TOKEN_CHAR:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 26);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"char",FILE_NAME(tree));
    break;
  case GEN_TOKEN_FILE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 27);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"FILE",FILE_NAME(tree));
    break;
  case GEN_TOKEN_REF:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 28);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"_REF",FILE_NAME(tree));
    break;
  case GEN_TOKEN_REF_FILE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 29);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"_REF_FILE",FILE_NAME(tree));
    break;
  case GEN_TOKEN_REF_CHAR:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 30);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"_REF_CHAR",FILE_NAME(tree));
    break;
  case GEN_TOKEN_STATIC:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 31);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"static",FILE_NAME(tree));
    break;
  case GEN_TOKEN_SIGNAL:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 32);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"signal",FILE_NAME(tree));
    break;
  case GNS_TOKEN_AND:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 33);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"&&",FILE_NAME(tree));
    break;
  case GEN_TOKEN_OR:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 34);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"||",FILE_NAME(tree));
    break;
  case GEN_TOKEN_EG:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 35);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"==",FILE_NAME(tree));
    break;
  case GEN_TOKEN_OPPOSITE: 
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 36);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"-",FILE_NAME(tree));
    break;
  case GEN_TOKEN_NOT:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 37);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"!",FILE_NAME(tree));
    break;
  case GEN_TOKEN_NOTEG:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 38);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"!=",FILE_NAME(tree));
    break;
  case GEN_TOKEN_SUPEG:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 39);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),">=",FILE_NAME(tree));
    break;
  case GEN_TOKEN_INFEG:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 40);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"<=",FILE_NAME(tree));
    break;
  case GEN_TOKEN_XOR:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 41);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"xor",FILE_NAME(tree));
    break;
  case GEN_TOKEN_FPRINTF:
  case GEN_TOKEN_SPRINTF:
  case GEN_TOKEN_BSPRINTF:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 42);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"fprintf/sprintf/bsprintf",FILE_NAME(tree));
    break;
  case GEN_TOKEN_EXCLUDE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 43);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"exclude",FILE_NAME(tree));
    break;
  case GEN_TOKEN_MBKFOPEN:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 44);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"mbkfopen",FILE_NAME(tree));
    break;
  case GEN_TOKEN_FOPEN:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 45);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"fopen",FILE_NAME(tree));
    break;
  case GEN_TOKEN_FCLOSE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 46);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"fclose",FILE_NAME(tree));
    break;
  case GEN_TOKEN_PRINTF:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 47);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"printf",FILE_NAME(tree));
    break;
  case GEN_TOKEN_FOR:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 48);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"for",FILE_NAME(tree));
    break;
  case GEN_TOKEN_IF:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 49);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"if",FILE_NAME(tree));
    break;
  case GEN_TOKEN_DO:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 50);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"do",FILE_NAME(tree));
    break;
  case GEN_TOKEN_WHILE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 51);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"while",FILE_NAME(tree));
    break;
  case GEN_TOKEN_ELSE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 52);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"else",FILE_NAME(tree));
    break;
  case GEN_TOKEN_IN:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 53);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"in",FILE_NAME(tree));
    break;
  case GEN_TOKEN_OUT:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 54);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"out",FILE_NAME(tree));
    break;
  case GEN_TOKEN_INOUT:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 55);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"inout",FILE_NAME(tree));
    break;
  case GEN_TOKEN_LINKAGE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 56);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"linkage",FILE_NAME(tree));
    break;
  case GEN_TOKEN_TRISTATE:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 57);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,
//	    LINE(tree),"tristate",FILE_NAME(tree));
    break;
  case GEN_TOKEN_TRANSCV:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 58);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,
//	    LINE(tree),"transcv",FILE_NAME(tree));
    break;
  case GEN_TOKEN_EQUI:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 59);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"=>",FILE_NAME(tree));
    break;
  case GEN_TOKEN_SHL:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 60);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"<<",FILE_NAME(tree));
    break;
  case GEN_TOKEN_SHR:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 61);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),">>",FILE_NAME(tree));
    break;
  case GEN_TOKEN_DEC_BEFORE: case GEN_TOKEN_DEC_AFTER:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 62);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"--",FILE_NAME(tree));
    break;
  case GEN_TOKEN_INC_BEFORE: case GEN_TOKEN_INC_AFTER:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 63);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"++",FILE_NAME(tree));
    break;
  case GEN_TOKEN_STRING: case GEN_TOKEN_IDENT:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 64);
//    fprintf(stderr, "(%s) %d: unexpected string '%s' in file %s\n",
//            message, LINE(tree), (char*)tree->DATA,FILE_NAME(tree));
    break;
  case GEN_TOKEN_DIGIT:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 65);
//    fprintf(stderr,"(%s) %d: unexpected digit '%d' in file %s\n",message,
//	    LINE(tree),(int)(long)tree->DATA,FILE_NAME(tree));
    break;
   case GEN_TOKEN_LOINS:/*pointer on a loins in lofig*/
      if (tree->DATA) avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 66);
//    fprintf(stdout,"(%s) unexpected loins in file %s\n",
//                              message,FILE_NAME(tree));
      else avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 67);
//    fprintf(stdout,"(%s) unexpected loins with null pointer in file %s\n",
//                   message,FILE_NAME(tree));
      break;
  case GEN_TOKEN_EXIT:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 68);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,
//	    LINE(tree),"EXIT",FILE_NAME(tree));
    break;
  case GEN_TOKEN_RETURN:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 69);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,
//	    LINE(tree),"RETURN",FILE_NAME(tree));
    break;
  case GEN_TOKEN_BREAK:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 70);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,
//	    LINE(tree),"BREAK",FILE_NAME(tree));
    break;
  case GEN_TOKEN_NOP:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 71);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,
//	    LINE(tree),"NOP",FILE_NAME(tree));
    break;
  case GEN_TOKEN_STDOUT:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 72);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"stdout",FILE_NAME(tree));
    break;
  case GEN_TOKEN_STDERR:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 73);
//    fprintf(stderr,"(%s) %d: unexpected token '%s' in file %s\n",message,LINE(tree),"stderr",FILE_NAME(tree));
    break;
  default:
    avt_errmsg(API_ERRMSG, "004", AVT_ERROR, 74);
//    fprintf(stderr,"(%s): token %d unknown\n",message,TOKEN(tree));
  }
}


/****************************************************************************/
/*       Free all the tree, just stay strings in memory(namealloc)          */
/****************************************************************************/
extern void Free_Tree(tree)
   tree_list *tree;
{
  if (!tree) {
    return;
  //  fprintf(stderr,"Free_Tree: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
  /* node of ABL */
  case GEN_TOKEN_NODE:
    Free_Tree(tree->DATA);
    break;
  case GEN_TOKEN_PRAGMA_SYM:
    freechain((chain_list *)tree->DATA);
    freetree(tree);
    break;
  /*fourth class*/
  case GEN_TOKEN_FOR: case GEN_TOKEN_ENTITY: case GEN_TOKEN_OF:
    Free_Tree(tree->NEXT->NEXT->NEXT->NEXT);
    Free_Tree(tree->NEXT->NEXT->NEXT);
    Free_Tree(tree->NEXT->NEXT);
    Free_Tree(tree->NEXT);
    freetree(tree); 
    break;
  /*third class*/
  case GEN_TOKEN_ELSE: case '?':   case GEN_TOKEN_ARCHITECTURE:
  case GEN_TOKEN_COMPONENT: case GEN_TOKEN_MBKFOPEN:
    Free_Tree(tree->NEXT->NEXT->NEXT);
    Free_Tree(tree->NEXT->NEXT);
    Free_Tree(tree->NEXT);
    freetree(tree); 
    break;
  case GEN_TOKEN_CAST:
    Free_Tree(tree->NEXT->NEXT->NEXT);
    freetree(tree);
    break;
  /*binary class*/
  case GEN_TOKEN_FPRINTF: case GEN_TOKEN_SPRINTF: case GEN_TOKEN_BSPRINTF: case'(': case GEN_TOKEN_TO: case GEN_TOKEN_DOWNTO:
  case GEN_TOKEN_ACTION: case GEN_TOKEN_EQUI: case GEN_TOKEN_MAP:
  case '+': case '-': case '*': case '/': case '%': case '>': case '<':
  case '&': case '|': case '^': case GNS_TOKEN_AND: case GEN_TOKEN_OR: case GEN_TOKEN_EG:
  case GEN_TOKEN_SUPEG: case GEN_TOKEN_INFEG: case GEN_TOKEN_NOTEG: case GEN_TOKEN_SHR: case GEN_TOKEN_SHL:
  case ';': case ',': case GEN_TOKEN_XOR: case GEN_TOKEN_MOD: case GEN_TOKEN_POW:
  case '=': case GEN_TOKEN_IF: case GEN_TOKEN_DO: case GEN_TOKEN_WHILE: case GEN_TOKEN_FOPEN:  case GEN_TOKEN_FUNCTION:
  case GEN_TOKEN_OTHER_TYPES:  case GEN_TOKEN_ARRAY:
    Free_Tree(tree->NEXT->NEXT);
    Free_Tree(tree->NEXT);
    freetree(tree); 
    break;
  /*unary class*/
  case GEN_TOKEN_OPPOSITE: case '~':
  case GEN_TOKEN_INC_BEFORE: case GEN_TOKEN_INC_AFTER: case GEN_TOKEN_DEC_BEFORE: case GEN_TOKEN_DEC_AFTER:
  case GEN_TOKEN_IN: case GEN_TOKEN_OUT: case GEN_TOKEN_INOUT: case GEN_TOKEN_TRANSCV: case GEN_TOKEN_TRISTATE: case GEN_TOKEN_LINKAGE:
  case GEN_TOKEN_NOT: case GEN_TOKEN_GENERIC: case GEN_TOKEN_PORT: 
  case GEN_TOKEN_EXCLUDE: 
  case GEN_TOKEN_ADR: 
  case GEN_TOKEN_VARIABLE: case GEN_TOKEN_SIGNAL: case GEN_TOKEN_INTEGER: case GEN_TOKEN_FCLOSE: case GEN_TOKEN_VOIDTOKEN_D:
  case GEN_TOKEN_CHAR: case GEN_TOKEN_FILE: case GEN_TOKEN_REF: case GEN_TOKEN_REF_CHAR: case GEN_TOKEN_REF_FILE: case GEN_TOKEN_DOUBLE: case GEN_TOKEN_LONG:
  case GEN_TOKEN_BLOCK: case GEN_TOKEN_EXIT: case GEN_TOKEN_STATIC: case GEN_TOKEN_REF_VOID: case GEN_TOKEN_REF_INT: case GEN_TOKEN_REF_DOUBLE:
    Free_Tree(tree->NEXT);
    tree->NEXT->NEXT=NULL;
    freetree(tree); 
    break;
  /*terminal*/
  case GEN_TOKEN_STRING: 
    mbkfree(tree->DATA);  /*mbkalloc*/
    freetree(tree); 
    break;
  case GEN_TOKEN_STDOUT: case GEN_TOKEN_STDERR: case GEN_TOKEN_IDENT: case GEN_TOKEN_DIGIT: case GEN_TOKEN_NOP: case GEN_TOKEN_LOINS:
  case GEN_TOKEN_BREAK: case GEN_TOKEN_RETURN: case GEN_TOKEN_DIGIT_CHAR: case GEN_TOKEN_NULL:
    freetree(tree); 
    break;
  case GEN_TOKEN_DIGIT_DOUBLE:
    mbkfree(tree->DATA); // (double*)
    freetree(tree); 
    break;
  case GEN_TOKEN_VOIDTOKEN:
    freeptype((ptype_list *)tree->DATA);
    freetree(tree); 
    break;
  case GEN_TOKEN_VOIDTOKEN_CL:
    freechain((chain_list *)tree->DATA);
    freetree(tree); 
    break;
  case GEN_TOKEN_VOIDTOKEN_CL_TA:
    {
      chain_list *cl;
      for (cl=(chain_list *)tree->DATA; cl!=NULL; cl=cl->NEXT)
         APIFreeTARG((t_arg *)cl->DATA);
      freechain((chain_list *)tree->DATA);
      freetree(tree); 
      break;
    }
  default:
    Error_Tree("Free_Tree",tree);
    EXIT(2);
  }
}


/****************************************************************************/
/*               return 1 if token is met in tree                           */
/****************************************************************************/
extern int SeekToken_Tree(tree,token)
   tree_list *tree;
   int token;
{
  if (!tree) {
    return 0;
//    fprintf(stderr,"SeekToken_Tree: NULL pointer\n");
    EXIT(1);
  }
  if (TOKEN(tree)==token) return 1;
  
  switch (TOKEN(tree)) {
  /* node of ABL */
  case GEN_TOKEN_NODE:
    return SeekToken_Tree(tree->DATA,token);
    break;
  /*fourth class*/
  case GEN_TOKEN_FOR:
    return SeekToken_Tree(tree->NEXT->NEXT->NEXT->NEXT,token)
    || SeekToken_Tree(tree->NEXT->NEXT->NEXT,token)
    || SeekToken_Tree(tree->NEXT->NEXT,token)
    || SeekToken_Tree(tree->NEXT,token);
    break;
  /*third class*/
  case GEN_TOKEN_ELSE: case '?': case GEN_TOKEN_ENTITY:  case GEN_TOKEN_ARCHITECTURE:
  case GEN_TOKEN_COMPONENT: case GEN_TOKEN_MBKFOPEN:
    return SeekToken_Tree(tree->NEXT->NEXT->NEXT,token)
    || SeekToken_Tree(tree->NEXT->NEXT,token)
    || SeekToken_Tree(tree->NEXT,token);
    break;
  /*binary class*/
  case GEN_TOKEN_FPRINTF: case GEN_TOKEN_SPRINTF:  case GEN_TOKEN_BSPRINTF: case'(': case GEN_TOKEN_TO: case GEN_TOKEN_DOWNTO:
  case GEN_TOKEN_ACTION: case GEN_TOKEN_EQUI: case GEN_TOKEN_MAP:
  case '+': case '-': case '*': case '/': case '%': case '>': case '<':
  case '&': case '|': case '^': case GNS_TOKEN_AND: case GEN_TOKEN_OR: case GEN_TOKEN_EG:
  case GEN_TOKEN_SUPEG: case GEN_TOKEN_INFEG: case GEN_TOKEN_NOTEG: case GEN_TOKEN_SHR: case GEN_TOKEN_SHL:
  case ';': case ',': case GEN_TOKEN_XOR: case GEN_TOKEN_MOD: case GEN_TOKEN_POW:
  case '=': case GEN_TOKEN_IF: case GEN_TOKEN_DO: case GEN_TOKEN_WHILE: case GEN_TOKEN_OF: case GEN_TOKEN_FOPEN:  case GEN_TOKEN_FUNCTION:
  case GEN_TOKEN_OTHER_TYPES:  case GEN_TOKEN_ARRAY:
    return SeekToken_Tree(tree->NEXT->NEXT,token)
    || SeekToken_Tree(tree->NEXT,token);
    break;
  /*unary class*/
  case GEN_TOKEN_OPPOSITE: case '~':
  case GEN_TOKEN_INC_BEFORE: case GEN_TOKEN_INC_AFTER: case GEN_TOKEN_DEC_BEFORE: case GEN_TOKEN_DEC_AFTER:
  case GEN_TOKEN_IN: case GEN_TOKEN_OUT: case GEN_TOKEN_INOUT: case GEN_TOKEN_TRANSCV: case GEN_TOKEN_TRISTATE: case GEN_TOKEN_LINKAGE:
  case GEN_TOKEN_NOT: case GEN_TOKEN_GENERIC: case GEN_TOKEN_PORT: 
  case GEN_TOKEN_EXCLUDE: 
  case GEN_TOKEN_VARIABLE: case GEN_TOKEN_SIGNAL: case GEN_TOKEN_INTEGER: case GEN_TOKEN_FCLOSE:
  case GEN_TOKEN_CHAR: case GEN_TOKEN_FILE: case GEN_TOKEN_REF: case GEN_TOKEN_REF_CHAR: case GEN_TOKEN_REF_FILE:
  case GEN_TOKEN_BLOCK: case GEN_TOKEN_EXIT: case GEN_TOKEN_STATIC: case GEN_TOKEN_VOIDTOKEN_D: case GEN_TOKEN_DOUBLE: case GEN_TOKEN_LONG:
  case GEN_TOKEN_REF_VOID: case GEN_TOKEN_REF_INT: case GEN_TOKEN_REF_DOUBLE: case GEN_TOKEN_ADR: 
    return SeekToken_Tree(tree->NEXT,token);
    break;
  /*terminal*/
  case GEN_TOKEN_STRING: 
  case GEN_TOKEN_STDOUT: case GEN_TOKEN_STDERR: case GEN_TOKEN_IDENT: case GEN_TOKEN_DIGIT: case GEN_TOKEN_NOP: case GEN_TOKEN_LOINS:
  case GEN_TOKEN_BREAK: case GEN_TOKEN_RETURN: case GEN_TOKEN_DIGIT_DOUBLE: case GEN_TOKEN_DIGIT_CHAR: case GEN_TOKEN_NULL:
    return 0;
    break;
  default:
    Error_Tree("SeekToken_Tree",tree);
    EXIT(2);
    return 0;
  }
}


SymInfoItem *AddSymInfoItem(SymInfoItem *head)
{
  SymInfoItem *tmp=(SymInfoItem *)AddHeapItem(&syminfo_heap);
  tmp->NEXT=head;
  return tmp;
}
