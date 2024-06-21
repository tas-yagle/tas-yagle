proc sa_plus_mux {} {
  global writesig
  begCreateInterface

  begAddMemDriver "bl" "ext@$writesig and (sel0 or not sel0b)" "in0" 0 NULL
  begAddMemDriver "blb" "ext@$writesig and (sel0 or not sel0b)" "in0b" 0 NULL

  begAddMemDriver "bl" "ext@$writesig and (sel1 or not sel1b)" "in1" 0 NULL
  begAddMemDriver "blb" "ext@$writesig and (sel1 or not sel1b)" "in1b" 0 NULL

  begKeepModel
  begSaveModel
}
