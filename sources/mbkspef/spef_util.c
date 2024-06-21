/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPEF Version 1.00                                            */
/*    Fichier : spef_annot.c                                                 */
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
#include MUT_H
#include MLO_H
#include EQT_H
#include MSL_H
#include "spef_util.h"
#include "spef_annot.h"

#include <ctype.h>

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

#define HASH_MULT 314159
#define HASH_PRIME 516595003
#define LOFIG_HT_SIZE 0

#if 0
typedef struct
{
  int SPEF_HASHSIZE = 100003 ;
  chain_list **SPEF_HASHTABLE = NULL ;
  static ht *all_lofig_losig;
  static ht *all_lofig_loins;
  static ht *all_lofig_lotrs;
  losig_list *spef_groundlosig=NULL;
} _SPEF_CACHE_;

static _SPEF_CACHE_ *CUR;
#endif

int SPEF_HASHSIZE = 100003 ;
chain_list **SPEF_HASHTABLE = NULL ;
static ht_v2 *all_lofig_losig;
static ht_v2 *all_lofig_loins;
static ht_v2 *all_lofig_lotrs, *all_lofig_capa, *all_lofig_resi;
losig_list *spef_groundlosig=NULL;
int spef_quiet=0;

void spef_ht_stats()
{
  printf("losig: "); ht_v2Stat(all_lofig_losig);
  printf("loins: "); ht_v2Stat(all_lofig_loins);
  printf("lotrs: "); ht_v2Stat(all_lofig_lotrs);
}
/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/


/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
#if 0
static _SPEF_CACHE_ *spef_setstruct(lofig_list *lf)
{
  ptype_list *pt;
  if ((pt=getptype(lf->USER, SPEF_CACHE_PTYPE))==NULL)
    {
      CUR=NULL;
      return NULL;
    }
  CUR=(_SPEF_CACHE_ *)pt->DATA;
  return CUR;
}

void spef_createstruct(lofig_list *lf)
{
  ptype_list *pt;
  _SPEF_CACHE_ *sc;

  if ((pt=getptype(lf->USER, SPEF_CACHE_PTYPE))==NULL)

  sc=(_SPEF_CACHE_ *)mbkalloc(sizeof(_SPEF_CACHE_));
  sc->spef_groundlosig=NULL;
  sc->SPEF_HASHTABLE=NULL;
  lf->USER=addptype(lf->USER, SPEF_CACHE_PTYPE, sc);
}
#endif

// the same function as in mbkspice parser
char*           spef_spi_devect( char *nom )
{
  int           taille;
  int           i;
  int           j;
  char          nouv[4096] ;
  int           fin;
  int           modif;
 

  if (MBK_DEVECT==1)
    { 
      taille = strlen( nom );
      
      fin   = taille - 1 ;
      modif = 0;
      
      if( nom[fin] == SPEF_INFO->SUFFIX_BUS_DELIMITER )
    { 
      /* Retrouve le crochet ouvrant */
      for( i = fin-1 ; i >= 0 && isdigit( (int)nom[i] ) ; i-- );
      
      if( nom[i] == SPEF_INFO->PREFIX_BUS_DELIMITER )
        {
          if( i != 0 && i != fin - 1 )
        {
          /* bon vecteur : toto[32] */
          for( j = 0 ; j < fin ; j++ )
            if( i != j )
              nouv[j] = nom[j];
            else
              nouv[i] = ' ';
          nouv[j] = 0;
          
          modif = 1;
        }
        }
    }
      
      if( ! modif )
    strcpy( nouv, nom );
      
      return(namealloc(nouv));
    }
  else
    return(namealloc(nom));
}

/******************************************************************************/
char spef_sufdelim(char delim)
{
    if(delim == '[')
        return ']';
    if(delim == '{')
        return '}';
    if(delim == '(')
        return ')';
    if(delim == '<')
        return '>';
    return ']';
}
/******************************************************************************/
// create a lofig htable to get a signal using its name
void spef_create_losig_htable(lofig_list *lofig)
{
  losig_list *ls;
  locon_list *lc;
  chain_list *ch;
  int nbsig=0;

  all_lofig_losig=addht_v2(LOFIG_HT_SIZE);

  for (ls=lofig->LOSIG;ls!=NULL;ls=ls->NEXT,nbsig++)
    {
#ifdef SPEF_DEBUG
      printf("Losig %s (idx=%ld)\n",(char *)ls->NAMECHAIN->DATA,ls->INDEX);
#endif
      for (ch=ls->NAMECHAIN;ch!=NULL;ch=ch->NEXT)
    addhtitem_v2(all_lofig_losig,(char *)ch->DATA,(long)ls);
    }    

#ifdef SPEF_DEBUG
  printf("%d signaux\n",nbsig);
#endif

  for (lc=lofig->LOCON;lc!=NULL;lc=lc->NEXT)
    {
#ifdef SPEF_DEBUG
      printf("Locon %s losig %s\n",lc->NAME,(char *)lc->SIG->NAMECHAIN->DATA);
#endif
    }
}

