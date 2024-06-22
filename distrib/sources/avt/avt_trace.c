/****************************************************************************/
/*                                                                          */
/*                      Chaine de verification                              */
/*                                                                          */
/*    Produit : AVT Version 1                                               */
/*    Fichier : avt_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1998-1999 AVERTEC                                       */
/*    Tous droits reserves                                                  */
/*    Support : e-mail support@avertec.com                                  */
/*                                                                          */
/*    Auteur(s) : AUGUSTINS Gilles                                          */
/*                                                                          */
/****************************************************************************/

#include "avt_headers.h"

#define BG_NONE  ""
#define FG_NONE  ""

#define CL_RESET ""        // Reset All Attributes (return to normal mode)
#define CL_BOLD         "1"        // Bright (Usually turns on BOLD)
#define CL_DIM         "2"         // Dim
#define CL_UDL          "3"        // Underline
#define CL_BLINK "5"        // Blink
#define CL_REV   "7"         // Reverse
//        8        Hidden

#define FG_BLACK        "30"        // Black
#define FG_RED                "31"        // Red
#define FG_GREEN        "32"        // Green
#define FG_YELLOW        "33"        // Yellow
#define FG_BLUE         "34"        // Blue
#define FG_MAGENTA        "35"        // Magenta
#define FG_CYAN         "36"        // Cyan
#define FG_WHITE        "37"        // White

#define BG_BLACK        "40"        // Black
#define BG_RED                "41"        // Red
#define BG_GREEN        "42"        // Green
#define BG_YELLOW        "43"        // Yellow
#define BG_BLUE         "44"        // Blue
#define BG_MAGENTA        "45"        // Magenta
#define BG_CYAN         "46"        // Cyan
#define BG_WHITE        "47"        // White


#define COLOR(a) "\x1B[" a "m"

char *colors[]=
  {
    COLOR(FG_BLACK),
    COLOR(FG_MAGENTA),
    COLOR(FG_BLUE),
    COLOR(FG_CYAN),
    COLOR(FG_YELLOW),
    COLOR(FG_WHITE),
    COLOR(FG_RED),
    COLOR(FG_GREEN)
  };

#define NBCOLORS ((signed)(sizeof(colors)/sizeof(*colors)))

char *leveltab[]=
  {
    "",
    "    ",
    "        ",
    "            ",
    "                ",
    "                    ",
    "                        "
  };

void readusercolor()
{
  unsigned int i;
  char temp[100], *e;
  for (i=0;(unsigned)i<sizeof(colors)/sizeof(*colors);i++)
    {
      sprintf(temp,"AVT_COL_%d",i);
      e=avt_gethashvar(temp);
      if (e!=NULL)
        {
          strcpy(temp,"\x1B[");
          switch(*e)
            {
            case '+': strcat(temp,CL_BOLD); strcat(temp,";"); e++; break;
            case '-': strcat(temp,CL_DIM); strcat(temp,";"); e++; break;
            default: break;
            }
          switch(*e)
            {
            case 'B': strcat(temp,FG_BLACK); break;
            case 'r': strcat(temp,FG_RED); break;
            case 'g': strcat(temp,FG_GREEN); break;
            case 'y': strcat(temp,FG_YELLOW); break;
            case 'b': strcat(temp,FG_BLUE); break;
            case 'm': strcat(temp,FG_MAGENTA); break;
            case 'c': strcat(temp,FG_CYAN); break;
            case 'w': strcat(temp,FG_WHITE); break;
            default: break;
            }
          strcat(temp,"m");
          colors[i]=strdup(temp);
        }
    }

}

void avt_error (char *lib, int code, int severity, char *fmt, ...)
{
    va_list pa;
    char *typemsg;
    char buf[8192], error_msg[8192];
    int car;

    va_start (pa, fmt);
    vsprintf(buf, fmt, pa);
    va_end(pa);

    switch(severity)
      {
      case AVT_ERR: typemsg="¤6¤+Error"; break;
      case AVT_WAR: typemsg="¤4Warning"; break;        
      case AVT_INFO:
      default:
        typemsg="¤1Info";
      }
    if (code<=0)
      sprintf (error_msg, "[%s¤.][¤+%s¤.] ", typemsg, lib);
    else
      sprintf (error_msg, "[%s #%d¤.][¤+%s¤.] ", typemsg, code, lib);

    car = strlen (error_msg);

    strcpy(&error_msg[car], buf);
    avt_fprintf (stderr, "%s", error_msg);
}

