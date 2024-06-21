/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf_annot.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include AVT_H
#include INF_H
#include "spf_util.h"
#include "spf_actions.h"
#include "spf_annot.h"

#include <ctype.h>

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

#define LOFIG_HT_SIZE 500

static ht *all_lofig_losig;
static ht *all_lofig_loins;
static ht *all_lofig_lotrs;
mbk_match_rules SPF_IGNORE_SIGNAL;

losig_list *groundlosig=NULL;
static char spf_BUS_LEFTB[3];
static char spf_BUS_RIGHTB[3];
static char spf_DIVIDER;
static char spf_DELIMITER;

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/


/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

char spf_getDivider()
{
  return spf_DIVIDER;
}

char spf_getDelimiter()
{
  return spf_DELIMITER;
}

void spf_setDelimiter(char c)
{
  spf_DELIMITER=c;
}

void spf_setDivider(char c)
{
  spf_DIVIDER=c;
}

void spf_initBUSDelimiters(char *leftb, char *rightb)
{
  sprintf(spf_BUS_LEFTB, "%s", leftb);
  sprintf(spf_BUS_RIGHTB, "%s", rightb);
}

void spf_setBUSDelimiters(char leftb, char rightb)
{
  sprintf(spf_BUS_LEFTB, "%c", leftb);
  sprintf(spf_BUS_RIGHTB, "%c", rightb);
}

char *spf_spi_devect( char *nom )
{

  if (MBK_DEVECT==1)
    return mbk_devect(nom, spf_BUS_LEFTB, spf_BUS_RIGHTB);
  else
    return(namealloc(nom));
}
/******************************************************************************/

void spf_prepare_lofig(lofig_list *ptlofig)
{
     char *str;
     inffig_list *ifl;

     if(V_BOOL_TAB[__MBK_FLATTEN_FOR_PARA].VALUE)
       {
         char separavant = SEPAR;
         char oldkeepstate;
         
         SEPAR = spf_getDivider();
         oldkeepstate=FLATTEN_KEEP_ALL_NAMES;
         FLATTEN_KEEP_ALL_NAMES=1;

         rflattenlofig(ptlofig, YES, YES);

         FLATTEN_KEEP_ALL_NAMES=oldkeepstate;
         SEPAR = separavant;
       }
     spf_create_losig_htable(ptlofig);
     spf_create_loins_htable(ptlofig);
     spf_create_lotrs_htable(ptlofig);

     ifl=getloadedinffig(ptlofig->NAME);
     inf_buildmatchrule(ifl, INF_IGNORE_PARASITICS, &SPF_IGNORE_SIGNAL, 1);
}

/******************************************************************************/

// create a lofig htable to get a signal using its name
void spf_create_losig_htable(lofig_list *lofig)
{
  losig_list *ls;
  chain_list *ch;
  int nbsig=0;

  all_lofig_losig=addht(LOFIG_HT_SIZE);

  for (ls=lofig->LOSIG;ls!=NULL;ls=ls->NEXT,nbsig++)
    {
#ifdef SPF_DEBUG
      printf("Losig %s (idx=%ld)\n",(char *)ls->NAMECHAIN->DATA,ls->INDEX);
#endif
      for (ch=ls->NAMECHAIN;ch!=NULL;ch=ch->NEXT)
	addhtitem(all_lofig_losig,(char *)ch->DATA,(long)ls);
    }    

#ifdef SPF_DEBUG
  printf("%d signaux\n",nbsig);
#endif

#ifdef SPF_DEBUG
  for (lc=lofig->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      printf("Locon %s losig %s\n",lc->NAME,(char *)lc->SIG->NAMECHAIN->DATA);
    }
#endif
}

// create a lofig htable to get a instance using its name
void spf_create_loins_htable(lofig_list *lofig)
{
  loins_list *li;
  int nbins=0;

  all_lofig_loins=addht(LOFIG_HT_SIZE);

  for (li=lofig->LOINS;li!=NULL;li=li->NEXT,nbins++)
    {
#ifdef SPF_DEBUG
      printf("Loins %s (idx=%ld)\n",(char *)li->INSNAME);
#endif
      addhtitem(all_lofig_loins,(char *)li->INSNAME,(long)li);
    }    

#ifdef SPF_DEBUG
  printf("%d instances\n",nbins);
#endif
}

// create a lofig htable to get a instance using its name
void spf_create_lotrs_htable(lofig_list *lofig)
{
  lotrs_list *tr;
  int nbtr=0;

  all_lofig_lotrs=addht(LOFIG_HT_SIZE);

  for (tr=lofig->LOTRS;tr!=NULL;tr=tr->NEXT,nbtr++)
    {
#ifdef SPF_DEBUG
      printf("Lotrs %s (idx=%ld)\n",(char *)tr->TRNAME);
#endif
      addhtitem(all_lofig_lotrs,(char *)tr->TRNAME,(long)tr);
    }    

#ifdef SPF_DEBUG
  printf("%d transistors\n",nbtr);
#endif
}

// remove all the RC information from the signals
void spf_destroyexistingRC(lofig_list *lofig)
{
  losig_list *ls;

  for (ls=lofig->LOSIG;ls!=NULL;ls=ls->NEXT)
    {
      if (givelorcnet(ls)!=NULL)
	{
	  freelorcnet(ls);
	}
      addlorcnet(ls);
    }    
}

