/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_eval.c                                           */
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

float stm_modtbl_constraint (timing_table *table, float inputslew, float clockslew) 
{
    float constraint = 0;

    if (!table)
        return 0;
    
    if (table->SET2D) {
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_CLOCK_SLEW)
            constraint = stm_modtbl_interpol2Dbilinear (table, inputslew, clockslew);
        if (table->XTYPE == STM_CLOCK_SLEW && table->YTYPE == STM_INPUT_SLEW)
            constraint = stm_modtbl_interpol2Dbilinear (table, clockslew, inputslew);
    }
    else
    if (table->SET1D) {
        if (table->XTYPE == STM_INPUT_SLEW)
            constraint = stm_modtbl_interpol1Dlinear (table, inputslew);
        if (table->XTYPE == STM_CLOCK_SLEW)
            constraint = stm_modtbl_interpol1Dlinear (table, clockslew);
    }
    else
    if (!stm_modtbl_isnull (table->CST))
        constraint = table->CST;
    

    return constraint;
}

/****************************************************************************/

float stm_modtbl_slew (timing_table *table, float load, float slew) {
    return stm_modtbl_delay (table, load, slew);
}

/****************************************************************************/
    
float stm_modtbl_delay (timing_table *table, float load, float slew)
{
    float delay;

    if (!table) {
        return 0;
    }
    
    if (table->SET2D) {
        if (table->XTYPE == STM_LOAD && table->YTYPE == STM_INPUT_SLEW)
            delay = stm_modtbl_interpol2Dbilinear (table, load, slew);
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_LOAD)
            delay = stm_modtbl_interpol2Dbilinear (table, slew, load);
    }
    else
    if (table->SET1D) {
        if (table->XTYPE == STM_LOAD)
            delay = stm_modtbl_interpol1Dlinear (table, load);
        if (table->XTYPE == STM_INPUT_SLEW)
            delay = stm_modtbl_interpol1Dlinear (table, slew);
    }
    if (!stm_modtbl_isnull (table->CST))
        delay = table->CST;

    return delay;
}

/****************************************************************************/

float stm_modtbl_interpol1Dlinear (timing_table *table, float x)
{
    int   i;
    int   nx1, nx2;
    float x1, x2;
    float y1, y2;
    float d;

    /* interpolation */
    
    if (x > table->XRANGE[0] && x < table->XRANGE[table->NX - 1]) 
        for (i = 0; i <= table->NX - 1; i++)
            if (x < table->XRANGE[i]) {
                nx1 = i - 1;
                nx2 = i;
                break;
            }
    
    /* extrapolation */
    
    if (x >= table->XRANGE[table->NX - 1]) {
        nx1 = table->NX - 2;
        nx2 = table->NX - 1;
    }

    if (x <= table->XRANGE[0]) {
        nx1 = 0;
        nx2 = 1;
    }
    
    /* delay calculation */

    x1 = table->XRANGE[nx1];
    x2 = table->XRANGE[nx2];

    y1 = table->SET1D[nx1];
    y2 = table->SET1D[nx2];

    if (x1 == x2)
      d = y1;
    else
      d = stm_modtbl_interpol1DCalc(x1,x2,y1,y2,x);
      //d = (((y1 - y2) / (x1 - x2)) * (x - x1)) + y1;
    
    return d;
}

/****************************************************************************/

float stm_modtbl_interpol1DCalc(float x1, float x2, float y1, float y2, float x)
{
  return (((y1 - y2) / (x1 - x2)) * (x - x1)) + y1;
}

/****************************************************************************/

