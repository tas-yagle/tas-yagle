#include "mcc_tsort.h"
#include "mcc_util.h"

//-------------------------------------------------------------------------

char *mcc_MakeScript (lofig_list *fig, double m, char *options)
{
    tlist_t    *models;
    couple_t   *couples;

    models = mcc_SortModels (fig, m);
    couples = mcc_MakeCouples (models);
    return mcc_DumpScript (couples,options);
}

//-------------------------------------------------------------------------

int mcc_GetL (char *size)
{
    char *pt = strchr (size, ' ');
    if (!pt) return -1;
    else return atoi (pt+1);
}

//-------------------------------------------------------------------------

int mcc_GetW (char *size)
{
    char buf[1024];
    char *pt; 

    strcpy (buf, size);
    if (!(pt = strchr (buf, ' '))) return -1;
    else {
        *pt = '\0';
        return atoi (buf);
    }
}

//-------------------------------------------------------------------------

char *mcc_GetCoupledPTrans (char *model, chain_list *plist, chain_list *nlist)
{
    chain_list *mp, *mn;
    int         i, j;

    for (mn = nlist, j = 0; mn; mn = mn->NEXT, j++)
        if ((char*)mn->DATA == model) break;

    if (mn) {
        for (mp = plist, i = 0; mp->NEXT && i < j; mp = mp->NEXT, i++);
        return (char*)mp->DATA;
    } 
    else {
        fprintf (stderr, "error: no N model %s\n", model);
        return NULL;
    }
}

//-------------------------------------------------------------------------

char *mcc_GetCoupledNTrans (char *model, chain_list *plist, chain_list *nlist)
{
    chain_list *mp, *mn;
    int         i, j;

    for (mp = plist, j = 0; mp; mp = mp->NEXT, j++)
        if ((char*)mp->DATA == model) break;

    if (mp) {
        for (mn = nlist, i = 0; mn->NEXT && i < j; mn = mn->NEXT, i++);
        return (char*)mn->DATA;
    } 
    else {
        fprintf (stderr, "error: no P model %s\n", model);
        return NULL;
    }
}

//-------------------------------------------------------------------------

int mcc_IsACouple (int lp, int ln, int wp, int wn)
{
    int lok = 0, wok = 0;
    if (lp - 0.2 * lp <= ln && lp + 0.2 * lp >= ln) lok = 1;
    if (wp >= wn && wp <= 4 * wn) wok = 1;
    return lok && wok;
}


//-------------------------------------------------------------------------

