#include "avt_API_types.h"

#define TTV_NONE       ' '
#define TTV_LATCH      'l'
#define TTV_FLIPFLOP   'f'
#define TTV_BREAKPOINT 'b'
#define TTV_COMMAND    'q'
#define TTV_PRECHARGE  'r'

#define TTV_CONNECTOR          'c'
#define TTV_SIGNAL             's'
#define TTV_INSTANCE_CONNECTOR 'n'

#define TTV_TIMING  0x101010
#define TTV_SETUP   0x101011
#define TTV_HOLD    0x101012
#define TTV_ACCESS  0x101013


void ttv_API_AtLoad_Initialize();

void ttv_SetBaseNetlist(Netlist *lf);

/*
    MAN ttv_CreateTimingFigure
    CATEG tcl+generation
    DESCRIPTION
    Initiates the creation of a timing figure. Computes the connectors capacitances assuming the netlist provided is a flat-transistor one.
    ARGS
    lf % Netlist the timing figure is based on. 
    EXAMPLE % {ttv_CreateTimingFigure $netlist}
*/

void ttv_CreateTimingFigure(Netlist *lf);

/*
    MAN ttv_EditTimingFigure
    CATEG tcl+generation
    DESCRIPTION
    Edit an existing timing timing figure.
    {ttv_FinishTimingFigure} must be called after the modifications to make the timing figure usable.
    ARGS
    tvf % TimingFigure to edit. 
    EXAMPLE % {ttv_EditTimingFigure $fig}
*/
void ttv_EditTimingFigure(TimingFigure *tvf);

/*
    MAN ttv_AddConnector
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a connector timing signal. 
    ARGS
    name % Name of the newly created command. If this timing signal already exists, no new timing signal is created. In the last case, incompatible timing signal types will raise a warning and the timing signal type will be overridden
    dir % Direction of the connector: i=in, o=out, t=tristate, z=hz, x=unknown, b=inout (output reused internaly)
    EXAMPLE % {set con [ttv_AddCommand input i]}
*/
TimingSignal *ttv_AddConnector(char *name, char dir);

/*
    MAN ttv_AddCommand
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a command timing signal. 
    ARGS
    name % Name of the newly created command. If this timing signal already exists, no new timing signal is created. In the last case, incompatible timing signal types will raise a warning and the timing signal type will be overridden
    EXAMPLE % {set com [ttv_AddCommand com0]}
*/
TimingSignal *ttv_AddCommand(char *name);

/*
    MAN ttv_AddLatch
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a latch timing signal. 
    ARGS
    name % Name of the newly created latch. If this timing signal already exists, no new timing signal is created. In the last case, incompatible timing signal types will raise a warning and the timing signal type will be overridden
    EXAMPLE % {set latch [ttv_AddLatch lt0]}
*/
TimingSignal *ttv_AddLatch(char *name);
TimingSignal *ttv_AddFlipFlop(char *name);

/*
    MAN ttv_AddPrecharge
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a precharge timing signal. 
    ARGS
    name % Name of the newly created precharge. If this timing signal already exists, no new timing signal is created. In the last case, incompatible timing signal types will raise a warning and the timing signal type will be overridden
*/
TimingSignal *ttv_AddPrecharge(char *name);


/*
    MAN ttv_AddBreakpoint
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a breakpoint timing signal. 
    ARGS
    name % Name of the newly created breakpoint. If this timing signal already exists, no new timing signal is created. In the last case, incompatible timing signal types will raise a warning and the timing signal type will be overridden
*/
TimingSignal *ttv_AddBreakpoint(char *name);


/*
    MAN ttv_AddTiming
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a data delay line.
    ARGS
    input % Delay line start point
    output % Delay line end point
    max_slope % Maximum slope associated with delay line
    min_slope % Minimum slope associated with delay line
    max_delay % Maximum propagation delay associated with delay line
    min_delay % Minimum propagation delay associated with delay line
    dir       % Transitions on start and end points: {uu} for input rising/output rising, {ud} for input rising/output falling, {dd} for input falling/output falling, {du} for input falling/output rising.
*/
TimingLine *ttv_AddTiming(char *input, char *output, double max_delay, double max_slop, double min_delay, double min_slope, char *dir);

/*
    MAN ttv_AddHZTiming
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a HZ data delay line.
    ARGS
    input % Delay line start point
    output % Delay line end point
    max_slope % Maximum slope associated with delay line
    max_delay % Maximum propagation delay associated with delay line
    dir       % Transitions on start and end points: {uu} for input rising/output rising, {ud} for input rising/output falling, {dd} for input falling/output falling, {du} for input falling/output rising.
*/
TimingLine *ttv_AddHZTiming(char *input, char *output, double max_delay, double min_delay, char *dir);


/*
MAN ttv_AddSetup
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a setup line.
    ARGS
    input % Data point
    output % Clock point
    delay % Constraint associated with the setup line
    dir % Transitions on data and clock points: {uu} for input rising/output rising, {ud} for input rising/output falling, {dd} for input falling/output falling, {du} for input falling/output rising.
*/
TimingLine *ttv_AddSetup(char *input, char *output, double delay, char *dir);


/*
    MAN ttv_AddAccess
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a data access delay line 
    ARGS
    input % Data access line start point
    output % Data access line end point
    max_slope % Maximum slope associated with delay line
    min_slope % Minimum slope associated with delay line
    max_delay % Maximum propagation delay associated with delay line
    min_delay % Minimum propagation delay associated with delay line
    dir %  Transitions on start and end timing signals: {uu}, {ud}, {dd} or {du}.
*/
TimingLine *ttv_AddAccess(char *input, char *output, double max_delay, double max_slop, double min_delay, double min_slope, char *dir);

/*
    MAN ttv_AddHZAccess
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a HZ data access line.
    ARGS
    input % Delay line start point
    output % Delay line end point
    max_slope % Maximum slope associated with delay line
    max_delay % Maximum propagation delay associated with delay line
    dir       % Transitions on start and end points: {uu} for input rising/output rising, {ud} for input rising/output falling, {dd} for input falling/output falling, {du} for input falling/output rising.
*/
TimingLine *ttv_AddHZAccess(char *input, char *output, double max_delay, double min_delay, char *dir);

/*
    MAN ttv_AddHold
    CATEG gns+tcl+generation
    DESCRIPTION
    Creates a hold constraint line between a data pin and a clock pin. 
    ARGS
    input % Data pin
    output % Clock pin
    delay % Nominal constraint value associated with the line
    dir %  Transitions on the data and clock pins: {uu} for input rising/output rising, {ud} for input rising/output falling, {dd} for input falling/output falling, {du} for input falling/output rising.
*/
TimingLine *ttv_AddHold(char *input, char *output, double delay, char *dir);

/*
    MAN ttv_SetLineCommand
    CATEG gns+tcl+generation
    DESCRIPTION
    Associates commands to a line. The command is the signal that enables reading or writing in a latch point. 
    ARGS
    max_command % Command generating the maximum delay 
    min_command % Command generating the minimum delay 
    dir % First character is the edge of the max command and the second of the min command.
*/

void ttv_SetLineCommand(TimingLine *tl, char *max_command, char *min_command, char *dir);

/*
    MAN ttv_SetLineModel
    CATEG gns+tcl+generation
    DESCRIPTION
    Associates a model name to a line. 
    ARGS
    tl % Timing Line to set the model name
    modelname % name of the model to associate. 
    where % value to associate model to: "delay max", "delay min", "slope max" or "slope min". 
*/

void ttv_SetLineModel(TimingLine *tl, char *modelname, char *where);

/*
    MAN ttv_FinishTimingFigure
    CATEG tcl+generation
    DESCRIPTION
    Returns the finished timing figure currently in creation. No more modification will be
    permitted on the timing figure after this step.
*/

TimingFigure *ttv_FinishTimingFigure();

/*
    MAN ttv_DriveTimingFigure
    CATEG tcl+generation
    DESCRIPTION
    Drives a timing figure on disk
    ARGS
    tvf % Figure to be driven
    filename % Full filename
    format % File format, {dtx} or {ttx}
*/

void ttv_DriveTimingFigure(TimingFigure *tvf, char *filename, char *format);

// fonction pour la creation libre de ttv
TimingSignal *ttv_SetNodeSubtype(char *node, char subtype);
TimingSignal *ttv_GetNode(char *node, char *signal);
TimingSignal *ttv_AddCustomNode(char *node, char *signal, char type, char subtype);
TimingLine *ttv_AddCustomLine(TimingSignal *input, TimingSignal *output, double max_delay, double max_slop, double min_delay, double min_slope, char *dir, int type);


/*
MAN ttv_BuildTiming
  CATEG gns
DESCRIPTION
Creates a timing figure for the current instance model using the timing figure created for the sub-instances' models. The new timing figure is flat. The sub-instances' models without a timing figure associated will be disregarded.
*/
void ttv_BuildTiming();

void ttv_API_Action_Initialize();
void ttv_API_Action_Terminate();
void ttv_API_TopLevel(long list);


/*
MAN ttv_TasBuiltTiming
  CATEG gns
DESCRIPTION
Creates a timing figure for the current instance model using HiTas. {lf} should be a transistor level netlist with or without RC, obtained using the GNS API. This fonction is usefull for pure digital models whose timing can be computed without care for analog behaviours.
*/
void ttv_TasBuiltTiming(Netlist *lf);


