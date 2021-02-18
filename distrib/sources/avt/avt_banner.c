/****************************************************************************/
/*                                                                          */
/*                      Chaine de verification                              */
/*                                                                          */
/*    Produit : AVT Version 1                                               */
/*    Fichier : avt_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1998-1999 AVERTEC                                       */
/*    Tous droits reserves                                                  */
/*    Support : e-mail Karim.Dioury@asim.lip6.fr                            */
/*                                                                          */
/*    Auteur(s) : DIOURY Karim                                              */
/*                                                                          */
/****************************************************************************/

#include <unistd.h>
#include <sys/utsname.h>
#include <time.h>
#include <pwd.h>
#include <ctype.h>
#include "avt_lib.h"
#include "avt_banner.h"

#define _H_LINE_SIZE_ 72

#ifdef Solaris
extern char *ctime_r(const time_t *clock, char *buf, int buflen);
#endif

/*}}}************************************************************************/
/*{{{                    avt_getusername()                                  */
/*                                                                          */
/* get user name and put it in the buffer buf                               */
/*                                                                          */
/****************************************************************************/
char *avt_getusername(char *buf)
{
  struct passwd *userpasswd;

  userpasswd = getpwuid(geteuid());
  if (userpasswd)
    strncpy(buf,userpasswd->pw_name,32);
  else
    strncpy(buf,"AVT_unknown_user",32);

  return buf;
}

/****************************************************************************/
/*{{{                    fill_line()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
fill_line(char *buf, void (*prn)(void *,...), void *file,
          char *flagon, char *txt, char *flagoff, char fillchar, ...)
{
  int        i, j;
  va_list    arg;

  j             = 0;
  
  // comment on
  for (i = 0; flagon[i] != '\0'; i++)
    buf[j++]    = flagon[i];
  
  // get the text
  if (txt)
  {
    for (i = 0; i < 2; i ++)
      buf[j++]  = fillchar;
    
    va_start(arg,fillchar);
    
    j          += vsprintf(buf+j,txt,arg);
    va_end(arg);
  }
  
  // comment off if it exists
  if (flagoff[0] != '\0')
  {
    for (i = j; i < _H_LINE_SIZE_; i ++)
      buf[j++]  = fillchar;
    
    for (i = 0; flagoff[i] != '\0'; i ++)
      buf[j++]  = flagoff[i];
  }
  
  buf[j++]      = '\n';
  buf[j++]      = '\0';

  prn(file,buf);
}


/*}}}************************************************************************/
/*{{{                    printExecInfoCustom()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
printExecInfoCustom(void *file, char *fon, char *txt, char *foff,
                    void (*prn)(void *, ...), int mode)
{
  time_t             clock;
  struct utsname     name;
  char               buf[1024];
  char               tmp[1024];
  char              *space;
  int                i, j, done;
  int                la, lb;

  uname(&name);
  time(&clock);

  if (mode)
  {
    la      = strlen(fon);
    lb      = strlen(foff);

    if (la == lb && fon[la-1] == foff[0])
      fill_line(tmp,prn,file,fon,"",foff,foff[0]);
  }
    
  fill_line(tmp,prn,file,fon,"",foff,' ');
  fill_line(tmp,prn,file,fon,"Avertec Release v%s%s (%d bits on %s %s)", foff,' ',AVT_VERSION,PATCH_NUM,8*sizeof(void *),name.sysname,name.release);

  mbk_whoAmILong(getpid(),buf);
  space     = strchr(buf,' ');
  if (space)
    space[0]= '\0';
#ifdef AVT_MORE_INFOS
#warning mode verbose header
  fill_line(tmp,prn,file,fon,"[AVT_only] host: %s",foff,' ',name.nodename);
  fill_line(tmp,prn,file,fon,"[AVT_only] arch: %s",foff,' ',name.machine);
  fill_line(tmp,prn,file,fon,"[AVT_only] path: %s",foff,' ',buf);
#endif
  if (space)
    fill_line(tmp,prn,file,fon,"argv: %s",foff,' ',space+1);
  fill_line(tmp,prn,file,fon,"",foff,' ');
  
  // user
  avt_getusername(buf);
  fill_line(tmp,prn,file,fon,"User: %s",foff,' ',buf);

#ifdef Solaris
  ctime_r(&clock,buf,52);
#else
  ctime_r(&clock,buf);
#endif
  
  buf[strlen(buf)-1]    = '\0';
  fill_line(tmp,prn,file,fon,"Generation date %s",foff,' ',buf);
  fill_line(tmp,prn,file,fon,"",foff,' ');
  
  if (txt && txt[0] != '\0')
  {
    // skip '\n'
    done            = 0;
    for (i = 0, j = 0; !done; i ++)
      /*                                  this is not an error */
      if (txt[i] == '\n' || (txt[i] == '\0' && (done = 1) ))
      {
        buf[j]      = '\0';
        j           = 0;
        fill_line(tmp,prn,file,fon,buf,foff,' ');
      }
      else
        buf[j++]    = txt[i];
  }

  if (mode && la == lb && fon[la-1] == foff[0])
    fill_line(tmp,prn,file,fon,"",foff,foff[0]);

  prn(file,"\n");
}

