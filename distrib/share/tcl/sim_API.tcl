
proc __sim__get_or_bit { flags } {
 set v M
 foreach f $flags {
   switch -exact $f {
     SIM_MAX      { set v M }
     SIM_MIN      { set v m }
     default      { puts "Invalid flag '$f'"}
   }
 }
 return $v
}

proc sim_AddSpiceMeasure {sc delay slope sig1 sig2 tran type} {
 set rflags [__sim__get_or_bit $type]
 sim_AddSpiceMeasure_sub $sc $delay $slope $sig1 $sig2 $tran $rflags
}

proc sim_AddSpiceMeasureDelay {sc delay sig1 sig2 tran type} {
 set rflags [__sim__get_or_bit $type]
 sim_AddSpiceMeasureDelay_sub $sc $delay $sig1 $sig2 $tran $rflags
}

proc sim_AddSpiceMeasureSlope {sc slope sig1 tran type} {
 set rflags [__sim__get_or_bit $type]
 sim_AddSpiceMeasureSlope_sub $sc $slope $sig1 $tran $rflags
}

proc cpe_DefineCorrelation {sig1 op sig2} {
 global tcl_interactive
 if {$op=="="} {
   gsp_AddCorrelation $sig1 $sig2 0
   gsp_AddCorrelation $sig2 $sig1 0
 } elseif {$op=="!="} {
   gsp_AddCorrelation $sig1 $sig2 1
   gsp_AddCorrelation $sig2 $sig1 1
 } else {
   puts "Unknown relation '$op'"
   puts "Usage: cpe_DefineCorrelation <net1> = <net2>"
   puts "    or cpe_DefineCorrelation <net1> != <net2>"
   if {!$tcl_interactive} {
     exit
   }
 }
}
