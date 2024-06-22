# makefile for mcc
# date    : 26/06/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

MCCGENELP = genelp

MCC_OBJ_LIB = mcc_util.o mcc_optim.o mcc_curv.o mcc_fit.o mcc_debug.o mcc_genspi.o mcc_drvelp.o mcc_ivmodels.o mcc_genelp.o mcc_parse_cfg.o

MCC_MOD_OBJ = lex.mcc_mod.o mcc_mod.tab.o mcc_mod_util.o mcc_mod_spice.o \
              mcc_mod_bsim3v3.o mcc_mod_bsim4.o mcc_mod_mos2.o mcc_mod_mm9.o mcc_tsort.o \
			  mcc_mod_psp.o mcc_mod_psp_interface.o mcc_mod_ext.o

MCC_MOD_MAIN_OBJ = mcc_main.o

MCC_MOD_BIN = parsetech

MCC_HEADER   = mcc_lib.h

READ_L = $(EDITLINELIBS)

LOCAL_INC = $(INCLUDE64BIT) \
            -I$(TARGET_INCLUDE)

LOCAL_CFLAGS      = $(CFLAGS)
LOCAL_CPPFLAGS    = $(CPPFLAGS) $(LOCAL_INC) \
                  -DAVERTEC \
		  -D$(AVERTEC_OS) \
                  -D$(AVERTEC_LICENSE) \
                  -DEQT_H='"$(EQT_H)"' \
                  -DLOG_H='"$(LOG_H)"' \
                  -DMCC_H='"$(MCC_H)"' \
                  -DTTV_H='<$(TTV_H)>' \
                  -DSTM_H='<$(STM_H)>' \
                  -DSIM_H='<$(SIM_H)>' \
                  -DCNS_H='<$(CNS_H)>' \
                  -DGSP_H='<$(GSP_H)>' \
                  -DEFG_H='<$(EFG_H)>' \
                  -DCBH_H='<$(CBH_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBEG_H='<$(BEG_H)>' \
                  -DELP_H='<$(ELP_H)>' \
                  -DINF_H='<$(INF_H)>' \
                  -DAVT_H='<$(AVT_H)>' \
                  -DMLO_H='<$(MLO_H)>' \
                  -DMLU_H='<$(MLU_H)>' \
                  -DMSL_H='<$(MSL_H)>' \
                  -DAPI_H='<$(API_H)>' \
                  -DRCN_H='<$(RCN_H)>' \
                  -DMUT_H='<$(MUT_H)>'

LOCAL_YFLAGS = $(YACCFLAGS) -d -t

LOCAL_LIB  = $(FLEXOBJS) $(STATIC_LIB) \
             -L$(TARGET_LIB) \
             -L$(GNU_LIB) \
             $(MCC_L) \
             $(SIM_L) \
             $(GSP_L) \
             $(EFG_L) \
             $(EQT_L) \
             $(CBH_L) \
             $(LIB_L) \
             $(TTV_L) \
             $(STB_L) \
             $(TTV_L) \
             $(STM_L) \
             $(GEN_L) \
             $(CNS_L) \
             $(BEG_L) \
             $(BHL_L) \
             $(BEH_L) \
             $(LOG_L) \
             $(TRC_L) \
             $(TLC_L) \
             $(ELP_L) \
             $(MLU_L) \
             $(MCL_L) \
             $(MAL_L) \
             $(MSL_L) \
             $(MGL_L) \
             $(MVL_L) \
             $(MEL_L) \
             $(MHL_L) \
             $(SPF_L) \
             $(SPE_L) \
             $(MLO_L) \
             $(RCN_L) \
             $(API_L) \
             $(MUT_L) \
             $(INF_L) \
             $(AVT_L) \
             $(RPC_L) \
             $(READ_L) \
             -lm $(FLEXLIBS) -ldl

all : lib 

lib : $(TARGET_INCLUDE)/$(MCC_H) $(TARGET_LIB)/$(MCC_LIB) 

headers : $(TARGET_INCLUDE)/$(MCC_H)

link:
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(MCC_GENCURV_OBJ) -o $(TARGET_BIN)/$(MCCGENELP) $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/$(MCCGENELP)


$(TARGET_INCLUDE)/$(MCC_H) : $(MCC_HEADER)
	$(CP) $(MCC_HEADER) $(TARGET_INCLUDE)/$(MCC_H)

bin : $(TARGET_BIN)/$(MCC_MOD_BIN)

$(TARGET_BIN)/$(MCC_MOD_BIN) : $(MCC_MOD_OBJ) $(MCC_MOD_MAIN_OBJ)
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(MCC_MOD_MAIN_OBJ) -o $(TARGET_BIN)/$(MCC_MOD_BIN) $(MCC_MOD_OBJ) $(LOCAL_LIB)


$(TARGET_LIB)/$(MCC_LIB) : $(MCC_OBJ_LIB) $(MCC_MOD_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MCC_LIB) $(MCC_OBJ_LIB) $(MCC_MOD_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(MCC_LIB)
	$(CP) $(MCC_HEADER) $(TARGET_INCLUDE)/$(MCC_H)

mcc_mod.tab.h : mcc_mod.tab.c

mcc_mod.tab.c : mcc_mod.y
	$(YACC) $(LOCAL_YFLAGS) -b mcc_mod -p mcc_mod mcc_mod.y

lex.mcc_mod.c : mcc_mod.l mcc_mod.tab.h
	$(LEX) $(LEXFLAGS) -Pmcc_mod mcc_mod.l

.c.o :
	$(CC) $(LOCAL_CFLAGS) -c $(LOCAL_CPPFLAGS) $<

realclean : clean
	-$(RM) -f $(TARGET_BIN)/$(MCCGENELP)
	-$(RM) -f $(TARGET_BIN)/$(MCC_MOD_BIN)
	-$(RM) -f $(TARGET_INCLUDE)/$(MCC_H)
	-$(RM) -f $(TARGET_LIB)/$(MCC_LIB)

clean :
	-$(RM) -f $(MCC_OBJ_LIB) $(MCC_GENCURV_OBJ) 
	-$(RM) -f $(MCC_MOD_OBJ) $(MCC_MOD_MAIN_OBJ)
	-$(RM) lex.mcc_mod.c mcc_mod.tab.c mcc_mod.tab.h mcc_mod.output

