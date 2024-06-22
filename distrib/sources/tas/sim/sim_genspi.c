/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : SIM Version 1                                               */
/*    Fichier : sim_genspi.c                                                */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
#define NEWELDO

#include <semaphore.h>

#include SIM_H
#include "sim.h"

int sim_ftoi (double f)
{
    long l;
    long d;

    d = (long)(f * 10) - (long)f *10;
    if (d <= 5)
        l = (long)f;
    if (d > 5 && f > 0)
        l = (long)f + 1;
    if (d > 5 && f < 0)
        l = (long)f - 1;
    if (f == 0.0)
        l = (long)0;

    return l;
}

double sim_calcdelayslope(tab,tabsize,begin,seuil,sfl,
                          sfh,last,delay,slope,type,
                          step,indice,transition)
double *tab ;
int tabsize ;
int begin ;
double seuil ;
double sfl ;
double sfh ;
double last ;
double *delay ;
double *slope ;
int *type ;
double step;
int  *indice;
char *transition;
{
 double vx    = 0.0;
 double vb    = 0.0;
 double time  = 0.0;
 double timeb = 0.0;
 double timel = 0.0;
 double timeh = 0.0;
 int j ;
 int sfl_flag = 0 ; 
 int sfh_flag = 0 ; 
 char flag ='N' ;

 *slope = 0.0;
 if (begin == 0)
     time = step;
 else
     time = begin * step ;
 vb = tab[begin] ;
 begin++ ;
 timeb = time - step;
 for(j = begin ; j < tabsize ; j++)
  {
   vx = tab[j] ;
   if ( !sfl_flag && vx <= sfl)
     sfl_flag = 1;
   if ( !sfh_flag && vx >= sfh)
     sfh_flag = 1;
   if((vb <= seuil) && (vx >= seuil) && (flag == 'N'))
     {
      if(last > 0.0)
       {
        *delay = (seuil-vb)/(vx-vb)*(time-timeb)+timeb ;
        *type = 1 ;
       }
      if (indice != NULL)
          *indice = j;
      if (transition != NULL)
       {
        if (tab[j-1] < tab[j])
            *transition = 'U';
        else
            *transition = 'D';
       }
      last = (seuil-vb)/(vx-vb)*(time-timeb)+timeb ;
      flag = 'Y' ;
     }
   else if((vb >= seuil) && (vx <= seuil) && (flag == 'N'))
     {
      if(last > 0)
       {
        *delay = (seuil-vb)/(vx-vb)*(time-timeb)+timeb ;
        *type = 0 ;
       }
      if (indice != NULL)
          *indice = j;
      if (transition != NULL)
       {
        if (tab[j-1] < tab[j])
            *transition = 'U';
        else
            *transition = 'D';
       }
      last = (seuil-vb)/(vx-vb)*(time-timeb)+timeb ;
      flag = 'Y' ;
     }
   if(((vb <= sfl) && (vx >= sfl)) ||
      ((vb >= sfl) && (vx <= sfl)))
     {
      //timel = time ;
      timel = (sfl-vb)/(vx-vb)*(time-timeb)+timeb ;
      if ( sfl_flag && sfh_flag )
       {
        if(((vb >= sfh) && (vx <= sfh)) ||
           ((vb <= sfh) && (vx >= sfh)))
          *slope = step;
        else
          *slope = timel - timeh ;
       }
     }
   else if(((vb >= sfh) && (vx <= sfh)) ||
           ((vb <= sfh) && (vx >= sfh)))
     {
      //timeh = time ;
      timeh = (sfh-vb)/(vx-vb)*(time-timeb)+timeb ;
      if ( sfl_flag && sfh_flag ) 
       {
        if(((vb <= sfl) && (vx >= sfl)) ||
           ((vb >= sfl) && (vx <= sfl)))
          *slope = step;
        else
          *slope = timeh - timel ;
       }
     }
   vb = vx ;
   timeb = time ;
   time += step ;
  }
 return(last) ;
}

