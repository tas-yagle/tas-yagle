########################################################################
#                                                                      #
# Set of procedures to generate a wrapup report of a list of slacks    #
#                                                                      #
# Slacks are grouped together if the names of start, end, and thru     #
# nodes are similar. Node names are similar if the only differences    #
# are indexes suffixed at the end of name segments.                    #
#                                                                      #
########################################################################

########################################################################
#                                                                      #
# For a given slack create a string which is a colon separated list of #
# the start, thru and end names                                        #
#                                                                      #
########################################################################

proc get_slack_name {slack} {
    set event [stb_GetSlackProperty $slack START_EVENT]
    set signal [ttv_GetTimingEventProperty $event SIGNAL]
    set start_name [ttv_GetTimingSignalProperty $signal NET_NAME]

    set event [stb_GetSlackProperty $slack END_EVENT]
    set signal [ttv_GetTimingEventProperty $event SIGNAL]
    set end_name [ttv_GetTimingSignalProperty $signal NET_NAME]
    
    set event [stb_GetSlackProperty $slack THRU_EVENT]
    if {$event != "NULL"} {
        set signal [ttv_GetTimingEventProperty $event SIGNAL]
        set thru_name [ttv_GetTimingSignalProperty $signal NET_NAME]
    } else {
        set thru_name ""
    }
    
    return "$start_name:$thru_name:$end_name"
}

proc get_path_name {path} {
    set event [ttv_GetTimingPathProperty $path START_EVENT]
    set signal [ttv_GetTimingEventProperty $event SIGNAL]
    set start_name [ttv_GetTimingSignalProperty $signal NET_NAME]

    set event [ttv_GetTimingPathProperty $path END_EVENT]
    set signal [ttv_GetTimingEventProperty $event SIGNAL]
    set end_name [ttv_GetTimingSignalProperty $signal NET_NAME]
    
    set event [ttv_GetTimingPathProperty $path ACCESS_LATCH]
    if {$event != "NULL"} {
        set signal [ttv_GetTimingEventProperty $event SIGNAL]
        set thru_name [ttv_GetTimingSignalProperty $signal NET_NAME]
    } else {
        set thru_name ""
    }
    
    return "$start_name:$thru_name:$end_name"
}

########################################################################
#                                                                      #
# Create bus notation form from an unsorted set of bus indices         #
#                                                                      #
########################################################################

proc get_bus {bus_indices} {
    set bus_indices [lsort -integer $bus_indices]
    set bus [lindex $bus_indices 0]
    set last_index $bus
    set last_bus $bus
    foreach index $bus_indices {
        if {$index == $last_index} continue
        if {$index != [expr $last_index+1]} {
            if {$last_index == $last_bus} {
                set bus "$bus,$index"
            } else {
                set bus "$bus-$last_index,$index"
            }
            set last_bus $index
        }
        set last_index $index
    }
    if {$last_index != $last_bus} {
        set bus "$bus-$last_index"
    }
    return "$bus"
}

########################################################################
#                                                                      #
# Function which does most of the work                                 #
#                                                                      #
# Input: A list of slacks                                              #
# Output: An array of typical slacks                                   #
#         Each element of this array is a 2-element list where the     #
#         first element is a string made up of colon separated start,  #
#         end, and thru names in bus notation and the second element   #
#         is an example of this slack                                  #
#                                                                      #
########################################################################

proc get_typical_slacks {slacks typical_slacks get_name_func} {
   upvar $typical_slacks typical_slack_array
   set base_name_count 0
   set slackcount 0

    foreach slack $slacks {
        set slack_name [$get_name_func $slack]
        set index_list [regexp -all -inline {[0-9]+[:\/\.]|[0-9]+$} $slack_name]
        if {[llength index_list] > 0} {
            regsub -all {[0-9]+([:\/\.])|[0-9]+$} $slack_name {\[%s\]\1} base_name
            if {$base_name_count == 0} {
                set base_names(1) $base_name
                set indices(1) [list $index_list]
                set example_slacks(1) $slack
                set group_count(1) 1
                set base_name_count 1
                puts "$slackcount : $base_name_count : $slack_name"
            } else {
                set match 0
                foreach i [array names base_names] {
                    if {$base_names($i) == $base_name} {
                        lappend indices($i) $index_list
                        incr group_count($i) 1
                        set match 1
                        break
                    }
                }
                if {$match == 0} {
                    incr base_name_count 1
                    set base_names($base_name_count) $base_name
                    set indices($base_name_count) [list $index_list]
                    set example_slacks($base_name_count) $slack
                    set group_count($base_name_count) 1
                    puts "$slackcount : $base_name_count : $slack_name"
                }
            }
        }
        incr slackcount 1
    }
    
    foreach i [array names base_names] {
        set count [llength [lindex $indices($i) 0]]
        set bus_list ""
        for {set j 0} {$j < $count} {incr j 1} {
            set bus ""
            foreach index_list $indices($i) {
                lappend bus [string trimright [lindex $index_list $j] :./]
            }
            set bus [get_bus $bus]
            set bus_list "$bus_list $bus"
        }
        set bussed_name [eval format $base_names($i) $bus_list]
        set typical_slack_array($i) [list $bussed_name $example_slacks($i) $group_count($i)]
    }
}

