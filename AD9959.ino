//******** Da abbinare ad Arduino Due che lavora a 3.3V come l'AD9959 **********
#include <Arduino.h>
#include <AD9959.h>
#include <LiquidCrystal.h>

//#include <Key.h>  // attenzione ai pin della tastiera...

LiquidCrystal lcd(8,9,10,11,12,13);   // 

void setup() 
{ 
  lcd.begin(8,2);
  lcd.clear();
  lcd.print("AD9959");
  delay(1500); 
  AD9959.begin();
}

void loop() 
{
  uint32_t Fa, Fb; uint16_t Ampiezza, Fase;
  
  Fa=100000000;
  Ampiezza=1023;
  Fase=0;
  AD9959.set_grand_A(Fa, Ampiezza, Fase); 
    
  Fb=0;
  AD9959.set_grand_B(Fb, Ampiezza, Fase);
    
  while(true){};
}


//-------------------------- Funzioni di Test ----------------------------------
//------------------------------------------------------------------------------
  
//------------------------- Imposta i canali -----------------------------------
/*

  AD9959.sel_canale(A); 
  f=10000000; 
  AD9959.imposta_grandezze(512, 10000000, 0); // ampiezza, freq., fase
  AD9959.update();
  f=10000000;
  lcd.print("f="+String(f)); 
  //
  //AD9959.sel_canale(B);  
  //AD9959.imposta_grandezze(512, 10000000, 0);
  //AD9959.update();



// ----------------Treno di impulsi con la freq. del PWM -----------------------
  AD9959.sel_canale(A);
  AD9959.genera_treno_impulsi(true,512);
  delay(10000);
  AD9959.genera_treno_impulsi(false,512);
  AD9959.update();
  
// -------------------------- Dente di sega ------------------------------------
  AD9959.sel_canale(A);
  AD9959.set_valori_iniz_finali(t_ampiezza, 200, 800);
  AD9959.genera_dentedisega(true, t_ampiezza, 10, 10);  //tipo_grandezza: 00 nessuna, 01 Ampiezza , 10 Frequenza , 11 Fase
  AD9959.update();
  delay(10000);
  AD9959.genera_dentedisega(false, t_ampiezza, 10, 10);
  AD9959.update();

//------------------------- Modulazione d'ampiezza -----------------------------

  // Modulazione d'ampiezza (e' e rimane in modalita' manuale) -----------------
  // setta la f_portante a 10MHz, f_modulante=3KHz, tasso di modulazione=50%
  //if ...  
    {
    AD9959.start_mod_amp=true; 
    AD9959.sel_canale(A);
    AD9959.set_frequenza(10000000); // imposta la freq. della portante a 10MHz
    AD9959.update();
    while (AD9959.start_mod_amp==true)
      { 
      //Tasti.leggi();
      // Visualizza();                       
      AD9959.genera_modulazione_ampiezza(1000, 3000, 50);  // Ricorsiva. (amp, f_modulante, tasso_modulazione in %)
      //if ... 
        {
        AD9959.start_mod_amp=false;
        AD9959.set_ampiezza(512);
        } 
      AD9959.update();
      }
    }
//------------------------------------------------------------------------------ 
*/