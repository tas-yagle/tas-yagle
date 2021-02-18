# makefile for tlf
# date    : 15/05/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

TLF_HEADER   = tlf_lib.h

SHELL = /bin/sh

TLF_OBJ = tlf_main.o 
TLF_OBJ_LIB = tlf_env.o tlf_parse.o lex.tlf3.o tlf3.tab.o lex.tlf4.o tlf4.tab.o lex.gcf.o gcf.tab.o \
              tlf3_drive.o  tlf4_drive.o tlf_derate.o tlf_util.o

TLF_BIN = tlftr

LOCAL_INC = -I$(TARGET_INCLUDE)
			
LOCAL_LIB  = -L$(TARGET_LIB) \
               $(AVT_L) \
               $(TLF_L) \
               $(TUT_L) \
               $(TTV_L) \
               $(STM_L) \
               $(TRC_L) \
               $(TLC_L) \
               $(ELP_L) \
               $(MCC_L) \
               $(EQT_L) \
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
               $(SPEF_L) \
               $(RCN_L) \
               $(MUT_L) \
               -lm 

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DTLF_H='"$(TLF_H)"' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DTTV_H='<$(TTV_H)>' \
                 -DSTM_H='<$(STM_H)>' \
                 -DMCC_H='<$(MCC_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DSIM_H='<$(SIM_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DEFG_H='<$(EFG_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DBEF_H='<$(BEF_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DTUT_H='<$(TUT_H)>'

LOCAL_YFLAGS = $(YACCFLAGS) -d #-v

all : $(TARGET_INCLUDE)/$(TLF_H) $(TARGET_LIB)/$(TLF_LIB)

headers : $(TARGET_INCLUDE)/$(TLF_H)

$(TARGET_LIB)/$(TLF_LIB) : $(TLF_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(TLF_LIB) $(TLF_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(TLF_LIB)
	
$(TARGET_INCLUDE)/$(TLF_H) :
	$(CP) $(TLF_HEADER) $(TARGET_INCLUDE)/$(TLF_H)
	

# TLF 4

tlf4.tab.h : tlf4.tab.c

tlf4.tab.c : tlf4.y 
	$(YACC) $(LOCAL_YFLAGS) -b tlf4 -p tlf4 tlf4.y 

lex.tlf4.c : tlf4.l tlf4.tab.h
	$(LEX) $(LEXFLAGS) -Ptlf4 tlf4.l 

# TLF 3

tlf3.tab.h : tlf3.tab.c

tlf3.tab.c : tlf3.y 
	$(YACC) $(LOCAL_YFLAGS) -b tlf3 -p tlf3 tlf3.y 

lex.tlf3.c : tlf3.l tlf3.tab.h
	$(LEX) $(LEXFLAGS) -Ptlf3 tlf3.l 

test : $(TARGET_BIN)/$(TLF_BIN)
	
$(TARGET_BIN)/$(TLF_BIN) : $(TARGET_LIB)/$(TLF_LIB) $(TLF_OBJ) 
	$(CC) $(LOCAL_CFLAGS) $(TLF_OBJ) -o $(TARGET_BIN)/$(TLF_BIN) $(LOCAL_LIB)


# GCF

gcf.tab.h : gcf.tab.c

gcf.tab.c : gcf.y
	$(YACC) $(LOCAL_YFLAGS) -b gcf -p gcf gcf.y 

lex.gcf.c : gcf.l gcf.tab.h
	$(LEX) $(LEXFLAGS) -Pgcf gcf.l 
	
.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(TLF_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(TLF_H)

clean : 
	-$(RM) -f $(TLF_OBJ_LIB) lex.gcf.c gcf.tab.c gcf.tab.h lex.tlf4.c tlf4.tab.c tlf4.tab.h lex.tlf3.c tlf3.tab.c tlf3.tab.h
	-$(RM) -f $(TLF_OBJ)
	
