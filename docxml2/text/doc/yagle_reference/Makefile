include $(AVERTEC_TOP)/etc/Avertec.mk
include $(AVERTEC_TOP)/etc/AVT_$(AVERTEC_OS).mk

WRAP = wrap_nolicense

DOCXML     = $(AVERTEC_TOP)/docxml2
COMPILED   = $(DOCXML)/compiled
TEXT       = $(DOCXML)/text
PICT_HTML  = $(DOCXML)/imgs_html
PICT_PDF   = $(DOCXML)/imgs_pdf
HTML       = $(DOCXML)/html
PDF        = $(DOCXML)/pdf
API_SRCDIR = $(AVERTEC_TOP)/sources/api
DATE       = $(DOCXML)/date

INF        = $(TEXT)/common/inf/*.xml
FORMATS    = $(TEXT)/common/formats/*.xml
CONFIG     = $(TEXT)/common/configuration/*.xml

DOC        = yagle_reference

all : pdf html
html : $(COMPILED)/dochtml/$(DOC).html
pdf : $(COMPILED)/docpdf/$(DOC).pdf

################################################################################################

AVT_API = avt_API_files.xml avt_API_system.xml avt_API_lofig.xml avt_API_config.xml

MBK_API = mbk_API_files.xml

INF_API = inf_API_netlist.xml inf_API_disa.xml inf_API_yag.xml inf_API_general.xml

API = $(AVT_API) $(MBK_API) $(INF_API) \
      yagle_API.xml

$(AVT_API): $(API_SRCDIR)/tcl/avt_API.h 
	$(WRAP) -filter $(subst avt_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/tcl/avt_API.h

$(MBK_API): $(API_SRCDIR)/mbk/mbk_API.h 
	$(WRAP) -filter $(subst mbk_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/mbk/mbk_API.h

$(INF_API): $(API_SRCDIR)/inf/inf_API.h 
	$(WRAP) -filter $(subst inf_API_,tcl+,$(basename $@)) $@ -xonly . -xmllevel 4 $(API_SRCDIR)/inf/inf_API.h

yagle_API.xml: $(API_SRCDIR)/yagle/yagle_API.h 
	$(WRAP) -xonly . -xmllevel 3 $(API_SRCDIR)/yagle/yagle_API.h

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
