#include <signal.h>
#include <setjmp.h>

#include AVT_H
#include API_H
#include CNS_H

#define API_USE_REAL_TYPES
#include "cns_API.h"

static sigjmp_buf  cnsapi_JumpBuffer;
static void     (*OldExitHandler) () = NULL;

static void 
cnsapi_ExitHandler()
{
    MBK_EXIT_FUNCTION = OldExitHandler;
    siglongjmp(cnsapi_JumpBuffer, 1);
}

static void 
cnsapi_SetExitHandler()
{
    OldExitHandler = MBK_EXIT_FUNCTION;
    MBK_EXIT_FUNCTION = cnsapi_ExitHandler;
}

cnsfig_list *cns_LoadConeFigure(char *name)
{
    cnsfig_list    *cf = NULL;
    lofig_list     *lf = NULL;
    char           *n;

    cnsenv();
    if ((cf = getloadedcnsfig(name)) != NULL)
        avt_errmsg(CNS_API_ERRMSG, "001", AVT_WARNING, n);
    else {
        if ((lf = getloadedlofig(name)) != NULL) {
            avt_errmsg(CNS_API_ERRMSG, "002", AVT_WARNING, n);
        }
        else {
            cnsapi_SetExitHandler();
            if (sigsetjmp(cnsapi_JumpBuffer, 1) == 0) {
                cf = loadcnsfig(name, NULL);
            }
        }
    }
    
    if (cf == NULL)
        avt_errmsg(CNS_API_ERRMSG, "003", AVT_ERROR, n);
    return cf;
}

cnsfig_list *cns_GetConeFigure(char *name)
{
    return getloadedcnsfig(name);
}

void cns_SaveVerboseConeFile(cnsfig_list *cf)
{
    cnsenv();
    savecnvfig(cf);
}

void cns_DisplayPowerSupplies(FILE *f, cnsfig_list *cf)
{
    alim_list      *ptsupply;
    lotrs_list     *ptlotrs;
    cone_list      *ptcone;
    ptype_list     *ptuser;
    int             numsupply;

    if ((ptuser = getptype(cf->USER, CNS_POWER_SUPPLIES)) == NULL) return;
    fprintf(f, "Figure Supply List\n\n");
    numsupply = 1;
    for (ptsupply = (alim_list *)ptuser->DATA; ptsupply; ptsupply = ptsupply->NEXT) {
        fprintf(f, "%d)\n", numsupply++);
        fprintf(f, "\tVDDMAX: %.2f\n", ptsupply->VDDMAX);
        fprintf(f, "\tVDDMIN: %.2f\n", ptsupply->VDDMIN);
        fprintf(f, "\tVSSMAX: %.2f\n", ptsupply->VSSMAX);
        fprintf(f, "\tVSSMIN: %.2f\n", ptsupply->VSSMIN);
        fprintf(f, "\n");
    }
    numsupply = 1;
    fprintf(f, "Transistor Supplies\n\n");
    for (ptlotrs = cf->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        ptsupply = cns_get_lotrs_multivoltage(ptlotrs);
        fprintf(f, "%d) %s\n", numsupply++, ptlotrs->TRNAME);
        fprintf(f, "\tVDDMAX: %.2f\n", ptsupply->VDDMAX);
        fprintf(f, "\tVDDMIN: %.2f\n", ptsupply->VDDMIN);
        fprintf(f, "\tVSSMAX: %.2f\n", ptsupply->VSSMAX);
        fprintf(f, "\tVSSMIN: %.2f\n", ptsupply->VSSMIN);
        fprintf(f, "\n");
    }
    numsupply = 1;
    fprintf(f, "Cone Supplies\n\n");
    for (ptcone = cf->CONE; ptcone; ptcone = ptcone->NEXT) {
        if ((ptuser = getptype(ptcone->USER, CNS_SUPPLY)) == NULL) continue;
        ptsupply = (alim_list *)ptuser->DATA;
        fprintf(f, "%d) %s\n", numsupply++, ptcone->NAME);
        fprintf(f, "\tVDDMAX: %.2f\n", ptsupply->VDDMAX);
        fprintf(f, "\tVDDMIN: %.2f\n", ptsupply->VDDMIN);
        fprintf(f, "\tVSSMAX: %.2f\n", ptsupply->VSSMAX);
        fprintf(f, "\tVSSMIN: %.2f\n", ptsupply->VSSMIN);
        fprintf(f, "\n");
    }
}

