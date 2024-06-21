#include <stdio.h>
#include "avt_API_types.h"

#define IN_RC        1
#define INTERNAL_RC  2
#define OUT_RC       4
#define NO_RC        8
#define ALL_RC       16
#define XTALK_TO_GND 32

//#define IFGNS(a) if (GENIUS_GLOBAL_LOFIG!=NULL) { a }

void gen_API_Action_Initialize ();
void gen_API_Action_Terminate ();


extern void gns_ChangeHierarchyDivider (Netlist *netlist, char divider);

//_____________________________________________________________________________
/*
MAN gns_StripNetlist 
CATEG gns
DESCRIPTION
Suppresses all unconnected RC networks. This function is very useful after using netlist reduction.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_StripNetlistFurther
*/
extern void gns_StripNetlist (Netlist *netlist);

/*
MAN gns_StripNetlistFurther
CATEG gns
DESCRIPTION
Suppresses all unconnected RC networks *AND* connectors. This function is very useful after using netlist reduction.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_StripNetlist
*/
void gns_StripNetlistFurther (Netlist *netlist);

//extern void gns_DumpCorrespTable ();
//_____________________________________________________________________________
/*
MAN gns_SetLoad
CATEG gns
DESCRIPTION
Sets an additional capacitance on the connector `connector'.
This function should be primarily used for characterization purposes.
Further call of gns_SetLoad overrides previous characterization capacitance setting.
RETURN VALUE
none.
ERRORS
none.
*/
extern void gns_SetLoad (Netlist *netlist, char *connector, double load);

//_____________________________________________________________________________
/*
MAN gns_FlattenNetlist
CATEG gns
DESCRIPTION
Flattens the given netlist to the transistor level or to a cell level.
The cell levels can be specified by using gns_SetModelAsLeaf(<modelname>) where model name is the cell name.
The RC information can be added by specifying the flags whose value can be INTERNAL_RC, IN_RC, OUT_RC, ALL_RC or 0.
Those values can be ORed.
In most cases, the returned netlist must be freed by the user.\$
If used in TCL, {flags} is a list of the different flags.
RETURN VALUE
a new 'Netlist *'.
ERRORS
none.
SEE ALSO
gns_SetModelAsLeaf, gns_FreeNetlist, gns_AddRC
*/

extern Netlist *gns_FlattenNetlist (Netlist *netlist, int rc);

/*
MAN gns_FreeNetlist
CATEG gns
DESCRIPTION
Deletes a Netlist. It should not be called on a netlist obtained with gns_GetNetlist().
RETURN VALUE
none.
ERRORS
none.
*/
void gns_FreeNetlist (Netlist *netlist);

/*
MAN gns_AddRC
CATEG gns
DESCRIPTION
Adds the RC information in the current netlist.
The flags whose value can be INTERNAL_RC, IN_RC, OUT_RC, ALL_RC or 0 indicates where the RC should be added. Those values can be ORed.
In most cases, the returned netlist must be freed by the user.\$
If used in TCL, {flags} is a list of the different flags.
RETURN VALUE
a new 'Netlist *'.
ERRORS
none.
SEE ALSO
gns_SetModelAsLeaf, gns_FreeNetlist, gns_AddRC
*/

Netlist *gns_AddRC (Netlist *netlist, int rc);

/*
MAN gns_SetModelAsLeaf
CATEG gns
DESCRIPTION
Adds the model <name> to the list of models for which the flatten process won't  flat the instances to transistor level. In the flattened netlist should appear the instances whose model is <name>. Beware, the leaf model <name> will be taken into account for all the flatten process. To clear the list of leaves, use gns_SetModelAsLeaf(NULL).
RETURN VALUE
none.
ERRORS
none.
*/
void gns_SetModelAsLeaf(char *name);

//_____________________________________________________________________________
/*
MAN gns_ReduceInstance
CATEG gns
DESCRIPTION
Reduces an instance to the corresponding capacitances of its interface pins.
RETURN VALUE
none.
ERRORS
none.
*/

void gns_ReduceInstance (Netlist *netlist, char *ins_name);

//_____________________________________________________________________________
/*
MAN gns_KeepInstance
CATEG gns
DESCRIPTION
Keeps an instance from a reducing process.
RETURN VALUE
none.
ERRORS
none.
*/
void gns_KeepInstance (Netlist *fig, char *ins_name);
//_____________________________________________________________________________
/*
MAN gns_AddExternalTransistors
CATEG gns
DESCRIPTION
Allows the user to choice between two possibilities of representation 
of external transistors connected to an output pin. When the parameter 'str' is set 
to 'dynamic', capacitances are extracted from the grid, source or drain of the 
external transistor, and added into the netlist. When the parameter 'str' is set 
to 'transistor', the external transistors are added into the netlist, and the transistor's 
connectors that are not connected to the pin, are connected either to 'gnd' or 'vdd', 
in a way that they always remain non-passant.
RETURN VALUE
none.
ERRORS
none.
*/

