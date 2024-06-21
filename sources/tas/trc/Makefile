# makefile for trc libraries
# date    : 15/02/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

TRC_OBJ = trc_rcx.o trc_drvrcx.o trc_prsrcx.o trc_util.o trc_awe.o trc_awematrice.o trc_awepolynomes.o trc_awe_generalized_moment.o trc_fifo.o trc_hier.o trc_delay.o trc_rcxloins.o trc_cache.o trc_noise.o trc_delayloop.o trc_capa.o trc_spice.o trc_rcxi.o trc_timings.o trc_piload_generalized.o

TRC_INC = trc_awe.h trc_awematrice.h trc_awepolynomes.h trc_cache.h trc_capa.h trc_delay.h trc_delayloop.h trc_drvrcx.h trc_fifo.h trc_hier.h trc_noise.h trc_prsrcx.h trc_rcx.h trc_rcxloins.h trc_util.h trc_spice.h trc_rcxi.h trc_timings.h trc_piload_generalized.h 

LOCAL_INC = -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -D$(AVERTEC_OS) \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DTLC_H='<$(TLC_H)>' \
                 -DTRC_H='"$(TRC_H)"' \
                 -DAVT_H='"$(AVT_H)"' \
                 -DSTM_H='"$(STM_H)"' \
                 -DELP_H='"$(ELP_H)"' \
                 -DAPI_H='"$(API_H)"' \
                 -DSIM_H='"$(SIM_H)"' \
                 -DBEH_H='"$(BEH_H)"' \
                 -DLOG_H='"$(LOG_H)"' \
                 -DCNS_H='"$(CNS_H)"' \
                 -DGSP_H='"$(GSP_H)"' \
                 -DEFG_H='"$(EFG_H)"' \
                 -DMSL_H='"$(MSL_H)"' \
                 -DMLU_H='"$(MLU_H)"' \
                 -DBEG_H='"$(BEG_H)"' \
                 -DMCC_H='"$(MCC_H)"' \
                 -DEQT_H='"$(EQT_H)"' \
                 -DCBH_H='"$(CBH_H)"' \
                 -DINF_H='"$(INF_H)"'

all : $(TARGET_INCLUDE)/$(TRC_H) $(TARGET_LIB)/$(TRC_LIB)

headers : $(TARGET_INCLUDE)/$(TRC_H)

$(TARGET_LIB)/$(TRC_LIB) : $(TRC_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(TRC_LIB) $(TRC_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(TRC_LIB)

$(TARGET_INCLUDE)/$(TRC_H) : trc_lib.h $(TRC_INC)
	( echo "/* This is a generated file. Do not edit */" ; echo ; cat trc_lib.h ; ( for FILE in $(TRC_INC) ; do echo "// Externals from file $$FILE" ; echo ; cat $$FILE | grep "^extern" ; echo ; done ) ; echo "#endif" ) > $(TARGET_INCLUDE)/$(TRC_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(TRC_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(TRC_H)
 
clean :
	-$(RM) -f $(TRC_OBJ)
