#include MUT_H
#include SIM_H
#include MCC_H
#include MSL_H
#include GEN_H

#define API_USE_REAL_TYPES
#include "gen_API.h"
#include "sim_API_extract.h"

//#############################################################################
//#############################################################################

//typedef SIM_FLOAT (*PWL_FUNCTION) (SIM_FLOAT, void *);

//_____________________________________________________________________________

chain_list  *FLOAT_TO_FREE = NULL;
chain_list  *SIM_MODEL     = NULL;
//PWL_FUNCTION PWL_FUNC      = NULL;
int          READ_RESULT   = 0;
char         SIM_SEPAR     = '.';


char *gen_sim_devect(char *name)
{
  return namealloc(gen_makesignalname(name));
}


//_____________________________________________________________________________

void sim_DriveTransistorAsInstance(sim_model *sc, char mode)
{
  if (tolower(mode)=='y')
    sc->PARAMETER.TRANSISTOR_AS_INSTANCE=SIM_YES;
  else
    sc->PARAMETER.TRANSISTOR_AS_INSTANCE=SIM_NO;
}


void sim_API_Action_Initialize ()// commentaire pour desactiver l'ajout de token
{
    char *str;
    READ_RESULT = 0;
//    PWL_FUNC = NULL;

    FLOAT_TO_FREE = NULL;
    SIM_MODEL = NULL;

    if ((str = V_STR_TAB[__MBK_SEPAR].VALUE))
        SIM_SEPAR = str[0];
    else 
        SIM_SEPAR = '.';

}

//_____________________________________________________________________________

void sim_API_Action_Terminate ()// commentaire pour desactiver l'ajout de token
{
    char *figname;
    char separ = SEPAR;
    chain_list *to_free, *sim;
    sim_model *mdl;

    SEPAR = SIM_SEPAR;
    
    for (sim = SIM_MODEL; sim; sim = sim->NEXT) {
      mdl=(sim_model*)sim->DATA;
      figname = mdl->FIG->NAME;
      freeflatmodel (mdl->FIG);
      mdl->FIG = NULL;
      sim_model_clear (figname);
    }
    freechain (SIM_MODEL);
    SIM_MODEL = NULL;

    for (to_free = FLOAT_TO_FREE; to_free; to_free = to_free->NEXT)
        free ((SIM_FLOAT*)to_free->DATA);
    freechain (FLOAT_TO_FREE);
    FLOAT_TO_FREE = NULL;

    SEPAR = separ;
}

//#############################################################################
//#############################################################################
//_______________ UTILS _______________________________________________________
//#############################################################################
//#############################################################################

sim_model *sim_new_model_if_null (lofig_list *fig)
{
    sim_model *sc;

    if (!fig) return NULL;
    sc = sim_model_create_new (fig->NAME);
    sim_parameter_set_tool (sc,V_INT_TAB[__SIM_TOOL].VALUE);
    sim_model_set_lofig (sc, fig);
    sim_parameter_set_drive_netlist (sc, SIM_YES);
    return sc;
}

//_____________________________________________________________________________

sim_model *sim_CreateContext (lofig_list *fig)
{
    sim_model *mod = sim_new_model_if_null ((lofig_list*)fig);
//    SIM_MODEL = addchain (SIM_MODEL, mod);
    return mod;
}

void sim_FreeContext (sim_model *model)
{
  sim_clean_netlist(model);
  gns_FreeNetlist(model->FIG);
  model->FIG=NULL;
  sim_free_context (model);
}
//_____________________________________________________________________________

void sim_read_tab_if_null (sim_model *sc)
{
    if (!READ_RESULT) {
        READ_RESULT = 1;
        sim_parse_spiceout (sc);
    }
}

//#############################################################################
//#############################################################################
//_______________ GENERAL PARAMETERS __________________________________________
//#############################################################################
//#############################################################################

void sim_SetSimulatorType (sim_model *sc, char *type)
{
    if (!sc) return;
    if (!strcasecmp (type, "eldo"))
        sim_parameter_set_tool (sc, SIM_TOOL_ELDO);
    else
    if (!strcasecmp (type, "mspice"))
        sim_parameter_set_tool (sc, SIM_TOOL_MSPICE);
    else
    if (!strcasecmp (type, "hspice"))
        sim_parameter_set_tool (sc, SIM_TOOL_HSPICE);
    else
    if (!strcasecmp (type, "titan"))
        sim_parameter_set_tool (sc, SIM_TOOL_TITAN);
    else
    if (!strcasecmp (type, "titanv7"))
        sim_parameter_set_tool (sc, SIM_TOOL_TITAN7);
    else
    if (!strcasecmp (type, "spice"))
        sim_parameter_set_tool (sc, SIM_TOOL_SPICE);
    else
    if (!strcasecmp (type, "ngspice"))
        sim_parameter_set_tool (sc, SIM_TOOL_NGSPICE);
    else
    if (!strcasecmp (type, "ltspice"))
        sim_parameter_set_tool (sc, SIM_TOOL_LTSPICE);
    else {
        V_INT_TAB[__SIM_TOOL].VALUE = SIM_TOOL_HSPICE;
        avt_errmsg(SIM_API_ERRMSG, "015", AVT_WARNING, type);
//        fprintf (stderr, "'%s' command file is generated with default wrapper\n", type);
    }
}

//_____________________________________________________________________________

void sim_SetSlopeForm (sim_model *sc, void *f)
{
    sc->PARAMETER.PWL_FUNC = (PWL_FUNCTION)f;
}

//_____________________________________________________________________________

void sim_SetSimulationTime (sim_model *sc, double time) // unit: SECOND
{
    if (!sc) return;
    sim_parameter_set_tran_tmax ((sim_model*)sc, SIM_UNIT_X_TO_Y (time, 1, SIM_UNIT_TIME));
}