/*
MAN ttv_SetFunctionsFile
  CATEG gns
DESCRIPTION
Allows the user to specify the file in which the functions are to be found.
*/
void ttv_SetFunctionsFile (char *functions_file);


void ttv_Config(double value, double out_capa);

/*
MAN ttv_Associate_Model
  CATEG gns
DESCRIPTION
associate a delay computation method to a timing line. When the {line} delay will need to be recomputed, {function call} will be executed.
*/
void ttv_Associate_Model(TimingLine *tvl, void *func);

/*
MAN ttv_SetOutputSwing
  CATEG gns
DESCRIPTION
defines the swing of a signal to use when using the function 'ttv_Associate_Model'.
*/
void ttv_SetOutputSwing(double low, double high);

/*
MAN ttv_Associate_Sim
  CATEG gns
DESCRIPTION
associate a simulation method to a timing line. When CALL_SIMULATION() will be called, {function call} will be executed.
*/
void ttv_Associate_Sim(TimingLine *tvl, void *func);

/*
MAN ttv_Associate_Env
  CATEG gns
DESCRIPTION
associate a simulation environment positioning method to a timing line. When CALL_SIMULATION_ENV() will be called, {function call} will be executed.
*/
void ttv_Associate_Env(TimingLine *tvl, void *func);



void ttv_Associate_Ctk_Env(TimingLine *tvl, void *func);

/*
MAN ttv_MinDelay
  CATEG gns
DESCRIPTION
indicates if the delay to be computed is a minimum transition delay.
Returns 1 if true, 0 otherwise.
*/
int ttv_MinDelay();

/*
MAN ttv_MaxDelay
  CATEG gns
DESCRIPTION
indicates if the delay to be computed is a maximum transition delay.
returns 1 if true, 0 otherwise.
*/
int ttv_MaxDelay();

/*
MAN ttv_UseInstanceMode
  CATEG gns
DESCRIPTION
sets the ttv API to generate one timing figure per instance. The default behaviour is to create a timing model which will describe all the instances of the the same model.
*/
void ttv_UseInstanceMode();

void ASSOCIATE_TIMING(void *func);




/*
  MAN ttv_InitializeNewRun
  CATEG tcl+pvt
  DESCRIPTION
  Initializes the environment to enable the use of the TTV computing functions like {ttv_ComputeMaxPathDelay} or {ttv_LoadTimingFigure}. This function cleans up all the previously loaded timing figures.
*/
void ttv_InitializeNewRun();

/*
  MAN ttv_TerminateRun
  CATEG tcl+pvt
  DESCRIPTION
  Cleans up all the timing figures loaded with {ttv_LoadTimingFigure}.
*/
void ttv_TerminateRun();

/*
  MAN ttv_LoadTimingFigure
  CATEG tcl+pvt
  DESCRIPTION
  Reads a list of DTX timing figures from disk. The given {name} is the radical of the files. To use the timing computing functions, the figures on disk should look like {radical_voltage value_temperature value}. eg. for files like 'circuit_2.4_70', {ttv_LoadTimingFigure circuit} should be called. The last loaded timing figure is returned.
  Returns NULL on failure.
*/
TimingFigure *ttv_LoadTimingFigure(char *name);

/*
  MAN xxxxttv_LoadTimingPathFigure
  CATEG tcl+pvt
  DESCRIPTION
  Reads a list of TTX timing path figures from disk. The given {name} is the radical of the files. To use the timing computing functions, the figures on disk should look like {radical_voltage value_temperature value}. eg. for files like 'circuit_2.4_70', {ttv_LoadTimingPathFigure circuit} should be called. The last loaded timing figure is returned.
  Returns NULL on failure.
*/
TimingFigure *ttv_LoadTimingPathFigure(char *name);

/*
  MAN ttv_GetTimingFigure
  CATEG tcl+dtb
  SYNOPSIS
    <TimingPathList *> ttv_GetTimingFigure <name>
    \$
    Obsolete but still working:\$
    <TimingPathList *> ttv_GetTimingFigure <name> <temperature> <supply value>
  DESCRIPTION
  Gets a timing figure from memory, assuming it has already been loaded. Only usefull when a timing figure comes from a {.lib} of {.tlf} file.
  ARGS
  name % Timing figure's name
  EXAMPLE % {set fig [ttv_GetTimingFigure my_design]}
*/
TimingFigure *ttv_GetTimingFigure(char *name, float temp, float alim);

/*---------------------------------
  MAN ttv_ComputeMaxPathDelay
  CATEG tcl+pvt
  DESCRIPTION
  Re-computes the delay of the maximum path from {start} to {end} with transitions {dir} on 
  {start} and {end}, from the timing figure {figname}, with voltage {alim} and temperature {temp}. {in_slope} is applied on {start} and {out_capa} is set on {end}. If {temp} and {alim} are negative or 0, the timing figure {figname} is used.
  eg. ttv_ComputeMaxPathDelay(circuit, {reset[3]}, {internalreset}, {ud}, 200, 1e-15, 2.5, 70) returns the delay between {reset[3]} rising and {internalreset} falling with an input slope of 200ps and 1fF on {internalreset} when voltage is 2.5V and temperature 70C.
  Returns 0 on failure.
*/
double ttv_ComputeMaxPathDelay(char *figure, char *start, char *end, char *dir, double input_slope, double output_capa, double VDD, double temperature);

/*
  MAN ttv_ComputeMaxPathSlope
  CATEG tcl+pvt
  DESCRIPTION
  Re-computes the slope of the maximum path from {start} to {end} with transitions  {dir} on {start} and {end}, from the timing figure {figname}, with voltage {alim} and temperature {temp}. {in_slope} is applied on {start} and {out_capa} is set on {end}. If {temp} and {alim} are negative or 0, the timing figure named {figname} will be used.
  eg. ttv_ComputeMaxPathSlope(circuit, {reset[3]}, {internalreset}, {ud}, 200, 1e-15, 2.5, 70) returns the slope of {internalreset} for the maximum path between {reset[3]} rising and {internalreset} falling with input slope of 200ps on {reset[3]} and 1fF on {internalreset} when voltage is 2.5V and temperature 70C.
  Returns 0 on failure.
*/
double ttv_ComputeMaxPathSlope(char *figure, char *start, char *end, char *dir, double input_slope, double output_capa, double VDD, double temperature);

/*
  MAN ttv_ComputeMinPathDelay
  CATEG tcl+pvt
  DESCRIPTION
  Re-computes the delay of the minimum path from {start} to {end} with transitions {dir} on 
  {start} and {end}, from the timing figure {figname}, with voltage {alim} and temperature {temp}. {in_slope} is applied on {start} and {out_capa} is set on {end}. If {temp} and {alim} are negative or 0, the timing figure {figname} is used.
  eg. ttv_ComputeMaxPathDelay(circuit, {reset[3]}, {internalreset}, {ud}, 200, 1e-15, 2.5, 70) returns the delay between {reset[3]} rising and {internalreset} falling with an input slope of 200ps and 1fF on {internalreset} when voltage is 2.5V and temperature 70C.
  Returns 0 on failure.
*/
double ttv_ComputeMinPathDelay(char *figure, char *start, char *end, char *dir, double input_slope, double output_capa, double VDD, double temperature);

/*
  MAN ttv_ComputeMinPathSlope
  CATEG tcl+pvt
  DESCRIPTION
  Returns 0 on failure.
  Re-computes the slope of the minimum path from {start} to {end} with transitions  {dir} on {start} and {end}, from the timing figure {figname}, with voltage {alim} and temperature {temp}. {in_slope} is applied on {start} and {out_capa} is set on {end}. If {temp} and {alim} are negative or 0, the timing figure named {figname} will be used.
  eg. ttv_ComputeMaxPathSlope(circuit, {reset[3]}, {internalreset}, {ud}, 200, 1e-15, 2.5, 70) returns the slope of {internalreset} for the minimum path between {reset[3]} rising and {internalreset} falling with input slope of 200ps on {reset[3]} and 1fF on {internalreset} when voltage is 2.5V and temperature 70C.
*/
double ttv_ComputeMinPathSlope(char *figure, char *start, char *end, char *dir, double input_slope, double output_capa, double VDD, double temperature);

/*
  MAN ttv_GetPathMaxDelay
  CATEG tcl+pvt
  DESCRIPTION
  Returns the maximum delay path from {start} to {end} with transitions {dir} on {start} and {end}, fromthe timing figure {figname}, with voltage {alim} and temperature {temp}.
  eg. ttv_GetPathMaxDelay(circuit, {reset[3]}, {internalreset}, {ud}, 2.5, 70) returns the delay of the maximum delay path between {reset[3]} rising and {internalreset} falling when voltage is 2.5V and temperature 70C.
  Returns 0 on failure.
*/
double ttv_GetPathMaxDelay(char *figure, char *start, char *end, char *dir, double VDD, double temperature);

/*
  MAN ttv_GetPathMaxSlope
  CATEG tcl+pvt
  DESCRIPTION
  Returns the slope attached to the maximum path from {start} to {end} with transitions {dir} on {start} and {end}, from the timing figure {figname}, with voltage {alim} and temperature {temp}.
  eg. ttv_GetPathMaxSlope(circuit, {reset[3]}, {internalreset}, {ud}, 2.5, 70) returns the slope of {internalreset} for the maximum delay path between {reset[3]} rising and {internalreset} falling when voltage is 2.5V and temperature 70C.
  Returns 0 on failure.
*/
double ttv_GetPathMaxSlope(char *figure, char *start, char *end, char *dir, double VDD, double temperature);

