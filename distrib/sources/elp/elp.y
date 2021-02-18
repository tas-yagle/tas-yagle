%{
#include ELP_H
elpmodel_list *LIST_ELPMODEL = NULL ;
elpmodel_list LIST_ELPMODEL_S ;
static char *NNAME = NULL ;
static char *PNAME = NULL ;
static int  newformat = 0 ;
static int  oldformat = 0 ;
int yyerror();
int yylex();
int elpEsimLevel = 0 ;

void getelptrans() ;


%}

%union {
       double elpdbl       ;
       int    elpint       ;
       char   *elpstr      ;
       }

%token T_TECNAME T_TECVER T_LMLT T_WMLT T_DL T_DW T_DLC T_DWC T_DWCJ
%token T_ESIM T_MODEL T_TECHNO T_ELEVEL T_LEVEL
%token T_ACM T_TEMP T_SLOPE T_VDDmax T_VDDBEST T_VDDWORST
%token T_DTHR T_SLTHR T_SHTHR T_VTHR
%token T_VDDdeg T_VSSdeg T_VDEG T_VTI
%token T_INDEX T_TYPE
%token T_LMIN T_LMAX T_WMIN T_WMAX
%token T_VT T_VT0 T_KT T_A T_B T_RT T_RS T_KRT T_KRS T_KS T_KR T_M
%token T_RACCS T_RACCD
%token T_MULU0 T_DELVT0 T_SA T_SB T_SD T_VBULK T_NF T_NRS T_NRD T_SC T_SCA T_SCB T_SCC
%token T_CGS T_CGS0 T_CGP T_CGD T_CGD0 T_CGD1 T_CGD2 T_CGDC T_CGDC0 T_CGDC1 T_CGDC2 T_CGSI T_CGSIC T_CGSU T_CGSU0 T_CGSD T_CGSD0
%token T_CGSUMIN T_CGSUMAX T_CGSDMIN T_CGSDMAX
%token T_CGPUMIN T_CGPUMAX T_CGPDMIN T_CGPDMAX
%token T_CDS T_CDSU T_CDSD T_CDP T_CDPU T_CDPD T_CDW T_CDWU T_CDWD
%token T_CSS T_CSSU T_CSSD T_CSP T_CSPU T_CSPD T_CSW T_CSWU T_CSWD
%token T_BEGIN T_END
%token T_MOS T_NMOS T_PMOS
%token T_CASE T_WORST T_BEST T_TYPICAL
%token T_NAME_ALIAS

/* token pour ancien format ELP */
%token T_DLN T_DLP T_DWN T_DWP T_VTN T_BN T_AN T_RNT
%token T_VTP T_BP T_AP T_RPT T_CGSN T_CGSP T_CGPN T_CGPP
%token T_CDSN T_CDSP T_CDPN T_CDPP T_CDWN T_CDWP 
%token T_CSSN T_CSSP T_CSPN T_CSPP T_CSWN T_CSWP
%token T_NNAME T_PNAME

%token <elpstr> T_STRING
%token <elpdbl> T_NUMBER

%type <elpstr> trans_name
%type <elpint> trans_type
%type <elpint> trans_index
%type <elpint> trans_techno
%type <elpdbl> number

%start elp

%%

elp           : T_TECNAME ':' T_STRING T_TECVER ':' number
              {
                strcpy(elpTechnoName,$3) ;
                elpTechnoVersion = $6 ;
                NNAME = NULL ;
                PNAME = NULL ;
                newformat = 0 ;
                oldformat = 0 ;
                LIST_ELPMODEL = NULL ;
              }
               param_list
              ;

number        : T_NUMBER
              {
               $$ = $1 ;
              }
              | '-' T_NUMBER
              {
               $$ = -$2 ;
              }
              ;

param_list    :
                sim_param
              | param_list sim_param
              ;

sim_param     : 
                 name_sim 
               | trans_model 
               | level
               | valim 
               | vseuil 
               | dseuil 
               | shseuil 
               | slseuil 
               | slope 
               | acm 
               | temp 
               | setmodels 
               {
                 if(oldformat == 1)
                    fprintf(stderr, "\nFile %s contains differents elp format!!!\n", elpTechnoName) ;
                 else
                    newformat = 1 ;
               }
               | oldparam
               {
                 if(newformat == 1)
                    fprintf(stderr, "\nFile %s contains differents elp format!!!\n", elpTechnoName) ;
                 else
                    oldformat = 1 ;
               }
              ;

name_sim      : T_ESIM '=' T_STRING
              {
                strcpy(elpEsimName,$3) ;
              }
              ;

trans_model   : T_MODEL '=' T_MOS
              {
              }
              ;
              
trans_techno : T_TECHNO '=' number
               {
                 $$ = $3 ;
               }
              ;
level         : T_LEVEL '=' number
              {
                elpEsimLevel = $3 ;
                fprintf(stderr, "\nWarning : File %s is an obsolete format.", elpTechnoName );
              }
              ;

valim         : T_VDDmax '=' number
              {
                elpGeneral[elpGVDDMAX] = $3 ;
              }
              | T_VDDBEST '=' number
              {
                elpGeneral[elpGVDDBEST] = $3 ;
              }
              | T_VDDWORST '=' number
              {
                elpGeneral[elpGVDDWORST] = $3 ;
              }
              ;
              
vseuil        : T_VTHR '=' number
              {
                elpGeneral[elpGDTHR] = $3 / elpGeneral[elpGVDDMAX] ;
              }
              ;

dseuil        : T_DTHR '=' number
              {
                elpGeneral[elpGDTHR] = $3 ;
              }
              ;

shseuil       : T_SHTHR '=' number
              {
                elpGeneral[elpGSHTHR] = $3 ;
              }
              ;

