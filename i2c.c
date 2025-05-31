#include "i2c.h"

void I2C1_Init(void){
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // Configurar PB6 y PB7 como Alternate Function Open-Drain
  GPIOB->CRL &= ~((0xF << 24) | (0xF << 28));
  GPIOB->CRL |= (0xB << 24) | (0xB << 28);  // 0xB: 50 MHz, AF Open-Drain

  // Reset y configuración
  I2C1->CR1 |= I2C_CR1_SWRST;
  I2C1->CR1 &= ~I2C_CR1_SWRST;

  I2C1->CR2 = 36;        // PCLK1=36 MHz
  I2C1->CCR = 180;       // 100 kHz
  I2C1->TRISE = 37;      // Máx subida
  I2C1->CR1 |= I2C_CR1_ACK; // ACK automático
  I2C1->CR1 |= I2C_CR1_PE;
}

void I2C1_Start(void){
  I2C1->CR1 |= I2C_CR1_START;
  while(!(I2C1->SR1 & I2C_SR1_SB));
}

void I2C1_Stop(void){
  I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C1_Write(uint8_t data){
  while(!(I2C1->SR1 & I2C_SR1_TXE));
  I2C1->DR = data;
  while(!(I2C1->SR1 & I2C_SR1_BTF));
}

uint8_t I2C1_ReadAck(void){
  I2C1->CR1 |= I2C_CR1_ACK;
  while(!(I2C1->SR1 & I2C_SR1_RXNE));
  return I2C1->DR;
}

uint8_t I2C1_ReadNack(void){
  I2C1->CR1 &= ~I2C_CR1_ACK;
  while(!(I2C1->SR1 & I2C_SR1_RXNE));
  return I2C1->DR;
}

void I2C1_WriteReg(uint8_t addr, uint8_t reg, uint8_t data){
  I2C1_Start();
  I2C1_Write(addr << 1); // Dirección + write
  I2C1_Write(reg);
  I2C1_Write(data);
  I2C1_Stop();
}

void I2C1_ReadReg(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t size){
  I2C1_Start();
  I2C1_Write(addr << 1); // Dirección + write
  I2C1_Write(reg);

  I2C1_Start();          // Repeated start
  I2C1_Write((addr << 1) | 1); // Dirección + read

  while(size--){
    if(size == 0)
      *data++ = I2C1_ReadNack();
    else
      *data++ = I2C1_ReadAck();
  }
  I2C1_Stop();
}
