#include GEN_H
#include SIM_H
#include STM_H
#include TTV_H

#define API_USE_REAL_TYPES
#include "gen_API.h"
#include "sim_API.h"
#include "stm_API.h"
#include "sim_API_lowlevel.h"


//_____________________________________________________________________________
//
double sim_ComputeSetup (sim_model *sc, char *data, double tstart_d,
                         double tslope_d, char sens_d, char *cmd,
                         double t_start_min_c, double t_start_max_c, double tslope_c,
                         char sens_c, char *mem, int data_val)
{
    double setup;
    char separ = SEPAR;
    char sens_data;
    char sens_cmd;

    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;
    
    sens_d=tolower(sens_d);
    if (sens_d == 'u') 
        sens_data = SIM_RISE;
    else if (sens_d == 'd')
        sens_data = SIM_FALL;
    else {
        avt_errmsg(SIM_API_ERRMSG, "005", AVT_ERROR);
//        avt_error("sim_api", -1, AVT_ERR, "sim_ComputeSetup: sens_d must be 'U' or 'D'\n");
        return 0.0;
    }
    sens_c=tolower(sens_c);
    if (sens_c == 'u') 
        sens_cmd = SIM_RISE;
    else if (sens_c == 'd')
        sens_cmd = SIM_FALL;
    else {
        avt_errmsg(SIM_API_ERRMSG, "005", AVT_ERROR);
//        avt_error("sim_api", -1, AVT_ERR, "sim_ComputeSetup: sens_d must be 'U' or 'D'\n");
        return 0.0;
    }
    setup = sim_get_setup_time ((sim_model*)sc,gen_sim_devect (data),
                                SIM_UNIT_X_TO_Y (tstart_d,1,SIM_UNIT_TIME),
                                SIM_UNIT_X_TO_Y (tslope_d,1,SIM_UNIT_TIME),
                                sens_data,gen_sim_devect (cmd),
                                SIM_UNIT_X_TO_Y (t_start_min_c,1,SIM_UNIT_TIME),
                                SIM_UNIT_X_TO_Y (t_start_max_c,1,SIM_UNIT_TIME),
                                SIM_UNIT_X_TO_Y (tslope_c,1,SIM_UNIT_TIME),
                                sens_c,gen_sim_devect (mem),data_val);
    SEPAR = separ;
    return setup;
}

//_____________________________________________________________________________

double sim_ComputeHold  (sim_model *sc, char *data, double tstart_d,
                         double tslope_d, char sens_d, char *cmd,
                         double t_start_min_c, double t_start_max_c, double tslope_c,
                         char sens_c, char *mem, int data_val)
{
    double hold;
    char separ = SEPAR;
    char sens_data;
    char sens_cmd;

    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;

    sens_d=tolower(sens_d);
    if (sens_d == 'u') 
        sens_data = SIM_RISE;
    else if (sens_d == 'd')
        sens_data = SIM_FALL;
    else {
        avt_errmsg(SIM_API_ERRMSG, "006", AVT_ERROR);
//        avt_error("sim_api", -1, AVT_ERR, "sim_ComputeHold: sens_d must be 'U' or 'D'\n");
        return 0.0;
    }
    sens_c=tolower(sens_c);
    if (sens_c == 'u') 
        sens_cmd = SIM_RISE;
    else if (sens_c == 'd')
        sens_cmd = SIM_FALL;
    else {
        avt_errmsg(SIM_API_ERRMSG, "006", AVT_ERROR);
//      avt_error("sim_api", -1, AVT_ERR, "sim_ComputeHold: sens_d must be 'U' or 'D'\n");
      return 0.0;
    }
    hold  = sim_get_hold_time ((sim_model*)sc,gen_sim_devect (data),
                               SIM_UNIT_X_TO_Y (tstart_d,1,SIM_UNIT_TIME),
                               SIM_UNIT_X_TO_Y (tslope_d,1,SIM_UNIT_TIME),
                               sens_data,gen_sim_devect (cmd),
                               SIM_UNIT_X_TO_Y (t_start_min_c,1,SIM_UNIT_TIME),
                               SIM_UNIT_X_TO_Y (t_start_max_c,1,SIM_UNIT_TIME),
                               SIM_UNIT_X_TO_Y (tslope_c,1,SIM_UNIT_TIME),
                               sens_c,gen_sim_devect (mem),data_val);
    SEPAR = separ;
    return hold;
}

//_____________________________________________________________________________

