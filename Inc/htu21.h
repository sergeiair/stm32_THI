
#define TEMPERATURE_COEFF_MUL								(175.72)
#define TEMPERATURE_COEFF_ADD								(-46.85)

#define HUMIDITY_COEFF_MUL									(125)
#define HUMIDITY_COEFF_ADD									(-6)

#define BMP_I2C_TIMEOUT					(50U)


typedef struct {
	float hum;
	float temp;
} htuData;

void read_htu_hum(htuData * htu);

void read_htu_temp(htuData * htu);

void clean_up_htu(htuData * htu);