float stm_modtbl_interpol2Dbilinear (timing_table *table, float x, float y)
{
    int   nx1, nx2;
    int   ny1, ny2;
    int   i;
    float x1, x2;
    float y1, y2;
    float d0, d1, d2, d3;
    float a, b, d;

    /* interpolation */
    
    if (x > table->XRANGE[0] && x < table->XRANGE[table->NX - 1]) 
        for (i = 0; i <= table->NX - 1; i++)
            if (x < table->XRANGE[i]) {
                nx1 = i - 1;
                nx2 = i;
                break;
            }
            
    if (y > table->YRANGE[0] && y < table->YRANGE[table->NY - 1]) 
        for (i = 0; i <= table->NY - 1; i++)
            if (y < table->YRANGE[i]) {
                ny1 = i - 1;
                ny2 = i;
                break;
            }

   /* extrapolation */
    
    if (x >= table->XRANGE[table->NX - 1]) {
        nx1 = table->NX - 2;
        nx2 = table->NX - 1;
    }

    if (x <= table->XRANGE[0]) {
        nx1 = 0;
        nx2 = 1;
    }

    if (y >= table->YRANGE[table->NY - 1]) {
        ny1 = table->NY - 2;
        ny2 = table->NY - 1;
    }

    if (y <= table->YRANGE[0]) {
        ny1 = 0;
        ny2 = 1;
    }

    /* delay calculation */

    d0 = table->SET2D[nx1][ny1];
    d1 = table->SET2D[nx2][ny1];
    d2 = table->SET2D[nx1][ny2];
    d3 = table->SET2D[nx2][ny2];

    x1 = table->XRANGE[nx1];
    x2 = table->XRANGE[nx2];
    y1 = table->YRANGE[ny1];
    y2 = table->YRANGE[ny2];


    if (y1 == y2) {
      a = d0;
      b = d1;
    } else {
      a = ((d0 - d2) / (y1 - y2)) * (y - y1) + d0;
      b = ((d1 - d3) / (y1 - y2)) * (y - y1) + d1;
    }

    if (x1 == x2)
      d = a;
    else
      d = ((b - a) / (x2 - x1)) * (x - x1) + a;

    return d;
}

/****************************************************************************/

float stm_modtbl_lowerslew (timing_table *table, float slew)
{
    float *srange;
    int    i, ns;

    if (table->XTYPE == STM_INPUT_SLEW) {
        srange = table->XRANGE;
        ns = table->NX;
    }
    if (table->YTYPE == STM_INPUT_SLEW) {
        srange = table->YRANGE;
        ns = table->NY;
    }

    if (slew > srange[0] && slew < srange[ns - 1]) 
        for (i = 0; i <= ns - 1; i++)
            if (slew < srange[i]) 
                return srange[i - 1];
            
    if (slew >= srange[ns - 1]) 
        return (srange[ns - 2]);
    else /* slew <= srange[0] */ 
        return srange[0];
}

/****************************************************************************/

float stm_modtbl_upperslew (timing_table *table, float slew)
{
    float *srange;
    int    i, ns;

    if (table->XTYPE == STM_INPUT_SLEW) {
        srange = table->XRANGE;
        ns = table->NX;
    }
    if (table->YTYPE == STM_INPUT_SLEW) {
        srange = table->YRANGE;
        ns = table->NY;
    }

    if (slew > srange[0] && slew < srange[ns - 1]) 
        for (i = 0; i <= ns - 1; i++)
            if (slew < srange[i]) 
                return srange[i];
            
    if (slew >= srange[ns - 1]) 
        return (srange[ns - 1]);
    else  /* slew <= srange[0] */ 
        return srange[1];
}

/****************************************************************************/

float stm_modtbl_lowerload (timing_table *table, float load)
{
    float *lrange;
    int    i, nl;

    if (table->XTYPE == STM_LOAD) {
        lrange = table->XRANGE;
        nl = table->NX;
    }
    if (table->YTYPE == STM_LOAD) {
        lrange = table->YRANGE;
        nl = table->NY;
    }

    if (load > lrange[0] && load < lrange[nl - 1]) 
        for (i = 0; i <= nl - 1; i++)
            if (load < lrange[i]) 
                return lrange[i - 1];
            
    if (load >= lrange[nl - 1]) 
        return lrange[nl - 2];
    else /* load <= lrange[0] */ 
        return lrange[0];
}

/****************************************************************************/

float stm_modtbl_upperload (timing_table *table, float load)
{
    float *lrange;
    int    i, nl;

    if (table->XTYPE == STM_LOAD) {
        lrange = table->XRANGE;
        nl = table->NX;
    }
    if (table->YTYPE == STM_LOAD) {
        lrange = table->YRANGE;
        nl = table->NY;
    }

    if (load > lrange[0] && load < lrange[nl - 1]) 
        for (i = 0; i <= nl - 1; i++)
            if (load < lrange[i]) 
                return lrange[i];
            
    if (load >= lrange[nl - 1]) 
        return lrange[nl - 1]; 
    else /* load <= lrange[0] */ 
        return lrange[1];
}

/****************************************************************************/

