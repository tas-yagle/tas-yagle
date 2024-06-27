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

DOC        = hitas_tutorial

all : pdf html
pdf : $(COMPILED)/docpdf/$(DOC).pdf 
html : $(COMPILED)/dochtml/$(DOC).html 

################################################################################################

$(COMPILED)/dochtml/$(DOC).html: *.xml
	cat filename.xsl > $(HTML)/file.xsl
	echo "<xsl:param name=\"release_num\">$(AVT_VERSION)$(DISTRIB_PATCHVERSION)</xsl:param>" >> $(HTML)/file.xsl
	echo "<xsl:param name=\"date\">$(AVT_DATE_A)</xsl:param>" >> $(HTML)/file.xsl
	echo "<xsl:param name=\"dochtmldir\">$(COMPILED)/dochtml</xsl:param></xsl:stylesheet>" >> $(HTML)/file.xsl
	$(SAXON) $(DOC).xml $(HTML)/genHTML.xsl

################################################################################################

$(COMPILED)/docpdf/$(DOC).pdf: *.xml
	cat filename.xsl > $(PDF)/file.xsl
	echo "<xsl:param name=\"release_num\">$(AVT_VERSION)$(DISTRIB_PATCHVERSION)</xsl:param>" >> $(PDF)/file.xsl
	echo "<xsl:param name=\"date\">$(AVT_DATE_A)</xsl:param>" >> $(PDF)/file.xsl
	echo  "<xsl:param name=\"pictpdf\">$(PICT_PDF)</xsl:param></xsl:stylesheet>" >> $(PDF)/file.xsl
	$(SAXON) $(DOC).xml $(PDF)/genPDF.xsl > $(DOC).fo
	$(FOP) -c $(PDF)/cfg.xml -fo $(DOC).fo -pdf $(COMPILED)/docpdf/$(DOC).pdf

clean:
	-@rm -f $(COMPILED)/dochtml/$(DOC)*.html
	-@rm -f $(COMPILED)/dochtml/$(DOC).html
	-@rm -f $(COMPILED)/docpdf/$(DOC).pdf
	-@rm -f $(PDF)/file.xsl
	-@rm -f $(HTML)/file.xsl
	-@rm -f $(DOC).fo

