#include <AD9959.h>

//#define PDC           //  (PDC)                        LOW  (via hard.)
#define reset   32      //  (RESET)                                          Nero
#define up_date 34      //  (IO_Update)                                      Bianco
#define cs_     36      //  (/CS: chipset-negato)                            Grigio
#define clk     38      //  (SCLK)                                           Viola
#define dati    40      //  (SDIO_0)                                         Azzurro
//#define IO_3          //  (SDIO_3)                     LOW   (via hard.)

#define interrupt_pin_2 2    // pin di ingresso per l'interrupt N.0 da collegare col pin 3
#define pwm_pin 3

//----------- Definizione dei registri----------------
byte CSR[1];
byte FR1[3];
byte FR2[2];
byte CFR[3];
byte CFTW0[4]; 
byte CPOW0[2];
byte ACR[3];
byte LSRR[2];
byte RDW[4];
byte FDW[4];
byte CFTW1[4];

bool quadra;
unsigned int ampiezza=1023;


/*********************** Scrittura registri AD 9959 ***************************/
/******************************************************************************/

void AD9959_::begin(void)
{ 
//
pinMode(reset,OUTPUT);
digitalWrite(reset,LOW);    //   Resetta il Chip
delay(5);
digitalWrite(reset,HIGH); 
delay(5);
digitalWrite(reset,LOW); 
//
//pinMode(PDC,OUTPUT);        //  PDC  Power_Down_Control=0 
//digitalWrite(PDC, LOW); 
pinMode(up_date,OUTPUT);     // update   
digitalWrite(up_date,LOW);
pinMode(dati,OUTPUT);       //  Dati SDIO_0   
digitalWrite(dati, LOW); 
pinMode(clk,OUTPUT);        //  Clk     
digitalWrite(clk, LOW); 
//pinMode(P0,OUTPUT);        //   P0     
//digitalWrite(P0, LOW); 
//pinMode(IO_3,OUTPUT);        //   SDIO_3     
//digitalWrite(IO_3, LOW);
Fa_old=0;
Fb_old=0; 
// Imposta fr1 con i dati per il PLL interno a 500MHz (quarzo a 25MHz*20)
pinMode(cs_,OUTPUT);      
digitalWrite(cs_,LOW);
sel_canale(A); 
set_fr1(); 
sel_canale(B); 
set_fr1(); 
update();     
delay(1);
digitalWrite(cs_,HIGH);
}

//---------------------- Imposta la frequenza A --------------------------------
String AD9959_::set_dds_A(void)
  {
  double mV, a; int d; String st;
  if (dbmA>-10)       // dBm=[-70..-8]
    dbmA=-10; 
  if (dbmA<-70)
    dbmA=-70; 
  dbmA=dbmA;       // Correzione dalla misura reale 
  mV=1000*dbm_volt(dbmA);      // Ampiezza in mV
  d=0;
  if (mV<1.0) d=1;
  st=String(mV,d); 
  a=(mV-0.1)*1023/100;      // dBm=[-10dB..-70] -> mV=[0,1..100] si rapporta a [0..1023] dell'AD
  AmpiezzaA=(uint32_t)a;
  set_grand_A(Fa, AmpiezzaA, FaseA); 
  return st;  // Ritorna i mV
  } 

void AD9959_::set_grand_A(uint32_t Fa, uint16_t Ampiezza, uint16_t Fase)
{
pinMode(cs_,OUTPUT);      
digitalWrite(cs_,LOW);  //   Cipset negato 
//
sel_canale(A);
//set_fr1();    // Gia settato in .begin
//set_fr2();    // Righe piu' avanti
//set_cfr();
if (Fa!=Fa_old)
  {
  set_frequenza(Fa);
  Fa_old=Fa;
  }
set_ampiezza(Ampiezza); 
set_fase(Fase);      
update();     
//
delay(1);
digitalWrite(cs_,HIGH);  
}
//------------------------ Imposta la frequenza B ------------------------------
String AD9959_::set_dds_B(void)
   {
   double mV, a; int d; String st;
   if (AD9959.dbmB>-8) 
      AD9959.dbmB=-8; 
   if (AD9959.dbmB<-70)
      AD9959.dbmB=-70; 
   mV=1000*dbm_volt(AD9959.dbmB);
   d=0;
   if (mV<1.0)  d=1;
   st=String(mV,d);  
   a=(mV-0.1)*1023/126;    // Ampiezza Max= -8dB -> Amp*1000=126
   AD9959.AmpiezzaB=(uint32_t)a; 
   AD9959.set_grand_B(AD9959.Fb, AD9959.AmpiezzaB, AD9959.FaseB);
   return st; // Ritorna i mV
   } 

void AD9959_::set_grand_B(uint32_t Fb, uint16_t Ampiezza, uint16_t Fase)
{
pinMode(cs_,OUTPUT);      
digitalWrite(cs_,LOW);  //   Cipset negato 
//
sel_canale(B);
//set_fr1();    // Gia settato in .begin
//set_fr2(); 
//set_cfr();
if (Fb!=Fb_old)
  { 
  set_frequenza(Fb);
  Fb_old=Fb;
  } 
set_ampiezza(Ampiezza);  // 50% 
set_fase(0);
update();  
//
delay(1);
digitalWrite(cs_,HIGH); 
}
//------------------------------------------------------------------------------

