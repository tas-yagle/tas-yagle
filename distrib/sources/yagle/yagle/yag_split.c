/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_split.c                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 01/08/1999     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static void transferLotrsInsfig(loins_list *ptgnsloins, lofig_list *ptinsfig, lofig_list *ptlofig, int copytrans);
static void transferLoinsInsfig(loins_list *ptgnsloins, lofig_list *ptinsfig, lofig_list *ptlofig, int copytrans);
static void createTopLosig(loins_list *ptloins, lofig_list *pttoplofig);
static lofig_list *createTopLofig(lofig_list *ptlofig, char *name);
static losig_list *getExtSigFromName(lofig_list *ptlofig, char *name);
static lofig_list *giveInstanceLofig(loins_list *ptloins, int copytrans);
static losig_list *giveLoinsLosig(lofig_list *ptinsfig, losig_list *ptoldsig);

static long topsigindex;
static long inssigindex;
static ht *inssight;
static chain_list *insextsigchain;

/* connector direction bit patterns */
/* bit 0 : input                    */
/* bit 1 : output                   */
/* bit 2 : not tristate             */

#define HCON_IN        1
#define HCON_OUT       6
#define HCON_INOUT     7
#define HCON_OUTHZ     2
#define HCON_INOUTHZ   3


/****************************************************************************
 *                         function yagCutLofig()                           *
 ****************************************************************************/
        /* create hierarchical figure from GENIUS or FCL instances */

