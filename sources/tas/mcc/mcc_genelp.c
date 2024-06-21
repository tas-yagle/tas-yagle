/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_genelp.c                                                */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc.h"
#include "mcc_mod_util.h"
#include "mcc_util.h"
#include "mcc_genspi.h"
#include "mcc_debug.h"
#include "mcc_curv.h"

static int param_used=0;
static float ncurrent_vt=-1000, pcurrent_vt=-1000;

static struct
{
  float l, w, as, ad, ps, pd;
} param[2];

void resetparam()
{
  param[0].l=param[0].w=param[0].as=param[0].ad=param[0].ps=param[0].pd=0;
  param[1].l=param[1].w=param[1].as=param[1].ad=param[1].ps=param[1].pd=0;
  param_used=1;
}

char *mcc_getline (str)
char *str;
{
    char prompt[1024];

    sprintf (prompt, "%s : ", str);

    if (MCC_BUF != NULL)
        mbkfree(MCC_BUF);

    fprintf (stdout, "\n");
    MCC_BUF = readline (prompt);
    if ((MCC_BUF != NULL) && (strcmp (MCC_BUF, "") != 0))
        add_history (strdup (MCC_BUF));
    else
        return (NULL);

    return (MCC_BUF);
}

int mcc_gettab (str, tab, size, old)
char *str;
char **tab;
int size;
int old;
{
    char strx[1024];
    char *pt;
    int i;
    int res = 0;

    sprintf (strx, "%s [", str);
    pt = strx + strlen (strx);

    for (i = 0; i < size - 1; i++) {
        sprintf (pt, "%s ", tab[i]);
        pt = pt + strlen (pt);
    }

    sprintf (pt, "%s] [%s]", tab[i], tab[old]);

    while (res != 1) {
        pt = mcc_getline (strx);

        if (pt == NULL) {
            return (old);
        }

        for (i = 0; i < size; i++) {
            if (strcasecmp (tab[i], pt) == 0)
                return (i);
        }

        fprintf (stderr, "\nmcc error : bad string\n");
        res = 0;
    }

    return (old);
}

int mcc_getint (str, min, max, old)
char *str;
int min;
int max;
int old;
{
    int param = 0;
    char *pt;
    int res = 0;
    char strx[1024];

    sprintf (strx, "%s [%d:%d] [%d]", str, min, max, old);

    while (res != 1) {
        pt = mcc_getline (strx);

        if (pt == NULL) {
            return (old);
        }

        res = sscanf (pt, "%d", &param);


        if ((param < min) || (param > max) || (res != 1)) {
            fprintf (stderr, "\nmcc error : bad number\n");
            res = 0;
        }
    }

    return (param);
}

double mcc_getdouble (str, min, max, old)
char *str;
double min;
double max;
double old;
{
    float param = 0.0;
    double paramx;
    char *pt;
    int res = 0;
    char strx[1024];

    sprintf (strx, "%s [%g:%g] [%g]", str, min, max, old);

    while (res != 1) {
        pt = mcc_getline (strx);

        if (pt == NULL) {
            return (old);
        }

        res = sscanf (pt, "%f", &param);

        paramx = (double)param;

        if ((paramx < min) || (paramx > max) || (res != 1)) {
            fprintf (stderr, "\nmcc error : bad number\n");
            res = 0;
        }
    }

    return (paramx);
}

char *mcc_getstr (str, old)
char *str;
char *old;
{
    char word[1024];
    int len = (1024 - 1);
    char *pt;
    char strx[1024];

    if (old != NULL) {
        sprintf (strx, "%s [%s]", str, old);
    }
    else {
        sprintf (strx, "%s", str);
    }

    pt = NULL;

    while (pt == NULL) {
        pt = mcc_getline (strx);

        if ((pt == NULL) && (old != NULL)) {
            return (old);
        }
        else if (pt == NULL) {
            fprintf (stderr, "\nmcc error : bad string\n");
            continue;
        }
    }

    strcpy (word, pt);

    *(word + (len)) = '\0';

    pt = strchr (word, (int)('\n'));

    if (pt != NULL)
        *pt = '\0';

    mbkfree (old);
    pt = (char *)mbkalloc (strlen (word) + 1);
    strcpy (pt, word);

    return (pt);
}

char *mcc_getword (str, old)
char *str;
char *old;
{
    int res = 0;
    char word[1024];
    unsigned int len = (1024 - 1);
    char *pt;
    char strx[1024];

    if (old != NULL) {
        sprintf (strx, "%s [%s]", str, old);
    }
    else {
        sprintf (strx, "%s", str);
    }

    while (res != 1) {
        pt = mcc_getline (strx);

        if ((pt == NULL) && (old != NULL)) {
            return (old);
        }
        else if (pt == NULL) {
            fprintf (stderr, "\nmcc error : bad string\n");
            continue;
        }

        if (strlen (pt) > len) {
            fprintf (stderr, "\nmcc error : string too long\n");
            continue;
        }

        res = sscanf (pt, "%s", word);

        if (res != 1) {
            fprintf (stderr, "\nmcc error : bad string\n");
            res = 0;
        }
    }

    *(word + (len)) = '\0';
    pt = strchr (word, (int)('\n'));

    if (pt != NULL)
        *pt = '\0';

    pt = (char *)mbkalloc (strlen (word) + 1);
    strcpy (pt, word);

    mbkfree (old);

    return (pt);
}

