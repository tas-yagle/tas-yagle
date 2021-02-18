#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Global Configuration

avt_config simVthHigh 0.8
avt_config simVthLow 0.2
avt_config tasBefig yes
avt_config tasGenerateConeFile yes

# Simulation Config
avt_config avtSpiceString     "../../bin/Linux/ngspice -b $ -o $.stdout"
avt_config SimToolModel ngspice
avt_config simTechnologyName ../techno/bsim4.tec
avt_config simOutLoad dynamic
avt_config avtTechnologyName ../techno/bsim4.tec

avt_LoadFile library.inc spice

foreach cell { ao2o22 ff2 inv mux2 na2 nand2x1 no3 ts xor2 } {
    inf_SetFigureName $cell
    if { $cell == "ff2" } { create_clock -period 2.8 -waveform { 0 1.4 } ck }

    inf_DefineSlopeRange default [list 50e-12 100e-12 200e-12 400e-12 800e-12] custom
    inf_DefineCapacitanceRange default [list 50e-15 100e-15 200e-15 400e-15 800e-15] custom

    set fig [hitas $cell]
    set beh_fig [beh_getbefig $cell]
    
    set abs_fig [tmabs $fig $beh_fig * * * -verbose -simulate *]

    lappend fig_list $abs_fig
    lappend beh_fig_list $beh_fig
}

lib_drivefile $fig_list $beh_fig_list "stdcells_simu.lib" max
