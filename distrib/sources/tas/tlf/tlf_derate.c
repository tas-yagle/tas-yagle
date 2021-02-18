/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/
#include MCC_H 
#include "tlf_derate.h" 

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

/****************************************************************************/
/*     function  tlf_derate_temp                                            */
/*     Calcul du derating en temperature : commun pour tlf3 et tlf4         */
/*     temps de propagation : tpfinal = tpinitial * derate_temp             */
/****************************************************************************/
double tlf_derate_temp(double T0, double T1, int transition)
{
    double derate_temp = 1.0 ;
    double rapport_Ids = 1.0 ;

    switch(transition) {
        case TLF_RISING :  rapport_Ids = mcc_calcRapIdsTemp(MCC_TECHFILE,
                                         NULL, MCC_PMOS,
                                         MCC_TYPICAL, MCC_TECSIZE*1.0e-6,
                                         12.0*MCC_TECSIZE*1.0e-6, MCC_VDDmax,
                                         T0, T1) ;
                           derate_temp = 1.0/rapport_Ids ;
                           break ;
        case TLF_FALLING : rapport_Ids = mcc_calcRapIdsTemp(MCC_TECHFILE,
                                         NULL, MCC_NMOS,
                                         MCC_TYPICAL, MCC_TECSIZE*1.0e-6,
                                         6.0*MCC_TECSIZE*1.0e-6, MCC_VDDmax,
                                         T0, T1) ;
                           derate_temp = rapport_Ids ;
                           break ;
    }
    
    return(derate_temp) ;
}

/****************************************************************************/
/*     function  tlf_derate_tension                                         */
/*     Calcul du derating en tension     : commun pour tlf3 et tlf4         */
/*     temps de propagation : tpfinal = tpinitial * derate_tension          */
/****************************************************************************/
double tlf_derate_tension(double V0, double V1, int transition)
{
    double derate_tension = 1.0 ;
    double rapport_Ids = 1.0 ;

    switch(transition) {
        case TLF_RISING :  rapport_Ids = mcc_calcRapIdsVolt(MCC_TECHFILE,
                                         NULL, MCC_PMOS,
                                         MCC_TYPICAL, MCC_TECSIZE*1.0e-6,
                                         12.0*MCC_TECSIZE*1.0e-6, MCC_TEMP,
                                         V0, V1) ;
                           break ;
        case TLF_FALLING : rapport_Ids = mcc_calcRapIdsVolt(MCC_TECHFILE,
                                         NULL, MCC_NMOS,
                                         MCC_TYPICAL, MCC_TECSIZE*1.0e-6,
                                         6.0*MCC_TECSIZE*1.0e-6, MCC_TEMP,
                                         V0, V1) ;
                           break ;
    }

    derate_tension = 1.0/rapport_Ids ;
    
    return(derate_tension) ;
}

