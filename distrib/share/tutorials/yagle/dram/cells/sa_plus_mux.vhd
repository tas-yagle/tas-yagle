entity sa_plus_mux is
  port(
    bl0, blb0, bl1, blb1, wlen0, wlen1, prech, bl, blb, tozero, toone, eq0, eq1, blen : in bit;
    sel0, sel1, sel0b, sel1b, in0, in0b, in1, in1b: in bit
    ); 
end;

architecture normal of sa_plus_mux is

component gns_sensamp
  port(
    bl0, blb0, bl1, blb1, wlen0, wlen1, prech, bl, blb, tozero, toone, eq0, eq1, blen : in bit
    ); 
end component;

component tn
   port    ( gate          : in    bit;
             source, drain : inout bit;
             bulk          : in    bit);
end component;

component tp
   port    ( gate          : in    bit;
             source, drain : inout bit;
             bulk          : in    bit);
end component;

signal vdd, vss : bit;

begin

-- pragma exclude sense

  sense : gns_sensamp
    port map (
               bl0, blb0, bl1, blb1, wlen0, wlen1, prech, bl, blb, tozero, toone, eq0, eq1, blen
             );

  trans_p0 : tp port map ( sel0b, in0, bl, vdd );
  trans_n0 : tn port map ( sel0, in0, bl, vss );

  trans_p0b : tp port map ( sel0b, in0b, blb, vdd );
  trans_n0b : tn port map ( sel0, in0b, blb, vss );

  trans_p1 : tp port map ( sel1b, in1, bl, vdd );
  trans_n1 : tn port map ( sel1, in1, bl, vss );

  trans_p1b : tp port map ( sel1b, in1b, blb, vdd );
  trans_n1b : tn port map ( sel1, in1b, blb, vss );
end;

