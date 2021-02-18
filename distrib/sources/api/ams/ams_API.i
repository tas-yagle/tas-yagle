void ams_DriveCommandFile     (Netlist *origlf, Netlist *lf, char *filename);
void ams_SetPowerSupply       (Netlist *lf, char *name, double vss, double vdd);
void ams_DriveWaveCommandFile (Netlist *lf, char *TOP, char *name);
void ams_DumpADvanceMSDeck    (char *tb);
int  ams_SetContext           (char *outname, char *spiname, char *figname);
void ams_LoadMainFile         (char *filename, char *format);