int mcc_genelp (int spice, int vt, double tec, int aut, int fit, mcc_modellist **modeln, mcc_modellist **modelp,elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p)
{
    char *pt;
    int res = 1;
    long nlmax;
    long nlmin;
    long nwmax;
    long nwmin;
    long plmax;
    long plmin;
    long pwmax;
    long pwmin;
    int b3=0,b4=0;
    int setwn=0, setwp=0;
    mcc_corner_info infon, infop;
    

    if ((MCC_MODE != MCC_FAST_MODE) && (MCC_MODE != MCC_NEXT_MODE)) {
        mcc_prsparam (lotrsparam_n,lotrsparam_p);
        while (1) {
            pt = MCC_TECHFILE;
            MCC_TECHFILE = mcc_getword ("Transistor model file name", MCC_TECHFILE);
            if (pt != MCC_TECHFILE) {
                if (MCC_MODELFILE != NULL)
                    mbkfree (MCC_MODELFILE);
                MCC_MODELFILE = mcc_initstr (MCC_TECHFILE);
            }
            MCC_MODELFILE = mcc_getword ("model file name", MCC_MODELFILE);
            if (mcc_gettechnofile (MCC_MODELFILE) != NULL)
                break;
        }

        MCC_ELPFILE = mcc_getword ("elp file name", MCC_ELPFILE);
        MCC_ELPVERSION = mcc_getword ("elp version", MCC_ELPVERSION);
        MCC_TEMP = mcc_getdouble ("Electrical simulation temperature (in degree)", -273.15, 1000, MCC_TEMP);
    }

    MCC_SPICEMODELTYPE = mcc_getmodeltype (MCC_MODELFILE);

    if (MCC_SPICEMODELTYPE == MCC_NOMODEL)
        MCC_SPICEMODELTYPE = MCC_MOS2;

    if ((MCC_MODE != MCC_FAST_MODE) && (MCC_MODE != MCC_NEXT_MODE)) {
        pt = MCC_TNMODEL;
        MCC_TNMODEL = mcc_getword ("N Transistor model name", MCC_TNMODEL);

        pt = MCC_TPMODEL;
        MCC_TPMODEL = mcc_getword ("P Transistor model name", MCC_TPMODEL);

        b3 = ( MCC_SPICEMODELTYPE == MCC_BSIM3V3 ? 1 : 0 ) ;
        b4 = ( MCC_SPICEMODELTYPE == MCC_BSIM4   ? 1 : 0 ) ;

        MCC_SPICEMODELTYPE = mcc_gettab ("Model type", MCC_MOD_NAME, MCC_VALID_MOD, MCC_SPICEMODELTYPE);

        MCC_SPICESTRING = mcc_getstr ("Spice string", MCC_SPICESTRING);
        MCC_SPICENAME = mcc_getfisrtarg (MCC_SPICESTRING, MCC_SPICENAME);
        MCC_SPICENAME = mcc_getword ("Spice tool name", MCC_SPICENAME);
        MCC_SPICESTDOUT = mcc_getword ("Spice standard out file", MCC_SPICESTDOUT);
        MCC_SPICEOUT = mcc_getword ("Spice out file", MCC_SPICEOUT);
        MCC_TASNAME = mcc_getword ("Tas tool name", MCC_TASNAME);

        mcc_drvparam (lotrsparam_n,lotrsparam_p);
    }

    if (aut == 1) {
        *modeln = mcc_getnextmodel (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, *modeln);

        *modelp = mcc_getnextmodel (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, *modelp);

        if ((*modeln == NULL) && (*modelp == NULL))
            return (0);

        if (*modeln != NULL) {
            nlmax = mcc_ftol (SCALE_X * mcc_getparam_quick(*modeln, __MCC_QUICK_LMAX) * 1e06);
            nlmin = mcc_ftol (SCALE_X * mcc_getparam_quick(*modeln, __MCC_QUICK_LMIN) * 1e06);
            nwmax = mcc_ftol (SCALE_X * mcc_getparam_quick(*modeln, __MCC_QUICK_WMAX) * 1e06);
            nwmin = mcc_ftol (SCALE_X * mcc_getparam_quick(*modeln, __MCC_QUICK_WMIN) * 1e06);
        }

        if (*modelp != NULL) {
            plmax = mcc_ftol (SCALE_X * mcc_getparam_quick(*modelp, __MCC_QUICK_LMAX) * 1e06);
            plmin = mcc_ftol (SCALE_X * mcc_getparam_quick(*modelp, __MCC_QUICK_LMIN) * 1e06);
            pwmax = mcc_ftol (SCALE_X * mcc_getparam_quick(*modelp, __MCC_QUICK_WMAX) * 1e06);
            pwmin = mcc_ftol (SCALE_X * mcc_getparam_quick(*modelp, __MCC_QUICK_WMIN) * 1e06);
        }

        if (*modeln == NULL) {
            nlmax = plmax;
            nlmin = plmin;
            nwmax = pwmax;
            nwmin = pwmin;
        }

        if (*modelp == NULL) {
            plmax = nlmax;
            plmin = nlmin;
            pwmax = nwmax;
            pwmin = nwmin;
        }

        if (nlmin == 0)
            nlmin = nlmax / (long)2;
        if (plmin == 0)
            plmin = plmax / (long)2;

        if ((nwmax > ((long)1000 * nwmin)) || (nwmax == 0))
            nwmax = nwmin * (long)10;

        if ((pwmax > ((long)1000 * pwmin)) || (pwmax == 0))
            pwmax = pwmin * (long)10;

        MCC_LN = (double)nlmin / SCALE_X;
        MCC_LP = (double)plmin / SCALE_X;
        MCC_WN = ((double)nwmin + (double)nwmax) / (2.0 * SCALE_X);
        MCC_WP = 2.0 * MCC_WN;

        if (param_used)
        { 
          if (param[0].l!=0) MCC_LN=param[0].l* 1e06;
          if (param[1].l!=0) MCC_LP=param[1].l* 1e06;
          if (param[0].w!=0) MCC_WN=param[0].w* 1e06, setwn=1;
          if (param[1].w!=0) MCC_WP=param[1].w* 1e06, setwp=1;
        }
         
        if (!setwn && !setwp)
        {
          while ((pwmin > (long)(MCC_WP * SCALE_X)) || (pwmax <= (long)(MCC_WP * SCALE_X))) {
              if (pwmin > (long)(MCC_WP * SCALE_X))
                  MCC_WN = MCC_WN * 11.0 / 10.0;
              else
                  MCC_WN = MCC_WN * 9.0 / 10.0;

              if (nwmin > (long)(MCC_WN * SCALE_X)) {
                  MCC_WN = (double)nwmin / (double)SCALE_X;
                  MCC_WP = (double)pwmax / (double)SCALE_X *0.95;
                  break;
              }

              if (nwmax <= (long)(MCC_WN * SCALE_X)) {
                  MCC_WN = (double)nwmax / (double)SCALE_X *0.95;
                  MCC_WP = (double)pwmin / (double)SCALE_X;
                  break;
              }


              MCC_WP = 2.0 * MCC_WN;
          }
        }
        if (tec < 0.0)
            MCC_DIF = MCC_LN * 2.0;
        else
            MCC_DIF = tec * 2.0;

        if (tec > 0.0001) {
            if (MCC_LN < 0.0001)
                MCC_LN = tec;
            if (MCC_WN < 0.0001)
                MCC_WN = tec * 6.0;
            if (MCC_LP < 0.0001)
                MCC_LP = tec;
            if (MCC_WP < 0.0001)
                MCC_WP = tec * 12.0;
            if (MCC_DIF < 0.0001)
                MCC_DIF = tec * 2.0;
        }

        if (*modeln == NULL)
        {
            *modeln = mcc_getmodel (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS,
                                    MCC_NCASE, MCC_LN * 1.0e-06, MCC_WN * 1.0e-06,0);
        }

        if (*modelp == NULL)
        {
            *modelp = mcc_getmodel (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS,
                                    MCC_PCASE, MCC_LP * 1.0e-06, MCC_WP * 1.0e-06,0);
        }

        if (*modeln==NULL || *modelp==NULL) return res;

        fprintf (stdout, "Transistor size for automatic model calculation:\n");
        fprintf (stdout, "%s : LN=%g WN=%g %s : LP=%g WP=%g\n", (*modeln)->NAME,
                 MCC_LN, MCC_WN, (*modelp)->NAME, MCC_LP, MCC_WP);

        mcc_drvparam (lotrsparam_n,lotrsparam_p);
    }
    else
    {
        if (*modeln == NULL)
        {
            *modeln = mcc_getmodel (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS,
                                    MCC_NCASE, MCC_LN * 1.0e-06, MCC_WN * 1.0e-06,0);
        }

        if (*modelp == NULL)
        {
            *modelp = mcc_getmodel (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS,
                                    MCC_PCASE, MCC_LP * 1.0e-06, MCC_WP * 1.0e-06,0);
        }

        if (*modeln==NULL || *modelp==NULL) return res;
        mcc_drvparam (lotrsparam_n,lotrsparam_p);
    }

    lotrsparam_n->SUBCKTNAME=MCC_TN_SUBCKT;
    lotrsparam_p->SUBCKTNAME=MCC_TP_SUBCKT;
    memcpy(&mcc_user_lotrsparam_n, lotrsparam_n, sizeof(mcc_user_lotrsparam_n));
    memcpy(&mcc_user_lotrsparam_p, lotrsparam_p, sizeof(mcc_user_lotrsparam_p));
    mcc_check_techno_with_subckt(lotrsparam_n,lotrsparam_p, MCC_TN_SUBCKT, MCC_TP_SUBCKT);
    
    if(MCC_OPTIM_MODE == 1){
        MCC_DEBUG_INTERNAL = YES ;
        mcc_spicecurrent( MCC_VDDmax, MCC_VDDmax, lotrsparam_n->VBULK, MCC_TRANS_N, lotrsparam_n );
        mcc_spicecurrent( MCC_VDDmax, MCC_VDDmax, lotrsparam_p->VBULK-MCC_VDDmax, MCC_TRANS_P, lotrsparam_p );
        MCC_DEBUG_INTERNAL = NO ;
    }

    if (MCC_MODE != MCC_FAST_MODE) {
        mcc_prsparam (lotrsparam_n,lotrsparam_p);

        tec = mcc_getdouble ("Technology size (in micro-meter)", 0.0, 100.0, (tec < 0.0) ? 0.0 : tec);

        if (tec > 0.0001) {
            MCC_TECSIZE = tec;
            MCC_LN = tec;
            MCC_WN = tec * 6.0;
            MCC_LP = tec;
            MCC_WP = tec * 12.0;
            MCC_DIF = tec * 2.0;
        }

        MCC_LN = mcc_getdouble ("N MOS tansistor length size (in micro-meter)", 0.0, 100.00, MCC_LN);
        MCC_WN = mcc_getdouble ("N MOS tansistor width size (in micro-meter)", 0.0, 1000.0, MCC_WN);
        MCC_LP = mcc_getdouble ("P MOS tansistor length size (in micro-meter)", 0.0, 100.0, MCC_LP);
        MCC_WP = mcc_getdouble ("P MOS tansistor width size (in micro-meter)", 0.0, 1000.0, MCC_WP);
        MCC_VDDmax = mcc_getdouble ("Power supply (in Volt)", 0.1, 10.0, MCC_VDDmax);
        MCC_VGS = MCC_VDDmax / 2.0 ;
        if ( lotrsparam_n->VBULK < ELPMINVBULK )
          lotrsparam_n->VBULK = 0.0;
        if ( lotrsparam_p->VBULK < ELPMINVBULK )
          lotrsparam_p->VBULK = MCC_VDDmax;
        if ( b3 || b4 )
         {
          lotrsparam_n->PARAM[elpDELVT0] = mcc_getdouble ("N MOS transistor threshold voltage shrink (DELVT0)", -MCC_VTN/2.0, MCC_VTN/2.0, lotrsparam_n->PARAM[elpDELVT0]);
          lotrsparam_n->PARAM[elpMULU0] = mcc_getdouble ("N MOS transistor mobility mult factor (U0)", 0.5, 1.5, lotrsparam_n->PARAM[elpMULU0]);
         }
        lotrsparam_n->VBULK = mcc_getdouble ("N MOS transistor bulk voltage (in Volt)", -MCC_VDDmax, MCC_VDDmax, lotrsparam_n->VBULK);
        MCC_VBULKN = lotrsparam_n->VBULK;
        if ( b3 || b4 )
         {
          lotrsparam_p->PARAM[elpDELVT0] = mcc_getdouble ("P MOS transistor threshold voltage shrink (DELVT0)", -MCC_VTP/2.0, MCC_VTP/2.0, lotrsparam_p->PARAM[elpDELVT0]);
          lotrsparam_p->PARAM[elpMULU0] = mcc_getdouble ("P MOS transistor mobility mult factor (U0)", 0.5, 1.5, lotrsparam_p->PARAM[elpMULU0]);
         }
        lotrsparam_p->VBULK = mcc_getdouble ("P MOS transistor bulk voltage (in Volt)", -MCC_VDDmax, 2.0*MCC_VDDmax, lotrsparam_p->VBULK);
        MCC_VBULKP = lotrsparam_p->VBULK;
        MCC_DIF = mcc_getdouble ("Transistor diffusion width (in micro-meter)", 0.1, 100.0, MCC_DIF);

        if (MCC_MODE == MCC_EXPERT_MODE) {
            MCC_DC_STEP = mcc_getdouble ("Electrical simulation dc step", 0.00001, 0.100, MCC_DC_STEP);
            MCC_INSNUMB = mcc_getint ("Number of instances to fit parametres", 10, 100, MCC_INSNUMB);
            MCC_CAPA = mcc_getdouble ("Capacitance to fit curent parameters (in fF)", 10, 10000, MCC_CAPA);
            MCC_INVNUMB = mcc_getint ("Number of inverters to fit gate capacitance", 1, 100, MCC_INVNUMB);
            MCC_TRANSNUMB = mcc_getint ("Number of transistors to fit diffusion capacitance", 1, 100, MCC_TRANSNUMB);
        }

        mcc_drvparam (lotrsparam_n,lotrsparam_p);
    }
    else
    {
        if (param_used)
        { 
          if (param[0].l!=0) MCC_LN=param[0].l* 1e06;
          if (param[1].l!=0) MCC_LP=param[1].l* 1e06;
          if (param[0].w!=0) MCC_WN=param[0].w* 1e06;
          if (param[1].w!=0) MCC_WP=param[1].w* 1e06;
        }
    }
    {
      double kasn=MCC_WN*MCC_DIF, kadn=MCC_WN*MCC_DIF, kpsn=(2*MCC_DIF)+(2*MCC_WN), kpdn=(2*MCC_DIF)+(2*MCC_WN);
      double kasp=MCC_WP*MCC_DIF, kadp=MCC_WP*MCC_DIF, kpsp=(2*MCC_DIF)+(2*MCC_WP), kpdp=(2*MCC_DIF)+(2*MCC_WP);
      if (param_used)
        { 
          if (param[0].as!=0) kasn=param[0].as* 1e12/SCALE_X;
          if (param[0].ad!=0) kadn=param[0].ad* 1e12/SCALE_X;
          if (param[0].ps!=0) kpsn=param[0].ps* 1e06/SCALE_X;
          if (param[0].pd!=0) kpdn=param[0].pd* 1e06/SCALE_X;
          if (param[1].as!=0) kasp=param[1].as* 1e12/SCALE_X;
          if (param[1].ad!=0) kadp=param[1].ad* 1e12/SCALE_X;
          if (param[1].ps!=0) kpsp=param[1].ps* 1e06/SCALE_X;
          if (param[1].pd!=0) kpdp=param[1].pd* 1e06/SCALE_X;
        }
      sprintf(surf_string_n, "as=%gp ad=%gp ps=%gu pd=%gu", kasn, kadn, kpsn, kpdn);
      sprintf(surf_string_p, "as=%gp ad=%gp ps=%gu pd=%gu", kasp, kadp, kpsp, kpdp);      
    }
    if ( lotrsparam_n )
      lotrsparam_n = mcc_lotrsparam_set ( lotrsparam_n,
                                          lotrsparam_n->PARAM[elpDELVT0],
                                          lotrsparam_n->PARAM[elpMULU0],
                                          lotrsparam_n->PARAM[elpSA],
                                          lotrsparam_n->PARAM[elpSB],
                                          lotrsparam_n->PARAM[elpSD],
                                          lotrsparam_n->PARAM[elpNF],
                                          lotrsparam_n->PARAM[elpM],
                                          lotrsparam_n->PARAM[elpNRS],
                                          lotrsparam_n->PARAM[elpNRD],
                                          lotrsparam_n->PARAM[elpSC],
                                          lotrsparam_n->PARAM[elpSCA],
                                          lotrsparam_n->PARAM[elpSCB],
                                          lotrsparam_n->PARAM[elpSCC],
                                          lotrsparam_n->VBULK );
    if ( lotrsparam_p )
      lotrsparam_p = mcc_lotrsparam_set ( lotrsparam_p,
                                          lotrsparam_p->PARAM[elpDELVT0],
                                          lotrsparam_p->PARAM[elpMULU0],
                                          lotrsparam_p->PARAM[elpSA],
                                          lotrsparam_p->PARAM[elpSB],
                                          lotrsparam_p->PARAM[elpSD],
                                          lotrsparam_p->PARAM[elpNF],
                                          lotrsparam_p->PARAM[elpM],
                                          lotrsparam_p->PARAM[elpNRS],
                                          lotrsparam_p->PARAM[elpNRD],
                                          lotrsparam_p->PARAM[elpSC],
                                          lotrsparam_p->PARAM[elpSCA],
                                          lotrsparam_p->PARAM[elpSCB],
                                          lotrsparam_p->PARAM[elpSCC],
                                          lotrsparam_p->VBULK );

    lotrsparam_n->SUBCKTNAME=MCC_TN_SUBCKT;
    lotrsparam_p->SUBCKTNAME=MCC_TP_SUBCKT;

    if (MCC_VBULKP <= ELPMINVBULK) MCC_VBULKP=lotrsparam_p->VBULK;
    if (MCC_VBULKN <= ELPMINVBULK) MCC_VBULKN=lotrsparam_n->VBULK;
    if (MCC_VBULKP <= ELPMINVBULK) MCC_VBULKP=MCC_VDDmax;
    if (MCC_VBULKN <= ELPMINVBULK) MCC_VBULKN=0;

    mcc_calcspiparam (MCC_TRANS_B,lotrsparam_n,lotrsparam_p);

    mcc_genspi (fit,lotrsparam_n,lotrsparam_p);

    if (spice)
        mcc_runspice (fit);

   if ( avt_islog(2,LOGMCC) ) 
    {
     mcc_DisplayInfos (MCC_MODELFILE, MCC_TNMODEL,
                       MCC_NMOS, MCC_NCASE, MCC_LN*1.0e-6, MCC_WN*1.0e-6, 
                       MCC_TEMP, MCC_VDDmax/2.0, 0.0, MCC_VDDmax/2.0,
                       MCC_VDDmax,lotrsparam_n);
     mcc_DisplayInfos (MCC_MODELFILE, MCC_TPMODEL,
                       MCC_PMOS, MCC_PCASE, MCC_LP*1.0e-6, MCC_WP*1.0e-6, 
                       MCC_TEMP, MCC_VDDmax/2.0, 0.0, MCC_VDDmax/2.0,
                       MCC_VDDmax,lotrsparam_p);
    }


    mcc_drvparam (lotrsparam_n,lotrsparam_p);

    if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE )
      mcc_readspidata (fit,lotrsparam_n,lotrsparam_p);
    if ( V_BOOL_TAB[__SIM_USE_MEAS].VALUE )
      mcc_optim_readspidata (fit,lotrsparam_n,lotrsparam_p);

    if (MCC_MODE != MCC_FAST_MODE) {
        mcc_prsparam (lotrsparam_n,lotrsparam_p);

        MCC_TNMODELTYPE = mcc_getword ("N Transistor model type name", MCC_TNMODELTYPE);

        MCC_NCASE = mcc_gettab ("N transistor corner", MCC_CASE_NAME, MCC_NB_CASE, MCC_NCASE);

        MCC_TPMODELTYPE = mcc_getword ("P Transistor model type name", MCC_TPMODELTYPE);

        MCC_PCASE = mcc_gettab ("P transistor corner", MCC_CASE_NAME, MCC_NB_CASE, MCC_PCASE);

        if (MCC_SPICEMODELTYPE == MCC_OTHER)
            MCC_SPICEMODELTYPE = MCC_NOMODELTYPE;

        MCC_LNMIN = mcc_getdouble ("N Transistor minimum length", 0.0, 1.0e99, MCC_LNMIN);

        MCC_LNMAX = mcc_getdouble ("N Transistor maximun length", 0.0, 1.0e99, MCC_LNMAX);

        MCC_WNMIN = mcc_getdouble ("N Transistor minimum width", 0.0, 1.0e99, MCC_WNMIN);

        MCC_WNMAX = mcc_getdouble ("N Transistor maximun width", 0.0, 1.0e99, MCC_WNMAX);

        MCC_LPMIN = mcc_getdouble ("P Transistor minimum length", 0.0, 1.0e99, MCC_LPMIN);

        MCC_LPMAX = mcc_getdouble ("P Transistor maximun length", 0.0, 1.0e99, MCC_LPMAX);

        MCC_WPMIN = mcc_getdouble ("P Transistor minimum width", 0.0, 1.0e99, MCC_WPMIN);

        MCC_WPMAX = mcc_getdouble ("P Transistor maximun width", 0.0, 1.0e99, MCC_WPMAX);

        if (MCC_MODE == MCC_EXPERT_MODE) {
            MCC_XWN = mcc_getdouble ("N Transistor width scale", 0.1, 1.0, MCC_XWN);
            MCC_XLN = mcc_getdouble ("N Transistor length scale", 0.1, 1.0, MCC_XLN);
            MCC_XWP = mcc_getdouble ("P Transistor width scale", 0.1, 1.0, MCC_XWP);
            MCC_XLP = mcc_getdouble ("P Transistor length scale", 0.1, 1.0, MCC_XLP);
        }

        MCC_DWN = mcc_getdouble ("N transistor width shrink parameter (in micro-meter)", -100.0, 100.0, MCC_DWN);
        MCC_DWCJN = mcc_getdouble ("N transistor effective width shrink parameter for capacitance (in micro-meter)",
                                  -100.0, 100.0, MCC_DWCJN);
        MCC_DLN = mcc_getdouble ("N transistor length shrink parameter (in micro-meter)", -100.0, 100.0, MCC_DLN);
        MCC_DWP = mcc_getdouble ("P transistor width shrink parameter (in micro-meter)", -100.0, 100.0, MCC_DWP);
        MCC_DWCJP = mcc_getdouble ("P transistor effective width shrink parameter for capacitance (in micro-meter)",
                                  -100.0, 100.0, MCC_DWCJP);
        MCC_DLP = mcc_getdouble ("P transistor length shrink parameter (in micro-meter)", -100.0, 100.0, MCC_DLP);

        MCC_DWCN = mcc_getdouble ("N transistor active width shrink parameter (in micro-meter)",
                                  -100.0, 100.0, MCC_DWCN);
        MCC_DLCN = mcc_getdouble ("N transistor active length shrink parameter (in micro-meter)",
                                  -100.0, 100.0, MCC_DLCN);
        MCC_DWCP = mcc_getdouble ("P transistor active width shrink parameter (in micro-meter)",
                                  -100.0, 100.0, MCC_DWCP);
        MCC_DLCP = mcc_getdouble ("P transistor active length shrink parameter (in micro-meter)",
                                  -100.0, 100.0, MCC_DLCP);
        mcc_drvparam (lotrsparam_n,lotrsparam_p);
    }

    if (vt == 1) {
        mcc_trs_corner( MCC_TRANS_N, lotrsparam_n, lotrsparam_p, &infon );
        mcc_trs_corner( MCC_TRANS_P, lotrsparam_n, lotrsparam_p, &infop );
        if( V_BOOL_TAB[ __MCC_NEW_IDS_SAT ].VALUE ) {
          mcc_calcul_sat_parameter( MCC_TRANS_N, lotrsparam_n, &infon );
          mcc_calcul_sat_parameter( MCC_TRANS_P, lotrsparam_p, &infop );
        }
        else {
          mcc_calcul_vt (MCC_TRANS_N,lotrsparam_n,lotrsparam_p,&infon);
          mcc_calcul_vt (MCC_TRANS_P,lotrsparam_n,lotrsparam_p,&infop);
        }
    }

    if (MCC_MODE != MCC_FAST_MODE) {
        MCC_VTN = mcc_getdouble ("N transistor threshold voltage (in volt)", 0.0, MCC_VDDmax, MCC_VTN);
        MCC_VTP = mcc_getdouble ("P transistor threshold voltage (in volt)", 0.0, MCC_VDDmax, MCC_VTP);
    }

    if (vt==1)
    {
      if( !V_BOOL_TAB[ __MCC_NEW_IDS_SAT ].VALUE ) {
        if ( MCC_NEW_CALC_ABR && MCC_OPTIM_MODE )
         {
          mcc_get_best_abr_from_vt ( MCC_TRANS_N, lotrsparam_n,lotrsparam_p,&infon );
          mcc_get_best_abr_from_vt ( MCC_TRANS_P, lotrsparam_n,lotrsparam_p,&infop );
         }
        else {
          mcc_calcul_abr (MCC_TRANS_N,-1.0,-1.0,lotrsparam_n,lotrsparam_p);
          mcc_calcul_abr (MCC_TRANS_P,-1.0,-1.0,lotrsparam_n,lotrsparam_p);
        }
      }

      
      if( V_BOOL_TAB[__AVT_RST_BETTER].VALUE ) {
        mcc_calcul_rst_better(MCC_TRANS_N,lotrsparam_n,lotrsparam_p, &infon );
        mcc_calcul_rst_better(MCC_TRANS_P,lotrsparam_n,lotrsparam_p, &infop );
      }
      else {
        mcc_calcul_rst(MCC_TRANS_N,lotrsparam_n,lotrsparam_p);
        mcc_calcul_rst(MCC_TRANS_P,lotrsparam_n,lotrsparam_p);
      }
      mcc_calcul_k (MCC_TRANS_N,lotrsparam_n);
      mcc_calcul_k (MCC_TRANS_P,lotrsparam_p);
      mcc_calcul_vdeg (MCC_TRANS_B,lotrsparam_n,lotrsparam_p);
      mcc_calcul_vti (MCC_TRANS_B,lotrsparam_n,lotrsparam_p);
    }
    if(MCC_DEBUG_MODE > 0) 
     {
      if(MCC_DEBUG_LEAK == 'Y')
        mcc_calcul_leak (lotrsparam_n,lotrsparam_p) ;
      if(MCC_DEBUG_RSAT == 'Y')
      {
        mcc_fill_current_for_rsat(lotrsparam_n,lotrsparam_p);
        mcc_calcul_rsat(lotrsparam_n,lotrsparam_p) ;
      }
      if(MCC_DEBUG_RLIN == 'Y')
      {
        mcc_calcul_rlin(lotrsparam_n,lotrsparam_p) ;
      }
      if((MCC_DEBUG_RSAT == 'Y') && (MCC_DEBUG_RLIN == 'Y'))
        mcc_calcul_vsat(lotrsparam_n,lotrsparam_p) ;
      if(MCC_DEBUG_VTH == 'Y')
        mcc_calcul_vth(lotrsparam_n,lotrsparam_p) ;
      if(MCC_DEBUG_QINT == 'Y') {
        mcc_calcul_qint (lotrsparam_n,lotrsparam_p, 'G' ) ;
        mcc_calcul_qint (lotrsparam_n,lotrsparam_p, 'D' ) ;
        mcc_check_capa( lotrsparam_n, lotrsparam_p ) ;
      }
      if(MCC_DEBUG_JCT_CAPA == 'Y')
       {
        mcc_calc_jct_capa (lotrsparam_n,lotrsparam_p) ;
       }
      if(MCC_DEBUG_INPUTCAPA == 'Y')
       {
        mcc_bilan_capa(lotrsparam_n,lotrsparam_p);
        mcc_calc_coupling_capa(lotrsparam_n,lotrsparam_p);
       }
      if(MCC_DEBUG_CARAC == 'Y') {
        mcc_calcul_trans(lotrsparam_n,lotrsparam_p) ;
        mcc_calcul_rapisat_fqmulu0 (lotrsparam_n,lotrsparam_p);
      }
      if(MCC_DEBUG_CARACNEG == 'Y')
        mcc_calcul_transneg(lotrsparam_n,lotrsparam_p) ;
      if(MCC_DEBUG_DEG == 'Y')
        mcc_calcul_deg(lotrsparam_n,lotrsparam_p) ;
     }

    mcc_drvparam (lotrsparam_n,lotrsparam_p);

    if (vt==0 && MCC_CALC_CUR == MCC_SIM_MODE) {
       ncurrent_vt = mcc_spicesimcurrent(MCC_VTN,MCC_VDDmax,0.0,MCC_TRANS_N) ;
       pcurrent_vt = mcc_spicesimcurrent(MCC_VTP,MCC_VDDmax,0.0,MCC_TRANS_P) ;
    }
    mcc_freespidata (fit, MCC_TRANS_B);

    if (MCC_MODE != MCC_FAST_MODE) {
        mcc_prsparam (lotrsparam_n,lotrsparam_p);

        MCC_CGSN = mcc_getdouble ("N tansistor gate capacitance (in pF/u2)", 0.0, 1.0, MCC_CGSN);
        MCC_CGSP = mcc_getdouble ("P tansistor gate capacitance (in pF/u2)", 0.0, 1.0, MCC_CGSP);
        MCC_CGDN = mcc_getdouble ("N tansistor gate/drain capacitance (in pF/u2)", 0.0, 1.0, MCC_CGDN);
        MCC_CGDCN = mcc_getdouble ("N tansistor gate/drain conflict capacitance (in pF/u2)", 0.0, 1.0, MCC_CGDCN);
        MCC_CGDP = mcc_getdouble ("P tansistor gate/drain capacitance (in pF/u2)", 0.0, 1.0, MCC_CGDP);
        MCC_CGDCP = mcc_getdouble ("P tansistor gate/drain conflict capacitance (in pF/u2)", 0.0, 1.0, MCC_CGDCP);
        MCC_CGPN = mcc_getdouble ("N tansistor gate diffusion capacitance (in pF/u)", 0.0, 1.0, MCC_CGPN);
        MCC_CGPP = mcc_getdouble ("P tansistor gate diffusion capacitance (in pF/u)", 0.0, 1.0, MCC_CGPP);
        MCC_CDSN = MCC_CSSN = mcc_getdouble ("Area N transistor diffusion capacitance (in pF/u2)", 0.0, 1.0, MCC_CDSN);
        MCC_CDS_U_N = MCC_CDS_D_N = MCC_CDSN;
        MCC_CDSP = MCC_CSSP = mcc_getdouble ("Area P transistor diffusion capacitance (in pF/u2)", 0.0, 1.0, MCC_CDSP);
        MCC_CDS_U_P = MCC_CDS_D_P = MCC_CDSP;
        MCC_CDPN = MCC_CSPN = mcc_getdouble ("Perimeter N transistor diffusion capacitance (in pF/u)",
                                             0.0, 1.0, MCC_CDPN);
        MCC_CDP_U_N = MCC_CDP_D_N = MCC_CDPN;
        MCC_CDPP = MCC_CSPP = mcc_getdouble ("Perimeter P transistor diffusion capacitance (in pF/u)",
                                             0.0, 1.0, MCC_CDPP);
        MCC_CDP_U_P = MCC_CDP_D_P = MCC_CDPP;
        MCC_CDWN = MCC_CSWN = mcc_getdouble ("Width N transistor diffusion capacitance (in pF/u)", 0.0, 1.0, MCC_CDWN);
        MCC_CDW_U_N = MCC_CDW_D_N = MCC_CDWN;
        MCC_CDWP = MCC_CSWP = mcc_getdouble ("Width P transistor diffusion capacitance (in pF/u)", 0.0, 1.0, MCC_CDWP);
        MCC_CDW_U_P = MCC_CDW_D_P = MCC_CDWP;

        if (MCC_MODE == MCC_EXPERT_MODE) {
            MCC_ERROR = mcc_getdouble ("Max error to fit parameters (in %)", 0.1, 100.0, MCC_ERROR);
            MCC_NBMAXLOOP = mcc_getint ("Maximun number of loop to fit parameters", 1, 100, MCC_NBMAXLOOP);
        }

        mcc_drvparam (lotrsparam_n,lotrsparam_p);
        pt = mcc_initstr ("y");;
        while (res) {
            pt = mcc_getword ("Do you want to characterize other models y/n", pt);
            if (strcmp (pt, "n") == 0) {
                res = 0;
            }
            else if (strcmp (pt, "y") == 0) {
                break;
            }
        }
        mbkfree (pt);
    }

   if(vt && MCC_DEBUG_MODE > 0) 
     {
      if(MCC_DEBUG_INPUTCAPA == 'Y')
       {
/*        mcc_cal_con_capa ( 'n', 's') ;
        mcc_cal_con_capa ( 'n', 'g') ;
        mcc_cal_con_capa ( 'n', 'd') ;
        mcc_cal_con_capa ( 'p', 's') ;
        mcc_cal_con_capa ( 'p', 'g') ;
        mcc_cal_con_capa ( 'p', 'd') ;
        */
       }
     }
  return (res);
}


