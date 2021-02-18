#include "avt_API_types.h"

#define Timing void
#define Noise void
#define PwlFunction void

#define SIM_FALL  'D'
#define SIM_RISE  'U'
#define SIM_STATE 'S'

#define SIM_RAMP 'r'
#define SIM_TANH 't'

#define SIM_MIN 'm'
#define SIM_MAX 'M'
#define SIM_ALL 'A'

void sim_API_Action_Initialize ();
void sim_API_Action_Terminate ();
void sim_SetSlopeForm (SimulationContext *sc, PwlFunction *f);

//*************************************
// General parameters
//*************************************

/*
  MAN sim_SetSimulatorType
  CATEG gns
  DESCRIPTION
  sim_SetSimulatorType specifies the type of simulator towards whom the simulation
  netlists are created. Supported simulators types are 'NGSPICE' and 'ELDO'. 
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void sim_SetSimulatorType (SimulationContext *sc, char *type);

/*
  MAN sim_CreateContext
  CATEG gns
  DESCRIPTION
  sim_CreateContext creates a simulation context, relatively to a netlist. Electrical simulations directives are applied to this netlist and
  stored in the newly created simulation context.
  RETURN VALUE
  Pointer on the newly created simulation context.
  ERRORS
  none.
*/
extern SimulationContext *sim_CreateContext (Netlist *netlist);

/*
  MAN sim_CreateNetlistContext
  CATEG gns
  DESCRIPTION
  sim_CreateNetlistContext creates a simulation context, relatively to the current gns netlist which is flattened to transistor level. 
  RETURN VALUE
  Pointer on the newly created simulation context.
  ERRORS
  none.
*/
SimulationContext *sim_CreateNetlistContext();

/*
  MAN sim_GetContextNetlist
  CATEG gns
  DESCRIPTION
  sim_GetContextNetlist returns the netlist the simulation context {sc} is associated to. 
*/
Netlist *sim_GetContextNetlist(SimulationContext *sc);

/*
  MAN sim_CreateContext
  CATEG gns
  DESCRIPTION
  sim_FreeContext deletes a previously created simulation context. The netlist associated with the simulation context is not affected.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
void sim_FreeContext (SimulationContext *model);

/*
  MAN sim_SetDelayVTH
  CATEG gns
  DESCRIPTION
  sim_SetDelayVTH sets the threshold voltage for delay calculations. Delay is computed
  between the vth-crossing ins
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_SetDelayVTH    (SimulationContext *sc, double vth); 

/*
  MAN Simulation_SetSlopeVTH
  CATEG gns
  DESCRIPTION
  sim_SetSlopeVTH specifies the bounds of the slope (percentage of vdd). 
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_SetSlopeVTH   (SimulationContext *sc, double vth_low, double vth_high);

/*
  MAN sim_SetSimulationSlope
  CATEG gns
  DESCRIPTION
  sim_SetSimulationSlope sets the default input slope for electrical simulation.
  The unit is the second.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_SetSimulationSlope       (SimulationContext *sc, double slope);

/*
  MAN sim_SetSimulationTime
  CATEG gns
  DESCRIPTION
  sim_SetSimulationTime sets the duration of the electrical simulation.
  Unit is SECOND.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_SetSimulationTime (SimulationContext *sc, double time);

/*
  MAN sim_SetSimulationStep
  CATEG gns
  DESCRIPTION
  sim_SetSimulationStep sets the step used during the electrical simulation.
  The unit is the second.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_SetSimulationStep (SimulationContext *sc, double step);

/*
  MAN sim_SetSimulationSupply
  CATEG gns
  DESCRIPTION
  sim_SetSimulationSupply sets the applied power supply during the electrical simulation.
  The unit is the volt.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_SetSimulationSupply (SimulationContext *sc, double v_max);

/*
  MAN sim_SetInputSwing
  CATEG gns
  DESCRIPTION
  sets the swing to use for input connector/signal. Those values are used for instance when setting an input slope on a connector.
  RETURN VALUE
  none.
*/
void sim_SetInputSwing (SimulationContext *sc, double v_vss, double v_max);

/*
  MAN sim_SetOutputSwing
  CATEG gns
  DESCRIPTION
  sets the swing to use for output signal when computing a delay to the output signal or the slope on an output signal.
  RETURN VALUE
  none.
*/
void sim_SetOutputSwing (SimulationContext *sc, double v_vss, double v_max);

/*
  MAN sim_GetSimulationSupply
  CATEG gns
  DESCRIPTION
  sim_GetSimulationSupply returns the simulation voltage defined for the alimentation connector.
  The unit is the volt.
  RETURN VALUE
  returns a double.
  ERRORS
  none.
*/
extern double sim_GetSimulationSupply();