void AD9959_::invia(int n_byte)
{
byte bout; int n;
digitalWrite(dati,LOW);
for (n=n_byte-1;n>=0;n--) // Invia n_byte di reg dal piu' significativo 
   {
   digitalWrite(clk, LOW);
   bout=reg.reg_byte[n];
   shiftOut(dati, clk, MSBFIRST, bout);
   //shiftOutS(dati, clk, MSBFIRST, bout); 
   digitalWrite(clk, LOW);
   }
digitalWrite(dati,LOW);
}

void AD9959_::update(void)
{
// Update alto per un po' conferma i dati appena inviati con invia()
digitalWrite(up_date,LOW);  
digitalWrite(up_date,HIGH);
delay(1);
digitalWrite(up_date,LOW);   
}

void AD9959_::sel_canale(byte can)  // can= A oppure  B
{
registro_addr(CSR_A);
switch (can) 
  { 
  case A: CSR[0]=B00010000; break; // Canale A
  case B: CSR[0]=B00100000; break; // Canale B
  case D: CSR[0]=0; break;         // Disabilita tutti
  }
reg.reg_byte[0]=CSR[0];
invia(1);
}

void AD9959_::registro_addr(byte addr) // INSTRUCTION BYTE
{
reg.reg_byte[0]=addr;
invia(1);
}

void AD9959_::set_fr1(void)  // Statico
{
byte n;
registro_addr(FR1_A);
reg.reg_byte[2]=B11010000;  // 500MHz (25MHz*20) ; Pll=20 dal 18 al 22
reg.reg_byte[1]=B00000000;
reg.reg_byte[0]=B00000000;
invia(3);
for (n=0;n<=2;n++) FR1[n]=reg.reg_byte[n];
}

void AD9959_::set_fr2(void)  // Statico
{
byte n;
registro_addr(FR2_A);
reg.reg_byte[1]=B00000000;
reg.reg_byte[0]=B00000000;
invia(2); 
for (n=0;n<=1;n++) FR2[n]=reg.reg_byte[n];
}

void AD9959_::set_cfr(void)
{
byte n;
registro_addr(CFR_A); 
reg.reg_byte[2]=B00000000;
reg.reg_byte[1]=B00000011;   // sweep inattivo (14=0)
reg.reg_byte[0]=B00000000;
invia(3); 
for (n=0;n<=2;n++) CFR[n]=reg.reg_byte[n]; 
}

// Imposta la frequenza del canale selezionato
void AD9959_::set_frequenza(uint64_t freq)    // Imposta la frequenza al canale selezionato
{
uint64_t P,Q,N; byte n;
registro_addr(CFTW0_A); 
P=16777216ULL;
Q=1953125ULL;
N=freq*P/Q;
reg.reg_long=(unsigned long)N;
invia(4);
for (n=0;n<=3;n++) CFTW0[n]=reg.reg_byte[n];
}

// Imposta la fase al canale selezionato
void AD9959_::set_fase(unsigned long fase_gradi)  // registro per fase a 14 bit: da 0 a 16383 (0-360°)
{
unsigned long P,Q,fase_; uint64_t N; byte n;
registro_addr(CPOW0_A);

fase_=16383*fase_gradi/360;
P=2048ULL;
Q=45ULL;
N=(uint64_t)(fase_gradi*P/Q);
reg.reg_long=(unsigned int)N;  
invia(2);
for (n=0;n<=1;n++) CPOW0[n]=reg.reg_byte[n];
}

// Imposta l'ampiezza regolabile manualmente (del canale selezionato)
void AD9959_::set_ampiezza(unsigned int amp) // amp: da 0 a 1023
{
byte b,n;
registro_addr(ACR_A);

reg.reg_byte[2]=B00000000; // I parametri per la rampa sono tutti disabilitati
b=lowByte(amp >> 8);  
reg.reg_byte[1]=b;         // Abilita la modulazione in modo manuale
b=B00010000;    // B00010000;
reg.reg_byte[1]=reg.reg_byte[1] | b; // Attiva il fattore di moltiplicazione amp in modo manuale
reg.reg_byte[0]=lowByte(amp);  
invia(3);
for (n=0;n<=2;n++) ACR[n]=reg.reg_byte[n];
}

// Tempi di dsalita e discesa in sweep (modulazione)
void AD9959_::set_sweep_rate(unsigned long fsrr, unsigned long rsrr) // tempi relativi delta (salita e discesa)
{
byte n;
registro_addr(LSRR_A);
reg.reg_byte[1]=fsrr;
reg.reg_byte[0]=rsrr;
invia(2);
for (n=0;n<=1;n++) LSRR[n]=reg.reg_byte[n];
}

// delta positivo in sweep
void AD9959_::set_raising_delta(unsigned long r_delta) 
{
byte n;
registro_addr(RDW_A);
reg.reg_long=r_delta;
invia(4);
for (n=0;n<=3;n++) RDW[n]=reg.reg_byte[n];
}

