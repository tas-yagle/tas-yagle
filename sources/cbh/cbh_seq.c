/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_seq.c                                                   */
/*                                                                          */
/*    (c) copyright 2001 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Anthony LESTER                                          */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>

#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include CBH_H

typedef struct {
    pNode           OLD;
    pNode           NEW;
}               cbh_pair;

static void     cbh_latch(befig_list * ptcellbefig, cbhseq * ptcbhseq, int mode);
static void     cbh_flipflop(befig_list * ptcellbefig, cbhseq * ptcbhseq, int mode);
static int      cbh_calcpolarity(pCircuit pC, pNode ptdepend, chain_list * ptdatainputs);

static int      cbh_setrsconf(ht * valht, pNode ptset, pNode ptreset);
static pNode    cbh_evalbereg(pCircuit ptcircuit, bereg_list * ptbereg, ht * valht);
static pNode    cbh_evalbeout(pCircuit ptcircuit, beout_list * ptbeout, ht * valht);
static pNode    cbh_bddeval(pNode ptbdd, ht * valht);

/****************************************************************************/
/*                cbh_getseqfunc                                            */
/****************************************************************************/
cbhseq         *
cbh_getseqfunc(befig_list * ptcellbefig, int mode)
{
    cbhseq         *ptcbhseq = NULL;

    if (ptcellbefig->ERRFLG != 0)
        return NULL;

    if (ptcellbefig->BEBUX != NULL || cbh_countchains((chain_list *) ptcellbefig->BEREG) > 2) {
        return NULL;
    }

    if (ptcellbefig->CIRCUI == NULL) {
        beh_depend(ptcellbefig);
        if (ptcellbefig->BEAUX != NULL) {
            if (cbh_suppressaux(ptcellbefig) != 0)
                return NULL;
        }
        beh_indexbdd(ptcellbefig);
    }
    else if (ptcellbefig->CIRCUI == (void *)-1) {
        ptcellbefig->CIRCUI = NULL;
        beh_indexbdd(ptcellbefig);
    }

    if (ptcellbefig->BEREG != NULL) {
        ptcbhseq = cbh_newseq();
        if (ptcellbefig->BEREG->NEXT != NULL) {
            cbh_flipflop(ptcellbefig, ptcbhseq, mode);
        }
        else {
            cbh_latch(ptcellbefig, ptcbhseq, mode);
        }
        if (ptcbhseq->SEQTYPE == CBH_UNKNOWN) {
            cbh_delseq(ptcbhseq);
            ptcbhseq = NULL;
        }
    }

    return ptcbhseq;
}

