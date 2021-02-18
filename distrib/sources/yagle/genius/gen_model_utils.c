/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_utils.c                                           */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 09/02/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include MUT_H
#include MLO_H
#include RCN_H
#include MLU_H
#include CNS_H
#include API_H
#include "gen_env.h"
#include "gen_MBK_utils.h"
#include "gen_model_utils.h"
#include "gen_corresp.h"
#include "gen_search_utils.h"

char GEN_SEPAR = '.';


chain_list *blocks_to_free=NULL;

//#define NOHEAPALLOC
/*head unreachable by foreigners like for lofig in alliance*/
model_list *HEAD_MODEL=NULL;                /*list of all models*/
/*to accelerate memory allocation*/
mark_list *HEAD_MARK=NULL;                   /*list of allocated marks*/

void InitModels()
{
  HEAD_MODEL=NULL;   
  HEAD_MARK=NULL;
}

void free_all_blocks_to_free()
{
  chain_list *cl;
  for (cl=blocks_to_free;cl!=NULL;cl=cl->NEXT) mbkfree(cl->DATA);
  freechain(blocks_to_free);
  blocks_to_free=NULL;
}

/****************************************************************************/
/*               put a new elem in DATA field of head                       */
/****************************************************************************/
extern mark_list* addmark(ptype_list *head, int numbit, losig_list *losig, int visited, locon_list *me)
{
   mark_list *pt;
#ifndef NOHEAPALLOC
   register int i;
   if (HEAD_MARK == NULL) {
      pt = (mark_list *)mbkalloc(BUFSIZE0*sizeof(mark_list));
      blocks_to_free=addchain(blocks_to_free,pt);
      HEAD_MARK = pt;
      for (i = 1; i < BUFSIZE0; i++) {
         pt->NEXT = pt + 1;
         pt++;
      }
      pt->NEXT = NULL;
   }

   pt = HEAD_MARK;
   HEAD_MARK = HEAD_MARK->NEXT;
#else
   pt = (mark_list *)mbkalloc(sizeof(mark_list));
#endif
   /*change fields*/
   pt->LOSIG=losig;
   pt->BIT=numbit;
   pt->ROOT=head;
   pt->VISITED=visited;
   pt->COUNT=1;
   pt->one_locon=me;
   pt->NEXT=head->DATA;
   head->DATA=pt;
   return pt;
}

/****************************************************************************/
/*              erase recursively from memory a list of marks               */
/****************************************************************************/
extern void freemark(mark_list *top)
{
#ifndef NOHEAPALLOC
  HEAD_MARK = (mark_list*)append((chain_list*)top, (chain_list*)HEAD_MARK);
#else
  mark_list *l;
  while (top!=NULL)
    {
      l=top->NEXT;
      mbkfree(top);
      top=l;
    }
#endif
}


/***************************************************************************/
/*        add to USER field of locon the generic signals linked to it      */
/* gen is top of list, vectcon is bit number of connector and vectsig bit  */
/* number of losig                                                         */
/***************************************************************************/
extern genconchain_list* addgenconchain(genconchain_list *gen, 
                                        tree_list *vectcon,
                                        losig_list *losig, tree_list *vectsig)
{

   genconchain_list *new;
   if (!losig) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 25);
      //fprintf(stderr,"addgenconchain: NULL pointer\n");
      EXIT(1);
   }
   new=(genconchain_list*)mbkalloc(sizeof(genconchain_list));
   new->NEXT=gen;
   new->LOSIG=losig;
   new->LOCON_EXP=vectcon;
   new->LOSIG_EXP=vectsig;
   return new;
}   


/***************************************************************************/
/*              erase recursively a list of gencon                         */
/***************************************************************************/
extern void freegenconchain(genconchain_list *gencon) {
   if (!gencon) return;
   else freegenconchain(gencon->NEXT);
   Free_Tree(gencon->LOSIG_EXP);
   Free_Tree(gencon->LOCON_EXP);
   mbkfree(gencon);
}   


extern char *ccname(locon_list *);
/***************************************************************************/
/*     add to USER field of losig the generic connectors linked to it      */
/* gen is top of list, vectsig is bit number of signal and vectcon bit     */
/* number of locon                                                         */
/***************************************************************************/
extern gensigchain_list* addgensigchain(gensigchain_list *gen, 
                                        tree_list *vectsig,
                                        locon_list *locon, tree_list *vectcon)
{

   gensigchain_list *new;
   if (!locon) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 26);
