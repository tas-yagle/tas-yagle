/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_name_interface.c                                        */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

timing_cell *stm_getmemcell (char *cellname)
{
    long  cell;
    char *cname;

    if (!STM_CELLS_HT)
       return NULL;

    cname = namealloc (cellname);
    cell = gethtitem (STM_CELLS_HT, cname);

    if ((cell == EMPTYHT) || (cell == DELETEHT)) 
       return NULL;
    else
       return (timing_cell*)cell;
}
    
/****************************************************************************/

void stm_fctparse (timing_cell *cell)
{
  cell = NULL;
#if 0
    FILE             *f;
    ExecutionContext *ec;
    tree_list        *tr, 
                     *func;
    char              buf[1024];
    chain_list       *ch;
    timing_model     *model;
    long              mod_number;
    sim_model        *sc;
    long              l;
    t_arg            *ta;

    sprintf (buf, "%s.sim", cell->NAME);

    if (!(f = mbkfopen (cell->NAME, "sim", READ_TEXT))) {
        fprintf (stderr, "Cannot open file %s\n", buf);
        return;
    }

    LoadDynamicLibraries(NULL);
    APIInit ();

    ec = APINewExecutionContext ();
    tr = APIParseFile (f, strdup (buf), ec);
    APIVerify_C_Functions (ec);

    if (APICheckCFunctions (ec)) return;

    for (ch = cell->MODEL_LIST; ch; ch = ch->NEXT) {
        model = (timing_model*)ch->DATA;
        if (model->UTYPE == STM_MOD_MODFCT) {
            mod_number = model->UMODEL.FUNCTION->MODEL_NUMBER;
            if (mod_number == -1) continue; 
            l = gethtitem (cell->SIM_MODEL_HT, (void*)mod_number);
            if (l == EMPTYHT || l == DELETEHT) {
                sprintf (buf, "SimModel_%ld", mod_number);
                if (!(func = APIGetCFunction (ec, buf))) {
                    fprintf (stderr, "Cannot retrieve function '%s'\n", buf);
                    continue;
                }
                if (APIExecute_C (func, NULL, &ta, ec, 1)) return;

		        // zinaps: ta = retour de fonction
		        // .....
		        // _______
		
                if (!(ta = APIGetGlobalVariable (ec, "SC"))) {
                    fprintf (stderr, "Cannot retrieve SC variable\n");
                    continue;
                }
                sc = *(sim_model**)ta->VALUE;
                addhtitem (cell->SIM_MODEL_HT, (void*)mod_number, (long)sc);
                cell->SIM_MODEL_LIST = addchain (cell->SIM_MODEL_LIST, sc);
            }
            else
                sc = (sim_model*)l;
            model->UMODEL.FUNCTION->MODEL = sc;
        }
    }

    APIFreeExecutionContext (ec);

    fclose (f);
#endif
}

/****************************************************************************/
timing_cell *stm_getcell (char *cellname)
{
    long  cell;
    char *cname;

    if (!STM_CELLS_HT)
        STM_CELLS_HT = addht (STM_NBCELLS);

    cname = namealloc (cellname);
    cell = gethtitem (STM_CELLS_HT, cname);

    if ((cell == EMPTYHT) || (cell == DELETEHT)) {
        stm_parse (cname, STM_IN);
        cell = gethtitem (STM_CELLS_HT, cname);
        stm_fctparse ((timing_cell*)cell);
    }
    else
        return ((timing_cell*)cell);
    
    cell = gethtitem (STM_CELLS_HT, cname);

    if ((cell == EMPTYHT) || (cell == DELETEHT)) 
        return NULL;
    else
        return (timing_cell*)cell;
}
    

/****************************************************************************/
int stm_renamecell( char *oldcellname, char *newcellname )
{
  timing_cell *cell ;
  char        *aname ;
  
  if( !STM_CELLS_HT ) return 0;
  
  aname = namealloc( oldcellname );
  cell = (timing_cell *)gethtitem( STM_CELLS_HT, aname );
  if( (long)cell == DELETEHT || (long)cell == EMPTYHT ) return 0 ;
  delhtitem( STM_CELLS_HT, aname );
  
  aname = namealloc( newcellname );
  addhtitem( STM_CELLS_HT, aname, (long)cell );
  cell->NAME = aname ;

  return 1;
  
}
/****************************************************************************/

