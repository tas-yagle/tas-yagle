#include TBG_H

#define DESIGN      "ram4x128"
#define DATA_SIZE    4
#define ADR_SIZE     7
#define UNIT         300

int    TCL = 120, TCH = 120, TCC = 240;
  
long puiss2(long a)
{
  if (a)
    return puiss2(a-1)*2;
  else
    return 1;
}

long invert(long a, long nb_bit)
{
  long    i, r=0, m=puiss2(nb_bit-1);

  for (i = 0; i < nb_bit; i ++)
  {
    r  += (a%2)*m;
    m  /= 2;
    a  /= 2;
  }

  return r;
}

void cycle(int nbcycle)
{
  int    i;

  for (i = 0; i < nbcycle; i ++)
    ExecuteCycle();
}

int time(int i, int a)
{
  return i*TCC + TCL + a;
}


int timer(int base, int a)
{
  static int old, obase = 0;
  int        res;
  
  if (base != obase)
  {
    obase   = base;
    res     = base + a;
    old     = a;
  }
  else
  {
    res     = a - old;
    old     = a;
  }
  
  return res;
}

void ckUP()
{
  Assign ("ck"      , 1);
}

void ckDW()
{
  Assign ("ck"      , 0);
}

void init()
{
  Assign ("dout"    , FORCE_UNK);
  Assign ("adr"     , FORCE_UNK);
  Assign ("write"   , FORCE_UNK);
  Assign ("ck"      , FORCE_UNK);
  Assign ("en"      , 1);
  Assign ("vdd"     , 1);
  Assign ("vss"     , 0);
}

void dead_cycle()
{
  timer(0,0);
  
  // __/ии 
  ckUP();
  cycle(timer(TCH,0));
  // ии\__ 
  ckDW();
  cycle(timer(TCH,TCL));
}

void write_mem(long nextadress, long data)
{
  data      = invert(data,4);
  timer(0,0);
  // __/ии 
  ckUP ();
  cycle  (timer(TCH,-40));
  Assign ("write"    , 1);
  Assign ("dout"     , data);
  cycle(timer(TCH,0));
  // ии\__ 
  ckDW();
  cycle  (timer(TCH,80));
  Assign ("adr"      , nextadress);
  cycle  (timer(TCH,100));
  Assign ("dout"     , WEAK_UNK);
  cycle  (timer(TCH,TCL));
  ckUP();
}

void read_mem(long adress)
{
  timer(0,0);
  // ии\__ 
  ckDW();
  cycle  (timer(TCH,-40));
  Assign ("adr"      , adress);
  Assign ("write"    , 0);
  cycle(timer(TCH,0));
  // __/ии 
  ckUP ();
  cycle  (timer(TCH,100));
  //Assign ("dout"     , WEAK_UNK);
  cycle  (timer(TCH,TCL));
  // ии\__ 
  ckDW();
}

void half_cycle()
{
  timer(0,0);
  cycle(timer(TCH,0));
  ckDW ();
}

int main (void)
{
  int        i;
  int        test;
  
  Period (UNIT, "ps");
  Design (DESIGN".vhd", DESIGN);

  init();

//  dead_cycle();
//  read_write_mem(long adress, long data, int read, int write)
//  for (i = 0; i < ValMax("adr"); i ++)
  for (i = 0; i < 5; i ++)
    write_mem(i,i%ValMax("dout"));
  half_cycle();
//  for (i = 0; i < ValMax("adr"); i ++)
  for (i = 0; i < 5; i ++)
    read_mem(i);
  write_mem(3,8);
  half_cycle();
  read_mem(3);
  
  GenerateTestbench ("TB_"DESIGN);
  
  return 0;
}
