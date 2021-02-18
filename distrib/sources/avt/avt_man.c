#include "avt_lib.h"
#include "avt_env.h"


int main (int argc, char *argv[])
{
    avtenv ();
    
    avt_initerrmsg(argv[0]);
    
    
	if ((argc != 1)) {
        if (!strcmp (argv[1], "-x")) avt_man (1);   
    }
    else avt_man (0);

    return EXIT_SUCCESS;
}

