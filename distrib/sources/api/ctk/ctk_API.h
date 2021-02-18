#include "avt_API_types.h"

int ctk_WorstCase();

int ctk_BestCase();

int ctk_Active();

void ctk_API_AtLoad_Initialize();

void SET_CTK_NETLIST(Netlist *netlist);

Netlist *GET_CTK_NETLIST();

void SET_CTK_CONTEXT(CtkContext *cc);

CtkContext *GET_CTK_CONTEXT();

void ctk_SetCtkRatio(CtkContext *cc, Signal *victim, Signal *aggressor, double ratio);

void ctk_SetInternalCrosstalkToGround(CtkContext *cc, double ratio);

void ctk_SetExternalCrosstalkRatio(CtkContext *cc, double ratio);

List *ctk_GetAllVictims();

CtkContext *ctk_CreateContext();

void ctk_FreeContext(CtkContext *cc);

/* functions in ctk_API.c related to crosstalk information */

/*
    MAN ctk_LoadAggressionFile
    CATEG tcl+build
    DESCRIPTION
    Loads the crosstalk database (generated with {stb} in crosstalk mode), and back-annotates a stability figure
    ARGS
    stbfig % Pointer on the stability figure to back-annotate
*/
extern void ctk_LoadAggressionFile( StabilityFigure *stbfig );

/*------------------
    MAN ctk_LoadCrosstalkResults
    CATEG tcl+build
    SYNOPSIS
    ctk_LoadCrosstalkResults <{timingfigure}>
    DESCRIPTION
    Puts the UTD in the same state as after the crosstalk analysis was done. The corresponding StabilityFigure is returned.
    Functions 'ttv_LoadCrosstalkFile', 'stb_LoadSwitchingWindows' then 'ctk_LoadAggressionFile' are called to do this action.
    ARGS
    timingfigure     % TimingFigure to put into post-crosstalk analysis state
    EXAMPLE % {set stabfig [ctk_LoadCrosstalkResults $myUTD]}.
*/

void ctk_LoadCrosstalkResults(void); // (void) disables the gns wrappers

/*
    MAN ctk_DriveStatCtk
    CATEG tcl+build
    DESCRIPTION
    Drives a {.ctk} crosstalk analysis report file
    ARGS
    stbfig % Pointer on the stability figure associated
*/
extern void ctk_DriveStatCtk( StabilityFigure *stbfig );

/*
    MAN ctk_BuildCtkStat
    CATEG tcl+build
    DESCRIPTION
    Builds the internal crosstalk table of statistics for events and delays
    ARGS
    stbfig % Pointer on the stability figure associated
*/
void ctk_BuildCtkStat( StabilityFigure *stbfig );

/*
    MAN ctk_GetStatNodeProperty
    CATEG tcl+statistics
    DESCRIPTION 
    Returns a property of a crosstalk node. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    stbfig %  Pointer on a stability figure
    index % Number between 1 and the value returned by {ctk_GetNumberOfCtkStatNode}
    property % Property code; for available property codes, see the {StatNode} object section
*/

Property* ctk_GetStatNodeProperty( StabilityFigure *stbfig, int index, char *property );

/*
    MAN ctk_GetAggressorList
    CATEG tcl+aggressor
    DESCRIPTION 
    Returns the list of all the aggressors of an event. This list must be freed with the {ctk_FreeAggressorList} command.
    ARGS
    stbfig %  Pointer on a stability figure
    event % Pointer on a timing event
*/
AggressorList* ctk_GetAggressorList( StabilityFigure *stbfig, TimingEvent *event );

/*
    MAN ctk_GetAggressorProperty
    CATEG tcl+aggressor
    DESCRIPTION 
    Returns a property of a aggressor node. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    aggressor %  Pointer on a aggressor
    property % Property code; for available property codes, see the {Aggressor} object section
*/
Property* ctk_GetAggressorProperty( Aggressor *aggressor, char *property );

/*
    MAN ctk_FreeAggressorList
    CATEG tcl+aggressor
    DESCRIPTION 
    Frees the list given by {ctk_GetAggressorList}
    ARGS
    list %  Pointer on the aggressor list to be freed
*/
void ctk_FreeAggressorList( AggressorList *list );

/*
    MAN tcl+ctk_SortCtkStatNode
    CATEG sort
    DESCRIPTION 
    Sorts crosstalk noise statistics according to various criteria
    ARGS
    stbfig %  Pointer on a stability figure
    criterion % {SCORE}, {SCORE_NOISE}, {SCORE_ACTIVITY}, {SCORE_CTK}, {SCORE_INTERVAL}, {NOISE_INSIDE_ALIM_MAX}, {NOISE_INSIDE_ALIM_REAL}, {NOISE_RISE_MAX}, {NOISE_RISE_REAL}, {NOISE_FALL_MAX} or {NOISE_FALL_REAL}.
*/

void ctk_SortCtkStatNode( StabilityFigure *stbfig, char *criterion );

/*
    MAN ctk_GetNumberOfCtkStatNode
    CATEG tcl+statistics
    DESCRIPTION 
    Returns the number crosstalk node statistics
    ARGS
    stbfig % Pointer on a stability figure
*/
int ctk_GetNumberOfCtkStatNode( StabilityFigure *stbfig );

/*
    MAN ctk_GetCtkStatNodeFromEvent
    CATEG tcl+statistics
    DESCRIPTION 
    Returns the crosstalk statistic index of a given timing event
    ARGS
    stbfig %  Pointer on a stability figure
    event % Pointer on a timing event
*/
int ctk_GetCtkStatNodeFromEvent( StabilityFigure *stbfig, TimingEvent *event );

/*
    MAN ctk_SortCtkStatLine
    CATEG tcl+sort
    DESCRIPTION 
    Sorts crosstalk delay statistics according to various criteria
    ARGS
    stbfig %  Pointer on a stability figure
    criterion % {ABSOLUTE_DELAY}, {ABSOLUTE_MAX_DELAY}, {ABSOLUTE_MIN_DELAY}, {RELATIVE_DELAY}, {RELATIVE_MAX_DELAY}, {RELATIVE_MIN_DELAY}, {ABSOLUTE_SLOPE}, {ABSOLUTE_MAX_SLOPE}, {ABSOLUTE_MIN_SLOPE}, {RELATIVE_SLOPE}, {RELATIVE_MAX_SLOPE} or {RELATIVE_MIN_SLOPE}.
*/
void ctk_SortCtkStatLine( StabilityFigure *stbfig, char *criterion );

/*
    MAN ctk_GetNumberOfCtkStatLine
    CATEG tcl+statistics
    DESCRIPTION 
    Returns the number crosstalk delay statistics
    ARGS
    stbfig % Pointer on a stability figure
*/
int ctk_GetNumberOfCtkStatLine( StabilityFigure *stbfig );

/*
  MAN ctk_GetStatLineProperty
  CATEG tcl+statistics
  DESCRIPTION 
    Returns a property of a crosstalk delay line. A Property is a polymorphic type, the returned type depends on the property code.
    ARGS
    stbfig %  Pointer on a stability figure
    index % Number between 1 and the value returned by {ctk_GetNumberOfCtkStatLine}
    property % Property code; for available property codes, see the {StatLine} object section
*/
Property* ctk_GetStatLineProperty( StabilityFigure *stbfig, int index, char *property );
