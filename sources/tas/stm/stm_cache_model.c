/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : STM Version 2.00                                             */
/*    Fichier : stm_cache_model.c                                            */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Caroline BLED                                              */
/*                                                                           */
/*****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/
#include "stm.h" 

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/
char    STM_PRELOAD = 'N';
mbkcache*   STM_CACHE=NULL;  

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/ 
extern void  stmmodelrestart(FILE *);
extern int   stmmodelparse() ;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
void stm_enable_cache ( char              (*isactive)(void*,void*),
                        unsigned long int (*fn_load) (void*,void*),
                        unsigned long int (*fn_free) (void*,void*))
{
    if ((STM_CACHE!=NULL) || (STM_CACHE_SIZE==0ul)) 
        return;

    if (STM_CACHE==NULL) {
        STM_CACHE = mbk_cache_create (
                        (char(*)(void*,void*)) isactive,
                        (unsigned long int (*)(void*,void*)) fn_load,
                        (unsigned long int (*)(void*,void*)) fn_free,
                        STM_CACHE_SIZE 
                        );
    }
}

FILE*   stm_cache_getfile (timing_cell *cell)
{
    if (cell->FILE_D != 0) {
        return mbk_cache_get_file (cell->FILE_D);
    }
    return NULL;
}

void    stm_cache_setfile (char *cellname, FILE *f, char *filename, char *extension)
{
    timing_cell *cell;
    int          file;

    if ((cell=stm_getcell (cellname))!= NULL) {
        file = mbk_cache_set_file (f, filename, extension);
        if (cell->FILE_D != 0)
            mbk_cache_clear_file (cell->FILE_D);
        cell->FILE_D = file;
    }
}

void    stm_cache_clearfile (char *cellname)
{
        timing_cell *cell;
        
        if ((cell=stm_getcell (cellname))!=NULL) {
            mbk_cache_clear_file (cell->FILE_D);
            cell->FILE_D = 0;
        }
}

void    stm_cache_setfilepos (char *cellname, char *modelname, MBK_OFFSET_MAX *pos)
{
    timing_cell     *cell;
    timing_model    *model;

    cell = stm_getcell (cellname);
    model = (timing_model*)gethtitem (cell->MODEL_HT, namealloc (modelname));
    model->OFFSET = *pos;    
}


FILE    *stm_cache_getfilepos (char *cellname, timing_model *model, MBK_OFFSET_MAX *pos)
{
    timing_cell     *cell;
    FILE            *file;

    cell = stm_getcell (cellname);
    if (cell) {
        *pos = model->OFFSET;
        if (*pos == (MBK_OFFSET_MAX)0)
            return NULL;
        else
            return stm_cache_getfile (cell);
    }
    else
        return NULL;
}

unsigned long int   stm_cache_sizeof_table (timing_table *table) 
{
    unsigned long int   size=0;
    
    /* size of SET1D */
    if (table->SET1D)
        size += table->NX*sizeof (float);

    /* size of SET2D */
    if (table->SET2D) {
        size += (table->NX*table->NY)*sizeof (float);
        size += table->NX*sizeof(float*);
    }

    /* size of XRANGE */
    if (table->XRANGE)
        size += table->NX*sizeof (float);

    /* size of YRANGE */
    if (table->YRANGE)
        size += table->NY*sizeof (float);

    /* size of the timing_table structure */
    size += sizeof (timing_table);

    return size;
}

unsigned long int   stm_cache_sizeof_linkips (float *li) 
{
    unsigned long int   size;
    float   nb;

    nb = *li; /* la premiere case indique le nombre de groupes de parametres */
    size = ((nb*STM_NB_LINK_I_PARAMS)+1)*sizeof (float);

    return size;
}

unsigned long int   stm_cache_sizeof_scm (timing_scm *scm) 
{
    unsigned long int   size=0;
    uparams             params;

    params = scm->PARAMS;
   
    switch (scm->TYPE) {    
        case STM_MODSCM_DUAL    :   if (params.DUAL->DP)
                                        size += STM_NB_DUAL_PARAMS*sizeof (float);
                                    size += sizeof (dualparams);
                                    break;
        case STM_MODSCM_GOOD    :   if (params.GOOD->DP)
                                        size += STM_NB_DUAL_PARAMS*sizeof (float);
                                    if (params.GOOD->L0)
                                        size += STM_NB_LINK_0_PARAMS*sizeof (float);
                                    if (params.GOOD->LI)
                                        size += stm_cache_sizeof_linkips (params.GOOD->LI);
                                    if (params.GOOD->LN)
                                        size += STM_NB_LINK_N_PARAMS*sizeof (float);
                                    size += sizeof (goodparams);
                                    break;
        case STM_MODSCM_FALSE   :   if (params.FALS->FP)
                                        size += STM_NB_FALSE_PARAMS*sizeof (float);
                                    if (params.FALS->L0)
                                        size += STM_NB_LINK_0_PARAMS*sizeof (float);
                                    if (params.FALS->LI)
                                        size += stm_cache_sizeof_linkips (params.FALS->LI);
                                    if (params.FALS->LN)
                                        size += STM_NB_LINK_N_PARAMS*sizeof (float);
                                    size += sizeof (falseparams);
                                    break;
        case STM_MODSCM_PATH    :   if (params.PATH->PP)
                                        size += STM_NB_PATH_PARAMS*sizeof (float);
                                    if (params.PATH->L0)
                                        size += STM_NB_LINK_0_PARAMS*sizeof (float);
                                    if (params.PATH->LI)
                                        size += stm_cache_sizeof_linkips (params.PATH->LI);
                                    if (params.PATH->LN)
                                        size += STM_NB_LINK_N_PARAMS*sizeof (float);
                                    size += sizeof (pathparams);
                                    break;
        case STM_MODSCM_CST     :   if (params.CST->DS)
                                        size += STM_NB_CST_PARAMS*sizeof (float);
                                    size += sizeof (cstparams);
                                    break;
    }
    
    /* size of the timing_scm structure */
    size += sizeof (timing_scm);

    return size;
}