//_____________________________________________________________________________

void sim_SetSimulationSlope (sim_model *sc, double slope) // unit: SECOND
{
    if (!sc) return;
    sim_parameter_set_slope ((sim_model*)sc, SIM_UNIT_X_TO_Y (slope, 1, SIM_UNIT_TIME));
}

//_____________________________________________________________________________

void sim_SetSimulationStep (sim_model *sc, double step) // unit: SECOND
{
    if (!sc) return;
    sim_parameter_set_tran_step ((sim_model*)sc, SIM_UNIT_X_TO_Y (step, 1, SIM_UNIT_TIME));
}

//_____________________________________________________________________________

void sim_SetSimulationSupply (sim_model *sc, double v_max) // unit: VOLT
{
    if (!sc) return;
    sim_parameter_set_alim ((sim_model*)sc, SIM_UNIT_X_TO_Y (v_max, 1, SIM_UNIT_VOLTAGE), 'i');
}

void sim_SetInputSwing (sim_model *sc, double v_vss, double v_max) // unit: VOLT
{
    if (!sc) return;
    sim_parameter_set_alim ((sim_model*)sc, SIM_UNIT_X_TO_Y (v_max, 1, SIM_UNIT_VOLTAGE), 'i');
    sim_parameter_set_vss ((sim_model*)sc, SIM_UNIT_X_TO_Y (v_vss, 1, SIM_UNIT_VOLTAGE), 'i');
}

void sim_SetOutputSwing (sim_model *sc, double v_vss, double v_max) // unit: VOLT
{
    if (!sc) return;
    sim_parameter_set_alim ((sim_model*)sc, SIM_UNIT_X_TO_Y (v_max, 1, SIM_UNIT_VOLTAGE), 'o');
    sim_parameter_set_vss ((sim_model*)sc, SIM_UNIT_X_TO_Y (v_vss, 1, SIM_UNIT_VOLTAGE), 'o');
}
//_____________________________________________________________________________

double sim_GetSimulationSupply () // unit: VOLT
{
  return V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
}

//_____________________________________________________________________________

void sim_SetSimulationTemp (sim_model *sc, double temp) // unit: CELSIUS
{
    if (!sc) return;
    sim_parameter_set_temperature ((sim_model*)sc, temp);
}

//_____________________________________________________________________________

void sim_SetDelayVTH (sim_model *sc, double vth) // pourcentage de VDD
{
    if (!sc) return;
    sim_parameter_set_delayVTH ((sim_model*)sc, vth, vth);
}

//_____________________________________________________________________________

void sim_SetSlopeVTH (sim_model *sc, double vth_low, double vth_high) // pourcentage de VDD
{
    if (!sc) return;
    sim_parameter_set_slopeVTHH ((sim_model*)sc, vth_high);
    sim_parameter_set_slopeVTHL ((sim_model*)sc, vth_low);
}

//_____________________________________________________________________________

void sim_AddSimulationTechnoFile (sim_model *sc, char *tech_file)
{
    if (!sc) return;
    sim_parameter_add_techno_file ((sim_model*)sc, tech_file);
}
//_____________________________________________________________________________

void sim_SetSimulationOutputFile (sim_model *sc, char *output_file)
{
    if (!sc) return;
    MCC_SPICEOUT = output_file;
}

//_____________________________________________________________________________

void sim_SetSimulationCall (sim_model *sc, char *sim_call)
{
    if (!sc) return;
    if (sim_call)
        sim_parameter_set_tool_cmd ((sim_model*)sc, sim_call);
}

//_____________________________________________________________________________

void sim_NoiseSetAnalyseType (sim_model *sc, char noise_type)
{
    if (!sc) return;
    if ((noise_type != SIM_MIN) && (noise_type != SIM_MAX))
        avt_errmsg(SIM_API_ERRMSG, "010", AVT_ERROR);
//        fprintf (stderr,"\n[SIMAPI ERR] sim_SetNoiseAnanlyseType : noise_type must be SIM_MIN or SIM_MAX\n");
    else
        sim_parameter_set_noise_type ((sim_model*)sc,noise_type);
}

//#############################################################################
//#############################################################################
//_______________ DC values ___________________________________________________ 
//#############################################################################
//#############################################################################

void sim_AddStuckLevelVector (sim_model *sc, char *node, char *level)
{
    int left, right, i, j = 0, lbin;
    char binary[1024];
    char *val;
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;

    if (level[0] == '0' && (level[1] == 'x' || level[1] == 'X')) 
        val = level + 2;
    else
        val = level;
    
    if (vectorindex (node) == -1) 
        vectorbounds (((sim_model*)sc)->FIG, namealloc (node), &left, &right);

    if ((vectorindex (node) != -1) || (left == -1 && right == -1)) { //vector element || not a vector
        if (!strcmp (val, "0"))
            sim_input_set_stuck_level ((sim_model*)sc, gen_sim_devect (node), SIM_ZERO);
        else
        if (!strcmp (val, "1"))
            sim_input_set_stuck_level ((sim_model*)sc, gen_sim_devect (node), SIM_ONE);
        else
            avt_errmsg(SIM_API_ERRMSG, "011", AVT_ERROR, val, node);
//            fprintf (stderr, "AddStuckLevelVector: bad value 0x%s for node %s\n", val, node);
        return;
    }
    
    if (left != -1 && right != -1) { // a vector

        sim_hex2bin (binary, val);
        lbin = strlen (binary);

        if (left <= right) { // v (left TO right)
            for (i = left; i <= right; i++) {
                if (j < lbin) {
                    if (binary[j] == '1')
                        sim_input_set_stuck_level ((sim_model*)sc, sim_vectorize (node, i), SIM_ONE);
                    else
                    if (binary[j] == '0')
                        sim_input_set_stuck_level ((sim_model*)sc, sim_vectorize (node, i), SIM_ZERO);
                    j++;
                }
                else // Fill the remaining LSB (right) bits with '0'
                    sim_input_set_stuck_level ((sim_model*)sc, sim_vectorize (node, i), SIM_ZERO);
            }
        }
                
        if (left > right) { // v (left DOWNTO right)
            j = lbin - 1;
            for (i = right; i <= left; i++) {
                if (j >= 0) {
                    if (binary[j] == '1')
                        sim_input_set_stuck_level ((sim_model*)sc, sim_vectorize (node, i), SIM_ONE);
                    else
                    if (binary[j] == '0')
                        sim_input_set_stuck_level ((sim_model*)sc, sim_vectorize (node, i), SIM_ZERO);
                    j--;
                }
                else // Fill the remaining LSB (left) bits with '0'
                    sim_input_set_stuck_level ((sim_model*)sc, sim_vectorize (node, i), SIM_ZERO);
            }
        }
    }
    
    SEPAR = separ;
}

