/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_util.c                                                */
/*                                                                            */
/*                                                                            */
/*    (c) copyright 2001 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Marc KUOCH                                                  */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#include MCC_H
#include "mcc_util.h"
#include "mcc_mod_util.h"
#include "mcc_mod_bsim3v3.h"
#include "mcc_mod_mos2.h"

/******************************************************************************/
/* GLOBALS                                                                    */
/******************************************************************************/

mcc_technolist *MCC_HEADTECHNO     = NULL ;

/******************************************************************************/
/* EXTERNS                                                                    */
/******************************************************************************/

extern void yyinit(char *) ;
eqt_ctx *mccEqtCtx;
eqt_ctx *mccMCPARAMctx;
static eqt_ctx *MCC_MC_NOM_VALUES=NULL;

/******************************************************************************/
/* FONCTIONS                                                                  */
/******************************************************************************/

static void mcc_eqt_init();

static struct 
{
  char *name;
  int index;
} mcc_translate_tab[]=
  {
{"k1", __MCC_QUICK_K1 }, {"lk1", __MCC_QUICK_LK1 }, {"wk1", __MCC_QUICK_WK1 }, {"pk1", __MCC_QUICK_PK1 },
{"k2", __MCC_QUICK_K2 }, {"lk2", __MCC_QUICK_LK2 }, {"wk2", __MCC_QUICK_WK2 }, {"pk2", __MCC_QUICK_PK2 },
{"k3", __MCC_QUICK_K3 }, {"lk3", __MCC_QUICK_LK3 }, {"wk3", __MCC_QUICK_WK3 }, {"pk3", __MCC_QUICK_PK3 },
{"k3b", __MCC_QUICK_K3B }, {"lk3b", __MCC_QUICK_LK3B }, {"wk3b", __MCC_QUICK_WK3B }, {"pk3b", __MCC_QUICK_PK3B },
{"w0", __MCC_QUICK_W0 }, {"lw0", __MCC_QUICK_LW0 }, {"ww0", __MCC_QUICK_WW0 }, {"pw0", __MCC_QUICK_PW0 },
{"nlx", __MCC_QUICK_NLX }, {"lnlx", __MCC_QUICK_LNLX }, {"wnlx", __MCC_QUICK_WNLX }, {"pnlx", __MCC_QUICK_PNLX },
{"dvt0", __MCC_QUICK_DVT0 }, {"ldvt0", __MCC_QUICK_LDVT0 }, {"wdvt0", __MCC_QUICK_WDVT0 }, {"pdvt0", __MCC_QUICK_PDVT0 },
{"dvt1", __MCC_QUICK_DVT1 }, {"ldvt1", __MCC_QUICK_LDVT1 }, {"wdvt1", __MCC_QUICK_WDVT1 }, {"pdvt1", __MCC_QUICK_PDVT1 },
{"dvt2", __MCC_QUICK_DVT2 }, {"ldvt2", __MCC_QUICK_LDVT2 }, {"wdvt2", __MCC_QUICK_WDVT2 }, {"pdvt2", __MCC_QUICK_PDVT2 },
{"dvt0w", __MCC_QUICK_DVT0W }, {"ldvt0w", __MCC_QUICK_LDVT0W }, {"wdvt0w", __MCC_QUICK_WDVT0W }, {"pdvt0w", __MCC_QUICK_PDVT0W },
{"dvt1w", __MCC_QUICK_DVT1W }, {"ldvt1w", __MCC_QUICK_LDVT1W }, {"wdvt1w", __MCC_QUICK_WDVT1W }, {"pdvt1w", __MCC_QUICK_PDVT1W },
{"dvt2w", __MCC_QUICK_DVT2W }, {"ldvt2w", __MCC_QUICK_LDVT2W }, {"wdvt2w", __MCC_QUICK_WDVT2W }, {"pdvt2w", __MCC_QUICK_PDVT2W },
{"dsub", __MCC_QUICK_DSUB }, {"ldsub", __MCC_QUICK_LDSUB }, {"wdsub", __MCC_QUICK_WDSUB }, {"pdsub", __MCC_QUICK_PDSUB },
{"nch", __MCC_QUICK_NCH }, {"lnch", __MCC_QUICK_LNCH }, {"wnch", __MCC_QUICK_WNCH }, {"pnch", __MCC_QUICK_PNCH },
{"npeak", __MCC_QUICK_NPEAK }, {"lnpeak", __MCC_QUICK_LNPEAK }, {"wnpeak", __MCC_QUICK_WNPEAK }, {"pnpeak", __MCC_QUICK_PNPEAK },
{"eta0", __MCC_QUICK_ETA0 }, {"leta0", __MCC_QUICK_LETA0 }, {"weta0", __MCC_QUICK_WETA0 }, {"peta0", __MCC_QUICK_PETA0 },
{"etab", __MCC_QUICK_ETAB }, {"letab", __MCC_QUICK_LETAB }, {"wetab", __MCC_QUICK_WETAB }, {"petab", __MCC_QUICK_PETAB },
{"kt1", __MCC_QUICK_KT1 }, {"lkt1", __MCC_QUICK_LKT1 }, {"wkt1", __MCC_QUICK_WKT1 }, {"pkt1", __MCC_QUICK_PKT1 },
{"kt2", __MCC_QUICK_KT2 }, {"lkt2", __MCC_QUICK_LKT2 }, {"wkt2", __MCC_QUICK_WKT2 }, {"pkt2", __MCC_QUICK_PKT2 },
{"kt1l", __MCC_QUICK_KT1L }, {"lkt1l", __MCC_QUICK_LKT1L }, {"wkt1l", __MCC_QUICK_WKT1L }, {"pkt1l", __MCC_QUICK_PKT1L },
{"vth0", __MCC_QUICK_VTH0 }, {"lvth0", __MCC_QUICK_LVTH0 }, {"wvth0", __MCC_QUICK_WVTH0 }, {"pvth0", __MCC_QUICK_PVTH0 },
{"vfb", __MCC_QUICK_VFB }, {"lvfb", __MCC_QUICK_LVFB }, {"wvfb", __MCC_QUICK_WVFB }, {"pvfb", __MCC_QUICK_PVFB },
{"dwg", __MCC_QUICK_DWG }, {"ldwg", __MCC_QUICK_LDWG }, {"wdwg", __MCC_QUICK_WDWG }, {"pdwg", __MCC_QUICK_PDWG },
{"dwb", __MCC_QUICK_DWB }, {"ldwb", __MCC_QUICK_LDWB }, {"wdwb", __MCC_QUICK_WDWB }, {"pdwb", __MCC_QUICK_PDWB },
{"ngate", __MCC_QUICK_NGATE }, {"lngate", __MCC_QUICK_LNGATE }, {"wngate", __MCC_QUICK_WNGATE }, {"pngate", __MCC_QUICK_PNGATE },
{"rdsw", __MCC_QUICK_RDSW }, {"lrdsw", __MCC_QUICK_LRDSW }, {"wrdsw", __MCC_QUICK_WRDSW }, {"prdsw", __MCC_QUICK_PRDSW },
{"prt", __MCC_QUICK_PRT }, {"lprt", __MCC_QUICK_LPRT }, {"wprt", __MCC_QUICK_WPRT }, {"pprt", __MCC_QUICK_PPRT },
{"prwb", __MCC_QUICK_PRWB }, {"lprwb", __MCC_QUICK_LPRWB }, {"wprwb", __MCC_QUICK_WPRWB }, {"pprwb", __MCC_QUICK_PPRWB },
{"prwg", __MCC_QUICK_PRWG }, {"lprwg", __MCC_QUICK_LPRWG }, {"wprwg", __MCC_QUICK_WPRWG }, {"pprwg", __MCC_QUICK_PPRWG },
{"wr", __MCC_QUICK_WR }, {"lwr", __MCC_QUICK_LWR }, {"wwr", __MCC_QUICK_WWR }, {"pwr", __MCC_QUICK_PWR },
{"nfactor", __MCC_QUICK_NFACTOR }, {"lnfactor", __MCC_QUICK_LNFACTOR }, {"wnfactor", __MCC_QUICK_WNFACTOR }, {"pnfactor", __MCC_QUICK_PNFACTOR },
{"cdsc", __MCC_QUICK_CDSC }, {"lcdsc", __MCC_QUICK_LCDSC }, {"wcdsc", __MCC_QUICK_WCDSC }, {"pcdsc", __MCC_QUICK_PCDSC },
{"cdscb", __MCC_QUICK_CDSCB }, {"lcdscb", __MCC_QUICK_LCDSCB }, {"wcdscb", __MCC_QUICK_WCDSCB }, {"pcdscb", __MCC_QUICK_PCDSCB },
{"cdscd", __MCC_QUICK_CDSCD }, {"lcdscd", __MCC_QUICK_LCDSCD }, {"wcdscd", __MCC_QUICK_WCDSCD }, {"pcdscd", __MCC_QUICK_PCDSCD },
{"cit", __MCC_QUICK_CIT }, {"lcit", __MCC_QUICK_LCIT }, {"wcit", __MCC_QUICK_WCIT }, {"pcit", __MCC_QUICK_PCIT },
{"voff", __MCC_QUICK_VOFF }, {"lvoff", __MCC_QUICK_LVOFF }, {"wvoff", __MCC_QUICK_WVOFF }, {"pvoff", __MCC_QUICK_PVOFF },
{"a1", __MCC_QUICK_A1 }, {"la1", __MCC_QUICK_LA1 }, {"wa1", __MCC_QUICK_WA1 }, {"pa1", __MCC_QUICK_PA1 },
{"a2", __MCC_QUICK_A2 }, {"la2", __MCC_QUICK_LA2 }, {"wa2", __MCC_QUICK_WA2 }, {"pa2", __MCC_QUICK_PA2 },
{"vsat", __MCC_QUICK_VSAT }, {"lvsat", __MCC_QUICK_LVSAT }, {"wvsat", __MCC_QUICK_WVSAT }, {"pvsat", __MCC_QUICK_PVSAT },
{"at", __MCC_QUICK_AT }, {"lat", __MCC_QUICK_LAT }, {"wat", __MCC_QUICK_WAT }, {"pat", __MCC_QUICK_PAT },
{"ua", __MCC_QUICK_UA }, {"lua", __MCC_QUICK_LUA }, {"wua", __MCC_QUICK_WUA }, {"pua", __MCC_QUICK_PUA },
{"ub", __MCC_QUICK_UB }, {"lub", __MCC_QUICK_LUB }, {"wub", __MCC_QUICK_WUB }, {"pub", __MCC_QUICK_PUB },
{"uc", __MCC_QUICK_UC }, {"luc", __MCC_QUICK_LUC }, {"wuc", __MCC_QUICK_WUC }, {"puc", __MCC_QUICK_PUC },
{"ua1", __MCC_QUICK_UA1 }, {"lua1", __MCC_QUICK_LUA1 }, {"wua1", __MCC_QUICK_WUA1 }, {"pua1", __MCC_QUICK_PUA1 },
{"ub1", __MCC_QUICK_UB1 }, {"lub1", __MCC_QUICK_LUB1 }, {"wub1", __MCC_QUICK_WUB1 }, {"pub1", __MCC_QUICK_PUB1 },
{"uc1", __MCC_QUICK_UC1 }, {"luc1", __MCC_QUICK_LUC1 }, {"wuc1", __MCC_QUICK_WUC1 }, {"puc1", __MCC_QUICK_PUC1 },
{"ute", __MCC_QUICK_UTE }, {"lute", __MCC_QUICK_LUTE }, {"wute", __MCC_QUICK_WUTE }, {"pute", __MCC_QUICK_PUTE },
{"a0", __MCC_QUICK_A0 }, {"la0", __MCC_QUICK_LA0 }, {"wa0", __MCC_QUICK_WA0 }, {"pa0", __MCC_QUICK_PA0 },
{"b0", __MCC_QUICK_B0 }, {"lb0", __MCC_QUICK_LB0 }, {"wb0", __MCC_QUICK_WB0 }, {"pb0", __MCC_QUICK_PB0 },
{"b1", __MCC_QUICK_B1 }, {"lb1", __MCC_QUICK_LB1 }, {"wb1", __MCC_QUICK_WB1 }, {"pb1", __MCC_QUICK_PB1 },
{"xj", __MCC_QUICK_XJ }, {"lxj", __MCC_QUICK_LXJ }, {"wxj", __MCC_QUICK_WXJ }, {"pxj", __MCC_QUICK_PXJ },
{"keta", __MCC_QUICK_KETA }, {"lketa", __MCC_QUICK_LKETA }, {"wketa", __MCC_QUICK_WKETA }, {"pketa", __MCC_QUICK_PKETA },
{"ags", __MCC_QUICK_AGS }, {"lags", __MCC_QUICK_LAGS }, {"wags", __MCC_QUICK_WAGS }, {"pags", __MCC_QUICK_PAGS },
{"delta", __MCC_QUICK_DELTA }, {"ldelta", __MCC_QUICK_LDELTA }, {"wdelta", __MCC_QUICK_WDELTA }, {"pdelta", __MCC_QUICK_PDELTA },
{"pscbe1", __MCC_QUICK_PSCBE1 }, {"lpscbe1", __MCC_QUICK_LPSCBE1 }, {"wpscbe1", __MCC_QUICK_WPSCBE1 }, {"ppscbe1", __MCC_QUICK_PPSCBE1 },
{"pscbe2", __MCC_QUICK_PSCBE2 }, {"lpscbe2", __MCC_QUICK_LPSCBE2 }, {"wpscbe2", __MCC_QUICK_WPSCBE2 }, {"ppscbe2", __MCC_QUICK_PPSCBE2 },
{"pdiblc1", __MCC_QUICK_PDIBLC1 }, {"lpdiblc1", __MCC_QUICK_LPDIBLC1 }, {"wpdiblc1", __MCC_QUICK_WPDIBLC1 }, {"ppdiblc1", __MCC_QUICK_PPDIBLC1 },
{"pdiblc2", __MCC_QUICK_PDIBLC2 }, {"lpdiblc2", __MCC_QUICK_LPDIBLC2 }, {"wpdiblc2", __MCC_QUICK_WPDIBLC2 }, {"ppdiblc2", __MCC_QUICK_PPDIBLC2 },
{"drout", __MCC_QUICK_DROUT }, {"ldrout", __MCC_QUICK_LDROUT }, {"wdrout", __MCC_QUICK_WDROUT }, {"pdrout", __MCC_QUICK_PDROUT },
{"pdiblcb", __MCC_QUICK_PDIBLCB }, {"lpdiblcb", __MCC_QUICK_LPDIBLCB }, {"wpdiblcb", __MCC_QUICK_WPDIBLCB }, {"ppdiblcb", __MCC_QUICK_PPDIBLCB },
{"pclm", __MCC_QUICK_PCLM }, {"lpclm", __MCC_QUICK_LPCLM }, {"wpclm", __MCC_QUICK_WPCLM }, {"ppclm", __MCC_QUICK_PPCLM },
{"pvag", __MCC_QUICK_PVAG }, {"lpvag", __MCC_QUICK_LPVAG }, {"wpvag", __MCC_QUICK_WPVAG }, {"ppvag", __MCC_QUICK_PPVAG },
{"u0", __MCC_QUICK_U0 }, {"lu0", __MCC_QUICK_LU0 }, {"wu0", __MCC_QUICK_WU0 }, {"pu0", __MCC_QUICK_PU0 },
{"cf", __MCC_QUICK_CF }, {"lcf", __MCC_QUICK_LCF }, {"wcf", __MCC_QUICK_WCF }, {"pcf", __MCC_QUICK_PCF },
{"ckappa", __MCC_QUICK_CKAPPA }, {"lckappa", __MCC_QUICK_LCKAPPA }, {"wckappa", __MCC_QUICK_WCKAPPA }, {"pckappa", __MCC_QUICK_PCKAPPA },
{"cgdl", __MCC_QUICK_CGDL }, {"lcgdl", __MCC_QUICK_LCGDL }, {"wcgdl", __MCC_QUICK_WCGDL }, {"pcgdl", __MCC_QUICK_PCGDL },
{"cgd1", __MCC_QUICK_CGD1 }, {"lcgd1", __MCC_QUICK_LCGD1 }, {"wcgd1", __MCC_QUICK_WCGD1 }, {"pcgd1", __MCC_QUICK_PCGD1 },
{"cgsl", __MCC_QUICK_CGSL }, {"lcgsl", __MCC_QUICK_LCGSL }, {"wcgsl", __MCC_QUICK_WCGSL }, {"pcgsl", __MCC_QUICK_PCGSL },
{"cgs1", __MCC_QUICK_CGS1 }, {"lcgs1", __MCC_QUICK_LCGS1 }, {"wcgs1", __MCC_QUICK_WCGS1 }, {"pcgs1", __MCC_QUICK_PCGS1 },
{"tox", __MCC_QUICK_TOX }, {"ltox", __MCC_QUICK_LTOX }, {"wtox", __MCC_QUICK_WTOX }, {"ptox", __MCC_QUICK_PTOX },
{"acde", __MCC_QUICK_ACDE }, {"lacde", __MCC_QUICK_LACDE }, {"wacde", __MCC_QUICK_WACDE }, {"pacde", __MCC_QUICK_PACDE },
{"vfbcv", __MCC_QUICK_VFBCV }, {"lvfbcv", __MCC_QUICK_LVFBCV }, {"wvfbcv", __MCC_QUICK_WVFBCV }, {"pvfbcv", __MCC_QUICK_PVFBCV },
{"clc", __MCC_QUICK_CLC }, {"lclc", __MCC_QUICK_LCLC }, {"wclc", __MCC_QUICK_WCLC }, {"pclc", __MCC_QUICK_PCLC },
{"cle", __MCC_QUICK_CLE }, {"lcle", __MCC_QUICK_LCLE }, {"wcle", __MCC_QUICK_WCLE }, {"pcle", __MCC_QUICK_PCLE },
{"toxm", __MCC_QUICK_TOXM }, {"ltoxm", __MCC_QUICK_LTOXM }, {"wtoxm", __MCC_QUICK_WTOXM }, {"ptoxm", __MCC_QUICK_PTOXM },
{"voffcv", __MCC_QUICK_VOFFCV }, {"lvoffcv", __MCC_QUICK_LVOFFCV }, {"wvoffcv", __MCC_QUICK_WVOFFCV }, {"pvoffcv", __MCC_QUICK_PVOFFCV },
{"noff", __MCC_QUICK_NOFF }, {"lnoff", __MCC_QUICK_LNOFF }, {"wnoff", __MCC_QUICK_WNOFF }, {"pnoff", __MCC_QUICK_PNOFF },
{"moin", __MCC_QUICK_MOIN }, {"lmoin", __MCC_QUICK_LMOIN }, {"wmoin", __MCC_QUICK_WMOIN }, {"pmoin", __MCC_QUICK_PMOIN },
{"nsd", __MCC_QUICK_NSD }, {"lnsd", __MCC_QUICK_LNSD }, {"wnsd", __MCC_QUICK_WNSD }, {"pnsd", __MCC_QUICK_PNSD },
{"ndep", __MCC_QUICK_NDEP }, {"lndep", __MCC_QUICK_LNDEP }, {"wndep", __MCC_QUICK_WNDEP }, {"pndep", __MCC_QUICK_PNDEP },
{"vbm", __MCC_QUICK_VBM }, {"lvbm", __MCC_QUICK_LVBM }, {"wvbm", __MCC_QUICK_WVBM }, {"pvbm", __MCC_QUICK_PVBM },
{"minv", __MCC_QUICK_MINV }, {"lminv", __MCC_QUICK_LMINV }, {"wminv", __MCC_QUICK_WMINV }, {"pminv", __MCC_QUICK_PMINV },
{"tvoff", __MCC_QUICK_TVOFF }, {"ltvoff", __MCC_QUICK_LTVOFF }, {"wtvoff", __MCC_QUICK_WTVOFF }, {"ptvoff", __MCC_QUICK_PTVOFF },
{"lpeb", __MCC_QUICK_LPEB }, {"llpeb", __MCC_QUICK_LLPEB }, {"wlpeb", __MCC_QUICK_WLPEB }, {"plpeb", __MCC_QUICK_PLPEB },
{"eu", __MCC_QUICK_EU }, {"leu", __MCC_QUICK_LEU }, {"weu", __MCC_QUICK_WEU }, {"peu", __MCC_QUICK_PEU },
{"ud", __MCC_QUICK_UD }, {"lud", __MCC_QUICK_LUD }, {"wud", __MCC_QUICK_WUD }, {"pud", __MCC_QUICK_PUD },
{"ud1", __MCC_QUICK_UD1 }, {"lud1", __MCC_QUICK_LUD1 }, {"wud1", __MCC_QUICK_WUD1 }, {"pud1", __MCC_QUICK_PUD1 },
{"up", __MCC_QUICK_UP }, {"lup", __MCC_QUICK_LUP }, {"wup", __MCC_QUICK_WUP }, {"pup", __MCC_QUICK_PUP },
{"lp", __MCC_QUICK_LP }, {"llp", __MCC_QUICK_LLP }, {"wlp", __MCC_QUICK_WLP }, {"plp", __MCC_QUICK_PLP },
{"phin", __MCC_QUICK_PHIN }, {"lphin", __MCC_QUICK_LPHIN }, {"wphin", __MCC_QUICK_WPHIN }, {"pphin", __MCC_QUICK_PPHIN },
{"xt", __MCC_QUICK_XT }, {"lxt", __MCC_QUICK_LXT }, {"wxt", __MCC_QUICK_WXT }, {"pxt", __MCC_QUICK_PXT },
{"lpe0", __MCC_QUICK_LPE0 }, {"llpe0", __MCC_QUICK_LLPE0 }, {"wlpe0", __MCC_QUICK_WLPE0 }, {"plpe0", __MCC_QUICK_PLPE0 },
{"dvtp0", __MCC_QUICK_DVTP0 }, {"ldvtp0", __MCC_QUICK_LDVTP0 }, {"wdvtp0", __MCC_QUICK_WDVTP0 }, {"pdvtp0", __MCC_QUICK_PDVTP0 },
{"dvtp1", __MCC_QUICK_DVTP1 }, {"ldvtp1", __MCC_QUICK_LDVTP1 }, {"wdvtp1", __MCC_QUICK_WDVTP1 }, {"pdvtp1", __MCC_QUICK_PDVTP1 },
{"fprout", __MCC_QUICK_FPROUT }, {"lfprout", __MCC_QUICK_LFPROUT }, {"wfprout", __MCC_QUICK_WFPROUT }, {"pfprout", __MCC_QUICK_PFPROUT },
{"pdits", __MCC_QUICK_PDITS }, {"lpdits", __MCC_QUICK_LPDITS }, {"wpdits", __MCC_QUICK_WPDITS }, {"ppdits", __MCC_QUICK_PPDITS },
{"pditsl", __MCC_QUICK_PDITSL }, {"lpditsl", __MCC_QUICK_LPDITSL }, {"wpditsl", __MCC_QUICK_WPDITSL }, {"ppditsl", __MCC_QUICK_PPDITSL },
{"pditsd", __MCC_QUICK_PDITSD }, {"lpditsd", __MCC_QUICK_LPDITSD }, {"wpditsd", __MCC_QUICK_WPDITSD }, {"ppditsd", __MCC_QUICK_PPDITSD },
{"vtl", __MCC_QUICK_VTL }, {"lvtl", __MCC_QUICK_LVTL }, {"wvtl", __MCC_QUICK_WVTL }, {"pvtl", __MCC_QUICK_PVTL },
{"xn", __MCC_QUICK_XN }, {"lxn", __MCC_QUICK_LXN }, {"wxn", __MCC_QUICK_WXN }, {"pxn", __MCC_QUICK_PXN },
{"lc", __MCC_QUICK_LC }, {"llc", __MCC_QUICK_LLC }, {"wlc", __MCC_QUICK_WLC }, {"plc", __MCC_QUICK_PLC },
{"ckappad", __MCC_QUICK_CKAPPAD }, {"lckappad", __MCC_QUICK_LCKAPPAD }, {"wckappad", __MCC_QUICK_WCKAPPAD }, {"pckappad", __MCC_QUICK_PCKAPPAD },
{"ntox", __MCC_QUICK_NTOX }, {"lntox", __MCC_QUICK_LNTOX }, {"wntox", __MCC_QUICK_WNTOX }, {"pntox", __MCC_QUICK_PNTOX },
{"aigbacc", __MCC_QUICK_AIGBACC }, {"laigbacc", __MCC_QUICK_LAIGBACC }, {"waigbacc", __MCC_QUICK_WAIGBACC }, {"paigbacc", __MCC_QUICK_PAIGBACC },
{"bigbacc", __MCC_QUICK_BIGBACC }, {"lbigbacc", __MCC_QUICK_LBIGBACC }, {"wbigbacc", __MCC_QUICK_WBIGBACC }, {"pbigbacc", __MCC_QUICK_PBIGBACC },
{"cigbacc", __MCC_QUICK_CIGBACC }, {"lcigbacc", __MCC_QUICK_LCIGBACC }, {"wcigbacc", __MCC_QUICK_WCIGBACC }, {"pcigbacc", __MCC_QUICK_PCIGBACC },
{"nigbacc", __MCC_QUICK_NIGBACC }, {"lnigbacc", __MCC_QUICK_LNIGBACC }, {"wnigbacc", __MCC_QUICK_WNIGBACC }, {"pnigbacc", __MCC_QUICK_PNIGBACC },
{"aigbinv", __MCC_QUICK_AIGBINV }, {"laigbinv", __MCC_QUICK_LAIGBINV }, {"waigbinv", __MCC_QUICK_WAIGBINV }, {"paigbinv", __MCC_QUICK_PAIGBINV },
{"bigbinv", __MCC_QUICK_BIGBINV }, {"lbigbinv", __MCC_QUICK_LBIGBINV }, {"wbigbinv", __MCC_QUICK_WBIGBINV }, {"pbigbinv", __MCC_QUICK_PBIGBINV },
{"cigbinv", __MCC_QUICK_CIGBINV }, {"lcigbinv", __MCC_QUICK_LCIGBINV }, {"wcigbinv", __MCC_QUICK_WCIGBINV }, {"pcigbinv", __MCC_QUICK_PCIGBINV },
{"nigbinv", __MCC_QUICK_NIGBINV }, {"lnigbinv", __MCC_QUICK_LNIGBINV }, {"wnigbinv", __MCC_QUICK_WNIGBINV }, {"pnigbinv", __MCC_QUICK_PNIGBINV },
{"eigbinv", __MCC_QUICK_EIGBINV }, {"leigbinv", __MCC_QUICK_LEIGBINV }, {"weigbinv", __MCC_QUICK_WEIGBINV }, {"peigbinv", __MCC_QUICK_PEIGBINV },
{"agidl", __MCC_QUICK_AGIDL }, {"lagidl", __MCC_QUICK_LAGIDL }, {"wagidl", __MCC_QUICK_WAGIDL }, {"pagidl", __MCC_QUICK_PAGIDL },
{"bgidl", __MCC_QUICK_BGIDL }, {"lbgidl", __MCC_QUICK_LBGIDL }, {"wbgidl", __MCC_QUICK_WBGIDL }, {"pbgidl", __MCC_QUICK_PBGIDL },
{"cgidl", __MCC_QUICK_CGIDL }, {"lcgidl", __MCC_QUICK_LCGIDL }, {"wcgidl", __MCC_QUICK_WCGIDL }, {"pcgidl", __MCC_QUICK_PCGIDL },
{"egidl", __MCC_QUICK_EGIDL }, {"legidl", __MCC_QUICK_LEGIDL }, {"wegidl", __MCC_QUICK_WEGIDL }, {"pegidl", __MCC_QUICK_PEGIDL },
{"agisl", __MCC_QUICK_AGISL }, {"lagisl", __MCC_QUICK_LAGISL }, {"wagisl", __MCC_QUICK_WAGISL }, {"pagisl", __MCC_QUICK_PAGISL },
{"bgisl", __MCC_QUICK_BGISL }, {"lbgisl", __MCC_QUICK_LBGISL }, {"wbgisl", __MCC_QUICK_WBGISL }, {"pbgisl", __MCC_QUICK_PBGISL },
{"cgisl", __MCC_QUICK_CGISL }, {"lcgisl", __MCC_QUICK_LCGISL }, {"wcgisl", __MCC_QUICK_WCGISL }, {"pcgisl", __MCC_QUICK_PCGISL },
{"egisl", __MCC_QUICK_EGISL }, {"legisl", __MCC_QUICK_LEGISL }, {"wegisl", __MCC_QUICK_WEGISL }, {"pegisl", __MCC_QUICK_PEGISL },
{"jsd", __MCC_QUICK_JSD }, {"ljsd", __MCC_QUICK_LJSD }, {"wjsd", __MCC_QUICK_WJSD }, {"pjsd", __MCC_QUICK_PJSD },
{"jsswd", __MCC_QUICK_JSSWD }, {"ljsswd", __MCC_QUICK_LJSSWD }, {"wjsswd", __MCC_QUICK_WJSSWD }, {"pjsswd", __MCC_QUICK_PJSSWD },
{"jsswgd", __MCC_QUICK_JSSWGD }, {"ljsswgd", __MCC_QUICK_LJSSWGD }, {"wjsswgd", __MCC_QUICK_WJSSWGD }, {"pjsswgd", __MCC_QUICK_PJSSWGD },
{"bvd", __MCC_QUICK_BVD }, {"lbvd", __MCC_QUICK_LBVD }, {"wbvd", __MCC_QUICK_WBVD }, {"pbvd", __MCC_QUICK_PBVD },
{"njd", __MCC_QUICK_NJD }, {"lnjd", __MCC_QUICK_LNJD }, {"wnjd", __MCC_QUICK_WNJD }, {"pnjd", __MCC_QUICK_PNJD },
{"xjbvd", __MCC_QUICK_XJBVD }, {"lxjbvd", __MCC_QUICK_LXJBVD }, {"wxjbvd", __MCC_QUICK_WXJBVD }, {"pxjbvd", __MCC_QUICK_PXJBVD },
{"xtid", __MCC_QUICK_XTID }, {"lxtid", __MCC_QUICK_LXTID }, {"wxtid", __MCC_QUICK_WXTID }, {"pxtid", __MCC_QUICK_PXTID },
{"jss", __MCC_QUICK_JSS }, {"ljss", __MCC_QUICK_LJSS }, {"wjss", __MCC_QUICK_WJSS }, {"pjss", __MCC_QUICK_PJSS },
{"jssws", __MCC_QUICK_JSSWS }, {"ljssws", __MCC_QUICK_LJSSWS }, {"wjssws", __MCC_QUICK_WJSSWS }, {"pjssws", __MCC_QUICK_PJSSWS },
{"jsswgs", __MCC_QUICK_JSSWGS }, {"ljsswgs", __MCC_QUICK_LJSSWGS }, {"wjsswgs", __MCC_QUICK_WJSSWGS }, {"pjsswgs", __MCC_QUICK_PJSSWGS },
{"bvs", __MCC_QUICK_BVS }, {"lbvs", __MCC_QUICK_LBVS }, {"wbvs", __MCC_QUICK_WBVS }, {"pbvs", __MCC_QUICK_PBVS },
{"njs", __MCC_QUICK_NJS }, {"lnjs", __MCC_QUICK_LNJS }, {"wnjs", __MCC_QUICK_WNJS }, {"pnjs", __MCC_QUICK_PNJS },
{"xjbvs", __MCC_QUICK_XJBVS }, {"lxjbvs", __MCC_QUICK_LXJBVS }, {"wxjbvs", __MCC_QUICK_WXJBVS }, {"pxjbvs", __MCC_QUICK_PXJBVS },
{"xtis", __MCC_QUICK_XTIS }, {"lxtis", __MCC_QUICK_LXTIS }, {"wxtis", __MCC_QUICK_WXTIS }, {"pxtis", __MCC_QUICK_PXTIS },
{"poxedge", __MCC_QUICK_POXEDGE }, {"lpoxedge", __MCC_QUICK_LPOXEDGE }, {"wpoxedge", __MCC_QUICK_WPOXEDGE }, {"ppoxedge", __MCC_QUICK_PPOXEDGE },
{"dlcigd", __MCC_QUICK_DLCIGD }, {"ldlcigd", __MCC_QUICK_LDLCIGD }, {"wdlcigd", __MCC_QUICK_WDLCIGD }, {"pdlcigd", __MCC_QUICK_PDLCIGD },
{"aigsd", __MCC_QUICK_AIGSD }, {"laigsd", __MCC_QUICK_LAIGSD }, {"waigsd", __MCC_QUICK_WAIGSD }, {"paigsd", __MCC_QUICK_PAIGSD },
{"bigsd", __MCC_QUICK_BIGSD }, {"lbigsd", __MCC_QUICK_LBIGSD }, {"wbigsd", __MCC_QUICK_WBIGSD }, {"pbigsd", __MCC_QUICK_PBIGSD },
{"cigsd", __MCC_QUICK_CIGSD }, {"lcigsd", __MCC_QUICK_LCIGSD }, {"wcigsd", __MCC_QUICK_WCIGSD }, {"pcigsd", __MCC_QUICK_PCIGSD },
{"dlcig", __MCC_QUICK_DLCIG }, {"ldlcig", __MCC_QUICK_LDLCIG }, {"wdlcig", __MCC_QUICK_WDLCIG }, {"pdlcig", __MCC_QUICK_PDLCIG },
{"aigs", __MCC_QUICK_AIGS }, {"laigs", __MCC_QUICK_LAIGS }, {"waigs", __MCC_QUICK_WAIGS }, {"paigs", __MCC_QUICK_PAIGS },
{"bigs", __MCC_QUICK_BIGS }, {"lbigs", __MCC_QUICK_LBIGS }, {"wbigs", __MCC_QUICK_WBIGS }, {"pbigs", __MCC_QUICK_PBIGS },
{"cigs", __MCC_QUICK_CIGS }, {"lcigs", __MCC_QUICK_LCIGS }, {"wcigs", __MCC_QUICK_WCIGS }, {"pcigs", __MCC_QUICK_PCIGS },
{"vfbsdoff", __MCC_QUICK_VFBSDOFF }, {"lvfbsdoff", __MCC_QUICK_LVFBSDOFF }, {"wvfbsdoff", __MCC_QUICK_WVFBSDOFF }, {"pvfbsdoff", __MCC_QUICK_PVFBSDOFF },
{"tvfbsdoff", __MCC_QUICK_TVFBSDOFF }, {"ltvfbsdoff", __MCC_QUICK_LTVFBSDOFF }, {"wtvfbsdoff", __MCC_QUICK_WTVFBSDOFF }, {"ptvfbsdoff", __MCC_QUICK_PTVFBSDOFF },
{"aigc", __MCC_QUICK_AIGC }, {"laigc", __MCC_QUICK_LAIGC }, {"waigc", __MCC_QUICK_WAIGC }, {"paigc", __MCC_QUICK_PAIGC },
{"bigc", __MCC_QUICK_BIGC }, {"lbigc", __MCC_QUICK_LBIGC }, {"wbigc", __MCC_QUICK_WBIGC }, {"pbigc", __MCC_QUICK_PBIGC },
{"cigc", __MCC_QUICK_CIGC }, {"lcigc", __MCC_QUICK_LCIGC }, {"wcigc", __MCC_QUICK_WCIGC }, {"pcigc", __MCC_QUICK_PCIGC },
{"nigc", __MCC_QUICK_NIGC }, {"lnigc", __MCC_QUICK_LNIGC }, {"wnigc", __MCC_QUICK_WNIGC }, {"pnigc", __MCC_QUICK_PNIGC },
{"pigcd", __MCC_QUICK_PIGCD }, {"lpigcd", __MCC_QUICK_LPIGCD }, {"wpigcd", __MCC_QUICK_WPIGCD }, {"ppigcd", __MCC_QUICK_PPIGCD },
{"kvth0we", __MCC_QUICK_KVTH0WE }, {"lkvth0we", __MCC_QUICK_LKVTH0WE }, {"wkvth0we", __MCC_QUICK_WKVTH0WE }, {"pkvth0we", __MCC_QUICK_PKVTH0WE },
{"k2we", __MCC_QUICK_K2WE }, {"lk2we", __MCC_QUICK_LK2WE }, {"wk2we", __MCC_QUICK_WK2WE }, {"pk2we", __MCC_QUICK_PK2WE },
{"ku0we", __MCC_QUICK_KU0WE }, {"lku0we", __MCC_QUICK_LKU0WE }, {"wku0we", __MCC_QUICK_WKU0WE }, {"pku0we", __MCC_QUICK_PKU0WE },
{"pjsm", __MCC_QUICK_PJSM },
{"pjsw", __MCC_QUICK_PJSW },
{"zet1r", __MCC_QUICK_ZET1R },
{"xrcrg2", __MCC_QUICK_XRCRG2 },
{"xrcrg1", __MCC_QUICK_XRCRG1 },
{"xqc", __MCC_QUICK_XQC },
{"xgw", __MCC_QUICK_XGW },
{"xgl", __MCC_QUICK_XGL },
{"wvfcbv", __MCC_QUICK_WVFCBV },
{"wdog", __MCC_QUICK_WDOG },
{"w", __MCC_QUICK_W },
{"vpr", __MCC_QUICK_VPR },
{"vnds", __MCC_QUICK_VNDS },
{"vmax", __MCC_QUICK_VMAX },
{"vfcbv", __MCC_QUICK_VFCBV },
{"vbx", __MCC_QUICK_VBX },
{"vb", __MCC_QUICK_VB },
{"uo", __MCC_QUICK_UO },
{"uexp", __MCC_QUICK_UEXP },
{"ucrit", __MCC_QUICK_UCRIT },
{"tvj", __MCC_QUICK_TVJ },
{"ttt2", __MCC_QUICK_TTT2 },
{"ttt1", __MCC_QUICK_TTT1 },
{"tt", __MCC_QUICK_TT },
{"trsh2", __MCC_QUICK_TRSH2 },
{"trsh1", __MCC_QUICK_TRSH1 },
{"trs2", __MCC_QUICK_TRS2 },
{"trs1", __MCC_QUICK_TRS1 },
{"trs", __MCC_QUICK_TRS },
{"trnqsmod", __MCC_QUICK_TRNQSMOD },
{"tref", __MCC_QUICK_TREF },
{"trd2", __MCC_QUICK_TRD2 },
{"trd1", __MCC_QUICK_TRD1 },
{"tr", __MCC_QUICK_TR },
{"tpg", __MCC_QUICK_TPG },
{"tnoimod", __MCC_QUICK_TNOIMOD },
{"tnoib", __MCC_QUICK_TNOIB },
{"tnoia", __MCC_QUICK_TNOIA },
{"tm2", __MCC_QUICK_TM2 },
{"tm1", __MCC_QUICK_TM1 },
{"tlevr", __MCC_QUICK_TLEVR },
{"tlevi", __MCC_QUICK_TLEVI },
{"tikf", __MCC_QUICK_TIKF },
{"theta", __MCC_QUICK_THETA },
{"the3r", __MCC_QUICK_THE3R },
{"the2r", __MCC_QUICK_THE2R },
{"the1r", __MCC_QUICK_THE1R },
{"tcv", __MCC_QUICK_TCV },
{"tbv2", __MCC_QUICK_TBV2 },
{"tbv1", __MCC_QUICK_TBV1 },
{"swthe3", __MCC_QUICK_SWTHE3 },
{"swthe2", __MCC_QUICK_SWTHE2 },
{"swthe1", __MCC_QUICK_SWTHE1 },
{"swalp", __MCC_QUICK_SWALP },
{"swa3", __MCC_QUICK_SWA3 },
{"swa2", __MCC_QUICK_SWA2 },
{"swa1", __MCC_QUICK_SWA1 },
{"stthe3r", __MCC_QUICK_STTHE3R },
{"stthe2r", __MCC_QUICK_STTHE2R },
{"stthe1r", __MCC_QUICK_STTHE1R },
{"stmo", __MCC_QUICK_STMO },
{"stlthe3", __MCC_QUICK_STLTHE3 },
{"stlthe2", __MCC_QUICK_STLTHE2 },
{"stlthe1", __MCC_QUICK_STLTHE1 },
{"sta1", __MCC_QUICK_STA1 },
{"slzet1", __MCC_QUICK_SLZET1 },
{"slvsbt", __MCC_QUICK_SLVSBT },
{"slthe3r", __MCC_QUICK_SLTHE3R },
{"slthe2r", __MCC_QUICK_SLTHE2R },
{"slthe1r", __MCC_QUICK_SLTHE1R },
{"slmo", __MCC_QUICK_SLMO },
{"slalp", __MCC_QUICK_SLALP },
{"sla3", __MCC_QUICK_SLA3 },
{"sla2", __MCC_QUICK_SLA2 },
{"sla1", __MCC_QUICK_SLA1 },
{"scalev", __MCC_QUICK_SCALEV },
{"sc", __MCC_QUICK_SC },
{"sbth", __MCC_QUICK_SBTH },
{"rshg", __MCC_QUICK_RSHG },
{"rnoib", __MCC_QUICK_RNOIB },
{"rnoia", __MCC_QUICK_RNOIA },
{"rgeomod", __MCC_QUICK_RGEOMOD },
{"rgatemod", __MCC_QUICK_RGATEMOD },
{"rdwmin", __MCC_QUICK_RDWMIN },
{"rdw", __MCC_QUICK_RDW },
{"rdsmin", __MCC_QUICK_RDSMIN },
{"rds", __MCC_QUICK_RDS },
{"rbsb", __MCC_QUICK_RBSB },
{"rbps", __MCC_QUICK_RBPS },
{"rbpd", __MCC_QUICK_RBPD },
{"rbpb", __MCC_QUICK_RBPB },
{"rbodymod", __MCC_QUICK_RBODYMOD },
{"rbdb", __MCC_QUICK_RBDB },
{"rb", __MCC_QUICK_RB },
{"pvfcbv", __MCC_QUICK_PVFCBV },
{"pt", __MCC_QUICK_PT },
{"phi", __MCC_QUICK_PHI },
{"perim", __MCC_QUICK_PERIM },
{"pditls", __MCC_QUICK_PDITLS },
{"ntr", __MCC_QUICK_NTR },
{"ntnoi", __MCC_QUICK_NTNOI },
{"nstar", __MCC_QUICK_NSTAR },
{"nss", __MCC_QUICK_NSS },
{"nsj", __MCC_QUICK_NSJ },
{"ns", __MCC_QUICK_NS },
{"nrs", __MCC_QUICK_NRS },
{"nrd", __MCC_QUICK_NRD },
{"nr", __MCC_QUICK_NR },
{"nqsmod", __MCC_QUICK_NQSMOD },
{"noic", __MCC_QUICK_NOIC },
{"noib", __MCC_QUICK_NOIB },
{"noia", __MCC_QUICK_NOIA },
{"ngj", __MCC_QUICK_NGJ },
{"ngcon", __MCC_QUICK_NGCON },
{"ng", __MCC_QUICK_NG },
{"nfs", __MCC_QUICK_NFS },
{"nfr", __MCC_QUICK_NFR },
{"nfmod", __MCC_QUICK_NFMOD },
{"nfcr", __MCC_QUICK_NFCR },
{"nfbr", __MCC_QUICK_NFBR },
{"nfar", __MCC_QUICK_NFAR },
{"neff", __MCC_QUICK_NEFF },
{"nds", __MCC_QUICK_NDS },
{"nbvl", __MCC_QUICK_NBVL },
{"nbv", __MCC_QUICK_NBV },
{"nbj", __MCC_QUICK_NBJ },
{"nb", __MCC_QUICK_NB },
{"n", __MCC_QUICK_N },
{"mor", __MCC_QUICK_MOR },
{"meto", __MCC_QUICK_METO },
{"lvfcbv", __MCC_QUICK_LVFCBV },
{"ls", __MCC_QUICK_LS },
{"lmob", __MCC_QUICK_LMOB },
{"lis", __MCC_QUICK_LIS },
{"lg", __MCC_QUICK_LG },
{"lclm", __MCC_QUICK_LCLM },
{"lcap", __MCC_QUICK_LCAP },
{"l", __MCC_QUICK_L },
{"kp", __MCC_QUICK_KP },
{"kf", __MCC_QUICK_KF },
{"kappa", __MCC_QUICK_KAPPA },
{"jsgsr", __MCC_QUICK_JSGSR },
{"jsggr", __MCC_QUICK_JSGGR },
{"jsgbr", __MCC_QUICK_JSGBR },
{"jsdsr", __MCC_QUICK_JSDSR },
{"jsdgr", __MCC_QUICK_JSDGR },
{"jsdbr", __MCC_QUICK_JSDBR },
{"jf", __MCC_QUICK_JF },
{"isw", __MCC_QUICK_ISW },
{"istmp", __MCC_QUICK_ISTMP },
{"istemp", __MCC_QUICK_ISTEMP },
{"isr", __MCC_QUICK_ISR },
{"isp", __MCC_QUICK_ISP },
{"isa", __MCC_QUICK_ISA },
{"is", __MCC_QUICK_IS },
{"ikr", __MCC_QUICK_IKR },
{"ik", __MCC_QUICK_IK },
{"igbmod", __MCC_QUICK_IGBMOD },
{"ibvl", __MCC_QUICK_IBVL },
{"ibv", __MCC_QUICK_IBV },
{"gbmin", __MCC_QUICK_GBMIN },
{"gamma", __MCC_QUICK_GAMMA },
{"fte1", __MCC_QUICK_FTE1 },
{"fnoimod", __MCC_QUICK_FNOIMOD },
{"flkflag", __MCC_QUICK_FLKFLAG },
{"exp", __MCC_QUICK_EXP },
{"exa", __MCC_QUICK_EXA },
{"etazet", __MCC_QUICK_ETAZET },
{"etamr", __MCC_QUICK_ETAMR },
{"etabet", __MCC_QUICK_ETABET },
{"etaalp", __MCC_QUICK_ETAALP },
{"eta", __MCC_QUICK_ETA },
{"em", __MCC_QUICK_EM },
{"elm", __MCC_QUICK_ELM },
{"ef", __MCC_QUICK_EF },
{"dtox", __MCC_QUICK_DTOX },
{"dta", __MCC_QUICK_DTA },
{"dmcgt", __MCC_QUICK_DMCGT },
{"diomod", __MCC_QUICK_DIOMOD },
{"dcaplev", __MCC_QUICK_DCAPLEV },
{"cox", __MCC_QUICK_COX },
{"cjp", __MCC_QUICK_CJP },
{"cja", __MCC_QUICK_CJA },
{"cj0", __MCC_QUICK_CJ0 },
{"cgbo", __MCC_QUICK_CGBO },
{"cbs", __MCC_QUICK_CBS },
{"cbd", __MCC_QUICK_CBD },
{"bex", __MCC_QUICK_BEX },
{"betsq", __MCC_QUICK_BETSQ },
{"beta0", __MCC_QUICK_BETA0 },
{"area", __MCC_QUICK_AREA },
{"alpr", __MCC_QUICK_ALPR },
{"alpha1", __MCC_QUICK_ALPHA1 },
{"alpha0", __MCC_QUICK_ALPHA0 },
{"alev", __MCC_QUICK_ALEV },
{"af", __MCC_QUICK_AF },
{"acnqsmod", __MCC_QUICK_ACNQSMOD },
{"ab", __MCC_QUICK_AB },
{"a3r", __MCC_QUICK_A3R },
{"a2r", __MCC_QUICK_A2R },
{"a1r", __MCC_QUICK_A1R },
{"binu", __MCC_QUICK_BINU },
{"deltvto", __MCC_QUICK_DELTVTO },
{"deltvt0", __MCC_QUICK_DELTVT0 },
{"col", __MCC_QUICK_COL },
{"cgs0", __MCC_QUICK_CGS0 },
{"cgd0", __MCC_QUICK_CGD0 },
{"wd", __MCC_QUICK_WD },
{"ver", __MCC_QUICK_VER },
{"delvto", __MCC_QUICK_DELVTO },
{"vt0", __MCC_QUICK_VT0 },
{"vto", __MCC_QUICK_VTO },
{"vtho", __MCC_QUICK_VTHO },
{"swjuncap", __MCC_QUICK_SWJUNCAP },
{"ptp", __MCC_QUICK_PTP },
{"pta", __MCC_QUICK_PTA },
{"ctp", __MCC_QUICK_CTP },
{"cta", __MCC_QUICK_CTA },
{"tphp", __MCC_QUICK_TPHP },
{"m", __MCC_QUICK_M },
{"scalm", __MCC_QUICK_SCALM },
{"shrink", __MCC_QUICK_SHRINK },
{"xm", __MCC_QUICK_XM },
{"wm", __MCC_QUICK_WM },
{"lm", __MCC_QUICK_LM },
{"xp", __MCC_QUICK_XP },
{"wp", __MCC_QUICK_WP },
{"xom", __MCC_QUICK_XOM },
{"xoi", __MCC_QUICK_XOI },
{"vj", __MCC_QUICK_VJ },
{"cjo", __MCC_QUICK_CJO },
{"kms", __MCC_QUICK_KMS },
{"fc", __MCC_QUICK_FC },
{"dcap", __MCC_QUICK_DCAP },
{"hdif", __MCC_QUICK_HDIF },
{"ldif", __MCC_QUICK_LDIF },
{"dw", __MCC_QUICK_DW },
{"ld", __MCC_QUICK_LD },
{"rd", __MCC_QUICK_RD },
{"rs", __MCC_QUICK_RS },
{"rdc", __MCC_QUICK_RDC },
{"rsc", __MCC_QUICK_RSC },
{"rsh", __MCC_QUICK_RSH },
{"rlev", __MCC_QUICK_RLEV },
{"jr", __MCC_QUICK_JR },
{"er", __MCC_QUICK_ER },
{"fcs", __MCC_QUICK_FCS },
{"calcacm", __MCC_QUICK_CALCACM },
{"tmod", __MCC_QUICK_TMOD },
{"delw", __MCC_QUICK_DELW },
{"dell", __MCC_QUICK_DELL },
{"pg", __MCC_QUICK_PG },
{"vdgr", __MCC_QUICK_VDGR },
{"cjgate", __MCC_QUICK_CJGATE },
{"cjgr", __MCC_QUICK_CJGR },
{"cjsr", __MCC_QUICK_CJSR },
{"vdsr", __MCC_QUICK_VDSR },
{"ps", __MCC_QUICK_PS },
{"mjsw", __MCC_QUICK_MJSW },
{"cjsw", __MCC_QUICK_CJSW },
{"trdio9", __MCC_QUICK_TRDIO9 },
{"cjbr", __MCC_QUICK_CJBR },
{"vdbr", __MCC_QUICK_VDBR },
{"vr", __MCC_QUICK_VR },
{"tlevc", __MCC_QUICK_TLEVC },
{"pb", __MCC_QUICK_PB },
{"mj", __MCC_QUICK_MJ },
{"cj", __MCC_QUICK_CJ },
{"diolev", __MCC_QUICK_DIOLEV },
{"eg", __MCC_QUICK_EG },
{"gap2", __MCC_QUICK_GAP2 },
{"gap1", __MCC_QUICK_GAP1 },
{"wmlt", __MCC_QUICK_WMLT },
{"lmlt", __MCC_QUICK_LMLT },
{"tlev", __MCC_QUICK_TLEV },
{"etadsr", __MCC_QUICK_ETADSR },
{"swgam1", __MCC_QUICK_SWGAM1 },
{"slgam1", __MCC_QUICK_SLGAM1 },
{"gam1r", __MCC_QUICK_GAM1R },
{"etagamr", __MCC_QUICK_ETAGAMR },
{"slgamoo", __MCC_QUICK_SLGAMOO },
{"gamoor", __MCC_QUICK_GAMOOR },
{"slvbst", __MCC_QUICK_SLVBST },
{"vsbtr", __MCC_QUICK_VSBTR },
{"swvsbx", __MCC_QUICK_SWVSBX },
{"slvsbx", __MCC_QUICK_SLVSBX },
{"vsbxr", __MCC_QUICK_VSBXR },
{"swko", __MCC_QUICK_SWKO },
{"swk", __MCC_QUICK_SWK },
{"slko", __MCC_QUICK_SLKO },
{"slk", __MCC_QUICK_SLK },
{"kor", __MCC_QUICK_KOR },
{"kr", __MCC_QUICK_KR },
{"swvto", __MCC_QUICK_SWVTO },
{"sl2vto", __MCC_QUICK_SL2VTO },
{"slvto", __MCC_QUICK_SLVTO },
{"wer", __MCC_QUICK_WER },
{"ler", __MCC_QUICK_LER },
{"stvto", __MCC_QUICK_STVTO },
{"vtor", __MCC_QUICK_VTOR },
{"phibr", __MCC_QUICK_PHIBR },
{"wot", __MCC_QUICK_WOT },
{"lap", __MCC_QUICK_LAP },
{"wvar", __MCC_QUICK_WVAR },
{"lvar", __MCC_QUICK_LVAR },
{"init", __MCC_QUICK_INIT },
{"scref", __MCC_QUICK_SCREF },
{"wec", __MCC_QUICK_WEC },
{"web", __MCC_QUICK_WEB },
{"min", __MCC_QUICK_MIN },
{"dmdg", __MCC_QUICK_DMDG },
{"geomod", __MCC_QUICK_GEOMOD },
{"toxref", __MCC_QUICK_TOXREF },
{"igcmod", __MCC_QUICK_IGCMOD },
{"tcjswg", __MCC_QUICK_TCJSWG },
{"tpbswg", __MCC_QUICK_TPBSWG },
{"permod", __MCC_QUICK_PERMOD },
{"tcjsw", __MCC_QUICK_TCJSW },
{"tpbsw", __MCC_QUICK_TPBSW },
{"optacm", __MCC_QUICK_OPTACM },
{"tcj", __MCC_QUICK_TCJ },
{"tpb", __MCC_QUICK_TPB },
{"lambda", __MCC_QUICK_LAMBDA },
{"pbswgd", __MCC_QUICK_PBSWGD },
{"pbswgs", __MCC_QUICK_PBSWGS },
{"pbsws", __MCC_QUICK_PBSWS },
{"pbswd", __MCC_QUICK_PBSWD },
{"pbs", __MCC_QUICK_PBS },
{"pbd", __MCC_QUICK_PBD },
{"mjswgd", __MCC_QUICK_MJSWGD },
{"mjswgs", __MCC_QUICK_MJSWGS },
{"cjswgd", __MCC_QUICK_CJSWGD },
{"cjswgs", __MCC_QUICK_CJSWGS },
{"cjsws", __MCC_QUICK_CJSWS },
{"cjswd", __MCC_QUICK_CJSWD },
{"mjsws", __MCC_QUICK_MJSWS },
{"mjswd", __MCC_QUICK_MJSWD },
{"mjs", __MCC_QUICK_MJS },
{"mjd", __MCC_QUICK_MJD },
{"cjs", __MCC_QUICK_CJS },
{"cjd", __MCC_QUICK_CJD },
{"jswgs", __MCC_QUICK_JSWGS },
{"jswgd", __MCC_QUICK_JSWGD },
{"jsws", __MCC_QUICK_JSWS },
{"jswd", __MCC_QUICK_JSWD },
{"ijthsfwd", __MCC_QUICK_IJTHSFWD },
{"ijthdfwd", __MCC_QUICK_IJTHDFWD },
{"ijthsrev", __MCC_QUICK_IJTHSREV },
{"ijthdrev", __MCC_QUICK_IJTHDREV },
{"dwj", __MCC_QUICK_DWJ },
{"dmcg", __MCC_QUICK_DMCG },
{"dmci", __MCC_QUICK_DMCI },
{"ckappas", __MCC_QUICK_CKAPPAS },
{"cigd", __MCC_QUICK_CIGD },
{"bigd", __MCC_QUICK_BIGD },
{"aigd", __MCC_QUICK_AIGD },
{"ndef", __MCC_QUICK_NDEF },
{"toxp", __MCC_QUICK_TOXP },
{"epsrox", __MCC_QUICK_EPSROX },
{"paramchk", __MCC_QUICK_PARAMCHK },
{"rdswmin", __MCC_QUICK_RDSWMIN },
{"rdsmod", __MCC_QUICK_RDSMOD },
{"tempmod", __MCC_QUICK_TEMPMOD },
{"toxe", __MCC_QUICK_TOXE },
{"voffl", __MCC_QUICK_VOFFL },
{"sbref", __MCC_QUICK_SBREF },
{"saref", __MCC_QUICK_SAREF },
{"tku0", __MCC_QUICK_TKU0 },
{"wlodku0", __MCC_QUICK_WLODKU0 },
{"llodku0", __MCC_QUICK_LLODKU0 },
{"pku0", __MCC_QUICK_PKU0 },
{"wku0", __MCC_QUICK_WKU0 },
{"lku0", __MCC_QUICK_LKU0 },
{"ku0", __MCC_QUICK_KU0 },
{"wlod", __MCC_QUICK_WLOD },
{"wlodvth", __MCC_QUICK_WLODVTH },
{"llodvth", __MCC_QUICK_LLODVTH },
{"pkvth0", __MCC_QUICK_PKVTH0 },
{"wkvth0", __MCC_QUICK_WKVTH0 },
{"lkvth0", __MCC_QUICK_LKVTH0 },
{"kvth0", __MCC_QUICK_KVTH0 },
{"lodk2", __MCC_QUICK_LODK2 },
{"stk2", __MCC_QUICK_STK2 },
{"lodeta0", __MCC_QUICK_LODETA0 },
{"steta0", __MCC_QUICK_STETA0 },
{"kvsat", __MCC_QUICK_KVSAT },
{"sd", __MCC_QUICK_SD },
{"sb", __MCC_QUICK_SB },
{"sa", __MCC_QUICK_SA },
{"nf", __MCC_QUICK_NF },
{"dtoxcv", __MCC_QUICK_DTOXCV },
{"vfbflag", __MCC_QUICK_VFBFLAG },
{"xpart", __MCC_QUICK_XPART },
{"xti", __MCC_QUICK_XTI },
{"jsw", __MCC_QUICK_JSW },
{"nj", __MCC_QUICK_NJ },
{"js", __MCC_QUICK_JS },
{"capmod", __MCC_QUICK_CAPMOD },
{"tnom", __MCC_QUICK_TNOM },
{"binunit", __MCC_QUICK_BINUNIT },
{"nd", __MCC_QUICK_ND },
{"wwn", __MCC_QUICK_WWN },
{"wln", __MCC_QUICK_WLN },
{"wwl", __MCC_QUICK_WWL },
{"ww", __MCC_QUICK_WW },
{"wl", __MCC_QUICK_WL },
{"wint", __MCC_QUICK_WINT },
{"lwn", __MCC_QUICK_LWN },
{"lln", __MCC_QUICK_LLN },
{"lwl", __MCC_QUICK_LWL },
{"lw", __MCC_QUICK_LW },
{"ll", __MCC_QUICK_LL },
{"lint", __MCC_QUICK_LINT },
{"xw", __MCC_QUICK_XW },
{"xl", __MCC_QUICK_XL },
{"nsub", __MCC_QUICK_NSUB },
{"gamma2", __MCC_QUICK_GAMMA2 },
{"gamma1", __MCC_QUICK_GAMMA1 },
{"cgso", __MCC_QUICK_CGSO },
{"", __MCC_QUICK_ },
{"cgdo", __MCC_QUICK_CGDO },
{"noimod", __MCC_QUICK_NOIMOD },
{"pbsw", __MCC_QUICK_PBSW },
{"pbswg", __MCC_QUICK_PBSWG },
{"php", __MCC_QUICK_PHP },
{"acm", __MCC_QUICK_ACM },
{"pjswg", __MCC_QUICK_PJSWG },
{"mjswg", __MCC_QUICK_MJSWG },
{"cjswg", __MCC_QUICK_CJSWG },
{"wwlc", __MCC_QUICK_WWLC },
{"lwlc", __MCC_QUICK_LWLC },
{"wwc", __MCC_QUICK_WWC },
{"lwc", __MCC_QUICK_LWC },
{"dwc", __MCC_QUICK_DWC },
{"dlc", __MCC_QUICK_DLC },
{"mobmod", __MCC_QUICK_MOBMOD },
{"version", __MCC_QUICK_VERSION },
{"level", __MCC_QUICK_LEVEL },
{"wmin", __MCC_QUICK_WMIN },
{"wmax", __MCC_QUICK_WMAX },
{"lmin", __MCC_QUICK_LMIN },
{"lmax", __MCC_QUICK_LMAX },
{"delvt0", __MCC_QUICK_DELVT0 },
{"mulu0", __MCC_QUICK_MULU0 }
  };


