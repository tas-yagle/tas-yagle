#ifdef USE_SWIG
#include "swig11.h"
#define WIGType SwigType
#define WIGType_str SwigType_str
#define WIGType_base SwigType_base
#define WIGType_type SwigType_type
#define WIGType_ispointer SwigType_ispointer
#define WIG_banner Swig_banner
#define WIG_main SWIG_main
#define WIG_init_args Swig_init_args
#endif

#ifdef USE_AVTWIG
#include "Language.h"
#define WIGType ApiType
#define WIGType_str ApiType_str
#define WIGType_name ApiType_name
#define WIGType_base ApiType_base
#define WIGType_type ApiType_type
#define WIGType_ispointer ApiType_ispointer
#define WIG_banner Api_banner
#define WIG_main AVTWIG_main
#define WIG_init_args Avtwig_init_args
#endif

class GENIUS: public Language {
private:
    char *module;
public:
    GENIUS () { module = 0; };
    void parse_args (int, char *argv[]);
    void parse ();
    void create_defines_function();
    void create_function (char *, char *, WIGType *, ParmList *);
    void create_function_getargs (char *name, char *iname, WIGType *d, ParmList *l);

#ifdef USE_AVTWIG
    void create_man (const char *, WIGType *, ParmList *, functionman*);
    void create_online (char *, WIGType *, ParmList *, functionman*);
    void create_xml (const char *, const char *, WIGType *, ParmList *, functionman*, int first, int mode);
    void create_man_man (chain_list *);
    void create_online (chain_list *, chain_list *);
    void create_db (const char *filename, const char *name, WIGType *d, ParmList *l, functionman *man, int first, int mode);
    void create_filter (char *filter, char *filterfile, Man *);
    void create_filter_online (char *filter, char *online_path, Man *);
    void create_filter4xls (char *filter, char *filterfile, Man * mans);

#endif
    void link_variable (char *, char *, WIGType *);
    void declare_const (char *, char *, WIGType *, char*);
    void initialize (void);
    void headers (void);
    void close (void);
    void set_module (const char *);
    void create_command (char *, char *);
};