// find a valid signal index a te able to add a new signal in the netlist
long findindexinlofig(lofig_list *lofig)
{
  long current=-1;
  losig_list *ls;

  for (ls=lofig->LOSIG;ls!=NULL;ls=ls->NEXT)
    {
      if (ls->INDEX>current) current=ls->INDEX;
    }    
  return current+1;
}

void spf_destroy_losig_htable()
{
  delht(all_lofig_losig);
}

void spf_destroy_loins_htable()
{
  delht(all_lofig_loins);
}

void spf_destroy_lotrs_htable()
{
  delht(all_lofig_lotrs);
}

// return a signal given its name, NULL if it does not exist
losig_list *getlosigbyname(char *name)
{
  long value;
  value=gethtitem(all_lofig_losig,name);
  if (value==EMPTYHT) return NULL;
  return (losig_list *)value;
}

// return a instance given its name, NULL if it does not exist
loins_list *getloinsbyname(char *name)
{
  long value;
  value=gethtitem(all_lofig_loins,name);
  if (value==EMPTYHT) return NULL;
  return (loins_list *)value;
}

// return a transistor given its name, NULL if it does not exist
lotrs_list *getlotrsbyname(char *name)
{
  long value;
  value=gethtitem(all_lofig_lotrs,name);
  if (value==EMPTYHT) return NULL;
  return (lotrs_list *)value;
}

// return a signal and a node number for a given signal node name
void getsignalandnode(lofig_list *Lofig, losig_list *currentsignal, char *nametofind, ht *nodelist, char *ground, losig_list **signalfound, long *nodefound, int noforce)
{
  long tmp;
  char *name;
  
//  name=namealloc(nametofind); // spf_spi_devect en amont
  name=nametofind;

  if (spf_find_pending_node(name, signalfound, nodefound)) return;

  // is it a node?
  *nodefound=gethtitem(nodelist,name);

  if (*nodefound==EMPTYHT) 
    {
      // no... is it the ground signal?
      if (name==ground)
        {
          if (groundlosig==NULL && (groundlosig=getlosigbyname(name))==NULL
              && (groundlosig=getlosigbyname(mbk_getvssname()))==NULL)
            {
              // yes but the signal does not exist, let create it...
              tmp=findindexinlofig(Lofig); // last valid index

    #ifdef SPF_DEBUG
              printf("Creating ground signal %s idx=%ld\n",ground,tmp);
    #endif

              groundlosig=addlosig(Lofig,tmp,addchain(NULL,mbk_getvssname()),INTERNAL);
              groundlosig->USER=addptype(groundlosig->USER, LOFIGCHAIN, NULL);
              addlorcnet(groundlosig);
              addhtitem(all_lofig_losig,ground,(long)groundlosig);	      
              groundlosig->ALIMFLAGS=MBK_HAS_GLOBAL_NODE_FLAG;
            }
          // all's right
          *signalfound=groundlosig;
          groundlosig->ALIMFLAGS|=MBK_ALIM_VSS_FLAG|MBK_SIGNAL_ALIM_DONE_FLAG;
          *nodefound=0;
        }
      else if (!noforce)
        {
          spf_AddSubNode(name);
          *nodefound=gethtitem(nodelist,name);
          *signalfound=currentsignal;
          if (getlosigbyname(spf_spi_devect(name))!=currentsignal)
            avt_errmsg(SPF_ERRMSG, "003", AVT_WARNING, nametofind, spf_Line-1);
        }
      else
        *nodefound=-1, *signalfound=NULL;
    }
  else
    *signalfound=currentsignal;
}
/*******************************************************************/
char *spf_deqstring (char *str)
{
    char ss[1024];
    int i = 0, j = 0;

    while (str[i] != '\0'){
        if (str[i] == '\"'){
            i++;
        }else{
            ss[j] = str[i]; 
            i++;
            j++;
        }
    }
    ss[j] = '\0';
 
    return namealloc (ss);
}
/*******************************************************************/
char *spf_rename (char *str)
{
    char ss[1024];
    int i = 0, j = 0;
    char flag = 0;

    while (str[i] != '\0'){
        if (str[i] == '\\'){
            flag = 1;
            i++;
        }else{
            ss[j] = str[i]; 
            i++;
            j++;
        }
    }
    ss[j] = '\0';
 
    if(!flag)
        return namealloc(spf_spi_devect(ss));
    else
        return namealloc (ss);
}

void spf_checklofigsignal(lofig_list *lf)
{
  losig_list *ls;
  float tot, cnet, cdspf;
  char *first=NULL;
  ptype_list *pt;
  
  if (lf==NULL) return;

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
  {
    if (ls->PRCN!=NULL)
    {
      if ((pt=getptype(ls->USER, SPF_ORIG_CAPA_TOT))!=NULL)
      {
        tot=*(float *)&pt->DATA;
        if ((pt=getptype(ls->USER, SPF_ORIG_CAPA_SUM))!=NULL) cdspf=*(float *)&pt->DATA;
        else cdspf=0;

        ls->USER=testanddelptype(ls->USER, SPF_ORIG_CAPA_TOT);
        ls->USER=testanddelptype(ls->USER, SPF_ORIG_CAPA_SUM);

        if (fabs(tot-cdspf)>1e-5)
        {
           avt_errmsg(SPF_ERRMSG, "004", AVT_WARNING, getsigname(ls), tot, cdspf);
        }
      }

      rcn_calccapa(ls);
    }
  }
  
  mbk_FreeREGEX(&SPF_IGNORE_SIGNAL);
}
