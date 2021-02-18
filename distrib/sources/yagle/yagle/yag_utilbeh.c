/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_utilbeh.c                                               */
/*                                                                          */
/*    (c) copyright 1993 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 8/05/1994      */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static ht   *auxvecht;
static ht   *buxvecht;
static ht   *porvecht;
static ht   *outvecht;
static ht   *invecht;
static ht   *busvecht;
static ht   *regvecht;

static ht   *beporht = NULL;
static ht   *beauxht = NULL;
static ht   *bebuxht = NULL;
static ht   *beoutht = NULL;
static ht   *bebusht = NULL;
static ht   *bereght = NULL;

static int behdefinedht = FALSE;

befig_list  *yagInitBefig(name)
    char    *name;
{
    befig_list  *ptbefig;

    auxvecht = addht(100);
    buxvecht = addht(20);
    porvecht = addht(40);
    outvecht = addht(20);
    invecht = addht(20);
    busvecht = addht(20);
    regvecht = addht(20);

    ptbefig = beh_addbefig(NULL, name);
    ptbefig->TYPE = BEH_NODE_QUAD|BEH_NODE_USER;
    return ptbefig;
}

void yagInitBehDefined()
{
    beporht = addht(100);
    beauxht = addht(500);
    bebuxht = addht(200);
    beoutht = addht(100);
    bebusht = addht(100);
    bereght = addht(100);
    behdefinedht = TRUE;
}

void yagCleanBehDefined()
{
    delht(beporht);
    delht(beauxht);
    delht(bebuxht);
    delht(beoutht);
    delht(bebusht);
    delht(bereght);
    beporht = NULL;
    beauxht = NULL;
    bebuxht = NULL;
    beoutht = NULL;
    bebusht = NULL;
    bereght = NULL;
    behdefinedht = FALSE;
}

void
yagCleanBehVect()
{
    delht(auxvecht);
    delht(buxvecht);
    delht(porvecht);
    delht(outvecht);
    delht(invecht);
    delht(busvecht);
    delht(regvecht);
}