lofig_list *
yagCutLofig(inffig_list *ifl, lofig_list *ptlofig, chain_list *instances, lofig_list **ptpttopfig, int copytrans)
{
    chain_list *ptchain;
    loins_list *ptloins;
    loins_list *ptprevloins, *ptnextloins;
    lofig_list *ptinsfig;
    losig_list *pttopsig;
    losig_list *ptlosig;
    losig_list *ptprevsig;
    locon_list *ptlocon;
    chain_list *sigchain;
    chain_list *savechain;
    ptype_list *ptuser;
    lotrs_list *ptlotrs;
    lotrs_list *ptprevlotrs, *ptnextlotrs;
    locon_list *ptnextlocon;
    locon_list *oldloconlist;
    loins_list *ptcoreins;
    losig_list *sigarray[4];
    char       *name;
    char        buf[YAGBUFSIZE];
    int         i;

    /* remove the rcnet */
    /*
    lofigchain( ptlofig );
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        if (ptlosig->PRCN) freelorcnet(ptlosig);
        ptlosig->PRCN = NULL;
    }
    */

    /* remove the old lofigchain */

    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        if ((ptuser = getptype(ptlosig->USER, LOFIGCHAIN)) != NULL) {
            freechain(ptuser->DATA);
            ptlosig->USER = delptype(ptlosig->USER, LOFIGCHAIN);
        }
    }

    /* remove power supply markings */

    for (ptlocon = ptlofig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = ptlocon->SIG;
        if (ptlosig->TYPE != 'E') {
            ptlosig->TYPE = 'E';
            ptlocon->DIRECTION = 'I';
        }
    }

    if (getptype(ptlofig->USER, PH_INTERF) != NULL) {
        ptlofig->USER = delptype(ptlofig->USER, PH_INTERF);
    }
    if (getptype(ptlofig->USER, PH_REAL_INTERF) != NULL) {
        ptlofig->USER = delptype(ptlofig->USER, PH_REAL_INTERF);
    }

    name = ptlofig->NAME;
    delhtitem(HT_LOFIG, name);

    strcpy(buf, name);
    strcat(buf, "_yagcore");
    ptlofig->NAME = namealloc(buf);
    addhtitem(HT_LOFIG, ptlofig->NAME, (long)ptlofig);

    strcpy(buf, name);
    strcat(buf, "_yaglo");
    *ptpttopfig = createTopLofig(ptlofig, namealloc(buf));

    insextsigchain = NULL;

    /* traverse the instances to create corresponding figures */
    /* corresponding transistors are removed from core figure  */

    yagInitLosigVect();
    for (ptchain = instances; ptchain; ptchain = ptchain->NEXT) {
        inssigindex = 1;
        ptloins = (loins_list *)ptchain->DATA;
        createTopLosig(ptloins, *ptpttopfig);
        sigchain = NULL;
        for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            /* remove power supply markings */
            if (ptlocon->DIRECTION == 'D' || ptlocon->DIRECTION == 'S') ptlocon->DIRECTION = 'I';

            ptlosig = ptlocon->SIG;
            pttopsig = (losig_list *)getptype(ptlosig->USER, YAG_TOPSIG_PTYPE)->DATA;
            sigchain = addchain(sigchain, pttopsig);
        }
        sigchain = reverse(sigchain);
        ptinsfig = giveInstanceLofig(ptloins, copytrans);
        addloins(*ptpttopfig, ptloins->INSNAME, ptinsfig, sigchain);
        transferLotrsInsfig(ptloins, ptinsfig, ptlofig, copytrans);
        transferLoinsInsfig(ptloins, ptinsfig, ptlofig, copytrans);
    }
    yagCloseLosigVect();

    /* delete transistors marked for deletion */
    ptprevlotrs = NULL;
    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptnextlotrs) {
        ptnextlotrs = ptlotrs->NEXT;
        if (ptlotrs->TYPE == 0) {
            if (ptprevlotrs == NULL) ptlofig->LOTRS = ptlotrs->NEXT;
            else ptprevlotrs->NEXT = ptlotrs->NEXT;
            delloconuser( ptlotrs->GRID );
            mbkfree(ptlotrs->GRID);
            delloconuser( ptlotrs->SOURCE );
            mbkfree(ptlotrs->SOURCE);
            delloconuser( ptlotrs->DRAIN );
            mbkfree(ptlotrs->DRAIN);
            if( ptlotrs->BULK ) {
                delloconuser( ptlotrs->BULK );
                mbkfree( ptlotrs->BULK );
            }
            mbkfree(ptlotrs);
        }
        else ptprevlotrs = ptlotrs;
    }

    /* delete instances marked for deletion */
    ptprevloins = NULL;
    for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptnextloins) {
        ptnextloins = ptloins->NEXT;
        if (ptloins->FIGNAME == NULL) {
            if (ptprevloins == NULL) ptlofig->LOINS = ptloins->NEXT;
            else ptprevloins->NEXT = ptloins->NEXT;
            for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptnextlocon) {
                ptnextlocon = ptlocon->NEXT;
                delloconuser(ptlocon);
                mbkfree(ptlocon);
            }
            mbkfree(ptloins);
        }
        else ptprevloins = ptloins;
    }

    /* traverse transistors in core figure to identify external connections */

    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {

        sigarray[0] = ptlotrs->GRID->SIG;
        sigarray[1] = ptlotrs->SOURCE->SIG;
        sigarray[2] = ptlotrs->DRAIN->SIG;
        if (ptlotrs->BULK != NULL) {
            sigarray[3] = ptlotrs->BULK->SIG;
        }
        else sigarray[3] = NULL;

        for (i = 0; i < 4; i++) {
            ptlosig = sigarray[i];
            if (ptlosig == NULL) continue;
            if (ptlosig->TYPE == 'I' && getptype(ptlosig->USER, YAG_TOPSIG_PTYPE) != NULL) {
                ptlosig->TYPE ='E';
                addlocon(ptlofig, ptlosig->NAMECHAIN->DATA, ptlosig, 'X');
                if (getptype(ptlosig->USER, YAG_MARKSIG_PTYPE) == NULL) {
                    ptlosig->USER = addptype(ptlosig->USER, YAG_MARKSIG_PTYPE, NULL);
                }
            }
            if (ptlosig->TYPE != 'I' ) {
                if (getptype(ptlosig->USER, YAG_MARKSIG_PTYPE) == NULL) {
                    ptlosig->USER = addptype(ptlosig->USER, YAG_MARKSIG_PTYPE, NULL);
                }
            }
        }
    }
    for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
        for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            ptlosig = ptlocon->SIG;
            if (ptlosig->TYPE == 'I' && getptype(ptlosig->USER, YAG_TOPSIG_PTYPE) != NULL) {
                ptlosig->TYPE ='E';
                addlocon(ptlofig, ptlosig->NAMECHAIN->DATA, ptlosig, 'X');
                if (getptype(ptlosig->USER, YAG_MARKSIG_PTYPE) == NULL) {
                    ptlosig->USER = addptype(ptlosig->USER, YAG_MARKSIG_PTYPE, NULL);
                }
            }
            if (ptlosig->TYPE != 'I' ) {
                if (getptype(ptlosig->USER, YAG_MARKSIG_PTYPE) == NULL) {
                    ptlosig->USER = addptype(ptlosig->USER, YAG_MARKSIG_PTYPE, NULL);
                }
            }
        }
    }

    /* remove unwanted external connections and sort the rest */

    yagInitLoconVect();
    oldloconlist = ptlofig->LOCON;
    ptlofig->LOCON = NULL;
    for (ptlocon = oldloconlist; ptlocon; ptlocon = ptnextlocon) {
        ptnextlocon = ptlocon->NEXT;
        if (getptype(ptlocon->SIG->USER, YAG_MARKSIG_PTYPE) != NULL) {
            yagAddGivenLoconVect(ptlofig, ptlocon, YAG_VECTO);
        }
        else {
            if (ptlocon->PNODE) delrcnlocon(ptlocon);
            delloconuser(ptlocon);
            mbkfree(ptlocon);
        }
    }
    yagCloseLoconVect();
    /*ptlofig->LOCON = (locon_list *)reverse((chain_list *)ptlofig->LOCON);*/

    /* add core instance */

    sigchain = NULL;
    for (ptlocon = ptlofig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = ptlocon->SIG;
        if ((ptuser = getptype(ptlosig->USER, YAG_TOPSIG_PTYPE)) != NULL) {
            pttopsig = (losig_list *)ptuser->DATA;
        }
        else {
            ptlosig->USER = delptype(ptlosig->USER, YAG_MARKSIG_PTYPE);
            pttopsig = getExtSigFromName(*ptpttopfig, ptlosig->NAMECHAIN->DATA);
        }
        sigchain = addchain(sigchain, pttopsig);
    }
    sigchain = reverse(sigchain);
    sprintf(buf, "ins_%s", ptlofig->NAME);
    ptcoreins = addloins(*ptpttopfig, namealloc(buf), ptlofig, sigchain);
    freechain(sigchain);
    ptlofig->LOCON = (locon_list *)reverse((chain_list *)ptlofig->LOCON);

    /* delete old signals connected only on instances */

    lofigchain( ptlofig );
    for (ptchain = insextsigchain; ptchain; ptchain = ptchain->NEXT) {
        ptlosig = (losig_list *)ptchain->DATA;
        if (getptype(ptlosig->USER, YAG_MARKSIG_PTYPE) == NULL) {
            savechain = (chain_list *)getptype(ptlosig->USER, LOFIGCHAIN)->DATA; 
            freeptype(ptlosig->USER);
            ptlosig->USER = NULL;
            ptlosig->USER = addptype(ptlosig->USER, LOFIGCHAIN, savechain);
            dellosig(ptlofig, ptlosig->INDEX);
        }
        else ptlosig->USER = delptype(ptlosig->USER, YAG_MARKSIG_PTYPE);
    }

    /* delete unused signals */
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        ptchain = (chain_list *)getptype(ptlosig->USER, LOFIGCHAIN)->DATA;
        if (yagCountChains(ptchain) == 0) {
            if (ptlosig == ptlofig->LOSIG) {
                ptlofig->LOSIG = ptlosig->NEXT;
            }
            else ptprevsig->NEXT = ptlosig->NEXT;
            ptlosig->INDEX = 0;
        }
        else ptprevsig = ptlosig;
    }

    /* restore power supply markings */
    yagFindSupplies(ifl, ptlofig, TRUE);
    yagFindInternalSupplies(ifl, ptlofig, TRUE);

    return ptlofig;
}

