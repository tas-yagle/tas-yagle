# makefile for lib
# date    : 19/04/02

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LIB_HEADER   = lib_lib.h

SHELL = /bin/sh

LIB_OBJ = lib_main.o 
LIB_OBJ_LIB = lib_parse.o lex.lib.o lib.tab.o lib_drive.o lib_env.o lib_global.o lib_util.o lib_func.o lib_timing.o




LIB_BIN = lib

LOCAL_INC = -I$(TARGET_INCLUDE)
			
LOCAL_LIB  = -L$(TARGET_LIB) \
               $(AVT_L) \
               $(LIB_L) \
               $(TUT_L) \
               $(TTV_L) \
               $(STM_L) \
               $(TRC_L) \
               $(TLC_L) \
               $(ELP_L) \
               $(EQT_L) \
               $(MCC_L) \
               $(INF_L) \
               $(CBH_L) \
               $(BEF_L) \
               $(BVL_L) \
               $(BGL_L) \
               $(BHL_L) \
               $(BEH_L) \
               $(LOG_L) \
               $(MLU_L) \
               $(MCL_L) \
               $(MAL_L) \
               $(MSL_L) \
               $(MGL_L) \
               $(MVL_L) \
               $(MEL_L) \
               $(MHL_L) \
               $(MLO_L) \
               $(SPF_L) \
               $(SPE_L) \
               $(RCN_L) \
               $(MUT_L) \
               -lm 

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DTMA_H='<$(TMA_H)>' \
                 -DSIM_H='<$(SIM_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DEFG_H='<$(EFG_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DLIB_H='"$(LIB_H)"' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DTTV_H='<$(TTV_H)>' \
                 -DSTM_H='<$(STM_H)>' \
                 -DMCC_H='<$(MCC_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DTRC_H='<$(TRC_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DBEF_H='<$(BEF_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DTUT_H='<$(TUT_H)>'
				

LOCAL_YFLAGS = $(YACCFLAGS) -d 

all : $(TARGET_INCLUDE)/$(LIB_H) $(TARGET_LIB)/$(LIB_LIB)

headers : $(TARGET_INCLUDE)/$(LIB_H)

$(TARGET_LIB)/$(LIB_LIB) : $(LIB_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(LIB_LIB) $(LIB_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(LIB_LIB)

$(TARGET_INCLUDE)/$(LIB_H) :
	$(CP) $(LIB_HEADER) $(TARGET_INCLUDE)/$(LIB_H)
	
lib.tab.h : lib.tab.c

lib.tab.c : lib.y
	$(YACC) $(LOCAL_YFLAGS) -b lib -p lib lib.y

lex.lib.c : lib.l lib.tab.h
	$(LEX) $(LEXFLAGS) -Plib lib.l

.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

test : $(TARGET_BIN)/$(LIB_BIN)
	
$(TARGET_BIN)/$(LIB_BIN) : $(TARGET_LIB)/$(LIB_LIB) $(LIB_OBJ) 
	$(CC) $(LOCAL_CFLAGS) $(LIB_OBJ) -o $(TARGET_BIN)/$(LIB_BIN) $(LOCAL_LIB)
	
realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(LIB_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(LIB_H)
	-$(RM) -f $(TARGET_BIN)/$(LIB_BIN)
	

clean : 
	-$(RM) $(LIB_OBJ_LIB) lex.lib.c lib.tab.h lib.tab.c
	-$(RM) $(LIB_OBJ)
	
