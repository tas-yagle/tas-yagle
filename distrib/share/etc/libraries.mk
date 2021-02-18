# ###---------------------------------------------------------###
# file          : libraries.mk                                  #
# description   : Alliance Shared Libraries and Include Files   #
# ###---------------------------------------------------------###

# $Id: libraries.mk,v 1.31 2009/07/06 14:54:38 olivier Exp $


# ###---------------------------------------------------------###
#    Common settings                                            #
# ###---------------------------------------------------------###

ALLIANCE_VERSION = '"4.0.2"'

# ###---------------------------------------------------------###
#    libraries version settings                                 #
# ###---------------------------------------------------------###
API_LIBVERSION = 100
AVT_LIBVERSION = 100
BCK_LIBVERSION = 100
BDD_LIBVERSION = 104
BEH_LIBVERSION = 110
BEF_LIBVERSION = 100
BEG_LIBVERSION = 100
BHL_LIBVERSION = 110
BGL_LIBVERSION = 100
BSC_LIBVERSION = 100
BVL_LIBVERSION = 202
CBH_LIBVERSION = 100
CGV_LIBVERSION = 100
CNS_LIBVERSION = 211
ELP_LIBVERSION = 110
EQT_LIBVERSION = 100
FCL_LIBVERSION = 110
GEN_LIBVERSION = 165
INF_LIBVERSION = 112
LIB_LIBVERSION = 100
MAL_LIBVERSION = 100
MCC_LIBVERSION = 100
MCL_LIBVERSION = 413
MCP_LIBVERSION = 409
MEL_LIBVERSION = 407
MHL_LIBVERSION = 403
MGL_LIBVERSION = 100
MLO_LIBVERSION = 501
MLU_LIBVERSION = 501
MPH_LIBVERSION = 412
MPU_LIBVERSION = 408
MSL_LIBVERSION = 700
MUT_LIBVERSION = 325
MVL_LIBVERSION = 409
RCN_LIBVERSION = 200
SDC_LIBVERSION = 100
SDLR_LIBVERSION = 100
SIM_LIBVERSION = 100
SLB_LIBVERSION = 100
SPE_LIBVERSION = 100
SPF_LIBVERSION = 100
STB_LIBVERSION = 100
STM_LIBVERSION = 200
TAS_LIBVERSION = 600
TMA_LIBVERSION = 100
TLC_LIBVERSION = 100
TLF_LIBVERSION = 100
TRC_LIBVERSION = 102
TTV_LIBVERSION = 200
TUT_LIBVERSION = 100
VCD_LIBVERSION = 100
XAL_LIBVERSION = 102
XYA_LIBVERSION = 100
YAGLE_LIBVERSION = 355
ZEN_LIBVERSION = 101

# ###---------------------------------------------------------###
#    general purpose utilities                                  #
# ###---------------------------------------------------------###
STDCPP_L   = $(ENABLE_STATIC) -lstdc++ $(DISABLE_STATIC)
MUT_L      =  -lMut$(MUT_LIBVERSION) $(STDCPP_L)
MUT_LIB    = libMut$(MUT_LIBVERSION).a
MUT_H      =    mut$(MUT_LIBVERSION).h

# ###---------------------------------------------------------###
#    functions related to cone net-list representation :        #
#                                                               #
#      - cns : cone net_list                                    #
#      - yag : cone extractor form transistor net-list          #
# ###---------------------------------------------------------###
CNS_L      =  -lCns$(CNS_LIBVERSION)
CNS_LIB    = libCns$(CNS_LIBVERSION).a
CNS_H      =    cns$(CNS_LIBVERSION).h

YAG_L      =  -lYag$(YAGLE_LIBVERSION)
YAG_LIB    = libYag$(YAGLE_LIBVERSION).a
YAG_H      =    yag$(YAGLE_LIBVERSION).h

XYA_L      =  -lXya$(XYA_LIBVERSION)
XYA_LIB    = libXya$(XYA_LIBVERSION).a
XYA_H      =    xya$(XYA_LIBVERSION).h

FCL_L      =  -lFcl$(FCL_LIBVERSION)
FCL_LIB    = libFcl$(FCL_LIBVERSION).a
FCL_H      =    fcl$(FCL_LIBVERSION).h

GEN_L      =  -lGen$(GEN_LIBVERSION)
GEN_LIB    = libGen$(GEN_LIBVERSION).a
GEN_H      =    gen$(GEN_LIBVERSION).h