/****************************************************************************
 *                         function transferLotrsInsfig()                   *
 ****************************************************************************/
        /* transfer transistors from old figure to new instance figures */

static void
transferLotrsInsfig(loins_list *ptgnsloins, lofig_list *ptinsfig, lofig_list *ptlofig, int copytrans)
{
    chain_list *translist;
    ptype_list *ptuser;
    chain_list *ptchain;
    lotrs_list *ptlotrs;
    losig_list *ptlosig;
    losig_list *ptprevious = NULL;
    losig_list *ptgridsig, *ptsourcesig, *ptdrainsig, *ptbulksig;

    ptuser = getptype(ptgnsloins->USER, FCL_TRANSLIST_PTYPE);
    if (ptuser == NULL) {
        yagBug(DBG_NULL_PTR, "markLotrsLoins", NULL, NULL, 0);
    }
    translist = (chain_list *)ptuser->DATA;

    if (copytrans) inssight = addht(1000);

    for (ptchain = translist; ptchain; ptchain = ptchain->NEXT) {
        ptlotrs = (lotrs_list *)ptchain->DATA;

        if (copytrans) {
            ptlosig = ptlotrs->GRID->SIG;
            if (getptype(ptlosig->USER, YAG_TOPSIG_PTYPE) != NULL) {
                ptgridsig = getExtSigFromName(ptinsfig, (char *)ptlosig->NAMECHAIN->DATA);
            }
            else ptgridsig = giveLoinsLosig(ptinsfig, ptlosig);

            ptlosig = ptlotrs->SOURCE->SIG;
            if (getptype(ptlosig->USER, YAG_TOPSIG_PTYPE) != NULL) {
                ptsourcesig = getExtSigFromName(ptinsfig, (char *)ptlosig->NAMECHAIN->DATA);
            }
            else ptsourcesig = giveLoinsLosig(ptinsfig, ptlosig);

            ptlosig = ptlotrs->DRAIN->SIG;
            if (getptype(ptlosig->USER, YAG_TOPSIG_PTYPE) != NULL) {
                ptdrainsig = getExtSigFromName(ptinsfig, (char *)ptlosig->NAMECHAIN->DATA);
            }
            else ptdrainsig = giveLoinsLosig(ptinsfig, ptlosig);

            if (ptlotrs->BULK != NULL) {
                ptlosig = ptlotrs->BULK->SIG;
                if (ptlosig == NULL) ptbulksig = NULL;
                else if (getptype(ptlosig->USER, YAG_TOPSIG_PTYPE) != NULL) {
                    ptbulksig = getExtSigFromName(ptinsfig, (char *)ptlosig->NAMECHAIN->DATA);
                }
                else ptbulksig = giveLoinsLosig(ptinsfig, ptlosig);
            }
            else ptbulksig = NULL;

            addlotrs(ptinsfig, ptlotrs->TYPE, ptlotrs->X, ptlotrs->Y, 
                     ptlotrs->WIDTH, ptlotrs->LENGTH, ptlotrs->PS, ptlotrs->PD, 
                     ptlotrs->XS, ptlotrs->XD, 
                     ptgridsig, ptsourcesig, ptdrainsig, ptbulksig, ptlotrs->TRNAME);
        }

        /* mark transistor for deletion */
        ptlotrs->TYPE = 0;
    }

    /*  delete corresponding signal in old lofig except for internal supplies */

    if (copytrans) {
        for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
            if (gethtitem(inssight, (void *)ptlosig->INDEX) != EMPTYHT && (ptlosig->TYPE == 'I' || ptlosig->TYPE == 'E')) {
                if (ptlosig == ptlofig->LOSIG) {
                    ptlofig->LOSIG = ptlosig->NEXT;
                }
                else ptprevious->NEXT = ptlosig->NEXT;
                ptlosig->INDEX = 0;
            }
            else ptprevious = ptlosig;
        }
    }

    if (copytrans) delht(inssight);
}