static struct
{
  char *name0, *name1;
  double value;
} mcc_mc_nomtrsparams[]=
  {
    {"mulu0", "factu0", 0},
    {"delvto", NULL, 0},
    {"sa", NULL, 0},
    {"sb", NULL, 0},
    {"sd", NULL, 0},
    {"nf", NULL, 0},
    {"m", NULL, 0},
    {"nrs", NULL, 0},
    {"nrd", NULL, 0},
    {"vbulk", NULL, 0},
    {"sc", NULL, 0},
    {"sca", NULL, 0},
    {"scb", NULL, 0},
    {"scc", NULL, 0},
    {"l", NULL, 0},
    {"w", NULL, 0},
    {"ps ", NULL, 0},
    {"pd ", NULL, 0},
    {"as", NULL, 0},
    {"ad", NULL, 0}
  };

static double nom_dev(char *var)
{
  unsigned int i;
  for (i=0; i<sizeof(mcc_mc_nomtrsparams)/sizeof(*mcc_mc_nomtrsparams); i++)
    {
      if (var==mcc_mc_nomtrsparams[i].name0 || var==mcc_mc_nomtrsparams[i].name1)
        return mcc_mc_nomtrsparams[i].value;
    }
  return 0;
}

static double nom_mod(char *var)
{
  double val=0;
  
  if (MCC_MC_NOM_VALUES)
    val=eqt_getvar_in_context_only(MCC_MC_NOM_VALUES, var);
  return val;
}

