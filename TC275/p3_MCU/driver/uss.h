#ifndef USS_H_
#define USS_H_

void ultrasonic_init(void);
int measure_ultrasonic_distance(void);
void trigger_ultrasonic_pulse(void);

void delay_us(uint32 microseconds);
void delay_ms(uint32 milliseconds);

#endif /* USS_H_ */
