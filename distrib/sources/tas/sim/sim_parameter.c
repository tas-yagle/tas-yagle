/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim_obj.c                                                    */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                                                                           */
/*****************************************************************************/

#include AVT_H
#include SIM_H
#include "sim.h"

void  sim_parameter_set_remove_files( sim_model *model, char rm )
{
  model->PARAMETER.REMOVE_FILES = rm;
}

char  sim_parameter_get_remove_files( sim_model *model )
{
  return model->PARAMETER.REMOVE_FILES ;
}

void  sim_parameter_set_allow_overwrite_files( sim_model *model, char ovr )
{
  model->PARAMETER.OVR_FILES = ovr;
}

char  sim_parameter_get_allow_overwrite_files( sim_model *model )
{
  return model->PARAMETER.OVR_FILES ;
}

void  sim_parameter_set_slope( sim_model *model, SIM_FLOAT slope )
{
  model->PARAMETER.SLOPE = slope;
}

SIM_FLOAT sim_parameter_get_slope( sim_model *model )
{
  return model->PARAMETER.SLOPE;
}

void  sim_parameter_set_noise_type( sim_model *model, char type)
{
  model->PARAMETER.NOISE_TYPE = type;
}

char sim_parameter_get_noise_type( sim_model *model )
{
  return model->PARAMETER.NOISE_TYPE;
}

void  sim_parameter_set_alim( sim_model *model, SIM_FLOAT alim, char where )
{
  if (where=='i')
    model->PARAMETER.ALIM_VOLTAGE_IN = alim;
  else
    model->PARAMETER.ALIM_VOLTAGE_OUT = alim;
}

SIM_FLOAT sim_parameter_get_alim( sim_model *model, char where )
{
  if (where=='i')
    return model->PARAMETER.ALIM_VOLTAGE_IN;
  else
    return model->PARAMETER.ALIM_VOLTAGE_OUT;
}

SIM_FLOAT sim_parameter_get_vss( sim_model *model, char where )
{
  if (where=='i')
    return model->PARAMETER.VSS_VOLTAGE_IN;
  else
    return model->PARAMETER.VSS_VOLTAGE_OUT;
}

void  sim_parameter_set_vss( sim_model *model, SIM_FLOAT alim, char where )
{
  if (where=='i')
    model->PARAMETER.VSS_VOLTAGE_IN = alim;
  else
    model->PARAMETER.VSS_VOLTAGE_OUT = alim;
}

void  sim_parameter_set_slopeVTHH   ( sim_model *model, SIM_FLOAT vthh )
{
  model->PARAMETER.VTH_HIGH = vthh;
}

SIM_FLOAT sim_parameter_get_slopeVTHH ( sim_model *model )
{
  return model->PARAMETER.VTH_HIGH;
}

void  sim_parameter_set_slopeVTHL  ( sim_model *model, SIM_FLOAT vthl )
{
  model->PARAMETER.VTH_LOW = vthl;
}

SIM_FLOAT sim_parameter_get_slopeVTHL ( sim_model *model )
{
  return model->PARAMETER.VTH_LOW;
}

void  sim_parameter_set_delayVTH   ( sim_model *model, SIM_FLOAT vthstart,  SIM_FLOAT vthend)
{
  model->PARAMETER.VTHSTART = vthstart;
  model->PARAMETER.VTHEND = vthend;
}

SIM_FLOAT sim_parameter_get_delayVTHSTART( sim_model *model )
{
  return model->PARAMETER.VTHSTART;
}
SIM_FLOAT sim_parameter_get_delayVTHEND( sim_model *model )
{
  return model->PARAMETER.VTHEND;
}

void  sim_parameter_set_temperature( sim_model *model, SIM_FLOAT temp )
{
  model->PARAMETER.TEMP = temp;
}

SIM_FLOAT sim_parameter_get_temperature( sim_model *model )
{
  return model->PARAMETER.TEMP;
}

void  sim_parameter_set_tran_tmax( sim_model *model, SIM_FLOAT tmax )
{
  model->PARAMETER.TRANSIANT_TMAX = tmax;
}

SIM_FLOAT sim_parameter_get_tran_tmax( sim_model *model )
{
  return model->PARAMETER.TRANSIANT_TMAX;
}

void  sim_parameter_set_tran_step( sim_model *model, SIM_FLOAT step )
{
  model->PARAMETER.TRANSIANT_STEP = step;
}

SIM_FLOAT sim_parameter_get_tran_step( sim_model *model )
{
  return model->PARAMETER.TRANSIANT_STEP;
}

void  sim_parameter_set_simu_step( sim_model *model, SIM_FLOAT step )
{
  model->PARAMETER.SIMULATOR_STEP = step;
}

SIM_FLOAT sim_parameter_get_simu_step( sim_model *model )
{
  return model->PARAMETER.SIMULATOR_STEP;
}

void  sim_parameter_set_tool_cmd( sim_model *model, char *cmd )
{
  model->PARAMETER.TOOL_CMD = sensitive_namealloc(cmd);
}

char* sim_parameter_get_tool_cmd( sim_model *model )
{
  return model->PARAMETER.TOOL_CMD;
}

void  sim_parameter_set_output_file( sim_model *model, char *out )
{
  model->PARAMETER.TOOL_OUTFILE = sensitive_namealloc(out);
}