void gns_AddExternalTransistors (char *str);

//_____________________________________________________________________________
/*
MAN gns_ViewLo
CATEG gns
DESCRIPTION
Displays debugging information about the netlist. Information
is displayed on stdout.
RETURN VALUE
gns_ViewLo returns no value.
ERRORS
none.
*/

void gns_ViewLo (Netlist *ptfig);

//_____________________________________________________________________________
/*
MAN gns_DriveNetlist
CATEG gns
DESCRIPTION
Drives the netlist in path with the specified format. <format> can be "spice". The filename will be <name>.EXT where EXT depends on the <format> and the figure name in the file will be <name>.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_DriveSpiceNetlistGroup
*/

void gns_DriveNetlist (Netlist *ptfig, char *format, char *path, char *name);

//_____________________________________________________________________________
/*
MAN gns_GetNetlist
CATEG gns
DESCRIPTION
Returns the netlist corresponding with the currently recognized model.
PS: this netlist must not be freed.
RETURN VALUE
returns a `Netlist *'.
ERRORS
none.
SEE ALSO
gns_GetInstanceNetlist, gns_DuplicateNetlist
*/

Netlist *gns_GetNetlist ();

/*
MAN gns_DuplicateNetlist
CATEG gns
DESCRIPTION
Duplicates the netlist <source>. The copied netlist should be freed later.
RETURN VALUE
returns a `Netlist *'.
ERRORS
none.
SEE ALSO
gns_GetInstanceNetlist, gns_GetNetlist
*/
Netlist *gns_DuplicateNetlist (Netlist *source);

/*
MAN gns_GetInstanceNetlist
CATEG gns
DESCRIPTION
Returns the netlist corresponding with a recognized instance used in the current hierarchy. <name> can be hierarchical.
PS: this netlist should be freed.
RETURN VALUE
returns a `Netlist *'.
ERRORS
returns NULL if the instance does not exist.
SEE ALSO
gns_GetNetlist
*/

Netlist *gns_GetInstanceNetlist (char *name);

//_____________________________________________________________________________
/*
MAN gns_BuildNetlist
CATEG gns
DESCRIPTION
Builds the netlist corresponding with the recognized model. 
If the model is a transistor level model, the created netlist is a
transistor level netlist. If the model is hierarchical, then the created netlist is a
hierarchical netlist.
RETURN VALUE
gns_BuildNetlist returns a pointer on the newly created netlist.
ERRORS
Aborts if not called during genius process.
*/

//Netlist *gns_BuildNetlist ();

//_____________________________________________________________________________
/*
MAN gns_GetCorrespondingSignal
CATEG gns
DESCRIPTION
Returns the signal in the circuit corresponding to a 
signal in the model.
RETURN VALUE
gns_GetCorrespondingSignal returns a 'Signal'.
ERRORS
Aborts if the signal does not exist.
*/

Signal *gns_GetCorrespondingSignal(char *name);

//_____________________________________________________________________________
/*
MAN gns_GetSignalName
CATEG gns
DESCRIPTION
Returns the name of a signal. When used with a signal in the 
model, there will not be any information about the signal index range in the 
returned string.
RETURN VALUE
gns_GetSignalName returns a string.
ERRORS
Aborts if not called during genius recognition process.
Aborts if the signal does not exist.
*/

char *gns_GetSignalName(Signal *signal);


//_____________________________________________________________________________
/*
MAN gns_GetInstanceName
CATEG gns
DESCRIPTION
Returns the name of an instance.
RETURN VALUE
gns_GetInstanceName returns a string.
ERRORS
none.
*/

char *gns_GetInstanceName(Instance *instance);

//_____________________________________________________________________________
/*
MAN gns_GetInstanceModelName
CATEG gns
DESCRIPTION
Returns the name of the model of an instance in the 
circuit.
RETURN VALUE
gns_GetInstanceName returns a string.
ERRORS
none.
*/

char *gns_GetInstanceModelName(Instance *instance);

//_____________________________________________________________________________
/*
MAN gns_GetModelSignalRange
CATEG gns
DESCRIPTION
Returns the index range of a signal in the model. If 
left = -1, the signal is not a vector. If <left> < <right> then the signal is 
range is left to right else if <left> > <right> then the signal range is left 
downto right.
RETURN VALUE
none.
ERRORS
Aborts if not called during genius recognition process.
Aborts if the signal does not exist.
*/

void gns_GetModelSignalRange(char *name, int *left, int *right);
//_____________________________________________________________________________

/*
MAN gns_GetModelConnectorList
CATEG gns
DESCRIPTION
Returns the list of all the interface connectors of the model.
RETURN VALUE
gns_GetModelConnectorList returns a 'List'.
ERRORS
none.
*/

