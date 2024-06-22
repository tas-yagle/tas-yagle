

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include MUT_H
#include BEH_H
#include LOG_H
#include BGL_H
#include AVT_H

#include "bgl_type.h"
#include "bgl_parse.h"
#include "bgl_util.h"
#include "bgl_error.h"

#define BGL_CONTEXT(x) ((bgl_getcontext(context))->x)

/*
  Calcul un signal (out) sur n bits effectuant une opération
  logique entre deux signaux (expr1 et expr2) de n bits

  oper=? AND, OR, NAND, NOR, ...

  PS: NAND et NOR sont générée avec NOT(AND) et NOT(OR)
*/
void bgl_NewOperation(bgl_expr *out, short oper, bgl_expr *expr1, bgl_expr *expr2)
{
  
  int i;
  chain_list *cl0,*cl1;
  out->TYPE = BGL_EXPR;

  if (expr1->WIDTH != expr2->WIDTH) 
  {
    avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"100");
//    printf("Width mismatch %d != %d\n",expr1->WIDTH,expr2->WIDTH);EXIT(9);
  }
  
  for (	i=0,out->LIST_ABL=NULL,cl0=expr1->LIST_ABL,cl1=expr2->LIST_ABL;
	i<expr1->WIDTH;
	i++,cl0=cl0->NEXT,cl1=cl1->NEXT)
    {
      if (oper==NAND)
	out->LIST_ABL = addchain(out->LIST_ABL, notExpr(createBinExpr(AND,cl0->DATA,cl1->DATA)));
      else 
      if (oper==NOR)
	out->LIST_ABL = addchain(out->LIST_ABL, notExpr(createBinExpr(OR,cl0->DATA,cl1->DATA)));
      else
	out->LIST_ABL = addchain(out->LIST_ABL, createBinExpr(oper,cl0->DATA,cl1->DATA));
    }
  out->WIDTH=expr1->WIDTH;
  out->IDENT=NULL;
  freechain(expr1->LIST_ABL); if (expr1->IDENT!=NULL) freechain(expr1->IDENT);
  freechain(expr2->LIST_ABL); if (expr2->IDENT!=NULL) freechain(expr2->IDENT);
#ifdef BGL_DEBUG
  displayExpr(out->LIST_ABL->DATA);
#endif
}

/*
  Ajoute une entrée de déclaration d'un signal
  à la liste des signaux du circuit
*/
bgldecl_list *bgl_addgen(bgl_bcompcontext *context, char *name)
{
    bgldecl_list   *l_pgen;

    l_pgen = (bgldecl_list *) mbkalloc(sizeof(struct bgldecl));

    if (!l_pgen) {
      avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"101");
//        fprintf(stderr, "not enough memory.\n");
//        EXIT(1);
    }

    l_pgen->NAME = name;
    l_pgen->NEXT = context->ALLSIGNALS;
    l_pgen->BIABL = NULL;
    l_pgen->BINODE = NULL;
    l_pgen->ABL = NULL;
    l_pgen->NODE = NULL;
    l_pgen->DIRECTION = 0;
    l_pgen->TYPE = 0;
    l_pgen->TIME = 0;
    l_pgen->OBJTYPE = 0;
    return l_pgen;
}

/*
  Ajoute un signal à la liste des signaux du circuit
  NB: Le signal est dévectorisé si c'est un vecteur
*/
int AddaNewSignalorSignals(bgl_bcompcontext *context, char *name, int left, int right, char dir)
{ 
  bgldecl_list   *whoiam;
  if (left==-1) 
    {
      if ((whoiam=(bgldecl_list *)beh_chktab(context->HSHTAB, name, context->FIGNAME, BGL_PNTDFN)) != (bgldecl_list *)0) 
	{
	  if ((whoiam->DIRECTION!='O' && dir=='R') || (whoiam->DIRECTION==' ' && dir==' '))
	    {
	      bgl_error(10, name, context);
	    }
	  else
	    return 0;
	}
#ifdef BGL_DEBUG
      printf("- decl '%c' %s\n",dir,name);
#endif
      context->ALLSIGNALS=bgl_addgen(context,name); // nouveau signal a driver
      beh_addtab(context->HSHTAB, name, context->FIGNAME, BGL_PNTDFN,(long) context->ALLSIGNALS);
      context->ALLSIGNALS->DIRECTION=dir;
    }
  else 
    {
      // signal vectorisé, on devectorise
      int i,inc=1;
      char extname[200];
      beh_addtab(context->HSHTAB, name, context->FIGNAME, BGL_PNTDFN, 1); // 1 = c'est un vecteur
      // mais a des bornes
      beh_addtab(context->HSHTAB, name, context->FIGNAME, BGL_LFTDFN, left);
      beh_addtab(context->HSHTAB, name, context->FIGNAME, BGL_RGTDFN, right);

      // creation des signaux non vectorisés
      if (left >= right)
	inc = -1;
      
      for (i = left; i != (right + inc); i += inc)
	{
	  sprintf(extname, "%s %d", name, i);
#ifdef BGL_DEBUG
	  printf("- decl [] '%c' %s\n",dir,extname);
#endif
	  // a driver
	  if ((whoiam=(bgldecl_list *)beh_chktab(context->HSHTAB, namealloc(extname), context->FIGNAME, BGL_PNTDFN)) != (bgldecl_list *)0) 
	    {
	      if ((whoiam->DIRECTION!='O' && dir=='R') || (whoiam->DIRECTION==' ' && dir==' '))
		{	      
		  bgl_error(10, extname, context);
		}
	    }
	  
	  context->ALLSIGNALS=bgl_addgen(context,namealloc(extname));
	  beh_addtab(context->HSHTAB, namealloc(extname), context->FIGNAME, BGL_PNTDFN,(long) context->ALLSIGNALS);
	  context->ALLSIGNALS->DIRECTION=dir;
	}
    }
  return 0;
}

