       

#---------------------------------------------------------------------
#
proc Debug_Table {margin_list} {
    puts "TABLE"
    set li [llength $margin_list]
    set i 0
    while {$i < $li} {
        set margin_by_data [lindex $margin_list $i]
        set lj [llength $margin_by_data]
        set j 0
        while {$j < $lj - 1} {
            puts -nonewline "[lindex $margin_by_data $j] "
	        incr j
        }
        puts "[lindex $margin_by_data $j]"
	    incr i
    }
    puts " "
    puts " "
}

#---------------------------------------------------------------------
#
proc Print_Table {margin_list} {
    set li [llength $margin_list]
    set i 0
    while {$i < $li} {
        if {$i == 0} {puts -nonewline "                values (\""}
        if {$i > 0}  {puts -nonewline "                        \""}
        set margin_by_data [lindex $margin_list $i]
        set lj [llength $margin_by_data]
        set j 0
        while {$j < $lj - 1} {
            puts -nonewline "[lindex $margin_by_data $j], "
	        incr j
        }
        if {$i < $li - 1} {
            puts "[lindex $margin_by_data $j]\", \\"
        } else {
            puts "[lindex $margin_by_data $j]\");"
        }
	    incr i
    }
}

#---------------------------------------------------------------------
#
proc Get_Sum {margin_list} {
    set sum 0
    set margin_list [lindex $margin_list 0]
    set li [llength $margin_list]
    set i 0
    while {$i < $li} {
        set margin_by_data [lindex $margin_list $i]
        set lj [llength $margin_by_data]
        set j 0
        while {$j < $lj} {
            set sum [expr "$sum + [lindex $margin_by_data $j]"]
	        incr j
        }
	    incr i
    }
    return $sum
}

#---------------------------------------------------------------------
#
proc DebugTimingGroup {pin clocks rise_constraint rise_debug fall_constraint fall_debug type mode} {
    set canonic_pin [lib_CanonicPinName $pin]
    set canonic_ck [lib_CanonicPinName $clocks]
    if {$type == "setup_rising"} {
        if {$rise_constraint != ""} {
            puts "--- SETUP $canonic_pin (R) $canonic_ck (R)"
            PrintPath "DATA PATH" [lindex $rise_debug 0]
            PrintPath "CLOCK PATH" [lindex $rise_debug 1]
            Debug_Table $rise_constraint
        }
        if {$fall_constraint != ""} {
            puts "--- SETUP $canonic_pin (F) $canonic_ck (R)"
            PrintPath "DATA PATH" [lindex $fall_debug 0]
            PrintPath "CLOCK PATH" [lindex $fall_debug 1]
            Debug_Table $fall_constraint
        }
    } elseif {$type == "setup_falling"} {
        if {$rise_constraint != ""} {
            puts "--- SETUP $canonic_pin (R) $canonic_ck (F)"
            PrintPath "DATA PATH" [lindex $rise_debug 0]
            PrintPath "CLOCK PATH" [lindex $rise_debug 1]
            Debug_Table $rise_constraint
        }
        if {$fall_constraint != ""} {
            puts "--- SETUP $canonic_pin (F) $canonic_ck (F)"
            PrintPath "DATA PATH" [lindex $fall_debug 0]
            PrintPath "CLOCK PATH" [lindex $fall_debug 1]
            Debug_Table $fall_constraint
        }
    } elseif {$type == "hold_rising"} {
        if {$rise_constraint != ""} {
            puts "--- HOLD $canonic_pin (R) $canonic_ck (R)"
            PrintPath "DATA PATH" [lindex $rise_debug 0]
            PrintPath "CLOCK PATH" [lindex $rise_debug 1]
            Debug_Table $rise_constraint
        }
        if {$fall_constraint != ""} {
            puts "--- HOLD $canonic_pin (F) $canonic_ck (R)"
            PrintPath "DATA PATH" [lindex $fall_debug 0]
            PrintPath "CLOCK PATH" [lindex $fall_debug 1]
            Debug_Table $fall_constraint
        }
    } elseif {$type == "hold_falling"} {
        if {$rise_constraint != ""} {
            puts "--- HOLD $canonic_pin (R) $canonic_ck (F)"
            PrintPath "DATA PATH" [lindex $rise_debug 0]
            PrintPath "CLOCK PATH" [lindex $rise_debug 1]
            Debug_Table $rise_constraint
        }
        if {$fall_constraint != ""} {
            puts "--- HOLD $canonic_pin (F) $canonic_ck (F)"
            PrintPath "DATA PATH" [lindex $fall_debug 0]
            PrintPath "CLOCK PATH" [lindex $fall_debug 1]
            Debug_Table $fall_constraint
        }
    } elseif {$type == "rising_edge"} {
        if {$rise_constraint != ""} {
            puts "--- ACCESS $canonic_ck (R) $canonic_pin (R)"
            PrintPath "DATA PATH" [lindex $rise_debug 0]
            PrintPath "CLOCK PATH" [lindex $rise_debug 1]
            Debug_Table $rise_constraint
        }
        if {$fall_constraint != ""} {
            puts "--- ACCESS $canonic_ck (R) $canonic_pin (F)"
            PrintPath "DATA PATH" [lindex $fall_debug 0]
            PrintPath "CLOCK PATH" [lindex $fall_debug 1]
            Debug_Table $fall_constraint
        }
    } elseif {$type == "falling_edge"} {
        if {$rise_constraint != ""} {
            puts "--- ACCESS $canonic_ck (F) $canonic_pin (R)"
            PrintPath "DATA PATH" [lindex $rise_debug 0]
            PrintPath "CLOCK PATH" [lindex $rise_debug 1]
            Debug_Table $rise_constraint
        }
        if {$fall_constraint != ""} {
            puts "--- ACCESS $canonic_ck (F) $canonic_pin (F)"
            PrintPath "DATA PATH" [lindex $fall_debug 0]
            PrintPath "CLOCK PATH" [lindex $fall_debug 1]
            Debug_Table $fall_constraint
        }
    }
}

