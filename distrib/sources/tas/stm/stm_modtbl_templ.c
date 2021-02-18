/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_templ.c                                             */
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

timing_ttable *stm_modtbl_replacetemplate (chain_list *chain, timing_ttable *ttemplate)
{
    chain_list *ch;

    for (ch = chain; ch; ch = ch->NEXT)
        if (((timing_ttable*)ch->DATA)->NAME == ttemplate->NAME) {
            ((timing_ttable*)ch->DATA)->NAME   = ttemplate->NAME;
            ((timing_ttable*)ch->DATA)->NX     = ttemplate->NX;
            ((timing_ttable*)ch->DATA)->NY     = ttemplate->NY;
            mbkfree(((timing_ttable*)ch->DATA)->XRANGE);
            mbkfree(((timing_ttable*)ch->DATA)->YRANGE);
            ((timing_ttable*)ch->DATA)->XRANGE = ttemplate->XRANGE;
            ((timing_ttable*)ch->DATA)->YRANGE = ttemplate->YRANGE;
            ((timing_ttable*)ch->DATA)->XTYPE  = ttemplate->XTYPE;
            ((timing_ttable*)ch->DATA)->YTYPE  = ttemplate->YTYPE;
            mbkfree(ttemplate);
            return (timing_ttable*)ch->DATA;
        }

    return ttemplate;
}

/****************************************************************************/

timing_ttable *stm_modtbl_storetemplate (char *templ, timing_ttable *ttemplate)
{
    char            *lname;
    long             temp;
    timing_ttable *oldtemplate;

    if (!STM_TEMPLATES_HT)
        STM_TEMPLATES_HT = addht (STM_NBTEMPLATES);
    
    lname = namealloc (templ);
    STM_TEMPLATE_NAME = addchain (STM_TEMPLATE_NAME, lname);
    ttemplate->NAME = lname;
    temp = gethtitem (STM_TEMPLATES_HT, lname);

    if (temp == EMPTYHT || temp == DELETEHT) {
        STM_TEMPLATES_CHAIN = addchain (STM_TEMPLATES_CHAIN, ttemplate);
        addhtitem (STM_TEMPLATES_HT, lname, (long)ttemplate);
        return ttemplate;
    } else {
        oldtemplate = stm_modtbl_replacetemplate (STM_TEMPLATES_CHAIN, ttemplate);
        addhtitem (STM_TEMPLATES_HT, lname, (long)oldtemplate);
        return oldtemplate;
    }
}

/****************************************************************************/

timing_ttable *stm_modtbl_addtemplate (char *name, int nx, int ny, char xtype, char ytype)
{
    timing_ttable *ttemplate;
    char  *lname;

    lname = namealloc (name);
    ttemplate = stm_modtbl_createtemplate (lname, nx, ny, xtype, ytype);
    
    return stm_modtbl_storetemplate (name, ttemplate);
}

/****************************************************************************/

timing_ttable *stm_modtbl_createtemplate (char *name, int nx, int ny, char xtype, char ytype)
{
    timing_ttable *ttemplate;
    int    i;
    
    ttemplate = (timing_ttable*)mbkalloc (sizeof (struct timing_ttable));
    
    ttemplate->NAME = name;
    
    ttemplate->NX= nx > 0 ? nx : 0; 
    ttemplate->NY= ny > 0 ? ny : 0; 
    ttemplate->XTYPE = xtype;
    ttemplate->YTYPE = ytype;
    ttemplate->XRANGE = NULL;
    ttemplate->YRANGE = NULL;
    ttemplate->XTYPEBIS = 0;
        
    if (ttemplate->NX) {
        ttemplate->XRANGE = (float*)mbkalloc (ttemplate->NX * sizeof (float));
        for (i = 0; i < ttemplate->NX; i++)
            ttemplate->XRANGE[i] = stm_modtbl_initval ();
    }

    if (ttemplate->NY) {
        ttemplate->YRANGE = (float*)mbkalloc (ttemplate->NY * sizeof (float));
        for (i = 0; i < ttemplate->NY; i++)
            ttemplate->YRANGE[i] = stm_modtbl_initval ();
    }

    return ttemplate;
}

/****************************************************************************/

int stm_modtbl_sametemplate (timing_ttable *temp1, timing_ttable *temp2)
{
    int i;

    if (temp1->XTYPE != temp2->XTYPE)
        return 0;
    if (temp1->YTYPE != temp2->YTYPE)
        return 0;
    if (temp1->NX != temp2->NX)
        return 0;
    if (temp1->NY != temp2->NY)
        return 0;
    for (i = 0; i < temp1->NX; i++)
        if (temp1->XRANGE[i] != temp2->XRANGE[i])
            return 0;
    for (i = 0; i < temp1->NY; i++)
        if (temp1->YRANGE[i] != temp2->YRANGE[i])
            return 0;
    if (temp1->XTYPEBIS != temp2->XTYPEBIS)
        return 0;
    return 1;
}

/****************************************************************************/

timing_ttable *stm_modtbl_getmtemplate (timing_model *model)
{
    timing_ttable *templ;
    int i;

    if (model->UTYPE != STM_MOD_MODTBL)
        return NULL;
    
    templ = stm_modtbl_createtemplate (NULL, model->UMODEL.TABLE->NX, 
                                         model->UMODEL.TABLE->NY, 
                                         model->UMODEL.TABLE->XTYPE, 
                                         model->UMODEL.TABLE->YTYPE);
    for (i = 0; i < model->UMODEL.TABLE->NX; i++)
        templ->XRANGE[i] = model->UMODEL.TABLE->XRANGE[i];
    for (i = 0; i < model->UMODEL.TABLE->NY; i++)
        templ->YRANGE[i] = model->UMODEL.TABLE->YRANGE[i];
    templ->XTYPEBIS = model->UMODEL.TABLE->XTYPEBIS; 
    return templ;
}