/*
  MAN simn_SetSimulationTemp
  CATEG gns
  DESCRIPTION
  sim_SetSimulationTemp sets the applied temperature during the electrical simulation.
  The unit is the celsius degree.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_SetSimulationTemp (SimulationContext *sc, double temp);

/*
  MAN sim_AddSimulationTechnoFile
  CATEG gns
  DESCRIPTION
  sim_AddSimulationTechnoFile adds a technology file in list of technology files used to 
  parametrize the electrical simulation.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddSimulationTechnoFile (SimulationContext *sc, char *tech_file);

/*
  MAN sim_SetSimulationCall
  CATEG gns
  DESCRIPTION
  sim_SetSimulationCall sets the string which will be called to run the electrical simulator.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_SetSimulationCall (SimulationContext *sc, char *sim_call);

/*
  MAN sim_NoiseSetAnalyseType
  CATEG gns
  DESCRIPTION
  sim_NoiseSetAnalyseType sets the type of noise analysis. Allowed values are SIM_MIN and SIM_MAX.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_NoiseSetAnalyseType (SimulationContext *sc, char noise_type);

/*
  MAN sim_SetSimulationOutputFile
  CATEG gns
  DESCRIPTION
  sim_SetSimulationOutputFile specifies the extension of the file generated by the electrical simulator.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_SetSimulationOutputFile (SimulationContext *sc, char *output_file);

//*************************************
// Input PWL - DC values 
//*************************************

/*
  MAN sim_AddStuckLevel
  CATEG gns
  DESCRIPTION
  sim_AddStuckLevel stucks the node to VDD if level is 1, to GND if level is 0.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddStuckLevel (SimulationContext *sc, char *node, int level);

/*
  MAN sim_AddStuckLevelVector
  CATEG gns
  DESCRIPTION
  sim_AddStuckLevelVector stucks the input bit vector to the hexadecimal value (VDD
  if the bit value is 1, GND if the bit value is 0).
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddStuckLevelVector (SimulationContext *sc, char *node, char *level);

/*
  MAN sim_AddStuckVoltage
  CATEG gns
  DESCRIPTION
  sim_AddStuckVoltage stucks the input to the value.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddStuckVoltage     (SimulationContext *sc, char *node, double voltage);

/*
  MAN sim_AddSlope
  CATEG gns
  DESCRIPTION
  sim_AddSlope sets a rising slope on the input if sense is 'U', a falling slope
  if sense is 'D'.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddSlope (SimulationContext *sc, char *node, double start_time, double transition_time, char sense);

/*
  MAN sim_AddSlope
  CATEG gns
  DESCRIPTION
  sim_AddSlope sets a rising slope on the input (an internal signal) if sense is 'U', a falling slope
  if sense is 'D'.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddSignalSlope (SimulationContext *sc, char *node, double start_time, double transition_time, char sense);

PwlFunction *sim_builtin_tanh ();
PwlFunction *sim_builtin_ramp ();

/*
  MAN sim_SetExternalCapacitance
  CATEG gns
  DESCRIPTION
  sim_SetExternalCapacitance sets a capacitance {value} on the toplevel connector {node}.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void sim_SetExternalCapacitance(SimulationContext *sc, char *node, double value);
        

/*
  MAN sim_AddWaveForm
  CATEG gns
  DESCRIPTION
  sim_AddWaveForm sets rising and falling transitions on the node, according to the string 'pattern'.
  Specifying 1 in 'pattern' sets rising slope, 0 a falling slope.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddWaveForm (SimulationContext *sc, char *node, double trise, double tfall, double periode, char *pattern);
                                
//*************************************
// Initial IC values
//*************************************
/*
  MAN sim_AddInitLevel
  CATEG gns
  DESCRIPTION
  sim_AddInitLevel initialize a node voltage to VDD if level = 1, to GND if level = 0.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddInitLevel        (SimulationContext *sc, char *node, int level);

/*
  MAN sim_AddInitVoltage
  CATEG gns
  DESCRIPTION
  sim_AddInitVoltage initialize a node voltage to the value voltage.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddInitVoltage     (SimulationContext *sc, char *node, double voltage);


/*
  MAN sim_AddOutLoad
  CATEG gns
  DESCRIPTION
  sim_AddOutLoad adds the capacitance 'load' on the specified 'node' output connector.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
void sim_AddOutLoad (SimulationContext *sc, char *node, double load);

//*************************************
// Mesure PRINT points 
//*************************************
/*
  MAN sim_AddMeasure
  CATEG gns
  DESCRIPTION
  sim_AddMeasure prints the signal voltage to the simulator output file.
  This function is needed to compute timing.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddMeasure (SimulationContext *sc, char *node);

/*
  MAN sim_AddMeasureCurrent
  CATEG gns
  DESCRIPTION
  sim_AddMeasureCurrent prints the node current to the simulator output file.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_AddMeasureCurrent   (SimulationContext *sc, char *node);

//*************************************
// Simulation primitives
//*************************************
/*
  MAN sim_RunSimulation
  CATEG gns
  DESCRIPTION
  sim_RunSimulation launches the electrical simulation.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_RunSimulation (SimulationContext *sc, char *sim_call);

//*************************************
// Computing primitives
//*************************************

/*
  MAN sim_ExtractMinSlope
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_ExtractMinSlope extracts the minimum slope of a node. 
  RETURN VALUE
  sim_ExtractMinSlope returns the computed slope.
  ERRORS
  none.
*/
extern double sim_ExtractMinSlope (SimulationContext *sc, char *node);

/*
  MAN sim_ExtractMaxSlope
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_ExtractMaxSlope extracts the minimum slope of a node. 
  RETURN VALUE
  sim_ExtractMaxSlope returns the computed slope.
  ERRORS
  none.
*/
extern double sim_ExtractMaxSlope (SimulationContext *sc, char *node);