//      fprintf(stderr,"addgensigchain: NULL pointer\n");
      EXIT(1);
   }
   new=(gensigchain_list*)mbkalloc(sizeof(gensigchain_list));
   new->NEXT=gen;
   new->LOCON=locon;
   new->LOCON_EXP=vectcon;
   new->LOSIG_EXP=vectsig;
   new->ghost=0; // care about this connexion

   return new;
}


/***************************************************************************/
/*              erase recursively a list of gensig                         */
/***************************************************************************/
extern void freegensigchain(gensigchain_list *gensig) {
   if (!gensig) return;
   else freegensigchain(gensig->NEXT);
   Free_Tree(gensig->LOSIG_EXP);
   Free_Tree(gensig->LOCON_EXP);
   mbkfree(gensig);
}   


/***************************************************************************/
/*                            erase a generic ABL                          */
/***************************************************************************/
static inline void Free_Generic(tree_list *gen)
{
  if (!gen) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 27);
    //fprintf(stderr,"Free_Generic: NULL pointer\n");
    EXIT(1);
  }
   switch(TOKEN(gen)) {
   /*node*/
   case GEN_TOKEN_NODE:
      Free_Generic(gen->DATA);
      break;
   /*unary*/
   case GEN_TOKEN_OPPOSITE: case GEN_TOKEN_NOT: case GEN_TOKEN_EXCLUDE: 
      Free_Generic(gen->NEXT);
      freetree(gen);
      break;
   /*binary*/
   case '+': case '-': case '*': case '/': case '%': case '>': case '<':
   case GEN_TOKEN_MOD: case GEN_TOKEN_POW:
   case GNS_TOKEN_AND: case GEN_TOKEN_XOR: 
   case GEN_TOKEN_OR: case GEN_TOKEN_EG:
   case GEN_TOKEN_SUPEG: case GEN_TOKEN_INFEG: case GEN_TOKEN_NOTEG: case ';': case GEN_TOKEN_IF:
      Free_Generic(gen->NEXT->NEXT);
      Free_Generic(gen->NEXT);
      freetree(gen);
      break;
   /*4 branches*/
   case GEN_TOKEN_FOR:
      Free_Generic(gen->NEXT->NEXT->NEXT->NEXT);
      Free_Generic(gen->NEXT->NEXT->NEXT);
      Free_Generic(gen->NEXT->NEXT);
      Free_Generic(gen->NEXT);
      freetree(gen);
      break;
   case GEN_TOKEN_IDENT: case GEN_TOKEN_DIGIT: case GEN_TOKEN_LOINS:/*pointer on LOINS in LOFIG*/
      freetree(gen);
      break;
   default:
      Error_Tree("Free_Generic",gen);
      EXIT(1);
   }
}


/***************************************************************************/
/*                            dump a generic ABL                           */
/***************************************************************************/
extern void Dump_Generic(tree_list *gen)
{
  if (!gen) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 28);
    //fprintf(stderr,"Dump_Generic: NULL pointer\n");
    EXIT(1);
  }
   switch(TOKEN(gen)) {
   case GEN_TOKEN_NODE:
      Dump_Generic(gen->DATA);
      break;
/*root*/
   case GEN_TOKEN_FOR:
      fprintf(GENIUS_OUTPUT,"   |---FOR (");
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT," IN ");
      Dump_Generic(gen->NEXT->NEXT);
      fprintf(GENIUS_OUTPUT," TO ");
      Dump_Generic(gen->NEXT->NEXT->NEXT);
      fprintf(GENIUS_OUTPUT,")\n");
      Dump_Generic(gen->NEXT->NEXT->NEXT->NEXT);
      break; 
   case GEN_TOKEN_IF:
      fprintf(GENIUS_OUTPUT,"   |---IF (");
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT,")\n");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_EXCLUDE: 
      fprintf(GENIUS_OUTPUT,"   |---EXCLUDE\n");
      Dump_Generic(gen->NEXT);
      break; 
