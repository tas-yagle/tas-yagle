#include <stdio.h>
#include "avt_API_types.h"

void tma (void);
BehavioralFigure *beh_getbefig (char *name);
TimingFigure *tma_abstract_old (TimingFigure *fig, BehavioralFigure *befig);
/*
    MAN xxxtma_Abstract
    DESCRIPTION
    Generates a blackbox view of a timing database and associates behavior if the behavioral information is provided.
    ARGS
    fig % Pointer on the timing figure to be blackboxed
    befig % Pointer on the behavioral figure to associate with the blackbox. Set NULL if no info
    EXAMPLE % {set bbox [tma_Abstract $fig NULL]}
*/

TimingFigure *tma_DuplicateInterface (TimingFigure *fig, char *newname, List *argv);

void tma_DetectClocksFromBeh (TimingFigure *fig, BehavioralFigure *befig);
void tma_AddInsertDelays (TimingFigure *blackbox, TimingFigure *fig);
void tma_UpdateSetReset (TimingFigure *fig, BehavioralFigure *befig);


/*
    MAN tma_GetConnectorAxis
    DESCRIPTION
    Returns the defined connector axis set for a connector/signal.\$
    If no axis have been defined for the signal or the signal does not exist, the returned list contains only one element with value {-1}.\$
    There is an exeption if the signal is an internal clock and no axis is defined for it. In this case, the slope max of the clock is returned.
    ARGS
    fig % Pointer on the timing figure.
    type % Type of axis: {slope} or {capacitance}
    name % Name of the signal.
    EXAMPLE % {set slopeaxis [tma_GetConnectorAxis $fig slope ck]}
*/
DoubleList *tma_GetConnectorAxis(TimingFigure *tvf, char *type, char *name);

/*
  MAN tmabs
  SYNOPSIS
  TimingFigure *tmabs <TimingFigure *fig> <BehavioralFigure *befig> <clocks> <inputs> <outputs> [flags]\$
  or\$
  TimingFigure *tmabs <TimingFigure *fig> \[-behaviour <bef>] \[-clocks <clocklist>] \[-inputs <inputlist>]\$
                      \[-outputs <outputlist>] \[-internal <internallist>] [flags]\$
  DESCRIPTION
  Generates a characterization from a timing database and associates functionality if provided.\$
  It returns a blackbox timing figure with constraints at the interface of the circuit.\$
  The constraints generated can be filter by input, output and/or clock connectors.\$
  Generated clocks are handled and simulations can also be used thru CPE to get spice precise results for the circuit characterisation.
  ARGS
  fig % Pointer on the timing figure to be blackboxed.
  befig % Pointer on the behavioral figure to associate with the blackbox. Set to {NULL} if no info is available.
  clocks % list of clock names to consider in the timing figure.
  inputs % list of input names to consider in the timing figure.
  outputs % list of output names to consider in the timing figure.
  flags: %
  -simulate {<list>} % Indicates what to simulate. {<list>} is a list of path description. A path description is a list of 1 or 2 pin names. The list looks like {\{pin_name related_pin_name\}} following .lib convention. if the related_pin_name is omited it is considered to be *.
  -maxsim {<number>} % Maximum number of parallel simulations to launch to speed up characterisations done with simulations.
  -enablecache % Enables the use of a cache file with already simulated path delays to continue a previously stopped run.
  -scalevalues {<list>} % Gives a list of factors to use to computed default slope and capacitance axis in case they are not specified in the information file.
  -verbose % Enables verbose mode.
  -detailfile {<filename>} % Drives in the file {<filename>} the detail of the paths used for each characterisation.
  -minonly % Computes only min delays.
  -maxonly % Computes only max delays.
  -setuponly % Computes only setups.
  -holdonly % Computes only holds.
  -dumpdtx % Drive the computed blackbox at the end of the process.
  -exitonerror % Exit on any error. Most likely simulation errors.
  -ignoremargins % Dot not add defined path margins to the computed/simulated delays.
  -detectfalsepath % Run a falsepath detection prior to begin the blackbox creation.
  -addpins {<pinlist>} % Add non existing connectors into the blackbox. Direction of pins can be defined in the list. eg. {input0 {input1 i} {output o}}. See {ttv_AddConnector} for available pin directions.
  EXAMPLE % {set bbox [tmabs $tf NULL * * * -maxsim 4 -simulate * -verbose -enablecache]}
*/
void tmabs(void); // (void) disables the gns wrappers

/*
    MAN tma_SetMaxCapacitance
    DESCRIPTION
    Sets the max capacitance to report in .lib file.
    ARGS
    bbox % Blackbox timing figure.
    name % Name of the connector (can be a regular expression).
    value % Capacitance value
    EXAMPLE % {set slopeaxis [tma_GetConnectorAxis $fig slope ck]}
*/
void tma_SetMaxCapacitance(TimingFigure *bbox, char *name, CapaValue value);
void tma_SetGeneratedClockInfo(TimingFigure *bbox, char *name, char *string);
void tma_TransfertSignalInformation(TimingSignal *src, TimingSignal *dst);
void tma_SetEnergyInformation(TimingFigure *bbox, char *name, char dir, char *modelname);

