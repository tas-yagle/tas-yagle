#include <stdio.h>
#include "avt_API_types.h"

TimingFigure *tas (char *figname);
TimingFigure *hitas_sub (char *figname, char cnsannotatelofig, char loadcnsfig);
void tas_command_line (char *args);

/*
    MAN hitas
    CATEG tcl+gen
    SYNOPSIS
    <TimingFigure *> hitas <figname> [-annotatefromcns] [-startfromcns]\$
    \$
    DESCRIPTION
    Generates a timing figure from a pre- or post-layout transistor netlist. It is assumed that an internal representation of the netlist exists in the program's memory, i.e., that the related files (including MOS models) have already been loaded. See {avt_LoadFile} function.
    ARGS
    figname % Name of the subcircuit the timing figure is to be derived
    -annotatefromcns % [experimental] If a {$filename}.cns file exist, disassembling circuit stage is replaced by annotate the circuit from cns file information.
    -startfromcns % [experimental] Same as {-annotatefromcns} expect that the netlist saved in the cns file is used, not the user loaded one.
    EXAMPLE % {set fig [hitas my_design]}
*/
void hitas (void);

/*
    MAN hitas_pvt_count
    CATEG tcl+gen
    DESCRIPTION
    Returns the number of PVT errors encountered in the last hitas run.
    ARGS
    EXAMPLE % {set pvterrors [hitas_pvt_count]}
*/
int hitas_pvt_count();

