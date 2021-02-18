#include "avt_API_types.h"

/* ================================================================== */
/* GENERAL */
/* ================================================================== */

/*------------------
    MAN inf_SetFigureName
    CATEG tcl+general
    DESCRIPTION
    Sets the target figure on which to apply the INF functions
    ARGS
    name % Name of the target figure
    EXAMPLE % {inf_SetFigureName cpu}
*/
void inf_SetFigureName(char *name);

/*------------------
    MAN inf_AddFile
    CATEG tcl+general
    DESCRIPTION
    Loads an INF file and applies included statements on a figure (this function does not invoke {inf_SetFigureName}).
    ARGS
    filename % INF file to load
    figname % Figure on which to apply INF statements. Those statements will be added to the ones that my be already present.
    EXAMPLE % {inf_AddFile cpu.inf cpu}
*/
void inf_AddFile(char *filename, char *figname);

/*------------------
    MAN inf_Drive
    CATEG tcl+general
    DESCRIPTION
    Saves applied INF statements on disk
    ARGS
    outputname % File where to save INF statements (the {.inf} suffix is not automatically added)
    EXAMPLE % {inf_Drive cpu.inf}
*/
void inf_Drive(char *outputname);

/*------------------
    MAN inf_ExportSections
    CATEG tcl+general
    DESCRIPTION
    Saves on disk applied INF statements related to specific INF sections
    ARGS
    outputname % File where to save INF statements
    section % {OperatingCondition}, {PinSlew}, {Rename}, {Stop}, {Sensitive}, {Suppress}, {Inputs}, {NotLatch}, {CkLatch}, {Ckprech}, {Precharge}, {Dirout}, {Mutex}, {CrosstalkMutex}, {Constraint}, {ConnectorDirections}, {PathIN}, {PathOUT}, {PathDelayMargin}, {MulticyclePath}, {Ignore}, {NoCheck}, {Bypass}, {NoRising}, {NoFalling}, {Break}, {Inter}, {Asynchron}, {DoNotCross}, {Transparent}, {RC}, {NORC}, {SIGLIST}, {Falsepath}, {Delay}, {Dlatch}, {FlipFlop}, {Slopein}, {Capaout}, {OutputCapacitance}, {SwitchingProbability}, {Directives}, {Stb} and {Stuck}.
    EXAMPLE % {inf_ExportSections cpu.inf "Dirout CrosstalkMutex"}
*/
void inf_ExportSections(char *outputname, char *section);

/*------------------
    MAN inf_CleanFigure
    CATEG tcl+general
    DESCRIPTION
    Removes all INF statements on current figure
*/
void inf_CleanFigure();

/* ================================================================== */
/* DISASSEMBLY */
/* ================================================================== */

/*------------------
    MAN inf_DefineIgnore
    CATEG tcl+netlist
    DESCRIPTION
    The tool ignores specified components. Equivalent to commenting out elements in a SPICE netlist. 
    ARGS
    list % Pointer on the list of components to ignore. An component name can be a regular expression.
    type % Supported types are {Instances}, {Transistors}, {Resistances}, {Capacitances}, {Diodes}, {Parasitics} and {SignalNames}. {Parasitics} affects only DSPF files. {SignalNames} affects only the flattening of a hierarchical netlist, by ignoring the given name if several hierarchical names are available for one net.
    EXAMPLE % {inf_DefineIgnore Transistors *.M23*}
*/
void inf_DefineIgnore(char *type, List *list);

/*------------------
    MAN inf_DefineMutex
    CATEG tcl+disa
    DESCRIPTION
    Adds mutual exclusion constraints on signals, in order to help the disassembly process. May be especially usefull when dealing with shifters or multiplexors, in case mutual exclusion constraints can not be directly derived from internal combinational circuitry (if the mutual exclusions constraints come from latched values or come from constraints on external pins).
    ARGS
    type % Mutual exclusion constraints, legal values for are {muxup}, {muxdn}, {cmpup} and {cmpdn} (see INF file description)
    list % List of signals mutual exclusions constraints should be applied on
    EXAMPLE % {inf_DefineMutex cmpup [list a_0 a_1 a_2 a_3]}
*/
void inf_DefineMutex(char *type, List *list);

