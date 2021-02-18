
#include "apitype.h"

extern "C" 
{
#include AVT_H
#include MUT_H
#include API_H
extern int yyparse ();
}

extern FILE *f_header, *f_init, *f_wrappers, *yyin;
extern char *XMLPATH;
extern char *MANPATH;
extern char *ONLINE_FILE;
extern char *ONLINE_PATH;
extern char *DBPATH;
extern int xmllevel;
extern char *FILTER, *FILETERFILE;
extern chain_list *proto_list;
extern char *wrapperfile;
extern chain_list *alldefines;

#define ParmList chain_list
#define Parm chain_list
#define ManList chain_list
#define Man chain_list
#define ApiType chain_list
#define String char
#define Printf fprintf

class Language 
{
 public:
  Language();
  virtual ~Language();
  //virtual functions required by the SWIG parser

  virtual void parse_args (int, char *argv[]);
  virtual void parse ();
  virtual void create_function (char *, char *, ApiType *, ParmList *);
  virtual void create_man (char *, ApiType *, ParmList *, functionman *);
  virtual void create_xml (char *, char *, ApiType *, ParmList *, functionman *, int, int);
  virtual void create_db (char *, char *, ApiType *, ParmList *, functionman *, int, int);
  virtual void create_filter (char *filter, char *filterfile, Man *);
  virtual void link_variable (char *, char *, ApiType *);
  virtual void declare_const (char *, char *, ApiType *, char*);
  virtual void initialize (void);
  virtual void headers (void);
  virtual void close (void);
  virtual void set_module (char *);
  virtual void create_command (char *, char *);
  virtual void create_man_man (chain_list *);
  virtual void create_filter_online (char *filter, char *online_path, Man *mans) ;
  virtual void create_online (char *, ApiType *, ParmList *, functionman *);
};

char *ApiType_str (ApiType *d, int i);
char *ApiType_name (ApiType *d, int i);
Parm *Getnext (Parm *p);
ApiType *Gettype (Parm *p);
int ParmList_len (ParmList *l);
int ApiType_type (ApiType *p);
int ApiType_ispointer (ApiType *p);
char *ApiType_base (ApiType *p);
char *Char(char *c);
void Api_banner(FILE *f);
void *Getdata(Parm *p);
void Avtwig_init_args (int argc, char **argv);
int AVTWIG_main (int argc, char **argv, Language *l);