/*}}}************************************************************************/
/*{{{                    avt_printExecInfo()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
avt_printExecInfo(FILE *fd, char *fon, char *txt, char *foff)
{
  printExecInfoCustom((void*)fd,fon,txt,foff,(void(*)(void*,...))fprintf,0);
}

/*}}}************************************************************************/
/*{{{                    avt_printExecInfo()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
avt_printExecInfoFlourish(FILE *fd, char *fon, char *txt, char *foff)
{
  printExecInfoCustom((void*)fd,fon,txt,foff,(void(*)(void*,...))fprintf,1);
}

/*}}}************************************************************************/
/*{{{                    avt_printExecInfoCustom()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
avt_printExecInfoCustom(void *file, char *fon, char *txt, char *foff,
                        void (*prn)(void *, ...))
{
  printExecInfoCustom(file,fon,txt,foff,prn,0);
}

/*}}}************************************************************************/
/*                        function avt_indx()                                */
/* parametres :                                                              */
/* c : caractere                                                             */
/*                                                                           */
/* renvoie le caractere dans le tableau des polices                          */
/*****************************************************************************/

static int avt_indx (char c)
{
   return c >= '0' && c <= '9' ? (int)c - '0'
                              : isupper((int)c) ? (int)10 + c - 'A'
                              : islower((int)c) ? (int)11 + 'Z' - 'A' + c - 'a'
                              : -1;
}

/*****************************************************************************/
/*                        function avt_bannerhead()                          */
/* parametres :                                                              */
/* s : nom de l'outil                                                        */
/* police : police utilisee                                                  */
/* nl : number of lines                                                      */
/*                                                                           */
/* genere les informations  les outils avertec                               */
/*****************************************************************************/

static void avt_bannerhead (char *s, char *police[][62], int nl)
{
    
    int i, j, k, l, m;
    char *line;
    int color = 0;
    int funny = 0;

    if (isatty (1))
        if (AVT_COL)
            color = 1;

   /* rince off :
      the buffer is filled with nul characteres. */
   for (j = 0; j < nl; j++)
      for (i = 0; i < WINDOW_SIZE; i++)
         screen[j][i] = '\0';
   /* first :
      filling the buffer with direct table output. */
   while (*s) {
      for (i = 0; i < nl; i++) {
         if ((j = avt_indx(*s)) == -1) {
            avt_errmsg(AVT_ERRMSG,"001",AVT_INTERNAL);
            EXIT(1);
         }
         line = police[j][i];
         if (strlen(line) + strlen(screen[i]) >= WINDOW_SIZE) {
            avt_errmsg(AVT_ERRMSG,"002",AVT_INTERNAL,WINDOW_SIZE - 1);
            EXIT(1);
         }
         strcat(screen[i], line);
         if (*(s + 1) != '\0')
            strcat(screen[i], " ");
      }
      s++;
   }
   for (m = l = -1, j = 0; j < nl; j++)
      for (i = 0; i < WINDOW_SIZE; i++)
         if (screen[j][i] == '@') {
            if (m == -1)
               m = j;
            l = j;
            break;
         }
   k = strlen(screen[0]);
   /* banner :
      output on stdout. */
   if (color)
      fprintf (stdout, "\x1B[1m");

   putc('\n', stdout);
   for (j = m; j <= l; j++) {
      for (i = 0; i < (WINDOW_SIZE - k) / 2; i++)
         putc(' ', stdout);
      for (i = 0; i < k; i++) {
         if (funny)
            switch ((i + j) % 6 + 1) {
               case 1:
                  fprintf (stdout, "\x1B[31m");
                  break;
               case 2:
                  fprintf (stdout, "\x1B[32m");
                  break;
               case 3:
                  fprintf (stdout, "\x1B[33m");
                  break;
               case 4:
                  fprintf (stdout, "\x1B[34m");
                  break;
               case 5:
                  fprintf (stdout, "\x1B[35m");
                  break;
               case 6:
                  fprintf (stdout, "\x1B[36m");
                  break;
               case 7:
                  fprintf (stdout, "\x1B[37m");
                  break;
           }
           putc(screen[j][i], stdout);
      }
      putc('\n', stdout);
   }
   if (color)
      fprintf (stdout, "\x1B[0m");
}

