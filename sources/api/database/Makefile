include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_API_INCLUDE) -I$(TARGET_INCLUDE) 

WRAP = $(TARGET_BIN)/wrap -silent

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DBASIC_TOKEN\
                 -DAVT_H='<$(AVT_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DBEF_H='<$(BEF_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
				 -DTRC_H='<$(TRC_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DYAG_H='<$(YAG_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DGEN_H='<$(GEN_H)>' \
                 -DTAS_H='<$(TAS_H)>' \
                 -DTRC_H='<$(TRC_H)>' \
                 -DSTM_H='<$(STM_H)>' \
                 -DTTV_H='<$(TTV_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DMCC_H='<$(MCC_H)>' \
                 -DFCL_H='<$(FCL_H)>'

WRAPPER = database_API_wrap.c
OBJ = database_l.o database_API_wrap.o
GENERATED = database_l.c

SOURCES_LIB=$(OBJ:.o=.c)

$(TARGET_LIB)/$(APIDATABASE_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(APIDATABASE_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(APIDATABASE_LIB)
	cp database_API.h $(TARGET_API_INCLUDE)

dynamic_link: $(OBJ) 
	$(SCC) $(CFLAGS) -shared $(OBJ) -o $(TARGET_API_LIB)/database_API.so
	cp database_API.h $(TARGET_API_INCLUDE)

$(WRAPPER): database_API.h
	$(WRAP) -p $(TARGET_API_MAN) database_API.h
	@$(RM) -f gmon.out

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

%_l.c : %.c $(TARGET_BIN)/$(LICENSE_API)
	$(TARGET_BIN)/$(LICENSE_API) $< API_TEST_TOKEN > $@

clean :
	-@$(RM) -f $(WRAPPER) $(OBJ) $(GENERATED) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_API_LIB)/database_API.so $(TARGET_API_INCLUDE)/database_API.h > /dev/null 2>&1
	-@$(RM) -f $(TARGET_LIB)/$(APIDATABASE_LIB) > /dev/null 2>&1

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM  $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