static inline double __interpole(double ax, double ay, double bx, double by, double cy)
{
  return ax+(bx-ax)*(cy-ay)/(by-ay);
}
// peut etre ameliore mais je laisse tel quel pour plus de clarte
int sim_getallslopes(double *tabx, double *taby, int tabsize, double seuil, double sfl,
                     double sfh, sim_slopes *slopes)
{
 double timel;
 double timeh, time;
 int j, i=0;
 int has_low=0, has_high=0, has_mid= 0 ; 
 
 for(j = 1 ; j < tabsize ; j++)
  {
    if (taby[j-1]<taby[j])
    {
      if (taby[j-1]<sfl && sfl<=taby[j]) { timel=__interpole(tabx[j-1], taby[j-1], tabx[j], taby[j], sfl); has_low=1; }
      if (taby[j-1]<sfh && sfh<=taby[j]) { timeh=__interpole(tabx[j-1], taby[j-1], tabx[j], taby[j], sfh); has_high=1; }
      if (taby[j-1]<seuil && seuil<=taby[j]) { time=__interpole(tabx[j-1], taby[j-1], tabx[j], taby[j], seuil); has_mid=1; }
    }
    else if (taby[j-1]>taby[j])
    {
      if (taby[j-1]>sfl && sfl>=taby[j]) { timel=__interpole(tabx[j-1], taby[j-1], tabx[j], taby[j], sfl); has_low=1; }
      if (taby[j-1]>sfh && sfh>=taby[j]) { timeh=__interpole(tabx[j-1], taby[j-1], tabx[j], taby[j], sfh); has_high=1; }
      if (taby[j-1]>seuil && seuil>=taby[j]) { time=__interpole(tabx[j-1], taby[j-1], tabx[j], taby[j], seuil); has_mid=1; }
    }
    if (has_low && has_mid && has_high)
    {
      slopes[i].time=time;
      slopes[i].direction=timel<timeh?'u':'d';
      slopes[i].time_high=timeh;
      slopes[i].time_low=timel;
      has_low=has_mid=has_high=0;
      i++;
    }
  }
 return i;
}

/*-------------------------------------------------------------*/
/* FUNCTION : sim_GetDelaySlopeByIndex                         */
/*                                                             */
/* Retourne  : - le prochain temps de passage a vdd/2          */
/*               a partir de l indice de depart du tableau     */
/*             - le slope correspondant                        */
/*             - l'indice du tablo correspondant a ce tps      */
/*             - la nature de la transition du front.          */
/*                                                             */
/*-------------------------------------------------------------*/

void sim_GetDelaySlopeByIndex (tab,delay,slope,vthr,vsthrl,vsthrh,time,step,indice,transition)
    double     *tab;   /* tableaux des valeurs spice d une variable */
    double     *delay;
    double     *slope;
    double      vthr;
    double      vsthrl;
    double      vsthrh;
    double      time;
    double      step;
    int        *indice; // indice du tablo pour lekel on a atteind vdd/2
    char       *transition;
{
    int      nby;
    int      type;
    double   d,f ;
    
    nby = sim_ftoi(time/step) ;
    nby++ ;
    d = 0.0 ;
    f = 0.0 ;
    *delay = sim_calcdelayslope(tab,nby,*indice,vthr,vsthrl,vsthrh,0.0,
                                &d,&f,&type,step,indice,transition) ;
    *slope = f ;
}

char *sim_getnode (char *line, char *node)
{
    const char *separ = "()# ";
    char *buffer;
    char *s, *pt;

    buffer = mbkstrdup (line);
    s = strtok (buffer, separ);
    while (s) {
        if (!strcasecmp (s, node)) {
            pt = s - buffer + line;
            mbkfree (buffer);
            return pt;
        }
        s = strtok (NULL, separ);
    }
    mbkfree (buffer);
    return NULL;
}

/* ---------------------------------------------------------------------------- */

