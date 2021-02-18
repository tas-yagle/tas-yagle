#include ELP_H
#include STM_H
#define API_USE_REAL_TYPES
#include "stm_API.h"

//_____________________________________________________________________________

void *stm_AddCell (char *cellname)
{
    return (timing_cell*)stm_addcell (cellname);
}

//_____________________________________________________________________________

void *stm_ModCreateTable (char *name, int nx, int ny, char xtype, char ytype)
{
    return (timing_model*)stm_mod_create_table (name,nx,ny,xtype,ytype);
}

//_____________________________________________________________________________

void stm_ModTblSetXrangeVal (void *model, int xpos, double value)
{
    timing_table *table;

    table = ((timing_model*)model)->UMODEL.TABLE;
    stm_modtbl_setXrangeval (table, xpos, value);
}

//_____________________________________________________________________________

void stm_ModTblSetYrangeVal (void *model, int ypos, double value)
{
    timing_table *table;

    table = ((timing_model*)model)->UMODEL.TABLE;
    stm_modtbl_setYrangeval (table, ypos, value);
}

//_____________________________________________________________________________

void stm_ModTblSetXrange (void *model, chain_list *xrange)
{
    timing_table *table;

    table = ((timing_model*)model)->UMODEL.TABLE;
    stm_modtbl_setXrange (table, (chain_list*)xrange, 1.0);
}

//_____________________________________________________________________________

void stm_ModTblSetYrange (void *model, chain_list *yrange)
{
    timing_table *table;

    table = ((timing_model*)model)->UMODEL.TABLE;
    stm_modtbl_setXrange (table, (chain_list*)yrange, 1.0);
}

//_____________________________________________________________________________

void stm_ModTblSet2Dval (void *model, int xpos, int ypos, double value)
{
    timing_table *table;

    table = ((timing_model*)model)->UMODEL.TABLE;
    stm_modtbl_set2Dval (table, xpos, ypos, value);
}

//_____________________________________________________________________________

char *stm_CellAddModel (void *cell, void *model, int noshare)
{
    return stm_cell_addmodel ((timing_cell*)cell, (timing_model*)model, noshare);
}

//_____________________________________________________________________________

void stm_DriveCell (char *cellname)
{
    stm_drivecell (cellname);
}

//_____________________________________________________________________________

chain_list *stm_ModTblAddRangeVal (chain_list *range, double value)
{
    double *ptval;

    ptval = (double*) malloc (sizeof (double));
    *ptval = value;

    range = addchain (range,ptval);
    return (chain_list*)range;
}

//_____________________________________________________________________________

chain_list *stm_ModTblCreateRange (char *rangevalues)
{
    double *ptval;
    char    separ = ':';
    char   *val;
    char    buf[2048];
    chain_list *range = NULL;

    if (!rangevalues) return NULL;
    sprintf (buf,rangevalues);
    val = strtok (buf,&separ);
    if (val) {
        ptval = (double*) malloc (sizeof (double));
        *ptval = (double) atof (val);
        range = addchain (range,ptval);
        while ((val = strtok(NULL,&separ))) {
            ptval = (double*) malloc (sizeof (double));
            *ptval = (double) atof (val);
            range = addchain (range,ptval);
        }

    }
    return range;
}

//_____________________________________________________________________________

void stm_FreeTblRange (chain_list *range)
{
    chain_list *chain = (chain_list*)range;
    chain_list *chain2free;

    if (!chain) return;

    while (chain->NEXT) {
        chain2free = chain;
        chain = chain->NEXT;
        free (chain2free->DATA);
    }
    freechain ((chain_list*)range);
}


// METHO

void SET_DELAY(double value)
{  
  stm_set_computed_delay(value);
}

double GET_DELAY()
{
  return stm_get_computed_delay();
}

void SET_SLOPE(double value)
{  
  stm_set_computed_slope(value);
}

double GET_SLOPE()
{
  return stm_get_computed_slope();
}

double GET_OUTPUT_CAPA()
{
  return stm_get_output_load();
}

double GET_INPUT_SLOPE()
{
  return stm_get_input_slope();
}

double GET_COMMAND_INPUT_SLOPE()
{
  return stm_get_command_slope();
}

void CALL_SIMULATION()
{
  stm_call_simulation();
}

void CALL_SIMULATION_ENV()
{
  stm_call_simulation_env();
}

void CALL_CTK_ENV()
{
  stm_call_ctk_env();
}


