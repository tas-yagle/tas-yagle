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

# makefile for logical edif view
# version : 407
# date    : 30/11/1995

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

EDIF_LIB = $(MEL_LIB)
EDIF_OBJ = parser_yac.o parser_lex.o driver.o time.o

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -I$(TARGET_INCLUDE) \
               -D$(AVERTEC_LICENSE) \
               -DMLO_H='"$(MLO_H)"' \
               -DRCN_H='"$(RCN_H)"' \
               -DAVT_H='"$(AVT_H)"' \
               -DMUT_H='"$(MUT_H)"'

$(TARGET_LIB)/$(EDIF_LIB) : $(EDIF_OBJ)
	$(RM) -f $(TARGET_LIB)/$(EDIF_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(EDIF_LIB) $(EDIF_OBJ) 
	$(RANLIB) $(TARGET_LIB)/$(EDIF_LIB)

parser_yac.o : parser.yac
	$(YACC) $(YACCFLAGS) -d parser.yac
	echo "s/yy/edifl/g" > transy
	echo "s/YY/EDIFL/g" >> transy
	$(SED) -f transy y.tab.c > parser_yac.c
	$(RM) -f transy y.tab.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c parser_yac.c

parser_lex.o : parser.lex parser_yac.o
	$(LEX) parser.lex
	echo "s/yy/edifl/g" > transl
	echo "s/YY/EDIFL/g" >> transl
	$(SED) -f transl lex.yy.c > parser_lex.c
	$(SED) -f transl y.tab.h > x.tab.h
	$(RM) -f transl lex.yy.c
	mv x.tab.h y.tab.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c parser_lex.c

driver.o : driver.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c driver.c

time.o : time.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c time.c

headers:

clean :
	$(RM) -f *.o parser_yac.c parser_lex.c trans y.tab.c lex.yy.c y.tab.h
realclean : clean
	$(RM) -f $(TARGET_LIB)/$(EDIF_LIB)