void
yagCleanBequad(befig_list *ptbefig)
{
    beaux_list  *ptbeaux;
    bebux_list  *ptbebux;
    bebus_list  *ptbebus;
    beout_list  *ptbeout;
    bereg_list  *ptbereg;
    bequad_list *ptbequad;
    binode_list *ptbinode;
    
    if (ptbefig == NULL) return;
    ptbefig->TYPE = 0;
    for (ptbebux = ptbefig->BEBUX; ptbebux; ptbebux = ptbebux->NEXT) {
        for (ptbinode = ptbebux->BINODE; ptbinode; ptbinode = ptbinode->NEXT) {
            ptbequad = (bequad_list *)ptbinode->VALNODE;
            if (ptbequad != NULL) {
                if (getptype(ptbequad->USER, YAG_CONE_PTYPE) != NULL) {
                    ptbequad->USER = delptype(ptbequad->USER, YAG_CONE_PTYPE);
                }
                beh_delbequad(ptbequad, 'N');
                ptbinode->VALNODE = NULL;
            }
            ptbequad = (bequad_list *)ptbinode->CNDNODE;
            if (ptbequad != NULL) {
                if (getptype(ptbequad->USER, YAG_CONE_PTYPE) != NULL) {
                    ptbequad->USER = delptype(ptbequad->USER, YAG_CONE_PTYPE);
                }
                beh_delbequad(ptbequad, 'N');
                ptbinode->CNDNODE = NULL;
            }
        }
        beh_frebinode(ptbebux->BINODE);
        ptbebux->BINODE = NULL;
    }
    for (ptbebus = ptbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        for (ptbinode = ptbebus->BINODE; ptbinode; ptbinode = ptbinode->NEXT) {
            ptbequad = (bequad_list *)ptbinode->VALNODE;
            if (ptbequad != NULL) {
                if (getptype(ptbequad->USER, YAG_CONE_PTYPE) != NULL) {
                    ptbequad->USER = delptype(ptbequad->USER, YAG_CONE_PTYPE);
                }
                beh_delbequad(ptbequad, 'N');
                ptbinode->VALNODE = NULL;
            }
            ptbequad = (bequad_list *)ptbinode->CNDNODE;
            if (ptbequad != NULL) {
                if (getptype(ptbequad->USER, YAG_CONE_PTYPE) != NULL) {
                    ptbequad->USER = delptype(ptbequad->USER, YAG_CONE_PTYPE);
                }
                beh_delbequad(ptbequad, 'N');
                ptbinode->CNDNODE = NULL;
            }
        }
        beh_frebinode(ptbebus->BINODE);
        ptbebus->BINODE = NULL;
    }
    for (ptbereg = ptbefig->BEREG; ptbereg; ptbereg = ptbereg->NEXT) {
        for (ptbinode = ptbereg->BINODE; ptbinode; ptbinode = ptbinode->NEXT) {
            ptbequad = (bequad_list *)ptbinode->VALNODE;
            if (ptbequad != NULL) {
                if (getptype(ptbequad->USER, YAG_CONE_PTYPE) != NULL) {
                    ptbequad->USER = delptype(ptbequad->USER, YAG_CONE_PTYPE);
                }
                beh_delbequad(ptbequad, 'N');
                ptbinode->VALNODE = NULL;
            }
            ptbequad = (bequad_list *)ptbinode->CNDNODE;
            if (ptbequad != NULL) {
                if (getptype(ptbequad->USER, YAG_CONE_PTYPE) != NULL) {
                    ptbequad->USER = delptype(ptbequad->USER, YAG_CONE_PTYPE);
                }
                beh_delbequad(ptbequad, 'N');
                ptbinode->CNDNODE = NULL;
            }
        }
        beh_frebinode(ptbereg->BINODE);
        ptbereg->BINODE = NULL;
    }
    for (ptbeaux = ptbefig->BEAUX; ptbeaux; ptbeaux = ptbeaux->NEXT) {
        ptbequad = (bequad_list *)ptbeaux->NODE;
        if (ptbequad != NULL) {
            if (getptype(ptbequad->USER, YAG_CONE_PTYPE) != NULL) {
                ptbequad->USER = delptype(ptbequad->USER, YAG_CONE_PTYPE);
            }
            beh_delbequad(ptbequad, 'N');
            ptbeaux->NODE = NULL;
        }
    }
    for (ptbeout = ptbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        ptbequad = (bequad_list *)ptbeout->NODE;
        if (ptbequad != NULL) {
            if (getptype(ptbequad->USER, YAG_CONE_PTYPE) != NULL) {
                ptbequad->USER = delptype(ptbequad->USER, YAG_CONE_PTYPE);
            }
            beh_delbequad(ptbequad, 'N');
            ptbeout->NODE = NULL;
        }
    }
}

bepor_list *
yagAddBeporVect(ptbefig, name, dir, type)
    befig_list  *ptbefig;
    char        *name;
    char        dir;
    char        type;
{
    bepor_list  *listpos, *prevpos;
    union beobj  newbeobj;
    char        *stem;
    char        *nextname;
    int         index;
    
    stem = vectorradical(name);
    if (!YAG_CONTEXT->YAG_INTERFACE_VECTORS || stem == name) {
        newbeobj.BEPOR = beh_addbepor(ptbefig->BEPOR, name, dir, type);
        ptbefig->BEPOR = newbeobj.BEPOR;
    }
    else {
        if ((listpos = (bepor_list *)gethtitem(porvecht, stem)) == (bepor_list *)EMPTYHT) {
            newbeobj.BEPOR = beh_addbepor(ptbefig->BEPOR, name, dir, type);
            ptbefig->BEPOR = newbeobj.BEPOR;
            addhtitem(porvecht, stem, (long)ptbefig->BEPOR);
        }
        else {
            index = vectorindex(name);
            if (vectorindex(listpos->NAME) > index) {
                if (listpos == ptbefig->BEPOR) {
                    newbeobj.BEPOR = beh_addbepor(ptbefig->BEPOR, name, dir, type);
                    ptbefig->BEPOR = newbeobj.BEPOR;
                }
                else {
                    for (prevpos = ptbefig->BEPOR; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    newbeobj.BEPOR = beh_addbepor(listpos, name, dir, type);
                    prevpos->NEXT = newbeobj.BEPOR;
                }
                sethtitem(porvecht, stem, (long)newbeobj.BEPOR);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = prevpos->NEXT->NAME;
                    if (vectorindex(nextname) > index || vectorradical(nextname) != stem) break;
                }
                newbeobj.BEPOR = beh_addbepor(prevpos->NEXT, name, dir, type);
                prevpos->NEXT = newbeobj.BEPOR;
            }
        }
    }
    if (behdefinedht) yagBehSetDefine(name, YAG_BEPOR, newbeobj);
    return newbeobj.BEPOR;
}

