#ifndef AD9959_h
#define AD9959_h 
// library interface description
#include <Arduino.h>

/*********************** Scrittura registri **********************/
//------------- Indirizzi dei registri ---------------
#define CSR_A   0x00    //  8 - [4,7] (il canale selezionato da settare; seguito da altri registri)
#define FR1_A   0x01    // 24
#define FR2_A   0x02    // 16
#define CFR_A   0x03    // 24 - (Grandezza da modulare e varie sul Sweep)

#define CFTW0_A 0x04    // 32    (Set Frequenza)
#define CPOW0_A 0x05    // 16    (Set Fase)
#define ACR_A   0x06    // 24    (Set Ampiezza e parametri su ampiezza ramp)

#define LSRR_A  0x07    // 16    (Inizio e fine rampa)
#define RDW_A   0x08    // 32    (Delta Rising)
#define FDW_A   0x09    // 32    (Delta Falling)

#define CFTW1_A 0x0A    // 32    (Set valori finali nello sweep)

#define t_ampiezza  1
#define t_frequenza 2
#define t_fase      3

typedef union reg_ {byte reg_byte[4]; unsigned long reg_long;};

double dbm_volt(double dbm);

class AD9959_
{
public: 
     reg_ reg;
     uint32_t Fa, Fb, Fa_old, Fb_old;
     long int off_set_A;
     uint16_t AmpiezzaA, AmpiezzaB; 
     float dbmA, dbmB;
     uint16_t FaseA, FaseB;
     bool start_mod_amp=false;
     bool start_treno=false;
     void begin(void);
     double dbm_volt(double dbm);
     String set_dds_A(void);
     String set_dds_B(void); 
     void set_grand_A(uint32_t Fa, uint16_t AmpiezzaA, uint16_t FaseA); 
     void set_grand_B(uint32_t Fb, uint16_t AmpiezzaB, uint16_t FaseB);
     void set_freq_B(uint32_t Fb);
     void registro_addr(byte addr);
     void invia(int n_byte); 
     void update(void);
     void sel_canale(byte can);
     void avvia_canale(void);
     void set_fr1(void);
     void set_fr2(void);
     void set_cfr(void);
     void set_frequenza(uint64_t freq); 
     void set_ampiezza(unsigned int amp); 
     void set_fase(unsigned long fase_gradi);
     void set_raising_delta(unsigned long r_delta);
     void set_falling_delta(unsigned long f_delta);  
     void set_sweep_rate(unsigned long fsrr, unsigned long rsrr);
     // Funzioni evolute
     void genera_treno_impulsi(bool abilita, unsigned long amp); 
     void set_valori_iniz_finali(byte grandezza, unsigned long val_ini, unsigned long val_fin);
     void genera_dentedisega(bool abilita, byte grandezza, unsigned int rate, unsigned long delta);
     void set_modulazione_ampiezza(unsigned long f_portante);
     void genera_modulazione_ampiezza(unsigned int ampiezza, unsigned int f_modulante, byte tasso_modulazione);
     //void stabilizza_ampiezza();
          
private: 
};

extern 
  AD9959_ AD9959;
  #define A 0
  #define B 1
  #define D 2

#endif