/*******************************************************************/
// create a lofig htable to get a instance using its name
void spef_create_loins_htable(lofig_list *lofig)
{
  loins_list *li;
  lotrs_list *lt;
  losig_list *ls;
  int nbins=0, hasparas;
  loctc_list *lctc;
  chain_list *cl;
  locon_list *lc;
  lowire_list *lw;
  ptype_list *pt;
  long minfree;

  all_lofig_loins=addht_v2(LOFIG_HT_SIZE);

  for (li=lofig->LOINS;li!=NULL;li=li->NEXT,nbins++)
    {
#ifdef SPEF_DEBUG
//      printf("Loins %s (idx=%ld)\n",(char *)li->INSNAME); // modifier par zinaps
      printf("Loins %s\n",(char *)li->INSNAME);
#endif
      addhtitem_v2(all_lofig_loins,(char *)li->INSNAME,(long)li);
    }    

#ifdef SPEF_DEBUG
  printf("%d instances\n",nbins);
#endif


  // zinaps:
  all_lofig_lotrs=addht_v2(LOFIG_HT_SIZE);
  set_ht_v2_rehashlimit(all_lofig_lotrs, 16);
  for (lt=lofig->LOTRS, nbins=0;lt!=NULL;lt=lt->NEXT,nbins++)
    {
#ifdef SPEF_DEBUG
      printf("Lotrs %s\n",lt->TRNAME);
#endif
      addhtitem_v2(all_lofig_lotrs,lt->TRNAME,(long)lt);
    }    

#ifdef SPEF_DEBUG
  printf("%d transistors\n",nbins);
#endif

  all_lofig_capa=addht_v2(LOFIG_HT_SIZE);
  all_lofig_resi=addht_v2(LOFIG_HT_SIZE);
  if (SPEF_PRESERVE_RC)
  {
    for (ls=lofig->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      hasparas=0; minfree=0;
      if (ls->PRCN!=NULL)
      {
        for (cl=ls->PRCN->PCTC; cl!=NULL; cl=cl->NEXT)
        {
          lctc=(loctc_list *)cl->DATA;
          if (lctc->SIG1==ls) { lctc->NODE1=-lctc->NODE1; if (lctc->NODE1<minfree) minfree=lctc->NODE1; }
          if (lctc->SIG2==ls) { lctc->NODE2=-lctc->NODE2; if (lctc->NODE2<minfree) minfree=lctc->NODE2; }
          if ((pt=getptype(lctc->USER, MSL_CAPANAME))!=NULL)
          {
            addhtitem_v2(all_lofig_capa,pt->DATA,(long)lctc);
//            printf("add c %s\n",(char *)pt->DATA);
          }
          hasparas=1;
        }
        for (lw=ls->PRCN->PWIRE; lw!=NULL; lw=lw->NEXT)
        {
          lw->NODE1=-lw->NODE1;
          lw->NODE2=-lw->NODE2;
          if (lw->NODE1<minfree) minfree=lw->NODE1;
          if (lw->NODE2<minfree) minfree=lw->NODE2;
          if ((pt=getptype(lw->USER, RESINAME))!=NULL)
          {
            addhtitem_v2(all_lofig_resi,pt->DATA,(long)lw);
            lw->USER=addptype(lw->USER, SPEF_RESISIG, ls);
//            printf("add w %s\n",(char *)pt->DATA);
          }
          hasparas=1;
        }
        ls->PRCN->NBNODE=1;
      }
      if (hasparas)
      {
        for (cl=(chain_list *)getptype(ls->USER, LOFIGCHAIN)->DATA; cl!=NULL; cl=cl->NEXT)
        {
          lc=(locon_list *)cl->DATA;
          if (lc->PNODE) 
          {
            lc->PNODE->DATA=-lc->PNODE->DATA;
            if (lc->PNODE->DATA<minfree) minfree=lc->PNODE->DATA;
          }
        }
        ls->USER=addptype(ls->USER, SPEF_RESISIG, NULL);
        ls->USER = addptype(ls->USER, SPEF_NODE_MIN, (void*)(minfree));
      }
    }
  }
}

/*******************************************************************/
// remove all the RC information from the signals
void spef_destroyexistingRC(lofig_list *lofig)
{
  losig_list *ls;

  for (ls=lofig->LOSIG;ls!=NULL;ls=ls->NEXT)
    {
      if (ls->PRCN!=NULL)
      {
        if (!SPEF_PRESERVE_RC) freelorcnet(ls);
      }
      if (ls->PRCN==NULL) addlorcnet(ls);
    }    
}

/*******************************************************************/
// find a valid signal index a te able to add a new signal in the netlist
long spef_findindexinlofig(lofig_list *lofig)
{
  long current=-1;
  losig_list *ls;

  for (ls=lofig->LOSIG;ls!=NULL;ls=ls->NEXT)
    {
      if (ls->INDEX>current) current=ls->INDEX;
    }    
  return current+1;
}

/*******************************************************************/
void spef_destroy_losig_htable()
{
  if(all_lofig_losig)
      delht_v2(all_lofig_losig);
}