#---------------------------------------------------------------------
#
proc PrintTimingGroup {pin clocks rise_constraint rise_debug fall_constraint fall_debug type mode} {
    set canonic_pin [lib_CanonicPinName $pin]
    set canonic_ck [lib_CanonicPinName $clocks]
    if {$rise_constraint != "" || $fall_constraint != "" } {
        puts " "
        if {$type == "setup_rising"} {
            puts "            timing ($canonic_pin\_$canonic_ck\_setup_rising) \{"
            puts "                timing_type : setup_rising;"
        } elseif {$type == "setup_falling"} {
            puts "            timing ($canonic_pin\_$canonic_ck\_setup_falling) \{"
            puts "                timing_type : setup_falling;"
        } elseif {$type == "hold_rising"} {
            puts "            timing ($canonic_pin\_$canonic_ck\_hold_rising) \{"
            puts "                timing_type : hold_rising;"
        } elseif {$type == "hold_falling"} {
            puts "            timing ($canonic_pin\_$canonic_ck\_hold_falling) \{"
            puts "                timing_type : hold_falling;"
        } elseif {$type == "rising_edge"} {
            puts "            timing ($canonic_ck\_$canonic_pin\_rising_edge) \{"
            puts "                timing_type : rising_edge;"
        } elseif {$type == "falling_edge"} {
            puts "            timing ($canonic_ck\_$canonic_pin\_falling_edge) \{"
            puts "                timing_type : falling_edge;"
        }
        puts "                related_pin : $clocks;"
        if { $type == "rising_edge" || $type == "falling_edge" } {
            puts "                cell_rise (access_template) \{"
        } else {
            puts "                rise_constraint (setup_hold_template) \{"
        }
        Print_Table $rise_constraint
        puts "                \}"
        if { $type == "rising_edge" || $type == "falling_edge" } {
            puts "                cell_fall (access_template) \{"
        } else {
            puts "                fall_constraint (setup_hold_template) \{"
        }
        Print_Table $fall_constraint
        puts "                \}"
        puts "            \}"
    }
}

