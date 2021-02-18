/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_constrain.c                                             */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include "fcl_headers.h"

void
fclConstrainConnectedInstances(losig_list *ptmodelsig, losig_list *ptsig)
{
    ht         *occurht_model, *occurht;
    loins_list *ptloins;
    locon_list *ptlocon;
    chain_list *modelchain = NULL, *modelchain_model = NULL;
    chain_list *ptchain;
    chain_list *inschain_model, *inschain;
    char       *figname;

    occurht = addht(40);
    occurht_model = addht(40);

    ptchain = (chain_list *)getptype(ptmodelsig->USER, LOFIGCHAIN)->DATA;
    for (;ptchain; ptchain = ptchain->NEXT) {
        ptlocon = (locon_list *)ptchain->DATA;
        if (ptlocon->TYPE != 'I') continue;
        ptloins = (loins_list *)ptlocon->ROOT;
        if ((inschain_model = (chain_list *)gethtitem(occurht_model, ptloins->FIGNAME)) != (void *)EMPTYHT) {
            sethtitem(occurht_model, ptloins->FIGNAME, (long)addchain(inschain_model, ptloins->INSNAME));
        }
        else {
            addhtitem(occurht_model, ptloins->FIGNAME, (long)addchain(NULL, ptloins->INSNAME));
            modelchain_model = addchain(modelchain_model, ptloins->FIGNAME);
        }
    }
    ptchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (;ptchain; ptchain = ptchain->NEXT) {
        ptlocon = (locon_list *)ptchain->DATA;
        if (ptlocon->TYPE != 'I') continue;
        ptloins = (loins_list *)ptlocon->ROOT;
        if ((inschain = (chain_list *)gethtitem(occurht, ptloins->FIGNAME)) != (void *)EMPTYHT) {
            sethtitem(occurht, ptloins->FIGNAME, (long)addchain(inschain, ptloins->INSNAME));
        }
        else {
            addhtitem(occurht, ptloins->FIGNAME, (long)addchain(NULL, ptloins->INSNAME));
            modelchain = addchain(modelchain, ptloins->FIGNAME);
        }
    }

    for (ptchain = modelchain_model; ptchain; ptchain = ptchain->NEXT) {
        figname = (char *)ptchain->DATA;
        inschain_model = (chain_list *)gethtitem(occurht_model, figname);
        if (inschain_model->NEXT == NULL) {
            inschain = (chain_list *)gethtitem(occurht, figname);
            if (inschain != (void *)EMPTYHT && inschain->NEXT == NULL) {
                sethtitem(FCL_REAL_CORRESP_HT, inschain_model->DATA, (long)inschain->DATA);
            }
        }
        freechain(inschain_model);
    }
    freechain(modelchain_model);
    delht(occurht_model);
    for (ptchain = modelchain; ptchain; ptchain = ptchain->NEXT) {
        figname = (char *)ptchain->DATA;
        inschain = (chain_list *)gethtitem(occurht, figname);
        freechain(inschain);
    }
    freechain(modelchain);
    delht(occurht);
}