static void 
cbh_latch(befig_list * ptcellbefig, cbhseq * ptcbhseq, int mode)
{
    pCircuit        pC;
    biabl_list     *ptbiabl;
    beout_list     *ptbeout;
    bebus_list     *ptbebus;
    cbhcomb        *ptcbhcomb;
    pNode           ptval, ptcond;
    pNode           ptset = NULL, ptreset = NULL, ptclock = NULL, ptwrite = BDD_zero;
    pNode           ptdata = NULL;
    pNode           ptnode;
    pNode           ptdepend;
    chain_list     *ptdataexpr = NULL;
    chain_list     *ptdatainputs = NULL;
    chain_list     *ptoutlist = NULL;
    chain_list     *ptnoninvertlist = NULL, *ptinvertlist = NULL;
    chain_list     *pthznoninvertlist = NULL, *pthzinvertlist = NULL;
    chain_list     *ptchain1, *ptchain2;
    ht             *valht;
    long            htres;
    int             polarity, num, numhz;
    char            val;
    char            buffer[256];

    pC = ptcellbefig->CIRCUI;
    ptcbhseq->SEQTYPE = CBH_LATCH;

    for (ptbiabl = ptcellbefig->BEREG->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        ptcond = ablToBddCct(pC, ptbiabl->CNDABL);
        ptval = ablToBddCct(pC, ptbiabl->VALABL);
        if (ptval == BDD_zero)
            ptreset = ptcond;
        else if (ptval == BDD_one)
            ptset = ptcond;
        else if (ptdata == NULL) {
            ptdata = ptval;
            ptclock = ptcond;
            ptdataexpr = ptbiabl->VALABL;
        }
        else {
            ptcbhseq->SEQTYPE = CBH_UNKNOWN;
            return;
        }
        ptwrite = applyBinBdd(OR, ptwrite, ptcond);
    }
    /* eliminate set/reset priority encoding */
    if (ptset != NULL && ptreset != NULL
        && applyBinBdd(AND, ptset, ptreset) == BDD_zero) {
        ptchain1 = supportChain_listBdd(ptset);
        ptchain2 = supportChain_listBdd(ptreset);
        if (cbh_countchains(ptchain1) > cbh_countchains(ptchain2))
            ptset = constraintBdd(ptset, notBdd(ptreset));
        else if (cbh_countchains(ptchain2) > cbh_countchains(ptchain1))
            ptreset = constraintBdd(ptreset, notBdd(ptset));
        freechain(ptchain1);
        freechain(ptchain2);
    }

    /* eliminate clock priority encoding */
    if (ptdata == NULL)
        ptwrite = BDD_zero;
    else {
        if (ptset != NULL)
            ptwrite = constraintBdd(ptwrite, notBdd(ptset));
        if (ptreset != NULL)
            ptwrite = constraintBdd(ptwrite, notBdd(ptreset));
    }

    if (ptwrite == BDD_zero)
        ptwrite = NULL;
    if (ptset == BDD_zero)
        ptset = NULL;
    if (ptreset == BDD_zero)
        ptreset = NULL;

    /* obtain list of inverting and non-inverting outputs */
    if (ptdata != NULL)
        ptdatainputs = supportChain_listExpr(ptdataexpr);
    for (ptbeout = ptcellbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        ptnode = ablToBddCct(pC, ptbeout->ABL);
        addOutputCct(pC, ptbeout->NAME, ptnode);
        if (ptdata != NULL) {
            ptdepend = composeBdd(ptnode, ptdata,
                              searchInputCct(pC, ptcellbefig->BEREG->NAME));
            polarity = cbh_calcpolarity(pC, ptdepend, ptdatainputs);
        }
        else
            polarity = CBH_NONE;
        switch (polarity) {
          case CBH_INVERT:
            ptinvertlist = addchain(ptinvertlist, ptbeout->NAME);
            break;
          case CBH_NONINVERT:
            ptnoninvertlist = addchain(ptnoninvertlist, ptbeout->NAME);
            break;
          case CBH_BOTH:
          case CBH_NONE:
            ptoutlist = addchain(ptoutlist, ptbeout->NAME);
            break;
        }
    }
    for (ptbebus = ptcellbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        ptcbhcomb = cbh_newcomb();
        cbh_calchzfunc(pC, ptbebus->BIABL, ptcbhcomb);
        ptnode = ablToBddCct(pC, ptcbhcomb->FUNCTION);
        cbh_delcomb(ptcbhcomb);
        addOutputCct(pC, ptbebus->NAME, ptnode);
        if (ptdata != NULL) {
            ptdepend = composeBdd(ptnode, ptdata,
                              searchInputCct(pC, ptcellbefig->BEREG->NAME));
            polarity = cbh_calcpolarity(pC, ptdepend, ptdatainputs);
        }
        else
            polarity = CBH_NONE;
        switch (polarity) {
          case CBH_INVERT:
            pthzinvertlist = addchain(pthzinvertlist, ptbebus->NAME);
            break;
          case CBH_NONINVERT:
            pthznoninvertlist = addchain(pthznoninvertlist, ptbebus->NAME);
            break;
          case CBH_BOTH:
          case CBH_NONE:
            ptoutlist = addchain(ptoutlist, ptbebus->NAME);
            break;
        }
    }

    /* detect impossible configurations of inverting and non-inverting outputs */
    num = cbh_countchains(ptinvertlist) + cbh_countchains(ptnoninvertlist) + cbh_countchains(ptoutlist);
    numhz = cbh_countchains(pthzinvertlist) + cbh_countchains(pthznoninvertlist);
    if ((num + numhz) < 1 || num > 2 || numhz > 2 
    || cbh_countchains(ptinvertlist) > 1 || cbh_countchains(ptnoninvertlist) > 1 
    || cbh_countchains(pthzinvertlist) > 1 || cbh_countchains(pthznoninvertlist) > 1) {
        freechain(ptdatainputs);
        freechain(ptoutlist);
        freechain(ptnoninvertlist);
        freechain(ptinvertlist);
        freechain(pthznoninvertlist);
        freechain(pthzinvertlist);
        ptcbhseq->SEQTYPE = CBH_UNKNOWN;
        return;
    }
    /* handle cases of incomplete detection */
    if (cbh_countchains(ptoutlist) == 2) {
        /* try to guess from name */
        if (cbh_testnegname((char *) ptoutlist->DATA) == TRUE) {
            ptinvertlist = ptoutlist;
            ptnoninvertlist = ptoutlist->NEXT;
            ptinvertlist->NEXT = NULL;
            ptoutlist = NULL;
        }
        else {
            ptnoninvertlist = ptoutlist;
            ptinvertlist = ptoutlist->NEXT;
            ptnoninvertlist->NEXT = NULL;
            ptoutlist = NULL;
        }
    }
    else if (num == 2 && cbh_countchains(ptoutlist) == 1) {
        if (ptinvertlist == NULL) {
            ptinvertlist = ptoutlist;
            ptoutlist = NULL;
        }
        else if (ptnoninvertlist == NULL) {
            ptnoninvertlist = ptoutlist;
            ptoutlist = NULL;
        }
    }
    else if (num == 1) {
        if (ptoutlist != NULL) {
            ptnoninvertlist = ptoutlist;
            ptoutlist = NULL;
        }
        else if (ptinvertlist != NULL) {
            ptnoninvertlist = ptinvertlist;
            ptinvertlist = NULL;
        }
    }

    /* Affect latch variables and output pins handling single var case */
    if (ptnoninvertlist != NULL)
        ptcbhseq->PIN = ptnoninvertlist->DATA;
    if (ptinvertlist != NULL)
        ptcbhseq->NEGPIN = ptinvertlist->DATA;
    if (pthznoninvertlist != NULL)
        ptcbhseq->HZPIN = pthznoninvertlist->DATA;
    if (pthzinvertlist != NULL)
        ptcbhseq->HZNEGPIN = pthzinvertlist->DATA;
    if (mode == CBH_LIB_MODE) {
        if (ptcbhseq->PIN != NULL) {
            sprintf(buffer, "i%s", ptcbhseq->PIN);
            ptcbhseq->STATEPIN = ptcbhseq->PIN;
        }
        else if (ptcbhseq->HZPIN != NULL) {
            sprintf(buffer, "i%s", ptcbhseq->HZPIN);
            ptcbhseq->STATEPIN = ptcbhseq->HZPIN;
        }
        else if (ptcbhseq->NEGPIN != NULL) {
            sprintf(buffer, "i%s_neg", ptcbhseq->NEGPIN);
            ptcbhseq->STATEPIN = ptcbhseq->NEGPIN;
        }
        else if (ptcbhseq->HZNEGPIN != NULL) {
            sprintf(buffer, "i%s_neg", ptcbhseq->HZNEGPIN);
            ptcbhseq->STATEPIN = ptcbhseq->HZNEGPIN;
        }
        else sprintf(buffer, "i_none");
        ptcbhseq->NAME = namealloc(buffer);
        if (ptcbhseq->NEGPIN != NULL)
            sprintf(buffer, "i%s", ptcbhseq->NEGPIN);
        else if (ptcbhseq->HZNEGPIN != NULL)
            sprintf(buffer, "i%s", ptcbhseq->HZNEGPIN);
        else if (ptcbhseq->PIN != NULL)
            sprintf(buffer, "i%s_neg", ptcbhseq->PIN);
        else if (ptcbhseq->HZPIN != NULL)
            sprintf(buffer, "i%s_neg", ptcbhseq->HZPIN);
        else sprintf(buffer, "i_none_neg");
        ptcbhseq->NEGNAME = namealloc(buffer);
        if (!ptcbhseq->STATEPIN) ptcbhseq->SEQTYPE = CBH_UNKNOWN;
    }
    else {
        ptcbhseq->NAME = ptcbhseq->PIN;
        if (!ptcbhseq->NAME) ptcbhseq->NAME = ptcbhseq->HZPIN;
        ptcbhseq->NEGNAME = ptcbhseq->NEGPIN;
        if (!ptcbhseq->NEGNAME) ptcbhseq->NAME = ptcbhseq->HZNEGPIN;
    }

    /* propagate set/reset conflict case */

    if (ptset != NULL && ptreset != NULL) {
        valht = addht(20);
        cbh_setrsconf(valht, ptset, ptreset);
        cbh_evalbereg(pC, ptcellbefig->BEREG, valht);
        for (ptbeout = ptcellbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
            if (ptbeout->NAME == ptcbhseq->PIN)
                cbh_evalbeout(pC, ptbeout, valht);
            if (ptbeout->NAME == ptcbhseq->NEGPIN)
                cbh_evalbeout(pC, ptbeout, valht);
        }
        htres = gethtitem(valht, (void *) (long) searchInputCct(pC, ptcbhseq->PIN));
        if (htres != EMPTYHT && htres != DELETEHT) {
            val = (char) htres;
            if (val == '0')
                ptcbhseq->RSCONF = createAtom("'l'");
            else if (val == '1')
                ptcbhseq->RSCONF = createAtom("'h'");
        }
        htres = gethtitem(valht, (void *) (long) searchInputCct(pC, ptcbhseq->NEGPIN));
        if (htres != EMPTYHT && htres != DELETEHT) {
            val = (char) htres;
            if (val == '0')
                ptcbhseq->RSCONFNEG = createAtom("'l'");
            else if (val == '1')
                ptcbhseq->RSCONFNEG = createAtom("'h'");
        }
        delht(valht);
        if (ptcbhseq->RSCONF == NULL && ptcbhseq->RSCONFNEG == NULL)
            ptcbhseq->RSCONF = createAtom("'x'");
    }

    /* check for polarity of memory node */
    polarity = CBH_NONE;
    ptnode = searchOutputCct(pC, ptcbhseq->STATEPIN);
    ptchain1 = addchain(NULL, ptcellbefig->BEREG->NAME);
    if (ptnode != NULL)
        polarity = cbh_calcpolarity(pC, ptnode, ptchain1);
    freechain(ptchain1);
    ptcbhseq->POLARITY = polarity;
    ptcbhseq->LATCHNAME = ptcellbefig->BEREG->NAME;
    
    /* Invert if STATEPIN is NEGPIN or HZNEGPIN */
    if (ptcbhseq->STATEPIN == ptcbhseq->NEGPIN || ptcbhseq->STATEPIN == ptcbhseq->HZNEGPIN) {
        if (ptcbhseq->POLARITY == CBH_INVERT) ptcbhseq->POLARITY = CBH_NONINVERT;
        else if (ptcbhseq->POLARITY == CBH_NONINVERT) ptcbhseq->POLARITY = CBH_INVERT;
    }

    if (ptcbhseq->POLARITY == CBH_INVERT) {
        if (ptwrite != NULL)
            ptcbhseq->CLOCK = bddToAblCct(pC, ptwrite);
        if (ptdata != NULL)
            ptcbhseq->DATA = bddToAblCct(pC, notBdd(ptdata));
        if (ptset != NULL)
            ptcbhseq->RESET = bddToAblCct(pC, ptset);
        if (ptreset != NULL)
            ptcbhseq->SET = bddToAblCct(pC, ptreset);
    }
    else if (ptcbhseq->POLARITY == CBH_NONINVERT) {
        if (ptwrite != NULL)
            ptcbhseq->CLOCK = bddToAblCct(pC, ptwrite);
        if (ptdata != NULL)
            ptcbhseq->DATA = bddToAblCct(pC, ptdata);
        if (ptreset != NULL)
            ptcbhseq->RESET = bddToAblCct(pC, ptreset);
        if (ptset != NULL)
            ptcbhseq->SET = bddToAblCct(pC, ptset);
    }
    else ptcbhseq->SEQTYPE = CBH_UNKNOWN;

    freechain(ptdatainputs);
    freechain(ptoutlist);
    freechain(ptnoninvertlist);
    freechain(ptinvertlist);
    freechain(pthznoninvertlist);
    freechain(pthzinvertlist);
}