#define NBSPECPARAMS (sizeof(mcc_mc_nomtrsparams)/sizeof(*mcc_mc_nomtrsparams))

static void mcc_ComputeMosModelKey(lofig_list *lf)
{
  mcc_modellist  *ptmodel ;
  lotrs_list *lt;
  char *modelname;
  mcc_paramlist *param;
  double key, val, i;
  chain_list *keychain;
  ptype_list *pt;

  for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
    {
      if ((pt=getptype(lt->USER,TRANS_FIGURE))!=NULL)
        {
          modelname=getlotrsmodel(lt);
          MCC_CURRENT_LOTRS=lt;
          ptmodel = mcc_getmodel(MCC_MODELFILE,modelname,MLO_IS_TRANSN(lt->TYPE)?MCC_NMOS:MCC_PMOS,MCC_TYPICAL,((float)lt->LENGTH/SCALE_X)*1.0e-6, ((float)lt->WIDTH/SCALE_X)*1.0e-6,1) ;
          MCC_CURRENT_LOTRS=NULL;
          if (ptmodel!=NULL)
            {
              keychain=NULL;
              key=0;
              for (param=ptmodel->PARAM, i=1; param!=NULL; param=param->NEXT, i+=0.3)
                if (!param->CONSTVALUE)
                  {
                    val=param->VALUE;
                    key+=val*i;
                    keychain=addchain(keychain, NULL);
                    *(float *)&keychain->DATA=val;
                  }
              key+=(float)((long)pt->DATA)*i;
              keychain=addchain(keychain, pt->DATA);
              keychain=addchain(keychain, NULL);
              *(float *)&keychain->DATA=key;
              lt->USER=addptype(lt->USER, MCC_COMPUTED_KEY, keychain);
            }
        }
    }
}

