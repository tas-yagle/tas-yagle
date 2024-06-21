#include "avt_API_types.h"

#define INPUT_CLOCK        1
#define OUTPUT_CLOCK       2
#define MAIN_CLOCK         3

/****************************************************************************/
/****************************************************************************/
/* Stability Back Annotation */
/****************************************************************************/
/****************************************************************************/

/*
    MAN stb
    CATEG tcl+gen
    DESCRIPTION
    Launches the static timing analysis, based upon SDC and INF constraints.
    The analysis can be tuned with configuration variables through {avt_Config ()}.
    The function returns a stability figure, which is a mapping of stabilty information (switching windows) on the timing figure.
    ARGS
    tf % Pointer on the timing figure the analysis is to be run on
*/
StabilityFigure *stb (TimingFigure *tf);

/*
    MAN stb_LoadSwitchingWindows
    CATEG tcl+gen
    DESCRIPTION
    Reads an STO file ({.sto} extension), result of a static timing analysis ({stb} funcion). Back-annotates (maps) a timing figure with stability information.
Warning: it is not yet compatible with false paths configuration.
    ARGS 
    tvf % Pointer on the timing figure to back-annotate
    filename % Name of the STO file to load
*/
StabilityFigure *stb_LoadSwitchingWindows (TimingFigure *tvf, char *filename);

/*
    MAN stb_FreeStabilityFigure
    CATEG tcl+gen
    DESCRIPTION
    Deletes a stability figure from memory
    ARGS
    sf % Pointer on the stability figure to be freed
*/
void stb_FreeStabilityFigure (StabilityFigure *sf);

/*
    MAN stb_GetStabilityFigureProperty
    CATEG tcl+gen
    DESCRIPTION
    Returns a property of a stability figure. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    sf %  Pointer on the stability figure.
    code % Property code; for available property codes, see the {StabilityFigure} object section.
*/
Property *stb_GetStabilityFigureProperty (StabilityFigure *sf, char *property);

/****************************************************************************/
/****************************************************************************/
/* Violations */
/****************************************************************************/
/****************************************************************************/

/*
    MAN stb_DisplayErrorList
    CATEG tcl+error
    DESCRIPTION
    Prints a report of signals presenting timing violations (setup and hold)
    ARGS
    f % File where to save the report, {stdout} for standard output
    stbfig % Pointer on a stability figure
    margin % Value below which a setup/hold slack is reported as a violation. For example, if the margin is 100ps (100e-12 second), a signal with a setup or a hold slack below 100ps will be reported as a violation.
    nberror % Maximum number of reported violations

*/
void stb_DisplayErrorList (FILE *f, StabilityFigure *stbfig, double margin, int nberror);

/*
    MAN stb_GetErrorList
    CATEG tcl+error
    DESCRIPTION
    Returns a sorted list of the signals having the worst violations.
    ARGS
    sf % Pointer on a stability figure
    margin % Value below which a setup/hold slack is reported as a violation. For example, if the margin is 100ps (100e-12 second), a signal with a setup or a hold slack below 100ps will be reported as a violation.
    nberror % Maximum number of reported violations
*/
TimingSignalList *stb_GetErrorList (StabilityFigure *sf, double margin, int nberror);


/*
    MAN stb_GetSetupSlack
    CATEG tcl+error
    DESCRIPTION
    Returns the setup slack value of a given signal. If there is no setup slack value, -1 is returned. 
    ARGS
    fig % Pointer on a stability figure
    signame % Name of the signal to consider 
    dir % {u}, {d} or {?}; reports slack value for a given transition  
*/
double stb_GetSetupSlack (StabilityFigure *fig, char *signame, char dir);

