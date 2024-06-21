#!/usr/bin/env avt_shell

set writesig "write_in"

set figname dram
avt_config simToolModel hspice
avt_config simTool ngspice
avt_config simTechnologyName "../techno/bsim4_dummy.hsp"
avt_config avtSpiceString "../../bin/Linux/ngspice -b $"

avt_config avtVddName "vdd:vddprch"
avt_config avtVssName "vss"
avt_config avtGlobalVddName "vdd:vddprch"
avt_config avtGlobalVssName "vss"

avt_config yagUseGenius yes
avt_config avtOutputBehaviorFormat vlg

avt_config avtOutputNetlistFormat vlg
avt_config avtVerboseConeFile yes
avt_config yagGenerateConeFile yes
avt_config yagTasTiming max
avt_config avtSpiFlags KeepBBOXContent
avt_config yagleVectorizeInterface yes
avt_config yagleIgnoreBlackboxes yes
avt_config avtSpiKeepCards all
avt_config apiDriveCorrespondenceTable yes
avt_config apiUseCorrespondenceTable yes
avt_config avtVerilogKeepNames yes

avt_SetBlackBoxes {cells}

inf_SetFigureName $figname

avt_LoadFile dram.net spice

yagle $figname