static void mcc_set_var_err_msg(eqt_ctx *ctx, char *expr, char *buf)
{
  chain_list *cl, *ch;
  int nb;

  cl=eqt_GetVariables(ctx, expr, 0);
  nb=countchain(cl);

  strcpy(buf,": ");
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      if (ch!=cl)
      {
        if (ch->NEXT==NULL)
         strcat(buf,"and ");
        else
         strcat(buf,", ");
      }
      strcat(buf,"'");
      strcat(buf,(char *)ch->DATA);
      strcat(buf,"'");
    }
  freechain(cl);
  strcat(buf," set to 0");
}


static int mcc_sorttrs(const void *a, const void *b)
{
  lotrs_list **a0=(lotrs_list **)a;
  lotrs_list **b0=(lotrs_list **)b;
  return mbk_casestrcmp((*a0)->TRNAME, (*b0)->TRNAME);
}

void mcc_InitGlobalMonteCarloDistributions(lofig_list *lf)
{
  lotrs_list *lt;
  unsigned int i;
  int nb, j;
  long l;
  char *mckey, *subcktname;
  ptype_list *pt, *mcparam;
  double scale, res[NBSPECPARAMS];
  int done[NBSPECPARAMS];
  int present[NBSPECPARAMS];
  char errbuf[2048];
  lotrs_list **tab;

  if (!V_BOOL_TAB[__AVT_ENABLE_STAT].VALUE) return;

  if (mccMCPARAMctx!=NULL) eqt_term(mccMCPARAMctx);
  mccMCPARAMctx = eqt_init(EQT_NB_VARS) ;
  eqt_add_spice_extension(mccMCPARAMctx);
  eqt_setspecialfunc(mccMCPARAMctx, EQTSPECFUNC_NOM_MOD, nom_mod);
  eqt_setspecialfunc(mccMCPARAMctx, EQTSPECFUNC_NOM_DEV, nom_dev);
  if (MBK_GLOBALPARAMS)
    {
      EQT_CONTEXT_HIERARCHY[0]=eqt_init(32);
      eqt_import_vars (EQT_CONTEXT_HIERARCHY[0], MBK_GLOBALPARAMS);
    }
  if (MBK_GLOBALFUNC) eqt_import_func(MBK_GLOBALFUNC, mccMCPARAMctx);

  mbk_EvalGlobalMonteCarloDistributions(mccMCPARAMctx);

  if (MBK_MONTE_MODEL_PARAMS!=NULL)
    {
      lofiginfo *lfif;

      for (i=0; i<NBSPECPARAMS; i++)
        {
          mcc_mc_nomtrsparams[i].name0=namealloc(mcc_mc_nomtrsparams[i].name0);
          if (mcc_mc_nomtrsparams[i].name1!=NULL) mcc_mc_nomtrsparams[i].name1=namealloc(mcc_mc_nomtrsparams[i].name1);
        }

      if ((lfif=mbk_getlofiginfo(lf, 0))!=NULL) scale=lfif->scale;
      else scale=1;

      nb=countchain((chain_list *)lf->LOTRS);
      if (nb>0)
      {
        tab=(lotrs_list **)mbkalloc(sizeof(lotrs_list *)*nb);
        for (j=0, lt=lf->LOTRS; j<nb; lt=lt->NEXT, j++) tab[j]=lt;
        qsort(tab, nb, sizeof(lotrs_list *), mcc_sorttrs);
        
        for (j=0; j<nb; j++)
                
//      for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
        {
          lt=tab[j];
          elp_lotrs_param_get(lt,
                              &mcc_mc_nomtrsparams[0].value,
                              &mcc_mc_nomtrsparams[1].value,
                              &mcc_mc_nomtrsparams[2].value,
                              &mcc_mc_nomtrsparams[3].value,
                              &mcc_mc_nomtrsparams[4].value,
                              &mcc_mc_nomtrsparams[5].value,
                              &mcc_mc_nomtrsparams[6].value,
                              &mcc_mc_nomtrsparams[7].value,
                              &mcc_mc_nomtrsparams[8].value,
                              &mcc_mc_nomtrsparams[9].value,
                              &mcc_mc_nomtrsparams[10].value,
                              &mcc_mc_nomtrsparams[11].value,
                              &mcc_mc_nomtrsparams[12].value,
                              &mcc_mc_nomtrsparams[13].value);
          mcc_mc_nomtrsparams[14].value=((((double)lt->LENGTH)/SCALE_X)*1e-6)/scale;
          mcc_mc_nomtrsparams[15].value=((((double)lt->WIDTH)/SCALE_X)*1e-6)/scale;
          mcc_mc_nomtrsparams[16].value=((((double)lt->PS)/SCALE_X)*1e-6)/scale;
          mcc_mc_nomtrsparams[17].value=((((double)lt->PD)/SCALE_X)*1e-6)/scale;
          mcc_mc_nomtrsparams[18].value=((((double)lt->XS * lt->WIDTH) / ( SCALE_X * SCALE_X ))*1e-12)/(scale*scale);
          mcc_mc_nomtrsparams[19].value=((((double)lt->XD * lt->WIDTH) / ( SCALE_X * SCALE_X ))*1e-12)/(scale*scale);

          for (i=0; i<NBSPECPARAMS; i++)
            {
              res[i]=mcc_mc_nomtrsparams[i].value, done[i]=0;
              if (i<14)
                getlotrsparam(lt, mcc_mc_nomtrsparams[i].name0, NULL, &present[i]);
            }
    
          if ((pt=getptype (lt->USER,TRANS_FIGURE))!=NULL)
            subcktname = (char*)pt->DATA;
          else
            subcktname=NULL;

          mckey=mbk_montecarlo_key(getlotrsmodel(lt), subcktname, MLO_IS_TRANSN(lt->TYPE)?MCC_NMOS:MCC_PMOS, 'L'); 
          if ((l=gethtitem(MBK_MONTE_MODEL_PARAMS, mckey))!=EMPTYHT) mcparam=(ptype_list *)l;
          else mcparam=NULL;

          for (i=0; i<NBSPECPARAMS; i++)
            {
              for (pt=mcparam; pt!=NULL && (pt->TYPE!=(long)mcc_mc_nomtrsparams[i].name0 && pt->TYPE!=(long)mcc_mc_nomtrsparams[i].name1); pt=pt->NEXT) ;
              if (pt!=NULL)
                {
                  res[i]=eqt_eval(mccMCPARAMctx, (char *)pt->DATA, EQTFAST);
                  done[i]=1;
                  if (!eqt_resistrue(mccMCPARAMctx))
                    {
                      mcc_set_var_err_msg(mccMCPARAMctx, (char *)pt->DATA, errbuf);
                      avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, (char *)pt->TYPE, (char *)pt->DATA, errbuf);
                    }
                  else if (!isfinite(res[i]))
                    avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, (char *)pt->TYPE, (char *)pt->DATA," : returned NaN or Inf");
                }
            }

          freelotrsparams(lt);
    
          for (i=0; i<NBSPECPARAMS; i++)
            {
              if (i>=14)
                {
                  if (done[i])
                    {
                      switch (i)
                        {
                        case 14:
                          lt->LENGTH=mbk_long_round(res[i] * SCALE_X * 1E6*scale);
                          break;
                        case 15:
                          lt->WIDTH=mbk_long_round(res[i] * SCALE_X * 1E6*scale);
                          break;
                        case 16:
                          if (res[i]<0)
                            lt->PS=-1;
                          else
                            lt->PS=mbk_long_round(res[i] * SCALE_X * 1E6*scale);
                          break;
                        case 17:
                          if (res[i]<0)
                            lt->PD=-1;
                          else
                            lt->PD=mbk_long_round(res[i] * SCALE_X * 1E6*scale);
                          break;
                        case 18:
                          if (lt->XS<0)
                            lt->XS=-1;
                          else
                            lt->XS=mbk_long_round(res[i] *scale*scale* SCALE_X * 1E6 / res[15]);
                          break;
                        case 19:
                          if (lt->XD<0)
                            lt->XD=-1;
                          else
                            lt->XD=mbk_long_round(res[i] *scale*scale* SCALE_X * 1E6 / res[15]);
                          break;
                        }
                    }
                }
              else
                {
                  if (present[i] || done[i])
                    addlotrsparam(lt, mcc_mc_nomtrsparams[i].name0, res[i], NULL);
                }
            }
        }
        mbkfree(tab);
      }

    }
          
  if (MBK_GLOBALPARAMS)
    {
      eqt_term(EQT_CONTEXT_HIERARCHY[0]);
      EQT_CONTEXT_HIERARCHY[0]=NULL;
    }

  mcc_ComputeMosModelKey(lf);
}

/******************************************************************************/
/* Ajout d'expression                                                         */
/******************************************************************************/
mcc_explist *mcc_addexp(mcc_explist *head, char *name, char *exp , double defaut)
{
 mcc_explist *ptexp ;
 char pt[1024] ;

 ptexp = (mcc_explist *)mbkalloc(sizeof(mcc_explist)) ;

 downstr(name,pt) ;
 name = pt ;

 ptexp->NEXT = head ;
 head = ptexp ;
 ptexp->NAME = namealloc(name) ;
 if(exp != NULL)
  {
   ptexp->EXPR = mbkstrdup(exp) ;
   downstr(ptexp->EXPR,ptexp->EXPR) ;
  }
 else
   ptexp->EXPR = NULL;
 
 ptexp->EXPR_TO_TREE = NULL;
 ptexp->DEFAULT = defaut ;

 return(head) ;
}

/******************************************************************************/
/* Ajout d'expression                                                         */
/******************************************************************************/
mcc_explist *mcc_setexp(mcc_explist *head, char *name, char *exp , double defaut)
{
mcc_explist *pt = mcc_getexp(head, name) ;

 if(pt == NULL)
   {
     pt = mcc_addexp(head,name,exp,defaut) ;
   }
 else
   {
    pt->EXPR = mbkstrdup(exp) ;
    downstr(pt->EXPR,pt->EXPR) ;
   }

 return(pt) ;
}

/******************************************************************************/
/* recherche d'expression                                                     */
/******************************************************************************/
mcc_explist *mcc_getexp(mcc_explist *head, char *name)
{
 mcc_explist *ptexp ;
 char pt[1024] ;

 downstr(name,pt) ;
 name = namealloc(pt) ;
 ptexp = head ;

 while (ptexp) {
  if (ptexp->NAME == name) {
    return(ptexp) ;
  }
  ptexp = ptexp->NEXT ;
 }
 return (NULL) ;
}

/******************************************************************************/
/* suppression d'expression                                                   */
/******************************************************************************/
void mcc_freeexp(mcc_explist *head)
{
 mcc_explist *ptexp ;

 while (head)
 {
  ptexp = head ;
  head = head->NEXT ;
  mbkfree(ptexp) ;
 }

}

