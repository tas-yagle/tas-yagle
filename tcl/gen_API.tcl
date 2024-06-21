
proc callfunc {funcname args} {
  return [callfunc_tcl $funcname $args]
}

proc __gns__get_or_bit { flags } {
 set orbit 0
 set v 0
 foreach f $flags {
   switch -exact $f {
     IN_RC        { set v 1 }
     INTERNAL_RC  { set v 2 }
     OUT_RC       { set v 4 }
     NO_RC        { set v 8 }
     ALL_RC       { set v 16 }
     XTALK_TO_GND { set v 32 }
     default      { puts "Invalid flag '$f'"}
   }
   set orbit [expr $orbit|$v]
 }
 return $orbit
}

proc gns_FlattenNetlist {netlist flags} {
 set rflags [__gns__get_or_bit $flags]
 return [gns_FlattenNetlist_sub $netlist $rflags]
}

proc gns_AddRC {netlist flags} {
 set rflags [__gns__get_or_bit $flags]
 return [gns_AddRC_sub $netlist $rflags]
}

proc begAddBusDriver_any {func argsx} {
  set idx 0
  set flag 0
  set delayR 0
  set delayF 0

  if {[lindex $argsx $idx]=="-normal"} {
    set flag 1
    incr idx
  }
  if {[lindex $argsx $idx]=="-weak"} {
    set flag [expr $flag|2]
    incr idx
  } elseif {[lindex $argsx $idx]=="-strong"} {
    set flag [expr $flag|4]
    incr idx
  } elseif {[lindex $argsx $idx]=="-pull"} {
    set flag [expr $flag|8]
    incr idx
  }
  if {[lindex $argsx $idx]=="-delays"} {
    incr idx
    set delayR [lindex $argsx $idx]
    incr idx
    set delayF [lindex $argsx $idx]
    incr idx
  } 
  
  set arg4 [lindex $argsx [expr $idx+3]]
  set arg5 [lindex $argsx [expr $idx+4]]
  if {$arg4==""} {
    set arg4 0
  }
  if {$arg5==""} {
    set arg5 NULL
  }
  $func [lindex $argsx $idx] [lindex $argsx [expr $idx+1]] [lindex $argsx [expr $idx+2]] $arg4 $delayR $delayF $arg5 $flag
}

proc begAddMemDriver {args} {
  begAddBusDriver_any "begAddMemDriver_sub" $args
}

proc begAddBusDriver {args} {
  begAddBusDriver_any "begAddBusDriver_sub" $args
}

proc begAddMemElse {args} {
  begAddBusDriver_any "begAddMemElse_sub" $args
}

proc begAddBusElse {args} {
  begAddBusDriver_any "begAddBusElse_sub" $args
}

proc begAssign {args} {
  set idx 0
  set flag 0
  set delayR 0
  set delayF 0

  if {[lindex $args $idx]=="-weak"} {
    set flag [expr $flag|2]
    incr idx
  } elseif {[lindex $args $idx]=="-strong"} {
    set flag [expr $flag|4]
    incr idx
  } elseif {[lindex $args $idx]=="-pull"} {
    set flag [expr $flag|8]
    incr idx
  }
  if {[lindex $args $idx]=="-delays"} {
    incr idx
    set delayR [lindex $args $idx]
    incr idx
    set delayF [lindex $args $idx]
    incr idx
  } 
  set arg3 [lindex $args [expr $idx+2]]
  set arg4 [lindex $args [expr $idx+3]]
  if {$arg3==""} {
    set arg3 0
  }
  if {$arg4==""} {
    set arg4 NULL
  }
  begAssign_sub [lindex $args $idx] [lindex $args [expr $idx+1]] $arg3 $delayR $delayF $arg4 $flag
}