/*
  Met à jour le flag d'un signal selon le context courant
  dans un IF, dans un ELSE, dans un BLOCK
  TAG: IS_BUS, IS_REG
*/
void setsignalflag(bgldecl_list   *sig, chain_list *pile_context)
{
  int contextflags;

  if (pile_context==NULL) return;

  contextflags=(int)((long)pile_context->DATA);

  if (contextflags & WE_ARE_IN_A_REGISTER_BLOCK)
    sig->OBJTYPE|=SIG_IS_REG;

  if ((contextflags & WE_ARE_IN_A_CONDITION_ELSE_STATEMENT) && !(contextflags & WE_ARE_IN_A_CONDITION_STATEMENT))
    {
      if (ATOM(sig->BIABL->CNDABL))
	{
	  if (strcmp(VALUE_ATOM(sig->BIABL->CNDABL), "'1'") == 0)
	    sig->OBJTYPE|=SIG_IS_BUS;
	  else
	      sig->OBJTYPE|=SIG_IS_REG;
	}
      sig->OBJTYPE|=SIG_IS_IN_ELSE;
    }

  if ((contextflags & WE_ARE_IN_A_CONDITION_STATEMENT))
    sig->OBJTYPE|=SIG_IS_BUS;
}

/*
  Inverse une liste de BIABL en mettant comme il faut
  les tags PRECEDE/NO_PRECEDE
*/
biabl_list *reverseAndreflag(biabl_list *bl0)
{
  biabl_list *bl;
#ifdef BGL_DEBUG
  for (bl=bl0;bl!=NULL;bl=bl->NEXT)
    {
      printf("%c",bl->FLAG & BEH_CND_PRECEDE?'P':'n');
    }
  printf("=>");
#endif
  for (bl=bl0;bl!=NULL;bl=bl->NEXT)
    {
      if (bl->FLAG & BEH_CND_PRECEDE)
	{
	  if (bl->USER!=NULL)
	    {
	      if (bl->CNDABL==NULL ||  (ATOM(bl->CNDABL) && strcasecmp((char *)bl->CNDABL, "'1'") != 0))
		{
		  bl->CNDABL=(chain_list *)bl->USER;
		}
	      else
		bl->CNDABL=createBinExpr(AND,bl->CNDABL,(chain_list *)bl->USER);
	      bl->USER=NULL;
	    }
	  bl=bl->NEXT;
	  while (bl->FLAG & BEH_CND_PRECEDE)
	    {
	      if (bl->USER!=NULL) printf("*");
	      bl=bl->NEXT;
	    }
	}
      else
	if (bl->USER!=NULL)
	  bl->CNDABL=(chain_list *)bl->USER;
      bl->USER=NULL;
    }

  bl0=(biabl_list *)reverse((chain_list *)bl0);
#ifdef BGL_DEBUG
  for (bl=bl0;bl!=NULL;bl=bl->NEXT)
    {
      printf("%c",bl->FLAG & BEH_CND_PRECEDE?'P':'n');
    }
  printf("\n");
  for (bl=bl0;bl!=NULL;bl=bl->NEXT)
    {
      printf("\t<= ");
      displayExpr(bl->VALABL);	
      if (bl->CNDABL!=NULL) 
	{
	  printf("\t\tsi ");
	  displayExpr(bl->CNDABL);	
	}
    }
  printf("\n");
#endif
  return bl0;
}

