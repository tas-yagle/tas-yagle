/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include AVT_H
#include "mcc_util.h"
#include "mcc_debug.h"
#include "mcc_mod_spice.h"
#include "mcc_curv.h"

double MCC_CBXG = -1.0;
double MCC_CBXGU = -1.0;
double MCC_CBXGD = -1.0;
int    MCC_USE_MULTICORNER = 0;
int    MCC_PLOT         = 0;
int    MCC_CALC_VT      = 1;
int    MCC_OPTIM_MODE   = 1;
int    MCC_NEW_CALC_ABR = 1;
int    MCC_USE_SPI_PARSER = 1;
char   MCC_MOD_SEPAR    = '.';
char  *MCC_BUF          = NULL;
int    MCC_EQUATE_RANGE = 1;
int    MCC_RESIZE_RANGE = 0;
double MCC_RANGE_MARGIN = 0.0;
double MCC_TECSIZE      = 1.0;
double MCC_AN           = 5.0e-05;
double MCC_BN           = 1.0;
double MCC_AP           = 2.0e-05;
double MCC_BP           = 1.0;
double MCC_RNT          = 10000.0;
double MCC_RPT          = 20000.0;
double MCC_KRNT         = 1.0/6.0;
double MCC_KRPT         = 1.0/6.0;
double MCC_RNS          = 10000.0;
double MCC_RPS          = 20000.0;
double MCC_KRNS         = 3.0/4.0;
double MCC_KRPS         = 3.0/4.0;
double MCC_KSP          = 1.0;
double MCC_KSN          = 1.0;
double MCC_KRP          = 1.0;
double MCC_KRN          = 1.0;
double MCC_VDDmax       = 5.0;
double MCC_VDD_BEST     = 5.0;
double MCC_VDD_WORST    = 5.0;
double MCC_VDDdeg       = 3.5;
double MCC_VSSdeg       = 1.5;
double MCC_VTIN = 5.0;
double MCC_VTIP = 0.0;
double MCC_VTP          = 1.0;
double MCC_VT0P         = 1.0;
double MCC_VTN          = 1.0;
double MCC_VT0N         = 1.0;
double MCC_KTP          = 0.0;
double MCC_KTN          = 0.0;
double MCC_WN           = 6.0;
double MCC_LN           = 1.0;
double MCC_WP           = 12.0;
double MCC_LP           = 1.0;
double MCC_ASN          = -1.0;
double MCC_ASP          = -1.0;
double MCC_ADN          = -1.0;
double MCC_ADP          = -1.0;
double MCC_PSN          = -1.0;
double MCC_PSP          = -1.0;
double MCC_PDN          = -1.0;
double MCC_PDP          = -1.0;
double MCC_LNMAX        = MCC_D_LWMAX;
double MCC_LNMIN        = 0.0;
double MCC_WNMAX        = MCC_D_LWMAX;
double MCC_WNMIN        = 0.0;
double MCC_LPMAX        = MCC_D_LWMAX;
double MCC_LPMIN        = 0.0;
double MCC_WPMAX        = MCC_D_LWMAX;
double MCC_WPMIN        = 0.0;
double MCC_DWN          = 0.0;
double MCC_DLN          = 0.0;
double MCC_DWP          = 0.0;
double MCC_DLP          = 0.0;
double MCC_DWCN         = 0.0;
double MCC_DWCJN        = 0.0;
double MCC_DLCN         = 0.0;
double MCC_DWCP         = 0.0;
double MCC_DWCJP        = 0.0;
double MCC_DLCP         = 0.0;
double MCC_DIF          = 2.0;
double MCC_XWN          = 1.0;
double MCC_XLN          = 1.0;
double MCC_XWP          = 1.0;
double MCC_XLP          = 1.0;
double MCC_RACCNS       = 0.0;
double MCC_RACCND       = 0.0;
double MCC_RACCPS       = 0.0;
double MCC_RACCPD       = 0.0;
double MCC_CGSN         = 1000e-6;
double MCC_CGS0N        = 1000e-6;
double MCC_CGSUN        = 1000e-6;
double MCC_CGSUFN       = 1000e-6;
double MCC_CGSU0N       = 1000e-6;
double MCC_CGSDN        = 1000e-6;
double MCC_CGSDFN       = 1000e-6;
double MCC_CGSD0N       = 1000e-6;
double MCC_CGSU_N_MAX   = 1000e-6;
double MCC_CGSU_N_MIN   = 1000e-6;
double MCC_CGSD_N_MAX   = 1000e-6;
double MCC_CGSD_N_MIN   = 1000e-6;
double MCC_CGSP         = 1000e-6;
double MCC_CGS0P        = 1000e-6;
double MCC_CGSUP        = 1000e-6;
double MCC_CGSUFP       = 1000e-6;
double MCC_CGSU0P       = 1000e-6;
double MCC_CGSDP        = 1000e-6;
double MCC_CGSDFP       = 1000e-6;
double MCC_CGSD0P       = 1000e-6;
double MCC_CGSU_P_MAX   = 1000e-6;
double MCC_CGSU_P_MIN   = 1000e-6;
double MCC_CGSD_P_MAX   = 1000e-6;
double MCC_CGSD_P_MIN   = 1000e-6;
double MCC_CGDN         = 0.0;
double MCC_CGD0N        = 0.0;
double MCC_CGD1N        = 0.0;
double MCC_CGD2N        = 0.0;
double MCC_CGDCN        = 0.0;
double MCC_CGDC0N       = 0.0;
double MCC_CGDC1N       = 0.0;
double MCC_CGDC2N       = 0.0;
double MCC_CGDP         = 0.0;
double MCC_CGD0P        = 0.0;
double MCC_CGD1P        = 0.0;
double MCC_CGD2P        = 0.0;
double MCC_CGDCP        = 0.0;
double MCC_CGDC0P        = 0.0;
double MCC_CGDC1P        = 0.0;
double MCC_CGDC2P        = 0.0;
double MCC_CGSIN        = 0.0;
double MCC_CGSICN       = 0.0;
double MCC_CGSIP        = 0.0;
double MCC_CGSICP       = 0.0;
double MCC_CGPN         = 100e-6;
double MCC_CGPU_N_MAX   = 100e-6;
double MCC_CGPU_N_MIN   = 100e-6;
double MCC_CGPD_N_MAX   = 100e-6;
double MCC_CGPD_N_MIN   = 100e-6;
double MCC_CGPO_N       = 0.0;
double MCC_CGPOC_N      = 0.0;
double MCC_CGPP         = 100e-6;
double MCC_CGPU_P_MAX   = 100e-6;
double MCC_CGPU_P_MIN   = 100e-6;
double MCC_CGPD_P_MAX   = 100e-6;
double MCC_CGPD_P_MIN   = 100e-6;
double MCC_CGPO_P       = 0.0;
double MCC_CGPOC_P      = 0.0;
double MCC_CDSN         = 100e-6;
double MCC_CDS_U_N      = 100e-6;
double MCC_CDS_D_N      = 100e-6;
double MCC_CDSP         = 100e-6;
double MCC_CDS_U_P      = 100e-6;
double MCC_CDS_D_P      = 100e-6;
double MCC_CDPN         = 100e-6;
double MCC_CDP_U_N      = 100e-6;
double MCC_CDP_D_N      = 100e-6;
double MCC_CDPP         = 100e-6;
double MCC_CDP_U_P      = 100e-6;
double MCC_CDP_D_P      = 100e-6;
double MCC_CDWN         = 100e-6;
double MCC_CDW_U_N      = 100e-6;
double MCC_CDW_D_N      = 100e-6;
double MCC_CDWP         = 100e-6;
double MCC_CDW_U_P      = 100e-6;
double MCC_CDW_D_P      = 100e-6;
double MCC_CSSN         = 100e-6;
double MCC_CSS_U_N      = 100e-6;
double MCC_CSS_D_N      = 100e-6;
double MCC_CSSP         = 100e-6;
double MCC_CSS_U_P      = 100e-6;
double MCC_CSS_D_P      = 100e-6;
double MCC_CSPN         = 100e-6;
double MCC_CSP_U_N      = 100e-6;
double MCC_CSP_D_N      = 100e-6;
double MCC_CSPP         = 100e-6;
double MCC_CSP_U_P      = 100e-6;
double MCC_CSP_D_P      = 100e-6;
double MCC_CSWN         = 100e-6;
double MCC_CSW_U_N      = 100e-6;
double MCC_CSW_D_N      = 100e-6;
double MCC_CSWP         = 100e-6;
double MCC_CSW_U_P      = 100e-6;
double MCC_CSW_D_P      = 100e-6;
double MCC_VGS          = 5.0;
double MCC_VBULKN       = 0.0;
double MCC_VBULKP       = 5.0;
double MCC_TEMP         = 70.0;
double MCC_TEMP_BEST    = 70.0;
double MCC_TEMP_WORST   = 70.0;
double MCC_VTH          = 0.5;
double MCC_VTH_HIGH     = 0.8;
double MCC_VTH_LOW      = 0.2;
double MCC_SLOPE        = 200.0;
double MCC_DC_STEP      = 0.001;
double MCC_ERROR        = 1.0;
double MCC_WNeff;
double MCC_LNeff;
double MCC_WPeff;
double MCC_LPeff;
int    MCC_NINDEX       = 0;
int    MCC_PINDEX       = 0;
int    MCC_NCASE        = MCC_TYPICAL;
int    MCC_PCASE        = MCC_TYPICAL;
int    MCC_SWJUNCAPN      = -1;
int    MCC_SWJUNCAPP      = -1;

char  *MCC_ELPFILE      = "techno.elp";
char  *MCC_TECHFILE     = "avtdefault.tec";
char  *MCC_MODELFILE    = "avtdefault.tec";
char  *MCC_MODELFILE_BEST = NULL;
char  *MCC_MODELFILE_WORST = NULL;
char  *MCC_SPICESTRING  = "spice $";
char  *MCC_SPICENAME    = "spice";
char  *MCC_SPICEOPTIONS = "list";
char  *MCC_TASNAME      = "hitas";
char  *MCC_SPICEOUT     = "$.out";
char  *MCC_SPICESTDOUT  = "$.out";
char  *MCC_TNMODEL      = "tn";
char  *MCC_TPMODEL      = "tp";
char  *MCC_TNMODELTYPE  = "tn";
char  *MCC_TPMODELTYPE  = "tp";
char  *MCC_ELPVERSION   = "1.0";
int    MCC_SPICEMODELTYPE         = MCC_NOMODEL;
char  *MCC_MOD_NAME[MCC_NB_MOD]   = MCC_TABMOD_NAME;
char  *MCC_CASE_NAME[MCC_NB_CASE] = MCC_TABCASE_NAME;
int    MCC_INSNUMB      = 15;
int    MCC_TRANSNUMB    = 12;
int    MCC_INVNUMB      = 5;
int    MCC_NBMAXLOOP    = 10;
float  MCC_CAPA         = 200.0;

char   MCC_MODE         = MCC_NORMAL_MODE;
char   MCC_CALC_CUR     = MCC_SIM_MODE;
char   MCC_CALC_ORG     = MCC_CALC_MODE;

char  *MCC_SPICEFILE[]  = { "fit_a_sim.spi", "fit_cg_sim.spi", "fit_cg_nocapa_sim.spi", "fit_cdn_sim.spi", "fit_cdp_sim.spi", "param.spi" };
int    MCC_SPICENB      = 6;
char  *MCC_TASFILE[]    = { "fit_a.spi", "fit_cg.spi", "fit_cg_nocapa.spi", "fit_cdn.spi", "fit_cdp.spi" };
int    MCC_TASNB        = 5;

struct mcc_modelx *MODEL ;

char   **MCC_GLOBAL_CHR[] = { &MCC_ELPFILE, \
                              &MCC_TNMODEL, \
                              &MCC_TPMODEL
                            } ;

double *MCC_GLOBAL_DBL[] = { &MCC_VDDmax ,\
                             &MCC_VGS    ,\
                             &MCC_VBULKN ,\
                             &MCC_VBULKP ,\
                             &MCC_TEMP   ,\
                             &MCC_WNeff  ,\
                             &MCC_LNeff  ,\
                             &MCC_WPeff  ,\
                             &MCC_LPeff  ,\
                             &MCC_LNMAX  ,\
                             &MCC_LNMIN  ,\
                             &MCC_WNMAX  ,\
                             &MCC_WNMIN  ,\
                             &MCC_LPMAX  ,\
                             &MCC_LPMIN  ,\
                             &MCC_WPMAX  ,\
                             &MCC_WPMIN
                           } ;
int mcc_retkey (char *chaine)
{
	unsigned int i;

	for (i = 0; (i < strlen (chaine)) && (chaine[i] != '='); i++);

	return (i == strlen(chaine)) ? -1 : (int)i;
}

long mcc_ftol (double f)
{
	long l;
	long d;

	if ((f > (double)MCC_LWMAX) || ((f * 10.0) > MCC_LWMAX))
		return (MCC_LWMAX);

	d = (long)(f * 10.0) - (long)f *10;
	if (d <= 5)
		l = (long)f;
	if (d > 5 && f > 0)
		l = (long)f + 1;
	if (d > 5 && f < 0)
		l = (long)f - 1;
	if (f == 0.0)
		l = (long)0;

	return l;
}

int mcc_ftoi (double f)
{
	long l;
	long d;

	d = (long)(f * 10) - (long)f *10;
	if (d <= 5)
		l = (long)f;
	if (d > 5 && f > 0)
		l = (long)f + 1;
	if (d > 5 && f < 0)
		l = (long)f - 1;
	if (f == 0.0)
		l = (long)0;

	return l;
}

char *mcc_getfilename (filename)
char *filename;
{
	char *pt;
	char *ptx;

	pt = (char *)mbkalloc (strlen (filename) + 1);
	strcpy (pt, filename);
	ptx = strstr (pt, ".spi");
	if (ptx != NULL)
		*ptx = '\0';
	return (pt);
}

char *mcc_getfisrtarg (str, old)
char *str;
char *old;
{
	char *pt;

	pt = (char *)mbkalloc (strlen (str) + 1);
	strcpy (pt, str);
	mbkfree (old);
	old = pt;
	while (isalnum ((int)*pt) != 0)
		pt++;
	*pt = '\0';

	return (old);
}

