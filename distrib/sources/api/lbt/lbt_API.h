#include "avt_API_types.h"
/*
    MAN lib_DriveFile
    DESCRIPTION
    Saves a list of blackboxed timing figures on disk, in a single {.lib} file
    ARGS
    fig_list % List of timing figures to print in the {.lib} file. For a single timing figure, use a list of one element (example given below)
    befig_list % List of behavioral figures to associate with the timing figures. There should be a one-to-one correspondence between the elements in {befig_list} and {fig_list}, and there should be the same number of elements if the two lists. Use {NULL} to fill the gaps in {befig_list}.
    file % Name of the {.lib} file to create
    delaytype % Defines if the {cell_rise} and {cell_fall} timing groups will have only maximum delays ({max}), minimum delays ({min}) or both ({both}).
    EXAMPLE % {lib_DriveFile [list $bbox] NULL cpu.lib max}
*/
void lib_DriveFile (List *fig_list, List *befig_list, char* file, char* delaytype);
void lib_drivefile (List *fig_list, List *befig_list, char* file, char* delaytype);

/*
    MAN lib_DriveHeader
    DESCRIPTION
    Prints a {.lib} header in a file, regarding to the information present in the given timing figure
    ARGS
    fig % Pointer on the timing figure to consider
    file % Pointer on the file where to print the {.lib} header
    libname % Name to be put in the {library} statement of the {.lib} header
*/
void lib_DriveHeader (TimingFigure *fig, FILE* file, char* libname);

/*
    MAN lib_CanonicPinName
    DESCRIPTION
    Adapts pin names according to the {.lib} syntax (typically bus delimiter are replaced by {_)}
    ARGS
    name % Pin's name
*/
char *lib_CanonicPinName (char *name);

/*
    MAN xxxtlf_DriveFile
    DESCRIPTION
    ARGS
    fig_list % List of timing figures to print in the {.lib} file. For a single timing figure, use a list of one element (example given below)
    befig_list % List of behavioral figures to associate with the timing figures. There should be a one-to-one correspondence between the elements in {befig_list} and {fig_list}, and there should be the same number of elements if the two lists. Use {NULL} to fill the gaps in {befig_list}.
    file % Name of the {.lib} file to create
    format % {tlf3} or {tlf4}
    EXAMPLE % {tlf_DriveFile [list $bbox] NULL cpu.tlf tlf4}
*/

void tlf_DriveFile  (List *fig_list, List *befig_list, char* file, char* format);