/****************************************************************************
 *                         function transferLoinsInsfig()                   *
 ****************************************************************************/
        /* transfer instances from old figure to new instance figures */

static void
transferLoinsInsfig(loins_list *ptgnsloins, lofig_list *ptinsfig, lofig_list *ptlofig, int copytrans)
{
    chain_list *inslist;
    ptype_list *ptuser;
    chain_list *ptchain;
    chain_list *sigchain = NULL;
    loins_list *ptloins;
    locon_list *ptlocon;
    losig_list *ptlosig;
    losig_list *ptprevious = NULL;

    ptuser = getptype(ptgnsloins->USER, FCL_INSLIST_PTYPE);
    if (ptuser == NULL) return;
    inslist = (chain_list *)ptuser->DATA;

    if (copytrans) inssight = addht(1000);

    for (ptchain = inslist; ptchain; ptchain = ptchain->NEXT) {
        ptloins = (loins_list *)ptchain->DATA;

        if (copytrans) {
            for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
                ptlosig = ptlocon->SIG;
                if (getptype(ptlosig->USER, YAG_TOPSIG_PTYPE) != NULL) {
                    sigchain = addchain(sigchain, getExtSigFromName(ptinsfig, (char *)ptlosig->NAMECHAIN->DATA));
                }
                else sigchain = addchain(sigchain, giveLoinsLosig(ptinsfig, ptlosig));
            }
            sigchain = reverse(sigchain);
            addloins(ptinsfig, ptloins->INSNAME, getlofig(ptloins->FIGNAME, 'P'), sigchain);
        }

        /* mark instance for deletion */
        ptloins->FIGNAME = NULL;
    }

    /*  delete corresponding signal in old lofig except for internal supplies */

    if (copytrans) {
        for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
            if (gethtitem(inssight, (void *)ptlosig->INDEX) != EMPTYHT && (ptlosig->TYPE == 'I' || ptlosig->TYPE == 'E')) {
                if (ptlosig == ptlofig->LOSIG) {
                    ptlofig->LOSIG = ptlosig->NEXT;
                }
                else ptprevious->NEXT = ptlosig->NEXT;
                ptlosig->INDEX = 0;
            }
            else ptprevious = ptlosig;
        }
    }

    if (copytrans) delht(inssight);
}

