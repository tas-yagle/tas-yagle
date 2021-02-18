include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_INCLUDE) 

AVTCMI_LIB = libAVTCMI.so

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DBASIC_TOKEN\
                  -DEQT_H='<$(EQT_H)>' \
                  -DLOG_H='<$(LOG_H)>' \
                  -DMCC_H='<$(MCC_H)>' \
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

OBJ = mcc_mod_cmi.o mcc_mod_cmi_interface.o

$(TARGET_API_LIB)/$(AVTCMI_LIB): $(OBJ)
	$(SCC) -shared $(OBJ) -o $(TARGET_API_LIB)/$(AVTCMI_LIB)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

clean :
	-@$(RM) -f $(TARGET_API_LIB)/$(AVTCMI_LIB) $(OBJ) > /dev/null 2>&1
