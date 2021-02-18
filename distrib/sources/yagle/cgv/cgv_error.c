/*------------------------------------------------------------\
|                                                             |
| Tool    :                     Cgv                           |
|                                                             |
| File    :                 Cgv Errors                        |
|                                                             |
| Authors :              Picault  Stephane                    |
|                        Miramond   Benoit                    |
|                                                             |
| Date    :                   04.03.98                        |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include MUT_H
#include SLB_H
#include CGV_H
#include AVT_H

void 
cgv_error(Error, Text, File, Line)
    char            Error;
    char           *Text;
    char           *File;
    long            Line;
{
    char           *Name;

    Name = mbkstrdup(File);
    Name[strlen(File) - 1] = '\0';

//    fprintf(stderr, "%s%ld ", Name, Line);

    switch (Error) {
    default:

        avt_errmsg(CGV_ERRMSG, "003", AVT_FATAL, Name, Line, Error);
        // fprintf(stderr, "unknown internal error %d !\n", Error);
    }

    EXIT(1);
}