/****************************************************************************/
/*     function  tlf_drive_derate                                           */
/*     Drive all derating models f=(P,V,T)                                  */ 
/****************************************************************************/
void tlf_drive_derate(FILE *f, double mintemp, double typtemp, double maxtemp, 
                      double minvolt, double typvolt, double maxvolt)
{
    /* No derating in Process */

    double RD_volt_p = 1.0 ;  /* Rising  derating when Vfinal > Vinit (typ->max)*/
    double RD_volt_n = 1.0 ;  /* Rising  derating when Vfinal < Vinit (typ->min)*/
    double RD_temp_p = 1.0 ;  /* Rising  derating when Tfinal > Tinit (typ->max)*/
    double RD_temp_n = 1.0 ;  /* Rising  derating when Tfinal < Tinit (typ->min)*/

    double FD_volt_p = 1.0 ;  /* falling derating when Vfinal > Vinit (typ->max)*/
    double FD_volt_n = 1.0 ;  /* falling derating when Vfinal < Vinit (typ->min)*/
    double FD_temp_p = 1.0 ;  /* falling derating when Tfinal > Tinit (typ->max)*/
    double FD_temp_n = 1.0 ;  /* falling derating when Tfinal < Tinit (typ->min)*/

    RD_volt_p = tlf_derate_tension(typvolt,maxvolt,TLF_RISING) ;
    RD_volt_n = tlf_derate_tension(typvolt,minvolt,TLF_RISING) ;
    RD_temp_p = tlf_derate_temp(typtemp,maxtemp,TLF_RISING) ;
    RD_temp_n = tlf_derate_temp(typtemp,mintemp,TLF_RISING) ;

    FD_volt_p = tlf_derate_tension(typvolt,maxvolt,TLF_FALLING) ;
    FD_volt_n = tlf_derate_tension(typvolt,minvolt,TLF_FALLING) ;
    FD_temp_p = tlf_derate_temp(typtemp,maxtemp,TLF_FALLING) ;
    FD_temp_n = tlf_derate_temp(typtemp,mintemp,TLF_FALLING) ;

    /* Process derating set to default value */
    fprintf(f,"model(RiseProcMultConstraints (const 1.0))\n") ;
    fprintf(f,"model(FallProcMultConstraints (const 1.0))\n") ;
    fprintf(f,"model(RiseProcMultMpwl        (const 1.0))\n") ;
    fprintf(f,"model(FallProcMultMpwl        (const 1.0))\n") ;
    fprintf(f,"model(RiseProcMultMpwh        (const 1.0))\n") ;
    fprintf(f,"model(FallProcMultMpwh        (const 1.0))\n") ;
    fprintf(f,"model(RiseProcMultPeriod      (const 1.0))\n") ;
    fprintf(f,"model(FallProcMultPeriod      (const 1.0))\n") ;
    fprintf(f,"model(RiseProcMultPropagation (const 1.0))\n") ;
    fprintf(f,"model(FallProcMultPropagation (const 1.0))\n") ;
    fprintf(f,"\n") ;

    /* Voltage derating */
    fprintf(f,"model(RiseVoltMultPropagation (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,RD_volt_n,1.0,RD_volt_p) ;
    fprintf(f,"model(FallVoltMultPropagation (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,FD_volt_n,1.0,FD_volt_p) ;
    fprintf(f,"model(RiseVoltMultMpwl        (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,RD_volt_n,1.0,RD_volt_p) ;
    fprintf(f,"model(FallVoltMultMpwl        (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,FD_volt_n,1.0,FD_volt_p) ;
    fprintf(f,"model(RiseVoltMultMpwh        (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,RD_volt_n,1.0,RD_volt_p) ;
    fprintf(f,"model(FallVoltMultMpwh        (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,FD_volt_n,1.0,FD_volt_p) ;
    fprintf(f,"model(RiseVoltMultPeriod      (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,RD_volt_n,1.0,RD_volt_p) ;
    fprintf(f,"model(FallVoltMultPeriod      (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,FD_volt_n,1.0,FD_volt_p) ;
    fprintf(f,"model(RiseVoltMultConstraint  (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,RD_volt_n,1.0,RD_volt_p) ;
    fprintf(f,"model(FallVoltMultConstraint  (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               minvolt,typvolt,maxvolt,FD_volt_n,1.0,FD_volt_p) ;
    fprintf(f,"\n") ;

    /* Temperature derating */
    fprintf(f,"model(RiseTempMultPropagation (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,RD_temp_n,1.0,RD_temp_p) ;
    fprintf(f,"model(FallTempMultPropagation (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,FD_temp_n,1.0,FD_temp_p) ;
    fprintf(f,"model(RiseTempMultMpwl        (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,RD_temp_n,1.0,RD_temp_p) ;
    fprintf(f,"model(FallTempMultMpwl        (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,FD_temp_n,1.0,FD_temp_p) ;
    fprintf(f,"model(RiseTempMultMpwh        (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,RD_temp_n,1.0,RD_temp_p) ;
    fprintf(f,"model(FallTempMultMpwh        (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,FD_temp_n,1.0,FD_temp_p) ;
    fprintf(f,"model(RiseTempMultPeriod      (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,RD_temp_n,1.0,RD_temp_p) ;
    fprintf(f,"model(FallTempMultPeriod      (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,FD_temp_n,1.0,FD_temp_p) ;
    fprintf(f,"model(RiseTempMultConstraint  (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,RD_temp_n,1.0,RD_temp_p) ;
    fprintf(f,"model(FallTempMultConstraint  (spline (axis %.2f %.2f %.2f) ( %.5f %.5f %.5f)))\n",
               mintemp,typtemp,maxtemp,FD_temp_n,1.0,FD_temp_p) ;
    fprintf(f,"\n") ;

}
