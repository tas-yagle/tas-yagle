/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE v3.50                                                 */
/*    Fichier : yag_constrain.c                                             */
/*                                                                          */
/*    (c) copyright 1995 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 30/05/1995     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static chain_list *getOppositeSwitchList(losig_list *ptsig);
static void markConstrained (list_list *headlist);
static locon_list *getLocon (char *name);
static cone_list *getCone (char *name);

static ht *YAG_CONSTRAINT_HT = NULL;
static ht *YAG_VARLIST_HT = NULL;

/*****************************************************************************
 *                          function yagInitConstraints()                    *
 *****************************************************************************/

/* generate the list of constraints, mark the connectors and cones */
/* concerned, and generate the list of constrained variables.      */

void 
yagInitConstraints(inffig_list *ifl)
{
    chain_list     *ptconstraints = NULL;
    chain_list     *ptchain, *ptvarlist;
    list_list      *ptlist;
    void           *object;
    long           *common;

    ptconstraints = yagMuxToConstraint(ifl);
    if (ptconstraints != NULL) {
        if (yagCountChains(ptconstraints) > 10) {
            YAG_CONSTRAINT_HT = addht(100);
            YAG_VARLIST_HT = addht(100);
        }
        else {
            YAG_CONSTRAINT_HT = NULL;
            YAG_VARLIST_HT = NULL;
        }
        markConstrained((list_list *)ptconstraints->DATA);
        YAG_CONTEXT->YAG_CONSTRAINT_LIST = ptconstraints;
        if (YAG_VARLIST_HT) {
            for (ptlist = (list_list *)ptconstraints->DATA; ptlist; ptlist = ptlist->NEXT) {
                addhtitem(YAG_VARLIST_HT, ptlist->DATA, (long)ptlist);
            }
        }
        for (ptchain = YAG_CONTEXT->YAG_CONSTRAINT_LIST->NEXT; ptchain; ptchain = ptchain->NEXT) {
            common=(long *)mbkalloc(sizeof(long));
            *common=0;
            for (ptlist = (list_list *) ptchain->DATA; ptlist; ptlist = ptlist->NEXT) {
                ptlist->COMMON=common;
                (*common)++;
                if (YAG_CONSTRAINT_HT) {
                    object = ptlist->DATA;
                    if ((ptvarlist = (chain_list *)gethtitem(YAG_CONSTRAINT_HT, object)) != (void *)EMPTYHT) {
                        ptvarlist = addchain(ptvarlist, ptlist);
                        sethtitem(YAG_CONSTRAINT_HT, object, (long)ptvarlist);
                    }
                    else addhtitem(YAG_CONSTRAINT_HT, object, (long)addchain(NULL, ptlist));
                }
            }
        }
    }
}

void 
yagDeleteConstraints()
{
    chain_list     *ptchain, *ptvarlist;
    list_list      *ptlist, *ptnextlist;
    void           *object;

    if (YAG_CONTEXT->YAG_CONSTRAINT_LIST != NULL) {
        for (ptlist = (list_list *)YAG_CONTEXT->YAG_CONSTRAINT_LIST->DATA; ptlist; ptlist = ptnextlist) {
            ptnextlist = ptlist->NEXT;
            freechain(ptlist->SUPDATA);
            mbkfree(ptlist);
        }
        for (ptchain = YAG_CONTEXT->YAG_CONSTRAINT_LIST->NEXT; ptchain; ptchain = ptchain->NEXT) {
            for (ptlist = (list_list *) ptchain->DATA; ptlist; ptlist = ptnextlist) {
                ptnextlist = ptlist->NEXT;
                freechain(ptlist->SUPDATA);
                if (ptlist == ptchain->DATA) mbkfree(ptlist->COMMON);
                if (YAG_CONSTRAINT_HT) {
                    object = ptlist->DATA;
                    ptvarlist = (chain_list *)gethtitem(YAG_CONSTRAINT_HT, object);
                    if (ptvarlist != (void *)EMPTYHT && ptvarlist != (void *)DELETEHT) {
                        freechain(ptvarlist);
                        delhtitem(YAG_CONSTRAINT_HT, object);
                    }
                }
                mbkfree(ptlist);
            }
        }
        freechain(YAG_CONTEXT->YAG_CONSTRAINT_LIST);
        YAG_CONTEXT->YAG_CONSTRAINT_LIST = NULL;
        if (YAG_CONSTRAINT_HT) {
            delht(YAG_CONSTRAINT_HT);
            YAG_CONSTRAINT_HT = NULL;
        }
        if (YAG_VARLIST_HT) {
            delht(YAG_VARLIST_HT);
            YAG_VARLIST_HT = NULL;
        }
    }
}

