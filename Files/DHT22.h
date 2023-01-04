#define  xtal 8000000
#include <delay.h>
#include <stdlib.h>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define DHT22_PIN          PIND.4       //œ— «Ì‰Ã« Å«ÌÂ òÂ œ„«”‰Ã »Â «‰ Ê’· ‘œÂ —« „‘Œ’ ò‰Ìœ
#define DHT22_DDR          DDRD.4
#define DHT22_PORT         PORTD.4

#define DHT22_INPUT_MODE   DHT22_DDR=0;
#define DHT22_OUTPUT_MODE  DHT22_DDR=1;
#define DHT22_LOW          DHT22_PORT=0;
#define DHT22_HIGH         DHT22_PORT=1;
#define DHT22_READ         DHT22_PIN
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define  DHT_BUS_HUNG 0
#define DHT22_DATA_BIT_COUNT 41
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int dht22_read(float *temperature, float *humidity);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int dht22_read(float *temperature, float *humidity)
{
// uint8_t bitmask = _bitmask;
//volatile uint8_t *reg asm("r30") = _baseReg;
int Count, i;
unsigned char bitTimes[DHT22_DATA_BIT_COUNT];
int Humidity;
int Temperature;
unsigned char checkSum, csPart1, csPart2, csPart3, csPart4;

Humidity = 0;
Temperature = 0;
checkSum = 0;
    
//return currentTime;
for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
    {
    bitTimes[i] = 0;
    }

// Pin needs to start HIGH, wait until it is HIGH with a timeout
//cli();
DHT22_INPUT_MODE
//sei();
Count = 0;

do
 {
 if(Count > 125)
   {
   return DHT_BUS_HUNG;
   }
 Count++;
 delay_us(2);
 }while(!DHT22_READ);
    
// Send the activate pulse
//cli();
DHT22_LOW
DHT22_OUTPUT_MODE // Output Low
//sei();
delay_us(1100); // 1.1 ms
//cli();
DHT22_INPUT_MODE    // Switch back to input so pin can float
//sei();
// Find the start of the ACK Pulse
Count = 0;

do
 {
 if(Count > 25) //(Spec is 20 to 40 us, 25*2 == 50 us)
   {
   return DHT_BUS_HUNG;
   }
   Count++;
   delay_us(2);
 } while(!DHT22_READ);
 
// Find the end of the ACK Pulse
    
Count = 0;
do
 {
 if(Count > 50) //(Spec is 80 us, 50*2 == 100 us)
   {
   return DHT_BUS_HUNG;
   }
   Count++;
   delay_us(2);
 }while(DHT22_READ);
 
// Read the 40 bit data stream
for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
   {
   // Find the start of the sync pulse
   Count = 0;
   do
    {
    if(Count > 35) //(Spec is 50 us, 35*2 == 70 us)
      {
      return DHT_BUS_HUNG;
      }
    Count++;
    delay_us(2);
    } while(!DHT22_READ);

   // Measure the width of the data pulse
   Count = 0;
   do
     {
     if(Count > 50) //(Spec is 80 us, 50*2 == 100 us)
       {
       return DHT_BUS_HUNG;
       }
       Count++;
       delay_us(2);
     }while(DHT22_READ);
   bitTimes[i] = Count;
   }

// Now bitTimes have the number of retries (us *2)
// that were needed to find the end of each data bit
// Spec: 0 is 26 to 28 us
// Spec: 1 is 70 us
// bitTimes[x] <= 11 is a 0
// bitTimes[x] >  11 is a 1
// Note: the bits are offset by one from the data sheet, not sure why
for(i = 0; i < 16; i++)
   {
   if(bitTimes[i + 1] > 11)
     {
     Humidity |= (1 << (15 - i));
     }
   }

for(i = 0; i < 16; i++)
   {
   if(bitTimes[i + 17] > 11)
     {
     Temperature |= (1 << (15 - i));
     }
   }
    
for(i = 0; i < 8; i++)
   {
   if(bitTimes[i + 33] > 11)
     {
     checkSum |= (1 << (7 - i));
     }
   }

*humidity = ((float)(Humidity & 0x7FFF)) / 10.0;
    
if(Temperature & 0x8000)
  {
  // Below zero, non standard way of encoding negative numbers!
  Temperature &= 0x7FFF;
  *temperature = ((float)Temperature / 10.0) * -1.0;
  }
  else
  {
  *temperature = (float)Temperature / 10.0;
  }

csPart1 = Humidity >> 8;
csPart2 = Humidity & 0xFF;
csPart3 = Temperature >> 8;
csPart4 = Temperature & 0xFF;
    
if(checkSum == ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF))
  {
  //*humidity = 10;
  //return DHT_ERROR_NONE;
  return 1;
  }

//*humidity = 20;
//return DHT_ERROR_CHECKSUM;

return 0;
}