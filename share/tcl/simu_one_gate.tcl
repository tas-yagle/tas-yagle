proc simu_one_gate { fig path tolerence slope forward_cone prefix } { 
    # tolerence: selects the accepted percentage range (ex: 5). Any deviation above or below is resimulated individually 
    # slope: selects the slope applied on the spicedeck input pin ("tas" or "sim")
    # forward_cone: defines whether a cone is simulated individually or with its n+1 cone ("yes" or "no")
    # prefix: for output file names
    
    ttv_DisplayPathListDetail [fopen $prefix.path.sim w] $path
    exec mv cmd_$prefix\_ext.spi cmd_$prefix\_ext.spi.path
    exec mv $prefix\_ext.spi $prefix\_ext.spi.path

    set detail_list [ttv_GetPathDetail $path]

    set dt_length [llength $detail_list]
    set probe_index 1
    while { $probe_index < $dt_length } {
        set detail [lindex $detail_list $probe_index]
        set detail_m_1 [lindex $detail_list [expr $probe_index - 1]]
        set detail_p_1 [lindex $detail_list [expr $probe_index + 1]]
        set node [ttv_GetTimingDetailProperty $detail NODE_NAME]
        set tran [ttv_GetTimingDetailProperty $detail TRANS]
        set type [ttv_GetTimingDetailProperty $detail TYPE]
        set node_m_1  [ttv_GetTimingDetailProperty $detail_m_1 NODE_NAME]
        set tran_m_1  [ttv_GetTimingDetailProperty $detail_m_1 TRANS]
        set node_p_1 ""
        set tran_p_1 ""
        if { $detail_p_1 != "" } { set node_p_1  [ttv_GetTimingDetailProperty $detail_p_1 NODE_NAME] }
        if { $detail_p_1 != "" } { set tran_p_1  [ttv_GetTimingDetailProperty $detail_p_1 TRANS] }
        set ref_delay [ttv_GetTimingDetailProperty $detail REF_DELAY]
        set sim_delay [ttv_GetTimingDetailProperty $detail SIM_DELAY]
        set ref_slope [ttv_GetTimingDetailProperty $detail REF_SLOPE]
        set sim_slope [ttv_GetTimingDetailProperty $detail SIM_SLOPE]
        set ref_slope_m_1 [ttv_GetTimingDetailProperty $detail_m_1 REF_SLOPE]
        set sim_slope_m_1 [ttv_GetTimingDetailProperty $detail_m_1 SIM_SLOPE]
        set error [expr (($ref_delay - $sim_delay) / $sim_delay) * 100]

        if { ($error > $tolerence) || ($error < -$tolerence) } {
            puts " "
            puts "############### DEVIATION CHECK $probe_index ##########################"
            puts " "
            puts "$node_m_1 ($tran_m_1) -> $node ($tran) : ref=$ref_delay sim=$sim_delay error=$error %"
            puts "type: $type"
            puts " "
            puts "PROBING..."

            # ----------- Final node
            if { $forward_cone == "yes" } {
                if { $node_p_1 != "" } {
                    set final_node $node_p_1
                    set final_tran $tran_p_1
                } else {
                    puts "Warning: end of chain, simulation with n+1 cone impossible"
                    set final_node $node
                    set final_tran $tran
                }
            } else {
                set final_node $node
                set final_tran $tran
            }

            # ----------- input slope
            if { $slope == "sim" } {
                set input_slope $sim_slope_m_1
            } else {
                set input_slope $ref_slope_m_1
            }
            
            # ----------- Probe
            if { $type != "rc" } {
                set probe [ttv_ProbeDelay $fig $input_slope [list $node_m_1 $final_node] $tran_m_1$final_tran 1 path max] 
                if { probe != "" } {
                    ttv_DisplayPathListDetail [fopen $prefix.probe\_$probe_index.sim w] $probe
                    exec mv cmd_$prefix\_ext.spi cmd_$prefix\_ext.spi.probe_$probe_index
                    exec mv $prefix\_ext.spi $prefix\_ext.spi.probe_$probe_index
                } else {
                    puts "Error: Probe failed..."
                }
            } else {
                puts "Warning: RC simulation ot implemented, skipping..."
            }
        }
        incr probe_index
    }
}