CGV_L      =  -lCgv$(CGV_LIBVERSION)
CGV_LIB    = libCgv$(CGV_LIBVERSION).a
CGV_H      =    cgv$(CGV_LIBVERSION).h

SLB_L      =  -lSlb100
SLB_LIB    = libSlb100.a
SLB_H      =    slb100.h

# ###---------------------------------------------------------###
#    functions related to net-list representation :             #
#                                                               #
#      - rcn : data structures for resistor representation      #
#      - mlo : basic data structures                            #
#                                                               #
#      - mal : parser & driver for Al format                    #
#      - mcl : parser & driver for COMPASS net-lists            #
#      - mel : parser & driver for EDIF                         #
#      - mgl :          driver for VERILOG net-lists            #
#      - mhl :          driver for GHDL net-list                #
#      - msl : parser & driver for SPICE                        #
#      - mvl : parser & driver for VHDL net-lists               #
#                                                               #
#      - mlu : user level functions                             #
# ###---------------------------------------------------------###


RCN_L      =  -lRcn$(RCN_LIBVERSION)
RCN_LIB    = libRcn$(RCN_LIBVERSION).a
RCN_H      =    rcn$(RCN_LIBVERSION).h

MLO_L      =  -lMlo$(MLO_LIBVERSION)
MLO_LIB    = libMlo$(MLO_LIBVERSION).a
MLO_H      =    mlo$(MLO_LIBVERSION).h

MAL_L      =  -lMal$(MAL_LIBVERSION)
MAL_LIB    = libMal$(MAL_LIBVERSION).a

MCL_L      =  -lMcl$(MCL_LIBVERSION)
MCL_LIB    = libMcl$(MCL_LIBVERSION).a

MEL_L      =  -lMel$(MEL_LIBVERSION)
MEL_LIB    = libMel$(MEL_LIBVERSION).a

MHL_L      =  -lMhl$(MHL_LIBVERSION)
MHL_LIB    = libMhl$(MHL_LIBVERSION).a

SPF_L      =  -lSpf$(SPF_LIBVERSION)
SPF_LIB    = libSpf$(SPF_LIBVERSION).a
SPF_H      =    spf$(SPF_LIBVERSION).h

SPE_L      =  -lSpe$(SPE_LIBVERSION)
SPE_LIB    = libSpe$(SPE_LIBVERSION).a
SPE_H      =    spe$(SPE_LIBVERSION).h

MGL_L      =  -lMgl$(MGL_LIBVERSION)
MGL_LIB    = libMgl$(MGL_LIBVERSION).a
MGL_H      =    mgl$(MGL_LIBVERSION).h

MSL_L      =  -lMsl$(MSL_LIBVERSION)
MSL_LIB    = libMsl$(MSL_LIBVERSION).a
MSL_H      =    msl$(MSL_LIBVERSION).h

MVL_L      =  -lMvl$(MVL_LIBVERSION)
MVL_LIB    = libMvl$(MVL_LIBVERSION).a

MLU_L      =  -lMlu$(MLU_LIBVERSION)
MLU_LIB    = libMlu$(MLU_LIBVERSION).a
MLU_H      =    mlu$(MLU_LIBVERSION).h

# ###---------------------------------------------------------###
#    functions related to symbolic layout representation :      #
#                                                               #
#      - mph : basic data structures                            #
#                                                               #
#      - map : parser & driver for AP format                    #
#      - mcp : parser & driver for COMPASS Compose layout       #
#      - mmg : parser & driver for MODGEN                       #
#                                                               #
#      - mpu : user level functions                             #
# ###---------------------------------------------------------###

MPH_L      =  -lMph$(MPH_LIBVERSION)
MPH_LIB    = libMph$(MPH_LIBVERSION).a
MPH_H      =    mph$(MPH_LIBVERSION).h

MAP_L      =  -lMap408
MAP_LIB    = libMap408.a

MCP_L      =  -lMcp$(MCP_LIBVERSION)
MCP_LIB    = libMcp$(MCP_LIBVERSION).a

MMG_L      =  -lMmg403
MMG_LIB    = libMmg403.a
MMG_H      = mmg403.h

MPU_L      =  -lMpu$(MPU_LIBVERSION)
MPU_LIB    = libMpu$(MPU_LIBVERSION).a
MPU_H      =    mpu$(MPU_LIBVERSION).h

# ###---------------------------------------------------------###
#    user level functions for GENLIB                            #
# ###---------------------------------------------------------###