List *gns_GetModelConnectorList();

//_____________________________________________________________________________

/*
MAN gns_GetInstanceConnector
CATEG gns
DESCRIPTION
Returns the connector <name> of <instance> the model.
RETURN VALUE
gns_GetInstanceConnector returns a 'Connector'.
ERRORS
none.
*/

Connector *gns_GetInstanceConnector(Instance *instance, char *name);

//_____________________________________________________________________________

/*
MAN gns_GetInstance
CATEG gns
DESCRIPTION
Returns the instance <name> present in <netlist>.
RETURN VALUE
gns_GetInstance returns an 'Instance'. The value is NULL is the instance was not found.
ERRORS
none.
*/

Instance *gns_GetInstance(Netlist *netlist, char *name);

//_____________________________________________________________________________

/*
MAN gns_GetConnectorCapa
CATEG gns
DESCRIPTION
Returns the computed capacitance of the connector `lc'. This
capacitance is the sum of all capacitances corresponding with the drain, grid or
source connectors linked to `lc'.
the model.
RETURN VALUE
gns_GetConnectorCapa () returns the capacitance of the connector `lc'.
ERRORS
none.
*/

double gns_GetConnectorCapa (Connector *lc);

//_____________________________________________________________________________

/*
MAN gns_GetConnectorList
CATEG gns
DESCRIPTION
Returns the list of all the interface connectors of the netlist.
RETURN VALUE
gns_GetConnectorList returns a 'List'.
ERRORS
none.
*/

List *gns_GetConnectorList(Netlist *netlist);

//_____________________________________________________________________________
/*
MAN gns_GetConnectorDirection
CATEG gns
DESCRIPTION
Returns the direction of a connector.
RETURN VALUE
gns_GetModelConnectorList returns a string. Values of this string can be:
- "in"
- "out"
- "inout"
- "linkage"
- "mux_bit"
ERRORS
none.
*/

char *gns_GetConnectorDirection(Connector *connector);

//_____________________________________________________________________________
/*
MAN gns_GetConnectorName
CATEG gns
DESCRIPTION
Returns the name of a connector.
RETURN VALUE
gns_GetConnectorName returns a string.
ERRORS
none.
*/

char *gns_GetConnectorName(Connector *connector);

//_____________________________________________________________________________
/*
MAN gns_GetConnectorSignal
CATEG gns
DESCRIPTION
Returns the signal linked to the connector.
RETURN VALUE
gns_GetConnectorSignal returns a 'Signal'.
ERRORS
none.
*/

Signal *gns_GetConnectorSignal(Connector *connector);

//_____________________________________________________________________________
/*
MAN gns_GetModelSignalList
CATEG gns
DESCRIPTION
Returns the list of all the signal in the model.
RETURN VALUE
gns_GetModelSignalList returns a 'List'.
ERRORS
none.
*/

List *gns_GetModelSignalList();

//_____________________________________________________________________________
/*
MAN gns_IsSignalExternal
CATEG gns
DESCRIPTION
Returns 1 if the signal is linked to a connector in the 
interface, 0 otherwise.
RETURN VALUE
gns_IsSignalExternal returns an integer representing a Boolean value.
ERRORS
none.
*/

int gns_IsSignalExternal(Signal *signal);

//_____________________________________________________________________________
/*
MAN gns_Vectorize
CATEG gns
DESCRIPTION
Returns a string containing the name associated with a vector 
index : name(index).
RETURN VALUE
gns_IsSignalExternal returns a string.
ERRORS
none.
SEE ALSO
gns_Vectorize2D
*/

char *gns_Vectorize(char *name, int index);

//_____________________________________________________________________________
/*
MAN gns_Vectorize2D
CATEG gns
DESCRIPTION
Returns a string containing the name associated 
with a 2 vector index : name(index1)(index2).
RETURN VALUE
gns_IsSignalExternal returns a string.
ERRORS
none.
SEE ALSO
gns_Vectorize
*/

char *gns_Vectorize2D(char *name, int index0, int index1);

//_____________________________________________________________________________
/*
MAN gns_GetInstanceConnectorList
CATEG gns
DESCRIPTION
Returns the list of all the connectors of an instance.
RETURN VALUE
gns_GetInstanceConnectorList returns a 'List'.
ERRORS
none.
*/


List *gns_GetInstanceConnectorList(Instance *ls);

//_____________________________________________________________________________
/*
MAN gns_GetAllCorrespondingInstances
CATEG gns
DESCRIPTION
Returns the list of all the instances in the 
circuit used when matching the model.
RETURN VALUE
gns_GetAllCorrespondingInstances returns a 'List' of 'Instance'.
ERRORS
Aborts if not called during genius recognition process.
*/

List *gns_GetAllCorrespondingInstances();