char *mcc_initstr (str)
char *str;
{
	char *pt;

    if(str == NULL)
       return(NULL) ;

	pt = (char *)mbkalloc (strlen (str) + 1);
	strcpy (pt, str);
	return (pt);
}

void mcc_initcalcparam (int force)
{
    static int already_init = 0;
    if (!already_init || force) {
		MCC_USE_MULTICORNER = 0;
		MCC_PLOT       = 0;
		MCC_CALC_VT    = 1;
		MCC_OPTIM_MODE = 1;
		MCC_NEW_CALC_ABR = 1;
		MCC_USE_SPI_PARSER = 1;
        MCC_EQUATE_RANGE = 1;
        MCC_RESIZE_RANGE = 0;
        MCC_RANGE_MARGIN = 0.0;
		MCC_MOD_SEPAR = '.';
		MCC_TECSIZE = 1.0;
		MCC_AN = 5.0e-05;
		MCC_BN = 1.0;
		MCC_AP = 2.0e-05;
		MCC_BP = 1.0;
		MCC_RNT = 10000.0;
		MCC_RPT = 20000.0;
		MCC_RNS = 10000.0;
		MCC_RPS = 20000.0;
		MCC_KSN = 1.0;
		MCC_KSP = 1.0;
		MCC_KRN = 1.0;
		MCC_KRP = 1.0;
		MCC_VDDmax = 5.0;
		MCC_VDD_BEST = 5.0;
		MCC_VDD_WORST = 5.0;
		MCC_VDDdeg = 3.5;
		MCC_VSSdeg = 1.5;
		MCC_VTIN= 5.0;
		MCC_VTIP= 0.0;
		MCC_VTP = 1.0;
		MCC_VT0P = 1.0;
		MCC_VTN = 1.0;
		MCC_VT0N = 1.0;
		MCC_KTN = 0.0;
		MCC_KTP = 0.0;
		MCC_WN = 6.0;
		MCC_LN = 1.0;
		MCC_WP = 12.0;
		MCC_LP = 1.0;
        MCC_ASN = -1.0;
        MCC_ASP = -1.0;
        MCC_ADN = -1.0;
        MCC_ADP = -1.0;
        MCC_PSN = -1.0;
        MCC_PSP = -1.0;
        MCC_PDN = -1.0;
        MCC_PDP = -1.0;
		MCC_LNMAX = MCC_D_LWMAX;
		MCC_LNMIN = 0.0;
		MCC_WNMAX = MCC_D_LWMAX;
		MCC_WNMIN = 0.0;
		MCC_LPMAX = MCC_D_LWMAX;
		MCC_LPMIN = 0.0;
		MCC_WPMAX = MCC_D_LWMAX;
		MCC_WPMIN = 0.0;
		MCC_DWN = 0.0;
		MCC_DLN = 0.0;
		MCC_DWP = 0.0;
		MCC_DLP = 0.0;
		MCC_DWCN = 0.0;
		MCC_DWCJN = 0.0;
		MCC_DLCN = 0.0;
		MCC_DWCP = 0.0;
		MCC_DWCJP = 0.0;
		MCC_DLCP = 0.0;
		MCC_DIF = 2.0;
		MCC_XWN = 1.0;
		MCC_XLN = 1.0;
		MCC_XWP = 1.0;
		MCC_XLP = 1.0;
                MCC_RACCNS = 0.0;
                MCC_RACCND = 0.0;
                MCC_RACCPS = 0.0;
                MCC_RACCPD = 0.0;
		MCC_CGSN = MCC_CGS0N = 1000e-6;
		MCC_CGSU_N_MAX = 1000e-6;
		MCC_CGSU_N_MIN = 1000e-6;
		MCC_CGSD_N_MAX = 1000e-6;
		MCC_CGSD_N_MIN = 1000e-6;
		MCC_CGPU_N_MAX = 1000e-6;
		MCC_CGPU_N_MIN = 1000e-6;
		MCC_CGPD_N_MAX = 1000e-6;
		MCC_CGPD_N_MIN = 1000e-6;
		MCC_CGSU_P_MAX = 1000e-6;
		MCC_CGSU_P_MIN = 1000e-6;
		MCC_CGSD_P_MAX = 1000e-6;
		MCC_CGSD_P_MIN = 1000e-6;
		MCC_CGPU_P_MAX = 1000e-6;
		MCC_CGPU_P_MIN = 1000e-6;
		MCC_CGPD_P_MAX = 1000e-6;
		MCC_CGPD_P_MIN = 1000e-6;
                MCC_CGPO_N = 0.0;
                MCC_CGPOC_N = 0.0;
                MCC_CGPO_P = 0.0;
                MCC_CGPOC_P = 0.0;
		MCC_CGSP = MCC_CGS0P = 1000e-6;
		MCC_CGDN = 0.0;
		MCC_CGD0N = 0.0;
		MCC_CGD1N = 0.0;
		MCC_CGD2N = 0.0;
		MCC_CGDCN = 0.0;
		MCC_CGDC0N = 0.0;
		MCC_CGDC1N = 0.0;
		MCC_CGDC2N = 0.0;
		MCC_CGDP = 0.0;
		MCC_CGD0P = 0.0;
		MCC_CGD1P = 0.0;
		MCC_CGD2P = 0.0;
		MCC_CGDCP = 0.0;
		MCC_CGDC0P = 0.0;
		MCC_CGDC1P = 0.0;
		MCC_CGDC2P = 0.0;
		MCC_CGSIN = 0.0;
		MCC_CGSICN = 0.0;
		MCC_CGSIP = 0.0;
		MCC_CGSICP = 0.0;
		MCC_CGPN = 100e-6;
		MCC_CGPP = 100e-6;
		MCC_CDSN = 100e-6;
		MCC_CDS_U_N = 100e-6;
		MCC_CDS_D_N = 100e-6;
		MCC_CDSP = 100e-6;
		MCC_CDS_U_P = 100e-6;
		MCC_CDS_D_P = 100e-6;
		MCC_CDPN = 100e-6;
		MCC_CDP_U_N = 100e-6;
		MCC_CDP_D_N = 100e-6;
		MCC_CDPP = 100e-6;
		MCC_CDP_U_P = 100e-6;
		MCC_CDP_D_P = 100e-6;
		MCC_CDWN = 100e-6;
		MCC_CDW_U_N = 100e-6;
		MCC_CDW_D_N = 100e-6;
		MCC_CDWP = 100e-6;
		MCC_CDW_U_P = 100e-6;
		MCC_CDW_D_P = 100e-6;
		MCC_CSSN = 100e-6;
		MCC_CSS_U_N = 100e-6;
		MCC_CSS_D_N = 100e-6;
		MCC_CSSP = 100e-6;
		MCC_CSS_U_P = 100e-6;
		MCC_CSS_D_P = 100e-6;
		MCC_CSPN = 100e-6;
		MCC_CSP_U_N = 100e-6;
		MCC_CSP_D_N = 100e-6;
		MCC_CSPP = 100e-6;
		MCC_CSP_U_P = 100e-6;
		MCC_CSP_D_P = 100e-6;
		MCC_CSWN = 100e-6;
		MCC_CSW_U_N = 100e-6;
		MCC_CSW_D_N = 100e-6;
		MCC_CSWP = 100e-6;
		MCC_CSW_U_P = 100e-6;
		MCC_CSW_D_P = 100e-6;
		MCC_VGS = MCC_VDDmax / 2.0 ;
        MCC_VBULKN = ELPMINVBULK;
        MCC_VBULKP = ELPMINVBULK;
		MCC_TEMP = 70.0;
		MCC_TEMP_BEST = 70.0;
		MCC_TEMP_WORST = 70.0;
		MCC_SLOPE = elpGeneral[elpSLOPE];
		MCC_DC_STEP = 0.001;
		MCC_ERROR = 1.0;
		MCC_NINDEX = 0;
		MCC_PINDEX = 0;
		MCC_NCASE = MCC_TYPICAL;
		MCC_PCASE = MCC_TYPICAL;
    
		MCC_ELPFILE = mcc_initstr ("techno.elp");
		MCC_TECHFILE = mcc_initstr ("avtdefault.tec");
		MCC_MODELFILE = mcc_initstr ("avtdefault.tec");
		MCC_MODELFILE_BEST = NULL;
		MCC_MODELFILE_WORST = NULL;
		MCC_SPICESTRING = mcc_initstr ("spice $");
		MCC_SPICENAME = mcc_initstr ("spice");
		MCC_SPICEOPTIONS = mcc_initstr ("list");
		MCC_TASNAME = mcc_initstr ("hitas");
		MCC_SPICEOUT = mcc_initstr ("$.out");
		MCC_SPICESTDOUT = mcc_initstr ("$.out");
		MCC_TNMODEL = mcc_initstr ("tn");
		MCC_TPMODEL = mcc_initstr ("tp");
		MCC_TNMODELTYPE = mcc_initstr ("tn");
		MCC_TPMODELTYPE = mcc_initstr ("tp");
		MCC_ELPVERSION = mcc_initstr ("1.0");
		MCC_SPICEMODELTYPE = MCC_NOMODEL;
		MCC_INSNUMB = 15;
		MCC_TRANSNUMB = 12;
		MCC_INVNUMB = 5;
		MCC_NBMAXLOOP = 10;
		MCC_CAPA = 200.0;
        already_init = 1;
    }
}

static int done_mcc_env=0;

void mccenv ()
{
	char *env;

    if (done_mcc_env) return;
    done_mcc_env=1;

    mcc_check_quick_param_namealloc();
    
	elpSetFct (mcc_genparam);
	elpSetLeakFct (mcc_calcIleakage_from_lotrs);
        elpSetCalcPAFct (mcc_calcPAfromgeomod );
    mcc_initcalcparam (0);
    simenv() ;

	MCC_TECSIZE = SIM_TECSIZE ;
    MCC_VDDmax = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE ;
    MCC_VGS = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE/2.0 ;
    MCC_TEMP = V_FLOAT_TAB[__SIM_TEMP].VALUE ; 
    MCC_VTH = SIM_VTH ;
    MCC_VTH_HIGH = SIM_VTH_HIGH ;
    MCC_VTH_LOW = SIM_VTH_LOW ;
    MCC_SLOPE = SIM_SLOP ;
    MCC_DC_STEP = SIM_DC_STEP ;
	if (MCC_SPICEOPTIONS != NULL)
		mbkfree (MCC_SPICEOPTIONS);
    MCC_SPICEOPTIONS = mcc_initstr(SIM_SPICE_OPTIONS) ;
	if (MCC_SPICESTRING != NULL)
		mbkfree (MCC_SPICESTRING);
    MCC_SPICESTRING = mcc_initstr(SIM_SPICESTRING) ;
	if (MCC_SPICENAME != NULL)
		mbkfree (MCC_SPICENAME);
    MCC_SPICENAME = mcc_initstr(SIM_SPICENAME) ;
	if (MCC_SPICEOUT != NULL)
		mbkfree (MCC_SPICEOUT);
    MCC_SPICEOUT = mcc_initstr(SIM_SPICEOUT) ;
	if (MCC_SPICESTDOUT != NULL)
		mbkfree (MCC_SPICESTDOUT);
    MCC_SPICESTDOUT = mcc_initstr(SIM_SPICESTDOUT) ;

    if(V_STR_TAB[__SIM_TECH_FILE].VALUE != NULL)
       {
	    if (MCC_TECHFILE != NULL)
		    mbkfree (MCC_TECHFILE);
        MCC_TECHFILE = mcc_initstr ( SIM_TECHFILE ) ;
       }

	env = V_STR_TAB[__MCC_MOD_SEPAR].VALUE;
	if (env != NULL) 
		MCC_MOD_SEPAR = *env;
    MCC_EQUATE_RANGE = V_BOOL_TAB[__MCC_EQUATE_RANGE].VALUE;
	MCC_RESIZE_RANGE = V_BOOL_TAB[__MCC_RESIZE_RANGE].VALUE;
	if (V_FLOAT_TAB[__MCC_VDD_BEST].SET) {
      MCC_VDD_BEST = V_FLOAT_TAB[__MCC_VDD_BEST].VALUE;
      MCC_USE_MULTICORNER = 1;
    }
    else
      MCC_VDD_BEST = MCC_VDDmax;
	if (V_FLOAT_TAB[__MCC_VDD_WORST].SET) {
      MCC_VDD_WORST = V_FLOAT_TAB[__MCC_VDD_WORST].VALUE;
      MCC_USE_MULTICORNER = 1;
    }
    else
      MCC_VDD_WORST = MCC_VDDmax;
	if (V_FLOAT_TAB[__MCC_TEMP_BEST].SET) {
      MCC_TEMP_BEST = V_FLOAT_TAB[__MCC_TEMP_BEST].VALUE;
      MCC_USE_MULTICORNER = 1;
    }
    else
      MCC_TEMP_BEST = MCC_TEMP;
	if (V_FLOAT_TAB[__MCC_TEMP_WORST].SET)
      MCC_TEMP_WORST = V_FLOAT_TAB[__MCC_TEMP_WORST].VALUE;
    else
      MCC_TEMP_WORST = MCC_TEMP;
	MCC_RANGE_MARGIN = V_FLOAT_TAB[__MCC_RANGE_MARGIN].VALUE;
	if (MCC_RANGE_MARGIN > 1.0 || MCC_RANGE_MARGIN < 0.0)
		MCC_RANGE_MARGIN = 0.0;
	env = getenv ("MCC_FIT_ERROR");
	if (env != NULL) {
		MCC_ERROR = atof (env);
	}
	env = getenv ("MCC_ELP_FILE");
	if (env != NULL) {
	if (MCC_ELPFILE != NULL)
			mbkfree (MCC_ELPFILE);
		MCC_ELPFILE = mcc_initstr (env);
	}
      /*---------------------------*/
     /* Typical model file corner */
    /*---------------------------*/
	env = V_STR_TAB[__MCC_MODEL_FILE].VALUE;
	if (env != NULL) {
		if (MCC_MODELFILE != NULL)
			mbkfree (MCC_MODELFILE);
		MCC_MODELFILE = mcc_initstr (env);
        spi_set_model_corner ( MCC_TYPICAL );
        parsespice ( MCC_MODELFILE ); 
	}
      /*------------------------*/
     /* Best model file corner */
    /*------------------------*/
	env = V_STR_TAB[__MCC_MODEL_FILE_BEST].VALUE;
	if (env != NULL) {
		MCC_MODELFILE_BEST = mcc_initstr (env);
        MCC_USE_MULTICORNER = 1;
        
        //====> update corner for parserspice
        spi_set_model_corner ( MCC_BEST );
        parsespice ( MCC_MODELFILE_BEST ); 
	}
    else {
      if ( MCC_USE_MULTICORNER && MCC_MODELFILE ) {
		MCC_MODELFILE_BEST = MCC_MODELFILE;
        spi_set_model_corner ( MCC_BEST );
        avt_errmsg(MCC_ERRMSG, "033", AVT_ERROR,MCC_MODELFILE_BEST) ;
        parsespice ( MCC_MODELFILE_BEST ); 
      }
    }
      /*-------------------------*/
     /* Worst model file corner */
    /*-------------------------*/
	env = V_STR_TAB[__MCC_MODEL_FILE_WORST].VALUE;
	if (env != NULL) {
		MCC_MODELFILE_WORST = mcc_initstr (env);
        MCC_USE_MULTICORNER = 1;

        //====> update corner for parserspice
        spi_set_model_corner ( MCC_WORST );
        parsespice ( MCC_MODELFILE_WORST ); 
	}
    else {
      if ( MCC_USE_MULTICORNER && MCC_MODELFILE ) {
		MCC_MODELFILE_WORST = MCC_MODELFILE;
        spi_set_model_corner ( MCC_WORST );
        avt_errmsg(MCC_ERRMSG, "034", AVT_ERROR,MCC_MODELFILE_WORST) ;
        parsespice ( MCC_MODELFILE_WORST ); 
      }
    }
	env = avt_gethashvar ("MCC_TAS_NAME");
	if (env != NULL) {
		if (MCC_TASNAME != NULL)
			mbkfree (MCC_TASNAME);
		MCC_TASNAME = mcc_initstr (env);
	}
	env = avt_gethashvar ("MCC_TN_MODEL");
	if (env != NULL) {
		if (MCC_TNMODEL != NULL)
			mbkfree (MCC_TNMODEL);
		MCC_TNMODEL = mcc_initstr (env);
	}
	env = avt_gethashvar ("MCC_TP_MODEL");
	if (env != NULL) {
		if (MCC_TPMODEL != NULL)
			mbkfree (MCC_TPMODEL);
		MCC_TPMODEL = mcc_initstr (env);
	}
	env = avt_gethashvar ("MCC_ELP_VERSION");
	if (env != NULL) {
		if (MCC_ELPVERSION != NULL)
			mbkfree (MCC_ELPVERSION);
		MCC_ELPVERSION = mcc_initstr (env);
	}
	env = avt_gethashvar ("MCC_INS_FIT_NUMB");
	if (env != NULL) {
		MCC_INSNUMB = atoi (env);
	}
	env = avt_gethashvar ("MCC_TRANS_FIT_NUMB");
	if (env != NULL) {
		MCC_TRANSNUMB = atoi (env);
	}
	env = avt_gethashvar ("MCC_INV_FIT_NUMB");
	if (env != NULL) {
		MCC_INVNUMB = atoi (env);
	}
	env = avt_gethashvar ("MCC_MAX_FIT_LOOP");
	if (env != NULL) {
		MCC_NBMAXLOOP = atoi (env);
	}
	env = avt_gethashvar ("MCC_CAPA_FIT");
	if (env != NULL) {
		MCC_CAPA = atof (env);
	}
    MCC_OPTIM_MODE = V_BOOL_TAB[__MCC_OPTIM_MODE].VALUE ;
	MCC_CALC_VT = V_BOOL_TAB[__MCC_CALC_VT].VALUE;
	env = getenv ("MCC_NEW_CALC_ABR");
	if ((env != NULL) && (strcmp (env, "no") == 0)) 
      MCC_NEW_CALC_ABR = 0;
	env = getenv ("MCC_USE_SPI_PARSER");
	if ((env != NULL) && (strcmp (env, "no") == 0)) 
      MCC_USE_SPI_PARSER = 0;
	elpSetFct2 ( mcc_setelpparam );
}

