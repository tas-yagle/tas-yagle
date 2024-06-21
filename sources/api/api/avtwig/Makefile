include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LIB_LANG = $(TARGET_LIB)/libavtwig.a
LANG_H = Language.h

LOCAL_INC      = -I. -I$(TARGET_INCLUDE)

LOCAL_CFLAGS   =  $(CFLAGS) -c
LOCAL_YACCFLAGS = $(YACCFLAGS) -d 
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DYAG_H='<$(YAG_H)>' \
                 -DGEN_H='<$(GEN_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DFCL_H='<$(FCL_H)>'

OBJ = wrapper_lexer.o wrapper_parser.o
OBJCPP = Language.o

SOURCES_LIB=$(OBJ:.o=.c)
SOURCES_LIB0=$(OBJCPP:.o=.cc)

PRE_OBJ = wrapper_parser.c wrapper_parser.h wrapper_parser.output wrapper_lexer.c

$(LIB_LANG) : $(OBJ) $(OBJCPP)
	$(AR) $(ARFLAGS) $(LIB_LANG) $(OBJ) $(OBJCPP)
	$(RANLIB) $(LIB_LANG)

%.o: %.c 
	$(CC) -c $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

%.o: %.cc
	$(CC) -c $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

wrapper_parser.c wrapper_parser.h: wrapper_parser.y
	$(YACC) $(LOCAL_YACCFLAGS) $< -o wrapper_parser.c 

wrapper_lexer.c: wrapper_lexer.l wrapper_parser.h
	$(LEX) -t $<  >  $@

clean :
	-@$(RM) -f $(OBJ) $(OBJCPP) $(PRE_OBJ) .depend > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(LIB_LANG)

dep: $(SOURCES_LIB) $(SOURCES)
	$(CC) -MM $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES) $(SOURCES_LIB) $(SOURCES_LIB0) > .depend

-include .depend

