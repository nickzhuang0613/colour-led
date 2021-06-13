#include <TuyaWifi.h>
#include <stdint.h>
#include "bsp-led.h"
#include "ws2812.h"
#include <SoftwareSerial.h>

LED led;

#define WS2812_DIN SCL
#define WS2812_NUM 6
// Declare our NeoPixel strip object:
WS2812 led_group(WS2812_NUM, WS2812_DIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

#define DPID_NUM                    6
#define DPID_SWITCH_LED             20   /* Bool RW   */
#define DPID_WORK_MODE              21   /* Enum RW   */
#define DPID_COUNTDOWN              26   /* Value RW  */
#define DPID_MUSIC_DATA             27   /* String WR */
#define DPID_CONTROL_DATA           28   /* String WR */
#define DPID_DREAMLIGHT_SCENE_MODE  51   /* Raw RW    */
unsigned char dp_array[][2] = {
    { DPID_SWITCH_LED, DP_TYPE_BOOL },
    { DPID_WORK_MODE, DP_TYPE_ENUM },
    { DPID_COUNTDOWN, DP_TYPE_VALUE },
    { DPID_MUSIC_DATA, DP_TYPE_STRING },
    { DPID_CONTROL_DATA, DP_TYPE_STRING },
    { DPID_DREAMLIGHT_SCENE_MODE, DP_TYPE_RAW }
};
TuyaWifi my_device;
unsigned char pid[] = { "自己在涂鸦IOT平台创建幻彩灯带产品活动的PID" };
unsigned char mcu_ver[] = { "1.0.0" };

unsigned char dp_bool_value       = 0;
long dp_value_value               = 0; 
unsigned char dp_enum_value       = 0;
unsigned char dp_string_value[128] = {"Hi,Tuya"};
unsigned char dp_raw_value[128]    = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};


static unsigned char dp_process(unsigned char dpid, const unsigned char value[], unsigned short length);
static void dp_update_all(void);

SoftwareSerial DebugSerial(8, 9);

uint32_t time_out_ms = 0;
uint32_t time_out_s = 0;
uint32_t time_out_cmp = 0;

unsigned char dynamic = 0;
unsigned char dynamic_index = 0;
unsigned char water = 0;
unsigned char water_index = 0;
uint32_t color_temp[WS2812_NUM];

uint16_t cnt = 0;

void setup()
{
    Serial.begin(9600);
    DebugSerial.begin(9600);
    //incoming all DPs and their types array, DP numbers
    //Enter the PID and MCU software version
    my_device.init(pid, mcu_ver);
    my_device.set_dp_cmd_total(dp_array, DPID_NUM);
    //register DP download processing callback function
    my_device.dp_process_func_register(dp_process);
    //register upload all DP callback function
    my_device.dp_update_all_func_register(dp_update_all);
    my_device.mcu_set_wifi_mode(SMART_CONFIG);
    led_group.begin();    // Initialize NeoPixel strip object (REQUIRED)
    led_group.show();     // Initialize all pixels to 'off'
}


void loop()
{
    my_device.uart_service();
    delay(10);

    ++cnt;
    if (cnt == 10)
    {
        if (dynamic)
        {
            for (cnt = 0; cnt < WS2812_NUM; cnt++)
            {
                color_temp[cnt] = colors[dynamic_index % (COLOR_INDEX_MAX_LEN - 1)];
            }
            ++dynamic_index;
            led_group.ws2812_fill(color_temp, WS2812_NUM);
        }

        if (water)
        {
            led_group.setPixelColor(water_index % WS2812_NUM, colors[water_index % (COLOR_INDEX_MAX_LEN - 1)]); 
            led_group.show(); 
            ++water_index;
        }

        cnt = 0;
    }

    if (time_out_ms)
    {
        ++time_out_ms;
        if (time_out_ms == 101)
        {
            time_out_ms = 1;
            ++time_out_s;
            if (time_out_s >= time_out_cmp)
            {
                time_out_ms = 0;
                time_out_s = 0;
                time_out_cmp = 0;
                led.led_off();
                led_group.ws2812_fill(colors[BLACK_INDEX]);
                my_device.mcu_dp_update(DPID_SWITCH_LED, 0, 1);
                my_device.mcu_dp_update(DPID_COUNTDOWN, 0, 1);
            }
        }
    }
    else
    {
        time_out_s = 0;
        time_out_cmp = 0;
    }
}

