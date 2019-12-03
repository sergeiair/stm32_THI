
#include "stdint.h"


// Data type definitions:
#define BYTE_SHIFT					(8U)
#define SHORT_SHIFT					(16U)

/* Chip-id [R/O] (register D0h): This value is fixed to 0x55,
 * and can be used to check whether communication is functioning. */
#define BMP_CHIP_ID_REG			      		(0xD0)
#define BMP_CHIP_ID_VAL					(0x55)

/* Measurement control [R/W] (register F4h):
 * <4:0> - Controls measurements.
 * <5>   - SCO : Start of conversion. The value of this bit stays “1” during conversion,
 *               and is reset to “0” after conversion is complete
 * <7:6> - OSS : Controls the oversampling ratio of the pressure measurement
 */
#define BMP_CTRL_REG	         	       		(0xF4)
#define BMP_CTRL_SCO_BIT(reg)				((reg & 0x20) >> 5)
#define BMP_CTRL_OSS_MASK(oss)				(oss = (oss & 0x3) << 6)

#define BMP_OSS0_CONV_TIME				(5U)
#define BMP_OSS1_CONV_TIME				(8U)
#define BMP_OSS2_CONV_TIME				(14U)
#define BMP_OSS3_CONV_TIME				(26U)

/* Soft reset [W/O] (register E0h):
 * If set to 0xB6, will perform the same sequence as power on reset. */
#define BMP_SOFT_RST_REG 	                	(0xE0)
#define BMP_SOFT_RST_VAL  	                	(0xB6)

// Calibration data [R/O] (register AAh up to BFh):
#define BMP_CALIB_ADDR  	                	(0xAA)
#define BMP_CALIB_DATA_SIZE				(22U)

// Device I2C addr register [R/O]: write EEh, read EFh:
#define BMP_READ_ADDR	                    		(0xEF)
#define BMP_WRITE_ADDR		                	(0xEE)
#define BMP_I2C_TIMEOUT					(50U)
#define BMP_SET_I2CRW_REG(i2c_buff, reg)		(i2c_buff = reg)

// BMP measurmenet regs
#define BMP_DATA_MSB_ADDR	                	(0xF6)
#define BMP_DATA_LSB_ADDR	                	(0xF7)
#define BMP_DATA_XLSB_ADDR	                	(0xF8)

// Temp. measurement :
#define BMP_SET_TEMP_CONV		            	(0x2E)

#define BMP_TEMP_CONV_TIME				(50U)
#define BMP_MIN_TEMP_THRESHOLD				(-40)
#define BMP_MAX_TEMP_THRESHOLD				(85U)

// Pressure measurment :
#define BMP_SET_PRESS_CONV		            	(0x34)
#define BMP_PRESS_CONST_SEA_LEVEL 	    		(101325.0f) // pressure in Pa
#define BMP_PRESS_CONST_COEFICIENT 	    		(44330.0f)

// Altitude measurment :
#define BMP_MIN_ALT_THRESHOLD				(-500)	// m. relating to sea level)
#define BMP_MAX_ALT_THRESHOLD				(9000U)	// m. relating to sea level)

struct bmpCalibParams
{
	int16_t	 AC1;
	int16_t  AC2;
	int16_t  AC3;
	uint16_t AC4;
	uint16_t AC5;
	uint16_t AC6;
	int16_t  B1;
	int16_t  B2;
	int16_t  MB;
	int16_t  MC;
	int16_t  MD;
};

struct bmpMeasurementData
{
	float temp;
	float altitude;
	int32_t press;
	int32_t B5;
};

typedef enum
{
	ULTRA_LOW_PWR_MODE = 0,
	STANDARD_MODE,
	HIGH,
	ULTRA_HIGH_RESOLUTION
} bmpOssRatio;

typedef struct
{
	bmpOssRatio ratio;
	uint8_t wait_time;
} bmpOss;

typedef enum
{
	NO_ERR = 0,
	CHIP_ID_INVALID_ERR,
	GET_CALIB_ERR,
	GET_TEMP_ERR,
	GET_PRESSURE_ERR,
	GET_ALTITUDE_ERR
} bmpError;

typedef struct
{
	struct bmpCalibParams calib;
	struct bmpMeasurementData uncompData;
	struct bmpMeasurementData compData;
	bmpOss oss;
	bmpError err;
} bmpData;

static void read_calib_data (bmpData * bmp);

static void read_chip_id (bmpData * bmp);

static void set_oss (bmpOss * oss, bmpOssRatio ratio);

void clean_up_bmp(bmpData * bmp);

void init_bmp (bmpData * bmp);

void read_comp_temp(bmpData * bmp);

void read_uncomp_temp (bmpData * bmp);