//_____________________________________________________________________________
/*
MAN gns_GetAllCorrespondingInstanceModels
CATEG gns
DESCRIPTION
Returns the list of all the models in 
the circuit used when matching the model.
RETURN VALUE
gns_GetAllCorrespondingInstanceModels returns a 'List' of strings.
ERRORS
Aborts if not called during genius recognition process.
*/

List *gns_GetAllCorrespondingInstanceModels();

//_____________________________________________________________________________
/*
MAN gns_GetCorrespondingTransistor
CATEG gns
DESCRIPTION
Returns the transistor in the circuit corresponding 
to a transistor instance in the model.
RETURN VALUE
gns_GetCorrespondingTransistor returns a 'Transistor'.
ERRORS
Aborts if not called during genius recognition process.
Aborts if the transistor does not exist.
*/

Transistor *gns_GetCorrespondingTransistor(char *name);

//_____________________________________________________________________________
/*
MAN gns_GetAllCorrespondingTransistors
CATEG gns
DESCRIPTION
Returns the list of all the transistors in 
the circuit used when matching the model.
RETURN VALUE
gns_GetAllCorrespondingTransistors returns a 'List' of 'Transistor'.
ERRORS
Aborts if not called during genius recognition process.
*/

List *gns_GetAllCorrespondingTransistors();

//_____________________________________________________________________________
/*
MAN gns_GetTransistorGrid
CATEG gns
DESCRIPTION
Returns grid connector of a transistor.
RETURN VALUE
gns_GetTransistorGrid returns a 'Connector'.
ERRORS
none.
*/

Connector *gns_GetTransistorGrid(Transistor *transistor);

//_____________________________________________________________________________
/*
MAN gns_GetTransistorDrain
CATEG gns
DESCRIPTION
Returns grid connector of a transistor.
RETURN VALUE
gns_GetTransistorDrain returns a 'Connector'.
ERRORS
none.
*/

Connector *gns_GetTransistorDrain(Transistor *transistor);

//_____________________________________________________________________________
/*
MAN gns_GetTransistorSource
CATEG gns
DESCRIPTION
Returns source connector of a transistor.
RETURN VALUE
gns_GetTransistorSource returns a 'Connector'.
ERRORS
none.
*/

Connector *gns_GetTransistorSource(Transistor *transistor);

//_____________________________________________________________________________
/*
MAN gns_GetTransistorType
CATEG gns
DESCRIPTION
Returns the type of a transistor.
RETURN VALUE
gns_GetTransistorType return value can be 'n' or 'p'.
ERRORS
none.
*/

char gns_GetTransistorType(Transistor *transistor);

//_____________________________________________________________________________
/*
MAN gns_GetTransistorTypeName
CATEG gns
DESCRIPTION
Returns the circuit type of a transistor.
RETURN VALUE
gns_GetTransistorTypeName returns a string.
ERRORS
none.
*/

char *gns_GetTransistorTypeName(Transistor *transistor);

//_____________________________________________________________________________
/*
MAN gns_GetTransistorParameter
CATEG gns
DESCRIPTION
Returns the value of a transistor parameter. 
<name> can be "w", "l", "as", "ad", "ps" or "pd".
RETURN VALUE
gns_GetTransistorParameter returns a double.
ERRORS
none.
*/

double gns_GetTransistorParameter(char *name, Transistor *transistor);

//_____________________________________________________________________________
/*
MAN gns_GetTransistorName
CATEG gns
DESCRIPTION
Returns the name of a transistor.
RETURN VALUE
gns_GetTransistorName returns a string.
ERRORS
none.
*/

char *gns_GetTransistorName(Transistor *transistor);

//_____________________________________________________________________________
/*
MAN gns_GetAllTransistorsConnectedtoSignal
CATEG gns
DESCRIPTION
Returns the list of all the transistors 
in the circuit connected to the given signal at the current step of genius recognition.
RETURN VALUE
gns_GetAllTransistorsConnectedtoSignal returns a 'List' of 'Transistor'.
ERRORS
none.
*/

List *gns_GetAllTransistorsConnectedtoSignal(Signal *signal);

//_____________________________________________________________________________

Connector *gns_GetExternalConnectorOfSignal(Signal *signal);

//_____________________________________________________________________________
/*
MAN gns_VectorIndex
CATEG gns
DESCRIPTION
Returns the index in a signal name. If the signal is not 
a vector, the value returned is -1. 
RETURN VALUE
gns_VectorIndex(name) returns an integer.
ERRORS
none.
*/

int gns_VectorIndex(char *name);

//_____________________________________________________________________________
/*
MAN gns_VectorRadical
CATEG gns
DESCRIPTION
Returns the base name of a signal name. The basic action 
is to remove the index from a vector name. 
RETURN VALUE
gns_VectorRadical(name) returns a string.
ERRORS
none.
*/

