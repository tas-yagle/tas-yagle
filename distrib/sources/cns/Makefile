#
# This file is part of the Alliance CAD System
# Copyright (C) Laboratoire LIP6 - Département ASIM
# Universite Pierre et Marie Curie
# 
# Home page          : http://www-asim.lip6.fr/alliance/
# E-mail support     : mailto:alliance-support@asim.lip6.fr
# 
# This progam is  free software; you can redistribute it  and/or modify it
# under the  terms of the GNU  General Public License as  published by the
# Free Software Foundation;  either version 2 of the License,  or (at your
# option) any later version.
# 
# Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
# useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
# Public License for more details.
# 
# You should have received a copy  of the GNU General Public License along
# with the GNU C Library; see the  file COPYING. If not, write to the Free
# Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

# 10/23/97 Makefile for cns

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

CNSh = cnsi.h cns.h cnsd.h cnsv.h cnsh.h

CNSsce = cnsv.c cnsenv.c cnsptype.c cnslocon.c cnslotrs.c link.c branch.c edge.c inoutcone.c cone.c coneexpr.c cell.c cnsfig.c cnsfigprs.c cnsfigdrv.c cnsstats.c cns_utils.c

CNSallh = cnsall.h

CNSobj = $(CNSsce:.c=.o)

PARSEh = parser/prsh.h

PARSEsce = parser/prsutil.c parser/cnsfig.yac parser/cnsfig.lex

PARSEallh = parser/prsall.h

PARSEobj = parser/prsutil.o parser/cnsfigy.tab.o parser/cnsfiglex.yy.o

CNSDIFF_BIN = cnsdiff

CNS_INC = cns_lib.h

LOCAL_INC = -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                     -D$(AVERTEC_LICENSE) \
					 -D$(AVERTEC_OS) \
					 -DAVT_H='<$(AVT_H)>' \
					 -DMUT_H='<$(MUT_H)>' \
					 -DMLO_H='<$(MLO_H)>' \
					 -DMLU_H='<$(MLU_H)>' \
					 -DEQT_H='<$(EQT_H)>' \
					 -DRCN_H='<$(RCN_H)>' \
					 -DBEG_H='<$(BEG_H)>' \
					 -DBEH_H='<$(BEH_H)>' \
					 -DCBH_H='<$(CBH_H)>' \
					 -DINF_H='<$(INF_H)>' \
					 -DLOG_H='<$(LOG_H)>'

LOCAL_YACCFLAGS = $(YACCFLAGS) -d -t

LOCAL_LEXFLAGS = $(LEXFLAGS)

all : $(TARGET_INCLUDE)/$(CNS_H) $(TARGET_LIB)/$(CNS_LIB)

bin :  $(TARGET_BIN)/$(CNSDIFF_BIN)

headers : $(TARGET_INCLUDE)/$(CNS_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -o $@ $<

$(TARGET_BIN)/$(CNSDIFF_BIN):  
	@cd cnsdiff ; $(MAKE) -f Makefile

$(TARGET_INCLUDE)/$(CNS_H) : $(CNSh)
	echo "/* file   : $(CNS_INC)" > $(TARGET_INCLUDE)/$(CNS_H)
	echo " | date   : `date`" >> $(TARGET_INCLUDE)/$(CNS_H)
	echo " | author : Pascal REMY & Anthony LESTER" >> $(TARGET_INCLUDE)/$(CNS_H)
	echo " | content: #includes, #defines, structures and variables for the CNS library */" >> $(TARGET_INCLUDE)/$(CNS_H)
	echo "" >> $(TARGET_INCLUDE)/$(CNS_H)
	echo "#ifndef CNS" >> $(TARGET_INCLUDE)/$(CNS_H)
	echo "#define CNS" >> $(TARGET_INCLUDE)/$(CNS_H)
	echo "" >> $(TARGET_INCLUDE)/$(CNS_H)
	echo "" >> $(TARGET_INCLUDE)/$(CNS_H)
	$(CAT) $(CNSh) | expand -3 >> $(TARGET_INCLUDE)/$(CNS_H)
	echo "#endif" >> $(TARGET_INCLUDE)/$(CNS_H)
	@if [ -f y.output ] ; then $(RM) y.output ; fi

$(TARGET_LIB)/$(CNS_LIB) : $(CNSobj) $(PARSEh) $(PARSEobj)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(CNS_LIB) $(CNSobj) $(PARSEobj)
	$(RANLIB) $(TARGET_LIB)/$(CNS_LIB)
	@if [ -f y.output ] ; then $(RM) y.output ; fi

$(CNSobj): $(CNSh) $(CNSallh)

parser/prsutil.o : $(CNSh) $(PARSEh) parser/prsutil.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -o parser/prsutil.o parser/prsutil.c

parser/cnsfigy.tab.o : $(CNSh) $(PARSEh) parser/cnsfig.yac
	$(YACC) $(LOCAL_YACCFLAGS) parser/cnsfig.yac
	echo "s/yy/cnsfigyy/g" > parser/tmpy
	echo "s/YY/cnsfigYY/g" >> parser/tmpy
	$(SED) -f parser/tmpy y.tab.c > parser/cnsfigy.tab.c
	$(SED) -f parser/tmpy y.tab.h > parser/cnsfigy.tab.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -o parser/cnsfigy.tab.o parser/cnsfigy.tab.c
	$(RM) y.tab.h y.tab.c 

parser/cnsfiglex.yy.o : parser/cnsfig.lex parser/cnsfigy.tab.o
	$(LEX) $(LOCAL_LEXFLAGS) parser/cnsfig.lex
	echo "s/yy/cnsfigyy/g" > parser/tmpl
	echo "s/YY/cnsfigYY/g" >> parser/tmpl
	echo "s/y\.tab\.h/cnsfigy.tab.h/g" >> parser/tmpl
	$(SED) -f parser/tmpl lex.yy.c > parser/cnsfiglex.yy.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -o parser/cnsfiglex.yy.o parser/cnsfiglex.yy.c
	$(RM) lex.yy.c 

realclean : clean
	$(RM) -f $(TARGET_LIB)/$(CNS_LIB) \
		 $(TARGET_INCLUDE)/$(CNS_H)
	@cd cnsdiff ; $(MAKE) -f Makefile realclean

clean :
	$(RM) -f $(CNSobj) $(PARSEobj) \
        parser/cnsfigy.tab.c parser/cnsfigy.tab.h parser/tmp \
        parser/cnsfiglex.yy.c parser/tmpl parser/tmpy
	@cd cnsdiff ; $(MAKE) -f Makefile clean
