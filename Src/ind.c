
#include "ind.h"
#include "stm32f1xx_hal.h"

void indicate_condition (float temp)
{
	if (temp >= 21.5 && temp <= 23.5) {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
	} else if (
		(temp >= 20 && temp < 21.5) ||
		(temp > 23.5 && temp <= 25)
	) {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	}
}

void reset_indication()
{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
}

void upd_temp_2_display(float unit1Temp, float unit2Temp, float * res)
{
	*res = (unit1Temp + unit2Temp) / 2;
}