void sim_AddStuckLevel(sim_model *sc, char *node, int level)
{
    int left, right, i, lbin;
    char separ = SEPAR;
 
    if (!sc) return;

    if (level!=0 && level!=1)
      {
        avt_errmsg(SIM_API_ERRMSG, "011", AVT_ERROR, level, node);
//        fprintf (stderr, "AddStuckLevel: bad value '%c' for node %s\n", level, node);
        return;
      }

    SEPAR = SIM_SEPAR;
    
    node=gen_sim_devect (node);

    lbin=vectorindex (node);
    
    if (lbin == -1) 
      vectorbounds (sc->FIG, node, &left, &right);
    else
      left=right=lbin;

    if (left==-1 || right==-1)
      { 
        if (level==0)
          sim_input_set_stuck_level (sc, node, SIM_ZERO);
        else 
          sim_input_set_stuck_level (sc, node, SIM_ONE);
      }
    else
      {
        if (left>right) { lbin=right; right=left; left=lbin; }
        node=vectorradical(node);
        for (i = left; i <= right; i++) 
          {
            if (level==0)
              sim_input_set_stuck_level (sc, sim_vectorize (node, i), SIM_ZERO);
            else 
              sim_input_set_stuck_level (sc, sim_vectorize (node, i), SIM_ONE);
          }
      }
    
    SEPAR = separ;
}
//_____________________________________________________________________________

void sim_AddStuckVoltage (sim_model *sc, char *node, double voltage)
{
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;
    
    sim_input_set_stuck_voltage ((sim_model*)sc, gen_sim_devect (node), voltage);

    SEPAR = separ;
}

//_____________________________________________________________________________
/*
void sim_AddStuckLevel (sim_model *sc, char *node, int level)
{
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;
    
    switch (level) {
        case 0: 
            sim_input_set_stuck_level ((sim_model*)sc, gen_sim_devect (node), SIM_ZERO);
            break;
        case 1: 
            sim_input_set_stuck_level ((sim_model*)sc, gen_sim_devect (node), SIM_ONE);
            break;
        default:
            fprintf (stderr, "[SIMAPI WAR] AddStuckLevel param 2: allowed values are 0 and 1\n");
    }

    SEPAR = separ;
}
*/
//#############################################################################
//#############################################################################
//_______________ PWL _________________________________________________________ 
//#############################################################################
//#############################################################################
//

//_____________________________________________________________________________

void sim_AddSignalSlope (sim_model *sc, char *node, double start_time, double transition_time, char sense)
{
    char *vnode = gen_sim_devect (node);
    char separ = SEPAR;
    SIM_FLOAT t_time = SIM_UNIT_X_TO_Y (transition_time, 1.0, SIM_UNIT_TIME);
    SIM_FLOAT s_time = SIM_UNIT_X_TO_Y (start_time, 1.0, SIM_UNIT_TIME);
    SIM_FLOAT *data = (SIM_FLOAT*)malloc (7 * sizeof (SIM_FLOAT)); 

//    FLOAT_TO_FREE = addchain (FLOAT_TO_FREE, data);

    if (!sc) return;
    SEPAR = SIM_SEPAR;

//    sim_new_model_if_null (((sim_model*)sc)->FIG);
    sense=toupper(sense);
    switch (sense) {
        case SIM_RISE:
            if (!sc->PARAMETER.PWL_FUNC)
                sim_input_set_slope_single ((sim_model*)sc, 
                                             vnode,
                                             SIM_RISE,
                                             t_time,
                                             s_time,
                                             SIM_INPUT_SIGNAL);
            else {
                data[0] = s_time;
                data[1] = t_time;
                data[2] = sim_parameter_get_slopeVTHH ((sim_model*)sc);
                data[3] = sim_parameter_get_slopeVTHL ((sim_model*)sc);
                data[4] = sim_parameter_get_alim ((sim_model*)sc, 'i');
                data[5] = 1.0;
                data[6] = sim_parameter_get_vss ((sim_model*)sc, 'i');
                sim_input_set_func ((sim_model*)sc, vnode, sc->PARAMETER.PWL_FUNC, data,SIM_INPUT_SIGNAL, NULL);
            }
            sim_measure_set_signal ((sim_model*)sc, vnode);
            break;
        case SIM_FALL:
            if (!sc->PARAMETER.PWL_FUNC)
                sim_input_set_slope_single ((sim_model*)sc, vnode, SIM_FALL,  t_time, s_time,SIM_INPUT_SIGNAL);
            else {
                data[0] = s_time;
                data[1] = t_time;
                data[2] = sim_parameter_get_slopeVTHH ((sim_model*)sc);
                data[3] = sim_parameter_get_slopeVTHL ((sim_model*)sc);
                data[4] = sim_parameter_get_alim ((sim_model*)sc, 'i');
                data[5] = -1.0;
                data[6] = sim_parameter_get_vss ((sim_model*)sc, 'i');
                sim_input_set_func ((sim_model*)sc, vnode, sc->PARAMETER.PWL_FUNC, data,SIM_INPUT_SIGNAL, NULL);
            }
            sim_measure_set_signal ((sim_model*)sc, vnode);
            break;
        default:
            avt_errmsg(SIM_API_ERRMSG, "013", AVT_ERROR);
//            fprintf (stderr, "[SIMAPI WAR] AddInputSlope param 4: allowed values are SIM_RISE and SIM_FALL\n");
    }
    SEPAR = separ;
}

