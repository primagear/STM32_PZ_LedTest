
#include "stm32f10x.h"
uint8_t sts=0x00;
uint8_t systickbasetimeHz=0x00;

// "core_cm3.h"
#ifndef SysTick
#define SysTick ((SysTickTypeDef*)SysTickBase)
#endif
#ifndef SysTickBase
#define SysTickBase ((uint32_t)0xE000E010)
#endif
#ifndef SysTickCtrlEnaMask
#define SysTickCtrlEnaMask ((uint32_t)0x01)
#endif
typedef struct{
	__IO uint32_t CTRL;
	__IO uint32_t LOAD;
	__IO uint32_t VAL;
	__IO uint32_t VALIB;
}SysTickTypeDef;

// "stm32f10x.h"
#ifndef GpiocBase
#define GpiocBase ((uint32_t)0x40011000)
#endif
#ifndef GpiobBase
#define GpiobBase ((uint32_t)0x40010C00)
#endif
#ifndef GpioeBase
#define GpioeBase ((uint32_t)0x40011800)
#endif
#ifndef RccBase
#define RccBase ((uint32_t)0x40021000)
#endif
#ifndef Gpioc
#define Gpioc ((GPIOTypeDef*)GpiocBase)
#endif
#ifndef Gpiob
#define Gpiob ((GPIOTypeDef*)GpiobBase)
#endif
#ifndef Gpioe
#define Gpioe ((GPIOTypeDef*)GpioeBase)
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

// "misc.h"
#ifndef SysTick_CLKSource_HCLK_Div8
#define SysTick_CLKSource_HCLK_Div8 ((uint32_t)0xFFFFFFFB)
#endif
#ifndef SysTick_CLKSource_HCLK
#define SysTick_CLKSource_HCLK ((uint32_t)0x00000004)
#endif
// "misc.c"
void SysTickClksourceConfig(uint32_t SysTickClksource){
		if(SysTickClksource==SysTick_CLKSource_HCLK){
				SysTick->CTRL |=SysTick_CLKSource_HCLK;
		}else{
				SysTick->CTRL &=SysTick_CLKSource_HCLK_Div8;
		}
}

// "gpio.h"
#ifndef GpioPin0
#define GpioPin0 ((uint16_t)0x0000)
#endif
#ifndef GpioPin5
#define GpioPin5 ((uint16_t)0x0005)
#endif
#ifndef GpioPin2
#define GpioPin2 ((uint16_t)0x0002)
#endif
#ifndef GpioPin3
#define GpioPin3 ((uint16_t)0x0003)
#endif
#ifndef GpioPin4
#define GpioPin4 ((uint16_t)0x0004)
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
			targetmode = (currentmode&0x0F)/4;   //output pin config
			tareg|=targetmode<<2;
	}else{
			targetmode=((currentmode&0x0F)>>2); //input pin config
			tareg=targetmode<<2;
	}
	
	if(pinpos<8){	
		temreglow&=~(0x0F<<(pinpos*4));
		temreglow|=(tareg<<(pinpos*4));
		Gpiox->CRL=temreglow;
	}else{	
		temreghigh&=~(0x0F<<((pinpos-8)*4));
		temreghigh|=(tareg<<((pinpos-8)*4));
		Gpiox->CRH=temreghigh;
	}
	if(GPIOInitStructure->GPIOMode ==GPIOModeIPU ){
		Gpiox->BSRR= (((uint32_t)0x01)<<pinpos);
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
uint16_t GPIOReadBit(GPIOTypeDef* Gpiox,uint16_t GpioPin){
	uint16_t temp=Gpiox->IDR;
	if( (Gpiox->IDR & (0x01<<GpioPin)) != (uint32_t)0x00){
		temp=(uint8_t)0x01;
	}else{
		temp=(uint8_t)0x00;
	}
	return temp;
}

// "rcc.h"
#ifndef Rcc_Apb2Periph_Gpioc
#define Rcc_Apb2Periph_Gpioc ((uint32_t)0x00000010)
#endif
#ifndef Rcc_Apb2Periph_Gpiob
#define Rcc_Apb2Periph_Gpiob ((uint32_t)0x00000008)
#endif
#ifndef Rcc_Apb2Periph_Gpioe
#define Rcc_Apb2Periph_Gpioe ((uint32_t)0x00000040)
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
	sts=0x03; //LED initiation done
}