void stm_freecell (char *cellname)
{
    long        cell;
    char       *cname;

    if (STM_CELLS_HT) {
        cname = namealloc (cellname);
        cell = gethtitem (STM_CELLS_HT, cname);
        if ((cell != EMPTYHT) && (cell != DELETEHT)) {
            stm_cell_destroy ((timing_cell*)cell);
            delhtitem (STM_CELLS_HT, cname);
        }
    }
}

/****************************************************************************/

timing_cell *stm_addcell (char *cellname)
{
    timing_cell *cell;
    char  *lname;

    if (!STM_CELLS_HT)
        STM_CELLS_HT = addht (STM_NBCELLS);
    lname = namealloc (cellname);
    cell = (timing_cell*)gethtitem (STM_CELLS_HT, lname);
    if (((long)cell == EMPTYHT) || ((long)cell == DELETEHT)) {
        cell = stm_cell_create (lname);
        addhtitem (STM_CELLS_HT, lname, (long)cell);
        STM_CELLS_CHAIN = addchain (STM_CELLS_CHAIN, cell->NAME);
    }

    return cell;
}

/****************************************************************************/

timing_model *stm_addfctmodel (char *cellname, char *modelname, char *ins, char *localname, APICallFunc *cf)
{
    timing_model *model;

    model = stm_mod_create (modelname);
    model->UTYPE = STM_MOD_MODFCT;
    model->UMODEL.FUNCTION = stm_modfct_create (ins, localname, cf);

    stm_storemodel (cellname, modelname, model, 1); /* modele "fct": pas de partage possible */

    return model;
}

/****************************************************************************/

timing_model *stm_addmodel (char *cellname, char *modelname)
{
    timing_model *model;

    model = stm_mod_create (modelname);
    stm_storemodel (cellname, modelname, model, 1); /* modele "vide" : pas de partage possible */

    return model;
}

/****************************************************************************/

timing_model *stm_addscmmodel (char *cellname, char *modelname)
{
    timing_model *model;

    model = stm_mod_create (modelname);
    model->UTYPE = STM_MOD_MODSCM;
    stm_storemodel (cellname, modelname, model, 1); /* modele "vide" : pas de partage possible */

    return model;
}

/****************************************************************************/

timing_model *stm_addtblmodel (char *cellname, char *modelname, int nx, int ny, char xtype, char ytype)
{
    timing_model *model;

    model = stm_mod_create_ftable (modelname, nx, ny, xtype, ytype, STM_MOD_MODTBL);
    stm_storemodel (cellname, modelname, model, 1); /* modele "vide" : pas de partage possible */

    return model;
}

/****************************************************************************/

int stm_getreferences (char *cellname, char *modelname)
{
    timing_model *model;
    timing_cell  *cell;

    if (!cellname)
        return 0;
    if (!(cell = stm_getmemcell (cellname)))
        return 0;
    if (!modelname)
        return 0;

    model = (timing_model*)gethtitem (cell->MODEL_HT, namealloc (modelname));

    if (((long)model != EMPTYHT) && ((long)model != DELETEHT))
        return stm_cell_share_getn (cell, model);

    return 0;
}

/****************************************************************************/

void stm_delreferences (char *cellname, char *modelname, int n)
{
    timing_model *model;
    timing_cell  *cell;

    if (!cellname)
        return;
    if (!(cell = stm_getmemcell (cellname)))
        return;
    if (!modelname)
        return;

    model = (timing_model*)gethtitem (cell->MODEL_HT, namealloc (modelname));

    if (((long)model != EMPTYHT) && ((long)model != DELETEHT))
        stm_cell_share_decrement (cell, model, n);
}

/****************************************************************************/