static int
cbh_calcpolarity(pCircuit pC, pNode ptdepend, chain_list * ptdatainputs)
{
    pNode           f0, f1;
    pNode           datavar;
    chain_list     *ptchain;
    int             polarity = CBH_NONE;
    int             index;

    for (ptchain = ptdatainputs; ptchain; ptchain = ptchain->NEXT) {
        index = searchInputCct(pC, (char *) ptchain->DATA);
        datavar = createNodeTermBdd(index);
        f1 = constraintBdd(ptdepend, datavar);
        f0 = constraintBdd(ptdepend, notBdd(datavar));
        if (applyBinBdd(AND, f1, notBdd(f0)) != BDD_zero)
            polarity |= CBH_NONINVERT;
        if (applyBinBdd(AND, f0, notBdd(f1)) != BDD_zero)
            polarity |= CBH_INVERT;
    }
    return polarity;
}

static int
cbh_compare(cbh_pair * pair1, cbh_pair * pair2)
{
    chain_list     *ptchain1;
    chain_list     *ptchain2;
    int             num1, num2;

    ptchain1 = supportChain_listBdd(pair1->OLD);
    ptchain2 = supportChain_listBdd(pair2->OLD);
    num1 = cbh_countchains(ptchain1);
    num2 = cbh_countchains(ptchain2);
    freechain(ptchain1);
    freechain(ptchain2);
    if (num1 > num2)
        return 1;
    else if (num1 < num2)
        return -1;
    else
        return 0;
}

