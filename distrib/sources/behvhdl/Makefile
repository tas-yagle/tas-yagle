include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_CFLAGS    = $(CFLAGS)
LOCAL_YACCFLAGS = $(YACCFLAGS) -d
LOCAL_LEXFLAGS  = $(LEXFLAGS)
LOCAL_ARFLAGS   = $(ARFLAGS)

LOCAL_INCLUDE   = -I. -I$(TARGET_INCLUDE)

LOCAL_CFLAGS    = $(CFLAGS)
LOCAL_CPPFLAGS =   $(CPPFLAGS) $(LOCAL_INCLUDE) \
                   -D$(AVERTEC_LICENSE) \
                   -DAVT_H='<$(AVT_H)>' \
                   -DMUT_H='<$(MUT_H)>' \
                   -DBEH_H='<$(BEH_H)>' \
                   -DBEG_H='<$(BEG_H)>' \
                   -DBEF_H='<$(BEF_H)>' \
                   -DBHL_H='<$(BHL_H)>' \
                   -DLOG_H='<$(LOG_H)>' \
                   -DBVL_H='<$(BVL_H)>' \
                   -DMLO_H='<$(MLO_H)>'

LOCAL_BVL_OBJ    = bvl_parse.o bvl_drive.o \
                   bvl_bspec.o bvl_util.o

LOCAL_VBE_OBJ    = vbe_parse.o  vbe_drive.o  \
                   vbe_bcomp.tab.o    vbe_bcomp.yy.o

LOCAL_VHD_OBJ    = vhd_parse.o vhd_drive.o \
                   vhd_bcomp.tab.o   vhd_bcomp.yy.o

LOCAL_OBJ = $(LOCAL_BVL_OBJ) $(LOCAL_VBE_OBJ) $(LOCAL_VHD_OBJ)

all : $(TARGET_INCLUDE)/$(BVL_H) $(TARGET_LIB)/$(BVL_LIB)

headers : $(TARGET_INCLUDE)/$(BVL_H)

$(TARGET_INCLUDE)/$(BVL_H) : bvl_lib.h
	$(CP) bvl_lib.h $(TARGET_INCLUDE)/$(BVL_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $<

$(TARGET_LIB)/$(BVL_LIB) : $(LOCAL_OBJ)
	$(AR) $(LOCAL_ARFLAGS) $(TARGET_LIB)/$(BVL_LIB) $(LOCAL_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(BVL_LIB)

vbe_bcomp.yy.c : vbe_bcomp.lex vbe_bcomp.tab.c
	$(LEX) $(LOCAL_LEXFLAGS) -Pvbe_bcomp -t vbe_bcomp.lex > vbe_bcomp.yy.c

vbe_bcomp.tab.c : vbe_bcomp.yac
	$(YACC) $(LOCAL_YACCFLAGS) -p vbe_bcomp -o vbe_bcomp.tab.c vbe_bcomp.yac

vhd_bcomp.yy.c : vhd_bcomp.lex vhd_bcomp.tab.c
	$(LEX) $(LOCAL_LEXFLAGS) -Pvhd_bcomp -t vhd_bcomp.lex > vhd_bcomp.yy.c

vhd_bcomp.tab.c : vhd_bcomp.yac
	$(YACC) $(LOCAL_YACCFLAGS) -p vhd_bcomp -o vhd_bcomp.tab.c vhd_bcomp.yac

#vhd_bcomp.tab.c : vhd_bcomp.y
#$(YACC) $(LOCAL_YACCFLAGS) -t -p vhd_bcomp -o vhd_bcomp.tab.c vhd_bcomp.y
	
clean :
	$(RM) -f vbe_bcomp.tab.h vbe_bcomp.tab.c vbe_bcomp.yy.c
	$(RM) -f vhd_bcomp.tab.h vhd_bcomp.tab.c vhd_bcomp.yy.c
	$(RM) -f $(LOCAL_OBJ)

realclean : clean
	$(RM) -f $(TARGET_LIB)/$(BVL_LIB) $(TARGET_INCLUDE)/$(BVL_H)
	$(RM) -f bgl_bcomp.output