/*
  MAN ttv_GetPathMinDelay
  CATEG tcl+pvt
  DESCRIPTION
  Returns the minimum delay path from {start} to {end} with transitions {dir} on {start} and {end}, from the timing figure {figname}, with voltage {alim} and temperature {temp}.
  eg. ttv_GetPathMinDelay(circuit, {reset[3]}, {internalreset}, {ud}, 2.5, 70) returns the delay of the minimum delay path between {reset[3]} rising and {internalreset} falling when voltage is 2.5V and temperature 70C.
  Returns 0 on failure.
*/
double ttv_GetPathMinDelay(char *figure, char *start, char *end, char *dir, double VDD, double temperature);

/*
  MAN ttv_GetPathMinSlope
  CATEG tcl+pvt
  DESCRIPTION
  Returns the slope attached to the minimum path from {start} to {end} with transitions {dir} on {start} and {end}, from the timing figure {figname}, with voltage {alim} and temperature {temp}.
  eg. ttv_GetPathMinSlope(circuit, {reset[3]}, {internalreset}, {ud}, 2.5, 70) returns the slope of {internalreset} for the minimum delay path between {reset[3]} rising and {internalreset} falling when voltage is 2.5V and temperature 70C.
  Returns 0 on failure.
*/
double ttv_GetPathMinSlope(char *figure, char *start, char *end, char *dir, double VDD, double temperature);

/***********************************************************************************************/
/***********************************************************************************************/
/* TimingFigure  */
/***********************************************************************************************/
/***********************************************************************************************/

/*
    MAN ttv_LoadSpecifiedTimingFigure
    CATEG tcl+dtb
    DESCRIPTION
    Reads a DTX timing figure from disk.
    ARGS
    name % Timing figure's name. The name of the timing figure should be the same as its file name, without the {dtx} suffix.
    EXAMPLE % {set fig [ttv_LoadSpecifiedTimingFigure "my_design"]}
*/
TimingFigure *ttv_LoadSpecifiedTimingFigure(char *name);


/*
    MAN ttv_LoadSpecifiedTimingPathFigure
    CATEG tcl+dtb
    DESCRIPTION
    Reads a TTX timing path figure from disk.
    ARGS
    name % Timing figure's name. The name of the timing figure should be the same as its file name, without the {ttx} suffix.
    EXAMPLE % {set fig [ttv_LoadSpecifiedTimingPathFigure "my_design"]}
*/
TimingFigure *ttv_LoadSpecifiedTimingPathFigure(char *name);

/*
    MAN ttv_LoadCrosstalkFile
    CATEG tcl+dtb
    DESCRIPTION
    Loads a CTX file, result of a previous crosstalk analysis, and annotates a timing figure with crosstalk information such as delta-delays. The CTX file should have the same name of the timing figure, with the {ctx} extension. Returns 1 on failure, 0 on success.
    ARGS
    tvf % Pointer on the timing figure to annotate
    EXAMPLE % {ttv_LoadCrosstalkFile $fig}
*/
int ttv_LoadCrosstalkFile(TimingFigure *tvf);

/*
    MAN ttv_RecomputeDelays
    CATEG tcl+dtb
    DESCRIPTION
    Recomputes delays and slopes in a timing figure according to new input slopes and output capacitances.
    ARGS
    tvf % Pointer on the timing figure
    EXAMPLE % {ttv_RecomputeDelays $fig}
*/
void ttv_RecomputeDelays(TimingFigure *tvf);

/***********************************************************************************************/
/***********************************************************************************************/
/* TimingFigure Properties  */
/***********************************************************************************************/
/***********************************************************************************************/
Property *ttv_GetTimingFigureProperty (TimingFigure *fig, char *code);
/*
    MAN ttv_GetTimingFigureProperty
    CATEG tcl+dtb
    DESCRIPTION
    Returns a property of a timing figure. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    fig %  Pointer on the timing figure 
    code % Property code; for available property codes, see the {TimingFigure} object section
*/

TimingFigure *ttv_GetTopTimingFigure(TimingFigure *tvf);
double ttv_GetTimingFigureTemperature(TimingFigure *tf);
double ttv_GetTimingFigureSupply(TimingFigure *tf);
double ttv_GetTimingFigureOutputCapacitance(TimingFigure *tf);
double ttv_GetTimingFigureInputSlope(TimingFigure *tf);
double ttv_GetTimingFigureLowThreshold(TimingFigure *tf);
double ttv_GetTimingFigureHighThreshold(TimingFigure *tf);
double ttv_GetTimingFigureThreshold(TimingFigure *tf);
char *ttv_GetTimingFigureTechno(TimingFigure *tf);
char *ttv_GetTimingFigureGenerationDate(TimingFigure *tf);
char *ttv_GetTimingFigureGenerationTime(TimingFigure *tf);

/***********************************************************************************************/
/***********************************************************************************************/
/* List * of TimingPath  */
/***********************************************************************************************/
/***********************************************************************************************/

TimingPathList *ttv_RemoveIncludedSmallerPath(TimingPathList *lpl, TimingPathList *spl);
TimingPathList *ttv_GetMaxTimingPathFrom(TimingSignal *start, char dir);
TimingPathList *ttv_GetMaxTimingPathTo(TimingSignal *end, char dir);
TimingPathList *ttv_GetMaxTimingPath(TimingSignal *start,TimingSignal *end, char *dir);
TimingPathList *ttv_GetMinTimingPathFrom(TimingSignal *start, char dir);
TimingPathList *ttv_GetMinTimingPathTo(TimingSignal *dest, char dir);
TimingPathList *ttv_GetMinTimingPath(TimingSignal *start,TimingSignal *end, char *dir);
TimingPathList *ttv_GetCriticPathList(TimingFigure *tf, char *start, char *end, char *dir, long number, char *minmax);
TimingPathList *ttv_GetUnsortedCriticMaxPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetUnsortedCriticMinPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList* ttv_GetCriticMaxAccessList(TimingFigure *tf, char *start, char *end, char *clock, char *dir, long number);
TimingPathList* ttv_GetCriticMinAccessList(TimingFigure *tf, char *start, char *end, char *clock, char *dir, long number);
TimingPathList* ttv_GetAllMaxAccessList(TimingFigure *tf, char *start, char *end, char *clock, char *dir, long number);
TimingPathList* ttv_GetAllMinAccessList(TimingFigure *tf, char *start, char *end, char *clock, char *dir, long number);
TimingPathList* ttv_GetMaxAccess(TimingSignal *sig, TimingSignal *end, char *clock, char *dir );
TimingPathList* ttv_GetMinAccess(TimingSignal *sig, TimingSignal *end, char *clock, char *dir );
TimingPathList* ttv_GetAllMaxAccess(TimingSignal *sig, TimingSignal *end, char *clock, char *dir, long number);
TimingPathList* ttv_GetAllMinAccess(TimingSignal *sig, TimingSignal *end, char *clock, char *dir, long number );
TimingPathList *ttv_GetCriticMaxPath(TimingSignal *start, TimingSignal *end, char *dir, long number);
TimingPathList *ttv_GetCriticMinPath(TimingSignal *start, TimingSignal *end, char *dir, long number);
TimingPathList *ttv_GetAllMaxPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetAllMinPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetAllMaxPath(TimingSignal *start, TimingSignal *end, char *dir, long number);
TimingPathList *ttv_GetAllMinPath(TimingSignal *start, TimingSignal *end, char *dir, long number);

/*
    MAN ttv_GetPaths
    CATEG tcl+path
    SYNOPSIS
    <TimingPathList *> ttv_GetPaths [<TimingFigue *tf>] [-from <startnodelist>] [-to <endnodelist>] [-thru <accessnodelist>] [-dir <dir>] [-nb <nb>] [-critic|-all] [-path|-access] [-max|-min]\$
    \$
    Obsolete but still working:\$
    TimingPathList *ttv_GetPaths(TimingFigure *tf, char *clock, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax);\$
    DESCRIPTION
    Gets a selection of paths/accesss, depending upon configuration.\$
    If {tf} is not specified then {startnodelist}, {endnodelist} and {accessnodelist} are considered {TimingSignal} lists. They are node name lists otherwise.\$
    The {accessnodelist} is used only if {-access} is specified.\$
    ARGS
    tf % Related timing figure to use.
    -from {<startnodelist>} % Path start node list (clock node if {-access} is used). If not specified, all possible start nodes are considered.
    -thru {<accessnodelist>} % Path access node list (latchs, precharges, breakpoints). Used only with {-access}. If not specified, all possible access nodes are considered.
    -to {<endnodelist>} % Path end node list (latchs, precharges, breakpoints, connectors). If not specified, all possible end nodes are considered.
    -dir {<dir>} % Path transitions: {uu}, {dd}, {ud}, {du}; {u} or {d} can be replaced by the wildcard {?}.  {z} or {/} can be added to the usual direction to respectively retreive only HZ path or only non-HZ path. Default is {??}.
    -nb {<nb>} % Maximum number of paths that will appear in the returned list. If {<nb>} is negative or zero, no path number limit will be considered. Default is {-1}.
    -critic % Returns only critical paths. This is the default.
    -all % Returns critical and parallel paths.
    -path % Searches for paths. This is the default.
    -access % Searches for accesses.
    -max % Uses maximum delays for the search. This is the default.
    -min % Uses minimum delays for the search.
    Obsolete options: %
    start % Path starting point (clock signal for an access path) 
    end   % Path destination point 
    dir   % Path transitions: {uu}, {dd}, {ud}, {du}; {u} or {d} can be replaced by the wildcard {?}.  {z} or {/} can be added to the usual direction to respectively retreive only HZ path or only non-HZ path.
    number % Number of paths/access that will appear in the returned list. If {number} is negative, no path number limit will be considered 
    all % {all} or {critic}: all paths appear in the returned list or only critical paths disregarding parallel paths
    path % Type of paths to return, either {path} or {access}
    minmax % Maximum ({max}) or minimum ({min}) paths are returned
    EXAMPLE % {set p_list [ttv_GetPaths $fig -to \{*outsig* out\} -dir ?r -nb 10 -critic -path -min]}\$or with Timing Signals myclock1 and myclock2:\${set p_list [ttv_GetPaths -from \{$myclock1 $myclock2\} -access]}\$Obsolete use:\${set p_list [ttv_GetPaths $fig * * ?? 10 critic path max]}
*/
void ttv_GetPaths(void); // void to avoid gns wrappers
TimingPathList *ttv_GetPaths_sub (TimingFigure *tf, char *clock, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax);
TimingPathList *ttv_internal_GetPaths_EXPLICIT(TimingFigure *tf, TimingSignalList *clock, TimingSignalList *start, TimingSignalList *end, char *dir, long number, char *all, char *path, char *minmax);