#---------------------------------------------------------------------
proc PrintPath { tag path } {
    set detail_list [ttv_GetPathDetail $path]
    puts -nonewline "$tag"
    foreach detail $detail_list {
        set net_name [ttv_GetTimingDetailProperty $detail NODE_NAME]
        set trans [ttv_GetTimingDetailProperty $detail TRANS]
        puts -nonewline " - $net_name ($trans) "
    }
    puts ""
}
#---------------------------------------------------------------------
#It gets all the interface latches
proc Interface_Latches { fig } { 
	set latch_list ""
	foreach latch [ttv_GetTimingSignalList $fig latch interface] {
        lappend latch_list [ttv_GetSignalName $latch]
	}
	return $latch_list
}

#---------------------------------------------------------------------
#It gets all the interface clocks
proc Interface_Commands { fig ck } { 
	set com_list ""
	foreach com [ttv_GetTimingSignalList $fig command interface] {
        lappend com_list [ttv_GetSignalName $com]
	}
	set red_com_list ""
    foreach com $com_list {
		set ckpath_list [ttv_GetPaths $fig $ck $com ?? 1000 critic path max]
        if {$ckpath_list != ""} { lappend red_com_list $com }
    }
    # add commands on interface
    lappend red_com_list $ck
	return $red_com_list
}

#---------------------------------------------------------------------
# Constructs all data_paths to interface latches clocked on clocks
proc DPLatch { fig source sink clock delay_type } {
    set all_max_data_paths [ttv_GetPaths $fig $source $sink ?? 1000 critic path $delay_type]

    # Refines data_paths to those ending on latches 
    set max_data_paths ""
    foreach data_path $all_max_data_paths {
        set com      [ttv_GetTimingPathProperty $data_path COMMAND]
        set com_sig  [ttv_GetTimingEventProperty $com SIGNAL]
        set com_name [ttv_GetTimingSignalProperty $com_sig NAME]
        set clock_path [ttv_GetPaths $fig $clock $com_name ?? 1000 critic path $delay_type]
	    if {$clock_path != ""} { lappend max_data_paths $data_path }
    }
    return $max_data_paths
}

#---------------------------------------------------------------------
#
proc Compute_Delay { fig slope load path delay_type slope_unit cap_unit } {
    set start_tran [ttv_GetTimingPathProperty   $path      START_TRAN]
    set end_tran   [ttv_GetTimingPathProperty   $path      END_TRAN]
    set start_sig  [ttv_GetTimingPathProperty   $path      START_SIG]
    set start_name [ttv_GetTimingSignalProperty $start_sig NAME]
    set end_sig    [ttv_GetTimingPathProperty   $path      END_SIG]
    set end_name   [ttv_GetTimingSignalProperty $end_sig   NAME]
    set sl [expr $slope * $slope_unit] 
    set ld [expr $load * $cap_unit] 
    set tran $start_tran$end_tran
    # no slope propagation         prop = 0
    # full slope propagation       prop = 1 
    # 1-stage slope propagation    prop = 2
    set prop 1
    set res_path [ttv_CharacPaths $fig $sl $start_name $end_name $tran 1 critic path $delay_type $ld $prop]
    set delay [ttv_GetTimingPathProperty $res_path REF_DELAY]
    return [expr $delay / $slope_unit]
}

#---------------------------------------------------------------------
#
proc Intrinsic_Margin { fig path com_sig type slope_unit } {
    
    set end_sig    [ttv_GetTimingPathProperty  $path END_SIG]
    set end_tran   [ttv_GetTimingPathProperty  $path END_TRAN]
    set start_sig  [ttv_GetTimingPathProperty  $path START_SIG]
    set start_tran [ttv_GetTimingPathProperty  $path START_TRAN]

    if {$type == "setup"} {
        set intrinsic [ttv_GetLatchSetup $fig $end_sig $end_tran $com_sig]
    } elseif {$type == "hold"} {
        set intrinsic [ttv_GetLatchHold $fig $end_sig $end_tran $com_sig]
    } elseif {$type == "access_min"} {
        set intrinsic [ttv_GetLatchAccess $fig $start_sig $start_tran $com_sig min]
    } elseif {$type == "access_max"} {
        set intrinsic [ttv_GetLatchAccess $fig $start_sig $start_tran $com_sig max]
    }
    return [expr $intrinsic / $slope_unit]
}