/*
  MAN sim_ExtractMinDelay
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_ExtractMinDelay extracts the minimum delay between two nodes.
  RETURN VALUE
  sim_ExtractMinDelay returns the computed delay.
  ERRORS
  none.
*/
extern double sim_ExtractMinDelay (SimulationContext *sc, char *node_a,char *node_b);

/*
  MAN sim_ExtractMaxDelay
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_ExtractMaxDelay gets the maximum delay between two nodes.
  RETURN VALUE
  sim_ExtractMaxDelay returns the computed delay.
  ERRORS
  none.
*/
extern double sim_ExtractMaxDelay (SimulationContext *sc, char *node_a,char *node_b);

/*
  MAN sim_ComputeAllDelay
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_ComputeAllDelay computes all the delays between two nodes.
  RETURN VALUE
  sim_ComputeAllDelay returns the list of computed delays.
  ERRORS
  none.
*/
extern Timing *sim_ExtractAllTimings (SimulationContext *sc, char *node_a,char *node_b);

/*
  MAN sim_ExtractMinTransitionDelay
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_ExtractMinTransitionDelay extracts the mininum delay between two nodes, see sim_ExtractMinDelay.
  Parameter 'transition' can be for example "U1D2". In this configuration, delay will be extracted between the second
  rise transition of 'node_a' and the third falling transition of 'node_b'.
  RETURN VALUE
  sim_ExtractMinTransitionDelay returns a delay.
  ERRORS
  none.
*/
extern double sim_ExtractMinTransitionDelay (SimulationContext *sc, char *node_a,char *node_b,char *transition);

/*
  MAN sim_ExtractMaxTransitionDelay
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_ExtractMaxTransitionDelay extracts the maximum delay between two nodes, see sim_ExtractMaxDelay.
  Parameter 'transition' can be for example "U1D2". In this configuration, delay will be extracted between the second
  rise transition of 'node_a' and the third falling transition of 'node_b'.
  RETURN VALUE
  sim_ExtractMaxTransitionDelay returns a delay.
  ERRORS
  none.
*/
extern double sim_ExtractMaxTransitionDelay (SimulationContext *sc, char *node_a,char *node_b,char *transition);

/*
  MAN sim_ExtractMinTransitionSlope
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_ExtractMinTransitionSlope extracts the minimum slope of a node.
  Parameter 'transition' can be for example "U1". In this configuration, slope will be extracted from the
  second rising transition of the node.
  RETURN VALUE
  sim_ExtractMaxTransitionDelay returns a slope.
  ERRORS
  none.
*/
extern double sim_ExtractMinTransitionSlope (SimulationContext *sc, char *node,char *transition);

/*
  MAN sim_ExtractMaxTransitionSlope
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_ExtractMaxTransitionSlope extracts the maximum slope of a node.
  Parameter 'transition' can be for example "D2". In this configuration, slope will be extracted from the third falling transition of the node.
  RETURN VALUE
  sim_ExtractMaxTransitionSlope returns a slope.
  ERRORS
  none.
*/
extern double sim_ExtractMaxTransitionSlope (SimulationContext *sc, char *node,char *transition);

/*
  MAN sim_ComputeSetup
  CATEG gns
  DESCRIPTION
  sim_ComputeSetup computes the setup time of 'data' relatively to 'command'. Setup time is computed by
  observing the latest 'data' transition, relatively to 'command', that generates a transition on 
  the memory point.
  data       : name of the data
  tstart_d   : start time of the pulse on data
  tslope_d   : slope of the pulse on data
  sens_d     : transition of the data : 'U' for rising, 'D' for falling
  cmd        : name of the command
  tstart_min : minimum starting time of the pulse on cmd
  tstart_max : maximum starting time of the pulse on cmd
  tslope_c   : slope of the pulse on cmd
  sens_c     : transition of the cmd : 'U' for rising, 'D' for falling
  mem        : name of the memory point
  data_val   : expected value on mem : 1 for VDD, 0 for VSS
  RETURN VALUE
  sim_ComputeSetup returns the setup computed.
  ERRORS
  none.
*/
extern double sim_ComputeSetup (SimulationContext *sc, char *data,double tstart_d,double tslope_d, char sense_d, char *cmd,double t_start_min_c, double t_start_max_c, double tslope_c,char sense_c, char *mem, int data_val);


/*
  MAN sim_ComputeHold
  CATEG gns
  DESCRIPTION
  sim_ComputeHold computes the hold time of 'data' relatively to 'command'. Hold time is computed by observing
  the latest 'data' transition, relatively to command, that doesn't generate
  any transition on the memory point.
  data       : name of the data
  tstart_d   : start time of the pulse on data
  tslope_d   : slope of the pulse on data
  sens_d     : transition of the data : 'U' for rising, 'D' for falling
  cmd        : name of the command
  tstart_min : minimum starting time of the pulse on cmd
  tstart_max : maximum starting time of the pulse on cmd
  tslope_c   : slope of the pulse on cmd
  sens_c     : transition of the cmd : 'U' for rising, 'D' for falling
  mem        : name of the memory point
  data_val   : expected value on mem : 1 for VDD, 0 for VSS
  RETURN VALUE
  sim_ComputeHold returns the setup computed.
  ERRORS
  none.
*/
extern double sim_ComputeHold (SimulationContext *sc, char *data, double tstart_d,double tslope_d, char sense_d, char *cmd,double t_start_min_c, double t_start_max_c, double tslope_c,char sense_c, char *mem, int data_val);