beout_list *
yagAddBeoutVect(ptbefig, name, expr, ptbequad, ptcone, time)
    befig_list  *ptbefig;
    char        *name;
    chain_list  *expr;
    bequad_list *ptbequad;
    cone_list   *ptcone;
    unsigned int time;
{
    beout_list  *listpos, *prevpos;
    union beobj  newbeobj;
    char        *stem;
    char        *nextname;
    int         index;

    if (ptbequad == NULL && YAG_CONTEXT->YAG_BEFIG) {
        ptbequad = beh_addbequad(NULL, NULL, NULL, NULL);
        ptbequad->USER = addptype(ptbequad->USER, YAG_CONE_PTYPE, ptcone);
    }
    
    stem = vectorradical(name);
    if (!YAG_CONTEXT->YAG_INTERFACE_VECTORS || stem == name) {
        newbeobj.BEOUT = beh_addbeout(ptbefig->BEOUT, name, expr, (pNode)ptbequad, 0);
        ptbefig->BEOUT = newbeobj.BEOUT;
        ptbefig->BEOUT->TIME = time;
    }
    else {
        if ((listpos = (beout_list *)gethtitem(outvecht, stem)) == (beout_list *)EMPTYHT) {
            newbeobj.BEOUT = beh_addbeout(ptbefig->BEOUT, name, expr, (pNode)ptbequad, 0);
            ptbefig->BEOUT = newbeobj.BEOUT;
            ptbefig->BEOUT->TIME = time;
            addhtitem(outvecht, stem, (long)ptbefig->BEOUT);
        }
        else {
            index = vectorindex(name);
            if (vectorindex(listpos->NAME) > index) {
                if (listpos == ptbefig->BEOUT) {
                    newbeobj.BEOUT = beh_addbeout(ptbefig->BEOUT, name, expr, (pNode)ptbequad, 0);
                    newbeobj.BEOUT->TIME = time;
                    ptbefig->BEOUT = newbeobj.BEOUT;
                }
                else {
                    for (prevpos = ptbefig->BEOUT; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    newbeobj.BEOUT = beh_addbeout(listpos, name, expr, (pNode)ptbequad, 0);
                    newbeobj.BEOUT->TIME = time;
                    prevpos->NEXT = newbeobj.BEOUT;
                }
                sethtitem(outvecht, stem, (long)newbeobj.BEOUT);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = prevpos->NEXT->NAME;
                    if (vectorindex(nextname) > index || vectorradical(nextname) != stem) break;
                }
                newbeobj.BEOUT = beh_addbeout(prevpos->NEXT, name, expr, (pNode)ptbequad, 0);
                newbeobj.BEOUT->TIME = time;
                prevpos->NEXT = newbeobj.BEOUT;
            }
        }
    }
    if (behdefinedht) yagBehSetDefine(name, YAG_BEOUT, newbeobj);
    return newbeobj.BEOUT;
}

void yagAddBerinVect(ptbefig, name)
    befig_list  *ptbefig;
    char        *name;
{
    berin_list  *listpos, *prevpos;
    berin_list  *newberin;
    char        *stem;
    char        *nextname;
    int         index;
    
    stem = vectorradical(name);
    if (stem == name) {
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
    }
    else {
        if ((listpos = (berin_list *)gethtitem(invecht, stem)) == (berin_list *)EMPTYHT) {
            ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
            addhtitem(invecht, stem, (long)ptbefig->BERIN);
        }
        else {
            index = vectorindex(name);
            if (vectorindex(listpos->NAME) > index) {
                if (listpos == ptbefig->BERIN) {
                    newberin = beh_addberin(ptbefig->BERIN, name);
                    ptbefig->BERIN = newberin;
                }
                else {
                    for (prevpos = ptbefig->BERIN; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    newberin = beh_addberin(listpos, name);
                    prevpos->NEXT = newberin;
                }
                sethtitem(invecht, stem, (long)newberin);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = prevpos->NEXT->NAME;
                    if (vectorindex(nextname) < index || vectorradical(nextname) != stem) break;
                }
                prevpos->NEXT = beh_addberin(prevpos->NEXT, name);
            }
        }
    }
}

