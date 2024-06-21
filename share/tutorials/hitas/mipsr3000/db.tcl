#!/usr/bin/env avt_shell

avt_config avtLogFile run.log
avt_config avtLogEnable "error:1"

avt_config avtErrorPolicy lenient

# Global Configuration
avt_config simToolModel hspice
avt_config simPowerSupply 3.5
avt_config avtVddName "vdd:vddp"
avt_config avtVssName "vss:vssp"

avt_config avtVerboseConeFile yes

avt_config avtPowerCalculation yes
avt_config tmaLeakagePowerUnit "1uW"

avt_LoadFile top.spi spice

set fig [hitas mips_chip]

