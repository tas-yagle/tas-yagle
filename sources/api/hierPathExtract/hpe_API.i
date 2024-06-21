Netlist *hpe_ExtractNetlist       (Netlist *lf, char *name);
void     hpe_AddRC                (Netlist *origlofig, Netlist *fig);
Netlist *hpe_getSubNetlist        (char *topname, List *argv);
Netlist *hpe_CreateSubNetlist     (char *subnetlist, char *topname, List *argv);
void     hpe_SetInstanceVariation (char *cellname, char *parameter, char *value);