char *gns_VectorRadical(char *name);

//_____________________________________________________________________________
/*
MAN gns_CreateVhdlName
CATEG gns
DESCRIPTION
Returns transforms <name> so it is suitable for a 
VHDL syntaxe. 
RETURN VALUE
gns_CreateVhdlName(name) returns a string.
ERRORS
none.
*/

char *gns_CreateVhdlName(char *name);

//_____________________________________________________________________________
/*
MAN gns_ChangeInstanceModelName
CATEG gns
DESCRIPTION
Changes the model name of a recognized 
instance. If instance is NULL, the new name is applied to the current instance.
RETURN VALUE
none.
ERRORS
none.
*/

void gns_ChangeInstanceModelName(Instance *instance, char *name);

//_____________________________________________________________________________
/*
MAN gns_GetSignal
CATEG gns
DESCRIPTION
Retrieves the signal 'signame' in the netlist 'netlist'.
The netlist maybe either flat or hierarchical. To retrieve
a signal in a hierarchical netlist, one must provide a hierarchical name,
i.e containing the successives instances separated with dots.
For example, the signal 'ins1.ins2.sig' describes the signal 'sig' in the 
instance 'ins2', the instance 'ins2' being contained in the instance 'ins1'.
RETURN VALUE
gns_GetSignal returns the pointer on the signal in the netlist, NULL if 
the signal doesn't exist.
ERRORS
none.
*/

Signal *gns_GetSignal (Netlist *netlist, char *signame);

//_____________________________________________________________________________
/*
MAN gns_GetConnector
CATEG gns
DESCRIPTION
Retrieves the connector 'conname' in the netlist 'netlist'.
RETURN VALUE
gns_GetConnector returns the pointer on the connector in the netlist, NULL if 
the connector doesn't exist.
ERRORS
none.
*/

Connector *gns_GetConnector (Netlist *netlist, char *con_name);

//_____________________________________________________________________________
/*
MAN gns_GetTransistor
CATEG gns
DESCRIPTION
Retrieves the transistor 'con_name' in the netlist 'netlist'.
RETURN VALUE
gns_GetTransistor returns the pointer on the transistor in the netlist, NULL if 
the transistor doesn't exist.
ERRORS
none.
*/

Transistor *gns_GetTransistor (Netlist *netlist, char *tr_name);

//_____________________________________________________________________________
/*
MAN gns_AWE_GetWorstInstance
CATEG gns
DESCRIPTION
Retrieves the worst instance (worst AWE delay) 
which is connected to the connector 'lc'.
'netlist' is the hierarchical netlist.
'insname' is the generic name of the instance to reduce.
For example, if the netlist contains "mem_cell.0", "mem_cell.1", "mem_cell.2" ...
'insname' is "mem_cell"
'vdd' is the value of power supply needed to compute AWE delays.
RETURN VALUE
gns_AWE_GetWorstInstance returns the name of this instance, NULL if the instance doesn't exist.
ERRORS
none.
*/

char    *gns_AWE_GetWorstInstance ( Netlist *netlist, char *insname, Connector *lc, double vdd);

//_____________________________________________________________________________
/*
MAN gns_AWE_GetBestInstance
CATEG gns
DESCRIPTION
Retrieves the best instance (worst AWE delay) 
which is connected to the connector 'lc'.
'netlist' is the hierarchical netlist.
'insname' is the generic name of the instance to reduce.
RETURN VALUE
For example, if the netlist contains "mem_cell.0", "mem_cell.1", "mem_cell.2" ...
'insname' is "mem_cell"
'vdd' is the value of power supply needed to compute AWE delays.
gns_AWE_GetBestInstance returns the name of this instance, NULL if the instance doesn't exist.
ERRORS
none.
*/

char    *gns_AWE_GetBestInstance ( Netlist *netlist, char *insname, Connector *lc, double vdd);

//_____________________________________________________________________________
/*
MAN gns_AWE_KeepBestInstance
CATEG gns
DESCRIPTION
Keeps the best instance (best AWE delay) 
which is connected to the connector 'lc', the other instance are reduced.
'netlist' is the hierarchical netlist, 'flatnetlist' is the flattened netlist.
'insname' is the generic name of the instance to reduce.
For example, if the netlist contains "mem_cell.0", "mem_cell.1", "mem_cell.2" ...
'insname' is "mem_cell"
'vdd' is the value of power supply needed to compute AWE delays.
RETURN VALUE
gns_AWE_KeepBestInstance returns a flattened and reduced netlist, NULL if the instance doesn't exist.
ERRORS
none.
*/

Netlist *gns_AWE_KeepBestInstance  ( Netlist *netlist, Netlist *flatnetlist, char *insname, Connector *lc, double vdd);