beaux_list *
yagAddBeauxVect(ptbefig, name, expr, ptbequad, ptcone, time)
    befig_list  *ptbefig;
    char        *name;
    chain_list  *expr;
    bequad_list *ptbequad;
    cone_list   *ptcone;
    unsigned int time;
{
    beaux_list  *listpos, *prevpos;
    union beobj  newbeobj;
    char        *stem;
    char        *nextname;
    int         index;
    
    if (ptbequad == NULL && YAG_CONTEXT->YAG_BEFIG) {
        ptbequad = beh_addbequad(NULL, NULL, NULL, NULL);
        ptbequad->USER = addptype(ptbequad->USER, YAG_CONE_PTYPE, ptcone);
    }
    
    stem = vectorradical(name);
    if (stem == name) {
        newbeobj.BEAUX = beh_addbeaux(ptbefig->BEAUX, name, expr, (pNode)ptbequad, 0);
        ptbefig->BEAUX = newbeobj.BEAUX;
        ptbefig->BEAUX->TIME = time;
    }
    else {
        if ((listpos = (beaux_list *)gethtitem(auxvecht, stem)) == (beaux_list *)EMPTYHT) {
            newbeobj.BEAUX = beh_addbeaux(ptbefig->BEAUX, name, expr, (pNode)ptbequad, 0);
            ptbefig->BEAUX = newbeobj.BEAUX;
            ptbefig->BEAUX->TIME = time;
            addhtitem(auxvecht, stem, (long)ptbefig->BEAUX);
        }
        else {
            index = vectorindex(name);
            if (vectorindex(listpos->NAME) > index) {
                if (listpos == ptbefig->BEAUX) {
                    newbeobj.BEAUX = beh_addbeaux(ptbefig->BEAUX, name, expr, (pNode)ptbequad, 0);
                    newbeobj.BEAUX->TIME = time;
                    ptbefig->BEAUX = newbeobj.BEAUX;
                }
                else {
                    for (prevpos = ptbefig->BEAUX; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    newbeobj.BEAUX = beh_addbeaux(listpos, name, expr, (pNode)ptbequad, 0);
                    newbeobj.BEAUX->TIME = time;
                    prevpos->NEXT = newbeobj.BEAUX;
                }
                sethtitem(auxvecht, stem, (long)newbeobj.BEAUX);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = prevpos->NEXT->NAME;
                    if (vectorindex(nextname) > index || vectorradical(nextname) != stem) break;
                }
                newbeobj.BEAUX = beh_addbeaux(prevpos->NEXT, name, expr, (pNode)ptbequad, 0);
                prevpos->NEXT = newbeobj.BEAUX;
                prevpos->NEXT->TIME = time;
            }
        }
    }
    if (behdefinedht) yagBehSetDefine(name, YAG_BEAUX, newbeobj);
    return newbeobj.BEAUX;
}

bebus_list *
yagAddBebusVect(ptbefig, name, ptbiabl, ptbinode, ptcone)
    befig_list  *ptbefig;
    char        *name;
    biabl_list  *ptbiabl;
    binode_list *ptbinode;
    cone_list   *ptcone;
{
    bebus_list  *listpos, *prevpos;
    union beobj  newbeobj;
    char        *stem;
    char        *nextname;
    int         index;
    
    if (ptbinode == NULL) {
        ptbinode = yagMakeBinode(ptbiabl, ptcone);
    }
    
    stem = vectorradical(name);
    if (!YAG_CONTEXT->YAG_INTERFACE_VECTORS || stem == name) {
        newbeobj.BEBUS = beh_addbebus(ptbefig->BEBUS, name, ptbiabl, ptbinode, 'M',0);
        ptbefig->BEBUS = newbeobj.BEBUS;
    }
    else {
        if ((listpos = (bebus_list *)gethtitem(busvecht, stem)) == (bebus_list *)EMPTYHT) {
            newbeobj.BEBUS = beh_addbebus(ptbefig->BEBUS, name, ptbiabl, ptbinode, 'M',0);
            ptbefig->BEBUS = newbeobj.BEBUS;
            addhtitem(busvecht, stem, (long)ptbefig->BEBUS);
        }
        else {
            index = vectorindex(name);
            if (vectorindex(listpos->NAME) > index) {
                if (listpos == ptbefig->BEBUS) {
                    newbeobj.BEBUS = beh_addbebus(ptbefig->BEBUS, name, ptbiabl, ptbinode, 'M',0);
                    ptbefig->BEBUS = newbeobj.BEBUS;
                }
                else {
                    for (prevpos = ptbefig->BEBUS; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    newbeobj.BEBUS = beh_addbebus(listpos, name, ptbiabl, ptbinode, 'M',0);
                    prevpos->NEXT = newbeobj.BEBUS;
                }
                sethtitem(busvecht, stem, (long)newbeobj.BEBUS);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = prevpos->NEXT->NAME;
                    if (vectorindex(nextname) > index || vectorradical(nextname) != stem) break;
                }
                newbeobj.BEBUS = beh_addbebus(prevpos->NEXT, name, ptbiabl, ptbinode, 'M',0);
                prevpos->NEXT = newbeobj.BEBUS;
            }
        }
    }
    if (behdefinedht) yagBehSetDefine(name, YAG_BEAUX, newbeobj);
    return newbeobj.BEBUS;
}

