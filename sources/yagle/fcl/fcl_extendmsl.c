/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_extendmsl.c                                             */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include "fcl_headers.h"

fcltype_list *FCL_TYPEHEAD = NULL;

char *nettype_names[NUMNETTYPES];
long  nettype_values[NUMNETTYPES];
char *transtype_names[NUMTRANSTYPES];
long  transtype_values[NUMTRANSTYPES];

int mslFclExtension();
int mslFclCompletion();

static int typetrans();
static int typelosig();

static fcltype_list *addfcltype();
static void freefcltype();

static char *fclVectName();

void
fclExtendSpice()
{
    nettype_names[0] = min_namealloc("NET_LATCH");
    nettype_names[1] = min_namealloc("NET_MEMSYM");
    nettype_names[2] = min_namealloc("NET_RS");
    nettype_names[3] = min_namealloc("NET_FLIPFLOP");
    nettype_names[4] = min_namealloc("NET_MASTER");
    nettype_names[5] = min_namealloc("NET_SLAVE");
    nettype_names[6] = min_namealloc("NET_BLOCKER");
    nettype_names[7] = min_namealloc("NET_STOP");
    nettype_names[8] = min_namealloc("NET_NO_FALSE_BRANCH");
    nettype_names[9] = min_namealloc("NET_UNUSED");
    nettype_names[10] = min_namealloc("NET_VDD");
    nettype_names[11] = min_namealloc("NET_VSS");
    nettype_names[12] = min_namealloc("NET_MATCHNAME");
    nettype_names[13] = min_namealloc("NET_SENSITIVE");
    nettype_names[14] = min_namealloc("NET_NOTLATCH");
    nettype_names[15] = min_namealloc("NET_BYPASS");
    transtype_names[0] = min_namealloc("TRANS_BLEEDER");
    transtype_names[1] = min_namealloc("TRANS_FEEDBACK");
    transtype_names[2] = min_namealloc("TRANS_COMMAND");
    transtype_names[3] = min_namealloc("TRANS_NOT_FUNCTIONAL");
    transtype_names[4] = min_namealloc("TRANS_BLOCKER");
    transtype_names[5] = min_namealloc("TRANS_UNUSED");
    transtype_names[6] = min_namealloc("TRANS_NOSHARE");
    transtype_names[7] = min_namealloc("TRANS_MATCHSIZE");
    transtype_names[8] = min_namealloc("TRANS_SHARE");
    transtype_names[9] = min_namealloc("TRANS_SHORT");

    nettype_values[0] = FCL_LATCH;
    nettype_values[1] = FCL_MEMSYM;
    nettype_values[2] = FCL_RS;
    nettype_values[3] = FCL_FLIPFLOP;
    nettype_values[4] = FCL_MASTER;
    nettype_values[5] = FCL_SLAVE;
    nettype_values[6] = FCL_BLOCKER;
    nettype_values[7] = FCL_STOP;
    nettype_values[8] = FCL_NOFALSEBRANCH;
    nettype_values[9] = FCL_NEVER;
    nettype_values[10] = FCL_VDD;
    nettype_values[11] = FCL_VSS;
    nettype_values[12] = FCL_MATCHNAME;
    nettype_values[13] = FCL_SENSITIVE;
    nettype_values[14] = FCL_NOTLATCH;
    nettype_values[15] = FCL_BYPASS;
    transtype_values[0] = FCL_BLEEDER;
    transtype_values[1] = FCL_FEEDBACK;
    transtype_values[2] = FCL_COMMAND;
    transtype_values[3] = FCL_NOT_FUNCTIONAL;
    transtype_values[4] = FCL_BLOCKER;
    transtype_values[5] = FCL_NEVER;
    transtype_values[6] = FCL_NOSHARE;
    transtype_values[7] = FCL_MATCHSIZE;
    transtype_values[8] = FCL_SHARE_TRANS;
    transtype_values[9] = FCL_SHORT;

    mslAddExtension(mslFclExtension);
    mslAddCompletion(mslFclCompletion);
}

void
fclRmvSpiceExtensions()
{
    mslRmvExtension(mslFclExtension);
    mslRmvCompletion(mslFclCompletion);
}