static void
cbh_flipflop(befig_list * ptcellbefig, cbhseq * ptcbhseq, int mode)
{
    pCircuit        pC;
    biabl_list     *ptbiabl;
    beout_list     *ptbeout;
    bebus_list     *ptbebus;
    cbhcomb        *ptcbhcomb;
    berin_list     *ptberin;
    bereg_list     *ptmaster = NULL, *ptslave = NULL;
    pNode           ptval, ptcond;
    pNode           ptset = NULL, ptreset = NULL, ptclock = NULL, ptwrite = BDD_zero,
                    ptdata = NULL;
    pNode           ptslaveset = NULL, ptslavereset = NULL, ptslaveclock = NULL,
                    ptslavewrite = BDD_zero, ptslavedata = NULL;
    pNode           ptnode;
    pNode           ptdepend;
    pNode           ptloopbdd;
    chain_list     *ptdataexpr = NULL;
    chain_list     *ptslavedataexpr = NULL;
    chain_list     *ptdatainputs = NULL;
    chain_list     *ptoutlist = NULL;
    chain_list     *ptnoninvertlist = NULL, *ptinvertlist = NULL;
    chain_list     *pthznoninvertlist = NULL, *pthzinvertlist = NULL;
    chain_list     *ptchain1, *ptchain2;
    ht             *valht;
    long            htres;
    char            val;
    int             polarity, num, numhz;
    short           index;
    char            buffer[256];

    pC = ptcellbefig->CIRCUI;
    ptcbhseq->SEQTYPE = CBH_FLIPFLOP;

    /* identify master and slave */
    for (ptberin = ptcellbefig->BERIN; ptberin; ptberin = ptberin->NEXT) {
        if (ptberin->REG_REF != NULL && ptberin->OUT_REF == NULL) {
            if (ptberin->NAME == ptcellbefig->BEREG->NAME) {
                ptmaster = ptcellbefig->BEREG;
                ptslave = ptcellbefig->BEREG->NEXT;
                break;
            }
            if (ptberin->NAME == ptcellbefig->BEREG->NEXT->NAME) {
                ptmaster = ptcellbefig->BEREG->NEXT;
                ptslave = ptcellbefig->BEREG;
                break;
            }
        }
    }
    if (ptmaster == NULL || ptslave == NULL) {
        ptcbhseq->SEQTYPE = CBH_UNKNOWN;
        return;
    }

    /* obtain master write conditions */
    for (ptbiabl = ptmaster->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        ptcond = ablToBddCct(pC, ptbiabl->CNDABL);
        ptval = ablToBddCct(pC, ptbiabl->VALABL);
        if (ptval == BDD_zero) {
            ptreset = ptcond;
        }
        else if (ptval == BDD_one) {
            ptset = ptcond;
        }
        else {
            if (ptdata == NULL) {
                ptdata = ptval;
                ptclock = ptcond;
                ptdataexpr = ptbiabl->VALABL;
            }
            else {
                ptcbhseq->SEQTYPE = CBH_UNKNOWN;
                return;
            }
        }
        ptwrite = applyBinBdd(OR, ptwrite, ptcond);
    }
    if (ptclock == NULL) {
        ptcbhseq->SEQTYPE = CBH_UNKNOWN;
        return;
    }

    /* eliminate  master priority encoding */
    if (ptset == NULL && ptreset != NULL && applyBinBdd(AND, ptclock, ptreset) == BDD_zero) {
        ptchain1 = supportChain_listBdd(ptclock);
        ptchain2 = supportChain_listBdd(ptreset);
        if (cbh_countchains(ptchain1) > cbh_countchains(ptchain2)) {
            ptclock = constraintBdd(ptclock, notBdd(ptreset));
        }
        else if (cbh_countchains(ptchain2) > cbh_countchains(ptchain1)) {
            ptreset = constraintBdd(ptreset, notBdd(ptclock));
        }
        freechain(ptchain1);
        freechain(ptchain2);
    }
    else if (ptreset == NULL && ptset != NULL && applyBinBdd(AND, ptclock, ptset) == BDD_zero) {
        ptchain1 = supportChain_listBdd(ptclock);
        ptchain2 = supportChain_listBdd(ptset);
        if (cbh_countchains(ptchain1) > cbh_countchains(ptchain2)) {
            ptclock = constraintBdd(ptclock, notBdd(ptset));
        }
        else if (cbh_countchains(ptchain2) > cbh_countchains(ptchain1)) {
            ptset = constraintBdd(ptset, notBdd(ptclock));
        }
        freechain(ptchain1);
        freechain(ptchain2);
    }
    else if (ptreset != NULL && ptset != NULL && applyBinBdd(AND, ptclock, ptset) == BDD_zero
             && applyBinBdd(AND, ptclock, ptreset) == BDD_zero
             && applyBinBdd(AND, ptreset, ptset) == BDD_zero) {
        cbh_pair        pairarray[3];
        int             i;

        pairarray[0].OLD = ptclock;
        pairarray[1].OLD = ptset;
        pairarray[2].OLD = ptreset;
        qsort(pairarray, 3, sizeof(cbh_pair), (void *) cbh_compare);
        pairarray[0].NEW = pairarray[0].OLD;
        pairarray[1].NEW = constraintBdd(pairarray[1].OLD, notBdd(pairarray[0].NEW));
        pairarray[2].NEW = constraintBdd(pairarray[2].OLD, notBdd(pairarray[0].NEW));
        pairarray[2].NEW = constraintBdd(pairarray[2].NEW, notBdd(pairarray[1].NEW));
        for (i = 0; i < 3; i++) {
            if (pairarray[i].OLD == ptclock)
                ptclock = pairarray[i].NEW;
            else if (pairarray[i].OLD == ptset)
                ptset = pairarray[i].NEW;
            else if (pairarray[i].OLD == ptreset)
                ptreset = pairarray[i].NEW;
        }
    }
    if (ptset == BDD_zero)
        ptset = NULL;
    if (ptreset == BDD_zero)
        ptreset = NULL;

    /* obtain slave write conditions */
    for (ptbiabl = ptslave->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        ptcond = ablToBddCct(pC, ptbiabl->CNDABL);
        ptval = ablToBddCct(pC, ptbiabl->VALABL);
        if (ptval == BDD_zero) {
            ptslavereset = ptcond;
        }
        else if (ptval == BDD_one) {
            ptslaveset = ptcond;
        }
        else {
            if (ptslavedata == NULL) {
                ptslavedata = ptval;
                ptslaveclock = ptcond;
                ptslavedataexpr = ptbiabl->VALABL;
            }
            else {
                ptcbhseq->SEQTYPE = CBH_UNKNOWN;
                return;
            }
        }
        ptslavewrite = applyBinBdd(OR, ptwrite, ptcond);
    }
    if (ptslaveclock == NULL) {
        ptcbhseq->SEQTYPE = CBH_UNKNOWN;
        return;
    }

    /* eliminate  slave priority encoding */
    if (ptslaveset == NULL && ptslavereset != NULL && applyBinBdd(AND, ptslaveclock, ptslavereset) == BDD_zero) {
        ptchain1 = supportChain_listBdd(ptslaveclock);
        ptchain2 = supportChain_listBdd(ptslavereset);
        if (cbh_countchains(ptchain1) > cbh_countchains(ptchain2)) {
            ptslaveclock = constraintBdd(ptslaveclock, notBdd(ptslavereset));
        }
        else if (cbh_countchains(ptchain2) > cbh_countchains(ptchain1)) {
            ptslavereset = constraintBdd(ptslavereset, notBdd(ptslaveclock));
        }
        freechain(ptchain1);
        freechain(ptchain2);
    }
    else if (ptslavereset == NULL && ptslaveset != NULL && applyBinBdd(AND, ptslaveclock, ptslaveset) == BDD_zero) {
        ptchain1 = supportChain_listBdd(ptslaveclock);
        ptchain2 = supportChain_listBdd(ptslaveset);
        if (cbh_countchains(ptchain1) > cbh_countchains(ptchain2)) {
            ptslaveclock = constraintBdd(ptslaveclock, notBdd(ptslaveset));
        }
        else if (cbh_countchains(ptchain2) > cbh_countchains(ptchain1)) {
            ptslaveset = constraintBdd(ptslaveset, notBdd(ptslaveclock));
        }
        freechain(ptchain1);
        freechain(ptchain2);
    }
    else if (ptslavereset != NULL && ptslaveset != NULL && applyBinBdd(AND, ptslaveclock, ptslaveset) == BDD_zero
             && applyBinBdd(AND, ptslaveclock, ptslavereset) == BDD_zero
             && applyBinBdd(AND, ptslavereset, ptslaveset) == BDD_zero) {
        cbh_pair        pairarray[3];
        int             i;

        pairarray[0].OLD = ptslaveclock;
        pairarray[1].OLD = ptslaveset;
        pairarray[2].OLD = ptslavereset;
        qsort(pairarray, 3, sizeof(cbh_pair), (void *) cbh_compare);
        pairarray[0].NEW = pairarray[0].OLD;
        pairarray[1].NEW = constraintBdd(pairarray[1].OLD, notBdd(pairarray[0].NEW));
        pairarray[2].NEW = constraintBdd(pairarray[2].OLD, notBdd(pairarray[0].NEW));
        pairarray[2].NEW = constraintBdd(pairarray[2].NEW, notBdd(pairarray[1].NEW));
        for (i = 0; i < 3; i++) {
            if (pairarray[i].OLD == ptslaveclock)
                ptslaveclock = pairarray[i].NEW;
            else if (pairarray[i].OLD == ptslaveset)
                ptslaveset = pairarray[i].NEW;
            else if (pairarray[i].OLD == ptslavereset)
                ptslavereset = pairarray[i].NEW;
        }
    }
    if (ptslaveset == BDD_zero)
        ptslaveset = NULL;
    if (ptslavereset == BDD_zero)
        ptslavereset = NULL;

    /* check if really master/slave */
    /* to do */

    /* obtain list of inverting and non-inverting outputs */
    if (ptdata != NULL) {
        ptdatainputs = supportChain_listExpr(ptdataexpr);
    }
    for (ptbeout = ptcellbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        ptnode = ablToBddCct(pC, ptbeout->ABL);
        addOutputCct(pC, ptbeout->NAME, ptnode);
        if (ptdata != NULL && ptslavedata != NULL) {
            ptdepend = composeBdd(ptnode, ptslavedata, searchInputCct(pC, ptslave->NAME));
            ptdepend = composeBdd(ptdepend, ptdata, searchInputCct(pC, ptmaster->NAME));
            polarity = cbh_calcpolarity(pC, ptdepend, ptdatainputs);
        }
        else {
            ptcbhseq->SEQTYPE = CBH_UNKNOWN;
            return;
        }
        switch (polarity) {
          case CBH_INVERT:
            ptinvertlist = addchain(ptinvertlist, ptbeout->NAME);
            break;
          case CBH_NONINVERT:
            ptnoninvertlist = addchain(ptnoninvertlist, ptbeout->NAME);
            break;
          case CBH_BOTH:
            ptoutlist = addchain(ptoutlist, ptbeout->NAME);
            break;
        }
    }
    for (ptbebus = ptcellbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        ptcbhcomb = cbh_newcomb();
        cbh_calchzfunc(pC, ptbebus->BIABL, ptcbhcomb);
        ptnode = ablToBddCct(pC, ptcbhcomb->FUNCTION);
        cbh_delcomb(ptcbhcomb);
        addOutputCct(pC, ptbebus->NAME, ptnode);
        if (ptdata != NULL && ptslavedata != NULL) {
            ptdepend = composeBdd(ptnode, ptslavedata, searchInputCct(pC, ptslave->NAME));
            ptdepend = composeBdd(ptdepend, ptdata, searchInputCct(pC, ptmaster->NAME));
            polarity = cbh_calcpolarity(pC, ptdepend, ptdatainputs);
        }
        else {
            polarity = CBH_NONE;
        }
        switch (polarity) {
          case CBH_INVERT:
            pthzinvertlist = addchain(pthzinvertlist, ptbebus->NAME);
            break;
          case CBH_NONINVERT:
            pthznoninvertlist = addchain(pthznoninvertlist, ptbebus->NAME);
            break;
          case CBH_BOTH:
          case CBH_NONE:
            ptoutlist = addchain(ptoutlist, ptbebus->NAME);
            break;
        }
    }

    /* detect impossible configurations of inverting and non-inverting
     * outputs */
    num = cbh_countchains(ptinvertlist) + cbh_countchains(ptnoninvertlist) + cbh_countchains(ptoutlist);
    numhz = cbh_countchains(pthzinvertlist) + cbh_countchains(pthznoninvertlist);
    if ((num + numhz) < 1 || num > 2 || numhz > 2 
    || cbh_countchains(ptinvertlist) > 1 || cbh_countchains(ptnoninvertlist) > 1 
    || cbh_countchains(pthzinvertlist) > 1 || cbh_countchains(pthznoninvertlist) > 1) {
        freechain(ptdatainputs);
        freechain(ptoutlist);
        freechain(ptnoninvertlist);
        freechain(ptinvertlist);
        freechain(pthznoninvertlist);
        freechain(pthzinvertlist);
        ptcbhseq->SEQTYPE = CBH_UNKNOWN;
        return;
    }
    /* handle cases of incomplete detection */
    if (cbh_countchains(ptoutlist) == 2) {
        /* try to guess from name */
        if (cbh_testnegname((char *) ptoutlist->DATA) == TRUE) {
            ptinvertlist = ptoutlist;
            ptnoninvertlist = ptoutlist->NEXT;
            ptinvertlist->NEXT = NULL;
            ptoutlist = NULL;
        }
        else {
            ptnoninvertlist = ptoutlist;
            ptinvertlist = ptoutlist->NEXT;
            ptnoninvertlist->NEXT = NULL;
            ptoutlist = NULL;
        }
    }
    else if (num == 2 && cbh_countchains(ptoutlist) == 1) {
        if (ptinvertlist == NULL) {
            ptinvertlist = ptoutlist;
            ptoutlist = NULL;
        }
        else if (ptnoninvertlist == NULL) {
            ptnoninvertlist = ptoutlist;
            ptoutlist = NULL;
        }
    }
    else if (num == 1) {
        if (ptoutlist != NULL) {
            ptnoninvertlist = ptoutlist;
            ptoutlist = NULL;
        }
        else if (ptinvertlist != NULL) {
            ptnoninvertlist = ptinvertlist;
            ptinvertlist = NULL;
        }
    }

    /* Affect flip-flop variables and output pins handling single var case */
    if (ptnoninvertlist != NULL)
        ptcbhseq->PIN = ptnoninvertlist->DATA;
    if (ptinvertlist != NULL)
        ptcbhseq->NEGPIN = ptinvertlist->DATA;
    if (pthznoninvertlist != NULL)
        ptcbhseq->HZPIN = pthznoninvertlist->DATA;
    if (pthzinvertlist != NULL)
        ptcbhseq->HZNEGPIN = pthzinvertlist->DATA;
    if (mode == CBH_LIB_MODE) {
        if (ptcbhseq->PIN != NULL) {
            sprintf(buffer, "i%s", ptcbhseq->PIN);
            ptcbhseq->STATEPIN = ptcbhseq->PIN;
        }
        else if (ptcbhseq->HZPIN != NULL) {
            sprintf(buffer, "i%s", ptcbhseq->HZPIN);
            ptcbhseq->STATEPIN = ptcbhseq->HZPIN;
        }
        else if (ptcbhseq->NEGPIN != NULL) {
            sprintf(buffer, "i%s_neg", ptcbhseq->NEGPIN);
            ptcbhseq->STATEPIN = ptcbhseq->NEGPIN;
        }
        else if (ptcbhseq->HZNEGPIN != NULL) {
            sprintf(buffer, "i%s_neg", ptcbhseq->HZNEGPIN);
            ptcbhseq->STATEPIN = ptcbhseq->HZNEGPIN;
        }
        else sprintf(buffer, "i_none");
        ptcbhseq->NAME = namealloc(buffer);
        if (ptcbhseq->NEGPIN != NULL)
            sprintf(buffer, "i%s", ptcbhseq->NEGPIN);
        else if (ptcbhseq->HZNEGPIN != NULL)
            sprintf(buffer, "i%s", ptcbhseq->HZNEGPIN);
        else if (ptcbhseq->PIN != NULL)
            sprintf(buffer, "i%s_neg", ptcbhseq->PIN);
        else if (ptcbhseq->HZPIN != NULL)
            sprintf(buffer, "i%s_neg", ptcbhseq->HZPIN);
        else sprintf(buffer, "i_none_neg");
        ptcbhseq->NEGNAME = namealloc(buffer);
        if (!ptcbhseq->STATEPIN) ptcbhseq->SEQTYPE = CBH_UNKNOWN;
    }
    else {
        if (ptcbhseq->PIN != NULL) ptcbhseq->STATEPIN = ptcbhseq->PIN;
        else if (ptcbhseq->HZPIN != NULL) ptcbhseq->STATEPIN = ptcbhseq->HZPIN;
        else if (ptcbhseq->NEGPIN != NULL) ptcbhseq->STATEPIN = ptcbhseq->NEGPIN;
        else if (ptcbhseq->HZNEGPIN != NULL) ptcbhseq->STATEPIN = ptcbhseq->HZNEGPIN;
        ptcbhseq->NAME = ptcbhseq->STATEPIN;
        if (!ptcbhseq->STATEPIN) ptcbhseq->SEQTYPE = CBH_UNKNOWN;
    }

    /* Handle case of toggle loop */
    if (ptdatainputs != NULL) {
        for (ptchain1 = ptdatainputs; ptchain1; ptchain1 = ptchain1->NEXT) {
            if (ptchain1->DATA == ptslave->NAME) {
                polarity = CBH_NONINVERT;
                ptchain2 = addchain(NULL, ptmaster->NAME);
                polarity = cbh_calcpolarity(pC, ptslavedata, ptchain2);
                freechain(ptchain2);
                index = addInputCct(pC, ptcbhseq->NAME);
                ptloopbdd = createNodeTermBdd(index);
                if (polarity == CBH_NONINVERT) {
                    ptdata = composeBdd(ptdata, ptloopbdd, searchInputCct(pC, ptslave->NAME));
                }
                else if (polarity == CBH_INVERT) {
                    ptdata = composeBdd(ptdata, notBdd(ptloopbdd), searchInputCct(pC, ptslave->NAME));
                }
                else {
                    freechain(ptdatainputs);
                    ptcbhseq->SEQTYPE = CBH_UNKNOWN;
                    return;
                }
                if (ptset)
                    ptdata = constraintBdd(ptdata, notBdd(ptset));
                if (ptreset)
                    ptdata = constraintBdd(ptdata, notBdd(ptreset));
                break;
            }
        }
    }

    /* propagate set/reset conflict case */
    if (ptset != NULL && ptreset != NULL) {
        valht = addht(20);
        cbh_setrsconf(valht, ptset, ptreset);
        cbh_evalbereg(pC, ptmaster, valht);
        cbh_evalbereg(pC, ptslave, valht);
        for (ptbeout = ptcellbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
            if (ptbeout->NAME == ptcbhseq->PIN)
                cbh_evalbeout(pC, ptbeout, valht);
            if (ptbeout->NAME == ptcbhseq->NEGPIN)
                cbh_evalbeout(pC, ptbeout, valht);
        }
        htres = gethtitem(valht, (void *) (long) searchInputCct(pC, ptcbhseq->PIN));
        if (htres != EMPTYHT && htres != DELETEHT) {
            val = (char) htres;
            if (val == '0')
                ptcbhseq->RSCONF = createAtom("'l'");
            else if (val == '1')
                ptcbhseq->RSCONF = createAtom("'h'");
        }
        htres = gethtitem(valht, (void *) (long) searchInputCct(pC, ptcbhseq->NEGPIN));
        if (htres != EMPTYHT && htres != DELETEHT) {
            val = (char) htres;
            if (val == '0')
                ptcbhseq->RSCONFNEG = createAtom("'l'");
            else if (val == '1')
                ptcbhseq->RSCONFNEG = createAtom("'h'");
        }
        delht(valht);
        if (ptcbhseq->RSCONF == NULL && ptcbhseq->RSCONFNEG == NULL) {
            ptcbhseq->RSCONF = createAtom("'x'");
        }
    }

    /* check for polarity of master memory node */
    polarity = CBH_NONE;
    ptnode = searchOutputCct(pC, ptcbhseq->STATEPIN);
    ptchain1 = addchain(NULL, ptmaster->NAME);
    if (ptnode != NULL) {
        ptdepend = composeBdd(ptnode, ptslavedata, searchInputCct(pC, ptslave->NAME));
        polarity = cbh_calcpolarity(pC, ptdepend, ptchain1);
    }
    ptcbhseq->POLARITY = polarity;
    ptcbhseq->MSPOLARITY = cbh_calcpolarity(pC, createNodeTermBdd(searchInputCct(pC, ptslave->NAME)), ptchain1);
    freechain(ptchain1);
    ptcbhseq->LATCHNAME = ptmaster->NAME;
    ptcbhseq->SLAVENAME = ptslave->NAME;

    /* Invert if STATEPIN is NEGPIN or HZNEGPIN */
    if (ptcbhseq->STATEPIN == ptcbhseq->NEGPIN || ptcbhseq->STATEPIN == ptcbhseq->HZNEGPIN) {
        if (ptcbhseq->POLARITY == CBH_INVERT) ptcbhseq->POLARITY = CBH_NONINVERT;
        else if (ptcbhseq->POLARITY == CBH_NONINVERT) ptcbhseq->POLARITY = CBH_INVERT;
    }

    if (polarity == CBH_INVERT) {
        if (ptslaveclock != notBdd(ptclock)) {
            ptcbhseq->SLAVECLOCK = bddToAblCct(pC, ptslaveclock);
            ptcbhseq->CLOCK = bddToAblCct(pC, ptclock);
        }
        else {
            ptcbhseq->CLOCK = bddToAblCct(pC, ptslaveclock);
        }
        if (ptdata != NULL)
            ptcbhseq->DATA = bddToAblCct(pC, notBdd(ptdata));
        if (ptset != NULL)
            ptcbhseq->RESET = bddToAblCct(pC, ptset);
        if (ptreset != NULL)
            ptcbhseq->SET = bddToAblCct(pC, ptreset);
    }
    else if (ptcbhseq->POLARITY == CBH_NONINVERT) {
        if (ptslaveclock != notBdd(ptclock)) {
            ptcbhseq->SLAVECLOCK = bddToAblCct(pC, ptslaveclock);
            ptcbhseq->CLOCK = bddToAblCct(pC, ptclock);
        }
        else {
            ptcbhseq->CLOCK = bddToAblCct(pC, ptslaveclock);
        }
        if (ptdata != NULL)
            ptcbhseq->DATA = bddToAblCct(pC, ptdata);
        if (ptreset != NULL)
            ptcbhseq->RESET = bddToAblCct(pC, ptreset);
        if (ptset != NULL)
            ptcbhseq->SET = bddToAblCct(pC, ptset);
    }
    else ptcbhseq->SEQTYPE = CBH_UNKNOWN;

    freechain(ptdatainputs);
    freechain(ptoutlist);
    freechain(ptnoninvertlist);
    freechain(ptinvertlist);
    freechain(pthznoninvertlist);
    freechain(pthzinvertlist);
}