/******************************************************************************/
/* Ajout de model                                                             */
/******************************************************************************/
mcc_modellist *mcc_addmodel(char *technoname, char *transname, char *subcktname,
                            int transtype, int transcase)
{
 mcc_modellist *ptmodel ;
 mcc_technolist *pttechnofile ;
 char *cutname;
 unsigned int i;

 if ((pttechnofile = mcc_gettechno(technoname))) {
   ptmodel = (mcc_modellist *)mbkalloc(sizeof(mcc_modellist)) ;
  
   ptmodel->NEXT = pttechnofile->MODEL ;
   ptmodel->TECHNO = pttechnofile ;
   ptmodel->NAME = namealloc(transname) ;
   ptmodel->MODELTYPE = MCC_NOMODEL;
   ptmodel->SUBCKTNAME = namealloc(subcktname) ;
   ptmodel->TYPE = transtype ;
   ptmodel->CASE = transcase ;
   ptmodel->PARAM = NULL ;
   ptmodel->HPARAM = addht(100) ;
   ptmodel->origparams = NULL ;
   ptmodel->MCSEED = 0 ;
   ptmodel->USER = NULL ;
   pttechnofile->MODEL = ptmodel ;
  
   for (i=0; i<__MCC_QUICK_LAST_ITEM; i++)
     ptmodel->QUICK[i]=NULL;

   // update TNMOS and TPMOS
   
   if ( ptmodel->TYPE == MCC_NMOS ) {
     if (!mbk_istransn(ptmodel->NAME))
       TNMOS = addchain(TNMOS, ptmodel->NAME);
     cutname = namealloc(mcc_cutname ( ptmodel->NAME ));
     if (!mbk_istransn(cutname)) 
       TNMOS = addchain(TNMOS, cutname);
   }
   else if ( ptmodel->TYPE == MCC_PMOS ) {
     if (!mbk_istransp(ptmodel->NAME))
       TPMOS = addchain(TPMOS, ptmodel->NAME);
     cutname = namealloc(mcc_cutname ( ptmodel->NAME ));
     if (!mbk_istransp(cutname))
       TPMOS = addchain(TPMOS, cutname);
   }
         
  
   return pttechnofile->MODEL ;
 }
 else {
      avt_errmsg(MCC_ERRMSG, "026", AVT_ERROR, technoname) ;
     return NULL;
 }
}

/******************************************************************************/
/* fonction qui detruit tous les parametres d 1 modeles                       */
/******************************************************************************/
void free_all_param(mcc_modellist *ptmodel)
{
 mcc_paramlist *paramtodel ;

		while (ptmodel->PARAM)
		{
			paramtodel = ptmodel->PARAM ;
			ptmodel->PARAM = ptmodel->PARAM->NEXT ;
                        mcc_freeexp(paramtodel->EXPR) ;
			mbkfree(paramtodel) ;
		}
}

/******************************************************************************/
/* fonction qui detruit tous les modeles relatifs a un technofile             */
/******************************************************************************/
void free_all_model(mcc_technolist *pttechnolist)
{
 mcc_modellist *modeltodel ;

		while (pttechnolist->MODEL)
		{
			modeltodel = pttechnolist->MODEL ;
			pttechnolist->MODEL = pttechnolist->MODEL->NEXT ;
                        free_all_param(modeltodel) ;
                        delht(modeltodel->HPARAM) ;
			mbkfree(modeltodel) ;
		}
}

/******************************************************************************/
/* Suppression d'un model d'un technofile                                     */
/******************************************************************************/
void mcc_delmodel(char *technoname, char *name)
{
 mcc_modellist *ptmodel ;
 mcc_modellist *ptmodelprev ;
 mcc_modellist *ptmodelnext ;
 mcc_technolist *pttechnofile ;

 pttechnofile = mcc_gettechno(technoname) ;
 ptmodel = pttechnofile->MODEL ;

 if (ptmodel->NAME == namealloc(name))
	{
	ptmodelprev = ptmodel ; 
	ptmodel = ptmodel->NEXT ;
 	free_all_param(ptmodelprev) ;
    delht(ptmodelprev->HPARAM) ;
    freechain(ptmodelprev->origparams);
    mbkfree(ptmodelprev) ;            
	return ;
	}
 else

	while (ptmodel->NEXT != NULL)
	{
		ptmodelprev = ptmodel ;
		ptmodel = ptmodel->NEXT ;
		ptmodelnext = ptmodel->NEXT ;

		if (ptmodel->NAME == namealloc(name))
		{
			ptmodelprev->NEXT = ptmodelnext ;
			free_all_param(ptmodel) ;
            delht(ptmodel->HPARAM) ;
            freechain(ptmodel->origparams);
			mbkfree(ptmodel) ;
			return ;
		}

	}	

 if (ptmodel->NAME == namealloc(name))
	{
	ptmodelprev->NEXT = NULL ;
	free_all_param(ptmodel) ;
    delht(ptmodel->HPARAM) ;
    freechain(ptmodel->origparams);
	mbkfree(ptmodel) ;	
	return ;
	}
  else 
	{
      avt_errmsg(MCC_ERRMSG, "027", AVT_ERROR, name) ;
	return ;
	}
}   

/******************************************************************************\
FUNC : mcc_is_prevmod
\******************************************************************************/
int mcc_is_prevmod (char *technoname, char *transname, int transtype, 
                    int transcase, double L, double W, char *subckt ) 
{
 static char *oldtechnoname=NULL;
 static char *oldtransname=NULL;
 static int  oldtranstype=-1;
 static int  oldtranscase=-1;
 static char *oldsubckt=NULL;
 static long oldL=-1;
 static long oldW=-1;
 int res = 0;

 if ( ( oldtechnoname ) ) {
   if ( (!strcasecmp(technoname,oldtechnoname)) && 
        (!strcasecmp(transname,oldtransname)) &&
      ( transtype == oldtranstype ) && ( transcase == oldtranscase ) &&
      ( L == oldL ) && ( W == oldW) && oldsubckt==subckt) 
     res = 1;
 }
 if ( !res ) {
   if (oldtechnoname!=NULL) mbkfree(oldtechnoname);
   oldtechnoname = mbkstrdup(technoname);
   if (oldtransname!=NULL) mbkfree(oldtransname);
   oldtransname = mbkstrdup(transname);
   oldtranstype = transtype;
   oldtranscase = transcase;
   oldL = L;
   oldW = W;
   oldsubckt=subckt;
 }
 return res;
}

/******************************************************************************/
/* Obtention de model                                                         */
/******************************************************************************/
mcc_modellist *mcc_getmodel(char *technoname, char *modelname, int modeltype, 
                            int modelcase, double L, double W, int forceifsubckt) 
{ 
 mcc_technolist *pttechnofile ;
 mcc_modellist *ptmodel,*model_closest=NULL ;
 static mcc_modellist *prevmodel=NULL;
 int name_type;
 double LMIN,LMAX,WMIN,WMAX;
 float nfing=1 ;
 long lmax, lmin, wmax, wmin ;
 long l ;
 long w ;
 double error = 1000.0; // to minimise if we want the closest model
 double err,slmin,slmax,swmin,swmax;
 ptype_list *ptype;
 char *subcktname=NULL;
 char *newtechnoname;
 char *lotrsname = "transistor";
 int diode_flag=0;
 int status ;
 int errpolicy ;
 
 if (modeltype == MCC_DIODE)
 {
   diode_flag=1;
   MCC_CURRENT_LOTRS=NULL;
 }

/* Instance specific transistor parameters which influence model selection */
 if ( MCC_CURRENT_LOTRS ) {
   if (V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE ) {
     nfing = getlotrsparam (MCC_CURRENT_LOTRS,MBK_NF,NULL,&status);
     if (status != 1) nfing = 1;
   }
 }

 l = mcc_ftol((double)SCALE_X * L * 1e06) ;
 w = mcc_ftol((double)SCALE_X * (W/nfing) * 1e06) ;

 if ( MCC_CURRENT_LOTRS ) {
   if (( ptype = getptype (MCC_CURRENT_LOTRS->USER,TRANS_FIGURE) ))
        subcktname = (char*)ptype->DATA;
 }
 else
 {
   if (MCC_TN_SUBCKT!=NULL && modeltype == MCC_NMOS)
     subcktname=MCC_TN_SUBCKT;
   else if (MCC_TP_SUBCKT!=NULL && modeltype == MCC_PMOS)
     subcktname=MCC_TP_SUBCKT;
   else
     subcktname = MCC_CURRENT_SUBCKT;
 }
 
 if ((forceifsubckt & 2)==0 && mcc_is_prevmod (technoname,modelname,modeltype,modelcase,l,w,subcktname) && !(subcktname!=NULL && (forceifsubckt & 1)!=0))
   return prevmodel;

 newtechnoname = mcc_get_modelfilename ( modelcase );
 technoname = newtechnoname;

 if((pttechnofile = mcc_gettechno(technoname))) {
    if ( MCC_CURRENT_LOTRS ) {
      nfing = getlotrsparam (MCC_CURRENT_LOTRS,MBK_NF,NULL,&status);
      if (status != 1)
        nfing = 1;
    }
    for ( ptmodel = pttechnofile->MODEL ; ptmodel ; ptmodel=ptmodel->NEXT ) {
       name_type = 0;

 	    if ( (((mcc_cmpname(modelname, ptmodel->NAME)) || 
                /*(mcc_cmpname(modelname, ptmodel->SUBCKTNAME)) ||*/
                (modelname == NULL))) &&
                (ptmodel->TYPE == modeltype)
                && ((ptmodel->CASE == modelcase))) {
           if (subcktname==NULL && ptmodel->SUBCKTNAME!=NULL)
               continue;
           if ( subcktname ) {
             if ( !mcc_cmpname(subcktname, ptmodel->SUBCKTNAME) )
               continue;
           }
           name_type = 1;
           LMAX = mcc_getparam_quick(ptmodel, __MCC_QUICK_LMAX) ;
           LMIN = mcc_getparam_quick(ptmodel, __MCC_QUICK_LMIN) ;
           WMAX = mcc_getparam_quick(ptmodel, __MCC_QUICK_WMAX) ;
           WMIN = mcc_getparam_quick(ptmodel, __MCC_QUICK_WMIN) ;
           lmax = mcc_ftol((double)SCALE_X * LMAX * 1e06) ;
           lmin = mcc_ftol((double)SCALE_X * LMIN * 1e06) ;
           wmax = mcc_ftol((double)SCALE_X * WMAX * 1e06) ;
           wmin = mcc_ftol((double)SCALE_X * WMIN * 1e06) ;
           if(lmax == 0) lmax = MCC_LWMAX ;
           if(wmax == 0) wmax = MCC_LWMAX ;
           if ( ((l >= lmin) && (l < lmax) && (w >= wmin) && (w < wmax)) || diode_flag ) {
              if ((forceifsubckt & 2)==0) // just get model
              {
                if (ptmodel->MCSEED==0 || ptmodel->SUBCKTNAME!=NULL) // need reevaluate
                  mcc_evalmodel(ptmodel,L,W,modeltype) ;
                prevmodel = ptmodel;
              }
		      return (ptmodel) ;
           }
        }
        // try to get the closest model...
        if ( name_type ) {
          err = pow(LMAX-L,2.0) + pow (L-LMIN,2.0)+ pow(WMAX-W,2.0) + pow(W-WMIN,2.0);
          if ( err < error ) {
            model_closest = ptmodel;
            error = err;
            slmin = LMIN;
            slmax = LMAX;
            swmin = WMIN;
            swmax = WMAX;
          }
        }
    }
 }
 
 if ( MCC_CURRENT_LOTRS ) {
   if ( MCC_CURRENT_LOTRS->TRNAME )
     lotrsname = MCC_CURRENT_LOTRS->TRNAME;
 }
 
 if (!model_closest) 
   errpolicy = AVT_ERROR ;
 else
   errpolicy = AVT_WARNING ;

 if ( modelname ) {
   if ( !diode_flag )
      avt_errmsg(MCC_ERRMSG, "028", errpolicy, 
             modelname,(modelcase == elpBEST) ? "best" : (modelcase == elpWORST) ? "worst" : "typical",
             mcc_ftol(L*1e09),mcc_ftol(W*1e09),lotrsname) ;
   else 
     avt_errmsg(MCC_ERRMSG, "029", errpolicy, modelname);
 }
 else {
   if ( !diode_flag )
      avt_errmsg(MCC_ERRMSG, "028", errpolicy, 
             "unknown",(modelcase == elpBEST) ? "best" : (modelcase == elpWORST) ? "worst" : "typical",
             mcc_ftol(L*1e09),mcc_ftol(W*1e09), lotrsname) ;
 }

 prevmodel = model_closest;

 if( !model_closest )
   return NULL ;

 if (model_closest->MCSEED==0 || model_closest->SUBCKTNAME!=NULL) // need reevaluate
   mcc_evalmodel(model_closest,L,W,modeltype) ;

 avt_errmsg(MCC_ERRMSG, "030", AVT_WARNING, model_closest->NAME, mcc_ftol(slmin*1e09),mcc_ftol(slmax*1e09), mcc_ftol(swmin*1e09),mcc_ftol(swmax*1e09) ) ;

 return model_closest ;
}

/******************************************************************************/
/* recherce le model suivant                                                  */
/******************************************************************************/
mcc_modellist *mcc_getnextmodel(char *technoname, char *transname, int transtype, int transcase, mcc_modellist *model)
{
 mcc_modellist *ptmodel ;
 mcc_modellist *resmodel = NULL ;
 mcc_technolist *pttechnofile ;
 long lmax ;
 long lmin ;
 long wmax ;
 long wmin ;

 if(model == NULL)
  {
   lmax = (long)0 ;
   lmin = (long)0 ;
   wmax = (long)0 ;
   wmin = (long)0 ;
  }
 else
  {
   lmax = mcc_ftol(SCALE_X * mcc_getparam_quick(model, __MCC_QUICK_LMAX) * 1e06) ;
   lmin = mcc_ftol(SCALE_X * mcc_getparam_quick(model, __MCC_QUICK_LMIN) * 1e06) ;
   wmax = mcc_ftol(SCALE_X * mcc_getparam_quick(model, __MCC_QUICK_WMAX) * 1e06) ;
   wmin = mcc_ftol(SCALE_X * mcc_getparam_quick(model, __MCC_QUICK_WMIN) * 1e06) ;
  }

 if((pttechnofile = mcc_gettechno(technoname))) {
    ptmodel = pttechnofile->MODEL ;
    while(ptmodel) {
 	    if ( (mcc_cmpname(transname, ptmodel->NAME) || mcc_cmpname(transname, ptmodel->SUBCKTNAME)) && 
                 (ptmodel->TYPE == transtype) && 
                 (model != ptmodel) &&
                 ((ptmodel->CASE == transcase) ))
               {
                 if(lmin > mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_LMIN) * 1e06))
                   {
                    ptmodel = ptmodel->NEXT ;
                    continue ;
                   }
                 else if(lmin == mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_LMIN) * 1e06))
                   {
                     {
                      if(lmax > mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_LMAX) * 1e06))
                        {
                         ptmodel = ptmodel->NEXT ;
                         continue ;
                        }
                      else if(lmax == mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_LMAX) * 1e06))
                        {
                         if(wmin > mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_WMIN) * 1e06))
                           {
                            ptmodel = ptmodel->NEXT ;
                            continue ;
                           }
                         else if(wmin == mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_WMIN) * 1e06))
                           {
                            if(wmax > mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_WMAX) * 1e06))
                              {
                               ptmodel = ptmodel->NEXT ;
                               continue ;
                              }
                           }
                        }
                      }
                   }

                 if(resmodel == NULL)
                   {
                    resmodel = ptmodel ;
                   }
                 else
                   {
                    if(mcc_ftol(SCALE_X * mcc_getparam_quick(resmodel, __MCC_QUICK_LMIN) * 1e06) > 
                       mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_LMIN) * 1e06))
                      {
                       resmodel = ptmodel ;
                       ptmodel = ptmodel->NEXT ;
                       continue ;
                      }
                    else if(mcc_ftol(SCALE_X * mcc_getparam_quick(resmodel, __MCC_QUICK_LMIN) * 1e06) < 
                            mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_LMIN) * 1e06))
                      {
                       ptmodel = ptmodel->NEXT ;
                       continue ;
                      }
                    else if(mcc_ftol(SCALE_X * mcc_getparam_quick(resmodel, __MCC_QUICK_LMAX) * 1e06) > 
                            mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_LMAX) * 1e06))
                      {
                       resmodel = ptmodel ;
                       ptmodel = ptmodel->NEXT ;
                       continue ;
                      }
                    else if(mcc_ftol(SCALE_X * mcc_getparam_quick(resmodel, __MCC_QUICK_LMAX) * 1e06) < 
                            mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_LMAX) * 1e06))
                      {
                       ptmodel = ptmodel->NEXT ;
                       continue ;
                      }
                    else if(mcc_ftol(SCALE_X * mcc_getparam_quick(resmodel, __MCC_QUICK_WMIN) * 1e06) > 
                            mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_WMIN) * 1e06))
                      {
                       resmodel = ptmodel ;
                       ptmodel = ptmodel->NEXT ;
                       continue ;
                      }
                    else if(mcc_ftol(SCALE_X * mcc_getparam_quick(resmodel, __MCC_QUICK_WMIN) * 1e06) < 
                            mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_WMIN) * 1e06))
                      {
                       ptmodel = ptmodel->NEXT ;
                       continue ;
                      }
                    else if(mcc_ftol(SCALE_X * mcc_getparam_quick(resmodel, __MCC_QUICK_WMAX) * 1e06) > 
                            mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_WMAX) * 1e06))
                      {
                       resmodel = ptmodel ;
                       ptmodel = ptmodel->NEXT ;
                       continue ;
                      }
                    else if(mcc_ftol(SCALE_X * mcc_getparam_quick(resmodel, __MCC_QUICK_WMAX) * 1e06) < 
                            mcc_ftol(SCALE_X * mcc_getparam_quick(ptmodel, __MCC_QUICK_WMAX) * 1e06))
                      {
                       ptmodel = ptmodel->NEXT ;
                       continue ;
                      }
                   }
               }
        ptmodel = ptmodel->NEXT ;
        }
    }
 
 return (resmodel) ;
}

static int mcc_quick_param(char *name)
{
  unsigned int i;
  for (i=0; i<sizeof(mcc_translate_tab)/sizeof(*mcc_translate_tab); i++)
    {
      if (strcasecmp(name, mcc_translate_tab[i].name)==0) return mcc_translate_tab[i].index;
    }
  return -1;
}

void mcc_check_quick_param_namealloc()
{
  unsigned int i;
  static int done=0;

  if (!done)
    {
      for (i=0; i<sizeof(mcc_translate_tab)/sizeof(*mcc_translate_tab); i++)
        {
          mcc_translate_tab[i].name=namealloc(mcc_translate_tab[i].name);
        }
      done++;
    }
}

/******************************************************************************/
/* Ajout de parametre                                                         */
/******************************************************************************/
mcc_paramlist *mcc_addparam(mcc_modellist *ptmodel, char *name, double value, int typevalue)
{
 mcc_paramlist *ptparam ;
 char pt[1024] ;
 int idx;

 ptparam = (mcc_paramlist *)mbkalloc(sizeof(mcc_paramlist)) ;

 if (!ptmodel) {
	 fprintf(stderr, "\n[MCC ERR] Can't add parameter %s because no model!!!\n", name) ;
     return(NULL) ;
 }

 downstr(name,pt) ;
 ptparam->NEXT = ptmodel->PARAM ;
 ptparam->NAME = namealloc(pt) ;
 ptparam->VALUE = ptparam->MCVALUE = value ;
 ptparam->EXPR = NULL ;
 ptparam->MODEL = ptmodel ;
 ptparam->TYPEVALUE = typevalue ;
 ptparam->CONSTVALUE = 1 ;
 ptmodel->PARAM = ptparam ;
 addhtitem(ptmodel->HPARAM,ptparam->NAME,(long)ptparam) ;
 if ((idx= mcc_quick_param(ptparam->NAME))!=-1)
   ptmodel->QUICK[idx]=ptparam;

 return(ptmodel->PARAM) ;
}

