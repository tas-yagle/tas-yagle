# makefile for stb
# date    : 15/05/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

STB_OBJ_LIB = stb_init.o stb_analysis.o stb_transfer.o stb_relaxation.o \
              stb_parse.o stb_drive.o stb_util.o stb_error.o \
              stb_overlap.o stb_debug.o stb_ctk.o stb_ctk_noise.o \
              stb_ctk_debug.o stb_ctki.o stb_ctk_score.o stb_fromx.o \
			  stb_ctk_mutex.o stb_delaymargin.o stb_ctk_agr.o \
			  stb_clock_tree.o stb_clocks.o stb_directives.o stb_falseslack.o\
			  stb_relaxation_correction.o stb_ctk_report.o

STB_OBJ      = stb_main.o

STB = stb

STB_HEADER   = stb.h

LOCAL_INC = -I$(TARGET_INCLUDE)

REGULAR_LIB = $(STB_L) \
              $(TTV_L) \
              $(BCK_L) \
			  $(STM_L) \
			  $(GEN_L) \
			  $(TRC_L) \
			  $(TLC_L) \
              $(SIM_L) \
              $(MLU_L) \
              $(MLO_L) \
              $(RCN_L) \
              $(MCL_L) \
              $(MAL_L) \
              $(MSL_L) \
              $(MGL_L) \
              $(MVL_L) \
              $(MEL_L) \
              $(MHL_L) \
              $(EQT_L) \
              $(MCC_L) \
              $(ELP_L) \
              $(EFG_L) \
              $(GSP_L) \
              $(CBH_L) \
              $(LIB_L) \
              $(TLF_L) \
              $(TUT_L) \
              $(BHL_L) \
              $(BEH_L) \
              $(CNS_L) \
              $(LOG_L) \
              $(SPF_L) \
              $(SPE_L) \
              $(API_L) \
			  $(INF_L) \
              $(MUT_L) \
              $(AVT_L) \
              $(RPC_L) 
			  
LOCAL_LIB  = $(FLEXOBJS) -L$(TARGET_LIB) $(REGULAR_LIB) -lm $(FLEXLIBS) -ldl

API_STATICLIB = $(FLEXOBJS) -L$(TARGET_LIB) $(STATIC_LIB) $(WHOLE) \
				$(APIALL_L) \
				$(BEG_L) \
				$(YAG_L) \
				$(TAS_L) \
				$(BVL_L) \
				$(FCL_L) \
				$(BEF_L) \
				$(BGL_L) \
				$(REGULAR_LIB) $(NOWHOLE) -lm -ldl $(FLEXLIBS)

LOCAL_CFLAGS      = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DSTB_H='"$(STB_HEADER)"' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DSTM_H='<$(STM_H)>' \
                 -DTTV_H='<$(TTV_H)>' \
                 -DBCK_H='<$(BCK_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DTRC_H='<$(TRC_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DMCC_H='<$(MCC_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DSIM_H='<$(SIM_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DEFG_H='<$(EFG_H)>' \
                 -DTRC_H='<$(TRC_H)>' \
                 -DINF_H='<$(INF_H)>'
		 

LOCAL_YFLAGS = $(YACCFLAGS) -y -d #-t
LOCAL_LFLAGS = $(LEXFLAGS) #-t

all : lib

lib : $(TARGET_LIB)/$(STB_LIB) $(TARGET_INCLUDE)/$(STB_H)

bin : $(TARGET_BIN)/$(STB)

link : 
	-$(RM) -f $(TARGET_BIN)/$(STB)
	$(CC) $(LOCAL_CFLAGS) $(STB_OBJ) -o $(TARGET_BIN)/$(STB) $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/stb

headers : $(TARGET_INCLUDE)/$(STB_H)

$(TARGET_BIN)/$(STB) : $(TARGET_LIB)/$(STB_LIB) $(STB_OBJ)
	$(CC) $(LOCAL_CFLAGS) $(STB_OBJ) -o $(TARGET_BIN)/$(STB) $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/stb

api_link :
	$(CC) $(LOCAL_CFLAGS) $(STB_OBJ) -o $(TARGET_BIN)/$(STB) $(API_STATICLIB) $(TCL_L)
	$(STRIP) $(TARGET_BIN)/stb

$(TARGET_LIB)/$(STB_LIB) : $(STB_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(STB_LIB) $(STB_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(STB_LIB)

$(TARGET_INCLUDE)/$(STB_H) : $(STB_HEADER)
	$(CP) $(STB_HEADER) $(TARGET_INCLUDE)/$(STB_H)

stb.tab.h : stb.tab.c

stb.tab.c : stb.yac
	$(YACC) $(LOCAL_YFLAGS) -p stb -o stb.tab.c stb.yac

lex.stb.c : stb.lex stb.tab.h
	$(LEX) $(LOCAL_LFLAGS) -Pstb stb.lex

.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_BIN)/$(STB)
	-$(RM) -f $(TARGET_INCLUDE)/$(STB_H)
	-$(RM) -f $(TARGET_LIB)/$(STB_LIB)

clean : 
	-$(RM) -f $(STB_OBJ) $(STB_OBJ_LIB) lex.stb.c stb.tab.h stb.tab.c

SOURCES_LIB=$(STB_OBJ_LIB:.o=.c)

dep: $(SOURCES_LIB)
	$(SCC) -MM  $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend
