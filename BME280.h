#ifndef BME280_H_
#define BME280_H_

#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define BME280_ADDR          0x76 << 1  // 7-bit address shifted for STM32 (write/read)
#define CHIP_ID_BME          0x60

#define RESET_REG_ADDR       0xE0
#define CHIP_ID_REG_ADDR     0xD0
#define CONFIG_REG_ADDR      0xF5
#define CTRL_HUM_REG_ADDR    0xF2
#define CTRL_MEAS_REG_ADDR   0xF4
#define CALIB_DATA00_25_BASEADDR  0x88
#define CALIB_DATA26_41_BASEADDR  0xE1
#define RAWDATA_BASEADDR     0xF7

#define BME280_SLEEP_MODE    0x00

typedef struct {
  int32_t pressr;
  int32_t tempr;
  int32_t humr;
} Raw_Data_t;

typedef struct {
  float Temperature;
  float Pressure;
  float Humidity;
  float AltitudeP;
  float AltitudeTP;
} BME280_Data_t;

typedef struct {
  uint8_t T_StandBy;
  uint8_t Filter;
  uint8_t SPI_EnOrDis;
  uint8_t OverSampling_H;
  uint8_t OverSampling_T;
  uint8_t OverSampling_P;
  uint8_t Mode;
} BME280_Init_t;

// Funciones de driver
void BME280_I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
void BME280_I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

void Reset_BME280(void);
void Calibdata_BME280(void);
Raw_Data_t RawdataBME280(void);
int32_t BME280_measure_Temp(int32_t adc_T);
uint32_t BME280_measure_Press(int32_t adc_P);
uint32_t BME280_measure_Hum(int32_t adc_H);
void BME280Calculation(BME280_Data_t *result);
void BME280Init(BME280_Init_t BME280Init);
void BME280_Config(BME280_Init_t *bme);
void print_bme280_data(const BME280_Data_t *bmeData);

#endif /* BME280_H_ */