// "beep.h"
#define BEEPPORT Gpiob
#define BEEPPIN	GpioPin5
#define BEEPCLOCK Rcc_Apb2Periph_Gpiob

// "beep.c"
void BeepInit(){
	GPIOInitTypeDef GPIOInitStructureBeep;
	RCCAPB2PeriphClockEnable(BEEPCLOCK,EnableValue);
	GPIOInitStructureBeep.GPIOPin=BEEPPIN;
	GPIOInitStructureBeep.GPIOSpeed =GPIOSpeed50MHz;
	GPIOInitStructureBeep.GPIOMode=GPIOModeOutPP;
	GPIOInit(BEEPPORT,&GPIOInitStructureBeep);
	sts=0x04; //Beep initiation done
}

// "SysTick.c"
void SysTickInit(u8 sysclk){
		SysTickClksourceConfig(SysTick_CLKSource_HCLK_Div8);
		systickbasetimeHz = sysclk>>3;
}
void delayus(uint32_t nus){
		uint32_t tem;
		SysTick->LOAD=nus*systickbasetimeHz;
		SysTick->VAL=0x00;
		SysTick->CTRL |=SysTickCtrlEnaMask ;
	do{
		tem=SysTick->CTRL;
	}while((tem&0x01)&&!(tem&(1<<16)));
		SysTick->CTRL &=~SysTickCtrlEnaMask ;
	  SysTick->VAL=0x00;
}

void delay10ms(){
	uint16_t time=10,i=0;
	
	while(time-->0){
			i=12000;
			while(i--);
	}
}
void delaynms(uint16_t nms){
	uint16_t i=0;
	
	while(nms-->0){
			i=12000;
			while(i--);
	}
}
//"key.h"
#define KeyLeftPin GpioPin2
#define KeyDownPin GpioPin3
#define KeyRightPin GpioPin4
#define KEYPORT Gpioe
#define KEYCLOCK Rcc_Apb2Periph_Gpioe
#define K_LEFT  GPIOReadBit(KEYPORT,KeyLeftPin)

//"key.c"
void KEYInit(){
	GPIOInitTypeDef GPIOInitStructureKey;
	RCCAPB2PeriphClockEnable(KEYCLOCK,EnableValue);
	GPIOInitStructureKey.GPIOPin=KeyLeftPin;
	GPIOInitStructureKey.GPIOMode=GPIOModeIPU;
	GPIOInit(KEYPORT,&GPIOInitStructureKey);
	
}
//uint8_t KEYScan(){
//	static uint16_t key=1;
//	
//	if(key==1 && K_LEFT==0){
//		delay10ms();
//		key=0;
//		if(K_LEFT==0){ return 0x0F;}
//		else { return 0x01;}
//		
//	}
//	return 0x01;
//}


uint8_t KEYScan(u8 mode)
{
	static u8 key=1;
	
	if(key==1&& K_LEFT==0) //????????
	{
		delay10ms();  //??
		key=0;
		if(K_LEFT==0){return 0x0F; }
	}
	else if(K_LEFT==1)    //?????
	{
		key=1;
	}
	if(mode==1) //??????
	{
		key=1;
	}
	return 0x01;
}

// "main.c"

int main()
{
	uint8_t keyvalue,i,j;
	SysTickInit(72);
	LEDInit();
	//BeepInit();
	KEYInit();
		
	while(1){
			keyvalue=KEYScan(0);
				if(keyvalue==0x01){
				GPIOSetBit(LEDPORT,GpioPin0);
					}else{
						for(i=0;i<5;i++){
							for(j=0;j<5;j++){
							GPIOResetBit(LEDPORT,GpioPin0);
									delaynms(500);
							GPIOSetBit(LEDPORT,GpioPin0);
									delaynms(500);	
							}
						}	
					}

	}	
}
