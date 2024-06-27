# makefile for spf
# date    : 15/05/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SPF_HEADER   = spf_lib.h

SHELL = /bin/sh

SPF_OBJ_LIB = spf_annot.o spf_actions.o spf_util.o lex.spf.o y.tab.o
SOURCES_LIB=$(SPF_OBJ_LIB:.o=.c)

SPF = dspf

LOCAL_INC = -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS) -c

LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -DSPF_H='"$(SPF_H)"' \
                 -DRCN_H='"$(RCN_H)"' \
                 -DMUT_H='"$(MUT_H)"' \
                 -DMLU_H='"$(MLU_H)"' \
                 -DEQT_H='"$(EQT_H)"' \
                 -DAVT_H='"$(AVT_H)"' \
                 -DLOG_H='"$(LOG_H)"' \
                 -DMSL_H='"$(MSL_H)"' \
                 -DINF_H='"$(INF_H)"' \
                 -DMLO_H='"$(MLO_H)"' \
                 -g

LOCAL_YFLAGS = $(YACCFLAGS) -y -d -v -t

all : $(TARGET_INCLUDE)/$(SPF_H) $(TARGET_LIB)/$(SPF_LIB)

headers : $(TARGET_INCLUDE)/$(SPF_H)

$(TARGET_LIB)/$(SPF_LIB) : $(SPF_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(SPF_LIB) $(SPF_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(SPF_LIB)
	
$(TARGET_INCLUDE)/$(SPF_H) : $(SPF_HEADER)
	$(CP) $(SPF_HEADER) $(TARGET_INCLUDE)/$(SPF_H)

y.tab.h : y.tab.c

y.tab.c : spf.y
	$(YACC) $(LOCAL_YFLAGS) -p spf spf.y

lex.spf.c : spf.l y.tab.h
	$(LEX) $(LEXFLAGS) -Pspf spf.l

.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(SPF_LIB) $(TARGET_INCLUDE)/$(SPF_H)

clean : 
	-$(RM) $(SPF_OBJ_LIB) lex.spf.c y.tab.h y.tab.c y.output

dep: $(SOURCES_LIB) $(SOURCES)
	$(CC) -MM $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES) $(SOURCES_LIB) > .depend

-include .depend
