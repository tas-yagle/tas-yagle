# makefile for val
# date    : 26/06/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL       = /bin/sh
COMPIL_FILE = "COMPIL"

VAL_OBJ     = val_main.o
TEST_OBJ    = test_main.o
TTVDIFF_OBJ = ttvdiff.o ttv_comp.o
TTVREN_OBJ  = ttvren.o ttv_ren.o

LOCAL_INC   = -I$(TARGET_INCLUDE)

LOCAL_CFLAGS      = $(CFLAGS)
LOCAL_CPPFLAGS    = $(CPPFLAGS) -c $(LOCAL_INC) \
                  -DAVERTEC \
                  -D$(AVERTEC_LICENSE) \
                  -DSIM_H='<$(SIM_H)>' \
                  -DCNS_H='<$(CNS_H)>' \
                  -DGSP_H='<$(GSP_H)>' \
                  -DEFG_H='<$(EFG_H)>' \
                  -DAPI_H='<$(API_H)>' \
                  -DMSL_H='<$(MSL_H)>' \
                  -DLOG_H='<$(LOG_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBEG_H='<$(BEG_H)>' \
                  -DMLU_H='<$(MLU_H)>' \
                  -DEQT_H='<$(EQT_H)>' \
                  -DCBH_H='<$(CBH_H)>' \
                  -DAVT_H='<$(AVT_H)>' \
                  -DMUT_H='<$(MUT_H)>' \
                  -DMLO_H='<$(MLO_H)>' \
                  -DRCN_H='<$(RCN_H)>' \
                  -DELP_H='<$(ELP_H)>' \
                  -DINF_H='<$(INF_H)>' \
                  -DSIM_H='<$(SIM_H)>' \
                  -DSTM_H='<$(STM_H)>' \
                  -DTTV_H='<$(TTV_H)>'

LOCAL_LIB  = $(FLEXOBJS) -L$(TARGET_LIB) $(STATIC_LIB)\
             $(TTV_L) \
             $(STB_L) \
             $(TTV_L) \
             $(STM_L) \
             $(GEN_L) \
             $(SIM_L) \
             $(TRC_L) \
             $(TLC_L) \
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
             $(STM_L) \
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
             $(EQT_L) \
             $(LOG_L) \
             $(API_L) \
             $(INF_L) \
             $(AVT_L) \
             $(MUT_L) \
             $(RPC_L) \
             $(TCL_L) \
             -lpthread -lm $(FLEXLIBS) -ldl $(PSP_L)

all : init $(TARGET_BIN)/ttvdiff $(TARGET_BIN)/ttvren

extras : $(TARGET_BIN)/val  $(TARGET_BIN)/ttvtest

$(TARGET_BIN)/ttvdiff : $(TTVDIFF_OBJ) 
	@echo $(TCL_L)
	@echo "[VAL make] -- CC    " $(TTVDIFF_OBJ)
	@echo  $(CC) $(LOCAL_CFLAGS) $(TTVDIFF_OBJ) -o $@ $(LOCAL_LIB) \
	>> $(COMPIL_FILE)
	@$(CPLUSPLUS) $(LOCAL_CFLAGS) $(TTVDIFF_OBJ) -o $@ $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/ttvdiff
    
$(TARGET_BIN)/ttvren : $(TTVREN_OBJ) 
	@echo "[VAL make] -- CC    " $(TTVREN_OBJ)
	@echo  $(CC) $(LOCAL_CFLAGS) $(TTVREN_OBJ) -o $@ $(LOCAL_LIB) \
	>> $(COMPIL_FILE)
	@$(CPLUSPLUS) $(LOCAL_CFLAGS) $(TTVREN_OBJ) -o $@ $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/ttvren

$(TARGET_BIN)/val : $(VAL_OBJ) 
	@echo "[VAL make] -- CC    " $(VAL_OBJ)
	@echo  $(CC) $(LOCAL_CFLAGS) $(VAL_OBJ) -o $@ $(LOCAL_LIB) \
	>> $(COMPIL_FILE)
	@$(CPLUSPLUS) $(LOCAL_CFLAGS) $(VAL_OBJ) -o $@ $(LOCAL_LIB)

$(TARGET_BIN)/ttvtest : $(TEST_OBJ) 
	@echo "[VAL make] -- CC    " $(TEST_OBJ)
	@echo  $(CC) $(LOCAL_CFLAGS) $(TEST_OBJ) -o $@ $(LOCAL_LIB) \
	>> $(COMPIL_FILE)
	@$(CPLUSPLUS) $(LOCAL_CFLAGS) $(TEST_OBJ) -o $@ $(LOCAL_LIB)

.c.o :
	@echo "[VAL make] -- CC    " $<
	@echo $(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $< \
	>> $(COMPIL_FILE)
	@$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

init :
	@echo "" > $(COMPIL_FILE)

info :
	@echo "[VAL make] -- Info"
	@echo "       CFLAGS    :" $(LOCAL_CFLAGS)
	@echo "       CPPFLAGS  :" $(LOCAL_CPPFLAGS)
	@echo "       LEXFLAGS  :" $(LOCAL_LEXFLAGS)
	@echo "       YACCFLAGS :" $(LOCAL_YACCFLAGS)
	@echo "       ARFLAGS   :" $(LOCAL_ARFLAGS)
	@echo "       TARGETLIB :" $(TARGET_LIB)
	@echo "       TARGETBIN :" $(TARGET_BIN)
	@echo "       TARGETINC :" $(TARGET_INCLUDE)
	@echo "       CC        :" $(CC)
	@echo "       YACC      :" $(YACC)
	@echo "       LEX       :" $(LEX)
	@echo "       AR        :" $(AR)
	@echo "       RANLIB    :" $(RANLIB)

realclean : clean
	@echo "[VAL make] -- Global cleanning :" val ttvtest ttvdiff ttvren
	@-$(RM) -f $(TARGET_BIN)/val $(TARGET_BIN)/ttvtest
	@-$(RM) -f $(TARGET_BIN)/ttvdiff
	@-$(RM) -f $(TARGET_BIN)/ttvren

clean :
	@-$(RM) -f $(COMPIL_FILE)
	@echo "[VAL make] -- Local  cleanning :" $(VAL_OBJ) $(TEST_OBJ) $(TTVDIFF_OBJ) $(TTVREN_OBJ)
	@-$(RM) -f $(VAL_OBJ) $(TEST_OBJ)
	@-$(RM) -f $(TTVDIFF_OBJ)
	@-$(RM) -f $(TTVREN_OBJ) $(COMPIL_FILE)