unsigned long int   stm_cache_sizeof_pln (timing_polynom *pln) 
{
    unsigned long int   size=0ul;



    return size;
}

unsigned long int   stm_cache_sizeof_fct (timing_function *func) 
{
    unsigned long int   size=0ul;



    return size;
}

unsigned long int   stm_cache_sizeof_iv (timing_iv *iv) 
{
    unsigned long int   size=0ul;


    return size;
}

unsigned long int   stm_cache_sizeof_noisescr (noise_scr *scr) 
{
    unsigned long int   size=0ul;

    
    return size;
}


unsigned long int   stm_cache_sizeof_energytable (timing_table *table) 
{
    unsigned long int   size=0ul;

    
    return size;
}

unsigned long int   stm_cache_sizeof_energyparams (energyparams *params) 
{
    unsigned long int   size=0ul;

    
    return size;
}


unsigned long int   stm_cache_sizeof_model (timing_model *model) 
{
    unsigned long int size_umodel=0ul;
    unsigned long int size_noisemodel=0ul;
    unsigned long int size_energymodel=0ul;

    switch (model->UTYPE) {
        case STM_MOD_MODTBL:  size_umodel = stm_cache_sizeof_table (model->UMODEL.TABLE);
            break;
        case STM_MOD_MODSCM:  size_umodel = stm_cache_sizeof_scm (model->UMODEL.SCM);
            break;
        case STM_MOD_MODPLN:  size_umodel = stm_cache_sizeof_pln (model->UMODEL.POLYNOM);
            break;
        case STM_MOD_MODFCT:  size_umodel = stm_cache_sizeof_fct (model->UMODEL.FUNCTION);
            break;
        case STM_MOD_MODIV :  size_umodel = stm_cache_sizeof_iv (model->UMODEL.IV);
            break;
        default:    size_umodel = 0ul;                    
    }

    switch (model->NOISETYPE) {
        case STM_NOISE_SCR:   size_noisemodel = stm_cache_sizeof_noisescr (model->NOISEMODEL.SCR); 
            break;
        default:    size_noisemodel = 0ul;                      
    }

    switch (model->ENERGYTYPE) {
        case STM_ENERGY_TABLE:   size_energymodel = stm_cache_sizeof_energytable (model->ENERGYMODEL.ETABLE); 
            break;
        case STM_ENERGY_PARAMS:  size_energymodel = stm_cache_sizeof_energyparams (model->ENERGYMODEL.EPARAMS); 
            break;
        default:    size_energymodel = 0ul;                      
    }

    return (size_umodel + size_noisemodel + size_energymodel);
}


unsigned long int   stm_cache_parse_model (char *cellname, timing_model *model)
{
    MBK_OFFSET_MAX   offset;
    FILE            *file;

//    STM_PRELOAD = 'N';
    file = stm_cache_getfilepos (cellname, model, &offset); 
    if (file) {
        if (!mbkfseek (file, &offset, SEEK_SET)) {
            fflush (stdout);
            avt_errmsg (STM_ERRMSG, "036", AVT_FATAL);
        }
        stmmodelrestart (file);
//        printf ("REFRESH model: %s in cell: %s size:", model->NAME, cellname);
        STM_CNAME = cellname;
        stmmodelparse();
//        printf ("%ld\n", stm_cache_sizeof_model (model));
        return stm_cache_sizeof_model (model);
    }
    else
        return 0;
}

unsigned long int    stm_cache_free_model (char *cellname, timing_model *model)
{
    ptype_list *ptype;
    unsigned long int    size;
    
    
//    printf ("FREE model: %s\n", model->NAME); 
    size = stm_cache_sizeof_model (model);
    
    switch (model->UTYPE) {
        case STM_MOD_MODTBL:
            stm_modtbl_destroy (model->UMODEL.TABLE);
            model->UMODEL.TABLE = NULL;
            break;
        case STM_MOD_MODSCM:
            stm_modscm_destroy (model->UMODEL.SCM);
            model->UMODEL.SCM = NULL;
            break;
        case STM_MOD_MODPLN:
            stm_modpln_destroy (model->UMODEL.POLYNOM);
            model->UMODEL.POLYNOM = NULL;
            break;
        case STM_MOD_MODFCT:
            stm_modfct_destroy (model->UMODEL.FUNCTION);
            model->UMODEL.FUNCTION = NULL;
            break;
    }
    if ((ptype = getptype (model->USER, STM_CARAC_VALUES))) {
        mbkfree ((stm_carac_values*)ptype->DATA);
        model->USER = delptype (model->USER, STM_CARAC_VALUES);
        model->USER = NULL;
    }
    stm_noise_destroy (model);
    stm_energy_destroy (model);

    return size;
}


void    stm_cache_refresh_model (char *cellname, timing_model *model)
{
    if (STM_CACHE == NULL)
        return;
    
    mbk_cache_refresh (STM_CACHE, cellname, model);
}

char    stm_cache_isactive (char *cellname, timing_model *model)
{
    timing_cell *cell;
    
    if (cellname == NULL)
        return 0;
    if (STM_CACHE == NULL)
        return 0;
    
    cell = stm_getcell (cellname);

    if (!cell)  
        return 0;

    if (!cell->FILE_D)
        return 0;

    return 1;
}