/*------------------
    MAN inf_DefineInputs
    CATEG tcl+disa
    DESCRIPTION
    Sets a signal as a circuit input, in order to help the disassembly process.
    ARGS
    name % Signal's name
*/
void inf_DefineInputs(char *name);

/*------------------
    MAN xxxinf_DefineStop
    CATEG tcl+disa
    DESCRIPTION
    Sets a signal as a functional analysis stop point, in order to help the disassembly process.
    ARGS
    name % Signal's name
*/
void inf_DefineStop(char *name);

/*------------------
    MAN inf_DefineDirout
    CATEG tcl+disa
    DESCRIPTION
    Defines the level of a signal for transistor orientation, in order to help the disassembly process. 
    ARGS
    name % Signal's name
    level % Signal's level; transistors are oriented (the sense of the current is) from high-level to low-level signals.
*/
void inf_DefineDirout(char *name, int level);

/*------------------
    MAN inf_DefineDLatch
    CATEG tcl+disa
    DESCRIPTION
    Sets a signal as a dynamic latch. Works only if the surrounding circuitry permits a HZ state on the signal. Commands are then
    identified automatically.
    ARGS
    name % Signal's name
*/

void inf_DefineDLatch(char *name);

/*------------------
    MAN inf_DefineNotDLatch
    CATEG tcl+disa
    DESCRIPTION
    Disables a dynamic latch directive on a signal. To be used together with {yagMarkTristateMemory}
    ARGS
    name % Signal's name
*/
void inf_DefineNotDLatch(char *name);

/*------------------
    MAN inf_DefineNotLatch
    CATEG tcl+disa
    DESCRIPTION
    Disables the identification of a latch on a signal
    ARGS
    name % Signal's name
*/
void inf_DefineNotLatch(char *name);


/*------------------
    MAN inf_DefineKeepTristateBehaviour
    CATEG tcl+disa
    DESCRIPTION
    Disables the transformation of bus into register when configurations 'avtVerilogTristateIsMemory' or 'yagleTristateIsMemory' is used to drive a behavioural model.
    ARGS
    name % Signal's name
*/
void inf_DefineKeepTristateBehaviour(char *name);

/*------------------
    MAN inf_DefinePrecharge
    CATEG tcl+disa
    DESCRIPTION
    Sets a signal as a precharge.
    ARGS
    name % Signal's name
*/
void inf_DefinePrecharge(char *name);

/*------------------
    MAN inf_DefineNotPrecharge
    CATEG tcl+disa
    DESCRIPTION
    Disables the identification of a precharge on a signal
    ARGS
    name % Signal's name
*/
void inf_DefineNotPrecharge(char *name);

/*------------------
    MAN inf_DefineModelLatchLoop
    CATEG tcl+disa
    DESCRIPTION
    Feedback loop is explicitly modeled in behavioural model if signal is a static latch.
    ARGS
    name % Signal's name
*/
void inf_DefineModelLatchLoop(char *name);

/*------------------
    MAN inf_DefineMemsym
    CATEG tcl+disa
    DESCRIPTION
    Sets a pair of signals to be a symmetric memory so long as there is a loop between the two signals.
    ARGS
    name0 % name of first memsym signal.
    name1 % name of second memsym signal.
    EXAMPLE % {inf_DefineMemsym memsym0 memsym1}
*/
void inf_DefineMemsym(char *name0, char *name1);

/*------------------
    MAN inf_DefineRS
    CATEG tcl+disa
    DESCRIPTION
    Allows control of how individual RS are handled. Overrides the global setting in yagAutomaticRSDetection.
    ARGS
    name % Signal's name, either the set or the reset one is enough.
    type % LEGAL, ILLEGAL or MARK_ONLY.
    EXAMPLE % {inf_DefineRS rsnode "LEGAL"}
*/
void inf_DefineRS(char *name, char *type);