void stm_addreferences (char *cellname, char *modelname, int n)
{
    timing_model *model;
    timing_cell  *cell;

    if (!cellname)
        return;
    if (!(cell = stm_getmemcell (cellname)))
        return;
    if (!modelname)
        return;

    model = (timing_model*)gethtitem (cell->MODEL_HT, namealloc (modelname));

    if (((long)model != EMPTYHT) && ((long)model != DELETEHT))
        stm_cell_share_increment (cell, model, n);
}

/****************************************************************************/

char *stm_storemodel (char *cellname, char *modelname, timing_model *model, int noshare)
{
    timing_cell  *cell;

    if (!model || !cellname)
        return NULL;

    if (!modelname && !model->NAME)
        return NULL;

    if (modelname)
        model->NAME = namealloc (modelname);

    if (!(cell = stm_getmemcell (cellname)))
        cell = stm_addcell (cellname);

    return stm_cell_addmodel (cell, model, noshare);
}

/****************************************************************************/
    
timing_model *stm_getmodel_sub (char *cellname, char *modelname, int mustexist)
{
    timing_model *model;
    timing_cell  *cell;

    if (!cellname || !modelname)
        return NULL;

    if (!(cell = stm_getcell (cellname))) {
       if (mustexist) avt_errmsg(STM_ERRMSG,"040", AVT_FATAL, cellname);
       return NULL;
    }
    model = (timing_model*)gethtitem (cell->MODEL_HT, namealloc (modelname));
//    model = (timing_model*)gethtitem (cell->MODEL_HT, modelname);

    if (((long)model == EMPTYHT) || ((long)model == DELETEHT))
        return NULL;  

    stm_cache_refresh_model (cellname, model);
    return model;
}

timing_model *stm_getmodel (char *cellname, char *modelname)
{
  return stm_getmodel_sub (cellname, modelname, 0);
}

/****************************************************************************/

void stm_freemodel (char *cellname, char *modelname)
{
    timing_model *model;
    timing_cell  *cell;

    if (!(cell = stm_getmemcell (cellname)))
        return;
    if (!modelname)
        return;

    model = (timing_model*)gethtitem (cell->MODEL_HT, namealloc (modelname));

    if (((long)model != EMPTYHT) && ((long)model != DELETEHT))
        stm_cell_delmodel (cell, model, 0);
}

/****************************************************************************/

void stm_freemodellist (char *cellname, chain_list *modellist)
{
    timing_model *model;
    timing_cell  *cell;
    chain_list   *current, *next;
    char         *lname;
    chain_list   *mlist = NULL;

    if (!(cell = stm_getmemcell (namealloc (cellname)))) {
        avt_errmsg (STM_ERRMSG, "030", AVT_ERROR);
        return;
    }
    
    for (current = modellist; current; current = current->NEXT) {
        lname = namealloc ((char*)current->DATA);
        model = (timing_model*)gethtitem (cell->MODEL_HT, lname);
        if (((long)model != EMPTYHT) && ((long)model != DELETEHT)) { 
            if (!stm_cell_share_decrement (cell, model, 1)) {
                mlist = addchain (mlist, model);
                delhtitem (cell->MODEL_HT, model->NAME);
            } 
        }
    }

    lname = ((timing_model*)cell->MODEL_LIST->DATA)->NAME;
    model = (timing_model*)gethtitem (cell->MODEL_HT, lname);

    while (((long)model == EMPTYHT) || ((long)model == DELETEHT)) { 
        next = cell->MODEL_LIST;
        cell->MODEL_LIST = cell->MODEL_LIST->NEXT;
        next->NEXT   = NULL;
        freechain (next);
        if (cell->MODEL_LIST) {
            lname = ((timing_model*)cell->MODEL_LIST->DATA)->NAME;
            model = (timing_model*)gethtitem (cell->MODEL_HT, lname);
        } else {
            for (current = mlist; current; current = current->NEXT)
                stm_mod_destroy ((timing_model*)current->DATA);
            return;
        }
    }

    current = cell->MODEL_LIST;
    next    = cell->MODEL_LIST->NEXT;

    while (next) {
        lname = ((timing_model*)next->DATA)->NAME;
        model = (timing_model*)gethtitem (cell->MODEL_HT, lname);
        if (((long)model == EMPTYHT) || ((long)model == DELETEHT)) {
            current->NEXT = next->NEXT;
            next->NEXT    = NULL;
            freechain (next);
            next = current->NEXT;
        } else {
            current = current->NEXT;
            next    = next->NEXT;
        }
    } 

    for (current = mlist; current; current = current->NEXT)
        stm_mod_destroy ((timing_model*)current->DATA);
}