couple_t *mcc_MakeCouples (tlist_t *models)
{
    chain_list *p_msizes = NULL, *n_msizes = NULL;
    tlist_t    *tl, *tl2, *head_tlist = NULL;
    chain_list *p_ch, *n_ch, *ch;
    chain_list *pn_models = NULL;
    char       *p_model, *n_model;
    couple_t   *couple, *head_couple = NULL;
    int         ln, lp, wn, wp;
    char       *size, buf[1024];

    TPMOS = reverse (TPMOS);
    TNMOS = reverse (TNMOS);

    // order models: P ones then N ones 
    for (tl = models; tl; tl = tl->NEXT)
        if (mbk_istransp (tl->MODEL))
            pn_models = addchain (pn_models, tl);
    for (tl = models; tl; tl = tl->NEXT)
        if (mbk_istransn (tl->MODEL))
            pn_models = addchain (pn_models, tl);
    for (ch = pn_models; ch; ch = ch->NEXT) {
        if (!head_tlist) {
            head_tlist = ch->DATA;
            head_tlist->NEXT = NULL;
        }
        else {
            ((tlist_t*)ch->DATA)->NEXT = head_tlist;
            head_tlist = (tlist_t*)ch->DATA;
        }
    }
    freechain (pn_models);

    for (tl = head_tlist; tl; tl = tl->NEXT) {
        if (mbk_istransp (tl->MODEL)) { // assoc P models with N models
            p_model = tl->MODEL;
            p_msizes = tl->MSIZES;
            n_model = mcc_GetCoupledNTrans (p_model, TPMOS, TNMOS);
            for (tl2 = head_tlist; tl2; tl2 = tl2->NEXT) {
                if (tl2->MODEL == n_model) {
                    n_msizes = tl2->MSIZES;
                    break;
                }
            }

            for (p_ch = p_msizes; p_ch; p_ch = p_ch->NEXT) {
                lp = mcc_GetL ((char*)p_ch->DATA);
                wp = mcc_GetW ((char*)p_ch->DATA);
                for (n_ch = n_msizes; n_ch; n_ch = n_ch->NEXT) {
                    size = (char*)n_ch->DATA;
                    if (size[0] == 'F') continue;
                    ln = mcc_GetL ((char*)n_ch->DATA);
                    wn = mcc_GetW ((char*)n_ch->DATA);
                    if (mcc_IsACouple (lp, ln, wp, wn)) break;
                }

                couple = (couple_t*)mbkalloc (sizeof (struct couple_t));
                couple->PMODEL = p_model;
                couple->NMODEL = n_model;
                couple->PSIZE = (char*)p_ch->DATA;

                if (!head_couple) {
                    head_couple = couple;
                    head_couple->NEXT = NULL;
                }
                else {
                    couple->NEXT = head_couple;
                    head_couple = couple;
                }

                if (!n_ch)
                    couple->NSIZE = (char*)p_ch->DATA;
                else {
                    couple->NSIZE = (char*)n_ch->DATA;
                    sprintf (buf, "F%s", (char*)n_ch->DATA);
                    n_ch->DATA = buf;
                }
            }
        }
        else { //remaining N models
            n_model = tl->MODEL;
            n_msizes = tl->MSIZES;
            p_model = mcc_GetCoupledPTrans (n_model, TPMOS, TNMOS);
            for (n_ch = n_msizes; n_ch; n_ch = n_ch->NEXT) {
                size = (char*)n_ch->DATA;
                if (size[0] == 'F') continue;
                ln = mcc_GetL (size);
                wn = mcc_GetW (size);
                couple = (couple_t*)mbkalloc (sizeof (struct couple_t));
                couple->PMODEL = p_model;
                couple->NMODEL = n_model;
                couple->NSIZE = size;
                couple->PSIZE = size;
                couple->NEXT = head_couple;
                head_couple = couple;
            }
        }
    }

    TPMOS = reverse (TPMOS);
    TNMOS = reverse (TNMOS);

    return head_couple;
}

//-------------------------------------------------------------------------

tlist_t *mcc_SortModels (lofig_list *figlist, double m)
{
    lotrs_list *tr;
    lofig_list *fig;
    tlist_t *models = NULL;
    chain_list *sz, *msz;
    tlist_t    *tl;
    char       *model, *size, *msize;
    char        buf[1024];
    long        item;
    int         /*i,*/ w, l, mw, ml;

    for (fig = figlist; fig; fig = fig->NEXT) {
        for (tr = fig->LOTRS; tr; tr = tr->NEXT) {
            model = getlotrsmodel (tr);
            for (tl = models; tl; tl = tl->NEXT) {
                if (tl->MODEL == model) {
                    sprintf (buf, "%ld %ld", tr->WIDTH, tr->LENGTH);
                    size = namealloc (buf);
                    item = gethtitem (tl->TRANSHT, size);
                    if (item == EMPTYHT) {
                        addhtitem (tl->TRANSHT, size, (long)addchain (NULL, tr));
                        tl->SIZES = addchain (tl->SIZES, size);
                    }
                    else addhtitem (tl->TRANSHT, size, (long)addchain ((chain_list*)item, tr));
                    break;
                }
            }
            if (!tl) {
                tl = (tlist_t*)mbkalloc (sizeof (struct tlist_t));
                tl->MODEL = model;
                tl->TRANSHT = addht (11);
                sprintf (buf, "%ld %ld", tr->WIDTH, tr->LENGTH);
                size = namealloc (buf);
                tl->SIZES = addchain (NULL, size);
                tl->MSIZES = NULL;
                addhtitem (tl->TRANSHT, size, (long)addchain (NULL, tr));
                tl->NEXT = models;
                models = tl;
            }
        }
    }

    for (tl = models; tl; tl = tl->NEXT) {
        for (sz = tl->SIZES; sz; sz = sz->NEXT) {
            size = (char*)sz->DATA;
            w = mcc_GetW (size);
            l = mcc_GetL (size);
            if (!tl->MSIZES)
                tl->MSIZES = addchain (NULL, size);
            else {
                for (msz = tl->MSIZES; msz; msz = msz->NEXT) {
                    msize = (char*)msz->DATA;
                    mw = mcc_GetW (msize);
                    ml = mcc_GetL (msize);
                    //if (((l * (1 - m) <= ml && l * (1 + m) >= ml)) && ((w * (1 - m) <= mw && w * (1 + m) >= mw)))
                    if (l <= ml*(1+m) && l >= ml * (1-m) && w <= mw*(1+m) && w >= mw*(1-m))
                        break;
                }
                if (!msz) tl->MSIZES = addchain (tl->MSIZES, size);
            }
        }
    }

    return models;
}

