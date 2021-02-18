/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl.c                                                */
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

int stm_modtbl_getnslew (timing_table *table)
{
    if (table) {
        if (table->XTYPE == STM_INPUT_SLEW)
            return table->NX;
        if (table->YTYPE == STM_INPUT_SLEW)
            return table->NY;
    }
    return 0;
}

/****************************************************************************/

int stm_modtbl_getnckslew (timing_table *table)
{
    if (table) {
        if (table->XTYPE == STM_CLOCK_SLEW)
            return (table->NX);
        if (table->YTYPE == STM_CLOCK_SLEW)
            return (table->NY);
    }
    return 0;
}

/****************************************************************************/

int stm_modtbl_getnload (timing_table *table)
{
    if (table) {
        if (table->XTYPE == STM_LOAD)
            return (table->NX);
        if (table->YTYPE == STM_LOAD)
            return (table->NY);
    }
    return 0;
}

/****************************************************************************/

timing_table *stm_modtbl_create_ftemplate (timing_ttable *ttemplate)
{
    timing_table *table;
    int           nx, ny, i;
    char          xtype, ytype;

    nx = ttemplate->NX;
    ny = ttemplate->NY;
    xtype = ttemplate->XTYPE;
    ytype = ttemplate->YTYPE;
    
    table = stm_modtbl_create (nx, ny, xtype, ytype);
    table->XTYPEBIS = ttemplate->XTYPEBIS;
 
    for (i = 0; i < nx; i++)
        table->XRANGE[i] = ttemplate->XRANGE[i];
    for (i = 0; i < ny; i++)
        table->YRANGE[i] = ttemplate->YRANGE[i];
    
    return table;
}

/****************************************************************************/

timing_table *stm_modtbl_duplicate (timing_table *table)
{
    timing_table *duptable;
    int           i, j;

    if (!table)
        return NULL;

    duptable = stm_modtbl_create (table->NX, table->NY, table->XTYPE, table->YTYPE);
    duptable->XTYPEBIS =  table->XTYPEBIS;
    
    for (i = 0; i < duptable->NX; i++)
        duptable->XRANGE[i] = table->XRANGE[i];
    for (i = 0; i < duptable->NY; i++)
        duptable->YRANGE[i] = table->YRANGE[i];
    if (table->SET2D)
        for (i = 0; i < duptable->NX; i++)
            for (j = 0; j < duptable->NY; j++)
                duptable->SET2D[i][j] = table->SET2D[i][j];
    if (table->SET1D)
        for (i = 0; i < duptable->NX; i++)
            duptable->SET1D[i] = table->SET1D[i];
    if (!stm_modtbl_isnull (table->CST))
        duptable->CST = table->CST;

    return duptable;
}

/****************************************************************************/

timing_table *stm_modtbl_create (int nx, int ny, char xtype, char ytype)
{
    timing_table *table;
    int           i;
    
    table = (timing_table*)mbkalloc (sizeof (struct timing_table));
    
    table->TEMPLATE = NULL;
    table->XRANGE   = NULL;
    table->YRANGE   = NULL;
    table->SET1D    = NULL;
    table->SET2D    = NULL;
    table->CST    = stm_modtbl_initval ();
    table->XTYPEBIS = 0;
    
    if (nx > 0)
        table->NX = nx;
    else
        table->NX = 0;
    
    if (ny > 0)
        table->NY = ny;
    else
        table->NY = 0;
    
    table->XTYPE = xtype;
    table->YTYPE = ytype;
        
    if (table->NX > 0 && table->NY > 0)
        table->SET2D = stm_modtbl_createset2D (nx, ny);
    if (table->NX > 0 && table->NY == 0)
        table->SET1D = stm_modtbl_createset1D (nx);

    if (table->NX) {
        table->XRANGE = (float*)mbkalloc (table->NX * sizeof (float));
        for (i = 0; i < table->NX; i++)
            table->XRANGE[i] = stm_modtbl_initval ();
    }

    if (table->NY) {
        table->YRANGE = (float*)mbkalloc (table->NY * sizeof (float));
        for (i = 0; i < table->NY; i++)
            table->YRANGE[i] = stm_modtbl_initval ();
    }

    return table;
}

/****************************************************************************/

void stm_modtbl_destroy (timing_table *table)
{
    if(!table) return;
    if (table->NX > 0 && table->NY > 0)
        stm_modtbl_destroyset2D (table->SET2D, table->NX);
    if (table->NX > 0 && table->NY == 0)
        stm_modtbl_destroyset1D (table->SET1D);
    if (table->XRANGE)
        mbkfree (table->XRANGE);
    if (table->YRANGE)
        mbkfree (table->YRANGE);
    mbkfree (table);
}

/****************************************************************************/
/*                         2D set                                           */
/****************************************************************************/

