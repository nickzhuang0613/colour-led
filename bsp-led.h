#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include <stdint.h>

class LED
{
    public:
        LED(uint8_t pin = 13);
        ~LED();
        void led_on(void);
        void led_off(void);
        void led_toggle(void);
    public:
        bool led_status;
    private:
        uint8_t led_pin;
};

#endif  /* __BSP_LED_H__ */
