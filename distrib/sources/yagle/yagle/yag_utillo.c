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

static ht   *loconvecht;
static ht   *losigvecht;

void yagInitLoconVect()
{
    loconvecht = addht(100);
}

void  yagCloseLoconVect()
{
    delht(loconvecht);
}

void yagInitLosigVect()
{
    losigvecht = addht(100);
}

void  yagCloseLosigVect()
{
    delht(losigvecht);
}

locon_list *
yagAddLoconVect(ptlofig, name, ptlosig, dir, vec, pnode, user)
    lofig_list  *ptlofig;
    char        *name;
    losig_list  *ptlosig;
    char         dir;
    int          vec;
    num_list    *pnode;
    ptype_list  *user;
{
    locon_list  *listpos, *prevpos;
    locon_list  *newlocon;
    char        *stem;
    char        *nextname;
    int         index;
    
    stem = vectorradical(name);
    if (stem == name) {
        newlocon = addlocon(ptlofig, name, ptlosig, dir);
    }
    else {
        if ((listpos = (locon_list *)gethtitem(loconvecht, stem)) == (locon_list *)EMPTYHT) {
            newlocon = addlocon(ptlofig, name, ptlosig, dir);
            addhtitem(loconvecht, stem, (long)ptlofig->LOCON);
        }
        else {
            index = vectorindex(name);
            if ((vectorindex(listpos->NAME) < index && vec == YAG_VECDOWNTO)
            ||  (vectorindex(listpos->NAME) > index && vec == YAG_VECTO)) {
                if (listpos == ptlofig->LOCON) {
                    newlocon = addlocon(ptlofig, name, ptlosig, dir);
                }
                else {
                    for (prevpos = ptlofig->LOCON; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    newlocon = addlocon(ptlofig, name, ptlosig, dir);
                    ptlofig->LOCON = ptlofig->LOCON->NEXT;
                    prevpos->NEXT = newlocon;
                    newlocon->NEXT = listpos;
                }
                sethtitem(loconvecht, stem, (long)newlocon);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = prevpos->NEXT->NAME;
                    if ((vectorindex(nextname) < index && vec == YAG_VECDOWNTO)
                    ||  (vectorindex(nextname) > index && vec == YAG_VECTO)                
                    || vectorradical(nextname) != stem) break;
                }
                newlocon = addlocon(ptlofig, name, ptlosig, dir);
                ptlofig->LOCON = ptlofig->LOCON->NEXT;
                newlocon->NEXT = prevpos->NEXT;
                prevpos->NEXT = newlocon;
            }
        }
    }
    newlocon->PNODE = pnode;
    newlocon->USER = user;
    return newlocon;
}

void
yagAddGivenLoconVect(ptlofig, ptlocon, vec)
    lofig_list  *ptlofig;
    locon_list  *ptlocon;
    int         vec;
{
    locon_list  *listpos, *prevpos;
    char        *stem;
    char        *nextname;
    int         index;
    
    stem = vectorradical(ptlocon->NAME);
    if (stem == ptlocon->NAME) {
        ptlocon->NEXT = ptlofig->LOCON;
        ptlofig->LOCON = ptlocon;
    }
    else {
        if ((listpos = (locon_list *)gethtitem(loconvecht, stem)) == (locon_list *)EMPTYHT) {
            ptlocon->NEXT = ptlofig->LOCON;
            ptlofig->LOCON = ptlocon;
            addhtitem(loconvecht, stem, (long)ptlocon);
        }
        else {
            index = vectorindex(ptlocon->NAME);
            if ((vectorindex(listpos->NAME) < index && vec == YAG_VECDOWNTO)
            ||  (vectorindex(listpos->NAME) > index && vec == YAG_VECTO)) {
                if (listpos == ptlofig->LOCON) {
                    ptlocon->NEXT = ptlofig->LOCON;
                    ptlofig->LOCON = ptlocon;
                }
                else {
                    for (prevpos = ptlofig->LOCON; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    prevpos->NEXT = ptlocon;
                    ptlocon->NEXT = listpos;
                }
                sethtitem(loconvecht, stem, (long)ptlocon);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = prevpos->NEXT->NAME;
                    if ((vectorindex(nextname) < index && vec == YAG_VECDOWNTO)
                    ||  (vectorindex(nextname) > index && vec == YAG_VECTO)                
                    || vectorradical(nextname) != stem) break;
                }
                ptlocon->NEXT = prevpos->NEXT;
                prevpos->NEXT = ptlocon;
            }
        }
    }
}

losig_list *
yagAddLosigVect(ptlofig, sigindex, ptnamechain, type)
    lofig_list  *ptlofig;
    long         sigindex;
    chain_list  *ptnamechain;
    char         type;
{
    losig_list  *listpos, *prevpos;
    losig_list  *newlosig;
    char        *name;
    char        *stem;
    char        *nextname;
    int         index;
    
    newlosig = addlosig(ptlofig, sigindex, ptnamechain, type);
    name = getsigname(newlosig);
    stem = vectorradical(name);

    if (stem == name) {
        return newlosig;
    }
    else {
        if ((listpos = (losig_list *)gethtitem(losigvecht, stem)) == (losig_list *)EMPTYHT) {
            addhtitem(losigvecht, stem, (long)newlosig);
        }
        else {
            index = vectorindex(name);
            if (vectorindex(getsigname(listpos)) > index) {
                if (listpos != ptlofig->LOSIG->NEXT) {
                    for (prevpos = ptlofig->LOSIG; prevpos->NEXT != listpos; prevpos = prevpos->NEXT);
                    ptlofig->LOSIG = ptlofig->LOSIG->NEXT;
                    prevpos->NEXT = newlosig;
                    newlosig->NEXT = listpos;
                }
                sethtitem(losigvecht, stem, (long)newlosig);
            }
            else {
                for (prevpos = listpos; prevpos->NEXT != NULL; prevpos = prevpos->NEXT) {
                    nextname = getsigname(prevpos->NEXT);
                    if (vectorindex(nextname) > index || vectorradical(nextname) != stem) break;
                }
                ptlofig->LOSIG = ptlofig->LOSIG->NEXT;
                newlosig->NEXT = prevpos->NEXT;
                prevpos->NEXT = newlosig;
            }
        }
    }
    return newlosig;
}
