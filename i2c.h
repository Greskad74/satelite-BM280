#ifndef I2C_H_
#define I2C_H_

#include "stm32f10x.h"

void I2C1_Init(void);

void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_Write(uint8_t data);
uint8_t I2C1_ReadAck(void);
uint8_t I2C1_ReadNack(void);

void I2C1_WriteReg(uint8_t addr, uint8_t reg, uint8_t data);
void I2C1_ReadReg(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t size);

#endif