int
mslFclExtension(refline, process)
    char *refline;
    int   process;
{
    char *token;
    char *typeexpr;
    char *name;
    char *line;
    int   level;
    int   handled;

    chain_list *namechain = NULL;
    line = (char *)mbkalloc(strlen(refline) + 1);
    strcpy(line, refline);

    if (msl_subckt == NULL && process) { mbkfree(line); return 0; }
    line = strtok(line, "\n");
    token = strtok(line, " ");
    if (token == NULL) { mbkfree(line); return 0; }
    if (*token != '*') { mbkfree(line); return 0; }
    token = strtok(NULL, " ");
    if (token != NULL) {
        if (strcasecmp(token, "transtype") == 0) {
            if (!process) { mbkfree(line); return 1; }
            name = strtok(NULL, " ");
            if (name == NULL) { mbkfree(line); return 1; }
            typeexpr = strtok(NULL, " ");
            if (typeexpr == NULL) { mbkfree(line); return 1; }
            if (strtok(NULL, " ") != NULL) { mbkfree(line); return 1; }
            handled = typetrans(namealloc(name), typeexpr);
            mbkfree(line);
            return handled;
        }
        else if (strcasecmp(token, "nettype") == 0) {
            if (!process) { mbkfree(line); return 1; }
            name = strtok(NULL, " ");
            if (name == NULL) { mbkfree(line); return 1; }
            typeexpr = strtok(NULL, " ");
            if (typeexpr == NULL) { mbkfree(line); return 1; }
            if (strtok(NULL, " ") != NULL) { mbkfree(line); return 1; }
            handled = typelosig(namealloc(name), typeexpr);
            mbkfree(line);
            return handled;
        }
        else if (strcasecmp(token, "cmpup") == 0) {
            if (!process) { mbkfree(line); return 1; }
            while ((name = strtok(NULL, " ,;\n")) != NULL) {
                namechain = addchain(namechain, namealloc(fclVectName(name)));
            }
            FCL_TYPEHEAD = addfcltype(FCL_TYPEHEAD, msl_subckt, NULL, FCL_CMPUP, (long)namechain);
            mbkfree(line);
            return 1;
        }
        else if (strcasecmp(token, "cmpdn") == 0) {
            if (!process) { mbkfree(line); return 1; }
            while ((name = strtok(NULL, " ,;\n")) != NULL) {
                namechain = addchain(namechain, namealloc(fclVectName(name)));
            }
            FCL_TYPEHEAD = addfcltype(FCL_TYPEHEAD, msl_subckt, NULL, FCL_CMPDN, (long)namechain);
            mbkfree(line);
            return 1;
        }
        else if (strcasecmp(token, "muxup") == 0) {
            if (!process) { mbkfree(line); return 1; }
            while ((name = strtok(NULL, " ,;\n")) != NULL) {
                namechain = addchain(namechain, namealloc(fclVectName(name)));
            }
            FCL_TYPEHEAD = addfcltype(FCL_TYPEHEAD, msl_subckt, NULL, FCL_MUXUP, (long)namechain);
            mbkfree(line);
            return 1;
        }
        else if (strcasecmp(token, "muxdn") == 0) {
            if (!process) { mbkfree(line); return 1; }
            while ((name = strtok(NULL, " ,;\n")) != NULL) {
                namechain = addchain(namechain, namealloc(fclVectName(name)));
            }
            FCL_TYPEHEAD = addfcltype(FCL_TYPEHEAD, msl_subckt, NULL, FCL_MUXDN, (long)namechain);
            mbkfree(line);
            return 1;
        }
        else if (strcasecmp(token, "netoutput") == 0) {
            if (!process) { mbkfree(line); return 1; }
            name = strtok(NULL, " ");
            if (name == NULL) { mbkfree(line); return 1; }
            typeexpr = strtok(NULL, " ");
            if (typeexpr == NULL) level = 1;
            else level = atoi(typeexpr);
            FCL_TYPEHEAD = addfcltype(FCL_TYPEHEAD, msl_subckt, namealloc(fclVectName(name)), FCL_NETOUTPUT, (long)level);
            mbkfree(line);
            return 1;
        }
    }
    mbkfree(line);
    return 0;
}

