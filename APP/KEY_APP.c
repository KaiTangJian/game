#include "mydefine.h"
void key_proc(void);

uint8_t key_read()
{
	uint8_t temp = 0;

  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15) == GPIO_PIN_RESET) temp = 1;
	return temp;

}
