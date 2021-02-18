#include "avt_API_types.h"

void ams_DriveCommandFile     (Netlist *origlf, Netlist *lf, char *filename);
void ams_SetPowerSupply       (Netlist *lf, char *name, double vss, double vdd);
void ams_DriveWaveCommandFile (Netlist *lf, char *TOP, char *name);
void ams_LoadMainFile         (char *filename, char *format);
void ams_DumpADvanceMSDeck    (char *testbench);
int  ams_SetContext           (char *outname, char *spiname, char *figname);

/*
  MAN ams_LoadMainFile
  DESCRIPTION
  loads the file <filename> assuming the file format is <format>. Available formats are: spice, tlf4, tlf3, lib, verilog, vhdl, spf, dspf, inf.
  It also sets the <filename> to be included for ADvance-MS deck generation.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
/*
  MAN ams_DumpADvanceMSDeck
  DESCRIPTION
  dumps the ADvance-MS deck. It needs the name of <testbench> to generate the compilation script.
  It includes : 
  ams_blackbox_file.v : contains blackbox definition.
  ams_command_file.cmd : command file for simulation.
  ams_compile : compilation script.
  RETURN VALUE
  none.
  ERRORS
  none.
*/