/*------------------
    MAN inf_MarkSignal
    CATEG tcl+disa
    DESCRIPTION
    Allows application of special signal markings, such as latch identification. 
    ARGS
    name % Signal's name
    marks % For a complete list of markings please refer to the INF section of this manual, MARKSIG subsection. 
    EXAMPLE % {inf_MarkSignal dff_m "LATCH+MASTER"}
*/
void  inf_MarkSignal(char *name, char *marks);

/*------------------
    MAN inf_MarkTransistor
    CATEG tcl+disa
    DESCRIPTION
    Allows application of special transistor markings, such as latch commands identification. 
    ARGS
    name % Signal's name
    marks % Legal markings are "Bleeder", "Feedback", "Command", "NonFunctional", "Blocker", "Short", "Unused". Types may be concatenated with the '+' character and are case-insensitive. For a description of the types please refer to the INF section of this manual, MARKTRANS subsection. 
    EXAMPLE % {inf_MarkTrans m0 "FEEDBACK+NOT_FUNCTIONAL"}
*/
void  inf_MarkTransistor(char *name, char *marks);


/* ================================================================== */
/* BEHAVIOR */
/* ================================================================== */

/*------------------
    MAN inf_DefineSensitive
    CATEG tcl+yag
    DESCRIPTION
    Sets a signal as a timing sensitive net.
    ARGS
    name % Signal's name
*/
void inf_DefineSensitive(char *name);

/*------------------
    MAN inf_DefineSuppress
    CATEG tcl+yag
    DESCRIPTION
    Sets an auxiliary signal to be suppressed.
    ARGS
    name % Signal's name
*/
void inf_DefineSuppress(char *name);

/* ================================================================== */
/* XTALK */
/* ================================================================== */

/*------------------
    MAN inf_DefineCrosstalkMutex
    CATEG tcl+xtalk
    DESCRIPTION
    Sets mutually exclusive signals with regard to crosstalk analysis.
    ARGS
    type % Legal values are {muxup} and {muxdn}.
    list % List of signals on which to apply mutual exclusion constraints
*/
void inf_DefineCrosstalkMutex(char *type, List *list);

/* ================================================================== */
/* TIMING */
/* ================================================================== */

/*------------------
    MAN xxxinf_DefineBreak
    CATEG tcl+timing
    DESCRIPTION
    Sets a signal as a break point.
    ARGS
    name % Signal's name
*/
void inf_DefineBreak(char *name);

/*------------------
    MAN xxxinf_DefineInter
    CATEG tcl+timing
    DESCRIPTION
    Sets signal as an intermediary point for path factorization
    ARGS
    name % Signal's name
*/
void inf_DefineInter(char *name);


/*------------------
    MAN inf_DefineConnectorSwing
    CATEG tcl+timing
    DESCRIPTION
    Sets the switching voltage magnitude for a connector. This is usefull for multivoltage analysis.
    ARGS
    name % Connector's name
    lowlevel % Lower-bound voltage level, in Volts
    highlevel % Upper-bound voltage level, in Volts
*/
void inf_DefineConnectorSwing(char *name, double lowlevel, double highlevel);

/*------------------
    MAN inf_DefinePathDelayMargin
    CATEG tcl+timing
    DESCRIPTION
    Defines a derating to be applied on a path delay, with the following formula: {new_delay = real_delay * factor + delta}. This derating is only used for the computation of setup and hold slacks by the by slack report functions.  
    ARGS
    type % Arrival point type of the path on which to apply the derating. Valid values are {any}, {latch}, {break}, {prech} and {con} (connector)
    name % Arrival point name of the path on which to apply the derating. Wildcards can be used.
    factor % Integer value
    delta % Integer value
    pathtype % String with tokens separated by spaces or {_}. Valid token values for pathtype are {clockpath}, {datapath}, {min}, {max}, {rise} and {fall}. An empty string means that all the tokens are used.

*/
void inf_DefinePathDelayMargin(char *type, char *name, double factor, double delta, char *pathtype);


