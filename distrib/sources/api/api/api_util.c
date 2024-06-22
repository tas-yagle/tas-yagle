#include <dlfcn.h>
#include <tcl.h>
#include API_H
#include AVT_H

#ifndef LIB_PATH
#define LIB_PATH "cells"        
#endif

// pour ajouter une autre lib, ajouter ':'<lib>
#define DEFAULT_LIBS "gen_builtin_functions.so:gen_API.so:beg_API.so:ctk_API.so:"\
                     "database_API.so:fcl_API.so:mbk_API.so:"\
                     "sim_API.so:spi_API.so:stm_API.so:ttv_API.so"

chain_list *API_DEFINES=NULL;
chain_list *API_ACTION_INIT=NULL;
chain_list *API_ACTION_END=NULL;
chain_list *API_ACTION_RESTART=NULL;
chain_list *API_ACTION_TOPLEVEL=NULL;
int token_already_taken=0;

/* ------------------------------------------------------------------------- */

t_arg *NewArg ()
{
    return (t_arg*)mbkalloc (sizeof (struct t_arg));
}

/* ------------------------------------------------------------------------- */

int TypeCheck (t_arg *arg, char *type)
{
    return strcmp (arg->TYPE, type);
}

/* ------------------------------------------------------------------------- */
    
int GetPointer (t_arg *arg)
{
    return arg->POINTER;
}

/* ------------------------------------------------------------------------- */

void SetPointer (t_arg *arg, int p)
{
    arg->POINTER = p;
}

/* ------------------------------------------------------------------------- */

void SetType (t_arg *arg, char *t)
{
    arg->TYPE = mbkstrdup (t);
}


void *grab_special_function(char *prefix, char *name)
{
  char temp[1024], *c;
  void *func;
  sprintf(temp,"wrap_%s",prefix);
  if ((c=strrchr(temp,'.'))!=NULL)
    *c='\0';
  strcat(temp,name);
  func=GetDynamicFunction(temp);
  return func;
}

void *nowrap_grab_special_function(char *prefix, char *name)
{
  char temp[1024], *c;
  void *func;
  strcpy(temp,prefix);
  if ((c=strrchr(temp,'.'))!=NULL)
    *c='\0';
  strcat(temp,name);
  func=GetDynamicFunction(temp);
  return func;
}

//typedef int (*libfunc_type)(t_arg **ret, t_arg **prm, int n_params, char *errstr);
//typedef void (*voidfunc)();
typedef t_arg ** (*targfunc)(int *);

void grab_special_functions(char *prefix)
{
  libfunc_type func;
  targfunc func0;
  char buf[256];
  t_arg *ret;

  func=grab_special_function(prefix,"_AtLoad_Initialize");
  if (func!=NULL) 
    { 
      if (func(&ret, NULL, 0 , buf)!=0)
        {
          avt_errmsg(API_ERRMSG, "001", AVT_ERROR, prefix, buf);
          //fprintf(stderr,"Error executing %s_AtLoad_Initialize() : %s\n", prefix, buf);
        }
      mbkfree(ret->TYPE);
      free(ret); 
    }
  func=grab_special_function(prefix,"_Action_Initialize");
  if (func!=NULL) API_ACTION_INIT=addchain(API_ACTION_INIT, func);
  func=grab_special_function(prefix,"_Action_Terminate");
  if (func!=NULL) API_ACTION_END=addchain(API_ACTION_END, func);
  func=grab_special_function(prefix,"_Restart");
  if (func!=NULL) API_ACTION_RESTART=addchain(API_ACTION_RESTART, func);
  func=grab_special_function(prefix,"_TopLevel");
  if (func!=NULL) API_ACTION_TOPLEVEL=addchain(API_ACTION_TOPLEVEL, func);

  func0=(targfunc)nowrap_grab_special_function(prefix, "_getdefines");
  if (func0!=NULL)
    {
      t_arg **tmp;
      int nb, i;
      tmp=func0(&nb);
      if (nb!=0)
        {
          for (i=0;i<nb;i++)
            {
              API_DEFINES=addchain(API_DEFINES, tmp[i]);
            }
        }
    }
}
// non definitif
static int libs_are_already_loaded=0;

