/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_cell.c                                                  */
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
/*     structures                                                           */
/****************************************************************************/

typedef struct stm_mdnum {
    char *NAME;
    long  COUNT;
#ifdef NEWSIGN
    timing_model *MODEL;
    void *SIGNATURE;
#else
#ifdef EXPNEW
    char *SIGNATURE;
#endif
#endif
} stm_mdnum;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
#ifdef EXPNEW
static stm_mdnum *stm_get_share_info(timing_model *model)
{
  ptype_list *pt;
  if ((pt=getptype(model->USER, STM_SHARE_INFO))!=NULL)
    return (stm_mdnum *)pt->DATA;
  return NULL;
}

static void stm_set_share_info(timing_model *model, stm_mdnum *sm)
{
  ptype_list *pt;
  model->USER=addptype(model->USER, STM_SHARE_INFO, sm);
}
#endif

timing_cell *stm_cell_create (char *cellname) 
{
    timing_cell *cell;
    
    cell = (timing_cell*)mbkalloc (sizeof (struct timing_cell));

    cell->NAME           = cellname;
    cell->MODEL_LIST     = NULL;
    cell->FILE_D         = 0;
    cell->MODEL_HT       = addht (STM_NBMODELS);
/*    cell->SIM_MODEL_LIST = NULL;
    cell->SIM_MODEL_HT   = addht (11);
*/ 
    if (V_BOOL_TAB[__STM_SHARE_MODEL].VALUE)
     {
#ifdef NEWSIGN
        cell->MODEL_HTSIGN = addht (STM_NBMODELS);
#else
#ifdef EXPNEW
        CreateNameAllocator( 4096, &cell->na, CASE_SENSITIVE);
        cell->MODEL_HTSIGN = addht (STM_NBMODELS);
#else
        cell->MODEL_HTSIGN = stm_addht (STM_NBMODELS);
#endif
#endif
     }
    
    return cell;
}

/****************************************************************************/

void stm_cell_destroy (timing_cell *cell)
{
    chain_list   *pt;
    char          signature[1024];
    timing_model *model;
    long          m;
    stm_mdnum *sm;
    
    for (pt = cell->MODEL_LIST; pt; pt = pt->NEXT) {
        model = (timing_model*)pt->DATA;
        if (V_BOOL_TAB[__STM_SHARE_MODEL].VALUE) {
#ifdef EXPNEW
            if ((sm=stm_get_share_info(model))!=NULL)
             {
               sm->COUNT--;
               if (sm->COUNT==0) mbkfree(sm);
             }
#else
            stm_mod_signature (model, signature);
            if (signature[0] != 'X') { /* not implemented */
                m = stm_gethtitem (cell->MODEL_HTSIGN, signature);
                if (m != EMPTYHT && m != DELETEHT)
                    mbkfree((stm_mdnum*)m);
                stm_delhtitem (cell->MODEL_HTSIGN, signature);
            }
#endif
        }
        stm_mod_destroy (model);
    }
/*    freechain (cell->SIM_MODEL_LIST);
    delht (cell->SIM_MODEL_HT);*/
    freechain (cell->MODEL_LIST);
    delht (cell->MODEL_HT);
    if (V_BOOL_TAB[__STM_SHARE_MODEL].VALUE)
    {
#ifdef NEWSIGN
      chain_list *cl;
      cl=GetAllHTElems(cell->MODEL_HTSIGN);
      while (cl!=NULL)
      {
        freechain((chain_list *)cl->DATA);
        cl=delchain(cl,cl);
      }
      delht (cell->MODEL_HTSIGN);
#else
#ifdef EXPNEW
      DeleteNameAllocator(&cell->na);
      delht (cell->MODEL_HTSIGN);
#else
      stm_delht (cell->MODEL_HTSIGN);
#endif
#endif
    }
    mbkfree(cell);
}

/****************************************************************************/