bebux_list *
yagAddBebuxVect(ptbefig, name, ptbiabl, ptbinode, ptcone)
    befig_list  *ptbefig;
    char        *name;
    biabl_list  *ptbiabl;
    binode_list *ptbinode;
    cone_list   *ptcone;
{
    bebux_list  *listpos, *prevpos;
    union beobj  newbeobj;
    char        *stem;
    char        *nextname;
    int         index;
    
    if (ptbinode == NULL) {
        ptbinode = yagMakeBinode(ptbiabl, ptcone);
    }
    
    stem = vectorradical(name);
    if (stem == name) {
        newbeobj.BEBUX = beh_addbebux(ptbefig->BEBUX, name, ptbiabl, ptbinode, 'M',0);
        ptbefig->BEBUX = newbeobj.BEBUX;
    }
    else {
        if ((listpos = (bebux_list *)gethtitem(buxvecht, stem)) == (bebux_list *)EMPTYHT) {
            newbeobj.BEBUX = beh_addbebux(ptbefig->BEBUX, name, ptbiabl, ptbinode, 'M',0);
            ptbefig->BEBUX = newbeobj.BEBUX;
            addhtitem(buxvecht, stem, (long)ptbefig->BEBUX);
        }
        else {
            index = vectorindex(name);
            if (vectorindex(listpos->NAME) > index) {
                if (listpos == ptbefig->BEBUX) {
                    newbeobj.BEBUX = beh_addbebux(ptbefig->BEBUX, name, ptbiabl, ptbinode, 'M',0);
                    ptbefig->BEBUX = newbeobj.BEBUX;
                }
                else {
                    for (prevpos = ptbefig->BEBUX; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    newbeobj.BEBUX = beh_addbebux(listpos, name, ptbiabl, ptbinode, 'M',0);
                    prevpos->NEXT = newbeobj.BEBUX;
                }
                sethtitem(buxvecht, stem, (long)newbeobj.BEBUX);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = prevpos->NEXT->NAME;
                    if (vectorindex(nextname) > index || vectorradical(nextname) != stem) break;
                }
                newbeobj.BEBUX = beh_addbebux(prevpos->NEXT, name, ptbiabl, ptbinode, 'M',0);
                prevpos->NEXT = newbeobj.BEBUX;
            }
        }
    }
    if (behdefinedht) yagBehSetDefine(name, YAG_BEBUX, newbeobj);
    return newbeobj.BEBUX;
}

