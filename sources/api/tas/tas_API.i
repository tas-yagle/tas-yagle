extern TimingFigure *tas (char *figname);
extern TimingFigure *hitas_sub (char *figname, char cnsannotatelofig, char loadcnsfig);
extern void tas_command_line (char *args);
//Netlist *tas_extract_SPICE_path (TimingDetail *critic);
//SimulationContext *tas_extract_SPICE_patterns (TimingDetail *critic, Netlist *figext);
int hitas_pvt_count();