double sim_getspidouble (char *param, char **ptres)
{
    double mul = 1.0;
    char *pt;
    char c = 0;
    int status = 0;

    if (strlen (param) == 0) {
        *ptres = param;
        return (0.0);
    }

    for (pt = param + (strlen (param) - 1); pt != param; pt--) {
        status = 1;
        if (isalpha ((int)(*pt)) == 0) {
            pt++;
            break;
        }
        c = *pt;
    }

    if (status == 0) {
        pt++;
        c = *pt;
    }

    switch (c) {
    case 'g':
        mul = 1.0e+9;
        break;
    case 'k':
        mul = 1.0e+3;
        break;
    case 'm':
        mul = 1.0e-3;
        break;
    case 'u':
        mul = 1.0e-6;
        break;
    case 'n':
        mul = 1.0e-9;
        break;
    case 'p':
        mul = 1.0e-12;
        break;
    case 'f':
        mul = 1.0e-15;
        break;
    }

    *pt = '\0';
    mul = mul * strtod (param, ptres);
    *pt = c;

    if (*ptres != pt) {
        *ptres = param;
    }

    return (mul);
}

/* ---------------------------------------------------------------------------- */

int sim_getspiparam (char *str, char *name, char *param)
{
    char *pt;
    char *ptx;
    int res = 0;
    int eq = 0;
    char c;

    *name = '\0';
    *param = '\0';
    pt = str;
    while ((isspace ((int)(*pt)) != 0) && (*pt != '\0')) {
        pt++;
        res++;
    }
    if (*pt == '\0')
        return (0);
    else {
        ptx = pt;
        while ((isspace ((int)(*ptx)) == 0) && (*ptx != '\0') && (*ptx != '=')) {
            ptx++;
            res++;
        }
        if (*ptx == '=') {
            eq = 1;
        }
        c = *ptx;
        *ptx = '\0';
        strcpy (name, pt);
        *ptx = c;
        while ((isspace ((int)(*ptx)) != 0) && (*ptx != '\0') && (*ptx != '=')) {
            ptx++;
            res++;
        }
        if (*ptx == '=') {
            ptx++;
            res++;
            while ((isspace ((int)(*ptx)) != 0) && (*ptx != '\0')) {
                ptx++;
                res++;
            }
            eq = 1;
            pt = ptx;
        }
        if (eq != 1) {
            return (res);
        }
        while ((isspace ((int)(*pt)) != 0) && (*pt != '\0')) {
            pt++;
            res++;
        }
        ptx = pt;
        while ((isspace ((int)(*ptx)) == 0) && (*ptx != '\0')) {
            ptx++;
            res++;
        }
        c = *ptx;
        *ptx = '\0';
        strcpy (param, pt);
        *ptx = c;
    }

    return (res);
}

/* ---------------------------------------------------------------------------- */

char *sim_getspiline (FILE *file, char *str)
{
    static char buf[16000];
    static int status = 0;
    char *pt;
    char *ptx;
    char c;

    if (status == 0) {
        if (fgets (buf, 2047, file) == NULL) {
            return (NULL);
        }
    }

    ptx = buf;
    while ((isspace ((int)*ptx) != 0) && (*ptx != '\0'))
        ptx++;

    status = 1;

    pt = str;

    while (status == 1) {
        while (*ptx == '*') {
            if (fgets (buf, 2047, file) == NULL) {
                status = 0;
                break;
            }
            ptx = buf;
            while ((isspace ((int)*ptx) != 0) && (*ptx != '\0'))
                ptx++;
        }
        if (*ptx == '+')
            *ptx = ' ';
        strcpy (pt, ptx);
        ptx = strchr (pt, (int)('\n'));
        if (ptx != NULL)
            *ptx = '\0';
        pt += strlen (pt);
        if (fgets (buf, 2047, file) == NULL) {
            status = 0;
            break;
        }
        ptx = buf;
        while ((isspace ((int)*ptx) != 0) && (*ptx != '\0'))
            ptx++;
        while (*ptx == '*') {
            if (fgets (buf, 2047, file) == NULL) {
                status = 0;
                break;
            }
            ptx = buf;
            while ((isspace ((int)*ptx) != 0) && (*ptx != '\0'))
                ptx++;
        }
        if ((*ptx != '+'))
            status = 2;
    }

    if ((status == 0) && (pt == str))
        return (NULL);

    for (pt = str; *pt != '\0'; pt++) {
        c = *pt;
        *pt = isupper ((int)c) ? tolower (c) : c;
    }

    return (str);
}