int stm_cell_share_decrement (timing_cell *cell, timing_model *model, int n)
{
    char signature[1024];
    int  shared = 1;
    long m;
    stm_mdnum *sm;

    if (V_BOOL_TAB[__STM_SHARE_MODEL].VALUE) {
#ifdef NEWSIGN
        if ((sm=stm_get_share_info(model))!=NULL)
          sm->COUNT-=n, shared=sm->COUNT;
        
        if (shared < 0)
        {
          avt_errmsg (STM_ERRMSG,"004", AVT_ERROR);
          shared = sm->COUNT = 0;
        }

        if (!shared)
        {
           chain_list *cl, *prev;
           m=gethtitem(cell->MODEL_HTSIGN, sm->SIGNATURE);
           if (m!=EMPTYHT)
           {
             for (prev=NULL, cl=(chain_list *)m; cl!=NULL && cl->DATA!=sm; prev=cl, cl=cl->NEXT) ;
             if (cl!=NULL)
             {
               mbkfree(sm);
               if (prev!=NULL)
                 prev->NEXT=cl->NEXT;
               else
                 sethtitem(cell->MODEL_HTSIGN, sm->SIGNATURE, (long)cl->NEXT);
               delchain(cl,cl);
             }
           }
        }
#else
#ifdef EXPNEW
        if ((sm=stm_get_share_info(model))!=NULL)
          sm->COUNT-=n, shared=sm->COUNT;
        
        if (shared < 0)
        {
          avt_errmsg (STM_ERRMSG,"004", AVT_ERROR);
          shared = sm->COUNT = 0;
        }

        if (!shared)
        {
           delhtitem(cell->MODEL_HTSIGN, sm->SIGNATURE);
           mbkfree(sm);
        }
#else
        stm_mod_signature (model, signature);
        if (signature[0] != 'X') { /* not implemented */
            m = stm_gethtitem (cell->MODEL_HTSIGN, signature); 
            if (m != EMPTYHT && m != DELETEHT) {
                shared = ((stm_mdnum*)m)->COUNT -= n;
                if (shared < 0) {
                    avt_errmsg (STM_ERRMSG,"004", AVT_ERROR);
                    shared = ((stm_mdnum*)m)->COUNT = 0;
                }
            }
            if (!shared)
                stm_delhtitem (cell->MODEL_HTSIGN, signature);
        }
#endif
#endif
    }

    return shared;
}

/****************************************************************************/

int stm_cell_share_increment (timing_cell *cell, timing_model *model, int n)
{
    char signature[1024];
    int  shared = 1;
    long m;
    stm_mdnum *sm;

    if (V_BOOL_TAB[__STM_SHARE_MODEL].VALUE) {
#ifdef EXPNEW
        if ((sm=stm_get_share_info(model))!=NULL)
          sm->COUNT+=n, shared=sm->COUNT;
        else
          stm_cell_addmodel (cell, model, 0);
#else
        stm_mod_signature (model, signature);
        if (signature[0] != 'X') { /* not implemented */
            m = stm_gethtitem (cell->MODEL_HTSIGN, signature); 
            if (m != EMPTYHT && m != DELETEHT)
                shared = ((stm_mdnum*)m)->COUNT += n;
            else 
                stm_cell_addmodel (cell, model, 0);
        }
#endif
    }

    return shared;
}


/****************************************************************************/

int stm_cell_share_getn (timing_cell *cell, timing_model *model)
{
    char signature[1024];
    long m;
    stm_mdnum *sm;

    if (V_BOOL_TAB[__STM_SHARE_MODEL].VALUE) {
#ifdef EXPNEW
        if ((sm=stm_get_share_info(model))!=NULL)
          return sm->COUNT;
#else
        stm_mod_signature (model, signature);
        if (signature[0] != 'X') { /* not implemented */
            m = stm_gethtitem (cell->MODEL_HTSIGN, signature); 
            if (m != EMPTYHT && m != DELETEHT)
                return ((stm_mdnum*)m)->COUNT;
        }
#endif
    }

    return 0;
}

/****************************************************************************/

