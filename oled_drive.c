#include "oled_drive.h"
#include "i2c.h"

// Dirección del OLED (SSD1306)
#define OLED_ADDR 0x3C // Dirección de 7 bits (ya sin desplazar)


// Enviar comando de 1 byte
void oled_cmd_1byte(uint8_t data) {
  I2C1_Start();
  I2C1_Write(OLED_ADDR << 1 | 0);  // Dirección + escritura
  I2C1_Write(0x00);                // Control byte: Co=0, D/C#=0 (comando)
  I2C1_Write(data);                // El comando
  I2C1_Stop();
}

// Enviar comando de 2 bytes
void oled_cmd_2byte(const uint8_t data[2]) {
  I2C1_Start();
  I2C1_Write(OLED_ADDR << 1 | 0);  // Dirección + escritura
  I2C1_Write(0x00);                // Control byte
  I2C1_Write(data[0]);             // Primer byte
  I2C1_Write(data[1]);             // Segundo byte
  I2C1_Stop();
}

// Inicializar OLED
void oled_init(uint8_t screen_size) {
  // ?? Asegúrate de inicializar I2C1 antes de llamar a esto
  uint8_t cmd1[] = {0xA8, 0x3F};    oled_cmd_2byte(cmd1); // Multiplex
  uint8_t cmd2[] = {0xD3, 0x00};    oled_cmd_2byte(cmd2); // Offset
  oled_cmd_1byte(0x40);                              // Display Start Line
  oled_cmd_1byte(0xA1);                              // Segment remap
  oled_cmd_1byte(0xC8);                              // COM scan direction
  uint8_t cmd3[] = {0xDA, screen_size}; oled_cmd_2byte(cmd3); // COM pins
  uint8_t cmd4[] = {0x81, 0x7F};    oled_cmd_2byte(cmd4); // Contrast
  oled_cmd_1byte(0xA4);                              // Resume to RAM content
  oled_cmd_1byte(0xA6);                              // Normal display
  uint8_t cmd5[] = {0xD5, 0x80};    oled_cmd_2byte(cmd5); // Clock divide
  uint8_t cmd6[] = {0x8D, 0x14};    oled_cmd_2byte(cmd6); // Charge pump
  oled_cmd_1byte(0xAF);                              // Display ON
  uint8_t cmd7[] = {0x20, 0x10};    oled_cmd_2byte(cmd7); // Page addressing
}

void oled_init_64() {
  oled_init(0x12); // 64px screen
}

// Enviar datos (1 byte)
void oled_data(uint8_t data) {
  I2C1_Start();
  I2C1_Write(OLED_ADDR << 1 | 0);  // Dirección + escritura
  I2C1_Write(0x40);                // Control byte: D/C#=1 (datos)
  I2C1_Write(data);                // El dato
  I2C1_Stop();
}

// Posicionar cursor
void oled_pos(uint8_t Ypos, uint8_t Xpos) {
  oled_cmd_1byte(0x00 + (0x0F & Xpos));           // Bajo nibble columna
  oled_cmd_1byte(0x10 + (0x0F & (Xpos >> 4)));    // Alto nibble columna
  oled_cmd_1byte(0xB0 + Ypos);                    // Página Y
}

// Limpiar pantalla
void oled_blank() {
  for (uint8_t page = 0; page < 8; page++) {
    oled_pos(page, 0);
    for (uint8_t col = 0; col < 128; col++) {
      oled_data(0x00);
    }
  }
  oled_pos(0, 0);
}

// Imprimir texto usando tabla ASCII externa
extern const uint8_t ASCII[][5];

void oled_print(const char *str) {
  while (*str) {
    for (uint8_t j = 0; j < 5; j++) {
      oled_data(ASCII[*str - 32][j]);
    }
    oled_data(0x00); // Espacio entre caracteres
    str++;
  }
}

// Mostrar mensaje en posición (Ypos, Xpos)
void oled_msg(uint8_t Ypos, uint8_t Xpos, const char *str) {
  oled_pos(Ypos, Xpos);
  oled_print(str);
}