//_____________________________________________________________________________
/*
MAN gns_AWE_KeepWorstInstance
CATEG gns
DESCRIPTION
Keeps the worst instance (worst AWE delay) 
which is connected to the connector 'lc', the other instance are reduced.
'netlist' is the hierarchical netlist, 'flatnetlist' is the flattened netlist.
'insname' is the generic name of the instance to reduce.
For example, if the netlist contains "mem_cell.0", "mem_cell.1", "mem_cell.2" ...
'insname' is "mem_cell"
'vdd' is the value of power supply needed to compute AWE delays.
RETURN VALUE
gns_AWE_KeepWorstInstance returns a flattened and reduced netlist, NULL if the instance doesn't exist.
ERRORS
none.
*/

Netlist *gns_AWE_KeepWorstInstance ( Netlist *netlist, Netlist *flatnetlist, char *insname, Connector *lc, double vdd);


/*
MAN gns_AWE_GetOrderedInstanceIndex
CATEG gns
DESCRIPTION
Creates and fills an array with the index of the instances on the signal connected to <connector>. The Instances are ordered with respect to their delay versus connector <connector>.
The <rule> defines how to retreive the instance index from the instance name. The '?' in the rule is the number desired. eg. "bitline(?).low(5).latch".
RETURN VALUE
returns an array that is to be deleted by the caller and the number of elements in the array.
ERRORS
none.
*/
void gns_AWE_GetOrderedInstanceIndex(Netlist *lofig, char *rule, Connector *connector, int **tab, int *nb);


/*
MAN gns_GetInstanceLoopIndex
CATEG gns
DESCRIPTION
Returns the index of <instance> and if <radical>!=NULL, fetch <radical> with the name of the instance in the model.
RETURN VALUE
returns an integer.
ERRORS
none.
SEE ALSO
gns_GetInstanceLoopRange
*/

int gns_GetInstanceLoopIndex(Instance *ins, char **ptptname);


/*
MAN gns_GetInstanceLoopRange
CATEG gns
DESCRIPTION
Returns the <left> and <right> range value of instances in a loop where <instance> is one of those instances. If <instance> is not in a loop, both <left> and <right> are assigned -1.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_GetInstanceLoopIndex
*/

void gns_GetInstanceLoopRange(Netlist *lf, Instance *ins, int *left, int *right);


/*
MAN gns_GetCorrespondingInstance
CATEG gns
DESCRIPTION
Returns the instance in the circuit corresponding to an instance in the model. An instance in a loop can be referenced using the instance name vectorized. <name> can be hierarchical.
RETURN VALUE
returns a `CorrespondingInstance *'.
ERRORS
returns NULL if the instance does not exist.
SEE ALSO
gns_GetCorrespondingInstanceConnectorSignal, gns_GetCorrespondingInstanceName
*/

CorrespondingInstance *gns_GetCorrespondingInstance(char *name);


/*
MAN gns_GetCorrespondingInstanceConnectorSignal
CATEG gns
DESCRIPTION
Returns the signal connected to the connector <name> of <instance>.
RETURN VALUE
returns a `Signal *'.
ERRORS
returns NULL if the connector <name> does not exist.
SEE ALSO
gns_GetCorrespondingInstance, gns_GetCorrespondingInstanceName
*/

Signal *gns_GetCorrespondingInstanceConnectorSignal(CorrespondingInstance *ins, char *name);


/*
MAN gns_GetCorrespondingInstanceName
CATEG gns
DESCRIPTION
Returns the name given by YagleGNS to a recognized instance.
RETURN VALUE
returns a string.
ERRORS
none.
SEE ALSO
gns_GetCorrespondingInstanceConnectorSignal, gns_GetCorrespondingInstance
*/
char *gns_GetCorrespondingInstanceName(CorrespondingInstance *crt);


char *gns_GetCorrespondingInstanceModelName(CorrespondingInstance *subins);


/*
MAN gns_GetGeneric
CATEG gns
DESCRIPTION
Returns the integer value of a generic variable in the current instance.
RETURN VALUE
returns an `integer'
ERRORS
returns 0 if the generic variable does not exist.
SEE ALSO
gns_GetCurrentArchi, gns_GetCurrentModel, gns_GetCurrentInstance
*/

int gns_GetGeneric(char *name);


/*
MAN gns_GetCurrentArchi
CATEG gns
DESCRIPTION
Returns the architecture name of the current instance.
RETURN VALUE
returns a `char *'
ERRORS
none.
SEE ALSO
gns_GetGeneric, gns_GetCurrentModel, gns_GetCurrentInstance
*/

char *gns_GetCurrentArchi();


/*
MAN gns_GetCurrentModel
CATEG gns
DESCRIPTION
Returns the model name of the current instance.
RETURN VALUE
returns a `char *'
ERRORS
none.
SEE ALSO
gns_GetCurrentArchi, gns_GetGeneric, gns_GetCurrentInstance
*/