// delta negativo in sweep
void AD9959_::set_falling_delta(unsigned long f_delta)  
{
byte n;
registro_addr(FDW_A);
reg.reg_long=f_delta;
invia(4);
for (n=0;n<=3;n++) FDW[n]=reg.reg_byte[n];
}

//*************************** Funzioni evolute *********************************
//******************************************************************************

//-------- Genera un treno d'impulsi sincronizzati col PWM di Arduino ----------
void inverti_ampiezza(void)
{
  quadra=!quadra;
  if (quadra==true)
    AD9959.set_ampiezza(ampiezza);
  else
    AD9959.set_ampiezza(0);
  AD9959.update();
}

void AD9959_::genera_treno_impulsi(bool abilita,unsigned long amp)  
{
/* Serve una uscita Arduino in PWM (pin 3) che collegato 
   al pin 2 genera l'interrups N.0 sul fronte di salita */
if (abilita==true)
  { 
  quadra=false; 
  ampiezza=amp;
  analogWrite(pwm_pin, 50);
  attachInterrupt(interrupt_pin_2, inverti_ampiezza, RISING);
  }         
else
  {
  detachInterrupt(digitalPinToInterrupt(interrupt_pin_2));
  digitalWrite(pwm_pin, LOW);
  set_ampiezza(amp);
  }
}   

//--------------------- Genera  dei denti di sega ------------------------------

// Inserisce i valori iniziali e finali della rampa (in sweep)
void AD9959_::set_valori_iniz_finali(byte grandezza, unsigned long val_ini, unsigned long val_fin)
{
byte n;
// Imposta i valori iniziali della grandezza coinvolta
switch (grandezza)   // grandezza coinvolta: 00 nessuna, 01 Ampiezza , 10 Frequenza , 11 Fase
  {
  case t_ampiezza: set_ampiezza(val_ini);
        break;
  case t_frequenza: set_frequenza(val_ini); 
        break;
  case t_fase:  set_fase(val_ini);   
        break;
  default:       
        break;
  }
// Imposta i valori finali della grandezza coinvolta. Vanno tutti nel registro CFTW1 da 4 byte
registro_addr(CFTW1_A);
reg.reg_long=val_fin;   //!!! i valori finali vanno sempre in CFTW1 (0A) MSB allineati???
invia(4);
for (n=0;n<=3;n++) CFTW1[n]=reg.reg_byte[n];
}

// Abilita lo sweep a rampa (denti di sega)
void AD9959_::genera_dentedisega(bool abilita, byte grandezza, unsigned int rate, unsigned long delta)
{
/* Range grandezze:
Delta temporale:  1-255 corrispondenti al range: 8,9ns fino a 2,27us
Delta incremento grandezza: 0-2^31
-------------------------
Un impulso al P1 ed altri successivi fanno partire la rampa
Usare P2 per il canale B
*/
byte n; 
// Disabilita la funzione
if (abilita==false)
  {
  registro_addr(CFR_A); 
  reg.reg_byte[1]=B10000011;   // sweep inattivo (14=0)
  invia(3); 
  for (n=0;n<=2;n++) CFR[n]=reg.reg_byte[n]; 
  exit;
  }

set_sweep_rate(0, rate);  // delta temporale in raising (8 bit)
set_raising_delta(delta);     // delta d'incremento della grandezza (32 bit)

// Indirizzo del registro della grandezza coinvolta
registro_addr(CFR_A); 
switch (grandezza)        // grandezza coinvolta: 00 nessuna, 01 Ampiezza , 10 Frequenza , 11 Fase
  {
  case t_ampiezza:  reg.reg_byte[2]=B01000000;
        break;
  case t_frequenza: reg.reg_byte[2]=B10000000;
        break;
  case t_fase:      reg.reg_byte[2]=B11000000;
        break;
  default:          reg.reg_byte[2]=B00000000;
        break; 
  }
reg.reg_byte[1]=B11000011;  // abilita lo sweep a dente di sega, usa l'intera scala del DAC
reg.reg_byte[0]=B00000001;  // funzione sin
invia(3);
for (n=0;n<=2;n++) CFR[n]=reg.reg_byte[n];
}

//-------- Genera una portante modulata in ampiezza da una sinusoide ----------- 
//-------------per attivarla start_mod=true, false per disattivarla
void AD9959_::genera_modulazione_ampiezza(unsigned int ampiezza, unsigned int f_modulante, byte tasso_modulazione)
{
double amp_mod; unsigned int amp_int;
amp_mod=sin((float)(6.28*f_modulante*millis()))+1.0;
amp_int=(unsigned int)(ampiezza*tasso_modulazione/200);
amp_int=(unsigned int)(amp_int*amp_mod); // da 0 a 1023
}

double AD9959_::dbm_volt(double dbm) // da dBm a mV
{
   return 0.316228*pow(10,dbm/20);
}

/*
void AD9959_::stabilizza_ampiezza()
{
}
*/

AD9959_ AD9959;