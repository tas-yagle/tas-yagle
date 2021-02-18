#include "avt_API_types.h"

/*
    MAN avt_Config
    CATEG tcl+config
    DESCRIPTION
    Main way to configure the tool. Affects a value to one of the variables listed in the Configuration Variables section
    ARGS
    var % Configuration variable to be set
    val % New value
    EXAMPLE % {avt_Config tasGenerateConeFile yes}
*/
void avt_config (char *var, char *val);
void avt_Config (char *var, char *val);

/*
    MAN avt_GetConfig
    CATEG tcl+config
    DESCRIPTION
    returns the configurated value for configuration variable {var}
    ARGS
    var % Configuration variable to be set
    EXAMPLE % {set cone_cfg [avt_GetConfig tasGenerateConeFile]}
*/
char *avt_GetConfig (char *var);

// --------------
/*
    MAN avt_SetBlackBoxes
    CATEG tcl+files
    DESCRIPTION
    Allows the user to blackbox subcircuits. Blackboxed subcircuits will not be analyzed. Instead, the tool will let a hole. Whether this hole should
    be filled up or not by a timing description depends on configuration variables {tasIgnoreBlackbox} and {tasTreatBlackboxHierarchically}.
    If a blackbox name is prefixed with "unused:", no hole will be created but instead all transistors in the blackbox will be marked as unused. Those blackboxes can still be retreived with GNS if the recognition rule uses the same transistor names as in the blackbox.
    This command is equivalent to and overrides the creation of a {BLACKBOX} file.
    ARGS
    list % List of subcircuits to be blackboxed. All intended blackboxed subcircuits should present as only one {avt_SetBlackBoxes} command is allowed.
    EXAMPLE % {avt_SetBlackBoxes [list "sense_amp"]}
*/
void avt_SetBlackBoxes(List *list);

// --------------
/*
    MAN avt_LoadBehavior
    CATEG tcl+files
    DESCRIPTION
    Loads behavioral descriptions and construct internal representation according to the file format
    ARGS
    filename % File to be loaded 
    format % Available formats are {vhdl} and {verilog}
    EXAMPLE % {avt_LoadFile model.v verilog}
*/
BehavioralFigure *avt_LoadBehavior(char *filename, char *format);

// --------------
/*
    MAN avt_DriveBehavior
    CATEG tcl+files
    DESCRIPTION
    Drives a behavioral description according to the file format from the given internal representation
    ARGS
    befig % Behavior to be driven 
    format % Available formats are {vhdl} and {verilog}
    EXAMPLE % {avt_DriveBehavior $befig output.v verilog}
*/
void avt_DriveBehavior(BehavioralFigure *befig, char *format);

// --------------
/*
    MAN avt_LoadFile
    CATEG tcl+files
    DESCRIPTION
    Loads files and construct internal representation according to the file format
    ARGS
    filename % File to be loaded 
    format % Available formats are {spice}, {tlf4}, {tlf3}, {lib}, {verilog}, {vhdl}, {spf}, {dspf}, {inf}, {spef} and {ttv}
    EXAMPLE % {avt_LoadFile design.hsp spice}
*/
void avt_LoadFile(char *filename, char *format);

// --------------
/*
    MAN avt_EncryptSpice
    CATEG tcl+files
    DESCRIPTION
    Encrypts all sections of a Spice file (netlist or technology file) which are encapsulated by the {.protect} and {.unprotect} spice cards.
    ARGS
    inputname % File to be encrypted 
    outputname % Destination for encrypted output 
    EXAMPLE % {avt_EncryptSpice techno.hsp techno.hsp.enc}
*/
void avt_EncryptSpice(char *inputname, char *outputname);

// --------------
/*
    MAN avt_GetNetlist
    CATEG tcl+lofig
    DESCRIPTION
    Retrieves a netlist from memory and returns its pointer 
    ARGS
    name % Name of the netlist to get in the program's memory
    EXAMPLE % {set netlist [avt_GetNetlist "my_design"]}
*/
Netlist *avt_GetNetlist(char *name);

// --------------
/*
    MAN avt_FlattenNetlist
    CATEG tcl+lofig
    DESCRIPTION
    Flattens a netlist to a given level.
    ARGS
    lf % Pointer on the netlist to be flattened
    level % Hierarchical level (coming from top-level) the nelist will be flattened to. Available levels are {trs}, {catal} or {bbox} (transistor, catalog or blackbox). If none of those levels are used, {level} will be considered an instance name, to which the netlist will be flattened.
    EXAMPLE % {avt_FlattenNetlist $netlist trs}
*/
void avt_FlattenNetlist(Netlist *lf, char *level);