/****************************************************************************/

void stm_addconstmodel (char *cell,
                        char *model,
                        float value)
{
    timing_model *newmodel = stm_mod_create_fcst (model, value, STM_MOD_MODTBL);
    stm_storemodel (cell, model, newmodel, 0);
}

/****************************************************************************/

void stm_addequacmodel (char *cell,
                        char *model,
                        float sdt,
                        float sck,
                        float fdt,
                        float fck,
                        float t)
{
    timing_model *newmodel =  stm_mod_create_fequac (model, sdt, sck, fdt, fck, t, STM_MOD_MODTBL);
    stm_storemodel (cell, model, newmodel, 0);
}

/****************************************************************************/

void stm_addequamodel (char *cell,
                       char *model,
                       float r,
                       float s,
                       float c,
                       float f,
                       float t)
    
{
    timing_model *newmodel = stm_mod_create_fequa (model, r, s, c, f, t, STM_MOD_MODTBL);
    stm_storemodel (cell, model, newmodel, 0);
}

/****************************************************************************/

void stm_addreducedmodel (char  *rcell,
                          char  *rmodel,
                          char  *cell,
                          char  *model,
                          float  slew,
                          float  load,
                          char   redmode,
                          char   modeltype)
    
{
    timing_model *rdelaymod;
    timing_model *tmodel;
    
    tmodel = stm_getmodel (cell, namealloc(model)); 
    rdelaymod = stm_mod_reduce (rmodel, tmodel, slew, load, redmode, modeltype);
    stm_storemodel (rcell, rmodel, rdelaymod, 0);
}

/****************************************************************************/

void stm_addreducedcmodel (char *rcell,
                           char *rmodel,
                           char *cell,
                           char *model,
                           float slew,
                           float ckslew,
                           char  redmode)
    
{
    timing_model *rcstrmod;
    timing_model *tmodel;
    
    tmodel = stm_getmodel (cell, namealloc(model));

    rcstrmod  = stm_mod_Creduce (rmodel, tmodel, slew, ckslew, redmode);
    stm_storemodel (rcell, rmodel, rcstrmod, 0);
    
}

/****************************************************************************/

void stm_dupmodelbyname (char *dupcell,
                         char *dupname,
                         char *cell,
                         char *model)
    
{
    timing_model *dupmodel;
    timing_model *tmodel;
    
    tmodel = stm_getmodel (cell, namealloc(model)); 

    dupmodel  = stm_mod_duplicate (dupname, tmodel);
    stm_storemodel (dupcell, dupname, dupmodel, 1);
    
}


char *stm_dupmodelbyname_and_updatedynamicmodelinfo(char *dupcell, char *dupname, char *cell, char *model, char *insname)
{
    timing_model *dupmodel;
    timing_model *tmodel;
    char temp[1024], temp0[1024], *n;

    tmodel = stm_getmodel (cell, model); 
	if (STM_CACHE && tmodel) 
        mbk_cache_lock (STM_CACHE, tmodel);

    switch (tmodel->UTYPE) {
    case STM_MOD_MODTBL:
    case STM_MOD_MODSCM:
    case STM_MOD_MODPLN:
      if (stm_getmodel(dupcell,dupname) == NULL)
        {
          dupmodel  = stm_mod_duplicate (dupname, tmodel);
          dupname = stm_storemodel (dupcell, dupname, dupmodel, 0);
        }
      if (STM_CACHE)  
        mbk_cache_unlock (STM_CACHE, tmodel);
      return dupname;
    case STM_MOD_MODFCT:
      sprintf(temp0,"%s.%s",insname, model);
      n=namealloc(temp0);

      if (strcmp(tmodel->UMODEL.FUNCTION->INS,"?")==0)
        strcpy(temp,insname);
      else
        sprintf(temp,"%s.%s",insname, tmodel->UMODEL.FUNCTION->INS);

      if ((dupmodel=stm_getmodel(dupcell, n)) == NULL)
        {
          dupmodel  = stm_mod_duplicate (n, tmodel);
          stm_storemodel (dupcell, n, dupmodel, 1);
          dupmodel->UMODEL.FUNCTION->INS=namealloc(temp);
        }
      if (STM_CACHE)  
        mbk_cache_unlock (STM_CACHE, tmodel);
      return n;
    }
    if (STM_CACHE)  
        mbk_cache_unlock (STM_CACHE, tmodel);
    return namealloc("?bug?");
}


