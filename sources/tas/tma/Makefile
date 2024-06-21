# makefile for tma
# date    : 26/06/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

TMA_OBJ_LIB = tma_util.o tma_files.o tma_blackbox.o

TMA_HEADER   = tma_lib.h

LOCAL_INC = -I. -I$(TARGET_INCLUDE) 

LOCAL_CFLAGS      = $(CFLAGS)
LOCAL_CPPFLAGS    = $(CPPFLAGS) -c $(LOCAL_INC) \
                  -DAVERTEC \
                  -D$(AVERTEC_LICENSE) \
                  -DSIM_H='<$(SIM_H)>' \
                  -DGSP_H='<$(GSP_H)>' \
                  -DEFG_H='<$(EFG_H)>' \
                  -DMUT_H='<$(MUT_H)>' \
                  -DMLO_H='<$(MLO_H)>' \
                  -DAPI_H='<$(API_H)>' \
                  -DMLU_H='<$(MLU_H)>' \
                  -DMSL_H='<$(MSL_H)>' \
                  -DGEN_H='<$(GEN_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBEG_H='<$(BEG_H)>' \
                  -DINF_H='<$(INF_H)>' \
                  -DAVT_H='<$(AVT_H)>' \
                  -DSTB_H='<$(STB_H)>' \
                  -DELP_H='<$(ELP_H)>' \
                  -DCNS_H='<$(CNS_H)>' \
                  -DFCL_H='<$(FCL_H)>' \
                  -DYAG_H='<$(YAG_H)>' \
                  -DRCN_H='<$(RCN_H)>' \
                  -DTLC_H='<$(TLC_H)>' \
                  -DTRC_H='<$(TRC_H)>' \
                  -DSTM_H='<$(STM_H)>' \
                  -DTTV_H='<$(TTV_H)>' \
                  -DMCC_H='<$(MCC_H)>' \
                  -DEQT_H='<$(EQT_H)>' \
                  -DTAS_H='"$(TAS_H)"' \
                  -DTLF_H='<$(TLF_H)>' \
                  -DLOG_H='<$(LOG_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBHL_H='<$(BHL_H)>' \
                  -DBEF_H='<$(BEF_H)>' \
                  -DBVL_H='<$(BVL_H)>' \
                  -DCBH_H='<$(CBH_H)>' \
                  -DBVL_H='<$(BVL_H)>' \
                  -DLIB_H='<$(LIB_H)>' \
                  -DTUT_H='<$(TUT_H)>' \
                  -DTMA_H='<$(TMA_H)>'

all : $(TARGET_LIB)/$(TMA_LIB) 

headers : $(TARGET_INCLUDE)/$(TMA_H)

$(TARGET_LIB)/$(TMA_LIB) : $(TARGET_INCLUDE)/$(TMA_H) $(TMA_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(TMA_LIB) $(TMA_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(TMA_LIB)
	
$(TARGET_INCLUDE)/$(TMA_H) :
	$(CP) $(TMA_HEADER) $(TARGET_INCLUDE)/$(TMA_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(TMA_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(TMA_H)

clean :
	-$(RM) -f $(TMA_OBJ_LIB)
