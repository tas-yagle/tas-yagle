/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_supply.c                                                */
/*                                                                          */
/*    (c) copyright 1993 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Marc LAURENTIN                        le : 27/01/1993     */
/*                                                                          */
/*    Modifie par : Anthony LESTER                      le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static long testInfoSupply __P((inffig_list *ifl, losig_list *ptsig));
//static char *deepest_name __P((char *name));
//static char *next_name_segment __P((char *name));

/****************************************************************************
*                      function yagFindSupplies();                          *
****************************************************************************/
void yagFindSupplies(ifl, ptlofig, silent)
    inffig_list *ifl;
    lofig_list *ptlofig;
    int         silent;
{
    locon_list    *connect= NULL;
    int            n_VDD = 0;
    int            n_VSS = 0;
    int            type  = 0;
    long           infsupply;
    float          value;
    char           buffer[256];

    /* external connector list traversal */
    for (connect = ptlofig->LOCON; connect; connect=connect->NEXT) {
        type = 0;
        if (connect->TYPE==CNS_I) continue; /* connecteur interne */
    
        infsupply = testInfoSupply(ifl, connect->SIG);
        if (mbk_LosigIsVDD(connect->SIG) || infsupply == CNS_VDD) {
            if (!silent) {
                buffer[0] = 0;
                if (getlosigalim(connect->SIG, &value))  sprintf(buffer, " (%.3fV)", value);
                yagWarning(WAR_SUPPLY_CONNECTOR, "VDD", buffer, connect->NAME, 0);
            }
            type |= CNS_VDD;
            connect->SIG->TYPE = CNS_SIGVDD;
            connect->DIRECTION = CNS_VDDC;
            n_VDD++;
        }
    
        if (mbk_LosigIsVSS(connect->SIG) || infsupply == CNS_VSS) {
            if (!silent) {
                buffer[0] = 0;
                if (getlosigalim(connect->SIG, &value))  sprintf(buffer, " (%.3fV)", value);
                yagWarning(WAR_SUPPLY_CONNECTOR, "VSS", buffer, connect->NAME, 0);
            }
            type |= CNS_VSS;
            connect->SIG->TYPE = CNS_SIGVSS; 
            connect->DIRECTION = CNS_VSSC;
            n_VSS++;
        }
    
        if (type == (CNS_VSS|CNS_VDD)) {
            avt_errmsg(YAG_ERRMSG, "003", AVT_FATAL, connect->NAME);
        } 
    }
}

/****************************************************************************
*                    function yagFindInternalSupplies();                    *
****************************************************************************/
void yagFindInternalSupplies(ifl,ptlofig, silent)
    inffig_list *ifl;
    lofig_list  *ptlofig;
    int         silent;
{
    chain_list *ptchain = NULL;
    chain_list *ptconlist;
    losig_list *ptsig = NULL;
    locon_list *ptcon = NULL;
    char        buffer[256];
    float       value;
    long        infsupply;
    int         n_VDD = 0;
    int         n_VSS = 0;
    int         n_con_VSS = 0;
    int         n_con_VDD = 0;
    int         type = 0;
    int         numshorts = 0;
    int         numunconnected = 0;
    int         global;

    for (ptsig = ptlofig->LOSIG; ptsig != NULL; ptsig = ptsig->NEXT) {
        /*------------------------------------------*
        | signals connected to external connectors  |
        | already detected by yagFindSupplies()     |
        *------------------------------------------*/
        if(ptsig->TYPE == CNS_SIGVDD) {
            n_VDD++;
            continue;
        }
    
        if (ptsig->TYPE == CNS_SIGVSS) {
            n_VSS++;
            continue;
        }
    
        type = 0;
        global = FALSE;

        infsupply = testInfoSupply(ifl, ptsig);
        if (((infsupply & CNS_VDD) & ~CNS_POWER) != 0 || mbk_LosigIsVDD(ptsig)) {
            if (!silent) {
                buffer[0] = 0;
                if (getlosigalim(ptsig, &value)) sprintf(buffer, " (%.3fV)", value);
                yagWarning(WAR_INTERNAL_SUPPLY, "VDD", buffer, (char *)ptsig, 0);
            }
            ptsig->TYPE = CNS_SIGVDD;
            type |= CNS_VDD;
            global = TRUE;
            n_VDD++;
        }
        if (((infsupply & CNS_VSS) & ~CNS_POWER) != 0 || mbk_LosigIsVSS(ptsig)) {
            if (!silent) {
                buffer[0] = 0;
                if (getlosigalim(ptsig, &value)) sprintf(buffer, " (%.3fV)", value);
                yagWarning(WAR_INTERNAL_SUPPLY, "VSS", buffer, (char *)ptsig, 0);
            }
            ptsig->TYPE = CNS_SIGVSS;
            type |= CNS_VSS;
            global = TRUE;
            n_VSS++;
        }
        /* Mark the external connectors */
        if (ptsig->TYPE == EXTERNAL && type == CNS_VDD) {
            ptconlist = yagGetExtLoconList(ptsig);
            for (ptchain = ptconlist; ptchain; ptchain = ptchain->NEXT) {
                ptcon = (locon_list *)ptchain->DATA;
                ptcon->DIRECTION = CNS_VDDC;
            }
        }
        if (ptsig->TYPE == EXTERNAL && type == CNS_VSS) {
            ptconlist = yagGetExtLoconList(ptsig);
            for (ptchain = ptconlist; ptchain; ptchain = ptchain->NEXT) {
                ptcon = (locon_list *)ptchain->DATA;
                ptcon->DIRECTION = CNS_VSSC;
            }
        }
        
        /* Warning if non-external signals would have been supplies if external */
        if (type != 0 && !global && !silent) {
            numunconnected++;
            yagWarning(WAR_SUPPLY_UNCONNECTED, NULL, NULL, ptsig->NAMECHAIN->DATA, ptsig->INDEX);
        }
        /* Error if signal is VDD and VSS */
        if (type == (CNS_VSS|CNS_VDD) && !silent) {
            numshorts++;
            yagError(ERR_SHORT_CIRCUIT, NULL, yagGetName(ifl, ptsig), NULL, 0, 0);
        }
    }

    if (n_VDD == 0 && !silent) {
        yagError(ERR_NO_VDD_SIG,NULL,NULL,NULL,0,0);
        avt_errmsg(YAG_ERRMSG,"004",AVT_ERROR);
    }
    
    if (n_VSS == 0 && !silent) {
        yagError(ERR_NO_VSS_SIG,NULL,NULL,NULL,0,0);
        avt_errmsg(YAG_ERRMSG,"005",AVT_ERROR);
    }
    
    if (YAG_CONTEXT->YAG_STAT_MODE && !silent) {
        if (numshorts > 0) {
            fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Shorted supplies               : %d \n" , numshorts );
        }
        if (numunconnected > 0) {
            fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Unconnected supplies           : %d \n" , numunconnected );
        }
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "VDD signal(s)                  : %d \n" , n_VDD );
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "VDD connector(s)               : %d \n" , n_con_VDD );
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "VSS signal(s)                  : %d \n" , n_VSS);
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "VSS connector(s)               : %d \n" , n_con_VSS );
    }
}