float stm_modtbl_loadparam (timing_table *table, float load, float slew)
{
    int           nl, ns;
    float         lowload, upload, lowd, upd;

    ns = stm_modtbl_getnslew (table);
    nl = stm_modtbl_getnload (table);
    
    if (nl && ns) {
        lowload = stm_modtbl_lowerload (table, load);
        upload  = stm_modtbl_upperload (table, load);
        if (table->XTYPE == STM_LOAD && table->YTYPE == STM_INPUT_SLEW) {
            upd  = stm_modtbl_interpol2Dbilinear (table, upload, slew);
            lowd = stm_modtbl_interpol2Dbilinear (table, lowload, slew);
        }
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_LOAD) {
            upd  = stm_modtbl_interpol2Dbilinear (table, slew, upload);
            lowd = stm_modtbl_interpol2Dbilinear (table, slew, lowload);
        }
        return (((upd - lowd) / (upload - lowload)) * 1e3);
    }
    if (nl && !ns) {
        lowload = stm_modtbl_lowerload (table, load);
        upload  = stm_modtbl_upperload (table, load);
        lowd = stm_modtbl_interpol1Dlinear (table, lowload);
        upd  = stm_modtbl_interpol1Dlinear (table, upload);
        return (((upd - lowd) / (upload - lowload)) * 1e3);
    }
        
    return 0; /* !nl */
}

/****************************************************************************/

float stm_modtbl_clockslewparam (timing_table *table, float clockslew, float slew)
{
    int           nc, ns;
    float         lowckslew, upckslew, lowd = 0, upd = 0;

    ns = stm_modtbl_getnslew (table);
    nc = stm_modtbl_getnckslew (table);
    
    if (nc && ns) {
        lowckslew = stm_modtbl_lowerload (table, clockslew);
        upckslew  = stm_modtbl_upperload (table, clockslew);
        if (table->XTYPE == STM_CLOCK_SLEW && table->YTYPE == STM_INPUT_SLEW) {
            upd  = stm_modtbl_interpol2Dbilinear (table, upckslew, slew);
            lowd = stm_modtbl_interpol2Dbilinear (table, lowckslew, slew);
        }
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_CLOCK_SLEW) {
            upd  = stm_modtbl_interpol2Dbilinear (table, slew, upckslew);
            lowd = stm_modtbl_interpol2Dbilinear (table, slew, lowckslew);
        }
        return (((upd - lowd) / (upckslew - lowckslew)) * 1e3);/* pour que ca rentre 
                                                                  dans un long    */
    }
    if (nc && !ns) {
        lowckslew = stm_modtbl_lowerload (table, clockslew);
        upckslew  = stm_modtbl_upperload (table, clockslew);
        lowd = stm_modtbl_interpol1Dlinear (table, lowckslew);
        upd  = stm_modtbl_interpol1Dlinear (table, upckslew);
        return (((upd - lowd) / (upckslew - lowckslew)) * 1e3);/* pour que ca rentre 
                                                                  dans un long    */
    }
        
    return 0; /* !nl */
}
    
/****************************************************************************/
    
float stm_modtbl_dataslewparam (timing_table *table, float clockslew, float slew)
{
    int   ns, nc;
    float lowslew, upslew, lowd, upd;

    ns = stm_modtbl_getnslew (table);
    nc = stm_modtbl_getnckslew (table);
    
    if (nc && ns) {
        lowslew = stm_modtbl_lowerslew (table, slew);
        upslew  = stm_modtbl_upperslew (table, slew);
        if (table->XTYPE == STM_CLOCK_SLEW && table->YTYPE == STM_INPUT_SLEW) {
            upd  = stm_modtbl_interpol2Dbilinear (table, clockslew, upslew);
            lowd = stm_modtbl_interpol2Dbilinear (table, clockslew, lowslew);
        }
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_CLOCK_SLEW) {
            upd = stm_modtbl_interpol2Dbilinear (table, upslew, clockslew);
            lowd = stm_modtbl_interpol2Dbilinear (table, lowslew, clockslew);
        }
        return (((upd - lowd) / (upslew - lowslew)) * 1e3);/* pour que ca rentre 
                                                              dans un long    */
    }
    if (!nc && ns) {
        lowslew = stm_modtbl_lowerslew (table, slew);
        upslew  = stm_modtbl_upperslew (table, slew);
        lowd = stm_modtbl_interpol1Dlinear (table, lowslew);
        upd  = stm_modtbl_interpol1Dlinear (table, upslew);
        return (((upd - lowd) / (upslew - lowslew)) * 1e3);/* pour que ca rentre 
                                                              dans un long    */
    }
        
    return 0; /* !nc */
}
    
/****************************************************************************/
    
