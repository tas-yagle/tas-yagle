include $(AVERTEC_TOP)/etc/Avertec.mk

WRAP = wrap_nolicense

HELPDIR    = $(AVERTEC_TOP)/tcl/help/avt
DOCXML     = $(AVERTEC_TOP)/../docxml2
API_SRCDIR = $(DOCXML)/../sources/api
COMPILED   = compiled
PICT_HTML  = imgs_html
HTML       = html

all: directory pictures help
	$(MAKE) -C text/doc/hitas_reference all
	$(MAKE) -C text/doc/hitas_tutorial all
	$(MAKE) -C text/doc/hitas_user all
	$(MAKE) -C text/doc/yagle_reference all
	$(MAKE) -C text/doc/yagle_tutorial all
	$(MAKE) -C text/doc/yagle_user all
	$(MAKE) -C text/doc/gns_user all
	#$(MAKE) -C text/doc/gns_tutorial all
	#$(MAKE) -C text/doc/faq all
	sed "s/RELEASE/$(AVT_VERSION)$(DISTRIB_PATCHVERSION)/" html/docavertec.html > $(COMPILED)/docavertec.html
	cp $(HTML)/tutorialavertec.html $(COMPILED)/dochtml/

pdf: directory
	$(MAKE) -C text/doc/hitas_reference pdf
	$(MAKE) -C text/doc/hitas_tutorial pdf
	$(MAKE) -C text/doc/hitas_user pdf
	$(MAKE) -C text/doc/yagle_reference pdf
	$(MAKE) -C text/doc/yagle_tutorial pdf
	$(MAKE) -C text/doc/yagle_user pdf
	$(MAKE) -C text/doc/gns_user pdf
	#$(MAKE) -C text/doc/gns_tutorial pdf
	#$(MAKE) -C text/doc/faq pdf

html: directory pictures
	$(MAKE) -C text/doc/hitas_reference html
	$(MAKE) -C text/doc/hitas_tutorial html
	$(MAKE) -C text/doc/hitas_user html
	$(MAKE) -C text/doc/yagle_reference html
	$(MAKE) -C text/doc/yagle_tutorial html
	$(MAKE) -C text/doc/yagle_user html
	$(MAKE) -C text/doc/gns_user html
	#$(MAKE) -C text/doc/gns_tutorial html
	#$(MAKE) -C text/doc/faq html
	sed "s/RELEASE/$(AVT_VERSION)$(DISTRIB_PATCHVERSION)/" html/docavertec.html > $(COMPILED)/docavertec.html
	cp $(HTML)/tutorialavertec.html $(COMPILED)/dochtml/

pictures: directory
	cp $(PICT_HTML)/*.* $(COMPILED)/dochtml/imgs


directory:
	@if [ ! -d $(COMPILED) ] ; then \
	  echo %%%%%%%%% creating directory $(COMPILED) ;\
	  mkdir  $(COMPILED) ;\
	  mkdir  $(COMPILED)/dochtml ;\
	  mkdir  $(COMPILED)/docpdf ;\
	  mkdir  $(COMPILED)/dochtml/imgs ;\
	else \
	  echo %%%%%%%%% existing directory $(COMPILED) ;\
	fi ;\

help:
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/sim/sim_API.h
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/ttv/ttv_API.h
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/tcl/avt_API.h
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/mbk/mbk_API.h
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/inf/inf_API.h
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/stb/stb_API.h
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/tas/tas_API.h
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/tma/tma_API.h
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/lbt/lbt_API.h
	$(WRAP) -online tcl $(HELPDIR) $(API_SRCDIR)/ctk/ctk_API.h

clean:
	$(MAKE) -C text/doc/faq clean
	$(MAKE) -C text/doc/gns_user clean
	$(MAKE) -C text/doc/hitas_reference clean
	$(MAKE) -C text/doc/gns_tutorial clean
	$(MAKE) -C text/doc/hitas_tutorial clean
	$(MAKE) -C text/doc/yagle_tutorial clean
	$(MAKE) -C text/doc/hitas_user clean
	$(MAKE) -C text/doc/yagle_reference clean
	$(MAKE) -C text/doc/yagle_user clean
	-@rm -rf $(COMPILED)
	-@rm -rf $(HELPDIR)/*

