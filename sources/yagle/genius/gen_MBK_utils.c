/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : genius_MBK_utils.c                                          */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 07/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include MUT_H
#include MLO_H
#include RCN_H
#include API_H
#include AVT_H
#include "gen_env.h"
#include "gen_model_utils.h"
#include "gen_corresp.h"
#include "gen_search_utils.h"
#include "gen_MBK_utils.h"




/*******************************************************************************
* function addloins()                                                          *
*******************************************************************************/
loins_list *gen_addloins(ptfig, insname, ptnewfig, sigchain, sigtreechain)
lofig_list *ptfig;
char *insname;
lofig_list *ptnewfig;
chain_list *sigchain,*sigtreechain;
{
  locon_list *ptcon = NULL;
  locon_list *ptnewcon = NULL;
  loins_list *ptins = NULL;
  char *figname;
  chain_list *ptchain;
  
  insname = namealloc(insname);
  figname = ptnewfig->NAME;
  
  /* check insname  unicity */
  if (strcmp(insname, "*") && FAST_MODE != 'Y') {
    for (ptins = ptfig->LOINS; ptins; ptins = ptins->NEXT) {
      if (ptins->INSNAME == insname) {
              avt_errmsg(GNS_ERRMSG, "001", AVT_FATAL, insname, ptfig->NAME);
/*    (void)fflush(stdout);
    (void)fprintf(stderr, "*** mbk error ***\n");
    (void)fprintf(stderr, "addloins impossible : ");
    (void)fprintf(stderr, "instance %s already exist in figure %s\n", 
              insname, ptfig->NAME);*/
    EXIT(1);
      }
    }
  }
  
  if (ptfig->NAME == figname) {
    avt_errmsg(GNS_ERRMSG, "002", AVT_FATAL, figname);
/*    (void)fflush(stdout);
    (void)fprintf(stderr, "*** mbk error ***\n");
    (void)fprintf(stderr, "addloins impossible :\n");
    (void)fprintf(stderr, "instance model is the figure %s itself\n", 
          figname);*/
    EXIT(1);
  }
  
  ptins = (loins_list *)mbkalloc(sizeof(loins_list));
  
  ptins->INSNAME  = insname;
  ptins->FIGNAME  = figname;
  ptins->LOCON   = NULL;
  ptins->USER    = NULL;
  ptins->NEXT    = ptfig->LOINS;
  ptfig->LOINS   = ptins;
  
  /* update model list   */
  for (ptchain = ptfig->MODELCHAIN; ptchain; ptchain = ptchain->NEXT)
    if (ptchain->DATA == (void *)figname)
      break;
  
  if (!ptchain)
    ptfig->MODELCHAIN = addchain(ptfig->MODELCHAIN, (void *)figname);
  
  for (ptcon = ptnewfig->LOCON; ptcon; ptcon = ptcon->NEXT) {
    /*skip all bit vector from Alliance*/
    if (ptcon->NEXT && 
    vectorradical(ptcon->NAME)==vectorradical(ptcon->NEXT->NAME)) continue;
    
    if (sigchain == NULL || sigtreechain == NULL) {
      avt_errmsg(GNS_ERRMSG, "003", AVT_FATAL, ptnewfig->NAME, insname, ptfig->NAME);
/*      (void)fflush(stdout);
      (void)fprintf(stderr, "*** mbk error ***\n");
      (void)fprintf(stderr, "addloins impossible : connector number ");
      (void)fprintf(stderr, "discrepancy between figure %s", 
            ptnewfig->NAME);
      (void)fprintf(stderr, " and instance %s in figure %s\n",
            insname, ptfig->NAME);*/
      EXIT(1);
    }
    ptnewcon = (locon_list *)mbkalloc(sizeof(locon_list));
    ptnewcon->NAME = vectorradical(ptcon->NAME);
    ptnewcon->DIRECTION = ptcon->DIRECTION;
    ptnewcon->TYPE = 'I';
    ptnewcon->SIG  = (losig_list *)sigchain->DATA;
    ptnewcon->ROOT = (void *)ptins;
    ptnewcon->USER = NULL;
    ptnewcon->PNODE= NULL;
    ptnewcon->NEXT = ptins->LOCON;
    ptins->LOCON  = ptnewcon;
    sigchain = sigchain->NEXT;
    
    /*GENIUS lofigchain*/
    {
      tree_list *tree_con,*tree_sig;
      ptype_list *p;
      losig_list *sig;
      int lineno;
      char* file;
      
      p = getptype(ptcon->USER, GENCONCHAIN);
      if (!p) {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 1);
//    fprintf(stderr,"gen_addloins: PTYPE not found on locon %s for loins %s\n",ptcon->NAME,insname);
    EXIT(1);
      }
      /* TOKEN(tree_con)=GEN_TOKEN_DIGIT or TOKEN(tree_con)=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO */
      tree_con = ((genconchain_list*)p->DATA)->LOCON_EXP;
      tree_con = Duplicate_Tree(tree_con);
      tree_sig = sigtreechain->DATA;   /* TOKEN=GEN_TOKEN_IDENT or TOKEN='(' */
      if (TOKEN(tree_sig)=='(') {
    tree_sig=tree_sig->NEXT->NEXT->DATA;      /* TOKEN=GEN_TOKEN_TO or GEN_TOKEN_DOWNTO */
    tree_sig=Duplicate_Tree(tree_sig);
      }
      else {
    file=FILE_NAME(tree_sig);
    lineno=LINE(tree_sig);
    tree_sig=PUT_ATO(GEN_TOKEN_DIGIT,-1);
      }
      sig = ptnewcon->SIG;
      
      ptnewcon->USER = addptype(ptnewcon->USER,GENCONCHAIN,NULL);
      ptnewcon->USER->DATA = addgenconchain(NULL, tree_con, sig, tree_sig);
      
      p = getptype(sig->USER, GENSIGCHAIN);
      if (!p) {
    sig->USER = addptype(sig->USER,GENSIGCHAIN,NULL);
    p = sig->USER;
      }
      tree_con = Duplicate_Tree(tree_con);
      tree_sig = Duplicate_Tree(tree_sig);
      p->DATA = addgensigchain( p->DATA, tree_sig, ptnewcon, tree_con);
      sigtreechain=sigtreechain->NEXT;
    }/*end lofigchain GENIUS*/
  }
  if (sigchain != NULL || sigtreechain != NULL) {
      avt_errmsg(GNS_ERRMSG, "003", AVT_FATAL, ptnewfig->NAME, insname, ptfig->NAME);
/*    (void)fflush(stdout);
    (void)fprintf(stderr, "*** mbk error ***\n");
    (void)fprintf(stderr, "addloins impossible : ");
    (void)fprintf(stderr, "connector number discrepancy between figure %s ", 
          ptnewfig->NAME);
    (void)fprintf(stderr, "and instance %s in figure %s\n",
          insname, ptfig->NAME);*/
    EXIT(1);
  }
  
  ptins->LOCON = (locon_list *)reverse((chain_list *)ptins->LOCON);
  
  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- addloins  : %s of figure %s\n", 
          insname, figname);
  return ptins;
}