//-------------------------------------------------------------------------
void mcc_calcInputCapa ( int type, elp_lotrs_param *lotrsparam )
{
  double vg1,vg2,vd1,vd2,vs1,vs2;
  double def_low = 0.0;
  double def_high = 0.5;

  if ( type == MCC_NMOS ) {
    vs1 = vs2 = 0.0;

    /*--------- UP Transition -------------*/
    //==>  Min capacitance                    
    vg1 = def_low*MCC_VDDmax;
    vg2 = def_high*MCC_VDDmax;
    vd1 = MCC_VDDmax;
    vd2 = 0.0;
    mcc_GetInputCapa (MCC_MODELFILE, MCC_TNMODEL,
                      MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                      MCC_TEMP, vg1, vg2, vd1, vd1, vs1, vs2, lotrsparam,
                      &MCC_CGSU_N_MIN,NULL,&MCC_CGPU_N_MIN);
    MCC_CGPU_N_MIN *= 1.0e6;
    //==>  Max capacitance                    
    mcc_GetInputCapa (MCC_MODELFILE, MCC_TNMODEL,
                      MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                      MCC_TEMP, vg1, vg2, vd1, vd2, vs1, vs2, lotrsparam,
                      &MCC_CGSU_N_MAX,NULL,&MCC_CGPU_N_MAX);
    MCC_CGPU_N_MAX *= 1.0e6;

    /*------  DOWN Transition -------------*/
    //==>  Min capacitance                    
    vg1 = (1.0-def_low)*MCC_VDDmax;
    vg2 = (1.0-def_high)*MCC_VDDmax;
    vd1 = 0.0;
    vd2 = MCC_VDDmax;
    mcc_GetInputCapa (MCC_MODELFILE, MCC_TNMODEL,
                      MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                      MCC_TEMP, vg1, vg2, vd1, vd1, vs1, vs2, lotrsparam,
                      &MCC_CGSD_N_MIN,NULL,&MCC_CGPD_N_MIN);
    MCC_CGPD_N_MIN *= 1.0e6;
    //==>  Max capacitance                    
    mcc_GetInputCapa (MCC_MODELFILE, MCC_TNMODEL,
                      MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                      MCC_TEMP, vg1, vg2, vd1, vd2, vs1, vs2, lotrsparam,
                      &MCC_CGSD_N_MAX,NULL,&MCC_CGPD_N_MAX);
    MCC_CGPD_N_MAX *= 1.0e6;
    
  }
  else {
    vs1 = vs2 = MCC_VDDmax;

    /*--------- UP Transition -------------*/
    //==>  Min capacitance                    
    vg1 = def_low*MCC_VDDmax;
    vg2 = def_high*MCC_VDDmax;
    vd1 = MCC_VDDmax;
    vd2 = 0.0;
    mcc_GetInputCapa (MCC_MODELFILE, MCC_TPMODEL,
                      MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                      MCC_TEMP, vg1, vg2, vd1, vd1, vs1, vs2, lotrsparam,
                      &MCC_CGSU_P_MIN,NULL,&MCC_CGPU_P_MIN);
    MCC_CGPU_P_MIN *= 1.0e6;
    //==>  Max capacitance                    
    mcc_GetInputCapa (MCC_MODELFILE, MCC_TPMODEL,
                      MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                      MCC_TEMP, vg1, vg2, vd1, vd2, vs1, vs2, lotrsparam,
                      &MCC_CGSU_P_MAX,NULL,&MCC_CGPU_P_MAX);
    MCC_CGPU_P_MAX *= 1.0e6;

    /*------  DOWN Transition -------------*/
    //==>  Min capacitance                    
    vg1 = (1.0-def_low)*MCC_VDDmax;
    vg2 = (1.0-def_high)*MCC_VDDmax;
    vd1 = 0.0;
    vd2 = MCC_VDDmax;
    mcc_GetInputCapa (MCC_MODELFILE, MCC_TPMODEL,
                      MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                      MCC_TEMP, vg1, vg2, vd1, vd1, vs1, vs2, lotrsparam,
                      &MCC_CGSD_P_MIN,NULL,&MCC_CGPD_P_MIN);
    MCC_CGPD_P_MIN *= 1.0e6;
    //==>  Max capacitance                    
    mcc_GetInputCapa (MCC_MODELFILE, MCC_TPMODEL,
                      MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                      MCC_TEMP, vg1, vg2, vd1, vd2, vs1, vs2, lotrsparam,
                      &MCC_CGSD_P_MAX,NULL,&MCC_CGPD_P_MAX);
    MCC_CGPD_P_MAX *= 1.0e6;

  }
}