int trytoopenlib(char *path, char *name, char *mes, char *where)
{
  char *tmp, newlibpath[4096], pwd[2048], *c;
  // LD_LIBRARY_PATH first
  if(dlopen(name,RTLD_LAZY|RTLD_GLOBAL)!=NULL) return 1;

  // API PATH next
  strcpy(newlibpath,path);
  path=newlibpath;
  tmp=strchr(newlibpath,':');
  while (tmp!=NULL)
    {
      *tmp='\0';
      strcpy(where,path);
      sprintf(pwd,"%s/%s",path,name);
      if(dlopen(pwd,RTLD_LAZY|RTLD_GLOBAL)!=NULL) return 1;
      if (strlen(mes)==0 || strstr(mes," such ")!=NULL)
        {
          if ((c=dlerror())!=NULL)
            strcpy(mes,c);
          else
            strcpy(mes,"unknown error");
        }
      tmp++;
      path=tmp;
      tmp=strchr(path,':');
    }
  strcpy(where,path);
  sprintf(pwd,"%s/%s",path,name);
  if(dlopen(pwd,RTLD_LAZY|RTLD_GLOBAL)!=NULL) return 1;
  if (strlen(mes)==0 || strstr(mes," such ")!=NULL)
    {
      if ((c=dlerror())!=NULL)
        strcpy(mes,c);
      else
        strcpy(mes,"unknown error");
    }

  return 0;
}

static char *getdistpath()
{
  char *e;
  char temp[4096];
  e=getenv("AVT_TOOLS_DIR");
  if (e==NULL)
    {
      e=getenv("AVT_DISTRIB_DIR");
      if (e==NULL) return NULL;
      sprintf(temp,"%s/api_lib",e);
      return mbkstrdup(temp);
    }
  if (getenv("AVTOS")==NULL)
      sprintf(temp,"%s/tools/Solaris/api_lib",e);
  else
      sprintf(temp,"%s/tools/%s/api_lib",e,getenv("AVTOS"));
  return mbkstrdup(temp);
}

void LoadDynamicLibraries(FILE *debug)
{
  char path[4096], *tpath, *libpath, libpath0[4096], *tmp, newlibpath[4096];
  char message[4096], location[2048];
  char *DEFAULT_LIB_PATH;

  if (libs_are_already_loaded) return;
  libs_are_already_loaded=1;
  
  DEFAULT_LIB_PATH=getdistpath();
  tpath=V_STR_TAB[__API_LIB_PATH].VALUE;
  if (DEFAULT_LIB_PATH!=NULL)
    {
      if (tpath!=NULL)
        sprintf(path,"%s:%s",tpath, DEFAULT_LIB_PATH);
      else
        strcpy(path,DEFAULT_LIB_PATH);
      mbkfree(DEFAULT_LIB_PATH);
    }
  else
    {
      if (tpath!=NULL)
        strcpy(path,tpath);
      else
        strcpy(path,".");
    }

  tpath=V_STR_TAB[__API_LIBS].VALUE;
  if (tpath!=NULL)
    {
      sprintf(libpath0,"%s:%s",DEFAULT_LIBS,tpath);
      libpath=libpath0;
    }
  else
    libpath=DEFAULT_LIBS;

  if (libpath!=NULL)
    {
      strcpy(newlibpath,libpath);
      libpath=newlibpath;
      tmp=strchr(newlibpath,':');
      while (tmp!=NULL)
        {
          *tmp='\0';
#if 0
          if (debug) fprintf(debug,"Opening dynamic library '%s' ... ",libpath);
          strcpy(message,"");
          if(!trytoopenlib(path,libpath, message, location))
            {
              if (debug)
                {
                  fprintf(debug,"failed\n\t");
                  fprintf(debug,"%s\n",message);
                }
              else 
                avt_errmsg(API_ERRMSG, "002", AVT_WARNING, libpath, message);
                // avt_fprintf(stderr,"[¤4warning¤.] could not open dynamic library '¤+%s¤.'\n\treason: %s\n", libpath, message);
              grab_special_functions(libpath); // API en static peut etre
            }
          else   
            {
#endif
              grab_special_functions(libpath);
#if 0
              if (debug) fprintf(debug,"done (%s)\n",location);
            }
#endif
          tmp++;
          libpath=tmp;
          tmp=strchr(libpath,':');
        }
#if 0      
      if (debug) fprintf(debug,"Opening dynamic library '%s' ... ",libpath);
      strcpy(message,"");
      if(!trytoopenlib(path,libpath,message,location))
        {
          if (debug)
            {
              fprintf(debug,"failed\n\t");
              fprintf(debug,"%s\n",message);
            }
          else 
            avt_errmsg(API_ERRMSG, "002", AVT_WARNING, libpath, message);
            // avt_fprintf(stderr,"[¤4warning¤.] could not open dynamic library '¤+%s¤.'\n\treason: %s\n", libpath, message);
          grab_special_functions(libpath); // API en static peut etre
        }
      else 
        {
#endif
          grab_special_functions(libpath);
#if 0
          if (debug) fprintf(debug,"done (%s)\n",location);
        }
#endif
    }
}