/*
  Génère les signaux d'une befig à partir de la déclaration
  des signaux
  les tags IS_BUS, IS_REG ainsi que la direction des signaux
  sont utilisés pour savoir dans quelles listes mettre les
  signaux
*/
void bgl_generateBEH(bgl_bcompcontext *context)
{
  bgldecl_list   *head;

  context->BEFIG->ERRFLG=context->ERRFLG;
//  printf("%s %d\n",context->BEFIG->NAME,context->BEFIG->ERRFLG);
  for (head=context->ALLSIGNALS;head!=NULL;head=head->NEXT)
    {

#ifdef BGL_DEBUG
      printf("---------> %s\n",head->NAME);
#endif
      if (head->DIRECTION=='O')
	{ 
	  //if (head->OBJTYPE & SIG_IS_BUS) head->OBJTYPE|=SIG_IS_REG;
	  if (head->OBJTYPE & SIG_IS_IN) head->DIRECTION='B';
	}

      if (!(head->OBJTYPE & SIG_IS_IN_ELSE) && (head->OBJTYPE & SIG_IS_BUS))
	head->OBJTYPE|=SIG_IS_REG;

      switch(head->DIRECTION)
	{
	case 'I':
	  context->BEFIG->BEPOR=beh_addbepor(context->BEFIG->BEPOR, head->NAME, 'I', 'B');
	  context->BEFIG->BERIN=beh_addberin(context->BEFIG->BERIN, head->NAME);
	  break;
	case 'O':
	  if (head->OBJTYPE & (SIG_IS_REG|SIG_IS_BUS))
	    {
	      context->BEFIG->BEPOR=beh_addbepor(context->BEFIG->BEPOR, head->NAME, 'Z', 'B');
	      if (head->OBJTYPE & SIG_IS_REG)
            context->BEFIG->BEREG=beh_addbereg(context->BEFIG->BEREG, head->NAME, reverseAndreflag(head->BIABL), head->BINODE,0);
	      else
            context->BEFIG->BEBUS=beh_addbebus(context->BEFIG->BEBUS, head->NAME, reverseAndreflag(head->BIABL), head->BINODE,'M',0);
	      if (head->BINODE==NULL)
            //EXIT(88);
            avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"102");
	    }
	  else
	    {
	      context->BEFIG->BEPOR=beh_addbepor(context->BEFIG->BEPOR, head->NAME, 'O', 'B');
	      if (head->BIABL!=NULL)
		{
		  context->BEFIG->BEOUT=beh_addbeout(context->BEFIG->BEOUT, head->NAME, head->BIABL->VALABL, NULL, 0);
		  context->BEFIG->BEOUT->TIME=head->BIABL->TIME;
		  mbkfree(head->BIABL);
		  mbkfree(head->BINODE);
		}
	    }
	  break;
	case 'B':
	  context->BEFIG->BERIN=beh_addberin(context->BEFIG->BERIN, head->NAME);
	  if (head->OBJTYPE & (SIG_IS_REG|SIG_IS_BUS))
	    {
	      context->BEFIG->BEPOR=beh_addbepor(context->BEFIG->BEPOR, head->NAME, 'T', 'B');
	      if (head->OBJTYPE & SIG_IS_REG)
		context->BEFIG->BEREG=beh_addbereg(context->BEFIG->BEREG, head->NAME, reverseAndreflag(head->BIABL), head->BINODE,0);
	      else
		context->BEFIG->BEBUS=beh_addbebus(context->BEFIG->BEBUS, head->NAME, reverseAndreflag(head->BIABL), head->BINODE,'M',0);
	      if (head->BINODE==NULL)
            // EXIT(88);
            avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"103");
	    }
	  else
	    {
	      context->BEFIG->BEPOR=beh_addbepor(context->BEFIG->BEPOR, head->NAME, 'B', 'B');
	      if (head->BIABL!=NULL)
		{
		  context->BEFIG->BEOUT=beh_addbeout(context->BEFIG->BEOUT, head->NAME, head->BIABL->VALABL, NULL, 0);
		  context->BEFIG->BEOUT->TIME=head->BIABL->TIME;
		  mbkfree(head->BIABL);
		  mbkfree(head->BINODE);
		}
	    }
	  break;
	case ' ':
	  if (head->OBJTYPE & SIG_IS_REG)
	    {
	      context->BEFIG->BERIN=beh_addberin(context->BEFIG->BERIN, head->NAME);
	      
	      context->BEFIG->BEREG=beh_addbereg(context->BEFIG->BEREG, head->NAME,reverseAndreflag(head->BIABL), head->BINODE,0);
//	  context->BEFIG->BERIN=beh_addberin(context->BEFIG->BERIN, head->NAME);
	    }
	  else
	    if (head->OBJTYPE & SIG_IS_BUS)
	      {
		context->BEFIG->BEBUX=beh_addbebux(context->BEFIG->BEBUX, head->NAME,reverseAndreflag(head->BIABL), head->BINODE,'M',0);
		context->BEFIG->BERIN=beh_addberin(context->BEFIG->BERIN, head->NAME);
	      }
	  else
	    {
	      if (head->BIABL!=NULL)
		{
		  context->BEFIG->BEAUX=beh_addbeaux(context->BEFIG->BEAUX, head->NAME,head->BIABL->VALABL, NULL, 0);
		  context->BEFIG->BEAUX->TIME=head->BIABL->TIME;
		  mbkfree(head->BIABL);
		  mbkfree(head->BINODE);
		}
	      if (BGL_AUXMOD)
		context->BEFIG->BERIN=beh_addberin(context->BEFIG->BERIN, head->NAME);
	    }
	  break;
	default:
      avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"104");
//	  EXIT(65);
	}