MGN_L      =  -lMgn330
MGN_LIB    = libMgn330.a
MGN_H      =    mgn330.h

# ###---------------------------------------------------------###
#    functions related to expression representation :           #
#                                                               #
#      - aut : utilites                                         #
#      - abl : lisp-like trees                                  #
#      - bdd : binary decision diagram                          #
#      - gef : factorized expression representation             #
# ###---------------------------------------------------------###

AUT_L      =  -lAut103
AUT_LIB    = libAut103.a
AUT_H      =    aut103.h

ABL_L      =  -lAbl103
ABL_LIB    = libAbl103.a
ABL_H      =    abl103.h

BDD_L      =  -lBdd$(BDD_LIBVERSION)
BDD_LIB    = libBdd$(BDD_LIBVERSION).a
BDD_H      =    bdd$(BDD_LIBVERSION).h

# attention il s'agit de la lib luc
# ne pas utiliser avec aut abl bdd : utiliser log102...

LOG_L      =  -llog202
LOG_LIB    = liblog202.a
LOG_H      =    log202.h

# ###---------------------------------------------------------###
#    functions related to another behavioural representation :  #
#                                                               #
#      - abe : basic data structures                            #
#      - abt : high level functions                             #
#      - abv : parser & driver for Data-Flow VHDL and user      #
#              level functions                                  #
# ###---------------------------------------------------------###

ABE_L      =  -lAbe200
ABE_LIB    = libAbe200.a
ABE_H      =    abe200.h
ABT_L      =  -lAbt200
ABT_LIB    = libAbt200.a
ABT_H      =    abt200.h
ABV_L      =  -lAbv200
ABV_LIB    = libAbv200.a
ABV_H      =    abv200.h

# ###---------------------------------------------------------###
#    functions related to simulation scheduler :                #
#                                                               #
#      - sch : simulation scheduler                             #
# ###---------------------------------------------------------###

SCH_L      =  -lSch110
SCH_LIB    = libSch110.a
SCH_H      =    sch110.h


# ###---------------------------------------------------------###
#    functions related to behavioural representation :          #
#                                                               #
#      - beh : basic data structures                            #
#      - cst : chain-like set treatment                         #
#      - bhl : high level functions                             #
#      - bvl : parser & driver for Data-Flow VHDL and user      #
#              level functions                                  #
# ###---------------------------------------------------------###

BEH_L      =  -lBeh$(BEH_LIBVERSION)
BEH_LIB    = libBeh$(BEH_LIBVERSION).a
BEH_H      =    beh$(BEH_LIBVERSION).h

CST_L      =  -lCst100
CST_LIB    = libCst100.a
CST_H      =    cst100.h

BHL_L      =  -lBhl$(BHL_LIBVERSION)
BHL_LIB    = libBhl$(BHL_LIBVERSION).a
BHL_H      =    bhl$(BHL_LIBVERSION).h

BVL_L      =  -lBvl$(BVL_LIBVERSION)
BVL_LIB    = libBvl$(BVL_LIBVERSION).a
BVL_H      =    bvl$(BVL_LIBVERSION).h

BSC_L      =  -lBsc$(BSC_LIBVERSION)
BSC_LIB    = libBsc$(BSC_LIBVERSION).a
BSC_H      =    bsc$(BSC_LIBVERSION).h

BGL_L      =  -lBgl$(BGL_LIBVERSION)
BGL_LIB    = libBgl$(BGL_LIBVERSION).a
BGL_H      =    bgl$(BGL_LIBVERSION).h

BEF_L      =  -lBef$(BEF_LIBVERSION)
BEF_LIB    = libBef$(BEF_LIBVERSION).a
BEF_H      =    bef$(BEF_LIBVERSION).h

BEG_L      =  -lBeg$(BEG_LIBVERSION)
BEG_LIB    = libBeg$(BEG_LIBVERSION).a
BEG_H      =    beg$(BEG_LIBVERSION).h

# ###---------------------------------------------------------###
#    functions related to finite state machine representation : #
#                                                               #
#       - fsm : basic data structures                           #
#       - fbh : intermediate data structures                    #
#                                                               #
#       - fvh : VHDL parser                                     #
#       - fks : KISS parser                                     #
# ###---------------------------------------------------------###

FSM_L      =  -lFsm104
FSM_LIB    = libFsm104.a
FSM_H      =    fsm104.h

FVH_L      =  -lFvh104
FVH_LIB    = libFvh104.a
FKS_L      =  -lFks104
FKS_LIB    = libFks104.a

