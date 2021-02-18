#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "mut_lib.h"
#include AVT_H

void mbk_debugstat(char *label, int reset)
{
    unsigned long   temps;
    unsigned long   user, syst;
    unsigned long   userM, userS, userD;
    unsigned long   systM, systS, systD;
    long long  bytes;
    static struct rusage start, end;
    static unsigned long mem;
    static time_t rstart, rend;

    if (reset)
      {
        getrusage(RUSAGE_SELF, &end);
        mem=mbkprocessmemoryusage();
        time(&rend);
      }
    else
      {
        memcpy(&start, &end, sizeof(struct rusage));
        rstart=rend;
        
        time(&rend);
        getrusage(RUSAGE_SELF, &end);

        temps = rend - rstart;
        user = (100 * end.ru_utime.tv_sec + (end.ru_utime.tv_usec / 10000))
          - (100 * start.ru_utime.tv_sec + (start.ru_utime.tv_usec / 10000));
        syst = (100 * end.ru_stime.tv_sec + (end.ru_stime.tv_usec / 10000))
          - (100 * start.ru_stime.tv_sec + (start.ru_stime.tv_usec / 10000));

        userM = user / 6000;
        userS = (user % 6000) / 100;
        userD = (user % 100) / 10;
        
        systM = syst / 6000;
        systS = (syst % 6000) / 100;
        systD = (syst % 100) / 10;
        
//        fprintf(stderr, "s:+%02ldm%02lds", (long) (temps / 60), (long) (temps % 60));
        avt_fprintf(stdout, "¤5%s",label!=NULL?label:"");
        avt_fprintf(stdout, "s:+%02ldm%02ld.%ld", systM, systS, systD);
        avt_fprintf(stdout, " u:+%02ldm%02ld.%ld", userM, userS, userD);
        bytes = (unsigned long long)mbkprocessmemoryusage()-(unsigned long long)mem;
        mem=mbkprocessmemoryusage();
        avt_fprintf(stdout, " m:%+lldk (top=%ldk)¤.\n", ((long long)bytes)/1024, mbkprocessmemoryusage()/1024);
 
        fflush(stderr);
      }
}
