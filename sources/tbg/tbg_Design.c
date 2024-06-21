/* functions for the pattern.c file */

#include "tbg.h"

extern int   vhdlparse ();
extern FILE *vhdlin;

extern int vhdldebug;

void Design (char *file_name, char *design_name)
{
    static int already_done = 0;
    char pat_file_name[1024];
    char out_file_name[1024];

    if (!already_done) {
        if ((vhdlin = fopen (file_name, "r"))) {
            DESIGN_NAME = strdup (design_name);
            //vhdldebug = 1;
            vhdlparse ();
            fclose (vhdlin); 
        } else {
            fprintf (stderr, "Can't open file %s for writing, exiting...\n", file_name);
            EXIT (1);
        }
    }
    already_done = 1;

    sprintf (pat_file_name, "%s.pat", design_name);
    sprintf (out_file_name, "%s.out", design_name);
    if (!(PAT_FILE = fopen (pat_file_name, "w+"))) {
        fprintf (stderr, "Can't open file %s for writing, exiting...\n", pat_file_name);
        EXIT (1);
    }
    PAT_FILE_NAME = strdup (pat_file_name);
    OUT_FILE_NAME = strdup (out_file_name);
}
