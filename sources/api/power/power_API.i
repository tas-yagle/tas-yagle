
/* SWITCHING POWER                                           */
void             SwitchingPower         (TimingFigure *tf);
void             vcd_parsefile          (char *name, char *modulename);
void             cdl_parse              (char *name);
void             DisplaySwitchingPower  (void);
void             DisplaySwitchingPowerSignal  (TimingFigure *tf, char *signame);
float            GetSwitchingPowerSignal_sub  (TimingFigure *tf, char *signame, TimeValue interval_d, TimeValue begindate_d, TimeValue enddate_d, char *plot);