mcc_paramlist *mcc_addparam_quick(mcc_modellist *ptmodel, int idx, char *name, double value, int typevalue)
{
 mcc_paramlist *ptparam ;

 ptparam = (mcc_paramlist *)mbkalloc(sizeof(mcc_paramlist)) ;

 if (!ptmodel) {
	 fprintf(stderr, "\n[MCC ERR] Can't add parameter %s because no model!!!\n", name) ;
     return(NULL) ;
 }

 ptparam->NEXT = ptmodel->PARAM ;
 ptparam->NAME = mcc_translate_tab[idx].name;
 ptparam->VALUE = ptparam->MCVALUE = value ;
 ptparam->EXPR = NULL ;
 ptparam->MODEL = ptmodel ;
 ptparam->TYPEVALUE = typevalue ;
 ptparam->CONSTVALUE = 1 ;
 ptmodel->PARAM = ptparam ;
 addhtitem(ptmodel->HPARAM,ptparam->NAME,(long)ptparam) ;
 ptmodel->QUICK[idx]=ptparam;

 return(ptmodel->PARAM) ;
}

/******************************************************************************/
/* Destruction de parametre                                                   */
/******************************************************************************/
void mcc_delparam(mcc_modellist *ptmodel, char *name)
{
 mcc_paramlist *ptparam ;
 mcc_paramlist *ptparamprev ;
 mcc_paramlist *ptparamnext ;
 int idx;

 if (ptmodel == NULL)
 {
   fprintf(stderr, "\n[MCC ERR] Can't del parameter %s because no model!\n", name) ;
   return ;
 }
 
 ptparam = ptmodel->PARAM ;

 if(ptparam == NULL)
     return ;

 if ((idx= mcc_quick_param(name))!=-1)
   ptmodel->QUICK[idx]=NULL;

 if (ptparam->NAME == namealloc(name))
   {
     ptparamnext = ptparam->NEXT ;
     ptmodel->PARAM = ptparamnext ;
     mcc_freeexp(ptparam->EXPR) ;
     delhtitem(ptmodel->HPARAM,ptparam->NAME) ;
     mbkfree(ptparam) ;
     return ;
   }
 else
   
   while (ptparam->NEXT != NULL)
     {
       ptparamprev = ptparam ;
       ptparam = ptparam->NEXT ;
       ptparamnext = ptparam->NEXT ;
       
       if (ptparam->NAME == namealloc(name))
         
         {
           ptparamprev->NEXT = ptparamnext ;
           mcc_freeexp(ptparam->EXPR) ;
           delhtitem(ptmodel->HPARAM,ptparam->NAME) ;
           mbkfree(ptparam) ;
           return ;
         }
       
     }
 
 if (ptparam->NAME == namealloc(name))
   {
     ptparamprev->NEXT = NULL ;
     mcc_freeexp(ptparam->EXPR) ;
     delhtitem(ptmodel->HPARAM,ptparam->NAME) ;
     mbkfree(ptparam) ;
     return ;
   }
 
 return ;
}

void mcc_eval_all_param(mcc_modellist *ptmodel);

/******************************************************************************\
FUNC: mcc_evalmodel

   evaluation d'un parametre en fonction de ses expression                    

   si le modtype est MCC_DIODE : L <=> area
                                 W <=> perimeter

\******************************************************************************/
void mcc_evalmodel(mcc_modellist *ptmodel, double L, double W, int modtype)
{
  mcc_technolist *pttechno ;
  mcc_explist *ptexp ;
  lofig_list *lofig ;
  //lotrs_list *lotrs ;
  optparam_list *optparams, *ptopt ;
  ptype_list *ptype ;
  double value;
  int trace = 0;
  int i, nbc=0, newdone, fromflatten;
  eqt_param *HIERPARAM[5];
  long oldseed;
  char errbuf[2048];

  pttechno = ptmodel->TECHNO ;

  eqt_term(mccEqtCtx);
  mcc_eqt_init();

  oldseed=eqt_get_current_srand_seed();

  if ( MCC_CURRENT_LOTRS && V_BOOL_TAB[__AVT_ENABLE_STAT].VALUE && ptmodel->SUBCKTNAME!=NULL)
    {
      if (( ptype = getptype (MCC_CURRENT_LOTRS->USER,MCC_LOTRS_SEED))!=NULL)
        eqt_srand((long)ptype->DATA);
      else
        MCC_CURRENT_LOTRS->USER=addptype(MCC_CURRENT_LOTRS->USER, MCC_LOTRS_SEED, (void *)oldseed);
    }

  fromflatten=(EQT_CONTEXT_HIERARCHY[0]!=NULL);

  // PARAMETRES GLOBAUX
  if (MBK_GLOBALPARAMS && !fromflatten)
    {
      EQT_CONTEXT_HIERARCHY[nbc]=eqt_init(32);
      HIERPARAM[nbc]=MBK_GLOBALPARAMS;
      eqt_import_vars (EQT_CONTEXT_HIERARCHY[nbc], MBK_GLOBALPARAMS);
      nbc++;
    }
  
  if (MBK_GLOBALFUNC) eqt_import_func(MBK_GLOBALFUNC, mccEqtCtx);

  if(ptmodel->SUBCKTNAME != NULL)
    {
      if((lofig = getloadedlofig(ptmodel->SUBCKTNAME)) != NULL)
        {
          if (!fromflatten)
          {
            // PARAMETRES D'INSTANCE + PARAM PAR DEFAUT
            EQT_CONTEXT_HIERARCHY[nbc]=eqt_init(32);
            ptype = getptype(lofig->USER,PARAM_CONTEXT) ;
            if(ptype != NULL)
              {
                eqt_import_vars (EQT_CONTEXT_HIERARCHY[nbc], (eqt_param *)ptype->DATA);
              }
            /* set L and W according to transistor */
            switch ( modtype ) {
            case MCC_NMOS:
            case MCC_PMOS:
              eqt_addvar(EQT_CONTEXT_HIERARCHY[nbc],namealloc("W"),W) ; 
              eqt_addvar(EQT_CONTEXT_HIERARCHY[nbc],namealloc("w"),W) ; 
              eqt_addvar(EQT_CONTEXT_HIERARCHY[nbc],namealloc("L"),L) ; 
              eqt_addvar(EQT_CONTEXT_HIERARCHY[nbc],namealloc("l"),L) ; 
              if ( avt_islog(2,LOGMCC) )
                {
                  trace = 1;
                  avt_log(LOGMCC,2,"\n");
                  avt_log(LOGMCC,2,
                          " -- Expressions evaluations for subckt %s type %s --\n\n",
                          ptmodel->SUBCKTNAME,(modtype==MCC_NMOS)?"NMOS":"PMOS");
                }
              break;
            case MCC_DIODE : 
              if ( L > 0.0 ) {
                eqt_addvar(EQT_CONTEXT_HIERARCHY[nbc],namealloc("AREA"),L) ; 
                eqt_addvar(EQT_CONTEXT_HIERARCHY[nbc],namealloc("area"),L) ; 
              }
              if ( W > 0.0 ) {
                eqt_addvar(EQT_CONTEXT_HIERARCHY[nbc],namealloc("PERIMETER"),W) ; 
                eqt_addvar(EQT_CONTEXT_HIERARCHY[nbc],namealloc("perimeter"),W) ; 
              }
              if ( avt_islog(2,LOGMCC) )
                {
                  trace = 1;
                  avt_log(LOGMCC,2,"\n");
                  avt_log(LOGMCC,2,
                          " -- Expressions evaluations for subckt %s type DIODE --\n\n",
                          ptmodel->SUBCKTNAME);
                }
              break;
            }
            if ( MCC_CURRENT_LOTRS )
              if( (ptype = getptype(MCC_CURRENT_LOTRS->USER,PARAM_CONTEXT)))
                {
                  eqt_import_vars(EQT_CONTEXT_HIERARCHY[nbc],(eqt_param *)ptype->DATA);
                }
            nbc++;
          }

          // EXPRESSIONS
          do 
            {
              newdone=0;
              if ((ptype = getptype(lofig->USER, OPT_PARAMS)))
                {
                  optparams = (optparam_list*)ptype->DATA;
                  for (ptopt = optparams; ptopt; ptopt = ptopt->NEXT)
                    {
                      if (ptopt->TAG == '$')
                        {
                          if (eqt_isdefined(mccEqtCtx, ptopt->UNAME.SPECIAL, 0)) continue;
                          value = eqt_eval (mccEqtCtx, ptopt->UDATA.EXPR, EQTFAST);
                          if (eqt_resistrue (mccEqtCtx))
                            {
                              newdone++;
                              eqt_addvar (mccEqtCtx, ptopt->UNAME.SPECIAL, value);
                              if (!isfinite(value))
                                avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, ptopt->UNAME.SPECIAL, ptopt->UDATA.EXPR," : returned NaN or Inf");
                              if ( trace )
                                {
                                  avt_log(LOGMCC,2," [OPT_PARAMS] opt_exp : %s=%s -> value = %g\n",
                                          ptopt->UNAME.SPECIAL,ptopt->UDATA.EXPR,value);
                                }
                            }
                        }
                      else
                        {
                          eqt_addvar (mccEqtCtx, ptopt->UNAME.SPECIAL, ptopt->UDATA.VALUE);
                          if ( trace )
                            {
                              avt_log(LOGMCC,2," [OPT_PARAMS] opt_exp (special) : %s -> value = %g\n",
                                      ptopt->UNAME.SPECIAL,ptopt->UDATA.VALUE);
                            }
                        }
                    }
                }
             
              ptype = getptype(lofig->USER,MCC_MODEL_EXPR) ;
              if(ptype != NULL)
                {
                  for(ptexp = (mcc_explist *)ptype->DATA ; ptexp != NULL ; ptexp = ptexp->NEXT)
                    {
                      if(ptexp->EXPR == NULL)
                        {
                          eqt_addvar(mccEqtCtx,ptexp->NAME,ptexp->DEFAULT) ; 
                          value = ptexp->DEFAULT;
                        }
                      else
                        {
                          if (eqt_isdefined(mccEqtCtx, ptexp->NAME, 0)) continue;
                          if (ptexp->EXPR_TO_TREE==NULL) ptexp->EXPR_TO_TREE=eqt_create (mccEqtCtx, ptexp->EXPR);
                          value = eqt_calcval (mccEqtCtx, ptexp->EXPR_TO_TREE);
                          //value = eqt_eval(mccEqtCtx,ptexp->EXPR,EQTFAST);
                          if (eqt_resistrue (mccEqtCtx))
                            {
                              newdone++;
                              eqt_addvar(mccEqtCtx,ptexp->NAME, value);
                              if (!isfinite(value))
                                avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, ptexp->NAME, ptexp->EXPR," : returned NaN or Inf");
                              if ( trace )
                                {
                                  avt_log(LOGMCC,2," MCC_DEFMODEL_EXPR : exp : %s=%s -> value = %g\n",
                                          ptexp->NAME,ptexp->EXPR,value);
                                }
                            }
                        }
                    }
                }
            } while (newdone>0);

          // ERROR CHECKING
          if ((ptype = getptype(lofig->USER, OPT_PARAMS)))
            {
              optparams = (optparam_list*)ptype->DATA;
              for (ptopt = optparams; ptopt; ptopt = ptopt->NEXT)
                {
                  if (ptopt->TAG == '$')
                    {
                      if (!eqt_isdefined(mccEqtCtx, ptopt->UNAME.SPECIAL, 0))
                        {
                          mcc_set_var_err_msg(mccEqtCtx, ptopt->UDATA.EXPR, errbuf);
                          avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, ptopt->UNAME.SPECIAL, ptopt->UDATA.EXPR, errbuf);
                          value = 0.0;
                          eqt_addvar(mccEqtCtx,ptopt->UNAME.SPECIAL, value);
                        }
                    }
                }
            }
          ptype = getptype(lofig->USER,MCC_MODEL_EXPR) ;
          if(ptype != NULL)
            {
              for(ptexp = (mcc_explist *)ptype->DATA ; ptexp != NULL ; ptexp = ptexp->NEXT)
                {
                  if(ptexp->EXPR != NULL)
                    {
                      if (!eqt_isdefined(mccEqtCtx, ptexp->NAME, 0))
                        {
                          mcc_set_var_err_msg(mccEqtCtx, ptexp->EXPR, errbuf);
                          avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, ptexp->NAME, ptexp->EXPR, errbuf);
                          value = 0.0;
                          eqt_addvar(mccEqtCtx,ptexp->NAME, value);
                        }
                    }
                }
            }
        }
    }
  
  // building final context
  for (i=nbc-1; i>=0; i--)
    {
      eqt_param *ep;
      ep=eqt_export_vars  (EQT_CONTEXT_HIERARCHY[i]);
      eqt_import_vars(mccEqtCtx,ep);
      eqt_free_param(ep);
      if (!fromflatten)
      {
        eqt_term(EQT_CONTEXT_HIERARCHY[i]);
        EQT_CONTEXT_HIERARCHY[i]=NULL; 
      }
    }
  mcc_initmodel(ptmodel);
  eqt_srand(oldseed);
  if (!fromflatten/* && MBK_MONTE_MODEL_PARAMS!=NULL*/) mcc_eval_all_param(ptmodel);
}

/******************************************************************************/
/* evaluation d'un parametre en fonction de ses expression                    */
/******************************************************************************/
double mcc_evalparam_sub(mcc_paramlist *param)
{
 mcc_explist *ptexp ;
 double res ;
 char errbuf[2048];

 if(param->EXPR == NULL)
   res = param->VALUE,  param->CONSTVALUE=1;

 for(ptexp = param->EXPR ; ptexp ; ptexp = ptexp->NEXT)
  {
   if(ptexp->EXPR == NULL)
    {
     eqt_addvar(mccEqtCtx,ptexp->NAME,ptexp->DEFAULT) ; 
     res = ptexp->DEFAULT, param->CONSTVALUE=1;
    }
   else
    {
     if (ptexp->EXPR_TO_TREE==NULL) ptexp->EXPR_TO_TREE=eqt_create (mccEqtCtx, ptexp->EXPR);
     res = eqt_calcval (mccEqtCtx, ptexp->EXPR_TO_TREE);
     if (!eqt_resistrue(mccEqtCtx))
       {
         mcc_set_var_err_msg(mccEqtCtx, ptexp->EXPR, errbuf);
         avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, ptexp->NAME, ptexp->EXPR, errbuf);
       }
     else if (!isfinite(res))
       avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, ptexp->NAME, ptexp->EXPR," : returned NaN or Inf");
//     res = eqt_eval(mccEqtCtx,ptexp->EXPR,EQTFAST) ;
     eqt_addvar(mccEqtCtx,ptexp->NAME,res) ;
     /*if (eqt_resisrandom(mccEqtCtx) || eqt_var_involved(mccEqtCtx)) */
     param->CONSTVALUE=0;
     param->VALUE=res;
    }
  }

 return(res) ;
}


/*
double nom_dev(char *var)
{
  double val=0;
  if (MCC_CURRENT_LOTRS)
    val=eqt_getvar(MCC_MC_NOM_VALUES, var);
  return val;
}
*/

void mcc_eval_all_param(mcc_modellist *ptmodel)
{
 mcc_paramlist *param;
 eqt_ctx *nom_values;
 long oldseed;
 int justseed=0;
 char errbuf[2048];

 nom_values=eqt_init(EQT_NB_VARS) ;

 oldseed=eqt_get_current_srand_seed();
 if (ptmodel->MCSEED==0)
   {
     ptmodel->MCSEED=oldseed;
     if (ptmodel->MCSEED==0) ptmodel->MCSEED=1;
     justseed=1;
   }

 eqt_srand(ptmodel->MCSEED);
 for (param=ptmodel->PARAM; param!=NULL; param=param->NEXT)
   {
     eqt_addvar(nom_values, param->NAME, param->MCVALUE=mcc_evalparam_sub(param));    
   }
 
 if (MBK_MONTE_MODEL_PARAMS!=NULL)
 {
   eqt_ctx *saveEQT_CONTEXT_HIERARCHY[5];
   int nbc=0;
   char *mckey;
   long l;
   ptype_list *pt;
   ht *quick;

   if (justseed || ptmodel->SUBCKTNAME!=NULL)
     {
       mckey=mbk_montecarlo_key(ptmodel->NAME, ptmodel->SUBCKTNAME, ptmodel->TYPE, 'G'); 
       if ((l=gethtitem(MBK_MONTE_MODEL_PARAMS, mckey))!=EMPTYHT)
         {
           quick=addht(30);
           for (pt=(ptype_list *)l; pt!=NULL; pt=pt->NEXT)
             addhtitem(quick, (void *)pt->TYPE, (long)pt->DATA);

           memcpy(saveEQT_CONTEXT_HIERARCHY, EQT_CONTEXT_HIERARCHY, sizeof(EQT_CONTEXT_HIERARCHY));
           if (MBK_GLOBALPARAMS)
             {
               EQT_CONTEXT_HIERARCHY[nbc]=eqt_init(32);
               eqt_import_vars (EQT_CONTEXT_HIERARCHY[nbc], MBK_GLOBALPARAMS);
               nbc++;
             }
           EQT_CONTEXT_HIERARCHY[nbc++]=NULL;
           MCC_MC_NOM_VALUES=nom_values;
             
           for (param=ptmodel->PARAM; param!=NULL; param=param->NEXT)
             {
               if ((l=gethtitem(quick, param->NAME))!=EMPTYHT)
                 {
                   param->MCVALUE=eqt_eval(mccMCPARAMctx, (char *)l, EQTFAST);
                   param->CONSTVALUE=0;
                   if (!eqt_resistrue(mccMCPARAMctx))
                     {
                       mcc_set_var_err_msg(mccMCPARAMctx, (char *)l, errbuf);
                       avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, param->NAME, (char *)l, errbuf);
                     }
                   else if (!isfinite(param->MCVALUE))
                     avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, param->NAME, (char *)l," : returned NaN or Inf");
                 }
             }

           MCC_MC_NOM_VALUES=NULL;
           if (MBK_GLOBALPARAMS)
             eqt_term(EQT_CONTEXT_HIERARCHY[0]);
           memcpy(EQT_CONTEXT_HIERARCHY, saveEQT_CONTEXT_HIERARCHY, sizeof(EQT_CONTEXT_HIERARCHY));
           delht(quick);
         }
     }
 }

 eqt_srand(oldseed);
 eqt_term(nom_values);
}

double mcc_evalparam(mcc_paramlist *param)
{
  double val;
  if (param->MODEL->MCSEED!=0)
    {
//      double oldval;
      val=param->MCVALUE;
/*      oldval=mcc_evalparam_sub(param);
      if (val!=oldval)
        printf("%s: %g != %g\n", param->NAME, val, oldval);*/
      return val;
    }
  
  return mcc_evalparam_sub(param);
}

