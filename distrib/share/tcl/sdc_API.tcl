source [file join [file dirname [info script]] sdcparsercore.tcl]

proc sdc_API_callback {command parse_data} {
    upvar $parse_data data
    
    switch -- $command {
        
        create_clock -
        create_generated_clock -
        set_clock_latency -
        set_input_delay -
        set_load -
        set_input_transition -
        set_output_delay -
        set_multicycle_path -
        set_false_path -
        set_disable_timing -
        set_clock_uncertainty -
        set_clock_groups -
        set_min_delay -
        set_max_delay -
        set_case_analysis {
            infsdc_$command [array get data]
            return ""
        }

        get_nets -
        get_clocks -
        get_ports -
        get_pins {
            return $data(patterns)
        }

        all_clocks -
        all_inputs -
        all_outputs {
            return "default"
        }
        
        default {
            infsdc_not_supported $command
        }    
    }
}

proc all_clocks {args} {
    sdc::parse_command $sdc::sdc_version all_clocks $args
}

proc all_inputs {args} {
    sdc::parse_command $sdc::sdc_version all_inputs $args
}

proc all_outputs {args} {
    sdc::parse_command $sdc::sdc_version all_outputs $args
}

proc create_clock {args} {
    sdc::parse_command $sdc::sdc_version create_clock $args
}

proc current_design {args} {
    sdc::parse_command $sdc::sdc_version current_design $args
}

proc current_instance {args} {
    sdc::parse_command $sdc::sdc_version current_instance $args
}

proc get_cells {args} {
    sdc::parse_command $sdc::sdc_version get_cells $args
}

proc get_clocks {args} {
    sdc::parse_command $sdc::sdc_version get_clocks $args
}

proc get_lib_cells {args} {
    sdc::parse_command $sdc::sdc_version get_lib_cells $args
}


proc get_lib_pins {args} {
    sdc::parse_command $sdc::sdc_version get_lib_pins $args
}

proc get_libs {args} {
    sdc::parse_command $sdc::sdc_version get_libs $args
}

proc get_nets {args} {
    sdc::parse_command $sdc::sdc_version get_nets $args
}

proc get_pins {args} {
    sdc::parse_command $sdc::sdc_version get_pins $args
}

proc get_ports {args} {
    sdc::parse_command $sdc::sdc_version get_ports $args
}

proc set_case_analysis {args} {
    sdc::parse_command $sdc::sdc_version set_case_analysis $args
}

proc set_clock_latency {args} {
    sdc::parse_command $sdc::sdc_version set_clock_latency $args
}

proc set_clock_transition {args} {
    sdc::parse_command $sdc::sdc_version set_clock_transition $args
}

proc set_clock_uncertainty {args} {
    sdc::parse_command $sdc::sdc_version set_clock_uncertainty $args
}

proc set_clock_groups {args} {
    sdc::parse_command $sdc::sdc_version set_clock_groups $args
}

proc set_disable_timing {args} {
    sdc::parse_command $sdc::sdc_version set_disable_timing $args
}

proc set_drive {args} {
    sdc::parse_command $sdc::sdc_version set_drive $args
}

proc set_driving_cell {args} {
    sdc::parse_command $sdc::sdc_version set_driving_cell $args
}

proc set_false_path {args} {
    sdc::parse_command $sdc::sdc_version set_false_path $args
}

proc set_fanout_load {args} {
    sdc::parse_command $sdc::sdc_version set_fanout_load $args
}

proc set_input_delay {args} {
    sdc::parse_command $sdc::sdc_version set_input_delay $args
}

proc set_input_transition {args} {
    sdc::parse_command $sdc::sdc_version set_input_transition $args
}

proc set_load {args} {
    sdc::parse_command $sdc::sdc_version set_load $args
}

proc set_logic_dc {args} {
    sdc::parse_command $sdc::sdc_version set_logic_dc $args
}

proc set_logic_one {args} {
    sdc::parse_command $sdc::sdc_version set_logic_one $args
}

proc set_logic_zero {args} {
    sdc::parse_command $sdc::sdc_version set_logic_zero $args
}

proc set_max_area {args} {
    sdc::parse_command $sdc::sdc_version set_max_area $args
}

proc set_max_capacitance {args} {
    sdc::parse_command $sdc::sdc_version set_max_capacitance $args
}

proc set_max_delay {args} {
    sdc::parse_command $sdc::sdc_version set_max_delay $args
}

proc set_max_fanout {args} {
    sdc::parse_command $sdc::sdc_version set_max_fanout $args
}

proc set_max_time_borrow {args} {
    sdc::parse_command $sdc::sdc_version set_max_time_borrow $args
}

proc set_max_transition {args} {
    sdc::parse_command $sdc::sdc_version set_max_transition $args
}

proc set_min_capacitance {args} {
    sdc::parse_command $sdc::sdc_version set_min_capacitance $args
}

proc set_min_delay {args} {
    sdc::parse_command $sdc::sdc_version set_min_delay $args
}


proc set_multicycle_path {args} {
    sdc::parse_command $sdc::sdc_version set_multicycle_path $args
}

proc set_operating_conditions {args} {
    sdc::parse_command $sdc::sdc_version set_operating_conditions $args
}

proc set_output_delay {args} {
    sdc::parse_command $sdc::sdc_version set_output_delay $args
}

proc set_port_fanout_number {args} {
    sdc::parse_command $sdc::sdc_version set_port_fanout_number $args
}

proc set_propagated_clock {args} {
    sdc::parse_command $sdc::sdc_version set_propagated_clock $args
}

proc set_resistance {args} {
    sdc::parse_command $sdc::sdc_version set_resistance $args
}


proc set_wire_load_min_block_size {args} {
    sdc::parse_command $sdc::sdc_version set_wire_load_min_block_size $args
}

proc set_wire_load_mode {args} {
    sdc::parse_command $sdc::sdc_version set_wire_load_mode $args
}

proc set_wire_load_model {args} {
    sdc::parse_command $sdc::sdc_version set_wire_load_model $args
}

proc set_wire_load_selection_model {args} {
    sdc::parse_command $sdc::sdc_version set_wire_load_selection_model $args
}

proc set_hierarchy_separator {args} {
    sdc::parse_command $sdc::sdc_version set_hierarchy_separator $args
}

proc create_generated_clock {args} {
    sdc::parse_command $sdc::sdc_version create_generated_clock $args
}

proc set_clock_gating_check {args} {
    sdc::parse_command $sdc::sdc_version set_clock_gating_check $args
}

proc set_data_check {args} {
    sdc::parse_command $sdc::sdc_version set_data_check $args
}

proc set_max_dynamic_power {args} {
    sdc::parse_command $sdc::sdc_version set_max_dynamic_power $args
}

proc set_leakage_power {args} {
    sdc::parse_command $sdc::sdc_version set_leakage_power $args
}

proc set_min_porosity {args} {
    sdc::parse_command $sdc::sdc_version set_min_porosity $args
}

#main
sdc::register_callback sdc_API_callback