/*
    MAN stb_GetHoldSlack
    CATEG tcl+error
    DESCRIPTION
    Returns the hold slack value of a given signal. If there is no hold slack value, -1 is returned. 
    ARGS
    fig % Pointer on a stability figure
    signame % Name of the signal to consider 
    dir % {u}, {d} or {?}; reports slack value for a given transition  
*/
double stb_GetHoldSlack  (StabilityFigure *fig, char *signame, char dir);
/*
    MAN stb_DisplaySlackReport
    CATEG tcl+error
    SYNOPSIS
    stb_DisplaySlackReport <FILE *file> <StabilityFigure *stbfig> [-from <nodename>] [-to <nodename>] [-thru_node <nodename>] [-dir <dir>] [-nbslacks <nb>] [-margin <marginval>] [-noprechlag] [common-options]\$
    or\$
    stb_DisplaySlackReport <FILE *file> -slacks <stabilityslackslist> [-nbslacks <nb>] [common-options]\$
    \$
    common-options:\$
    [-setuponly] [-holdonly] [-simple] [-summary] [-displaythru] [-displaymargins]\$
    \$
    Obsolete but still working:\$
    stb_DisplaySlackReport(FILE *f, StabilityFigure *stbfig, char *start, char *end, char *dir, int number, char *mode, TimeValue margin)
    DESCRIPTION
    Retreives and prints or only prints a slack report.
    ARGS
    file % File where to save the report, {stdout} for standard output.
    stbfig % Stability figure.
    -from {<nodename>} % DATA VALID start point (clock or input pin).
    -to {<nodename>} % DATA VALID end point (latch, precharge, output or directive node).
    -thru_node {nodename} % DATA VALID access node when the DATA VALID is an acces path.
    -dir {<dir>} % Start and end points transitions, {u}, {d}, {?}. {z} or {/} can be added to the usual direction to respectively display only HZ slacks or only non-HZ slacks. Default is {??}.
    -nbslacks {<nb>} % Maximum number of reported setup and hold slacks. Default is unlimited ({<nb>}<=0).
    -margin {<marginval>} % Value below which a setup/hold slack is reported as a violation. For example, if the margin is 100ps (100e-12 second), a signal with a setup or a hold slack below 100ps will be reported as a violation. Default is {0}.
    -noprechlag % Removes slacks whose DATA VALID passes thru transparent precharges when DATA VALID is an access.
    common-options: %
    -setuponly % Displays only setup slacks.
    -holdonly % Displays only hold slacks.
    -summary % Displays only the summary of the slacks.
    -displaythru % Displays access node in the summary if the DATA VALID path is an access.
    -displaymargins % Displays the path margins and the intrinsic margin in the summary.
    -simple % Displays less detailed slack details by hiding period change operations.
    EXAMPLE % To display an already obtained StabilitySlack list:\${stb_DisplaySlackReport $myfile -slacks $myslacklist -displaythru -simple -summary -displaymargins}\$To display a slack report:\${stb_DisplaySlackReport $myfile $sf -margin 100e-9 -from myclock -to mylatch -dir ?f -simple}
    obsolete options: %
    mode % Selects the displayed slack types: {setup}, {hold}, {all}, {margins}, {summary setup}, {summary hold} or {summary all}. {all} means {setup} and {hold}. {margins} will show in the summary the internal margins used for the slack computation. {thru} shows in the summary the latch through which the data arrives as well as the latch command which enables this data.
*/
void stb_DisplaySlackReport_sub(FILE *f, StabilityFigure *stbfig, char *start, char *end, char *dir, int number, char *mode, TimeValue margin);
void stb_DisplaySlackReport_sub2(FILE *f, StabilitySlackList *solist, int number, char *mode);

void stb_DisplaySlackReport(void); // (void) disables the gns wrappers

/*
    MAN xxxxxstb_UpdateSlacks
    CATEG error
    DESCRIPTION
    Recomputes all the slacks 
    ARGS
    stbfig % Stability figure
*/
void stb_UpdateSlacks (StabilityFigure *stbfig);

/*
    MAN stb_DisplayCoverage
    CATEG tcl+error
    SYNOPSIS
    stb_DisplayCoverage <FILE *file> <StabilityFigure *stbfig> [-detail]\$
    DESCRIPTION
    Prints a coverage report concerning all the signals checked or not checked in the design.
    ARGS
    file % File where to save the report, {stdout} for standard output.
    stbfig % Stability figure.
    -detail % Output the list of the signals not checked.
    EXAMPLE % {stb_DisplayCoverage stdout $sfig -detail}
*/
void stb_DisplayCoverage(void);
void stb_DisplayCoverage_sub(FILE *f, StabilityFigure *sf, int detail);

