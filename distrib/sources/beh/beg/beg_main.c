#include MUT_H
#include LOG_H
#include BEH_H
#include BEG_H

#include <stdio.h>
#include <stdlib.h>
#include "beg_gen.h"

extern void  avtenv(void);
extern void  savebefig(befig_list *ptbefig, unsigned int trace_mode);

befig_list  *befig;
befig_list  *befig2;

void dotest()
{
  verilogloadlofig(NULL,"testeat",'A');
  return;
}

void addaccutest()
{
  //{{{
  beg_def_befig  ("addaccu");

  beg_def_por    ("a(0:3)"   ,'I');
  beg_def_por    ("b(0:3)"   ,'I');
  beg_def_por    ("sel"      ,'I');
  beg_def_por    ("ck"       ,'I');
  beg_def_por    ("s(0:3)"   ,'O');
  beg_def_por    ("vdd"      ,'I');
  beg_def_por    ("vdde"     ,'I');
  beg_def_por    ("vss"      ,'I');
  beg_def_por    ("vsse"     ,'I');

  beg_assertion  ("vdd and not vss"     ,"Wrong power supplies"         ,
                  'W',NULL);
  beg_assertion  ("vdde and not vsse"   ,"Wrong power external supplies",
                  'W',NULL);
  
  beg_def_mux    ("outmux(0:3)" ,"sel"  ,"0b 0" ,"a(0:3)"          ,0);
  beg_def_mux    ("outmux(0:3)" ,"sel"  ,"0b 1" ,"outreg(0:3)"     ,0);
  
  beg_def_sig    ("sum(0)"      ,"outmux(0) xor b(0)"                   ,0);
  beg_def_sig    ("carry(0)"    ,"outmux(0) and b(0)"                   ,0);
  beg_def_sig    ("sum(1)"      ,"outmux(1) xor b(1) xor carry(0)"      ,0);
  beg_def_sig    ("carry(1)"    ,"(outmux(1) and b(1)) or (outmux(1) "
                  "and carry(0)) or (b(1) and carry(0))"                ,0);
  beg_def_sig    ("sum(2)"      ,"outmux(2) xor b(2) xor carry(1)"      ,0);
  beg_def_sig    ("carry(2)"    ,"(outmux(2) and b(2)) or (outmux(2) "
                  "and carry(1)) or (b(2) and carry(1))"                ,0);
  beg_def_sig    ("sum(3)"      ,"outmux(3) xor b(3) xor carry(2)"      ,0);
  beg_def_sig    ("s(0:3)"      ,"sum(0:3)"                             ,0);
  
  beg_def_process("reg(0:3)"    ,"not ck and ck'event"  ,"not sum(0:3)" ,0,
                  BEG_REG);
  
  beg_def_sig    ("outreg(0:3)" ,"not reg(0:3)"                         ,0);

  befig     = beg_get_befig(0);
  savebefig      (befig,0);
  //}}}
}