/*******************************************************************************
* function Dump_loinscon()                                                      *
*******************************************************************************/
extern void Dump_loinscon(ptcon)
locon_list  *ptcon;
{
    num_list    *scannum;
        ptype_list      *scanptype;
        chain_list      *scanchain;

if (ptcon->NAME == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |---no connector name\n");
else
    (void)fprintf(GENIUS_OUTPUT,"   |    |---connector  : \"%s\"\n" , ptcon->NAME);
    (void)fprintf(GENIUS_OUTPUT,"   |    |    |---direction  : %c\n" , ptcon->DIRECTION);
if (ptcon->SIG == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |    |---no signal\n");
else
    (void)fprintf(GENIUS_OUTPUT,"   |    |    |---signal    : %ld\n" , ptcon->SIG->INDEX);
if ((ptcon->ROOT) == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |    |---no root\n");
else
    (void)fprintf(GENIUS_OUTPUT,"   |    |    |---root    : %s\n" , ((char*)(((loins_list *)(ptcon->ROOT))->INSNAME)));
    (void)fprintf(GENIUS_OUTPUT,"   |    |    |---type    : %c\n" , ptcon->TYPE);
    if(ptcon->PNODE == NULL)
        (void)fprintf(GENIUS_OUTPUT,"   |    |    |---no node\n");
    else {
        (void)fprintf(GENIUS_OUTPUT,"   |    |    |---node\n");
        for(scannum = ptcon->PNODE; scannum; scannum = scannum->NEXT )
            (void)fprintf(GENIUS_OUTPUT,"   |    |    |    |---%ld\n",scannum->DATA);
    }
    (void)fprintf(GENIUS_OUTPUT,"   |    |    |---USER list\n");
        for( scanptype = ptcon->USER ; scanptype ; scanptype = scanptype->NEXT )
        {
                switch( scanptype->TYPE ) {
                case LOFIGCHAIN:
                fprintf(GENIUS_OUTPUT,"   |    |    |    |-LOFIGCHAIN\n" );
                 for( scanchain = (chain_list*)scanptype->DATA ; scanchain ; scanchain = scanchain->NEXT ) {
                  chain_list *namechain=((losig_list*)scanchain->DATA)->NAMECHAIN;
                    fprintf(GENIUS_OUTPUT,"   |    |    |    |   |-%s\n",namechain?(char*)namechain->DATA:" *no name* ");
               }
                        break;
                case GENSIGCHAIN: case GEN_MARK_PTYPE:
                  fprintf(stderr,"Dump_loinscon: illegal ptype\n");
                  EXIT(1);
                  break;
                case GENCONCHAIN:
                { genconchain_list *genchain;
                  chain_list *namechain;
                fprintf(GENIUS_OUTPUT,"   |    |    |    |-LOFIGCHAIN\n" );
                 for( genchain = (genconchain_list*)scanptype->DATA ; genchain ; genchain = genchain->NEXT ) {
                       fprintf(GENIUS_OUTPUT,"   |    |    |    |   |-");
                  fprintf(GENIUS_OUTPUT,"%s",ptcon->NAME);
                  if(genchain->LOCON_EXP) {
                     fprintf(GENIUS_OUTPUT,"(");
                     Dump_Tree(genchain->LOCON_EXP);
                     fprintf(GENIUS_OUTPUT,")");
                  }   
                  namechain=genchain->LOSIG->NAMECHAIN;
                  fprintf(GENIUS_OUTPUT," => %s",namechain?(char*)namechain->DATA:" *no name* ");
                  if(genchain->LOSIG_EXP) {
                     fprintf(GENIUS_OUTPUT,"(");
                     Dump_Tree(genchain->LOSIG_EXP);
                     fprintf(GENIUS_OUTPUT,")");
                  }   
                  fprintf(GENIUS_OUTPUT,"\n");
               }}
                        break;
                case PNODENAME:
                (void)fprintf(GENIUS_OUTPUT,"   |    |    |    |---PNODENAME\n");
                        for( scanchain = (chain_list*)(scanptype->DATA);
                             scanchain;
                             scanchain = scanchain->NEXT )
                        (void)fprintf(GENIUS_OUTPUT,"   |    |    |    |   |-%s\n", (char*)(scanchain->DATA)?(char*)(scanchain->DATA):"NULL" );
                        break;
                default:
                (void)fprintf(GENIUS_OUTPUT,"   |    |    |    |---%ld\n",scanptype->TYPE);
                }
        }
    (void)fprintf(GENIUS_OUTPUT,"   |    | \n");
}


/*******************************************************************************
* function Dump_lofigcon()                                                      *
*******************************************************************************/
extern void Dump_lofigcon(ptcon)
locon_list  *ptcon; 
{ 
    num_list    *scannum;
        ptype_list      *scanptype;
        chain_list      *scanchain;

    if (ptcon->NAME == NULL)
        (void)fprintf(GENIUS_OUTPUT,"   |--- !!! connector without name !!!\n");
    else
        (void)fprintf(GENIUS_OUTPUT,"   |---connector  : \"%s\"\n", ptcon->NAME);
    (void)fprintf(GENIUS_OUTPUT,"   |    |---direction  : %c\n", ptcon->DIRECTION);
    if (ptcon->SIG == NULL)
        (void)fprintf(GENIUS_OUTPUT,"   |    |---no signal\n");
    else
        (void)fprintf(GENIUS_OUTPUT,"   |    |---signal    : %ld\n", ptcon->SIG->INDEX);
    if (ptcon->ROOT == NULL)
        (void)fprintf(GENIUS_OUTPUT,"   |---no root\n");
    else
        (void)fprintf(GENIUS_OUTPUT,"   |    |---root    : %s\n",
                                    ((char *)(((lofig_list *)(ptcon->ROOT))->NAME)));
    (void)fprintf(GENIUS_OUTPUT,"   |    |---type    : %c \n", ptcon->TYPE);

    (void)fprintf(GENIUS_OUTPUT,"   |    |---USER list\n");
        for( scanptype = ptcon->USER ; scanptype ; scanptype = scanptype->NEXT )
        {
                switch( scanptype->TYPE )
                {
                case LOFIGCHAIN:
                fprintf(GENIUS_OUTPUT,"   |    |    |-LOFIGCHAIN\n" );
                 for( scanchain = (chain_list*)scanptype->DATA ; scanchain ; scanchain = scanchain->NEXT ) {
                    chain_list *namechain=((losig_list*)scanchain->DATA)->NAMECHAIN;
               fprintf(GENIUS_OUTPUT,"   |    |    |   |-%s\n",namechain?(char*)namechain->DATA:" *no name* ");
               }
                        break;
                case GENSIGCHAIN: case GEN_MARK_PTYPE:
                  fprintf(stderr,"Dump_lofigcon: illegal ptype\n");
                  EXIT(1);
                  break;
                case GENCONCHAIN:
                { genconchain_list *genchain;
                  chain_list *namechain;
                fprintf(GENIUS_OUTPUT,"   |    |    |-LOFIGCHAIN\n" );
                 for( genchain = (genconchain_list*)scanptype->DATA ; genchain ; genchain = genchain->NEXT ) {
                       fprintf(GENIUS_OUTPUT,"   |    |    |   |-");
                  fprintf(GENIUS_OUTPUT,"%s",ptcon->NAME);
                  if(genchain->LOCON_EXP) {
                     fprintf(GENIUS_OUTPUT,"(");
                     Dump_Tree(genchain->LOCON_EXP);
                     fprintf(GENIUS_OUTPUT,")");
                  }   
                  namechain=genchain->LOSIG->NAMECHAIN;
                  fprintf(GENIUS_OUTPUT," => %s",namechain?(char*)namechain->DATA:" *no name* ");
                  if(genchain->LOSIG_EXP) {
                     fprintf(GENIUS_OUTPUT,"(");
                     Dump_Tree(genchain->LOSIG_EXP);
                     fprintf(GENIUS_OUTPUT,")");
                  }   
                  fprintf(GENIUS_OUTPUT,"\n");
               }}
                        break;
                case PNODENAME:
                (void)fprintf(GENIUS_OUTPUT,"   |    |    |---PNODENAME\n");
                        for( scanchain = (chain_list*)(scanptype->DATA);
                             scanchain;
                             scanchain = scanchain->NEXT )
                        (void)fprintf(GENIUS_OUTPUT,"   |    |    |   |---%s\n", (char*)(scanchain->DATA)?(char*)(scanchain->DATA):"NULL" );
                        break;
                default:
                (void)fprintf(GENIUS_OUTPUT,"   |    |    |    |---%ld\n",scanptype->TYPE);
                }
        }

    if(ptcon->PNODE == NULL)
        (void)fprintf(GENIUS_OUTPUT,"   |    |---no node\n");
    else {
        (void)fprintf(GENIUS_OUTPUT,"   |    |---node\n");
        for(scannum = ptcon->PNODE; scannum; scannum = scannum->NEXT )
            (void)fprintf(GENIUS_OUTPUT,"   |    |    |---%ld\n",scannum->DATA);

    }

    (void)fprintf(GENIUS_OUTPUT,"   |\n");
}


/*******************************************************************************
* function Dump_losig()                                                         *
*******************************************************************************/
extern void Dump_losig(ptsig)
losig_list  *ptsig;
{ 
chain_list  *ptchain;
lowire_list *scanwire;
chain_list  *scanctc;
loctc_list  *ptctc;
char         levelrc;
ptype_list  *ptptype;
mark_list *mark;

    (void)fprintf(GENIUS_OUTPUT,"   |---signal    : %ld \n" , ptsig->INDEX);
    if (ptsig->NAMECHAIN == NULL)
        (void)fprintf(GENIUS_OUTPUT,"   |    |---no alias names\n");
    else for (ptchain = ptsig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {  
          if (ptchain->DATA == NULL)
            (void)fprintf(GENIUS_OUTPUT,"   |    |--- !!! empty name !!!\n");
          else
            (void)fprintf(GENIUS_OUTPUT,"   |    |---alias   : \"%s\"\n", (char *)ptchain->DATA);
    }
    (void)fprintf(GENIUS_OUTPUT,"   |    |---type    : %c\n" , ptsig->TYPE);

    levelrc = rclevel( ptsig );
    
    if( levelrc == MBK_RC_A )
        (void)fprintf(GENIUS_OUTPUT,"   |    |---no parasitic data\n");
    else {
        (void)fprintf(GENIUS_OUTPUT,"   |    |---parasitic data\n");
        (void)fprintf(GENIUS_OUTPUT,"   |    |    |---capacitance : %g\n" , rcn_getcapa(NULL,ptsig));
        if( levelrc == MBK_RC_C || levelrc == MBK_RC_E ) {
            (void)fprintf(GENIUS_OUTPUT,"   |    |    |---wire list\n");
            for( scanwire = ptsig->PRCN->PWIRE ; scanwire != NULL ; scanwire = scanwire->NEXT )
                            (void)fprintf(GENIUS_OUTPUT,"   |    |    |    |- n1=%3ld n2=%3ld r=%g c=%g\n",
                             scanwire->NODE1,
                             scanwire->NODE2,
                             scanwire->RESI,
                             scanwire->CAPA   );
        }
        else
            (void)fprintf(GENIUS_OUTPUT,"   |    |    |---no wire\n");
        if( levelrc == MBK_RC_D || levelrc == MBK_RC_E ) {
            (void)fprintf(GENIUS_OUTPUT,"   |    |    |---crosstalk capacitance list\n");
            for( scanctc = ptsig->PRCN->PCTC ; scanctc != NULL ; scanctc = scanctc->NEXT ) {
                                ptctc = (loctc_list*)scanctc->DATA;
                (void)fprintf(GENIUS_OUTPUT,"   |    |    |    |-S1=%3ld N1=%3ld S2=%3ld N2=%3ld Capa=%g.\n", 
                                             ptctc->SIG1->INDEX,
                                             ptctc->NODE1,
                                             ptctc->SIG2->INDEX,
                                             ptctc->NODE2,
                                             ptctc->CAPA
                                             );
                        }
                }
        else
            (void)fprintf(GENIUS_OUTPUT,"   |    |    |---no crosstalk capacitance\n");
        (void)fprintf(GENIUS_OUTPUT,"   |    |    |---%ld node\n",ptsig->PRCN->NBNODE);
    }
    (void)fprintf(GENIUS_OUTPUT,"   |    |---USER list\n");
        for( ptptype = ptsig->USER ; ptptype ; ptptype = ptptype->NEXT )
        {
                switch( ptptype->TYPE )
                {
                case LOFIGCHAIN:
                fprintf(GENIUS_OUTPUT,"   |    |    |-LOFIGCHAIN\n" );
                 for( ptchain = (chain_list*)ptptype->DATA ; ptchain ; ptchain = ptchain->NEXT )
                    fprintf(GENIUS_OUTPUT,"   |    |    |   |-%s => %s\n",(char*)(ptsig->NAMECHAIN?ptsig->NAMECHAIN->DATA:"*no name*"),((locon_list*)ptchain->DATA)->NAME);
                        break;
                case GENCONCHAIN:
                  fprintf(stderr,"Dump_losig: illegal ptype\n");
                  EXIT(1);
                  break;
                case GENSIGCHAIN:
                { gensigchain_list *genchain;
                fprintf(GENIUS_OUTPUT,"   |    |    |-LOFIGCHAIN\n" );
                 for( genchain = (gensigchain_list*)ptptype->DATA ; genchain ; genchain = genchain->NEXT ) {
                       fprintf(GENIUS_OUTPUT,"   |    |    |   |-%s",
                 (char*)(ptsig->NAMECHAIN?ptsig->NAMECHAIN->DATA:"*no name*"));
                  if(genchain->LOSIG_EXP) {
                     fprintf(GENIUS_OUTPUT,"(");
                     Dump_Tree(genchain->LOSIG_EXP);
                     fprintf(GENIUS_OUTPUT,")");
                  }   
                  fprintf(GENIUS_OUTPUT," => %s",genchain->LOCON->NAME);
                  if(genchain->LOCON_EXP) {
                     fprintf(GENIUS_OUTPUT,"(");
                     Dump_Tree(genchain->LOCON_EXP);
                     fprintf(GENIUS_OUTPUT,")");
                  }   
                  fprintf(GENIUS_OUTPUT," %p",genchain->LOCON);
                  fprintf(GENIUS_OUTPUT,"\n");
                  }}
                        break;
                case GEN_MARK_PTYPE:
                  for (mark=(mark_list*)ptptype->DATA; mark; mark=mark->NEXT) 
                          fprintf(GENIUS_OUTPUT,"   |    |    |   |-MARK %d -> losig %p  %s\n",mark->BIT,mark->LOSIG,mark->VISITED?"#visited#":"#not visited#");
                break;
                default:
                break;
                }
        }
    {
    }
    (void)fprintf(GENIUS_OUTPUT,"   | \n");
}


/*******************************************************************************
* function Dump_loins()                                                         *
*******************************************************************************/
extern void Dump_loins(loins_list  *ptins)
{ 
locon_list  *ptcon;
chain_list *scanchain;
ptype_list *pt;

if (ptins->INSNAME == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |--- !!! no instance name !!!\n");
else
    (void)fprintf(GENIUS_OUTPUT,"   |---instance   : %s\n", ptins->INSNAME);
if (ptins->FIGNAME == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |--- !!! no instance model    !!!\n");
else
    (void)fprintf(GENIUS_OUTPUT,"   |    |---model     : %s \n" , ptins->FIGNAME);
if (ptins->LOCON == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |---empty list of connectors\n");
else
{
  for (ptcon = ptins->LOCON; ptcon != NULL; ptcon = ptcon->NEXT)
    Dump_loinscon(ptcon);
  pt = getptype( ptins->USER, PH_INTERF );
  if( pt )
  {
    (void)fprintf(GENIUS_OUTPUT,"   |    |---connector list physical order\n");   
    for( scanchain = (chain_list*)(pt->DATA); scanchain ; scanchain = scanchain->NEXT )
      (void)fprintf(GENIUS_OUTPUT,"   |    |    |---%s\n",(char*)(scanchain->DATA) );   
      (void)fprintf(GENIUS_OUTPUT,"   |    |\n");   
  }
}
if (ptins->USER != NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |---non empty USER field\n");
    (void)fprintf(GENIUS_OUTPUT,"   | \n");
}


/*******************************************************************************
* function Dump_lotrs()                                                         *
*******************************************************************************/
extern void Dump_lotrs(pttrs)
lotrs_list  *pttrs;
{ 

    (void)fprintf(GENIUS_OUTPUT,"   |---transistor\n");
if (MLO_IS_TRANSN(pttrs->TYPE))
    (void)fprintf(GENIUS_OUTPUT,"   |    |---type  : TRANSN \n");
else if (MLO_IS_TRANSP(pttrs->TYPE))
    (void)fprintf(GENIUS_OUTPUT,"   |    |---type  : TRANSP \n");
else 
    (void)fprintf(GENIUS_OUTPUT,"   |    |---illegal type\n");
if (pttrs->TRNAME != NULL )
    (void)fprintf(GENIUS_OUTPUT,"   |    |---name : %s\n", pttrs->TRNAME );
else
    (void)fprintf(GENIUS_OUTPUT,"   |    |---no name\n" );

if (pttrs->SOURCE == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |--- !!! no signal on source !!! \n");
else
        Dump_loinscon( pttrs->SOURCE );

if (pttrs->GRID == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |--- !!! no signal on grid !!!\n");
else
        Dump_loinscon( pttrs->GRID );

if (pttrs->DRAIN == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |--- !!! no signal on drain !!!\n");
else
        Dump_loinscon( pttrs->DRAIN );

if (pttrs->BULK == NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |--- !!! no signal on bulk !!!\n");
else
        Dump_loinscon( pttrs->BULK );

    (void)fprintf(GENIUS_OUTPUT,"   |    |---x      : %ld\n", pttrs->X);
    (void)fprintf(GENIUS_OUTPUT,"   |    |---y      : %ld\n", pttrs->Y);
    (void)fprintf(GENIUS_OUTPUT,"   |    |---width  : %ld\n", pttrs->WIDTH);
    (void)fprintf(GENIUS_OUTPUT,"   |    |---length : %ld\n", pttrs->LENGTH);
    (void)fprintf(GENIUS_OUTPUT,"   |    |---ps     : %ld\n", pttrs->PS);
    (void)fprintf(GENIUS_OUTPUT,"   |    |---pd     : %ld\n", pttrs->PD);
    (void)fprintf(GENIUS_OUTPUT,"   |    |---xs     : %ld\n", pttrs->XS);
    (void)fprintf(GENIUS_OUTPUT,"   |    |---xd     : %ld\n", pttrs->XD);
if (pttrs->USER != NULL)
    (void)fprintf(GENIUS_OUTPUT,"   |    |---non empty USER field\n");
    (void)fprintf(GENIUS_OUTPUT,"   |\n");
}

void transfert_needed_lofigs(lofig_list *lf, mbkContext *ctx)
{
  loins_list *li;
  lofig_list *nlf;
  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      if ((nlf=external_getlofig(li->FIGNAME))!=NULL)
        {
          mbkSwitchContext(ctx);
          if (getloadedlofig(li->FIGNAME)==NULL)
            {
              mbkSwitchContext(ctx);
              addhtitem(ctx->HT_LOFIG, nlf->NAME, (long)nlf); // duplofig??
              transfert_needed_lofigs(nlf, ctx);
            }
          else
            mbkSwitchContext(ctx);
        }
    }
}

