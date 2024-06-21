/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include "fcl_headers.h"

void fclExit()
{
    EXIT(1);
}

fcl_label      CountChain(pthead)
chain_list *pthead;
{
    chain_list *pt;
    fcl_label      i = 0;

    for(pt=pthead;pt;pt=pt->NEXT,i++);

    return ((fcl_label) i);
}

fcl_label getlabellocon(ptlocon)
locon_list *ptlocon;
{
   if (ptlocon == NULL) return (fcl_label)0;
   if (ptlocon->TYPE == 'T') {
       if (ptlocon->NAME == MBK_GRID_NAME) return (fcl_label)POID_GRID;
       if (ptlocon->NAME == MBK_SOURCE_NAME) return (fcl_label)POID_SOURCE;
       if (ptlocon->NAME == MBK_DRAIN_NAME) return (fcl_label)POID_DRAIN;
       if (ptlocon->NAME == MBK_BULK_NAME) return (fcl_label)POID_BULK;
   }
   if (ptlocon->TYPE != INTERNAL) return (fcl_label)POID_PAD;
   if (ptlocon->FLAGS != 0) return (fcl_label)ptlocon->FLAGS;

   return (fcl_label)POID_INS;
}

fcl_label getlabeltrs(ptlotrs)
lotrs_list *ptlotrs;
{
   ptype_list *ptype;

   ptype = getptype(ptlotrs->USER,(long)FCL_LABEL_PTYPE); 

   if (ptype == (ptype_list *) NULL)
      return ((fcl_label) 1); 

   return ((fcl_label) (long)ptype->DATA);
}

fcl_label getlabelsig(ptlosig)
losig_list *ptlosig;
{
   ptype_list *ptype;

   ptype = getptype(ptlosig->USER,(long)FCL_LABEL_PTYPE); 

   if (ptype == (ptype_list *) NULL)
      return ((fcl_label) 0); 

   return ((fcl_label) (long)ptype->DATA);
}

void
fclCorruptLotrs(ptlotrs)
    lotrs_list *ptlotrs;
{
    ptype_list *ptuser;

    if ((ptuser = getptype(ptlotrs->USER, FCL_LABEL_PTYPE)) != NULL) {
        ptuser->DATA = (void *)FCL_CORRUPT;
    }
    else {
        ptlotrs->USER = addptype(ptlotrs->USER, FCL_LABEL_PTYPE, (void *)FCL_CORRUPT);
    }
}

void
fclCorruptLosig(ptlosig)
    losig_list *ptlosig;
{
    ptype_list *ptuser;

    if ((ptuser = getptype(ptlosig->USER, FCL_LABEL_PTYPE)) != NULL) {
        ptuser->DATA = (void *)FCL_CORRUPT;
    }
    else {
        ptlosig->USER = addptype(ptlosig->USER, FCL_LABEL_PTYPE, (void *)FCL_CORRUPT);
    }
}

fcl_label getlabelsigtrs(ptlotrs,ptlosig)
lotrs_list *ptlotrs;
losig_list *ptlosig;
{
   if (ptlotrs->DRAIN->SIG == ptlosig)
      return getlabellocon(ptlotrs->DRAIN);

   if (ptlotrs->GRID->SIG == ptlosig)
      return getlabellocon(ptlotrs->GRID);

   if (ptlotrs->SOURCE->SIG == ptlosig)
      return getlabellocon(ptlotrs->SOURCE);

   if (ptlotrs->BULK) {
       if (ptlotrs->BULK->SIG == ptlosig)
          return getlabellocon(ptlotrs->BULK);
   }

   return 0;
}

lotrs_list *whichtrs(ptlofig,ptlocon)
lofig_list *ptlofig;
locon_list *ptlocon;
{
    lotrs_list *ptr;

    for(ptr=ptlofig->LOTRS;ptr;ptr=ptr->NEXT)
    {
       if (ptr->DRAIN  == ptlocon) return ptr;
       if (ptr->SOURCE == ptlocon) return ptr;
       if (ptr->GRID   == ptlocon) return ptr;
       if (ptr->BULK   == ptlocon) return ptr;
    }

    return (lotrs_list *) NULL;
}

void
fclMarkTrans(transchain)
    chain_list *transchain;
{
    chain_list *ptchain;
    lotrs_list *pttrans;
    ptype_list *ptuser;
    
    for (ptchain = transchain; ptchain; ptchain = ptchain->NEXT) {
        pttrans = (lotrs_list *)ptchain->DATA;
        if ((ptuser = getptype(pttrans->USER, FCL_MARK_PTYPE)) != NULL) {
            ptuser->DATA  = (void *)((long)ptuser->DATA | FCL_USED);
        }
        else pttrans->USER = addptype(pttrans->USER, FCL_MARK_PTYPE, (void *)FCL_USED);
    }
}

void
fclUnmarkTrans(transchain)
    chain_list *transchain;
{
    chain_list *ptchain;
    lotrs_list *pttrans;
    long        mark;
    
    for (ptchain = transchain; ptchain; ptchain = ptchain->NEXT) {
        pttrans = (lotrs_list *)ptchain->DATA;
        mark = (long)getptype(pttrans->USER, FCL_MARK_PTYPE)->DATA;
        mark &= ~FCL_USED;
        if (mark == 0) pttrans->USER = delptype(pttrans->USER, FCL_MARK_PTYPE);
    }
}