static int
typetrans(name, typeexpr)
    char   *name;
    char   *typeexpr;
{
    char       *typeconst;
    char       *pttok;
    void       *ptr;
    long        type = 0;
    int         i;
   
    ptr = typeexpr;
    while ((pttok = strtok(ptr, "|")) != NULL) {
        ptr = NULL;
        typeconst = min_namealloc(pttok);
        for (i=0; i<NUMTRANSTYPES; i++) {
            if (transtype_names[i] == typeconst) type |= transtype_values[i];
        }
        if (type == 0) {
            printf("FCL: Illegal transtype at line %d\n", msl_line);
            return 0;
        }
    }

    FCL_TYPEHEAD = addfcltype(FCL_TYPEHEAD, msl_subckt, name, FCL_TRANSTYPE, type);

    return 1;
}

static int
typelosig(name, typeexpr)
    char   *name;
    char   *typeexpr;
{
    char       *typeconst;
    char       *pttok;
    void       *ptr;
    long        type = 0;
    int         i;
   
    ptr = typeexpr;
    while ((pttok = strtok(ptr, "|")) != NULL) {
        ptr = NULL;
        typeconst = min_namealloc(pttok);
        for (i=0; i<NUMNETTYPES; i++) {
            if (nettype_names[i] == typeconst) type |= nettype_values[i];
        }
        if (type == 0) {
            printf("FCL: Illegal nettype at line %d\n", msl_line);
            return 0;
        }
    }

    FCL_TYPEHEAD = addfcltype(FCL_TYPEHEAD, msl_subckt, name, FCL_NETTYPE, type);
   
    return 1;
}

int
mslFclCompletion()
{
    lofig_list     *ptlofig = NULL;
    char           *curfigname = NULL;
    fcltype_list   *ptfcldata;
    lotrs_list     *pttrans;
    losig_list     *ptlosig;
    ptype_list     *ptuser;

    for (ptfcldata = FCL_TYPEHEAD; ptfcldata; ptfcldata = ptfcldata->NEXT) {
        if (ptfcldata->SUBCKT != curfigname) {
            ptlofig = getlofig(ptfcldata->SUBCKT, 'A');
            if (ptlofig != NULL) curfigname = ptlofig->NAME;
            else {
                printf("FCL: type not in any SUBCKT\n");
                continue;
            }
        }
        if (ptfcldata->MARKTYPE == FCL_TRANSTYPE) {
            for (pttrans = ptlofig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
                if (pttrans->TRNAME == ptfcldata->NAME) break;
            }
            if (pttrans != NULL) {
                if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    ptuser->DATA = (void *)((long)ptuser->DATA | ptfcldata->TYPE);
                }
                else pttrans->USER = addptype(pttrans->USER, FCL_TRANSFER_PTYPE, (void *)ptfcldata->TYPE);
            }
            else {
                printf("FCL: transistor '%s' does not exist\n", ptfcldata->NAME);
            }
        }
        else if (ptfcldata->MARKTYPE == FCL_NETTYPE) {
            for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
                if (getsigname(ptlosig) == ptfcldata->NAME) break;
            }
            if (ptlosig != NULL) {
                if ((ptuser = getptype(ptlosig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    ptuser->DATA = (void *)((long)ptuser->DATA | ptfcldata->TYPE);
                }
                else ptlosig->USER = addptype(ptlosig->USER, FCL_TRANSFER_PTYPE, (void *)ptfcldata->TYPE);
            }
            else {
                printf("FCL: signal '%s' does not exist\n", ptfcldata->NAME);
            }
        }
        else if (ptfcldata->MARKTYPE == FCL_CMPUP) {
            if ((ptuser = getptype(ptlofig->USER, FCL_CMPUP_PTYPE)) != NULL) {
                ptuser->DATA = addchain(ptuser->DATA, (chain_list *)ptfcldata->TYPE);
            }
            else ptlofig->USER = addptype(ptlofig->USER, FCL_CMPUP_PTYPE, addchain(NULL, (chain_list *)ptfcldata->TYPE));
        }
        else if (ptfcldata->MARKTYPE == FCL_CMPDN) {
            if ((ptuser = getptype(ptlofig->USER, FCL_CMPDN_PTYPE)) != NULL) {
                ptuser->DATA = addchain(ptuser->DATA, (chain_list *)ptfcldata->TYPE);
            }
            else ptlofig->USER = addptype(ptlofig->USER, FCL_CMPDN_PTYPE, addchain(NULL, (chain_list *)ptfcldata->TYPE));
        }
        else if (ptfcldata->MARKTYPE == FCL_MUXUP) {
            if ((ptuser = getptype(ptlofig->USER, FCL_MUXUP_PTYPE)) != NULL) {
                ptuser->DATA = addchain(ptuser->DATA, (chain_list *)ptfcldata->TYPE);
            }
            else ptlofig->USER = addptype(ptlofig->USER, FCL_MUXUP_PTYPE, addchain(NULL, (chain_list *)ptfcldata->TYPE));
        }
        else if (ptfcldata->MARKTYPE == FCL_MUXDN) {
            if ((ptuser = getptype(ptlofig->USER, FCL_MUXDN_PTYPE)) != NULL) {
                ptuser->DATA = addchain(ptuser->DATA, (chain_list *)ptfcldata->TYPE);
            }
            else ptlofig->USER = addptype(ptlofig->USER, FCL_MUXDN_PTYPE, addchain(NULL, (chain_list *)ptfcldata->TYPE));
        }
        else if (ptfcldata->MARKTYPE == FCL_NETOUTPUT) {
            if ((ptuser = getptype(ptlofig->USER, FCL_NETOUTPUT_PTYPE)) != NULL) {
                ptuser->DATA = addptype(ptuser->DATA, ptfcldata->TYPE, ptfcldata->NAME);
            }
            else ptlofig->USER = addptype(ptlofig->USER, FCL_NETOUTPUT_PTYPE, addptype(NULL, ptfcldata->TYPE, ptfcldata->NAME));
        }
    }

    freefcltype(FCL_TYPEHEAD);
    FCL_TYPEHEAD = NULL;
    return 1;
}

