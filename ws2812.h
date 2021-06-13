#ifndef __WS2812_h__
#define __WS2812_H__

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>    // Required for 16 MHz Adafruit Trinket
#endif

extern const uint32_t colors[10];
enum
{
    RED_INDEX = 0,
    ORANGE_INDEX,
    YELLOW_INDEX,
    GREEN_INDEX,
    CYAN_INDEX,
    BLUE_INDEX,
    PURPLE_INDEX,
    WHITE_INDEX,
    GRAY_INDEX,
    BLACK_INDEX,
    COLOR_INDEX_MAX_LEN
};


class WS2812 : public Adafruit_NeoPixel
{
    public:
        WS2812(uint16_t n, uint16_t pin = 6, neoPixelType type = NEO_GRB + NEO_KHZ800);
        ~WS2812();
        void ws2812_fill(uint32_t color);
        void ws2812_fill(uint32_t color, uint32_t interinterval);
        void ws2812_fill(const uint32_t *color, uint16_t n);
        void ws2812_fill(const uint32_t *color, uint16_t n, uint32_t interinterval);
        void ws2812_rainbow();
        void ws2812_data_ctl(const unsigned char value[], uint16_t length);
    public:

    private:
        uint16_t ctl_pin;
        uint16_t led_num;
};

#endif  /* __WS2812_h__ */
