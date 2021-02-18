include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_API_INCLUDE) -I$(TARGET_INCLUDE) 

WRAP = $(TARGET_BIN)/wrap -silent

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DTIMING_TOKEN\
                 -DAVT_H='<$(AVT_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DSIM_H='<$(SIM_H)>' \
                 -DAPI_H='<$(API_H)>' \
				 -DSTM_H='<$(STM_H)>' \
				 -DBEH_H='<$(BEH_H)>' \
				 -DBEG_H='<$(BEG_H)>' \
				 -DLOG_H='<$(LOG_H)>' \
				 -DCNS_H='<$(CNS_H)>' \
				 -DINF_H='<$(INF_H)>' \
				 -DCBH_H='<$(CBH_H)>' \
				 -DGSP_H='<$(GSP_H)>' \
				 -DEFG_H='<$(EFG_H)>' \
				 -DGEN_H='<$(GEN_H)>' \
				 -DEQT_H='<$(EQT_H)>' \
				 -DMSL_H='<$(MSL_H)>'

WRAPPER = stm_API_wrap.c
OBJ = stm_API_wrap.o stm_API_l.o stm_API_cache.o 

GENERATED = stm_API_l.c

$(TARGET_LIB)/$(APISTM_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(APISTM_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(APISTM_LIB)
	cp stm_API.h $(TARGET_API_INCLUDE)

dynamic_link: $(OBJ)
	$(SCC) -shared $(OBJ) -o $(TARGET_API_LIB)/stm_API.so
	cp stm_API.h $(TARGET_API_INCLUDE)

$(WRAPPER): stm_API.h
	$(WRAP) -p $(TARGET_API_MAN) stm_API.h
	@$(RM) -f gmon.out

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

%_l.c : %.c $(TARGET_BIN)/$(LICENSE_API)
	$(TARGET_BIN)/$(LICENSE_API) $< API_TEST_TOKEN > $@

clean :
	-@$(RM) -f $(WRAPPER) $(OBJ) $(GENERATED) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_API_LIB)/stm_API.so $(TARGET_API_INCLUDE)/stm_API.h > /dev/null 2>&1
	-@$(RM) -f $(TARGET_LIB)/$(APISTM_LIB) > /dev/null 2>&1