/*****************************************************************************
 *                          function yagGetConstraint()                      *
 *****************************************************************************/

/* scans the list of constraints and return the first */
/* for which all the variables have been tagged.      */

chain_list *
yagGetConstraint(chain_list *ptcontraints)
{
    chain_list     *ptchain;
    list_list      *ptlist;
    long common;
    for (ptchain = ptcontraints; ptchain; ptchain = ptchain->NEXT) {
        ptlist = (list_list *)ptchain->DATA;
        common = *(long *)ptlist->COMMON;
        if (common==0) return ptchain;
    }
    return ptchain;
}

/*****************************************************************************
 *                          function yagTagConstraint()                    *
 *****************************************************************************/

/* tag the constraint variables which refer to the given object */

void 
yagTagConstraint(void *object)
{
    chain_list     *pt;
    chain_list     *ptchain, *ptvarlist;
    list_list      *list;
    list_list      *ptmainvar;
    chain_list     *ptnamechain;
    char           *ptothername;


    if (YAG_CONSTRAINT_HT) {
        if ((ptvarlist = (chain_list *)gethtitem(YAG_CONSTRAINT_HT, object)) != (void *)EMPTYHT) {
            for (ptchain = ptvarlist; ptchain; ptchain = ptchain->NEXT) {
                list=(list_list *)ptchain->DATA;
                if (list->TYPE == 0) {
                    (*(long *)list->COMMON)--;
                    list->TYPE = -1L;
                }
            }
        }
        /* Tag also the opposite switch variables */
        ptmainvar = (list_list *)gethtitem(YAG_VARLIST_HT, object);
        if (ptmainvar != (list_list *)EMPTYHT) {
            for (ptnamechain = ptmainvar->SUPDATA; ptnamechain; ptnamechain = ptnamechain->NEXT) {
                ptothername = (char *)ptnamechain->DATA;
                if ((ptvarlist = (chain_list *)gethtitem(YAG_CONSTRAINT_HT, ptothername)) != (void *)EMPTYHT) {
                    for (ptchain = ptvarlist; ptchain; ptchain = ptchain->NEXT) {
                        list=(list_list *)ptchain->DATA;
                        if (list->TYPE == 0) {
                            (*(long *)list->COMMON)--;
                            list->TYPE = -1L;
                        }
                    }
                }
            }
        }
    }
    else if (YAG_CONTEXT->YAG_CONSTRAINT_LIST) { /* Without the hash table */
        for (pt = YAG_CONTEXT->YAG_CONSTRAINT_LIST->NEXT; pt; pt = pt->NEXT) {
            for (list = (list_list *) pt->DATA; list; list = list->NEXT) {
                if (object == list->DATA) {
                    if (list->TYPE == 0) {
                        (*(long *)list->COMMON)--;
                        list->TYPE = -1L;
                    }
                }
            }
        }
        /* Tag also the opposite switch variables */
        for (ptmainvar = YAG_CONTEXT->YAG_CONSTRAINT_LIST->DATA; ptmainvar && ptmainvar->DATA != object; ptmainvar = ptmainvar->NEXT);
        if (ptmainvar) {
            for (ptnamechain = ptmainvar->SUPDATA; ptnamechain; ptnamechain = ptnamechain->NEXT) {
                ptothername = (char *)ptnamechain->DATA;
                for (pt = YAG_CONTEXT->YAG_CONSTRAINT_LIST->NEXT; pt; pt = pt->NEXT) {
                    for (list = (list_list *) pt->DATA; list; list = list->NEXT) {
                        if (ptothername == list->DATA) {
                            if (list->TYPE == 0) {
                                (*(long *)list->COMMON)--;
                                list->TYPE = -1L;
                            }
                        }
                    }
                }
            }
        }
    }
}

/*****************************************************************************
 *                          function yagRemoveConstraintTags()               *
 *****************************************************************************/

/* remove all the tags of the constraint list */

