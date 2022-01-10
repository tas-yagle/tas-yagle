#include <string.h>
#include <stdlib.h>
#include "Language.h"

extern "C" {
#include AVT_H
}

#define MAX_API 2048

FILE *f_header, *f_init, *f_wrappers;
char *MANPATH = NULL;
char *ONLINE_FILE = NULL;
char *ONLINE_PATH = NULL;
char *XMLPATH = NULL;
char *DBPATH = NULL;
int xmllevel=2;
char *FILTER=NULL, *FILTERFILE;
char *filename;
extern chain_list *includes;
extern chain_list *mans;
extern "C" void setmans();

static int nowrapper;

void Avtwig_init_args (int argc, char **argv)
{
    char buf[1024], buf2[1024];
    char *dot;
    int s=1, err=0, nobanner=0;
    
    filename=NULL;

    nowrapper = 0;

    for (s=1; s<argc; s++)
    {
      if (!strcmp (argv[s], "-silent")) {
        nobanner=1;
      } 
      else if (!strcmp (argv[s], "-p") && s+1<argc) {
        s++;
        MANPATH = argv[s];
      }
      else if (!strcmp (argv[s], "-online") && s+1<argc) {
        s++;
        FILTER = strdup(argv[s]);
        s++;
        ONLINE_PATH = strdup (argv[s]);
      }
      else if (!strcmp (argv[s], "-xmllevel") && s+1<argc) {
        s++;
        xmllevel = atoi(argv[s]);
      }
      else if (!strcmp (argv[s], "-filter") && s+2<argc) {
        s++;
        FILTER = strdup(argv[s]);
        s++;
        FILTERFILE = strdup(argv[s]);
        nowrapper = 1;
      }
       else if (!strcmp (argv[s], "-db") && s+1<argc) {
        s++;
        DBPATH = argv[s];
      }
      else if (!strncmp (argv[s], "-x", 2) && s+1<argc) {
          if (!strcmp(argv[s], "-xonly")) nowrapper = 1;
          s++;
          XMLPATH = argv[s]; 
      }
      else  {
          if (filename!=NULL) err=1; 
          filename = argv[s];
      }
    }
    
    if (!nobanner) avt_banner ("Wrap", "API wrapper", "2004");
    
    if (filename == NULL || err)
    {
      fprintf (stderr, "Usage: %s [-p <manptah>] [-x[only] <xmlpath>] [-db <dbpath>] [-xmllevel <#startlevel>] <filename>\n", argv[0]);
      EXIT (1);
    }

    avtenv();
    mbkenv();


#ifdef AVERTEC_LICENSE
    if(!avt_givetoken("AVT_LICENSE_SERVER", "genapi")) EXIT(1);
#endif

    yyin = fopen (filename, "rt");
    if (yyin==NULL)
    {
      fprintf (stderr, "Could not open file '%s'\n", filename);
      EXIT (1);
    }
    strcpy(buf2, filename);
    if ((dot = strrchr (buf2, '.'))) *dot = '\0';
    snprintf (buf, 1024, "%s_wrap.c", buf2);
    wrapperfile = strdup (filename);
    if (!nowrapper) f_wrappers = f_init = f_header = fopen (buf, "w+");
}

static int sortcmp(const void *a, const void *b)
{
  Parm *function;
  char *aname, *bname;
  function = (Parm*)Getdata (*(ParmList **)a);
  aname = (char*)Getdata (function);
  function = (Parm*)Getdata (*(ParmList **)b);
  bname = (char*)Getdata (function);
  if (strcmp(aname, bname)<0) return -1;
  else if (strcmp(aname, bname)>0) return 1;
  return 0;
}

static ParmList *SortProto(ParmList *proto)
{
  ParmList *tab[MAX_API], *pt;
  int i, j;
  for (i=0, pt = proto_list; pt; pt = Getnext (pt), i++) 
    tab[i]=pt;
  
  if (i<=1) return proto;

  qsort(tab, i, sizeof(ParmList *), sortcmp);

  for (j=0; j<i-1; j++)
    tab[j]->NEXT=tab[j+1];
  tab[j]->NEXT=NULL;

  return tab[0];
}


