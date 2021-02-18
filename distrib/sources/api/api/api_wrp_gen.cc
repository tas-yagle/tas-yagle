#include "api_wrp_gns.h"

extern int SWIG_main (int, char**, Language*);

int main (int argc, char **argv)
{
    int rescode;

    GENIUS *l = new GENIUS;

    WIG_init_args (argc, argv);
    rescode = WIG_main (argc, argv, l);
    EXIT(rescode);
}
