/****************************************************************************/
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_hierbeh.c                                               */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 01/08/1999     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static void createTopLosig(loins_list *ptloins, lofig_list *pttoplofig);
static lofig_list *createTopLofig(lofig_list *ptlofig, char *name);
static losig_list *getExtSigFromName(lofig_list *ptlofig, char *name);
static lofig_list *giveInstanceLofig(loins_list *ptloins, int copytrans);

static long inssigindex;
static long topsigindex;

/****************************************************************************
 *                    function yagSortModeledInstances()                    *
 ****************************************************************************/
        /* obtain list of instances without befig */
void
yagSortModeledInstances(lofig_list *ptlofig, chain_list **ptmodeled, chain_list **ptunmodeled)
{
    befig_list *ptbefig;
    loins_list *ptloins;
    ht         *befight;
    chain_list *ptchain;
    ptype_list *ptuser;

    befight = addht(40);
    for (ptchain = YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS; ptchain; ptchain = ptchain->NEXT) {
        ptbefig = (befig_list *)ptchain->DATA;
        addhtitem(befight, ptbefig->NAME, 0);
    }
    for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
        ptuser = getptype(ptloins->USER, FCL_TRANSFER_PTYPE);
        if (ptuser != NULL) {
            if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
        }
        if (gethtitem(befight, ptloins->INSNAME) == EMPTYHT) {
            *ptunmodeled = addchain(*ptunmodeled, ptloins);
        }
        else *ptmodeled = addchain(*ptmodeled, ptloins);
    }
    delht(befight);
}

/****************************************************************************
 *                         function yagBuildHierarchy()                     *
 ****************************************************************************/
        /* create hierarchical figure from given instances */

