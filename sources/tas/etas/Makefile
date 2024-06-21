# makefile for etas
# date    : 26/06/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

ETAS_OBJ = etas.o

READ_L = -leditline
TERM_L = -ltermcap

LOCAL_INC = $(INCLUDE64BIT) \
            -I$(TARGET_INCLUDE) \

LOCAL_CFLAGS      = $(CFLAGS)

LOCAL_CPPFLAGS    = $(CPPFLAGS) -c $(LOCAL_INC) \
                  -DAVERTEC \
                  -D$(AVERTEC_LICENSE) \
                  -DCNS_H='<$(CNS_H)>' \
                  -DGSP_H='<$(GSP_H)>' \
                  -DEFG_H='<$(EFG_H)>' \
                  -DMSL_H='<$(MSL_H)>' \
                  -DAVT_H='<$(AVT_H)>' \
                  -DEQT_H='<$(EQT_H)>' \
                  -DMUT_H='<$(MUT_H)>' \
                  -DRCN_H='<$(RCN_H)>' \
                  -DMLO_H='<$(MLO_H)>' \
                  -DMLU_H='<$(MLU_H)>' \
                  -DCBH_H='<$(CBH_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBEG_H='<$(BEG_H)>' \
                  -DLOG_H='<$(LOG_H)>' \
                  -DELP_H='<$(ELP_H)>' \
                  -DINF_H='<$(INF_H)>' \
                  -DAPI_H='<$(API_H)>' \
                  -DSIM_H='<$(SIM_H)>' \
                  -DSTM_H='<$(STM_H)>' \
                  -DTTV_H='<$(TTV_H)>'

LOCAL_LIB  = $(FLEXOBJS) $(STATIC_LIB) \
             -L$(TARGET_LIB) \
             -L$(GNU_LIB) \
             $(TTV_L) \
             $(STB_L) \
             $(TTV_L) \
             $(STM_L) \
             $(GEN_L) \
             $(TRC_L) \
             $(TLC_L) \
             $(SIM_L) \
             $(ELP_L) \
             $(MLU_L) \
             $(MCL_L) \
             $(MAL_L) \
             $(MSL_L) \
             $(MGL_L) \
             $(MVL_L) \
             $(MEL_L) \
             $(MHL_L) \
             $(MLO_L) \
             $(MCC_L) \
             $(ELP_L) \
             $(EFG_L) \
             $(GSP_L) \
             $(CBH_L) \
             $(LIB_L) \
             $(BHL_L) \
             $(BEH_L) \
             $(CNS_L) \
             $(SPF_L) \
             $(SPE_L) \
             $(RCN_L) \
             $(API_L) \
             $(EQT_L) \
             $(LOG_L) \
             $(INF_L) \
             $(AVT_L) \
             $(MUT_L) \
             $(READ_L) \
             $(RPC_L) \
             $(TERM_L) \
             -lm  $(FLEXLIBS) -ldl

$(TARGET_BIN)/etas : $(ETAS_OBJ) 
	$(CC) $(LOCAL_CFLAGS) $(ETAS_OBJ) -o $(TARGET_BIN)/etas $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/etas

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

realclean : clean
	-$(RM) -f $(TARGET_BIN)/etas

clean :
	-$(RM) -f $(ETAS_OBJ) 