void
fclCleanShareMarks(ptlofig)
    lofig_list *ptlofig;
{
    lotrs_list *ptlotrs;
    loins_list *ptloins;

    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        if (getptype(ptlotrs->USER, FCL_MARK_PTYPE) != NULL) {
            ptlotrs->USER = delptype(ptlotrs->USER, FCL_MARK_PTYPE);
        }
    } 
    for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
        if (getptype(ptloins->USER, FCL_MARK_PTYPE) != NULL) {
            ptloins->USER = delptype(ptloins->USER, FCL_MARK_PTYPE);
        }
    } 
}

void
fclCleanTransferMarks(ptlofig)
    lofig_list *ptlofig;
{
    ptype_list *ptuser;
    lotrs_list *ptlotrs;
    loins_list *ptloins;
    losig_list *ptlosig;

    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        if (getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE) != NULL) {
            ptlotrs->USER = delptype(ptlotrs->USER, FCL_TRANSFER_PTYPE);
        }
    } 
    for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
        if (getptype(ptloins->USER, FCL_TRANSFER_PTYPE) != NULL) {
            ptloins->USER = delptype(ptloins->USER, FCL_TRANSFER_PTYPE);
        }
    } 
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        if (getptype(ptlosig->USER, FCL_TRANSFER_PTYPE) != NULL) {
            ptlosig->USER = delptype(ptlosig->USER, FCL_TRANSFER_PTYPE);
        }
    } 
    if ((ptuser = getptype(ptlofig->USER, FCL_LOCON_PTYPE)) != NULL) {
        freechain((chain_list *)ptuser->DATA);
        ptlofig->USER = delptype(ptlofig->USER, FCL_LOCON_PTYPE);
    }
}

void
fclCleanCouplingMarks(ptlofig)
    lofig_list *ptlofig;
{
    losig_list *ptlosig;
    ptype_list *ptuser;

    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        if ((ptuser = getptype(ptlosig->USER, FCL_COUP_LIST_PTYPE)) != NULL) {
            freenum((num_list *)ptuser->DATA);
            ptlosig->USER = delptype(ptlosig->USER, FCL_COUP_LIST_PTYPE);
        }
    } 
}

void fclCleanTransList(ptloins)
    loins_list *ptloins;
{
    ptype_list *ptuser;

    if ((ptuser = getptype(ptloins->USER, FCL_TRANSLIST_PTYPE)) != NULL) {
        freechain((chain_list *)ptuser->DATA);
        ptloins->USER = delptype(ptloins->USER, FCL_TRANSLIST_PTYPE);
    }
    if ((ptuser = getptype(ptloins->USER, FCL_INSLIST_PTYPE)) != NULL) {
        freechain((chain_list *)ptuser->DATA);
        ptloins->USER = delptype(ptloins->USER, FCL_INSLIST_PTYPE);
    }

}

void
fclDeleteInstance(ptloins)
    loins_list *ptloins;
{
    ptype_list *ptuser;
    locon_list *ptlocon, *ptnextlocon;

    if ((ptuser = getptype(ptloins->USER, FCL_TRANSLIST_PTYPE)) != NULL) {
        freechain((chain_list *)ptuser->DATA);
        ptloins->USER = delptype(ptloins->USER, FCL_TRANSLIST_PTYPE);
    }
    if ((ptuser = getptype(ptloins->USER, FCL_INSLIST_PTYPE)) != NULL) {
        freechain((chain_list *)ptuser->DATA);
        ptloins->USER = delptype(ptloins->USER, FCL_INSLIST_PTYPE);
    }
    for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptnextlocon) {
        ptnextlocon = ptlocon->NEXT;
        mbkfree(ptlocon);
    }
    mbkfree(ptloins);
}

void
fclFreeCorrespList(ptlist)
    fclcorresp_list *ptlist;
{
    fclcorresp_list *ptcorresp;
    fclcorresp_list *ptnext;

    for (ptcorresp = ptlist; ptcorresp; ptcorresp = ptnext) {
        ptnext = ptcorresp->NEXT;
        mbkfree(ptcorresp);
    }
}

/* display system and user time */
extern long NUM_PTYPE;

void
fclPrintTime(start, end, rstart, rend)
    struct rusage  *start, *end;
    time_t          rstart, rend;

{
    unsigned long   temps;
    unsigned long   user, syst;
    unsigned long   userM, userS, userD;
    unsigned long   systM, systS, systD;
    unsigned long   bytes;

    temps = rend - rstart;
    user = (100 * end->ru_utime.tv_sec + (end->ru_utime.tv_usec / 10000))
        - (100 * start->ru_utime.tv_sec + (start->ru_utime.tv_usec / 10000));
    syst = (100 * end->ru_stime.tv_sec + (end->ru_stime.tv_usec / 10000))
        - (100 * start->ru_stime.tv_sec + (start->ru_stime.tv_usec / 10000));

    userM = user / 6000;
    userS = (user % 6000) / 100;
    userD = (user % 100) / 10;

    systM = syst / 6000;
    systS = (syst % 6000) / 100;
    systD = (syst % 100) / 10;

    fprintf(stdout, " %02ldm%02lds", (long) (temps / 60), (long) (temps % 60));
    fprintf(stdout, "  u:%02ldm%02ld.%ld", userM, userS, userD);
    bytes = mbkprocessmemoryusage();
    fprintf(stdout, "  M:%ldKb\n", bytes / 1024);
    fflush(stdout);
}

void
fclChrono(t, rt)
    struct rusage  *t;
    time_t         *rt;
{
    getrusage(RUSAGE_SELF, t);
    time(rt);
}