//_____________________________________________________________________________

void sim_AddSlope (sim_model *sc, char *node, double start_time, double transition_time, char sense)
{
    char *vnode = gen_sim_devect (node);
    char separ = SEPAR;
    SIM_FLOAT t_time = SIM_UNIT_X_TO_Y (transition_time, 1.0, SIM_UNIT_TIME);
    SIM_FLOAT s_time = SIM_UNIT_X_TO_Y (start_time, 1.0, SIM_UNIT_TIME);
    SIM_FLOAT *data = (SIM_FLOAT*)malloc (7 * sizeof (SIM_FLOAT)); 

//    FLOAT_TO_FREE = addchain (FLOAT_TO_FREE, data);

    if (!sc) return;
    if (sim_find_locon(vnode, sc->FIG, NULL, NULL)==NULL)
      {
         avt_errmsg(SIM_API_ERRMSG, "008", AVT_ERROR, node);
         //avt_error("sim_api", 0, AVT_ERR, "Connector '%s' could not be found in netlist.\n", node);
        return;
      }
    SEPAR = SIM_SEPAR;

//    sim_new_model_if_null (((sim_model*)sc)->FIG);
    sense=toupper(sense);
    switch (sense) {
        case SIM_RISE:
            if (!sc->PARAMETER.PWL_FUNC)
                sim_input_set_slope_single ((sim_model*)sc, vnode, SIM_RISE, t_time, s_time,SIM_INPUT_LOCON);
            else {
                data[0] = s_time;
                data[1] = t_time;
                data[2] = sim_parameter_get_slopeVTHH ((sim_model*)sc);
                data[3] = sim_parameter_get_slopeVTHL ((sim_model*)sc);
                data[4] = sim_parameter_get_alim ((sim_model*)sc, 'i');
                data[5] = 1.0;
                data[6] = sim_parameter_get_vss ((sim_model*)sc, 'i');
                sim_input_set_func ((sim_model*)sc, vnode, sc->PARAMETER.PWL_FUNC, data,SIM_INPUT_LOCON, NULL);
            }
            break;
        case SIM_FALL:
            if (!sc->PARAMETER.PWL_FUNC)
                sim_input_set_slope_single ((sim_model*)sc, vnode, SIM_FALL,  t_time, s_time,SIM_INPUT_LOCON);
            else {
                data[0] = s_time;
                data[1] = t_time;
                data[2] = sim_parameter_get_slopeVTHH ((sim_model*)sc);
                data[3] = sim_parameter_get_slopeVTHL ((sim_model*)sc);
                data[4] = sim_parameter_get_alim ((sim_model*)sc, 'i');
                data[5] = -1.0;
                data[6] = sim_parameter_get_vss ((sim_model*)sc, 'i');
                sim_input_set_func ((sim_model*)sc, vnode, sc->PARAMETER.PWL_FUNC, data,SIM_INPUT_LOCON, NULL);
            }
            break;
        default:
            avt_errmsg(SIM_API_ERRMSG, "013", AVT_ERROR);
//            fprintf (stderr, "[SIMAPI WAR] AddInputSlope param 4: allowed values are SIM_RISE and SIM_FALL\n");
    }
    SEPAR = separ;
}

//_____________________________________________________________________________

void sim_AddOutLoad (sim_model *sc, char *node, double load)
{
    //char *vnode = gen_sim_devect (node);
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;
    
    //TODO sim_input_set_out_load ((sim_model*)sc, vnode, load);

    SEPAR = separ;
}

//_____________________________________________________________________________

void sim_AddWaveForm (sim_model *sc, char *node, double trise, double tfall, double periode, char *pattern)
{
    char *vnode = gen_sim_devect (node);
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;
    
    sim_input_set_slope_pattern ((sim_model*)sc, vnode, 
                                 SIM_UNIT_X_TO_Y (trise, 1.0, SIM_UNIT_TIME), 
                                 SIM_UNIT_X_TO_Y (tfall, 1.0, SIM_UNIT_TIME), 
                                 SIM_UNIT_X_TO_Y (periode, 1.0, SIM_UNIT_TIME), 
                                 pattern);
    if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE )
      sim_measure_set_locon ((sim_model*)sc, vnode);
    SEPAR = separ;
}

//#############################################################################
//#############################################################################
//_______________ Initial IC values ___________________________________________
//#############################################################################
//#############################################################################

void sim_add_init_level (sim_model *sc, char *node, char locate, int level)
{
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;
    
    switch (level) {
        case 0: 
            sim_ic_set_level (sc, gen_sim_devect (node), locate, SIM_ZERO);
            break;
        case 1: 
            sim_ic_set_level (sc, gen_sim_devect (node), locate, SIM_ONE);
            break;
        default:
            avt_errmsg(SIM_API_ERRMSG, "014", AVT_ERROR);
//            fprintf (stderr, "[SIMAPI WAR] AddInitLevel param 2: allowed values are 0 and 1\n");
    }

    SEPAR = separ;
}

//_____________________________________________________________________________

void sim_AddInitLevel (sim_model *sc, char *node, int level)
{
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;

    if (!sim_find_locon (gen_sim_devect (node), ((sim_model*)sc)->FIG,NULL,NULL))
        sim_add_init_level ((sim_model*)sc, node, SIM_IC_SIGNAL, level);
    else
        sim_add_init_level ((sim_model*)sc, node, SIM_IC_LOCON, level);
    
    SEPAR = separ;
}