/* ---------------------------------------------------------------------------- */

void sim_fclose (FILE *file, char *filename)
{
    if (fclose (file)) {
        avt_errmsg( SIM_ERRMSG, "002", AVT_FATAL, filename);
//        fprintf (stderr, "\nsim error: can't close file %s\n", filename);
        EXIT (1);
    }
}

/* ---------------------------------------------------------------------------- */

void sim_readspifiletab (char *fileout, char *argv[], int nbx, int nby, double **tab, double limit, double simstep)
{
  switch( V_INT_TAB[__SIM_TOOL].VALUE ) {
      case SIM_TOOL_TITAN :
      case SIM_TOOL_TITAN7 :
        sim_parse_titan_ppr( fileout, 
                             argv, 
                             nbx, 
                             nby, 
                             tab, 
                             limit, 
                             simstep
                           );
        break;
#ifdef NEWELDO
      case SIM_TOOL_ELDO :        
        sim_parse_eldo_chi(fileout,argv,nbx,nby,tab,limit,simstep);
        break;
#endif
      default :
        sim_readspifile( fileout, 
                         argv, 
                         nbx, 
                         nby, 
                         tab, 
                         limit, 
                         simstep
                       );
        break;
      }
}

/* ---------------------------------------------------------------------------- */

void sim_readspifile (char *fileout, char *argv[], int nbx, int nby, double **tab, double limit, double simstep)
{
    FILE *file;
    char *pt;
    char *ptold;
    char *ptres;
    char param[256];
    char paramx[256];
    char buf[16000];
    double db;
    double step;
    int x;
    int fdx;
    int y;
    int i;
    int j;
    int one;
    int pos;
    int posx;
    int *flag;
    int status;
    int count = 0;
    int debug = 0;
    char *str;
    int line ;

    if(( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN) || (V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7)){
      sim_parse_titan_ppr(fileout,argv,nbx,nby,tab,limit,simstep) ;
      return ;
    }
#ifdef NEWELDO
    if( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_ELDO) {
      sim_parse_eldo_chi(fileout,argv,nbx,nby,tab,limit,simstep);
      return ;
    }
#endif
    if( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_NGSPICE ) {
      sim_parse_ngspice_stdout(fileout,argv,nbx,nby,tab,limit);
      return ;
    }

    if( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_HSPICE ) {
      sim_parse_hspice(fileout,argv,nbx,nby,tab);
      return ;
    }

    str = avt_gethashvar ("SIM_DEBUG_MODE");
    if (str != NULL && !strcasecmp (str,"yes"))
        debug = 1;
    if ((file = fopen (fileout, "r")) == NULL) {
        avt_errmsg( SIM_ERRMSG, "001", AVT_FATAL, fileout);
//        fprintf (stderr, "\nsim error: can't open file %s\n", fileout);
        EXIT (1);
    }

    flag = (int *)mbkalloc ((nbx + 1) * sizeof (int));

    for (x = 0; x < nbx; x++)
        flag[x] = 0;
    flag[nbx] = 0;

    y = 0;
    step = 0.0;
    fdx = 0;

    line = 0 ;
    
    while (1) {
        line++ ;

        if (debug) {
            count++;
            fprintf (stdout, "\n\n-- [readspifile] --\n");
            fprintf (stdout, " count = %d\n", count);
            fprintf (stdout, " buf   = %s\n", buf);
            for (i = 0; i < nbx; i++)
                fprintf (stdout, " flag[%s] = %d\n", argv[i], flag[i]);
            fprintf (stdout, " status = %d\n", status);
        }
        if (sim_getspiline (file, buf) == NULL)
            break;

        ptold = NULL;
        status = 0;

        one = 0;
        j = 0;

        for (x = fdx; x < nbx; x++) {
            if (flag[x] == 2)
                continue;
            if ((pt = sim_getnode (buf, argv[x])) != NULL) {
                if (pt < ptold) {
                    status = 0;
                    for (i = 0; i < nbx; i++) {
                        if (flag[i] == 2)
                            continue;
                        flag[i] = 0;
                    }
                    break;
                }
                ptold = pt;
                if (flag[x] == 1)
                    one = 1;
                flag[x] = 1;
                j++;
                i = 1;
                while (flag[x + i] == 1) {
                    flag[x + i] = 0;
                    i++;
                    one++;
                }
                status = 1;
            }
            else {
                status = 1;
                break;
            }
        }

        if (status == 0)
            continue;

        if (j != one) {
            step = 0.0;
            y = 0;
        }

        pos = 0;
        posx = 0;
        i = 0;

        ptres = NULL;

        while ((pos = sim_getspiparam (buf + posx, param, paramx)) != 0) {
            db = sim_getspidouble (param, &ptres);
            if (ptres == param)
                break;
            posx += pos;
            i++;
        }

        if (ptres == param)
            continue;

        for (x = fdx; x < nbx; x++)
            if (flag[x] == 1)
                break;

        if (x == nbx)
            continue;

        for (x = fdx; x < nbx; x++) {
            if (flag[x] == 2)
                continue;
            if (flag[x] == 1)
                i--;
            if (flag[x] == 0)
                break;
        }

        i--;

        pos = 0;
        posx = 0;

        if (i < 0)
            continue;

        while (i > 0) {
            pos = sim_getspiparam (buf + posx, param, paramx);
            posx += pos;
            i--;
        }

        pos = sim_getspiparam (buf + posx, param, paramx);
        posx += pos;

        db = sim_getspidouble (param, &ptres);

        if ((ptres == param) || (pos == 0) || (db < (0.0 - simstep)) ||
            (db > (limit + simstep)) || ((y == 0) && (db > (simstep)))) {
            continue;
        }

        if (fabs (db - step) > (simstep / 10.0)) {
            if (db < step) {
                continue;
            }
            else {
                while ((db > (step + (simstep / 2.0))) && (y < (nby - 1)) && (y > 0) &&
                       (fabs (db - step) > (simstep / 10.0))) {
                    for (x = fdx; x <= nbx; x++) {
                        if (flag[x] == 2)
                            continue;
                        if (flag[x] == 1) {
                          // TODO : interpolation par une droite
                          // recupere les bornes d'interpolation
                            tab[x][y] = tab[x][y - 1];
                        }
                        else
                            break;
                    }
                    y++;
                    step += simstep;
                }
            }
        }

        status = 0;

        for (x = fdx; x <= nbx; x++) {
            if (flag[x] == 2)
                continue;
            pos = sim_getspiparam (buf + posx, param, paramx);
            posx += pos;
            if (flag[x] == 0) {
                if (pos != 0) {
                    status = 0;
                    y = 0;
                    step = 0.0;
                }
                else
                    status = 1;
                break;
            }
            db = sim_getspidouble (param, &ptres);
            if (ptres == param) {
                y = 0;
                status = 0;
                step = 0.0;
                break;
            }
            tab[x][y] = db;
        }

        if (status == 1) {
            y++;
            step += simstep;
            if (y == nby) {
                for (x = fdx; x < nbx; x++) {
                    if (flag[x] == 1) {
                        flag[x] = 2;
                        fdx++;
                    }
                }
                y = 0;
                step = 0.0;
            }
        }
    }

    for (x = 0; x < nbx; x++) {
        if (flag[x] != 2) {
            fprintf (stderr, "sim error: error reading data for signal '%s' in file %s\n", argv[x], fileout);
        }
    }

    mbkfree (flag);
    sim_fclose (file, fileout);
}