/*leaf*/
   case GEN_TOKEN_LOINS:/*pointer on a loins in lofig*/
      Dump_loins((loins_list*)gen->DATA);
      break;
/*operators*/
   case ';':
      Dump_Generic(gen->NEXT);
      Dump_Generic(gen->NEXT->NEXT);
      break;      
   case '+': case '-': case '*': case '/': case '>': case '<': case ',':
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT,"%c",TOKEN(gen));
      Dump_Generic(gen->NEXT->NEXT);
      break;     
   case GEN_TOKEN_TO:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT," to ");
      Dump_Generic(gen->NEXT->NEXT);
      break;        
   case GEN_TOKEN_DOWNTO:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT," downto ");
      Dump_Generic(gen->NEXT->NEXT);
      break;        
   case '%':
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT," %s ","REM");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_MOD:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT," %s ","MOD");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_POW:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT,"%s","**");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GNS_TOKEN_AND:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT," %s ","&&");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_OR:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT," %s ","||");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_XOR:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT," %s ","XOR");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_NOT:
      fprintf(GENIUS_OUTPUT,"%s(","NOT");
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT,")");
      break; 
   case GEN_TOKEN_EG:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT,"%s","==");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_SUPEG:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT,"%s",">=");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_INFEG:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT,"%s","<=");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_NOTEG:
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT,"%s","!=");
      Dump_Generic(gen->NEXT->NEXT);
      break; 
   case GEN_TOKEN_OPPOSITE: 
      fprintf(GENIUS_OUTPUT,"(%s","-");
      Dump_Generic(gen->NEXT);
      fprintf(GENIUS_OUTPUT,")");
      break; 
   case GEN_TOKEN_IDENT: 
      fprintf(GENIUS_OUTPUT,"%s",(char*)gen->DATA);
      break;
   case GEN_TOKEN_DIGIT: 
      fprintf(GENIUS_OUTPUT,"%d",(int)(long)gen->DATA);
      break;
   default:
      Error_Tree("Dump_Generic",gen);
      EXIT(1);
   }
}


/***************************************************************************/
/*                            dump a generic model                         */
/***************************************************************************/
extern void Dump_Model(model_list *model)
{
locon_list *ptcon;
losig_list *ptsig;
lotrs_list *pttrs;
chain_list *scanchain;
ptype_list *pt;
lofig_list *ptfig=model->LOFIG;

	if (ptfig->NAME == NULL)
		(void)fprintf(GENIUS_OUTPUT,"!!! figure without name !!! /n");
	else
		(void)fprintf(GENIUS_OUTPUT,"\nfigure   : %-20s  mode : %c\n", ptfig->NAME, ptfig->MODE);

	if (ptfig->LOCON == NULL) 
		(void)fprintf(GENIUS_OUTPUT,"   |---empty connector list\n");   
	else 
        {
          for (ptcon = ptfig->LOCON; ptcon; ptcon = ptcon->NEXT)
	    Dump_lofigcon(ptcon);

          pt = getptype( ptfig->USER, PH_INTERF );
          if( pt )
          {
            (void)fprintf(GENIUS_OUTPUT,"   |---connector list physical order\n");   
            for( scanchain = (chain_list*)(pt->DATA); scanchain ; scanchain = scanchain->NEXT )
              (void)fprintf(GENIUS_OUTPUT,"   |    |---%s\n",(char*)(scanchain->DATA) );   
            (void)fprintf(GENIUS_OUTPUT,"   |\n");   
          }
        }

	if (ptfig->LOSIG == NULL)
		(void)fprintf(GENIUS_OUTPUT,"   |---empty signal list\n");
	else for (ptsig = ptfig->LOSIG; ptsig; ptsig = ptsig->NEXT)
		Dump_losig(ptsig);

	if (ptfig->LOINS == NULL)
		(void)fprintf(GENIUS_OUTPUT,"   |---empty instance list\n");
	else Dump_Generic(model->GENERIC);
	if (ptfig->LOTRS == NULL)
		(void)fprintf(GENIUS_OUTPUT,"   |---empty transistor list\n");
	else for (pttrs = ptfig->LOTRS; pttrs; pttrs = pttrs->NEXT)
		Dump_lotrs(pttrs);

	if (ptfig->USER != NULL)
        {
		(void)fprintf(GENIUS_OUTPUT,"   |---USER field\n");
                for( pt = ptfig->USER ; pt ; pt = pt->NEXT )
        		(void)fprintf(GENIUS_OUTPUT,"   |    |-- %ld\n",pt->TYPE);
        }
        else
		(void)fprintf(GENIUS_OUTPUT,"   |---empty USER field\n");

	(void)fprintf(GENIUS_OUTPUT,"   | \n");
}   