#---------------------------------------------------------------------
#
proc Global_Margin { fig slope_list data_path ck_path com_sig type slope_unit cap_unit } {
    # Tables construction 
    set data_delay_list ""
    set ck_delay_list ""

    foreach slope $slope_list {
        if {$type == "setup"} {
            # max data path
            # min clock path
            set data_delay [Compute_Delay $fig $slope 0 $data_path max $slope_unit $cap_unit]
            set ck_delay [Compute_Delay $fig $slope 0 $ck_path min $slope_unit $cap_unit]
        } elseif {$type == "hold"} {
            # min data path
            # max clock path
            set data_delay [Compute_Delay $fig $slope 0 $data_path min $slope_unit $cap_unit]
            set ck_delay [Compute_Delay $fig $slope 0 $ck_path max $slope_unit $cap_unit]
        }
        lappend data_delay_list [format "%.4f" $data_delay] 				  
        lappend ck_delay_list [format "%.4f" $ck_delay] 				  
    }

    # Setup/Hold calculation 
    set margin_list ""
    foreach data_delay $data_delay_list {
        set margin_by_data ""
        foreach ck_delay $ck_delay_list {
            set intrinsic_margin [Intrinsic_Margin $fig $data_path $com_sig $type $slope_unit]
            if {$type == "setup"} {
                set margin [expr $data_delay - $ck_delay + $intrinsic_margin]
            } elseif {$type == "hold"} {
                set margin [expr $ck_delay - $data_delay + $intrinsic_margin]
            }
            lappend margin_by_data [format "%.4f" $margin]
        }
        lappend margin_list $margin_by_data 
    }
    return $margin_list
}

#---------------------------------------------------------------------
#
proc Global_Prop { fig slope_list load_list path type slope_unit cap_unit } {
    # Tables construction 
    set delay_list_list ""

    foreach slope $slope_list {
        set delay_list ""
        foreach load $load_list {
            if {$type == "max"} {
                set delay [Compute_Delay $fig $slope $load $path max $slope_unit $cap_unit]
            } elseif {$type == "min"} {
                set delay [Compute_Delay $fig $slope $load $path max $slope_unit $cap_unit]
            }
            lappend delay_list [format "%.4f" $delay] 				  
        }
        lappend delay_list_list $delay_list 				  
    }

    return $delay_list_list
}

#---------------------------------------------------------------------
#
proc Global_Access { fig slope_list load_list data_path ck_path com_sig type slope_unit cap_unit } {
    # Tables construction 
    set data_delay_list ""
    set ck_delay_list ""

    foreach slope $slope_list {
        if {$type == "max"} {
            # max clock path
            set ck_delay [Compute_Delay $fig $slope 0 $ck_path max $slope_unit $cap_unit]
        } elseif {$type == "min"} {
            # min clock path
            set ck_delay [Compute_Delay $fig $slope 0 $ck_path min $slope_unit $cap_unit]
        }
        lappend ck_delay_list [format "%.4f" $ck_delay] 				  
    }

    foreach load $load_list {
        if {$type == "max"} {
            # max data path
            set data_delay [Compute_Delay $fig 0 $load $data_path max $slope_unit $cap_unit]
        } elseif {$type == "min"} {
            # min data path
            set data_delay [Compute_Delay $fig 0 $load $data_path min $slope_unit $cap_unit]
        }
        lappend data_delay_list [format "%.4f" $data_delay] 				  
    }

    # Setup/Hold/Access calculation 
    set margin_list ""
    foreach ck_delay $ck_delay_list {
        set margin_by_data ""
        foreach data_delay $data_delay_list {
            set intrinsic_margin [Intrinsic_Margin $fig $data_path $com_sig $type $slope_unit]
            set margin [expr $ck_delay + $data_delay + $intrinsic_margin]
            lappend margin_by_data [format "%.4f" $margin]
        }
        lappend margin_list $margin_by_data 
    }
    return $margin_list
}

