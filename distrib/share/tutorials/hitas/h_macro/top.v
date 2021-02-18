// Verilog structural description generated from `top`
// Date : Tue Sep  7 17:59:10 2004


// Module Declaration.

module top (
a, b, c, d,
i, q0, q3, fonc,
test, scin, r0, r3,
ck, noe, vdd, vss,
cin, cout_cpu, cout_addit, sign,
np, ng, zero, ovr,
f0, f3, scout, decald,
decalg, decaldr, decalgr, s0,
s3, dout, oe, write,
en, adr);


// Declaration of external signals.

input [3:0] a;
input [3:0] b;
input [3:0] c;
input [3:0] d;
input [8:0] i;
input q0;
input q3;
input fonc;
input test;
input scin;
input r0;
input r3;
input ck;
input noe;
input vdd;
input vss;
input cin;
input cout_cpu;
input cout_addit;
input sign;
input np;
input ng;
input zero;
input ovr;
input f0;
input f3;
input scout;
input decald;
input decalg;
input decaldr;
input decalgr;
input s0;
input s3;
input [3:0] dout;
input oe;
input write;
input en;
input [6:0] adr;


// Declaration of internal signals.

wire [3:0] y;


// Declaration of instances.

cpu2901 cpu2901 (
  .a({a[3], a[2], a[1], a[0]}),
  .b({b[3], b[2], b[1], b[0]}),
  .d({d[3], d[2], d[1], d[0]}),
  .cin(cin),
  .cout(cout_cpu),
  .np(np),
  .ng(ng),
  .sign(sign),
  .zero(zero),
  .ovr(ovr),
  .i({i[8], i[7], i[6], i[5], i[4], i[3], i[2], i[1], i[0]}),
  .q0(q0),
  .q3(q3),
  .f0(f0),
  .f3(f3),
  .fonc(fonc),
  .test(test),
  .scin(scin),
  .scout(scout),
  .decald(decald),
  .decalg(decalg),
  .decaldr(decaldr),
  .decalgr(decalgr),
  .r0(r0),
  .r3(r3),
  .s0(s0),
  .s3(s3),
  .ck(ck),
  .y({y[3], y[2], y[1], y[0]}),
  .noe(noe),
  .oe(oe));
adder adder (
  .s_3(dout[3]),
  .s_2(dout[2]),
  .s_1(dout[1]),
  .s_0(dout[0]),
  .cout(cout_addit),
  .b_3(c[3]),
  .b_2(c[2]),
  .b_1(c[1]),
  .b_0(c[0]),
  .a_3(y[3]),
  .a_2(y[2]),
  .a_1(y[1]),
  .a_0(y[0]));
ram4x128 ram4x128 (
  .write(write),
  .en(en),
  .dout({dout[3], dout[2], dout[1], dout[0]}),
  .ck(ck),
  .adr({adr[6], adr[5], adr[4], adr[3], adr[2], adr[1], adr[0]}));
endmodule
