#include <stdio.h>
#include "avt_API_types.h"

void beg_API_Restart();
void beg_API_AtLoad_Initialize();
void beg_API_Action_Initialize();
void beg_API_TopLevel(long list);

/*
  MAN begCreateModel
  CATEG gns
  DESCRIPTION
  Initializes a behavioral model with the given name making it the current model.
  RETURN VALUE
  none
  ERRORS
  none
*/

void begCreateModel(char *name);

/*
  MAN begCreatePort
  CATEG gns
  DESCRIPTION
  Adds an I/O port to the current model.
  RETURN VALUE
  none
  ERRORS
  none
*/

void begCreatePort(char *name, char direction);

/*
  MAN begCreateModelFromConnectors
  CATEG gns
  DESCRIPTION
  Initializes a behavioral model with the given name and interface, making it the current model.
  RETURN VALUE
  none
  ERRORS
  none
*/

void begCreateModelFromConnectors(char *name, List *connectors);

/*
  MAN begCreateModelInterface
  CATEG gns
  DESCRIPTION
  Initializes a behavioral model with the given name and the physical model interface, making it the current model.
  RETURN VALUE
  none
  ERRORS
  none
*/

void begCreateModelInterface(char *name);

/*
  MAN begCreateInterface
  CATEG gns
  DESCRIPTION
  Initializes a behavioral model and the physical model interface, making it the current model. The model name will be handled by the API.
  RETURN VALUE
  none
  ERRORS
  none
*/
void begCreateInterface();

/*
  MAN begWriteCorrespondanceList
  CATEG gns
  DESCRIPTION
  Drive the correspondence table  between the physical model and the behavioral model.
  RETURN VALUE
  none
  ERRORS
  none
*/

//void begWriteCorrespondanceList(char *name);

/*
  MAN begRenameSignalsFromModel
  CATEG gns
  DESCRIPTION
  Rename the behavioral model interface with the name of physical connectors.
  If the corresponding signal is not a connector the behavioral name is prefixed
  by the behavioral model name.
  RETURN VALUE
  none
  ERRORS
  none
*/

void begRenameSignalsFromModel();

/*
  MAN begAddInstanceModel
  CATEG gns
  DESCRIPTION
  Add a behavioral model into the current behavioral model.
  RETURN VALUE
  none
  ERRORS
  none
*/

//void begAddInstanceModel(Instance *loins);

/*
  MAN xxxxbegAddAllInstanceModels
  CATEG gns
  DESCRIPTION
  Add all the instances previoulsy described into the current behavioral model. 
  RETURN VALUE
  none
  ERRORS
  none
*/

void begAddAllInstanceModels(int compact);

/*
  MAN begAssign
  CATEG gns
  SYNOPSIS
    begAssign [-weak|-strong] <name> <value> [delay [delayvar]]
  DESCRIPTION
  Creates a simple concurrent assignment in the current model.
  RETURN VALUE
  none
  ERRORS
  none
*/

void begAssign(char *name, char *expr, int delay, char *delayvar);
void begAssign_sub(char *name, char *expr, TimeValue delay, TimeValue delayr, TimeValue delayf, char *delayvar, int flags);

/*
  MAN begAddBusDriver
  CATEG gns
  SYNOPSIS
   in C:\$
     begAddBusDriver(char *name, char *condition, char *value, int delay, char *delayvar)\$
   in TCL:\$
     begAddBusDriver [-normal] [-weak|-strong] [-delays <risedelay> <falldelay>] <name> <condition> <value> [delay [delayvar]]\$
  DESCRIPTION
  Adds a driver to a given bussed signal of the current model, creating the signal if necessary.
  ARGS
  -normal % In verilog, will force the signal to be assigned in a sequential block.
  -weak or -strong % In verilog, defines the strength of the driver.
  -delays <risedelay> <falldelay> % Specifies different values for rising and falling. No effect if field <delay> is used.
  name % Affected signal name.
  value % Affected expression.
  condition % Condition for the value to be affected.
  delay % Delay of the operation. Default is 0ps.
  delayvar % Delay variable name for defining the delay later.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begAddBusDriver(char *name, char *condition, char *value, int delay, char *delayvar);
void begAddBusDriver_sub(char *name, char *condition, char *value, TimeValue delay, TimeValue delayr, TimeValue delayf, char *delayvar, int flags);

/*
  MAN begAddBusElse
  CATEG gns
  SYNOPSIS
   begAddBusElse [-normal] [-weak|-strong] [-delays <risedelay> <falldelay>] <name> <condition> <value> [delay [delayvar]]\$
  DESCRIPTION
  Adds an else alternative to the previous driver of a given bussed signal of the curent model, creating the signal if necessary.
  ARGS
  -normal % In verilog, will force the signal to be assigned in a sequential block.
  -weak or -strong % In verilog, defines the strength of the driver.
  -delays <risedelay> <falldelay> % Specifies different values for rising and falling. No effect if field <delay> is used.
  name % Affected signal name.
  value % Affected expression.
  condition % Condition for the value to be affected.
  delay % Delay of the operation. Default is 0ps.
  delayvar % Delay variable name for defining the delay later.
*/