/****************************************************************************
*                       function yagTestTransparence();                     *
****************************************************************************/
/*---------------------------*
| mark the transparences     | 
*----------------------------*/
void yagTestTransparence(ptlofig)
    lofig_list  *ptlofig;
{
    locon_list  *ptcon = NULL;
    chain_list  *ptconchain = NULL;

    /* for each external connector */
    for (ptcon = ptlofig->LOCON; ptcon != NULL; ptcon = ptcon->NEXT) {
        if (ptcon->DIRECTION == CNS_VDDC) continue; 
        if (ptcon->DIRECTION == CNS_VSSC) continue; 
    
        /* for each connector to which it is connected */
        for (ptconchain = (chain_list *)getptype(ptcon->SIG->USER, LOFIGCHAIN)->DATA; ptconchain != NULL; ptconchain = ptconchain->NEXT ) {
            if ((((locon_list *)ptconchain->DATA )->TYPE != 'E')
            && (((locon_list *)ptconchain->DATA)->TYPE != 'X')) {
                /* it's not a transparence */
                break;
            }
        }
        if (ptconchain == NULL) {
            /* mark connector and signal */
            ptcon->TYPE = 'X';
            ptcon->DIRECTION = 'T';
            ptcon->SIG->TYPE = 'T';
        }
    }
}

static long
testInfoSupply(ifl, ptsig)
    inffig_list *ifl;
    losig_list  *ptsig;
{
//    infcond_list *ptinf;
    chain_list *cl, *maincl, *ptchain;
    char       *name;
    double      val;
    long        type = 0;
    ptype_list  *p;
    
    maincl=inf_GetEntriesByType(ifl, INF_POWER, INF_ANY_VALUES);

    for (ptchain = ptsig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {
        name = (char *)ptchain->DATA;
        for (cl = maincl; cl; cl=cl->NEXT) {
            if (mbk_TestREGEX(name, (char *)cl->DATA)) {
                if (inf_GetDouble(ifl, (char *)cl->DATA, INF_POWER, &val)) {
                    if ((long)(val*SCALE_ALIM + 0.5) > VDD_VSS_THRESHOLD) type |= CNS_VDD;
                    else type |= CNS_VSS;
                    addlosigalim(ptsig, val, NULL);
                    if ((p=getptype(ptsig->USER, MBK_SIGNALNAME_NODE))!=NULL)
                          mbk_addvcardnode(ptsig, name, val, (long)p->DATA);

                }
            }
        }
    }
    freechain(maincl);
    return type;
}
