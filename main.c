#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "Satelite.h"
#include "BME280.h"
#include "delay.h"
#include "i2c.h"
// Buffers y estructuras
char gps_buffer[128];
gps_point_t gps_point;
GPS_Time_t gps_time;

int main(void) {
    delay_init();
    uart_dma_init(gps_buffer, sizeof(gps_buffer)); // USART1 RX con DMA
    uart_send_string("USART1-DMA inicializado.\r\n");

    I2C1_Init();
    uart_send_string("I2C1 inicializado.\r\n");

    BME280_Init_t bmeInit;
    bmeInit.T_StandBy = 0x00;        // 0.5ms standby
    bmeInit.Filter = 0x00;           // Filter off
    bmeInit.SPI_EnOrDis = 0x00;      // SPI off
    bmeInit.OverSampling_H = 0x01;   // 1x
    bmeInit.OverSampling_T = 0x01;   // 1x
    bmeInit.OverSampling_P = 0x01;   // 1x
    bmeInit.Mode = 0x03;  
    
    Reset_BME280();
    Calibdata_BME280();
			BME280_Config(&bmeInit);

    uart_send_string("BME280 inicializado.\r\n");
					BME280_Data_t bmeData;
    while (1) {
        procesar_gps(gps_buffer, &gps_point, &gps_time);

        
        BME280Calculation(&bmeData);

        uart_send_string("\r\n------ DATOS Satelite\r\n");
        char time_str[16];
        sprintf(time_str, "%02d:%02d:%02d UTC", gps_time.hour, gps_time.minute, gps_time.second);
        uart_send_string(time_str);
        uart_send_string("\r\n");

        uart_send_string("Lat: ");
        uart_send_double(gps_point.latitude);
        uart_send_string(" Lon: ");
        uart_send_double(gps_point.longitude);
        uart_send_string(" Vel: ");
        uart_send_double(gps_point.speed);
        uart_send_string(" m/s\r\n");

        print_bme280_data(&bmeData);

        delay_ms(1000);
    }
}