void 
yagRemoveConstraintTags()
{
    chain_list     *ptchain;
    list_list      *ptlist;

    for (ptchain = YAG_CONTEXT->YAG_CONSTRAINT_LIST->NEXT; ptchain; ptchain = ptchain->NEXT) {
        for (ptlist = (list_list *)ptchain->DATA; ptlist; ptlist = ptlist->NEXT)
        {
           (*(long *)ptlist->COMMON)++;
        }
    }
}


/*****************************************************************************
 *                          function yagGetOppositeConstraintVars()          *
 *****************************************************************************/

/* given a name returns list of any names on other side of eventual switches */

chain_list *
yagGetOppositeConstraintVars(char *name)
{
    list_list *ptmainvar;

    if (YAG_CONTEXT->YAG_CONSTRAINT_LIST == NULL) return NULL;
    if (YAG_VARLIST_HT) {
        ptmainvar = (list_list *)gethtitem(YAG_VARLIST_HT, name);
        if (ptmainvar != (list_list *)EMPTYHT) {
            return ptmainvar->SUPDATA;
        }
    }
    else {
        for (ptmainvar = YAG_CONTEXT->YAG_CONSTRAINT_LIST->DATA; ptmainvar && ptmainvar->DATA != name; ptmainvar = ptmainvar->NEXT);
        if (ptmainvar) {
            return ptmainvar->SUPDATA;
        }
    }
    return NULL;    
}

/*****************************************************************************
 *                          function getOppositeSwitchList()               *
 *****************************************************************************/

/* given a losig returns list of any names on other side of eventual switches */

static chain_list *
getOppositeSwitchList(losig_list *ptsig)
{
    chain_list *reschain = NULL;
    chain_list *loconchain;
    chain_list *ptchain;
    locon_list *ptlocon;
    lotrs_list *pttrans, *ptothertrans;
    char       *oppositename;
    ptype_list *ptuser;
    

    loconchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
        ptlocon = (locon_list *)ptchain->DATA;
        if (ptlocon->TYPE == 'T' && ptlocon->NAME == CNS_GRIDNAME) {
            pttrans = ptlocon->ROOT;
            if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
                ptothertrans = (lotrs_list *)ptuser->DATA;
                oppositename = ((cone_list *)ptothertrans->GRID)->NAME;
                if (yagGetChain(reschain, oppositename) == NULL) {
                    reschain = addchain(reschain, oppositename);
                }
            }
        }
    }
    return reschain;
}

/*****************************************************************************
 *                          function markConstrained()                       *
 *****************************************************************************/

/* mark all constrained cones and connectors */
/* build list of opposite switch names */

static void 
markConstrained(list_list *headlist)
{
    list_list      *ptlist;
    locon_list     *ptcon;
    cone_list      *ptcone;
    losig_list     *ptsig;
    char           *varName;
    int             found;

    for (ptlist = headlist; ptlist; ptlist = ptlist->NEXT) {
        varName = (char *)ptlist->DATA;
        found = FALSE;
        ptsig = NULL;
        if ((ptcon = getLocon(varName)) != NULL) {
            ptcon->USER = addptype(ptcon->USER, YAG_CONSTRAINT_PTYPE, NULL);
            found = TRUE;
        }
        if ((ptcone = getCone(varName)) != NULL) {
            ptcone->TYPE |= YAG_CONSTRAINT;
            ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
            found = TRUE;
        }
        if (found == FALSE) {
            avt_errmsg(YAG_ERRMSG, "008", AVT_FATAL, varName);
        }
        if (ptsig) ptlist->SUPDATA = getOppositeSwitchList(ptsig);
    }
}

/*****************************************************************************
 *                          function getLocon()                              *
 *****************************************************************************/

/* returns a connector pointer from its name */

static locon_list *
getLocon(char *name)
{
    locon_list     *ptcon;

    for (ptcon = CNS_HEADCNSFIG->LOCON; ptcon != NULL; ptcon = ptcon->NEXT) {
        if (ptcon->NAME == name) break;
    }
    return ptcon;
}

/*****************************************************************************
 *                          function getCone()                               *
 *****************************************************************************/

/* returns a cone pointer from its name */

static cone_list *
getCone(char *name)
{
    cone_list  *ptcone = NULL;

    for (ptcone = CNS_HEADCNSFIG->CONE; ptcone; ptcone = ptcone->NEXT) {
        if (ptcone->NAME == name) break;
    }
    return ptcone;
}