void begAddBusElse(char *name, char *condition, char *value, int delay, char *delayvar);
void begAddBusElse_sub(char *name, char *condition, char *value, TimeValue delay, TimeValue delayr, TimeValue delayf, char *delayvar, int flags);

/*
  MAN begSaveModel
  CATEG gns
  DESCRIPTION
  Saves the current model to disk.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
/*
  MAN begAddBusDriverLoop
  CATEG gns
  DESCRIPTION
  Adds a loop driver to a given bussed signal of the current model, creating the signal if necessary.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begAddBusDriverLoop(char *name, char *condition, char *value, char *loopvar, int delay, char *delayvar);

/*
  MAN begAddBusDriverDoubleLoop
  CATEG gns
  DESCRIPTION
  Adds a loop driver to a given bussed signal of the current model, creating the signal if necessary.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begAddBusDriverDoubleLoop(char *name, char *condition, char *value, char *loopvar1, char *loopvar2, int delay, char *delayvar);

/*
  MAN begAddMemDriver
  CATEG gns
  SYNOPSIS
   in C:\$
     begAddMemDriver(char *name, char *condition, char *value, int delay, char *delayvar)\$
   in TCL:\$
     begAddMemDriver [-normal] [-weak|-strong] [-delays <risedelay> <falldelay>] <name> <condition> <value> [delay [delayvar]]\$
  DESCRIPTION
  Adds a driver to a given register signal of the curent model, creating the signal if necessary.
  ARGS
  -normal % In verilog, will force the signal to be assigned in a sequential block.
  -weak or -strong % In verilog, defines the strength of the driver.
  -delays <risedelay> <falldelay> % Specifies different values for rising and falling. No effect if field <delay> is used.
  name % Affected signal name.
  value % Affected expression.
  condition % Condition for the value to be affected.
  delay % Delay of the operation. Default is 0ps.
  delayvar % Delay variable name for defining the delay later.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begAddMemDriver(char *name, char *condition, char *value, int delay, char *delayvar);
void begAddMemDriver_sub(char *name, char *condition, char *value, TimeValue delay, TimeValue delayr, TimeValue delayf, char *delayvar, int flags);

/*
  MAN begAddMemDriverLoop
  CATEG gns
  DESCRIPTION
  Adds a loop driver to a given register signal of the curent model, creating the signal if necessary.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begAddMemDriverLoop(char *name, char *condition, char *value, char *loopvar, int delay, char *delayvar);

/*
  MAN begAddMemDriverDoubleLoop
  CATEG gns
  DESCRIPTION
  Adds a loop driver to a given register signal of the curent model, creating the signal if necessary.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begAddMemDriverDoubleLoop(char *name, char *condition, char *value, char *loopvar1, char *loopvar2, int delay, char *delayvar);

/*
  MAN begAddMemElse
  CATEG gns
  SYNOPSIS
   begAddMemElse [-normal] [-weak|-strong] [-delays <risedelay> <falldelay>] <name> <condition> <value> [delay [delayvar]]\$
  DESCRIPTION
  Adds an else alternative to the previous driver of a given register signal of the curent model, creating the signal if necessary.
  ARGS
  -normal % In verilog, will force the signal to be assigned in a sequential block.
  -weak or -strong % In verilog, defines the strength of the driver.
  -delays <risedelay> <falldelay> % Specifies different values for rising and falling. No effect if field <delay> is used.
  name % Affected signal name.
  value % Affected expression.
  condition % Condition for the value to be affected.
  delay % Delay of the operation. Default is 0ps.
  delayvar % Delay variable name for defining the delay later.
*/