void mcc_calcspiparam (type,lotrsparam_n,lotrsparam_p)
int type;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
    double vt ;
    double vbsn = lotrsparam_n->VBULK;
    double vbsp = lotrsparam_p->VBULK - MCC_VDDmax;
    double vbx_d_n1 = lotrsparam_n->VBULK-MCC_VDDmax;
    double vbx_d_n2 = lotrsparam_n->VBULK-MCC_VDDmax/2.0;
    double vbx_u_n1 = lotrsparam_n->VBULK;
    double vbx_u_n2 = lotrsparam_n->VBULK-MCC_VDDmax/2.0;

    double vbx_d_p1 = lotrsparam_p->VBULK-MCC_VDDmax;
    double vbx_d_p2 = lotrsparam_p->VBULK-MCC_VDDmax/2.0;
    double vbx_u_p1 = lotrsparam_p->VBULK;
    double vbx_u_p2 = lotrsparam_p->VBULK-MCC_VDDmax/2.0;
    double vgx = MCC_VDDmax/2.0;
    double v1, v2 ;
    int mcclog;
    //double cgpu,cgpd;

    if( lotrsparam_n->ISVBSSET )
      vbsn = lotrsparam_n->VBS ;
    if( lotrsparam_p->ISVBSSET )
      vbsp = lotrsparam_p->VBS ;

    mcclog =  ( avt_islog(2,LOGMCC) ) ? MCC_DRV_LOG:MCC_NO_LOG;

    if( (type == MCC_TRANS_N) || (type == MCC_TRANS_B) ) 
      MCC_SPICEMODELTYPE = mcc_getspicetechno( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6 );
    else
      MCC_SPICEMODELTYPE = mcc_getspicetechno( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6 );

    if ( avt_islog( 2, LOGMCC) ) 
      mcc_get_area_perim(); // obtention des aires et perim du .mccgenelp

	if ((type == MCC_TRANS_N) || (type == MCC_TRANS_B)) {

		MCC_NINDEX = mcc_gettransindex (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE,
										MCC_LN * 1.0e-6, MCC_WN * 1.0e-6);

        mbkfree(MCC_TNMODELTYPE);
		MCC_TNMODELTYPE = mcc_getmodelname (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE,
                                            MCC_LN * 1.0e-6, MCC_WN * 1.0e-6);

		if (MCC_TNMODELTYPE == NULL)
			MCC_TNMODELTYPE = mcc_initstr (MCC_TNMODEL);

		MCC_XWN = mcc_getXW (MCC_MODELFILE, MCC_TNMODEL,
							 MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6);

		MCC_XLN = mcc_getXL (MCC_MODELFILE, MCC_TNMODEL,
							 MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6);

		MCC_DLN = mcc_calcDL (MCC_MODELFILE, MCC_TNMODEL,
							  MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                              lotrsparam_n) * 1.0e6;

		MCC_DWN = mcc_calcDW (MCC_MODELFILE, MCC_TNMODEL,
							  MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                              MCC_WN * 1.0e-6, lotrsparam_n) * 1.0e6;

		MCC_DLCN = mcc_calcDLC (MCC_MODELFILE, MCC_TNMODEL,
								MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                lotrsparam_n) * 1.0e6;

		MCC_DWCN = mcc_calcDWC (MCC_MODELFILE, MCC_TNMODEL,
								MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6,
                                MCC_WN * 1.0e-6, lotrsparam_n) * 1.0e6;

		MCC_DWCJN = mcc_calcDWCJ(MCC_MODELFILE, MCC_TNMODEL,
								MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6,
                                MCC_WN * 1.0e-6,lotrsparam_n, MCC_TEMP) * 1.0e6;

		MCC_WNeff = MCC_XWN * MCC_WN + MCC_DWN;
		MCC_LNeff = MCC_XLN * MCC_LN + MCC_DLN;

		MCC_VTN = fabs (mcc_calcVTH (MCC_MODELFILE, MCC_TNMODEL,
									 MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                     MCC_TEMP, vbsn, MCC_VDDmax,lotrsparam_n, mcclog));
                MCC_VT0N = MCC_VTN ;

		vt = fabs (mcc_calcVTH (MCC_MODELFILE, MCC_TNMODEL,
								MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                MCC_TEMP, -MCC_VDDmax/4.0+vbsn, MCC_VDDmax,lotrsparam_n,MCC_NO_LOG));
        MCC_KTN = (vt - MCC_VTN)/(MCC_VDDmax/4.0) ;
        
		MCC_CGSUN = mcc_calcCGS( MCC_MODELFILE, 
                                         MCC_TNMODEL,
				  	 MCC_NMOS, 
                                         MCC_NCASE, 
                                         MCC_LN * 1.0e-6, 
                                         MCC_WN * 1.0e-6,
				  	 MCC_TEMP, 
                                         0.0, 
                                         MCC_VDDmax/2,
                                         MCC_VDDmax, 
                                         MCC_VDDmax,
                                         lotrsparam_n
                                       );
		MCC_CGSDN = mcc_calcCGS( MCC_MODELFILE, 
                                         MCC_TNMODEL,
				         MCC_NMOS, 
                                         MCC_NCASE, 
                                         MCC_LN * 1.0e-6, 
                                         MCC_WN * 1.0e-6,
				  	 MCC_TEMP, 
                                         MCC_VDDmax, 
                                         MCC_VDDmax/2,
                                         0.0,
                                         0.0,
                                         lotrsparam_n
                                       );
		MCC_CGSUFN = mcc_calcCGS( MCC_MODELFILE, 
                                          MCC_TNMODEL,
			 	   	  MCC_NMOS, 
                                          MCC_NCASE, 
                                          MCC_LN * 1.0e-6, 
                                          MCC_WN * 1.0e-6,
			 	  	  MCC_TEMP, 
                                          MCC_VDDmax/2,
                                          MCC_VDDmax,
                                          MCC_VDDmax, 
                                          MCC_VDDmax,
                                          lotrsparam_n
                                        );
		MCC_CGSDFN = mcc_calcCGS( MCC_MODELFILE, 
                                          MCC_TNMODEL,
				          MCC_NMOS, 
                                          MCC_NCASE, 
                                          MCC_LN * 1.0e-6, 
                                          MCC_WN * 1.0e-6,
				  	  MCC_TEMP, 
                                          MCC_VDDmax/2, 
                                          0.0,
                                          0.0,
                                          0.0,
                                          lotrsparam_n
                                        );
        MCC_CGSN = (MCC_CGSUN+MCC_CGSDN)/2.0;

		MCC_CGSU0N = mcc_calcCGS( MCC_MODELFILE, 
                                          MCC_TNMODEL,
				          MCC_NMOS, 
                                          MCC_NCASE, 
                                          MCC_LN * 1.0e-6, 
                                          MCC_WN * 1.0e-6,
				          MCC_TEMP, 
                                          0.0, 
                                          MCC_VDDmax/2,
                                          0.0, 
                                          0.0,
                                          lotrsparam_n
                                        );
		MCC_CGSD0N = mcc_calcCGS( MCC_MODELFILE, 
                                          MCC_TNMODEL,
				          MCC_NMOS, 
                                          MCC_NCASE, 
                                          MCC_LN * 1.0e-6, 
                                          MCC_WN * 1.0e-6,
				          MCC_TEMP, 
                                          MCC_VDDmax, 
                                          MCC_VDDmax/2,
                                          0.0,
                                          0.0,
                                          lotrsparam_n
                                        );
        MCC_CGS0N = (MCC_CGSU0N+MCC_CGSD0N)/2.0;

                if( MCC_VTN < MCC_VDDmax/2.0 ) {
                  v1 = MCC_VTN ;
                  v2 = MCC_VDDmax/2.0 ;
                }
                else {
                  v1 = MCC_VTN ;
                  v2 = MCC_VDDmax/2.0 ;
                }

		MCC_CGDN   = mcc_calcCGD( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, 0.0,     MCC_VDDmax, vbsn, MCC_VDDmax,lotrsparam_n );
                
		MCC_CGD0N  = mcc_calcCGD( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, 0.0, v1,         vbsn, MCC_VDDmax,lotrsparam_n );
                if( v2-v1>MCC_VDDmax/100.0 )
  		  MCC_CGD1N  = mcc_calcCGD( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, v1,  v2,         vbsn, MCC_VDDmax,lotrsparam_n );
                else
  		  MCC_CGD1N  = MCC_CGD0N ;
		MCC_CGD2N  = mcc_calcCGD( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, v2,  MCC_VDDmax, vbsn, MCC_VDDmax,lotrsparam_n );
                
		MCC_CGDCN  = mcc_calcCGD( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, 0.0, MCC_VDDmax, vbsn, 0.0,       lotrsparam_n );
		MCC_CGDC0N = mcc_calcCGD( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, 0.0, v1,         vbsn, 0.0,       lotrsparam_n );
                if( v2-v1>MCC_VDDmax/100.0 )
		  MCC_CGDC1N = mcc_calcCGD( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, v1,  v2,         vbsn, 0.0,       lotrsparam_n );
                else
		  MCC_CGDC1N = MCC_CGDC0N ;
		MCC_CGDC2N = mcc_calcCGD( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, v2,  MCC_VDDmax, vbsn, 0.0,       lotrsparam_n );

		MCC_CGSIN = mcc_calcCGSI (MCC_MODELFILE, MCC_TNMODEL,
								MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
								MCC_TEMP, MCC_VDDmax , vbsn, MCC_VDDmax,lotrsparam_n );

		MCC_CGSICN = mcc_calcCGSI (MCC_MODELFILE, MCC_TNMODEL,
                                 MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                 MCC_TEMP, MCC_VDDmax , vbsn, 0.0,lotrsparam_n );

		MCC_CGPN = mcc_calcCGP (MCC_MODELFILE, MCC_TNMODEL,
								MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                                MCC_WN * 1.0e-6, vgx, NULL,lotrsparam_n,MCC_TEMP) * 1.0e6;
                MCC_CGPO_N  = mcc_calcCGPO( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP,
                                           MCC_VDDmax, MCC_VDDmax, MCC_VDDmax/2.0, lotrsparam_n );
                MCC_CGPOC_N = mcc_calcCGPO( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP,
                                            0.0, 0.0, MCC_VDDmax/2.0, lotrsparam_n );

        // input capacitance 
        mcc_calcInputCapa ( MCC_NMOS, lotrsparam_n );

        /*----------------------------------------------------------------*\
         * NMOS : Junction (diffusion) capacitance
        \*----------------------------------------------------------------*/

                MCC_SWJUNCAPN = mcc_get_swjuncap( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN*1.0e-6, MCC_WN*1.0e-6 );

		MCC_CDS_U_N = mcc_calcCDS (MCC_MODELFILE, MCC_TNMODEL,
                                   MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                                   MCC_WN * 1.0e-6, MCC_TEMP, vbx_u_n1,vbx_u_n2,lotrsparam_n);

		MCC_CDS_D_N = mcc_calcCDS (MCC_MODELFILE, MCC_TNMODEL,
                                   MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                                   MCC_WN * 1.0e-6, MCC_TEMP, vbx_d_n1, vbx_d_n2,lotrsparam_n);

        MCC_CDSN = 0.5*(MCC_CDS_U_N+MCC_CDS_D_N);


        MCC_CDP_U_N = 1.0e6*mcc_calcCDP (MCC_MODELFILE, MCC_TNMODEL,
                                          MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6,
                                          MCC_WN * 1.0e-6, MCC_TEMP,
                                          vbx_u_n1, vbx_u_n2,lotrsparam_n);

        MCC_CDP_D_N = 1.0e6*mcc_calcCDP (MCC_MODELFILE, MCC_TNMODEL,
                                          MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                                          MCC_WN * 1.0e-6, MCC_TEMP,
                                          vbx_d_n1,vbx_d_n2,lotrsparam_n);

        MCC_CDPN = 0.5*(MCC_CDP_U_N+MCC_CDP_D_N);

        MCC_CDW_U_N = 1.0e6*mcc_calcCDW (MCC_MODELFILE, MCC_TNMODEL,
                                         MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                         MCC_TEMP, vbx_u_n1, vbx_u_n2, vgx,
                                         lotrsparam_n); 
        MCC_CBXGU = MCC_CBXG*1.0e6;

        MCC_CDW_D_N = 1.0e6*mcc_calcCDW (MCC_MODELFILE, MCC_TNMODEL,
                                         MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                         MCC_TEMP, vbx_d_n1, vbx_d_n2, vgx,
                                         lotrsparam_n); 
        MCC_CBXGD = MCC_CBXG*1.0e6;

        MCC_CDWN = 0.5*(MCC_CDW_U_N+MCC_CDW_D_N);

		MCC_CSS_U_N = mcc_calcCSS (MCC_MODELFILE, MCC_TNMODEL,
                                   MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                                   MCC_WN * 1.0e-6, MCC_TEMP, vbx_u_n1, vbx_u_n2,lotrsparam_n); 

		MCC_CSS_D_N = mcc_calcCSS (MCC_MODELFILE, MCC_TNMODEL,
                                   MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                                   MCC_WN * 1.0e-6, MCC_TEMP, vbx_d_n1,vbx_d_n2,lotrsparam_n);

        MCC_CSSN = 0.5*(MCC_CSS_U_N+MCC_CSS_D_N);

        MCC_CSP_U_N = 1.0e6*mcc_calcCSP (MCC_MODELFILE, MCC_TNMODEL,
                                         MCC_NMOS, MCC_NCASE, 
                                         MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                         MCC_TEMP, vbx_u_n1, vbx_u_n2, lotrsparam_n);

        MCC_CSP_D_N = 1.0e6*mcc_calcCSP (MCC_MODELFILE, MCC_TNMODEL,
                                         MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                         MCC_TEMP, vbx_d_n1, vbx_d_n2, lotrsparam_n);

        MCC_CSPN = 0.5*(MCC_CSP_U_N+MCC_CSP_D_N);


        MCC_CSW_U_N = 1.0e6*mcc_calcCSW (MCC_MODELFILE, MCC_TNMODEL,
                                         MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                         MCC_TEMP, vbx_u_n1, vbx_u_n2, vgx,
                                         lotrsparam_n); 

        MCC_CSW_D_N = 1.0e6*mcc_calcCSW (MCC_MODELFILE, MCC_TNMODEL,
                                         MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                                         MCC_TEMP, vbx_d_n1, vbx_d_n2, vgx,
                                         lotrsparam_n); 

        MCC_CSWN = 0.5*(MCC_CSW_U_N+MCC_CSW_D_N);

		MCC_LNMAX = mcc_getLMAX (MCC_MODELFILE, MCC_TNMODEL,
								 MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6);

		if (MCC_LNMAX < (double)MCC_LWMAX)
			MCC_LNMAX = MCC_LNMAX * 1.0e6;

		MCC_LNMIN = mcc_getLMIN (MCC_MODELFILE, MCC_TNMODEL,
								 MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6);

		if (MCC_LNMIN < (double)MCC_LWMAX)
			MCC_LNMIN = MCC_LNMIN * 1.0e6;

		MCC_WNMAX = mcc_getWMAX (MCC_MODELFILE, MCC_TNMODEL,
								 MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6);

		if (MCC_WNMAX < (double)MCC_LWMAX)
			MCC_WNMAX = MCC_WNMAX * 1.0e6;

		MCC_WNMIN = mcc_getWMIN (MCC_MODELFILE, MCC_TNMODEL,
								 MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6);

		if (MCC_WNMIN < (double)MCC_LWMAX)
			MCC_WNMIN = MCC_WNMIN * 1.0e6;
	}

	if ((type == MCC_TRANS_P) || (type == MCC_TRANS_B)) {
		MCC_PINDEX = mcc_gettransindex (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE,
										MCC_LP * 1.0e-6, MCC_WP * 1.0e-6);

        mbkfree(MCC_TPMODELTYPE);
		MCC_TPMODELTYPE = mcc_getmodelname (MCC_MODELFILE, 
											MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6);

		if (MCC_TPMODELTYPE == NULL)
			MCC_TPMODELTYPE = mcc_initstr (MCC_TPMODEL);

		MCC_XWP = mcc_getXW (MCC_MODELFILE, MCC_TPMODEL,
							 MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6);

		MCC_XLP = mcc_getXL (MCC_MODELFILE, MCC_TPMODEL,
							 MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6);

		MCC_DLP = mcc_calcDL (MCC_MODELFILE, MCC_TPMODEL,
							  MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                              lotrsparam_p) * 1.0e6;

		MCC_DWP = mcc_calcDW (MCC_MODELFILE, MCC_TPMODEL,
							  MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                              MCC_WP * 1.0e-6,lotrsparam_p) * 1.0e6;

		MCC_DLCP = mcc_calcDLC (MCC_MODELFILE, MCC_TPMODEL,
								MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                                lotrsparam_p) * 1.0e6;

		MCC_DWCP = mcc_calcDWC (MCC_MODELFILE, MCC_TPMODEL,
								MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6,
                                MCC_WP * 1.0e-6,lotrsparam_p) * 1.0e6;

		MCC_DWCJP = mcc_calcDWCJ (MCC_MODELFILE, MCC_TPMODEL,
								MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                                MCC_WP * 1.0e-6,lotrsparam_p, MCC_TEMP) * 1.0e6;

		MCC_WPeff = MCC_XWP * MCC_WP + MCC_DWP;
		MCC_LPeff = MCC_XLP * MCC_LP + MCC_DLP;

		MCC_VTP = fabs (mcc_calcVTH (MCC_MODELFILE, MCC_TPMODEL,
									 MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, 
                                     MCC_TEMP, vbsp, -MCC_VDDmax,lotrsparam_p,mcclog));
                MCC_VT0P = MCC_VTP ;

		vt = fabs (mcc_calcVTH (MCC_MODELFILE, MCC_TPMODEL,
								MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, 
                                MCC_TEMP, MCC_VDDmax/4.0+vbsp, -MCC_VDDmax,lotrsparam_p,MCC_NO_LOG));
        MCC_KTP = (vt - MCC_VTP)/(MCC_VDDmax/4.0) ;

        MCC_CGSUP = mcc_calcCGS( MCC_MODELFILE, 
                                 MCC_TPMODEL,
                                 MCC_PMOS, 
                                 MCC_PCASE, 
                                 MCC_LP * 1.0e-6, 
                                 MCC_WP * 1.0e-6,
                                 MCC_TEMP, 
                                 -MCC_VDDmax,
                                 -MCC_VDDmax/2, 
                                 0.0,
                                 0.0,
                                 lotrsparam_p
                               );

	MCC_CGSDP = mcc_calcCGS( MCC_MODELFILE, 
                                 MCC_TPMODEL,
                                 MCC_PMOS, 
                                 MCC_PCASE, 
                                 MCC_LP * 1.0e-6, 
                                 MCC_WP * 1.0e-6,
                                 MCC_TEMP, 
                                 0.0,
                                 -MCC_VDDmax/2,
                                 -MCC_VDDmax,
                                 -MCC_VDDmax,
                                 lotrsparam_p
                               );
        MCC_CGSUFP = mcc_calcCGS( MCC_MODELFILE, 
                                  MCC_TPMODEL,
                                  MCC_PMOS, 
                                  MCC_PCASE, 
                                  MCC_LP * 1.0e-6, 
                                  MCC_WP * 1.0e-6,
                                  MCC_TEMP, 
                                  -MCC_VDDmax/2, 
                                  0.0,
                                  0.0,
                                  0.0,
                                  lotrsparam_p
                                );

	MCC_CGSDFP = mcc_calcCGS( MCC_MODELFILE, 
                                  MCC_TPMODEL,
                                  MCC_PMOS, 
                                  MCC_PCASE, 
                                  MCC_LP * 1.0e-6, 
                                  MCC_WP * 1.0e-6,
                                  MCC_TEMP, 
                                  -MCC_VDDmax/2,
                                  -MCC_VDDmax,
                                  -MCC_VDDmax,
                                  -MCC_VDDmax,
                                  lotrsparam_p
                                );
        MCC_CGSP = (MCC_CGSUP+MCC_CGSDP)/2.0;
        
        MCC_CGSU0P = mcc_calcCGS( MCC_MODELFILE, 
                                  MCC_TPMODEL,
                                  MCC_PMOS, 
                                  MCC_PCASE, 
                                  MCC_LP * 1.0e-6, 
                                  MCC_WP * 1.0e-6,
                                  MCC_TEMP, 
                                  -MCC_VDDmax,
                                  -MCC_VDDmax/2, 
                                  0.0,
                                  0.0,
                                  lotrsparam_p
                                );

	MCC_CGSD0P = mcc_calcCGS( MCC_MODELFILE, 
                                  MCC_TPMODEL,
                                  MCC_PMOS, 
                                  MCC_PCASE, 
                                  MCC_LP * 1.0e-6, 
                                  MCC_WP * 1.0e-6,
                                  MCC_TEMP, 
                                  0.0,
                                  -MCC_VDDmax/2,
                                  0.0,
                                  0.0,
                                  lotrsparam_p
                                );
        MCC_CGS0P = (MCC_CGSU0P+MCC_CGSD0P)/2.0;
        
        if( MCC_VTP < MCC_VDDmax/2.0 ) {
          v1 = MCC_VTP ;
          v2 = MCC_VDDmax/2.0 ;
        }
        else {
          v1 = MCC_VTP ;
          v2 = MCC_VDDmax/2.0 ;
        }
        MCC_CGDP   = mcc_calcCGD( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, 0.0, -MCC_VDDmax, vbsp, -MCC_VDDmax, lotrsparam_p );
        MCC_CGD0P  = mcc_calcCGD( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, 0.0, -v1,         vbsp, -MCC_VDDmax, lotrsparam_p );
        if( v2-v1>MCC_VDDmax/100.0 )
          MCC_CGD1P  = mcc_calcCGD( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, -v1,  -v2,         vbsp, -MCC_VDDmax, lotrsparam_p );
        else
          MCC_CGD1P  = MCC_CGD0P ;
        MCC_CGD2P  = mcc_calcCGD( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, -v2,  -MCC_VDDmax, vbsp, -MCC_VDDmax, lotrsparam_p );
        
        MCC_CGDCP  = mcc_calcCGD( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, 0.0, -MCC_VDDmax, vbsp, 0.0,        lotrsparam_p );
        MCC_CGDC0P = mcc_calcCGD( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, 0.0, -v1,         vbsp, 0.0,        lotrsparam_p );
        if( v2-v1>MCC_VDDmax/100.0 )
          MCC_CGDC1P = mcc_calcCGD( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, -v1,  -v2,         vbsp, 0.0,        lotrsparam_p );
        else
          MCC_CGDC1P = MCC_CGDC0P ;
        MCC_CGDC2P = mcc_calcCGD( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, -v2,  -MCC_VDDmax, vbsp, 0.0,        lotrsparam_p );

        MCC_CGSIP = mcc_calcCGSI (MCC_MODELFILE, MCC_TPMODEL,
                                  MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                                  MCC_TEMP, -MCC_VDDmax , vbsp, -MCC_VDDmax,lotrsparam_p );

        MCC_CGSICP = mcc_calcCGSI (MCC_MODELFILE, MCC_TPMODEL,
                                   MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                                   MCC_TEMP, -MCC_VDDmax , vbsp, 0.0,lotrsparam_p );

       MCC_CGPP = mcc_calcCGP (MCC_MODELFILE, MCC_TPMODEL,
                               MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                               MCC_WP * 1.0e-6, -vgx, NULL,lotrsparam_p,MCC_TEMP) * 1.0e6;
       MCC_CGPO_P  = mcc_calcCGPO( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP,
                                   0.0, 0.0, -MCC_VDDmax/2.0, lotrsparam_p );
       MCC_CGPOC_P = mcc_calcCGPO( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP,
                                   -MCC_VDDmax, -MCC_VDDmax, -MCC_VDDmax/2.0, lotrsparam_n );
        // input capacitance 
        mcc_calcInputCapa ( MCC_PMOS, lotrsparam_p );

        /*----------------------------------------------------------------*\
         * PMOS : Junction (diffusion) capacitance
        \*----------------------------------------------------------------*/

                MCC_SWJUNCAPP = mcc_get_swjuncap( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP*1.0e-6, MCC_WP*1.0e-6 );

		MCC_CDS_U_P = mcc_calcCDS (MCC_MODELFILE, MCC_TPMODEL,
                                   MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                                   MCC_WP * 1.0e-6, MCC_TEMP, vbx_u_p1,vbx_u_p2,lotrsparam_p);

		MCC_CDS_D_P = mcc_calcCDS (MCC_MODELFILE, MCC_TPMODEL,
                                   MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                                   MCC_WP * 1.0e-6, MCC_TEMP, vbx_d_p1, vbx_d_p2,lotrsparam_p);

        MCC_CDSP = 0.5*(MCC_CDS_U_P+MCC_CDS_D_P);


        MCC_CDP_U_P = 1.0e6*mcc_calcCDP (MCC_MODELFILE, MCC_TPMODEL,
                                          MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6,
                                          MCC_WP * 1.0e-6, MCC_TEMP,
                                          vbx_u_p1, vbx_u_p2,lotrsparam_p);

        MCC_CDP_D_P = 1.0e6*mcc_calcCDP (MCC_MODELFILE, MCC_TPMODEL,
                                          MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                                          MCC_WP * 1.0e-6, MCC_TEMP,
                                          vbx_d_p1,vbx_d_p2,lotrsparam_p);

        MCC_CDPP = 0.5*(MCC_CDP_U_P+MCC_CDP_D_P);

        MCC_CDW_U_P = 1.0e6*mcc_calcCDW (MCC_MODELFILE, MCC_TPMODEL,
                                         MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                                         MCC_TEMP, vbx_u_p1, vbx_u_p2, -vgx,
                                         lotrsparam_p); 
        MCC_CBXGU = MCC_CBXG*1.0e6;

        MCC_CDW_D_P = 1.0e6*mcc_calcCDW (MCC_MODELFILE, MCC_TPMODEL,
                                         MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                                         MCC_TEMP, vbx_d_p1, vbx_d_p2, -vgx,
                                         lotrsparam_p); 
        MCC_CBXGD = MCC_CBXG*1.0e6;

        MCC_CDWP = 0.5*(MCC_CDW_U_P+MCC_CDW_D_P);

		MCC_CSS_U_P = mcc_calcCSS (MCC_MODELFILE, MCC_TPMODEL,
                                   MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                                   MCC_WP * 1.0e-6, MCC_TEMP, vbx_u_p1, vbx_u_p2,lotrsparam_p); 

		MCC_CSS_D_P = mcc_calcCSS (MCC_MODELFILE, MCC_TPMODEL,
                                   MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                                   MCC_WP * 1.0e-6, MCC_TEMP, vbx_d_p1,vbx_d_p2,lotrsparam_p);

        MCC_CSSP = 0.5*(MCC_CSS_U_P+MCC_CSS_D_P);

        MCC_CSP_U_P = 1.0e6*mcc_calcCSP (MCC_MODELFILE, MCC_TPMODEL,
                                         MCC_PMOS, MCC_PCASE, 
                                         MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                                         MCC_TEMP, vbx_u_p1, vbx_u_p2, lotrsparam_p);

        MCC_CSP_D_P = 1.0e6*mcc_calcCSP (MCC_MODELFILE, MCC_TPMODEL,
                                         MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                                         MCC_TEMP, vbx_d_p1, vbx_d_p2, lotrsparam_p);

        MCC_CSPP = 0.5*(MCC_CSP_U_P+MCC_CSP_D_P);


        MCC_CSW_U_P = 1.0e6*mcc_calcCSW (MCC_MODELFILE, MCC_TPMODEL,
                                         MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                                         MCC_TEMP, vbx_u_p1, vbx_u_p2, -vgx,
                                         lotrsparam_p); 

        MCC_CSW_D_P = 1.0e6*mcc_calcCSW (MCC_MODELFILE, MCC_TPMODEL,
                                         MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                                         MCC_TEMP, vbx_d_p1, vbx_d_p2, -vgx,
                                         lotrsparam_p); 

        MCC_CSWP = 0.5*(MCC_CSW_U_P+MCC_CSW_D_P);

		MCC_LPMAX = mcc_getLMAX (MCC_MODELFILE, MCC_TPMODEL,
								 MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6);

		if (MCC_LPMAX < (double)MCC_LWMAX)
			MCC_LPMAX = MCC_LPMAX * 1.0e6;

		MCC_LPMIN = mcc_getLMIN (MCC_MODELFILE, MCC_TPMODEL,
								 MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6);

		if (MCC_LPMIN < (double)MCC_LWMAX)
			MCC_LPMIN = MCC_LPMIN * 1.0e6;

		MCC_WPMAX = mcc_getWMAX (MCC_MODELFILE, MCC_TPMODEL,
								 MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6);

		if (MCC_WPMAX < (double)MCC_LWMAX)
			MCC_WPMAX = MCC_WPMAX * 1.0e6;

		MCC_WPMIN = mcc_getWMIN (MCC_MODELFILE, MCC_TPMODEL,
								 MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6);

		if (MCC_WPMIN < (double)MCC_LWMAX)
			MCC_WPMIN = MCC_WPMIN * 1.0e6;
	}
	mcc_modifyrange ();
}

