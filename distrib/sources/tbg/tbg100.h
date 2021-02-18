#define DONTCARE -1   /* - */
#define WEAK_UNK -2   /* W */
#define FORCE_UNK -3  /* X */

extern void Period (int t, char *unit);
extern void Assign (char *port_name, int value);
extern void AssignBin (char *port_name, char *value);
extern void Check (char *port_name, int value);
extern void Design (char *file_name, char *design_name);
extern void ExecuteCycle ();
extern void GenerateTestbench (char *name);
extern void Import (char *port_name);
extern long ValMax (char *port_name);