/*******************************************************************/
void spef_destroy_loins_htable()
{
  if(all_lofig_loins) delht_v2(all_lofig_loins);
  if(all_lofig_lotrs) delht_v2(all_lofig_lotrs);
  if(all_lofig_capa) delht_v2(all_lofig_capa);
  if(all_lofig_resi) delht_v2(all_lofig_resi);
}

/*******************************************************************/
// return a signal given its name, NULL if it does not exist
losig_list *spef_getlosigbyname(char *name)
{
  long value;
  value=gethtitem_v2(all_lofig_losig,name);
  if (value==EMPTYHT) return NULL;
  return (losig_list *)value;
}

/*******************************************************************/
// return a signal given its name, NULL if it does not exist
loins_list *spef_getloinsbyname(char *name)
{
  long value;
  value=gethtitem_v2(all_lofig_loins,name);
  if (value==EMPTYHT) return NULL;
  return (loins_list *)value;
}

// return a signal given its name, NULL if it does not exist
lotrs_list *spef_getlotrsbyname(char *name)
{
  long value;
  value=gethtitem_v2(all_lofig_lotrs,name);
  if (value==EMPTYHT) return NULL;
  return (lotrs_list *)value;
}
lowire_list *spef_getresibyname(char *name)
{
  long value;
  value=gethtitem_v2(all_lofig_resi,name);
  if (value==EMPTYHT) return NULL;
  return (lowire_list *)value;
}
loctc_list *spef_getcapabyname(char *name)
{
  long value;
  value=gethtitem_v2(all_lofig_capa,name);
  if (value==EMPTYHT) return NULL;
  return (loctc_list *)value;
}

/*******************************************************************/
char *spef_deqstring (char *str)
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
char *spef_rename (char *str)
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
        return spef_spi_devect(ss);
    else
        return namealloc (ss);
}


static char curnet[1024]="";
static losig_list *cursig=NULL;
static ht *spef_posttrait=NULL;


void spef_examine_set_nodes(losig_list *ls)
{
  ptype_list *pt;
  chain_list *cl;
  locon_list *lc;

  if ((pt=getptype(ls->USER, LOFIGCHAIN))!=NULL)
    {
      for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
        {
          lc=(locon_list *)cl->DATA;
          if (lc->PNODE!=NULL) lc->FLAGS=1;
          else lc->FLAGS=0;
        }
    }
}

void spef_examine_add_shortcircuit(int num, locon_list *lc)
{
  chain_list *ch;
  long l;

  if (num!=-1)
    {
      if (spef_posttrait==NULL) spef_posttrait=addht(100);
      if ((l=gethtitem(spef_posttrait, (void *)(long)num))==EMPTYHT) ch=NULL;
      else ch=(chain_list *)l;
      ch=addchain(ch, lc);
      addhtitem(spef_posttrait, (void *)(long)num, (long)ch);
    }
}


void spef_setcurnet(char *name, losig_list *ls)
{  
  if (spef_posttrait!=NULL)
    {
      chain_list *cl, *ch, *ch0;
      locon_list *lc;
      cl=GetAllHTElems(spef_posttrait);
      while (cl!=NULL)
        {
          for (ch=(chain_list *)cl->DATA; ch!=NULL; ch=ch->NEXT)
            {
              lc=(locon_list *)ch->DATA;
              if (lc->FLAGS) break;
            }
          if (ch!=NULL)
            {
              for (ch0=(chain_list *)cl->DATA; ch0!=NULL; ch0=ch0->NEXT)
                {                  
                  ((locon_list *)ch0->DATA)->PNODE->DATA=lc->PNODE->DATA;
                }
            }
          freechain((chain_list *)cl->DATA);
          cl=delchain(cl, cl);
        }
      delht(spef_posttrait);
      spef_posttrait=NULL;
    }
  if (cursig!=NULL && getptype(cursig->USER, SPEF_RESISIG)!=NULL)
  {
    cursig->USER=delptype(cursig->USER, SPEF_RESISIG);
    rcn_setsignal_disablecache(cursig);
  }
  strcpy(curnet, name);
  cursig=ls;
}

losig_list *spef_getcurnet()
{
  return cursig;
}

void spef_checkpnode(locon_list *lc, long *node)
{
  ptype_list *ptype, *pht=NULL;
  ht *atht;
  long l;
  if (lc->PNODE!=NULL && getptype(lc->SIG->USER, SPEF_RESISIG)!=NULL) 
    {
      if((pht = getptype(lc->SIG->USER, SPEF_ATTRIB_HT)) == NULL)
        pht =lc->SIG->USER=addptype(lc->SIG->USER, SPEF_ATTRIB_HT, addht(10));
      atht=(ht *)pht->DATA;
      *node=lc->PNODE->DATA;
      if ((l=gethtitem(atht, (void *)-lc->PNODE->DATA))==EMPTYHT)
      {
        addhtitem(atht, (void *)-lc->PNODE->DATA, (long)&lc->PNODE);
        return;
      }
      else if ((long)&lc->PNODE!=l)
      {
       freenum(lc->PNODE);
       lc->PNODE=NULL;
      }
      else return;
    }
  if(lc->PNODE){
      *node = lc->PNODE->DATA;
  }else if((ptype = getptype(lc->SIG->USER, SPEF_NODE_MIN)) != NULL){
    *node = (long)ptype->DATA - 1;
    setloconnode(lc, (long)ptype->DATA - 1); 
    ptype->DATA = (void*)((long)ptype->DATA - 1);
  }else{
    *node = -1;
    lc->SIG->USER = addptype(lc->SIG->USER, SPEF_NODE_MIN, (void*)(-1));
    setloconnode(lc, -1); 
  }
  if (pht!=NULL)
   addhtitem(atht, (void *)-*node, (long)&lc->PNODE);
}