slseuil       : T_SLTHR '=' number
              {
                elpGeneral[elpGSLTHR] = $3 ;
              }
              ;

slope         : T_SLOPE '=' number
              {
                elpGeneral[elpSLOPE] = $3 ;
              }
              ;
              
acm           : T_ACM '=' number
              {
                elpGeneral[elpACM] = $3 ;
              }
              ;
              
temp          : T_TEMP '=' number
              {
                elpGeneral[elpTEMP] = $3 ;
              }
              ;
              
setmodels     : T_BEGIN trans_name trans_index trans_techno trans_type
              {
               LIST_ELPMODEL_S.elpModelName = namealloc($2) ; 
               LIST_ELPMODEL_S.elpModelNameAlias = NULL ; 
               LIST_ELPMODEL_S.elpTransIndex = $3 ; 
               LIST_ELPMODEL_S.elpTransTechno = $4 ; 
               LIST_ELPMODEL_S.elpTransType = $5 ; 
               LIST_ELPMODEL_S.elpTransModel = elpMOS ; 
               LIST_ELPMODEL_S.elpTransCase = elpTYPICAL ; 
               LIST_ELPMODEL_S.elpTemp =  ELPINITTEMP ;
               LIST_ELPMODEL_S.elpRange[elpLMIN] = (long)0 ;
               LIST_ELPMODEL_S.elpRange[elpLMAX] = ELPMAXLONG ;
               LIST_ELPMODEL_S.elpRange[elpWMIN] = (long)0 ;
               LIST_ELPMODEL_S.elpRange[elpWMAX] = ELPMAXLONG ;
               LIST_ELPMODEL_S.elpShrink[elpLMLT] = 1.0 ;
               LIST_ELPMODEL_S.elpShrink[elpWMLT] = 1.0 ;
               LIST_ELPMODEL_S.elpShrink[elpDL] = 0.0 ;
               LIST_ELPMODEL_S.elpShrink[elpDW] = 0.0 ;
               LIST_ELPMODEL_S.elpShrink[elpDLC] = 0.0 ;
               LIST_ELPMODEL_S.elpShrink[elpDWC] = 0.0 ;
               LIST_ELPMODEL_S.elpShrink[elpDWCJ] = 0.0 ;
               LIST_ELPMODEL_S.elpModel[elpVT] = 0.0 ;
               LIST_ELPMODEL_S.elpModel[elpVT0] = 0.0 ;
               LIST_ELPMODEL_S.elpVoltage[elpVBULK]  = ELPINITVBULK;
               LIST_ELPMODEL_S.elpModel[elpMULU0] = 1.0 ;
               LIST_ELPMODEL_S.elpModel[elpM] = 1.0 ;
               LIST_ELPMODEL_S.elpModel[elpDELVT0] = 0.0 ;
               LIST_ELPMODEL_S.elpModel[elpSA] = ELPINITVALUE;
               LIST_ELPMODEL_S.elpModel[elpSB] = ELPINITVALUE;
               LIST_ELPMODEL_S.elpModel[elpSD] = ELPINITVALUE;
               LIST_ELPMODEL_S.elpModel[elpSCA] = ELPINITVALUE;
               LIST_ELPMODEL_S.elpModel[elpSCB] = ELPINITVALUE;
               LIST_ELPMODEL_S.elpModel[elpSCC] = ELPINITVALUE;
               LIST_ELPMODEL_S.elpModel[elpSC] = ELPINITVALUE;
               LIST_ELPMODEL_S.elpModel[elpNF] = 1.0 ;
               LIST_ELPMODEL_S.elpModel[elpKT] = 0.0 ;
               LIST_ELPMODEL_S.elpModel[elpA] = 0.0 ;
               LIST_ELPMODEL_S.elpModel[elpB] = 0.0 ;
               LIST_ELPMODEL_S.elpModel[elpRT] = 0.0 ;
               LIST_ELPMODEL_S.elpModel[elpRS] = 0.0 ;
               LIST_ELPMODEL_S.elpModel[elpKS] = 1.0 ;
               LIST_ELPMODEL_S.elpModel[elpKR] = 1.0 ;
               LIST_ELPMODEL_S.elpVoltage[elpVDEG] = ELPMINVOLTAGE ;
               LIST_ELPMODEL_S.elpVoltage[elpVTI] = ELPMINVOLTAGE ;
               LIST_ELPMODEL_S.elpVoltage[elpVDDMAX] = ELPMINVOLTAGE ;
               LIST_ELPMODEL_S.elpCapa[elpCGS] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGS0] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSU] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSU0] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSUMIN] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSUMAX] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSD] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSD0] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSDMIN] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSDMAX] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGP] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGPUMIN] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGPUMAX] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGPDMIN] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGPDMAX] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGD] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGD0] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGD1] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGD2] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGDC] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGDC0] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGDC1] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGDC2] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSI] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGSIC] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCDS] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCDSU] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCDSD] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCDP] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCDPU] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCDPD] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCDW] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCDWU] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCDWD] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCSS] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCSSU] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCSSD] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCSP] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCSPU] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCSPD] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCSW] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCSWU] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCSWD] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGPO] = 0.0 ;
               LIST_ELPMODEL_S.elpCapa[elpCGPOC] = 0.0 ;
               LIST_ELPMODEL_S.elpRacc[elpRACCS] = 0.0 ;
               LIST_ELPMODEL_S.elpRacc[elpRACCD] = 0.0 ;
              }
                mccvars T_END trans_name trans_index
              { 
                LIST_ELPMODEL = elpAddModel(LIST_ELPMODEL_S.elpModelName, 
                                            LIST_ELPMODEL_S.elpModelNameAlias, 
                                            LIST_ELPMODEL_S.elpTransType , 
                                            LIST_ELPMODEL_S.elpTransIndex ,
                                            LIST_ELPMODEL_S.elpRange[elpLMIN],
                                            LIST_ELPMODEL_S.elpRange[elpLMAX],
                                            LIST_ELPMODEL_S.elpRange[elpWMIN],
                                            LIST_ELPMODEL_S.elpRange[elpWMAX],
                                            LIST_ELPMODEL_S.elpShrink[elpDL],
                                            LIST_ELPMODEL_S.elpShrink[elpDW],
                                            LIST_ELPMODEL_S.elpShrink[elpLMLT],
                                            LIST_ELPMODEL_S.elpShrink[elpWMLT],
                                            LIST_ELPMODEL_S.elpVoltage[elpVDDMAX],
                                            LIST_ELPMODEL_S.elpTransCase,
                                            LIST_ELPMODEL_S.elpTransTechno ,
                                            LIST_ELPMODEL_S.elpModel[elpMULU0],
                                            LIST_ELPMODEL_S.elpModel[elpDELVT0],
                                            LIST_ELPMODEL_S.elpModel[elpSA],
                                            LIST_ELPMODEL_S.elpModel[elpSB],
                                            LIST_ELPMODEL_S.elpModel[elpSD],
                                            LIST_ELPMODEL_S.elpModel[elpNF],
                                            LIST_ELPMODEL_S.elpModel[elpM],
                                            LIST_ELPMODEL_S.elpModel[elpNRS],
                                            LIST_ELPMODEL_S.elpModel[elpNRD],
                                            LIST_ELPMODEL_S.elpVoltage[elpVBULK], 
                                            LIST_ELPMODEL_S.elpModel[elpSC],
                                            LIST_ELPMODEL_S.elpModel[elpSCA],
                                            LIST_ELPMODEL_S.elpModel[elpSCB],
                                            LIST_ELPMODEL_S.elpModel[elpSCC],
                                            NULL) ;

                if( LIST_ELPMODEL_S.elpTemp < ELPMINTEMP)
                  LIST_ELPMODEL->elpTemp =   elpGeneral[elpTEMP] ;
                else
                  LIST_ELPMODEL->elpTemp =  LIST_ELPMODEL_S.elpTemp ;
         
                LIST_ELPMODEL->elpTransModel =  LIST_ELPMODEL_S.elpTransModel ;

                LIST_ELPMODEL->elpShrink[elpDLC] =  LIST_ELPMODEL_S.elpShrink[elpDLC] ;
                LIST_ELPMODEL->elpShrink[elpDWC] =  LIST_ELPMODEL_S.elpShrink[elpDWC] ;
                LIST_ELPMODEL->elpShrink[elpDWCJ] =  LIST_ELPMODEL_S.elpShrink[elpDWCJ] ;
                LIST_ELPMODEL->elpModel[elpVT] =  LIST_ELPMODEL_S.elpModel[elpVT] ;
                LIST_ELPMODEL->elpModel[elpVT0] =  LIST_ELPMODEL_S.elpModel[elpVT0] ;
                LIST_ELPMODEL->elpModel[elpMULU0] =  LIST_ELPMODEL_S.elpModel[elpMULU0] ;
                LIST_ELPMODEL->elpModel[elpDELVT0] =  LIST_ELPMODEL_S.elpModel[elpDELVT0] ;
                LIST_ELPMODEL->elpModel[elpSA] =  LIST_ELPMODEL_S.elpModel[elpSA] ;
                LIST_ELPMODEL->elpModel[elpSB] =  LIST_ELPMODEL_S.elpModel[elpSB] ;
                LIST_ELPMODEL->elpModel[elpSD] =  LIST_ELPMODEL_S.elpModel[elpSD] ;
                LIST_ELPMODEL->elpModel[elpNF] =  LIST_ELPMODEL_S.elpModel[elpNF] ;
                LIST_ELPMODEL->elpModel[elpNRS] =  LIST_ELPMODEL_S.elpModel[elpNRS] ;
                LIST_ELPMODEL->elpModel[elpNRD] =  LIST_ELPMODEL_S.elpModel[elpNRD] ;
                LIST_ELPMODEL->elpModel[elpKT] =  LIST_ELPMODEL_S.elpModel[elpKT] ;
                LIST_ELPMODEL->elpModel[elpA] =  LIST_ELPMODEL_S.elpModel[elpA] ;
                LIST_ELPMODEL->elpModel[elpM] =  LIST_ELPMODEL_S.elpModel[elpM] ;
                LIST_ELPMODEL->elpModel[elpB] =  LIST_ELPMODEL_S.elpModel[elpB] ;
                LIST_ELPMODEL->elpModel[elpRT] =  LIST_ELPMODEL_S.elpModel[elpRT] ;
                LIST_ELPMODEL->elpModel[elpRS] =  LIST_ELPMODEL_S.elpModel[elpRS] ;
                LIST_ELPMODEL->elpModel[elpKS] =  LIST_ELPMODEL_S.elpModel[elpKS] ;
                LIST_ELPMODEL->elpModel[elpKR] =  LIST_ELPMODEL_S.elpModel[elpKR] ;
                LIST_ELPMODEL->elpVoltage[elpVDEG] =  LIST_ELPMODEL_S.elpVoltage[elpVDEG] ;
                LIST_ELPMODEL->elpVoltage[elpVTI] =  LIST_ELPMODEL_S.elpVoltage[elpVTI] ;
                if(LIST_ELPMODEL_S.elpVoltage[elpVDDMAX] < 0.0) {
                   LIST_ELPMODEL->elpVoltage[elpVDDMAX] =  elpGeneral[elpGVDDMAX] ;
                }
                else
                   LIST_ELPMODEL->elpVoltage[elpVDDMAX] =  LIST_ELPMODEL_S.elpVoltage[elpVDDMAX] ;
                LIST_ELPMODEL->elpVoltage[elpVBULK] =  LIST_ELPMODEL_S.elpVoltage[elpVBULK] ;
                if (LIST_ELPMODEL_S.elpVoltage[elpVBULK] < ELPMINVBULK) {
                  if ( LIST_ELPMODEL_S.elpTransType == elpNMOS )
                    LIST_ELPMODEL->elpVoltage[elpVBULK] = 0.0;
                  else
                    LIST_ELPMODEL->elpVoltage[elpVBULK] = LIST_ELPMODEL_S.elpVoltage[elpVDDMAX];
                }
                LIST_ELPMODEL->elpCapa[elpCGS] =  LIST_ELPMODEL_S.elpCapa[elpCGS] ;
                LIST_ELPMODEL->elpCapa[elpCGS0] =  LIST_ELPMODEL_S.elpCapa[elpCGS0] ;
                LIST_ELPMODEL->elpCapa[elpCGSU] =  LIST_ELPMODEL_S.elpCapa[elpCGSU] ;
                LIST_ELPMODEL->elpCapa[elpCGSU0] =  LIST_ELPMODEL_S.elpCapa[elpCGSU0] ;
                LIST_ELPMODEL->elpCapa[elpCGSUMIN] =  LIST_ELPMODEL_S.elpCapa[elpCGSUMIN] ;
                LIST_ELPMODEL->elpCapa[elpCGSUMAX] =  LIST_ELPMODEL_S.elpCapa[elpCGSUMAX] ;
                LIST_ELPMODEL->elpCapa[elpCGSD] =  LIST_ELPMODEL_S.elpCapa[elpCGSD] ;
                LIST_ELPMODEL->elpCapa[elpCGSD0] =  LIST_ELPMODEL_S.elpCapa[elpCGSD0] ;
                LIST_ELPMODEL->elpCapa[elpCGSDMIN] =  LIST_ELPMODEL_S.elpCapa[elpCGSDMIN] ;
                LIST_ELPMODEL->elpCapa[elpCGSDMAX] =  LIST_ELPMODEL_S.elpCapa[elpCGSDMAX] ;
                LIST_ELPMODEL->elpCapa[elpCGP] =  LIST_ELPMODEL_S.elpCapa[elpCGP] ;
                LIST_ELPMODEL->elpCapa[elpCGPUMIN] =  LIST_ELPMODEL_S.elpCapa[elpCGPUMIN];
                LIST_ELPMODEL->elpCapa[elpCGPUMAX] =  LIST_ELPMODEL_S.elpCapa[elpCGPUMAX];
                LIST_ELPMODEL->elpCapa[elpCGPDMIN] =  LIST_ELPMODEL_S.elpCapa[elpCGPDMIN];
                LIST_ELPMODEL->elpCapa[elpCGPDMAX] =  LIST_ELPMODEL_S.elpCapa[elpCGPDMAX];
                LIST_ELPMODEL->elpCapa[elpCGD] =  LIST_ELPMODEL_S.elpCapa[elpCGD] ;
                LIST_ELPMODEL->elpCapa[elpCGD0] =  LIST_ELPMODEL_S.elpCapa[elpCGD0] ;
                LIST_ELPMODEL->elpCapa[elpCGD1] =  LIST_ELPMODEL_S.elpCapa[elpCGD1] ;
                LIST_ELPMODEL->elpCapa[elpCGD2] =  LIST_ELPMODEL_S.elpCapa[elpCGD2] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGDC]) == 0) {
                  LIST_ELPMODEL->elpCapa[elpCGDC] =  LIST_ELPMODEL_S.elpCapa[elpCGD] ;
                  LIST_ELPMODEL->elpCapa[elpCGDC0] =  LIST_ELPMODEL_S.elpCapa[elpCGD0] ;
                  LIST_ELPMODEL->elpCapa[elpCGDC1] =  LIST_ELPMODEL_S.elpCapa[elpCGD1] ;
                  LIST_ELPMODEL->elpCapa[elpCGDC2] =  LIST_ELPMODEL_S.elpCapa[elpCGD2] ;
                }
                else {
                  LIST_ELPMODEL->elpCapa[elpCGDC] =  LIST_ELPMODEL_S.elpCapa[elpCGDC] ;
                  LIST_ELPMODEL->elpCapa[elpCGDC0] =  LIST_ELPMODEL_S.elpCapa[elpCGDC0] ;
                  LIST_ELPMODEL->elpCapa[elpCGDC1] =  LIST_ELPMODEL_S.elpCapa[elpCGDC1] ;
                  LIST_ELPMODEL->elpCapa[elpCGDC2] =  LIST_ELPMODEL_S.elpCapa[elpCGDC2] ;
                }
                LIST_ELPMODEL->elpCapa[elpCGSI] =  LIST_ELPMODEL_S.elpCapa[elpCGSI] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGSIC]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCGSIC] =  LIST_ELPMODEL_S.elpCapa[elpCGSI] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCGSIC] =  LIST_ELPMODEL_S.elpCapa[elpCGSIC] ;

                // CDS
                LIST_ELPMODEL->elpCapa[elpCDS] =  LIST_ELPMODEL_S.elpCapa[elpCDS] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCDSU]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCDSU] =  LIST_ELPMODEL_S.elpCapa[elpCDS] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCDSU] =  LIST_ELPMODEL_S.elpCapa[elpCDSU] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCDSD]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCDSD] =  LIST_ELPMODEL_S.elpCapa[elpCDS] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCDSD] =  LIST_ELPMODEL_S.elpCapa[elpCDSD] ;

                // CDP
                LIST_ELPMODEL->elpCapa[elpCDP] =  LIST_ELPMODEL_S.elpCapa[elpCDP] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCDPU]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCDPU] =  LIST_ELPMODEL_S.elpCapa[elpCDP] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCDPU] =  LIST_ELPMODEL_S.elpCapa[elpCDPU] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCDPD]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCDPD] =  LIST_ELPMODEL_S.elpCapa[elpCDP] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCDPD] =  LIST_ELPMODEL_S.elpCapa[elpCDPD] ;

                // CDW
                LIST_ELPMODEL->elpCapa[elpCDW] =  LIST_ELPMODEL_S.elpCapa[elpCDW] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCDWU]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCDWU] =  LIST_ELPMODEL_S.elpCapa[elpCDW] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCDWU] =  LIST_ELPMODEL_S.elpCapa[elpCDWU] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCDWD]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCDWD] =  LIST_ELPMODEL_S.elpCapa[elpCDW] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCDWD] =  LIST_ELPMODEL_S.elpCapa[elpCDWD] ;

                // CSS
                LIST_ELPMODEL->elpCapa[elpCSS] =  LIST_ELPMODEL_S.elpCapa[elpCSS] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCSSU]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCSSU] =  LIST_ELPMODEL_S.elpCapa[elpCSS] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCSSU] =  LIST_ELPMODEL_S.elpCapa[elpCSSU] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCSSD]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCSSD] =  LIST_ELPMODEL_S.elpCapa[elpCSS] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCSSD] =  LIST_ELPMODEL_S.elpCapa[elpCSSD] ;

                // CSP
                LIST_ELPMODEL->elpCapa[elpCSP] =  LIST_ELPMODEL_S.elpCapa[elpCSP] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCSPU]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCSPU] =  LIST_ELPMODEL_S.elpCapa[elpCSP] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCSPU] =  LIST_ELPMODEL_S.elpCapa[elpCSPU] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCSPD]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCSPD] =  LIST_ELPMODEL_S.elpCapa[elpCSP] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCSPD] =  LIST_ELPMODEL_S.elpCapa[elpCSPD] ;

                // CSW
                LIST_ELPMODEL->elpCapa[elpCSW] =  LIST_ELPMODEL_S.elpCapa[elpCSW] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCSWU]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCSWU] =  LIST_ELPMODEL_S.elpCapa[elpCSW] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCSWU] =  LIST_ELPMODEL_S.elpCapa[elpCSWU] ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCSWD]) == 0) 
                  LIST_ELPMODEL->elpCapa[elpCSWD] =  LIST_ELPMODEL_S.elpCapa[elpCSW] ;
                else
                  LIST_ELPMODEL->elpCapa[elpCSWD] =  LIST_ELPMODEL_S.elpCapa[elpCSWD] ;
                LIST_ELPMODEL->elpRacc[elpRACCS] = LIST_ELPMODEL_S.elpRacc[elpRACCS]; 
                LIST_ELPMODEL->elpRacc[elpRACCD] = LIST_ELPMODEL_S.elpRacc[elpRACCD]; 
              }
              ;
              