//_____________________________________________________________________________

void sim_add_init_voltage (sim_model *sc, char *node, char locate, double voltage)
{
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;
    
    sim_ic_set_voltage (sc, gen_sim_devect (node), locate, voltage);

    SEPAR = separ;
}

//_____________________________________________________________________________

void sim_AddInitVoltage (sim_model *sc, char *node, double volt)
{
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;

    if (!sim_find_locon (gen_sim_devect (node), ((sim_model*)sc)->FIG,NULL,NULL))
        sim_add_init_voltage ((sim_model*)sc, node, SIM_IC_SIGNAL, volt);
    else
        sim_add_init_voltage ((sim_model*)sc, node, SIM_IC_LOCON, volt);

    SEPAR = separ;
}


//#############################################################################
//#############################################################################
//_______________ Mesure PRINT points _________________________________________
//#############################################################################
//#############################################################################

void sim_AddMeasure (sim_model *sc, char *node)
{
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;
    
    if (!sim_find_locon (gen_sim_devect (node), ((sim_model*)sc)->FIG,NULL,NULL))
        sim_measure_set_signal ((sim_model*)sc, gen_sim_devect (node));
    else
        sim_measure_set_locon ((sim_model*)sc, gen_sim_devect (node));
    SEPAR = separ;
}

//_____________________________________________________________________________

void sim_AddMeasureCurrent (sim_model *sc, char *node)
{
    char separ = SEPAR;
    if (!sc) return;
    SEPAR = SIM_SEPAR;
    
    sim_measure_current ((sim_model*)sc, gen_sim_devect (node));

    SEPAR = separ;
}


//#############################################################################
//#############################################################################
//_______________ Simulation primitives _______________________________________
//#############################################################################
//#############################################################################

void sim_RunSimulation (sim_model *sc, char *sim_call)
{
    char result;
    char separ = SEPAR;
    char *prev_sim_call;
    if (!sc) return;
    SEPAR = SIM_SEPAR;

    prev_sim_call = strdup (sim_parameter_get_tool_cmd ((sim_model*)sc));

    if (sim_call && strcmp(sim_call,"NULL")!=0)
        sim_parameter_set_tool_cmd ((sim_model*)sc, sim_call);

    result = sim_run_simu ((sim_model*)sc, SIM_RUN_ALL, NULL, NULL);
    READ_RESULT = 0;
    SEPAR = separ;
    sim_parameter_set_tool_cmd ((sim_model*)sc, prev_sim_call);
    free (prev_sim_call);
    sim_GetAllMeasure(sc);
    sim_freeMeasAllArg(sc);
}

//#############################################################################
//#############################################################################
//_______________ Computing primitives ________________________________________
//#############################################################################
//#############################################################################

double sim_ExtractMinSlope (sim_model *sc, char *node)
{
    double slope, vth_low, vth_high;
    char separ = SEPAR;
    char type_node;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    sim_read_tab_if_null ((sim_model*)sc);
    vth_high = sim_parameter_get_slopeVTHH ((sim_model*)sc);
    vth_low = sim_parameter_get_slopeVTHL ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_node = 's';
    else 
        type_node = 'c';
    slope = sim_getslope ((sim_model*)sc, SIM_MIN, type_node, gen_sim_devect (node));
    SEPAR = separ;
    return slope;
}

//_____________________________________________________________________________
//
double sim_ExtractMaxSlope (sim_model *sc, char *node)
{
    double slope, vth_low, vth_high;
    char separ = SEPAR;
    char type_node;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    sim_read_tab_if_null ((sim_model*)sc);
    vth_high = sim_parameter_get_slopeVTHH ((sim_model*)sc);
    vth_low = sim_parameter_get_slopeVTHL ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_node = 's';
    else 
        type_node = 'c';
    slope = sim_getslope ((sim_model*)sc, SIM_MAX, type_node, gen_sim_devect (node));
    SEPAR = separ;
    return slope;
}

//_____________________________________________________________________________

