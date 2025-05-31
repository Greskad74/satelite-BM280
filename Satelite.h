#ifndef Satelite_H
#define Satelite_H

#include "stm32f10x.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "usart.h"


typedef struct {
    double latitude;
    double longitude;
    double speed;
    double course;
} gps_point_t;

typedef struct {
    int hour;
    int minute;
    int second;
} GPS_Time_t;

double nmea_to_decimal(double nmea, char hemisphere);
bool parse_gprmc(const char *nmea, gps_point_t *point, GPS_Time_t *time);
void procesar_gps(char *buffer, gps_point_t *point, GPS_Time_t *time);
#endif