/*      if (head->OBJTYPE & SIG_IS_IN)
	context->BEFIG->BERIN=beh_addberin(context->BEFIG->BERIN, head->NAME);
*/
    }

  context->BEFIG->BERIN=(struct berin *)reverse((chain_list *)context->BEFIG->BERIN);
  context->BEFIG->BEOUT=(struct beout *)reverse((chain_list *)context->BEFIG->BEOUT);
  context->BEFIG->BEPOR=(struct bepor *)reverse((chain_list *)context->BEFIG->BEPOR);
  context->BEFIG->BEBUS=(struct bebus *)reverse((chain_list *)context->BEFIG->BEBUS);
  context->BEFIG->BEBUX=(struct bebux *)reverse((chain_list *)context->BEFIG->BEBUX);
  context->BEFIG->BEAUX=(struct beaux *)reverse((chain_list *)context->BEFIG->BEAUX);
  context->BEFIG->BEREG=(struct bereg *)reverse((chain_list *)context->BEFIG->BEREG);
}

/*
  Recherche l'occurence de x and not stable x
  Renvoie cette occurence
  Retourne 1 si une occurence a été trouvée
*/
int
bgl_search_stable(chain_list *ptabl, chain_list **found, int *nb)
{
    int             num = 0;
    chain_list *ptabl0=ptabl;

    if (ATOM(ptabl)) return 0;
    if (OPER(ptabl) == STABLE) {
      (*nb)++;
        return 1;
    }
    else
      while ((ptabl = CDR(ptabl)) != NULL)
	{
	  if ((num=bgl_search_stable(CAR(ptabl),found,nb))!=0) break;
	}
    if (num==1 && OPER(ptabl0) == NOT) return 2;
    if (num==2 && OPER(ptabl0) == AND)
      	    {
	      *found=ptabl0;
	    }

    return 0;
}

/*
  Calcul un signal (out) sur 1 bit ayant comme fonction
  logique un "op" entre les n bits d'un signal (in)

  op=? AND, OR, ...
*/
void UnaryBitwise(int op, bgl_expr *out, bgl_expr *in)
{
  chain_list *cl,*newabl,*tmp;
  int addnot=0;

  if (in->LIST_ABL==NULL || in->LIST_ABL->NEXT==NULL)
    avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"105");
    //EXIT(99);

  if (op==NAND) {addnot=1;op=AND;}
  else if (op==NOR) {addnot=1;op=OR;}

  tmp=createBinExpr(op,in->LIST_ABL->NEXT->DATA,in->LIST_ABL->DATA);
  if (addnot)
    newabl=notExpr(tmp);
  else
    newabl=tmp;
  
  for (cl=in->LIST_ABL->NEXT->NEXT;cl!=NULL;cl=cl->NEXT)
    {
      tmp=createBinExpr(op,cl->DATA,newabl);
      if (addnot)
	newabl=notExpr(tmp);
      else
	newabl=tmp;
    }
  out->WIDTH=1;	
  out->LIST_ABL = addchain(NULL, newabl);
  freechain(in->LIST_ABL);
  if (in->IDENT!=NULL) freechain(in->IDENT);
}

/*
  Calcul un signal (out) ayant comme fonction logique la comparaison
  entre deux signaux (a et b) de n bits

  Renvoie une valeur différente de 0 si la condition est toujours
  fausse ou toujours vraie

  op==0 => égalité
  op==1 => différence
*/
int bgl_compare(int op, bgl_expr *out, bgl_expr *a, bgl_expr *b)
{
  chain_list *cla, *clb;
  chain_list *ex=NULL, *tmp;
  int atype,btype;
  for (cla=(chain_list *)a->LIST_ABL,clb=(chain_list *)b->LIST_ABL;
       cla!=NULL;
       cla=cla->NEXT,clb=clb->NEXT)
    {
      atype=btype=-1;
      if (ATOM(((chain_list *)cla->DATA)))
	{
	  if (strcasecmp(VALUE_ATOM(((chain_list *)cla->DATA)), "'z'")==0) return -1;
	  if (strcmp(VALUE_ATOM(((chain_list *)cla->DATA)), "'1'")==0) atype=1;
	  else if (strcmp(VALUE_ATOM(((chain_list *)cla->DATA)), "'0'")==0) atype=0;
	}
      if (ATOM(((chain_list *)clb->DATA)))
	{
	  if (strcasecmp(VALUE_ATOM(((chain_list *)clb->DATA)), "'z'")==0) return -1;
	  if (strcmp(VALUE_ATOM(((chain_list *)clb->DATA)), "'1'")==0) btype=1;
	  else if (strcmp(VALUE_ATOM(((chain_list *)clb->DATA)), "'0'")==0) btype=0;
	}


      if ((atype!=btype && atype!=-1 && btype!=-1 && op==0) || (atype!=btype && atype!=-1 && btype!=-1 && op==1))
	{
	  if (op==0) return -2; // condition always false
	  else return -3;
	}
      else
	{
	  if (!((atype==btype && atype!=-1 && op==0) || (atype==btype && atype!=-1 && op==1)))
	    {
	      if (atype==-1 && btype!=-1)
		{
		  if (btype==0)
		    {
		      if (op==0) tmp=notExpr(cla->DATA); // égalité a 0
		      else tmp=cla->DATA; // inégalité a 0
		    }
		  else
		    {
		      if (op==1) tmp=notExpr(cla->DATA); // inégalité a 0
		      else tmp=cla->DATA; // égalité a 1
		    }
		}
	      else
		if (atype!=-1 && btype==-1)
		  {
		    if (atype==0)
		      {
			if (op==0) tmp=notExpr(clb->DATA); // égalité a 0
			else tmp=clb->DATA; // inégalité a 0
		      }
		    else
		      {
			if (op==1) tmp=notExpr(clb->DATA); // inégalité a 0
			else tmp=clb->DATA; // égalité a 1
		      }
		  }
		else
		  {
		    if (op==0) 
		      {
			chain_list *ab,*ba;// égalité
			ab=createBinExpr(OR,notExpr(cla->DATA),copyExpr(clb->DATA)); 
			ba=createBinExpr(OR,notExpr(clb->DATA),copyExpr(cla->DATA)); 
			tmp=createBinExpr(AND,ab,ba); // NXOR
		      }
		    else 
		      {
			chain_list *ab,*ba;// inégalité
			ab=createBinExpr(AND,notExpr(cla->DATA),copyExpr(clb->DATA)); 
			ba=createBinExpr(AND,notExpr(clb->DATA),copyExpr(cla->DATA)); 
			tmp=createBinExpr(OR,ab,ba); // XOR
		      }
		  }
	      
	      if (ex!=NULL)
		{
		  if (op==0)
		    ex=createBinExpr(AND,ex,tmp);
		  else
		    ex=createBinExpr(OR,ex,tmp);
		}
	      else
		ex=tmp;
	    }
	}
    }
  if (ex==NULL)
    return -3; // condition always true
  out->LIST_ABL=addchain(NULL,ex);
  out->IDENT=NULL;
  out->WIDTH=1;
  return 0;
}