/****************************************************************************
 *                         function createTopLosig()                        *
 ****************************************************************************/
        /* create signals in top figure for connecting an instance */

static void
createTopLosig(loins_list *ptloins, lofig_list *pttoplofig)
{
    losig_list *ptlosig;
    losig_list *pttopsig;
    locon_list *ptlocon;

    for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = ptlocon->SIG;
        if (getptype(ptlosig->USER, YAG_TOPSIG_PTYPE) == NULL) {
            if ((ptlosig->TYPE == 'E') || (ptlosig->TYPE == 'D') || (ptlosig->TYPE == 'S')) {
                pttopsig = getExtSigFromName(pttoplofig, (char *)ptlosig->NAMECHAIN->DATA);
            }
            if ((ptlosig->TYPE == 'I') || (pttopsig == NULL)) {
                pttopsig = yagAddLosigVect(pttoplofig, topsigindex++, addchain(NULL, ptlosig->NAMECHAIN->DATA), 'I');
                mbk_copylosiginfo(ptlosig, pttopsig);
            }
            ptlosig->USER = addptype(ptlosig->USER, YAG_TOPSIG_PTYPE, pttopsig);
            insextsigchain = addchain(insextsigchain, ptlosig);
        }
    }
}

/****************************************************************************
 *                         function createTopLofig()                        *
 ****************************************************************************/
        /* create interface of top figure */

