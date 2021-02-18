#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "api_wrp_gns.h"
extern "C" {
#include AVT_H
}

/* -------------------------------------------------------------------------- */

void GENIUS::parse_args (int argc, char *argv[]) 
{ 
    int i;
    char *c;
    for (i = 0 ; i < argc; i++)
        c = argv[i];
}

/* -------------------------------------------------------------------------- */

void GENIUS::parse () 
{ 
    if (isatty (2))
        fprintf (stderr, "\x1B[35m>>> Making wrappers for GENIUS <<<\x1B[0m\n");
    else
        fprintf (stderr, "Making wrappers for GENIUS\n");

    yyparse ();
}

/* -------------------------------------------------------------------------- */

String *get_proto (char *name, WIGType *d, ParmList *l)
{
    WIGType *pt;
    Parm *p;
    char *return_type;
    char *arg_type, *arg_name;
    char  params[2048] = "";
    char  buf[1024];
    char  buf2[2048] = "";
    char  prototype[2048];

    return_type = Char (WIGType_str (d, 0));

    for (p = l; p; p = Getnext (p)) {
        pt = Gettype (p);
        arg_type = Char (WIGType_str (pt, 0));
        arg_name = Char (WIGType_name (pt, 0));

        if (Getnext (p)) {
            if (WIGType_ispointer (pt))    
                sprintf (buf, "%s%s, ", Char (arg_type), Char (arg_name));
            else
                sprintf (buf, "%s %s, ", Char (arg_type), Char (arg_name));
        } else {
            if (WIGType_ispointer (pt))    
                sprintf (buf, "%s%s", Char (arg_type), Char (arg_name));
            else
                sprintf (buf, "%s %s", Char (arg_type), Char (arg_name));
        }
       
        sprintf (params, "%s%s", buf2, buf);
        sprintf (buf2, "%s", params);
    }
    
    if (WIGType_ispointer (d))
        sprintf (prototype, "%s%s(%s)", return_type, name, params);
    else
        sprintf (prototype, "%s %s(%s)", return_type, name, params);

    return strdup (prototype);
}

/* -------------------------------------------------------------------------- */

String *get_short_proto (char *name, WIGType *d, ParmList *l)
{
    WIGType *pt;
    Parm *p;
    char *return_type;
    char *arg_type, *arg_name;
    char  params[2048] = "";
    char  buf[1024];
    char  buf2[2048] = "";
    char  prototype[2048];

    return_type = Char (WIGType_str (d, 0));

    for (p = l; p; p = Getnext (p)) {
        pt = Gettype (p);
        arg_type = Char (WIGType_str (pt, 0));
        arg_name = Char (WIGType_name (pt, 0));

        if (Getnext (p))
            sprintf (buf, "%s, ", Char (arg_type));
        else
            sprintf (buf, "%s", Char (arg_type));
       
        sprintf (params, "%s%s", buf2, buf);
        sprintf (buf2, "%s", params);
    }
    
    if (WIGType_ispointer (d))
        sprintf (prototype, "%-15s %s(%s)", return_type, name, params);
    else
        sprintf (prototype, "%-15s %s(%s)", return_type, name, params);

    return strdup (prototype);
}


/* -------------------------------------------------------------------------- */

#ifdef USE_AVTWIG

/* -------------------------------------------------------------------------- */

char **xml_text_args (char *args)
{
    int i, lnum = 0;
    char *lines[128];
    char **argtab;
    char *token;

    argtab = (char**)malloc (128 * sizeof (char*));

    token = strtok (args, "\n");
    lines[lnum++] = token;
    while ((token = strtok (NULL, "\n"))) lines[lnum++] = token;
    lines[lnum] = NULL;

    for (i = 0; i < lnum; i++) {
        token  = strchr (lines[i], '%');
        argtab[2 * i + 1] = strdup (token? token + 1: "No description");
        argtab[2 * i] = strdup (strtok (lines[i], "%") + 2);
    }
    argtab[2 * i] = NULL;

    
    return argtab;
}
/* -------------------------------------------------------------------------- */
void stripCR (char *str, char *buf)
{
    int i = 0, j = 0;
    while (str[i] != '\0') {
        if (str[i] != '\n') buf[j++] = str[i];
        i++;
    }
    buf[j] = '\0';
}