/***************************************************************************/
/*        return the model named name, NULL is returned if not found       */
/***************************************************************************/
extern model_list *getmodel(name)
  char *name;
{
   model_list *m;
   for (m=HEAD_MODEL;m;m=m->NEXT)
      if (m->NAME==name) return m;
   return NULL;
}


/***************************************************************************/
/*add to the list of model the model named fig->NAME with fig,generic and c*/
/***************************************************************************/
extern void addmodel(lofig_list *fig, chain_list *variables, tree_list *generic, char *c, char *regexp)
{
   model_list *m;
   
   if (fig==NULL) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 29);
      //fprintf(stderr,"addmodel: NULL pointer\n");
      EXIT(1);
   }
   m=(model_list*) mbkalloc(sizeof(model_list));
   m->NAME=fig->NAME;
   m->REGEXP=regexp;
   m->LOFIG=fig;
   m->VAR=variables;   /*list of generic variables for the model*/
   m->C=c;
   m->GENERIC=generic;
   m->NEXT=HEAD_MODEL;
   HEAD_MODEL=m;
}




void freeDoubleChain(chain_list *cl)
{
  chain_list *temp;
  for (temp=cl;temp!=NULL;temp=temp->NEXT)
    freechain(temp->DATA);
  freechain(cl);
}

void CleanGENCONandGENSIG(lofig_list *lf)
{
  loins_list *li;
  locon_list *lc;
  losig_list *ls;
  ptype_list *pt;

  FinishCleanLofigMarks(lf);
  
  // grad the signals to modify
  for (lc=lf->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      // destroy genconchain
      if ((pt=getptype(lc->USER,GENCONCHAIN))!=NULL) 
	{
	  freegenconchain((genconchain_list *)pt->DATA);
	  lc->USER=delptype(lc->USER, GENCONCHAIN);
	}
    }
  
  for (li=lf->LOINS;li!=NULL;li=li->NEXT)
    {
      if ((pt=getptype(li->USER,GENERIC_PTYPE))!=NULL) 
	{
	   if (pt->DATA!=NULL) Free_Tree((tree_list *)pt->DATA);
	   li->USER=delptype(li->USER, GENERIC_PTYPE);
	}

      for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
	{
	  // destroy genconchain
	  if ((pt=getptype(lc->USER,GENCONCHAIN))!=NULL) 
	    {
	      freegenconchain((genconchain_list *)pt->DATA);
	      lc->USER=delptype(lc->USER, GENCONCHAIN);
	    }
	}

      li->USER=delptype(li->USER, GEN_GENERIC_MAP_PTYPE);
      li->USER=delptype(li->USER, GENIUS_LOINS_FLAGS_PTYPE);

    }

  for (ls=lf->LOSIG;ls!=NULL;ls=ls->NEXT)
    {
      if ((pt=getptype(ls->USER,GENSIGCHAIN))!=NULL) 
	{
	  freegensigchain((gensigchain_list *)pt->DATA);
	  ls->USER=delptype(ls->USER, GENSIGCHAIN);
	}
       if ((pt=getptype(ls->USER,GEN_REAL_RANGE_PTYPE))!=NULL)
	 {
	   if (pt->DATA!=NULL) Free_Tree((tree_list *)pt->DATA);
	   ls->USER=delptype(ls->USER, GEN_REAL_RANGE_PTYPE);
	 }
    }

  // freeing symmetry info
  pt=getptype(lf->USER, GEN_SYM_INFO_PTYPE);
  if (pt==NULL) EXIT(66);
  lf->USER=delptype(lf->USER, GEN_SYM_INFO_PTYPE);

  // freeing coupling info
  pt=getptype(lf->USER, GEN_COUPLED_INFO_PTYPE);
  if (pt==NULL) EXIT(67);
  lf->USER=delptype(lf->USER, GEN_COUPLED_INFO_PTYPE);

}


