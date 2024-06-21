#!/usr/bin/env avt_shell


set fig [ttv_LoadSpecifiedTimingFigure mips_chip]

vcd_parsefile mips.vcd mips1

SwitchingPower $fig

puts ""
puts "Switching power (W): "
puts [GetSwitchingPowerSignal $fig "*" -interval 100ns -begindate 1us -enddate 3us -plot "plot1"]
puts ""
puts "Switching energy (J): "
puts [GetSwitchingPowerSignal $fig "*" -begindate 1us -enddate 3us -plot "plot2"]
puts ""