trans_name    : T_STRING
              {
                $$ = $1 ;
              }
              ;
              
trans_index   : '[' number ']'
              {
                $$ = $2 ;
              }
              | 
              {
               $$ = elpNOINDEX ;
              }
              ;
              
trans_type    : T_TYPE '=' T_NMOS
              {
                $$ = elpNMOS ;
              }
              | T_TYPE '=' T_PMOS
              {
                $$ = elpPMOS ;
              } 
              ;
             
mccvars       : /* empty */
                elpvar
              | mccvars elpvar
              ;
              
elpvar        :
                /* les dimensions valables du transistor */
                T_LMIN '=' number {LIST_ELPMODEL_S.elpRange[elpLMIN] = (long)((double)$3 * (double)SCALE_X + (double)0.5) ;}
              | T_LMAX '=' number {LIST_ELPMODEL_S.elpRange[elpLMAX] = (long)((double)$3 * (double)SCALE_X + (double)0.5) ;}
              | T_WMIN '=' number {LIST_ELPMODEL_S.elpRange[elpWMIN] = (long)((double)$3 * (double)SCALE_X + (double)0.5) ;}
              | T_WMAX '=' number {LIST_ELPMODEL_S.elpRange[elpWMAX] = (long)((double)$3 * (double)SCALE_X + (double)0.5) ;}
              | T_CASE '=' T_BEST {LIST_ELPMODEL_S.elpTransCase = elpBEST ;}
              | T_CASE '=' T_WORST {LIST_ELPMODEL_S.elpTransCase = elpWORST ;}
              | T_CASE '=' T_TYPICAL {LIST_ELPMODEL_S.elpTransCase = elpTYPICAL ;}
                
                /* les parametres de shrink des transistors */
              | T_LMLT '=' number {LIST_ELPMODEL_S.elpShrink[elpLMLT] = $3 ;}
              | T_WMLT '=' number {LIST_ELPMODEL_S.elpShrink[elpWMLT] = $3 ;}
              | T_DL   '=' number {LIST_ELPMODEL_S.elpShrink[elpDL]   = $3 ;}
              | T_DW   '=' number {LIST_ELPMODEL_S.elpShrink[elpDW]   = $3 ;}
              | T_DLC  '=' number {LIST_ELPMODEL_S.elpShrink[elpDLC]   = $3 ;}
              | T_DWC  '=' number {LIST_ELPMODEL_S.elpShrink[elpDWC]   = $3 ;}
              | T_DWCJ '=' number {LIST_ELPMODEL_S.elpShrink[elpDWCJ]   = $3 ;}
                
                /* les caracteristiques des transistors */
              | T_VT  '=' number {LIST_ELPMODEL_S.elpModel[elpVT] = $3 ;}
              | T_VT0  '=' number {LIST_ELPMODEL_S.elpModel[elpVT0] = $3 ;}
              | T_MULU0 '=' number {LIST_ELPMODEL_S.elpModel[elpMULU0] = $3 ;}
              | T_M '=' number {LIST_ELPMODEL_S.elpModel[elpM] = $3 ;}
              | T_DELVT0 '=' number {LIST_ELPMODEL_S.elpModel[elpDELVT0] = $3 ;}
              | T_SA '=' number {LIST_ELPMODEL_S.elpModel[elpSA] = $3 ;}
              | T_SB '=' number {LIST_ELPMODEL_S.elpModel[elpSB] = $3 ;}
              | T_SD '=' number {LIST_ELPMODEL_S.elpModel[elpSD] = $3 ;}
              | T_SC '=' number {LIST_ELPMODEL_S.elpModel[elpSC] = $3 ;}
              | T_SCA '=' number {LIST_ELPMODEL_S.elpModel[elpSCA] = $3 ;}
              | T_SCB '=' number {LIST_ELPMODEL_S.elpModel[elpSCB] = $3 ;}
              | T_SCC '=' number {LIST_ELPMODEL_S.elpModel[elpSCC] = $3 ;}
              | T_NF '=' number {LIST_ELPMODEL_S.elpModel[elpNF] = $3 ;}
              | T_NRS '=' number {LIST_ELPMODEL_S.elpModel[elpNRS] = $3 ;}
              | T_NRD '=' number {LIST_ELPMODEL_S.elpModel[elpNRD] = $3 ;}
              | T_KT  '=' number {LIST_ELPMODEL_S.elpModel[elpKT] = $3 ;}
              | T_A   '=' number {LIST_ELPMODEL_S.elpModel[elpA]  = $3 ;}
              | T_B   '=' number {LIST_ELPMODEL_S.elpModel[elpB]  = $3 ;}
              | T_RT  '=' number {LIST_ELPMODEL_S.elpModel[elpRT] = $3 ;}
              | T_KRT '=' number {LIST_ELPMODEL_S.elpModel[elpKRT] = $3 ;}
              | T_RS  '=' number {LIST_ELPMODEL_S.elpModel[elpRS] = $3 ;}
              | T_KRS '=' number {LIST_ELPMODEL_S.elpModel[elpKRS] = $3 ;}
              | T_KS  '=' number {LIST_ELPMODEL_S.elpModel[elpKS] = $3 ;}
              | T_KR  '=' number {LIST_ELPMODEL_S.elpModel[elpKR] = $3 ;}
          
                /* les caracteristiques des tensions */
              | T_VBULK  '=' number {LIST_ELPMODEL_S.elpVoltage[elpVBULK] = $3 ;}
              | T_VDDmax '=' number {LIST_ELPMODEL_S.elpVoltage[elpVDDMAX] = $3 ;
                                     if (LIST_ELPMODEL_S.elpVoltage[elpVBULK] < ELPMINVBULK) {
                                       if ( LIST_ELPMODEL_S.elpTransType == elpNMOS )
                                         LIST_ELPMODEL_S.elpVoltage[elpVBULK] = 0.0;
                                       else
                                         LIST_ELPMODEL_S.elpVoltage[elpVBULK] = elpGeneral[elpGVDDMAX];
                                     }
                                    }
              | T_VDEG   '=' number {LIST_ELPMODEL_S.elpVoltage[elpVDEG]   = $3 ;}
              | T_VTI   '=' number {LIST_ELPMODEL_S.elpVoltage[elpVTI]   = $3 ;}
              | T_TEMP   '=' number {LIST_ELPMODEL_S.elpTemp = $3 ;}
           
                /* les capacites dynamiques */
              | T_CGS   '=' number {LIST_ELPMODEL_S.elpCapa[elpCGS] = $3 ;
                                    if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGS0]) == 0) 
                                      LIST_ELPMODEL_S.elpCapa[elpCGS0] = $3;
                                    if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGSU]) == 0) 
                                      LIST_ELPMODEL_S.elpCapa[elpCGSU] = $3;
                                    if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGSU0]) == 0) 
                                      LIST_ELPMODEL_S.elpCapa[elpCGSU0] = $3;
                                    if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGSUMIN]) == 0) 
                                      LIST_ELPMODEL_S.elpCapa[elpCGSUMIN] = $3;
                                    if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGSUMAX]) == 0) 
                                      LIST_ELPMODEL_S.elpCapa[elpCGSUMAX] = $3;
                                    if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGSD]) == 0) 
                                      LIST_ELPMODEL_S.elpCapa[elpCGSD] = $3;
                                    if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGSD0]) == 0) 
                                      LIST_ELPMODEL_S.elpCapa[elpCGSD0] = $3;
                                    if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGSDMIN]) == 0) 
                                      LIST_ELPMODEL_S.elpCapa[elpCGSDMIN] = $3;
                                    if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGSDMAX]) == 0) 
                                      LIST_ELPMODEL_S.elpCapa[elpCGSDMAX] = $3;
                                    }
              | T_CGS0  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGS0] = $3 ;}
              | T_CGSU  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSU] = $3 ;}
              | T_CGSU0 '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSU0] = $3 ;}
              | T_CGSUMIN  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSUMIN] = $3 ;}
              | T_CGSUMAX  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSUMAX] = $3 ;}
              | T_CGSD  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSD] = $3 ;}
              | T_CGSD0  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSD0] = $3 ;}
              | T_CGSDMIN  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSDMIN] = $3 ;}
              | T_CGSDMAX  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSDMAX] = $3 ;}
              | T_CGP   '=' number 
              { LIST_ELPMODEL_S.elpCapa[elpCGP] = $3 ;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGPUMIN]) == 0) 
                  LIST_ELPMODEL_S.elpCapa[elpCGPUMIN] = $3;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGPUMAX]) == 0) 
                  LIST_ELPMODEL_S.elpCapa[elpCGPUMAX] = $3;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGPDMIN]) == 0) 
                  LIST_ELPMODEL_S.elpCapa[elpCGPDMIN] = $3;
                if (elpDouble2Long (ELPPRECISION*LIST_ELPMODEL_S.elpCapa[elpCGPDMAX]) == 0) 
                  LIST_ELPMODEL_S.elpCapa[elpCGPDMAX] = $3;
              }
              | T_CGPUMIN   '=' number {LIST_ELPMODEL_S.elpCapa[elpCGPUMIN] = $3 ;}
              | T_CGPUMAX   '=' number {LIST_ELPMODEL_S.elpCapa[elpCGPUMAX] = $3 ;}
              | T_CGPDMIN   '=' number {LIST_ELPMODEL_S.elpCapa[elpCGPDMIN] = $3 ;}
              | T_CGPDMAX   '=' number {LIST_ELPMODEL_S.elpCapa[elpCGPDMAX] = $3 ;}
              | T_CGD   '=' number {LIST_ELPMODEL_S.elpCapa[elpCGD] = $3 ;}
              | T_CGD0  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGD0] = $3 ;}
              | T_CGD1  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGD1] = $3 ;}
              | T_CGD2  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGD2] = $3 ;}
              | T_CGDC  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGDC] = $3 ;}
              | T_CGDC0 '=' number {LIST_ELPMODEL_S.elpCapa[elpCGDC0] = $3 ;}
              | T_CGDC1 '=' number {LIST_ELPMODEL_S.elpCapa[elpCGDC1] = $3 ;}
              | T_CGDC2 '=' number {LIST_ELPMODEL_S.elpCapa[elpCGDC2] = $3 ;}
              | T_CGSI  '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSI] = $3 ;}
              | T_CGSIC '=' number {LIST_ELPMODEL_S.elpCapa[elpCGSIC] = $3 ;}
              | T_CDS   '=' number {LIST_ELPMODEL_S.elpCapa[elpCDS] = $3 ;}
              | T_CDSU  '=' number {LIST_ELPMODEL_S.elpCapa[elpCDSU] = $3 ;}
              | T_CDSD  '=' number {LIST_ELPMODEL_S.elpCapa[elpCDSD] = $3 ;}
              | T_CDP   '=' number {LIST_ELPMODEL_S.elpCapa[elpCDP] = $3 ;}
              | T_CDPU  '=' number {LIST_ELPMODEL_S.elpCapa[elpCDPU] = $3 ;}
              | T_CDPD  '=' number {LIST_ELPMODEL_S.elpCapa[elpCDPD] = $3 ;}
              | T_CDW   '=' number {LIST_ELPMODEL_S.elpCapa[elpCDW] = $3 ;}
              | T_CDWU  '=' number {LIST_ELPMODEL_S.elpCapa[elpCDWU] = $3 ;}
              | T_CDWD  '=' number {LIST_ELPMODEL_S.elpCapa[elpCDWD] = $3 ;}
              | T_CSS   '=' number {LIST_ELPMODEL_S.elpCapa[elpCSS] = $3 ;}
              | T_CSSU  '=' number {LIST_ELPMODEL_S.elpCapa[elpCSSU] = $3 ;}
              | T_CSSD  '=' number {LIST_ELPMODEL_S.elpCapa[elpCSSD] = $3 ;}
              | T_CSP   '=' number {LIST_ELPMODEL_S.elpCapa[elpCSP] = $3 ;}
              | T_CSPU  '=' number {LIST_ELPMODEL_S.elpCapa[elpCSPU] = $3 ;}
              | T_CSPD  '=' number {LIST_ELPMODEL_S.elpCapa[elpCSPD] = $3 ;}
              | T_CSW   '=' number {LIST_ELPMODEL_S.elpCapa[elpCSW] = $3 ;}
              | T_CSWU  '=' number {LIST_ELPMODEL_S.elpCapa[elpCSWU] = $3 ;}
              | T_CSWD  '=' number {LIST_ELPMODEL_S.elpCapa[elpCSWD] = $3 ;}
              | T_NAME_ALIAS '=' name_alias
              | T_RACCS '=' number {LIST_ELPMODEL_S.elpRacc[elpRACCS] = $3 ;}
              | T_RACCD '=' number {LIST_ELPMODEL_S.elpRacc[elpRACCD] = $3 ;}
              ;

