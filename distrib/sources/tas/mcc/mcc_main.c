/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include MCC_H
#include "mcc_mod_spice.h" 
#include "mcc_mod_util.h" 

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

int main (int argc, char **argv)
{
  char *technoname;
  double CDEPa,CDEPp;
  double Cmetal,Cpoly;
  double area=5.100E-13;
  double perim=2.86e-6;
  double Va,Vc,capadiode;
  char *dioname;
  mcc_modellist *ptmodel;

  avtenv () ;
  mbkenv () ;
  mccenv();
  if (argc == 2) {
    technoname = namealloc(argv[1]);
    parsespice(technoname);
    dioname = "npdiolln3";
    area = atof (getenv("area"));
    perim = atof (getenv("perim"));
    Va = 0.0;
    Vc = MCC_VDDmax;
    capadiode = mcc_calcDioCapa ( technoname, dioname,
                                  MCC_DIODE,MCC_TYPICAL,
                                  Va, Vc, MCC_TEMP,
                                  area, perim
                                );
    printf ("\n Capa diode = %g\n\n",capadiode);
    return 0;
  }
  else
    fprintf(stderr,"\nmcc error : try parsetech <technofile>\n");

  return 0;
}
