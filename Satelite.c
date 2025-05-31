#include "Satelite.h"
#include <string.h>
#include <stdlib.h>
#include "usart.h"

double nmea_to_decimal(double nmea, char hemisphere) {
    int degrees = (int)(nmea / 100);
    double minutes = nmea - (degrees * 100);
    double decimal = degrees + (minutes / 60.0);
    if (hemisphere == 'S' || hemisphere == 'W') decimal = -decimal;
    return decimal;
}

bool parse_gprmc(const char *nmea, gps_point_t *point, GPS_Time_t *time) {
    if (strncmp(nmea, "$GPRMC", 6) != 0) return false;
    char buffer[128];
    strncpy(buffer, nmea, sizeof(buffer));
    buffer[sizeof(buffer)-1] = '\0';

    char *token = strtok(buffer, ",");
    int field = 0;
    double lat=0, lon=0, speed=0, course=0;
    char lat_hem='N', lon_hem='E';
    while (token != NULL) {
        switch (field) {
            case 1: {
                if (strlen(token)>=6) {
                    char hh[3]={token[0],token[1],0}, mm[3]={token[2],token[3],0}, ss[3]={token[4],token[5],0};
                    time->hour=atoi(hh); time->minute=atoi(mm); time->second=atoi(ss);
                }
            } break;
            case 3: lat=atof(token); break;
            case 4: lat_hem=token[0]; break;
            case 5: lon=atof(token); break;
            case 6: lon_hem=token[0]; break;
            case 7: speed=atof(token); break;
            case 8: course=atof(token); break;
        }
        token = strtok(NULL, ",");
        field++;
    }
    point->latitude=nmea_to_decimal(lat, lat_hem);
    point->longitude=nmea_to_decimal(lon, lon_hem);
    point->speed=speed*0.514444;
    point->course=course;
    return true;
}

void procesar_gps(char *buffer, gps_point_t *point, GPS_Time_t *time) {
    char *line = strchr(buffer, '$');
    if (line && strstr(line, "$GPRMC")) {
        if (parse_gprmc(line, point, time)) {
            // ok
        }
    }
}