void stm_cell_delmodel (timing_cell *cell, timing_model *model, int force)
{
    chain_list *ch;
    int         keep = 0;

    if (!cell)
        avt_errmsg (STM_ERRMSG,"005", AVT_ERROR);
    if (!model)
        avt_errmsg (STM_ERRMSG,"006", AVT_ERROR);

    if (force)
        keep = 0;
    else
        keep = stm_cell_share_decrement (cell, model, 1);

    if (!keep) {
        for (ch = cell->MODEL_LIST; ch; ch = ch->NEXT) 
            if (ch->DATA == model) {
                cell->MODEL_LIST = delchain (cell->MODEL_LIST, ch);
                break;
            }
        delhtitem (cell->MODEL_HT, model->NAME);
        stm_mod_destroy (model); 
    }
}

/****************************************************************************/
#if 0
void stm_cell_addsc (timing_cell *cell, sim_model *sc)
{
    long m;
    chain_list *ch;
    sim_model *model;

    if (!cell)
        avt_errmsg (STM_ERRMSG,"005", AVT_ERROR);

    if (!sc) return;

    m = gethtitem (cell->SIM_MODEL_HT, (void*)sc->NUMBER);
    if (m == EMPTYHT || m == DELETEHT) 
        cell->SIM_MODEL_LIST = addchain (cell->SIM_MODEL_LIST, sc);
    else {
        for (ch = cell->SIM_MODEL_LIST; ch; ch = ch->NEXT) {
            model = (sim_model*)ch->DATA;
            if ((model->NUMBER == sc->NUMBER) && (model != sc)) {
                sim_model_clear_bypointer(model);
                sim_model_free (model);
                ch->DATA = sc;
                break;
            }
        }
    }
    addhtitem (cell->SIM_MODEL_HT, (void*)sc->NUMBER, (long)sc);
}
#endif
/****************************************************************************/

char *stm_cell_addmodel (timing_cell *cell, timing_model *model, int noshare)
{
    long        m;
    chain_list *cl;
    chain_list *ch;
    char        *nsig, signature[1024];
    stm_mdnum  *mdnum;
    timing_model *prevmodel;
#ifdef NEWSIGN
    union
    {
      float equivf;
      int equivi;
    } quicksign;
#endif

    if (!cell)
        avt_errmsg (STM_ERRMSG,"005", AVT_ERROR);
    if (!model)
        avt_errmsg (STM_ERRMSG,"006", AVT_ERROR);

    if (V_BOOL_TAB[__STM_SHARE_MODEL].VALUE && !noshare) {
#ifdef NEWSIGN
        stm_mod_change_params(model);
        quicksign.equivf=stm_mod_signature (model, signature);
        if (signature[0] != 'X') { /* X = not implemented */
            nsig=(char *)(long)quicksign.equivi;
            m = gethtitem (cell->MODEL_HTSIGN, nsig);
            if (m==EMPTYHT) ch=cl=NULL; else ch=cl=(chain_list *)m;
            while (cl!=NULL && stm_mod_same_params((mdnum=(stm_mdnum *)cl->DATA)->MODEL, model)==0)
               cl=cl->NEXT;

            if (cl == NULL) { /* model doesn't exist */
                mdnum = (stm_mdnum*)mbkalloc (sizeof (struct stm_mdnum));
                mdnum->COUNT = 1;
                mdnum->NAME = model->NAME;
                mdnum->MODEL = model;
                mdnum->SIGNATURE = nsig;
                addhtitem (cell->MODEL_HTSIGN, nsig, (long)addchain(ch, mdnum));
                stm_set_share_info(model, mdnum);
                cell->MODEL_LIST=addchain(cell->MODEL_LIST, model);
                model->POS_IN_LIST=cell->MODEL_LIST;
                addhtitem (cell->MODEL_HT, model->NAME, (long)model);
            }
            else {
                mdnum->COUNT++;
            }
            return mdnum->NAME;
        }
#else
        stm_mod_signature (model, signature);
        if (signature[0] != 'X') { /* X = not implemented */
#ifdef EXPNEW
            nsig=NameAlloc(&cell->na, signature);
            m = gethtitem (cell->MODEL_HTSIGN, nsig); 
            if (m == EMPTYHT || m == DELETEHT) { /* model doesn't exist */
                mdnum = (stm_mdnum*)mbkalloc (sizeof (struct stm_mdnum));
                mdnum->COUNT = 1;
                mdnum->NAME = model->NAME;
                mdnum->SIGNATURE = nsig;
                addhtitem (cell->MODEL_HTSIGN, nsig, (long)mdnum);
                stm_mod_change_params(model);
                stm_set_share_info(model, mdnum);
            }
            else {
                ((stm_mdnum*)m)->COUNT++;
                return ((stm_mdnum*)m)->NAME;
            }
#else
            m = stm_gethtitem (cell->MODEL_HTSIGN, signature); 
            if (m == EMPTYHT || m == DELETEHT) { /* model doesn't exist */
                mdnum = (stm_mdnum*)mbkalloc (sizeof (struct stm_mdnum));
                mdnum->COUNT = 1;
                mdnum->NAME = model->NAME;
                stm_addhtitem (cell->MODEL_HTSIGN, signature, (long)mdnum);
                stm_mod_change_params(model);
            }
            else {
                ((stm_mdnum*)m)->COUNT++;
                return ((stm_mdnum*)m)->NAME;
            }
#endif
        }
#endif
    }

    m = gethtitem (cell->MODEL_HT, model->NAME);
    if (m == EMPTYHT || m == DELETEHT) /* add */ {
        cell->MODEL_LIST = addchain (cell->MODEL_LIST, model);
        model->POS_IN_LIST=cell->MODEL_LIST;
    }
    else { /* replace */
        prevmodel=(timing_model*)m;
        if (prevmodel->POS_IN_LIST==NULL)
        {
          for (ch = cell->MODEL_LIST; ch; ch = ch->NEXT) {
              prevmodel=(timing_model*)ch->DATA;
              prevmodel->POS_IN_LIST=ch;
              if ((prevmodel->NAME == model->NAME) && (prevmodel != model)) {
                  stm_mod_destroy ((timing_model*)ch->DATA);
                  ch->DATA = model;
                  model->POS_IN_LIST=ch;
                  break;
              }
           }
        }
        else
        {
          if(prevmodel!=model)
          {
            prevmodel->POS_IN_LIST->DATA=model;
            model->POS_IN_LIST=prevmodel->POS_IN_LIST;
            stm_mod_destroy (prevmodel);
          }
        }
    }

    addhtitem (cell->MODEL_HT, model->NAME, (long)model);
/*    
    if (model->UTYPE == STM_MOD_MODFCT)
        stm_cell_addsc (cell, model->UMODEL.FUNCTION->MODEL);
*/
    return model->NAME;
}