bereg_list *
yagAddBeregVect(ptbefig, name, ptbiabl, ptbinode, ptcone)
    befig_list  *ptbefig;
    char        *name;
    biabl_list  *ptbiabl;
    binode_list *ptbinode;
    cone_list   *ptcone;
{
    bereg_list  *listpos, *prevpos;
    union beobj  newbeobj;
    char        *stem;
    char        *nextname;
    int         index;
    
    if (ptbinode == NULL) {
        ptbinode = yagMakeBinode(ptbiabl, ptcone);
    }
    
    stem = vectorradical(name);
    if (stem == name) {
        newbeobj.BEREG = beh_addbereg(ptbefig->BEREG, name, ptbiabl, ptbinode,0);
        ptbefig->BEREG = newbeobj.BEREG;
    }
    else {
        if ((listpos = (bereg_list *)gethtitem(regvecht, stem)) == (bereg_list *)EMPTYHT) {
            newbeobj.BEREG = beh_addbereg(ptbefig->BEREG, name, ptbiabl, ptbinode,0);
            ptbefig->BEREG = newbeobj.BEREG;
            addhtitem(regvecht, stem, (long)ptbefig->BEREG);
        }
        else {
            index = vectorindex(name);
            if (vectorindex(listpos->NAME) > index) {
                if (listpos == ptbefig->BEREG) {
                    newbeobj.BEREG = beh_addbereg(ptbefig->BEREG, name, ptbiabl, ptbinode,0);
                    ptbefig->BEREG = newbeobj.BEREG;
                }
                else {
                    for (prevpos = ptbefig->BEREG; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    newbeobj.BEREG = beh_addbereg(listpos, name, ptbiabl, ptbinode,0);
                    prevpos->NEXT = newbeobj.BEREG;
                }
                sethtitem(regvecht, stem, (long)newbeobj.BEREG);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = prevpos->NEXT->NAME;
                    if (vectorindex(nextname) > index || vectorradical(nextname) != stem) break;
                }
                newbeobj.BEREG = beh_addbereg(prevpos->NEXT, name, ptbiabl, ptbinode,0);
                prevpos->NEXT = newbeobj.BEREG;
            }
        }
    }
    if (behdefinedht) yagBehSetDefine(name, YAG_BEREG, newbeobj);
    return newbeobj.BEREG;
}

int yagBehDefined(name, pttype, ptbeobj)
    char           *name;
    long           *pttype;
    union beobj    *ptbeobj;
{
    *pttype = (long)EMPTYHT;

    if (behdefinedht) {
        if ((ptbeobj->BEOUT = yagBeoutDefined(name)) != NULL) {
            *pttype = YAG_BEOUT;
            return TRUE;
        }
        else if ((ptbeobj->BEBUS = yagBebusDefined(name)) != NULL) {
            *pttype = YAG_BEBUS;
            return TRUE;
        }
        else if ((ptbeobj->BEBUX = yagBebuxDefined(name)) != NULL) {
            *pttype = YAG_BEBUX;
            return TRUE;
        }
        else if ((ptbeobj->BEAUX = yagBeauxDefined(name)) != NULL) {
            *pttype = YAG_BEAUX;
            return TRUE;
        }
        else if ((ptbeobj->BEREG = yagBeregDefined(name)) != NULL) {
            *pttype = YAG_BEREG;
            return TRUE;
        }
    }
    return FALSE;
}

void yagBehSetDefine(name, type, ubeobj)
    char           *name;
    long            type;
    union beobj     ubeobj;
    
{
    if (behdefinedht) {
        switch (type) {
        case YAG_BEPOR:
            sethtitem(beporht, name, (long)ubeobj.BEPOR);
            break;
        case YAG_BEAUX:
            sethtitem(beauxht, name, (long)ubeobj.BEAUX);
            break;
        case YAG_BEBUX:
            sethtitem(bebuxht, name, (long)ubeobj.BEBUX);
            break;
        case YAG_BEOUT:
            sethtitem(beoutht, name, (long)ubeobj.BEOUT);
            break;
        case YAG_BEBUS:
            sethtitem(bebusht, name, (long)ubeobj.BEBUS);
            break;
        case YAG_BEREG:
            sethtitem(bereght, name, (long)ubeobj.BEREG);
            break;
        }
    }
}

bepor_list *yagBeporDefined(name)
    char       *name;
{
    bepor_list *ptbepor;

    if (beporht) {
        if ((ptbepor = (bepor_list *)gethtitem(beporht, name)) != (bepor_list *)EMPTYHT) {
            return ptbepor;
        }
        else return NULL;
    }
    else return NULL;
}

beaux_list *yagBeauxDefined(name)
    char       *name;
{
    beaux_list *ptbeaux;

    if (beauxht) {
        if ((ptbeaux = (beaux_list *)gethtitem(beauxht, name)) != (beaux_list *)EMPTYHT) {
            return ptbeaux;
        }
        else return NULL;
    }
    else return NULL;
}

