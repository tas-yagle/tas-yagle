entity cell_plus_sa is
  port(
    wl0, wl1 : in bit;
    wlen0, wlen1, prech, bl, blb, tozero, toone, eq0, eq1, blen: in bit;
    saen, saenb : in bit
    ); 
end;

architecture normal of cell_plus_sa is

component cells
  port(
    bl0_up, blb0_up, bl1_dn, blb1_dn, wl0, wl1 : in bit
    ); 
end component;

component gns_sensamp
  port(
    bl0, blb0, bl1, blb1, wlen0, wlen1, prech, bl, blb, tozero, toone, eq0, eq1, blen: in bit
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

signal bl0, blb0, bl1, blb1, vdd, vss : bit;

begin

-- pragma exclude sense trans_p trans_n
-- pragma exclude_at_end cells

  sense : gns_sensamp
    port map (
               bl0, blb0, bl1, blb1, wlen0, wlen1, prech, bl, blb, tozero, toone, eq0, eq1, blen
             );

  cells : cells
    port map (
--               bl0, blb0, bl1, blb1, wl0, wl1
               bl1, blb1, bl0, blb0, wl0, wl1
             );
  
  trans_p : tp
    port map (
               saenb, toone, vdd, vdd
             );

  trans_n : tn
    port map (
               saen, tozero, vss, vss
             );
end;

