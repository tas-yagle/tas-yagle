include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I. -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS) -c 
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -D$(AVERTEC_OS)\
                 -DAVT_H='<$(AVT_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DINF_H='<$(INF_H)>'

LOCAL_YFLAGS = $(YACCFLAGS) -d #-v -t 
#LEXFLAGS = -d

OBJ = inf_main.o inf.tab.o lex.inf.o inf_drive.o inf_stb.o inf_grab.o
PRE_OBJ = inf.tab.c inf.tab.h lex.inf.c

all : $(TARGET_INCLUDE)/$(INF_H) $(TARGET_LIB)/$(INF_LIB)

headers : $(TARGET_INCLUDE)/$(INF_H)

$(TARGET_INCLUDE)/$(INF_H) : inf_lib.h inf_stb.h
	$(CAT) inf_lib.h inf_stb.h > $(TARGET_INCLUDE)/$(INF_H)

$(TARGET_LIB)/$(INF_LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(INF_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(INF_LIB)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

inf.tab.h : inf.tab.c

inf.tab.c : inf.y
	$(YACC) $(LOCAL_YFLAGS) -b inf -p inf inf.y

lex.inf.c : inf.l inf.tab.h
	$(LEX) $(LEXFLAGS) -Pinf inf.l

clean :
	-@$(RM) -f $(OBJ) $(PRE_OBJ) distrib .depend > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_LIB)/$(INF_LIB) $(TARGET_INCLUDE)/$(INF_H) > /dev/null 2>&1

SOURCES_LIB=$(OBJ:.o=.c)

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

