/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_prop_eval.c                                             */
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

float stm_getfancap (timing_props *properties, float fanout)
{
    timing_model *capmodel;
    float capa;
   
    if(!properties)
        return 0.0;
    
    capmodel = properties->CAPMODEL;

    if(capmodel){
        capa = stm_modtbl_interpol1Dlinear(capmodel->UMODEL.TABLE, fanout);
        if(capa < 0.0)
            return 0.0;
        else
            return capa;
    }else{
        return 0.0;
    }
}

/****************************************************************************/

float stm_getfanres (timing_props *properties, float fanout)
{
    timing_model *resmodel;
    float res;

    if(!properties)
        return 0.0;
    
    resmodel = properties->RESMODEL;

    if(resmodel){
        res = stm_modtbl_interpol1Dlinear(resmodel->UMODEL.TABLE, fanout);
        if(res < 0.0)
            return 0.0;
        else
            return res;
    }else{
        return 0.0;
    }
}

/****************************************************************************/


