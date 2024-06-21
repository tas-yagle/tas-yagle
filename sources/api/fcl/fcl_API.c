#include AVT_H
#include API_H
#include MUT_H
#include INF_H
#include YAG_H
#include INF_H
#include FCL_H
#include GEN_H

#define API_USE_REAL_TYPES
#include "gen_API.h"
#include "fcl_API.h"

static char *getwhere()
{
  static char where[1024];
  sprintf(where,"%s: ",gen_info());
  return where;
}

int
fclMarkCorrespondingSignal(char *signame, char *marks)
{
    ptype_list *ptuser;
    losig_list *signal;
    char       *copymarks;
    char       *typeconst;
    char       *pttok;
    void       *ptr;
    long        type = 0;
    int         i;
    inffig_list *ifl;
    
    if ((ifl=getloadedinffig(gns_GetWorkingFigureName()))==NULL)
      ifl=addinffig(gns_GetWorkingFigureName());

    signal = gns_GetCorrespondingSignal(signame);
    if (signal == NULL) return 0;
    copymarks = mbkstrdup(marks);
    ptr = copymarks;
    while ((pttok = strtok(ptr, "+")) != NULL) {
        ptr = NULL;
        typeconst = min_namealloc(pttok);
        for (i=0; i<NUMNETTYPES; i++) {
            if (nettype_names[i] == typeconst) type |= nettype_values[i];
        }
        if (type == 0) {
            avt_errmsg(FCL_API_ERRMSG, "001", AVT_ERROR, pttok);
            //printf("fclMarkSignal: Illegal markings\n");
            return 0;
        }
    }

    if ((type & FCL_VSS) == FCL_VSS) signal->TYPE = CNS_SIGVSS;
    if ((type & FCL_VDD) == FCL_VDD) signal->TYPE = CNS_SIGVDD;
    if ((type & FCL_BYPASS) == FCL_BYPASS) 
    {
      inf_AddString(ifl, INF_LOADED_LOCATION, signal->NAMECHAIN->DATA, INF_BYPASS, INF_ALL, getwhere());
      //infAddList (ifl, INF_LOADED_LOCATION, signal->NAMECHAIN->DATA, INF_BYPASS, 'd',NULL, getwhere());
    }
    if ((ptuser = getptype(signal->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        ptuser->DATA = (void *)((long)ptuser->DATA | type);
    }
    else signal->USER = addptype(signal->USER, FCL_TRANSFER_PTYPE, (void *)type);
    return 1;
}

int
fclMarkCorrespondingTransistor(char *transname, char *marks)
{
    ptype_list *ptuser;
    lotrs_list *transistor;
    char       *copymarks;
    char       *typeconst;
    char       *pttok;
    void       *ptr;
    long        type = 0;
    int         i;

    transistor = gns_GetCorrespondingTransistor(transname);
    if (transistor == NULL) {
        avt_errmsg(FCL_API_ERRMSG, "002", AVT_ERROR, transname);
        //printf("fclMarkTransistor: transistor '%s' does not exist\n", transname);
        return 0;
    }
    copymarks = mbkstrdup(marks);
    ptr = copymarks;
    while ((pttok = strtok(ptr, "+")) != NULL) {
        ptr = NULL;
        typeconst = min_namealloc(pttok);
        for (i=0; i<NUMTRANSTYPES; i++) {
            if (transtype_names[i] == typeconst) type |= transtype_values[i];
        }
        if (type == 0) {
            avt_errmsg(FCL_API_ERRMSG, "001", AVT_ERROR, pttok);
//            printf("fclMarkTransistor: Illegal markings\n");
            return 0;
        }
    }
    if ((ptuser = getptype(transistor->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        ptuser->DATA = (void *)((long)ptuser->DATA | type);
    }
    else transistor->USER = addptype(transistor->USER, FCL_TRANSFER_PTYPE, (void *)type);
    return 1;
}

void
fclOrientCorrespondingSignal(char *signame, int level)
{
    ptype_list   *ptuser;
    losig_list   *signal;
    int  oldlevel, newlevel;

    signal = gns_GetCorrespondingSignal(signame);
    if (signal == NULL) return;
    ptuser = getptype(signal->USER, YAG_VAL_S_PTYPE);
    if (ptuser != NULL) {
        oldlevel = (unsigned int)((long)ptuser->DATA);
        if (level > oldlevel) {
            newlevel = level;
            avt_errmsg(FCL_API_ERRMSG, "003", AVT_ERROR, signame, newlevel);
//            printf("fclOrientSignal: multiple orientation on '%s', resolved to %u\n", signame, newlevel);
        }
        else newlevel = oldlevel;
        ptuser->DATA = (void *)((long)newlevel);
    }
    else {
        signal->USER = addptype(signal->USER, YAG_VAL_S_PTYPE, (void *)((long)level));
    }
}

void
fclCmpUpConstraint(chain_list *siglist)
{
    chain_list *ptchain;
    chain_list *newchain = NULL;
    losig_list *ptlosig;
    inffig_list *ifl;

    if ((ifl=getloadedinffig(gns_GetWorkingFigureName()))==NULL)
      ifl=addinffig(gns_GetWorkingFigureName());
    
    if (siglist == NULL) return;
    for (ptchain = siglist; ptchain; ptchain = ptchain->NEXT) {
        ptlosig = (losig_list *)ptchain->DATA;
        newchain = addchain(newchain, ptlosig->NAMECHAIN->DATA);
    }
    inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_CMPU, "", newchain, getwhere());
//    ifl->LOADED.INF_CMPU = addchain(ifl->LOADED.INF_CMPU, newchain);
}

void
fclCmpDnConstraint(chain_list *siglist)
{
    chain_list *ptchain;
    chain_list *newchain = NULL;
    losig_list *ptlosig;
    inffig_list *ifl;

    if ((ifl=getloadedinffig(gns_GetWorkingFigureName()))==NULL)
      ifl=addinffig(gns_GetWorkingFigureName());

    if (siglist == NULL) return;
    for (ptchain = siglist; ptchain; ptchain = ptchain->NEXT) {
        ptlosig = (losig_list *)ptchain->DATA;
        newchain = addchain(newchain, ptlosig->NAMECHAIN->DATA);
    }
    inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_CMPD, "", newchain, getwhere());
//    ifl->LOADED.INF_CMPD = addchain(ifl->LOADED.INF_CMPD, newchain);
}

void
fclMuxUpConstraint(chain_list *siglist)
{
    chain_list *ptchain;
    chain_list *newchain = NULL;
    losig_list *ptlosig;
    inffig_list *ifl;

    if ((ifl=getloadedinffig(gns_GetWorkingFigureName()))==NULL)
      ifl=addinffig(gns_GetWorkingFigureName());

    if (siglist == NULL) return;
    for (ptchain = siglist; ptchain; ptchain = ptchain->NEXT) {
        ptlosig = (losig_list *)ptchain->DATA;
        newchain = addchain(newchain, ptlosig->NAMECHAIN->DATA);
    }
    inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_MUXU, "", newchain, getwhere());
  //  ifl->LOADED.INF_MUXU = addchain(ifl->LOADED.INF_MUXU, newchain);
}

void
fclMuxDnConstraint(chain_list *siglist)
{
    chain_list *ptchain;
    chain_list *newchain = NULL;
    losig_list *ptlosig;
    inffig_list *ifl;

    if ((ifl=getloadedinffig(gns_GetWorkingFigureName()))==NULL)
      ifl=addinffig(gns_GetWorkingFigureName());

    if (siglist == NULL) return;
    for (ptchain = siglist; ptchain; ptchain = ptchain->NEXT) {
        ptlosig = (losig_list *)ptchain->DATA;
        newchain = addchain(newchain, ptlosig->NAMECHAIN->DATA);
    }
    inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_MUXD, "", newchain, getwhere());
//    ifl->LOADED.INF_MUXD = addchain(ifl->LOADED.INF_MUXD, newchain);
}

void
fclAllowShare(lotrs_list *transistor)
{
    ptype_list *ptuser;

    ptuser = getptype(transistor->USER, FCL_MARK_PTYPE);
    if (ptuser != NULL) {
        ptuser->DATA = (void *)FCL_SHARE_TRANS;
    }
    else transistor->USER = addptype(transistor->USER, FCL_MARK_PTYPE, (void *)FCL_SHARE_TRANS);
}

