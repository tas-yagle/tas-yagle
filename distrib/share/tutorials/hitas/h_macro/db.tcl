#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# General configuration
avt_config avtLibraryDirs ".:../adder:../cpu2901:"
avt_config avtVddName vdd
avt_config avtVssName vss
avt_config simVthHigh 0.8
avt_config simVthLow 0.2

# Database configuration
avt_config tasHierarchicalMode yes

avt_LoadFile ./ram4x128.lib lib
avt_LoadFile top.v verilog
avt_LoadFile top.spef spef

set fig [hitas top]