FTL_L      =  -lFtl104
FTL_LIB    = libFtl104.a
FTL_H      =    ftl104.h

# ###---------------------------------------------------------###
#    functions related to simulation patterns representation :  #
#                                                               #
#      - pat : basic data structures                            #
#      - phl : high level functions                             #
#      - ppt : parser & driver for PAT format and user level    #
#              functions                                        #
#      - pgn : user level functions for GENPAT                  #
# ###---------------------------------------------------------###

PAT_L      =  -lPat108
PAT_LIB    = libPat108.a
PAT_H      =    pat108.h
PHL_L      =  -lPhl108
PHL_LIB    = libPhl108.a
PHL_H      =    phl108.h
PPT_L      =  -lPpt108
PPT_LIB    = libPpt108.a
PPT_H      =    ppt108.h
PGN_L      =  -lPgn103
PGN_LIB    = libPgn103.a
PGN_H      =    Pgn102.h

# ###---------------------------------------------------------###
#    functions related to physical (micron) layout :            #
#                                                               #
#      - rds : basic data structures                            #
#      - rut : user level functions                             #
#                                                               #
#      - rdf : parser & driver for CIF format                   #
#      - rgs : parser & driver for GDSII format                 #
#      - rfm : physical layout from symbolic layout             #
#      - rpr : parser for technology file                       #
#      - rwi : windowing functions                              #
#                                                               #
#      - rtl : user level functions                             #
# ###---------------------------------------------------------###

RDS_L      =  -lRds210
RDS_LIB    = libRds210.a
RDS_H      =    rds210.h

RUT_L      =  -lRut209
RUT_LIB    = libRut209.a
RUT_H      =    rut209.h

RCF_L      =  -lRcf112
RCF_LIB    = libRcf112.a
RFM_L      =  -lRfm212
RFM_LIB    = libRfm212.a
RFM_H      =    rfm212.h
RGS_L      =  -lRgs113
RGS_LIB    = libRgs113.a
RPR_L      =  -lRpr214
RPR_LIB    = libRpr214.a
RPR_H      =    rpr214.h
RWI_L      =  -lRwi110
RWI_LIB    = libRwi110.a
RWI_H      =    rwi110.h

RTL_L      =  -lRtl111
RTL_LIB    = libRtl111.a
RTL_H      =    rtl111.h

# ###---------------------------------------------------------###
#    functions related to icon representation :                 #
#                                                               #
#      - icn : basic data structures                            #
#                                                               #
#      - ica : parser & driver for Alliance icon format         #
#      - icc : parser & driver for Compass  icon format         #
#                                                               #
#      - icu : user level functions                             #
# ###---------------------------------------------------------###

ICN_L      =  -lIcn201
ICN_LIB    = libIcn201.a
ICN_H      =    icn201.h

IAC_H      =    iac201.h

ICA_L      =  -lIca201
ICA_LIB    = libIca201.a
ICC_L      =  -lIcc201
ICC_LIB    = libIcc201.a

ICU_L      =  -lIcu201
ICU_LIB    = libIcu201.a
ICU_H      =    icu201.h

# ###---------------------------------------------------------###
#    functions related to portable blocks generators :          #
#                                                               #
#      - gbs : barrel shifter                                   #
#      - gga : fast adder                                       #
#      - ggr : rom                                              #
#      - grf : register file                                    #
#      - gam : array pipelined multiplier                       #
#      - gfp : data path simple operators                       #
# ###---------------------------------------------------------###

GBS_L      =  -lGbs201
GBS_LIB    = libGbs201.a
GBS_H      =    gbs201.h

GGA_L      =  -lGga301
GGA_LIB    = libGga301.a
GGA_H      =    gga301.h

GGR_L      =  -lGgr001
GGR_LIB    = libGgr001.a
GGR_H      =    ggr001.h

GRF_L      =  -lGrf605
GRF_LIB    = libGrf605.a
GRF_H      =    grf605.h

GAM_L      =  -lGam000
GAM_LIB    = libGam000.a
GAM_H      =    gam000.h

GFP_L      =  -lGfp115
GFP_LIB    = libGfp115.a
GFP_H      =    gfp115.h

# ###---------------------------------------------------------###
#    place & route functions                                    #
# ###---------------------------------------------------------###

APR_L      =  -lApr103
APR_LIB    = libApr103.a
APR_H      =    apr103.h