static void mcc_parse_instance_spec(char *str, elp_lotrs_param *lotrsparam_n, int idx)
{
  eqt_ctx *ctx;
  char *c, *var;
  char buf[1024];
  chain_list *cl=NULL, *ch;
  int i=0, err;
  double value;

  if (str==NULL) return;

  ctx=eqt_init(10);
  eqt_add_spice_extension(ctx);
    
  c=str;
  while (*c!='\0')
  {
    if (*c!=' ')
    { 
      if (*c=='=')
      {
         buf[i]='\0';
         i=0;
         if (strlen(buf)!=0) cl=addchain(cl,mbkstrdup(buf));
         cl=addchain(cl,mbkstrdup("="));
      }
      else buf[i++]=*c;
    }
    else
    {
       buf[i]='\0';
       i=0;
       if (strlen(buf)!=0) cl=addchain(cl,mbkstrdup(buf));
    }
    c++;
  }

  buf[i]='\0';
  if (strlen(buf)!=0) cl=addchain(cl,mbkstrdup(buf));

  cl=ch=reverse(cl);
  while (ch!=NULL)
  {
    if (ch->NEXT!=NULL && ch->NEXT->NEXT!=NULL && strcmp((char *)ch->NEXT->DATA,"=")==0)
     {
       var=(char *)ch->DATA;
       value=eqt_eval (ctx, namealloc((char *)ch->NEXT->NEXT->DATA), EQTFAST);
       if (eqt_resistrue (ctx))
       {
         err=0;
         if (strcasecmp(var, "delvto")==0) lotrsparam_n->PARAM[elpDELVT0]=value;
         else if (strcasecmp(var, "mulu0")==0) lotrsparam_n->PARAM[elpMULU0]=value;
         else if (strcasecmp(var, "sa")==0) lotrsparam_n->PARAM[elpSA]=value;
         else if (strcasecmp(var, "sb")==0) lotrsparam_n->PARAM[elpSB]=value;
         else if (strcasecmp(var, "sd")==0) lotrsparam_n->PARAM[elpSD]=value;
         else if (strcasecmp(var, "nf")==0) lotrsparam_n->PARAM[elpNF]=value;
//         else if (strcasecmp(var, "m")==0) lotrsparam_n->PARAM[elpM]=value;
         else if (strcasecmp(var, "nrs")==0) lotrsparam_n->PARAM[elpNRS]=value;
         else if (strcasecmp(var, "nrd")==0) lotrsparam_n->PARAM[elpNRD]=value;
         else if (strcasecmp(var, "sc")==0) lotrsparam_n->PARAM[elpSC]=value;
         else if (strcasecmp(var, "sca")==0) lotrsparam_n->PARAM[elpSCA]=value;
         else if (strcasecmp(var, "scb")==0) lotrsparam_n->PARAM[elpSCB]=value;
         else if (strcasecmp(var, "scc")==0) lotrsparam_n->PARAM[elpSCC]=value;
         else if (strcasecmp(var, "vbulk")==0) lotrsparam_n->VBULK=value;
         else if (strcasecmp(var, "l")==0) param[idx].l=value;
         else if (strcasecmp(var, "w")==0) param[idx].w=value;
         else { avt_errmsg(MCC_ERRMSG, "036", AVT_ERROR, var); err=1; }
       }
       else
       {
          avt_errmsg(MCC_ERRMSG, "037", AVT_ERROR, (char *)ch->DATA, (char *)ch->NEXT->NEXT->DATA) ;
       }
     }
    mbkfree(ch->DATA);
    ch=delchain(ch, ch);
  }
  eqt_term(ctx);
}
void mcc_CheckTechno(char *label, char *tn, char *tp)
{
  int aut=0, spice=1, fit=1, vt=1;
  float tec=-1.0;
  int res, save_ELP_LOAD_FILE_TYPE;
  char buf[158];
  int save_print, save_meas;
  char *cn, *cp, *c, *search;
  char *newtech;
  char copytn[1024], copytp[1024];
  char newtn[128], newtp[128];

  elp_lotrs_param *lotrsparam_n = NULL;
  elp_lotrs_param *lotrsparam_p = NULL;
  mcc_modellist *modeln = NULL;
  mcc_modellist *modelp = NULL;

  strcpy(copytn, tn);
  strcpy(copytp, tp);

  sprintf(buf,"%s_techno_check.elp",label);
  
  cp = MCC_ELPFILE = mbkstrdup(buf);
  avt_sethashvar("avtSpiKeepCards", "all");
  avt_sethashvar("avtSpiKeepNames", "all");
  avt_sethashvar("avtElpTechnoName", MCC_ELPFILE);
  avt_sethashvar("avtElpGenTechnoName", MCC_ELPFILE);
  avt_sethashvar("avtElpDriveFile", "yes");
  avt_sethashvar("avtLoadSpecifiedElpFile", "no");
  
  elpenv() ;
  save_ELP_LOAD_FILE_TYPE=ELP_LOAD_FILE_TYPE;

//  mcc_initcalcparam(0);
  mccenv ();
  MCC_ELPFILE=cp;

  resetparam();


  //ELP_LOAD_FILE_TYPE = ELP_LOADELP_FILE;

  MCC_DEBUG_MODE = 1 ;
  MCC_DEBUG_GATE = NULL ;
  MCC_DEBUG_INPUT = -1 ;
  MCC_DEBUG_TRANS = -1 ;
  MCC_DEBUG_SLOPE = 0 ;
  MCC_DEBUG_CAPA = 0 ;
  MCC_DEBUG_RSAT = 'N' ;
  MCC_DEBUG_LEAK = 'N' ;
  MCC_DEBUG_RLIN = 'N' ;
  MCC_DEBUG_VTH = 'N' ;
  MCC_DEBUG_QINT = 'N' ;
  MCC_DEBUG_JCT_CAPA = 'N' ; // Junction capacitance => Cjbd, Cjbs...
  MCC_DEBUG_INPUTCAPA = 'N' ;
  MCC_DEBUG_CARAC = 'N' ;
  MCC_DEBUG_CARACNEG = 'N' ;
  MCC_DEBUG_DEG = 'N' ;
  MCC_DEBUG_PASS_TRANS  = 'X' ; // value can be 'N' or 'P'
  MCC_DEBUG_SWITCH = 'X' ;      // value can be 'N' or 'P'
  MCC_DEBUG_BLEEDER = 'N' ;   

  memset(&TRS_CURVS, 0, sizeof(Icurvs));

  if (getenv("MCC_FAST")!=NULL)
    fit=0, MCC_DEBUG_MODE=0;

  MCC_DEBUG_PREFIX=label;
  
  elpLoadOnceElp();
  MCC_TECHFILE = V_STR_TAB[__MCC_MODEL_FILE].VALUE;

  MCC_MODE = MCC_FAST_MODE;

  save_print=V_BOOL_TAB[__SIM_USE_PRINT].VALUE; V_BOOL_TAB[__SIM_USE_PRINT].VALUE=1;
  save_meas=V_BOOL_TAB[__SIM_USE_MEAS].VALUE; V_BOOL_TAB[__SIM_USE_MEAS].VALUE=0;

  if (V_FLOAT_TAB[__SIM_TECHNO_SIZE].SET)
  {
    tec=V_FLOAT_TAB[__SIM_TECHNO_SIZE].VALUE;
    aut=0;
    MCC_LN = tec;
    MCC_WN = tec * 6.0;
    MCC_LP = tec;
    MCC_WP = tec * 12.0;
    MCC_DIF = tec * 2.0;
   }
  else aut=1;

  MCC_VDDmax = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
  elpGeneral[elpGVDDMAX]=MCC_VDDmax;
  MCC_VGS = MCC_VDDmax / 2.0 ;

  MCC_SLOPE = V_FLOAT_TAB[__SIM_INPUT_SLOPE].VALUE*1e12;

  MCC_TEMP = V_FLOAT_TAB[__SIM_TEMP].VALUE;

  cn=strchr(copytn, ' '); if (cn!=NULL) *cn='\0', cn++;
  cp=strchr(copytp, ' '); if (cp!=NULL) *cp='\0', cp++;

  if ((search=strchr(copytn, '('))!=NULL)
  {
    cn = search;
    *cn='\0';
    cn++;
    if ((c=strchr(cn,')'))!=NULL) *c='\0';
    MCC_TN_SUBCKT=namealloc(cn);
    if (c != NULL) cn = c+1;
  }
  
  if ((search=strchr(copytp, '('))!=NULL)
  {
    cp = search;
    *cp='\0';
    cp++;
    if ((c=strchr(cp,')'))!=NULL) *c='\0';
    MCC_TP_SUBCKT=namealloc(cp);
    if (c != NULL) cp = c+1;
  }

  MCC_TNMODEL = mcc_initstr (copytn);
  MCC_TPMODEL = mcc_initstr (copytp);

  MCC_OPTIM_MODE = 0 ;
  MCC_CALC_CUR     = MCC_SIM_MODE;

  lotrsparam_n = mcc_init_lotrsparam ();
  lotrsparam_p = mcc_init_lotrsparam ();

  if ( lotrsparam_n->VBULK < ELPMINVBULK )
    lotrsparam_n->VBULK = 0.0;
  if ( lotrsparam_p->VBULK < ELPMINVBULK )
    lotrsparam_p->VBULK = MCC_VDDmax;

  mcc_parse_instance_spec(cn, lotrsparam_n, 0);
  mcc_parse_instance_spec(cp, lotrsparam_p, 1);
  
  res = mcc_genelp (spice, 0, tec, aut, 
                    fit, &modeln, &modelp,
                    lotrsparam_n,lotrsparam_p);
  
  if (modeln==NULL && modelp==NULL && aut==1)
  {
    if (modeln==NULL) avt_errmsg(MCC_ERRMSG, "027", AVT_ERROR, copytn) ;
    if (modelp==NULL) avt_errmsg(MCC_ERRMSG, "027", AVT_ERROR, copytp) ;
  }
  else
  {
    MCC_DEBUG_GATE = NULL ;
    MCC_DEBUG_RSAT = 'Y' ;
    MCC_DEBUG_LEAK = 'N' ;
    MCC_DEBUG_RLIN = 'Y' ;
    MCC_DEBUG_VTH = 'Y' ;
    MCC_DEBUG_QINT = 'Y' ;
    MCC_DEBUG_JCT_CAPA = 'Y' ; // Junction capacitance => Cjbd, Cjbs...
    MCC_DEBUG_INPUTCAPA = 'Y' ;
    MCC_DEBUG_CARAC = 'Y' ;
    MCC_DEBUG_CARACNEG = 'Y' ;
    MCC_DEBUG_DEG = 'Y' ;
    MCC_DEBUG_PASS_TRANS  = 'X' ; // value can be 'N' or 'P'
    MCC_DEBUG_SWITCH = 'X' ;      // value can be 'N' or 'P'
    MCC_DEBUG_BLEEDER = 'N' ;   

    MCC_PLOT=1;
    MCC_OPTIM_MODE = 1 ;
    MCC_CALC_CUR = MCC_CALC_MODE;
  
    elp_lotrs_param_free ( lotrsparam_n );
    elp_lotrs_param_free ( lotrsparam_p );
    lotrsparam_n = mcc_init_lotrsparam ();
    lotrsparam_p = mcc_init_lotrsparam ();
  
    modeln=modelp=NULL;
    if ( lotrsparam_n->VBULK < ELPMINVBULK )
      lotrsparam_n->VBULK = 0.0;
    if ( lotrsparam_p->VBULK < ELPMINVBULK )
      lotrsparam_p->VBULK = MCC_VDDmax;
  
    mcc_parse_instance_spec(cn, lotrsparam_n, 0);
    mcc_parse_instance_spec(cp, lotrsparam_p, 1);
    
    res = mcc_genelp (0, vt, tec, aut, 
                      0, &modeln, &modelp,
                      lotrsparam_n,lotrsparam_p);
  
    mcc_gencurv (MCC_PARAM,lotrsparam_n,lotrsparam_p);
    newtech=mcc_check_subckt(newtn, newtp);

    if (fit)
    {
      mcc_calcul_ibranch(lotrsparam_n, lotrsparam_p) ;
      mcc_genspi_for_inverter_simulator(lotrsparam_n,lotrsparam_p);
    }

    mcc_drvelp (MCC_PARAM, MCC_TRANS_B,lotrsparam_n,lotrsparam_p,MCC_DRV_ALL_MODEL);
//    mcc_gencurv (MCC_PARAM,lotrsparam_n,lotrsparam_p);
    mcc_printplot (MCC_PARAM, 1, ncurrent_vt, pcurrent_vt, modeln, modelp);
    if (fit && MCC_FLAG_FIT==MCC_FIT_OK)
    {
      mcc_fit (lotrsparam_n,lotrsparam_p,1);
    }
    else
    {
      mcc_gencurv (MCC_FIT,lotrsparam_n,lotrsparam_p);
    }
  //  mcc_printplot (MCC_FIT, 0);

    mcc_drive_summary(mcc_getmccname(modeln));

    if(MCC_DEBUG_MODE > 1) mcc_genbench() ;
    if (newtech!=NULL)
     {
       printf("*\n*\n");
       printf("* The original technology file contains .subckt statement probably with\n"
               "* parameter evaluations. To ensure the correct evaluation of the parameters\n"
               "* a parameter-free technology file as been genereted :\n*   \"%s\"\n"
               "* WARNING: THIS FILE CONTAINS SENSIBLE INFORMATION\n"
               "*\n*\n"
               , newtech
              );
     }
    
  }

  elp_lotrs_param_free ( lotrsparam_n );
  lotrsparam_n = NULL;
  elp_lotrs_param_free ( lotrsparam_p );
  lotrsparam_p = NULL;
  ELP_LOAD_FILE_TYPE=save_ELP_LOAD_FILE_TYPE;
  MCC_DEBUG_PREFIX="";
  V_BOOL_TAB[__SIM_USE_PRINT].VALUE=save_print;
  V_BOOL_TAB[__SIM_USE_MEAS].VALUE=save_meas;
  MCC_TN_SUBCKT=MCC_TP_SUBCKT=NULL;

  if (newtech!=NULL)
  {
    sprintf(buf,"%s_no_param", label);
    printf("*\n"
           "* Consider re-runing the techno check API with the new techno file\n"
           "* setting: avt_config avtTechnologyName \"%s\"\n"
           "* using:   avt_CheckTechno \"%s\" \"%s %s\" \"%s %s\"\n"
           "*\n\n"
           , newtech,buf,newtn, cn, newtp, cp
           );
/*    
    mcc_deltechnofile(MCC_MODELFILE);
    elpFreeModel();

    oldtech=V_STR_TAB[__MCC_MODEL_FILE].VALUE;
    if (oldtech!=NULL) oldtech=sensitive_namealloc(oldtech);
    avt_sethashvar("avtTechnologyName", newtech);
    mcc_CheckTechno(buf, copytn, copytp);
    avt_sethashvar("avtTechnologyName", oldtech);
    
    mcc_deltechnofile(MCC_MODELFILE);
    elpFreeModel();

    printf("*\n*\n");
    printf("* WARNING: A parameter-free technology file as been genereted :\n"
           "* WARNING:  \"%s\"\n"
           "* WARNING: THIS FILE CONTAINS SENSIBLE INFORMATION\n"
           "*\n*\n"
            , newtech
           );
*/
  }
}

