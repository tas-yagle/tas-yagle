proc hitas { args } {
  global tcl_interactive
  set filename ""
  set annotatefromcns N
  set startfromcns N
  
  foreach arg $args {
  
    if { $arg == "-annotatefromcns" } {
      set annotatefromcns Y
    } elseif { $arg == "-startfromcns" } {
      set startfromcns Y
    } else {
      set filename $arg
    }
  }

  if { $filename == "" } {
    puts "Figure name not specified."
    puts "Usage : hitas <figure name> \[-annotatefromcns]"
    if {!$tcl_interactive} {
      exit 1
    } else {
      return [_NULL_]
    }
  }
  
  return [ hitas_sub $filename $annotatefromcns $startfromcns ]
}