name_alias    : T_STRING
              {
                LIST_ELPMODEL_S.elpModelNameAlias = addchain (LIST_ELPMODEL_S.elpModelNameAlias,namealloc($1)) ;
              }
              | T_STRING ':' name_alias
              {
                LIST_ELPMODEL_S.elpModelNameAlias = addchain (LIST_ELPMODEL_S.elpModelNameAlias,namealloc($1)) ;
              }
              ;

oldparam      : /* shrink parameters */
                T_DLN   '=' number 
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpShrink[elpDL] = $3 ; 
              }
              | T_DLP   '=' number 
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpShrink[elpDL] = $3 ; 
              }
              | T_DWN   '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpShrink[elpDW] = $3 ; 
              }
              | T_DWP   '=' number 
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpShrink[elpDW] = $3 ; 
              }
              | T_LMLT   '=' number 
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpShrink[elpLMLT] = $3 ; 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpShrink[elpLMLT] = $3 ; 
              }
              | T_WMLT   '=' number 
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpShrink[elpWMLT] = $3 ; 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpShrink[elpWMLT] = $3 ; 
              }
                /* nom de model de transistor */
              | T_NNAME '=' T_STRING
              { 
                if(NNAME != NULL)
                   yyerror() ;
                NNAME = namealloc($3) ;
                getelptrans(elpNMOS) ;
              }
              | T_PNAME '=' T_STRING
              { 
                if(PNAME != NULL)
                   yyerror() ;
                PNAME = namealloc($3) ;
                getelptrans(elpPMOS) ;
              }
                /* Transistors characteristics */
              | T_VTN   '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpModel[elpVT] = $3 ; 
              }
              | T_VTP   '=' number
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpModel[elpVT] = $3 ; 
              }
              | T_BN    '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpModel[elpB] = $3 ; 
              }
              | T_BP    '=' number
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpModel[elpB] = $3 ; 
              }
              | T_AN    '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpModel[elpA] = $3 ; 
              }
              | T_AP    '=' number 
              {
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpModel[elpA] = $3 ; 
              }
              | T_RNT   '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpModel[elpRT] = $3 ; 
              }
              | T_RPT   '=' number 
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpModel[elpRT] = $3 ; 
              }
                /* Degraded Voltage */
              | T_VDDdeg '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpVoltage[elpVDEG] = $3 ; 
                LIST_ELPMODEL->elpVoltage[elpVTI] = $3 ; 
              }
              | T_VSSdeg '=' number 
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpVoltage[elpVDEG] = $3 ; 
                LIST_ELPMODEL->elpVoltage[elpVTI] = $3 ; 
              }
                /* Grid capacitance */
              | T_CGSN  '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpCapa[elpCGS] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGS0] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSU0] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSD] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSD0] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSUMIN] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGSUMAX] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGSDMIN] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGSDMAX] = $3 ;
              }
              | T_CGSP  '=' number
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpCapa[elpCGS] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGS0] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSU0] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSD] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSD0] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGSUMIN] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGSUMAX] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGSDMIN] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGSDMAX] = $3 ;
              }
              | T_CGPN  '=' number 
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpCapa[elpCGP] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGPUMIN] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGPUMAX] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGPDMIN] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGPDMAX] = $3 ;
              }
              | T_CGPP  '=' number
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpCapa[elpCGP] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCGPUMIN] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGPUMAX] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGPDMIN] = $3 ;
                LIST_ELPMODEL->elpCapa[elpCGPDMAX] = $3 ;
              }
                /* Drain capacitance */
              | T_CDSN  '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpCapa[elpCDS] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDSU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDSD] = $3 ; 
              }
              | T_CDSP  '=' number
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpCapa[elpCDS] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDSU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDSD] = $3 ; 
              }
              | T_CDPN  '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpCapa[elpCDP] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDPU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDPD] = $3 ; 
              }
              | T_CDPP  '=' number 
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpCapa[elpCDP] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDPU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDPD] = $3 ; 
              }
              | T_CDWN  '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpCapa[elpCDW] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDWU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDWD] = $3 ; 
              }
              | T_CDWP  '=' number 
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpCapa[elpCDW] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDWU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCDWD] = $3 ; 
              }
                /* Source capacitance */
              | T_CSSN  '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpCapa[elpCSS] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSSU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSSD] = $3 ; 
              }
              | T_CSSP  '=' number
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpCapa[elpCSS] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSSU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSSD] = $3 ; 
              }
              | T_CSPN  '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpCapa[elpCSP] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSPU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSPD] = $3 ; 
              }
              | T_CSPP  '=' number 
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpCapa[elpCSP] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSPU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSPD] = $3 ; 
              }
              | T_CSWN  '=' number
              { 
                getelptrans(elpNMOS) ;
                LIST_ELPMODEL->elpCapa[elpCSW] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSWU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSWD] = $3 ; 
              }
              | T_CSWP  '=' number 
              { 
                getelptrans(elpPMOS) ;
                LIST_ELPMODEL->elpCapa[elpCSW] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSWU] = $3 ; 
                LIST_ELPMODEL->elpCapa[elpCSWD] = $3 ; 
              }
              ;