void *GetDynamicFunction(char *function_name)
{
  void *libfunc;
#ifdef RTLD_DEFAULT
  libfunc=dlsym(RTLD_DEFAULT,function_name);
#else
  libfunc=dlsym(NULL, function_name);
#endif
  return libfunc;
}

char *getdistpath_2(char *temp)
{
  char *e;
  e=V_STR_TAB[__AVT_TEMPLATE_DIR].VALUE;
  if (e==NULL)
    {
      e=getenv("AVT_TOOLS_DIR");
      if (e==NULL) 
        {
          strcpy(temp,".");
          return temp;
        }
      sprintf(temp,"%s/gns_templates",e);
      return temp;
    }
  strcpy(temp,e);
  return temp;
}

static chain_list *splitenv()
{
  char *e, *c, *start;
  chain_list *cl=NULL;
  char temp[1024];

  e=V_STR_TAB[__GENIUS_LIB_PATH].VALUE;
  if (e==NULL)
    {
      strcpy(temp, WORK_LIB);
      strcat(temp, "/"LIB_PATH);
      cl=addchain(cl, mbkstrdup(temp));
    }
  else
    {
      strcpy(temp, e);
      start=temp;
      c=strchr(start,':');
      while (c!=NULL)
        {
          *c='\0';
          cl=addchain(cl, mbkstrdup(start));
          start=c+1;
          c=strchr(start,':');
        }
      cl=addchain(cl, mbkstrdup(start));
    }

  cl=addchain(cl, mbkstrdup(getdistpath_2(temp)));

  return reverse(cl);
}

static void freespitenv(chain_list *cl)
{
  chain_list *ch;
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    mbkfree(ch->DATA);

  freechain(cl);
}

static FILE *findfile(char *name, chain_list *paths, char *temp)
{
  chain_list *cl;
  FILE *f;

  if (name[0]=='/')
    {
      strcpy(temp, name);
      return mbkfopen(name,NULL, READ_TEXT);
    }

  for (cl=paths; cl!=NULL; cl=cl->NEXT)
    {
      sprintf(temp,"%s/%s", (char *)cl->DATA, name);
      if ((f=mbkfopen(temp,NULL, READ_TEXT))!=NULL) break;
    }
  return f;
}

FILE *APIFindFile(lib_entry *p)
{
  FILE *f;
  char temp[2048];
  chain_list *PATHS;
  
  PATHS=splitenv();
  
  f = findfile(p->name, PATHS, temp);
  
  if (f!=NULL)
    {
      mbkfree(p->name);
      p->name=mbkstrdup(temp);
    }
  freespitenv(PATHS);
  return f;
}