bebux_list *yagBebuxDefined(name)
    char       *name;
{
    bebux_list *ptbebux;

    if (bebuxht) {
        if ((ptbebux = (bebux_list *)gethtitem(bebuxht, name)) != (bebux_list *)EMPTYHT) {
            return ptbebux;
        }
        else return NULL;
    }
    else return NULL;
}

beout_list *yagBeoutDefined(name)
    char       *name;
{
    beout_list *ptbeout;

    if (beoutht) {
        if ((ptbeout = (beout_list *)gethtitem(beoutht, name)) != (beout_list *)EMPTYHT) {
            return ptbeout;
        }
        else return NULL;
    }
    else return NULL;
}

bebus_list *yagBebusDefined(name)
    char       *name;
{
    bebus_list *ptbebus;

    if (bebusht) {
        if ((ptbebus = (bebus_list *)gethtitem(bebusht, name)) != (bebus_list *)EMPTYHT) {
            return ptbebus;
        }
        else return NULL;
    }
    else return NULL;
}

bereg_list *yagBeregDefined(name)
    char       *name;
{
    bereg_list *ptbereg;

    if (bereght) {
        if ((ptbereg = (bereg_list *)gethtitem(bereght, name)) != (bereg_list *)EMPTYHT) {
            return ptbereg;
        }
        else return NULL;
    }
    else return NULL;
}

berin_list *yagGetBerin(ptbefig, name)
    befig_list *ptbefig;
    char       *name;
{
    berin_list *ptberin;

    for (ptberin = ptbefig->BERIN; ptberin; ptberin = ptberin->NEXT) {
        if (ptberin->NAME == name) break;
    }
    return ptberin;
}

beout_list *yagGetBeout(ptbefig, name)
    befig_list *ptbefig;
    char       *name;
{
    beout_list *ptbeout;

    for (ptbeout = ptbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        if (ptbeout->NAME == name) break;
    }
    return ptbeout;
}

beaux_list *yagGetBeaux(ptbefig, name)
    befig_list *ptbefig;
    char       *name;
{
    beaux_list *ptbeaux;

    for (ptbeaux = ptbefig->BEAUX; ptbeaux; ptbeaux = ptbeaux->NEXT) {
        if (ptbeaux->NAME == name) break;
    }
    return ptbeaux;
}

bebus_list *yagGetBebus(ptbefig, name)
    befig_list *ptbefig;
    char       *name;
{
    bebus_list *ptbebus;

    for (ptbebus = ptbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        if (ptbebus->NAME == name) break;
    }
    return ptbebus;
}

bebux_list *yagGetBebux(ptbefig, name)
    befig_list *ptbefig;
    char       *name;
{
    bebux_list *ptbebux;

    for (ptbebux = ptbefig->BEBUX; ptbebux; ptbebux = ptbebux->NEXT) {
        if (ptbebux->NAME == name) break;
    }
    return ptbebux;
}

bereg_list *yagGetBereg(ptbefig, name)
    befig_list *ptbefig;
    char       *name;
{
    bereg_list *ptbereg;

    for (ptbereg = ptbefig->BEREG; ptbereg; ptbereg = ptbereg->NEXT) {
        if (ptbereg->NAME == name) break;
    }
    return ptbereg;
}

beaux_list *yagGetBedly(ptbefig, name)
    befig_list *ptbefig;
    char       *name;
{
    beaux_list *ptbedly;

    for (ptbedly = ptbefig->BEDLY; ptbedly; ptbedly = ptbedly->NEXT) {
        if (ptbedly->NAME == name) break;
    }
    return ptbedly;
}

binode_list *yagMakeBinode(ptbiabl, ptcone)
    biabl_list *ptbiabl;
    cone_list  *ptcone;
{
    bequad_list *ptvalquad, *ptcndquad;
    biabl_list  *driver;
    binode_list *ptbinode = NULL;

    if (!YAG_CONTEXT->YAG_BEFIG) return NULL;

    for (driver = ptbiabl; driver; driver = driver->NEXT) {
        ptvalquad = beh_addbequad(NULL, NULL, NULL, NULL);
        ptvalquad->USER = addptype(ptvalquad->USER, YAG_CONE_PTYPE, ptcone);
        ptcndquad = beh_addbequad(NULL, NULL, NULL, NULL);
        ptbinode = beh_addbinode(ptbinode, (pNode)ptcndquad, (pNode)ptvalquad);
    }
    return ptbinode;
}