// --------------
/*
    MAN avt_DriveNetlist
    CATEG tcl+lofig
    DESCRIPTION
    Saves the netlist on disk according to the given format
    ARGS
    lf % Pointer on the netlist to be saved
    filename % Name of the file to be created
    format %  Available formats are {spice}, {verilog}, {vhdl} and {spef}
    EXAMPLE % {avt_DriveNetlist $netlist design.spi spice}
*/
void avt_DriveNetlist(Netlist *lf, char *filename, char *format);

// --------------
/*
    MAN avt_DisplayNetlistHierarchy
    CATEG tcl+lofig
    DESCRIPTION
    Displays hierarchy information of a given netlist, and other info such as number of transistors 
    ARGS
    f % Pointer on the file where to save information, for standard output set {stdout}
    netlistname % Pointer on the netlist
    maxdepth % Maximum hierarchical depth coming from top level; can be set to 0 for infinite depth
    EXAMPLE % {avt_DisplayNetlistHierarchy stdout "my_design" 3}
*/
void avt_DisplayNetlistHierarchy(FILE *f, char *netlistname, int maxdepth);

// --------------
/*
    MAN avt_DisplayResistivePath
    CATEG tcl+lofig
    DESCRIPTION
    Displays one resistive path between two connectors at the interface of a netlist.
    ARGS
    f % Pointer on the file where to save information, for standard output set {stdout}
    lf % Pointer on the netlist
    connector1 % first connector name
    connector2 % second connector name
    EXAMPLE % {avt_DisplayResistivePath stdout [avt_GetNetlist "mynetlistname"] vdd_0 vdd_1}
*/
void avt_DisplayResistivePath(FILE *f, Netlist *lf, char *connector1, char *connector2);

// --------------
/*
    MAN avt_RemoveResistances
    CATEG tcl+lofig
    DESCRIPTION
    Removes all resistances on signals matching a regular expression
    ARGS
    lf % Pointer on the netlist where to remove resistances 
    nameregex % Regular expression to be matched, for all signals use {*}
    EXAMPLE % {avt_RemoveResistances $netlist "cpu.*.sig3*"}

*/
void avt_RemoveResistances(Netlist *lf, char *nameregex);

// --------------
/*
    MAN avt_RemoveCapacitances
    CATEG tcl+lofig
    DESCRIPTION
    Removes all capacitances on signals matching a regular expression
    ARGS
    lf % Pointer on the netlist where to remove capacitances 
    nameregx % Regular expression to be matched, for all signals use {*}
    EXAMPLE % {avt_RemoveCapacitances $netlist "cpu.*.sig3*"}
*/
void avt_RemoveCapacitances(Netlist *lf, char *nameregex);

// --------------
/*
    MAN avt_StartWatch
    CATEG tcl+system
    DESCRIPTION
    Starts a timer; if the timer already exixts it'll be reset to 0.
    ARGS
    name % Timer name
    EXAMPLE % {avt_StartWatch "CPU_TIME"}
*/



void avt_StartWatch(char *name);

// --------------
/*
    MAN avt_StopWatch
    CATEG tcl+system
    DESCRIPTION
    Stops a timer; the timer must be started for the function to work
    ARGS
    name % Name of the timer to stop
    EXAMPLE % {avt_StopWatch "CPU_TIME"}
*/
void avt_StopWatch(char *name);

// --------------
/*
    MAN avt_PrintWatch
    CATEG tcl+system
    DESCRIPTION
    Returns a string with the value of a timer; the timer must have been started
    ARGS
    name % Name of the timer to print
    EXAMPLE % {avt_PrintWatch "CPU_TIME"}
*/
char *avt_PrintWatch(char *name);

// --------------
/*
    MAN avt_GetMemoryUsage
    CATEG tcl+system
    DESCRIPTION
    Returns an integer with the memory usage of the program in bytes
    ARGS
    EXAMPLE % {set memory [avt_GetMemoryUsage]}
*/
unsigned long avt_GetMemoryUsage();

