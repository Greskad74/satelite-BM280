#include <stm32f10x.h>
#include "delay.h"
#include "timers.h"
int pos;

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void servo_write(uint8_t angle){
	if(angle<0){angle=0;}
	if(angle>180){angle=180;}
	TIM3->CCR1=map (angle,0,180,3273,6545);
}

void servo2(){
	
while(1)
{
for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo_write(pos);              // tell servo to go to position in variable 'pos'
    delay_ms(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    servo_write(pos);              // tell servo to go to position in variable 'pos'
    delay_ms(15);                       // waits 15ms for the servo to reach the position
  }
}
}

void servo(void){
	long pos;
	
	for (pos = 3273; pos < 6545;pos++){
		TIM3->CCR1 = pos;
		delay_ms(15);
	}
	for (pos = 6545; pos < 3273;pos--){
		TIM3->CCR1 = pos;
		delay_ms(15);
	}
}