# ###---------------------------------------------------------###
#    design rule checker functions                              #
# ###---------------------------------------------------------###

VRD_L      =  -lVrd303
VRD_LIB    = libVrd303.a
VRD_H      =    vrd303.h

# ###---------------------------------------------------------###
#    user level functions for FPGEN				#
# ###---------------------------------------------------------###

FGN_L      =  -lFgn115
FGN_LIB    = libFgn115.a
FGN_H        = Fgn115.h


# ###---------------------------------------------------------###
#    Timing Analysis                                            #
#                                                               #
#      - ttv : basic timing data structures                     #
#                                                               #
#      - inf : information file parser                          #
#      - elp : electrical parameters' file parser               #
#      - tas : static delay analysis functions                  #
# ###---------------------------------------------------------###

TAS_L      =  -lTas$(TAS_LIBVERSION)
TAS_LIB    = libTas$(TAS_LIBVERSION).a
TAS_H      =    tas$(TAS_LIBVERSION).h

TMA_L      =  -lTma$(TMA_LIBVERSION)
TMA_LIB    = libTma$(TMA_LIBVERSION).a
TMA_H      =    tma$(TMA_LIBVERSION).h

TTV_L      =  -lTtv$(TTV_LIBVERSION)
TTV_LIB    = libTtv$(TTV_LIBVERSION).a
TTV_H      =    ttv$(TTV_LIBVERSION).h

TUT_L      =  -lTut$(TUT_LIBVERSION)
TUT_LIB    = libTut$(TUT_LIBVERSION).a
TUT_H      =    tut$(TUT_LIBVERSION).h

INF_L      =  -lInf$(INF_LIBVERSION)
INF_LIB    = libInf$(INF_LIBVERSION).a
INF_H      =    inf$(INF_LIBVERSION).h

ELP_L      =  -lElp$(ELP_LIBVERSION)
ELP_LIB    = libElp$(ELP_LIBVERSION).a
ELP_H      =    elp$(ELP_LIBVERSION).h

TLC_L      =  -lTlc$(TLC_LIBVERSION)
TLC_LIB    = libTlc$(TLC_LIBVERSION).a
TLC_H      =    tlc$(TLC_LIBVERSION).h

TEL_L      =  -lTel101
TEL_LIB    = libTel101.a
TEL_H      =    tel101.h

TRC_L      =  -lTrc$(TRC_LIBVERSION)
TRC_LIB    = libTrc$(TRC_LIBVERSION).a
TRC_H      =    trc$(TRC_LIBVERSION).h

STM_L      =  -lStm$(STM_LIBVERSION)
STM_LIB    = libStm$(STM_LIBVERSION).a
STM_H      =    stm$(STM_LIBVERSION).h

STB_L      =  -lStb$(STB_LIBVERSION)
STB_LIB    = libStb$(STB_LIBVERSION).a
STB_H      =    stb$(STB_LIBVERSION).h

CBH_L      =  -lCbh$(CBH_LIBVERSION)
CBH_LIB    = libCbh$(CBH_LIBVERSION).a
CBH_H      =    cbh$(CBH_LIBVERSION).h

MCC_L      =  -lMcc$(MCC_LIBVERSION)
MCC_LIB    = libMcc$(MCC_LIBVERSION).a
MCC_H      =    mcc$(MCC_LIBVERSION).h

EQT_L      =  -lEqt$(EQT_LIBVERSION)
EQT_LIB    = libEqt$(EQT_LIBVERSION).a
EQT_H      =    eqt$(EQT_LIBVERSION).h

LIB_L      =  -lLib$(LIB_LIBVERSION)
LIB_LIB    = libLib$(LIB_LIBVERSION).a
LIB_H      =    lib$(LIB_LIBVERSION).h

TLF_L      =  -lTlf$(TLF_LIBVERSION)
TLF_LIB    = libTlf$(TLF_LIBVERSION).a
TLF_H      =    tlf$(TLF_LIBVERSION).h

BCK_L      =  -lBck$(BCK_LIBVERSION)
BCK_LIB    = libBck$(BCK_LIBVERSION).a
BCK_H      =    bck$(BCK_LIBVERSION).h

ZEN_L      =  -lZen$(ZEN_LIBVERSION)
ZEN_LIB    = libZen$(ZEN_LIBVERSION).a
ZEN_H      =    zen$(ZEN_LIBVERSION).h