float stm_modtbl_slewparam (timing_table *table, float load, float slew)
{
    int           nl, ns;
    float         lowslew, upslew, lowd, upd;

    ns = stm_modtbl_getnslew (table);
    nl = stm_modtbl_getnload (table);
    
    if (nl && ns) {
        lowslew = stm_modtbl_lowerslew (table, slew);
        upslew  = stm_modtbl_upperslew (table, slew);
        if (table->XTYPE == STM_LOAD && table->YTYPE == STM_INPUT_SLEW) {
            upd  = stm_modtbl_interpol2Dbilinear (table, load, upslew);
            lowd = stm_modtbl_interpol2Dbilinear (table, load, lowslew);
        }
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_LOAD) {
            upd = stm_modtbl_interpol2Dbilinear (table, upslew, load);
            lowd = stm_modtbl_interpol2Dbilinear (table, lowslew, load);
        }
        return (((upd - lowd) / (upslew - lowslew)) * 1e3); /* pour que ca rentre 
                                                                dans un long    */
    }
    if (!nl && ns) {
        lowslew = stm_modtbl_lowerslew (table, slew);
        upslew  = stm_modtbl_upperslew (table, slew);
        lowd = stm_modtbl_interpol1Dlinear (table, lowslew);
        upd  = stm_modtbl_interpol1Dlinear (table, upslew);
        return (((upd - lowd) / (upslew - lowslew)) * 1e3); /* pour que ca rentre 
                                                               dans un long    */
    }
        
    return 0; /* !ns */
}

/****************************************************************************/

float stm_modtbl_maxwfixload (timing_table *table, float load)
{
    int           i, nslew;
    float         inslew, value, valuemax;

    nslew = stm_modtbl_getnslew (table);

    if (nslew) {
        valuemax = -FLT_MAX;
        for (i = 0; i < nslew; i++) {
            inslew = stm_modtbl_getslewaxisval (table, i);
            value = stm_modtbl_delay (table, load, inslew);
            valuemax = valuemax > value ? valuemax : value;
        }
    } else 
        valuemax = stm_modtbl_delay (table, load, STM_DONTCARE);
    
    return valuemax;
}

/****************************************************************************/

float stm_modtbl_maxwfixslew (timing_table *table, float slew)
{
    int           i, nload;
    float         outload, value, valuemax;

    nload = stm_modtbl_getnload (table);

    if (nload) {
        valuemax = -FLT_MAX;
        for (i = 0; i < nload; i++) {
            outload = stm_modtbl_getloadaxisval (table, i);
            value = stm_modtbl_delay (table, outload, slew);
            valuemax = valuemax > value ? valuemax : value;
        }
    } else 
        valuemax = stm_modtbl_delay (table, STM_DONTCARE, slew);
    
    return valuemax;
}

/****************************************************************************/

float stm_modtbl_minwfixload (timing_table *table, float load)
{
    int           i, nslew;
    float         inslew, value, valuemin;

    nslew = stm_modtbl_getnslew (table);

    if (nslew) {
        valuemin = FLT_MAX;
        for (i = 0; i < nslew; i++) {
            inslew = stm_modtbl_getslewaxisval (table, i);
            value = stm_modtbl_delay (table, load, inslew);
            valuemin = valuemin < value ? valuemin : value;
        }
    } else 
        valuemin = stm_modtbl_delay (table, load, STM_DONTCARE);
    
    return valuemin;
}

/****************************************************************************/

float stm_modtbl_minwfixslew (timing_table *table, float slew)
{
    int           i, nload;
    float         outload, value, valuemin;

    nload = stm_modtbl_getnload (table);

    if (nload) {
        valuemin = FLT_MAX;
        for (i = 0; i < nload; i++) {
            outload = stm_modtbl_getloadaxisval (table, i);
            value = stm_modtbl_delay (table, outload, slew);
            valuemin = valuemin < value ? valuemin : value;
        }
    } else 
        valuemin = stm_modtbl_delay (table, STM_DONTCARE, slew);
    
    return valuemin;
}

/****************************************************************************/

float stm_modtbl_value_minslew_maxcapa(timing_table *table)
{
    float value;

    if (!table) {
        return 0;
    }
    
    if(table->SET2D) {
        if(table->XTYPE == STM_LOAD && table->YTYPE == STM_INPUT_SLEW){
            value = stm_modtbl_delay (table, table->XRANGE[table->NX-1], table->YRANGE[0]);
        }else if(table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_LOAD){
            value = stm_modtbl_delay (table, table->YRANGE[table->NY-1], table->XRANGE[0]);
        }
    }else if(table->SET1D) {
        if(table->XTYPE == STM_LOAD){
            value = stm_modtbl_delay (table, table->XRANGE[table->NX-1], -1);
        }else if(table->XTYPE == STM_INPUT_SLEW){
            value = stm_modtbl_delay (table, -1, table->XRANGE[0]);
        }
    }else if(!stm_modtbl_isnull (table->CST)){
        value = table->CST;
    }

    return value;
}