double sim_ExtractMinDelay (sim_model *sc, char *node_a, char *node_b)
{
    double delay;
    char separ = SEPAR;
    char type_a,type_b;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node_a), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_a = 's';
    else 
        type_a = 'c';
    if (!sim_find_locon (gen_sim_devect (node_b), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_b = 's';
    else 
        type_b = 'c';
    delay = sim_getdelay ((sim_model*)sc, SIM_MIN, type_a, gen_sim_devect (node_a), 
                          type_b, gen_sim_devect (node_b));
    SEPAR = separ;
    return delay;
}

//_____________________________________________________________________________

double sim_ExtractMaxDelay (sim_model *sc, char *node_a, char *node_b)
{
    double delay;
    char separ = SEPAR;
    char type_a,type_b;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node_a), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_a = 's';
    else 
        type_a = 'c';
    if (!sim_find_locon (gen_sim_devect (node_b), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_b = 's';
    else 
        type_b = 'c';
    delay = sim_getdelay ((sim_model*)sc, SIM_MAX, type_a, gen_sim_devect (node_a), 
                          type_b, gen_sim_devect (node_b));
    SEPAR = separ;
    return delay;
}

//_____________________________________________________________________________

void *sim_ExtractAllTimings (sim_model *sc, char *node_a, char *node_b)
{
    char separ = SEPAR;
    char type_a,type_b;
    sim_timing *tmglist = NULL;
    if (!sc) return NULL;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node_a), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_a = 's';
    else 
        type_a = 'c';
    if (!sim_find_locon (gen_sim_devect (node_b), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_b = 's';
    else 
        type_b = 'c';
    tmglist = sim_GetTmgList ((sim_model*)sc, type_a,gen_sim_devect (node_a), 
                              type_b, gen_sim_devect (node_b));
    SEPAR = separ;
    return (sim_timing*)tmglist;
}

//_____________________________________________________________________________

double sim_ExtractMinTransitionDelay (sim_model *sc, char *node_a, char *node_b, char *transition)
{
    double delay, th_in, th_out;
    char separ = SEPAR;
    char type_a;
    char type_b;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node_a), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type_a = 's';
    else 
        type_a = 'c';
    if (!sim_find_locon (gen_sim_devect (node_b), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type_b = 's';
    else 
        type_b = 'c';
    sim_get_THR(sc, &th_in, &th_out, NULL, NULL);
    delay = sim_get_trans_delay ((sim_model*)sc, SIM_MIN, type_a, gen_sim_devect (node_a),
                                 type_b, gen_sim_devect (node_b), transition, th_in, th_out);
    SEPAR = separ;
    return delay;
}

//_____________________________________________________________________________

double sim_ExtractMaxTransitionDelay (sim_model *sc, char *node_a, char *node_b, char *transition)
{
    double delay, th_in, th_out;
    char separ = SEPAR;
    char type_a;
    char type_b;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node_a), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type_a = 's';
    else 
        type_a = 'c';
    if (!sim_find_locon (gen_sim_devect (node_b), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type_b = 's';
    else 
        type_b = 'c';
    sim_get_THR(sc, &th_in, &th_out, NULL, NULL);
    delay = sim_get_trans_delay ((sim_model*)sc, SIM_MAX, type_a, gen_sim_devect (node_a),
                                 type_b, gen_sim_devect (node_b), transition, th_in, th_out);
    SEPAR = separ;
    return delay;
}

//_____________________________________________________________________________

double sim_ExtractMinTransitionSlope (sim_model *sc, char *node, char *transition)
{
    double slope, slopelow, slopehigh;
    char separ = SEPAR;
    char type;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type = 's';
    else 
        type = 'c';
    sim_get_THR(sc, NULL, NULL, &slopelow, &slopehigh);
    slope = sim_get_trans_slope ((sim_model*)sc, SIM_MIN, type, gen_sim_devect (node),transition, slopelow, slopehigh);
    SEPAR = separ;
    return slope;
}

//_____________________________________________________________________________

double sim_ExtractMaxTransitionSlope (sim_model *sc, char *node, char *transition)
{
    double slope, slopelow, slopehigh;
    char separ = SEPAR;
    char type;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type = 's';
    else 
        type = 'c';
    sim_get_THR(sc, NULL, NULL, &slopelow, &slopehigh);
    slope = sim_get_trans_slope ((sim_model*)sc, SIM_MAX, type, gen_sim_devect (node),transition, slopelow, slopehigh);
    SEPAR = separ;
    return slope;
}

//__________ Timing API  ______________________________________________________

void *sim_GetTimingFromList (chain_list *list)
{
    if (list) 
        return ((chain_list*)list)->DATA;
    else
        return NULL;
}

//_____________________________________________________________________________

void *sim_GetTimingNext (void *timing)
{
    if (timing) 
        return ((sim_timing*)timing)->NEXT;
    else
        return NULL;
}

//_____________________________________________________________________________

void *sim_GetTiming (char *root, char *node)
{
    if (root && node) 
        return sim_timing_get (root,node);
    else
        return NULL;
}

//_____________________________________________________________________________

void *sim_GetTimingByEvent (char *root, char *node, char *nodeevent)
{
    if (root && node) 
        return sim_timing_get_by_event (root,node,nodeevent);
    else
        return NULL;
}

//_____________________________________________________________________________

double sim_GetTimingDelay (void *timing)
{
    if (timing) 
        return ((sim_timing*)timing)->DELAY;
    else
        return 0.0;
}

//_____________________________________________________________________________

double sim_GetTimingMinDelay (void *timing)
{
    if (timing) 
        return sim_timing_getdelaytype((sim_timing*)timing,SIM_MIN);
    else
        return 0.0;
}

//_____________________________________________________________________________

double sim_GetTimingMaxDelay (void *timing)
{
    if (timing) 
        return sim_timing_getdelaytype((sim_timing*)timing,SIM_MAX);
    else
        return 0.0;
}

//_____________________________________________________________________________

double sim_GetTimingSlope (void *timing)
{
    if (timing) 
        return ((sim_timing*)timing)->SLOPE;
    else
        return 0.0;
}

//_____________________________________________________________________________

double sim_GetTimingMinSlope (void *timing)
{
    if (timing) 
        return sim_timing_getslopetype((sim_timing*)timing,SIM_MIN);
    else
        return 0.0;
}

//_____________________________________________________________________________

double sim_GetTimingMaxSlope (void *timing)
{
    if (timing) 
        return sim_timing_getslopetype((sim_timing*)timing,SIM_MAX);
    else
        return 0.0;
}

//_____________________________________________________________________________

char *sim_GetTimingRoot (void *timing)
{
    if (timing) 
        return ((sim_timing*)timing)->ROOT4USR;
    else
        return NULL;
}

//_____________________________________________________________________________

char *sim_GetTimingNode (void *timing)
{
    if (timing) 
        return ((sim_timing*)timing)->NODE4USR;
    else
        return NULL;
}

//_____________________________________________________________________________

char *sim_GetTimingRootInNetlist (void *timing)
{
    if (timing) 
        return ((sim_timing*)timing)->ROOT;
    else
        return NULL;
}

//_____________________________________________________________________________

char *sim_GetTimingNodeInNetlist (void *timing)
{
    if (timing) 
        return ((sim_timing*)timing)->NODE;
    else
        return NULL;
}

//_____________________________________________________________________________

char sim_GetTimingRootEvent (void *timing)
{
    if (timing) 
        return ((sim_timing*)timing)->ROOTEVENT;
    else
        return 0;
}

//_____________________________________________________________________________

char sim_GetTimingNodeEvent (void *timing)
{
    if (timing) 
        return ((sim_timing*)timing)->NODEEVENT;
    else
        return 0;
}

//______ Noise API ____________________________________________________________

void sim_NoiseExtract (sim_model *sc, char *node, double vthnoise, double tinit, double tfinal)
{
    char separ = SEPAR;
    char type;
    if (!sc) return;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type = 's';
    else 
        type = 'c';
    sim_noise_extract ((sim_model*)sc, type, gen_sim_devect (node),vthnoise,tinit,tfinal);
    SEPAR = separ;
}

//_____________________________________________________________________________

double sim_NoiseGetVth (sim_model *sc, char *name)
{
    char separ = SEPAR;
    char type,noisetype;
    double vthnoise = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (name), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type = 's';
    else 
        type = 'c';
    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    vthnoise = sim_noise_get_vth (sim_noise_get (gen_sim_devect (name),noisetype));
    SEPAR = separ;

    return vthnoise;
}

//_____________________________________________________________________________

chain_list *sim_NoiseGetMomentList (sim_model *sc, char *name)
{
    char separ = SEPAR;
    char type,noisetype;
    chain_list *tclst = NULL;
    if (!sc) return NULL;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (name), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type = 's';
    else 
        type = 'c';
    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    tclst = sim_noise_get_idxc (sim_noise_get (gen_sim_devect (name),noisetype));
    SEPAR = separ;

    return tclst;
}

//_____________________________________________________________________________

double sim_NoiseGetMoment (sim_model *sc, chain_list *noise_tclist)
{
    char separ = SEPAR;
    double      res = 0.0;
    chain_list *tc;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    if (noise_tclist) {
        tc = (chain_list*)noise_tclist;
        res = sim_noise_get_time_byindex ((sim_model*)sc,(int)(long)tc->DATA);
    }
    SEPAR = separ;

    return res;
}

//_____________________________________________________________________________

double sim_NoiseGetPeakValue (sim_model *sc, char *name,chain_list *noise_peaklist)
{
    char separ = SEPAR;
    char noisetype;
    double      res = 0.0;
    chain_list *peak;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    if (noise_peaklist) {
        peak = (chain_list*)noise_peaklist;
        noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
        res = sim_noise_get_volt_byindex (sim_noise_get(gen_sim_devect(name),noisetype),
                                          (int)(long)peak->DATA);
    }
    SEPAR = separ;

    return res;
}

//_____________________________________________________________________________

double sim_NoiseGetPeakMoment (sim_model *sc, chain_list *noise_peaklist)
{
    char separ = SEPAR;
    double      res = 0.0;
    chain_list *peak;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    if (noise_peaklist) {
        peak = (chain_list*)noise_peaklist;
        res = sim_noise_get_time_byindex ((sim_model*)sc,(int)(long)peak->DATA);
    }
    SEPAR = separ;

    return res;
}

//_____________________________________________________________________________

chain_list *sim_NoiseGetPeakList (sim_model *sc, char *name)
{
    char separ = SEPAR;
    char type,noisetype;
    chain_list *peaklst = NULL;
    if (!sc) return NULL;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (name), sim_model_get_lofig ((sim_model*)sc), NULL, NULL))
        type = 's';
    else 
        type = 'c';
    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    peaklst = sim_noise_get_idxpeak (sim_noise_get (gen_sim_devect (name),noisetype));
    SEPAR = separ;

    return peaklst;
}

//_____________________________________________________________________________

double sim_NoiseExtractMaxPeakValue (sim_model *sc, char *name)
{
    char noisetype;
    char separ = SEPAR;
    double      maxpeak = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    maxpeak = sim_noise_get_max_peak_value (sim_noise_get (gen_sim_devect(name),noisetype));
    SEPAR = separ;

    return maxpeak;
}

//_____________________________________________________________________________

double sim_NoiseExtractMinPeakValue (sim_model *sc, char *name)
{
    char noisetype;
    char separ = SEPAR;
    double      minpeak = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    minpeak = sim_noise_get_min_peak_value (sim_noise_get (gen_sim_devect(name),noisetype));
    SEPAR = separ;

    return minpeak;
}

//_____________________________________________________________________________

double sim_NoiseExtractMaxPeakMoment (sim_model *sc, char *name)
{
    char noisetype;
    char separ = SEPAR;
    double      maxpeak_time = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    maxpeak_time = sim_noise_get_max_peak_time ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),noisetype));
    SEPAR = separ;

    return maxpeak_time;
}

