/*
 * PID - Instrumentation
 *
 * Author:ArshiaMadadi
 */
//LIB
#include <io.h>
#include <mega32.h>
#include <delay.h>
#include <lcd.h>
#include <stdio.h> //scaf,printf

#asm
   .equ __lcd_port=0x15 ;PORTC
#endasm
 
//Variable definition
    char buffer[64];
    float SP=100,temp,error,last_error,last_iterm,kp=.025,ki=.0000075,kd=1;//sp(0,100)
    float pterm,iterm,dterm,D;
    unsigned int  D2,LD,HD;
    
//ADC interrupt    
    interrupt [ADC_INT] void adc_isr(void){
        unsigned int adc_data;
        adc_data=ADCW;
        temp=(adc_data*2.56)/1023;
        temp=temp*100;
    }
    


 
 void main(void){ 
    lcd_init(16);
              
     
 //adc       
    GICR|=0x40;
    MCUCR=0x02;
    MCUCSR=0x00;
    GIFR=0x40;
    ADMUX=0xC0;  //ch=0
    ADCSRA=0x8D;
    #asm("sei")
    ADCSRA=0xCD;
      
   while(1){
            error=SP-temp; 
            pterm=kp*error; 
            iterm=(ki*error)+last_iterm; 
            dterm=(error-last_error)*kd; 
            D=(pterm+iterm+dterm)*100; 
            if(D<1) D=1; 
            if(D>1020) D=1020; //D=1020 MAX <0OCR Timer>
            last_iterm=iterm; 
            last_error=error; 
            D2=D;  
            HD=D2/256;  //change to 8 Bit
            LD=D2%256; 
            
    DDRD.4=1;
    DDRD.5=1;
    TCCR1A=0xE3;
    TCCR1B=0x0C;
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=HD;
    OCR1BL=LD;
    
    sprintf(buffer,"T=%3.1f SP=%2.1f \nKp=%f \nKi=%f \nKd=%f",temp,SP,pterm,iterm,dterm);
    lcd_clear(); 
    lcd_puts(buffer);
    delay_ms(200);
    ADCSRA=0xCD;
   }
 }