lofig_list *
yagBuildHierarchy(cnsfig_list *ptcnsfig, lofig_list *ptlofig, lofig_list **ptptrootlofig, chain_list *instances)
{
    lofig_list *ptcorelofig;
    chain_list *ptchain;
    chain_list *loconchain;
    lotrs_list *pttrans;
    loins_list *ptloins;
    lofig_list *ptinsfig;
    losig_list *pttopsig;
    losig_list *ptlosig;
    locon_list *ptlocon;
    locon_list *ptintcon;
    chain_list *sigchain;
    ptype_list *ptuser;
    loins_list *ptcoreins;
    char        buf[YAGBUFSIZE];
    char       *outname;

    strcpy(buf, ptlofig->NAME);
    strcat(buf, "_yagcore");
    ptcorelofig = addlofig(namealloc(buf));

    
    if (YAG_CONTEXT->YAG_OUTNAME == ptlofig->NAME) {
      if (getenv("YAGLE_OUT_NAME"))
        avt_errmsg(YAG_ERRMSG,"009",AVT_WARNING,outname);
      strcpy(buf, ptlofig->NAME);
      strcat(buf, "_yagroot");
      outname = buf;
      // because it's a ugly definition
      avt_sethashvar("yagleOutputName",outname);
    }
    else
      outname = YAG_CONTEXT->YAG_OUTNAME;

    *ptptrootlofig = createTopLofig(ptlofig, namealloc(outname));
 

    /* traverse the instances to create corresponding figures */

    yagInitLosigVect();
    for (ptchain = instances; ptchain; ptchain = ptchain->NEXT) {
        inssigindex = 1;
        ptloins = (loins_list *)ptchain->DATA;
        createTopLosig(ptloins, *ptptrootlofig);
        sigchain = NULL;
        for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            /* remove power supply markings */
            if (ptlocon->DIRECTION == 'D' || ptlocon->DIRECTION == 'S') ptlocon->DIRECTION = 'I';

            ptlosig = ptlocon->SIG;
            pttopsig = (losig_list *)getptype(ptlosig->USER, YAG_TOPSIG_PTYPE)->DATA;
            sigchain = addchain(sigchain, pttopsig);
        }
        sigchain = reverse(sigchain);
        ptinsfig = giveInstanceLofig(ptloins, FALSE);
        addloins(*ptptrootlofig, ptloins->INSNAME, ptinsfig, sigchain);
    }
    yagCloseLosigVect();

    /* traverse external and internal connectors to identify external connections of core */

    for (ptchain = instances; ptchain; ptchain = ptchain->NEXT) {
        ptloins = (loins_list *)ptchain->DATA;
        ptloins->USER = addptype(ptloins->USER, YAG_MARKINS_PTYPE, NULL);
    }
    yagInitLoconVect();
    for (ptlocon = ptcnsfig->INTCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = ptlocon->SIG;
        loconchain = (chain_list *)getptype(ptlosig->USER, LOFIGCHAIN)->DATA;
        for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
            ptintcon = (locon_list *)ptchain->DATA;
            if (ptintcon->TYPE == 'I') {
                ptloins = (loins_list *)ptintcon->ROOT;
                if (getptype(ptloins->USER, YAG_MARKINS_PTYPE) != NULL) break;
            }
        }
        if (ptchain == NULL) continue;
        for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
            ptintcon = (locon_list *)ptchain->DATA;
            if (ptintcon->TYPE == 'T') {
                pttrans = (lotrs_list *)ptintcon->ROOT;
                if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
                }
                break;
            }
            else if (ptintcon->TYPE == 'I') {
                ptloins = (loins_list *)ptintcon->ROOT;
                if ((ptuser = getptype(ptloins->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
                }
                if (getptype(ptloins->USER, YAG_MARKINS_PTYPE) != NULL) continue;
                break;
            }
        }
        if (ptchain != NULL) {
            yagAddLoconVect(ptcorelofig, ptlocon->NAME, NULL, ptlocon->DIRECTION, YAG_VECTO, NULL, addptype(NULL, YAG_LOCON_PTYPE, ptlocon));
        }
    }
    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = ptlocon->SIG;
        loconchain = (chain_list *)getptype(ptlosig->USER, LOFIGCHAIN)->DATA;
        for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
            ptintcon = (locon_list *)ptchain->DATA;
            if (ptintcon->TYPE == 'T') {
                pttrans = (lotrs_list *)ptintcon->ROOT;
                if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
                }
                break;
            }
            else if (ptintcon->TYPE == 'I') {
                ptloins = (loins_list *)ptintcon->ROOT;
                if ((ptuser = getptype(ptloins->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
                }
                if (getptype(ptloins->USER, YAG_MARKINS_PTYPE) != NULL) continue;
                break;
            }
        }
        if (ptchain != NULL) {
            yagAddLoconVect(ptcorelofig, ptlocon->NAME, NULL, ptlocon->DIRECTION, YAG_VECTO, NULL, addptype(NULL, YAG_LOCON_PTYPE, ptlocon));
        }
    }
    yagCloseLoconVect();
    /*ptcorelofig->LOCON = (locon_list *)reverse((chain_list *)ptcorelofig->LOCON);*/
    for (ptchain = instances; ptchain; ptchain = ptchain->NEXT) {
        ptloins = (loins_list *)ptchain->DATA;
        ptloins->USER = delptype(ptloins->USER, YAG_MARKINS_PTYPE);
    }

    /* add core instance */

    sigchain = NULL;
    for (ptlocon = ptcorelofig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = ((locon_list *)getptype(ptlocon->USER, YAG_LOCON_PTYPE)->DATA)->SIG;
        if ((ptuser = getptype(ptlosig->USER, YAG_TOPSIG_PTYPE)) != NULL) {
            pttopsig = (losig_list *)ptuser->DATA;
        }
        else {
            pttopsig = getExtSigFromName(*ptptrootlofig, ptlosig->NAMECHAIN->DATA);
        }
        sigchain = addchain(sigchain, pttopsig);
    }
    sigchain = reverse(sigchain);
    sprintf(buf, "ins_%s", ptcorelofig->NAME);
    ptcoreins = addloins(*ptptrootlofig, namealloc(buf), ptcorelofig, sigchain);
    freechain(sigchain);
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        if (getptype(ptlosig->USER, YAG_TOPSIG_PTYPE) != NULL) {
            ptlosig->USER = delptype(ptlosig->USER, YAG_TOPSIG_PTYPE);
        }
    }

    ptcorelofig->LOCON = (locon_list *)reverse((chain_list *)ptcorelofig->LOCON);
    return ptcorelofig;
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
        if (ptlocon->DIRECTION == 'D' || ptlocon->DIRECTION == 'S') ptlocon->DIRECTION = 'I';
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
    /* ptinslofig->LOCON = (locon_list *)reverse((chain_list *)ptinslofig->LOCON); */

    return ptinslofig;
}

