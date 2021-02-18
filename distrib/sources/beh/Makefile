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

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

all : headers
	$(MAKE) -C beh
	$(MAKE) -C bhl
	$(MAKE) -C bef
	$(MAKE) -C beg

headers :
	$(MAKE) -C beh $(TARGET_INCLUDE)/$(BEH_H) 
	$(MAKE) -C bhl $(TARGET_INCLUDE)/$(BHL_H)
	$(MAKE) -C bef $(TARGET_INCLUDE)/$(BEF_H)
	$(MAKE) -C beg $(TARGET_INCLUDE)/$(BEG_H)

clean :
	$(MAKE) -C beh clean
	$(MAKE) -C bhl clean
	$(MAKE) -C bef clean
	$(MAKE) -C beg clean

realclean :
	$(MAKE) -C beh realclean
	$(MAKE) -C bhl realclean
	$(MAKE) -C bef realclean
	$(MAKE) -C beg realclean

