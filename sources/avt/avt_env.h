#include "avt_headers.h"

extern void        avt_init_v                  ();
extern void        avt_sethashvar              (const char *var, const char *val);
extern const char *avt_gethashvar              (const char *var);
extern void        avt_parsavtfile             ();
extern void        avtenv                      ();
extern int         avt_is_default_technoname   (const char *name);
extern void        avt_LogConfig               ();
extern void        avt_man                     (int all);