/* Functions to evaluate SET/RESET conflict */

static int
cbh_setrsconf(ht * valht, pNode ptset, pNode ptreset)
{
    chain_list     *ptsupport, *ptchain;
    pNode           ptvar;

    ptsupport = supportChain_listBdd(ptset);
    for (ptchain = ptsupport; ptchain; ptchain = ptchain->NEXT) {
        ptvar = (pNode) ptchain->DATA;
        if (gethtitem(valht, ptvar) != EMPTYHT)
        {
            freechain(ptsupport);
            return FALSE;
        }
        if (ptvar->high == BDD_one)
            addhtitem(valht, (void *) (long) ptvar->index, (int) '1');
        else if (ptvar->high == BDD_zero)
            addhtitem(valht, (void *) (long) ptvar->index, (int) '0');
    }
    freechain(ptsupport);
    ptsupport = supportChain_listBdd(ptreset);
    for (ptchain = ptsupport; ptchain; ptchain = ptchain->NEXT) {
        ptvar = (pNode) ptchain->DATA;
        if (gethtitem(valht, ptvar) != EMPTYHT)
        {
            freechain(ptsupport);
            return FALSE;
        }
        if (ptvar->high == BDD_one)
            addhtitem(valht, (void *) (long) ptvar->index, (int) '1');
        else if (ptvar->high == BDD_zero)
            addhtitem(valht, (void *) (long) ptvar->index, (int) '0');
    }
    freechain(ptsupport);
    return TRUE;
}