static lofig_list *
createTopLofig(lofig_list *ptlofig, char *name)
{
    lofig_list *pttoplofig;
    locon_list *ptlocon;
    losig_list *ptlosig;
    losig_list *pttopsig;

    pttoplofig = addlofig(name);

    topsigindex = 1;

    yagInitLoconVect();
    for (ptlocon = ptlofig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = ptlocon->SIG;
        pttopsig = addlosig(pttoplofig, topsigindex++, addchain(NULL, ptlosig->NAMECHAIN->DATA), ptlosig->TYPE);
        mbk_copylosiginfo(ptlosig, pttopsig);
        yagAddLoconVect(pttoplofig, ptlocon->NAME, pttopsig, ptlocon->DIRECTION, YAG_VECTO, NULL, NULL);
    }
    /*pttoplofig->LOCON = (locon_list *)reverse((chain_list *)pttoplofig->LOCON);*/
    yagCloseLoconVect();
    return pttoplofig;
}

/****************************************************************************
 *                         function getExtSigFromName()                       *
 ****************************************************************************/
        /* find an external signal given its name */

static losig_list *
getExtSigFromName(lofig_list *ptlofig, char *name)
{
    locon_list *ptlocon;
    losig_list *ptlosig;

    for (ptlocon = ptlofig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = ptlocon->SIG;
        if (name == (char *)ptlosig->NAMECHAIN->DATA) return ptlosig;
    }
    return NULL;
}

/****************************************************************************
 *                         function giveInstanceLofig()                     *
 ****************************************************************************/
        /* create external interface of an instance figure */

static lofig_list *
giveInstanceLofig(loins_list *ptloins, int copytrans)
{
    locon_list *ptlocon;
    lofig_list *ptinslofig;
    losig_list *ptlosig;

    if (copytrans) {
        ptinslofig = addlofig(ptloins->INSNAME);
    }
    else {
        if ((ptinslofig = getloadedlofig(ptloins->FIGNAME)) == NULL) {
            ptinslofig = addlofig(ptloins->FIGNAME);
        }
        else return ptinslofig;
    }

    for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = addlosig(ptinslofig, inssigindex++, addchain(NULL, ptlocon->NAME), 'E');
        addlocon(ptinslofig, ptlocon->NAME, ptlosig, ptlocon->DIRECTION);
    }
    ptinslofig->LOCON = (locon_list *)reverse((chain_list *)ptinslofig->LOCON);

    return ptinslofig;
}

/****************************************************************************
 *                         function giveLoinsLosig()                        *
 ****************************************************************************/
        /* given a signal index in old figure return corresponding */
        /* signal in an instance figure. The hash table is checked */
        /* to see if signal already created. if not then it is     */
        /* created it and correspondance is added to hash table    */

static losig_list *
giveLoinsLosig(lofig_list *ptinsfig, losig_list *ptoldsig)
{
    losig_list *ptinssig;

    ptinssig = (losig_list *)gethtitem(inssight, (void *)ptoldsig->INDEX);
    if (ptinssig == (losig_list *)EMPTYHT) {
        ptinssig = addlosig(ptinsfig, inssigindex++, addchain(NULL, ptoldsig->NAMECHAIN->DATA), 'I');
        addhtitem(inssight, (void *)ptoldsig->INDEX, (long)ptinssig);
    }
    return ptinssig;
}

/****************************************************************************
 *                         function yagBuildGeniusBehHierarchy()            *
 ****************************************************************************/
        /* build structural figure for behavioural hierarchy */