/*
    MAN stb_GetSlacks
    CATEG tcl+error
    SYNOPSIS
    <StabilitySlackList *> stb_GetSlacks <StabilityFigure *stbfig> [-from <nodename>] [-to <nodename>] [-thru_node <nodename>] [-dir <dir>] [-nbslacks <nb>] [-margin <marginval>] [-noprechlag] [-setuponly] [-holdonly]\$
    \$
    Obsolete but still working:\$
    StabilitySlackList *stb_GetSlacks(StabilityFigure *stbfig, char *start, char *end, char *dir, int number, char *mode, TimeValue margin)\$
    DESCRIPTION
    Returns a list of stability slack objects.
    ARGS
    stbfig % Stability figure.
    -from {<nodename>} % DATA VALID start point (clock or input pin).
    -to {<nodename>} % DATA VALID end point (latch, precharge, output or directive node).
    -thru_node {nodename} % DATA VALID access node when the DATA VALID is an acces path.
    -dir {<dir>} % Start and end points transitions, {u}, {d}, {?}. {z} or {/} can be added to the usual direction to respectively display only HZ slacks or only non-HZ slacks. Default is {??}.
    -nbslacks {<nb>} % Maximum number of retreived setup and hold slacks. Default is unlimited ({<nb>}<=0).
    -margin {<marginval>} % Value below which a setup/hold slack is reported as a violation. For example, if the margin is 100ps (100e-12 second), a signal with a setup or a hold slack below 100ps will be reported as a violation. Default is {0}.
    -noprechlag % Removes slacks whose DATA VALID passes thru transparent precharges when DATA VALID is an access.
    -setuponly % Retreives only setup slacks.
    -holdonly % Retreives only hold slacks.
    EXAMPLE % {set sl [stb_GetSlacks $sf -from ck -to end* -setuponly -nbslacks 10]}\$Obsolete:\${set sl [stb_GetSlacks $sf * * ?? 0 setup 100e-9]}
*/
void stb_GetSlacks(void);
StabilitySlackList *stb_GetSlacks_sub(StabilityFigure *stbfig, char *start, char *end, char *dir, int number, char *mode, TimeValue margin, char *thru, int nolagprech);

/*
    MAN stb_FreeSlackList
    CATEG tcl+error
    DESCRIPTION
    Frees a list of stability slack objects.
    ARGS
    cl % slack object list
*/
void stb_FreeSlackList(StabilitySlackList *cl);

/*
    MAN stb_GetSlackProperty
    CATEG tcl+error
    DESCRIPTION
    Returns a property of a slack object. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    so %  Pointer on the slack object
    code % Property code; for available property codes, see the {StabilitySlack} object section
*/
Property *stb_GetSlackProperty (StabilitySlack *so, char *property);


/*
    MAN stb_SortSlacks
    CATEG tcl+error
    SYNOPSIS
    <StabilitySlackList *> stb_SortSlacks [<StabilitySlackList *> l1] [<StabilitySlackList *> l2] ...
    DESCRIPTION
    Concatenates the given slack lists and sorts the resulting list from the worst slack to the best.
    ARGS
    l1, l2, ... % StabilitySlack list to merge and sort.
    EXAMPLE % {set result [stb_SortSlacks $slacklist1 $slacklist2 $slacklist3]}
*/
void stb_SortSlacks(void); // (void) disables the gns wrappers

