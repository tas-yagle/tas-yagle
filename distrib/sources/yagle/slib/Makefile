# makefile for tlf
# date    : 21/03/02

# COMPILATION du binaire   : make bin
# COMPILATION de slib_view : make view
# CLEAN   : make clean

###############################################################################
#
# include pour la compilation du binaire
#
###############################################################################

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

#SLB_INSTALL_VERSION      = 100
#SLB_H        = slb$(SLB_INSTALL_VERSION).h
#SLB_L        = -lSlb$(SLB_INSTALL_VERSION)
#SLB_LIB      = libSlb$(SLB_INSTALL_VERSION).a
SLB_HEADER = slb_lib.h

LOCAL_CFLAGS = $(CFLAGS) -c
LOCAL_ARFLAGS = $(ARFLAGS)

LOCAL_YACCFLAGS = $(YACCFLAGS) -d -t  
LOCAL_LEXFLAGS = $(LEXFLAGS)

LOCAL_INC = -I. -I$(TARGET_INCLUDE)

LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVT_H='"$(AVT_H)"' \
                 -DSLB_H='"$(SLB_H)"' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DEQT_H='<$(EQT_H)>'
                 
LOCAL_OBJ = lex.slib.o slib.tab.o slib_util.o slib_error.o
LOCAL_PRE_OBJ = slib.tab.h slib.tab.c lex.slib.c

LOCAL_LIB  = -L$(TARGET_LIB) \
               $(AVT_L) \
               $(SLB_L) \
               $(MUT_L) \
               $(EQT_L) \
               $(LOG_L) \
               -lm 

all:  $(TARGET_INCLUDE)/$(SLB_H) $(TARGET_LIB)/$(SLB_LIB)

headers : $(TARGET_INCLUDE)/$(SLB_H)

$(TARGET_INCLUDE)/$(SLB_H): $(SLB_HEADER) 
	$(CP) $(SLB_HEADER) $(TARGET_INCLUDE)/$(SLB_H)

$(TARGET_LIB)/$(SLB_LIB) : $(LOCAL_OBJ) $(SLB_HEADER)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(SLB_LIB) $(LOCAL_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(SLB_LIB)
	
slib.tab.h : slib.tab.c

slib.tab.c : slib.y 
	$(YACC) $(LOCAL_YACCFLAGS) -b slib -p slib slib.y 

lex.slib.c : slib.l slib.tab.h
	$(LEX) $(LOCAL_LEXFLAGS) -Pslib slib.l  
	
.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

realclean : clean
	-@$(RM) -f $(TARGET_LIB)/$(SLB_LIB)
	-@$(RM) -f $(TARGET_INCLUDE)/$(SLB_H)
	-@$(RM) -f .depend

clean : 
	-@$(RM) -f $(LOCAL_OBJ) $(LOCAL_PRE_OBJ)

SOURCES_LIB=$(LOCAL_OBJ:.o=.c)

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