/*
    MAN ttv_CharacPaths
    CATEG tcl+path
    DESCRIPTION
    Search for timing paths. Path delay is computed with regard to input slope and output load.
    ARGS
    tf % Related timing figure to be driven
    slopein % Path input slope to be propagated, if 0, default slope is taken into account.
    start % Path starting point (clock signal for an access path) 
    end   % Path destination point. 
    dir   % Path transitions: {uu}, {dd}, {ud}, {du}; {u} or {d} can be replaced by the wildcard {?}
    number % Number of paths/access that will appear in the returned list. If {number} is negative, no path number limit will be considered
    all % {all} or {critic}: all paths appear in the returned list or only critical paths disregarding parallel paths
    path % Type of paths to return, either {path} or {access}
    minmax % Maximum ({max}) or minimum ({min}) paths are returned
    capaout % Load to add to output connectors (on-path bidir connectors affected only in full propagation, see below). If negative load, default is taken into account. 
    propagate % Type of slope propagation, can take values {0} (no propagation), {1} (full propagation) or {2} (1-stage propagation) 
    EXAMPLE % {set p_list [ttv_CharacPaths $fig 100e-12 a b ud 1 critic path max 10e-15 1]}
*/
TimingPathList *ttv_CharacPaths(TimingFigure *tf, double slopein, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax, double capaout, int propagate);

/*
    MAN ttv_GetParallelPaths
    CATEG tcl+path
    DESCRIPTION
    Returns a list of paths parallel to a given path, i.e. starting end ending on the same nodes, with same transitions, but crossing different intermediary nodes.  
    ARGS
    ta % Pointer to a timing path
    num % Max number of parallel paths to report
    EXAMPLE % {set pr_list [ttv_GetParallelPaths [lindex $p_list 1] 10]}
*/
TimingPathList *ttv_GetParallelPaths (TimingPath *ta, long num);

/*
    MAN ttv_ProbeDelay
    CATEG tcl+path
    SYNOPSIS
    TimingPathList *ttv_ProbeDelay <TimingFigure *tf> <StringList *nodenamelist> [flags]\$\$
    Obsolete but still working:\$
    TimingPathList *ttv_ProbeDelay(TimingFigure *tf, double inputslope, StringList *nodenamelist, char *dir, int nbpaths, char *path_or_access, char *minmax)
    DESCRIPTION
    Retreives the paths by specifying a list of nodes on the paths without the need for intermediate nodes to be forcibly path stop nodes (latches, precharges, connectors, ...).\$
    The type of the crossed nodes can be specified as wildcards can be used to indicate a set of nodes.
    ARGS
    tf             % Timing figure 
    nodenamelist   % Nodes that the path must cross. Must be present at least the start node, all memory/intermediate nodes on the path in the right order and the end node.\$The types of the nodes can be specified by adding the prefixes: "{-any=}", "{-latch=}", "{-prech=}" and "{-con=}". Respectively for any node, latch, precharge and connector.\$If wildcards are used for the intermediate nodes without precising the node types, "-latch=" is assumed.\$Using "-any=" for intermediate nodes can be very time consuming because all the matching nodes combinations will be considered. Adding {-access} before one of this option  (eg. -access-latch) will indicate a probe thru the access timing line to the specified node. 
    flags:         %
    -dir {<dir>}   % Requested start-to-end transition. Default is {??} (all transitions).
    -slope {<inputslope>} % Value of the input slope to propagate through the path. If negative, the slope computed during database creation will be propagated.
    -nb {<maxpath>} % If positive maximum number of paths returned. Default, is {-1} (unlimited).
    -min           % Search for minimum-delay paths
    -max           % Search for maximum-delay paths. This is the default.
    -nosync        % Disable synchornization to the latch opening when crossing latches if stb has been run on the timing figure. 
    -noprop        % Disable slope propagation thru the found path gates. {<inputslope>} is ignored.
    EXAMPLE % To probe a delay from node "net23" to node "net045" thru node "lt1":\${set p_list [ttv_ProbeDelay $fig \{net23 lt1 net045\} -slope 100e-12 -dir ud -nbpaths 1]}\$To probe paths from all connectors to node "net045" thru any latch:\${set p_list [ttv_ProbeDelay $fig \{-con=* -latch=* net045\} -slope 100e-12]}.\$To probe the minimum accesses from connector "ck" thru any latch to node "endnode":\${set p_list [ttv_ProbeDelay $fig \{-con=ck -access-latch=* endnode\} -min]}.
*/
TimingPathList *ttv_ProbeDelay_sub(TimingFigure *tf, double slopein, StringList *nodenamelist, char *dir, int nbpath, char *path_or_access, char *minmax, int nosync, int noprop);
void ttv_ProbeDelay(void); // (void) disables the gns wrappers

/*
    MAN ttv_FreePathList
    CATEG tcl+path
    DESCRIPTION
    Frees the paths in a list
    ARGS
    lst % Pointer on the head of a path list
    EXAMPLE % {ttv_FreePathList $p_list}
*/
void ttv_FreePathList(TimingPathList *lst);


/*
    MAN ttv_SearchExcludeNodeType
    CATEG tcl+path
    DESCRIPTION
    Allows the exclusion of nodes of a given type for path searching
    ARGS
    conf % List of node types separated by spaces. Valid types are {Command}, {Precharge}, {Latch}, {Connector} and {Breakpoint}.
    EXAMPLE % {ttv_SearchExcludeNodeType "Command Precharge"}
*/

void ttv_SearchExcludeNodeType(char *conf);

/*
  MAN ttv_CharacPathTables
  CATEG tcl+path
  DESCRIPTION
  Recompute path delay and output slope with all the combinations of slopes and capacitances.
  Returns a list of 2 matrices (list of lists). The first list contains the delays and the second the slopes. Each line correspond to an input slope and each element of a line to a capacitance.
  ARGS
  pth % Timing path to recompute
  slopes % List of slope values to use
  capas % List of capacitance values to use
  propagate % type of slope propagation, can take values {0} (no propagation), {1} (full propagation)) 
  EXAMPLE % {set res [ttv_CharacPathTables $mypath {`1e-12 3e-12}` {`1e-15 2e-15}` 1]}
*/
ListOfDoubleTable *ttv_CharacPathTables(TimingPath *pth, DoubleListTimeValue *slopes, DoubleListCapaValue *capas, int propagate);


/*
  MAN ttv_CreateTimingTableModel
  CATEG tcl+generation
  DESCRIPTION
  Creates a timing table model using the given axis and table values.\$
  For GNS usage, set {tvf} to NULL 
  ARGS
  tvf % Timing figure where table model will be created
  name % Name of the table model
  axis1 % Y-axis values of the table
  axis2 % X-axis values of the table
  values % Values to set into the table. It's a 2D array.
  type % defines the type of the table axis: "slope-slope", "slope-capa" or "slope-ckslope". 
  EXAMPLE % {ttv_CreateTimingTableModel $fig mymodel {`1e-12 3e-12}` {`1e-15 2e-15}` {`{`1e-12 2e-12}`{`3e-12 4e-12}`}` "slope-capa"}
*/
void ttv_CreateTimingTableModel(TimingFigure *tvf, char *name, DoubleListTimeValue *axis1, DoubleListCapaValue *axis2, DoubleTable *values, char *type);

/*
  MAN ttv_CreateEnergyTableModel
  CATEG tcl+generation
  DESCRIPTION
  Associate an energy table model using the given axis and table values.\$
  For GNS usage, set {tvf} to NULL 
  ARGS
  tvf % Timing figure where table model will be created
  name % Name of the table model
  axis1 % Y-axis values of the table
  axis2 % X-axis values of the table
  values % Values to set into the table. It's a 2D array.
  type % defines the type of the table axis: "slope-slope", "slope-capa" or "slope-ckslope". 
  EXAMPLE % {ttv_CreateTimingTableModel $fig mymodel {`1e-12 3e-12}` {`1e-15 2e-15}` {`{`1e-12 2e-12}`{`3e-12 4e-12}`}` "slope-capa"}
*/
void ttv_CreateEnergyTableModel(TimingFigure *tvf, char *name, DoubleListTimeValue *axis1, DoubleListCapaValue *axis2, DoubleTable *values, char *type);