/*
    MAN stb_ComputeSlacks
    CATEG tcl+error
    SYNOPSIS
    <StabilitySlackList *> stb_ComputeSlacks <StabilityFigure *sf> <char *datavalid> <operation> <char *datarequired> [options]\$
    or\$
    <StabilitySlackList *> stb_ComputeSlacks <TimingPathList *datavalid> <operation> <TimingPathList *datarequired> [options]\$
    options:\$
    [-nextperiod] [-margin <val>] [-nosync]
    DESCRIPTION
    Computes slacks for a combination of data valid path list and required path list.\$
    The data valid path and data required path can be given as a specification or directly as a path list.\$
    A specification is a string containing the arguments to be given to the ttv_ProbeDelay API without mentioning the Timing Figure argument which is deduced from the Stability Figure. If "-min" or "-max" is not specified in a specification, "-max" for data valid and "-min" for data required is assumed for {before} operation and the opposite for {after} operation.
    ARGS
    sf % Stability figure.
    datavalid % DATA VALID specification or path list.
    datarequired % DATA REQUIRED specification or path list.
    operation % Operation to do between the two lists. Value can be {before} or {after}. {before} corresponds to a setup and {after} to a hold.
    options: %
    -margin <val> % Adds an additionnal margin to the calculus. The higher the margin the worse is the slack values. If a negative value is given and the datariquired path ends on the command of the dala valid path end latch, the UTD latch to command intrinsic margin is will be used.
    -nextperiod % If the data required path and the data valid path comes from the same clock edge, tells the API that the data required is generated by the next cycle.
    -nosync % Disables all kind of guessed data required period changes done automatically by the API hence the given paths are compared directly.
    EXAMPLE % By using the specifications:\${set slacks [stb_ComputeSlacks $sf "\{-con=ck* -access-latch=convlat convsig\}" before "\{ck1 convck convsig\} -dir ?d" -margin 1ps]}\$By using the path lists:\${set slacks [stb_ComputeSlacks $data_paths_min after $clock_path_max -margin 2ps -nextperiod]}
*/
void stb_ComputeSlacks(void); // (void) disables the gns wrappers


/*
    MAN stb_FindLagPaths
    CATEG tcl+error
    SYNOPSIS
    stb_FindLagPaths <file> <slacklist> [-margin <value>] [-lagmargin <value>] [-maxdepth <number>] [-closingpath]
    DESCRIPTION
    Reports the lag paths for each given slack. This API can help find possible false paths generating data lag hence creating wrong negative slacks.
    ARGS
    file % File where to save the report, {stdout} for standard output.
    slacklist % List of slacks to analyse.
    -margin <value> % Only analyse slacks whose value is lower than or equal to {value}. Default is 1s (i.e. all given slacks).
    -lagmargin <value> % Only analyse slacks where the data lag value is greater than {value}. Default is 0.
    -maxdepth <number> % Stop backtracking after a maximum of {number} paths for each slack. Default is 3.
    -closingpath % Displays the closing clock path in case of huge lags where the latch closing is responsible for the lag maximum value.
    EXAMPLE % stb_FindLagPaths stdout $myslacks -lagmargin 100e-12
*/
void stb_FindLagPaths(void); // (void) disables the gns wrappers

/*
    MAN stb_DriveReport
    CATEG tcl+error
    DESCRIPTION
    Drives a report file corresponding to a stability figure
    ARGS
    stbfig % Stability figure
    filename % File where to save the report
*/
void stb_DriveReport (StabilityFigure *stbfig, char *filename);

/****************************************************************************/
/****************************************************************************/
/* Stability Source */
/****************************************************************************/
/****************************************************************************/

/*
    MAN stb_GetSignalStabilityPaths
    CATEG source
    DESCRIPTION
    Returns the list of paths producing violations on a given signal
    ARGS
    stbfig % Stability figure
    output % Name of the timing signal to consider

*/
StabilityPathList *stb_GetSignalStabilityPaths(StabilityFigure *stbfig, char *output);

/*
    MAN stb_FreeStabilityPathList
    CATEG source
    DESCRIPTION
    Deletes a list of stability paths from memory
    ARGS
    path_list % Pointer on the list of paths to be freed
*/
void stb_FreeStabilityPathList (StabilityPathList *path_list);

