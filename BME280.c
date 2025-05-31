#include "BME280.h"
#include "usart.h"
#include "delay.h"

// Calibración global
unsigned short dig_T1, dig_P1, dig_H1, dig_H3;
signed short dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9, dig_H2, dig_H4, dig_H5, dig_H6;
int32_t t_fine;

// Funciones básicas de I2C usando registros directos
void BME280_I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len) {
  while(I2C1->SR2 & I2C_SR2_BUSY);
  I2C1->CR1 |= I2C_CR1_START;
  while(!(I2C1->SR1 & I2C_SR1_SB));
  (void)I2C1->SR1;
  I2C1->DR = dev_addr;
  while(!(I2C1->SR1 & I2C_SR1_ADDR));
  (void)I2C1->SR2;
  I2C1->DR = reg_addr;
  while(!(I2C1->SR1 & I2C_SR1_TXE));
  for(uint16_t i=0; i<len; i++) {
    I2C1->DR = data[i];
    while(!(I2C1->SR1 & I2C_SR1_TXE));
  }
  I2C1->CR1 |= I2C_CR1_STOP;
}

void BME280_I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len) {
  while(I2C1->SR2 & I2C_SR2_BUSY);
  I2C1->CR1 |= I2C_CR1_START;
  while(!(I2C1->SR1 & I2C_SR1_SB));
  (void)I2C1->SR1;
  I2C1->DR = dev_addr;
  while(!(I2C1->SR1 & I2C_SR1_ADDR));
  (void)I2C1->SR2;
  I2C1->DR = reg_addr;
  while(!(I2C1->SR1 & I2C_SR1_TXE));
  I2C1->CR1 |= I2C_CR1_START;
  while(!(I2C1->SR1 & I2C_SR1_SB));
  (void)I2C1->SR1;
  I2C1->DR = dev_addr | 0x01;
  while(!(I2C1->SR1 & I2C_SR1_ADDR));
  (void)I2C1->SR2;
  for(uint16_t i=0; i<len; i++) {
    if(i == len-1) I2C1->CR1 &= ~I2C_CR1_ACK;
    while(!(I2C1->SR1 & I2C_SR1_RXNE));
    data[i] = I2C1->DR;
  }
  I2C1->CR1 |= I2C_CR1_STOP;
  I2C1->CR1 |= I2C_CR1_ACK;
}

// ... y ahora las funciones del sensor (igual que antes pero reemplazando HAL por nuestras funciones directas)
void Reset_BME280(void){
  uint8_t data = 0xB6;
  BME280_I2C_Write(BME280_ADDR, RESET_REG_ADDR, &data, 1);
}

void Calibdata_BME280(void){
  uint8_t calib1[26], calib2[7];
  BME280_I2C_Read(BME280_ADDR, CALIB_DATA00_25_BASEADDR, calib1, 26);
  BME280_I2C_Read(BME280_ADDR, CALIB_DATA26_41_BASEADDR, calib2, 7);

  dig_T1 = (calib1[1]<<8)|calib1[0];
  dig_T2 = (calib1[3]<<8)|calib1[2];
  dig_T3 = (calib1[5]<<8)|calib1[4];
  dig_P1 = (calib1[7]<<8)|calib1[6];
  dig_P2 = (calib1[9]<<8)|calib1[8];
  dig_P3 = (calib1[11]<<8)|calib1[10];
  dig_P4 = (calib1[13]<<8)|calib1[12];
  dig_P5 = (calib1[15]<<8)|calib1[14];
  dig_P6 = (calib1[17]<<8)|calib1[16];
  dig_P7 = (calib1[19]<<8)|calib1[18];
  dig_P8 = (calib1[21]<<8)|calib1[20];
  dig_P9 = (calib1[23]<<8)|calib1[22];
  dig_H1 = calib1[25];
  dig_H2 = (calib2[1]<<8)|calib2[0];
  dig_H3 = calib2[2];
  dig_H4 = (calib2[3]<<4) | (calib2[4]&0x0F);
  dig_H5 = (calib2[5]<<4) | (calib2[4]>>4);
  dig_H6 = calib2[6];
}

