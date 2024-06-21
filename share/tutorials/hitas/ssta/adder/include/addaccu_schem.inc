
.TEMP 70
.GLOBAL vdd vss
Vsupply vdd 0 DC 1.1
Vground vss 0 DC 0

.include ../../techno/bsim4_dummy.hsp
.include ./subckt.stat
.include ./addaccu_schem.spi