void spef_checkpnode_parasitic(long *nm, losig_list *ls, long *node)
{
  ptype_list *ptype, *pht=NULL;
  ht *atht;
  long l;
  if (getptype(ls->USER, SPEF_RESISIG)!=NULL) 
    {
      if((pht = getptype(ls->USER, SPEF_ATTRIB_HT)) == NULL)
        pht =ls->USER=addptype(ls->USER, SPEF_ATTRIB_HT, addht(10));
      atht=(ht *)pht->DATA;
      *node=*nm;
      if ((l=gethtitem(atht, (void *)-*nm))==EMPTYHT)
      {
        addhtitem(atht, (void *)-*nm, (long)nm);
        return;
      }
      else if ((long)nm==l) return;
    }
  if((ptype = getptype(ls->USER, SPEF_NODE_MIN)) != NULL){
    *node = (long)ptype->DATA - 1;
    ptype->DATA = (void*)*node;
  }else{
    *node = -1;
    ls->USER = addptype(ls->USER, SPEF_NODE_MIN, (void*)-1);
  }
  *nm=*node; 
  if (pht!=NULL)
   addhtitem(atht, (void *)-*node, (long)nm);
}

/*******************************************************************/
char spef_recupnodeandsig (lofig_list *Lofig, char *str, char *ground, losig_list **ptsig, long *node, long **pnode)
{
    char instorsig_name[1024];
    char nodeorlocon_name[1024];
    int i = 0, j = 0;
    char *ios_name, *nol_name, *name;
    long value, tmp;
    loins_list *ptins = NULL;
    lotrs_list *lt=NULL;
    locon_list *ptcon;
    ptype_list *ptype;
    char spef_pindelim = SPEF_INFO->DELIMITER;
    char flag = 0, flagnodevect = 0;
    char *delim=NULL;
    lowire_list *lw;
    loctc_list *lctc;
    int pos;
    char *posi, *negi;

    *ptsig = NULL;
    *pnode=NULL;
        
    while (str[i] != '\0'){
        if (str[i] == '\\'){
            flagnodevect = 1;
            i++;
        }else{
            instorsig_name[j] = str[i];
            if (instorsig_name[j]==spef_pindelim)
              delim=&instorsig_name[j];
            i++;
            j++;
        }
    }
    instorsig_name[j] = '\0';
    
    name=delim;
    if(name) /*(name = strrchr((char*)instorsig_name, spef_pindelim)) != NULL)*/{
        flag = 1;
        name[0] = '\0';
        name = name + 1;
        i=0;
        while (name[i] != '\0'){
            nodeorlocon_name[i] = name[i];
            i++;
        }
        nodeorlocon_name[i] = '\0';
    }
    
    if (flag && cursig!=NULL && strcmp(curnet, instorsig_name)==0)
      {
        *ptsig=cursig;
        *node = atol(nodeorlocon_name);
        return 0;
      }


    if((instorsig_name[0] == '*') && flag)
      {
        if((value = getititem(namemaptable, atoi(instorsig_name + 1))) != EMPTYHT)
          {
            ios_name = (char*)value; //namealloc((char*)value);

            if (nodeorlocon_name[0]>='0' && nodeorlocon_name[0]<='9' && (*ptsig = spef_getlosigbyname(ios_name))!=NULL)
              {
                *node = atol(nodeorlocon_name);
              }
            else if(Lofig->LOINS!=NULL && ((ptins = spef_getloinsbyname(spef_check_keep(ios_name,'x',1)))!=NULL || (ptins = spef_getloinsbyname(mbk_vect(spef_check_keep(ios_name,'x',0),'[',']')))!=NULL))
              {
                nol_name = spef_spi_devect(nodeorlocon_name);
                for (ptcon=ptins->LOCON;ptcon!=NULL && spef_spi_devect(ptcon->NAME)!=nol_name;ptcon=ptcon->NEXT) ;
                if(!ptcon) return 0;
                *ptsig = ptcon->SIG;
                spef_checkpnode(ptcon, node);
              }
            else if (Lofig->LOTRS!=NULL && ((lt = spef_getlotrsbyname(spef_check_keep(ios_name,'m',1)))!=NULL || (lt = spef_getlotrsbyname(mbk_vect(spef_check_keep(ios_name,'m',0),'[',']')))!=NULL))
              {          
                //                  nol_name = spef_spi_devect(nodeorlocon_name);
                nol_name=nodeorlocon_name;
                
                if (strcasecmp(nol_name, ANNOT_T_G?ANNOT_T_G:"g")==0) ptcon=lt->GRID;
                else if (strcasecmp(nol_name, ANNOT_T_D?ANNOT_T_D:"d")==0) ptcon=lt->DRAIN;
                else if (strcasecmp(nol_name, ANNOT_T_S?ANNOT_T_S:"s")==0) ptcon=lt->SOURCE;
                else if (strcasecmp(nol_name, ANNOT_T_B?ANNOT_T_B:"b")==0) ptcon=lt->BULK;
                else ptcon=NULL;

                if (ptcon==NULL)
                  {
                    avt_errmsg(SPE_ERRMSG, "010", AVT_FATAL, spef_ParsedFile, Line-1, nol_name, ios_name);      
                    EXIT(4);
                  }

                *ptsig = ptcon->SIG;
                spef_checkpnode(ptcon, node);
              }
         else if ((lw = spef_getresibyname(spef_check_keep(ios_name,'r',1)))!=NULL)
          { 
            nol_name=nodeorlocon_name;

            if ((ANNOT_R_POS==NULL
                 && (strcasecmp(nol_name, "pos")==0 || strcasecmp(nol_name, "1")==0))
                || (ANNOT_R_POS!=NULL && strcasecmp(nol_name, ANNOT_R_POS)==0)) pos=1;
            else if ((ANNOT_R_NEG==NULL
                 && (strcasecmp(nol_name, "neg")==0 || strcasecmp(nol_name, "2")==0))
                || (ANNOT_R_NEG!=NULL && strcasecmp(nol_name, ANNOT_R_NEG)==0)) pos=0;
            else 
             {
                avt_errmsg(SPE_ERRMSG, "012", AVT_FATAL, spef_ParsedFile, Line-1, nol_name, ios_name);      
                EXIT(4);
             }
            
            *ptsig = (losig_list *)getptype(lw->USER, SPEF_RESISIG)->DATA;
            if (pos==1) *node=lw->NODE1, *pnode=&lw->NODE1; else *node=lw->NODE2, *pnode=&lw->NODE2;
          } // end zinaps
         else if ((lctc = spef_getcapabyname(spef_check_keep(ios_name,'c',1)))!=NULL)
          { 
            nol_name=nodeorlocon_name;

            posi=ANNOT_C_POS; negi=ANNOT_C_NEG;
            if (strcasecmp(nol_name,posi?posi:"1")==0) pos=1;
            else if (strcasecmp(nol_name,negi?negi:"2")==0) pos=0;
            else 
             {
                avt_errmsg(SPE_ERRMSG, "013", AVT_FATAL, spef_ParsedFile, Line-1, nol_name, ios_name);      
                EXIT(4);
             }
            
            if (pos==1) *ptsig =lctc->SIG1, *node=lctc->NODE1, *pnode=&lctc->NODE1; else *ptsig =lctc->SIG2, *node=lctc->NODE2, *pnode=&lctc->NODE2;
          } // end zinaps
         else if ((lctc = spef_getcapabyname(spef_check_keep(ios_name,'d',1)))!=NULL)
          { 
            nol_name=nodeorlocon_name;

            posi=ANNOT_D_POS; negi=ANNOT_D_NEG;
            if (strcasecmp(nol_name,posi?posi:"1")==0) pos=1;
            else if (strcasecmp(nol_name,negi?negi:"2")==0) pos=0;
            else 
             {
                avt_errmsg(SPE_ERRMSG, "014", AVT_FATAL, spef_ParsedFile, Line-1, nol_name, ios_name);      
                EXIT(4);
             }
            
            if (pos==1) *ptsig =lctc->SIG1, *node=lctc->NODE1, *pnode=&lctc->NODE1; else *ptsig =lctc->SIG2, *node=lctc->NODE2, *pnode=&lctc->NODE2;
          } // end zinaps
         else
              avt_errmsg(SPE_ERRMSG, "011", AVT_WARNING, spef_ParsedFile, Line-1, ios_name);      
          }
      }
    else if((instorsig_name[0] == '*') && !flag)
      {
        if((value = getititem(namemaptable, atoi(instorsig_name + 1))) != EMPTYHT)
          {
            //            ios_name = namealloc((char*)value);
            ios_name = (char*)value; //spef_spi_devect((char*)value);
            for (ptcon=Lofig->LOCON;ptcon!=NULL && spef_spi_devect(ptcon->NAME)!=ios_name;ptcon=ptcon->NEXT) ;
            if(!ptcon) return 0;
            *ptsig = ptcon->SIG;
            spef_checkpnode(ptcon, node);
          }
      }
    else
      {
        if (flagnodevect==0)
          ios_name = spef_spi_devect(instorsig_name);//namealloc(instorsig_name);
        else
          ios_name = namealloc(instorsig_name);
        if(Lofig->LOINS!=NULL && (ptins = spef_getloinsbyname(spef_check_keep(ios_name,'x',1)))!=NULL)
          {
            nol_name = spef_spi_devect(nodeorlocon_name);
            for (ptcon=ptins->LOCON;ptcon!=NULL && spef_spi_devect(ptcon->NAME)!=nol_name;ptcon=ptcon->NEXT) ;
            if(!ptcon) return 0;
            *ptsig = ptcon->SIG;
            spef_checkpnode(ptcon, node);
          }
        else if (Lofig->LOTRS!=NULL && (lt = spef_getlotrsbyname(spef_check_keep(ios_name,'m',1)))!=NULL)
          {       
            //              nol_name = spef_spi_devect(nodeorlocon_name);
            nol_name=nodeorlocon_name;

            if (strcasecmp(nol_name, ANNOT_T_G?ANNOT_T_G:"g")==0) ptcon=lt->GRID;
            else if (strcasecmp(nol_name, ANNOT_T_D?ANNOT_T_D:"d")==0) ptcon=lt->DRAIN;
            else if (strcasecmp(nol_name, ANNOT_T_S?ANNOT_T_S:"s")==0) ptcon=lt->SOURCE;
            else if (strcasecmp(nol_name, ANNOT_T_B?ANNOT_T_B:"b")==0) ptcon=lt->BULK;
            else ptcon=NULL;
            
            if (ptcon==NULL)
              {
                avt_errmsg(SPE_ERRMSG, "010", AVT_FATAL, spef_ParsedFile, Line-1, nol_name, ios_name);      
                EXIT(4);
              }

            *ptsig = ptcon->SIG;
            spef_checkpnode(ptcon, node);
          } // end zinaps
         else if ((lw = spef_getresibyname(spef_check_keep(ios_name,'r',1)))!=NULL)
          { 
            nol_name=nodeorlocon_name;

            if ((ANNOT_R_POS==NULL
                 && (strcasecmp(nol_name, "pos")==0 || strcasecmp(nol_name, "1")==0))
                || (ANNOT_R_POS!=NULL && strcasecmp(nol_name, ANNOT_R_POS)==0)) pos=1;
            else if ((ANNOT_R_NEG==NULL
                 && (strcasecmp(nol_name, "neg")==0 || strcasecmp(nol_name, "2")==0))
                || (ANNOT_R_NEG!=NULL && strcasecmp(nol_name, ANNOT_R_NEG)==0)) pos=0;
            else 
             {
                avt_errmsg(SPE_ERRMSG, "012", AVT_FATAL, spef_ParsedFile, Line-1, nol_name, ios_name);      
                EXIT(4);
             }
            
            *ptsig = (losig_list *)getptype(lw->USER, SPEF_RESISIG)->DATA;
            if (pos==1) *node=lw->NODE1, *pnode=&lw->NODE1; else *node=lw->NODE2, *pnode=&lw->NODE2;
          } // end zinaps
         else if ((lctc = spef_getcapabyname(spef_check_keep(ios_name,'c',1)))!=NULL)
          { 
            nol_name=nodeorlocon_name;

            posi=ANNOT_C_POS; negi=ANNOT_C_NEG;
            if (strcasecmp(nol_name,posi?posi:"1")==0) pos=1;
            else if (strcasecmp(nol_name,negi?negi:"2")==0) pos=0;
            else 
             {
                avt_errmsg(SPE_ERRMSG, "013", AVT_FATAL, spef_ParsedFile, Line-1, nol_name, ios_name);      
                EXIT(4);
             }
            
            if (pos==1) *ptsig =lctc->SIG1, *node=lctc->NODE1, *pnode=&lctc->NODE1; else *ptsig =lctc->SIG2, *node=lctc->NODE2, *pnode=&lctc->NODE2;
          } // end zinaps
         else if ((lctc = spef_getcapabyname(spef_check_keep(ios_name,'d',1)))!=NULL)
          { 
            nol_name=nodeorlocon_name;

            posi=ANNOT_D_POS; negi=ANNOT_D_NEG;
            if (strcasecmp(nol_name,posi?posi:"1")==0) pos=1;
            else if (strcasecmp(nol_name,negi?negi:"2")==0) pos=0;
            else 
             {
                avt_errmsg(SPE_ERRMSG, "014", AVT_FATAL, spef_ParsedFile, Line-1, nol_name, ios_name);      
                EXIT(4);
             }
            
            if (pos==1) *ptsig =lctc->SIG1, *node=lctc->NODE1, *pnode=&lctc->NODE1; else *ptsig =lctc->SIG2, *node=lctc->NODE2, *pnode=&lctc->NODE2;
          } // end zinaps
        else if((value = gethtitem(portmap_ht, ios_name/* = spef_spi_devect(ios_name)*/)) != EMPTYHT)
          {
            for (ptcon=Lofig->LOCON;ptcon!=NULL && spef_spi_devect(ptcon->NAME)!=ios_name;ptcon=ptcon->NEXT) ;
            if(!ptcon) return 0;
            *ptsig = ptcon->SIG;
            spef_checkpnode(ptcon, node);
          }
        else if (ground[0]!='\0' && ios_name==ground)
          {
            if (spef_groundlosig==NULL && (spef_groundlosig=spef_getlosigbyname(ios_name))==NULL)
              {
                // yes but the signal does not exist, let create it...
                tmp=spef_findindexinlofig(Lofig); // last valid index
                
                spef_groundlosig=addlosig(Lofig,tmp,addchain(NULL,ground),INTERNAL);
                spef_groundlosig->USER=addptype(spef_groundlosig->USER, LOFIGCHAIN, NULL);
                addlorcnet(spef_groundlosig);
                addhtitem_v2(all_lofig_losig,ground,(long)spef_groundlosig);          
                spef_groundlosig->ALIMFLAGS=MBK_HAS_GLOBAL_NODE_FLAG;
              }
            // all's right
            *ptsig=spef_groundlosig;
            spef_groundlosig->ALIMFLAGS|=MBK_ALIM_VSS_FLAG|MBK_SIGNAL_ALIM_DONE_FLAG;
            *node = 1;
            return 1;
          }
        else if(flag)
          {
            *ptsig = spef_getlosigbyname(ios_name);
            if(!(*ptsig))
              avt_errmsg(SPE_ERRMSG, "009", AVT_WARNING, spef_ParsedFile, Line-1, instorsig_name);
            *node = atol(nodeorlocon_name);
          }
        else
          {
            if (flagnodevect==1)
              ios_name = spef_spi_devect(instorsig_name);
            for (ptcon=Lofig->LOCON;ptcon!=NULL && ptcon->NAME!=ios_name;ptcon=ptcon->NEXT) ;
            if(!ptcon) return 0;
            *ptsig = ptcon->SIG;
            spef_checkpnode(ptcon, node);
          }
      }
        
    return 0;
}