%%
extern char *elptext ;
extern int  ELPLINE  ;

void getelptrans(type)
int type ;
{
 
 for (LIST_ELPMODEL = ELP_MODEL_LIST ; LIST_ELPMODEL ; LIST_ELPMODEL = LIST_ELPMODEL->NEXT ) {
    if ((LIST_ELPMODEL->elpTransType) == type)
       break ;
 }

 if(LIST_ELPMODEL == NULL)
   {
     if ((type == elpNMOS) && (NNAME == NULL))
      {
       NNAME = namealloc("TN") ;
      }

     if ((type == elpPMOS) && (PNAME == NULL))
      {
       PNAME = namealloc("TP") ;
      }

        LIST_ELPMODEL = elpAddModel((type == elpNMOS) ? NNAME : PNAME, NULL,
                                    type, elpNOINDEX,(long)0,ELPMAXLONG,(long)0,ELPMAXLONG,
                                    0.0,0.0,1.0,1.0,ELPMINVOLTAGE,elpTYPICAL, 0,
                                    1.0,0.0,ELPINITVALUE,ELPINITVALUE,ELPINITVALUE,ELPINITVALUE,1.0,0.0,0.0,ELPINITVBULK,ELPINITVALUE,ELPINITVALUE,ELPINITVALUE,ELPINITVALUE,NULL) ;
   }
}

int yyerror()

{
elpError(1001,elptext,ELPLINE) ; /* si erreur de syntaxe => erreur 1001 */
return (0) ;
}