/*
    MAN stb_DisplaySignalStabilityReport
    CATEG source
    DESCRIPTION
    Reports setup and hold slack values of a given signal. Data path is also reported for both setup and access values
    ARGS
    f % File where to save the report, {stdout} for standard output
    stbfig % Stability figure
    name % Name of the timing signal to consider
*/
void stb_DisplaySignalStabilityReport (FILE *f, StabilityFigure *stbfig, char *name);


/****************************************************************************/
/****************************************************************************/
/* Stability Source Properties */
/****************************************************************************/
/****************************************************************************/

/*
    MAN stb_GetStabilityPathProperty
    CATEG prop
    DESCRIPTION
    Returns a property of a stability path. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    fig %  Pointer on the stability path
    code % Property code; for available property codes, see the {StabilityPath} object section
*/
Property *stb_GetStabilityPathProperty (StabilityPath *path, char *property);

/*
    MAN stb_GetClockTime
    CATEG prop
    DESCRIPTION
    Returns the edges times of a clock
    ARGS
    path % Pointer on a stability path
    clock % INPUT_CLOCK for clock attached to input, OUTPUT_CLOCK for clock attached to output, MAIN_CLOCK for path reference clock
    dir % {u} for rising edge, {d} for falling edge
*/
double stb_GetClockTime(StabilityPath *path, int clock, char dir);

/*
    MAN stb_GetClockDelta
    CATEG prop
    DESCRIPTION
    Returns the clock unstability duration after the time given by {stb_GetClockTime}.
    ARGS
    path % Pointer on a stability path
    clock % INPUT_CLOCK for clock attached to input, OUTPUT_CLOCK for clock attached to output, MAIN_CLOCK for path reference clock
    dir % {u} for rising edge, {d} for falling edge
*/
double stb_GetClockDelta(StabilityPath *path, int clock, char dir);

/*
    MAN stb_DisplayClock
    CATEG prop
    DESCRIPTION
    Reports all edges times for the clock related to a given path
    ARGS
    f % File where to save the report, {stdout} for standard output
    path % Pointer on a stability path
    clock % INPUT_CLOCK for clock attached to input, OUTPUT_CLOCK for clock attached to output, MAIN_CLOCK for path reference clock

*/
void stb_DisplayClock (FILE *f, StabilityPath *path, int clock);

/*
    MAN stb_DisplayInfos
    CATEG prop
    DESCRIPTION
    Reports setup and hold slack information about a given path
    ARGS
    f % File where to save the report, {stdout} for standard output
    path % Pointer on a stability path
*/
void    stb_DisplayInfos (FILE *f, StabilityPath *path);

/*
    MAN stb_DisplayInputInfos
    CATEG prop
    DESCRIPTION
    Reports all information about a given path
    ARGS
    f % File where to save the report, {stdout} for standard output
    path % Pointer on a stability path

*/
void    stb_DisplayInputInfos (FILE *f, StabilityPath *path);

/****************************************************************************/
/****************************************************************************/
/* Switching Windows */
/****************************************************************************/
/****************************************************************************/

/*
    MAN stb_GetInputInstabilityRanges
    CATEG browse
    DESCRIPTION
    Returns a list of switching windows on the source node of a path
    ARGS
    path % Pointer on a stability path
    dir % Selects the transition to consider on the source node; rising ({u}), falling ({d}) or both ({m})
*/
StabilityRangeList *stb_GetInputInstabilityRanges(StabilityPath *path, char dir);

/*
    MAN stb_GetOutputInstabilityRanges
    CATEG browse
    DESCRIPTION
    Returns a list of switching windows on the sink node of a path
    ARGS
    path % Pointer on a stability path
    dir % Selects the transition to consider on the sink node; rising ({u}), falling ({d}) or both ({m})
*/
StabilityRangeList *stb_GetOutputInstabilityRanges(StabilityPath *path, char dir);

/*
    MAN stb_GetOutputSpecificationRanges
    CATEG browse
    DESCRIPTION
    Returns a list of switching windows derived from the {set_output_delay} constraints.
    ARGS
    path % Pointer on a stability path
    dir % Selects the transition to consider on the sink node; rising ({u}), falling ({d}) or both ({m})
*/
StabilityRangeList *stb_GetOutputSpecificationRanges(StabilityPath *path, char dir);

