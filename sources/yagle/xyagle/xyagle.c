#include <stdio.h>
#include <string.h>
#include <Xm/Xm.h>

#include MUT_H
#include MLO_H
#include CNS_H
#include CGV_H
#include CBH_H
#include LIB_H
#include TLF_H
#include STM_H
#include TTV_H
#include XSB_H
#include XYA_H
#include XMX_H
#include TRC_H
#include YAG_H
#include FCL_H

#ifdef AVERTEC
#include AVT_H
#endif

#include "xyagle.h"

/*------------------------------------------------------------\
|                           present                           |
\------------------------------------------------------------*/

void 
XyaglePresent()
{
#ifdef AVERTEC_LICENSE
    if (avt_givetoken("YAGLE_LICENSE_SERVER", "xyagle")!=AVT_VALID_TOKEN)
        XyagExit(1);
#endif
    avt_banner("Xyagle", "Graphical Cone & Netlist Viewer", "1999");
}

/*------------------------------------------------------------\
|                            main                             |
\------------------------------------------------------------*/

int 
main(argc, argv)
    int             argc;
    char           *argv[];
{
    int             Index;
    int             Debug = 0;

#ifdef AVERTEC
    avtenv();
#endif

    mbkenv();
    XyaglePresent();

    cnsenv();
    yagenv(NULL);               
    fclenv();
    rcnenv();
    rcx_env();
    elpenv();
    mccenv();
    libenv() ;
    tlfenv() ;
    ttvenv() ;
    cbhenv() ;
    
    for (Index = 1;
         Index < argc;
         Index++) {
        if (!strcmp(argv[Index], "-l")) {
            if ((Index + 1) < argc) {
                Index = Index + 1;
                XyagFirstFileName = argv[Index];
            }
        }
        else if (!strcmp(argv[Index], "-xor")) {
            XYAG_XOR_CURSOR = XYAG_TRUE;
        }
        else if (!strcmp(argv[Index], "-debug")) {
            Debug = 1;
        }
        else if (!strcmp(argv[Index], "-force")) {
            XYAG_FORCE_DISPLAY = XYAG_TRUE;
        }
        else if (!strcmp(argv[Index], "-install")) {
            XYAG_SWITCH_COLOR_MAP = XYAG_TRUE;
        }
    }

    Xyaggetenv();
    
    XyagInitializeRessources(&argc, argv, NULL);
    XyagInitializeErrorMessage(Debug);

    XtAppMainLoop(XyagleApplication);

    XyagExit (0);
}