#---------------------------------------------------------------------
#
proc extractSetupHold {slope slope_unit cap_unit fig clocks pin delay mode} {

    # Constructs the list of interface latches
	set all_latches [Interface_Latches $fig]

    # Constructs all data_paths to interface latches clocked on clocks
    set max_data_paths [DPLatch $fig $pin $all_latches $clocks max]

    # Build setup & hold
    set su_rr ""
    set su_rf ""
    set su_fr ""
    set su_ff ""
    set su_rr_debug ""
    set su_rf_debug ""
    set su_fr_debug ""
    set su_ff_debug ""
    set su_rr_sum -1000000
    set su_rf_sum -1000000
    set su_fr_sum -1000000
    set su_ff_sum -1000000

    set ho_rr ""
    set ho_rf ""
    set ho_fr ""
    set ho_ff ""
    set ho_rr_debug ""
    set ho_rf_debug ""
    set ho_fr_debug ""
    set ho_ff_debug ""
    set ho_rr_sum 1000000
    set ho_rf_sum 1000000
    set ho_fr_sum 1000000
    set ho_ff_sum 1000000

    foreach data_path $max_data_paths {

        # Clock path
        set com      [ttv_GetTimingPathProperty   $data_path COMMAND]
        set com_sig  [ttv_GetTimingEventProperty  $com       SIGNAL]
        set com_name [ttv_GetTimingSignalProperty $com_sig   NAME]
        set com_tran [ttv_GetTimingEventProperty  $com       TRANS]
        if {$com_tran == "u"} {
            set com_tran d
        } else {
            set com_tran u
        }
        set ck_path [ttv_GetPaths $fig $clocks $com_name ?$com_tran 1 critic path max]
        set ck_tran [ttv_GetTimingPathProperty $ck_path START_TRAN]

        # Transitions
        set start_tran   [ttv_GetTimingPathProperty   $data_path START_TRAN]
        set end_tran     [ttv_GetTimingPathProperty   $data_path END_TRAN]
        set end_sig      [ttv_GetTimingPathProperty   $data_path END_SIG]
        set end_sig_name [ttv_GetTimingSignalProperty $end_sig   NAME]

        # Setup 
        set setup_list [Global_Margin $fig $slope $data_path $ck_path $com_sig setup $slope_unit $cap_unit] 
        set sum [Get_Sum $setup_list]
        if {$ck_tran == "u"} {
            if {$start_tran == "u"} {
                if {$sum > $su_rr_sum} {
                    set su_rr_sum $sum
                    set su_rr $setup_list
                    if {$mode == "debug"} {
                        set su_rr_debug $data_path
                        lappend su_rr_debug $ck_path
                    }
                }
            }
            if {$start_tran == "d"} {
                if {$sum > $su_rf_sum} {
                    set su_rf_sum $sum
                    set su_rf $setup_list
                    if {$mode == "debug"} {
                        set su_rf_debug $data_path
                        lappend su_rf_debug $ck_path
                    }
                }
            }
        } elseif {$ck_tran == "d"} {
            if {$start_tran == "u"} {
                if {$sum > $su_fr_sum} {
                    set su_fr_sum $sum
                    set su_fr $setup_list
                    if {$mode == "debug"} {
                        set su_fr_debug $data_path
                        lappend su_fr_debug $ck_path
                    }
                }
            }
            if {$start_tran == "d"} {
                if {$sum > $su_ff_sum} {
                    set su_ff_sum $sum
                    set su_ff $setup_list
                    if {$mode == "debug"} {
                        set su_ff_debug $data_path
                        lappend su_ff_debug $ck_path
                    }
                }
            }
        }

        # Hold
        set hold_list [Global_Margin $fig $slope $data_path $ck_path $com_sig hold $slope_unit $cap_unit] 
        set sum [Get_Sum $hold_list]
        if {$ck_tran == "u"} {
            if {$start_tran == "u"} {
                if {$sum < $ho_rr_sum} {
                    set ho_rr_sum $sum
                    set ho_rr $hold_list
                    if {$mode == "debug"} {
                        set ho_rr_debug $data_path
                        lappend ho_rr_debug $ck_path
                    }
                }
            }
            if {$start_tran == "d"} {
                if {$sum < $ho_rf_sum} {
                    set ho_rf_sum $sum
                    set ho_rf $hold_list
                    if {$mode == "debug"} {
                        set ho_rf_debug $data_path
                        lappend ho_rf_debug $ck_path
                    }
                }
            }
        } elseif {$ck_tran == "d"} {
            if {$start_tran == "u"} {
                if {$sum < $ho_fr_sum} {
                    set ho_fr_sum $sum
                    set ho_fr $hold_list
                    if {$mode == "debug"} {
                        set ho_fr_debug $data_path
                        lappend ho_fr_debug $ck_path
                    }
                }
            }
            if {$start_tran == "d"} {
                if {$sum < $ho_ff_sum} {
                    set ho_ff_sum $sum
                    set ho_ff $hold_list
                    if {$mode == "debug"} {
                        set ho_ff_debug $data_path
                        lappend ho_ff_debug $ck_path
                    }
                }
            }
        }
    }
#    PrintTimingGroup $pin $clocks $su_rr $su_rr_debug $su_rf $su_rr_debug "setup_rising" $mode
#    PrintTimingGroup $pin $clocks $ho_rr $ho_rr_debug $ho_rf $ho_rf_debug "hold_rising" $mode
#    PrintTimingGroup $pin $clocks $su_fr $su_fr_debug $su_ff $su_ff_debug "setup_falling" $mode
#    PrintTimingGroup $pin $clocks $ho_fr $ho_fr_debug $ho_ff $ho_ff_debug "hold_falling" $mode

    if {$mode == "debug"} {
        DebugTimingGroup $pin $clocks $su_rr $su_rr_debug $su_rf $su_rr_debug "setup_rising" $mode
        DebugTimingGroup $pin $clocks $ho_rr $ho_rr_debug $ho_rf $ho_rf_debug "hold_rising" $mode
        DebugTimingGroup $pin $clocks $su_fr $su_fr_debug $su_ff $su_ff_debug "setup_falling" $mode
        DebugTimingGroup $pin $clocks $ho_fr $ho_fr_debug $ho_ff $ho_ff_debug "hold_falling" $mode
    }

    return [list $su_rr $su_rf $ho_rr $ho_rf $su_fr $su_ff $ho_fr $ho_ff]  
}