void CleanConnector(locon_list *lc)
{
  ptype_list *pt;
  if ((pt=getptype(lc->USER,GENCONCHAIN))!=NULL) 
    {
      freegenconchain((genconchain_list *)pt->DATA);
      lc->USER=delptype(lc->USER, GENCONCHAIN);
    }
}

/***************************************************************************/
/*           erase all models but not the lofig (i.e LOFIG field)          */
/***************************************************************************/
extern void freemodel()
{
   model_list *m,*next;
   lofig_list *lf;
   locon_list *lc;
   chain_list *cl;
   ptype_list *p;

   for (m=HEAD_MODEL;m;m=next) {
      next=m->NEXT;
      if (m->VAR!=NULL) freechain(m->VAR);
      if (m->GENERIC!=NULL) Free_Generic(m->GENERIC);

      CleanGENCONandGENSIG(m->LOFIG);
      if ((p=getptype(m->LOFIG->USER, GEN_SYM_INFO_PTYPE))!=NULL) freechain((chain_list *)p->DATA);
      if ((p=getptype(m->LOFIG->USER, GEN_COUPLED_INFO_PTYPE))!=NULL) freechain((chain_list *)p->DATA);
      dellofig(m->LOFIG->NAME);
      mbkfree(m);
   }
   HEAD_MODEL=NULL;

   // dummy transistor model cleanup

   for (cl=TNMOS;cl!=NULL;cl=cl->NEXT)
     {
       lf=getloadedlofig((char *)cl->DATA);
       if (lf!=NULL)
         {
           for (lc=lf->LOCON;lc!=NULL;lc=lc->NEXT) CleanConnector(lc);
           dellofig((char *)cl->DATA);
         }
     }

   for (cl=TPMOS;cl!=NULL;cl=cl->NEXT)
     {
       lf=getloadedlofig((char *)cl->DATA);
       if (lf!=NULL)
         {
           for (lc=lf->LOCON;lc!=NULL;lc=lc->NEXT) CleanConnector(lc);
           dellofig((char *)cl->DATA);
         }
     }
}


void CleanMainLofig(lofig_list *lf)
{
  losig_list *losig;
  ptype_list *p;
  for (losig=lf->LOSIG;losig!=NULL;losig=losig->NEXT)
    {
      p=getptype(losig->USER,GEN_VISITED_PTYPE);
      if (p) 
	{
	  freeptype(p->DATA);
	  losig->USER=delptype(losig->USER,GEN_VISITED_PTYPE);
	}
    }
}

/***************************************************************************/
/*                    Dump all the model list                              */
/***************************************************************************/
extern void dumpmodel()
{
   model_list *m,*next;
   for (m=HEAD_MODEL;m;m=next) {
      next=m->NEXT;
      fprintf(GENIUS_OUTPUT,
             "************************* %s **************************\n",
             m->NAME);
      if (m->VAR) {
         chain_list *p;
         fprintf(GENIUS_OUTPUT,"VARIABLES:\n");
         for (p=m->VAR;p;p=p->NEXT) fprintf(GENIUS_OUTPUT,"%s ",(char*)p->DATA);
         fprintf(GENIUS_OUTPUT,"\n");
      }
      if (m->LOFIG) {
         fprintf(GENIUS_OUTPUT,"LOFIG:\n");
         Dump_Model(m);
         fprintf(GENIUS_OUTPUT,"\n");
      }
/*      if (m->C) {
        fprintf(GENIUS_OUTPUT,"\nACTION:\n");
        Dump_Tree(m->C);
        fprintf(GENIUS_OUTPUT,"\n");
      }*/
   }
   fprintf(GENIUS_OUTPUT,
             "********************************************************\n");
}


char *gen_canonize_trname (char* trname)
{
    char buf[2048], *ptc;
    if (trname==NULL) return NULL;
    strcpy (buf, trname);
    if ((ptc = strrchr (buf, GEN_SEPAR)))
        if (!strcmp (ptc + 1, "transistor")) {
            *ptc = '\0';
            return namealloc (buf);
        }
    return trname;
}