/*
  MAN sim_ComputeAccess
  CATEG gns
  DESCRIPTION
  sim_ComputeAccess gives the access time of 'dout' relatively to 'command'. Access time is computed by observing
  the delay between the transition on 'command' and the transition on 'dout'.
  RETURN VALUE
  sim_ComputeAccess returns the access computed.
  ERRORS
  none.
*/
extern double sim_ComputeAccess (SimulationContext *sc, char *dout, int dout_val,char *cmd, double tstart_c, double tslope_c, char sens_c, char *mem, int mem_val, double *out_slope);

/*
  MAN elp_GetCapaFromConnector
  CATEG gns
  DESCRIPTION
  elp_GetCapaFromConnector gives the capacitance of a transistor's connector.
  RETURN VALUE
  elp_GetCapaFromConnector returns a capacitance in F.
  ERRORS
  none.
*/
extern double elp_GetCapaFromConnector (SimulationContext *sc, Connector *locon);

/*
  MAN sim_ComputeDelay
  CATEG gns
  DESCRIPTION
  sim_ComputeDelay gives a list of delays between a constant input and a list of outputs.
  RETURN VALUE
  sim_ComputeDelay returns a list of delays.
  ERRORS
  none.
*/
extern List   *sim_ComputeDelay (SimulationContext *sc, char *input, char sens, List *list_output);

/*
  MAN sim_ComputeMaxDelayTransition
  CATEG gns
  DESCRIPTION
  sim_ComputeMaxDelayTransition gives the maximum delay corresponding to a transition between the input and the output.
  RETURN VALUE
  sim_ComputeMaxDelayTransition returns a delay.
  ERRORS
  none.
*/
extern double  sim_ComputeMaxDelayTransition (SimulationContext *sc, char *input, double input_start, double input_slope,char *output, char *transition);

/*
  MAN sim_ComputeMinDelayTransition
  CATEG gns
  DESCRIPTION
  sim_ComputeMinDelayTransition gives the minimum delay corresponding to a transition between the input and the output.
  RETURN VALUE
  sim_ComputeMinDelayTransition returns a delay.
  ERRORS
  none.
*/
extern double  sim_ComputeMinDelayTransition (SimulationContext *sc, char *input, double input_start, double input_slope,char *output, char *transition);

/*
  MAN sim_GetTimingFromList
  CATEG gns
  DESCRIPTION
  sim_GetTimingFromList gives a timing object corresponding to a list.
  RETURN VALUE
  sim_GetTimingFromList returns a timing object.
  ERRORS
  none.
*/
extern Timing *sim_GetTimingFromList   (List *list);

/*
  MAN sim_GetTimingNext
  CATEG gns
  DESCRIPTION
  sim_GetTimingNext gives the next timing object.
  RETURN VALUE
  sim_GetTimingNext returns a timing object.
  ERRORS
  none.
*/
extern Timing *sim_GetTimingNext       (Timing *timing);

/*
  MAN sim_GetTiming
  CATEG gns
  DESCRIPTION
  sim_GetTiming retrieves the timing between the root node name 'rootname' and the destination node name 'nodename'.
  RETURN VALUE
  sim_GetTiming returns the pointer on the timing, NULL if this corresponding timing doesn't exist.
  ERRORS
  none.
*/
extern Timing *sim_GetTiming           (char *root,char *node);

/*
  MAN sim_GetTimingByEvent
  CATEG gns
  DESCRIPTION
  sim_GetTimingByEvent retrieves the timing between the root node name 'rootname' and the destination node name 'nodename'.
  This timing must respect the good event on 'rootname' and 'nodename'.
  Event is the expected event from root to node, it can be 'U' (rising) or 'D' (falling) and can be followed by an integer. 
  RETURN VALUE
  sim_GetTimingByEvent returns the pointer on the timing, NULL if this corresponding timing doesn't exist.
  ERRORS
  none.
*/
extern Timing *sim_GetTimingByEvent    (char *root,char *node,char *event);

/*
  MAN sim_GetTimingDelay
  CATEG gns
  DESCRIPTION
  sim_GetTimingDelay gets the delay corresponding to the timing.
  RETURN VALUE
  sim_GetTimingDelay returns a delay.
  ERRORS
  none.
*/
extern double  sim_GetTimingDelay      (Timing *timing);

/*
  MAN sim_GetTimingMinDelay
  CATEG gns
  DESCRIPTION
  sim_GetTimingMinDelay gets the minimun delay corresponding to the timing.
  RETURN VALUE
  sim_GetTimingMinDelay returns a delay.
  ERRORS
  none.
*/
extern double  sim_GetTimingMinDelay   (Timing *timing);

/*
  MAN sim_GetTimingMaxDelay
  CATEG gns
  DESCRIPTION
  sim_GetTimingMaxDelay gets the maximun delay corresponding to the timing.
  RETURN VALUE
  sim_GetTimingMaxDelay returns a delay.
  ERRORS
  none.
*/
extern double  sim_GetTimingMaxDelay   (Timing *timing);