int ReadAllCorVHDL(ExecutionContext *genius_ec, lib_entry *files_list, FILE *debug) 
{
  int err;
  lib_entry *p;
  tree_list *res;
  FILE *f;
  char temp[2048];
  chain_list *PATHS;

  PATHS=splitenv();

  for (p=files_list;p;p=p->NEXT) 
    {
      if (p->format==0)
        {
          if (strcasecmp(p->name,"none")!=0)
            {
              f = findfile(p->name, PATHS, temp);
              if (f!=NULL) 
                {
                  char *r;
                  if (debug)
                    fprintf(debug, "Opening model file %s....\n",p->name);
                  mbkfree(p->name);
                  p->name=mbkstrdup(temp);
                  r=strrchr(p->name, '.');
                  if (r==NULL || strcasecmp(r, ".tcl")!=0)
                    res=APIParseFile(f, p->name, genius_ec, p->paramset);
                  else
                  {
                    int nb;
                    r=mbkalloc(1000000);
                    nb=fread(r,sizeof(char),1000000, f);
                    r[nb]='\0';
                    if (Tcl_EvalEx((Tcl_Interp *)TCL_INTERPRETER, r, -1, TCL_EVAL_GLOBAL)==TCL_ERROR)
                       avt_errmsg(GNS_ERRMSG, "165", AVT_ERROR, p->name, ((Tcl_Interp *)TCL_INTERPRETER)->errorLine, Tcl_GetStringResult((Tcl_Interp *)TCL_INTERPRETER));
                    mbkfree(r);
                  }

                }
              else 
                {
                  avt_errmsg(GNS_ERRMSG, "166", AVT_ERROR, p->name);
                  Inc_Error();
                }   
            }
        }
      else
        {
          f = findfile(p->name, PATHS, temp);
          if (f!=NULL)
            {
              mbkfree(p->name);
              p->name=mbkstrdup(temp);
            }
        }
    }
  freespitenv(PATHS);
  err = Get_Error();
  if (err) return 1;
  APIVerify_C_Functions (genius_ec);
  if (APICheckCFunctions (genius_ec)) return 1;
  return 0;
}

void APICallApiInitFunctions()
{
  chain_list *p;
  t_arg *ret;
  libfunc_type func;
  char buf[128];

  for (p=API_ACTION_INIT; p!=NULL; p=p->NEXT)
    {
      func=(libfunc_type)p->DATA;
      func(&ret, NULL, 0 , buf);
      mbkfree(ret->TYPE); mbkfree(ret);
    }
}

void APICallApiTerminateFunctions()
{
  chain_list *p;
  t_arg *ret;
  libfunc_type func;
  char buf[128];

  for (p=API_ACTION_END; p!=NULL; p=p->NEXT)
    {
      func=(libfunc_type)p->DATA;
      func(&ret, NULL, 0 , buf);
      mbkfree(ret->TYPE); mbkfree(ret);
    }
}

void APIPreprocess(char *src, char *dest, chain_list *defines)
{
  char *cs, *cd, *start;
  chain_list *cl;
  api_define_type *adt;
  
  cd=dest;
  cs=src;

  while (*cs!='$' && *cs!='\0') *(cd++)=*(cs++);
  while (*cs!='\0')
    {
      cs++;
      start=cs;
      while (*cs>' ' && *cs!='$' && *cs!='\0') cs++;
      
      cl=NULL;
      if (*cs=='$')
        {
          *cs='\0';
          for (cl=defines; cl!=NULL; cl=cl->NEXT)
           {
             adt=(api_define_type *)cl->DATA;
             if (strcasecmp(adt->ORIG, start)==0) break;
           }
          *cs='$';
          if (cl==NULL)
            while (start!=cs) *(cd++)=*(start++);
          else
            {
              for (start=adt->DEST; *start!='\0'; start++)
                *(cd++)=*start;
            }
          cs++;
        }
      else
        if (cs!='\0')
          {
            start--;
            while (start!=cs) *(cd++)=*(start++);
            *(cd++)=*start;
            cs++;
          }
      while (*cs!='$' && *cs!='\0') *(cd++)=*(cs++);
    }
  *(cd++)='\0';
}

static ht *tclfunc=NULL;

int api_has_tcl_func(char *name)
{
  long l;
  Tcl_CmdInfo tci;
  if (tclfunc==NULL) tclfunc=addht(10);
  name=sensitive_namealloc(name);
  if ((l=gethtitem(tclfunc, name))!=EMPTYHT)
    return (int)l;

  if (Tcl_GetCommandInfo((Tcl_Interp *)TCL_INTERPRETER, name, &tci)!=0) l=1;
  else l=0;
  addhtitem(tclfunc, name, l);
  return l;
}
