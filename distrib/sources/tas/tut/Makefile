# makefile for tlf
# date    : 15/05/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

TUT_HEADER   = tut_lib.h

SHELL = /bin/sh

#TUT_OBJ = tut_main.o
TUT_OBJ_LIB = tut_parse.o lex.tlfarea.o tlfarea.tab.o lex.libarea.o libarea.tab.o lex.txtarea.o txtarea.tab.o tut_derate.o

TUT_BIN = tut

LOCAL_INC = -I$(TARGET_INCLUDE)
			
LOCAL_LIB  = -L$(TARGET_LIB) \
               $(AVT_L) \
               $(TUT_L) \
               $(MUT_L) \
               -lm 

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DTUT_H='"$(TUT_H)"' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMUT_H='<$(MUT_H)>' 

LOCAL_YFLAGS = $(YACCFLAGS) -d

all : $(TARGET_INCLUDE)/$(TUT_H) $(TARGET_LIB)/$(TUT_LIB)

headers : $(TARGET_INCLUDE)/$(TUT_H)

$(TARGET_LIB)/$(TUT_LIB) : $(TUT_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(TUT_LIB) $(TUT_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(TUT_LIB)
	
$(TARGET_INCLUDE)/$(TUT_H) :
	$(CP) $(TUT_HEADER) $(TARGET_INCLUDE)/$(TUT_H)
	
# LIBAREA

libarea.tab.h : libarea.tab.c

libarea.tab.c : libarea.y 
	$(YACC) $(LOCAL_YFLAGS) -b libarea -p libarea libarea.y 

lex.libarea.c : libarea.l libarea.tab.h
	$(LEX) $(LEXFLAGS) -Plibarea libarea.l 

# TLFAREA

tlfarea.tab.h : tlfarea.tab.c

tlfarea.tab.c : tlfarea.y 
	$(YACC) $(LOCAL_YFLAGS) -b tlfarea -p tlfarea tlfarea.y 

lex.tlfarea.c : tlfarea.l tlfarea.tab.h
	$(LEX) $(LEXFLAGS) -Ptlfarea tlfarea.l 

# TXTAREA

txtarea.tab.h : txtarea.tab.c

txtarea.tab.c : txtarea.y 
	$(YACC) $(LOCAL_YFLAGS) -b txtarea -p txtarea txtarea.y 

lex.txtarea.c : txtarea.l txtarea.tab.h
	$(LEX) $(LEXFLAGS) -Ptxtarea txtarea.l 

	
.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(TUT_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(TUT_H)

clean : 
	-$(RM) $(TUT_OBJ_LIB) lex.libarea.c libarea.tab.c libarea.tab.h lex.tlfarea.c tlfarea.tab.c tlfarea.tab.h lex.txtarea.c txtarea.tab.c txtarea.tab.h
#	-$(RM) $(TUT_OBJ)
	
