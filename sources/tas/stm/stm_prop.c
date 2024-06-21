/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_prop.c                                                   */
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

timing_props *STM_PROPERTIES = NULL;
/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

timing_props *stm_get_prop()
{
    return STM_PROPERTIES;
}

timing_props *stm_prop_create (timing_model *resmodel, timing_model *capmodel)
{
    timing_props *properties;
    
    properties = (timing_props*)mbkalloc (sizeof (struct timing_props));
    
    properties->RESMODEL = resmodel;
    properties->CAPMODEL = capmodel;

    STM_PROPERTIES = properties;

    return properties;
}

/****************************************************************************/

void stm_addresmodel (timing_props *properties, timing_model *resmodel)
{
    properties->RESMODEL = resmodel;
}

/****************************************************************************/

void stm_addcapmodel (timing_props *properties, timing_model *capmodel)
{
    properties->CAPMODEL = capmodel;
}

/****************************************************************************/

void stm_scale_loadmodel (timing_model *model, float scale)
{
    int i;
    timing_table *table;

    if(model){
        table = model->UMODEL.TABLE;
        for(i = 0; i < table->NX; i++)
            table->SET1D[i] = table->SET1D[i] * scale;
    }
}

/****************************************************************************/

void stm_prop_destroy (timing_props *properties)
{

    if (properties) {
        if (properties->RESMODEL)
             mbkfree (properties->RESMODEL);
        if (properties->CAPMODEL)
             mbkfree (properties->CAPMODEL);
        mbkfree (properties);
    }
}

/****************************************************************************/

timing_table *stm_prop_seg2tbl (chain_list *chainseg, char type)
{
    timing_table *table;
    int i = 0;
    chain_list * chain;
    float x0, x1, inter, slo;

    for(chain = chainseg; chain; chain = chain->NEXT){
        if(!i)
            i++;
        i++;
    }

    table = stm_modtbl_create (i, 0, type, STM_NOTYPE);
    i = 0;
    
    for(chain = chainseg; chain; chain = chain->NEXT){
        x0 = ((segment*)(chain->DATA))->X0;
        x1 = ((segment*)(chain->DATA))->X1;
        inter = ((segment*)(chain->DATA))->INTER;
        slo = ((segment*)(chain->DATA))->SLO;
        if(!i){
            table->XRANGE[i] = x0;
            table->SET1D[i] = (x0 * slo) + inter;
        }
        table->XRANGE[i + 1] = x1;
        table->SET1D[i + 1] = (x1 * slo) + inter;
        i++;
    }
    return table;
}