double sim_ComputeAccess (sim_model *sc, char *dout, int dout_val, char *cmd, double tstart_c,
                          double tslope_c, char sens_c, char *mem, int mem_val, double *out_slope)
{
    double access;
    char separ = SEPAR;
    char sens_cmd;
    SEPAR = SIM_SEPAR;
    
    if (!sc) return 0.0;
    sens_c=tolower(sens_c);
    if (sens_c == 'u') 
        sens_cmd = SIM_RISE;
    else if (sens_c == 'd')
        sens_cmd = SIM_FALL;
    else {
      avt_errmsg(SIM_API_ERRMSG, "007", AVT_ERROR);
//      avt_error("sim_api", -1, AVT_ERR, "sim_ComputeAccess: sens_c must be 'U' or 'D'\n");
      return 0.0;
    }
    access = sim_get_access_time ((sim_model*)sc,gen_sim_devect (dout),dout_val,gen_sim_devect (cmd),
                                  SIM_UNIT_X_TO_Y (tstart_c,1,SIM_UNIT_TIME),
                                  SIM_UNIT_X_TO_Y (tslope_c,1,SIM_UNIT_TIME),
                                  sens_cmd,gen_sim_devect (mem),mem_val, out_slope);
    SEPAR = separ;
    return access;
}

//_____________________________________________________________________________

chain_list *sim_ComputeDelay (sim_model *sc, char *input, char sens, chain_list *list_measure)
{
    char separ = SEPAR;
    char type_node_dest;
    char *name;
    double slope;
    chain_list *chain;

    if (!sc) return NULL;
    SEPAR = SIM_SEPAR;
    
    slope = sim_parameter_get_slope( (sim_model*)sc ); 
    // positionne le front d entree
    sim_AddSlope (sc, gen_sim_devect(input),0.0,slope,sens);
    
    // ajoute les mesures sur les points de sortie
    for (chain = (chain_list*)list_measure ; chain ; chain = chain->NEXT) {
        name = (char*)chain->DATA;
        sim_AddMeasure (sc, gen_sim_devect(name));
    }

    // Launch simulation
    sim_RunSimulation (sc, NULL);
    sim_parse_spiceout ((sim_model*)sc);

    // Recupere les delais
    for (chain = (chain_list*)list_measure ; chain ; chain = chain->NEXT) {
        name = (char*)chain->DATA;
        if (!sim_find_locon (gen_sim_devect (name), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
            type_node_dest = 's';
        else 
            type_node_dest = 'c';
        sim_GetTmgList ((sim_model*)sc,'c',gen_sim_devect(input),type_node_dest,gen_sim_devect(name));
    }
    SEPAR = separ;
    return (chain_list*)SIM_HEAD_TMG;
}

//_____________________________________________________________________________

double sim_compute_delay_transition (sim_model *sc, char dtype,char *input, double input_start,double slope,char *output, char *event)
{
    char separ = SEPAR;
    char type_node_dest;
    char in_event;
    char out_event;
    long in_ind;
    char *endstr;
    int  init_output;
    double th_in, th_out;
    
    if (!sc) return 0.0;
    SEPAR = SIM_SEPAR;
    
    in_event  = event[0];
    in_ind    = strtol(&event[1],&endstr,0);
    out_event = endstr[0];
    if (out_event == SIM_FALL)
        init_output = 1;
    else
        init_output = 0;

    input_start = SIM_UNIT_X_TO_Y (input_start, 1.0, SIM_UNIT_TIME);
    // positionne le front d entree
    sim_AddSlope (sc, gen_sim_devect(input),input_start,slope,in_event);

    // ajoute les mesures sur les points de sortie
    sim_AddMeasure (sc, gen_sim_devect(output));

    // initialise la sortie
    sim_AddInitLevel (sc, gen_sim_devect(output),init_output);

    // Launch simulation
    sim_RunSimulation (sc, NULL);
    sim_parse_spiceout ((sim_model*)sc);

    if (!sim_find_locon (gen_sim_devect (output), sim_model_get_lofig((sim_model*)sc),NULL,NULL))
        type_node_dest = 's';
    else 
        type_node_dest = 'c';
    SEPAR = separ;

    sim_get_THR(sc, &th_in, &th_out, NULL, NULL);

    // Retourne le delai
    return sim_get_trans_delay ((sim_model*)sc, 
                                dtype,
                                'c',
                                gen_sim_devect(input), 
                                type_node_dest,
                                gen_sim_devect(output),
                                event, th_in, th_out);
}

//_____________________________________________________________________________

double sim_ComputeMaxDelayTransition (sim_model *sc, char *input, double input_start,double input_slope,char *output, char *event)
{
    return sim_compute_delay_transition (sc, SIM_MAX,input,input_start,input_slope,output,event);
}

//_____________________________________________________________________________

double sim_ComputeMinDelayTransition (sim_model *sc, char *input, double input_start,double input_slope,char *output, char *event)
{
    return sim_compute_delay_transition (sc, SIM_MIN,input,input_start,input_slope,output,event);
}

//_____________________________________________________________________________
void SET_CONTEXT(sim_model *sc)
{
  sim_set_ctx(sc);
}

sim_model *GET_CONTEXT()
{
  return sim_get_ctx();
}

void sim_DefineInclude(sim_model *sc, char *filename)
{
  sim_set_filename(sc, filename);
}