/*
  Calcul un signal (out) sur 1 bits effectuant une opération
  logique entre les n bits d'un signal (expr1)

  oper==0 => NOT
  oper!=0 => BUFFER
*/
void bgl_NewSimpleOperation(bgl_expr *out, short oper, bgl_expr *expr1) // NOT or -1=buff
{
  
  int i;
  chain_list *cl0;
  out->TYPE = BGL_EXPR;
  
  if (oper==-1)
    {
      memcpy(out,expr1,sizeof(bgl_expr));
      return;
    }

  for (	i=0,out->LIST_ABL=NULL,cl0=expr1->LIST_ABL;
	i<expr1->WIDTH;
	i++,cl0=cl0->NEXT)
    {
      out->LIST_ABL = addchain(out->LIST_ABL, notExpr(cl0->DATA));
    }
#ifdef BGL_DEBUG
  displayExpr(out->LIST_ABL->DATA);
#endif
  out->WIDTH=expr1->WIDTH;
  freechain(expr1->LIST_ABL);
}

int CheckSize(chain_list *cl, int skeepfirst)
{
  int curwidth=-1;

  if (skeepfirst && cl->NEXT!=NULL)
    cl=cl->NEXT;

  for (; cl != NULL; cl=cl->NEXT) 
    {
//      printf("%s %d,\n",((bgl_expr *)cl->DATA)->IDENT==NULL?"NULL":((bgl_expr *)cl->DATA)->IDENT->DATA,((bgl_expr *)cl->DATA)->WIDTH);
      if (curwidth==-1) curwidth=((bgl_expr *)cl->DATA)->WIDTH;
      else if (curwidth!=((bgl_expr *)cl->DATA)->WIDTH)
	return 1;
    }
  return 0;
}

/*
  Substitue les ATOM d'un ABL par un ABL lui correspondant
  La correspondance se trouve dans le tableau nouveau
  
  Renvoie l'ABL modifié
*/
chain_list *bgl_substPhyExpr (chain_list *expr1, chain_list **nouveau,int max,int *most)
{
  int num;
  chain_list *save=expr1;
  if (!ATOM (expr1))
    {
      while ((expr1 = CDR (expr1)))
        if (ATOM (CAR (expr1)))
          {
	    if (!(((char *)VALUE_ATOM(CAR(expr1)))[0]>='0' && ((char *)VALUE_ATOM(CAR(expr1)))[0]<='9'))
          avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"106");
          //EXIT(99);
	    num=atoi(VALUE_ATOM(CAR(expr1)));
	    if (num>*most) *most=num;

	    if (num>max) return NULL;

	    freechain (CAR (expr1));
	    expr1->DATA = copyExpr(nouveau[num]);
          }
        else
          if (bgl_substPhyExpr (CAR (expr1), nouveau, max,most)==NULL) return NULL;
    }
  else
    {
      if (!(((char *)VALUE_ATOM(expr1))[0]>='0' && ((char *)VALUE_ATOM(expr1))[0]<='9'))
        avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"107");
        //EXIT(99);
      num=atoi(VALUE_ATOM(expr1));
      if (num>*most) *most=num;

      if (num>max) return NULL;
      freechain (expr1);
      return copyExpr(nouveau[num]);
    }
  return save;
}