int AVTWIG_main (int argc, char **argv, Language *l)
{
    ParmList *pt;
    ManList *man;
    char *fname;
    Parm *function;
    ApiType *return_type;
    int first;
    int count=0;
    char *uppersection;
    functionman *fman;
    
    l->parse_args (argc, argv);
    if (!nowrapper) l->headers ();
    l->parse ();

    if (!XMLPATH && !FILTER)
      proto_list=SortProto(proto_list);
    setmans();

    if (!nowrapper) fprintf(f_header,"#define AVTWIG_AVOID_CONFLICT\n");
    if (!nowrapper) fprintf(f_header,"// Original include file\n");
    if (!nowrapper) fprintf(f_header,"#include \"%s\"\n\n",filename);
    if (!nowrapper) fprintf(f_header,"// Others\n");

    man = mans;
    first = 1;

    if (XMLPATH || FILTER)
      {
        proto_list = (ParmList *)reverse((chain_list *)proto_list);
        man = (ManList *)reverse((chain_list *)man);
      }
    
    if (ONLINE_PATH)
	{
      l->create_filter_online(FILTER, ONLINE_PATH, man);
	  return 0;
	  
	}
    if (FILTER)
      {  
        l->create_filter(FILTER, FILTERFILE, man);
        return 0;
      }
    for (pt = proto_list; pt; pt = Getnext (pt)) {
        function = (Parm*)Getdata (pt);
        fname = (char*)Getdata (function);
        function = Getnext (function);
        return_type = (ApiType*)function;
        fman=(functionman *)man->DATA;
        if (!nowrapper) l->create_function (fname, fname, return_type, Getnext (function));
        if (strstr(fname,"_Action_")==NULL)
          {
            l->create_man (fname, return_type, Getnext (function), fman);
            // FOR XML uncomment next line
            if (fman!=NULL)
              {
                uppersection=fman->uppersection;
                if (strcmp(uppersection,"")!=0)
                  {
                    
                    if (count==0)
                      {
                        l->create_xml(filename, uppersection, return_type, Getnext (function), NULL, first, 1); // ouverture de sous section
                        l->create_db(filename, uppersection, return_type, Getnext (function), NULL, first, 1); // ouverture de sous section
                        count++;
                      }
                    else
                      {
                        l->create_xml(filename, uppersection, return_type, Getnext (function), NULL, first, 3); // fermeture+ouverture de nouvelle sous section
                        l->create_db(filename, uppersection, return_type, Getnext (function), NULL, first, 3); // fermeture+ouverture de nouvelle sous section
                      }
                    first = 0;
                  }
              }
            if (count==0)
            {
              l->create_xml (filename, fname, return_type, Getnext (function), fman, first, 0); // creation en niveau 2
              l->create_db (filename, fname, return_type, Getnext (function), fman, first, 0); // creation en niveau 2
              }
            else
            {
              l->create_xml (filename, fname, return_type, Getnext (function), fman, first, 4);// creation en niveau 3
              l->create_db (filename, fname, return_type, Getnext (function), fman, first, 4);// creation en niveau 3
              }
            first = 0;
          }
        man = Getnext (man);
    }
    if (count!=0)
      {
        l->create_xml(filename, "", return_type, Getnext (function), NULL, first, 2); // fermeture de sous section
        l->create_db(filename, "", return_type, Getnext (function), NULL, first, 2); // fermeture de sous section
      }
#ifdef DELAY_DEBUG_STAT
    l->create_man_man (proto_list);
#endif
    //l->create_online (proto_list, (ManList *)reverse((chain_list *)man));

    return 0;
}

Language::Language() {}
Language::~Language() {}
  //virtual functions required by the SWIG parser

void Language::parse_args (int ac, char *argv[]) 
{
  ac=(int)(long)argv;
}
void Language::parse () {}
void Language::create_online (char *name, ApiType *t, ParmList *l, functionman *m) 
{
  m = NULL;
  l = NULL;
  t = NULL;
  name = NULL;
}
void Language::create_man_man (chain_list *proto_list)
{
    chain_list *to_keep_gcc_silent = proto_list;
    to_keep_gcc_silent = NULL;
}
void Language::create_filter_online (char *filter, char *online_path, Man *mans)
{

}

void Language::create_man (const char *name, ApiType *t, ParmList *l, functionman *m)
{
  m = NULL;
  l = NULL;
  t = NULL;
  name = NULL;
}
void Language::create_xml (const char *filename, const char *name, ApiType *t, ParmList *l, functionman *m, int first, int mode)
{
  mode = 0;
  first = 0;
  m = NULL;
  l = NULL;
  t = NULL;
  name = NULL;
  filename = NULL;
}
void Language::create_db (const char *filename, const char *name, ApiType *t, ParmList *l, functionman *m, int first, int mode)
{
  mode=0;
  first = 0;
  m = NULL;
  l = NULL;
  t = NULL;
  name = NULL;
  filename = NULL;
}
void Language::create_filter (char *, char *, Man *)
{

}
void Language::create_function (char *name, char *iname, ApiType *t, ParmList *l) 
{
  l = NULL;
  t = NULL;
  iname = NULL;
  name = NULL;
}
void Language::link_variable (char *a, char *b, ApiType *t) 
{
  a=b=(char *)t;
}
void Language::declare_const (char *a, char *b, ApiType *t, char *c) 
{
  a=b=c=(char *)t;
}
void Language::initialize (void) {}
void Language::headers (void) {}
void Language::close (void) {}
void Language::set_module (const char *a) 
{
  a=NULL;
}
void Language::create_command (char *a, char *b) 
{
  a=b;
}

char *ApiType_name (ApiType *d, int i)
{
  i=0;
  if (((_ApiType *)d->DATA)->varname)
    return ((_ApiType *)d->DATA)->varname;
  else
    return "";
}

char *ApiType_str (ApiType *d, int i)
{
  i=0;
  return ((_ApiType *)d->DATA)->TYPE_NAME;
}

Parm *Getnext (Parm *p)
{
  return p->NEXT;
}

ApiType *Gettype (Parm *p)
{
  return p;
}

int ParmList_len (ParmList *l)
{
  chain_list *cl;
  int i;
  for (cl=l,i=0;cl!=NULL;cl=cl->NEXT, i++) ;
  return i;
}

int ApiType_type (ApiType *p)
{
  return ((_ApiType *)p->DATA)->TYPE;
}

int ApiType_ispointer (ApiType *p) 
{
//  char *i;
  return strchr(((_ApiType *)p->DATA)->TYPE_NAME,' ') != NULL;
}

char *ApiType_base (ApiType *p)
{
  char *i, *c;
  i=strchr(((_ApiType *)p->DATA)->TYPE_NAME,' ');
  if (i==NULL) return ((_ApiType *)p->DATA)->TYPE_NAME;
  *i='\0';
  c=strdup((((_ApiType *)p->DATA)->TYPE_NAME));
  *i=' ';
  return c;
}

char *Char(char *c)
{
  return c;
}

void Api_banner(FILE *f)
{
  fprintf(f,"\n// File generated by avtwig wrapper generator\n\n");
}

void *Getdata(Parm *p)
{
  return p->DATA;
}