void amdtest()
{
  //{{{
  beg_def_befig  ("amd");
  //beg_def_por    ("cke"      ,'I');
  //beg_def_por    ("fonc"     ,'I');
  //beg_def_por    ("test"     ,'I');
  //beg_def_por    ("scin"     ,'I');
  beg_def_por    ("scout"    ,'O');
  //beg_def_por    ("i(8:0)"   ,'I');
  //beg_def_por    ("a(3:0)"   ,'I');
  //beg_def_por    ("b(3:0)"   ,'I');
  //beg_def_por    ("d(3:0)"   ,'I');
  //beg_def_por    ("noe"      ,'I');
  beg_def_por    ("r0"       ,'O');
  beg_def_por    ("r3"       ,'O');
  beg_def_por    ("q0"       ,'O');
  beg_def_por    ("q3"       ,'O');
  beg_def_por    ("ovr"      ,'O');
  beg_def_por    ("zero"     ,'O');
  beg_def_por    ("signe"    ,'O');
  beg_def_por    ("np"       ,'O');
  beg_def_por    ("ng"       ,'O');
  //beg_def_por    ("cin"      ,'I');
  beg_def_por    ("cout"     ,'O');
  beg_def_por    ("y(3:0)"   ,'O');
  beg_def_por    ("vddi"     ,'I');
  beg_def_por    ("vssi"     ,'I');
  beg_def_por    ("vdde"     ,'I');
  beg_def_por    ("vsse"     ,'I');

  beg_assertion  ("vddi and not vssi"   ,"Wrong power supplies",'W',NULL);
  beg_assertion  ("vdde and not vsse"   ,"Wrong power external supplies",'W',NULL);

  beg_def_mux    ("s(3:0)"       ,"i(2:0)"   ,"0o4"   ,"ra(3:0)"      ,0);
  beg_def_mux    ("s(3:0)"       ,0          ,"0o5"   ,"ra(3:0)"      ,0);
  beg_def_mux    ("s(3:0)"       ,0          ,"0o1"   ,"rb(3:0)"      ,0);
  beg_def_mux    ("s(3:0)"       ,0          ,"0o3"   ,"rb(3:0)"      ,0);
  beg_def_mux    ("s(3:0)"       ,0          ,"0o0"   ,"not accu(3:0)",0);
  beg_def_mux    ("s(3:0)"       ,0          ,"0o2"   ,"not accu(3:0)",0);
  beg_def_mux    ("s(3:0)"       ,0          ,"0o6"   ,"not accu(3:0)",0);
  beg_def_mux    ("s(3:0)"       ,0          ,"0o7"   ,"0x 0"         ,0);

  beg_def_mux    ("r(3:0)"       ,"i(2:0)"   ,"0o0"   ,"ra(3:0)"      ,0);
  beg_def_mux    ("r(3:0)"       ,0          ,"0o1"   ,"ra(3:0)"      ,0);
  beg_def_mux    ("r(3:0)"       ,0          ,"0o2"   ,"0x 0"         ,0);
  beg_def_mux    ("r(3:0)"       ,0          ,"0o3"   ,"0x 0"         ,0);
  beg_def_mux    ("r(3:0)"       ,0          ,"0o4"   ,"0x 0"         ,0);
  beg_def_mux    ("r(3:0)"       ,0          ,"0o5"   ,"d(3:0)"       ,0);
  beg_def_mux    ("r(3:0)"       ,0          ,"0o6"   ,"d(3:0)"       ,0);
  beg_def_mux    ("r(3:0)"       ,0          ,"0o7"   ,"d(3:0)"       ,0);

  beg_def_process("y(3:0)"       ,"not noe and i(8:6)=0o2" ,"ra(3:0)" ,0,BEG_PRE|BEG_REG);
  beg_def_process("y(3:0)"       ,"not noe" ,"alu_out(3:0)" ,0,BEG_PRE|BEG_REG);
  beg_def_process("y(3:0)"       ,"0b1"     ,"0xZ" ,0,BEG_PRE|BEG_REG);


  beg_def_sig    ("sumrs(3:0)"   ,"r(3:0) xor s(3:0) xor (c_sumrs(2:0)&cin)"        ,0);
  beg_def_sig    ("c_sumrs(3:0)" ,"( r(3:0) and s(3:0) ) or ( r(3:0) and (c_sumrs(2:0)&cin) ) or ( s(3:0) and (c_sumrs(2:0)&cin) )"               ,0);

  beg_def_sig    ("difrs(3:0)"   ,"r(3:0) xor not s(3:0) xor (c_difrs(2:0)&cin)"    ,0);
  beg_def_sig    ("c_difrs(3:0)" ,"( r(3:0) and not s(3:0) ) or ( r(3:0) and (c_difrs(2:0)&cin) ) or ( not s(3:0) and (c_difrs(2:0)&cin) )"               ,0);

  beg_def_sig    ("difsr(3:0)"   ,"not r(3:0) xor s(3:0) xor (c_difsr(2:0)&cin)"        ,0);
  beg_def_sig    ("c_difsr(3:0)" ,"( not r(3:0) and s(3:0) ) or ( not r(3:0) and (c_difsr(2:0)&cin) ) or ( s(3:0) and (c_difsr(2:0)&cin) )"                   ,0);

  beg_def_mux    ("ng"           ,"i(5:3)"   ,"0o0" ,"not ((r(3) and s(3)) or ((r(3) or s(3)) and (r(2) and s(2))) or ((r(3) or s(3)) and (r(2) or s(2)) and (r(1) and s(1))) or ((r(3) or s(3)) and (r(2) or s(2)) and (r(1) or s(1)) and (r(0) and s(0))))" ,0);
  beg_def_mux    ("ng"           ,0          ,"0o1" ,"NOT ((NOT r(3) AND s(3)) OR ((NOT r(3) OR s(3)) AND (NOT r(2) AND s(2))) OR ((NOT r(3) OR s(3)) AND (NOT r(2) OR s(2)) AND (NOT r(1) AND s(1))) OR ((NOT r(3) OR s(3)) AND (NOT r(2) OR s(2)) AND (NOT r(1) OR s(1)) AND (NOT r(0) AND s(0))))" ,0);
  beg_def_mux    ("ng"           ,0          ,"0o2" ,"NOT ((r(3) AND NOT s(3)) OR ((r(3) OR NOT s(3)) AND (r(2) AND NOT s(2))) OR ((r(3) OR NOT s(3)) AND (r(2) OR NOT s(2)) AND (r(1) AND NOT s(1))) OR ((r(3) OR NOT s(3)) AND (r(2) OR NOT s(2)) AND (r(1) OR NOT s(1)) AND (r(0) AND NOT s(0))))" ,0);

  beg_def_mux    ("np"           ,"i(5:3)"   ,"0o0" ,"NOT ((r(3) OR s(3)) AND (r(2) OR s(2)) AND (r(1) OR s(1)) AND (r(0) OR s(0)))" ,0);
  beg_def_mux    ("np"           ,0          ,"0o1" ,"NOT ((NOT r(3) OR s(3)) AND (NOT r(2) OR s(2)) AND (NOT r(1) OR s(1)) AND (NOT r(0) OR s(0)))" ,0);
  beg_def_mux    ("np"           ,0          ,"0o2" ,"NOT ((r(3) OR NOT s(3)) AND (r(2) OR NOT s(2)) AND (r(1) OR NOT s(1)) AND (r(0) OR NOT s(0)))" ,0);

  beg_def_sig    ("signe"        ,"alu_out(3)"                              ,0);
  beg_def_sig    ("zero"         ,"NOT (alu_out(3) OR alu_out(2) OR alu_out(1) OR alu_out(0))" ,0);

  beg_def_mux    ("ovr"          ,"i(5:3)"   ,"0o0" ,"c_sumrs(3) xor c_sumrs(2)" ,0);
  beg_def_mux    ("ovr"          ,0          ,"0o1" ,"c_difsr(3) xor c_difsr(2)" ,0);
  beg_def_mux    ("ovr"          ,0          ,"0o2" ,"c_difrs(3) xor c_difrs(2)" ,0);
  beg_def_mux    ("ovr"          ,0          ,"default","0b 0"       ,0);

  beg_def_mux    ("alu_out(3:0)" ,"i(5:3)"   ,"0o0" ,"sumrs(3:0)" ,0);
  beg_def_mux    ("alu_out(3:0)" ,0          ,"0o1" ,"difsr(3:0)" ,0);
  beg_def_mux    ("alu_out(3:0)" ,0          ,"0o2" ,"difrs(3:0)" ,0);
  beg_def_mux    ("alu_out(3:0)" ,0          ,"0o3" ,"r(3:0) or s(3:0)" ,0);
  beg_def_mux    ("alu_out(3:0)" ,0          ,"0o4" ,"r(3:0) and s(3:0)" ,0);
  beg_def_mux    ("alu_out(3:0)" ,0          ,"0o5" ,"not r(3:0) and s(3:0)" ,0);
  beg_def_mux    ("alu_out(3:0)" ,0          ,"0o6" ,"r(3:0) xor s(3:0)" ,0);
  beg_def_mux    ("alu_out(3:0)" ,0          ,"0o7" ,"not (r(3:0) xor s(3:0))" ,0);

  beg_def_mux    ("cout"         ,"i(5:3)"   ,"0o0" ,"c_sumrs(3)" ,0);
  beg_def_mux    ("cout"         ,0          ,"0o1" ,"c_difsr(3)" ,0);
  beg_def_mux    ("cout"         ,0          ,"0o2" ,"c_difrs(3)" ,0);
  beg_def_mux    ("cout"         ,0          ,"default","0b 0"       ,0);

  beg_def_sig    ("wram"         ,"(i(8) OR i(7)) AND fonc_mode"      ,0);
  beg_def_sig    ("waccu"        ,"NOT (i(6)) AND ((NOT i(7)) OR i(8))" ,0);
  beg_def_sig    ("fonc_mode"    ,"fonc AND (NOT test)"              ,0);
  beg_def_sig    ("test_mode"    ,"test AND (NOT fonc)"              ,0);
  beg_def_sig    ("shift_r"      ,"i(8) AND (NOT i(7))"              ,0);
  beg_def_sig    ("shift_l"      ,"i(8) AND i(7)"                  ,0);
  beg_def_sig    ("shift_n"      ,"NOT i(8)"                          ,0);

  beg_def_mux    ("sh_acc(3)"    ,"i(8:6)"   ,"0o0" ,"alu_out(3)" ,0);
  beg_def_mux    ("sh_acc(3)"    ,0          ,"0o6" ,"not accu(2)" ,0);
  beg_def_mux    ("sh_acc(3)"    ,0          ,"0o7" ,"not accu(2)" ,0);
  beg_def_mux    ("sh_acc(3)"    ,0          ,"0o4" ,"q3" ,0);
  beg_def_mux    ("sh_acc(3)"    ,0          ,"0o5" ,"q3" ,0);
  beg_def_mux    ("sh_acc(3)"    ,0          ,"default","0b 0" ,0);

  beg_def_mux    ("sh_acc(2)"    ,"i(8:6)"   ,"0o0" ,"alu_out(2)" ,0);
  beg_def_mux    ("sh_acc(2)"    ,0          ,"0o6" ,"not accu(1)" ,0);
  beg_def_mux    ("sh_acc(2)"    ,0          ,"0o7" ,"not accu(1)" ,0);
  beg_def_mux    ("sh_acc(2)"    ,0          ,"0o4" ,"not accu(3)" ,0);
  beg_def_mux    ("sh_acc(2)"    ,0          ,"0o5" ,"not accu(3)" ,0);
  beg_def_mux    ("sh_acc(2)"    ,0          ,"default","0b 0" ,0);

  beg_def_mux    ("sh_acc(1)"    ,"i(8:6)"   ,"0o0" ,"alu_out(1)" ,0);
  beg_def_mux    ("sh_acc(1)"    ,0          ,"0o6" ,"not accu(0)" ,0);
  beg_def_mux    ("sh_acc(1)"    ,0          ,"0o7" ,"not accu(0)" ,0);
  beg_def_mux    ("sh_acc(1)"    ,0          ,"0o4" ,"not accu(2)" ,0);
  beg_def_mux    ("sh_acc(1)"    ,0          ,"0o5" ,"not accu(2)" ,0);
  beg_def_mux    ("sh_acc(1)"    ,0          ,"default","0b 0" ,0);

  beg_def_mux    ("sh_acc(0)"    ,"i(8:6)"   ,"0o0" ,"alu_out(0)" ,0);
  beg_def_mux    ("sh_acc(0)"    ,0          ,"0o6" ,"q0" ,0);
  beg_def_mux    ("sh_acc(0)"    ,0          ,"0o7" ,"q0" ,0);
  beg_def_mux    ("sh_acc(0)"    ,0          ,"0o4" ,"not accu(1)" ,0);
  beg_def_mux    ("sh_acc(0)"    ,0          ,"0o5" ,"not accu(1)" ,0);
  beg_def_mux    ("sh_acc(0)"    ,0          ,"default","0b 0" ,0);

  beg_def_process("q3"           ,"i(8:6)=0o6 or i(8:6)=0o7","not accu(3)",0,BEG_REG);
  beg_def_process("q0"           ,"i(8:6)=0o4 or i(8:6)=0o5","not accu(0)",0,BEG_REG);

  beg_def_sig    ("wckaccu"      ,"cke AND (test_mode OR (waccu AND fonc_mode))" ,0);

  beg_def_mux    ("accu_in(3:0)" ,"test_mode","0b0","sh_acc(3:0)"   ,0);
  beg_def_mux    ("accu_in(3:0)" ,0          ,"0b1","not accu(2:0) & scin",0);

  beg_def_process("accu(3:0)"    ,"not wckaccu and not wckaccu'stable"   ,"not accu_in(3:0)",0,BEG_REG);

  beg_def_sig    ("scout"        ,"not accu(3) and test_mode"       ,0);

  beg_def_mux    ("sh_ram(3)"    ,"shift_n & shift_l & shift_r","0o4","alu_out(3)" ,0);
  beg_def_mux    ("sh_ram(3)"    ,0          ,"0o2"    ,"alu_out(2)",0);
  beg_def_mux    ("sh_ram(3)"    ,0          ,"0o1"    ,"r3"        ,0);
  beg_def_mux    ("sh_ram(3)"    ,0          ,"default","0b0"       ,0);

  beg_def_mux    ("sh_ram(2)"    ,"shift_n & shift_l & shift_r","0o4","alu_out(2)" ,0);
  beg_def_mux    ("sh_ram(2)"    ,0          ,"0o2"    ,"alu_out(1)",0);
  beg_def_mux    ("sh_ram(2)"    ,0          ,"0o1"    ,"alu_out(3)",0);
  beg_def_mux    ("sh_ram(2)"    ,0          ,"default","0b0" ,0);

  beg_def_mux    ("sh_ram(1)"    ,"shift_n & shift_l & shift_r","0o4","alu_out(1)" ,0);
  beg_def_mux    ("sh_ram(1)"    ,0          ,"0o2"    ,"alu_out(0)",0);
  beg_def_mux    ("sh_ram(1)"    ,0          ,"0o1"    ,"alu_out(2)",0);
  beg_def_mux    ("sh_ram(1)"    ,0          ,"default","0b0"       ,0);

  beg_def_mux    ("sh_ram(0)"    ,"shift_n & shift_l & shift_r","0o4","alu_out(0)" ,0);
  beg_def_mux    ("sh_ram(0)"    ,0          ,"0o2"    ,"r0"        ,0);
  beg_def_mux    ("sh_ram(0)"    ,0          ,"0o1"    ,"alu_out(1)",0);
  beg_def_mux    ("sh_ram(0)"    ,0          ,"default","0b0"       ,0);

  beg_def_process("r3"           ,"shift_l"  ,"not alu_out(3)"      ,0,BEG_REG);
  beg_def_process("r0"           ,"shift_l"  ,"not alu_out(0)"      ,0,BEG_REG);

  beg_def_sig    ("wck(0)"       ,"cke and wram and not b(3) and not b(2) and not b(1) and not b(0)",0);
  beg_def_sig    ("wck(1)"       ,"cke and wram and not b(3) and not b(2) and not b(1) and b(0)",0);
  beg_def_sig    ("wck(2)"       ,"cke and wram and not b(3) and not b(2) and b(1) and not b(0)",0);
  beg_def_sig    ("wck(3)"       ,"cke and wram and not b(3) and not b(2) and b(1) and b(0)",0);
  beg_def_sig    ("wck(4)"       ,"cke and wram and not b(3) and b(2) and not b(1) and not b(0)",0);
  beg_def_sig    ("wck(5)"       ,"cke and wram and not b(3) and b(2) and not b(1) and b(0)",0);
  beg_def_sig    ("wck(6)"       ,"cke and wram and not b(3) and b(2) and b(1) and not b(0)",0);
  beg_def_sig    ("wck(7)"       ,"cke and wram and not b(3) and b(2) and b(1) and b(0)",0);
  beg_def_sig    ("wck(8)"       ,"cke and wram and b(3) and not b(2) and not b(1) and not b(0)",0);
  beg_def_sig    ("wck(9)"       ,"cke and wram and b(3) and not b(2) and not b(1) and b(0)",0);
  beg_def_sig    ("wck(10)"      ,"cke and wram and b(3) and not b(2) and b(1) and not b(0)",0);
  beg_def_sig    ("wck(11)"      ,"cke and wram and b(3) and not b(2) and b(1) and b(0)",0);
  beg_def_sig    ("wck(12)"      ,"cke and wram and b(3) and b(2) and not b(1) and not b(0)",0);
  beg_def_sig    ("wck(13)"      ,"cke and wram and b(3) and b(2) and not b(1) and b(0)",0);
  beg_def_sig    ("wck(14)"      ,"cke and wram and b(3) and b(2) and b(1) and not b(0)",0);
  beg_def_sig    ("wck(15)"      ,"cke and wram and b(3) and b(2) and b(1) and b(0)",0);

  beg_def_biloop ("ram(j+4*i)"   ,"not wck(i) and not wck(i)'stable","sh_ram(j)","i(15:0)","j(3:0)",33,BEG_REG);

  beg_def_mux    ("rb(3:0)"      ,"b(3:0)"   ,"0x0"    ,"not ram (3:0)" ,0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0x1"    ,"not ram (7:4)" ,0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0x2"    ,"not ram(11:8)" ,0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0x3"    ,"not ram(15:12)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0x4"    ,"not ram(19:16)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0x5"    ,"not ram(23:20)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0x6"    ,"not ram(27:24)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0x7"    ,"not ram(31:28)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0x8"    ,"not ram(35:32)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0x9"    ,"not ram(39:36)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0xa"    ,"not ram(43:40)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0xb"    ,"not ram(47:44)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0xc"    ,"not ram(51:48)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0xd"    ,"not ram(55:52)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0xe"    ,"not ram(59:56)",0);
  beg_def_mux    ("rb(3:0)"      ,0          ,"0xf"    ,"not ram(63:60)",0);

  beg_def_mux    ("ra(3:0)"      ,"a(3:0)"   ,"0x0"    ,"not ram (3:0)" ,0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0x1"    ,"not ram (7:4)" ,0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0x2"    ,"not ram(11:8)" ,0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0x3"    ,"not ram(15:12)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0x4"    ,"not ram(19:16)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0x5"    ,"not ram(23:20)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0x6"    ,"not ram(27:24)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0x7"    ,"not ram(31:28)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0x8"    ,"not ram(35:32)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0x9"    ,"not ram(39:36)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0xa"    ,"not ram(43:40)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0xb"    ,"not ram(47:44)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0xc"    ,"not ram(51:48)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0xd"    ,"not ram(55:52)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0xe"    ,"not ram(59:56)",0);
  beg_def_mux    ("ra(3:0)"      ,0          ,"0xf"    ,"not ram(63:60)",0);

  befig        = beg_get_befig (0);
  savebefig(befig,0);
  befig2       = beh_duplicate(befig);
  beh_frebefig(befig);
  befig2->NAME = namealloc("duplication");
  savebefig(befig2,0);
  befig2->NAME = namealloc("biterizing");
  beh_biterize(befig2);
  savebefig(befig2,0);
  beh_frebefig(befig2);
  //}}}
}

void conflicttest()
{
  //{{{
#ifdef size
#undef size
#endif
#define     size    128
  int            i;
  char           buf[3][1024];
  befig_list    *fig;


  beg_def_befig(namealloc("testconflict"));

  for (i = 0; i < 128; i ++)
  {
    sprintf(buf[1],"outsaen=0b1 and dat(%d)=0b1 and daf(%d)=0b0",i,i);
    sprintf(buf[0],"dati(%d)",i);
    beg_def_process (buf[0] ,buf[1],"0b1",0,BEG_REG);

    sprintf(buf[1],"outsaen=0b1 and dat(%d)=0b0 and daf(%d)=0b1",i,i);
    sprintf(buf[0],"dati(%d)",i);
    beg_def_process (buf[0] ,buf[1],"0b0",0,BEG_REG|BEG_PRE);
  }
  beg_sort();
  beg_compact();

  befig    = beg_get_befig (0);
  //
  beg_def_befig(namealloc("testconflict2"));
  for (i = 0; i < 128; i ++)
  {
    sprintf(buf[1],
            "(dafi(%d)=0b0 and dinla%d_net202=0b0) or "
            "dinla%d_dinbpdw=0b1",i,
            128,128);
            //128-i,128-i);
    sprintf(buf[0],"dati(%d)",i);
    beg_def_process (buf[0] ,buf[1],"0b0",0,BEG_REG);

    sprintf(buf[1],
            "(dafi(%d)=0b1 and dinla%d_net202=0b0) or "
            "(dinla%d_dinbpup=0b0 and dinla%d_net202=0b0) or "
            "dinla%d_dinipup=0b0",i,
            128,128,128,128);
            //128-i,128-i,128-i,128-i);
    sprintf(buf[0],"dati(%d)",i);
    beg_def_process (buf[0] ,buf[1],"0b0",0,BEG_REG|BEG_PRE);
  }
  beg_sort();
  beg_compact();
  fig       = beg_get_befig(0);
//  savebefig(fig,0);

  beg_eat_figure(befig);

  fig       = beg_get_befig(0);
  savebefig(fig,0);
  //}}}
}

/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void printhour(time_t tim)
{
  int    h, m, s;

  s     = tim % 60;
  m     = (tim/60) % 60;
  h     = (tim/3600);

  printf(" ");
  if (h) printf("%d:",h);
  if (m) printf("%2d'",m);
  printf("%2d\" - OK\n",s);
}

/*}}}************************************************************************/


int main(int argc, char **argv)
{

  char          *str;
  chain_list    *abl;
  chain_list    *abl2;
  int            test;
  char           dat[1024],sig[1024], nsig[1024], cmd[1024], fig[1024], mem[1024], *mainfig;
  int            i, j, k, l, m, n;
  chain_list    *list;
  time_t         cur, beg, bef, ela;

  avtenv();
  mbkenv();

  test      = (argc > 1) ? atoi(argv[1]) : 6;

  time(&beg);
  bef       = beg;
  
  switch (test)
  {
    case 1 : // test -1- parser d'expression
         //{{{
         str    = namealloc("je(3 downto 1) & xab & (('1'&'0') "
                            "and test(1 downto 0) ) & (le xor premier)");
         printf("Test de : \"%s\"\n",str);
         abl    = beg_genStr2Abl(str);
         displayExpr(abl);
         str    = namealloc("a and b and c and d");
         printf("Test de : \"%s\"\n",str);
         abl    = beg_genStr2Abl(str);
         displayExpr(abl);
         str    = namealloc("not ck'stable");
         printf("Test de : \"%s\"\n",str);
         abl    = beg_genStr2Abl(str);
         displayExpr(abl);
         str    = namealloc("ck'event and r");
         printf("Test de : \"%s\"\n",str);
         abl    = beg_genStr2Abl(str);
         displayExpr(abl);
         str    = namealloc("ck'stable");
         printf("Test de : \"%s\"\n",str);
         abl    = beg_genStr2Abl(str);
         displayExpr(abl);
         //}}}
    case 2 : // test -2- inversion d'expression
         //{{{
         invertVectAbl(abl);
         displayExpr(abl);
         //}}}
         break;
    case 3 : // test -3- parser de bit string
         //{{{
         if (!strcmp("\"111101011010001001\"",beg_genBitStr("0O 7_5_3211")))
           printf("OK\n");
         else
           printf("Test FAILED\n");
         if (!strcmp("\"10101010101111010001000001111001010100010011\"",
                     beg_genBitStr("0X aAbD1079513")))
           printf("OK\n");
         else
           printf("Test FAILED\n");
         //}}}
         break;
    case 4 :
         //{{{
         abl    = createBitStr(genUBitStr(10));
         printf("abl  :");
         displayExpr(abl);
         str    = namealloc("a and b");
         abl2   = beg_genStr2Abl(str);
         printf("abl2 :");
         displayExpr(abl2);
         changeAblAtRange(abl,0,0,abl2,1);
         printf("changeAblAtRange (abl,5,5,abl2,0) :");
         displayExpr(abl);
         abl    = createBitStr(genHZBitStr(10));
         printf("abl  :");
         displayExpr(abl);
         printf("abl2 :");
         displayExpr(abl2);
         changeAblAtRange(abl,5,5,abl2,1);
         printf("changeAblAtRange (abl,5,5,abl2,0) :");
         displayExpr(abl);
         abl    = beg_genStr2Abl(namealloc("test(10:4)"));
         printf("abl  :");
         displayExpr(abl);
         printf("abl2 :");
         displayExpr(abl2);
         changeAblAtRange(abl,5,5,abl2,2);
         printf("changeAblAtRange (abl,5,5,abl2,0) :");
         displayExpr(abl);
         str    = namealloc("je(3 downto 1) & 0x ab & (('1'&'0') "
                            "and test(1 downto 0) ) & (le xor premier)");
         abl    = beg_genStr2Abl(str);
         printf("abl  :");
         displayExpr(abl);
         if (!changeAblAtRange(abl,10,10,abl2,2))
           printf("rate\n");
         printf("changeAblAtRange (abl,10,10,abl2,0) :");
         displayExpr(abl);
         str    = namealloc("^4 b(3:1)");
         abl    = beg_genStr2Abl(str);
         printf("abl  :");
         displayExpr(abl);
         repToCatVectAbl(abl);
         printf("abl  :");
         displayExpr(abl);
         //}}}
         break;
    case 5 : // test -
//         //{{{
//         str    = namealloc("testrigolo2, 1");
//         str3 = beg_changeName(str,str,"i",0,1);
//         if (str3)
//           printf("%s#\n",str3);
//         else
//           printf("erreur\n");
//
//         str2   = mbkstrdup("testrigolo2, 1");
//         //         str3 = beg_changeName(str,str2,"i",1,1);
//         if (str3)
//           printf("%s#\n",str3);
//         else
//           printf("erreur\n");
//         str2   = namealloc("testrigolo2, 2");
//         //         str3 = beg_changeName(str,str2,"i",2,1);
//         if (str3)
//           printf("%s#\n",str3);
//         else
//           printf("erreur\n");
//         str2   = namealloc("testrigolo1, 3");
//         //         str3 = beg_changeName(str,str2,"i",3,1);
//         if (str3)
//           printf("%s#\n",str3);
//         else
//           printf("erreur\n");
//         str2   = namealloc("testrigolo2, 4");
//         //         str3 = beg_changeName(str,str2,"i",4,1);
//         if (str3)
//           printf("%s#\n",str3);
//         else
//           printf("erreur\n");
//         str2   = namealloc("testrigolo2, 5");
//         //         str3 = beg_changeName(str,str2,"i",5,1);
//         if (str3)
//           printf("%s#\n",str3);
//         else
//           printf("erreur\n");
//         //}}}
    case 6 : // test -6- generation de beh + driver vectoriel
         //{{{
         beg_def_befig("testchip");
         // test of por
         beg_def_por  ("tet(1)",'I');
         beg_def_por  ("s"     ,'O');
         beg_def_por  ("a"     ,'I');
         beg_def_por  ("b(8:0)",'O');
         beg_def_por  ("t(0:4)",'O');
         beg_def_por  ("z(5:2)",'O');
         beg_def_por  ("y(7:0)",'O');

         // test of assertion
         beg_assertion("ck and ck'event"   ,"horloge stable",'W',"horloge1");
         beg_assertion("ck and ck'stable"   ,"horloge stable",'W',"horloge2");
         beg_assertion("ck and not ck'stable(23)" ,"horloge stable",'W',"horloge3");
         beg_assertion("ck and ck'event and a" ,"horloge stable",'W',"horloge4");
         beg_assertion("a"      ,"horloge stable",'W',"horloge5");

         // test of signal
         beg_def_sig  ("b(8)"     ,"r and tet(1)"             ,156);
         beg_def_sig  ("b(2:0)"   ,"0b 011"                   ,156);
         beg_def_sig  ("b(5:3)"   ,"0b 101"                   ,156);
         beg_def_sig  ("t(3:4)"   ,"c(2:1)"                   ,156);
         beg_def_sig  ("t(0:2)"   ,"c(3:1)"                   ,156);
         beg_def_sig  ("e(4)"     ,"c(4)"                     ,156);
         beg_def_sig  ("k"       ,"c(4)"                     ,156);
         beg_def_sig  ("c(3:1)"   ,"(a and r) & '0' & tet(1)" ,254);

         // test of multiplexer
         beg_def_mux  ("u"        ,"ab(5:4)"  ,"0b 11"    ,"f and e(4)"   ,26);
         beg_def_mux  ("u"        ,0          ,"0b 01"    ,"c(3) and e(4)",27);
         beg_def_mux  ("u"        ,0          ,"0b 10"    ,"c(3)"         ,23);

         beg_def_mux  ("x(5:4)"   ,"ab(5:4)"  ,"0b 01"    ,"d(4) & e(4)"  ,29);
         beg_def_mux  ("x(5:4)"   ,"ab(5:4)"  ,"0b 11"    ,"c(3) & e(4)"  ,27);
         beg_def_mux  ("x(5:4)"   ,"ab(5:4)"  ,"default"  ,"b(4) & e(4)"  ,28);

         // test of process
         beg_def_process("r"        ,"not ck and ck'event and cm"   ,"c(3) and e(4)",26,BEG_REG);

         beg_def_process("r2"       ,"cm"       ,"c(3) and e(4)",26,BEG_REG);
         beg_def_process("r2"       ,"not cm"   ,"c(3) and e(4)",26,BEG_REG|BEG_PRE);

         beg_def_process("r3(2)"    ,"cm"       ,"c(3) and e(4)",26,BEG_REG);
         beg_def_process("r3(2)"    ,"not cm"   ,"c(3) and e(4)",26,BEG_REG);

         beg_def_process("q(2:0)"   ,"ck and ck'event and cm"     ,"c(3)&r&r" ,26,BEG_REG);
         beg_def_process("q(2:0)"   ,"ck and ck'event and not cm" ,"c(3)&r&r" ,26,BEG_REG);

         beg_def_process("s"        ,"ck"      ,"c(3) and e(4)",26,BEG_BUS);
         beg_def_process("s"        ,"d(2:4)"   ,"c(3) and e(4)",26,BEG_BUS|BEG_PRE);


         beg_def_process("y(3:0)"   ,"not d(2:4) & c(3)","c(3:0)"       ,23,BEG_BUS);
         beg_def_process("y(7:6)"   ,"not d(2:4) & c(3)","c(4) & e(4)"  ,23,BEG_BUS);
         beg_def_process("y(5:4)"   ,"not (a) & a"      ,"c(3) & e(4)"  ,24,BEG_BUS);
         beg_def_process("y(5:4)"   ,"a & not a"        ,"b(4) & e(4)"  ,25,BEG_BUS);
         beg_def_process("y(5:4)"   ,"^2 a"             ,"d(4) & e(4)"  ,26,BEG_BUS);

         // test of preconditionned bus
         beg_def_process("z(5:4)"   ,"not (a) & a"      ,"c(3) & e(4)"  ,27,BEG_BUS|BEG_PRE);
         beg_def_process("z(5:4)"   ,"f & not a"        ,"b(4) & e(4)"  ,28,BEG_BUS|BEG_PRE);
         beg_def_process("z(5:4)"   ,"^2 a"             ,"d(4) & e(4)"  ,29,BEG_BUS|BEG_PRE);

         // test of loop
         beg_def_loop   ("n(i)"     ,"com(i)","g(i)" ,"i(0:8)"     ,33,0);
         beg_def_loop   ("n(i)"     ,"m(i)"  ,"g(i)" ,"i(0:8)"     ,33,0);
         beg_def_loop   ("n(i)"     ,"g(i)"  ,"m(i)" ,"i(0:8)"     ,33,0);

         // test of mixed loop and non loop
         beg_def_process("nn(9)"    ,"g(4)"             ,"m(4)"      ,33,BEG_REG);
         beg_def_loop   ("nn(9)"    ,"ck and ck'event and not com(j)","d(j)","j(0:8)",33,BEG_REG);
         beg_def_process("nn(9)"    ,"g(4)"             ,"m(4)"      ,33,BEG_REG);

         // test of mulitple loop
         beg_def_loop   ("m(i)"     ,"ck and ck'event and com(i)"    ,"g(i)","i(0:8)",33,BEG_REG);
         beg_def_loop   ("m(i)"     ,"ck and ck'event and not com(i)","d(i)","i(0:8)",33,BEG_REG);
         beg_def_loop   ("m(j)"     ,"ck and ck'event and not com(j)","d(j)","j(0:8)",33,BEG_REG);
         beg_def_process("m(9)"     ,"ck and ck'event and not com(9)","d(9)",33,BEG_REG);

         // test of single conditionned by loop
         beg_def_loop   ("h"     ,"ck and ck'event and not com(j)","d(j)","j(0:8)",33,BEG_REG);

         // test of vector conditionned by loop
         beg_def_loop   ("nm(9:5)"  ,"not com(j)","nm(9:5)" ,"j(0:8)"   ,33,0);

         // test of biloop
         beg_def_biloop ("o(j+8*i)" ,"ck and ck'event and not com(j)","d(i)","i(9:6)","j(8:0)",33,BEG_REG);

         // test of loop detection
         beg_def_process("ld(2)"    ,"ck and ck'event and g(3)","m(2)"  ,33,BEG_REG);
         beg_def_process("ld(3)"    ,"ck and ck'event and g(4)","m(3)" ,33,BEG_REG);
         beg_def_process("ld(4)"    ,"ck and ck'event and g(5)","m(4)" ,36,BEG_REG);
         beg_def_process("ld(5)"    ,"ck and ck'event and g(6)","m(5)" ,33,BEG_REG);

         beg_def_process("lv(2)"    ,"ck and ck'event"         ,"m(2)" ,33,BEG_REG);
         beg_def_process("lv(3)"    ,"ck and ck'event"         ,"m(3)" ,33,BEG_REG);
         beg_def_process("lv(4)"    ,"ck and ck'event"         ,"m(4)" ,36,BEG_REG);

         beg_def_process("lv(5)"    ,"toto(3) and toto(5)"     ,"m(5)" ,33,BEG_REG);
         beg_def_process("lv(5)"    ,"toto(4) and toto(6)"     ,"m(5)" ,33,BEG_REG);
         beg_def_process("lv(5)"    ,"toto(5) and toto(7)"     ,"m(5)" ,33,BEG_REG);
         beg_def_process("lv(5)"    ,"toto(6) and toto(8)"     ,"m(5)" ,33,BEG_REG);

         beg_def_process("lvb(5)"   ,"toto(3) and toto(5)"     ,"m(5)" ,33,BEG_REG);
         beg_def_process("lvb(5)"   ,"toto(4) and toto(6)"     ,"m(5)" ,33,BEG_REG);

         beg_def_process("lvb(6)"   ,"toto(3) and toto(5)"     ,"m(6)" ,33,BEG_REG);
         beg_def_process("lvb(6)"   ,"toto(4) and toto(6)"     ,"m(6)" ,33,BEG_REG);


         befig  = beg_get_befig(0);

         beg_def_befig  ("testeat");
         beg_def_por    ("y(7:0)",'O');
         beg_def_por    ("s"     ,'O');
         beg_def_por    ("se 1"  ,'O');
         beg_def_por    ("se 2"  ,'O');

         //obj = beg_getObjByName(befig,namealloc("lv"),str);
         //printf("test getObjByName %s %d:%d\n",beg_getName(obj),beg_getLeft(obj),
         //       beg_getRight(obj));

         beg_eat_figure (befig);

         beg_sort       ();
         beg_compact    ();

         befig  = beg_get_befig (0);
         savebefig      (befig,0);

         dotest         ();
         //}}}
         break;
    case 7 : // perform addacu test
         addaccutest();
         break;

    case 8 : // perform amd test
         amdtest();
         break;
    case 9 : // perform addacu test
         //{{{
         beg_def_befig ("biloop");

         //beg_def_por  ("m(0:3)"   ,'I');
         //beg_def_por  ("b(0:3)"   ,'I');


         //beg_def_process ("m(0)" ,"b(0)"   ,"0b1" ,0,BEG_REG);
         beg_def_loop ("m(i)"     ,"c(i)"   ,"0b1" ,"i(0:7)",33,BEG_REG);

         //beg_def_process ("m(1)" ,"b(1)"   ,"0b1" ,0,BEG_REG);
         beg_def_loop ("m(i+8)" ,"c(i+8)"   ,"0b1" ,"i(0:7)",33,BEG_REG);

         //beg_def_process ("m(2)" ,"b(2)"   ,"0b1" ,0,BEG_REG);
         beg_def_loop ("m(i+16)" ,"c(i+16)"   ,"0b1" ,"i(0:7)",33,BEG_REG);

         //beg_def_process ("m(3)" ,"b(3)"   ,"0b1" ,0,BEG_REG);
         beg_def_loop ("m(i+24)" ,"c(i+24)"   ,"0b1" ,"i(0:7)",33,BEG_REG);

         beg_sort();
         beg_compact();

         befig = beg_get_befig (0);
         savebefig     (befig,0);

         //}}}
         break;
    case 10 :
         //{{{
         beg_testCalcRange();
         //}}}
         break;
    case 11 :
         //{{{
         {
#ifdef size
#undef size
#endif
#define         size 128
           int         i,j,l;
           char        buf[10][1024];
           befig_list *fig[size];

           for (l = 0; l < size; l ++)
           {
             sprintf(buf[3],"tmp_%d",l);
             beg_def_befig(buf[3]);
             sprintf(buf[4],"blt(%d:%d)",3+4*l,4*l);
             beg_def_por(buf[4],'O');
             for (i = 0; i < 1024; i ++)
               for (j = 0; j < 4; j ++)
               {
                 sprintf(buf[0],"blt(%d)",j+4*l);
                 sprintf(buf[1],"lwl(%d)=0b1 and mem(%d)=0b0",-i+1023,-i+j*1024+4096*l+1023);
                 sprintf(buf[2],"lwl(%d)=0b1 and mem(%d)=0b1",-i+1023,-i+j*1024+4096*l+1023);

                 beg_def_process (buf[0] ,buf[1],"0b1",0,BEG_REG);
                 beg_def_process (buf[0] ,buf[2],"0b0",0,BEG_REG | BEG_PRE);
               }
             beg_sort();
             beg_compact();
             fig[l]    = beg_get_befig (0);
             savebefig(fig[l],0);
           }

           beg_def_befig("testEatBeforeCompact");
           for (l = 0; l < size; l ++)
             beg_eat_figure(fig[size-1 - l]);

           befig = beg_get_befig (0);
           savebefig     (befig,0);
           befig->NAME = "testCompact";
           beg_sort();
           beg_compact();
           befig = beg_get_befig (0);
           savebefig     (befig,0);
         }
         //}}}
         break;
    case 12 :
         //{{{
         {
           int            i,j;
#ifdef size
#undef size
#endif
#define     size    6
           char           buf[3][1024];
           befig_list    *fig[size];


           beg_def_befig(namealloc("testcompactbiabl"));
           beg_def_por("testpor(0:11)",'I');

           for (j = 0; j < size; j ++)
           {
             sprintf(buf[2],"tmp_%d",j);
             beg_def_befig(buf[2]);
             for (i = 0; i < 128; i ++)
             {
               sprintf(buf[0],"dat(%d)=0b0",i);
               sprintf(buf[1],
                       "multi_spl9bs_saenb(%d)=0b1"
                       " and "
                       "multi_spl9bs_blfi(%d)=0b0"
                       ,i*2+j*256 ,i+j*128);

               beg_def_process (buf[0] ,buf[1],"0b0",0,BEG_REG);
             }
             beg_sort();
             beg_compact();
             fig[j]  = beg_get_befig(0);
           }

           beg_def_befig(namealloc("testcompactbiabl"));

           for (j = 0; j < size; j ++)
             beg_eat_figure(fig[/*size-1 - */j]);

           beg_sort();
           beg_compact();
           befig    = beg_get_befig (0);
           savebefig(befig,0);
         }
         //}}}
         break;
    case 13 :
         conflicttest();
         break;
    case 14 :
         beg_def_befig  ("testifendif");
         beg_def_process("z"   ,"not(a)"    ,"c(3)"     ,0,BEG_BUS);
         beg_def_process("z"   ,"a"         ,"c(4)"     ,0,BEG_BUS|BEG_PRE);
         befig      = beg_get_befig (0);
         savebefig(befig,0);
         break;
    case 15 :
         beg_def_befig  ("edge_pb");
         beg_def_process("z"   ,"a and a'event and i" ,"'1'",0 ,BEG_REG);
         beg_def_process("z"   ,"a and a'event and not i" ,"'0'",0,BEG_REG);
         befig      = beg_get_befig (0);
         savebefig(befig,0);
         break;
    case 16 :
         /*{{{                                                                       */
         list       = NULL;
         for (i = 0; i < 4; i ++)
         {
           sprintf(fig,"por_pb_col%d",i);
           beg_def_befig  (namealloc(fig));
           
           sprintf(sig,"lblt(%d)",i);
           beg_def_por    (sig,'B');
           sprintf(nsig,"lblf(%d)",i);
           beg_def_por    (nsig,'B');
           for (j = 0; j < 8; j++)
           {
             sprintf(cmd,"com(%d) and %s and not %s",j,sig,nsig);
             sprintf(mem,"mem(%d)",j);
             beg_def_process(mem,cmd,"'1'",0,BEG_REG);
             
             sprintf(cmd,"com(%d) and %s and not %s",j,nsig,sig);
             beg_def_process(mem,cmd,"'0'",0,BEG_REG|BEG_PRE);
             
             sprintf(cmd,"com(%d) and %s",j,mem);
             beg_def_process(sig,cmd,"'1'",0,BEG_REG);
             beg_def_process(nsig,cmd,"'0'",0,BEG_REG);
             
             sprintf(cmd,"com(%d) and not %s",j,mem);
             beg_def_process(sig,cmd,"'0'",0,BEG_REG|BEG_PRE);
             beg_def_process(nsig,cmd,"'1'",0,BEG_REG|BEG_PRE);
           }
           
           beg_sort();
           beg_compact();
           befig    = beg_get_befig (0);
           //savebefig(befig,0);
           list     = addchain(list,befig);
         }
         /* */
         beg_def_befig  ("por_pb_mux");
         beg_def_por    ("gblt",'B');
         beg_def_por    ("gblf",'B');
         for (i = 0; i < 4; i ++)
         {
    
           sprintf          (sig,"lblt(%d)",i);
           sprintf          (cmd,"sel(%d) and wclk",i);
           beg_def_process  (sig,"not prech","'1'",0,BEG_REG);
           beg_def_process  (sig,cmd,"gblt",0,BEG_REG);
           
           sprintf          (sig,"lblf(%d)",i);
           beg_def_process  (sig,"not prech","'1'",0,BEG_REG);
           beg_def_process  (sig,cmd,"gblf",0,BEG_REG);

           sprintf          (cmd,"sel(%d) and not wclk",i);
           sprintf          (dat,"lblt(%d)",i);
           beg_def_process  ("gblt",cmd,dat,0,BEG_REG);
           
           sprintf          (dat,"lblf(%d)",i);
           beg_def_process  ("gblf",cmd,dat,0,BEG_REG);

         }
         beg_sort();
         beg_compact();
         befig      = beg_get_befig (0);
         //savebefig(befig,0);
         
         beg_def_befig  ("por_pb_top");
         for (; list; list = delchain(list,list))
           beg_eat_figure(list->DATA);
         
         beg_sort();
         beg_compact();
         befig2     = beg_get_befig (0);
         //savebefig(befig2,0);
         
         beg_eat_figure (befig);
         befig      = beg_get_befig (0);
         savebefig(befig,0);

         beg_sort();
         beg_compact();
         befig      = beg_get_befig (0);
         savebefig(befig,0);
         /* */
         break;
         /*}}}************************************************************************/
    case 17 :
         beg_def_befig  ("testCompact");
         for (i = 0; i < 2; i ++)
           for (j = 0; j < 3; j ++)
           {
             sprintf(sig,"s(%d)",3*i+j);
             beg_def_process(sig ,"gnd" , "'1'", 0, BEG_REG);
             sprintf(cmd,"mblt(%d) or blf(%d)",i,j);
             sprintf(dat,"blf(%d)",i);
             beg_def_process(sig , cmd , dat, 0, BEG_REG);
           }
         beg_sort();
         beg_compact();
         beg_compact();
         befig      = beg_get_befig (0);
         savebefig(befig,0);
         break;
    case 18:
         /*{{{                                                                       */
         // bug concerning hasardous loop given by driver
         //
         /*-------------------------*/
         printf("Phase 1\n");
         printf("    Constructing the Main figure");
         fflush(stdout);

         mainfig    = namealloc("testloop");
         
         beg_def_befig  (mainfig);
         for (k = 0; k < 16; k ++)
           for (j = 0; j < 32; j ++)
             for (i = 0; i < 128; i ++)
             {
               sprintf(sig,"mblf(%d)",j+k*32);
               sprintf(cmd,"wl(%d)",i+k*128);
               beg_def_process(sig, cmd, "'0'", i+25 , BEG_REG);
               sprintf(cmd,"mem(%d)",i+j*128+k*4096);
               beg_def_process(sig, cmd, "'0'", i+25 , BEG_REG|BEG_PRE);
             }
         time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         
         printf("    Sort");
         beg_sort();
         time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         
         for (k = 0; k < 2; k ++)
         {
           printf("    Compact n°%d",k);
           beg_compact();
           time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         }

         befig      = beg_get_befig (0);
         
         /*-------------------------*/
         printf("Phase 2\n");

         beg_def_befig  (namealloc("int_fig"));
         
         for (k = 0; k < 16; k ++)
         {
           sprintf(fig,"int_fign%d",k);
           printf("    building %s\n",fig);
           
           beg_def_befig  (namealloc(fig));
           for (j = 0; j < 8; j ++)
             for (i = 0; i < 4; i ++)
             {
               sprintf(sig,"mblf(%d)",i+j*4+k*32);
               sprintf(cmd,"not b(%d)",k);
               beg_def_process(sig, cmd, "'0'", 0, BEG_REG);
               sprintf(cmd,"b(%d)",k);
               beg_def_process(sig, cmd, "'0'", 0, BEG_REG|BEG_PRE);
               sprintf(cmd,"(not mblt(%d) or mblf(%d) and (not mblf(%d) or mblt(%d))) and ((iBLF(%d) and not iBLT(%d)) or (iBLT(%d) and not iBLF(%d))) and LSEL(%d) ",
                       i+j*4+k*32,i+j*4+k*32,i+j*4+k*32,i+j*4+k*32,j,j,j,j,i+k*4);
               beg_def_process(sig, cmd, "'0'", 0, BEG_REG);
             }
           
           printf("        Sort");
           beg_sort();
           time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         
           for (j = 0; j < 3; j ++)
           {
             printf("        Compact %d-n°%d",k,j);
             beg_compact();
             time(&cur);ela=cur-bef;bef=cur;printhour(ela);
           }
           
           befig        = beg_get_befig(0);
           beg_def_befig  (namealloc("int_fig"));
           
           printf("        eat figure");
           beg_eat_figure (befig);
           time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         }
         
         /*-------------------------*/
         printf("Phase 3\n");
         printf("    Sort");
         beg_sort();
         time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         
         for (k = 0; k < 3; k ++)
         {
           printf("    Compact n°%d",k);
           beg_compact();
           time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         }
         
         befig      = beg_get_befig (0);
         
         /*-------------------------*/
         printf("Phase 4\n");
         
         printf("    eating figure");
         beg_def_befig (mainfig);
         befig      = beg_get_befig(0);

         beg_def_befig(namealloc("int_fig"));
         beg_eat_figure(befig);
         time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         
         printf("    Sort");
         beg_sort();
         time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         
         for (k = 0; k < 1; k ++)
         {
           printf("    Compact n°%d",k);
           beg_compact();
           time(&cur);ela=cur-bef;bef=cur;printhour(ela);
         }
         
         befig      = beg_get_befig (0);
         printf("    Dump");
         savebefig(befig,0);
         time(&cur);ela=cur-bef;bef=cur;printhour(ela);

         time(&cur);
         printf("total time:");
         printhour(cur-beg);
         printf("\n");
  }
  /*}}}************************************************************************/
  
  return EXIT_SUCCESS;
}