#---------------------------------------------------------------------
#
proc extractPropDelay { slope load slope_unit cap_unit fig pin_in pin_out mode minmax} {

    set prop_rr ""
    set prop_rf ""
    set prop_fr ""
    set prop_ff ""
    
    if {$minmax == "max"} {
        set path_list [ttv_GetPaths $fig $pin_in $pin_out ?? 1000 critic path max]
    } elseif {$minmax == "min"} {
        set path_list [ttv_GetPaths $fig $pin_in $pin_out ?? 1000 critic path min]
    }


    foreach path $path_list {
        set start_sig  [ttv_GetTimingPathProperty $path START_SIG]
        set end_sig    [ttv_GetTimingPathProperty $path END_SIG]
        set start_tran [ttv_GetTimingPathProperty $path START_TRAN]
        set end_tran   [ttv_GetTimingPathProperty $path END_TRAN]

        set prop_list [Global_Prop $fig $slope $load $path $minmax $slope_unit $cap_unit] 
    
        if {$start_tran == "u"} {
            if {$end_tran == "u"} { set prop_rr $prop_list }
            if {$end_tran == "d"} { set prop_rf $prop_list }
        } elseif {$start_tran == "d"} {
            if {$end_tran == "u"} { set prop_fr $prop_list }
            if {$end_tran == "d"} { set prop_ff $prop_list }
        }
    }

    return [list $prop_rr $prop_rf $prop_fr $prop_ff]
}
#---------------------------------------------------------------------
#
proc extractAccess { slope load slope_unit cap_unit fig clocks pin fig mode minmax} {

    set acs_rr ""
    set acs_rf ""
    set acs_fr ""
    set acs_ff ""
    set acs_rr_debug ""
    set acs_rf_debug ""
    set acs_fr_debug ""
    set acs_ff_debug ""
    if {$minmax == "max"} {
        set acs_rr_sum 0
        set acs_rf_sum 0
        set acs_fr_sum 0
        set acs_ff_sum 0
    } elseif {$minmax == "min"} {
        set acs_rr_sum 100000
        set acs_rf_sum 100000
        set acs_fr_sum 100000
        set acs_ff_sum 100000
    }
    
    # Constructs the list of interface latches
	set all_latches [Interface_Latches $fig]

    # Constructs all data_paths from interface latches to output clocked on clocks
    if {$minmax == "max"} {
        set max_data_paths [ttv_GetPaths $fig $all_latches $pin ?? 1000 critic path max]
    } elseif {$minmax == "min"} {
        set max_data_paths [ttv_GetPaths $fig $all_latches $pin ?? 1000 critic path min]
    }

    # Build access
    foreach data_path $max_data_paths {

        # Clock path
        set start_sig  [ttv_GetTimingPathProperty $data_path START_SIG]
        set start_tran [ttv_GetTimingPathProperty $data_path START_TRAN]
        set end_tran   [ttv_GetTimingPathProperty $data_path END_TRAN]
        set com_list   [ttv_GetLatchEventCommands $start_sig $start_tran]

        foreach com $com_list {
            set com_sig  [ttv_GetTimingEventProperty  $com      SIGNAL]
            set com_name [ttv_GetTimingSignalProperty $com_sig  NAME]
            set com_tran [ttv_GetTimingEventProperty  $com      TRANS]

            set ck_path  [ttv_GetPaths $fig $clocks $com_name ?$com_tran 1 critic path max]
            if {[llength $ck_path] == 0} { continue }
            set ck_tran  [ttv_GetTimingPathProperty $ck_path START_TRAN]

            set access_list [Global_Access $fig $slope $load $data_path $ck_path $com_sig $minmax $slope_unit $cap_unit] 
            set sum [Get_Sum $access_list]

            if {$ck_tran == "u"} {
                if {$end_tran == "u"} {
                    if {$minmax == "max"} { set diff [expr $sum - $acs_rr_sum] } 
                    if {$minmax == "min"} { set diff [expr $acs_rr_sum - $sum] } 
                    if {$diff > 0} {
                        set acs_rr_sum $sum
                        set acs_rr $access_list
                        if {$mode == "debug"} {
                            set acs_rr_debug $data_path
                            lappend acs_rr_debug $ck_path
                        }
                    }
                }
                if {$end_tran == "d"} {
                    if {$minmax == "max"} { set diff [expr $sum - $acs_rf_sum] } 
                    if {$minmax == "min"} { set diff [expr $acs_rf_sum - $sum] } 
                    if {$diff > 0} {
                        set acs_rf_sum $sum
                        set acs_rf $access_list
                        if {$mode == "debug"} {
                            set acs_rf_debug $data_path
                            lappend acs_rf_debug $ck_path
                        }
                    }
                }
            } elseif {$ck_tran == "d"} {
                if {$end_tran == "u"} {
                    if {$minmax == "max"} { set diff [expr $sum - $acs_fr_sum] } 
                    if {$minmax == "min"} { set diff [expr $acs_fr_sum - $sum] } 
                    if {$diff > 0} {
                        set acs_fr_sum $sum
                        set acs_fr $access_list
                        if {$mode == "debug"} {
                            set acs_fr_debug $data_path
                            lappend acs_fr_debug $ck_path
                        }
                    }
                }
                if {$end_tran == "d"} {
                    if {$minmax == "max"} { set diff [expr $sum - $acs_ff_sum] } 
                    if {$minmax == "min"} { set diff [expr $acs_ff_sum - $sum] } 
                    if {$diff > 0} {
                        set acs_ff_sum $sum
                        set acs_ff $access_list
                        if {$mode == "debug"} {
                            set acs_ff_debug $data_path
                            lappend acs_ff_debug $ck_path
                        }
                    }
                }
            }
        }
    }

    if {$mode == "debug"} {
        DebugTimingGroup $pin $clocks $acs_rr $acs_rr_debug $acs_rf $acs_rf_debug "rising_edge" $mode
        DebugTimingGroup $pin $clocks $acs_fr $acs_fr_debug $acs_ff $acs_ff_debug "falling_edge" $mode
    }
#    PrintTimingGroup $pin $clocks $acs_rr $acs_rr_debug $acs_rf $acs_rf_debug "rising_edge" $mode
#    PrintTimingGroup $pin $clocks $acs_fr $acs_fr_debug $acs_ff $acs_ff_debug "falling_edge" $mode

    return [list $acs_rr $acs_rf $acs_fr $acs_ff]
}
