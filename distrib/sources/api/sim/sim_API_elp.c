#include ELP_H
#include MCC_H

void elp_API_env ()
{
    static int already_loaded = 0;
    if (!already_loaded) {
        elpenv ();
        mccenv ();
        already_loaded = 1;
    }
}

double elp_GetCapaFromConnector (locon_list *locon)
{
    elp_API_env ();
    return 1.0e-12*elpGetCapaFromLocon (locon,ELP_CAPA_TYPICAL,elpTYPICAL);
}