static fcltype_list *
addfcltype(pthead, subckt, name, marktype, type)
    fcltype_list   *pthead;
    char           *subckt;
    char           *name;
    int             marktype;
    long            type;
{
    fcltype_list   *ptnew;

    ptnew = (fcltype_list *)mbkalloc(sizeof(fcltype_list));
    ptnew->NEXT = pthead;
    ptnew->SUBCKT = namealloc(subckt);
    ptnew->NAME = name;
    ptnew->MARKTYPE = marktype;
    ptnew->TYPE = type;

    return ptnew;
}

static void
freefcltype(pthead)
    fcltype_list   *pthead;
{
    fcltype_list   *ptfcldata;
    fcltype_list   *ptnextdata;

    for (ptfcldata = pthead; ptfcldata; ptfcldata = ptnextdata) {
        ptnextdata = ptfcldata->NEXT;
        mbkfree(ptfcldata);
    }
}

/***********************************************************************
 *                         fonction fclVectName();                     *
 **********************************************************************/
 /*--------------------------------------------------------------------+
 | Remplace un nom contenant des crochet par son equivalent valide MBK |
 | ex 'toto[0]' => 'toto 0'                                            |
 +--------------------------------------------------------------------*/
static char *
fclVectName(name)
    char *name;
{
    char buff[1024];
    char *res ;
    char *car=name ;
    int i = 0 ;
 
    while(*car != '\0') {
        if(i>190) { 
            fprintf(stderr,"[FATAL ERR] Executing fclVectName : buff error\n");
            fflush(stderr);
            EXIT(-1);
        }
 
        if (*car != '[' && *car != '(') {
            buff[i++] = *car++ ;
        }
        else {   
            buff[i++]=' ';
            car++;
            while( ((*car>='0')&&(*car<='9')) || (*car=='*')) {
                buff[i++] = *car++;
            }
            break ;
        }
    }   
 
    buff[i]='\0';
    res=(char*)mbkalloc(strlen(buff)+1);
    strcpy(res,buff);
    return(res);
}