/*------------------
    MAN inf_DefineConnectorDirections
    CATEG tcl+timing
    DESCRIPTION
    Modifies the direction of connectors 
    ARGS
    type % Affected direction, can be {Input}, {Output}, {InOut}, {Tristate}, {HZ} or {Unknown} 
    list % List of signals a new direction will be affected to 
*/
void inf_DefineConnectorDirections(char *type, List *list);


/*------------------
    MAN inf_DefineNORC
    CATEG tcl+timing
    DESCRIPTION
    Disables RC delay computation on a signal
    ARGS
    name % Signal's name
*/
void inf_DefineNORC(char *name);

/*------------------
    MAN inf_DefineDoNotCross
    CATEG tcl+timing
    DESCRIPTION
    Disables the transparency of a latch or precharge, so that no path can traverse it. 
    Effective only when static timing analysis has been run ({stb}). To be used together with {avtMaxPathPeriodDepth}.
    ARGS
    name % Signal's name
*/
void inf_DefineDoNotCross(char *name);

/*------------------
    MAN inf_DefineTransparent
    CATEG tcl+timing
    DESCRIPTION
    Force transparency of a latch or precharge, so that path can traverse it. 
    ARGS
    name % Signal's name
*/
void inf_DefineTransparent(char *name);

/*------------------
    MAN inf_DisableTimingArc
    CATEG tcl+timing
    DESCRIPTION
    Disables the construction of timing arcs between timing signals.
    ARGS
    input % Source timing signal of the timing arc to disable
    output % Sink timing signal node of the timing arc to disable
    direction % Transition to disable: {du}, {ud}, {dd}, {uu}, {?u}...
    EXAMPLE % {inf_DisableTimingArc in out ud}
*/
void inf_DisableTimingArc(char *input, char *output, char *direction);

/*------------------
  MAN inf_DefineFalsePath
  CATEG tcl+timing
  DESCRIPTION
  Defines a false path.
  The parameters use the same syntax as in the .inf file except for the signal direction specification which does not allow spaces. Valid values {<up>} and {<down>}.
  ARGS
  EXAMPLE % {inf_DefineFalsePath [list sig1 <up> sig2 <down> sig3 sig4]}.
*/
void inf_DefineFalsePath(List *arglist);

/* ================================================================== */
/* TIMING ABSTRACTION */
/* ================================================================== */

/*------------------
    MAN inf_DefineSlopeRange
    CATEG tcl+abstraction
    DESCRIPTION
    Sets a range for input slopes in lookup tables construction (.lib file purpose), values in Seconds. This function must be called before database construction ({hitas} function) for ranges to be taken into account.
    ARGS
    name % Connector the defined range will be applied on, {default} is for all connectors. 
    type % {custom} or {linear}. 
    argv % If {type} is {custom}, list of values defining the slope range. If {type} is {linear}, 3-uple {(first_slope, last_slope, step)}. 
    EXAMPLE % {inf_DefineSlopeRange default [list 50e-12 120e-12 240e-12] custom}
*/
void inf_DefineSlopeRange(char *name, List *argv, char *type);

/*------------------
    MAN inf_DefineCapacitanceRange
    CATEG tcl+abstraction
    DESCRIPTION
    Sets a range for ouput capacitances in lookup tables construction (.lib file purpose), values in Fahrads. This function must be called before database construction ({hitas} function) for ranges to be taken into account.
    ARGS
    name % Connector the defined range will be applied on, {default} is for all connectors. 
    type % {custom} or {linear}. 
    argv % If {type} is {custom}, list of values defining the capacitance range. If {type} is {linear}, 3-uple {(first_capa, last_capa, step)}.
    EXAMPLE % {inf_DefineCapacitanceRange default [list 100e-15 200e-15 300e-15] custom}
*/
void inf_DefineCapacitanceRange(char *name, List *argv, char *type);