void GENIUS::create_online (char *name, WIGType *d, ParmList *l, functionman *man) 
{ 
    FILE *f;
    char fname[1024], date[1024];
    char *description;
    char *return_value;
    char *errors;
    char *see_also;

    if (!man) 
      {
        avt_fprintf(stderr,"¤6¤~ **  WARNING: NO MAN FOR FUNCTION '%s' ** ¤.\n", name);
        fflush(stderr);
        return;
      }
    if (!MANPATH) return;

    sprintf (fname, "%s/%s.3", MANPATH, name);
    if (!(f = fopen (fname, "w+"))) {
        fprintf (stderr, "[API WAR] Can't open file %s for writing\n", fname);
        fflush(stderr);
        return;
    }

    fprintf (stdout, "Creating man page for `%s'\n", get_short_proto (name, d, l));
    fflush(stdout);

    avt_date (date);

    fprintf (f, ".TH %s 3 \"%s\" \"Avertec\" \"Genius Dynamic Libraries\"\n", name, date);
    // _____ prototype
    fprintf (f, ".SH NAME\n");
    fprintf (f, "%s\n", name);
    fprintf (f, "\n.LP\n");
    fprintf (f, ".SH SYNOPSIS\n");
    fprintf (f, "%s;\n", man->synop?man->synop:get_proto (name, d, l));
    fprintf (f, "\n.LP\n");
    // _____ description
    fprintf (f, ".SH DESCRIPTION\n");
//    section = man;
    description = man->desc;
    fprintf (f, "%s", description);
    fprintf (f, "\n.LP\n");
    // _____ return value
    fprintf (f, ".SH RETURN VALUE\n");
//    section = Getnext (section);
    return_value = man->retval;
    fprintf (f, "%s", return_value);
    fprintf (f, "\n.LP\n");
    // _____ errors
    fprintf (f, ".SH ERRORS\n");
//    section = Getnext (section);
    errors = man->errors;
    fprintf (f, "%s", errors);
    fprintf (f, "\n.LP\n");
    // _____ see also
    fprintf (f, ".SH SEE ALSO\n");
//    section = Getnext (section);
    see_also = man->seealso;
    fprintf (f, "%s", see_also);
    fprintf (f, "\n.LP\n");

    fclose (f);
}
/*
void GENIUS::create_online (chain_list *proto_list, chain_list *man)
{
    chain_list *pt;
    Parm *function;
    char *name, *proto;
    FILE *f;
    WIGType *return_type;
    functionman *m;
    char *args;
    int i;
    char **argtab;
    char buf[8192];

    if (!proto_list) return;

    if (!(f = fopen (ONLINE_FILE, "w+"))) {
        fprintf (stderr, "[API WAR] Can't open file %s for writing\n", ONLINE_FILE);
        return;
    }

    fprintf (stderr, "Creating online help %s\n", ONLINE_FILE);

    fprintf (f, "#include <stdio.h>\n\n");
    fprintf (f, "char *avt_API_COMMANDS[] =\n");
    fprintf (f, "{\n");


    for (pt = proto_list; pt; pt = Getnext (pt)) {
        m=(functionman *)man->DATA;
        function = (Parm*)Getdata (pt);
        name = (char*)Getdata (function);
        fprintf (f, "// ------------------- %s\n", name);
        fprintf (f, "\"%s\", ", name);
        function = Getnext (function);
        return_type = (ApiType*)function;
        proto = m->synop?m->synop:get_proto (name, return_type, Getnext (function));
        fprintf (f, "\"%s\", ", proto);
        if (m) {
            stripCR (m->desc, buf);
            fprintf (f, "\"%s\", ", buf);
            stripCR (m->retval, buf);
            fprintf (f, "\"%s\", ", buf);
            if ((args = m->errors)) {
                if (strcmp (args, "")) {
                    if ((argtab = xml_text_args (args))) {
                        fprintf (f, "\"");
                        for (i = 0; argtab[i]; i += 2) 
                            fprintf (f, "%s %s\\\n", argtab[i], argtab[i + 1]);
                        fprintf (f, "\"");
                        for (i = 0; argtab[i]; i++) free (argtab[i]);
                    }
                }
                else
                    fprintf (f, "NULL");
            }
        }
        else {
            fprintf (f, "NULL,");
            fprintf (f, "NULL,");
            fprintf (f, "NULL");
        }
        fprintf (f, ",\n");
        man = Getnext (man);
    }
    fprintf (f, "// ------------------- END OF ARRAY\n");
    fprintf (f, "NULL};\n");

    fclose (f);
}
*/
void GENIUS::create_man_man (chain_list *proto_list)
{
    chain_list *pt;
    Parm *function;
    char *name, *proto, *dot;
    char buf[1024], date[1024], man[1024];
    FILE *f;
    WIGType *return_type;

    if (!proto_list) return;
    if (!MANPATH) return;

    strcpy (man, wrapperfile);
    if ((dot = strrchr (man, '.')))
        *dot = '\0';

    sprintf (buf, "%s/%s.3", MANPATH, man);
    if (!(f = fopen (buf, "w+"))) {
        fprintf (stderr, "[API WAR] Can't open file %s for writing\n", man);
        return;
    }

    fprintf (stderr, "Creating man page for %s\n", man);
    avt_date (date);

    fprintf (f, ".TH %s 3 \"%s\" \"Avertec\" \"Genius Dynamic Libraries\"\n", 
             man, date);

    fprintf (f, ".SH SEE ALSO\n");
    for (pt = proto_list; pt; pt = Getnext (pt)) {
        function = (Parm*)Getdata (pt);
        name = (char*)Getdata (function);
        function = Getnext (function);
        return_type = (ApiType*)function;
        proto = get_proto (name, return_type, Getnext (function));
        if (strstr(name,"_Action_")==NULL && strstr(name,"_TopLevel")==NULL && strstr(name,"_AtLoad_")==NULL && strstr(name,"_Restart")==NULL)
          {
            fprintf (f, "%s\n", proto);
            fprintf (f, ".LP\n");
          }
    }

    fclose (f);
}

