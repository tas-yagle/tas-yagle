# makefile for mbkal libraries
# date    : 26/06/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

MAL_OBJ      = alc_driv_l.o alc_pars_l.o alc_pars_l6.o 

LOCAL_MBK_LIB = $(MLU_L) \
                $(MMG_L) \
                $(MCL_L) \
                $(MEL_L) \
                $(MHL_L) \
                $(MVL_L) \
                $(MLO_L) \
                $(MUT_L) \
		$(MAL_L) \
		$(RCN_L) \
		$(MSL_L) \
		$(MGL_L)

LOCAL_CFLAGS = $(CFLAGS)

LOCAL_INC = -I$(TARGET_INCLUDE)

LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -D$(AVERTEC_LICENSE) \
                 -DMLU_H='<$(MLU_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DRCN_H='<$(RCN_H)>'

$(TARGET_LIB)/$(MAL_LIB) : $(MAL_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MAL_LIB) $(MAL_OBJ) 
	$(RANLIB) $(TARGET_LIB)/$(MAL_LIB)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

headers :

realclean : clean
	$(RM) -f $(TARGET_LIB)/$(MAL_LIB)

clean : 
	$(RM) -f $(MAL_OBJ)

test : main.o
	$(CC) $(LOCAL_CFLAGS) -o test main.o $(LOCAL_MBK_LIB) -lm