/* ================================================================== */
/* STATIC TIMING ANALYSIS */
/* ================================================================== */

/*------------------
    MAN xxxinf_SetSetupMargin
    CATEG tcl+stab
    DESCRIPTION
    Sets an additionnal global margin on all setup slacks
    ARGS
    value % Additionnal margin (in Seconds)
    EXAMPLE % {inf_SetSetupMargin 100e-12}
*/
void inf_SetSetupMargin(double value);

/*------------------
    MAN xxxinf_SetHoldMargin
    CATEG tcl+stab
    DESCRIPTION
    Sets an additionnal global margin on all hold slacks
    ARGS
    value % Additionnal margin (in Seconds)
    EXAMPLE % {inf_SetHoldMargin 100e-12}
*/
void inf_SetHoldMargin(double value);


/*------------------
    MAN inf_DefineStrictSetup
    CATEG tcl+stab
    DESCRIPTION
    Defines an latch whose setup must be check on the command opening event rather than the closing event.
    ARGS
    name % Name of the latch
    EXAMPLE % {inf_DefineStrictSetup mylatch}
*/
void inf_DefineStrictSetup(char *name);

/*------------------
    MAN inf_DefineAsynchron
    CATEG tcl+stab
    DESCRIPTION
    Defines an asynchronous pin, i.e. reset pin. Necessary to obtain recovery/removal timing groups in .lib characterization.
    ARGS
    name % Name of the reset pin
    EXAMPLE % {inf_DefineAsynchron reset}
*/
void inf_DefineAsynchron(char *name);

/*------------------
    MAN inf_DefineEquivalentClockGroup
    CATEG tcl+stab
    DESCRIPTION
    Sets clocks as belonging to the same timing domain.
    ARGS
    domain % Timing domain name
    list % Set of clocks belonging to the domain
    EXAMPLE % {inf_DefineEquivalentClockGroup domain1 [list ck1 ck2] }
*/
void inf_DefineEquivalentClockGroup(char *domain, List *list);

/*------------------
    MAN inf_DefineClockPriority
    CATEG tcl+stab
    DESCRIPTION
    Defines the preferred clock for a signal in case of multiple clock possibility.
    ARGS
    name % Signal's name
    clock % Priority clock
*/
void inf_DefineClockPriority(char *name, char *clock);


void  inf_DefineCharacteristics          (char *type, List *argv);


/*------------------
    MAN inf_DefineDirective
    CATEG tcl+stab
    SYNOPSIS
    inf_DefineDirective <{mode}> [{type1}] <{signal1}> [{dir1}] <{operation}> [{type2}] <{signal2}> [{dir2}] [{margin}]
    DESCRIPTION
    Adds a custom timing check between any two nodes. This timing check will be taken into account in STA and slack reports.
    ARGS
    mode      % Type of operation: {check} or {filter}.
    type1     % Type of propagated information on {signal1} to check: {clock} or {data}. Default is {data}.
    signal1   % Net name of the first signal.
    dir1      % Edge or value of {signal1} to consider: {rising} or {falling}. Default is both.
    operation % {before}, {after} or {with}. Operation {with} checks that {signal1} is stable when {signal2}'s state is up or down. {with} cannot be used with {signal1} as a clock or {signal2} as a data, and {dir2} as falling or rising. Operations {before} and {after} compare arrival times between clock/data and clock/data. {dir1} and {dir2} can be {rising} or {falling} only.
    type2     % Type of propagated information on {signal2} to check: {clock} or {data}. Default is {data}.
    signal2   % Net name of the second signal.
    dir2      % Edge or value of {signal2} to consider: {rising}, {falling}, {up} or {down}. Default is rising and falling.
    margin    % Margin to add when computing slack or constraint.
    EXAMPLE % {inf_DefineDirective check "data" with clock "data" up}.\$Checks that "data" is stable when the propagated clock state on data is high.
*/
void inf_DefineDirective(void); // (void) disables the gns wrappers
void inf_DefineDirective_sub(List *argv);