/****************************************************************************/

void stm_addmergedmodel (char  *cellAB,
                         char  *modelAB,
                         char  *cellA,
                         char  *modelA,
                         char  *cellB,
                         char  *modelB,
                         float  delay,
                         float  load,
                         float  slew)
{
    timing_model *delaymodAB;
    timing_model *delaymodA;
    timing_model *delaymodB;
    
    delaymodA  = stm_getmodel (cellA, namealloc(modelA)); 
	if (STM_CACHE && delaymodA) 
        mbk_cache_lock (STM_CACHE, delaymodA);
    delaymodB  = stm_getmodel (cellB, namealloc(modelB)); 
	if (STM_CACHE && delaymodB) 
        mbk_cache_lock (STM_CACHE, delaymodB);

    delaymodAB = stm_mod_merge (modelAB, delaymodA, delaymodB, delay, load, slew);
    stm_storemodel (cellAB, modelAB, delaymodAB, 0);
    if (STM_CACHE)  {
        mbk_cache_unlock (STM_CACHE, delaymodA);
        mbk_cache_unlock (STM_CACHE, delaymodB);
    }
    
}

/****************************************************************************/

void stm_addmergedcmodel (char *cellAB,
                          char *modelAB,
                          char *cellA,
                          char *datamodel,
                          char *cellB,
                          char *clockmodel,
                          float constraint,
                          float dataload,
                          float clockload)
    
{
    timing_model *modAB;
    timing_model *modA;
    timing_model *modB;
    
    modA = stm_getmodel (cellA, namealloc(datamodel)); 
	if (STM_CACHE && modA) 
        mbk_cache_lock (STM_CACHE, modA);
    modB = stm_getmodel (cellB, namealloc(clockmodel)); 
	if (STM_CACHE && modB) 
        mbk_cache_lock (STM_CACHE, modB);
    
    modAB = stm_mod_mergec (modelAB, modA, modB, constraint, dataload, clockload);
    stm_storemodel (cellAB, modelAB, modAB, 0);
    if (STM_CACHE)  {
        mbk_cache_unlock (STM_CACHE, modA);
        mbk_cache_unlock (STM_CACHE, modB);
    }
}

/****************************************************************************/

float stm_getconstraint (char *cell, char *model, float inputslew, float clockslew)
{
    timing_model *tmodel = stm_getmodel (cell, model);
    return stm_mod_constraint (tmodel, inputslew, clockslew);
}

/****************************************************************************/

void stm_gettiming( char *cell, char *dmodel, char *fmodel, float fin, stm_pwl *pwlin, stm_driver *driver, float load, float *delay, float *fout, stm_pwl **pwlout, char *signame, char *inputname, char dir )
{
    timing_model *tdmodel = stm_getmodel_sub (cell, dmodel, 1);
    timing_model *tfmodel = stm_getmodel_sub (cell, fmodel, 1);
    fin = stm_mod_shrinkslew_thr2scm(tfmodel?tfmodel:tdmodel, fin);
    stm_mod_timing (tdmodel, tfmodel, fin, pwlin, driver, load, delay, fout, pwlout, signame, inputname, dir, dir );
    if( fout ) 
      *fout = stm_mod_shrinkslew_scm2thr( tfmodel, *fout );
  
}

/****************************************************************************/

void stm_gettiming_pi( char *cell, char *dmodel, char *fmodel, float fin, stm_pwl *pwlin, stm_driver *driver, float c1, float c2, float r, float *delay, float *fout, stm_pwl **pwlout, char *signame, char *inputname, char dir)
{
    timing_model *tdmodel = stm_getmodel_sub (cell, dmodel, 1);
    timing_model *tfmodel = stm_getmodel_sub (cell, fmodel, 1);
    fin = stm_mod_shrinkslew_thr2scm(tfmodel?tfmodel:tdmodel, fin);
    stm_mod_timing_pi (tdmodel, tfmodel, fin, pwlin, driver, c1, c2, r, delay, fout, pwlout, signame, inputname, dir, dir );
    if( fout ) 
      *fout = stm_mod_shrinkslew_scm2thr( tfmodel, *fout );
  
}

/****************************************************************************/

float stm_getslew_pi (char *cell, char *model, float c1, float c2, float r, float slew, stm_pwl *pwl, stm_pwl **ptpwl, char *signame)
{
    timing_model *tmodel = stm_getmodel_sub (cell, model, 1);
    slew = stm_mod_shrinkslew_thr2scm(tmodel, slew);
    return stm_mod_shrinkslew_scm2thr(tmodel, stm_mod_slew_pi (tmodel, c1, c2, r, slew, pwl, ptpwl, signame));
}

/****************************************************************************/

float stm_getdelay_pi (char *cell, char *model, float c1, float c2, float r, float slew, stm_pwl *pwl, char *signame)
{
    timing_model *tmodel = stm_getmodel_sub (cell, model, 1);
    slew = stm_mod_shrinkslew_thr2scm(tmodel, slew);
    return stm_mod_delay_pi (tmodel, c1, c2, r, slew, pwl, signame);
}

/****************************************************************************/

float stm_getslew (char *cell, char *model, float load, float slew, stm_pwl *pwl, stm_pwl **ptpwl, char *signame)
{
    timing_model *tmodel = stm_getmodel_sub (cell, model, 1);
    slew = stm_mod_shrinkslew_thr2scm(tmodel, slew);
    return stm_mod_shrinkslew_scm2thr(tmodel, stm_mod_slew (tmodel, load, slew, pwl, ptpwl, signame));
}

/****************************************************************************/

float stm_getdelay (char *cell, char *model, float load, float slew, stm_pwl *pwl, char *signame)
{
    timing_model *tmodel = stm_getmodel_sub (cell, model, 1);
    slew = stm_mod_shrinkslew_thr2scm(tmodel, slew);
    return stm_mod_delay (tmodel, load, slew, pwl, signame);
}

/****************************************************************************/

float stm_getloadparam (char *cell, char *model, float load, float slew)
{
    timing_model *tmodel = stm_getmodel (cell, namealloc(model));
    return stm_mod_loadparam (tmodel, load, slew);
}

/****************************************************************************/

float stm_getclockslewparam (char *cell, char *model, float clockslew, float slew)
{
    timing_model *tmodel = stm_getmodel (cell, namealloc(model));
    return stm_mod_clockslewparam (tmodel, clockslew, slew);
}
    
/****************************************************************************/
    
float stm_getdataslewparam (char *cell, char *model, float clockslew, float slew)
{
    timing_model *tmodel = stm_getmodel (cell, namealloc(model));
    return stm_mod_dataslewparam (tmodel, clockslew, slew);
}
    
/****************************************************************************/

float stm_getslewparam (char *cell, char *model, float load, float slew)
{
    timing_model *tmodel = stm_getmodel (cell, namealloc(model));
    return stm_mod_slewparam (tmodel, load, slew);
}

/****************************************************************************/

char *stm_generate_name (char *cell, char *input, char itr, char *output, char otr, unsigned int mtype, char minmax)
{
    int n = 0;
    char *modelname = stm_mod_name (input, itr, output, otr, mtype, minmax, n);

    while (stm_getmodel (cell, modelname))
        modelname = stm_mod_name (input, itr, output, otr, mtype, minmax, n++);

    return modelname;
}

/****************************************************************************/