//_____________________________________________________________________________

double sim_NoiseExtractMinPeakMoment (sim_model *sc, char *name)
{
    char noisetype;
    char separ = SEPAR;
    double      minpeak_time = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    minpeak_time = sim_noise_get_min_peak_time ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),noisetype));
    SEPAR = separ;

    return minpeak_time;
}

//_____________________________________________________________________________

double sim_NoiseGetMomentBeforePeak (sim_model *sc, char *name,chain_list *peakl)
{
    chain_list *peak = (chain_list*)peakl;
    char noisetype;
    char separ = SEPAR;
    double      beforepeakintant = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    beforepeakintant = sim_noise_get_time_before_peak ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),
                                                       noisetype),(int)(long)peak->DATA);
    SEPAR = separ;

    return beforepeakintant;
}

//_____________________________________________________________________________

double sim_NoiseGetMomentAfterPeak (sim_model *sc, char *name,chain_list *peakl)
{
    chain_list *peak = (chain_list*)peakl;
    char noisetype;
    char separ = SEPAR;
    double      afterpeakintant = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    afterpeakintant = sim_noise_get_time_after_peak ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),
                                                     noisetype),(int)(long)peak->DATA);
    SEPAR = separ;

    return afterpeakintant;
}

//_____________________________________________________________________________

