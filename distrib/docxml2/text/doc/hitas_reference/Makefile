include $(AVERTEC_TOP)/etc/Avertec.mk
include $(AVERTEC_TOP)/etc/AVT_$(AVERTEC_OS).mk

WRAP = wrap_nolicense

DOCXML     = $(AVERTEC_TOP)/../docxml2
COMPILED   = $(DOCXML)/compiled
TEXT       = $(DOCXML)/text
PICT_HTML  = $(DOCXML)/imgs_html
PICT_PDF   = $(DOCXML)/imgs_pdf
HTML       = $(DOCXML)/html
PDF        = $(DOCXML)/pdf
API_SRCDIR = $(DOCXML)/../sources/api
DATE       = $(DOCXML)/date

INF        = $(TEXT)/common/inf/*.xml
FORMATS    = $(TEXT)/common/formats/*.xml
CONFIG     = $(TEXT)/common/configuration/*.xml

DOC        = hitas_reference

all : pdf html
html : $(COMPILED)/dochtml/$(DOC).html
pdf : $(COMPILED)/docpdf/$(DOC).pdf

################################################################################################

TTV_API = ttv_API_sdf.xml ttv_API_dtb.xml ttv_API_path.xml ttv_API_line.xml ttv_API_sig.xml \
          ttv_API_event.xml ttv_API_detail.xml ttv_API_sim.xml ttv_API_report.xml \
		  ttv_API_generation.xml ttv_API_greybox.xml

AVT_API = avt_API_files.xml avt_API_system.xml avt_API_lofig.xml avt_API_config.xml avt_API_montecarlo.xml

MBK_API = mbk_API_files.xml mbk_API_montecarlo.xml

INF_API = inf_API_netlist.xml inf_API_disa.xml inf_API_timing.xml inf_API_abstraction.xml \
          inf_API_stab.xml inf_API_general.xml inf_API_xtalk.xml

STB_API = stb_API_gen.xml stb_API_error.xml stb_API_montecarlo.xml

SIM_API = sim_API_cpe.xml

CTK_API = ctk_API_build.xml ctk_API_sort.xml ctk_API_aggressor.xml ctk_API_statistics.xml

API = $(TTV_API) $(AVT_API) $(MBK_API) $(INF_API) $(STB_API) $(CTK_API) $(SIM_API) \
      tas_API.xml\
      tma_API.xml\
      lbt_API.xml\

$(TTV_API): $(API_SRCDIR)/ttv/ttv_API.h 
	$(WRAP) -filter $(subst ttv_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/ttv/ttv_API.h

$(AVT_API): $(API_SRCDIR)/tcl/avt_API.h 
	$(WRAP) -filter $(subst avt_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/tcl/avt_API.h

$(MBK_API): $(API_SRCDIR)/mbk/mbk_API.h 
	$(WRAP) -filter $(subst mbk_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/mbk/mbk_API.h

$(INF_API): $(API_SRCDIR)/inf/inf_API.h 
	$(WRAP) -filter $(subst inf_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/inf/inf_API.h

$(STB_API): $(API_SRCDIR)/stb/stb_API.h 
	$(WRAP) -filter $(subst stb_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/stb/stb_API.h
	
$(CTK_API): $(API_SRCDIR)/ctk/ctk_API.h 
	$(WRAP) -filter $(subst ctk_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/ctk/ctk_API.h

$(SIM_API): $(API_SRCDIR)/sim/sim_API.h 
	$(WRAP) -filter $(subst sim_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/sim/sim_API.h
	
tas_API.xml: $(API_SRCDIR)/tas/tas_API.h 
	$(WRAP) -xonly . -xmllevel 4 $(API_SRCDIR)/tas/tas_API.h

tma_API.xml: $(API_SRCDIR)/tma/tma_API.h 
	$(WRAP) -xonly . -xmllevel 4 $(API_SRCDIR)/tma/tma_API.h

lbt_API.xml: $(API_SRCDIR)/lbt/lbt_API.h 
	$(WRAP) -xonly . -xmllevel 4 $(API_SRCDIR)/lbt/lbt_API.h

################################################################################################

$(COMPILED)/dochtml/$(DOC).html: *.xml $(API) $(INF) $(FORMATS) $(CONFIG) 
	cat filename.xsl > $(HTML)/file.xsl
	echo "<xsl:param name=\"release_num\">$(AVT_VERSION)$(DISTRIB_PATCHVERSION)</xsl:param>" >> $(HTML)/file.xsl
	echo "<xsl:param name=\"date\">$(AVT_DATE_A)</xsl:param>" >> $(HTML)/file.xsl
	echo "<xsl:param name=\"dochtmldir\">$(COMPILED)/dochtml</xsl:param></xsl:stylesheet>" >> $(HTML)/file.xsl
	$(SAXON) $(DOC).xml $(HTML)/genHTML.xsl

################################################################################################

$(COMPILED)/docpdf/$(DOC).pdf: *.xml $(API) $(INF) $(FORMATS) $(CONFIG) 
	cat filename.xsl > $(PDF)/file.xsl
	echo "<xsl:param name=\"release_num\">$(AVT_VERSION)$(DISTRIB_PATCHVERSION)</xsl:param>" >> $(PDF)/file.xsl
	echo "<xsl:param name=\"date\">$(AVT_DATE_A)</xsl:param>" >> $(PDF)/file.xsl
	echo  "<xsl:param name=\"pictpdf\">$(PICT_PDF)</xsl:param></xsl:stylesheet>" >> $(PDF)/file.xsl
	$(SAXON) $(DOC).xml $(PDF)/genPDF.xsl > $(DOC).fo
	$(FOP) -c $(PDF)/cfg.xml -fo $(DOC).fo -pdf $(COMPILED)/docpdf/$(DOC).pdf

################################################################################################

clean:
	-@rm -f $(COMPILED)/dochtml/$(DOC)*.html
	-@rm -f $(COMPILED)/dochtml/$(DOC).html
	-@rm -f $(COMPILED)/docpdf/$(DOC).pdf
	-@rm -f $(PDF)/file.xsl
	-@rm -f $(HTML)/file.xsl
	-@rm -f $(API) $(DOC).fo