char *gns_GetCurrentModel();


/*
MAN gns_GetCurrentInstance
CATEG gns
DESCRIPTION
Returns the name of the current instance.
RETURN VALUE
returns a `char *'
ERRORS
none.
SEE ALSO
gns_GetCurrentArchi, gns_GetGeneric, gns_GetCurrentModel
*/

char *gns_GetCurrentInstance();

/*
MAN callfunc
CATEG gns
DESCRIPTION
Generates a function call. The number of arguments is variable. When the results of 'callfunc' is used, <funcname> will be called with the given arguments.
RETURN VALUE
returns a `void *'
ERRORS
none.
SEE ALSO
*/
void *callfunc(char *funcname, ...);
void *callfunc_tcl(char *funcname, StringList *args);

void ASSOCIATE_BEHAVIOUR(void *func);

//_____________________________________________________________________________
/*
MAN gns_DriveSpiceNetlistGroup
CATEG gns
DESCRIPTION
Saves the <list> of netlist in <filename> using the spice format.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_DriveNetlist
*/
void gns_DriveSpiceNetlistGroup(List *list, char *filename);

//_____________________________________________________________________________
/*
MAN gns_AddCapa
CATEG gns
DESCRIPTION
Add <capa> between <con_name> member of <netlist> and the ground.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
*/
void gns_AddCapa(Netlist *fig, char *con_name, double capa);

//_____________________________________________________________________________
/*
MAN gns_AddResi
CATEG gns
DESCRIPTION
Add <resi> between <con1_name> and <con2_name> member of <netlist>.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
*/
void gns_AddResi(Netlist *fig, char *con1_name, char *con2_name, double resi);

//_____________________________________________________________________________
/*
MAN gns_AddLineRC
CATEG gns
DESCRIPTION 
Add <resi> between <con1_name> and <con2_name> member of <netlist>. Add <capa(n)> to <con(n)_name>
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
*/
void gns_AddLineRC(Netlist *fig, char *con1_name, char *con2_name, double resi, double capa1, double capa2);


//_____________________________________________________________________________
/*
MAN gns_RunGNS
CATEG gns
DESCRIPTION 
Initiate a GNS recognition on the specified netlist. <dir> is the directory where to search for GNS rules and actions. <lib> is the library file describing the rules and actions to use. If they are set to NULL, befault directory and library file will be used.
RETURN VALUE
returns a 'GNSRun *'
ERRORS
none.
SEE ALSO
gns_DestroyGNSRun
*/
GNSRun *gns_RunGNS(Netlist *netlist, char *celldir, char *libname);

/*
MAN gns_DestroyGNSRun
CATEG gns
DESCRIPTION 
Destroys the <gnsrun>.
RETURN VALUE
none
ERRORS
none.
SEE ALSO
gns_RunGNS
*/

void gns_DestroyGNSRun(GNSRun *afg);



/*
MAN gns_EnterGNSContext
CATEG gns
DESCRIPTION 
Changes GNS environment to match the one of <instance_name> in the <gnsrun>. The old environment is pushed into a stack. It can be retreive (poped) using gns_ExitGNSContext().
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_ExitGNSContext
*/
void gns_EnterGNSContext(GNSRun *gnsrun, char *instance);

/*
MAN gns_ExitGNSContext
CATEG gns
DESCRIPTION 
Changes GNS environment to the previous one.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_EnterGNSContext
*/
void gns_ExitGNSContext();

/*
MAN gns_GetBlackboxNetlist
CATEG gns
DESCRIPTION 
Returns the Netlist <name>. The Netlist is searched in the main netlist.
RETURN VALUE
returns a 'Netlist *' if the named figure exists, NULL otherwise.
ERRORS
none.
SEE ALSO
*/
Netlist *gns_GetBlackboxNetlist(char *name);

//_____________________________________________________________________________
/*
MAN gns_IsTopLevel
CATEG gns
DESCRIPTION 
Indicates if the current instance if a top level of genius recognition.
RETURN VALUE
returns 0 is the current instance is not at top level of GNS recognition otherwise, returns an integer different from 0.
ERRORS
none.
SEE ALSO
*/
int gns_IsTopLevel();

//_____________________________________________________________________________
/*
MAN gns_RenameInstanceFigure
CATEG gns
DESCRIPTION 
Changes the figure name of <instance> in <netlist>. The <original figure name> is replaced by <new figure name>. If instance is NULL, all instances will be checked for a figure name change.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
*/
void gns_RenameInstanceFigure(Netlist *lf, char *instance, char *origname, char *newname);

/*
MAN gns_FillBlackBoxes
CATEG gns
DESCRIPTION 
Will try to retreive the blackbox figures from the list of netlist <modellist> then in the original netlist, flatten the <netlist> blackbox instances to transistor level. The <modellist> is not freed.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_FlattenNetlist
*/
void gns_FillBlackBoxes(Netlist *lf, List *morenetlist);