double sim_NoiseGetPeakDuration (sim_model *sc, char *name,chain_list *peakl)
{
    chain_list *peak = (chain_list*)peakl;
    char noisetype;
    char separ = SEPAR;
    double      peakduration = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    noisetype = sim_parameter_get_noise_type ((sim_model*)sc);
    peakduration = sim_noise_get_peak_duration ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),
                                                noisetype),(int)(long)peak->DATA);
    SEPAR = separ;

    return peakduration;
}

//_____________________________________________________________________________

double sim_NoiseExtractMomentBeforeMaxPeak (sim_model *sc, char *name)
{
    char   separ = SEPAR;
    double instant = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    instant = sim_noise_get_max_ci_before_peak ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),
                                                SIM_MAX));
    SEPAR = separ;

    return instant;
}

//_____________________________________________________________________________

double sim_NoiseExtractMomentBeforeMinPeak (sim_model *sc, char *name)
{
    char   separ = SEPAR;
    double instant = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    instant = sim_noise_get_min_ci_before_peak ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),
                                                SIM_MIN));
    SEPAR = separ;

    return instant;
}

//_____________________________________________________________________________

double sim_NoiseExtractMomentAfterMaxPeak (sim_model *sc, char *name)
{
    char   separ = SEPAR;
    double instant = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    instant = sim_noise_get_max_ci_after_peak ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),
                                               SIM_MAX));
    SEPAR = separ;

    return instant;
}

//_____________________________________________________________________________

double sim_NoiseExtractMomentAfterMinPeak (sim_model *sc, char *name)
{
    char   separ = SEPAR;
    double instant = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    instant = sim_noise_get_min_ci_after_peak ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),
                                               SIM_MIN));
    SEPAR = separ;

    return instant;
}

//_____________________________________________________________________________

double sim_NoiseExtractMaxPeakDuration (sim_model *sc, char *name)
{
    char   separ = SEPAR;
    double duration = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    duration = sim_noise_get_max_peak_duration ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),
                                                SIM_MAX));
    SEPAR = separ;

    return duration;
}

//_____________________________________________________________________________

double sim_NoiseExtractMinPeakDuration (sim_model *sc, char *name)
{
    char   separ = SEPAR;
    double duration = 0.0;
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    duration = sim_noise_get_min_peak_duration ((sim_model*)sc,sim_noise_get (gen_sim_devect(name),
                                                SIM_MIN));
    SEPAR = separ;

    return duration;
}

//_____________________________________________________________________________

void sim_DriveNodeState (sim_model *sc, char *filename,char *node_ref,char *node_state2drive,char type)
{
    char type_a,type_b;
    char separ = SEPAR;

    if (!sc) return ;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node_ref), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_a = 'S';
    else 
        type_a = 'C';
    if (!sim_find_locon (gen_sim_devect (node_state2drive), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_b = 'S';
    else 
        type_b = 'C';

    sim_get_nodes_events ((sim_model*)sc,filename,node_ref,type_a,node_state2drive,type_b,type);

    SEPAR = separ;
}

//_____________________________________________________________________________

/*void sim_DriveFile2Plot (sim_model *sc, chain_list *name2plot)
{
    char separ = SEPAR;

    if (!sc || !name2plot) return ;
    SEPAR = SIM_SEPAR;
    
    sim_read_tab_if_null (sc);

    sim_drive_plot_file (sc, NULL, name2plot);

    SEPAR = separ;
}*/

//_____________________________________________________________________________
//
double sim_ExtractCommutInstant (sim_model *sc, char *node,double voltage)
{
    double instant;
    char separ = SEPAR;
    char type_node;

    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    sim_read_tab_if_null ((sim_model*)sc);
    if (!sim_find_locon (gen_sim_devect (node), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_node = 's';
    else 
        type_node = 'c';
    instant = sim_get_commut_instant ((sim_model*)sc, gen_sim_devect (node), type_node,voltage);
    SEPAR = separ;
    return instant;
}

void sim_AddAlias(sim_model *sc, char *source, char *destination)
{
  sim_input_add_alias(sc, namealloc(gen_makesignalname(source)), namealloc(gen_makesignalname(destination)));
}

void sim_RenameModel(sim_model *sc, char *name)
{
    sc->FIGNAME=namealloc(name);
    sc->FIG->NAME=sc->FIGNAME;
}

void sim_SetExternalCapacitance(sim_model *sc, char *node, double value)
{
  sim_set_external_capa(sc, node, value);
}

sim_model *sim_CreateNetlistContext()
{
  sim_model *sc;
  lofig_list *lf;

  lf=gns_GetNetlist();
  if (gns_IsBlackBox())
  {
     mbkContext *curctx;
     lofig_list *flatbb;
     loins_list *ptins;
     curctx=mbkCreateContext();
     lf=gns_GetBlackboxNetlist(lf->NAME);
     transfert_needed_lofigs(lf, curctx);
     flatbb=rduplofig(lf);
     lofigchain(flatbb);
     if (CUR_CORRESP_TABLE!=NULL)
     {
       ptins=mbk_quickly_getloinsbyname(LATEST_GNS_RUN->GLOBAL_LOFIG, CUR_CORRESP_TABLE->GENIUS_INSNAME);
       if (ptins!=NULL)
         mbk_transfert_loins_params(ptins, lf, flatbb);
     }
     mbkSwitchContext(curctx);
     flatbb=flatOutsideList(flatbb);
     mbkFreeAndSwitchContext(curctx);
     lf=flatbb;
  }
  else
    lf=gns_FlattenNetlist(lf, INTERNAL_RC|OUT_RC);
  sc=sim_CreateContext(lf);
  return sc;
}

lofig_list *sim_GetContextNetlist(sim_model *sc)
{
  if (sc==NULL) return NULL;
  return sc->FIG;
}
