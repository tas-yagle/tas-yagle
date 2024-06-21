# makefile for sim
# date    : 15/05/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

SIM_H_LIB   = sim_util.h        \
              sim_input.h       \
              sim_measure.h     \
              sim_model.h       \
              sim_parameter.h   \
              sim_drive.h       \
              sim_drive_spice.h \
              sim_ic.h          \
              sim_apply_pat.h   \
              sim_parse.h       \
              sim_noise.h       \
              sim_genspi.h      \
              sim_compute.h     \
              sim_meas.h        \
              sim_message.h     \
              sim_parse_titan_ppr.h \
              sim_parse_eldo_chi.h \
              sim_parse_hspice.h \
              sim_parse_ngspice_stdout.h
			  
SIM_OBJ_LIB = $(SIM_H_LIB:%.h=%.o)

LOCAL_INC = -I$(TARGET_INCLUDE)
			
LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DSIM_H='"$(SIM_H)"' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DEFG_H='<$(EFG_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMSL_H='<$(MSL_H)>'

# lib

all : lib 

lib : $(TARGET_INCLUDE)/$(SIM_H) $(TARGET_LIB)/$(SIM_LIB)

headers : $(TARGET_INCLUDE)/$(SIM_H)

$(TARGET_LIB)/$(SIM_LIB) : $(TARGET_INCLUDE)/$(SIM_H) $(SIM_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(SIM_LIB) $(SIM_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(SIM_LIB)
	
$(TARGET_INCLUDE)/$(SIM_H) : simx.h $(SIM_H_LIB)
	$(CP) simx.h $(TARGET_INCLUDE)/$(SIM_H)
	$(AWK) '{if ($$1 == "extern") print; next}' sim_*.h >> $(TARGET_INCLUDE)/$(SIM_H)
	$(ECHO) "#endif" >> $(TARGET_INCLUDE)/$(SIM_H)
	
.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(SIM_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(SIM_H)

clean : 
	-$(RM) $(SIM_OBJ_LIB)
	
