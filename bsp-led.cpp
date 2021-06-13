#include "bsp-led.h"
#include <Arduino.h>

LED::LED(uint8_t pin = 13)
{
    pinMode(pin, OUTPUT);
    led_status = 0;
    led_pin = pin;
}

LED::~LED()
{
}

void LED::led_on(void) 
{
    led_status = true;
    digitalWrite(led_pin, HIGH);
}

void LED::led_off(void) 
{
    led_status = false;
    digitalWrite(led_pin, LOW);
}

void LED::led_toggle(void) 
{
    if (led_status == true) 
    {
        led_off();
    } 
    else 
    {
        led_on();
    }
}