/*
  MAN sim_GetTimingSlope
  CATEG gns
  DESCRIPTION
  sim_GetTimingSlope gets the slope corresponding to the timing.
  RETURN VALUE
  sim_GetTimingSlope returns a slope.
  ERRORS
  none.
*/
extern double  sim_GetTimingSlope      (Timing *timing);

/*
  MAN sim_GetTimingMinSlope
  CATEG gns
  DESCRIPTION
  sim_GetTimingMinSlope gets the minimum slope corresponding to the timing.
  RETURN VALUE
  sim_GetTimingMinSlope returns a slope.
  ERRORS
  none.
*/
extern double  sim_GetTimingMinSlope   (Timing *timing);

/*
  MAN sim_GetTimingMaxSlope
  CATEG gns
  DESCRIPTION
  sim_GetTimingMaxSlope gets the maximum slope corresponding to the timing.
  RETURN VALUE
  sim_GetTimingMaxSlope returns a slope.
  ERRORS
  none.
*/
extern double  sim_GetTimingMaxSlope   (Timing *timing);

/*
  MAN sim_GetTimingRoot
  CATEG gns
  DESCRIPTION
  sim_GetTimingRoot gets the name of the root node corresponding to the timing.
  RETURN VALUE
  sim_GetTimingRoot returns a name.
  ERRORS
  none.
*/
extern char   *sim_GetTimingRoot       (Timing *timing);

/*
  MAN sim_GetTimingNode
  CATEG gns
  DESCRIPTION
  sim_GetTimingNode gets the name of the node (destination) corresponding to the timing.
  RETURN VALUE
  sim_GetTimingNode returns a name.
  ERRORS
  none.
*/
extern char   *sim_GetTimingNode       (Timing *timing);

/*
  MAN sim_GetTimingRootInNetlist
  CATEG gns
  DESCRIPTION
  sim_GetTimingRootInNetlist gets the name of the root node in the netlist corresponding to the timing.
  RETURN VALUE
  sim_GetTimingRootInNetlist returns a name.
  ERRORS
  none.
*/
extern char   *sim_GetTimingRootInNetlist  (Timing *timing);

/*
  MAN sim_GetTimingNodeInNetlist
  CATEG gns
  DESCRIPTION
  sim_GetTimingNodeInNetlist gets the name of node (destination) in the netlist corresponding to the timing.
  RETURN VALUE
  sim_GetTimingNodeInNetlist returns a name.
  ERRORS
  none.
*/
extern char   *sim_GetTimingNodeInNetlist  (Timing *timing);

/*
  MAN sim_GetTimingRootEvent
  CATEG gns
  DESCRIPTION
  sim_GetTimingRootEvent gets the event of the root node corresponding to the timing.
  Event is SIM_FALL or SIM_RISE.
  RETURN VALUE
  sim_GetTimingRootEvent returns an event.
  ERRORS
  none.
*/
extern char    sim_GetTimingRootEvent  (Timing *timing);

/*
  MAN sim_GetTimingNodeEvent
  CATEG gns
  DESCRIPTION
  sim_GetTimingNodeEvent gets the event of the node (destination) corresponding to the timing.
  Event is SIM_FALL or SIM_RISE.
  RETURN VALUE
  sim_GetTimingNodeEvent returns an event.
  ERRORS
  none.
*/
extern char    sim_GetTimingNodeEvent  (Timing *timing);