void begAddMemElse(char *name, char *condition, char *value, int delay, char *delayvar);
void begAddMemElse_sub(char *name, char *condition, char *value, TimeValue delay, TimeValue delayr, TimeValue delayf, char *delayvar, int flags);

/*
  MAN begSaveModel
  CATEG gns
  DESCRIPTION
  Saves the current model to disk.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begSaveModel();

/*
  MAN begKeepModel
  CATEG gns
  DESCRIPTION
  Finalize the current custom-built model.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begKeepModel();

/*
  MAN begDestroyModel
  CATEG gns
  DESCRIPTION
  Destroy the current model.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begDestroyModel();

/*
  MAN begVectorize
  CATEG gns
  DESCRIPTION
  Generate a name of type toto(n).
  RETURN VALUE
  A pointer to the name string.
  ERRORS
  none.
*/

char *begVectorize(char *radical, int index);

/*
  MAN begVarVectorize
  CATEG gns
  DESCRIPTION
  Generate a name of type toto(n).
  RETURN VALUE
  A pointer to the name string.
  ERRORS
  none.
*/

char *begVarVectorize(char *radical, char *var);

/*
  MAN begVectorRange
  CATEG gns
  DESCRIPTION
  Generate a name of type toto(l:r).
  RETURN VALUE
  A pointer to the name string.
  ERRORS
  none.
*/

char *begVectorRange(char *radical, int left, int right);

/*
  MAN begAddWarningCheck
  CATEG gns
  DESCRIPTION
  Add a assertion statement which generates a warning on activation.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begAddWarningCheck(char *testexpr, char *message);

/*
  MAN begAddErrorCheck
  CATEG gns
  DESCRIPTION
  Add a assertion statement which generates an error on activation.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begAddErrorCheck(char *testexpr, char *message);

/*
  MAN begSort
  CATEG gns
  DESCRIPTION
  Sort the drivers in the current behavioural model.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begSort();

/*
  MAN begCompact
  CATEG gns
  DESCRIPTION
  Compact the current behavioural model by vectorization and loop detection.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begCompact();

/*
  MAN begSetDelay
  CATEG gns
  DESCRIPTION
  Set the timing delay value associated with a particular delay variable declared by any of the expression creation functions. 
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begSetDelay(char *varname, int value);

/*
  MAN begBuildModel
  CATEG gns
  DESCRIPTION
  Automatically create a standard (nom-compacted) behavioral model for a recognized structural model.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begBuildModel();

/*
  MAN begBuildModelFromFiles
  CATEG gns
  DESCRIPTION
  Automatically create current behavioral model for a physical structural model.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

//void begBuildModelFromFiles(char *name);

/*
  MAN begBuildCompactModel
  CATEG gns
  DESCRIPTION
  Automatically create a compact behavioral model for a recognized structural model.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begBuildCompactModel();

/*
  MAN begBuildCompactModelFromFiles
  CATEG gns
  DESCRIPTION
  RETURN VALUE
  none.
  ERRORS
  none.
*/

//void begBuildCompactModelFromFiles(char *name);

/*
  MAN begBiterize
  CATEG gns
  DESCRIPTION
  Unvectorise current behavioral model.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begBiterize();

/*
  MAN begAddSelectDriver
  CATEG gns
  DESCRIPTION
  Add a with select description.
  'select' is the name of the signal selected,
  'value' is affected to 'name' when 'when' match to 'select'.
  When others is represented by 'when' set to string 'default'.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void begAddSelectDriver(char *name, char *select, char *when ,char *value, int delay, char *delayvar);

/*
  MAN begExport
  CATEG gns
  DESCRIPTION
  begExport(<name>) creates a copy of the current behavioural figure. The new figure gets the name <name>. This function is useful when it is needed to associate a known user name to a behaviour to easily retreive it knowing it's new name.
  RETURN VALUE
  none.
  ERRORS
  none.
  SEE ALSO
  begImport
*/

void begExport(char *name);

/*
  MAN begImport
  CATEG gns
  DESCRIPTION
  begImport(<name>) retreive the behaviour named <name> and merge it into the current behavioural figure.
  RETURN VALUE
  none.
  ERRORS
  none.
  SEE ALSO
  begExport
*/

void begImport(char *name);

void begSwitchMode();