/*******************************************************************/
int spef_compctc(ctc1,ctc2)
loctc_list *ctc1 ;
loctc_list *ctc2 ;
{
 if(((ctc1->SIG1->INDEX == ctc2->SIG1->INDEX) && 
     (ctc1->SIG2->INDEX == ctc2->SIG2->INDEX) &&
     (ctc1->NODE1 == ctc2->NODE1) &&
     (ctc1->NODE2 == ctc2->NODE2)) ||
    ((ctc1->SIG1->INDEX == ctc2->SIG2->INDEX) && 
     (ctc1->SIG2->INDEX == ctc2->SIG1->INDEX) &&
     (ctc1->NODE1 == ctc2->NODE2) &&
     (ctc1->NODE2 == ctc2->NODE1)))
    return(1) ;
 return(0) ;
}

/*******************************************************************/
char *spef_calcstringctc(ctc, name)
loctc_list *ctc ;
char *name ;
{

 if(ctc->SIG1->INDEX < ctc->SIG2->INDEX) 
 sprintf(name,"%ld.%ld/%ld.%ld",ctc->SIG1->INDEX,ctc->NODE1,
                                ctc->SIG2->INDEX,ctc->NODE2) ;
 else
 sprintf(name,"%ld.%ld/%ld.%ld",ctc->SIG2->INDEX,ctc->NODE2,
                                ctc->SIG1->INDEX,ctc->NODE1) ;

 return name;

}