/* ---------------------------------------------------------------------------- */

extern mbk_sem_t simInFork;

int sim_execspice (char *filename, int silent, char *spicename, char *spicestring, char *spicestdout)
{
    pid_t pid;
    int status;
    char *argv[1024];
    char buf[32000];
    char *fileout;
    int j, bufidx;

    if (V_BOOL_TAB[__SIM_USE_SYSTEM_CMD].VALUE)
    {
       char buf0[32000];
       j = 0;
       bufidx=0;
       while ((argv[j] = sim_getarg (spicestring, filename, j, buf, &bufidx)))
          j++;

       j=0;
       strcpy(buf0,"");
       if (silent == 0) avt_fprintf (stdout, "\n¤+RUN: ");
       while (argv[j]) {
                strcat(buf0, " ");
                strcat(buf0, argv[j]);
                if (silent == 0) fprintf (stdout, "%s ", argv[j]);
                j++;
                if (silent == 0) fflush (stdout);
            }
       if (silent == 0) avt_fprintf (stdout, "¤.");

       if (system(buf0)!=0)
       {
        if (silent == 0)
          avt_fprintf (stdout, "... ¤6¤~ Execution Failed ¤.\n");
        else if (silent == 2)
          {
            avt_fprintf (stdout, "¤6¤~X¤.");
            fflush(stdout);
          }
        avt_errmsg( SIM_ERRMSG, "003", AVT_WARNING, spicename, spicename, WEXITSTATUS (status));
        return 0;
      }
      else {
        if (silent == 0)
          avt_fprintf (stdout, "... ¤7¤~ Execution Completed ¤.\n");
        else if (silent == 2)
          {
            avt_fprintf (stdout, "¤7¤~*¤.");
            fflush(stdout);
          }
       }
       return 1;
    }
    
    mbk_sem_wait(&simInFork);

    pid = vfork ();

    if (pid == (pid_t) (-1)) {
        perror(buf);
        fprintf (stderr, "\nsim error: can't create a new process to run %s\n", spicename);
        fprintf (stderr, "%s\n", buf);
        mbk_sem_post(&simInFork);
        return 0;
    }
    else if (pid == 0) {
        j = 0;
        bufidx=0;
        while ((argv[j] = sim_getarg (spicestring, filename, j, buf, &bufidx)))
            j++;

        if (silent == 0) {
            j = 0;

            avt_fprintf (stdout, "\n¤+RUN: ");

            while (argv[j]) {
                fprintf (stdout, "%s ", argv[j]);
                j++;
                fflush (stdout);
            }

            avt_fprintf (stdout, "¤.");

//            fprintf (stdout, "\n");
            fflush (stdout);
        }

        fileout = sim_getjoker (spicestdout, filename);

        if ((j = open (fileout, O_WRONLY | O_CREAT | O_TRUNC, (S_IRWXU & ~S_IXUSR) | S_IRGRP | S_IROTH)) == -1) {
            avt_errmsg( SIM_ERRMSG, "001", AVT_FATAL, fileout);
//            fprintf (stderr, "\nsim error: can't open file %s\n", fileout);
            exit (1);
        }
        else if (dup2 (j, 1) != 1) {
            fprintf (stderr, "\nsim error: can't copy stdout to file %s\n", fileout);
            exit (1);
        }
/*        else {
            close (j);
        }*/

/*        if ((j = open ("/dev/null", O_WRONLY)) == -1) {
            avt_errmsg( SIM_ERRMSG, "001", AVT_FATAL, fileout);
            //fprintf (stderr, "\nsim error: can't open file %s\n", fileout);
            exit (1);
        }*/
        if (dup2 (j, 2) != 2) {
            fprintf (stderr, "\nsim error: can't copy stderr to file %s\n", fileout);
            exit (1);
        }
        close (j);
        if ((j = open ("/dev/null", O_RDONLY)) == -1) {
            avt_errmsg( SIM_ERRMSG, "001", AVT_FATAL, fileout);
            //fprintf (stderr, "\nsim error: can't open file %s\n", fileout);
            exit (1);
        }
        if (dup2 (j, 0) != 0) {
            fprintf (stderr, "\nsim error: can't copy stdin to file %s\n", fileout);
            exit (1);
        }
        close (j);
        mbkfree(fileout);

        if (execvp (argv[0], argv) != -1) {
            exit (0);
        }
        else {
            exit (1);
        }
    }
    else {
      mbk_sem_post(&simInFork);
      j = mbkwaitpid (pid, 1, &status);
      mbk_sem_wait(&simInFork);
      if (WEXITSTATUS (status) != 0) {
        if (silent == 0)
          avt_fprintf (stdout, "... ¤6¤~ Execution Failed ¤.\n");
        else if (silent == 2)
          {
            avt_fprintf (stdout, "¤6¤~X¤.");
            fflush(stdout);
          }
        avt_errmsg( SIM_ERRMSG, "003", AVT_WARNING, spicename, spicename, WEXITSTATUS (status));
        mbk_sem_post(&simInFork);
        return 0;
      }
      else {
        if (silent == 0)
          avt_fprintf (stdout, "... ¤7¤~ Execution Completed ¤.\n");
        else if (silent == 2)
          {
            avt_fprintf (stdout, "¤7¤~*¤.");
            fflush(stdout);
          }
      }
      mbk_sem_post(&simInFork);
    }
    return 1;
}