bequad_list *yagMakeBequadCone(ptcone)
    cone_list  *ptcone;
{
    bequad_list *ptbequad;
    
    if (!YAG_CONTEXT->YAG_BEFIG) return NULL;

    ptbequad = beh_addbequad(NULL, NULL, NULL, NULL);
    ptbequad->USER = addptype(ptbequad->USER, YAG_CONE_PTYPE, ptcone);

    return ptbequad;
}

bequad_list *yagMakeBequadConeList(conelist)
    chain_list  *conelist;
{
    bequad_list *ptbequad;
    
    if (!YAG_CONTEXT->YAG_BEFIG) {
       freechain(conelist);
       return NULL;
    }

    ptbequad = beh_addbequad(NULL, NULL, NULL, NULL);
    ptbequad->USER = addptype(ptbequad->USER, YAG_CONELIST_PTYPE, conelist);

    return ptbequad;
}

void yagCheckMissingDeclarations(befig_list *ptbefig)
{
  ht *doneht, *vect;
  berin_list *rin;
  bepor_list *por;
  bevectpor_list *vpor;
  bereg_list *bereg;
  bevectreg_list *bevectreg;
  bebus_list *bebus;
  bevectbus_list *bevectbus;
  beaux_list *beaux;
  bevectaux_list *bevectaux;
  bebux_list *bebux;
  bevectbux_list *bevectbux;
  beout_list *beout;
  bevectout_list *bevectout;
  char *rad;
  int idx;
  long l;

  doneht=addht(10000);

  for (por = ptbefig->BEPOR; por; por = por->NEXT)
    addhtitem(doneht, por->NAME, 0);

  for (vpor = ptbefig->BEVECTPOR; vpor; vpor = vpor->NEXT)
    addhtitem(doneht, vpor->NAME, 0);

  for (beaux = ptbefig->BEAUX; beaux; beaux = beaux->NEXT)
    addhtitem(doneht, beaux->NAME, 0);

  for (bevectaux = ptbefig->BEVECTAUX; bevectaux; bevectaux = bevectaux->NEXT)
    addhtitem(doneht, bevectaux->NAME, 0);

  for (bebus = ptbefig->BEBUS; bebus; bebus = bebus->NEXT)
    addhtitem(doneht, bebus->NAME, 0);

  for (bevectbus = ptbefig->BEVECTBUS; bevectbus; bevectbus = bevectbus->NEXT)
    addhtitem(doneht, bevectbus->NAME, 0);
  
  for (bebux = ptbefig->BEBUX; bebux; bebux = bebux->NEXT)
    addhtitem(doneht, bebux->NAME, 0);

  for (bevectbux = ptbefig->BEVECTBUX; bevectbux; bevectbux = bevectbux->NEXT)
    addhtitem(doneht, bevectbux->NAME, 0);

  for (bereg = ptbefig->BEREG; bereg; bereg = bereg->NEXT)
    addhtitem(doneht, bereg->NAME, 0);

  for (bevectreg = ptbefig->BEVECTREG; bevectreg; bevectreg = bevectreg->NEXT)
    addhtitem(doneht, bevectreg->NAME, 0);

  for (rin = ptbefig->BERIN; rin; rin = rin->NEXT)
  {
    rad=vectorradical(rin->NAME);
    idx=vectorindex(rin->NAME);
    if (((l=gethtitem(doneht, rad))==EMPTYHT && (l=gethtitem(doneht, rin->NAME))==EMPTYHT) || l!=0)
    {
      avt_errmsg(YAG_ERRMSG,"012",AVT_WARNING,rin->NAME);
      if (idx==-1) 
         ptbefig->BEAUX=beh_addbeaux(ptbefig->BEAUX, rin->NAME, createAtom("'u'"), NULL, 0);
      else
      {
         if (l==EMPTYHT)
         {
            ptbefig->BEVECTAUX=beh_addbevectaux(ptbefig->BEVECTAUX, rad, createAtom("'u'"), idx, idx, 0);
            addhtitem(doneht, rad, (long)ptbefig->BEVECTAUX);
         }
         else 
         {
            bevectaux=(bevectaux_list *)l;
            if (idx<bevectaux->LEFT) bevectaux->LEFT=idx;
            else if (idx>bevectaux->RIGHT) bevectaux->RIGHT=idx;
         }
      }
    }
  }

  delht(doneht);

}