float **stm_modtbl_createset2D (int nx, int ny)
{
    float **set;
    int       i, j;
    
    set = (float**)mbkalloc (nx * sizeof (float*)); 
    for (i = 0; i < nx; i++) {
        set[i] = (float*)mbkalloc (ny * sizeof (float));
        for (j = 0; j < ny; j++) 
            set[i][j] = stm_modtbl_initval (); 
    }
    return set;
}

/****************************************************************************/

void stm_modtbl_destroyset2D (float **set, int x)
{
    int i;
    
    for (i = 0; i < x; i++) 
        mbkfree (set[i]);
    mbkfree (set);
}

/****************************************************************************/
/*                         1D table                                         */
/****************************************************************************/

float *stm_modtbl_createset1D (int nx)
{
    float *set;
    int    i;
    
    set = (float*)mbkalloc (nx * sizeof (float)); 
    for (i = 0; i < nx; i++) 
        set[i] = stm_modtbl_initval (); 
    return set;
}

/****************************************************************************/

void stm_modtbl_destroyset1D (float *set)
{
    mbkfree (set);
}

/****************************************************************************/
/*                         x, y model ranges                                */
/****************************************************************************/

void  stm_modtbl_setXrange (timing_table *table, chain_list *xrange, float scale)
{
    chain_list *xr;
    int         x;

    if (table) {
        for (x = 0, xr = xrange; xr; xr = xr->NEXT)
            if (x < table->NX)
                table->XRANGE[x++] = *(float*)xr->DATA * scale;
    } else
        return;
}

/****************************************************************************/

void  stm_modtbl_setYrange (timing_table *table, chain_list *yrange, float scale)
{
    chain_list *yr;
    int         y;

    if (table) {
        for (y = 0, yr = yrange; yr; yr = yr->NEXT)
            if (y < table->NY)
                table->YRANGE[y++] = *(float*)yr->DATA * scale;
    } else
        return;
}

/****************************************************************************/
/*                  table values                                            */
/****************************************************************************/

void stm_modtbl_set1Dval (timing_table *table, int xpos, float value)
{
    if (table) {
        if (xpos >= 0 && xpos < table->NX)
            table->SET1D[xpos] = value;
    } else
        return;
}

/****************************************************************************/

void stm_modtbl_setXrangeval (timing_table *table, int xpos, float value)
{
    if (table) {
        if (xpos >= 0 && xpos < table->NX)
            table->XRANGE[xpos] = value;
    } else
        return;
}

/****************************************************************************/

void stm_modtbl_setYrangeval (timing_table *table, int ypos, float value)
{
    if (table) {
        if (ypos >= 0 && ypos < table->NY)
            table->YRANGE[ypos] = value;
    } else
        return;
}

/****************************************************************************/

void stm_modtbl_set2Dval (timing_table *table, int xpos, int ypos, float value)
{
    if (table) {
        if (xpos >= 0 && xpos < table->NX && ypos >= 0 && ypos < table->NY)
            table->SET2D[xpos][ypos] = value;
    } else
        return;
}

/****************************************************************************/

void stm_modtbl_set1Dset (timing_table *table, chain_list *xdata, float scale)
{
    chain_list   *xd;
    int           x;

    if (table)
        for (x = 0, xd = xdata; xd; xd = xd->NEXT)
            stm_modtbl_set1Dval (table, x++, *(float*)xd->DATA * scale);
    else
        return;
}

/****************************************************************************/

void stm_modtbl_set2Dset (timing_table *table, chain_list *xydata, float scale)
{
    chain_list   *xd;
    chain_list   *yd;
    int           x;
    int           y;

    if (table)
        for (x = 0, xd = xydata; xd; xd = xd->NEXT) {
            for (y = 0, yd = (chain_list*)xd->DATA; yd; yd = yd->NEXT)
                stm_modtbl_set2Dval (table, x, y++, *(float*)yd->DATA * scale);
            x++;
        }
    else
        return;
}

/****************************************************************************/

void stm_modtbl_setconst (timing_table *tab, float value)
{
    tab->CST = value;
}

/****************************************************************************/

int stm_modtbl_isnull (float value)
{
    return value == STM_NOVALUE;
}

/****************************************************************************/

float stm_modtbl_initval ()
{
    return STM_NOVALUE;
}

/****************************************************************************/

float stm_modtbl_getslewaxisval (timing_table *table, int i)
{
    if (!table)
        return 0;
        
    if (table->YTYPE == STM_INPUT_SLEW)
        return table->YRANGE[i];
        
    if (table->XTYPE == STM_INPUT_SLEW)
        return table->XRANGE[i];

    return 0;
}

/****************************************************************************/

float stm_modtbl_getckslewaxisval (timing_table *table, int i)
{
    if (!table)
        return 0;
        
    if (table->YTYPE == STM_CLOCK_SLEW)
        return table->YRANGE[i];
        
    if (table->XTYPE == STM_CLOCK_SLEW)
        return table->XRANGE[i];

    return 0;
}