/* cbu驱动 */
static unsigned char dp_process(unsigned char dpid, const unsigned char value[], unsigned short length)
{
    dynamic = 0;
    water = 0;
    switch (dpid)
    {
        case DPID_SWITCH_LED:
        {
            dp_bool_value = my_device.mcu_get_dp_download_data(dpid, value, length);
            if (dp_bool_value)
            {
                led.led_on();
                led_group.ws2812_fill(colors[WHITE_INDEX]);
            }
            else
            {
                led.led_off();
                led_group.ws2812_fill(colors[BLACK_INDEX]);
                if (time_out_ms)
                {
                    time_out_ms = 0;
                    time_out_s = 0;
                    my_device.mcu_dp_update(DPID_COUNTDOWN, 0, 1);
                }
            }
            my_device.mcu_dp_update(dpid, dp_bool_value, length);
            break;
        }
        case DPID_WORK_MODE:
        {
            uint8_t dp_enum_value = my_device.mcu_get_dp_download_data(dpid, value, length);
            switch (dp_enum_value)
            {
                case 0: // scene mode
                {
                    led_group.ws2812_fill(colors[GRAY_INDEX]);
                    break;
                }
                case 1: // music mode
                {
                    led_group.ws2812_fill(colors, 6, 50);
                    break;
                }
            }
            my_device.mcu_dp_update(dpid, dp_enum_value, length);
            break;
        }
        case DPID_COUNTDOWN:
        {
            dp_value_value = my_device.mcu_get_dp_download_data(dpid, value, length);
            if (dp_value_value)
            {
                time_out_cmp = dp_value_value;
                time_out_ms = 1;
                time_out_s = 0;
            }
            else
            {
                time_out_cmp = 0;
                time_out_ms = 0;
                time_out_s = 0;
            }
            my_device.mcu_dp_update(dpid, dp_value_value, length);
            break;
        }
        case DPID_MUSIC_DATA:
        {
            int i = 0;
            for (i = 0; i < length; i++)
            {
                dp_string_value[i] = value[i];
            }
            led_group.ws2812_data_ctl(dp_string_value, length);
            my_device.mcu_dp_update(dpid, dp_string_value, length);
            break;
        }
        case DPID_CONTROL_DATA:
        {
            int i = 0;
            for (i = 0; i < length; i++)
            {
                dp_string_value[i] = value[i];
            }
            led_group.ws2812_data_ctl(dp_string_value, length);
            my_device.mcu_dp_update(dpid, dp_string_value, length);
            break;
        }
        case DPID_DREAMLIGHT_SCENE_MODE:
        {
            uint8_t scene_mode = value[1];  /* 直接根据情景设置灯光 */
            switch (scene_mode)
            {
                case 0:        /* 电影 */
                {
                    led_group.ws2812_fill(led_group.Color(255, 48, 48));
                    break;
                }
                case 1:        /* 约会 */
                {
                    led_group.ws2812_fill(led_group.Color(255, 192, 203));
                    break;
                }
                case 2:        /* 晚霞 */
                {
                    led_group.ws2812_fill(led_group.Color(255, 132, 9));
                    break;
                }
                case 3:        /* 圣诞夜 */
                {
                    led_group.ws2812_fill(led_group.Color(204, 124, 60));
                    break;
                }
                case 4:        /* 浪漫 */
                {
                    led_group.ws2812_fill(led_group.Color(176, 196, 162));
                    break;
                }
                case 5:        /* 缤纷 */
                {
                    led_group.ws2812_fill(led_group.Color(166, 130, 170));
                    break;
                }
                case 6:        /* 动感 */
                {
                    dynamic = 1;
                    break;
                }
                case 7:        /* 梦幻 */
                {
                    led_group.ws2812_fill(led_group.Color(199, 101, 135));
                    break;
                }
                case 8:        /* 彩虹 */
                {
                    led_group.ws2812_rainbow();
                    break;
                }
                case 9:        /* 流水 */
                {
                    water = 1;
                    break;
                }
            }
            my_device.mcu_dp_update(dpid, value, length);
            break;
        }
    }
    return 1;
}

static void dp_update_all(void)
{
    my_device.mcu_dp_update(DPID_SWITCH_LED, (unsigned char)led.led_status, 1);
}
