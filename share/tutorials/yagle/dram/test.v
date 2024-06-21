`timescale 1 ps/1 ps

module test;

  reg ck ;
  wire dout;
  reg [1:0] a, din;
  reg sel = 1;
  reg write = 1;

  /* Make a reset that pulses once. */
  initial begin
  $dumpfile("test.vcd");
  $dumpvars(10, test);
  $dumpon;
      a = 0;
      din=0;
      #5000 ck = 0;
      #15000 ck = 1;
     # 2000000 $finish;
 end

  /* Make a regular pulsing clock. */

  //always #10000 ck = !ck;
  always @(ck) #10000 ck <= !ck;
  
  always #40000 din = din + 1;
  always #40000 a = a + 1;
  always #160000 write = write +1;
  always #320000 sel = sel +1;

  

  dram ram(a,din,dout,write,ck,sel);

  initial
     $monitor("At time %t, adr = %0d, din=%0d, dout = %0d, ck = %0d, write = %0d, sel = %0d",
              $time, a, din, dout, ck, write, sel);
endmodule // test