/****************************************************************************/

float stm_modtbl_getloadaxisval (timing_table *table, int i)
{
    if (!table)
        return 0;
        
    if (table->YTYPE == STM_LOAD)
        return table->YRANGE[i];
        
    if (table->XTYPE == STM_LOAD)
        return table->XRANGE[i];

    return 0;
}

/****************************************************************************/

float *stm_modtbl_getloadaxis (timing_table *table, int *nval)
{

    if (!table) {
        *nval = 0;
        return NULL;
    }
    
    if (table->XTYPE == STM_LOAD) {
        *nval = table->NX;
        return table->XRANGE;
    }
    else
    if (table->YTYPE == STM_LOAD) {
        *nval = table->NY;
        return table->YRANGE;
    }
    else {
        *nval = 0;
        return NULL;
    }
}
    
/****************************************************************************/

float *stm_modtbl_getslewaxis (timing_table *table, int  *nval)
{
    if (!table) {
        *nval = 0;
        return NULL;
    }
    
    if (table->XTYPE == STM_INPUT_SLEW || table->XTYPE == STM_CLOCK_SLEW) {
        *nval = table->NX;
        return table->XRANGE;
    }
    else
    if (table->YTYPE == STM_INPUT_SLEW || table->YTYPE == STM_CLOCK_SLEW) {
        *nval = table->NY;
        return table->YRANGE;
    }
    else {
        *nval = 0;
        return NULL;
    }
}
    
/****************************************************************************/

timing_table *stm_modtbl_create_fequac (float sdt, float sck, float fdt, float fck, float t)
{
    timing_table *table = NULL;

    if (fck >= 0 && fdt >= 0) {
        table = stm_modtbl_create (2, 2, STM_CLOCK_SLEW, STM_INPUT_SLEW);
        table->XRANGE[0] = fck;
        table->XRANGE[1] = 2 * fck;
        table->YRANGE[0] = fdt;
        table->YRANGE[1] = 2 * fdt;
    
        table->SET2D[0][0] = t;
        table->SET2D[0][1] = (sdt * fdt) / 1000.0 + t;
        table->SET2D[1][0] = (sck * fck) / 1000.0 + t;
        table->SET2D[1][1] = (sdt * fdt) / 1000.0 + (sck * fck) / 1000.0 + t;
    }

    if (fck >= 0 && fdt < 0) {
        table = stm_modtbl_create (2, 0, STM_CLOCK_SLEW, STM_NOTYPE);
        table->XRANGE[0] = fck;
        table->XRANGE[1] = 2 * fck;
    
        table->SET1D[0] = t;
        table->SET1D[1] = (sck * fck) / 1000.0 + t;
    }

    if (fck < 0 && fdt >= 0) {
        table = stm_modtbl_create (2, 0, STM_INPUT_SLEW, STM_NOTYPE);
        table->XRANGE[0] = fdt;
        table->XRANGE[1] = 2 * fdt;
    
        table->SET1D[0] = t;
        table->SET1D[1] = (sdt * fdt) / 1000.0 + t;
    }

    return table;
}

/****************************************************************************/

timing_table *stm_modtbl_create_fequa (float r, float s, float c, float f, float t)
{
    timing_table *table = NULL;

    if (c >= 0 && f >= 0) {
        table = stm_modtbl_create (2, 2, STM_INPUT_SLEW, STM_LOAD);
        table->YRANGE[0] = c;
        table->YRANGE[1] = 2 * c;
        table->XRANGE[0] = f;
        table->XRANGE[1] = 2 * f;
    
        table->SET2D[0][0] = t;
        table->SET2D[1][0] = (s * f) / 1000.0 + t;
        table->SET2D[0][1] = (r * c) / 1000.0 + t;
        table->SET2D[1][1] = (r * c) / 1000.0 + (s * f) /1000.0 + t;
    }

    if (c >= 0 && f < 0) {
        table = stm_modtbl_create (2, 0, STM_LOAD, STM_NOTYPE);
        table->XRANGE[0] = c;
        table->XRANGE[1] = 2 * c;
    
        table->SET1D[0] = t;
        table->SET1D[1] = (r * c) / 1000.0 + t;
    }

    if (c < 0 && f >= 0) {
        table = stm_modtbl_create (2, 0, STM_INPUT_SLEW, STM_NOTYPE);
        table->XRANGE[0] = f;
        table->XRANGE[1] = 2 * f;
    
        table->SET1D[0] = t;
        table->SET1D[1] = (s * f)/1000.0 + t;
    }

    return table;
}

/****************************************************************************/

timing_table *stm_modtbl_create_fcst (float value)
{
    timing_table *table = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
    stm_modtbl_setconst (table, value);

    return table;
}