char *_equiv(char *name);

/*
MAN gns_ChangeNetlistName
CATEG gns
DESCRIPTION 
Changes the name of the netlist <netlist> by <name>.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
*/
void gns_ChangeNetlistName(Netlist *lf, char *name);

/*
MAN gns_GetGNSTopLevels
CATEG gns
DESCRIPTION 
Returns the list of all the instances at the top level of the <gnsrun>. Each element of the list is a string containing an instance name.
RETURN VALUE
returns a 'List' of 'char *'
ERRORS
none.
SEE ALSO
gns_EnterGNSContext, gns_ExitGNSContext, gns_RunGNS, gns_DestroyGNSRun
*/
List *gns_GetGNSTopLevels(GNSRun *gnsrun);

/*
MAN gns_CutNetlist
CATEG gns
DESCRIPTION 
Returns a new netlist where the instances recognized at top level of <gnsrun> are cut. For each of those instances, a new figure at transistor level is also created and instanciated in the returned netlist. Concerning the parasitics, all couplings between blocks are put to ground and depending on the connector direction or user which, the RC trees are put inside or outside the instances.
RETURN VALUE
returns a 'Netlist *'
ERRORS
none.
SEE ALSO
gns_EnterGNSContext, gns_ExitGNSContext, gns_RunGNS, gns_DestroyGNSRun
*/
Netlist *gns_CutNetlist(GNSRun *gnsrun);

/*
MAN gns_ShowOutsideInfo
CATEG gns
DESCRIPTION 
Drives the connections of signal <signal> outside of the GNS instance. The result is put in file <file>.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
*/
void gns_ShowOutsideInfo(char *signame, FILE *f);

/*
MAN gns_REJECT_INSTANCE
CATEG gns
DESCRIPTION 
If called will result in the exclusion of the current instance from the instances to keep at the top level. This option overrides the GNS LIBRARY file settings.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_KEEP_INSTANCE, gns_REJECT_MODEL, gns_KEEP_MODEL
*/
void gns_REJECT_INSTANCE();

/*
MAN gns_KEEP_INSTANCE
CATEG gns
DESCRIPTION 
If called will result in the inclusion of the current instance in the instances to keep at the top level. This option overrides the GNS LIBRARY file settings.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_REJECT_INSTANCE, gns_REJECT_MODEL, gns_KEEP_MODEL
*/
void gns_KEEP_INSTANCE();

/*
MAN gns_REJECT_MODEL
CATEG gns
DESCRIPTION 
If called will result in the exclusion of all the instances of the current instance model from the instances to keep at the top level. This option overrides the GNS LIBRARY file settings.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_REJECT_INSTANCE, gns_KEEP_INSTANCE, gns_KEEP_MODEL
*/
void gns_REJECT_MODEL();

/*
MAN gns_KEEP_MODEL
CATEG gns
DESCRIPTION 
If called then all the instances of the current instance model are kept at the top level. This option overrides the GNS LIBRARY file settings.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
gns_REJECT_INSTANCE, gns_KEEP_INSTANCE, gns_REJECT_MODEL
*/
void gns_KEEP_MODEL();

/*
MAN gns_GetWorkingFigureName
CATEG gns
DESCRIPTION 
Returns the name of the figure the GNS recognition is working on.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
*/
char *gns_GetWorkingFigureName();

char *gen_info();

/*
MAN gns_IsVss
CATEG gns
DESCRIPTION 
Returns 1 if the signal <sig> is a vss alim signal, 0 otherwise.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
*/
int gns_IsVss(Signal *sig);

/*
MAN gns_IsVdd
CATEG gns
DESCRIPTION 
Returns 1 if the signal <sig> is a vdd alim signal, 0 otherwise.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
*/
int gns_IsVdd(Signal *sig);

/*
MAN gns_IsBlackBox
CATEG gns
DESCRIPTION
Returns 1 if the netlist corresponding with the current instance is a blackbox, 0 otherwise.
*/
int gns_IsBlackBox();

/*
MAN gns_GetSignalVoltage
CATEG gns
DESCRIPTION 
Returns the value of the voltage source set on the signal {name}. {name} is by delault a genius model signal name but if prefixed with 'ext:' the signal is considered to be in the original netlist.
If no voltage has been set on the signal, value -10000 is returned.
*/
double gns_GetSignalVoltage(char *name);

/*
MAN gns_GetSignalVoltageSwing
CATEG gns
DESCRIPTION 
Gives the voltage swing of the signal {name}. {low} is the lowest voltage possible on the signal and {high} the highest. {name} is must genius model signal name.
RETURN VALUE
returns 0 if the default swing was returned, 1 otherwise.
*/
int gns_GetSignalVoltageSwing(char *name, double *low, double *high);