//-------------------------------------------------------------------------

void mcc_CleanModels (tlist_t *models)
{
    chain_list *sz;
    tlist_t    *tl;
    char       *size;
    long        item;
    
    for (tl = models; tl; tl = tl->NEXT) {
        for (sz = tl->SIZES; sz; sz = sz->NEXT) {
            size = (char*)sz->DATA;
            if ((item = gethtitem (tl->TRANSHT, size)) != EMPTYHT)
                freechain ((chain_list*)item);
        }
        freechain (tl->SIZES);
        freechain (tl->MSIZES);
        delht (tl->TRANSHT);
    }
}

//-------------------------------------------------------------------------

void mcc_DumpModels (tlist_t *models)
{
    chain_list *sz, *msz, *chtr;
    lotrs_list *tr;
    tlist_t    *tl;
    char       *size, *msize;
    long        item;
    int         nbtr;
    
    for (tl = models; tl; tl = tl->NEXT) {
        fprintf (stdout, "model '%s'\n", tl->MODEL);
        for (sz = tl->SIZES; sz; sz = sz->NEXT) {
            size = (char*)sz->DATA;
            fprintf (stdout, "    W=%d L=%d ", mcc_GetW (size), mcc_GetL (size));
            item = gethtitem (tl->TRANSHT, size);
            for (chtr = (chain_list*)item, nbtr = 0; chtr; chtr = chtr->NEXT, nbtr++) {
                tr = (lotrs_list*)chtr->DATA;
                fprintf (stdout, "        %s\n", tr->TRNAME);
            }
            fprintf (stdout, "(%d)\n", nbtr);
        }
    }

    fprintf (stdout, "=================== CARAC VALUES ======================\n");
    for (tl = models; tl; tl = tl->NEXT) {
        fprintf (stdout, "model '%s'\n", tl->MODEL);
        for (msz = tl->MSIZES; msz; msz = msz->NEXT) {
            msize = (char*)msz->DATA;
            fprintf (stdout, "        W=%d L=%d \n", mcc_GetW (msize), mcc_GetL (msize));
        }
    }
}

//-------------------------------------------------------------------------

