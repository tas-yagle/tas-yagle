include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INCLUDE = -I. -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INCLUDE) \
                 -DBGL_H='<$(BGL_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEF_H='<$(BEF_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMLU_H='<$(MLU_H)>'

LOCAL_YFLAGS = $(YACCFLAGS) -d

LOCAL_LEXFLAGS = $(LEXFLAGS)

OBJ = bgl_drive.o bgl_parse.o lex.bgl_bcomp.o bgl_bcomp.tab.o bgl_util.o bgl_error.o bgl_action.o

PRE_OBJ = lex.bgl_bcomp.cc bgl_bcomp.tab.c bgl_bcomp.tab.h

all: $(TARGET_INCLUDE)/$(BGL_H) $(TARGET_LIB)/$(BGL_LIB)

headers : $(TARGET_INCLUDE)/$(BGL_H)
	
$(TARGET_INCLUDE)/$(BGL_H): bgl_lib.h
	cp bgl_lib.h $(TARGET_INCLUDE)/$(BGL_H)

$(TARGET_LIB)/$(BGL_LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(BGL_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(BGL_LIB)

bgl_bcomp.tab.h : bgl_bcomp.tab.c

bgl_bcomp.tab.c : bgl_bcomp.yac
	$(YACC) $(LOCAL_YFLAGS) -o bgl_bcomp.tab.c bgl_bcomp.yac

bgl_bcomp.tab.o : bgl_bcomp.tab.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

lex.bgl_bcomp.cc : bgl_bcomp.lex bgl_bcomp.tab.h
	$(LEX) $(LOCAL_LEXFLAGS) -Pbgl_bcomp bgl_bcomp.lex

lex.bgl_bcomp.o : lex.bgl_bcomp.cc
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

bgl_drive.o : bgl_drive.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

v_drive.o : v_drive.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<
	
bgl_parse.o : bgl_parse.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

bgl_util.o : bgl_util.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

bgl_action.o : bgl_action.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

bgl_error.o : bgl_error.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

clean :
	-@$(RM) -f $(OBJ) $(PRE_OBJ) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_LIB)/$(BGL_LIB) > /dev/null 2>&1
	-@$(RM) -f $(TARGET_INCLUDE)/$(BGL_H) > /dev/null 2>&1