/***********************************************************************************************/
/***********************************************************************************************/
/* TimingPaths Properties  */
/***********************************************************************************************/
/***********************************************************************************************/
double ttv_GetPathDelay(TimingPath *ta);
double ttv_GetPathRefDelay(TimingPath *ta);
double ttv_GetPathDataLag(TimingPath *ta);
double ttv_GetPathSlope(TimingPath *ta);
double ttv_GetPathRefSlope(TimingPath *ta);
double ttv_GetPathStartTime(TimingPath *ta);
double ttv_GetPathStartSlope(TimingPath * ta);
int ttv_PathIsHZ(TimingPath *ta);
TimingSignal *ttv_GetPathStartSignal(TimingPath *ta);
TimingSignal *ttv_GetPathEndSignal(TimingPath *ta);
TimingEvent *ttv_GetPathCommand(TimingPath *ta);
TimingEvent *ttv_GetPathAccessLatchCommand(TimingPath *ta);
char ttv_GetPathStartDirection(TimingPath *ta);

TimingPathList *ttv_SortPathList( TimingPathList *path, char *order );
char ttv_GetPathEndDirection(TimingPath *ta);

/***********************************************************************************************/
/***********************************************************************************************/
/* TimingSignals  */
/***********************************************************************************************/
/***********************************************************************************************/

/*
    MAN ttv_GetTimingSignalListByNet
    CATEG tcl+sig
    DESCRIPTION
    Retreives a timing signal list from a net name
    ARGS
    tvf % Pointer on a timing figure
    name % Name of the timing signal to retrieve. Should repect the naming conventions defined with {avtCaseSensitive} and {avtVectorize} configuration variables.
    EXAMPLE % {set siglist [ttv_GetTimingSignalListByNet $fig net045]}
*/
TimingSignalList *ttv_GetTimingSignalListByNet(TimingFigure *tvf, char *name);

/*
    MAN ttv_GetTimingSignal
    CATEG tcl+sig
    DESCRIPTION
    Retreives a timing signal from the timing figure; returns NULL on failure.
    ARGS
    tvf % Pointer on a timing figure
    name % Name of the timing signal to retrieve. Should repect the naming conventions defined with {avtCaseSensitive} and {avtVectorize} configuration variables.
    EXAMPLE % {set sig [ttv_GetTimingSignal $fig net045]}
*/
TimingSignal *ttv_GetTimingSignal(TimingFigure *tvf, char *name);


/*
    MAN ttv_GetTimingSignalList
    CATEG tcl+sig
    DESCRIPTION
    Retreives a list of timing signals of a given type and location
    ARGS
    tvf % Pointer on a timing figure
    type % {connector}, {precharge}, {latch}, {command} or {breakpoint}
    location % {interface}, {internal} or {all}
    EXAMPLE % {set ext_latches [ttv_GetTimingSignalList $fig latch interface]}
*/
TimingSignalList *ttv_GetTimingSignalList(TimingFigure *tvf, char *type, char *location);

/*
  presque redondant avec le ttv_GetTimingSignalList
*/
TimingSignalList *ttv_GetMatchingSignal(TimingFigure *tf, char *name, char *type);

TimingSignalList *ttv_GetConnectorList(TimingFigure *tf);
TimingSignalList *ttv_GetInterfaceBreakpointList(TimingFigure *tvf);
TimingSignalList *ttv_GetInterfaceLatchList(TimingFigure *tvf);
TimingSignalList *ttv_GetInterfaceCommandList(TimingFigure *tvf);
TimingSignalList *ttv_GetInterfacePrechargeList(TimingFigure *tvf);
TimingSignalList *ttv_GetInternalBreakpointList(TimingFigure *tvf);
TimingSignalList *ttv_GetInternalLatchList(TimingFigure *tvf);
TimingSignalList *ttv_GetInternalCommandList(TimingFigure *tvf);
TimingSignalList *ttv_GetInternalPrechargeList(TimingFigure *tvf);

/*
    MAN ttv_GetClockList
    CATEG tcl+sig
    DESCRIPTION
    Returns the list of signals marked as clock in a given timing figure, with a {create_clock} command for example.
    Returns an empty list if the timing figure is NULL, or if there are no clock connectors.
    ARGS
    tvf % Pointer on a timing figure
    EXAMPLE % {set clock_list [ttv_GetClockList $fig]}
*/
extern TimingSignalList *ttv_GetClockList (TimingFigure *tvf);

/***********************************************************************************************/
/***********************************************************************************************/
/* TimingSignals Properties */
/***********************************************************************************************/
/***********************************************************************************************/

TimingFigure *ttv_GetSignalTopTimingFigure(TimingSignal *tvs);

/*
    MAN ttv_GetTimingSignalProperty
    CATEG tcl+sig
    DESCRIPTION
    Returns a property of a timing signal. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    fig %  Pointer on the timing signal
    code % Property code; for available property codes, see the {TimingSignal} object section
*/
Property *ttv_GetTimingSignalProperty (TimingSignal *fig, char *code);

/*
    MAN ttv_GetLatchAccess
    CATEG tcl+sig
    DESCRIPTION
    Returns a latch's intrinsic access delay, from command to latch output
    ARGS
    tvf % Pointer on a timing figure
    latch % Pointer on the latch to consider
    dir % Indicates that the access delay is given for the {falling} or {rising} edge of the latch node. Use {both} to be able to choose between min and  max values.
    command % Associated command node; if NULL, all commands will be considered
    minmax % {min} or {max}; defines if min or max value is to be chosen when there are multiple choices. 
    EXAMPLE % {set intrinsic [ttv_GetLatchAccess $fig $lt1 falling $com1 min]}
*/
double ttv_GetLatchAccess(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command, char *minmax);

/*
    MAN ttv_GetLatchSetup
    CATEG tcl+sig
    DESCRIPTION
    Returns a latch's intrinsic setup constraint, from latch input to command
    ARGS
    tvf % Pointer on a timing figure
    latch % Pointer on the latch to consider
    dir % Indicates that the setup constraint is given for the {falling} or {rising} edge of the latch node. Use {both} to be able to choose between min and  max values.
    command % Associated command node; if NULL, all commands will be considered
    minmax % {min} or {max}; defines if min or max value is to be chosen when there are multiple choices. 
    EXAMPLE % {set intrinsic [ttv_GetLatchSetup $fig $lt1 falling $com1]}
*/
double ttv_GetLatchSetup(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command);

/*
    MAN ttv_GetLatchHold
    CATEG tcl+sig
    DESCRIPTION
    Returns a latch's intrinsic hold constraint, from latch input to command
    ARGS
    tvf % Pointer on a timing figure
    latch % Pointer on the latch to consider
    dir % Indicates that the hold constraint is given for the {falling} or {rising} edge of the latch node. Use {both} to be able to choose between min and  max values.
    command % Associated command node; if NULL, all commands will be considered
    minmax % {min} or {max}; defines if min or max value is to be chosen when there are multiple choices. 
    EXAMPLE % {set intrinsic [ttv_GetLatchHold $fig $lt1 falling $com1]}
*/

double ttv_GetLatchHold(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command);
int ttv_GetNameIndex(char *sig);

char *ttv_GetNameRadical(char *sig);

/*
    MAN ttv_GetSignalCapaList
    CATEG tcl+sig
    DESCRIPTION
    Returns the list of capacitance values (in Farads) for a given timing signal. Values are given in the order: nominal, min and max for rise transition then nominal, min and max for fall transition.
    ARGS
    tvs % Pointer on the timing signal to consider
    EXAMPLE % {set capa_list [ttv_GetSignalCapaList]}
*/
DoubleList *ttv_GetSignalCapaList (TimingSignal *tvs);

/*
    MAN ttv_GetFullSignalNetName
    CATEG tcl+sig
    DESCRIPTION
    Returns the full hierarchical net name of a given timing signal. Returned name is to be pasted for further use. Up to 16 full names are kept in order to reduce memory consumption.
    ARGS
    tvf % Pointer on a timing figure
    tvs % Pointer on the timing signal to consider
    EXAMPLE % {set name [ttv_GetFullSignalNetName $fig $sig]}
*/
char *ttv_GetFullSignalNetName(TimingFigure *tvf, TimingSignal *tvs);

/*
    MAN ttv_GetFullSignalName
    CATEG tcl+sig
    DESCRIPTION
    Returns the full hierarchical name of a given timing signal. Returned name is to be pasted for further use. Up to 16 full names are kept in order to reduce memory consumption.
    ARGS
    tvf % Pointer on a timing figure
    tvs % Pointer on the timing signal to consider
    EXAMPLE % {set name [ttv_GetFullSignalName $fig $sig]}
*/
char *ttv_GetFullSignalName(TimingFigure *tvf, TimingSignal *tvs);
char *ttv_GetSignalType(TimingSignal *tvs);



/***********************************************************************************************/
/***********************************************************************************************/
/* TimingEvent */
/***********************************************************************************************/
/***********************************************************************************************/

/*
    MAN ttv_GetTimingEventProperty
    CATEG tcl+event
    DESCRIPTION
    Returns a property of a timing event. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    ev %  Pointer on the timing event
    code % Property code; for available property codes, see the {TimingEvent} object section
*/

