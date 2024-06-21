# makefile for equation
# date    : 17/07/01

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

EQT_HEADER     = eqt_lib.h

SHELL 		   = /bin/sh

EQT_OBJ_LIB    = lex.eqt.o eqt.tab.o eqt_util.o 
EQT_OBJ_MAIN   = eqt_main.o 

EQT_BIN		   = eqt_exec

LOCAL_INC	   = -I$(TARGET_INCLUDE) \
                 -DEQT_H='"$(EQT_H)"' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMUT_H='<$(MUT_H)>' 
            
LOCAL_LIB  	   = -L$(TARGET_LIB) \
                 $(EQT_L) \
                 $(LOG_L) \
                 $(MUT_L) \
                 -lm 

LOCAL_CFLAGS   = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) -D$(AVERTEC_OS)
                                
LOCAL_YFLAGS   = $(YACCFLAGS) -d

all : $(TARGET_INCLUDE)/$(EQT_H) $(TARGET_LIB)/$(EQT_LIB)

headers : $(TARGET_INCLUDE)/$(EQT_H)

$(TARGET_LIB)/$(EQT_LIB) : $(EQT_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(EQT_LIB) $(EQT_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(EQT_LIB)
	
$(TARGET_INCLUDE)/$(EQT_H) : $(EQT_HEADER)
	$(CP) $(EQT_HEADER) $(TARGET_INCLUDE)/$(EQT_H)
	
eqt.tab.h : eqt.tab.c

eqt.tab.c : eqt.y 
	$(YACC) $(LOCAL_YFLAGS) -b eqt -p eqt eqt.y 

lex.eqt.c : eqt.l eqt.tab.h
	$(LEX) $(LEXFLAGS) -Peqt eqt.l 

.c.o : 
	$(CC) $(LOCAL_CFLAGS)  $(LOCAL_CPPFLAGS)  $<

test : all $(TARGET_BIN)/$(EQT_BIN)

$(TARGET_BIN)/$(EQT_BIN) : $(EQT_OBJ_MAIN) $(TARGET_LIB)/$(EQT_LIB)
	$(CC) $(EQT_OBJ_MAIN) $(LOCAL_CFLAGS) -o $(TARGET_BIN)/$(EQT_BIN) $(EQT_OBJ) $(LOCAL_LIB)
	
realclean : clean
	-$(RM) -f $(TARGET_BIN)/$(EQT_BIN)
	-$(RM) -f $(TARGET_LIB)/$(EQT_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(EQT_H)

clean : 
	-$(RM) -f $(EQT_OBJ) $(EQT_OBJ_MAIN) $(EQT_OBJ_LIB) lex.eqt.c eqt.tab.c eqt.tab.h 

	
