# makefile for spef
# date    : 15/05/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SPE_HEADER     = spe_lib.h

SHELL          = /bin/sh
SPE_OBJ        = spef_main.o
SPE_OBJ_LIB    = spef_drive.o spef_annot.o spef_actions.o spef_util.o lex.spef_dnet.o spef_dnet.tab.o lex.spef.o spef.tab.o

SPE_BIN 	   = spefbin

LOCAL_INC 	   = -I$(TARGET_INCLUDE)

LOCAL_LIB      = -L$(TARGET_LIB) \
                   $(SPE_L) \
                   $(RCN_L) \
                   $(MUT_L) \
                   $(MLU_L) \
                   $(MLO_L) \
			     -lm

LOCAL_CFLAGS   = $(CFLAGS) 
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -DSPE_H='<$(SPE_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -g

LOCAL_YFLAGS   = $(YACCFLAGS) -d #-v -t 

all : $(TARGET_INCLUDE)/$(SPE_H) $(TARGET_LIB)/$(SPE_LIB)

headers : $(TARGET_INCLUDE)/$(SPE_H)

$(TARGET_LIB)/$(SPE_LIB) : $(SPE_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(SPE_LIB) $(SPE_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(SPE_LIB)
	
$(TARGET_INCLUDE)/$(SPE_H) : $(SPE_HEADER)
	$(CP) $(SPE_HEADER) $(TARGET_INCLUDE)/$(SPE_H)

spef.tab.h : spef.tab.c

spef.tab.c : spef.y
	$(YACC) $(LOCAL_YFLAGS) -b spef -p spef spef.y

lex.spef.c : spef.l spef.tab.h
	$(LEX) $(LEXFLAGS) -Pspef spef.l

spef_dnet.tab.h : spef_dnet.tab.c

spef_dnet.tab.c : spef_dnet.y
	$(YACC) $(LOCAL_YFLAGS) -b spef_dnet -p spef_dnet spef_dnet.y

lex.spef_dnet.c : spef_dnet.l spef_dnet.tab.h
	$(LEX) $(LEXFLAGS) -Pspef_dnet spef_dnet.l

.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<


test : $(TARGET_BIN)/$(SPE_BIN)
	
$(TARGET_BIN)/$(SPE_BIN) : $(TARGET_LIB)/$(SPE_LIB) $(SPE_OBJ) 
	$(CC) $(LOCAL_CFLAGS) $(SPE_OBJ) -o $(TARGET_BIN)/$(SPE_BIN) $(LOCAL_LIB)

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(SPE_LIB) $(TARGET_INCLUDE)/$(SPE_H)

clean : 
	-$(RM) $(SPE_OBJ_LIB) lex.spef_dnet.c spef_dnet.tab.h spef_dnet.tab.c spef_dnet.output lex.spef.c spef.tab.h spef.tab.c spef.output
	-$(RM) $(SPE_OBJ)
	