Property *ttv_GetTimingEventProperty (TimingEvent *ev, char *code);

/*
    MAN ttv_GetLatchCommands
    CATEG tcl+event
    DESCRIPTION
    Returns the list of commands enabling writing in a given latch
    ARGS
    ts % Pointer on the latch to consider
    EXAMPLE % {set com_list [ttv_GetLatchCommands $lt1]}
*/
TimingEventList *ttv_GetLatchCommands(TimingSignal *tvs);

/*
    MAN ttv_GetLatchEventCommands
    CATEG tcl+event
    DESCRIPTION
    Returns the list of commands enabling writing of a specific transition in a given latch
    ARGS
    tvs % Pointer on the timing signal to consider
    dir % {u} or {d}; transition to be written
    EXAMPLE % {set com_list [ttv_GetLatchCommands $lt1 u]}
*/
TimingEventList *ttv_GetLatchEventCommands(TimingSignal *tvs, char dir);

TimingSignal *ttv_GetTimingEventSignal(TimingEvent *tve);
char ttv_GetTimingEventDirection(TimingEvent *tve);

/***********************************************************************************************/
/***********************************************************************************************/
/* Path Detail */
/***********************************************************************************************/
/***********************************************************************************************/

char *ttv_AutomaticDetailBuild(char *mode);

TimingDetailList *ttv_GetParallelPathDetail (TimingPath *ta);

/*
    MAN ttv_GetPathDetail
    CATEG tcl+detail
    DESCRIPTION
    Returns the list of elementary timing arcs (lines) making up a critical path
    ARGS
    ta % Pointer on the timing path to consider
    EXAMPLE % {set detail [ttv_GetPathDetail $path]}
*/
TimingDetailList *ttv_GetPathDetail (TimingPath *ta);
void ttv_FreePathDetail(TimingDetailList *detail);
void ttv_FreeParallelPathDetail(TimingDetailList *detail);

/***********************************************************************************************/
/***********************************************************************************************/
/* Detail Properties */
/***********************************************************************************************/
/***********************************************************************************************/
/*
    MAN ttv_GetTimingDetailProperty
    CATEG tcl+detail
    DESCRIPTION
    Returns a property of a timing detail. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    ev %  Pointer on the timing detail
    code % Property code; for available property codes, see the {TimingDetail} object section
*/
Property *ttv_GetTimingDetailProperty (TimingDetail *det, char *property);

int ttv_DetailIsHZ(TimingDetail *td);
char *ttv_GetDetailNodeName (TimingDetail *td);
char *ttv_GetDetailSignalName (TimingDetail *td);
char *ttv_GetDetailSignalType(TimingDetail *td);
double ttv_GetDetailDelay (TimingDetail *td);
double ttv_GetDetailSlope (TimingDetail *td);
double ttv_GetDetailRefDelay(TimingDetail *td);
double ttv_GetDetailRefSlope(TimingDetail *td);
double ttv_GetDetailSimDelay(TimingDetail *td);
double ttv_GetDetailSimSlope(TimingDetail *td);
double ttv_GetDetailDataLag(TimingDetail *td);
char *ttv_GetDetailType (TimingDetail *td);
char ttv_GetDetailDirection (TimingDetail *td);

/*
    MAN xxxttv_GetCrossedTransistorNames
    CATEG tcl+detail
    DESCRIPTION
    Returns the list of transistors the propagation of a signal is crossing. The cone netlist ({.cns} file) must have been generated.
    Returns a list of names.
    ARGS
    tvf % Pointer on a timing figure, used to retrieve the appropriate cone netlist
    namein % Start node of the signal's propagation
    dirin % Transition on the start node; {u} or {d}
    nameout % Arrival node of the signal's propagation
    dirout % Transition on the start node; {u} or {d}
    hzflag % If 0, returns the list of transistors activated (if 1 deactivated) by the signal's propagation
*/

List *ttv_GetCrossedTransistorNames(TimingFigure *tvf, char *namein, char dirin, char *nameout, char dirout, int hzflag);
List *__ttv_GetCrossedTransistors(TimingFigure *tvf, char *namein, char dirin, char *nameout, char dirout, int hzflag);


/***********************************************************************************************/
/***********************************************************************************************/
/* STDOUT Reports */
/***********************************************************************************************/
/***********************************************************************************************/

/*
    MAN ttv_DisplayPathList
    CATEG tcl+report
    DESCRIPTION
    Prints summary information about paths given in a path list
    ARGS
    tpl % Pointer on a path list
    f % File where to save the report, {stdout} for standard output
*/
void ttv_DisplayPathList(FILE *f, TimingPathList *tpl);

/*
    MAN ttv_DisplayPathListDetail
    CATEG tcl+report
    DESCRIPTION
    Prints detailed information about paths given in a path list
    ARGS
    tpl % Pointer on a path list
    f % File where to save the report, {stdout} for standard output
*/
void ttv_DisplayPathListDetail(FILE *f, TimingPathList *tpl);

//void ttv_DisplayPath(FILE *f, int num, TimingPath *tp);

/*
    MAN ttv_DisplayPathDetail
    CATEG tcl+report
    DESCRIPTION
    Prints detailed information about a given path
    ARGS
    tp % Pointer on a path
    f % File where to save the report, {stdout} for standard output
    num % Number to identify the path
*/
void ttv_DisplayPathDetail(FILE *f, int num, TimingPath *tp);

void ttv_DisplayCompletePathDetail(FILE *f, int num, TimingPath *tp, TimingDetailList *detail);

/*
    MAN ttv_DisplayPathDetailShowColumn
    CATEG tcl+report
    DESCRIPTION
    Selects the columns to be displayed in the reports provided by {ttv_DisplayPath}.
    By default, all columns are displayed when available information. Wild cards can be used, eg. {dt.*slope}. Prefix {pl.} refers to path reports, {.dt} to path detail reports, {c2l.} to connector to latch and {stab} to stability reports.
    ARGS
    conf % list of column IDs separated by spaces. Valid IDs are: {pl.index}, {pl.starttime}, {pl.startslope}, {pl.pathdelay}, {pl.totaldelay}, {pl.datalag}, {pl.endslope}, {pl.startnode}, {pl.endnode}, {dt.simacc}, {dt.simdelta}, {dt.simslope}, {dt.simerror}, {dt.refacc}, {dt.refdelta}, {dt.refslope}, {dt.reflagacc}, {dt.reflagdelta}, {dt.ctkacc}, {dt.ctkdelta}, {dt.ctkslope}, {dt.ctklagacc}, {dt.ctklagdelta}, {dt.capa}, {dt.nodetype}, {dt.nodename}, {dt.netname}, {dt.linetype}, {dt.transistors}, {dt.clockinfo}, {stab.from}, {stab.thru}, {c2l.absdata} or {all}. 
*/
void ttv_DisplayPathDetailShowColumn(char *conf);

/*
    MAN ttv_DisplayPathDetailHideColumn
    CATEG tcl+report
    DESCRIPTION
    Selects the columns not to be displayed in the reports provided by {ttv_DisplayPath}. 
    By default, all columns are displayed when available information. Wild cards can be used, eg. {dt.*slope}. Prefix {pl.} refers to path reports, {dt.} to path detail reports, {c2l.} to connector to latch and {stab} to stability reports. Hidding {dt.linetype} activates the merge of RC delays with gate delays.
    ARGS
    conf % list of column IDs separated by spaces. Valid IDs are: {pl.index}, {pl.starttime}, {pl.startslope}, {pl.pathdelay}, {pl.totaldelay}, {pl.datalag}, {pl.endslope}, {pl.startnode}, {pl.endnode}, {dt.simacc}, {dt.simdelta}, {dt.simslope}, {dt.simerror}, {dt.refacc}, {dt.refdelta}, {dt.refslope}, {dt.reflagacc}, {dt.reflagdelta}, {dt.ctkacc}, {dt.ctkdelta}, {dt.ctkslope}, {dt.ctklagacc}, {dt.ctklagdelta}, {dt.capa}, {dt.nodetype}, {dt.nodename}, {dt.netname}, {dt.linetype}, {dt.transistors}, {dt.clockinfo}, {stab.from}, {stab.thru}, {c2l.absdata} or {all}. 
*/
void ttv_DisplayPathDetailHideColumn(char *conf);


/*
    MAN ttv_DisplayClockPathReport
    CATEG tcl+report
    DESCRIPTION
    Prints a detailed report of timing paths originating from a clock.
    ARGS
    f % File where to save the report, {stdout} for standard output
    tf % Pointer on the timing figure to consider
    clock % Starting point name; can be a wildcard name
    minmax % {min} or {max}; type of path to be searched
    number % Maximum number of paths to report
*/
void ttv_DisplayClockPathReport(FILE *f, TimingFigure *tf, char *clock, char *minmax, int number);



/*
    MAN ttv_DisplayConnectorToLatchMargin
    CATEG tcl+report
    DESCRIPTION
    Prints a report displaying setup and hold constraints on latch points, originating from input connectors (both from rising and falling transitions). 
    ARGS
    f % File where to save the report, {stdout} for standard output
    tf % Pointer on the timing figure to consider
    inputconnector % Names of the connectors to consider, can be wildcards. 
    mode % Controls the amount of information displayed. Valid values are {summary} or {all} (display path detail) associated with {split}. Using {split} displays the report connector one connector at a time. This option can also be used to reduce memory usage on huge UTDs. {margins} will show the computed contsraints for each path in the summary. {pathcomp} permits will display the spice total error as total spice delay versus total tas delay + path margin. By default, the path margin is not included.
    EXAMPLE % {ttv_DisplayConnectorToLatchMargin $ofile $fig "*" "summary split"}
*/
void ttv_DisplayConnectorToLatchMargin (FILE *f, TimingFigure *tf, char *inputconnector, char *mode);

/*
    MAN ttv_PlotPathDetail
    CATEG tcl+report
    DESCRIPTION
    Plots the waveforms of nodes on a given path.
    ARGS
    tp % Pointer on a path
    f % File where to save the report, {stdout} for standard output
*/
void ttv_PlotPathDetail(FILE *f, TimingPath *tp);

void ttv_PlotCompletePathDetail(FILE *f, TimingPath *tp, TimingDetailList *detailchain);

/*
    MAN ttv_SetupReport
    CATEG tcl+report
    DESCRIPTION
    Changes units and format used in path reports. Default units are {ns} and {pf}. Formats affect the way most of the values are displayed in the report columns. By default, values are justified right, node and net names are displayed together, and delay deviations are given relatively to the path delay.  
    ARGS
    conf % List of names separated by spaces. Valid units are: {ns}, {ps}, {pf}, {ff} and {#}digits (which defines the precision for delay values, {#} ranging from 1 to 9). Valid formats are: {JustifyRight}, {JustifyLeft}, {OnlyNetName}, {NodeName}, {HideRC}, {ShowRC}, {LocalError} (which gives local delay deviations), {PathComp} (compares simulated delay versus hitas delay+path margin).
*/
void ttv_SetupReport(char *conf);

/*
    MAN ttv_DumpHeader
    CATEG tcl+report
    DESCRIPTION
    Prints information about the given timing figure and the running script
    ARGS
    f   % File where to save the report, {stdout} for standard output
    tvf % Pointer on a timing figure
*/
void ttv_DumpHeader(FILE *f, TimingFigure *tvf);

/*
    MAN ttv_DumpFigure
    CATEG tcl+report
    SYNOPSIS
    ttv_DumpFigure <FILE *f> <TimingFigure *tvf> [type]
    DESCRIPTION
    Prints a table with the Timing Lines of a TimingFigure.
    ARGS
    f   % File where to save the report, {stdout} for standard output
    tvf % Pointer on a timing figure
    type % Same type as ttv_GetLines API to filter output. Valid values: {setup}, {hold}, {access}, {hz}, {rc}, {prech}, {eval}, {data} or {all}.
*/
void ttv_DumpFigure(void); // (void) disables the gns wrappers


/***********************************************************************************************/
/***********************************************************************************************/
/* STDOUT Reports */
/***********************************************************************************************/
/***********************************************************************************************/

/*
    MAN ttv_SimulatePath
    CATEG tcl+sim
    SYNOPSIS
    ttv_SimulatePath <TimingFigure *tvf> <TimingPath *ta> [-force] [-mc <number>] [-plot]
    DESCRIPTION
    Constructs and saves on disk the spice netlist and stimuli required for simulation, relative to the given path. This requires all configuration variables related to simulation to be set correctly.  Returns 0 on success, 1 on error.
    ARGS
    tvf % Pointer on a timing figure
    path % Pointer on the timing path the spice deck is to be extracted from
    -force % The path will be simulated even if it has already been
    -mc {<number>} % The number of monte-carlo simulations to be performed
    -plot % Generated a waveform file for viewing with gnuplot or gwave. Note that {simUsePrint} must be set.
    
*/
void ttv_SimulatePath(void);
int ttv_SimulatePath_sub(TimingFigure *tvf, TimingPath *path, char *mode, int mc);
//int ttv_SimulatePathDetail(TimingFigure *tvf, TimingPath *path, char *mode);

/*
  MAN ttv_SimulateCharacPathTables
  CATEG tcl+sim
  DESCRIPTION
  Simulates path with all the combinations of slopes and capacitances values.
  Returns a matrix (list of lists) of paths. Each line corresponds to an input slope and each element of a line to a capacitance. Each path in the matrix is a replica of the user given path except for the simulated slopes and delays.
  ARGS
  tvf % Timing figure of the path
  tp % Timing path to simulate
  slopes % List of slope values to use
  capas % List of capacitance values to use
  maxsim % maximum number of concurent simulation authorized.
  EXAMPLE % {set res [ttv_SimulateCharacPathTables $myfig $mypath {`1e-12 3e-12}` {`1e-15 2e-15}` 3]}
*/
TimingPathTable *ttv_SimulateCharacPathTables(TimingFigure *tvf, TimingPath *tp, DoubleListTimeValue *slopes, DoubleListCapaValue *capas, int maxsim);

/*
    MAN ttv_DriveSpiceDeck
    CATEG tcl+sim
    DESCRIPTION
    Constructs and saves on disk the spice netlist and stimuli required for simulation, relative to a given path. This requires all configuration variables related to simulation to be set correctly.  Returns 0 on success, 1 on error.
    ARGS
    tvf % Pointer on a timing figure
    path % Pointer on the timing path the spice deck is to be extracted from
    filename % File where to save the report
*/

int ttv_DriveSpiceDeck(TimingFigure *tvf, TimingPath *path, char *filename);

/*
    MAN ttv_DriveSetupHoldSpiceDeck
    CATEG tcl+sim
    DESCRIPTION
    Constructs and saves on disk the spice netlist and stimuli required for simulation, relative to the computation of a setup or hold value. This requires all configuration variables related to simulation to be set correctly.  Returns 0 on success, 1 on error.
    ARGS
    tvf % Pointer on a timing figure
    datapath % Pointer on the data path the spice deck is to be extracted from
    clockpathlist % Pointer on the clock path the spice deck is to be extracted from
    filename % File where to save the report
*/

int ttv_DriveSetupHoldSpiceDeck(TimingFigure *tvf, TimingPathList *datapath, TimingPathList *clockpathlist, char *filename);

/*
    MAN ttv_DisplayActivateSimulation
    CATEG tcl+sim
    DESCRIPTION
    Enables the construction and simulation of the spice deck of a given path, when a reporting request occurs for this path. The simulation columns in the report appear automatically except explicitly disabled with {ttv_DisplayPathDetailHideColumn}. 
    ARGS
    mode % {yes} or {no} to activate or deactivate spice deck construction and simulation
*/
void ttv_DisplayActivateSimulation(char mode);

/*
    MAN ttv_GetTimingPathProperty
    CATEG tcl+path
    DESCRIPTION
    Returns a property of a timing path. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    fig %  Pointer on the timing path
    code % Property code; for available property codes, see the {TimingPath} object section
*/
Property *ttv_GetTimingPathProperty (TimingPath *fig, char *code);

/***********************************************************************************************/
/***********************************************************************************************/
/* TimingLines  */
/***********************************************************************************************/
/***********************************************************************************************/

/*
    MAN ttv_GetLines
    CATEG tcl+line
    DESCRIPTION
    Gets a selection of lines, depending upon configuration. 
    ARGS
    tf % Related timing figure
    start % Line starting point 
    end   % Line destination point 
    dir   % Line transitions: {uu}, {dd}, {ud}, {du}; {u} or {d} can be replaced by the wildcard {?}
    type  % Line type filters: {setup}, {hold}, {access}, {hz}, {rc}, {prech}, {eval}, {data} or {all}
    EXAMPLE % {set l_list [ttv_GetLines $fig * * ?? "access setup hold"]}
*/
TimingLineList *ttv_GetLines(TimingFigure *tvf, char *start, char *end, char *dir, char *linetype);

/*
    MAN ttv_ComputeLineDelay
    CATEG tcl+line
    DESCRIPTION
    Recomputes a line delay or slope depending on an input slope and an output capacitance. 
    ARGS
    tl % Related timing line
    slope_in     % desired input slope 
    output_capa  % desired output capacitance 
    delayslope   % type of delay to recompute {delay} or {slope}
    maxmin       % type of delay to recompute {max} or {min}
    EXAMPLE % {set val [ttv_ComputeLineDelay $line 1.5e-12 0.3e-15 delay max]}
*/
double ttv_ComputeLineDelay(TimingLine *tl, double slope_in, double output_capa, char *delayslope, char *maxmin);

/*
    MAN ttv_SetTimingLineDelay
    CATEG tcl+line
    DESCRIPTION
    Forces the value of the propagation delay or output slope of a line. The line's propagation delay or output slope will 
    never be reevaluated.
    ARGS
    tl % Related timing line
    delayslope   % type of delay to set {delay} or {slope}
    maxmin       % type of delay to set {max}, {min} or {all}
    value        % new value
    EXAMPLE % {ttv_SetTimingLineDelay $line delay max 1e-9]}
*/
void ttv_SetTimingLineDelay(TimingLine *tl, char *delayslope, char *maxmin, double value);