float stm_getVTH (char *cell, char *model)
{
    timing_model *tmodel = stm_getmodel (cell, namealloc(model));
    return stm_mod_vth (tmodel);
}

/****************************************************************************/

float stm_getVDD (char *cell, char *model)
{
    timing_model *tmodel = stm_getmodel (cell, namealloc(model));
    return stm_mod_vdd (tmodel);
}

/****************************************************************************/

float stm_getVT (char *cell, char *model)
{
    timing_model *tmodel = stm_getmodel (cell, namealloc(model));
    return stm_mod_vt (tmodel);
}

/****************************************************************************/

float stm_getVF (char *cell, char *model)
{
    timing_model *tmodel = stm_getmodel (cell, namealloc(model));
    return stm_mod_vf (tmodel);
}


/*****************************************************************************/
/*                        function stm_addaxisvalues()                       */
/*****************************************************************************/
void stm_addaxisvalues(ht *htslope, ht *htcapa, char *namein, char *nameout)
{
    stm_carac     *caracslope = NULL ;
    stm_carac     *caraccapa = NULL ;
    int i;


    if(namein && htslope){
        caracslope = (stm_carac*)gethtitem (htslope, namein) ;
        if ((long)caracslope == EMPTYHT)
             caracslope = (stm_carac*)gethtitem (htslope, namealloc ("default")) ;
        if ((long)caracslope == EMPTYHT){
            STM_AXIS_SLOPEIN = NULL ;
            STM_AXIS_NSLOPE = 0 ;
        }else{
            STM_AXIS_SLOPEIN = (float*)mbkalloc (caracslope->NVALUES * sizeof (float)) ;
            for (i = 0 ; i < caracslope->NVALUES ; i++)
                STM_AXIS_SLOPEIN[i] = caracslope->VALUES[i];
            STM_AXIS_NSLOPE = caracslope->NVALUES;
        }
    }else{
        STM_AXIS_SLOPEIN = NULL ;
        STM_AXIS_NSLOPE = 0 ;
    }
    if(nameout && htcapa){
        caraccapa = (stm_carac*)gethtitem (htcapa, nameout) ;
        if ((long)caraccapa == EMPTYHT)
             caraccapa = (stm_carac*)gethtitem (htcapa, namealloc ("default")) ;
        if ((long)caraccapa == EMPTYHT){
            STM_AXIS_CAPAOUT = NULL ;
            STM_AXIS_NCAPA = 0 ;
        }else{
            STM_AXIS_CAPAOUT = (float*)mbkalloc (caraccapa->NVALUES * sizeof (float)) ;
            for (i = 0 ; i < caraccapa->NVALUES ; i++)
                STM_AXIS_CAPAOUT[i] = caraccapa->VALUES[i];
            STM_AXIS_NCAPA = caraccapa->NVALUES;
        }
    }else{
            STM_AXIS_CAPAOUT = NULL ;
            STM_AXIS_NCAPA = 0 ;
    }

}

/*****************************************************************************/
/*                        function stm_delaxisvalues()                       */
/*****************************************************************************/
void stm_delaxisvalues(void)
{
if(STM_AXIS_SLOPEIN)
    mbkfree(STM_AXIS_SLOPEIN);
STM_AXIS_SLOPEIN = NULL;
if(STM_AXIS_CAPAOUT)
    mbkfree(STM_AXIS_CAPAOUT);
STM_AXIS_CAPAOUT = NULL ;
STM_AXIS_NSLOPE = 0 ;
STM_AXIS_NCAPA = 0 ;
}

/*****************************************************************************/
/*                        function stm_createname()                          */
/*****************************************************************************/
char* stm_createname( char *cellname, char *name )
{
  char buffer[1024] ;
  char *newname ;  
  int  index ;
 
  if( stm_getmodel( cellname, name ) ) {
    index = 1 ;
    do {
      sprintf( buffer, "%s@%d", name, index );
      index++;
    }
    while( stm_getmodel( cellname, buffer ) );
    newname = namealloc( buffer );
  }
  else {
    newname = namealloc( name );
  }

  return newname ;
}