/*
    MAN stb_GetInstabilityRangeStart
    CATEG browse
    DESCRIPTION
    Returns the starting time of a switching window
    ARGS
    range % Pointer on the switching window to consider
*/

double stb_GetInstabilityRangeStart(StabilityRange *range);

/*
    MAN stb_GetInstabilityRangeEnd
    CATEG browse
    DESCRIPTION
    Returns the ending time of a switching window
    ARGS
    range % Pointer on the switching window to consider
*/

double stb_GetInstabilityRangeEnd(StabilityRange *range);

/*
    MAN stb_DisplayInputInstabilityRanges
    CATEG browse
    DESCRIPTION
    Reports switching windows on the source node of a path
    ARGS
    f % File where to save the report, {stdout} for standard output
    path % Pointer on a stability path
*/
void stb_DisplayInputInstabilityRanges (FILE *f, StabilityPath *path);

/*
    MAN stb_DisplayOutputInstabilityRanges
    CATEG browse
    DESCRIPTION
    Reports switching windows on the sink node of a path
    ARGS
    f % File where to save the report, {stdout} for standard output
    path % Pointer on a stability path
*/
void stb_DisplayOutputInstabilityRanges (FILE *f, StabilityPath *path);

/*
    MAN stb_DisplayOutputSpecificationRanges
    CATEG browse
    DESCRIPTION
    Reports a list of switching windows derived from the {set_output_delay} constraints.
    ARGS
    f % File where to save the report, {stdout} for standard output
    path % Pointer on a stability path

*/
void stb_DisplayOutputSpecificationRanges (FILE *f, StabilityPath *path);

/****************************************************************************/
/****************************************************************************/
/* others */
/****************************************************************************/
/****************************************************************************/


/*
    MAN ssta_ToolBox
    CATEG tcl+montecarlo
    SYNOPSIS
    ssta_ToolBox [-parsedatafile] [-getfield <filednum>] [-getdistrib] [options]\$
    options:\$
    [-values <valuelist>] [-filename <fname>]\$
    [-parsefunction <funcname>] [-fixedrange <time>] [-nbrange <number>]\$
    DESCRIPTION
    Handles various operations related to statistical timing analysis results.\$
    ARGS
    -parsedatafile % Reads a SSTA data file and returns its content as a list (one entry per SSTA run) of 2 element lists with the SSTA run number and the line corresponding to the SSTA run from the file: \{\{<ssta run number> <file line>\} \{...\} ...\}.
    -getdistrib % Returns a distribution from the list of values. The distribution is a list of list with 5 elements: \{\{<low range value> <high range value> <number of occurence> <cumulative number of occurence> <cumulative % of occurence>\} \{...\} ...\}.
    -getfield <filednum> % Reads and returns the {<filednum>}'th field of each line of a file: \{<line1 field value> <line2 field value> ...\}.
    options: %
    -values <valuelist> % List of values to use.\$Works with {-getdistrib}.
    -filename <fname> % Filename of the file to read from or to drive to.\$Works with {-getfield}, {-parsedatafile}.
    -parsefunction <funcname> % Function name to run while parsing result file. Each line parsed is feed to the function as one argument which is a list with 2 values: \{<ssta run number> <file line>\}. When {-parsefunction} is used, the API returns nothing.\$Works with {-parsedatafile}.
    -nbrange <number> % Specifies the number of range to use when building a distribution. The default value is {20}.\$Works with {-getdistrib}.
    -fixedrange <time> % Specifies a range length rather than a range number to build a distribution.\$Works with {-getdistrib}.    
    EXAMPLE % ssta_ToolBox -getdistrib -values \{1 3 5 1 8\}
*/
void ssta_ToolBox(void); // (void) disables the gns wrappers