/*
    MAN avt_SetCatalog
    CATEG tcl+files
    DESCRIPTION
    Sets the leaves when flattening a netlist to catal level; equivalent to create a CATAL file
    ARGS
    argv % List of subcircuits that will be used as leaves
    EXAMPLE % {avt_SetCatalog [list "nand2" "inv" ]}
*/
void avt_SetCatalog(List *argv);

/*
    MAN avt_GetCatalog
    CATEG tcl+files
    DESCRIPTION
    Returns the current list of cells set as leaves for a catal-level flatten
    ARGS
    EXAMPLE % {set catal [avt_GetCatalog]}
*/
StringList *avt_GetCatalog();

void avt_SetSEED(int val);
void avt_AddRC(Netlist *lofig, int maxwire, double minc, double maxc, double minr, double maxr);
void avt_AddCC(Netlist *lofig, double minc, double maxc);
void avt_RemoveNetlist(char *name);
void avt_ViewNetlist(char *name);
void avt_ViewSignal(char *name, char *signal);
List* avt_SigList(char *name);
List* avt_PinList(char *name);
void avt_DriveSignalInfo( Netlist *lf, char *nameregex, char *filename );
void *_NULL_();
int avt_CodeName(char *name);

/*
    MAN avt_CheckTechno
    CATEG tcl+files
    DESCRIPTION
    Runs a set of benchs to findout possible technology errors
    ARGS
    label % A prefix label for the output result files
    tn % NMOS transistor characteristics. It's a space separated string with coming first the NMOS transistor name followed by the parameters. Authorized parameters are: l, w, delvt0, mulu0, sa, sb, sd, nf, nrs, nrd, sc, sca, scb, scc.
    tp % same as {tn} for PMOS transistor.
    EXAMPLE % avt_CheckTechno check1 "nmos l=0.4u w=0.8u" "pmos l=0.4u w=1.6u"

*/
void avt_CheckTechno(char *label, char *tn, char *tp);

int avt_BuildID();

/*
    MAN avt_RegexIsMatching
    CATEG tcl+system
    DESCRIPTION
    Returs 1 if {nametocheck} matches the regular expression {template}, 0 otherwise.
    ARGS
    nametocheck % name to check.
    template % regular expression to use.
    EXAMPLE % {set match [avt_RegexIsMatching tatoo5 *too*]}

*/
int avt_RegexIsMatching(char *nametocheck, char *template);

/*
    MAN avt_SetMainSeed
    CATEG tcl+montecarlo
    DESCRIPTION
    Sets the main seed to use when evaluating random mathematical function with monte-carlo runs enabled. The main seed is used for anything but model parameters in a ".mcparam" spice section concerning transistor model alterations. This function goal is to enable the reproducibility of past results.
    ARGS
    value % seed value.
    EXAMPLE % {avt_SetMainSeed 123456}

*/
void avt_SetMainSeed(unsigned int value);

/*
    MAN avt_GetMainSeed
    CATEG tcl+montecarlo
    DESCRIPTION
    Returns the main seed used to evaluate random mathematical function with monte-carlo runs enabled. The main seed is used for anything but model parameters in a ".mcparam" spice section concerning transistor model alterations. This function goal is to enable the reproducibility of past results.\$This function must be called after HiTas execution.
    ARGS
    EXAMPLE % {puts [avt_GetMainSeed]}

*/
unsigned int avt_GetMainSeed();

/*
    MAN avt_SetGlobalSeed
    CATEG tcl+montecarlo
    DESCRIPTION
    Sets the seed to use when evaluating random mathematical function with monte-carlo runs enabled. Global seed is used only for parameters in a ".mcparam" spice section concerning transistor model alterations. This function goal is to enable the reproducibility of past results.
    ARGS
    value % seed value.
    EXAMPLE % {avt_SetGlobalSeed 654321}

*/
void avt_SetGlobalSeed(unsigned int value);

/*
    MAN avt_GetGlobalSeed
    CATEG tcl+montecarlo
    DESCRIPTION
    Returns the main seed used to evaluate random mathematical function with monte-carlo runs enabled. Global seed is used only for parameters in a ".mcparam" spice section concerning transistor model alterations. This function goal is to enable the reproducibility of past results.\$This function must be called after HiTas execution.
    ARGS
    EXAMPLE % {puts [avt_GetGlobalSeed]}

*/
unsigned int avt_GetGlobalSeed();

void avt_banner (char *tool, char *comment, char *date);
