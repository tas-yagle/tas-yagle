/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_transistor.c                                      */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 23/06/1999    */
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
#include MLU_H
#include MSL_H
#include API_H
#include "gen_model_utils.h"
#include "gen_model_transistor.h"
#include "gen_env.h"




/****************************************************************************/
/*                build the model for a transistor X                        */
/****************************************************************************/
extern lofig_list *Build_Transistor(char* name) {
  lofig_list *fig_t;
  chain_list *namechain;
  losig_list *source,*drain,*grid,*bulk;
  locon_list *con;
  int index;
  char type;
  long x=1,y=1;  /*no extraction->no location*/
  /*put to quite reasonable values*/
  unsigned short width=120, length=10, ps=280, pd=280, xs=20, xd=20;
  int lineno=0;  /* for PUT_ATO() */
  char* file=NULL;  /* for PUT_ATO() */

/* transistor X*/
   name=namealloc(name);
   fig_t=addlofig(name);
    
    index=INDEX_START;

/*bulk*/
    namechain=addchain(NULL,MBK_BULK_NAME);
    bulk=addlosig(fig_t,index++,namechain,EXTERNAL);     /*WARN:no more capa*/
    con=addlocon(fig_t,MBK_BULK_NAME,bulk,INOUT);/*default addlocon set EXTERNAL*/
    /*for GENIUS*/
    con->USER=addptype(con->USER,GENCONCHAIN,NULL);
    con->USER->DATA=addgenconchain(NULL, PUT_ATO(GEN_TOKEN_DIGIT,-1),
                                  bulk, PUT_ATO(GEN_TOKEN_DIGIT,-1));

/*drain*/
    namechain=addchain(NULL,MBK_DRAIN_NAME);
    drain=addlosig(fig_t,index++,namechain,EXTERNAL);     /*WARN:no more capa*/
    con=addlocon(fig_t,MBK_DRAIN_NAME,drain,INOUT);/*default addlocon set EXTERNAL*/
    /*for GENIUS*/
    con->USER=addptype(con->USER,GENCONCHAIN,NULL);
    con->USER->DATA=addgenconchain(NULL, PUT_ATO(GEN_TOKEN_DIGIT,-1),
                                  drain, PUT_ATO(GEN_TOKEN_DIGIT,-1));
                                  
/*source*/
    namechain=addchain(NULL,MBK_SOURCE_NAME);
    source=addlosig(fig_t,index++,namechain,EXTERNAL);    /*WARN:no more capa*/
    con=addlocon(fig_t,MBK_SOURCE_NAME,source,INOUT);/*default addlocon set to EXTERNAL*/
    /*for GENIUS*/
    con->USER=addptype(con->USER,GENCONCHAIN,NULL);
    con->USER->DATA=addgenconchain(NULL, PUT_ATO(GEN_TOKEN_DIGIT,-1),
                                 source, PUT_ATO(GEN_TOKEN_DIGIT,-1));
    
/*grid*/
    namechain=addchain(NULL,MBK_GRID_NAME);
    grid=addlosig(fig_t,index++,namechain,EXTERNAL);   /*WARN:no more capa*/
    con=addlocon(fig_t,MBK_GRID_NAME,grid,INOUT);  /*default addlocon set EXTERNAL*/
    /*for GENIUS*/
    con->USER=addptype(con->USER,GENCONCHAIN,NULL);
    con->USER->DATA=addgenconchain(NULL, PUT_ATO(GEN_TOKEN_DIGIT,-1),
                                   grid, PUT_ATO(GEN_TOKEN_DIGIT,-1));

    type = mbk_istransn(name)?TRANSN:TRANSP;
    fig_t->LOTRS=addlotrs(fig_t, type, /*MBK data*/
                         x,y,width,length,ps,pd,xs,xd,/*physical data*/
                         grid,source,drain,bulk,/*connexion data*/
                         namealloc("transistor") /*MBK data*/);
    addlotrsmodel(fig_t->LOTRS, name) ;
    return fig_t;
}


locon_list *myaddlocon(char *name, losig_list *ptsig, char dir, void *root)
{
  locon_list *ptcon;
  
  ptcon            = (locon_list *)mbkalloc(sizeof(locon_list));
  ptcon->NAME      = name;
  ptcon->TYPE      = 'T';
  ptcon->SIG       = ptsig;
  ptcon->ROOT      = root;
  ptcon->DIRECTION = dir;
  ptcon->USER      = NULL;
  ptcon->PNODE     = NULL;
  ptcon->NEXT=NULL;
  return ptcon;
}


extern lotrs_list *BuildFakeTransistor(char type, losig_list *grid, losig_list *drain, losig_list *source, losig_list *bulk)
 {
  locon_list *con;
  lotrs_list *tr;

  tr=(lotrs_list *)mbkalloc(sizeof(lotrs_list));
  /*drain*/
  con=myaddlocon(MBK_DRAIN_NAME, drain, INOUT, tr);
  tr->DRAIN=con;

  con=myaddlocon(MBK_DRAIN_NAME, source, INOUT, tr);
  tr->SOURCE=con;

  con=myaddlocon(MBK_DRAIN_NAME, grid, INOUT, tr);
  tr->GRID=con;

/*bulk*/
  if (SPI_IGNORE_BULK=='N')
    {
      con=myaddlocon(MBK_BULK_NAME, bulk, INOUT, tr);
      tr->BULK=con;
    }
  else
    tr->BULK=NULL;

  tr->TRNAME=FAKE_TRAN_NAME;
  tr->NEXT=NULL;
  tr->MODINDEX = EMPTYHT;
  tr->TYPE=type;
  tr->USER=NULL;

  ClearLOTRSLoconFlags(tr);

  return tr;
}