/*
  Soit une double liste chainée T(a,b)

  Calcule une liste chainée contenant les éléments T(*,num)
  renvoie 1 si il n'y a pas d'élément T(*,num)
*/
int bgl_getline(chain_list **tab,int num, chain_list *terms,int max)
{
  chain_list *cl;
  int count,count0;

#ifndef __ALL__WARNING__
  // prevent warning ..anto..
  max = 0;
#endif
  
  for (count0=0;terms!=NULL;terms=terms->NEXT,count0++)
    {
      for (cl=((bgl_expr *)terms->DATA)->LIST_ABL,count=0;cl!=NULL && count<num;count++,cl=cl->NEXT) ;
      if (cl==NULL) return 1;
//      tab[max-count0-1]=(chain_list *)cl->DATA;
      tab[count0]=(chain_list *)cl->DATA;
    }
  return 0;
}

/*
  Retourne le nom d'un signal a la position (num)
  dans une chain_list
*/
char *bgl_getlvalue(int num, chain_list *terms)
{
  chain_list *cl;
  int count;
  for (;terms!=NULL;terms=terms->NEXT)
    {
      if (terms->NEXT==NULL)
	{
	  if (((bgl_expr *)terms->DATA)->TYPE!=BGL_IDENT)
	    return NULL;
	  for (cl=((bgl_expr *)terms->DATA)->IDENT,count=0;cl!=NULL && count<num;count++,cl=cl->NEXT) ;

	  if (cl==NULL) return NULL;
	  return (char *)cl->DATA;
	}
    }
  return NULL;
}