lofig_list     *
yagBuildGeniusBehHierarchy(lofig_list *pttoplofig, lofig_list *ptcorelofig, chain_list *instances, char *name)
{
    lofig_list *hBehFig;
    loins_list *ptrecins;
    loins_list *ptloins;
    chain_list *ptchain;
    locon_list *ptextcon;
    locon_list *ptlocon;
    losig_list *ptlosig;
    chain_list *loconchain;
    int         direction;
//    char        buf[YAGBUFSIZE];

    /* duplicate logical hierarchical figure */

    hBehFig = rduplofig(pttoplofig);
    delhtitem(HT_LOFIG, hBehFig->NAME);
    hBehFig->NAME = namealloc(name);
    addhtitem(HT_LOFIG, hBehFig->NAME, (long)hBehFig);
    lofigchain(hBehFig);

    /* group instances with identical models  unless already done */

    if (hBehFig->LOINS->FIGNAME == hBehFig->LOINS->INSNAME) {
        for (ptchain = instances; ptchain; ptchain = ptchain->NEXT) {
            ptrecins = (loins_list *)ptchain->DATA;
            for (ptloins = hBehFig->LOINS; ptloins; ptloins = ptloins->NEXT) {
                if (ptloins->INSNAME == ptrecins->INSNAME) break;
            }
            if (ptloins != NULL) ptloins->FIGNAME = ptrecins->FIGNAME;
        }
    }

    /* orient the core instance */

    ptcorelofig->LOCON = (locon_list *)reverse((chain_list *)ptcorelofig->LOCON);
    for (ptloins = hBehFig->LOINS; ptloins; ptloins = ptloins->NEXT) {
        if (ptloins->FIGNAME == ptcorelofig->NAME) break;
    }
    if (ptloins != NULL) {
        ptextcon = ptcorelofig->LOCON;
        for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            ptlocon->DIRECTION = ptextcon->DIRECTION;
            if (ptlocon->DIRECTION == 'D' || ptlocon->DIRECTION == 'S' || ptlocon->DIRECTION == 'X') {
                ptlocon->DIRECTION = 'I';
            }
            ptextcon = ptextcon->NEXT;
        }
    }
    ptcorelofig->LOCON = (locon_list *)reverse((chain_list *)ptcorelofig->LOCON);

    /* orient the interface */

    for (ptextcon = hBehFig->LOCON; ptextcon; ptextcon = ptextcon->NEXT) {
        direction = 0;
        ptlosig = ptextcon->SIG;
        loconchain = (chain_list *)getptype(ptlosig->USER, LOFIGCHAIN)->DATA;
        for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
            ptlocon = (locon_list *)ptchain->DATA;
            if (ptlocon->TYPE == 'I') {
                switch (ptlocon->DIRECTION) {
                    case 'I': direction |= HCON_IN; break;
                    case 'O': direction |= HCON_OUT; break;
                    case 'B': direction |= HCON_INOUT; break;
                    case 'Z': direction |= HCON_OUTHZ; break;
                    case 'T': direction |= HCON_INOUTHZ; break;
                }
            }
        }
        switch (direction) {
            case HCON_IN: ptextcon->DIRECTION = 'I'; break;
            case HCON_OUT: ptextcon->DIRECTION = 'O'; break;
            case HCON_INOUT: ptextcon->DIRECTION = 'B'; break;
            case HCON_OUTHZ: ptextcon->DIRECTION = 'Z'; break;
            case HCON_INOUTHZ: ptextcon->DIRECTION = 'T'; break;
        }
    }

    /* update model chain */

    freechain (hBehFig->MODELCHAIN);
    hBehFig->MODELCHAIN = NULL;

    for (ptloins = hBehFig->LOINS; ptloins != NULL; ptloins = ptloins->NEXT) {
        for (ptchain = hBehFig->MODELCHAIN; ptchain != NULL; ptchain = ptchain->NEXT)
            if (ptchain->DATA == ptloins->FIGNAME)
                break;
        if (ptchain == NULL)
            hBehFig->MODELCHAIN = addchain (hBehFig->MODELCHAIN, ptloins->FIGNAME);
    }

    return hBehFig;
}