/*******************************************************************/
loctc_list *spef_ctcalloc(sig1,sig2,node1,node2)
losig_list *sig1 ;
losig_list *sig2 ;
long node1 ;
long node2 ;
{
 chain_list *pt;
 chain_list *ptsav;
 chain_list *ptnext;
 loctc_list *ctc;
 char *inputname ;
 char name[1024];
 loctc_list ctctemp ;
 int code = 0 ;
 int i ;

 ctctemp.SIG1 = sig1 ;
 ctctemp.SIG2 = sig2 ;
 ctctemp.NODE1 = node1 ;
 ctctemp.NODE2 = node2 ;
 ctc = &ctctemp ;

 inputname = spef_calcstringctc(ctc,name) ;

   if (SPEF_HASHTABLE == NULL) {
     SPEF_HASHTABLE = mbkalloc(SPEF_HASHSIZE * sizeof(chain_list *));
   for (i = 0 ; i < SPEF_HASHSIZE ; i++)
       SPEF_HASHTABLE[i] = NULL ;
   }

      while (*inputname) {
         code += (code ^ (code >> 1)) + HASH_MULT * (unsigned char) *inputname++;
         while (code >= HASH_PRIME)
            code -= HASH_PRIME;
      }
      code %= SPEF_HASHSIZE ;

   for (pt = SPEF_HASHTABLE[code]; pt; pt = ptnext)
    {
      ptnext = pt->NEXT ;
      if (spef_compctc(ctc,(loctc_list *)pt->DATA) == 1)
        {
         if(pt == SPEF_HASHTABLE[code])
           {
            SPEF_HASHTABLE[code] = SPEF_HASHTABLE[code]->NEXT ;
           }
         else
           {
            ptsav->NEXT = pt->NEXT ;
           }
         pt->NEXT = NULL ;
         ctc = (loctc_list *)pt->DATA;
         freechain(pt) ;
         return (ctc) ;
        }
      else
        ptsav = pt ;
    }

   ctc = heaploctc() ;
   ctc->SIG1    = sig1 ;
   ctc->NODE1   = node1 ;
   ctc->SIG2    = sig2 ;
   ctc->NODE2   = node2 ;
   ctc->CAPA    = -1.0 ;
   ctc->USER    = NULL;

   SPEF_HASHTABLE[code] = addchain(SPEF_HASHTABLE[code], (void *)ctc);
   return (ctc);
}