/*****************************************************************************/
/*                        function avt_cartouche()                           */
/* parametres :                                                              */
/* tool : outil d'avertec                                                    */
/* tv : version de l'outil                                                   */
/* date : date de l'outil                                                    */
/* av : version des outils avertec                                           */
/*                                                                           */
/* genere les informations  les outils avertec                               */
/*****************************************************************************/
static void avt_cartouche (char *tool, char *date, char *av, char *pn, char *date_a, int bit)
{
    int i, j, k, l;
#ifdef AVT_EVAL
    static char *msg[4] = {
        "AVERTEC Release v%s%s (%dbit) - Evaluation version",
        "Copyright (c)%s-%d, \"All Rights Reserved",
        "E-mail: support@avertec.com",
        "%s"
    };
#else
    static char *msg[4] = {
        "AVERTEC Release v%s%s (%dbit)",
        "Copyright (c)%s-%d, \"All Rights Reserved",
        "E-mail: support@avertec.com",
        "%s"
    };
#endif

    int msgl[5];
    char *str;
    time_t timer;
    char day[4], month[4];
    int year, nday, hour, minute, second;
    char *date_lang;

   time(&timer);
   sscanf(ctime(&timer), "%3s %3s %d %d:%d:%d %d", day, month, &nday, &hour, &minute, &second, &year);

   date_lang = date_a ;

   /* rince off :
      the buffer is filled with nul characteres. */
   for (j = 0; j < 10; j++)
      for (i = 0; i < WINDOW_SIZE; i++)
         screen[j][i] = '\0';
   i = strlen(tool);
   str = mbkstrdup(tool);
   for (k = 0; k < i; k++)
      str[k] = isupper((int)tool[k]) ? tolower(tool[k]) : tool[k];
   sprintf(screen[0], msg[0], av, pn, bit);
   mbkfree(str);
   sprintf(screen[1], msg[1], date, year);
   strcat(screen[2], msg[2]);
   sprintf(screen[3], msg[3],date_lang);
   
   for (i = 1; i < 4; i++) {
      msgl[i] = strlen(screen[i]);
      j = j < msgl[i] ? msgl[i] : j;
   }
   for (i = 1; i < 4; i++)
      for (l = 0, k = 0; k < WINDOW_SIZE; k++) {
         if (screen[i][k] == '\0') {
            screen[i + 4][k + l] = '\0';
            break;
         }
         if (screen[i][k] == '"') { /* only once per line */
            for (; l <= j - msgl[i]; l++)
               screen[i + 4][k + l] = ' ';
            continue;
         }
         screen[i + 4][k + l] = screen[i][k];
      }
   /* cartouche :
      output on stdout. */
   i = strlen(screen[0]);
   putc('\n', stdout);
   for (k = 0; k < (WINDOW_SIZE - i) / 2; k++) putc(' ', stdout);
   puts(screen[0]);

   putc('\n', stdout);
   for (i = 1; i < 4; i++) {
      for (k = 0; k < (WINDOW_SIZE - j) / 2; k++)
         putc(' ', stdout);
      for (k = 0; k <= j; k++)
         if (screen[i + 4][k] != 0) /* not so nice, but */
            putc(screen[i + 4][k], stdout);
      putc('\n', stdout);
   }
   putc('\n', stdout);
}

/*****************************************************************************/
/*                        function avt_banner()                              */
/* parametres :                                                              */
/* tool : outil d'avertec                                                    */
/* tv : version de l'outil                                                   */
/* date : date de l'outil                                                    */
/* av : version des outils avertec                                           */
/*                                                                           */
/* genere un banniere pour avertec                                           */
/*****************************************************************************/

void avt_banner (char *tool, char *comment, char *date)
{

    avt_initerrmsg(tool);
    
    avt_bannerhead(tool, Unknown_Bold_Normal_14, 15);
    avt_cartouche(tool, date, AVT_VERSION, PATCH_NUM, AVT_DATE_A, 8*(int)sizeof(void *));
    if (AVERTEC_VERSION==NULL)
    {
      AVERTEC_VERSION = mbkalloc(strlen(AVT_VERSION) + 1) ;
      strcpy(AVERTEC_VERSION,AVT_VERSION) ;
    }
}