mcc_modifiedparam* mcc_setparamlist( mcc_modellist *model, chain_list *head, double value )
{
  int n;
  chain_list *chain ;
  mcc_modifiedparam *param ;

  for( n=0, chain=head ; chain ; chain=chain->NEXT, n++ );
  param = mbkalloc( sizeof( mcc_modifiedparam ) * (n+1) );

  for( n=0, chain=head ; chain ; chain=chain->NEXT, n++ ) {
    param[n].LABEL = (char*)chain->DATA ;
    param[n].VALUE = mcc_getparam( model, param[n].LABEL );
    mcc_setparam( model, param[n].LABEL, value );
  }
  param[n].LABEL = NULL ;
  param[n].VALUE = -1 ;

  return param ;
}

void mcc_restoreparamlist( mcc_modellist *model, mcc_modifiedparam *param )
{
  int n;

  if( !param )
    return ;

  n = 0;
  while( param[n].LABEL ) {
    mcc_setparam( model, param[n].LABEL, param[n].VALUE );
    n++ ;
  }
  mbkfree( param );
}

void mcc_hacktechno( lofig_list *lofig, int whatjob )
{
 lotrs_list    *lotrs ;
 char          *modelname ;
 float          l, w ;
 char           lotrstype ;
 mcc_modellist *model ;
 ht            *drivedmodel ;
 FILE          *file ;
 chain_list    *headparam ;
 mcc_modifiedparam *keepvalue0 ;
 mcc_modifiedparam *keepvalue1 ;
 
  drivedmodel = addht(2);
  file = mbkfopen( "techno_hack", "spi", "w" );
  fprintf( file, "* hacked model without capacitance\n" );

  for( lotrs = lofig->LOTRS ; lotrs ; lotrs = lotrs->NEXT ) {
  
    modelname = getlotrsmodel( lotrs ) ;
    lotrstype = MLO_IS_TRANSN(lotrs->TYPE) ? elpNMOS : elpPMOS ;
    l         = ((double)lotrs->LENGTH)/((double)SCALE_X)*1e-6;
    w         = ((double)lotrs->WIDTH)/((double)SCALE_X)*1e-6;
    
    model = mcc_getmodel( MCC_MODELFILE, modelname, lotrstype, MCC_TYPICAL, l, w, 1 );
    
    if( gethtitem( drivedmodel, model )==EMPTYHT ) {
    
      addhtitem( drivedmodel, model, 1 );
     
      keepvalue0 = NULL ;
      keepvalue1 = NULL ;

      if( whatjob == MCC_HACK_REMOVE_LOTRS_CAPA ) {
      
        /* remove intrinsic current */
        headparam = NULL ;
        headparam = addchain( headparam, "XPART" );
        keepvalue0 = mcc_setparamlist( model, headparam, -1.0 );
        freechain( headparam );

        headparam = NULL ;
        /* remove gate-source overlap capacitance (doc bsim430.pdf p 85) */
        headparam = addchain( headparam, "CGSO" );
        headparam = addchain( headparam, "CGSL" );
        
        /* remove gate-drain overlap capacitance (doc bsim430.pdf p 85) */
        headparam = addchain( headparam, "CGDO" );
        headparam = addchain( headparam, "CGDL" );

        /* remove fringing capacitance (doc bsim430.pdf p83) */
        headparam = addchain( headparam, "CF" );

        /* remove source diode capacitance (doc bsim430.pdf p109) */
        headparam = addchain( headparam, "CJS"    );
        headparam = addchain( headparam, "CJSWS"  );
        headparam = addchain( headparam, "CJSWGS" );
        
        /* remove drain diode capacitance (doc bsim430.pdf p 109) */
        headparam = addchain( headparam, "CJD"    );
        headparam = addchain( headparam, "CJSWD"  );
        headparam = addchain( headparam, "CJSWGD" );

        /* remove common drain and source diode capacitance (doc bsim430.pdf p 125) */
        headparam = addchain( headparam, "TCJSW" );
      
        keepvalue1 = mcc_setparamlist( model, headparam, 0.0 );
        freechain( headparam );
      }

      mcc_drive_dot_model( file, model );

      /* remet le modèle en sortant dans l'état où on l'a trouvé en entrant... */
      mcc_restoreparamlist( model, keepvalue0 );
      mcc_restoreparamlist( model, keepvalue1 );
    }
  }

  delht( drivedmodel );
  fclose( file );
}

