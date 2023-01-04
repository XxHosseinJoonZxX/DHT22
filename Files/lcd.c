#include <mega8.h>
#include <delay.h>
#include <stdio.h>
#include <alcd.h>
#include "DHT22.h"

void main(void)
{
float temperature,humidity;
char str[10],temp[10],hum[10];
UCSRA=0x00;
UCSRB=0x08;
UCSRC=0x06;
UBRRH=0x00;
UBRRL=0x33;
ACSR=0x80;
SFIOR=0x00;
lcd_init(16);
while (1)
      { 
      if(dht22_read(&temperature,&humidity) == -40) 
        {
        lcd_putsf("DHT22  ERROR");
        }
      else       
        {
        ftoa(temperature,1,temp);                 
        ftoa(humidity,1,hum); 
        lcd_clear();  
        sprintf(str,"Temp = %s C",temp);  
        lcd_puts(str);                                                        
        lcd_gotoxy(2,0); 
        sprintf(str,"Hum = %s % RH",hum);                                                           
        lcd_gotoxy(2,1);   
        lcd_puts(str);  
          
        }
             
      delay_ms(2000); 
      }        
}