/*
  Expanse toutes les primites dans toutes les befig présentes

  allcont => toutes les befig avec leur context
  allprims => toutes les primitives
*/
void AddPrimitiveToBEFIG(chain_list *allprims, chain_list *allcont)
{
  chain_list *ins,*prim,*nouveau[1000];
  custom_gate_instance *instance;
  bgl_bcompcontext *context;
  int most,max,count;
  bgldecl_list *sig,*sig1;
  char *lvalue;
  chain_list *copy;
  custom_gate *cg;
  int blockdone;
  
  for (;allcont!=NULL;allcont=allcont->NEXT)
    {
      context=(bgl_bcompcontext *)allcont->DATA;
#ifdef BGL_DEBUG
      printf("--MODULE--> %s <-----\n",context->BEFIG->NAME);
#endif
      for (ins=context->CUSTOM_GATE_INSTANCES;ins!=NULL;ins=ins->NEXT)
	{
	  instance=(custom_gate_instance *)ins->DATA;

	  for (prim=allprims;prim!=NULL && ((custom_gate *)prim->DATA)->name!=instance->gate_name;prim=prim->NEXT)
	  
	  if (prim==NULL)
        avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"107");
        //EXIT(15);
	  cg=(custom_gate *)prim->DATA;
	  max=countchain(instance->terminals);

	  count=0;
	  while (bgl_getline(nouveau,count, instance->terminals,max)==0)
	    {
	      lvalue=bgl_getlvalue(count, instance->terminals);
	      if (lvalue==NULL)
            //  EXIT(25);
            avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"108");
	      
	      sig=(bgldecl_list *)beh_chktab(context->HSHTAB, lvalue, context->FIGNAME, BGL_PNTDFN);

#ifdef BGL_DEBUG
	      printf("to %s\n",sig->NAME);
#endif
	      
	      if (cg->zui->cond_sig==-1)
		{
		  // expression sans condition
		  copy=copyExpr(cg->zui->abl1);
		  most=0;
		  if ((copy=bgl_substPhyExpr(copy, nouveau, max, &most))==NULL || most>max)
		    	 { bgl_error(91, "Bad number of arguments in primitive", context); /*EXIT (1);*/ }
		  sig->BIABL=   beh_addbiabl(	sig->BIABL,sig->NAME,NULL,copy);
		  sig->BINODE=beh_addbinode(sig->BINODE,NULL,NULL); sig->BINODE->TIME=0;

		  sig->BIABL->FLAG |= BEH_CND_NOPRECEDE;
		  sig->BIABL->TIME=0; // ???
		}
	      else
		{
		  // expression avec condition
		  int prems=1;
		  char intermname[500];
		  chain_list *block_cmd=NULL;
		  
		  blockdone=0;
		  sprintf(intermname,"next_%s",sig->NAME);
//		  nouveau[max-1]=createAtom(namealloc(intermname));
		  AddaNewSignalorSignals(context, namealloc(intermname), -1, -1, ' ');

		  sig1=(bgldecl_list *)beh_chktab(context->HSHTAB, namealloc(intermname) , context->FIGNAME, BGL_PNTDFN);

		  if (cg->zui->abl1!=NULL)
		    {
		      if (block_cmd==NULL) block_cmd=copyExpr(cg->zui->abl1);

		      copy=copyExpr(cg->zui->abl1);
		      most=0;
		      if ((copy=bgl_substPhyExpr(copy, nouveau, max, &most))==NULL || most>max)
			{ bgl_error(91, "Bad number of arguments in primitive", context); /*EXIT (1);*/ }
		       sig1->BIABL=   beh_addbiabl(sig1->BIABL,sig->NAME,copy,createAtom("'1'"));
		       sig1->BINODE=beh_addbinode(sig1->BINODE,NULL,NULL); sig1->BINODE->TIME=0;

		       sig1->BIABL->FLAG |= BEH_CND_NOPRECEDE;
		       sig1->BIABL->TIME=0;
		       prems=0;
		    }

		  if (cg->zui->abl0!=NULL) 
		    {
		      if (block_cmd==NULL) block_cmd=copyExpr(cg->zui->abl0);
		      else block_cmd=createBinExpr(OR,block_cmd,copyExpr(cg->zui->abl0));

		      copy=copyExpr(cg->zui->abl0);
		      most=0;
		      if ((copy=bgl_substPhyExpr(copy, nouveau, max, &most))==NULL || most>max)
			{ bgl_error(91, "Bad number of arguments in primitive", context); /*EXIT (1);*/ }
		       sig1->BIABL=   beh_addbiabl(sig1->BIABL,sig->NAME,copy,createAtom("'0'"));
		       sig1->BINODE=beh_addbinode(sig1->BINODE,NULL,NULL); sig1->BINODE->TIME=0;
		       if (prems) sig1->BIABL->FLAG |= BEH_CND_NOPRECEDE;
		       else sig1->BIABL->FLAG |= BEH_CND_PRECEDE;
		       sig1->BIABL->TIME=0;
		       prems=0;
		    }

		  if (cg->zui->abl_cond!=NULL) 
		    {
		      char *cond_sig;
		      chain_list *ch1,*ch2;

		      sprintf(intermname,"%d",cg->zui->cond_sig);
		      cond_sig=namealloc(intermname);

		      ch2 = (struct chain *)mbkalloc(sizeof(struct chain));
        	      ch1 = createExpr(STABLE);
        	      ch1->NEXT = ch2;
		      ch2->DATA = createAtom(cond_sig);
		      ch2->NEXT = NULL;
		      
		      context->BEFIG->TYPE|=BEH_STABLE;

		      if (cg->zui->xedge=='u')
			ch2=createBinExpr(AND, notExpr(ch1), createAtom(cond_sig));
		      else
			ch2=createBinExpr(AND, notExpr(ch1), notExpr(createAtom(cond_sig)));

		      if (block_cmd==NULL) block_cmd=ch2;
		      else block_cmd=createBinExpr(OR,block_cmd,ch2);

		      copy=copyExpr(cg->zui->abl_cond);
		      most=0;
		      if ((copy=bgl_substPhyExpr(copy, nouveau, max, &most))==NULL || most>max)
			{ bgl_error(91, "Bad number of arguments in primitive", context); /*EXIT (1);*/ }

		      most=0;
		      if ((ch2=bgl_substPhyExpr(ch2, nouveau, max, &most))==NULL || most>max)
			{ bgl_error(91, "Bad number of arguments in primitive", context); /*EXIT (1);*/ }

		      sig1->BIABL=beh_addbiabl(sig1->BIABL,sig->NAME,ch2,copy);
		      sig1->BINODE=beh_addbinode(sig1->BINODE,NULL,NULL); sig1->BINODE->TIME=0;
		      if (prems) sig1->BIABL->FLAG |= BEH_CND_NOPRECEDE;
		      else sig1->BIABL->FLAG |= BEH_CND_PRECEDE;

		      sig1->BIABL->TIME=0;

		      prems=0;
		    }

		  sig1->OBJTYPE|=SIG_IS_REG;

		  sig->BIABL=   beh_addbiabl(sig->BIABL,sig->NAME,NULL,createAtom(sig1->NAME));
		  sig->BINODE=  beh_addbinode(sig->BINODE,NULL,NULL); sig->BINODE->TIME=0;
		  sig->BIABL->FLAG |= BEH_CND_NOPRECEDE;
		  sig->BIABL->TIME=0;
		  sig->OBJTYPE|=SIG_IS_IN;

		}
	      count++;
	    }
	}
      bgl_generateBEH(context);
      context->BEFIG->ERRFLG=0;
      beh_fretab(context->HSHTAB);
      for (sig=context->ALLSIGNALS;sig!=NULL;sig=sig1)
	{
	  sig1=sig->NEXT;
	  mbkfree(sig);
	}
      mbkfree(context);
    }
}