void mccHackNetlist( lofig_list *lofig )
{
lofig_list *hackedlofig ;
char        buffer[1024] ;
int         hacknetlist ;
int         hacktechno ;
int         removeresi ;
int         found ;
char       *pt ;

  hackedlofig = rduplofig( lofig );

  sprintf( buffer, "%s_hack", lofig->NAME );
  hackedlofig->NAME = namealloc( buffer );

  lofigchain( hackedlofig );

  hacknetlist = ELP_HACK_NONE ;
  hacktechno  = MCC_HACK_NONE ;
  removeresi  = 0 ;

  if( V_STR_TAB[ __AVT_HACK_NETLIST_CONFIG ].VALUE ) {
    strcpy( buffer, V_STR_TAB[ __AVT_HACK_NETLIST_CONFIG ].VALUE ) ;

    pt = strtok( buffer, " " );
    
    while( pt ) {
    
      found = 0 ;
      
      if( !strcasecmp( pt, "removeresistance" ) ) {
        removeresi = 1 ;
        found = 1 ;
      }
      
      if( !strcasecmp( pt, "fixcapa" ) ) {
        if( hacknetlist != ELP_HACK_NONE )
          printf( "avtHackNetlistConfig : bad configuration\n" );
        hacknetlist = ELP_HACK_FIX_CAPA ;
        hacktechno  = MCC_HACK_REMOVE_LOTRS_CAPA ;
        found = 1 ;
      }
      
      if( !strcasecmp( pt, "fixcapatoground" ) ) {
        if( hacknetlist != ELP_HACK_NONE )
          printf( "avtHackNetlistConfig : bad configuration\n" );
        hacknetlist = ELP_HACK_FIX_CAPA_TO_GND ;
        hacktechno  = MCC_HACK_REMOVE_LOTRS_CAPA ;
        found = 1 ;
      }

      if( !strcasecmp( pt, "removecapa" ) ) {
        if( hacknetlist != ELP_HACK_NONE )
          printf( "avtHackNetlistConfig : bad configuration\n" );
        hacknetlist = ELP_HACK_NONE ;
        hacktechno  = MCC_HACK_REMOVE_LOTRS_CAPA ;
        found = 1 ;
      }

      pt = strtok( NULL, " " );
    }
  }
 
  if( hacknetlist != ELP_HACK_NONE )
    elpHackNetlistCapa( hackedlofig, hacknetlist );

  if( removeresi )
    elpHackNetlistResi( hackedlofig );
  
  mcc_hacktechno( hackedlofig, hacktechno  );

  savelofig( hackedlofig );
  dellofig( hackedlofig->NAME );
}