void avt_trace (int level, FILE *output, char *fmt, ...) 
{
    va_list pa;
    int color = 0;
    static int avt_color = 0;
    static int color_env = 1;
    char *str = NULL;

    if (color_env) {
        if ((str = avt_gethashvar ("AVT_COL")))
            if (!strcmp (str, "yes")) {
                avt_color = 1;
                        readusercolor();
                }
        color_env = 0;
    }

    if (isatty (fileno(output)))
        if (avt_color)
            color = 1;
    
    if (color) {
            if (level>=0 && (unsigned)level<sizeof(colors)/sizeof(*colors))
                fprintf (output, "%s%s",colors[level],leveltab[level]);
            else
                fprintf (output, "%s",leveltab[0]);
        }
    else
        fprintf (output, "%s",level>=0?leveltab[level]:"");
    
    va_start (pa, fmt);
    vfprintf(output, fmt, pa);
    va_end(pa);

    if (color)
        fputs(COLOR(CL_RESET), output);
}

void avt_trace_va (int level, FILE *output, char *fmt, va_list pa) 
{
    int color = 0;
    static int avt_color = 0;
    static int color_env = 1;
    char *str = NULL;

    if (color_env) {
        if ((str = avt_gethashvar ("AVT_COL")))
            if (!strcmp (str, "yes"))
              {
                avt_color = 1;
                readusercolor();
              }
        color_env = 0;
    }

    if (isatty (fileno(output)))
        if (avt_color)
            color = 1;
    
    if (color)
      {
        if (level>=0 && (unsigned)level<sizeof(colors)/sizeof(*colors))
          fprintf (output, "%s%s",colors[level],leveltab[level]);
        else
          fprintf (output, "%s",leveltab[0]);
      }
    else
      fprintf (output, "%s",level>=0?leveltab[level]:"");

    vfprintf(output, fmt, pa);

    if (color)
      fputs(COLOR(CL_RESET), output);
}

int avt_terminal(FILE *output) 
{
  if (isatty (fileno(output)))
    return 1;
  return 0;
}

void avt_back_fprintf(FILE *output, int length) 
{
  if (avt_terminal(output))
    {
      int i;
      for (i=1; i<=length;i++)
        fprintf (output, "\b");
    }
}

void avt_fprintf(FILE *output, char *fmt, ...) 
{
    va_list pa;
    int color = 0, ln;
    static int avt_color = 0;
    static int color_env = 1;
    char *str = NULL, *e;
    char temp[4096];

    if (color_env) {
            if (V_BOOL_TAB[__AVT_COLOR].VALUE)
              {
                avt_color = 1;
                readusercolor();
              }
        color_env = 0;
    }

    if (avt_terminal(output))
        if (avt_color)
            color = 1;
 
    va_start (pa, fmt);
    vsprintf(temp, fmt, pa);
    va_end(pa);

    str=temp;
    e=strchr(str,'¤');
    while (e!=NULL)
      {
        if (*(e+1)>='0' && *(e+1)<='0'+NBCOLORS-1)
          {
            *e='\0'; 
            fputs(str, output);
            if (color) fputs(colors[*(e+1)-'0'], output);
            str=e+2;
          }
        else
          switch(*(e+1))
          {
          case '+':
            *e='\0'; 
            fputs(str, output);
            if (color) fputs(COLOR(CL_BOLD), output);
            str=e+2;
            break;
          case '-':
            *e='\0'; 
            fputs(str, output);
            if (color) fputs(COLOR(CL_DIM), output);
            str=e+2;
            break;
          case '.':
            *e='\0'; 
            fputs(str, output);
            if (color) fputs(COLOR(CL_RESET), output);
            str=e+2;
            break;
          case '_':
            *e='\0'; 
            fputs(str, output);
            if (color) fputs(COLOR(CL_UDL), output);
            str=e+2;
            break;
          case '~':
            *e='\0'; 
            fputs(str, output);
            if (color) fputs(COLOR(CL_REV), output);
            str=e+2;
            break;
          default:
            *e='\0';
            fprintf(output,"%s¤",str);
            str=e+1;
          }
        e=strchr(str,'¤');
      }

    fputs (str, output);

    if (color && (ln=strlen(str))>0 && str[ln-1]=='\n')
      fputs(COLOR(CL_RESET), output);
}

int avt_text_real_length(char *buf)
{
  int cnt=0;
  while (*buf!='\0')
    {
      if (*buf=='¤' && *(buf+1)!='\0') cnt-=1;
      else cnt++;
      buf++;
    }
  return cnt;
}

void avt_format_text(char *resbuf, char *origbuf, int decal, int max)
{
  int i=0, j=0, k, cnt=decal;
  
  while (origbuf[i]!='\0')
    {
      resbuf[j++]=origbuf[i];
      if (origbuf[i]=='¤' && origbuf[i+1]!='\0') cnt-=1;
      else cnt++;
      i++;
      if (origbuf[i]=='\n' || cnt>=max)
        {
          resbuf[j++]='\n';
          if (origbuf[i]=='\n') i++;
          if (origbuf[i]!='\0') for (k=0; k<decal; k++) resbuf[j++]=' ';
          cnt=decal;
        }      
    }
  resbuf[j++]='\0';
}