char* sim_parameter_get_output_file( sim_model *model )
{
  return model->PARAMETER.TOOL_OUTFILE ;
}

void  sim_parameter_set_stdoutput_file( sim_model *model, char *out )
{
  model->PARAMETER.TOOL_STDOUTFILE = sensitive_namealloc(out);
}

char* sim_parameter_get_stdoutput_file( sim_model *model )
{
  return model->PARAMETER.TOOL_STDOUTFILE ;
}

void  sim_parameter_set_tool( sim_model *model, int tool )
{
  model->PARAMETER.TOOL = tool;
}

int sim_parameter_get_tool( sim_model *model )
{
  return model->PARAMETER.TOOL;
}

void sim_parameter_set_drive_netlist( sim_model *model, char need )
{
  model->PARAMETER.DRIVE_NETLIST = need;
}

char sim_parameter_get_drive_netlist( sim_model *model )
{
  return model->PARAMETER.DRIVE_NETLIST;
}

void sim_parameter_add_techno_file( sim_model *model, char *techno )
{
  if (!avt_is_default_technoname(techno))
    model->PARAMETER.TECHNOFILELIST = append( model->PARAMETER.TECHNOFILELIST, 
                                              addchain(NULL, sensitive_namealloc(techno)) 
                                              );
}

chain_list* sim_parameter_get_techno_file_head( sim_model *model )
{
  return model->PARAMETER.TECHNOFILELIST;
}

void sim_parameter_clean( sim_model *model )
{
/*
  chain_list    *scan;
  for( scan = model->PARAMETER.TECHNOFILELIST ; scan ; scan = scan->NEXT )
    mbkfree( scan->DATA );
*/
  freechain( model->PARAMETER.TECHNOFILELIST );
  model->PARAMETER.TECHNOFILELIST = NULL;

}

void sim_parameter_init( sim_model *model )
{
  simenv();

  model->PARAMETER.NOISE_TYPE     = SIM_MAX;
  model->PARAMETER.ALIM_VOLTAGE_OUT = model->PARAMETER.ALIM_VOLTAGE_IN = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
  model->PARAMETER.VSS_VOLTAGE_OUT = model->PARAMETER.VSS_VOLTAGE_IN = 0;
  model->PARAMETER.SLOPE          = SIM_UNIT_X_TO_Y (SIM_SLOP, 1e-12, SIM_UNIT_TIME);
  model->PARAMETER.TEMP           = V_FLOAT_TAB[__SIM_TEMP].VALUE;
  if (SIM_VTH != ELPINITTHR)
    model->PARAMETER.VTHSTART=  model->PARAMETER.VTHEND          = SIM_VTH;
  else
    model->PARAMETER.VTHSTART=  model->PARAMETER.VTHEND            = 0.5;
  if (SIM_VTH_LOW != ELPINITTHR)
    model->PARAMETER.VTH_LOW        = SIM_VTH_LOW;
  else
    model->PARAMETER.VTH_LOW        = 0.2;
  if (SIM_VTH_HIGH != ELPINITTHR)
    model->PARAMETER.VTH_HIGH       = SIM_VTH_HIGH;
  else
    model->PARAMETER.VTH_HIGH       = 0.8;
  model->PARAMETER.TRANSIANT_TMAX = V_FLOAT_TAB[ __SIM_TIME ].VALUE ;
  model->PARAMETER.TRANSIANT_STEP = V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE ;

  if( SIM_SIMU_STEP != SIM_UNDEF )
    model->PARAMETER.SIMULATOR_STEP = SIM_UNIT_X_TO_Y (SIM_SIMU_STEP, 1.0e-9, SIM_UNIT_TIME);
  else
    model->PARAMETER.SIMULATOR_STEP = SIM_SIMU_STEP ;
    
  model->PARAMETER.TOOL_CMD       = SIM_SPICESTRING;
  model->PARAMETER.TOOL_OUTFILE   = SIM_SPICEOUT;
  model->PARAMETER.TOOL_STDOUTFILE = SIM_SPICESTDOUT;
  model->PARAMETER.TOOL     = V_INT_TAB[__SIM_TOOL].VALUE;
  model->PARAMETER.TECHNOFILELIST = NULL;
  if ( !avt_is_default_technoname (SIM_TECHFILE))
  {
    char *cur, *c, *buf=mbkstrdup(SIM_TECHFILE);
    
    cur=strtok_r(buf, ",", &c);
    while (cur!=NULL)
    {
      model->PARAMETER.TECHNOFILELIST = append(model->PARAMETER.TECHNOFILELIST, addchain( NULL, sensitive_namealloc(cur) ));
      cur=strtok_r(NULL, ",", &c);
    }
    mbkfree(buf);
  }
  model->PARAMETER.DRIVE_NETLIST  = SIM_DEFAULT_DRIVE_NETLIST;
  model->PARAMETER.OVR_FILES      = SIM_DEFAULT_OVR_FILES;
  model->PARAMETER.REMOVE_FILES   = SIM_DEFAULT_REMOVE_FILES;
  model->PARAMETER.TRANSISTOR_AS_INSTANCE   = V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE;
  model->PARAMETER.PWL_FUNC = sim_builtin_tanh();
}