/* ---------------------------------------------------------------------------- */

char *sim_getjoker (char *fileout, char *filename)
{
    char *pt;
    char *ptx;
    char *pttok;
    char buf[1024];
    char *fileoutdup, *filenameorig;

    filenameorig=filename;
    filename = sim_getfilename (filename);

    ptx = buf;

    fileoutdup = mbkstrdup (fileout);
    pttok = strtok (fileoutdup,":");
    while ( pttok ) {
        for (pt = pttok; *pt != '\0'; pt++) {
            if (*pt == '$') {
                strcpy (ptx, filename);
                ptx += strlen (filename);
            }
            else if (*pt == '@') {
                strcpy (ptx, filenameorig);
                ptx += strlen (filenameorig);
            }
            else {
                *ptx = *pt;
                ptx++;
            }
        }
    
        *ptx = '\0';
        if ( filepath(buf,NULL) ) 
          break;
        ptx = buf;
        pttok = strtok(NULL,":");
    }

    pt = (char *)mbkalloc (strlen (buf) + 1);
    strcpy (pt, buf);
    mbkfree (filename);
    mbkfree (fileoutdup);

    return pt;
}

/* ---------------------------------------------------------------------------- */

char *sim_getfilename (char *filename)
{
    char *pt;
    char *ptx;

    pt = (char *)mbkalloc (strlen (filename) + 1);
    strcpy (pt, filename);
    ptx = strstr (pt, ".spi");
    if (ptx)
        *ptx = '\0';
    return (pt);
}