########################################################################
#                                                                      #
# Display a given bus grouping where start, end and thru nodes are     #
# colon separated                                                      #
#                                                                      #
########################################################################

proc display_bussed_slack_name {file slack_name index count total labelc } {
    regsub -all : $slack_name " " slack_name_list
    fputs "Bus Grouped $labelc ($index) : $count objects (out of $total)\n\n" $file
    fputs "    From: [lindex $slack_name_list 0]\n" $file
    if {[llength $slack_name_list] == 3} {
        fputs "      To: [lindex $slack_name_list 2]\n" $file
        fputs "    Thru: [lindex $slack_name_list 1]\n\n" $file
    } else {
        fputs "      To: [lindex $slack_name_list 1]\n\n" $file
    }
}

########################################################################
#                                                                      #
# Display the details of a given slack                                 #
#                                                                      #
########################################################################

proc display_slack_detail {file slack index lagdebug} {
    set value [format "%.1fps" [expr 1e12*[stb_GetSlackProperty $slack VALUE]]]
    fputs "Example of Bus Grouped Slack ($index) : Slack of $value\n\n" $file
    if {$lagdebug == 1} {
        set path [stb_GetSlackProperty $slack DATA_VALID_PATH]
        if {[ttv_GetTimingPathProperty $path DATA_LAG] > 0} {
            stb_FindLagPaths $file $slack -closingpath
        } else {
            stb_DisplaySlackReport $file -slacks $slack
        }
    } else {
        stb_DisplaySlackReport $file -slacks $slack
    }
}

proc display_path_detail {file path index lagdebug} {
    set value [format "%.1fps" [expr 1e12*[ttv_GetTimingPathProperty $path DELAY]]]
    fputs "Example of Bus Grouped Path ($index) : Delay of $value\n\n" $file
    ttv_DisplayPathDetail $file $index $path
}

########################################################################
#                                                                      #
# The top-level function for the wrap-up report                        #
#                                                                      #
########################################################################

proc write_wrapup_report_any {file slacks get_name_func display_detail_func labelc lagdebug} {
    get_typical_slacks $slacks typical_slacks $get_name_func
    
    set total_slacks [llength $slacks]
    foreach i [lsort -integer [array names typical_slacks]] {
        display_bussed_slack_name $file [lindex $typical_slacks($i) 0] $i [lindex $typical_slacks($i) 2] $total_slacks $labelc
        $display_detail_func $file [lindex $typical_slacks($i) 1] $i $lagdebug
    }
}

proc write_wrapup_report {file slacks args} {
    global tcl_interactive
    set i 0
    set lagdebug 0
  
    set lst $args
    while {[lindex $lst $i]!=""} {
        set opt [lindex $lst $i]
        incr i
        if {$opt=="-lagdebug"} {
            set lagdebug 1
        } else {
            set prei [expr $i-1]
            puts "Unknown option '[lindex $lst $prei]'"
            puts "Usage: write_wrapup_report <file> <slacklist> \[-lagdebug]"
            if {!$tcl_interactive} {
                exit
            } else {
                return [_NULL_]
            }
        } 
    }
    if {[string first "TimingPath" [lindex $slacks 0]]!=-1} {
        write_wrapup_report_any $file $slacks get_path_name display_path_detail Path $lagdebug
    } else {
        write_wrapup_report_any $file $slacks get_slack_name display_slack_detail Slack $lagdebug
    }
}



#----------------------------------------------------------------------
