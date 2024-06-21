all: headers tcl final_wrap

headers:
	$(MAKE) -C api headers
	$(MAKE) -C tcl headers

final_wrap: apis
	$(MAKE) -C api final_wrap

tcl: apis
	$(MAKE) -C tcl

apis: wrapper
	$(MAKE) -C mbk
	$(MAKE) -C gen_builtins
	$(MAKE) -C genius
	$(MAKE) -C stm
	$(MAKE) -C beg
	$(MAKE) -C fcl
	$(MAKE) -C mbkspice
	$(MAKE) -C sim
	$(MAKE) -C trc
	$(MAKE) -C ttv
	$(MAKE) -C power
	$(MAKE) -C database
	$(MAKE) -C ctk
	$(MAKE) -C yagle
	$(MAKE) -C tas
	$(MAKE) -C tma
	$(MAKE) -C stb
	$(MAKE) -C inf
	$(MAKE) -C lbt
	$(MAKE) -C ams
	$(MAKE) -C hierPathExtract
	$(MAKE) -C cns
	
wrapper : 
	$(MAKE) -C api

dynamic_link : wrapper
	$(MAKE) -C mbk             dynamic_link
	$(MAKE) -C gen_builtins    dynamic_link
	$(MAKE) -C genius          dynamic_link
	$(MAKE) -C stm             dynamic_link
	$(MAKE) -C beg             dynamic_link
	$(MAKE) -C fcl             dynamic_link
	$(MAKE) -C mbkspice        dynamic_link
	$(MAKE) -C sim             dynamic_link
	$(MAKE) -C trc             dynamic_link
	$(MAKE) -C ttv             dynamic_link
	$(MAKE) -C power           dynamic_link
	$(MAKE) -C database        dynamic_link
	$(MAKE) -C ctk             dynamic_link
	$(MAKE) -C inf             dynamic_link
	$(MAKE) -C lbt             dynamic_link
	$(MAKE) -C ams             dynamic_link
	$(MAKE) -C hierPathExtract dynamic_link
	$(MAKE) -C tas             dynamic_link
	$(MAKE) -C stb             dynamic_link
	$(MAKE) -C tma             dynamic_link
	$(MAKE) -C yagle           dynamic_link
	$(MAKE) -C cns             dynamic_link

clean:
	$(MAKE) -C api          clean
	$(MAKE) -C mbk          clean
	$(MAKE) -C gen_builtins clean
	$(MAKE) -C genius       clean
	$(MAKE) -C stm          clean
	$(MAKE) -C beg          clean
	$(MAKE) -C fcl          clean
	$(MAKE) -C mbkspice     clean
	$(MAKE) -C sim          clean
	$(MAKE) -C trc          clean
	$(MAKE) -C ttv          clean
	$(MAKE) -C power        clean
	$(MAKE) -C database     clean
	$(MAKE) -C ctk          clean
	$(MAKE) -C tcl          clean
	$(MAKE) -C yagle        clean
	$(MAKE) -C tas          clean
	$(MAKE) -C tma          clean
	$(MAKE) -C stb          clean
	$(MAKE) -C inf          clean
	$(MAKE) -C lbt          clean
	$(MAKE) -C ams          clean
	$(MAKE) -C hierPathExtract   clean
	$(MAKE) -C tas 			clean
	$(MAKE) -C stb 			clean
	$(MAKE) -C tma 			clean
	$(MAKE) -C yagle 		clean
	$(MAKE) -C cns          clean

realclean:
	$(MAKE) -C api          realclean
	$(MAKE) -C mbk          realclean
	$(MAKE) -C gen_builtins realclean
	$(MAKE) -C genius       realclean
	$(MAKE) -C stm          realclean
	$(MAKE) -C beg          realclean
	$(MAKE) -C fcl          realclean
	$(MAKE) -C mbkspice     realclean
	$(MAKE) -C sim          realclean
	$(MAKE) -C trc          realclean
	$(MAKE) -C ttv          realclean
	$(MAKE) -C power        realclean
	$(MAKE) -C database     realclean
	$(MAKE) -C ctk          realclean
	$(MAKE) -C tcl          realclean
	$(MAKE) -C yagle        realclean
	$(MAKE) -C tas          realclean
	$(MAKE) -C tma          realclean
	$(MAKE) -C stb          realclean
	$(MAKE) -C inf          realclean
	$(MAKE) -C lbt          realclean
	$(MAKE) -C ams          realclean
	$(MAKE) -C cns          realclean
	$(MAKE) -C hierPathExtract  realclean
