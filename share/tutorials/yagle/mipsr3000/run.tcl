#!/usr/bin/env avt_shell

avt_config avtLogFile run.log
avt_config avtLogEnable "error:1"

# Global Configuration
avt_config simToolModel hspice
avt_config simPowerSupply 3.5
avt_config avtVddName "vdd:vddp"
avt_config avtVssName "vss:vssp"

avt_config avtVerboseConeFile yes

avt_config yagleTasTiming max
avt_config avtOutputBehaviorFormat vlg
avt_config avtVerilogInertialMemory yes
avt_config avtVerilogOldStyleBus no
avt_config yagDetectGlitchers no
avt_config yagBusAnalysis yes

avt_LoadFile top.spi spice

yagle mips_chip
