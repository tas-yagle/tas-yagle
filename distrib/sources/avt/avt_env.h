#include "avt_headers.h"

extern void     avt_init_v                  ();
extern void     avt_sethashvar              (char *var, char *val);
extern char     *avt_gethashvar             (char *var);
extern void     avt_parsavtfile             ();
extern void     avtenv                      ();
extern int      avt_is_default_technoname   (char *name);
extern void     avt_LogConfig               ();
extern void     avt_man                     (int all);
