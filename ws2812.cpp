#include "ws2812.h"

static uint32_t __str2short(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
static unsigned char __asc2hex(unsigned char asccode);

const uint32_t colors[10] = {
    Adafruit_NeoPixel::Color(255,0,0),  //ºì
    Adafruit_NeoPixel::Color(255,165,10),  //³ÈÉ«
    Adafruit_NeoPixel::Color(255,255,0), //»Æ
    Adafruit_NeoPixel::Color(0,255,0), //ÂÌ
    Adafruit_NeoPixel::Color(0,255,255),  //Çà
    Adafruit_NeoPixel::Color(0,0,255),  //À¶
    Adafruit_NeoPixel::Color(139,0,255), //×Ï
    Adafruit_NeoPixel::Color(255,255,255), //°×
    Adafruit_NeoPixel::Color(192,192,192), //»Ò
    Adafruit_NeoPixel::Color(0,0,0)   //ºÚ
};

WS2812::WS2812(uint16_t n, uint16_t pin = 6, neoPixelType type = NEO_GRB + NEO_KHZ800)
    :  Adafruit_NeoPixel(n, pin, type)
{
    ctl_pin = pin;
    led_num = n;
}

WS2812::~WS2812()
{
}

void WS2812::ws2812_fill(uint32_t color)
{
    fill(color, 0, led_num);
    show();
}

void WS2812::ws2812_fill(uint32_t color, uint32_t interinterval)
{
    uint16_t i = 0;
    for (i = 0; i < led_num; i++)
    {
        setPixelColor(i, color);                       //  Set pixel's color (in RAM)
        show();                                        //  Update strip to match
        delay(interinterval);                          //  Pause for a moment
    }
}

void WS2812::ws2812_fill(const uint32_t *color, uint16_t n)
{
    for (uint16_t i = 0; i < led_num; i++)
    {
        setPixelColor(i, color[i % n]);                //  Set pixel's color (in RAM)
        show();                                        //  Update strip to match
    }
}

void WS2812::ws2812_fill(const uint32_t *color, uint16_t n, uint32_t interinterval)
{
    for (uint16_t i = 0; i < led_num; i++)
    {
        setPixelColor(i, color[i % n]);                //  Set pixel's color (in RAM)
        show();                                        //  Update strip to match
        delay(interinterval);                          //  Pause for a moment
    }
}

void WS2812::ws2812_rainbow(void)
{
    for (int i = 0; i < led_num; i++)
    {
        setPixelColor(i, colors[i % 7]);
    }
    show();
}

void WS2812::ws2812_data_ctl(const unsigned char value[], uint16_t length)
{
    uint16_t h, s, v;
    unsigned char r, g, b;
    uint16_t hue;
    unsigned char sat, val;

    uint32_t c = 0;

    if (length < 13)
    {
        return;
    }

    h = __str2short(__asc2hex(value[1]), __asc2hex(value[2]), __asc2hex(value[3]), __asc2hex(value[4]));
    s = __str2short(__asc2hex(value[5]), __asc2hex(value[6]), __asc2hex(value[7]), __asc2hex(value[8]));
    v = __str2short(__asc2hex(value[9]), __asc2hex(value[10]), __asc2hex(value[11]), __asc2hex(value[12]));

    hue = h * 182;
    sat = s / 4;
    val = v / 4;
    c = gamma32(ColorHSV(hue, sat, val)); // hue -> RGB

    ws2812_fill(c);
}

/**
 * @brief  str to short
 * @param[in] {a} Single Point
 * @param[in] {b} Single Point
 * @param[in] {c} Single Point
 * @param[in] {d} Single Point
 * @return Integrated value
 * @note   Null
 */
static uint32_t __str2short(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    return (a << 12) | (b << 8) | (c << 4) | (d & 0xf);
}

/**
  * @brief ASCALL to Hex
  * @param[in] {asccode} µ±Ç°ASCALLÖµ
  * @return Corresponding value
  * @retval None
  */
static unsigned char __asc2hex(unsigned char asccode)
{
    unsigned char ret;

    if ('0' <= asccode && asccode <= '9')
        ret = asccode - '0';
    else if ('a' <= asccode && asccode <= 'f')
        ret = asccode - 'a' + 10;
    else if ('A' <= asccode && asccode <= 'F')
        ret = asccode - 'A' + 10;
    else
        ret = 0;

    return ret;
}