/******************************************************************************/
/* Obtention de parametre : fonction qui retourne la value du param d 1 modele*/
/******************************************************************************/
double mcc_getparam(mcc_modellist *ptmodel, char *name)
{
 mcc_paramlist *ptparam ;
 char ptx[1024], *pt ;

 downstr(name,ptx) ;
 pt = namealloc(ptx) ;
 ptparam = (mcc_paramlist *)gethtitem(ptmodel->HPARAM,pt) ;
 if((ptparam != (mcc_paramlist *)EMPTYHT) && 
     (ptparam != (mcc_paramlist *)DELETEHT))
     return(mcc_evalparam(ptparam)) ;

 return (0.0) ;
}

double mcc_getparam_quick(mcc_modellist *ptmodel, int idx)
{
 if (idx>=__MCC_QUICK_LAST_ITEM) exit(6);

 if (ptmodel->QUICK[idx]==NULL) {  return 0; }

 return(mcc_evalparam(ptmodel->QUICK[idx]));
}
/******************************************************************************/
/* Obtention de parametre : fonction qui retourne la value du param d 1 modele*/
/******************************************************************************/
char *mcc_getparamexp(mcc_modellist *ptmodel, char *name)
{
 mcc_paramlist *ptparam ;
 char *pt, ptx[1024] ;

 downstr(name,ptx) ;
 pt = namealloc(ptx) ;
 ptparam = (mcc_paramlist *)gethtitem(ptmodel->HPARAM,pt) ;
 if((ptparam != (mcc_paramlist *)EMPTYHT) && 
     (ptparam != (mcc_paramlist *)DELETEHT))
    {
     if(ptparam->EXPR != NULL)
       return ptparam->EXPR->EXPR;
//       return(mbkstrdup(ptparam->EXPR->EXPR)) ;
    }

 return (NULL) ;
}
char *mcc_getparamexp_quick(mcc_modellist *ptmodel, int idx)
{
 mcc_paramlist *ptparam ;

 ptparam = mcc_chrparam_quick(ptmodel, idx);
 if(ptparam!=NULL)
   {
     if(ptparam->EXPR != NULL)
       return ptparam->EXPR->EXPR;
   }

 return (NULL) ;
}

/******************************************************************************/
/* Obtention de parametre : fonction qui retourne la value du param d 1 modele*/
/******************************************************************************/
mcc_paramlist *mcc_chrparam(mcc_modellist *ptmodel, char *name)
{
 mcc_paramlist *ptparam ;
 char *pt, ptx[1024] ;

 downstr(name,ptx) ;
 pt = namealloc(ptx) ;
 ptparam = (mcc_paramlist *)gethtitem(ptmodel->HPARAM,pt) ;
 if((ptparam != (mcc_paramlist *)EMPTYHT) && 
     (ptparam != (mcc_paramlist *)DELETEHT))
     return(ptparam) ;

 return (NULL) ;
}

mcc_paramlist *mcc_chrparam_quick(mcc_modellist *ptmodel, int idx)
{
  if (idx>=__MCC_QUICK_LAST_ITEM) exit(16);

  if (ptmodel->QUICK[idx]==NULL) return NULL;

  return ptmodel->QUICK[idx];
}

/******************************************************************************/
/* Obtention de parametre devant etre compatible pour tous les simulateurs    */
/******************************************************************************/
double mcc_getprm(mcc_modellist *ptmodel, char *name)
{
 char *pt,pt2[1024] ;
 double param_value = 0.0 ;

 upstr(name,pt2) ;
 pt = namealloc(pt2) ;

 if (pt == namealloc("VTH0")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VTH0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VTH0) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VTHO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VTHO) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VTO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VTO) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VT0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VT0) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VTH0) ;
 }
 else if (pt == namealloc("DELVT0") || pt == namealloc ("DELVTO")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DELVTO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELVTO) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DELVT0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELVT0) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELVT0) ; 
 }
 else if(pt == namealloc("PTA") || pt == namealloc ("TPB")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_PTA) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_PTA) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_TPB) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPB) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_PTA) ; 
 }
 else if (pt == namealloc("CTA") || pt == namealloc ("TCJ")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CTA) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTA) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_TCJ) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TCJ) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTA) ; 
 }
 else if(pt == namealloc("CTP") || pt == namealloc ("TCJSW")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CTP) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTP) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_TCJSW) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TCJSW) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTP) ; 
 }
 else if(pt == namealloc("VERSION") || pt == namealloc ("VER")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VERSION) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VERSION) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VER) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VER) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VERSION) ; 
 }
 else if(pt == namealloc("XL") || pt == namealloc ("LVAR")) {
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_XL) == MCC_SETVALUE)
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL) ;
    else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_LVAR) == MCC_SETVALUE)
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_LVAR) ;
    else
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL) ;
 }
 else if(pt == namealloc("XW") || pt == namealloc ("WVAR")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_XW) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_WVAR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_WVAR) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW) ; 
 }
 else if(pt == namealloc("LD") || pt == namealloc ("LAP")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_LD) == MCC_SETVALUE)
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_LD) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_LAP) == MCC_SETVALUE)
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_LAP) ;
     else
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_LD) ;
 }
 else if(pt == namealloc("WD") || pt == namealloc ("WOT")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_WD) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_WD) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_WOT) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_WOT) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_WD) ;
 }
 else if(pt == namealloc("NCH") || pt == namealloc ("NPEAK")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NCH) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_NCH)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NPEAK) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_NPEAK)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_NCH)    ;
 }
 else if(pt == namealloc("CGDO")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGD0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGD0)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSO)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGS0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGS0)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_COL) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_COL)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJGR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGR)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJGATE) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGATE)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO)    ;
 }
 else if(pt == namealloc("CGSO")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSO)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGS0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGS0)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGD0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGD0)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_COL) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_COL)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJGR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGR)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJGATE) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGATE)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO)    ;
 }
 else if(pt == namealloc("CJ") || pt == namealloc ("CJBR")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJ) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJ)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJBR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJBR)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJ)    ;
 }
 else if(pt == namealloc("CJSW") || pt == namealloc ("CJSR")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJSW) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSW)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJSR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSR)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSW)  ;
 }
 else if(pt == namealloc("CGDL") || pt == namealloc ("CGD1")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDL) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDL)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGD1) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGD1)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)  ;
 }
 else if(pt == namealloc("CGSL") || pt == namealloc ("CGS1")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSL) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGS1) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGS1)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)  ;
 }
 else if(pt == namealloc("DELTVT0") || pt == namealloc ("DELTVTO")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DELTVT0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELTVT0)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DELTVTO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELTVTO)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELTVT0)  ;
 }
 else if(pt == namealloc("TPBSW") || pt == namealloc ("PTP")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_TPBSW) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPBSW)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_PTP) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_PTP)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPBSW)  ;
 }

 return(param_value) ;
}
double mcc_getprm_quick(mcc_modellist *ptmodel, int pt)
{
 double param_value = 0.0 ;


 if (pt == __MCC_GETPRM_VTH0) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VTH0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VTH0) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VTHO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VTHO) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VTO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VTO) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VT0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VT0) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VTH0) ;
 }
 else if (pt == __MCC_GETPRM_DELVTO) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DELVTO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELVTO) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DELVT0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELVT0) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELVT0) ; 
 }
 else if(pt == __MCC_GETPRM_PTA) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_PTA) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_PTA) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_TPB) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPB) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_PTA) ; 
 }
 else if (pt == __MCC_GETPRM_CTA) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CTA) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTA) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_TCJ) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TCJ) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTA) ; 
 }
 else if(pt == __MCC_GETPRM_CTP) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CTP) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTP) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_TCJSW) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TCJSW) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTP) ; 
 }
/* else if(pt == namealloc("VERSION") || pt == namealloc ("VER")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VERSION) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VERSION) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VER) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VER) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_VERSION) ; 
 }*/
 else if(pt == __MCC_GETPRM_XL) {
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_XL) == MCC_SETVALUE)
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL) ;
    else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_LVAR) == MCC_SETVALUE)
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_LVAR) ;
    else
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL) ;
 }
 else if(pt == __MCC_GETPRM_XW) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_XW) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_WVAR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_WVAR) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW) ; 
 }
 else if(pt == __MCC_GETPRM_LD) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_LD) == MCC_SETVALUE)
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_LD) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_LAP) == MCC_SETVALUE)
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_LAP) ;
     else
        param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_LD) ;
 }
 else if(pt == __MCC_GETPRM_WD) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_WD) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_WD) ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_WOT) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_WOT) ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_WD) ;
 }
/* else if(pt == namealloc("NCH") || pt == namealloc ("NPEAK")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NCH) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_NCH)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NPEAK) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_NPEAK)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_NCH)    ;
 }*/
 else if(pt == __MCC_GETPRM_CGDO) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGD0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGD0)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSO)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGS0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGS0)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_COL) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_COL)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJGR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGR)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJGATE) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGATE)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO)    ;
 }
/* else if(pt == namealloc("CGSO")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSO)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGS0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGS0)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGD0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGD0)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_COL) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_COL)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJGR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGR)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJGATE) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGATE)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO)    ;
 }
 else if(pt == namealloc("CJ") || pt == namealloc ("CJBR")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJ) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJ)    ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJBR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJBR)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJ)    ;
 }
 else if(pt == namealloc("CJSW") || pt == namealloc ("CJSR")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJSW) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSW)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CJSR) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSR)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSW)  ;
 }*/
 else if(pt == __MCC_GETPRM_CGDL) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDL) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDL)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGD1) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGD1)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)  ;
 }
 /*
 else if(pt == namealloc("CGSL") || pt == namealloc ("CGS1")) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSL) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGS1) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGS1)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)  ;
 }*/
 /* en double
 else if(pt == __MCC_GETPRM_DELVTO) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DELTVT0) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELTVT0)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DELTVTO) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELTVTO)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELTVT0)  ;
 }*/
 else if(pt == __MCC_GETPRM_PTP) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_TPBSW) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPBSW)  ;
     else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_PTP) == MCC_SETVALUE)
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_PTP)  ;
     else
         param_value = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPBSW)  ;
 }
 else exit(50);

 return(param_value) ;
}

/******************************************************************************/
/* Remplacement de la valeur par defaut d'un parametre ou simple addparam     */
/******************************************************************************/
void mcc_setparam(mcc_modellist *ptmodel, char *name, double value)
{
 mcc_paramlist *ptparam ;
 char *pt ;
 char ptx[1024] ;

 downstr(name,ptx) ;
 pt = namealloc(ptx) ;
 ptparam = (mcc_paramlist *)gethtitem(ptmodel->HPARAM,pt) ;
 if((ptparam != (mcc_paramlist *)EMPTYHT) && 
     (ptparam != (mcc_paramlist *)DELETEHT))
   {
     ptparam->VALUE = ptparam->MCVALUE = value ;
     ptparam->TYPEVALUE = MCC_SETVALUE ;
     ptparam->CONSTVALUE = 1 ;
   }
 else
   {
    mcc_addparam(ptmodel, name, value, MCC_SETVALUE) ;
   }
}
void mcc_setparam_quick(mcc_modellist *ptmodel, int idx, char *name, double value)
{
 mcc_paramlist *ptparam ;

 ptparam=mcc_chrparam_quick(ptmodel, idx);
 if(ptparam)
   {
     ptparam->VALUE = ptparam->MCVALUE = value ;
     ptparam->TYPEVALUE = MCC_SETVALUE ;
     ptparam->CONSTVALUE=1 ;
   }
 else
   {
     mcc_addparam_quick(ptmodel, idx, name, value, MCC_SETVALUE) ;
   }
}

/******************************************************************************/
/* initialise un parametre si il n'existe pas                                 */
/******************************************************************************/
void mcc_initparam(mcc_modellist *ptmodel, char *name, double value, char *exp, int typevalue)
{
 mcc_paramlist *ptparam ;
 char pt[1024] ;

 downstr(name,pt) ;

 ptparam = mcc_chrparam(ptmodel,pt) ;

 if(ptparam == NULL)
 { 
  ptparam = mcc_addparam(ptmodel, pt, value, typevalue) ;

  if(exp != NULL)
     ptparam->EXPR = mcc_addexp(ptparam->EXPR,ptparam->NAME,exp,value) ;
 }
}
void mcc_initparam_quick(mcc_modellist *ptmodel, int idx, double value, char *exp, int typevalue)
{
 mcc_paramlist *ptparam ;

 ptparam = mcc_chrparam_quick(ptmodel,idx) ;

 if(ptparam == NULL)
 { 
  ptparam = mcc_addparam_quick(ptmodel, idx, NULL, value, typevalue) ;

  if(exp != NULL)
     ptparam->EXPR = mcc_addexp(ptparam->EXPR,ptparam->NAME,exp,value) ;
 }
}

/******************************************************************************/
/* Remplacement de la valeur par defaut d'un parametre ou simple addparam     */
/******************************************************************************/
int mcc_getparamtype(mcc_modellist *ptmodel, char *name)
{
 mcc_paramlist *ptparam ;

 ptparam = mcc_chrparam(ptmodel,name) ;

 if(ptparam == NULL)
   return MCC_INITVALUE;
 else
   return ptparam->TYPEVALUE;
}
int mcc_getparamtype_quick(mcc_modellist *ptmodel, int idx)
{
 mcc_paramlist *ptparam ;

 ptparam = mcc_chrparam_quick(ptmodel,idx) ;

 if(ptparam == NULL)
   return MCC_INITVALUE;
 else
   return ptparam->TYPEVALUE;
}

/******************************************************************************/
/* Driver de models dans stdout                                               */ 
/******************************************************************************/
void mcc_drvmod()
{
 mcc_modellist *ptmodel ;
 mcc_paramlist *param ;
 mcc_technolist *pttechnolist ;
 mcc_explist *ptexp ;
 lofig_list *lofig ;
 lotrs_list *lotrs ;
 ptype_list *ptype ;
 char *transtype ;
 char *transcase ;
 char *modeltype ;
 int need_eval = 0;
 double area = 5.100E-13;

 if(!MCC_HEADTECHNO) {
     fprintf(stderr, "[MCC ERR] Can't drive technolist because empty!!!\n") ;
     return ;
 }

 if (MBK_GLOBALPARAMS) eqt_import_vars (mccEqtCtx, MBK_GLOBALPARAMS);
 if (MBK_GLOBALFUNC) eqt_import_func(MBK_GLOBALFUNC, mccEqtCtx);

 for(pttechnolist = MCC_HEADTECHNO ; pttechnolist ; pttechnolist = pttechnolist->NEXT) {
       fprintf(stdout, "* Technofile : %s\n", pttechnolist->NAME) ;

       ptmodel = pttechnolist->MODEL ;
  
       while (ptmodel) {
         if ( need_eval ) {
           if ( ptmodel->TYPE == MCC_DIODE ) 
             mcc_evalmodel(ptmodel,area,0.0,ptmodel->TYPE) ;
           else
             mcc_evalmodel(ptmodel,mcc_getparam_quick(ptmodel,__MCC_QUICK_LMIN),mcc_getparam_quick(ptmodel,__MCC_QUICK_WMIN),ptmodel->TYPE) ;
         }
           if ((ptmodel->MODELTYPE == MCC_NOMODELTYPE) || 
                   (ptmodel->MODELTYPE == MCC_NOMODEL) || 
                   (ptmodel->MODELTYPE == MCC_MOS2))
               modeltype = "MOS2" ;
           else if (ptmodel->MODELTYPE == MCC_BSIM3V3)
               modeltype = "BSIM3V3" ;
           else if (ptmodel->MODELTYPE == MCC_MM9)
               modeltype = "MM9" ;
           else if (ptmodel->MODELTYPE == MCC_BSIM4)
               modeltype = "BSIM4" ;
           else if (ptmodel->MODELTYPE == MCC_MPSP)
               modeltype = "PSP" ;
           else if (ptmodel->MODELTYPE == MCC_MPSPB)
               modeltype = "PSPB" ;
           else if (ptmodel->MODELTYPE == MCC_EXTMOD)
               modeltype = "EXT" ;
           
           fprintf(stdout, "* Model type : %s\n\n", modeltype) ;

           if(ptmodel->SUBCKTNAME != NULL)
             {
              lofig = getloadedlofig(ptmodel->SUBCKTNAME) ;
              if(lofig != NULL)
               {
                viewlofig(lofig) ;
                if((ptype = getptype(lofig->USER,MCC_DEFMODEL_EXPR)) != NULL)
                  for(ptexp = (mcc_explist *)ptype->DATA ; ptexp != NULL ; ptexp = ptexp->NEXT)
                   {
                    if(ptexp->EXPR != NULL)
                     fprintf(stdout, "param  : %s = %s : %g\n",ptexp->NAME,ptexp->EXPR,ptexp->DEFAULT) ;
                    else
                     fprintf(stdout, "param  : %s = %g\n",ptexp->NAME,ptexp->DEFAULT) ;               }
                if((ptype = getptype(lofig->USER,MCC_MODEL_EXPR)) != NULL)
                  for(ptexp = (mcc_explist *)ptype->DATA ; ptexp != NULL ; ptexp = ptexp->NEXT)
                   {
                    if(ptexp->EXPR != NULL)
                     fprintf(stdout, "param %s = %s : %g\n",ptexp->NAME,ptexp->EXPR,ptexp->DEFAULT) ;
                    else
                     fprintf(stdout, "param %s = %g\n",ptexp->NAME,ptexp->DEFAULT) ;               }
                for(lotrs = lofig->LOTRS ; lotrs != NULL ; lotrs = lotrs->NEXT)
                 {
                  fprintf(stdout, "Transistor %s\n",lotrs->TRNAME) ;
                  if((ptype = getptype(lotrs->USER,MCC_MODEL_EXPR)) != NULL)
                    for(ptexp = (mcc_explist *)ptype->DATA ; ptexp != NULL ; ptexp = ptexp->NEXT)
                     {
                      if(ptexp->EXPR != NULL)
                       fprintf(stdout, "param %s = %s : %g\n",ptexp->NAME,ptexp->EXPR,ptexp->DEFAULT) ;
                      else
                       fprintf(stdout, "param %s = %g\n",ptexp->NAME,ptexp->DEFAULT) ;
                     }
                 }
               }
             }
           
           if(ptmodel->TYPE == MCC_NMOS)
               transtype = "NMOS" ;
           else if(ptmodel->TYPE == MCC_PMOS)
               transtype = "PMOS" ;
           else if(ptmodel->TYPE == MCC_DIODE)
               transtype = "Diode" ;
           else 
               transtype = "UNKNOWN" ;
           if(ptmodel->CASE == MCC_BEST)
               transcase = "BEST" ;
           else if (ptmodel->CASE == MCC_WORST)
               transcase = "WORST" ;
           else
               transcase = "TYPICAL" ;
           
             if(ptmodel->SUBCKTNAME != NULL)
             fprintf(stdout, ".SUBCKT %s\n", ptmodel->SUBCKTNAME) ;
             fprintf(stdout, ".MODEL %s %s $ %s\n", ptmodel->NAME, transtype, transcase) ;
             param = ptmodel->PARAM ;
 	         while (param){
                 if(param->EXPR == NULL)
                    fprintf(stdout, "+%s = %g  $ TYPEVALUE: %d\n", param->NAME, 
                                                           param->VALUE,
                                                           param->TYPEVALUE) ;
                  else
                    fprintf(stdout, "+%s = %g  $ : %s %g TYPEVALUE: %d\n", param->NAME,
                                                           mcc_getparam(ptmodel,param->NAME),
                                                           param->EXPR->EXPR,param->VALUE,
                                                           param->TYPEVALUE) ;
                    param =  param->NEXT ;
             }
	  fprintf(stdout, "*\n") ;
	  ptmodel = ptmodel->NEXT ;
      }
 }
}