/*
  MAN ttv_CharacTimingLineModel
  CATEG tcl+line
  DESCRIPTION
  Compute delays or slopes using a given timing line model name.
  Returns a matrix of values. Each line correspond to an input slope and each element of a line to a capacitance.
  ARGS
  tvf % Timing figure with the timing model
  name % Name of the timing model
  slopes % List of slope values to use
  capas % List of capacitance/slope values to use
  type % defines the type of the table axis: "slope-slope", "slope-capa:delay" or "slope-capa:slope".\$"{slope-slope}" ({capas} contains slopes values) returns delay values\$"{slope-capas:delays}" returns delay values\$"{slope-capa:slope}" returns slope values
  EXAMPLE % {set res [ttv_CharacTimingLineModel $fig "hold__EN/CP_01_1" \{1e-12 3e-12\} \{1e-15 2e-15\} slope-capa:slope]}
*/
DoubleTable *ttv_CharacTimingLineModel(TimingFigure *tf, char *name, DoubleListTimeValue *input_slope, DoubleListCapaValue *output_capa, char *type);

/***********************************************************************************************/
/***********************************************************************************************/
/* TimingLines Properties  */
/***********************************************************************************************/
/***********************************************************************************************/

/*
    MAN ttv_GetTimingLineProperty
    CATEG tcl+line
    DESCRIPTION
    Returns a property of a timing line. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    tl %  Pointer on the timing line
    code % Property code; for available property codes, see the {TimingLine} object section
*/
Property *ttv_GetTimingLineProperty (TimingLine *tl, char *code);


/***********************************************************************************************/
/***********************************************************************************************/
/* TimingConstraint  */
/***********************************************************************************************/
/***********************************************************************************************/

/*
  MAN ttv_GetConstraints
  CATEG tcl+greybox
  DESCRIPTION
  Computes the constraints at the interface of a timing figure.
  Returns a list of TimingConstraint.
  ARGS
  tf             % Timing figure 
  inputconnector % Connector whose constraints must be computed
  towhat         % Ending path node types, can be a mix of {latch}, {precharge}, {clockgating}, {precharge} or {all} 
  EXAMPLE % {set cl [ttv_GetConstraints $fig * all}
*/

TimingConstraintList *ttv_GetConstraints (TimingFigure *tf, char *inputconnector, char *towhat);

/*
  MAN ttv_FreeConstraints
  CATEG tcl+greybox
  SYNOPSIS
  ttv_FreeConstraints <allobj>
  DESCRIPTION
  Frees the constraints.
  ARGS
  allobj         % Constraint objects to free
  EXAMPLE % {ttv_FreeConstraints $myconstraints}
*/
void ttv_FreeConstraints(void); // (void) disables the gns wrappers
void ttv_FreeConstraints_sub(TimingConstraintList *allobj, TimingFigure *tf, int autoclean);


/***********************************************************************************************/
/***********************************************************************************************/
/* TimingConstraint Properties  */
/***********************************************************************************************/
/***********************************************************************************************/

/*
    MAN ttv_GetTimingConstraintProperty
    CATEG tcl+greybox
    DESCRIPTION
    Returns a property of a timing constraint. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    co %  Pointer on the timing constraint
    property % Property code; for available property codes, see the {TimingConstraint} object section
*/

Property *ttv_GetTimingConstraintProperty (TimingConstraint *co, char *property);

/***********************************************************************************************/
/***********************************************************************************************/
/* UTILITY FUNCTIONS */
/***********************************************************************************************/
/***********************************************************************************************/
/*
    MAN ttv_DetectFalseClockPath
    CATEG tcl+path
    DESCRIPTION
    Checks the evidence of signal propagation of clock paths arriving on command signals (and not on other signals), in order to eliminate possible false paths. Detected false paths are added to the INF file. 
    ARGS
    tf % Pointer on a timing figure
*/
void ttv_DetectFalseClockPath(TimingFigure *tf);

/*
    MAN ttv_DetectFalsePath
    CATEG tcl+path
    DESCRIPTION
    Checks the evidence of signal propagation between two signals, in order to eliminate possible false paths. Detected false paths are added to the INF file. 
    ARGS
    tf % Pointer on a timing figure
    start % Name of the starting point
    end % Name of the ending point
*/
void ttv_DetectFalsePath(TimingFigure *tvf, char *start, char *end);

/***********************************************************************************************/
/***********************************************************************************************/
/* DATABASE GENERATION */
/***********************************************************************************************/
/***********************************************************************************************/

/***********************************************************************************************/
/* INTERNAL FUNCTIONS NOT TO BE COMMENTED */
/***********************************************************************************************/
TimingPathList *ttv_internal_GetCriticPath(TimingSignal *start, TimingSignal *end, char *dir, long number, int max);
TimingPathList *ttv_internal_GetAccess(TimingSignal *sig, TimingSignal *end, char *clock, char *dir, int max );
//TimingPathList *ttv_internal_GetPaths(TimingFigure *tf, char *clock, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax);
TimingPathList *ttv_internal_filterpathbycommand( TimingPathList *, TimingEvent *cmd, TimingEvent *inev, int free);
TimingPath *ttv_getcritic_pathlist (TimingFigure *tf, void *start, void *end, char *dir, long number, int minmax, int no_class, int names, int all);
TimingPath *ttv_getsigaccesslist (TimingFigure *tf, void *start, void *end, void *clk, char *dir, int number, int minmax, int no_class, int all);
void ttv_setsearchexclude(long valstart, long valend, long *oldvalstart, long *oldvalend);

/***********************************************************************************************/
/***********************************************************************************************/
/* NOT COMMENTED ON PURPOSE / OBSOLETE */
/***********************************************************************************************/
/***********************************************************************************************/

double ttv_GetLatchMaxAccess(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command);
double ttv_GetLatchMinAccess(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command);
int ttv_GetVectorConnectorLeftBound(TimingSignal *tvs);
int ttv_GetVectorConnectorRightBound(TimingSignal *tvs);
double ttv_GetSignalCapacitance(TimingSignal *tvs);
double ttv_GetConnectorCapacitance(TimingSignal *tvs);
char *ttv_GetConnectorName(TimingSignal *con);
char ttv_GetConnectorDirection(TimingSignal *ts);
char *ttv_GetSignalNetName(TimingSignal *tvs);
char *ttv_GetSignalName(TimingSignal *tvs);
TimingPathList *ttv_GetCriticMaxPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetCriticMinPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
void ttv_RemoveTimingFigure(TimingFigure *tf);
void ttv_SetSearchMode(char *mode);
TimingSignal *ttv_GetLatchCommandSignal(TimingEvent *tve);
char ttv_GetLatchCommandDirection(TimingEvent *tve);
void ttv_SetReportUnit(char *val);
TimingPathList *ttv_RemoveDuplicatedPath(TimingPathList *talist );

// devel

/*
    MAN ttv_LoadSDF
    CATEG tcl+sdf
    DESCRIPTION
    Constructs a timing figure from a netlist and an SDF file. The netlist must have been constructed before (related files loaded)
    ARGS
    fig % Pointer on a netlist
    sdf_file % Name of the corresponding SDF file
*/
TimingFigure *ttv_LoadSDF (Netlist *fig, char *sdf_file);

TimingPathList *ttv_ProbeDelay_v2(TimingFigure *tvf, double slopein, char *start, char *end, char *dir, int nbpath, char *minmax);


/*
    MAN ttv_GetGeneratedClockPaths
    CATEG tcl+path
    DESCRIPTION
    Gets the path list from a top level clock to the specified generated clock timing event. 
    ARGS
    tvf % Timing figure
    tve % Generated clock timing event 
    minmax % Maximum ({max}) or minimum ({min}) paths are returned
    EXAMPLE % {set p_list [ttv_GetGeneratedClockPaths $fig $clockevent max]}
*/
TimingPathList *ttv_GetGeneratedClockPaths(TimingFigure *tvf, TimingEvent *tve, char *minmax);

char *ttv_GetClockInfos(TimingFigure *tvf, char *name, char *minmax);

/*
    MAN ttv_SetTimingFigureName
    CATEG tcl+generation
    DESCRIPTION
    Change the name of a timing figure.
    ARGS
    tvf % Figure to be changed
    name % New figure name
*/

void ttv_SetTimingFigureName(TimingFigure *tvf, char *name);
void ttv_SetFigureFlag(TimingFigure *tvf, char *mode);

/*
    MAN ttv_Simulate_AddDelayToVT
    CATEG tcl+sim
    DESCRIPTION
    Applyed on the path, indicate to CPE to add a measure to retreive the delay
    to VT of the ending node of {pth} which is a command generating the event {latch} on the latch.\$
    The value can be retreived using the corresponding TimingPath property after the simulation
    is done.
    ARGS
    tvf % Pointer on a timing figure.
    pth % Pointer on the timing path.
    latch % Event of the latch generated by the ending node of {pth}.
*/
void ttv_Simulate_AddDelayToVT(TimingFigure *tvf, TimingPath *pth, TimingEvent *latch);

/*
    MAN ttv_Simulate_FoundSolutions
    CATEG tcl+sim
    DESCRIPTION
    Returns the number of path activation solutions found by CPE.\$
    {0} means the path can not be activated hence it is a false path.\$
    More than {1} solution, means CPE tried to find the best solution depending on
    whether the path was a max or a min path between those solutions.
*/
int ttv_Simulate_FoundSolutions();



int ttv_LoadSSTAResults(TimingFigure *tvf, StringList *filenames, IntegerTable *order);
int ttv_SetSSTARunNumber(TimingFigure *tvf, int num);
int ttv_BuildSSTALineStats(TimingFigure *tvf);
void ttv_UpdateInf(TimingFigure *tvf);
void ttv_ChangePathStartTime(TimingPath *tp, TimeValue time);
void ttvapi_setdislaytab(int nb);