/****************************************************************************/

void stm_cleanfigmodel(char *cellname, ht *htable)
{
    timing_cell  *cell;
    timing_model *model;
    chain_list   *current, *next;
    char         *mname;

    cell = stm_getcell(cellname);
    if(!cell) return;

    current = cell->MODEL_LIST;
    if( cell->MODEL_LIST )
        next = cell->MODEL_LIST->NEXT;
    else
        next = NULL ;
    
    while (next) {
        mname = ((timing_model*)next->DATA)->NAME;
        if((long)gethtitem(htable, mname) == 1){
            current = current->NEXT;
            next    = next->NEXT;
        } else {
            model = (timing_model*)gethtitem (cell->MODEL_HT, mname);
            if (((long)model != EMPTYHT) && ((long)model != DELETEHT)) { 
                delhtitem (cell->MODEL_HT, mname);
                stm_mod_destroy(model);
            }
            current->NEXT = next->NEXT;
            next->NEXT    = NULL;
            freechain (next);
            next = current->NEXT;
        }
    }
    current = cell->MODEL_LIST;
    if( current ) {
        mname = ((timing_model*)current->DATA)->NAME;
        if((long)gethtitem(htable, mname) != 1){
            model = (timing_model*)gethtitem (cell->MODEL_HT, mname);
            if (((long)model != EMPTYHT) && ((long)model != DELETEHT)) { 
                delhtitem (cell->MODEL_HT, mname);
                stm_mod_destroy(model);
            }
            cell->MODEL_LIST = current->NEXT;
            current->NEXT = NULL;
            freechain(current);
        }
    }
}