void mcc_fclose (file, filename)
FILE *file;
char *filename;
{
	if (fclose (file) != 0) {
		fprintf (stderr, "\nmcc error : can't open file %s\n", filename);
		EXIT (1);
	}
}

FILE *mcc_fopen (filename, type)
char *filename;
char *type;
{
	FILE *file;

	file = fopen (filename, type);

	if ((file == NULL) && (strcmp (type, "w") == 0)) {
		fprintf (stderr, "\nmcc error : can't open file %s\n", filename);
		EXIT (1);
	}

	return (file);
}

void mcc_drvparam (lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
	FILE *file;

	file = mcc_fopen (mcc_debug_prefix(".mccgenelp"), "w");

	fprintf (file, "TECSIZE = %g\n", MCC_TECSIZE);
	fprintf (file, "AN = %g\n", MCC_AN);
	fprintf (file, "BN = %g\n", MCC_BN);
	fprintf (file, "AP = %g\n", MCC_AP);
	fprintf (file, "BP = %g\n", MCC_BP);
	fprintf (file, "RNT = %g\n", MCC_RNT);
	fprintf (file, "RPT = %g\n", MCC_RPT);
	fprintf (file, "RNS = %g\n", MCC_RNS);
	fprintf (file, "RPS = %g\n", MCC_RPS);
	fprintf (file, "VDD = %g\n", MCC_VDDmax);
	fprintf (file, "VDDdeg = %g\n", MCC_VDDdeg);
	fprintf (file, "VSSdeg = %g\n", MCC_VSSdeg);
	fprintf (file, "VTIN = %g\n", MCC_VTIN);
	fprintf (file, "VTIP = %g\n", MCC_VTIP);
	fprintf (file, "KSN = %g\n", MCC_KSN);
	fprintf (file, "KSP = %g\n", MCC_KSP);
	fprintf (file, "KRN = %g\n", MCC_KRN);
	fprintf (file, "KRP = %g\n", MCC_KRP);
	fprintf (file, "VTN = %g\n", MCC_VTN);
	fprintf (file, "VTP = %g\n", MCC_VTP);
    if ( lotrsparam_n ) {
	  fprintf (file, "DELVTON = %g\n", lotrsparam_n->PARAM[elpDELVT0]);
	  fprintf (file, "MULUON = %g\n", lotrsparam_n->PARAM[elpMULU0]);
      if ( lotrsparam_n->PARAM[elpM] > ELPMINVALUE )
	    fprintf (file, "MN = %g\n", lotrsparam_n->PARAM[elpM]);
      if ( lotrsparam_n->PARAM[elpNRS] > ELPMINVALUE )
	    fprintf (file, "NRSN = %g\n", lotrsparam_n->PARAM[elpNRS]);
      if ( lotrsparam_n->PARAM[elpNRD] > ELPMINVALUE )
	    fprintf (file, "NRDN = %g\n", lotrsparam_n->PARAM[elpNRD]);
      if ( lotrsparam_n->PARAM[elpSA] > ELPMINVALUE )
	    fprintf (file, "SAN = %g\n", lotrsparam_n->PARAM[elpSA]);
      if ( lotrsparam_n->PARAM[elpSB] > ELPMINVALUE )
	    fprintf (file, "SBN = %g\n", lotrsparam_n->PARAM[elpSB]);
      if ( lotrsparam_n->PARAM[elpSD] > ELPMINVALUE )
	    fprintf (file, "SDN = %g\n", lotrsparam_n->PARAM[elpSD]);
      if ( lotrsparam_n->PARAM[elpNF] > ELPMINVALUE )
	    fprintf (file, "NFN = %g\n", lotrsparam_n->PARAM[elpNF]);
      if ( lotrsparam_n->VBULK > ELPMINVBULK )
	    fprintf (file, "VBULKN = %g\n", lotrsparam_n->VBULK);
      else
	    fprintf (file, "VBULKN = %g\n", 0.0);
    }
    if ( lotrsparam_p ) {
      fprintf (file, "DELVTOP = %g\n", lotrsparam_p->PARAM[elpDELVT0]);
      fprintf (file, "MULUOP = %g\n", lotrsparam_p->PARAM[elpMULU0]);
      if ( lotrsparam_p->PARAM[elpM] > ELPMINVALUE )
	    fprintf (file, "MP = %g\n", lotrsparam_p->PARAM[elpM]);
      if ( lotrsparam_p->PARAM[elpNRS] > ELPMINVALUE )
	    fprintf (file, "NRSP = %g\n", lotrsparam_p->PARAM[elpNRS]);
      if ( lotrsparam_p->PARAM[elpNRD] > ELPMINVALUE )
	    fprintf (file, "NRDP = %g\n", lotrsparam_p->PARAM[elpNRD]);
      if ( lotrsparam_p->PARAM[elpSA] > ELPMINVALUE )
	    fprintf (file, "SAP = %g\n", lotrsparam_p->PARAM[elpSA]);
      if ( lotrsparam_p->PARAM[elpSB] > ELPMINVALUE )
	    fprintf (file, "SBP = %g\n", lotrsparam_p->PARAM[elpSB]);
      if ( lotrsparam_p->PARAM[elpSD] > ELPMINVALUE )
	    fprintf (file, "SDP = %g\n", lotrsparam_p->PARAM[elpSD]);
      if ( lotrsparam_p->PARAM[elpNF] > ELPMINVALUE )
	    fprintf (file, "NFP = %g\n", lotrsparam_p->PARAM[elpNF]);
      if ( lotrsparam_p->VBULK > ELPMINVBULK )
        fprintf (file, "VBULKP = %g\n", lotrsparam_p->VBULK);
      else 
        fprintf (file, "VBULKP = %g\n", MCC_VDDmax);
    }
	fprintf (file, "KTN = %g\n", MCC_KTN);
	fprintf (file, "KTP = %g\n", MCC_KTP);
	fprintf (file, "WN = %g\n", MCC_WN);
	fprintf (file, "LN = %g\n", MCC_LN);
	fprintf (file, "WP = %g\n", MCC_WP);
	fprintf (file, "LP = %g\n", MCC_LP);
    if ( MCC_ASN > 0.0 )
	  fprintf (file, "ASN = %g\n", MCC_ASN);
    if ( MCC_ADN > 0.0 )
	  fprintf (file, "ADN = %g\n", MCC_ADN);
    if ( MCC_PSN > 0.0 )
	  fprintf (file, "PSN = %g\n", MCC_PSN);
    if ( MCC_PDN > 0.0 )
	  fprintf (file, "PDN = %g\n", MCC_PDN);
    if ( MCC_ASP > 0.0 )
	  fprintf (file, "ASP = %g\n", MCC_ASP);
    if ( MCC_ADP > 0.0 )
	  fprintf (file, "ADP = %g\n", MCC_ADP);
    if ( MCC_PSP > 0.0 )
	  fprintf (file, "PSP = %g\n", MCC_PSP);
    if ( MCC_PDP > 0.0 )
	  fprintf (file, "PDP = %g\n", MCC_PDP);
	fprintf (file, "LNMAX = %g\n", MCC_LNMAX);
	fprintf (file, "WNMIN = %g\n", MCC_WNMIN);
	fprintf (file, "WNMAX = %g\n", MCC_WNMAX);
	fprintf (file, "LPMIN = %g\n", MCC_LPMIN);
	fprintf (file, "LPMAX = %g\n", MCC_LPMAX);
	fprintf (file, "WPMIN = %g\n", MCC_WPMIN);
	fprintf (file, "WPMAX = %g\n", MCC_WPMAX);
	fprintf (file, "DWN = %g\n", MCC_DWN);
	fprintf (file, "DLN = %g\n", MCC_DLN);
	fprintf (file, "DWP = %g\n", MCC_DWP);
	fprintf (file, "DLP = %g\n", MCC_DLP);
	fprintf (file, "DWCN = %g\n", MCC_DWCN);
	fprintf (file, "DWCJN = %g\n", MCC_DWCJN);
	fprintf (file, "DLCN = %g\n", MCC_DLCN);
	fprintf (file, "DWCP = %g\n", MCC_DWCP);
	fprintf (file, "DWCJP = %g\n", MCC_DWCJP);
	fprintf (file, "DLCP = %g\n", MCC_DLCP);
	fprintf (file, "DIFF = %g\n", MCC_DIF);
	fprintf (file, "XWN = %g\n", MCC_XWN);
	fprintf (file, "XLN = %g\n", MCC_XLN);
	fprintf (file, "XWP = %g\n", MCC_XWP);
	fprintf (file, "XLP = %g\n", MCC_XLP);
	fprintf (file, "CGSN = %g\n", MCC_CGSN);
	fprintf (file, "CGSUN = %g\n", MCC_CGSUN);
	fprintf (file, "CGSUFN = %g\n", MCC_CGSUFN);
	fprintf (file, "CGSDN = %g\n", MCC_CGSDN);
	fprintf (file, "CGSDFN = %g\n", MCC_CGSDFN);
	fprintf (file, "CGSP = %g\n", MCC_CGSP);
	fprintf (file, "CGSUP = %g\n", MCC_CGSUP);
	fprintf (file, "CGSUFP = %g\n", MCC_CGSUFP);
	fprintf (file, "CGSDP = %g\n", MCC_CGSDP);
	fprintf (file, "CGSDFP = %g\n", MCC_CGSDFP);
	fprintf (file, "CGDN = %g\n", MCC_CGDN);
	fprintf (file, "CGDP = %g\n", MCC_CGDP);
	fprintf (file, "CGDCN = %g\n", MCC_CGDCN);
	fprintf (file, "CGDCP = %g\n", MCC_CGDCP);
	fprintf (file, "CGSIN = %g\n", MCC_CGSIN);
	fprintf (file, "CGSIP = %g\n", MCC_CGSIP);
	fprintf (file, "CGSICN = %g\n", MCC_CGSICN);
	fprintf (file, "CGSICP = %g\n", MCC_CGSICP);
	fprintf (file, "CGPN = %g\n", MCC_CGPN);
	fprintf (file, "CGPP = %g\n", MCC_CGPP);
	fprintf (file, "CDSN = %g\n", MCC_CDSN);
	fprintf (file, "CDSP = %g\n", MCC_CDSP);
	fprintf (file, "CDPN = %g\n", MCC_CDPN);
	fprintf (file, "CDPP = %g\n", MCC_CDPP);
	fprintf (file, "CDWN = %g\n", MCC_CDWN);
	fprintf (file, "CDWP = %g\n", MCC_CDWP);
	fprintf (file, "CSSN = %g\n", MCC_CSSN);
	fprintf (file, "CSSP = %g\n", MCC_CSSP);
	fprintf (file, "CSPN = %g\n", MCC_CSPN);
	fprintf (file, "CSPP = %g\n", MCC_CSPP);
	fprintf (file, "CSWN = %g\n", MCC_CSWN);
	fprintf (file, "CSWP = %g\n", MCC_CSWP);
	fprintf (file, "VGS = %g\n", MCC_VGS);
	fprintf (file, "TEMP = %g\n", MCC_TEMP);
	fprintf (file, "SLOPE = %g\n", MCC_SLOPE);
	fprintf (file, "TIME = %g\n", V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9 );
	fprintf (file, "TRANSTEP = %g\n", V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9 );
	fprintf (file, "DCSTEP = %g\n", MCC_DC_STEP);
	fprintf (file, "ERROR = %g\n", MCC_ERROR);
	fprintf (file, "CAPA = %g\n", MCC_CAPA);
	fprintf (file, "ELPNAME = %s\n", MCC_ELPFILE);
	fprintf (file, "TECHNAME = %s\n", MCC_TECHFILE);
	fprintf (file, "MODELNAME = %s\n", MCC_MODELFILE);
	fprintf (file, "SPICESTRING = %s\n", MCC_SPICESTRING);
	fprintf (file, "SPICETOOL = %s\n", MCC_SPICENAME);
    if(MCC_SPICEOPTIONS != NULL)
	fprintf (file, "SPICEOPTIONS = %s\n", MCC_SPICEOPTIONS);
	fprintf (file, "TASTOOL = %s\n", MCC_TASNAME);
	fprintf (file, "SPICEOUT = %s\n", MCC_SPICEOUT);
	fprintf (file, "SPICESTDOUT = %s\n", MCC_SPICESTDOUT);
	fprintf (file, "TNMODELNAME = %s\n", MCC_TNMODEL);
	fprintf (file, "NINDEX = %d\n", MCC_NINDEX);
	fprintf (file, "TNMODELTYPE = %s\n", MCC_TNMODELTYPE);
	fprintf (file, "NCASE = %s\n", MCC_CASE_NAME[MCC_NCASE]);
	fprintf (file, "TPMODELNAME = %s\n", MCC_TPMODEL);
	fprintf (file, "PINDEX = %d\n", MCC_PINDEX);
	fprintf (file, "TPMODELTYPE = %s\n", MCC_TPMODELTYPE);
	fprintf (file, "PCASE = %s\n", MCC_CASE_NAME[MCC_PCASE]);
	fprintf (file, "ELPVERSION = %s\n", MCC_ELPVERSION);
	if (MCC_SPICEMODELTYPE >= 0)
		fprintf (file, "SPICEMODELTYPE = %s\n", MCC_MOD_NAME[MCC_SPICEMODELTYPE]);
	else
		fprintf (file, "SPICEMODELTYPE = UNKNOWN\n");
	fprintf (file, "NBMEASURE = %d\n", MCC_INSNUMB);
	fprintf (file, "NBDIFF = %d\n", MCC_TRANSNUMB);
	fprintf (file, "NBGATE = %d\n", MCC_INVNUMB);
	fprintf (file, "NBFITLOOP = %d\n", MCC_NBMAXLOOP);

	mcc_fclose (file, ".mccgenelp");
}