/*
  MAN sim_NoiseExtract
  CATEG gns
  DESCRIPTION
  After a simulation run, sim_NoiseExtract extracts the maximum noise on a node between
  two moments.
  The initial time (tinit) and the tfinal time (tfinal) represent the timing bounds
  to extract noise.
  vthnoise is the threshold voltage to extract noise (percentage of vdd).
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void   sim_NoiseExtract  (SimulationContext *sc, char *node, double vthnoise, double tinit, double tfinal);

/*
  MAN sim_NoiseGetVth
  CATEG gns
  DESCRIPTION
  sim_NoiseGetVth gets the noise threshold voltage on the node 'name'.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern double sim_NoiseGetVth   (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseGetPeakList
  CATEG gns
  DESCRIPTION
  sim_NoiseGetPeakList gets a list of peaks relatively to node 'name'.
  RETURN VALUE
  sim_NoiseGetPeakList returns a 'List'.
  ERRORS
  none.
*/
extern List  *sim_NoiseGetPeakList (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseGetMomentList
  CATEG gns
  DESCRIPTION
  sim_NoiseGetMomentList gets a list of moment of passage on noise threshold 
  voltage relatively to node 'name'.
  RETURN VALUE
  sim_NoiseGetMomentList returns a 'List'.
  ERRORS
  none.
*/
extern List  *sim_NoiseGetMomentList (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseGetMoment
  CATEG gns
  DESCRIPTION
  sim_NoiseGetMoment gets a the moment of passage on noise threshold 
  voltage from a pointer on a pointer list returned by sim_NoiseGetMomentList.
  RETURN VALUE
  sim_NoiseGetMoment returns a moment.
  ERRORS
  none.
*/
extern double sim_NoiseGetMoment (SimulationContext *sc, List *noise_tclist);

/*
  MAN sim_NoiseGetPeakValue 
  CATEG gns
  DESCRIPTION
  sim_NoiseGetPeakValue gets the peak value (voltage) relatively to the node 'name' and a pointer
  on a List returned by sim_NoiseGetPeakList.
  RETURN VALUE
  sim_NoiseGetPeakValue returns a voltage.
  ERRORS
  none.
*/
extern double sim_NoiseGetPeakValue (SimulationContext *sc, char *name,List *noise_peaklist);

/*
  MAN sim_NoiseGetPeakMoment
  CATEG gns
  DESCRIPTION
  sim_NoiseGetPeakMoment gets the peak moment relatively to a pointer
  on a List returned by sim_NoiseGetPeakList.
  RETURN VALUE
  sim_NoiseGetPeakMoment returns a moment (time).
  ERRORS
  none.
*/
extern double sim_NoiseGetPeakMoment (SimulationContext *sc, List *noise_peaklist);

/*
  MAN sim_NoiseExtractMaxPeakValue
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMaxPeakValue gets the maximum peak value relatively to the node 'name'.
  RETURN VALUE
  sim_NoiseExtractMaxPeakValue returns a voltage.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMaxPeakValue (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseExtractMinPeakValue
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMinPeakValue gets the minimum peak value relatively to the node 'name'.
  RETURN VALUE
  sim_NoiseExtractMinPeakValue returns a voltage.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMinPeakValue (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseExtractMaxPeakMoment
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMaxPeakMoment gets the moment of the maximum peak relatively to the node 'name'.
  RETURN VALUE
  sim_NoiseExtractMaxPeakMoment returns a moment.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMaxPeakMoment (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseExtractMinPeakMoment
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMinPeakMoment gets the moment of the minimum peak relatively to the node 'name'.
  RETURN VALUE
  sim_NoiseExtractMinPeakMoment returns a moment.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMinPeakMoment (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseGetMomentBeforePeak
  CATEG gns
  DESCRIPTION
  sim_NoiseGetMomentBeforePeak gets the moment of passage on noise threshold voltage
  relatively to node 'name' and before a peak which is a pointer on a List returned by sim_NoiseGetPeakList.
  RETURN VALUE
  sim_NoiseGetMomentBeforePeak returns a moment.
  ERRORS
  none.
*/
extern double sim_NoiseGetMomentBeforePeak (SimulationContext *sc, char *name,List *peak);

/*
  MAN sim_NoiseGetMomentAfterPeak
  CATEG gns
  DESCRIPTION
  sim_NoiseGetMomentAfterPeak gets the moment of passage on noise threshold voltage
  relatively to node 'name' and after a peak which is a pointer on a List returned by sim_NoiseGetPeakList.
  RETURN VALUE
  sim_NoiseGetMomentAfterPeak returns a moment.
  ERRORS
  none.
*/
extern double sim_NoiseGetMomentAfterPeak (SimulationContext *sc, char *name,List *peak);

/*
  MAN sim_NoiseGetPeakDuration.
  CATEG gns
  DESCRIPTION
  sim_NoiseGetPeakDuration gets the duration of a peak which is a pointer on a
  List returned by sim_NoiseGetPeakList.
  RETURN VALUE
  sim_NoiseGetPeakDuration returns a duration.
  ERRORS
  none.
*/
extern double sim_NoiseGetPeakDuration (SimulationContext *sc, char *name,List *peak);

/*
  MAN sim_NoiseExtractMomentBeforeMaxPeak.
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMomentBeforeMaxPeak extracts the moment of passage on noise threshold voltage
  before the maximum peak relatively to node 'name'.
  RETURN VALUE
  sim_NoiseExtractMomentBeforeMaxPeak returns a moment.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMomentBeforeMaxPeak (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseExtractMomentBeforeMinPeak.
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMomentBeforeMinPeak extracts the moment of passage on noise threshold voltage
  before the minimum peak relatively to node 'name'.
  RETURN VALUE
  sim_NoiseExtractMomentBeforeMinPeak returns a moment.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMomentBeforeMinPeak (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseExtractMomentAfterMaxPeak.
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMomentAfterMaxPeak extracts the moment of passage on noise threshold voltage
  after the maximum peak relatively to node 'name'.
  RETURN VALUE
  sim_NoiseExtractMomentAfterMaxPeak returns a moment.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMomentAfterMaxPeak (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseExtractMomentAfterMinPeak.
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMomentAfterMinPeak extracts the moment of passage on noise threshold voltage
  after the minimum peak relatively to node 'name'.
  RETURN VALUE
  sim_NoiseExtractMomentAfterMinPeak returns a moment.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMomentAfterMinPeak (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseExtractMaxPeakDuration.
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMaxPeakDuration extracts the duration of the maximum peak relatively to node 'name'.
  RETURN VALUE
  sim_NoiseExtractMaxPeakDuration returns a duration.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMaxPeakDuration (SimulationContext *sc, char *name);

/*
  MAN sim_NoiseExtractMinPeakDuration.
  CATEG gns
  DESCRIPTION
  sim_NoiseExtractMinPeakDuration extracts the duration of the minimum peak relatively to node 'name'.
  RETURN VALUE
  sim_NoiseExtractMinPeakDuration returns a duration.
  ERRORS
  none.
*/
extern double sim_NoiseExtractMinPeakDuration (SimulationContext *sc, char *name);

/*
  MAN sim_DriveLut
  CATEG gns
  DESCRIPTION
  sim_DriveLut drives a look up table of delays, X-axis correspond to slew variations and
  Y-axis correspond to load variations.
  'lcin' is the name of the input connector, 'lcout' is the name of the output connector.
  'transition' is the expected transition between 'lcin' and 'lcout'.
  'slew_range' is a list of slew, 'load_range' is a list of load.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
//extern void sim_DriveLut (SimulationContext *sc, char *lcin, char *lcout, char *transition, List *slew_range, List *load_range);

/*
  MAN sim_DriveNodeState
  CATEG gns
  DESCRIPTION
  sim_DriveNodeState drives successives states of the 'node_state2drive' in the file 'filename' 
  (which also contains file's extension). 'node_ref' is the node reference, 'type' can be
  SIM_RISE or SIM_FALL and represents the event on 'node_ref' which will sample 'node_state2drive'.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void sim_DriveNodeState (SimulationContext *sc, char *filename,char *node_ref,char *node_state2drive,char type);

/*
  MAN sim_ExtractCommutInstant
  CATEG gns
  DESCRIPTION
  sim_ExtractCommutInstant extract the first instant when the node reach the 
  voltage value 'voltage'.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern double sim_ExtractCommutInstant (SimulationContext *sc, char *node,double voltage);


/*
  MAN sim_DriveTransistorAsInstance
  CATEG gns
  DESCRIPTION
  sim_DriveTransistorAsInstance(<context>, <mode>) indicates is the transistor should be driven as instances. 
  
  eg. M124 src grid drain bulk ... => XM124 src grid drain bulk ...
  
  the value for mode is 'y' to enable the transformation else 'n'
  RETURN VALUE
  none.
  ERRORS
  none.
*/
extern void sim_DriveTransistorAsInstance (SimulationContext *sc, char mode);

extern Netlist *sim_GetNetlist (char *name);


/*
  MAN sim_SaveContext
  CATEG gns
  DESCRIPTION
  sim_SaveContext(<context>, <label>) duplicate the <context> so the simulation initial conditions  can be stored for automatic resimulations. The resulting simulation context is generaly used when creation timing lines who can be recomputed dynamically during a stability analysis process for instance. If label!=NULL, it can be used to retrieve the saved simulation context.
  RETURN VALUE
  returns a `SimulationContext *'.
  ERRORS
  none.
  SEE ALSO
  sim_GetSavedContext, sim_AddAlias
*/
//extern SimulationContext *sim_SaveContext (SimulationContext *sc, char *label);


/*
  MAN sim_GetSavedContext
  CATEG gns
  DESCRIPTION
  sim_GetSavedContext(<context>, <label>) returns a previously saved simulation context. If the context if not found, NULL is returned.
  RETURN VALUE
  returns a `SimulationContext *'.
  ERRORS
  none.
  SEE ALSO
  sim_SaveContext, sim_AddAlias
*/
//extern SimulationContext *sim_GetSavedContext(SimulationContext *src, char *label);


/*
  MAN sim_AddAlias
  CATEG gns
  DESCRIPTION
  sim_AddAlias(<context>, <src>, <dest>) create a signal alias. The effect is to translate all the simulation delay or slope computation from one signal to another. This function is very usefull when only one simulation is used to compute the delay or slope for a range of signal. When automatically recomputing the timing line delay for com(2) for instance, is com(2) is aliased to com(1), com(1) will be used to compute the delay or slope.
  RETURN VALUE
  none.
  ERRORS
  none.
  SEE ALSO
  sim_SaveContext
*/
//void sim_AddAlias(SimulationContext *sc, char *source, char *destination);


void SET_CONTEXT(SimulationContext *sc);
SimulationContext *GET_CONTEXT();


/*
  MAN sim_AddSpiceMeasure
  CATEG gns
  DESCRIPTION
  RETURN VALUE
  none.
  ERRORS
  none.
  SEE ALSO
*/
void sim_AddSpiceMeasure(SimulationContext *sc, char *delay, char *slope, char *sig1, char *sig2, char *transition, char delay_type);
/*
  MAN sim_AddSpiceMeasureSlope
  CATEG gns
  DESCRIPTION
  Add a slope measure of sig that can be extracted by the
  label (slope).
  Transition is a string containing the transition of sig.
  Tolerated transition are 'U' and 'D'.
  The <delay_type> can be SIM_MIN or SIM_MAX meaning the node choosen to compute the delay or the slope
  is the closer or the farther one.
  RETURN VALUE
  none.
  ERRORS
  none.
  SEE ALSO
*/
void sim_AddSpiceMeasureSlope(SimulationContext *sc, char *slope, char *sig, char *transition, char delay_type);
/*
  MAN sim_AddSpiceMeasureDelay
  CATEG gns
  DESCRIPTION
  Add a measure of delay between sig1 and sig2 that can be extracted by the
  label (delay).
  Transition is a string containing the transition of sig1 and sig2.
  The <delay_type> can be SIM_MIN or SIM_MAX meaning the node choosen to compute the delay or the slope
  is the closer or the farther one.
  Tolerated transition are 'U' and 'D'.
  RETURN VALUE
  none.
  ERRORS
  none.
  SEE ALSO
*/
void sim_AddSpiceMeasureDelay(SimulationContext *sc, char *delay, char *sig1, char *sig2, char *transition, char delay_type);
/*
  MAN sim_ReadMeasure
  CATEG gns
  DESCRIPTION
  Reads the simulation results from {filename} and return the value corresponding to the measure {label}.
  On failure, returns {-1}.
  RETURN VALUE
  returns -1 on failure.
  ERRORS
  none.
  SEE ALSO
*/
double sim_ReadMeasure(char *filename, char *label);

/*
  MAN sim_ResetMeasures
  CATEG gns
  DESCRIPTION
  Resets all the measures set in the simulation context.
*/
void sim_ResetMeasures(SimulationContext *model);

//int sim_GetSpiceMeasure(SimulationContext *sc, char *label, double *value);
/*
  MAN sim_GetSpiceMeasureSlope
  CATEG gns
  DESCRIPTION
  Returns the slope computed for the label, 0.0 if slope has not been computed.
  RETURN VALUE
  ERRORS
  none.
  SEE ALSO
*/
double sim_GetSpiceMeasureSlope(SimulationContext *model, char *label);
/*
  MAN sim_GetSpiceMeasureDelay
  CATEG gns
  DESCRIPTION
  Returns the delay computed for the label, 0.0 if delay has not been computed.
  RETURN VALUE
  ERRORS
  none.
  SEE ALSO
*/


double sim_GetSpiceMeasureDelay(SimulationContext *model, char *label);

/*
  MAN sim_SpiceMeasure
  CATEG gns
  DESCRIPTION
  Add a delay measure between sig1 and sig2 that can be extracted by the
  label (delay).
  Add a slope measure of sig that can be extracted by the
  label (slope).
  Transition is a string containing the transition of sig1 and sig2.
  Tolerated transition are 'U' and 'D'.
  The <delay_type> can be SIM_MIN or SIM_MAX meaning the node choosen to compute the delay or the slope
  is the closer or the farther one.
  After simulation completed get the delay and the slope computed
  for each label and store it into the adress pointed by valued(delay)
  and values(slope).
  RETURN VALUE
  none.
  ERRORS
  none.
  SEE ALSO
  sim_SpiceMeasureDelay, sim_SpiceMeasureSlope
*/
void sim_SpiceMeasure(SimulationContext *model, char *delay, double *valued, char *slope, double *values, char *sig1, char *sig2, char *transition, char delay_type);

/*
  MAN sim_SpiceMeasureDelay
  CATEG gns
  DESCRIPTION
  Add a measure of delay between sig1 and sig2 that can be extracted by the
  label (delay).
  Transition is a string containing the transition of sig1 and sig2.
  Tolerated transition are 'U' and 'D'.
  The <delay_type> can be SIM_MIN or SIM_MAX meaning the node choosen to compute the delay or the slope
  is the closer or the farther one.
  After simulation completed get the delay computed for the label 
  and store it into the adress pointed by value.
  RETURN VALUE
  none.
  ERRORS
  none.
  SEE ALSO
*/
void sim_SpiceMeasureDelay(SimulationContext *model, char *delay, double *value, char *sig1, char *sig2, char *transition, char delay_type);

/*
  MAN sim_SpiceMeasureSlope
  CATEG gns
  DESCRIPTION
  Add a slope measure of sig that can be extracted by the
  label (slope).
  Transition is a string containing the transition of sig.
  Tolerated transition are 'U' and 'D'.
  The <delay_type> can be SIM_MIN or SIM_MAX meaning the node choosen to compute the delay or the slope
  is the closer or the farther one.
  After simulation completed get the slope computed for the label 
  and store it into the adress pointed by value.
  RETURN VALUE
  none.
  ERRORS
  none.
  SEE ALSO
*/
void sim_SpiceMeasureSlope(SimulationContext *model, char *slope, double *value, char *sig, char *transition, char delay_type);

void sim_DumpDelayDetail(SimulationContext *model, char *name1, char *name2, char *transition, FILE *f);

void sim_RenameModel(SimulationContext *sc, char *name);

double sim_SpreadRC(SimulationContext *sc, char *input, char *output, char *dir, double input_delay, double delta_delay);

/*
  MAN sim_DefineInclude
  CATEG gns
  DESCRIPTION
  Sets the filename containing the netlist to apply the pattern to. When this option is used, the gns rule netlist won't be used for the simulation. It will be replaced by the external file given in <filename> at the simulation time.
  RETURN VALUE
  none.
*/
void sim_DefineInclude(SimulationContext *sc, char *filename);

/*
    MAN cpe_DefineCorrelation
    CATEG tcl+cpe
    SYNOPSIS
    cpe_DefineCorrelation <net1> <relation> <net2>\$
    DESCRIPTION
    Defines the relation between 2 signals to help computing propagation condition for simulations.
    ARGS
    net1 % First net name.
    relation % Value '{=}' means {net1}={net2}\$ Value '{!=}' means {net1}=not({net2})
    net2 % Second net name.
    EXAMPLE % {cpe_DefineCorrelation clk != clkb}
*/
void cpe_DefineCorrelation(void);