/*
  Effectue : out(1..n) <= in(1..n)
*/
void SimpleAffect(chain_list *out, bgl_expr *in, int delay, bgl_bcompcontext *context)
{
  int width;
  chain_list *ch,*ch0;
  bgldecl_list *sig;
  
  for (width=0,ch=out;ch!=NULL;width++,ch=ch->NEXT) ;
  if (width!=in->WIDTH)
    { bgl_error(38, "", bgl_getcontext(context)); /*EXIT (1);*/ }
  
  for (ch=out,ch0=in->LIST_ABL;ch!=NULL;ch=ch->NEXT,ch0=ch0->NEXT)
    {
      sig=(bgldecl_list *)ch->DATA;
#ifdef BGL_DEBUG
      printf("to %s after %d\n",sig->NAME,delay);
#endif
      sig->BIABL=
	beh_addbiabl(	sig->BIABL,
			sig->NAME,
			BGL_CONTEXT(PILE_CONDITION)==NULL?NULL:copyExpr(BGL_CONTEXT(PILE_CONDITION)->DATA),
			ch0->DATA
			);

      sig->BINODE=beh_addbinode(sig->BINODE,NULL,NULL); sig->BINODE->TIME=delay;

      if (BGL_CONTEXT(PILE_CONDITION)!=NULL && (BGL_CONTEXT(PILE_CONDITION)->NEXT!=NULL || ((int)((long)BGL_CONTEXT(WHERE_ARE_WE)->DATA) & WE_ARE_IN_A_CONDITION_ELSE_STATEMENT)))
	sig->BIABL->FLAG |= BEH_CND_PRECEDE;
      else
	sig->BIABL->FLAG |= BEH_CND_NOPRECEDE;
      sig->BIABL->TIME=delay;
      if (BGL_CONTEXT(CURRENT_EDGE_ABL)!=NULL)
	sig->BIABL->USER=(ptype_list *)copyExpr(BGL_CONTEXT(CURRENT_EDGE_ABL));
      else
	sig->BIABL->USER=NULL;
      setsignalflag(sig,BGL_CONTEXT(WHERE_ARE_WE));
    }
  freechain(in->LIST_ABL);
  if (in->IDENT!=NULL) freechain(in->IDENT);
  freechain(out);
}

/*
  Effectue : out(1..n) <= cond ? sivrai(1..n) : sifaux(1..n)
*/
void DirectIFAffect(chain_list *out, bgl_expr *cond, bgl_expr *sivrai, bgl_expr *sifaux, int delay, bgl_bcompcontext *context)
{
  int width;
  chain_list *ch,*ch0,*ch1;
  bgldecl_list *sig;
  
  for (width=0,ch=out;ch!=NULL;width++,ch=ch->NEXT) ;
  if (width!=sivrai->WIDTH || width!=sifaux->WIDTH || cond->WIDTH!=1)
    { bgl_error(38, "", bgl_getcontext(context)); /*EXIT (1);*/ }

  if (BGL_CONTEXT(PILE_CONDITION)!=NULL)
    { bgl_error(91, "Can't handle mix of IF and val=(cond)?v0:v1\n", bgl_getcontext(context)); /*EXIT (1);*/ }
  
  for (ch=out,ch0=sivrai->LIST_ABL,ch1=sifaux->LIST_ABL;ch!=NULL;ch=ch->NEXT,ch0=ch0->NEXT,ch1=ch1->NEXT)
    {
      sig=(bgldecl_list *)ch->DATA;

#ifdef BGL_DEBUG
      printf("to %s\n",sig->NAME);
#endif
      sig->OBJTYPE|=SIG_IS_BUS;
      sig->BIABL=
	beh_addbiabl(	sig->BIABL,
			sig->NAME,
			copyExpr(cond->LIST_ABL->DATA),
			ch0->DATA
			);

      sig->BINODE=beh_addbinode(sig->BINODE,NULL,NULL); sig->BINODE->TIME=delay;

      sig->BIABL->FLAG |= BEH_CND_NOPRECEDE;
      sig->BIABL->TIME=delay;
      if (BGL_CONTEXT(CURRENT_EDGE_ABL)!=NULL)
	sig->BIABL->USER=(ptype_list *)copyExpr(BGL_CONTEXT(CURRENT_EDGE_ABL));
      else
	sig->BIABL->USER=NULL;


      if (!(ATOM(((chain_list *)ch1->DATA)) && strcasecmp(VALUE_ATOM(((chain_list *)ch1->DATA)), "'z'")==0))
	{
	  sig->BIABL=
	    beh_addbiabl(	sig->BIABL,
				sig->NAME,
				copyExpr(cond->LIST_ABL->DATA),
				ch1->DATA
				);
	  sig->BIABL->FLAG |= BEH_CND_PRECEDE;
	  sig->BIABL->TIME=delay;
	  if (BGL_CONTEXT(CURRENT_EDGE_ABL)!=NULL)
	    sig->BIABL->USER=(ptype_list *)copyExpr(BGL_CONTEXT(CURRENT_EDGE_ABL));
	  else
	    sig->BIABL->USER=NULL;
	}
    }
  freeExpr(cond->LIST_ABL->DATA);
  freechain(cond->LIST_ABL);
  if (cond->IDENT!=NULL) freechain(cond->IDENT);
  freechain(sivrai->LIST_ABL);
  if (sivrai->IDENT!=NULL) freechain(sivrai->IDENT);
  freechain(sifaux->LIST_ABL);
  if (sifaux->IDENT!=NULL) freechain(sifaux->IDENT);
  freechain(out);
}
