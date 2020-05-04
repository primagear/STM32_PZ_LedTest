
#include "stm32f10x.h"
uint8_t sts=0x00;

// "stm32f10x.h"
#ifndef GpiocBase
#define GpiocBase ((uint32_t)0x40011000)
#endif
#ifndef RccBase
#define RccBase ((uint32_t)0x40021000)
#endif
#ifndef Gpioc
#define Gpioc ((GPIOTypeDef*)GpiocBase)
#endif
#ifndef Rcc
#define Rcc ((RCCTypeDef*)RccBase)
#endif
typedef enum{
	DisableValue=0,
	EnableValue =!DisableValue
}FunctionalStateType;
typedef struct{
	__IO uint32_t CRL;
	__IO uint32_t CRH;
	__IO uint32_t IDR;
	__IO uint32_t ODR;
	__IO uint32_t BSRR;
	__IO uint32_t BRR;
	__IO uint32_t LCKR;
}GPIOTypeDef;
typedef struct{
	__IO uint32_t CR;
	__IO uint32_t CFGR;
	__IO uint32_t CIR;
	__IO uint32_t APB2RSTR;
	__IO uint32_t APB1RSTR;
	__IO uint32_t AHBENR;
	__IO uint32_t APB2ENR;
	__IO uint32_t APB1ENR;
	__IO uint32_t BDCR;
	__IO uint32_t CSR;
}RCCTypeDef;

// "gpio.h"
#ifndef GpioPin0
#define GpioPin0 ((uint16_t)0x0003)
#endif
typedef enum{
	GPIOSpeed10MHz=0x01,
	GPIOSpeed2MHz=0x02,
	GPIOSpeed50MHz=0x03
}GPIOSpeedTypeDef;
typedef enum{
	GPIOModeAIN=0x00,
	GPIOModeINFLOATING=0x04,
	GPIOModeIPD=0x28,
	GPIOModeIPU=0x48,
	GPIOModeOutPP=0x10,
	GPIOModeOutOD=0x14,
	GPIOModeAFPP=0x18,
	GPIOModeAFOD=0x1C
}GPIOModeTypeDef;
typedef struct{
	uint16_t GPIOPin;
	GPIOSpeedTypeDef GPIOSpeed;
	GPIOModeTypeDef GPIOMode;
}GPIOInitTypeDef;

// "gpio.c"
void GPIOInit(GPIOTypeDef* Gpiox,GPIOInitTypeDef* GPIOInitStructure){
	uint32_t pinpos=GPIOInitStructure->GPIOPin;
	uint32_t tareg= GPIOInitStructure->GPIOSpeed;
	uint32_t currentmode=GPIOInitStructure->GPIOMode;
	uint32_t temreglow=(Gpiox->CRL),temreghigh=(Gpiox->CRH),targetmode=0xFF;
	if( (currentmode&0x10) != 0x00){
			targetmode = (currentmode&0x0F)/4;
	}	
	tareg|=targetmode<<2;
	if(pinpos<8){	
		temreglow&=~(0x0F<<(pinpos*4));
		temreglow|=(tareg<<(pinpos*4));
		Gpiox->CRL=temreglow;
	}else{	
		temreghigh&=~(0x0F<<((pinpos-8)*4));
		temreghigh|=(tareg<<((pinpos-8)*4));
		Gpiox->CRH=temreghigh;
	}
	sts=0x03; // GPIOC CRL config done
}

void GPIOResetBit(GPIOTypeDef* Gpiox,uint16_t GpioPin){
	uint16_t temp=Gpiox->BRR;
	temp|= ((0x01)<<GpioPin);
	Gpiox->BRR =temp;
}
void GPIOSetBit(GPIOTypeDef* Gpiox,uint16_t GpioPin){
	uint16_t temp=Gpiox->BSRR;
	temp|= ((0x01)<<GpioPin);
	Gpiox->BSRR =temp;
}

// "rcc.h"
#ifndef Rcc_Apb2Periph_Gpioc
#define Rcc_Apb2Periph_Gpioc ((uint32_t)0x00000010)
#endif

// "rcc.c"
void RCCAPB2PeriphClockEnable(uint32_t RCCAPB2Periph,FunctionalStateType NewState){
	if(NewState !=DisableValue){
		 Rcc->APB2ENR |= RCCAPB2Periph;
	}else{
		 Rcc->APB2ENR &= ~RCCAPB2Periph;
	}
	sts=0x02; //GPIOC clock enable done
}

// "led.h"
#define LEDPORT Gpioc
#define LEDPIN	GpioPin0
#define LEDCLOCK Rcc_Apb2Periph_Gpioc
//void LEDInit();

//"led.c"
void LEDInit(){
	GPIOInitTypeDef GPIOInitStructure;	
	RCCAPB2PeriphClockEnable(LEDCLOCK,EnableValue);
	GPIOInitStructure.GPIOPin=LEDPIN;
	GPIOInitStructure.GPIOSpeed=GPIOSpeed50MHz;
	GPIOInitStructure.GPIOMode=GPIOModeOutPP;
	GPIOInit(LEDPORT,&GPIOInitStructure);
}

void delay1000ms(){
	uint16_t time=1000,i=0;
	
	while(time-->0){
			i=12000;
			while(i--);
	}
}

int main()
{
	LEDInit();
	
	sts=0x01;
		
	while(1){
		GPIOSetBit(LEDPORT,LEDPIN);
		delay1000ms();
		GPIOResetBit(LEDPORT,LEDPIN);
		delay1000ms();

	}
	
}
