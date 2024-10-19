#include "LED.h"

int main(void)
{
	uint32_t i;

	LED_Init();

	while(1)
	{
		LED_ON();		for(i = 0; i < 70000; i++);
		LED_OFF();		for(i = 0; i < 70000; i++);
	}
}
