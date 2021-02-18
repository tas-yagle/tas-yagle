create_clock -period 2800 -waveform { 0 1400 } CK

set_input_delay -min 0 -clock CK {EN* D*}
set_input_delay -max 0 -clock CK {EN* D*}
set_input_delay -min 0 -clock CK -clock_fall [all_inputs]
set_input_delay -max 0 -clock CK -clock_fall [all_inputs]
