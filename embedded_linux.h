#include <stdio.h>

#define INPUT  0
#define OUTPUT 1
#define LOW    0
#define HIGH   1
#define GPIO_LED01 67
#define GPIO_LEDAC 68

void init_gpio(unsigned int , unsigned int);
void set_gpio_high(unsigned int );
void set_gpio_low(unsigned int );
int get_value(unsigned int);
void init_analog_pins();
int get_value_ain(unsigned int ain);
void init_pins();