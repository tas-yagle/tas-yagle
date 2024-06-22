# makefile for timing analyzer
# date    : 26/06/96

ifdef BUILD_GRAPHICAL
	XAL_MAKE = @cd xal ; $(MAKE)
	XTAS_MAKE = @cd xtas ; $(MAKE)
else
	XAL_MAKE = @true
	XTAS_MAKE = @true
endif


distrib :
	@cd tlc ; $(MAKE) 
	@cd trc ; $(MAKE) 
	@cd sim ; $(MAKE) 
	@cd stm ; $(MAKE) 
	@cd ttv ; $(MAKE) 
	@cd stb ; $(MAKE)
	@cd bck ; $(MAKE) 
	@cd tut ; $(MAKE) 
	@cd tlf ; $(MAKE) 
	@cd lib ; $(MAKE) 
	@cd mcc ; $(MAKE) 
	@cd stb ; $(MAKE) 
	@cd tas ; $(MAKE) bin
	@cd tma ; $(MAKE) 
	@cd val ; $(MAKE) 
	@cd zen ; $(MAKE) 
	$(XAL_MAKE)
	$(XTAS_MAKE) 

headers :
	-@cd tlc ; $(MAKE) headers
	-@cd trc ; $(MAKE) headers
	-@cd sim ; $(MAKE) headers
	-@cd stm ; $(MAKE) headers
	-@cd ttv ; $(MAKE) headers
	-@cd bck ; $(MAKE) headers
	-@cd tut ; $(MAKE) headers
	-@cd tlf ; $(MAKE) headers
	-@cd lib ; $(MAKE) headers
	-@cd mcc ; $(MAKE) headers
	-@cd stb ; $(MAKE) headers
	-@cd tas ; $(MAKE) headers
	-@cd tma ; $(MAKE) headers
	-@cd val ; $(MAKE) headers
	-@cd zen ; $(MAKE) headers
	$(XAL_MAKE) headers
	$(XTAS_MAKE) headers

clean :
	@cd tlc ; $(MAKE) clean
	@cd trc ; $(MAKE) clean
	@cd stm ; $(MAKE) clean
	@cd ttv ; $(MAKE) clean
	@cd bck ; $(MAKE) clean
	@cd tut ; $(MAKE) clean
	@cd sim ; $(MAKE) clean
	@cd tlf ; $(MAKE) clean
	@cd lib ; $(MAKE) clean
	@cd mcc ; $(MAKE) clean
	@cd stb ; $(MAKE) clean
	@cd tas ; $(MAKE) clean
	@cd tma ; $(MAKE) clean
	@cd etas ; $(MAKE) clean
	@cd x2v ; $(MAKE) clean
	@cd val ; $(MAKE) clean
	@cd zen ; $(MAKE) clean
	$(XAL_MAKE) clean
	$(XTAS_MAKE) clean

realclean :
	@cd tlc ; $(MAKE) realclean
	@cd trc ; $(MAKE) realclean
	@cd stm ; $(MAKE) realclean
	@cd ttv ; $(MAKE) realclean
	@cd bck ; $(MAKE) realclean
	@cd tut ; $(MAKE) realclean
	@cd sim ; $(MAKE) realclean
	@cd tlf ; $(MAKE) realclean
	@cd lib ; $(MAKE) realclean
	@cd mcc ; $(MAKE) realclean
	@cd stb ; $(MAKE) realclean
	@cd tas ; $(MAKE) realclean
	@cd tma ; $(MAKE) realclean
	@cd etas ; $(MAKE) realclean
	@cd x2v ; $(MAKE) realclean
	@cd val ; $(MAKE) realclean
	@cd zen ; $(MAKE) realclean
	$(XAL_MAKE) realclean
	$(XTAS_MAKE) realclean