void GENIUS::create_man (char *name, WIGType *d, ParmList *l, functionman *man) 
{ 
    FILE *f;
    char fname[1024], date[1024];
    char *description;
    char *return_value;
    char *errors;
    char *see_also;

    if (!man) 
      {
        avt_fprintf(stderr,"¤6¤~ **  WARNING: NO MAN FOR FUNCTION '%s' ** ¤.\n", name);
        fflush(stderr);
        return;
      }
    if (!MANPATH) return;

    sprintf (fname, "%s/%s.3", MANPATH, name);
    if (!(f = fopen (fname, "w+"))) {
        fprintf (stderr, "[API WAR] Can't open file %s for writing\n", fname);
        fflush(stderr);
        return;
    }

    fprintf (stdout, "Creating man page for `%s'\n", get_short_proto (name, d, l));
    fflush(stdout);

    avt_date (date);

    fprintf (f, ".TH %s 3 \"%s\" \"Avertec\" \"Genius Dynamic Libraries\"\n", name, date);
    // _____ prototype
    fprintf (f, ".SH NAME\n");
    fprintf (f, "%s\n", name);
    fprintf (f, "\n.LP\n");
    fprintf (f, ".SH SYNOPSIS\n");
    fprintf (f, "%s;\n", man->synop?man->synop:get_proto (name, d, l));
    fprintf (f, "\n.LP\n");
    // _____ description
    fprintf (f, ".SH DESCRIPTION\n");
//    section = man;
    description = man->desc;
    fprintf (f, "%s", description);
    fprintf (f, "\n.LP\n");
    // _____ return value
    fprintf (f, ".SH RETURN VALUE\n");
//    section = Getnext (section);
    return_value = man->retval;
    fprintf (f, "%s", return_value);
    fprintf (f, "\n.LP\n");
    // _____ errors
    fprintf (f, ".SH ERRORS\n");
//    section = Getnext (section);
    errors = man->errors;
    fprintf (f, "%s", errors);
    fprintf (f, "\n.LP\n");
    // _____ see also
    fprintf (f, ".SH SEE ALSO\n");
//    section = Getnext (section);
    see_also = man->seealso;
    fprintf (f, "%s", see_also);
    fprintf (f, "\n.LP\n");

    fclose (f);
}

/* -------------------------------------------------------------------------- */

static void
man_text(char *buf, char *src)
{
    while (isspace(*src)) src++;
    while (*src) {
        if ((*src == '{' || *src == '}') && *(src+1)=='`') {
            *buf = *src;
            src++;
            buf++;
        }
        else if (*src == '{') {
            *buf = 0;
            strcat(buf, "\\fI");
            buf += 3;
        }
        else if (*src == '}') {
            *buf = 0;
            strcat(buf, "\\fR");
            buf += 3;
        }
        else if (*src == '\\' && *(src+1)=='$') {
            *buf = 0;
            strcat(buf, "\n.br\n");
            src++;
            buf += 4;
        }
        else if (*src == '\\' && *(src+1)=='}') {
            *buf = '}';
            src++;
        }
        else if (*src == '\\' && *(src+1)=='{') {
            *buf = '{';
            src++;
        }
        else {
            *buf = *src;
            buf++;
        }
        src++;
    }
    *buf = 0;
}

static void
xml_text(char *buf, char *src)
{
    while (*src) {
        if (*src == '<') {
            *buf = 0;
            strcat(buf, "&lt;");
            buf += 4;
        }
        else if (*src == '>') {
            *buf = 0;
            strcat(buf, "&gt;");
            buf += 4;
        }
        else if ((*src == '{' || *src == '}') && *(src+1)=='`') {
            *buf = *src;
            src++;
            buf++;
        }
        else if (*src == '{') {
            *buf = 0;
            strcat(buf, "<f>");
            buf += 3;
        }
        else if (*src == '}') {
            *buf = 0;
            strcat(buf, "</f>");
            buf += 4;
        }
        else if (*src == '\\')
        {
           char *add=NULL;
           if (*(src+1)=='$') add="<br></br>";
           if (*(src+1)=='{') add="{";
           if (*(src+1)=='}') add="}";
           
           if (add!=NULL) {
             *buf = 0;
             strcat(buf, add);
             buf += strlen(add);
             src++;
           } else {
            *buf = *src;
            buf++;
           }
        }
        else {
            *buf = *src;
            buf++;
        }
        src++;
    }
    *buf = 0;
}