/*
    MAN ssta_PathReport
    CATEG tcl+montecarlo
    SYNOPSIS
    Usage 1: ssta_PathReport -senddata <path list> <methodname>\$
    Usage 2: ssta_PathReport -display <ssta result file> <filedescriptor>\$
    DESCRIPTION
    Sends a predefined monte-carlo result from a given path list to the master ssta script or drives a report from predefined sent data file.
    ARGS
    Usage 1: %
    -senddata <path list> <methodname> % Sends a predefined result extracted from the given {<path list>}. {<path list>} is standard path list obtained thru ttv_GetPaths for instance. Only one method (kind of results to output) exists for now called "simple".
    Usage 2: %
    -display <ssta result file> <filedescriptor> % Uses the predefined data writen into result file {<ssta result file>} to output a specific report concerning the path monte-carlo characteristics. {<filedescriptor>} if the result of a "fopen" command call in with the report will be written.
    EXAMPLE % ssta_ToolBox -senddata $mypathlist simple
*/
void ssta_PathReport(void); // (void) disables the gns wrappers


/*
    MAN ssta_SlackReport
    CATEG tcl+montecarlo
    SYNOPSIS
    Usage 1: ssta_SlackReport -senddata <stability figure> <methodname>
    Usage 2: ssta_SlackReport -display <ssta result file> <filedescriptor>
    Usage 3: ssta_SlackReport -plot <ssta result file> <output filename>
    DESCRIPTION
    Sends a predefined monte-carlo result from a given path list to the master ssta script or drives a report from predefined sent data file.
    ARGS
    Usage 1: %
    -senddata <stability figure> <methodname> % Sends a predefined result extracted from the given {<stability figure>}. Only one method (kind of results to output) exists for now called "simple".
    Usage 2: %
    -display <ssta result file> <filedescriptor> % Uses the predefined data writen into result file {<ssta result file>} to output a specific report concerning the path monte-carlo characteristics. {<filedescriptor>} if the result of a "fopen" command call in with the report will be written.
    Usage 3: %
    -plot <ssta result file> <output filename> % Uses the predefined data writen into result file {<ssta result file>} to output a gnuplot view of the distributions of worst setup and hold slacks. The gnuplot file output are prefixed with {<output filename>}.
    EXAMPLE % ssta_ToolBox -senddata $stbfig simple\$or\$ssta_SlackReport -plot "ssta_data.log" "distrib"
*/
void ssta_SlackReport(void); // (void) disables the gns wrappers


/****************************************************************************/
/****************************************************************************/
/* Internal or obsolete */
/****************************************************************************/
/****************************************************************************/

void    stb_DisplayErroneousSignals (FILE *f, TimingSignalList *errorlist);
double __stb_GetSignalHold(TimingSignal *tvs, char dir);
double __stb_GetSignalSetup(TimingSignal *tvs, char dir);

StabilityPathList *stb_internal_GetSignalStabilityPaths(TimingSignal *output, long moreopt);
void stb_DisplayDataRoute(FILE *f, StabilityPath *spath, char type, char datadir);
TimingSignal    *stb_GetPathInputSignal(StabilityPath *path);
TimingSignal    *stb_GetPathOutputSignal(StabilityPath *path);
char            *stb_GetClockName (StabilityPath *path);
char            *stb_GetCommandName (StabilityPath *path);
double           stb_GetClockPeriod (StabilityPath *path);
double           stb_GetSetup (StabilityPath *path);
double           stb_GetHold (StabilityPath *path);
double           stb_GetSetupMargin (StabilityPath *path);
double           stb_GetHoldMargin (StabilityPath *path);
void stb_DisplaySetupTimeReport (FILE *f, StabilityPath *spath, char datadir);
void stb_DisplayHoldTimeReport (FILE *f, StabilityPath *spath, char datadir);

void stb_trackstbpath (StabilityFigure *stbfig, char *nodename, char dir, double timeo);
StabilitySlack *stb_ComputeOneSlack(int setup, TimingPath *data, TimingPath *clock, TimeValue margin, int nextcycle, int nosync);
double stb_synchronized_slopes_move(StabilityFigure *ptstbfig, TimingEvent *startnode, TimingEvent *endclock);