/*******************************************************************/
chain_list *spef_addloctc(chain_list *ctclist,losig_list  *sig1,losig_list  *sig2,long node1,long node2,float value,loctc_list **lctc)
{
 loctc_list    *newctc = spef_ctcalloc(sig1,sig2,node1,node2), *crctc ;

 if( sig1->PRCN->NBNODE <= node1 )
    sig1->PRCN->NBNODE = node1+1;

 if( sig2->PRCN->NBNODE <= node2 )
     sig2->PRCN->NBNODE = node2+1;

 if(newctc->CAPA < 0.0)
   {
    newctc->CAPA = value ;
    crctc=addloctc(newctc->SIG1, newctc->NODE1, newctc->SIG2, newctc->NODE2, newctc->CAPA);
    rcn_addcapa( newctc->SIG1, newctc->CAPA );
    rcn_addcapa( newctc->SIG2, newctc->CAPA );
    ctclist = addchain(ctclist,(void *)newctc) ;
    *lctc=crctc;
   }
 else *lctc=NULL;
 return(ctclist) ;
}

/*******************************************************************/
void spef_treatafterctclist(chain_list **ctclist)
{
  loctc_list    *newctc;
  chain_list    *chain;
  chain_list    *chainnext;

  for(chain = *ctclist; chain; chain = chainnext){
      newctc = (loctc_list*)chain->DATA;
      //addloctc(newctc->SIG1, newctc->NODE1, newctc->SIG2, newctc->NODE2, newctc->CAPA);
      freeloctc(newctc);
      chainnext = chain->NEXT;
      chain->NEXT = NULL;
      freechain(chain);
  }
  *ctclist=NULL;
}