Raw_Data_t RawdataBME280(void){
  uint8_t raw[8];
  Raw_Data_t d;
  BME280_I2C_Read(BME280_ADDR, RAWDATA_BASEADDR, raw, 8);
  d.pressr = (raw[0]<<12)|(raw[1]<<4)|(raw[2]>>4);
  d.tempr = (raw[3]<<12)|(raw[4]<<4)|(raw[5]>>4);
  d.humr  = (raw[6]<<8)|raw[7];
  return d;
}

int32_t BME280_measure_Temp(int32_t adc_T)
{
	int32_t var1, var2, T;

	var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1)))>> 12) *((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

/**
  * @brief  Compensation formula for pressure
  *
  * @param  Raw pressure data
  *
  * @retval Processed pressure data
  */
uint32_t BME280_measure_Press(int32_t adc_P)
{
	int64_t var1, var2, p;

	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0;
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
	return (uint32_t)p;
}

/**
  * @brief  Compensation formula for humidity
  *
  * @param  Raw humidity data
  *
  * @retval Processed humidity data
  */
uint32_t BME280_measure_Hum(int32_t adc_H)
{
	int32_t v_x1_u32r;

	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) *\
			v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *\
					((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +\
							((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) +\
					8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *\
			((int32_t)dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (uint32_t)(v_x1_u32r>>12);
}

/**
  * @brief  Reading temperature, pressure and humidity and calculating altitude from pressure value
  *
  * @param  None
  *
  * @retval BME280 sensor datas in structure data type
  */
void BME280Calculation(BME280_Data_t *result)
{
	Raw_Data_t rawData = RawdataBME280();

	Calibdata_BME280();
	result->Temperature = (BME280_measure_Temp(rawData.tempr))/100.0;	//Degress
	result->Pressure = (BME280_measure_Press(rawData.pressr))/25600.0;	//hPa
	result->Humidity = (BME280_measure_Hum(rawData.humr))/1024.0;		//%RH

	result->AltitudeP = 44330*(1-pow(result->Pressure/1013.25,1/5.255)); 	/*Calculation of altitude parameter in meters with
													simplified atmospheric pressure formula*/

	result->AltitudeTP = ((log(result->Pressure/1013.25)*(288.16)*(-8.314))/(28.97*9.81))*1000; /*Calculation of altitude parameter in
																		                    meters with atmospheric pressure formula*/
}


void BME280_Config(BME280_Init_t *bme)
{
	// ctrl_hum
	uint8_t hum = bme->OverSampling_H & 0x07;
	BME280_I2C_Write(BME280_ADDR, 0xF2, &hum, 1);

	// ctrl_meas
	uint8_t meas = ((bme->OverSampling_T & 0x07) << 5) |
	                ((bme->OverSampling_P & 0x07) << 2) |
	                (bme->Mode & 0x03);
	BME280_I2C_Write(BME280_ADDR, 0xF4, &meas, 1);

	// config
	uint8_t config = ((bme->T_StandBy & 0x07) << 5) |
	                  ((bme->Filter & 0x07) << 2) |
	                  (bme->SPI_EnOrDis & 0x01);
	BME280_I2C_Write(BME280_ADDR, 0xF5, &config, 1);
}
void print_bme280_data(const BME280_Data_t *bmeData) {
    delay_ms(200);
    uart_send_string(" Datos BME280 ");
    uart_send_string(" \n");
    uart_send_string(" \r");

    uart_send_string("Temperatura: ");
    uart_send_double(bmeData->Temperature);
    uart_send_string(" C ");
    uart_send_string(" \n");
    uart_send_string(" \r");

	
    uart_send_string("Presion: ");
    uart_send_double(bmeData->Pressure);
    uart_send_string(" hPa ");

	    uart_send_string(" \n");
    uart_send_string(" \r");

    uart_send_string("Humedad: ");
    uart_send_double(bmeData->Humidity);
    uart_send_string(" %");
	    uart_send_string(" \n");
    uart_send_string(" \r");

    delay_ms(1000);
}
