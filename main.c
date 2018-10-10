/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"


#define BIT_COUNT 41


typedef struct dht11
{
  uint16_t pin;
  uint8_t temp;
  uint8_t humidity;
}dht11;




/* Global Variables ------------------------------------------------------------------*/

__IO uint32_t time = 0;

dht11 myDht11;



GPIO_InitTypeDef GPIO_struct_Out,GPIO_struct_In;


uint32_t downPeriodes[42];
uint32_t upPeriodes[42];


/* functiones ------------------------------------------------------------------*/

void startCounter();
uint32_t getCounterValue();
void waitUs( uint32_t nCount);

void setDht11GpioAsInput();
void setDht11GpioAsOutput();
void configureDht11();
void readDht11();



int main(void)
{ 
 configureDht11();


 // set systick to 1 us
 SysTick_Config(SystemCoreClock/1000000);

 while(1)
 {
   readDht11();
  
 }

}


void configureDht11()
{
 myDht11.pin = GPIO_Pin_11;
 myDht11.temp = 0;
 myDht11.humidity = 0 ;
 
 
 //enable GPIOD clock
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
 
 //config GPIOD PD11  as output
 GPIO_struct_Out.GPIO_Pin = myDht11.pin;
 GPIO_struct_Out.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_struct_Out.GPIO_Speed = GPIO_Speed_2MHz;
 GPIO_struct_Out.GPIO_OType = GPIO_OType_PP;
 
 //config GPIOD PD11  as intput
 GPIO_struct_In.GPIO_Pin = myDht11.pin;
 GPIO_struct_In.GPIO_Mode = GPIO_Mode_IN;
 GPIO_struct_In.GPIO_PuPd = GPIO_PuPd_UP;
}

void setDht11GpioAsInput()
{

 GPIO_Init(GPIOD,&GPIO_struct_In);
}


void setDht11GpioAsOutput()
{

 GPIO_Init(GPIOD,&GPIO_struct_Out);
}

void readDht11()
{
    int bitCounter =0;
    waitUs(1000000);
    setDht11GpioAsOutput();
    GPIO_SetBits(GPIOD, myDht11.pin);
    waitUs(80000);
    //trigger the dht11
    GPIO_ResetBits(GPIOD, myDht11.pin);
    waitUs(80000);
    //Start reading from the dht11
    setDht11GpioAsInput();
    
    //wait untill the dht11 pulls down
    while( GPIO_ReadInputDataBit(GPIOD,myDht11.pin) == 1);
    
    for ( bitCounter = 0 ; bitCounter < BIT_COUNT ; bitCounter ++)
    { 
      //count the down period
      
      startCounter();
      while( GPIO_ReadInputDataBit(GPIOD,myDht11.pin) == 0);
      downPeriodes[bitCounter] = getCounterValue();
      
      //count the up period
      startCounter();
      while( GPIO_ReadInputDataBit(GPIOD,myDht11.pin) == 1);
      upPeriodes[bitCounter] = getCounterValue();
    }
    
    myDht11.humidity = 0;
    for ( bitCounter = 0 ; bitCounter < 8 ; bitCounter ++)
    {
      if( upPeriodes[bitCounter + 1] > 42 )
      {
        myDht11.humidity = (myDht11.humidity<<1) + 1; 
      }
      else
      {
        myDht11.humidity = myDht11.humidity<<1 ;
      }
    }
    myDht11.temp = 0;
    for ( bitCounter = 0 ; bitCounter < 8 ; bitCounter ++)
    {
      if( upPeriodes[bitCounter + 17] > 42 )
      {
        myDht11.temp= (myDht11.temp<<1) + 1; 
      }
      else
      {
        myDht11.temp = myDht11.temp<<1 ;
      }
    }

}





void waitUs( uint32_t nCount)
{
  startCounter();
  while (getCounterValue() < nCount); 

}

void startCounter()
{
  time = 1;
}

uint32_t getCounterValue()
{

  return time;

}








#ifdef  USE_FULL_ASSERT


void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