/******************************************************************************/
/* Ajout de fichier technologique                                             */
/******************************************************************************/
mcc_technolist *mcc_addtechno(char *technoname)
{
 mcc_technolist *pttechnofile ;

 if ( !(pttechnofile = mcc_gettechno(technoname)) ) {
   pttechnofile = (mcc_technolist *)mbkalloc(sizeof(mcc_technolist)) ;
  
   pttechnofile->NEXT = MCC_HEADTECHNO ;
   pttechnofile->NAME = namealloc(technoname) ;
   pttechnofile->EXPR = NULL ;
   pttechnofile->MODEL = NULL ;
  
   MCC_HEADTECHNO = pttechnofile ;
  
   mcc_eqt_init();
 }

 return pttechnofile ;
}

/******************************************************************************/
/* Obtention de fichier technologique                                         */
/******************************************************************************/
mcc_technolist *mcc_gettechno(char *technoname)
{
    mcc_technolist *pttechnofile ;

    for(pttechnofile = MCC_HEADTECHNO ; pttechnofile ; pttechnofile = pttechnofile->NEXT ) {
        if((pttechnofile->NAME) == namealloc(technoname))
            return(pttechnofile) ;
    }
    return(NULL) ;
}

/******************************************************************************/
static void mcc_eqt_init()
{
    mccEqtCtx = eqt_init(EQT_NB_VARS) ;
    eqt_add_spice_extension(mccEqtCtx);
}

/******************************************************************************/
/* Parser de fichier technologique                                            */
/* Il parse le technofile puis il construit la liste mcc_modellist            */
/******************************************************************************/
int mcc_parserfile(char *technoname)
{
    int res_load = 0 ;

    if((mcc_modin = mbkfopen(technoname, NULL,READ_TEXT))) {
        mcc_addtechno(technoname) ;
        yyinit(technoname) ;
        if(!(res_load = mcc_modparse())) {
            fclose(mcc_modin) ;
        }
        else {
            fprintf(stderr, "[MCC ERR] error while parsing file: %s!!!\n", technoname) ;
            fclose(mcc_modin) ;
        }
    }
    else {
        fprintf(stderr, "[MCC ERR] can't open file %s to parse!!!\n", technoname) ;
        EXIT(1);
    }
    
    return(res_load) ;
}

/******************************************************************************/
/* COMPARE les noms des transistors                                           */ 
/* retourne 1 si les noms sont identiques en enlenvant les eventuels index    */
/* sinon retourne 0                                                           */
/******************************************************************************/
int mcc_cmpname(char *name, char *nameindx) 
{
    int rescmp = 0 ;
    char *namecut ;

    if((!name) || (!nameindx))
        return(0) ;

    namecut = mcc_cutname(nameindx) ;
    if(namealloc(name) == namealloc(namecut))
        rescmp = 1 ;
    else 
        rescmp = 0 ;

    return rescmp ;
}

/******************************************************************************/
/* Fonction qui renvoi un nom de model sans son index                         */
/******************************************************************************/
char *mcc_cutname(char *name)
{
 static char pt[1024] ;
 unsigned int i ;
 
 sprintf(pt,"%s", name) ;
    
 for(i = 0 ; i < strlen(pt) ; i++ ) {
    if (pt[i] == MCC_MOD_SEPAR)
        pt[i] = '\0' ;
 }

 return pt ;
}

/******************************************************************************/
/*                                                                            */
/* FUNCTION : mcc_getsubckt                                                   */
/*                                                                            */
/* Retourne le nom du subckt d'un modele                                      */
/*                                                                            */
/******************************************************************************/
char *mcc_getsubckt( char *technoname, char *transname, int transtype, 
                     int transcase, double L, double W) 
{
    mcc_modellist  *ptmodel ;
    
    ptmodel = mcc_getmodel(technoname,transname,transtype,transcase,L,W,2) ; // just get the model
    if (ptmodel)
        return (ptmodel->SUBCKTNAME) ;
    else
        return NULL ;
}

/******************************************************************************/
/*                                                                            */
/* FUNCTION : mcc_setsubcktsize                                               */
/*                                                                            */
/* Retourne le nom du subckt d'un modele                                      */
/*                                                                            */
/******************************************************************************/
int mcc_setsubcktsize( mcc_modellist  *ptmodel,int transtype,double L,double W) 
{
    lofig_list *lofig ;
    lotrs_list *lotrs ;
    char type ;
    int res = 0 ;

    if(transtype == MCC_PMOS)
      type = TRANSP ;
    else
      type = TRANSN ;

    if (ptmodel)
       {
        if(ptmodel->SUBCKTNAME != NULL)
        if((lofig = getloadedlofig (ptmodel->SUBCKTNAME)) != NULL)
          {
           for(lotrs = lofig->LOTRS ; lotrs != NULL ; lotrs = lotrs->NEXT)
             if(lotrs->TYPE == type)
             {
              //lotrs->WIDTH = (long)(W * (double)SCALE_X * 1e06) ;
              lotrs->WIDTH = mbk_long_round ( W * (double)SCALE_X * 1e06 );
              //lotrs->LENGTH = (long)(L * (double)SCALE_X * 1e06) ;
              lotrs->LENGTH = mbk_long_round ( L * (double)SCALE_X * 1e06 );
              res = 1 ;
             }
          }
       }
    return res ;
}

/*****************************************************************************************************/
/*                BINNING FUNCTION                                                                   */
/* Fonction utilisee par ELDO, lorsque les parametres d'un modele de transistor peuvent etre binnes  */
/* c'est a dire si leurs parametres de binning existent, ils sont automatiquement binnes             */
/* Exemple : VTH0(binne) = VTH0 + LVTH0/Weff + WVTH0/Weff + PVTH0/(Leff*Weff)                        */
/*****************************************************************************************************/

double binning(mcc_modellist *ptmodel, double binunit, double Leff, double Weff, char *param_name,int mcclog)
{
	char  Lparam_name[1024] ;
	char  Wparam_name[1024] ;
	char  Pparam_name[1024] ;
	double Lparam ;
	double Wparam ;
	double Pparam ;
	double init_value ;
	double bin_value ;
   
	sprintf(Lparam_name, "L%s", param_name) ;
	sprintf(Wparam_name, "W%s", param_name) ;
	sprintf(Pparam_name, "P%s", param_name) ;
 
	init_value = mcc_getparam(ptmodel, param_name)  ;
	Lparam     = mcc_getparam(ptmodel, Lparam_name) ;
	Wparam     = mcc_getparam(ptmodel, Wparam_name) ;
	Pparam     = mcc_getparam(ptmodel, Pparam_name) ;

    if(MCC_ROUND(binunit) == 1) {
        Leff = Leff*1.0e6 ;
        Weff = Weff*1.0e6 ;
	    bin_value  = init_value + Lparam / Leff + Wparam / Weff + Pparam / (Leff*Weff) ;
    }
    else
	    bin_value  = init_value + Lparam / Leff + Wparam / Weff + Pparam / (Leff*Weff) ;
	
    if ( mcclog == MCC_DRV_LOG ) {
      avt_log(LOGMCC,2,"[binning] binunit=%d, %s=%g, %s=%g,%s=%g, %s=%g, bin_val = %g\n",
                              MCC_ROUND(binunit),param_name,init_value,Lparam_name,Lparam,
                              Wparam_name,Wparam,Pparam_name,Pparam,bin_value);
    }
	return bin_value ;
}

double binning_quick(mcc_modellist *ptmodel, double binunit, double Leff, double Weff, int idx,int mcclog)
{
  double Lparam ;
  double Wparam ;
  double Pparam ;
  double init_value ;
  double bin_value ;
  
  if (idx>__MCC_QUICK_LAST_BINNABLE_ITEM) exit(50);
  
  init_value = mcc_getparam_quick(ptmodel, idx)  ;
  Lparam     = mcc_getparam_quick(ptmodel, idx+__MCC_QUICK_BIN_L_DEC) ;
  Wparam     = mcc_getparam_quick(ptmodel, idx+__MCC_QUICK_BIN_W_DEC) ;
  Pparam     = mcc_getparam_quick(ptmodel, idx+__MCC_QUICK_BIN_P_DEC) ;
  
  if(MCC_ROUND(binunit) == 1) {
    Leff = Leff*1.0e6 ;
        Weff = Weff*1.0e6 ;
        bin_value  = init_value + Lparam / Leff + Wparam / Weff + Pparam / (Leff*Weff) ;
  }
    else
      bin_value  = init_value + Lparam / Leff + Wparam / Weff + Pparam / (Leff*Weff) ;
  
  if ( mcclog == MCC_DRV_LOG ) {
    avt_log(LOGMCC,2,"[binning] binunit=%d, %s=%g, %s=%g,%s=%g, %s=%g, bin_val = %g\n",
            MCC_ROUND(binunit),mcc_translate_tab[idx].name,init_value,mcc_translate_tab[idx+__MCC_QUICK_BIN_L_DEC].name,Lparam,
            mcc_translate_tab[idx+__MCC_QUICK_BIN_W_DEC].name,Wparam,mcc_translate_tab[idx+__MCC_QUICK_BIN_P_DEC].name,Pparam,bin_value);
  }
  return bin_value ;
}

/**********************************************************************************\

 Function : binningval 

\**********************************************************************************/
double binningval (mcc_modellist *ptmodel, double binunit, double Leff, double Weff,
                   char *param_name, double param_value, int mcclog)
{
	char  Lparam_name[1024] ;
	char  Wparam_name[1024] ;
	char  Pparam_name[1024] ;
	double Lparam ;
	double Wparam ;
	double Pparam ;
	double bin_value ;
   
	sprintf(Lparam_name, "L%s", param_name) ;
	sprintf(Wparam_name, "W%s", param_name) ;
	sprintf(Pparam_name, "P%s", param_name) ;
 
	Lparam     = mcc_getparam(ptmodel, Lparam_name) ;
	Wparam     = mcc_getparam(ptmodel, Wparam_name) ;
	Pparam     = mcc_getparam(ptmodel, Pparam_name) ;

    if(MCC_ROUND(binunit) == 1) {
        Leff = Leff*1.0e6 ;
        Weff = Weff*1.0e6 ;
	    bin_value  = param_value + Lparam / Leff + Wparam / Weff + Pparam / (Leff*Weff) ;
    }
    else
	    bin_value  = param_value + Lparam / Leff + Wparam / Weff + Pparam / (Leff*Weff) ;
	
    if ( mcclog == MCC_DRV_LOG ) {
      avt_log(LOGMCC,2,"[binningval] binunit=%d, %s=%g, %s=%g,%s=%g, %s=%g, bin_val = %g\n",
                              MCC_ROUND(binunit),param_name,param_value,Lparam_name,Lparam,
                              Wparam_name,Wparam,Pparam_name,Pparam,bin_value);
    }
	return bin_value ;
}

double binningval_quick (mcc_modellist *ptmodel, double binunit, double Leff, double Weff,
                   int idx, double param_value, int mcclog)
{
  double Lparam ;
  double Wparam ;
  double Pparam ;
  double bin_value ;
  
  Lparam     = mcc_getparam_quick(ptmodel, idx+__MCC_QUICK_BIN_L_DEC) ;
  Wparam     = mcc_getparam_quick(ptmodel, idx+__MCC_QUICK_BIN_W_DEC) ;
  Pparam     = mcc_getparam_quick(ptmodel, idx+__MCC_QUICK_BIN_P_DEC) ;
  
  if(MCC_ROUND(binunit) == 1) {
    Leff = Leff*1.0e6 ;
    Weff = Weff*1.0e6 ;
    bin_value  = param_value + Lparam / Leff + Wparam / Weff + Pparam / (Leff*Weff) ;
  }
  else
    bin_value  = param_value + Lparam / Leff + Wparam / Weff + Pparam / (Leff*Weff) ;
  
  if ( mcclog == MCC_DRV_LOG ) {
    avt_log(LOGMCC,2,"[binningval] binunit=%d, %s=%g, %s=%g,%s=%g, %s=%g, bin_val = %g\n",
            MCC_ROUND(binunit),mcc_translate_tab[idx].name,param_value,mcc_translate_tab[idx+__MCC_QUICK_BIN_L_DEC].name,Lparam,
            mcc_translate_tab[idx+__MCC_QUICK_BIN_W_DEC].name,Wparam,mcc_translate_tab[idx+__MCC_QUICK_BIN_P_DEC].name,Pparam,bin_value);
  }
  return bin_value ;
}
/******************************************************************************\
 FUNCTION : mcc_affect_value ()
\******************************************************************************/
void mcc_affect_value (double *param, double value)
{
  if ( param )
    *param = value;
}

/****************************************************************************\
 Function : mcc_update_vth0

 Fonction qui met a jour le param VTH0 d'un model par rapport a son instance
\****************************************************************************/
double mcc_update_vth0 ( elp_lotrs_param *lotrsparam, double vth0 )
{
  if ( lotrsparam ) 
    vth0 += lotrsparam->PARAM[elpDELVT0];
  return vth0;
}

/****************************************************************************\
 Function : mcc_update_u0

 Fonction qui met a jour le param U0 d'un model par rapport a son instance
\****************************************************************************/
double mcc_update_u0 ( elp_lotrs_param *lotrsparam, double u0 )
{
  if ( lotrsparam ) 
    u0 *= lotrsparam->PARAM[elpMULU0];
  return u0;
}

/****************************************************************************\
 Function : mcc_check_vbs   

 Return 1 if OK
\****************************************************************************/
int mcc_check_vbs ( mcc_modellist *ptmodel, double L, double W, double vbs )
{
  int res = 1; // result ok
  char *trsname=NULL;

  if (MCC_CURRENT_LOTRS)
      if (MCC_CURRENT_LOTRS->TRNAME) trsname = MCC_CURRENT_LOTRS->TRNAME;

  if ( fabs(vbs) > 1.0e-5 ) {
    if ( ptmodel->TYPE == MCC_NMOS && vbs > 0.0 ) {
      avt_errmsg(MCC_ERRMSG, "031", AVT_ERROR, vbs,trsname?trsname:"trs",ptmodel->NAME,L,W);
      res = 0;
    }
    else if ( ptmodel->TYPE == MCC_PMOS && vbs < 0.0 ) {
      avt_errmsg(MCC_ERRMSG, "032", AVT_ERROR,vbs,trsname?trsname:"trs",ptmodel->NAME,L,W);
      res = 0;
    }
  }
  return res;
}

/****************************************************************************\
 Function : mcc_check_param 

 Fonction qui verifie les param mcc
\****************************************************************************/
int mcc_check_param ( int type )
{
  int res = 1; // result ok

  if ( type == MCC_TRANS_N || type == MCC_TRANS_B ) {
    if ( MCC_VTN > MCC_VDDmax )
      res = 0;
  }
  if ( type == MCC_TRANS_P || type == MCC_TRANS_B ) {
    if ( MCC_VTP > MCC_VDDmax )
      res = 0;
  }
  return res;
}

/******************************************************************************\
 FUNCTION : mcc_is_same_model

 return 1 if TRUE
\******************************************************************************/
int mcc_is_same_model ( mcc_modellist *ptmodel, double L , double W, int NF,
                        mcc_modellist *previous_model,
                        double previous_L,
                        double previous_W,
                        int PREVIOUS_NF,
                        elp_lotrs_param *lotrsparam,
                        chain_list *previous_longkey
                        )
{
  int res;

  if (!ELP_MODEL_LIST) return 0;
  if(((long)((double)previous_L*(double)1.0e9 + (double)0.5) != 
      (long)((double)L*(double)1.0e9 + (double)0.5)) ||
     ((long)((double)previous_W*(double)1.0e9 + (double)0.5) !=
       (long)((double)W*(double)1.0e9 + (double)0.5)) ||
      (PREVIOUS_NF != NF) ||
      (previous_model != ptmodel) ||
      lotrsparam==NULL || !elp_is_same_paramcontext(lotrsparam->longkey, previous_longkey)
      )
      res = 0 ;
  else
      res = 1 ;

  return res;
}
 
/******************************************************************************\
 FUNCTION : mcc_get_modelfilename
\******************************************************************************/
char *mcc_get_modelfilename ( int corner )
{
 char *filename;

 if ( !mcc_use_multicorner() ) 
   filename = MCC_MODELFILE;
 else {
   switch ( corner ) {
     case MCC_TYPICAL : filename = MCC_MODELFILE;
                        break;
                        /*
     case MCC_BEST    : if ( !(filename = MCC_MODELFILE_BEST) )
                          filename = MCC_MODELFILE;
                        break;
     case MCC_WORST   : if ( !(filename = MCC_MODELFILE_WORST) )
                          filename = MCC_MODELFILE;
                        break;
                        */
     default          : filename = MCC_MODELFILE;
                        break;
   }
 }
 return filename;
}
 
/******************************************************************************\
 FUNCTION : mcc_update_area_perim

 Func to call after mcc_calcspiparam!!!!
 
\******************************************************************************/
void mcc_update_area_perim ( int corner , int lotrscase )
{
  long xd_s,pd_s;
  double Weffcj;

  if ( MCC_CURRENT_LOTRS ) {
    elpLotrsGetShrinkDim(MCC_CURRENT_LOTRS,NULL,NULL,NULL,
                             &xd_s,NULL,&pd_s,NULL,NULL, lotrscase);
    if ( MLO_IS_TRANSN(MCC_CURRENT_LOTRS->TYPE) ) {
      Weffcj = MCC_WN*MCC_XWN+MCC_DWCJN;
      MCC_ASN = MCC_ADN = ((double)xd_s*Weffcj)/((double)(100.0*SCALE_X*SCALE_X));
      MCC_PSN = MCC_PDN = (double)pd_s/(double)SCALE_X;
    }
    else {
      Weffcj = MCC_WP*MCC_XWP+MCC_DWCJP;
      MCC_ASP = MCC_ADP = ((double)xd_s*Weffcj)/((double)(100.0*SCALE_X*SCALE_X));
      MCC_PSP = MCC_PDP = (double)pd_s/(double)SCALE_X;
    }
  }

  corner=0;
}