char *mcc_DumpScript (couple_t *couples, char *options)
{
    couple_t   *cpl;
    double      wp, lp, wn, ln;
    int first = 1;
    FILE *f;
    char buf[1024];
    char *elptech;

    if (!couples) return NULL;

    if (!(elptech = V_STR_TAB[__ELP_TECHNO_NAME].VALUE))
        elptech = MCC_ELPFILE;
    
    sprintf (buf, "%s.script", elptech);
    if (!(f = mbkfopen (buf, NULL, WRITE_TEXT))) {
        fprintf (stderr, "error: can't open file %s\n", buf);
        return NULL;
    }
            

    fprintf (f, "#!/bin/csh -f\n");
    fprintf (f, "set temp           = 'TEMP = %g'\n", MCC_TEMP);
    fprintf (f, "set vdd            = 'VDD = %g'\n", MCC_VDDmax);
    fprintf (f, "set vgs            = 'VGS = %g'\n", MCC_VDDmax);
    fprintf (f, "set elpname        = 'ELPNAME = %s'\n", elptech);
    fprintf (f, "set techname       = 'TECHNAME = %s'\n", MCC_TECHFILE);
    fprintf (f, "set modelname      = 'MODELNAME = %s'\n", MCC_MODELFILE);
    fprintf (f, "set spicestring    = 'SPICESTRING = %s'\n", MCC_SPICESTRING);
    fprintf (f, "set spicetool      = 'SPICETOOL = %s'\n", MCC_SPICENAME);
    fprintf (f, "set spiceout       = 'SPICEOUT = %s'\n", MCC_SPICEOUT);
    fprintf (f, "set spicestdout    = 'SPICESTDOUT = %s'\n", MCC_SPICESTDOUT);
    fprintf (f, "set transtep       = 'STEP = %g'\n", V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9);
    fprintf (f, "set dcstep         = 'STEP = %g'\n", MCC_DC_STEP);
    //fprintf (f, "set spicemodeltype = 'SPICEMODELTYPE = MM9'\n");
    //fprintf (f, "set spicelevel     = 'SPICELEVEL = 59'\n");

        
    fprintf (f, "\n");
            
    for (cpl = couples; cpl; cpl = cpl->NEXT) {
        fprintf (f, "set tnmodelname    = 'TNMODELNAME = %s'\n", cpl->NMODEL);
        fprintf (f, "set tpmodelname    = 'TPMODELNAME = %s'\n", cpl->PMODEL);

        ln = (double)mcc_GetL (cpl->NSIZE) / 1000;
        wn = (double)mcc_GetW (cpl->NSIZE) / 1000;
        lp = (double)mcc_GetL (cpl->PSIZE) / 1000;
        wp = (double)mcc_GetW (cpl->PSIZE) / 1000;


        fprintf (f, "set time = 'TIME = %g'\n", V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9);
        fprintf (f, "set ln = 'LN = %g'\n", ln);
        fprintf (f, "set lp = 'LP = %g'\n", lp);
        fprintf (f, "set wn = 'WN = %g'\n", wn);
        fprintf (f, "set wp = 'WP = %g'\n", wp);

        fprintf (f, "echo $ln              > .mccgenelp\n");
        fprintf (f, "echo $wn             >> .mccgenelp\n");
        fprintf (f, "echo $lp             >> .mccgenelp\n");
        fprintf (f, "echo $wp             >> .mccgenelp\n");
        fprintf (f, "echo $time           >> .mccgenelp\n");
        fprintf (f, "echo $transtep       >> .mccgenelp\n");
        fprintf (f, "echo $dcstep         >> .mccgenelp\n");
        fprintf (f, "echo $temp           >> .mccgenelp\n");
        fprintf (f, "echo $vdd            >> .mccgenelp\n");
        fprintf (f, "echo $vgs            >> .mccgenelp\n");
        fprintf (f, "echo $elpname        >> .mccgenelp\n");
        fprintf (f, "echo $techname       >> .mccgenelp\n");
        fprintf (f, "echo $modelname      >> .mccgenelp\n");
        fprintf (f, "echo $spicestring    >> .mccgenelp\n");
        fprintf (f, "echo $spicetool      >> .mccgenelp\n");
        fprintf (f, "echo $spiceout       >> .mccgenelp\n");
        fprintf (f, "echo $spicestdout    >> .mccgenelp\n");
        //fprintf (f, "echo $spicemodeltype >> .mccgenelp\n");
        //fprintf (f, "echo $spicelevel     >> .mccgenelp\n");
        fprintf (f, "echo $tnmodelname    >> .mccgenelp\n");
        fprintf (f, "echo $tpmodelname    >> .mccgenelp\n");

        fprintf (f, "echo '>>> Characterization of couple (%s, %s) (LN=%g, WN=%g, LP=%g WP=%g) <<<'\n", 
                    cpl->NMODEL, cpl->PMODEL, ln, wn, lp, wp);

        if (first) {
            fprintf (f, "genelp -f %s\n\n",options);
            first = 0;
        }
        else
            fprintf (f, "genelp -n -f %s\n\n",options);
    }

    fclose (f);

    return strdup (buf);
}

//-------------------------------------------------------------------------

void mcc_DumpCouples (couple_t *head_couple)
{
    couple_t *couple;
    int nbc = 0;

    fprintf (stdout, "================ COUPLES \n\n");
    for (couple = head_couple; couple; couple = couple->NEXT) {
        fprintf (stdout, ">>>> couple (%s, %s)\n", couple->PMODEL, couple->NMODEL);
        fprintf (stdout, "     P: %s\n", couple->PSIZE);
        fprintf (stdout, "     N: %s\n\n", couple->NSIZE);
        nbc++;
    }
    fprintf (stdout, "=====>>>> %d  COUPLES \n\n", nbc);
}
    