static          pNode
cbh_evalbereg(pCircuit ptcircuit, bereg_list * ptbereg, ht * valht)
{
    biabl_list     *ptbiabl;
    pNode           cndval = NULL, drvval = NULL;
    pNode           ptcnd = NULL, ptval = NULL;
    int             precede, prevcndval = FALSE;

    ptbiabl = ptbereg->BIABL;
    while (ptbiabl != NULL) {
        ptcnd = ablToBddCct(ptcircuit, ptbiabl->CNDABL);
        ptval = ablToBddCct(ptcircuit, ptbiabl->VALABL);
        if ((ptbiabl->FLAG & BEH_CND_NOPRECEDE) != 0)
            prevcndval = FALSE;
        precede = (prevcndval && ((ptbiabl->FLAG & BEH_CND_PRECEDE) != 0));
        if (precede)
            cndval = BDD_zero;
        else
            cndval = cbh_bddeval(ptcnd, valht);

        if (cndval == BDD_one) {
            if (drvval == NULL) {
                drvval = cbh_bddeval(ptval, valht);
            }
            else
                return NULL;
            prevcndval = TRUE;
        }
        ptbiabl = ptbiabl->NEXT;
    }
    if (drvval == BDD_one) {
        addhtitem(valht, (void *) (long) searchInputCct(ptcircuit, ptbereg->NAME), '1');
    }
    else if (drvval == BDD_zero) {
        addhtitem(valht, (void *) (long) searchInputCct(ptcircuit, ptbereg->NAME), '0');
    }
    return drvval;
}