int mcc_prsparam (lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
	FILE *file;
	char buf[1024];
	char *name;
	char *param;
	char *pt;
	int res = 1;
	int i;
	double d;
	float f;

	file = mcc_fopen (".mccgenelp", "r");

	if (file == NULL)
		return (0);

	while (fgets (buf, 1024, file) != NULL) {
		buf[1023] = '\0';
		name = buf;
		while ((isspace ((int)(*name)) != 0) && (*name != '\0'))
			name++;
		if (isalpha ((int)(*name)) == 0)
			continue;
		else {
			param = name;
			while (isalpha ((int)(*param)) != 0)
				param++;
			pt = strchr (param, '=');
			if (pt == NULL) {
				res = 0;
			}
			else {
				*param = '\0';
				param++;
				while (((isspace ((int)(*param)) != 0) || (*param == '='))
					   && (*param != '\0'))
					param++;
				pt = param;
				while ((isspace ((int)(*pt)) == 0) && (*pt != '\0'))
					pt++;
				if ((strcmp (name, "SPICESTRING") == 0) ||
                    (strcmp (name, "SPICEOPTIONS") == 0)) {
					while ((*pt != '\0') && (*pt != '\n'))
						pt++;
				}
				*pt = '\0';
			}
		}
		if (res != 0) {
			if (strcmp (name, "TECSIZE") == 0) {
				d = (double)atof (param);
				MCC_TECSIZE = d;
				SIM_TECSIZE = d;
			}
			else if (strcmp (name, "AN") == 0) {
				d = (double)atof (param);
				MCC_AN = d;
			}
			else if (strcmp (name, "BN") == 0) {
				d = (double)atof (param);
				MCC_BN = d;
			}
			else if (strcmp (name, "AP") == 0) {
				d = (double)atof (param);
				MCC_AP = d;
			}
			else if (strcmp (name, "BP") == 0) {
				d = (double)atof (param);
				MCC_BP = d;
			}
			else if (strcmp (name, "RNT") == 0) {
				d = (double)atof (param);
				MCC_RNT = d;
			}
			else if (strcmp (name, "RPT") == 0) {
				d = (double)atof (param);
				MCC_RPT = d;
			}
			else if (strcmp (name, "RNS") == 0) {
				d = (double)atof (param);
				MCC_RNS = d;
			}
			else if (strcmp (name, "RPS") == 0) {
				d = (double)atof (param);
				MCC_RPS = d;
			}
			else if (strcmp (name, "KSN") == 0) {
				d = (double)atof (param);
				MCC_KSN = d;
			}
			else if (strcmp (name, "KSP") == 0) {
				d = (double)atof (param);
				MCC_KSP = d;
			}
			else if (strcmp (name, "KRN") == 0) {
				d = (double)atof (param);
				MCC_KRN = d;
			}
			else if (strcmp (name, "KRP") == 0) {
				d = (double)atof (param);
				MCC_KRP = d;
			}
			else if (strcmp (name, "VDD") == 0) {
				d = (double)atof (param);
				MCC_VDDmax = d;
				V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE = d;
			}
			else if (strcmp (name, "VDDdeg") == 0) {
				d = (double)atof (param);
				MCC_VDDdeg = d;
			}
			else if (strcmp (name, "VSSdeg") == 0) {
				d = (double)atof (param);
				MCC_VSSdeg = d;
			}
			else if (strcmp (name, "VTIN") == 0) {
				d = (double)atof (param);
				MCC_VTIN= d;
			}
			else if (strcmp (name, "VTIP") == 0) {
				d = (double)atof (param);
				MCC_VTIP= d;
			}
			else if (strcmp (name, "VTP") == 0) {
				d = (double)atof (param);
				MCC_VTP = d;
			}
			else if (strcmp (name, "VTN") == 0) {
				d = (double)atof (param);
				MCC_VTN = d;
			}
			else if (lotrsparam_n && (strcmp (name, "MN") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->PARAM[elpM]= d;
			}
			else if (lotrsparam_n && (strcmp (name, "NRSN") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->PARAM[elpNRS]= d;
			}
			else if (lotrsparam_n && (strcmp (name, "NRDN") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->PARAM[elpNRD]= d;
			}
			else if (lotrsparam_p && (strcmp (name, "MP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->PARAM[elpM]= d;
			}
			else if (lotrsparam_p && (strcmp (name, "NRSP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->PARAM[elpNRS]= d;
			}
			else if (lotrsparam_p && (strcmp (name, "NRDP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->PARAM[elpNRD]= d;
			}
			else if (lotrsparam_n && (strcmp (name, "VBULKN") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->VBULK = d;
				MCC_VBULKN = d;
			}
			else if (lotrsparam_n && (strcmp (name, "MULUON") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->PARAM[elpMULU0] = d;
			}
			else if ((lotrsparam_n && strcmp (name, "DELVTON") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->PARAM[elpDELVT0] = d;
			}
			else if ((lotrsparam_n && strcmp (name, "SAN") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->PARAM[elpSA] = d;
			}
			else if ((lotrsparam_n && strcmp (name, "SBN") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->PARAM[elpSB] = d;
			}
			else if ((lotrsparam_n && strcmp (name, "SDN") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->PARAM[elpSD] = d;
			}
			else if ((lotrsparam_n && strcmp (name, "NFN") == 0)) {
				d = (double)atof (param);
				lotrsparam_n->PARAM[elpNF] = d;
			}
			else if ((lotrsparam_p && strcmp (name, "SAP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->PARAM[elpSA] = d;
			}
			else if ((lotrsparam_p && strcmp (name, "SBP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->PARAM[elpSB] = d;
			}
			else if ((lotrsparam_p && strcmp (name, "SDP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->PARAM[elpSD] = d;
			}
			else if ((lotrsparam_p && strcmp (name, "NFP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->PARAM[elpNF] = d;
			}
			else if (lotrsparam_p && (strcmp (name, "MULUOP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->PARAM[elpMULU0] = d;
			}
			else if (lotrsparam_p && (strcmp (name, "DELVTOP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->PARAM[elpDELVT0] = d;
			}
			else if (lotrsparam_p && (strcmp (name, "VBULKP") == 0)) {
				d = (double)atof (param);
				lotrsparam_p->VBULK = d;
				MCC_VBULKP = d;
			}
			else if (strcmp (name, "KTN") == 0) {
				d = (double)atof (param);
				MCC_KTN = d;
			}
			else if (strcmp (name, "KTP") == 0) {
				d = (double)atof (param);
				MCC_KTP = d;
			}
			else if (strcmp (name, "WN") == 0) {
				d = (double)atof (param);
				MCC_WN = d;
			}
			else if (strcmp (name, "LN") == 0) {
				d = (double)atof (param);
				MCC_LN = d;
			}
			else if (strcmp (name, "WP") == 0) {
				d = (double)atof (param);
				MCC_WP = d;
			}
			else if (strcmp (name, "LP") == 0) {
				d = (double)atof (param);
				MCC_LP = d;
			}
            /*------------------------------------*\
             * AREA 
            \*------------------------------------*/
			else if (strcmp (name, "ASN") == 0) {
				d = (double)atof (param);
				MCC_ASN = d;
			}
			else if (strcmp (name, "ASP") == 0) {
				d = (double)atof (param);
				MCC_ASP = d;
			}
			else if (strcmp (name, "ADN") == 0) {
				d = (double)atof (param);
				MCC_ADN = d;
			}
			else if (strcmp (name, "ADP") == 0) {
				d = (double)atof (param);
				MCC_ADP = d;
			}
            /*------------------------------------*\
             * PERIMETER
            \*------------------------------------*/
			else if (strcmp (name, "PSN") == 0) {
				d = (double)atof (param);
				MCC_PSN = d;
			}
			else if (strcmp (name, "PSP") == 0) {
				d = (double)atof (param);
				MCC_PSP = d;
			}
			else if (strcmp (name, "PDN") == 0) {
				d = (double)atof (param);
				MCC_PDN = d;
			}
			else if (strcmp (name, "PDP") == 0) {
				d = (double)atof (param);
				MCC_PDP = d;
			}
			else if (strcmp (name, "LNMIN") == 0) {
				d = (double)atof (param);
				MCC_LNMIN = d;
			}
			else if (strcmp (name, "LNMAX") == 0) {
				d = (double)atof (param);
				MCC_LNMAX = d;
			}
			else if (strcmp (name, "WNMIN") == 0) {
				d = (double)atof (param);
				MCC_WNMIN = d;
			}
			else if (strcmp (name, "WNMAX") == 0) {
				d = (double)atof (param);
				MCC_WNMAX = d;
			}
			else if (strcmp (name, "LPMIN") == 0) {
				d = (double)atof (param);
				MCC_LPMIN = d;
			}
			else if (strcmp (name, "LPMAX") == 0) {
				d = (double)atof (param);
				MCC_LPMAX = d;
			}
			else if (strcmp (name, "WPMIN") == 0) {
				d = (double)atof (param);
				MCC_WPMIN = d;
			}
			else if (strcmp (name, "WPMAX") == 0) {
				d = (double)atof (param);
				MCC_WPMAX = d;
			}
			else if (strcmp (name, "DWN") == 0) {
				d = (double)atof (param);
				MCC_DWN = d;
			}
			else if (strcmp (name, "DLN") == 0) {
				d = (double)atof (param);
				MCC_DLN = d;
			}
			else if (strcmp (name, "DWP") == 0) {
				d = (double)atof (param);
				MCC_DWP = d;
			}
			else if (strcmp (name, "DLP") == 0) {
				d = (double)atof (param);
				MCC_DLP = d;
			}
			else if (strcmp (name, "DWCN") == 0) {
				d = (double)atof (param);
				MCC_DWCN = d;
			}
			else if (strcmp (name, "DWCJN") == 0) {
				d = (double)atof (param);
				MCC_DWCJN = d;
			}
			else if (strcmp (name, "DLCN") == 0) {
				d = (double)atof (param);
				MCC_DLCN = d;
			}
			else if (strcmp (name, "DWCP") == 0) {
				d = (double)atof (param);
				MCC_DWCP = d;
			}
			else if (strcmp (name, "DWCJP") == 0) {
				d = (double)atof (param);
				MCC_DWCJP = d;
			}
			else if (strcmp (name, "DLCP") == 0) {
				d = (double)atof (param);
				MCC_DLCP = d;
			}
			else if (strcmp (name, "DIFF") == 0) {
				d = (double)atof (param);
				MCC_DIF = d;
			}
			else if (strcmp (name, "XWN") == 0) {
				d = (double)atof (param);
				MCC_XWN = d;
			}
			else if (strcmp (name, "XLN") == 0) {
				d = (double)atof (param);
				MCC_XLN = d;
			}
			else if (strcmp (name, "XWP") == 0) {
				d = (double)atof (param);
				MCC_XWP = d;
			}
			else if (strcmp (name, "XLP") == 0) {
				d = (double)atof (param);
				MCC_XLP = d;
			}
			else if (strcmp (name, "CGSN") == 0) {
				d = (double)atof (param);
				MCC_CGSN = d;
			}
			else if (strcmp (name, "CGSUN") == 0) {
				d = (double)atof (param);
				MCC_CGSUN = d;
			}
			else if (strcmp (name, "CGSUFN") == 0) {
				d = (double)atof (param);
				MCC_CGSUFN = d;
			}
			else if (strcmp (name, "CGSDN") == 0) {
				d = (double)atof (param);
				MCC_CGSDN = d;
			}
			else if (strcmp (name, "CGSDFN") == 0) {
				d = (double)atof (param);
				MCC_CGSDFN = d;
			}
			else if (strcmp (name, "CGSP") == 0) {
				d = (double)atof (param);
				MCC_CGSP = d;
			}
			else if (strcmp (name, "CGSUP") == 0) {
				d = (double)atof (param);
				MCC_CGSUP = d;
			}
			else if (strcmp (name, "CGSUFP") == 0) {
				d = (double)atof (param);
				MCC_CGSUFP = d;
			}
			else if (strcmp (name, "CGSDP") == 0) {
				d = (double)atof (param);
				MCC_CGSDP = d;
			}
			else if (strcmp (name, "CGSDFP") == 0) {
				d = (double)atof (param);
				MCC_CGSDFP = d;
			}
			else if (strcmp (name, "CGDN") == 0) {
				d = (double)atof (param);
				MCC_CGDN = d;
			}
			else if (strcmp (name, "CGDP") == 0) {
				d = (double)atof (param);
				MCC_CGDP = d;
			}
			else if (strcmp (name, "CGDCN") == 0) {
				d = (double)atof (param);
				MCC_CGDCN = d;
			}
			else if (strcmp (name, "CGDCP") == 0) {
				d = (double)atof (param);
				MCC_CGDCP = d;
			}
			else if (strcmp (name, "CGSIN") == 0) {
				d = (double)atof (param);
				MCC_CGSIN = d;
			}
			else if (strcmp (name, "CGSIP") == 0) {
				d = (double)atof (param);
				MCC_CGDP = d;
			}
			else if (strcmp (name, "CGSICN") == 0) {
				d = (double)atof (param);
				MCC_CGSICN = d;
			}
			else if (strcmp (name, "CGSICP") == 0) {
				d = (double)atof (param);
				MCC_CGDCP = d;
			}
			else if (strcmp (name, "CGPN") == 0) {
				d = (double)atof (param);
				MCC_CGPN = d;
			}
			else if (strcmp (name, "CGPP") == 0) {
				d = (double)atof (param);
				MCC_CGPP = d;
			}
			else if (strcmp (name, "CDSN") == 0) {
				d = (double)atof (param);
				MCC_CDSN = d;
			}
			else if (strcmp (name, "CDSP") == 0) {
				d = (double)atof (param);
				MCC_CDSP = d;
			}
			else if (strcmp (name, "CDPN") == 0) {
				d = (double)atof (param);
				MCC_CDPN = d;
			}
			else if (strcmp (name, "CDPP") == 0) {
				d = (double)atof (param);
				MCC_CDPP = d;
			}
			else if (strcmp (name, "CDWN") == 0) {
				d = (double)atof (param);
				MCC_CDWN = d;
			}
			else if (strcmp (name, "CDWP") == 0) {
				d = (double)atof (param);
				MCC_CDWP = d;
			}
			else if (strcmp (name, "CSSN") == 0) {
				d = (double)atof (param);
				MCC_CSSN = d;
			}
			else if (strcmp (name, "CSSP") == 0) {
				d = (double)atof (param);
				MCC_CSSP = d;
			}
			else if (strcmp (name, "CSPN") == 0) {
				d = (double)atof (param);
				MCC_CSPN = d;
			}
			else if (strcmp (name, "CSPP") == 0) {
				d = (double)atof (param);
				MCC_CSPP = d;
			}
			else if (strcmp (name, "CSWN") == 0) {
				d = (double)atof (param);
				MCC_CSWN = d;
			}
			else if (strcmp (name, "CSWP") == 0) {
				d = (double)atof (param);
				MCC_CSWP = d;
			}
			else if (strcmp (name, "VGS") == 0) {
				d = (double)atof (param);
				MCC_VGS = d;
			}
			else if (strcmp (name, "TEMP") == 0) {
				d = (double)atof (param);
				MCC_TEMP = d;
				V_FLOAT_TAB[__SIM_TEMP].VALUE = d;
			}
			else if (strcmp (name, "SLOPE") == 0) {
				d = (double)atof (param);
				MCC_SLOPE = d;
				SIM_SLOP = d;
			}
			else if (strcmp (name, "TIME") == 0) {
				d = (double)atof (param);
				V_FLOAT_TAB[ __SIM_TIME ].VALUE = d * 1E-9 ;
			}
			else if (strcmp (name, "TRANSTEP") == 0) {
				d = (double)atof (param);
				V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE = d * 1E-9 ;
			}
			else if (strcmp (name, "DCSTEP") == 0) {
				d = (double)atof (param);
				MCC_DC_STEP = d;
				SIM_DC_STEP = d;
			}
			else if (strcmp (name, "ERROR") == 0) {
				d = (double)atof (param);
				MCC_ERROR = d;
			}
			else if (strcmp (name, "CAPA") == 0) {
				f = (float)atof (param);
				MCC_CAPA = f;
			}
			else if (strcmp (name, "ELPNAME") == 0) {
				mbkfree (MCC_ELPFILE);
				MCC_ELPFILE = (char *)mbkalloc (strlen (param) + 1);
				strcpy (MCC_ELPFILE, param);
			}
			else if (strcmp (name, "TECHNAME") == 0) {
				mbkfree (MCC_TECHFILE);
				MCC_TECHFILE = (char *)mbkalloc (strlen (param) + 1);
				MCC_TECHFILE = strcpy (MCC_TECHFILE, param);
				SIM_TECHFILE = sensitive_namealloc(MCC_TECHFILE) ;
			}
			else if (strcmp (name, "MODELNAME") == 0) {
				mbkfree (MCC_MODELFILE);
				MCC_MODELFILE = (char *)mbkalloc (strlen (param) + 1);
				MCC_MODELFILE = strcpy (MCC_MODELFILE, param);
			}
			else if (strcmp (name, "SPICESTRING") == 0) {
				mbkfree (MCC_SPICESTRING);
				MCC_SPICESTRING = (char *)mbkalloc (strlen (param) + 1);
				MCC_SPICESTRING = strcpy (MCC_SPICESTRING, param);
                SIM_SPICESTRING = sensitive_namealloc(MCC_SPICESTRING) ;
			}
			else if (strcmp (name, "SPICETOOL") == 0) {
				mbkfree (MCC_SPICENAME);
				MCC_SPICENAME = (char *)mbkalloc (strlen (param) + 1);
				MCC_SPICENAME = strcpy (MCC_SPICENAME, param);
                SIM_SPICENAME = sensitive_namealloc(MCC_SPICENAME) ;
			}
			else if (strcmp (name, "SPICEOPTIONS") == 0) {
				mbkfree (MCC_SPICEOPTIONS);
				MCC_SPICEOPTIONS = (char *)mbkalloc (strlen (param) + 1);
				MCC_SPICEOPTIONS = strcpy (MCC_SPICEOPTIONS, param);
                SIM_SPICE_OPTIONS = sensitive_namealloc(MCC_SPICEOPTIONS) ;
			}
			else if (strcmp (name, "TASTOOL") == 0) {
				mbkfree (MCC_TASNAME);
				MCC_TASNAME = (char *)mbkalloc (strlen (param) + 1);
				MCC_TASNAME = strcpy (MCC_TASNAME, param);
			}
			else if (strcmp (name, "SPICEOUT") == 0) {
				mbkfree (MCC_SPICEOUT);
				MCC_SPICEOUT = (char *)mbkalloc (strlen (param) + 1);
				MCC_SPICEOUT = strcpy (MCC_SPICEOUT, param);
                SIM_SPICEOUT = sensitive_namealloc(MCC_SPICEOUT) ;
			}
			else if (strcmp (name, "SPICESTDOUT") == 0) {
				mbkfree (MCC_SPICESTDOUT);
				MCC_SPICESTDOUT = (char *)mbkalloc (strlen (param) + 1);
				MCC_SPICESTDOUT = strcpy (MCC_SPICESTDOUT, param);
                SIM_SPICESTDOUT = sensitive_namealloc(MCC_SPICESTDOUT) ;
			}
			else if (strcmp (name, "TNMODELNAME") == 0) {
				mbkfree (MCC_TNMODEL);
				MCC_TNMODEL = (char *)mbkalloc (strlen (param) + 1);
				MCC_TNMODEL = strcpy (MCC_TNMODEL, param);
			}
			else if (strcmp (name, "NINDEX") == 0) {
				i = atoi (param);
				MCC_NINDEX = i;
			}
			else if (strcmp (name, "NCASE") == 0) {
				for (i = 0; i < MCC_NB_CASE; i++)
					if (strcmp (MCC_CASE_NAME[i], param) == 0)
						break;
				MCC_NCASE = i;
			}
			else if (strcmp (name, "TPMODELNAME") == 0) {
				mbkfree (MCC_TPMODEL);
				MCC_TPMODEL = (char *)mbkalloc (strlen (param) + 1);
				MCC_TPMODEL = strcpy (MCC_TPMODEL, param);
			}
			else if (strcmp (name, "PINDEX") == 0) {
				i = atoi (param);
				MCC_PINDEX = i;
			}
			else if (strcmp (name, "PCASE") == 0) {
				for (i = 0; i < MCC_NB_CASE; i++)
					if (strcmp (MCC_CASE_NAME[i], param) == 0)
						break;
				MCC_PCASE = i;
			}
			else if (strcmp (name, "TNMODELTYPE") == 0) {
				mbkfree (MCC_TNMODELTYPE);
				MCC_TNMODELTYPE = (char *)mbkalloc (strlen (param) + 1);
				strcpy (MCC_TNMODELTYPE, param);
			}
			else if (strcmp (name, "TPMODELTYPE") == 0) {
				mbkfree (MCC_TPMODELTYPE);
				MCC_TPMODELTYPE = (char *)mbkalloc (strlen (param) + 1);
				MCC_TPMODELTYPE = strcpy (MCC_TPMODELTYPE, param);
			}
			else if (strcmp (name, "ELPVERSION") == 0) {
				mbkfree (MCC_ELPVERSION);
				MCC_ELPVERSION = (char *)mbkalloc (strlen (param) + 1);
				MCC_ELPVERSION = strcpy (MCC_ELPVERSION, param);
			}
			else if (strcmp (name, "SPICEMODELTYPE") == 0) {
				if (strcmp (param, "UNKNOWN") == 0)
					MCC_SPICEMODELTYPE = MCC_NOMODEL;
				else {
					for (i = 0; i < MCC_NB_MOD; i++)
						if (strcmp (MCC_MOD_NAME[i], param) == 0)
							break;
					MCC_SPICEMODELTYPE = i;
				}
			}
			else if (strcmp (name, "NBMEASURE") == 0) {
				i = atoi (param);
				MCC_INSNUMB = i;
			}
			else if (strcmp (name, "NBDIFF") == 0) {
				i = atoi (param);
				MCC_TRANSNUMB = i;
			}
			else if (strcmp (name, "NBGATE") == 0) {
				i = atoi (param);
				MCC_INVNUMB = i;
			}
			else if (strcmp (name, "NBFITLOOP") == 0) {
				i = atoi (param);
				MCC_NBMAXLOOP = i;
			}
		}
		else {
			fprintf (stderr, "\nmcc error : bad file .mccgenelp delete it\n");
		}
	}

	mcc_fclose (file, ".mccgenelp");

	return (1);
}

void mcc_equaterange ()
{
	MCC_LNMIN = MCC_LNMAX = MCC_LN;
	MCC_WNMIN = MCC_WNMAX = MCC_WN;
	MCC_LPMIN = MCC_LPMAX = MCC_LP;
	MCC_WPMIN = MCC_WPMAX = MCC_WP;
}

void mcc_resizerange ()
{
	MCC_LNMIN = MCC_LN * (1.0 - MCC_RANGE_MARGIN);
	MCC_LNMAX = MCC_LN * (1.0 + MCC_RANGE_MARGIN);
	MCC_WNMIN = MCC_WN * (1.0 - MCC_RANGE_MARGIN);
	MCC_WNMAX = MCC_WN * (1.0 + MCC_RANGE_MARGIN);

	MCC_LPMIN = MCC_LP * (1.0 - MCC_RANGE_MARGIN);
	MCC_LPMAX = MCC_LP * (1.0 + MCC_RANGE_MARGIN);
	MCC_WPMIN = MCC_WP * (1.0 - MCC_RANGE_MARGIN);
	MCC_WPMAX = MCC_WP * (1.0 + MCC_RANGE_MARGIN);
}

void mcc_modifyrange ()
{
	if (MCC_EQUATE_RANGE) {
		mcc_equaterange ();
		return;
	}
	if (MCC_RESIZE_RANGE) {
		mcc_resizerange ();
		return;
	}
}

/****************************************************************************\
 FUNCTION : mcc_lotrsparam_add
\****************************************************************************/
elp_lotrs_param *mcc_lotrsparam_add ( double delvt0, double mulu0,
                                      double sa, double sb, double sd, double nf,
                                      double m, double nrs, double nrd, double sc, double sca, double scb, double scc,
                                      double vbulk)
{
  elp_lotrs_param *lotrsparam;

  lotrsparam = elp_lotrs_param_alloc ();
  lotrsparam->PARAM[elpMULU0]  = mulu0;
  lotrsparam->PARAM[elpDELVT0] = delvt0;
  lotrsparam->PARAM[elpSA]  = sa;
  lotrsparam->PARAM[elpSB]  = sb;
  lotrsparam->PARAM[elpSD]  = sd;
  lotrsparam->PARAM[elpNF]  = nf;
  lotrsparam->PARAM[elpM]  = m;
  lotrsparam->PARAM[elpNRS]  = nrs;
  lotrsparam->PARAM[elpNRD]  = nrd;
  lotrsparam->PARAM[elpSC]     = sc;
  lotrsparam->PARAM[elpSCA]     = sca;
  lotrsparam->PARAM[elpSCB]     = scb;
  lotrsparam->PARAM[elpSCC]     = scc;
  lotrsparam->VBULK  = vbulk;
  lotrsparam->SUBCKTNAME  = NULL;
  lotrsparam->ISVBSSET = 0 ;

  return lotrsparam;
}

/****************************************************************************\
 FUNCTION : mcc_lotrsparam_set
\****************************************************************************/
elp_lotrs_param *mcc_lotrsparam_set ( elp_lotrs_param *lotrsparam,
                                       double delvt0, double mulu0, 
                                       double sa, double sb, double sd, double nf,
                                       double m, double nrs, double nrd, double sc, double sca, double scb, double scc,
                                       double vbulk)
{
  if ( lotrsparam ) {
    lotrsparam->PARAM[elpDELVT0] = delvt0;
    lotrsparam->PARAM[elpMULU0]  = mulu0;
    lotrsparam->PARAM[elpSA]     = sa;
    lotrsparam->PARAM[elpSB]     = sb;
    lotrsparam->PARAM[elpSD]     = sd;
    lotrsparam->PARAM[elpNF]     = nf;
    lotrsparam->PARAM[elpM]      = m;
    lotrsparam->PARAM[elpNRS]     = nrs;
    lotrsparam->PARAM[elpNRD]     = nrd;
    lotrsparam->PARAM[elpSC]     = sc;
    lotrsparam->PARAM[elpSCA]     = sca;
    lotrsparam->PARAM[elpSCB]     = scb;
    lotrsparam->PARAM[elpSCC]     = scc;
    lotrsparam->SUBCKTNAME     = NULL;
    lotrsparam->VBULK  = vbulk;
  }
  return lotrsparam;
}

/****************************************************************************\
 FUNCTION : mcc_init_lotrsparam
\****************************************************************************/
elp_lotrs_param *mcc_init_lotrsparam ( void )
{
  elp_lotrs_param *ptlotrsparam = NULL;

  ptlotrsparam = mcc_lotrsparam_add ( 0.0,1.0,ELPINITVALUE, ELPINITVALUE, ELPINITVALUE,
                                      1.0,1.0,ELPINITVALUE, ELPINITVALUE,ELPINITVALUE,ELPINITVALUE,ELPINITVALUE,ELPINITVALUE,ELPINITVBULK);

  return ptlotrsparam;
}

#if 0
/****************************************************************************\
 FUNCTION : mcc_is_default_technoname
 Return 1 if true
\****************************************************************************/
int mcc_is_default_technoname ( char *name )
{
  int res = 0;
  if ( !strcasecmp (name,"avtdefault.tec"))
    res = 1;
  return res;
}
#endif

/****************************************************************************\
 FUNCTION : mcc_get_date
\****************************************************************************/
char *mcc_get_date (void)
{
    time_t    counter;
    char     *date;
    
    time (&counter);
    date = ctime (&counter);
    date[strlen (date) - 1] = '\0';
    
    return date;
}

/****************************************************************************\
 FUNCTION : mcc_use_multicorner
 Return 1 if true
\****************************************************************************/
int mcc_use_multicorner (void)
{
 return MCC_USE_MULTICORNER;
}

/****************************************************************************\
 * Function : mcc_get_area_perim
 *
\****************************************************************************/
void mcc_get_area_perim ( void )
{
  /* NMOS */
  if ( MCC_CURRENT_LOTRS ) return; // because area and perim have been updated

  if ( MCC_ASN < 0.0 )
    MCC_ASN = MCC_WN*MCC_DIF;
  if ( MCC_ADN < 0.0 )
    MCC_ADN = MCC_WN*MCC_DIF;
  if ( MCC_PSN < 0.0 )
    MCC_PSN = (2.0*MCC_DIF)+(2.0*MCC_WN);
  if ( MCC_PDN < 0.0 )
    MCC_PDN = (2.0*MCC_DIF)+(2.0*MCC_WN);
  /* PMOS */
  if ( MCC_ASP < 0.0 )
    MCC_ASP = MCC_WP*MCC_DIF;
  if ( MCC_ADP < 0.0 )
    MCC_ADP = MCC_WP*MCC_DIF;
  if ( MCC_PSP < 0.0 )
    MCC_PSP = (2.0*MCC_DIF)+(2.0*MCC_WP);
  if ( MCC_PDP < 0.0 )
    MCC_PDP = (2.0*MCC_DIF)+(2.0*MCC_WP);
}

/****************************************************************************\
 * Function : mcc_init_globals
 *
\****************************************************************************/
void mcc_init_globals ()
{
  MCC_LNMAX = MCC_D_LWMAX;
  MCC_LNMIN = 0.0;
  MCC_WNMAX = MCC_D_LWMAX;
  MCC_WNMIN = 0.0;
  MCC_LPMAX = MCC_D_LWMAX;
  MCC_LPMIN = 0.0;
  MCC_WPMAX = MCC_D_LWMAX;
  MCC_WPMIN = 0.0;
}

void mcc_drive_dot_model(FILE *f, mcc_modellist *ptmodel)
{
 chain_list *ch;
 double val;
 char *ext;

 if (V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN)
   ext=mcc_getmccname(ptmodel);
 else
   ext="";
 fprintf(f, ".model %s", ptmodel->NAME);
 fprintf(f, " %s%s\n", ptmodel->TYPE == MCC_NMOS?"nmos":"pmos", ext);
 
 for (ch=ptmodel->origparams; ch!=NULL; ch=ch->NEXT)
 {
   val=mcc_getparam(ptmodel, (char *)ch->DATA);
   fprintf(f, "+ %s = %g\n", (char *)ch->DATA, val);
 }
}

mccglobal* mcc_getglobal()
{
  mccglobal *data ;
  int        i, n ;

  data = (mccglobal*)mbkalloc( sizeof( mccglobal ) );
  
  n = sizeof( MCC_GLOBAL_DBL )/sizeof( MCC_GLOBAL_DBL[0] ) ;
  data->dbl = (double*)mbkalloc( sizeof( double )*n );
  for( i=0 ; i<n ; i++ ) 
    data->dbl[i] = *(MCC_GLOBAL_DBL[i]);

  n = sizeof( MCC_GLOBAL_CHR )/sizeof( MCC_GLOBAL_CHR[0] ) ;
  data->chr = (char**)mbkalloc( sizeof( char* )*n );
  for( i=0 ; i<n ; i++ ) {
    data->chr[i] = *(MCC_GLOBAL_CHR[i]);
    *(MCC_GLOBAL_CHR[i]) = mbkstrdup( *(MCC_GLOBAL_CHR[i]) );
  }

  data->lotrs = MCC_CURRENT_LOTRS ;
  
  return data ;
}

void mcc_setglobal( mccglobal *data )
{
  int        i, n ;

  n = sizeof( MCC_GLOBAL_DBL )/sizeof( MCC_GLOBAL_DBL[0] ) ;
  for( i=0 ; i<n ; i++ ) 
    *(MCC_GLOBAL_DBL[i]) = data->dbl[i] ;

  n = sizeof( MCC_GLOBAL_CHR )/sizeof( MCC_GLOBAL_CHR[0] ) ;
  for( i=0 ; i<n ; i++ ) {
    if( *(MCC_GLOBAL_CHR[i]) ) 
      mbkfree( *(MCC_GLOBAL_CHR[i]) );
    *(MCC_GLOBAL_CHR[i]) = data->chr[i] ;
  }

  MCC_CURRENT_LOTRS = data->lotrs ;
  
  mbkfree( data->dbl );
  mbkfree( data->chr );
  mbkfree( data );
}
