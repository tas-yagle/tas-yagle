proc cell_plus_sa {} {
  set sc [sim_CreateNetlistContext]
  gns_FillBlackBoxes [sim_GetContextNetlist $sc] {}

  sim_SetSimulationTime $sc 8ns
  sim_SetInputSwing $sc 0 1v
  sim_SetOutputSwing $sc 0 1v
  
  #cells
  sim_AddInitVoltage $sc cells.mem00 0
  sim_AddInitVoltage $sc cells.mem01 0

  # prech
  sim_AddSlope $sc prech 100ps 30ps U
  sim_AddSlope $sc eq0 100ps 30ps D
  sim_AddSlope $sc eq1 100ps 30ps D
  
  # wl
  sim_AddSlope $sc wl0 200ps 30ps U
  sim_AddStuckVoltage $sc wl1 0

  # wlen
  sim_AddStuckVoltage $sc wlen1 1.0
  sim_AddStuckVoltage $sc wlen0 0
  sim_AddStuckVoltage $sc blen 1.0

  # sens
  sim_AddSlope $sc saen 700ps 30ps U
  sim_AddSlope $sc saenb 700ps 30ps D

  # --- reading ----------------------

  # measures
  sim_AddSpiceMeasureDelay $sc "delay" saen bl UD SIM_MAX

  sim_RunSimulation $sc NULL
  set delay [sim_GetSpiceMeasureDelay $sc delay]
  puts "fast read = $delay"
  begSetDelay "fastread_delay" [expr int($delay*1e12)]
  
  sim_ResetMeasures $sc

  # wl
  sim_AddSlope $sc wl1 200ps 30ps U
  sim_AddStuckVoltage $sc wl0 0

  # measures
  sim_AddSpiceMeasureDelay $sc "delay" saen blb UD SIM_MAX

  sim_RunSimulation $sc NULL
  set delay [sim_GetSpiceMeasureDelay $sc delay]
  puts "slow read = $delay"
  begSetDelay "slowread_delay" [expr int($delay*1e12)]
  
  # --- writing ----------------------
  sim_ResetMeasures $sc

  sim_AddInitVoltage $sc cells.mem00 1v
  sim_AddInitVoltage $sc cells.mem01 1v

  sim_AddStuckVoltage $sc wl1 0v
  sim_AddStuckVoltage $sc wl0 1v
  
  sim_AddSlope $sc bl 400ps 30ps D
  sim_AddSlope $sc blb 400ps 30ps U
  sim_AddSlope $sc blen 400ps 30ps U

  sim_AddSpiceMeasureDelay $sc "delay" bl cells.mem00 DD SIM_MAX

  sim_RunSimulation $sc NULL
  set delay [sim_GetSpiceMeasureDelay $sc delay]
  puts "fast write = $delay"
  begSetDelay "fastwrite_delay" [expr int($delay*1e12)]

  sim_ResetMeasures $sc
  
  sim_AddStuckVoltage $sc wl1 1v
  sim_AddStuckVoltage $sc wl0 0v
  
  sim_AddSlope $sc bl 400ps 30ps U
  sim_AddSlope $sc blb 400ps 30ps D
  
  sim_AddSpiceMeasureDelay $sc "delay" blb cells.mem01 DD SIM_MAX
  
  sim_RunSimulation $sc NULL
  set delay [sim_GetSpiceMeasureDelay $sc delay]
  puts "slow write = $delay"
  begSetDelay "slowwrite_delay" [expr int($delay*1e12)]

  sim_FreeContext $sc
}