static          pNode
cbh_evalbeout(pCircuit ptcircuit, beout_list * ptbeout, ht * valht)
{
    pNode           drvval;

    addInputCct(ptcircuit, ptbeout->NAME);
    ptbeout->NODE = ablToBddCct(ptcircuit, ptbeout->ABL);
    drvval = cbh_bddeval(ptbeout->NODE, valht);
    if (drvval == BDD_one) {
        addhtitem(valht, (void *) (long) searchInputCct(ptcircuit, ptbeout->NAME), '1');
    }
    else if (drvval == BDD_zero) {
        addhtitem(valht, (void *) (long) searchInputCct(ptcircuit, ptbeout->NAME), '0');
    }
    return drvval;
}

static          pNode
cbh_bddeval(pNode ptbdd, ht * valht)
{
    chain_list     *ptsupport;
    chain_list     *ptchain;
    long            htres;
    long            index;
    char            val;

    if (ptbdd == BDD_one || ptbdd == BDD_zero || ptbdd == NULL)
        return ptbdd;
    ptsupport = supportIndexBdd(ptbdd, 0);
    for (ptchain = ptsupport; ptchain; ptchain = ptchain->NEXT) {
        index = (long) ptchain->DATA;
        htres = gethtitem(valht, (void *) index);
        if (htres == EMPTYHT || htres == DELETEHT)
            continue;
        val = (char) htres;
        if (val == '0') {
            ptbdd = constraintBdd(ptbdd, notBdd(createNodeTermBdd(index)));
        }
        else if (val == '1') {
            ptbdd = constraintBdd(ptbdd, createNodeTermBdd(index));
        }
        else
        {
            freechain(ptsupport);
            return NULL;
        }
    }
    freechain(ptsupport);
    return ptbdd;
}