void GENIUS::create_xml (char *filename, char *name, WIGType *d, ParmList *l, functionman *man, int first, int mode) 
{ 
    FILE *f;
    char fname[1024];
    char tmpname[1024];
    char *description;
    char *prototype;
    char *basename;
    char  textbuf[10000];
    char *args;
    int i;
    char **argtab;

    if (!XMLPATH) return;

    basename = strrchr(filename, '/');
    if (basename == NULL) basename = filename;
    strncpy(tmpname, basename, strlen(basename)-2);
    tmpname[strlen(basename)-2] = 0;
    sprintf (fname, "%s/%s.xml", XMLPATH, tmpname);

    if (first) {
        if (!(f = fopen (fname, "wt"))) {
            fprintf (stderr, "[API WAR] Can't open file %s for writing\n", fname);
            return;
        }
    }
    else {
        if (!(f = fopen (fname, "a+"))) {
            fprintf (stderr, "[API WAR] Can't open file %s for writing\n", fname);
            return;
        }
    }

    if (first) {
        fprintf (f, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n");
    }
    if (!man && (mode & 3)==0) {
        fclose(f);
        return;
    }


    if (mode==0 || mode==4) fprintf (stdout, "Creating XML page for `%s'\n", get_proto (name, d, l));

    if (mode & 3)
      {
        if (mode & 2) // fermeture
            fprintf (f, "</func>\n");

        if (mode & 1) // ouverture
          fprintf (f, "<func>\n");
        fclose(f);
        return;
      }

    fprintf (f, "<func>\n");
    fprintf (f, "<funcname><section niv='%d'><title>%s</title></section></funcname>\n", xmllevel, name);

    prototype = man->synop?man->synop:get_proto (name, d, l);
    xml_text (textbuf, prototype);
    fprintf (f, "<proto>%s</proto>", textbuf);
    
    description = man->desc;
    xml_text (textbuf, description);
    fprintf (f, "<desc>%s</desc>", textbuf);

    if ((args = man->errors)) {
        if (strcmp (args, "")) {
            argtab = xml_text_args (args);
            if (argtab) {
                for (i = 0; argtab[i]; i += 2) {
                    fprintf (f, "<arg>");
                    xml_text (textbuf, argtab[i]);
                    fprintf (f, "<argn>%s</argn>", textbuf);
                    xml_text (textbuf, argtab[i + 1]);
                    fprintf (f, "<argdef>%s</argdef>", textbuf);
                    fprintf (f, "</arg>\n");
                }
                for (i = 0; argtab[i]; i++) free (argtab[i]);
            }
        }
    }
    else {
        fprintf (f, "<arg></arg>\n");
    }
    fprintf (f, "</func>\n");
    
    fclose (f);
}

void GENIUS::create_db (char *filename, char *name, WIGType *d, ParmList *l, functionman *man, int first, int mode) 
{ 
    FILE *f;
    char fname[1024];
    char tmpname[1024];
    char *prototype, *basename, *c;
    char  textbuf[10000];

    if (!DBPATH) return;

    basename = strrchr(filename, '/');
    if (basename == NULL) basename = filename;
    strncpy(tmpname, basename, strlen(basename)-2);
    tmpname[strlen(basename)-2] = 0;
    sprintf (fname, "%s/%s.db", DBPATH, tmpname);

    if (first) {
        if (!(f = fopen (fname, "wt"))) {
            fprintf (stderr, "[API WAR] Can't open file %s for writing\n", fname);
            return;
        }
    }
    else {
        if (!(f = fopen (fname, "a+"))) {
            fprintf (stderr, "[API WAR] Can't open file %s for writing\n", fname);
            return;
        }
    }

    if (!man && (mode & 3)==0) {
        fclose(f);
        return;
    }

    if (mode & 3)
      {
        fclose(f);
        return;
      }

    prototype = man->synop?man->synop:get_proto (name, d, l);
    strcpy(textbuf, man->desc);
    while ((c=strchr(textbuf,'\n'))!=NULL) *c='\\';
    fprintf(f, "%10s || %10s || %100s || %s\n", tmpname, man->categ, prototype, textbuf);
    
    fclose (f);
}

#endif

/* -------------------------------------------------------------------------- */
static int done=0;

void GENIUS::create_defines_function()
{
  chain_list *cl;
  defines *def;
  char buf[1024], *dot;
  int i;

  if (done) return;
  done=1;

  strcpy(buf, wrapperfile);
  if ((dot = strchr (buf, '.'))) *dot = '\0';
  Printf (f_wrappers,"t_arg **%s_getdefines(int *nb)\n{\n", buf);
  Printf (f_wrappers,"  static t_arg *tmp[1024];\n\n");
      
  for (i=0, cl=alldefines; cl!=NULL; cl=cl->NEXT)
    {
      def=(defines *)cl->DATA;
      Printf (f_wrappers,"  tmp[%d]=NewArg ();\n", i);
      switch(def->type)
    {
    case 'I':
      Printf (f_wrappers,"  tmp[%d]->POINTER=0; tmp[%d]->NAME=\"%s\";\n", i, i, def->VARNAME);
      Printf (f_wrappers,"  tmp[%d]->TYPE=\"int\"; tmp[%d]->VALUE=(int *)malloc(sizeof(int));\n", i, i);
      Printf (f_wrappers,"  *(int *)tmp[%d]->VALUE=%d;\n", i, def->T.I);
      break;
    case 'C':
      Printf (f_wrappers,"  tmp[%d]->POINTER=0; tmp[%d]->NAME=\"%s\";\n", i, i, def->VARNAME);
      Printf (f_wrappers,"  tmp[%d]->TYPE=\"char\"; tmp[%d]->VALUE=(char *)malloc(sizeof(char));\n", i, i);
      Printf (f_wrappers,"  *(char *)tmp[%d]->VALUE='%c';\n", i, def->T.I);
      break;
    case 'D':
      Printf (f_wrappers,"  tmp[%d]->POINTER=0; tmp[%d]->NAME=\"%s\";\n", i, i, def->VARNAME);
      Printf (f_wrappers,"  tmp[%d]->TYPE=\"double\"; tmp[%d]->VALUE=(double *)malloc(sizeof(double));\n", i, i);
      Printf (f_wrappers,"  *(double *)tmp[%d]->VALUE=%g;\n", i, def->T.D);
      break;
    }
      Printf (f_wrappers,"\n");
      i++;
    }

  Printf (f_wrappers,"  *nb=%d;\n",i);
  Printf (f_wrappers,"  return tmp;\n}\n\n");


}

void GENIUS::create_function_getargs (char *name, char *iname, WIGType *d, ParmList *l) 
{ 
    int i;
    WIGType *pt;
    Parm *p;
    int pcount = ParmList_len (l);
#ifdef __ALL__WARNING__
    char err_incomp[1024];
    char err_incomp_pt[1024];
    char type_check[1024];
    char pt_check[1024];
#else
    d 		= NULL;
    iname	= NULL;
#endif // ..anto..

    // on evite les fonctions a nombre de parametres variable
    for (p = l; p; p = Getnext (p)) 
      {
        pt = Gettype (p);
        if (!strcmp (Char (WIGType_base (pt)), "...") ||
          (!strcmp (Char (WIGType_base (pt)), "void") && WIGType_ispointer (pt) == 0)) return ;
        //if (!strcmp (Char (WIGType_base (pt)), "...")) return ;
      }

    Printf (f_wrappers, "\nvoid wrap_getargs_%s (t_arg ***ret, int *nb)\n", name);
    Printf (f_wrappers, "{\n");
    if (pcount>0)
      {
        Printf (f_wrappers, "    *ret=(t_arg **)malloc(sizeof(t_arg *)*%d);\n",pcount);
      }
    else
      Printf (f_wrappers, "    *ret=NULL;\n");
    Printf (f_wrappers, "    *nb=%d;\n",pcount);

    /* Type Checking for prm */
    i = 0;
    for (p = l; p; p = Getnext (p)) {
        pt = Gettype (p);
        Printf (f_wrappers, "    (*ret)[%d] = NewArg();\n", i);
        Printf (f_wrappers, "    ((*ret)[%d])->NAME = \"?\";\n", i);
        Printf (f_wrappers, "    SetType ((*ret)[%d], \"%s\");\n", i, WIGType_base (pt));
        Printf (f_wrappers, "    SetPointer ((*ret)[%d], %d);\n", i, WIGType_ispointer (pt));
        Printf (f_wrappers, "    (*ret)[%d]->VALUE= (%s*)malloc (sizeof (%s));\n", i, WIGType_str (pt, WIGType_ispointer (pt)), WIGType_str (pt, WIGType_ispointer (pt)));
        Printf (f_wrappers, "//  ---\n");
        i++;
    }
    Printf (f_wrappers, "}\n\n");
}

void GENIUS::create_function (char *name, char *iname, WIGType *d, ParmList *l) 
{ 
    int i;
    WIGType *pt;
    Parm *p;
    int pcount = ParmList_len (l);
    char err_incomp[1024];
    char err_incomp_pt[1024];
    char type_check[1024];
    char pt_check[1024];

    create_defines_function(); // it's a hack

    create_function_getargs (name, iname, d, l);

    // on evite les fonctions a nombre de parametres variable
    for (p = l; p; p = Getnext (p)) 
      {
        pt = Gettype (p);
        if (!strcmp (Char (WIGType_base (pt)), "...") ||
          (!strcmp (Char (WIGType_base (pt)), "void") && WIGType_ispointer (pt) == 0)) return ;
      }

    Printf (f_wrappers, "\nint wrap_%s (t_arg **ret, t_arg **prm, int n_params, char *errstr)\n", name);
    Printf (f_wrappers, "{\n");

    Printf (f_wrappers, "    int jmp;\n\n");
    Printf (f_wrappers, "    *ret = NewArg ();\n\n");

    Printf (f_wrappers, "    avt_PushSegVMessage (\"Segmentation violation in `%s'\");\n", get_proto (name, d, l));
    Printf (f_wrappers, "    avt_PushSegVExit (Exit, 2);\n");
    Printf (f_wrappers, "    avt_PushFPEMessage (\"Arithmetic exception in `%s'\");\n", get_proto (name, d, l));
    Printf (f_wrappers, "    avt_PushFPEExit (Exit, 3);\n");

    /* Number of prm Checking */
    Printf (f_wrappers, "    /* Number of prm Checking */\n");
    Printf (f_wrappers, "    if (n_params == 0)\n"); 
    Printf (f_wrappers, "        prm = NULL;\n");
    Printf (f_wrappers, "    if (n_params > %d) {\n", pcount); 
    Printf (f_wrappers, "        sprintf (errstr, \"too many arguments to function `%s'\");\n", get_proto (name, d, l)); 
    Printf (f_wrappers, "        return 1;\n");
    Printf (f_wrappers, "    }\n\n");
    Printf (f_wrappers, "    if (n_params < %d) {\n", pcount); 
    Printf (f_wrappers, "        sprintf (errstr, \"too few arguments to function `%s'\");\n", get_proto (name, d, l)); 
    Printf (f_wrappers, "        return 1;\n");
    Printf (f_wrappers, "    }\n\n");

    /* Type Checking for prm */
    Printf (f_wrappers, "    /* Type Checking for prm */\n");
    i = 0;
    for (p = l; p; p = Getnext (p)) {
        pt = Gettype (p);
        sprintf (err_incomp, "sprintf (errstr, \"incompatible type for argument %d of `%s'\");", i + 1, name);
        sprintf (err_incomp_pt, "sprintf (errstr, \"passing arg %d of `%s' from incompatible pointer type\");", i + 1, name);

        if (WIGType_type (pt) == T_POINTER && !strcmp (Char (WIGType_base (pt)), "void") && WIGType_ispointer (pt) == 1) {
            sprintf (pt_check, "(GetPointer (prm[%d]) == 0)", i);
            Printf (f_wrappers, "    if %s {\n        %s\n        return 1;\n    }\n", pt_check, err_incomp_pt);
        }
        else 
        if (WIGType_type (pt) == T_POINTER && WIGType_ispointer (pt) >= 1) {
            sprintf (type_check, "(TypeCheck (prm[%d], \"%s\") && TypeCheck (prm[%d], \"void\"))", i, Char (WIGType_base (pt)), i);
            sprintf (pt_check, "(GetPointer (prm[%d]) == 0)", i);
            Printf (f_wrappers, "    if %s {\n        %s\n        return 1;\n    }\n", type_check, err_incomp);
            Printf (f_wrappers, "    if %s {\n        %s\n        return 1;\n    }\n", pt_check, err_incomp_pt);
        }
        else {
            sprintf (type_check, "(TypeCheck (prm[%d], \"%s\"))", i, Char (WIGType_base (pt)));
            sprintf (pt_check, "(GetPointer (prm[%d]) != %d)", i, WIGType_ispointer (pt));
            Printf (f_wrappers, "    if %s {\n        %s\n        return 1;\n    }\n", type_check, err_incomp);
            Printf (f_wrappers, "    if %s {\n        %s\n        return 1;\n    }\n", pt_check, err_incomp_pt);
        }

        i++;
    }

    /* Function call */
    Printf (f_wrappers, "    /* Function call */\n");
    switch (WIGType_type (d)) {
        case T_VOID:
            Printf (f_wrappers, "    SetPointer (*ret, 0);\n");
            Printf (f_wrappers, "    SetType (*ret, \"void\");\n");
            Printf (f_wrappers, "    (*ret)->VALUE = NULL;\n");
            Printf (f_wrappers, "    if (!(jmp = setjmp (env))) {\n");
            Printf (f_wrappers, "        %s (", iname);
            break;
        case T_POINTER:
        case T_STRING:
        case T_CHAR:
        case T_INT:
        case T_LONG:
        case T_DOUBLE:
            String *type = WIGType_str (d, 0);
            Printf (f_wrappers, "    SetPointer (*ret, %d);\n", WIGType_ispointer (d));
            Printf (f_wrappers, "    SetType (*ret, \"%s\");\n", WIGType_base (d));
            Printf (f_wrappers, "    (*ret)->VALUE = (%s*)malloc (sizeof (%s));\n", type, type);
            Printf (f_wrappers, "    if (!(jmp = setjmp (env))) {\n");
            Printf (f_wrappers, "        *(%s*)(*ret)->VALUE = %s (", type, iname);
            break;
    }
            
    /* Arguments passing */
    i = 0;
    for (p = l; p; p = Getnext (p)) {
        pt = Gettype (p);
        if (WIGType_type (pt) != T_VOID) {
            Printf (f_wrappers, "\n            *(%s*)(prm[%d]->VALUE)", WIGType_str (pt, 0), i);
            if (Getnext (p))
                Printf (f_wrappers, ", ");
        }
        i++;
    }
    Printf (f_wrappers, "\n        );\n");
    Printf (f_wrappers, "    } else {\n");
    Printf (f_wrappers, "        avt_PopSegVMessage ();\n");
    Printf (f_wrappers, "        avt_PopSegVExit ();\n");
    Printf (f_wrappers, "        avt_PopFPEMessage ();\n");
    Printf (f_wrappers, "        avt_PopFPEExit ();\n");
    Printf (f_wrappers, "        return 2;\n");
    Printf (f_wrappers, "    }\n\n");

    Printf (f_wrappers, "    avt_PopSegVMessage ();\n");
    Printf (f_wrappers, "    avt_PopSegVExit ();\n");
    Printf (f_wrappers, "    avt_PopFPEMessage ();\n");
    Printf (f_wrappers, "    avt_PopFPEExit ();\n");

    Printf (f_wrappers, "    return 0;\n");
    Printf (f_wrappers, "}\n\n");
    create_command (name, iname);
}

/* -------------------------------------------------------------------------- */

void GENIUS::link_variable (char *name, char *iname, WIGType *t) 
{ 
    Printf (f_init, "\t/* Linking variable: %s%s (%s) */\n", WIGType_str (t, 0), iname, name);
}

/* -------------------------------------------------------------------------- */

void GENIUS::declare_const (char *name, char *iname, WIGType *t, char *value) 
{ 
    Printf (f_init, "\t/* Creating constant: %s%s (%s) = %s */\n", WIGType_str (t, 0), iname, name, value);
}

/* -------------------------------------------------------------------------- */

void GENIUS::initialize (void) 
{ 
    if (!module) module = "genius";
    Printf (f_init, "void %s_initialize () {\n", module);
}

/* -------------------------------------------------------------------------- */

void GENIUS::headers (void) 
{ 
    WIG_banner (f_header);
    Printf (f_header, "#include <stdio.h>\n");
    Printf (f_header, "#include <stdlib.h>\n");
    Printf (f_header, "#include <setjmp.h>\n");
    Printf (f_header, "#include <strings.h>\n");
    Printf (f_header, "#include <signal.h>\n");
    //Printf (f_header, "#include MUT_H\n");
    //Printf (f_header, "#include AVT_H\n");
    //Printf (f_header, "#include API_H\n");

    #include "headers.inc"

    Printf (f_header, "static jmp_buf env;\n\n");
    Printf (f_header, "static void Exit (int val)\n");
    Printf (f_header, "{\n");
    Printf (f_header, "    longjmp (env, val);\n");
    Printf (f_header, "}\n");
    
}

/* -------------------------------------------------------------------------- */

void GENIUS::close (void) 
{ 
    Printf (f_init, "}\n");
}

/* -------------------------------------------------------------------------- */

void GENIUS::set_module (char *mod_name) 
{ 
    if (module) return;
    module = new char[strlen (mod_name) + 1];
    strcpy (module, mod_name);
}

/* -------------------------------------------------------------------------- */

void GENIUS::create_command (char *cname, char *iname) 
{ 
    Printf (f_init, "\t/* Creating command %s (%s) */\n", iname, cname);
}

static chain_list *create_splitt(char *val)
{
  char *c, *s;
  chain_list *cl=NULL;
  s=strtok_r(val, "+", &c);
  while (s!=NULL)
    {
      cl=addchain(cl, s);
      s=strtok_r(NULL, "+", &c);
    }
  return cl;
}

void GENIUS::create_filter (char *filter, char *filterfile, Man *mans) 
{ 
    FILE *f;
    char *fname;
    char *description;
    char *prototype;
    char  textbuf[10000];
    chain_list *man, *filt, *cfilt, *cl, *ch, *pt;
    ApiType *return_type;
    functionman *fman;
    Parm *function;
    char **argtab;
    char *args;
    int i;

    if (!filter) return;

    if (!(f = fopen (filterfile, "wt"))) {
        fprintf (stderr, "[API WAR] Can't open file %s for writing\n", filterfile);
        return;
    }

    filt=create_splitt(filter);

    man=mans;

    fprintf (f, "<functions>\n");
    for (pt = proto_list; pt; pt = Getnext (pt)) {
        function = (Parm*)Getdata (pt);
        fname = (char*)Getdata (function);
        function = Getnext (function);
        return_type = (ApiType*)function;
        fman=(functionman *)man->DATA;

        if (fman) {
            cfilt = create_splitt(fman->categ);
            for (cl = filt; cl; cl = cl->NEXT) {
                for (ch = cfilt; ch; ch = ch->NEXT) {
                    if (strcmp((char *)ch->DATA, (char *)cl->DATA)==0) break;
                }
                if (ch==NULL) break;            
            }
            freechain(cfilt);

            if (cl==NULL) {
                fprintf (stdout, "Filtering XML page for `%s'\n", get_proto (fname, return_type, Getnext (function)));
                fprintf (f, "<func>\n");

                fprintf (f, "<funcname><section niv='%d'><title>%s</title></section></funcname>\n", xmllevel, fname);

                prototype = fman->synop?fman->synop:get_proto (fname, return_type, Getnext (function));
                xml_text (textbuf, prototype);
                fprintf (f, "<proto>%s</proto>", textbuf);
    
                description = fman->desc;
                xml_text (textbuf, description);
                fprintf (f, "<desc>%s</desc>", textbuf);

                if ((args = fman->errors)) {
                    if (strcmp (args, "")) {
                        argtab = xml_text_args (args);
                        if (argtab) {
                            for (i = 0; argtab[i]; i += 2) {
                                fprintf (f, "<arg>");
                                xml_text (textbuf, argtab[i]);
                                fprintf (f, "<argn>%s</argn>", textbuf);
                                xml_text (textbuf, argtab[i + 1]);
                                fprintf (f, "<argdef>%s</argdef>", textbuf);
                                fprintf (f, "</arg>\n");
                            }
                            for (i = 0; argtab[i]; i++) free (argtab[i]);
                        }
                    }
                }
                else {
                    fprintf (f, "<arg></arg>");
                }
                fprintf (f, "</func>\n");
            }
        }
        man = Getnext (man);
    }
    fprintf (f, "</functions>\n");

    freechain(filt);
    fclose (f);
}

void GENIUS::create_filter4xls (char *filter, char *filterfile, Man * mans)
{
    FILE *f;
    char *fname;
    char *description;
    char *prototype;
    char textbuf[10000];
    chain_list *man, *filt, *cfilt, *cl, *ch, *pt;
    ApiType *return_type;
    functionman *fman;
    Parm *function;
    char **argtab;
    char *args;
    int i;

    if (!filter)
    	return;

    if (!(f = fopen (filterfile, "wt"))) {
    	fprintf (stderr, "[API WAR] Can't open file %s for writing\n", filterfile);
    	return;
    }
    
    fprintf (f, "<?xml version=\"1.0\"?>\n");
    fprintf (f, "<?mso-application progid=\"Excel.Sheet\"?>\n");
    fprintf (f, "<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    fprintf (f, "xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n");
    fprintf (f, "xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n");
    fprintf (f, "xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    fprintf (f, "xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n");
    fprintf (f, "<ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\"> </ExcelWorkbook>\n");
    fprintf (f, "<Styles>\n");
    fprintf (f, "<Style ss:ID=\"proto\">\n");
    fprintf (f, "<Alignment ss:Vertical=\"Top\"/>\n");
    fprintf (f, "<Font ss:FontName=\"Courier New\" x:Family=\"Modern\" ss:Size=\"11\" ss:Bold=\"1\"/>\n");
    fprintf (f, "</Style>\n");
    fprintf (f, "<Style ss:ID=\"def\">\n");
    fprintf (f, "<Alignment ss:Vertical=\"Top\" ss:WrapText=\"1\"/>\n");
    fprintf (f, "<Font ss:FontName=\"Times New Roman\" x:Family=\"Roman\" ss:Size=\"11\"/>\n");
    fprintf (f, "</Style>\n");
    fprintf (f, "<Style ss:ID=\"desc\">\n");
    fprintf (f, "<Alignment ss:Vertical=\"Bottom\" ss:WrapText=\"1\"/>\n");
    fprintf (f, "<Font ss:FontName=\"Times New Roman\" x:Family=\"Roman\" ss:Size=\"11\" ss:Italic=\"1\"/>\n");
    fprintf (f, "</Style>\n");
    fprintf (f, "<Style ss:ID=\"param\">\n");
    fprintf (f, "<Alignment ss:Horizontal=\"Right\" ss:Vertical=\"Top\" ss:WrapText=\"1\" ss:Indent=\"1\"/>\n");
    fprintf (f, "<Font ss:FontName=\"Courier New\" x:Family=\"Modern\" ss:Size=\"11\"/>\n");
    fprintf (f, "</Style>\n");
    fprintf (f, "</Styles>\n");

    // table
    fprintf (f, "<Worksheet ss:Name=\"%s\">\n", filterfile);
    fprintf (f, "<Table ss:DefaultColumnWidth=\"60\" ss:DefaultRowHeight=\"15\">\n");
    fprintf (f, "<Column ss:AutoFitWidth=\"0\" ss:Width=\"99\"/>\n");
    fprintf (f, "<Column ss:AutoFitWidth=\"0\" ss:Width=\"522.75\"/>\n");

    filt = create_splitt (filter);

    man = mans;

    for (pt = proto_list; pt; pt = Getnext (pt)) {
    	function = (Parm *) Getdata (pt);
    	fname = (char *)Getdata (function);
    	function = Getnext (function);
    	return_type = (ApiType *) function;
    	fman = (functionman *) man->DATA;

    	if (fman != NULL) {
    		cfilt = create_splitt (fman->categ);

    		for (cl = filt; cl != NULL; cl = cl->NEXT) {
    			for (ch = cfilt; ch != NULL; ch = ch->NEXT) {
    				if (strcmp ((char *)ch->DATA, (char *)cl->DATA) == 0)
    					break;
    			}
    			if (ch == NULL)
    				break;
    		}
    		freechain (cfilt);

    		if (cl == NULL) {
    			fprintf (stdout, "Filtering XML page for `%s'\n", get_proto (fname, return_type, Getnext (function)));


                // prototype
    			prototype = fman->synop?fman->synop:get_proto (fname, return_type, Getnext (function));
    			xml_text (textbuf, prototype);
                fprintf (f, "<Row>\n");
                fprintf (f, "<Cell ss:StyleID=\"proto\"><Data ss:Type=\"String\">%s</Data></Cell>\n", prototype);
                fprintf (f, "<Cell ss:StyleID=\"def\"/>\n");
                fprintf (f, "</Row>\n");

                // description
    			description = fman->desc;
    			xml_text (textbuf, description);
                fprintf (f, "<Row>\n");
                fprintf (f, "<Cell ss:StyleID=\"def\"/>\n");
                fprintf (f, "<Cell ss:StyleID=\"desc\"><Data ss:Type=\"String\">%s</Data></Cell>\n", description);
                fprintf (f, "</Row>\n");


    			if ((args = fman->errors)) {
    				if (strcmp (args, "")) {
    					argtab = xml_text_args (args);
    					if (argtab) {
    						for (i = 0; argtab[i]; i += 2) {
                                fprintf (f, "<Row>\n");
                                fprintf (f, "<Cell ss:StyleID=\"param\"><Data ss:Type=\"String\">%s</Data></Cell>\n", argtab[i]);
                                fprintf (f, "<Cell ss:StyleID=\"def\"><Data ss:Type=\"String\">%s</Data></Cell>\n", argtab[i + 1]);
                                fprintf (f, "</Row>\n");
    						}
    						for (i = 0; argtab[i]; i++)
    							free (argtab[i]);
    					}
    				}
    			}
                fprintf (f, "<Row>");
                fprintf (f, "<Cell ss:StyleID=\"def\"/>");
                fprintf (f, "</Row>");
    		}
    	}
    	man = Getnext (man);
    }
    fprintf (f, "</Table>\n");
    fprintf (f, "</Worksheet>\n");
    fprintf (f, "</Workbook>\n");

    freechain (filt);
    fclose (f);
}

void GENIUS::create_filter_online (char *filter, char *online_path, Man *mans) 
{ 
    FILE *f;
    DIR  *dir;
    char *fname;
    char  buf[10000];
    chain_list *man, *filt, *cfilt, *cl, *ch, *pt;
    ParmList *parms;
    ApiType *return_type;
    functionman *fman;
    Parm *function;
    char **argtab;
    char *args;
    char *section;
    int i;

    if (!filter || !online_path || !mans) return;
    if ((dir = opendir(online_path)) == NULL) {
        fprintf (stderr, "[API WAR] Help directory '%s' does not exist\n", online_path);
        return;
    }
    closedir(dir);

    filt=create_splitt(filter);

    man=mans;

    for (pt = proto_list; pt; pt = Getnext (pt)) {
        function = (Parm*)Getdata (pt);
        fname = (char*)Getdata (function);
        function = Getnext (function);
        return_type = (ApiType*)function;
        function = Getnext (function);
        parms = (ParmList*)function;
        fman=(functionman *)man->DATA;

        if (fman) {
            cfilt = create_splitt(fman->categ);
            for (cl = filt; cl; cl = cl->NEXT) {
                for (ch = cfilt; ch; ch = ch->NEXT) {
                    if (strcmp((char *)ch->DATA, (char *)cl->DATA)==0) break;
                }
                if (ch==NULL) break;            
            }
            if (cfilt==NULL || (cl && !strcmp((char *)cfilt->DATA, (char *)cl->DATA))) {
                section="misc";
            }
            else section = (char *)cfilt->DATA;
            freechain(cfilt);

            if (cl==NULL || !strcmp(section, "misc")) {
                fprintf (stdout, "Filtering help page for `%s'\n", get_proto (fname, return_type, parms));
                sprintf(buf, "%s/%s", online_path, section);
                if ((dir = opendir(buf)) == NULL) {
                    if (mkdir(buf, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0) {
                        fprintf (stderr, "[API WAR] Cannot create help category '%s' \n", section);
                        return;
                    }
                }
                else closedir(dir);
                    
                sprintf(buf, "groff -man -Tascii | awk 'BEGIN { skip = 0 } /@@@BUILDHELP@@@/ { next } /^$/ { if (skip == 0) print $0 ; skip = 1 ; next } // { print $0 ; skip = 0 }' > %s/%s/%s", online_path, section, fname);
                if (!(f = popen (buf, "w"))) {
                    fprintf (stderr, "[API WAR] Can't open file %s for writing\n", fname);
                    fflush(stderr);
                    return;
                }
                fprintf (f, ".TH @@@BUILDHELP@@@\n");
                fprintf (f, ".SH \" \"\n");
                fprintf (f, "%s;\n", fman->synop?fman->synop:get_proto (fname, return_type, parms));
                // _____ description
                fprintf (f, ".SH DESCRIPTION\n");
                man_text (buf, fman->desc);
                fprintf (f, "%s\n", buf);
                // _____ arguments
                if ((args = fman->errors)) {
                    if (strcmp (args, "")) {
                        argtab = xml_text_args (args);
                        if (argtab) {
                            fprintf (f, ".SH ARGUMENTS\n");
                            for (i = 0; argtab[i]; i += 2) {
                                fprintf (f, ".TP 14\n.B %s\n", argtab[i]);
                                man_text (buf, argtab[i + 1]);
                                fprintf (f, "%s\n", buf);
                            }
                            for (i = 0; argtab[i]; i++) free (argtab[i]);
                        }
                    }
                }
                pclose (f);
            }
        }
        man = Getnext (man);
    }

    freechain(filt);
}