/*------------------
    MAN inf_DefineFalseSlack
    CATEG tcl+stab
    SYNOPSIS
    inf_DefineFalseSlack [<restriction>:] <{startclock}> [{<startclock_dir>}] <{startnode}> [{<startnode_dir>}] <{endnode}> [{<endnode_dir>}] [{<endnode_hz_state>}] <{endclock}>
    DESCRIPTION
    Defines a slack as being invalid.
    ARGS
    restriction % a '-' separated list of checks/signal types. Possible values: {setup}, {hold}, {latch}, {prech}. {latch} and {prech} define {endnode} signal type. Default is "setup-hold".
    startclock     % name of the clock generating the {startnode} data
    startclock_dir % transition of {startclock} : <up>, <down>, <rise> or <fall>. Default is both.
    startnode     % name of the generated data node
    startnode_dir % transition of {startnode} : <up>, <down>, <rise> or <fall>. Default is both.
    endnode     % name of the data arrival node
    endnode_dir % transition of {endnodestartnode} : <up>, <down>, <rise> or <fall>. Default is both.
    endnode_hz_state % specify whether the end node transition goes to hz state or not: <hz> or <nothz>. Default is both.
    endclock     % name of the clock controlling the arrival node
    EXAMPLE % {inf_DefineFalseSlack setup: "ck" <rise> "data" <rise> "arrival" "ck2"}.
*/
void inf_DefineFalseSlack(void); // (void) disables the gns wrappers
void inf_DefineFalseSlack_sub(List *argv);

/*------------------
    MAN inf_DefineSwitchingProbability
    CATEG tcl+stab
    SYNOPSIS
    inf_DefineSwitchingProbability {signal} {switching probability}
    DESCRIPTION
    [IN ALPHA DEVELOPMENT STAGE] Associates a switching probability to {signal}. This probability can be used by CTK to remove non interesting agressions (see also variable: stbCtkMinOccurenceProbability).
    ARGS
    signal    % signal to associate the probability to.
    switching probability % probability value from 0 to 1.
    EXAMPLE % {inf_DefineSwitchingProbability "enable" 0.25}. Signal "enable" can switch 1 time each 4 clock cycles.
*/
void inf_DefineSwitchingProbability(char *name, double val);

/* ================================================================== */
/* OBSOLETE */
/* ================================================================== */

void inf_DefineDisable(char *origin, char *destination);
void inf_DefineNoFalling(char *name);
void inf_DefineNoRising(char *name);
void inf_DefineStability(List *arglist);
void inf_DefinePower(char *name, double voltage);
void inf_DefineTemperature(double value);
void inf_DefineAsynchronousClockGroup(char *domain, List *list, double period);
void inf_DefineConstraint(char *name, int value);
void inf_DefineFlipFlop(char *name);
void inf_DefineRename(char *origin, char *destination);
void inf_DefineDelay(char *origin, char *destination, double delay);
void inf_DefineBypass(char *name, char *where);
void inf_SetPeriod(double value);
void inf_DefineCkPrech(char *name);
void inf_DefineSlew(char *name, double slope_rise, double slope_fall);
void inf_DefineSlope(char *name, double slope_rise, double slope_fall);
void inf_DefineRC(char *name);
void inf_DefineConditionedCommandStates(char *name, char *state);
void inf_DefineCkLatch(char *name);
void inf_DefineNotCkLatch(char *name);
void inf_DefineClock(List *argv);


/* ================================================================== */
/* DEBUG */
/* ================================================================== */

void inf_CheckRegistry_i(char *name, int level, List *data);
void inf_DumpRegistry_i(char *name);