/* ---------------------------------------------------------------------------- */

char *sim_getarg (char *str, char *filename, int nb, char *buf, int *bufidx)
{
    char *arg;
    char *pt;
    char *ptx, *ext;
    char c;
    char temp[1024];

    pt = str;

    while (nb >= 0) {
        while ((isspace ((int)*pt) != 0) && (*pt != '\0'))
            pt++;
        if (*pt == '\0')
            return NULL;
        ptx = pt;
        while ((isspace ((int)*pt) == 0) && (*pt != '\0'))
            pt++;
        nb--;
    }

    c = *pt;
    *pt = '\0';

    if (strchr (ptx, '$') || strchr (ptx, '@'))
    {
      int idx=0;
      while (*ptx!='\0')
      {
        if (*ptx=='$' || *ptx=='@')
        {
          strcpy(&temp[idx], filename);
          if (*ptx=='$' && *(ptx+1)=='<')
          {
             if ((ext=strrchr(&temp[idx],'.'))!=NULL) *ext='\0';
             ptx++;
          }
          idx+=strlen(&temp[idx]);
        }
        else temp[idx++]=*ptx;
        ptx++;
      }
      temp[idx]='\0';
      ptx = temp;
    }
    arg = &buf[*bufidx];
    *bufidx+=strlen (ptx) + 1;
    strcpy (arg, ptx);
    *pt = c;

    return (arg);
}

/* -------- y (x) = a.x + b --------------------------------------------------- */

void sim_line_interpol (double x1, double x2, double y1, double y2, 
                        double *a0, double *b0)
{
  double a,b;

  a = (y2 - y1) / (x2-x1);
  if ( y1 != 0.0)
    b = (a * x1) / y1;
  else
    b = (a * x2) / y2;
  if ( a0 ) *a0 = a;
  if ( b0 ) *b0 = b;
}