/****************************************************************************/

char *stm_modtbl_splittemplname (char *templ, int *n)
{
    char *temp;
    int   i;
    
    temp = strdup (templ);
    i = strlen (temp) - 1;
    while (isdigit ((int)temp[i--]));
    temp[i + 2] = '\0';
    *n = atoi (templ + i + 2);
    
    return temp;
}

/****************************************************************************/

char *stm_modtbl_templname (timing_ttable *templ)
{
    char        buf[1024];
    char       *buff,
               *name,
               *split;
    int         n, 
                order = 0;
    chain_list *ch;

    sprintf (buf, "const_");
    
    if (templ->XTYPE == STM_INPUT_SLEW)
        sprintf (buf, "inslew_");
    else
    if (templ->XTYPE == STM_CLOCK_SLEW)
        sprintf (buf, "ckslew_");
    else
    if (templ->XTYPE == STM_LOAD)
        sprintf (buf, "load_");
    
    buff = strdup (buf);
    if (templ->YTYPE == STM_INPUT_SLEW)
        sprintf (buf, "%sinslew_", buff);
    else
    if (templ->YTYPE == STM_CLOCK_SLEW)
        sprintf (buf, "%sckslew_", buff);
    else
    if (templ->YTYPE == STM_LOAD)
        sprintf (buf, "%sload_", buff);
    else
        sprintf (buf, "%s", buff);
    mbkfree(buff);

    buff = strdup (buf);
    if (templ->NX && templ->NY)
        sprintf (buf, "%s%dx%d__", buff, templ->NX, templ->NY);
    else
    if (templ->NX)
        sprintf (buf, "%s%d__", buff, templ->NX);
    else
    if (templ->NY)
        sprintf (buf, "%s%d__", buff, templ->NY);
    mbkfree(buff);

    for (ch = STM_TEMPLATE_NAME; ch; ch = ch->NEXT) {
        split = stm_modtbl_splittemplname ((char*)ch->DATA, &n);
        if (!strcmp (split, buf))
            order = n + 1 > order ? n + 1 : order; 
        mbkfree(split);
    }

    buff = strdup (buf);
    sprintf (buf, "%s%d", buff, order);
    mbkfree(buff);
    
    name = namealloc (buf);
    STM_TEMPLATE_NAME = addchain (STM_TEMPLATE_NAME, name);

    return name;
}

/****************************************************************************/

void stm_modtbl_findtemplates (timing_cell *cell)
{
    timing_model  *model;
    timing_ttable *templ;
    chain_list    *ch;
    chain_list    *modelch;
    char          *name;

    for (modelch = cell->MODEL_LIST; modelch; modelch = modelch->NEXT) {
        model = (timing_model*)modelch->DATA;
        if (model->UTYPE != STM_MOD_MODTBL)
            continue;
        if (!model->UMODEL.TABLE->SET1D && !model->UMODEL.TABLE->SET2D)
            continue;
        templ = stm_modtbl_getmtemplate (model);
        for (ch = STM_TEMPLATES_CHAIN; ch; ch = ch->NEXT)
            if (stm_modtbl_sametemplate ((timing_ttable*)ch->DATA, templ)) {
                model->UMODEL.TABLE->TEMPLATE = (timing_ttable*)ch->DATA;
                stm_modtbl_destroytemplate (templ);
                break;
            }
        if (!ch) {
            model->UMODEL.TABLE->TEMPLATE = templ;
            name = stm_modtbl_templname (templ);
            stm_modtbl_storetemplate (name, templ);
        }
    }
}

/****************************************************************************/

timing_ttable *stm_modtbl_gettemplate (char *name)
{
    long ttemplate;

    if (STM_TEMPLATES_HT == NULL)
        return NULL;

    ttemplate = gethtitem (STM_TEMPLATES_HT, namealloc (name));

    if ((ttemplate == EMPTYHT) || (ttemplate == DELETEHT))
        return NULL;
    else
        return (timing_ttable*)ttemplate;
}
    
/****************************************************************************/
    
void stm_modtbl_freetemplate (char *name)
{
    long  ttemplate;
    char *lname;

    lname = namealloc (name);
    ttemplate = gethtitem (STM_TEMPLATES_HT, lname);
    if ((ttemplate != EMPTYHT) && (ttemplate != DELETEHT)) {
        stm_modtbl_destroytemplate ((timing_ttable*)ttemplate);
        delhtitem (STM_TEMPLATES_HT, lname);
    }
}

/****************************************************************************/

void stm_modtbl_destroytemplate (timing_ttable *ttemplate)
{
    if(ttemplate->XRANGE)
        mbkfree (ttemplate->XRANGE);
    if(ttemplate->YRANGE)
        mbkfree (ttemplate->YRANGE);
    if(ttemplate)
        mbkfree (ttemplate);
}

/****************************************************************************/

void  stm_modtbl_settemplateXrange (timing_ttable *ttemplate, chain_list *xrange, float scale)
{
    chain_list *xr;
    int         x;

    if (ttemplate) {
        for (x = 0, xr = xrange; xr; xr = xr->NEXT)
            if (x < ttemplate->NX)
                ttemplate->XRANGE[x++] = *(float*)xr->DATA * scale;
    }
}

/****************************************************************************/

void  stm_modtbl_settemplateYrange (timing_ttable *ttemplate, chain_list *yrange, float scale)
{
    chain_list *yr;
    int         y;

    if (ttemplate) {
        for (y = 0, yr = yrange; yr; yr = yr->NEXT)
            if (y < ttemplate->NY)
                ttemplate->YRANGE[y++] = *(float*)yr->DATA * scale;
    }
}
