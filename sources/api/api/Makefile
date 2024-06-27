# makefile for api
# date    : 16/07/2002

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

API_BIN = genapi

WIG_LIB = $(TARGET_LIB)/libavtwig.a

WRP_BIN = wrap
WRP_BIN_NOLIC = wrap_nolicense

I_DIR = ./avtwig

LOCAL_YACCFLAGS = $(YACCFLAGS) -d -t 

LOCAL_CFLAGS = $(CFLAGS) $(TCL_H)

SHELL = /bin/sh
WRP_INC = -I. -I$(I_DIR) -I$(TARGET_INCLUDE)
WRP_LIB = $(FLEXOBJS)  -L$(TARGET_LIB) -lavtwig $(AVT_L) $(MUT_L) $(RPC_L) $(FLEXLIBS)
WRP_OBJ = api_wrp_gen.o api_wrp_gns.o
WRP_CPPFLAGS = $(CPPFLAGS) -c $(WRP_INC) -DUSE_AVTWIG -DMUT_H='<$(MUT_H)>' -DAVT_H='<$(AVT_H)>' -DAPI_H='<$(API_H)>'
API_CPPFLAGS = $(CPPFLAGS) -c

API_OBJ = api_util.o gen_library_parser.o gen_library_lexer.o gen_tree_parser.o gen_tree_lexer.o gen_tree_utils.o gen_tree_errors.o \
		  gen_execute_C.o gen_verif_C.o api_communication.o gen_templates.o

PRE_OBJ = gen_tree_parser.c gen_tree_parser.h gen_tree_lexer.c gen_library_parser.c gen_library_lexer.c gen_library_parser.h

SOURCES_LIB=$(API_OBJ:.o=.c)

LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -I$(TARGET_INCLUDE) -D$(AVERTEC_OS)\
                 -DAVT_H='<$(AVT_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLO_H='<$(MLO_H)>' 

LIB = -L$(TARGET_LIB) \
      $(API_L) \
      $(AVT_L) \
      $(MUT_L) \
      $(RPC_L) \
      -lm  -ldl

all: $(TARGET_INCLUDE)/$(API_H) $(TARGET_INCLUDE)/api_communication.h headers.inc $(TARGET_BIN)/$(WRP_BIN_NOLIC)

$(TARGET_INCLUDE)/$(API_H): api_lib.h gen_tree_parser.h $(TARGET_BIN)/license_api
	sed -e "s/#endif \/\/TOBECUT/#ifndef API_HIDE_TOKENS/g" api_lib.h > tmp.h
	grep "[0-9]" gen_tree_parser.h | grep define >> tmp.h
	echo "#endif" >> tmp.h
	echo "#endif" >> tmp.h
	mv tmp.h $(TARGET_INCLUDE)/$(API_H)

headers: $(TARGET_INCLUDE)/$(API_H)

$(TARGET_BIN)/license_api: license_patch.c
	$(CC) license_patch.c -o $(TARGET_BIN)/license_api

$(TARGET_INCLUDE)/api_communication.h: api_communication.h
	cp api_communication.h $(TARGET_INCLUDE)

$(TARGET_BIN)/$(WRP_BIN): $(WIG_LIB) $(WRP_OBJ) $(TARGET_LIB)/$(API_LIB) $(WIG_LIB)
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(WRP_OBJ) $(WRP_INC) $(WRP_LIB) -lm -o $(TARGET_BIN)/$(WRP_BIN)
	$(STRIP)  $(TARGET_BIN)/$(WRP_BIN)
	$(CP) $(API_BIN) $(TARGET_BIN)

$(TARGET_BIN)/$(WRP_BIN_NOLIC): $(WIG_LIB) $(WRP_OBJ) $(TARGET_LIB)/$(API_LIB) $(WIG_LIB)
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(WRP_OBJ) $(WRP_INC) $(WRP_LIB) -lm -o $(TARGET_BIN)/$(WRP_BIN_NOLIC)
	cp $(TARGET_BIN)/$(WRP_BIN_NOLIC) $(TARGET_BIN)/$(WRP_BIN)
	#-rm subdir_license
	$(STRIP)  $(TARGET_BIN)/$(WRP_BIN)
	$(CP) $(API_BIN) $(TARGET_BIN)

$(WIG_LIB): 
	$(MAKE) -C $(I_DIR) AVERTEC_LICENSE=NO_LICENSE

$(TARGET_LIB)/$(API_LIB): $(PRE_OBJ) $(API_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(API_LIB) $(API_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(API_LIB)
	$(CP) $(API_BIN) $(TARGET_BIN)
	

tt: $(TARGET_LIB)/$(API_LIB) api_lib.h test.o
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) test.o -o tt $(LIB)

gen_tree_parser.c gen_tree_parser.h: gen_tree_parser.y
	$(YACC) $(LOCAL_YACCFLAGS) $< -p gen_tree_parser -o gen_tree_parser.c 

gen_tree_lexer.c: gen_tree_lexer.l gen_tree_parser.h
	$(LEX) -Pgen_tree_parser -t $<  >  $@

gen_library_parser.c gen_library_parser.h: gen_library_parser.y
	$(YACC) $(LOCAL_YACCFLAGS) $< -p gen_library_parser -o gen_library_parser.c

gen_library_lexer.c: gen_library_lexer.l gen_library_parser.h
	        $(LEX) -Pgen_library_parser -t $<  >  $@

%.o: %.cc $(TARGET_INCLUDE)/$(API_H) $(PRE_OBJ) headers.inc
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(WRP_CPPFLAGS) $<

%.o: %.c $(TARGET_INCLUDE)/$(API_H) $(PRE_OBJ)
	$(CC) $(LOCAL_CFLAGS) $(WRP_CPPFLAGS) $<

headers.inc: $(TARGET_INCLUDE)/$(AVT_H) api_lib.h
	\grep "^GRAB_IT" $(TARGET_INCLUDE)/$(AVT_H) | sed -e "s/^GRAB_IT /Printf(f_header, \"/g" -e "s/$$/\\\\n\");/g" > tmp
	\grep "\/\/GRAB_IT" api_lib.h | sed -e "s/^\/\/GRAB_IT /Printf(f_header, \"/g" -e "s/$$/\\\\n\");/g" >> tmp
	\grep "^GRAB_IT" api_lib.h | sed -e "s/^GRAB_IT /Printf(f_header, \"/g" -e "s/$$/\\\\n\");/g" >> tmp
	mv tmp headers.inc

subdir_license:
	$(MAKE) -C $(I_DIR) realclean
	$(MAKE) -C $(I_DIR)
	$(TOUCH) subdir_license
	
final_wrap: subdir_license $(TARGET_BIN)/$(WRP_BIN)
	
	
clean:
	-$(RM) -f $(WRP_OBJ) $(API_OBJ) headers.inc $(PRE_OBJ)
	$(MAKE) -C $(I_DIR) clean
	-$(RM) -f subdir_license

realclean: clean
	-$(RM) -f $(TARGET_BIN)/license_api $(TARGET_BIN)/$(WRP_BIN) $(TARGET_BIN)/$(WRP_BIN_NOLIC) $(TARGET_BIN)/$(API_BIN) $(TARGET_LIB)/$(API_LIB) $(TARGET_INCLUDE)/$(API_H) $(TARGET_INCLUDE)/api_communication.h
	-$(RM) -f .depend
	-$(RM) -f gen_tree_parser.output
	$(MAKE) -C $(I_DIR) realclean

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM $(LOCAL_CFLAGS) $(WRP_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

