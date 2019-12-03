
#include <stdint.h>
#include <string.h>
#include "htu21.h"
#include "i2c.h"

void read_htu_hum(htuData * htu)
{
	uint8_t te[2] = {0};
	HAL_I2C_Mem_Read(&hi2c1, 0x80, 0xE5, 1, te, 2, BMP_I2C_TIMEOUT);

	if((te[1]&0x03)!=0x02) {
		htu->hum = -999;
	}
	
	htu->hum=te[0]<<8;
	htu->hum=htu->hum+(te[1]>>2);
	
	htu->hum = (float)htu->hum * HUMIDITY_COEFF_MUL / (1UL<<16) + HUMIDITY_COEFF_ADD;
}

void read_htu_temp(htuData * htu)
{
	uint8_t te[2] = {0};
	
	HAL_I2C_Mem_Read(&hi2c1, 0x80, 0xE3, 1, te, 2, BMP_I2C_TIMEOUT);

	if((te[1]&0x03)!=0x00) {
		htu->temp = -999;
	}
	
	htu->temp=te[0]<<8;
	htu->temp=htu->temp  +(te[1]>>2);
	

	htu->temp = (float)htu->temp * TEMPERATURE_COEFF_MUL / (1UL<<16) + TEMPERATURE_COEFF_ADD;
}

void clean_up_htu(htuData * htu) 
{
	memset(htu, 0x00, sizeof(&htu));
}