/*---------------------------------------------------------------------------*
| Guess Connector direction                                                  |
*----------------------------------------------------------------------------*/
void
yagGuessRootConnectorDirections(lofig_list *ptrootlofig)
{
#define DIRONSIG_PTYPE 0xfab50420
#define POWINFO_PTYPE 0xfab50421
#define DIRONSIG_IN  1
#define DIRONSIG_OUT 2
#define EXTDIRONSIG_IN  4
#define EXTDIRONSIG_OUT 8
#define ALIMSIG 16

    loins_list  *li, *li0;
    lofig_list  *inslf;
    locon_list  *lc, *lc0;
    losig_list  *ls;
    ht          *fight;
    ptype_list  *pt, *pt0;
    chain_list  *cl, *ch;
    long         l;

    for (li=ptrootlofig->LOINS; li!=NULL && strstr(li->INSNAME,"_yagcore")==NULL; li=li->NEXT) ;

    if (li!=NULL) {
        for (ls=ptrootlofig->LOSIG; ls!=NULL; ls=ls->NEXT) {
            ls->USER=addptype(ls->USER, DIRONSIG_PTYPE, 0);
        }

        for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT) {
            pt=getptype(lc->SIG->USER, DIRONSIG_PTYPE);
            l=(long)pt->DATA;
            if (lc->DIRECTION==IN) l|=DIRONSIG_IN;
            else if (lc->DIRECTION==OUT || lc->DIRECTION==INOUT || lc->DIRECTION==TRISTATE) l|=DIRONSIG_OUT;
            else if (lc->DIRECTION!=UNKNOWN) l|=DIRONSIG_IN|DIRONSIG_OUT;
            if (mbk_LosigIsVDD(lc->SIG) || mbk_LosigIsVSS(lc->SIG)) l|=ALIMSIG;
            pt->DATA=(void *)l;
        }

        for (lc=ptrootlofig->LOCON; lc!=NULL; lc=lc->NEXT) {
            pt=getptype(lc->SIG->USER, DIRONSIG_PTYPE);
            l=(long)pt->DATA;
            if (lc->DIRECTION==IN) l|=EXTDIRONSIG_IN;
            else if (lc->DIRECTION==OUT || lc->DIRECTION==INOUT || lc->DIRECTION==TRISTATE) l|=EXTDIRONSIG_OUT;
            if (mbk_LosigIsVDD(lc->SIG) || mbk_LosigIsVSS(lc->SIG)) {
                l|=ALIMSIG;
                if (lc->DIRECTION==UNKNOWN) lc->DIRECTION=IN;
            }
            pt->DATA=(void *)l;
        }

        fight=addht(10);

        for (li0=ptrootlofig->LOINS; li0!=NULL; li0=li0->NEXT)
        {          
            if (li0!=li && (inslf=getloadedlofig(li0->FIGNAME))!=NULL)
            {
                addhtitem(fight, li0->FIGNAME, (long)inslf);
                for (lc=inslf->LOCON, lc0=li0->LOCON;  lc!=NULL && lc0!=NULL; lc=lc->NEXT, lc0=lc0->NEXT)
                {
                    if ((pt=getptype(lc->SIG->USER, DIRONSIG_PTYPE))==NULL)
                        pt=lc->USER=addptype(lc->USER, DIRONSIG_PTYPE, 0);
                    pt0=getptype(lc0->SIG->USER, DIRONSIG_PTYPE);
                    l=(long)pt->DATA | (long)pt0->DATA;
                    pt->DATA=(void *)l;
                }
            }
        }

        for (lc=ptrootlofig->LOCON; lc!=NULL; lc=lc->NEXT)
        {
            if (lc->DIRECTION==UNKNOWN)
            {
                pt=getptype(lc->SIG->USER, DIRONSIG_PTYPE);
                l=(long)pt->DATA;
                if (!((l & (DIRONSIG_IN|DIRONSIG_OUT))==(DIRONSIG_IN|DIRONSIG_OUT)))
                {
                    if ((l & DIRONSIG_IN)==DIRONSIG_IN) lc->DIRECTION=IN;
                    else if ((l & DIRONSIG_OUT)==DIRONSIG_OUT) lc->DIRECTION=OUT;
                    else lc->DIRECTION=IN;
                }
            }
        }

        for (ls=ptrootlofig->LOSIG; ls!=NULL; ls=ls->NEXT)
        {
            ls->USER=delptype(ls->USER, DIRONSIG_PTYPE);
        }

        for (cl=ch=GetAllHTElems(fight); cl!=NULL; cl=cl->NEXT)
        {
            inslf=(lofig_list *)cl->DATA;
            for (lc=inslf->LOCON; lc!=NULL; lc=lc->NEXT)
            {
                if (mbk_LosigIsVDD(lc->SIG) || mbk_LosigIsVSS(lc->SIG))
                    lc->DIRECTION=IN;
                else if (lc->DIRECTION==UNKNOWN)
                {
                    pt=getptype(lc->USER, DIRONSIG_PTYPE);
                    l=(long)pt->DATA;
                    if ((l & ALIMSIG)==ALIMSIG) lc->DIRECTION=IN;
                    else if ((l & (DIRONSIG_IN|DIRONSIG_OUT))==(DIRONSIG_IN|DIRONSIG_OUT))
                        lc->DIRECTION=TRANSCV; //INOUT;
                    else if ((l & DIRONSIG_IN)==DIRONSIG_IN)
                        if ((l & EXTDIRONSIG_IN)==EXTDIRONSIG_IN)
                            lc->DIRECTION=IN;
                        else
                            lc->DIRECTION=OUT;
                    else if ((l & DIRONSIG_OUT)==DIRONSIG_OUT)
                        if ((l & EXTDIRONSIG_IN)==EXTDIRONSIG_IN)
                            lc->DIRECTION=OUT;
                        else
                            lc->DIRECTION=IN;
                    else
                        lc->DIRECTION=IN; // i don't know
                }
                lc->USER=delptype(lc->USER, DIRONSIG_PTYPE);
            }
        }
        freechain(ch);
        delht(fight); 

        for (li0=ptrootlofig->LOINS; li0!=NULL; li0=li0->NEXT)
            if (li0!=li && (inslf=getloadedlofig(li0->FIGNAME))!=NULL)
                for (lc=inslf->LOCON, lc0=li0->LOCON;  lc!=NULL && lc0!=NULL; lc=lc->NEXT, lc0=lc0->NEXT)
                    lc0->DIRECTION=lc->DIRECTION;
    }
#undef DIRONSIG_PTYPE
#undef POWINFO_PTYPE
#undef DIRONSIG_IN
#undef DIRONSIG_OUT
#undef EXTDIRONSIG_IN
#undef EXTDIRONSIG_OUT
#undef ALIMSIG
}
