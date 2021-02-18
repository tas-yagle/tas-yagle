include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_API_INCLUDE) -I$(TARGET_INCLUDE) 

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DYAGLE_TOKEN\
                 -DAVT_H='"$(AVT_H)"' \
                 -DAPI_H='"$(API_H)"' \
                 -DMLU_H='"$(MLU_H)"' \
                 -DMLO_H='"$(MLO_H)"' \
                 -DELP_H='"$(ELP_H)"' \
                 -DLOG_H='"$(LOG_H)"' \
                 -DBVL_H='"$(BVL_H)"' \
                 -DBEH_H='"$(BEH_H)"' \
                 -DBEG_H='"$(BEG_H)"' \
                 -DBHL_H='"$(BHL_H)"' \
                 -DBEF_H='"$(BEF_H)"' \
                 -DMUT_H='"$(MUT_H)"' \
        		 -DTRC_H='"$(TRC_H)"' \
                 -DMSL_H='"$(MSL_H)"' \
                 -DRCN_H='"$(RCN_H)"' \
                 -DCNS_H='"$(CNS_H)"' \
                 -DYAG_H='"$(YAG_H)"' \
                 -DINF_H='"$(INF_H)"' \
                 -DGEN_H='"$(GEN_H)"' \
                 -DTAS_H='"$(TAS_H)"' \
                 -DTRC_H='"$(TRC_H)"' \
                 -DSTM_H='"$(STM_H)"' \
                 -DTTV_H='"$(TTV_H)"' \
                 -DEQT_H='"$(EQT_H)"' \
                 -DMCC_H='"$(MCC_H)"' \
                 -DSIM_H='"$(SIM_H)"' \
                 -DGSP_H='"$(GSP_H)"' \
                 -DEFG_H='"$(EFG_H)"' \
                 -DCBH_H='"$(CBH_H)"' \
                 -DFCL_H='"$(FCL_H)"'

TCL_WRAPPER = yagle_TCL_wrap.c
OBJ = yagle_API.o

# Rules

# Static linking for Purify
$(TARGET_LIB)/$(APIYAG_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(APIYAG_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(APIYAG_LIB)
	cp yagle_API.h $(TARGET_API_INCLUDE)

# Shared lib
dynamic_link: $(OBJ) 
	$(SCC) $(CFLAGS) -shared $(OBJ) yagle_TCL_wrap.o -o $(TARGET_API_LIB)/yagle_API.so
	cp yagle_API.h $(TARGET_API_INCLUDE)

# TCL wrapper
$(TCL_WRAPPER): yag.i
	$(SWIG) -o $(TCL_WRAPPER) -tcl8 yag.i 

# Objects
.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

clean :
	-@$(RM) -f $(WRAPPER) $(TCL_WRAPPER) $(OBJ) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_API_LIB)/yagle_API.so $(TARGET_API_INCLUDE)/yagle_API.h > /dev/null 2>&1
	-@$(RM) -f $(TARGET_LIB)/$(APIYAG_LIB) > /dev/null 2>&1
