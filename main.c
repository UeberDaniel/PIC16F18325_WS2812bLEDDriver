/*
 * File:   main.c
 * Author: Daniel
 *
 * Created on 11. Mai 2023, 19:30
 */
// PIC16F18325 Configuration Bit Settings

// 'C' source line config statements


//Uncomment the next line to disable Watchdog timer
#define AUTO_RESET 


// CONFIG1
#pragma config FEXTOSC = OFF    // FEXTOSC External Oscillator mode Selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC with 2x PLL (32MHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; I/O or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR/VPP pin function is MCLR; Weak pull-up enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)

#ifndef AUTO_RESET
#pragma config WDTE = OFF       // Watchdog Timer Enable bits (WDT disabled; SWDTEN is ignored)
#else
#pragma config WDTE = ON       // Watchdog Timer Enable bits (WDT disabled; SWDTEN is ignored)
#endif

#pragma config LPBOREN = OFF    // Low-power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bits (Brown-out Reset enabled, SBOREN bit ignored)
#pragma config BORV = LOW       // Brown-out Reset Voltage selection bit (Brown-out voltage (Vbor) set to 2.45V)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (The PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a Reset)
#pragma config DEBUG = OFF      // Debugger enable bit (Background debugger disabled)

// CONFIG3
#pragma config WRT = OFF        // User NVM self-write protection bits (Write protection off)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored.)

// CONFIG4
#pragma config CP = OFF         // User NVM Program Memory Code Protection bit (User NVM code protection disabled)
#pragma config CPD = OFF        // Data NVM Memory Code Protection bit (Data NVM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <pic16f18325.h>
#define _XTAL_FREQ  32000000

//OC Mode 750khz -> max Time = 1,3333 µs per bit
#define D0H()  asm("nop");asm("nop");                                                                   //220 ns - 380 ns       --> 275ns
#define D0L()  asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");                                  //580 ns - 1 µs         --> 720ns
#define D1H()  asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop"); //580 ns - 1 µs         --> 650ns
#define D1L()  asm("nop");                                                                              //220 ns - 420 ns       --> 350ns

#define LEDstream RC2

void initSPI(){
    
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
    ANSELCbits.ANSC3 = 0;
    ANSELCbits.ANSC4 = 0;

    ANSC1 = 0;
    TRISC1 = 0;
    
    SSP1SSPPS = RC1;
    SSP1CLKPPS = 0b10100;
    SSP1DATPPS = 0b10011;
    RC0PPS      = 0;
    
    SSP1STATbits.SMP = 0;           //SMP must be cleared when SPI is used in Slave mode
    SSP1STATbits.CKE = 1;           //0 = Transmit occurs on transition from Idle to active clock state
    SSP1CON1bits.WCOL = 0;          //  (TX mode only) 
    SSP1CON1bits.SSPOV = 0;         // 1 = overflow in SSPBUF(must be cleared in software); 0 = No overflow
    SSP1CON1bits.SSPEN = 0;         // Synchronous Serial Port Enable bit
    SSP1CON1bits.CKP = 0;           // 0 = Idle state for clock is a low level
    SSP1CON1bits.SSPM = 5;          //= SPI Slave mode, clock = SCK pin, nSS pin control disabled, nSS can be used as I/O pin  
    SSP1CON3bits.BOEN = 0;          // 1 = SSPBUF updates every time that a new data byte is shifted in ignoring the BF bit; 0 = If new byte is received with BF bit of the SSPSTAT register already set, SSPOV bit of the SSPCON1 register is set, and the buffer is not updated
    SSP1CON3bits.BOEN = 1;
    SSP1CON1bits.SSPEN = 1;

}
//MISO -> ?  
//SS -> ?  
//MOSI -> RC3
//SCK -> RC4
//LED OUT -> RC2



int main(void)
{
    OSCFRQbits.HFFRQ = 0b1111;          //64 MHz Tune  
    RC2PPS = LATC2;
    TRISCbits.TRISC2 = 0;
    initSPI();
    
    #ifdef AUTO_RESET
    WDTCON = 0b00011101;                //Watchdog reset every 16 sec.
    #endif

    while(1)
    {

        if(SSP1STATbits.BF){
        #ifdef AUTO_RESET
        CLRWDT();
        #endif
           
        if(!(SSP1BUF & 0x80)){LEDstream = 1; D0H();LEDstream = 0; D0L();} 
                  else { LEDstream = 1; D1H(); LEDstream = 0; D1L();}
        if(!(SSP1BUF & 0x40)){ LEDstream = 1; D0H(); LEDstream =  0; D0L(); }
                  else { LEDstream = 1; D1H(); LEDstream = 0; D1L(); }
        if(!(SSP1BUF & 0x20)){ LEDstream = 1; D0H(); LEDstream =  0; D0L(); }
                  else { LEDstream = 1; D1H(); LEDstream = 0; D1L(); }
        if(!(SSP1BUF & 0x10)){ LEDstream = 1; D0H(); LEDstream =  0; D0L(); }
                  else { LEDstream = 1; D1H(); LEDstream = 0; D1L(); }
        if(!(SSP1BUF & 0x08)){ LEDstream = 1; D0H(); LEDstream =  0; D0L(); }
                  else { LEDstream = 1; D1H(); LEDstream = 0; D1L(); }
        if(!(SSP1BUF & 0x04)){ LEDstream = 1; D0H(); LEDstream =  0; D0L(); }
                  else { LEDstream = 1; D1H(); LEDstream = 0; D1L(); }
        if(!(SSP1BUF & 0x02)){ LEDstream = 1; D0H(); LEDstream =  0; D0L(); }
                  else { LEDstream = 1; D1H(); LEDstream = 0; D1L(); }
        if(!(SSP1BUF & 0x01)){ LEDstream = 1; D0H(); LEDstream =  0; D0L(); }
                  else { LEDstream = 1; D1H(); LEDstream = 0; D1L(); }
       }        
    }
}