XAL_L      =  -lXal$(XAL_LIBVERSION)
XAL_LIB    = libXal$(XAL_LIBVERSION).a
XAL_H      =    xal$(XAL_LIBVERSION).h

API_L      =  -lApi$(API_LIBVERSION)
API_LIB    = libApi$(API_LIBVERSION).a
API_H      =    api$(API_LIBVERSION).h

EFG_L      =  -lEfg100
EFG_LIB    = libEfg100.a
EFG_H      =    efg100.h

PAVO_L      =  -lPavo100
PAVO_LIB    = libPavo100.a
PAVO_H      =    Pavo100.h

GSP_L      =  -lGsp100
GSP_LIB    = libGsp100.a
GSP_H      =    gsp100.h

SIM_L      =  -lSim$(SIM_LIBVERSION)
SIM_LIB    = libSim$(SIM_LIBVERSION).a
SIM_H      =    sim$(SIM_LIBVERSION).h

AVT_L      =  -lAvt$(AVT_LIBVERSION)
AVT_LIB    = libAvt$(AVT_LIBVERSION).a
AVT_H      =    avt$(AVT_LIBVERSION).h

VCD_L      =  -lVcd$(VCD_LIBVERSION)
VCD_LIB    = libVcd$(VCD_LIBVERSION).a
VCD_H      =    vcd$(VCD_LIBVERSION).h

SDC_L      =  -lSdc$(SDC_LIBVERSION)
SDC_LIB    = libSdc$(SDC_LIBVERSION).a
SDC_H      =    Sdc$(SDC_LIBVERSION).h

SDLR_L     = -lscheduler$(SDLR_LIBVERSION)
SDLR_LIB   = libscheduler$(SDLR_LIBVERSION).a
SDLR_H      =    scheduler$(SDLR_LIBVERSION).h

# API libraries (for debug and profile only)

APIMBK_LIB          = libApiMbk.a
APIGEN_BUILTINS_LIB = libApiGenBuiltins.a
APIGENIUS_LIB       = libApiGenius.a
APISTM_LIB          = libApiStm.a
APIBEG_LIB          = libApiBeg.a
APIFCL_LIB          = libApiFcl.a
APIMBKSPICE_LIB     = libApiMbkspice.a
APISIM_LIB          = libApiSim.a
APITAS_LIB          = libApiTas.a
APITMA_LIB          = libApiTma.a
APIYAG_LIB          = libApiYag.a
APITRC_LIB          = libApiTrc.a
APITTV_LIB          = libApiTtv.a
APIPOWER_LIB        = libApiPower.a
APIAMS_LIB          = libApiAms.a
APIDATABASE_LIB     = libApiDatabase.a
APICTK_LIB          = libApiCtk.a
APIINF_LIB          = libApiInf.a
APIHPE_LIB          = libApiHpe.a
APISTB_LIB          = libApiStb.a
APICNS_LIB          = libApiCns.a
APILIB_LIB          = libApiLib.a

APIMBK_L            = -lApiMbk
APIGEN_BUILTINS_L   = -lApiGenBuiltins
APIGENIUS_L         = -lApiGenius
APISTM_L            = -lApiStm
APIBEG_L            = -lApiBeg
APIFCL_L            = -lApiFcl
APIMBKSPICE_L       = -lApiMbkspice
APISIM_L            = -lApiSim
APITRC_L            = -lApiTrc
APITTV_L            = -lApiTtv
APIPOWER_L          = -lApiPower
APIDATABASE_L       = -lApiDatabase
APICTK_L            = -lApiCtk
APIINF_L            = -lApiInf
APIAMS_L            = -lApiAms
APIHPE_L            = -lApiHpe
APITAS_L            = -lApiTas
APIYAG_L            = -lApiYag
APITMA_L            = -lApiTma
APISTB_L            = -lApiStb
APICNS_L            = -lApiCns
APILIB_L            = -lApiLib

APIALL_L            = -lApiMbk \
                      -lApiGenBuiltins \
                      -lApiGenius \
                      -lApiStm \
                      -lApiBeg \
                      -lApiFcl \
                      -lApiMbkspice \
                      -lApiSim \
                      -lApiTrc \
                      -lApiTtv \
                      -lApiPower \
                      -lApiDatabase \
                      -lApiCtk \
                      -lApiInf \
                      -lApiHpe \
                      -lApiYag \
                      -lApiTma \
                      -lApiTas \
                      -lApiStb \
                      -lApiCns \
                      -lApiAms \
                      -lApiLib
                      

# end of libraries.mk