int CheckModelInstances(model_list *model)
{
  locon_list *lc;
  loins_list *loins;
  gensigchain_list *gensig;
  int nbexclude=0, nbwithout=0, total=0, nbinterf;
  int retcode=0;
  long flags;

  for (loins=model->LOFIG->LOINS; loins!=NULL; loins=loins->NEXT)
    {
      total++;
      flags=(long)getptype(loins->USER, GENIUS_LOINS_FLAGS_PTYPE)->DATA;
      if (flags & LOINS_IS_WITHOUT) nbwithout++;
      if (flags & LOINS_IS_EXCLUDE) nbexclude++;
    }
  
  if (nbexclude==total)
    {
      fprintf(stderr,"model '%s' must not contain only excluded instances\n",model->NAME);
      retcode=1;
    }

  if (nbwithout==total)
    {
      fprintf(stderr,"model '%s' must not contain only non existant instances\n",model->NAME);
      retcode=1;
    }
  
  for (loins=model->LOFIG->LOINS; loins!=NULL; loins=loins->NEXT)
    {
      flags=(long)getptype(loins->USER, GENIUS_LOINS_FLAGS_PTYPE)->DATA;
      if (flags & LOINS_IS_WITHOUT) 
	{
	  for (lc=loins->LOCON; lc!=NULL; lc=lc->NEXT)
	    {
	      gensig=getptype(lc->SIG->USER,GENSIGCHAIN)->DATA;
	      nbwithout=0, total=0, nbinterf=0;
	      while (gensig!=NULL)
		{
		  total++;
		  if (gensig->LOCON->TYPE=='E') nbinterf++;
		  else
		    {
		      flags=(long)getptype(((loins_list *)gensig->LOCON->ROOT)->USER, GENIUS_LOINS_FLAGS_PTYPE)->DATA;
		      if (flags & LOINS_IS_WITHOUT) nbwithout++;
		    }
		  gensig=gensig->NEXT;
		}
	      if (!mbk_LosigIsVSS(lc->SIG)
		  && !mbk_LosigIsVDD(lc->SIG))
		{
		  if (nbinterf!=0 && total==2)
		    {
		      avt_errmsg(GNS_ERRMSG, "061", AVT_ERR, model->NAME, lc->NAME, loins->INSNAME);
              //fprintf(stderr,"in model '%s', connector '%s' of unexistant instance '%s' must not be linked the model interface\n",model->NAME, lc->NAME, loins->INSNAME);
		      retcode=1;
		    }
		  if (total==1)
		    {
		      avt_errmsg(GNS_ERRMSG, "062", AVT_ERR, model->NAME, lc->NAME, loins->INSNAME);
		      //fprintf(stderr,"in model '%s', if connector '%s' of unexistant instance '%s' is not used, it must be linked to a supply\n",model->NAME, lc->NAME, loins->INSNAME);
		      retcode=1;
		    }
		  if (nbwithout!=1)
		    {
		      avt_errmsg(GNS_ERRMSG, "063", AVT_ERR, model->NAME, lc->NAME, loins->INSNAME);
		      //fprintf(stderr,"in model '%s', connector '%s' of unexistant instance '%s' must not be linked to another unexistant instance connector\n",model->NAME, lc->NAME, loins->INSNAME);
		      retcode=1;
		    }
		}
	    }
	}
    }

  // one more optimization : marquer les signaux uniquement connecté a l'interface

  {
    losig_list *ls;
    int counti, counte;
    ptype_list *p;
    for (ls=model->LOFIG->LOSIG; ls!=NULL; ls=ls->NEXT)
      {
        counti=counte=0;
        p=getptype(ls->USER,GENSIGCHAIN);
        if (p!=NULL)
          {
            for ( gensig=p->DATA; gensig!=NULL; gensig=gensig->NEXT)
              {
                if (gensig->LOCON->TYPE=='E') counte++;
                else counti++;
              }
            if (counte>0 && counti==1) ls->FLAGS=1; // une connexion a l'interface
            else if (counte==0 && counti==1) ls->FLAGS=2; // uniquement interne non connecté a +ieurs lofig
            else ls->FLAGS=0;
          }
        else ls->FLAGS=0;
      }

  }

  return retcode;
}