/*******************************************************************/
void spef_remove_htctc()
{
    long i;

    if(SPEF_HASHTABLE){
        for(i = 0; i < SPEF_HASHSIZE; i++)
            if(SPEF_HASHTABLE[i])
                freechain(SPEF_HASHTABLE[i]);
        mbkfree(SPEF_HASHTABLE);
    }
    SPEF_HASHTABLE = NULL;
}
/****************************************************************************/
void spef_initinfo(spef_info *info)
{
    info->SPEF = NULL;
    info->VENDOR = NULL;
    info->PROGRAM = NULL;
    info->VERSION = NULL;
    info->DESIGN_FLOW = NULL;
    info->DIVIDER = '|';
    info->DELIMITER = '.';
    info->PREFIX_BUS_DELIMITER = '[';
    info->SUFFIX_BUS_DELIMITER = ']';
    info->SPEF_T_UNIT = 'N';
    info->SPEF_T_SCALE = 1.0;
    info->SPEF_CAP_UNIT = 'P';
    info->SPEF_CAP_SCALE = 1.0;
    info->SPEF_RES_UNIT = 'O';
    info->SPEF_RES_SCALE = 1.0;
    info->SPEF_L_UNIT = 'H';
    info->SPEF_L_SCALE = 1.0;
    
}
/****************************************************************************/
void spef_createinfo(void)
{
    SPEF_INFO = (spef_info*)mbkalloc(sizeof (struct spef_info));
    SPEF_INFO->DESIGN_FLOW = NULL;
    SPEF_INFO->SPEF = namealloc("\"IEEE 1481-1998\"");
    SPEF_INFO->VENDOR = namealloc("AVERTEC");
    SPEF_INFO->PROGRAM = namealloc("spef_driver");
    SPEF_INFO->VERSION = namealloc("1.0");
    SPEF_INFO->DESIGN_FLOW = addchain(SPEF_INFO->DESIGN_FLOW, strdup("\"EXTERNAL LOADS\"  \"EXTERNAL_SLEWS\""));
    SPEF_INFO->DIVIDER = SEPAR;
    SPEF_INFO->DELIMITER = SEPAR;
    SPEF_INFO->PREFIX_BUS_DELIMITER = '[';
    SPEF_INFO->SUFFIX_BUS_DELIMITER = ']';
    SPEF_INFO->SPEF_T_UNIT = 'N';
    SPEF_INFO->SPEF_T_SCALE = 1.0;
    SPEF_INFO->SPEF_CAP_UNIT = 'P';
    SPEF_INFO->SPEF_CAP_SCALE = 1.0;
    SPEF_INFO->SPEF_RES_UNIT = 'O';
    SPEF_INFO->SPEF_RES_SCALE = 1.0;
    SPEF_INFO->SPEF_L_UNIT = 'H';
    SPEF_INFO->SPEF_L_SCALE = 1.0;
    
}

