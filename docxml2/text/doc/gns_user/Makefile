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

DOC        = gns_user

all : pdf html
html : $(COMPILED)/dochtml/$(DOC).html
pdf : $(COMPILED)/docpdf/$(DOC).pdf

################################################################################################

API = beg_API.xml \
      database_API.xml \
      fcl_API.xml \
      gen_builtin_functions.xml \
      gen_API.xml \
      mbk_API.xml \
      sim_API.xml \
      stm_API.xml \
      spi_API.xml

beg_API.xml: $(API_SRCDIR)/beg/beg_API.h 
	$(WRAP) -filter "gns" beg_API.xml -xmllevel 3 $(API_SRCDIR)/beg/beg_API.h

database_API.xml: $(API_SRCDIR)/database/database_API.h 
	$(WRAP) -filter "gns" database_API.xml -xmllevel 3 $(API_SRCDIR)/database/database_API.h

fcl_API.xml: $(API_SRCDIR)/fcl/fcl_API.h 
	$(WRAP) -filter "gns" fcl_API.xml -xmllevel 3 $(API_SRCDIR)/fcl/fcl_API.h

gen_builtin_functions.xml: $(API_SRCDIR)/gen_builtins/gen_builtin_functions.h 
	$(WRAP) -filter "gns" gen_builtin_functions.xml -xmllevel 3 $(API_SRCDIR)/gen_builtins/gen_builtin_functions.h

gen_API.xml: $(API_SRCDIR)/genius/gen_API.h
	$(WRAP) -filter "gns" gen_API.xml -xmllevel 3 $(API_SRCDIR)/genius/gen_API.h

mbk_API.xml: $(API_SRCDIR)/mbk/mbk_API.h
	$(WRAP) -filter "gns" mbk_API.xml -xmllevel 3 $(API_SRCDIR)/mbk/mbk_API.h

sim_API.xml: $(API_SRCDIR)/sim/sim_API.h
	$(WRAP) -filter "gns" sim_API.xml -xmllevel 3 $(API_SRCDIR)/sim/sim_API.h

stm_API.xml: $(API_SRCDIR)/stm/stm_API.h
	$(WRAP) -filter "gns" stm_API.xml -xmllevel 3 $(API_SRCDIR)/stm/stm_API.h

spi_API.xml: $(API_SRCDIR)/mbkspice/spi_API.h
	$(WRAP) -filter "gns" spi_API.xml -xmllevel 3 $(API_SRCDIR)/mbkspice/spi_API.h
	
################################################################################################

$(COMPILED)/dochtml/$(DOC).html: *.xml $(API)
	cat filename.xsl > $(HTML)/file.xsl
	echo "<xsl:param name=\"release_num\">$(AVT_VERSION)$(DISTRIB_PATCHVERSION)</xsl:param>" >> $(HTML)/file.xsl
	echo "<xsl:param name=\"date\">$(AVT_DATE_A)</xsl:param>" >> $(HTML)/file.xsl
	echo "<xsl:param name=\"dochtmldir\">$(COMPILED)/dochtml</xsl:param></xsl:stylesheet>" >> $(HTML)/file.xsl
	$(SAXON) $(DOC).xml $(HTML)/genHTML.xsl

################################################################################################

$(COMPILED)/docpdf/$(DOC).pdf: *.xml $(API)
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
