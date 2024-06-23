# Makefile Avertec source
#
# Works well with gmake...
#
# Makefile v3.0 : added flexibility rules like build
#

XXX_H = header
XXX_C = compilation
XXX_Cl = clean
XXX_R = realclean
XXX_AB = apibuild

ALL_DIRS_NO_API = mbk vcd avt api.wrapper bdd eqt mbkal mbkedif mbkhilo mbkspice mbkvhdl mbkvti mbkvrlog mbkspf mbkspef beh behvhdl behvrlog cbh elp cns inf efg gsp scheduler pavo yagle.lib tas yagle
ALL_DIRS = $(ALL_DIRS_NO_API) api
ALL_DIRS_API = $(ALL_DIRS_NO_API)

ALL_DIRS_COMP = $(ALL_DIRS:%=%.$(XXX_C))

ALL_DIRS_LIST = $(shell echo $(ALL_DIRS)|sed "s/[a-z]*\.[a-z_]*//g")

ALL_DIRS_HEADERS   = $(ALL_DIRS_LIST:%=%.$(XXX_H))
ALL_DIRS_CLEAN     = $(ALL_DIRS_LIST:%=%.$(XXX_Cl))
ALL_DIRS_REALCLEAN = $(ALL_DIRS_LIST:%=%.$(XXX_R))

ALL_DIRS_API_BUILD = $(ALL_DIRS_API:%=%.$(XXX_AB))

WORK_DIRS = bin include lib api_bin api_include api_lib man man/man1 man/man3
FULL_NAME_WORK_DIRS = $(WORK_DIRS:%=$(AVT_DISTRIB_DIR)/%)

BIN_LIST = avt_shell avtreserve gnshr x2v avtdeltoken avttool gnstest xtas avtgenkey cnsdiff license_api ttvdiff xyagle avtinfo netutil ttvren avtlicense genapi router ttvtest avtlog genelp  val avtman gentest wrap yargle hitas
BIN_LIST_DEL = $(BIN_LIST:%=%.del)

all : working_dirs $(ALL_DIRS_HEADERS) build
	

working_dirs : $(AVT_DISTRIB_DIR) $(FULL_NAME_WORK_DIRS)
	

build : $(ALL_DIRS_COMP)
	@echo ""
	@echo "           ##########################################"
	@echo "           ###########                  #############"
	@echo "           ###########       O K        #############"
	@echo "           ###########                  #############"
	@echo "           ##########################################"
	@echo ""

$(ALL_DIRS_HEADERS) : $(AVT_DISTRIB_DIR) $(FULL_NAME_WORK_DIRS)
	@echo "           %%%%%%%%% creating HEADERS for" `echo $(@:%.$(XXX_H)=%)|sed 's/\..*//'` "%%%%%%%%%"
	@$(MAKE) -C `echo $(@:%.$(XXX_H)=%)|sed 's/\..*//'` headers
	@echo "           %%%%%%%%% HEADERS for" `echo $(@:%.$(XXX_H)=%)|sed 's/\..*//'` "done %%%%%%%%%"
	@echo ""

$(ALL_DIRS_COMP) :
	@echo "           %%%%%%%%% compiling $(@:%.$(XXX_C)=%) %%%%%%%%%"
	@echo "           %%%%%%%%% compiling $(@:%.$(XXX_C)=%) %%%%%%%%%"
	@echo "           m%%%%%%%%% compiling $(@:%.$(XXX_C)=%) %%%%%%%%%"
	@$(MAKE) -j2 -C `echo $(@:%.$(XXX_C)=%)|sed 's/\..*//'` `echo $(@:%.$(XXX_C)=%)|sed 's/[^\.]*\.*//'`
	@echo ""

$(ALL_DIRS_CLEAN) :
	@$(MAKE) -j2 -C `echo $(@:%.$(XXX_Cl)=%)|sed 's/\..*//'` clean

$(ALL_DIRS_REALCLEAN) :
	@$(MAKE) -j2 -C `echo $(@:%.$(XXX_R)=%)|sed 's/\..*//'` realclean

$(AVT_DISTRIB_DIR) $(FULL_NAME_WORK_DIRS): 
	@echo "           %%%%%%%%% creating directory $@"
	@mkdir  $@

# only for debug (create static libraries for apis)
$(ALL_DIRS_API_BUILD) :
	@echo "           %%%%%%%%% building $(@:%.$(XXX_AB)=%)"
	@$(MAKE) -j2 -C `echo $(@:%.$(XXX_AB)=%)|sed 's/\..*//'` `echo $(@:%.$(XXX_AB)=%)|sed 's/[^\.]*\.*//'`

dynamic_link : working_dirs $(ALL_DIRS_HEADERS) $(ALL_DIRS_API_BUILD)
	$(MAKE) -j2 -C api dynamic_link
	$(MAKE) -j2 -C yagle/yagle
	$(MAKE) -j2 -C yagle/xyagle
	$(MAKE) -j2 -C tas/tas 
	$(MAKE) -j2 -C tas/xtas 
	$(MAKE) -j2 -C tas/tma
	$(MAKE) -j2 -C tas/stb
	$(MAKE) -j2 -C api/tcl dynamic_link

$(BIN_LIST_DEL) :
	$(RM) $(AVT_DISTRIB_DIR)/bin/$(@:%.del=%)

link : $(BIN_LIST_DEL) all
	

clean : $(ALL_DIRS_CLEAN)
	

realclean : $(ALL_DIRS_REALCLEAN) $(BIN_LIST_DEL)
	

headers : $(ALL_DIRS_HEADERS)
	

help : 
	@echo "Makefile usage:"
	@echo "make [all|clean|realclean|headers|build|link|api_link]:"
	@echo "  all       : build the full binarie distribution"
	@echo "  clean     : clean recursivelly all directories"
	@echo "  realclean : realclean recursivelly all directories"
	@echo "              and remove all listed binaries"
	@echo "  headers   : only make headers"
	@echo "  build     : only compile and build the distribution"
	@echo "              WARNING it need that headers have been done once"
	@echo "  link      : compile and relink all binaries"
	@echo "  api_link  : compile and relink all binaries with apis in static"

color :
	$(MAKE) -e XXC_W="" XXB_W="" XXC_G="" XXR_0=""

% :
	echo "[1;30;41m$@"
	@for FILE in $(ALL_DIRS)        ;\
	do rules=`echo $$FILE|sed 's/\..*//'` ;\
	echo "           $$rules $$FILE" ;\
    if [ "$$rules" = "$$FILE" ] ;\
     then $(MAKE) -C `echo $$FILE|sed 's/\..*//'` $@       ;\
	fi                  ;\
	if [ $$? -ne 0 ]            ;\
	  then  exit 1              ;\
	fi                  ;\
	done

