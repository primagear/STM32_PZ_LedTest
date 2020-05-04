#include "stm32f10x.h"

// "led.h"
#define LEDPORT Gpioc
#define LEDPIN	GpioPin0
#define LEDCLOCK Rcc_Apb2Periph_Gpioc
void LEDInit();

