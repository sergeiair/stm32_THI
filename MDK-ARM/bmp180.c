
#include <string.h>
#include "bmp180.h"
#include "i2c.h"


void read_calib_data (bmpData * bmp)
{
	bmp->err = NO_ERR;
	uint8_t out_buff[BMP_CALIB_DATA_SIZE] = {0};

	HAL_I2C_Mem_Read(&hi2c1, BMP_READ_ADDR, BMP_CALIB_ADDR, 1, out_buff, BMP_CALIB_DATA_SIZE, BMP_I2C_TIMEOUT);
	
	bmp -> calib.AC1 = (out_buff[0]  << 8) | out_buff[1];
	bmp -> calib.AC2 = (out_buff[2]  << 8) | out_buff[3];
	bmp -> calib.AC3 = (out_buff[4]  << 8) | out_buff[5];
	bmp -> calib.AC4 = (out_buff[6]  << 8) | out_buff[7];
	bmp -> calib.AC5 = (out_buff[8]  << 8) | out_buff[9];
	bmp -> calib.AC6 = (out_buff[10] << 8) | out_buff[11];
	bmp -> calib.B1  = (out_buff[12] << 8) | out_buff[13];
	bmp -> calib.B2  = (out_buff[14] << 8) | out_buff[15];
	bmp -> calib.MB  = (out_buff[16] << 8) | out_buff[17];
	bmp -> calib.MC  = (out_buff[18] << 8) | out_buff[19];
	bmp -> calib.MD  = (out_buff[20] << 8) | out_buff[21];  

		if ((0 == bmp -> calib.AC1) | (-1 == bmp -> calib.AC1))
		{
			bmp->err = GET_CALIB_ERR;
		}
}

void read_chip_id (bmpData * bmp)
{
	uint8_t rst_buff[3];
	uint8_t out_buff = 0;
	
	bmp->err = NO_ERR;

	HAL_I2C_Mem_Write( &hi2c1, BMP_WRITE_ADDR, BMP_SOFT_RST_REG, 1, rst_buff, 1, BMP_I2C_TIMEOUT );
	HAL_Delay (10);
	HAL_I2C_Mem_Read(&hi2c1, BMP_READ_ADDR, BMP_CHIP_ID_REG, 1, &out_buff, 1, BMP_I2C_TIMEOUT);

	if (BMP_CHIP_ID_VAL != out_buff)
	{
		bmp->err = CHIP_ID_INVALID_ERR;
	}
}

void set_oss (bmpOss * oss, bmpOssRatio ratio)
{
	uint8_t in_buff[2] = {0};

	switch (ratio)
	{
		case ULTRA_LOW_PWR_MODE:
		{
			oss->wait_time = BMP_OSS0_CONV_TIME;
			break;
		}
		case STANDARD_MODE:
		{
			oss->wait_time = BMP_OSS1_CONV_TIME;
			break;
		}
		case HIGH:
		{
			oss->wait_time = BMP_OSS2_CONV_TIME;
			break;
		}
		case ULTRA_HIGH_RESOLUTION:
		{
			oss->wait_time = BMP_OSS3_CONV_TIME;
			break;
		}
		default:
		{
			oss->wait_time = BMP_OSS1_CONV_TIME;
			break;
		}
	}

	oss->ratio = ratio;
	
	BMP_SET_I2CRW_REG (in_buff[1], BMP_CTRL_OSS_MASK(ratio));
	HAL_I2C_Mem_Write( &hi2c1, BMP_WRITE_ADDR, BMP_CTRL_REG, 1, in_buff, 2, BMP_I2C_TIMEOUT );
}

void clean_up_bmp(bmpData * bmp) 
{
	memset(bmp, 0x00, sizeof(&bmp));
}

void init_bmp (bmpData * bmp)
{
	clean_up_bmp(bmp);
	
	HAL_Delay(30);
	read_chip_id(bmp); 
  
	HAL_Delay(50);
	read_calib_data(bmp);
	
	set_oss (&bmp->oss, 1);
}


void read_uncomp_temp (bmpData * bmp) 
{
	uint8_t out_buff[3];

	BMP_SET_I2CRW_REG (out_buff[0], BMP_SET_TEMP_CONV);
	HAL_I2C_Mem_Write( &hi2c1, BMP_WRITE_ADDR, BMP_CTRL_REG, 1, out_buff, 1, BMP_I2C_TIMEOUT );
	HAL_Delay (BMP_TEMP_CONV_TIME);
	HAL_I2C_Mem_Read ( &hi2c1, BMP_READ_ADDR, BMP_DATA_MSB_ADDR, 1, out_buff, 2, BMP_I2C_TIMEOUT );
	
	bmp->uncompData.temp = ((out_buff[0]<<16)  | out_buff[1]<<8 | out_buff[2])>>8;
}

void read_comp_temp(bmpData * bmp)
{
	int32_t X1 = 0;
	int32_t X2 = 0;
	int32_t B5 = 0;
	
	int32_t uncompTemp = (int)bmp->uncompData.temp;
	X1 = ((uncompTemp - bmp->calib.AC6) * bmp->calib.AC5) >> 15;
	X2 = (bmp->calib.MC << 11) / (X1 + bmp->calib.MD);
	B5 = X1 + X2;
	
	bmp->compData.temp = ((B5 + 8) >> 4) * 0.1f;

	if ((bmp->compData.temp <= BMP_MIN_TEMP_THRESHOLD) || (bmp->compData.temp >= BMP_MAX_TEMP_THRESHOLD))
	{
		bmp->compData.temp = -999;
		bmp->err = GET_TEMP_ERR;
	}
}