void setALIMSignalType(lofig_list *lf)
{
  losig_list *ls;
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      if (mbk_LosigIsVSS(ls)) ls->TYPE=CNS_SIGVSS;
      else if (mbk_LosigIsVDD(ls)) ls->TYPE=CNS_SIGVDD;
    }
}

int gen_is_token_to (int token)
{
    return token == GEN_TOKEN_TO;
}

int gen_is_token_downto (int token)
{
    return token == GEN_TOKEN_DOWNTO;
}

    
lofig_list *GENIUS_HEAD_LOFIG=NULL;
ht *GENIUS_HT_LOFIG=NULL;




void SwitchLOFIGContext()
{
  lofig_list *li=GENIUS_HEAD_LOFIG;
  ht *h=GENIUS_HT_LOFIG;
  GENIUS_HT_LOFIG=HT_LOFIG;
  GENIUS_HEAD_LOFIG=HEAD_LOFIG;
  HEAD_LOFIG=li;
  HT_LOFIG=h;
}

void DelContext()
{
  delht(HT_LOFIG);
}

void CheckLoinsLofigInconsistancy(loins_list *li, lofig_list *lf)
{
  locon_list *lc0, *lc1;
  int first=1, count=0;

  for (lc0=li->LOCON, lc1=lf->LOCON; lc0!=NULL && lc1!=NULL; lc0=lc0->NEXT, lc1=lc1->NEXT)
    {
      if (count>5) { fprintf(stderr,"\t...\n"); break; }
      if (lc0->NAME!=lc1->NAME)
	{
	  if (first==1) avt_errmsg(GNS_ERRMSG, "064", AVT_ERROR, li->INSNAME, li->FIGNAME);
                  //fprintf(stderr,"Inconsistancies found for instance '%s' of model '%s'\n",li->INSNAME, li->FIGNAME);
      avt_errmsg(GNS_ERRMSG, "065", AVT_ERROR, lc0->NAME, lc1->NAME);
	  //fprintf(stderr,"\tConnector '%s' of instance mismatched with connector '%s' of model\n",lc0->NAME, lc1->NAME);
	  first=0;
	  count++;
	}
      if (lc0->DIRECTION==UNKNOWN)
        lc0->DIRECTION=lc1->DIRECTION;
      else
        if (lc1->DIRECTION!=UNKNOWN && lc0->DIRECTION!=lc1->DIRECTION)
          {
            if (first==1) avt_errmsg(GNS_ERRMSG, "066", AVT_ERROR, lc0->NAME, li->INSNAME, li->FIGNAME);
                          // fprintf(stderr,"Inconsistancies found for connector '%s' of instance '%s' with model '%s'\n", lc0->NAME, li->INSNAME, li->FIGNAME);
            first=0;
            count++;
          }

    }
  if (count<=5 && (lc0!=NULL || lc1!=NULL))
    {
      if (first==1) avt_errmsg(GNS_ERRMSG, "064", AVT_ERROR, li->INSNAME, li->FIGNAME);
                    // fprintf(stderr,"Inconsistancies found for instance '%s' of model '%s'\n",li->INSNAME, li->FIGNAME);
      avt_errmsg(GNS_ERRMSG, "067", AVT_ERROR);
      //fprintf(stderr,"\tConnector number mismatched\n");
    }
  if (count!=0) EXIT(1);
}

char *completename(char *geniusmodel, char *model, char *archi)
{
  char temp[200];
  if (archi!=model)
    {
      sprintf(temp,"%s_%s",geniusmodel,archi);
      return namealloc(temp);
    }
  return geniusmodel;    
}

// _________________________________________________

chain_list *ModelTrace=NULL; // list of models marked as visited by the user

extern int gns_ModelVisitedDuringInit(char *name);
extern void gns_MarkModelVisitedDuringInit(char *name);


extern FILE *avtfopen(char *name, char *ext, char *mode)
{
  return mbkfopen(name, ext, mode);
}

extern int gns_ModelVisited(char *name)
{
  chain_list *cl;
  for (cl=ModelTrace; cl!=NULL && strcmp(cl->DATA,name)!=0; cl=cl->NEXT) ;
  return cl!=NULL;
}

extern void gns_MarkModelVisited(char *name)
{
  if (gns_ModelVisited(name)) return;
  ModelTrace=addchain(ModelTrace, namealloc(name));
}

