#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "fried_esp_cyd_screen.h"
//export from squareline
#include "ui.h"


#include "esp_log.h"
#include "controller_queue.h"
//#include "fdata.h"

#include <math.h>
#include <stdint.h>

static const char* TRADFRI_TAG = "FRIED_DISPLAY_TRADFRI_CONTROLLER";
static uint8_t transitionTime = 5;



// approximate gamma correction (0..255 in, 0..255 out)
static inline float srgb_to_linear(uint8_t c)
{
    float v = (float)c / 255.0f;
    if (v <= 0.04045f) return v / 12.92f;
    return powf((v + 0.055f) / 1.055f, 2.4f);
}

// Convert RGB (0..255) to XY (0..65535) using standard sRGB -> XYZ matrix
void rgb_to_xy_uint16(uint16_t r_in, uint16_t g_in, uint16_t b_in, uint16_t* x, uint16_t* y)
{
    // clamp inputs to 0..255 just in case
    uint8_t r8 = (r_in > 255) ? 255 : (uint8_t)r_in;
    uint8_t g8 = (g_in > 255) ? 255 : (uint8_t)g_in;
    uint8_t b8 = (b_in > 255) ? 255 : (uint8_t)b_in;

    // linearize (gamma expand)
    float r = srgb_to_linear(r8);
    float g = srgb_to_linear(g8);
    float b = srgb_to_linear(b8);

    // sRGB D65 matrix to XYZ (wide gamut)
    // values from standard sRGB to XYZ conversion
    float X = r * 0.4124564f + g * 0.3575761f + b * 0.1804375f;
    float Y = r * 0.2126729f + g * 0.7151522f + b * 0.0721750f;
    float Z = r * 0.0193339f + g * 0.1191920f + b * 0.9503041f;

    float sum = X + Y + Z;
    if (sum <= 1e-6f) {
        // black / near-zero -> use middle gray-ish xy (prevent div by zero)
        *x = 32768;
        *y = 32768;
        return;
    }

    float xf = X / sum;
    float yf = Y / sum;

    // scale to 0..65535 and clamp
    int xi = (int)roundf(xf * 65535.0f);
    int yi = (int)roundf(yf * 65535.0f);

    if (xi < 0) { xi = 0; }
    else if (xi > 65535) { xi = 65535; }

    if (yi < 0) { yi = 0; }
    else if (yi > 65535) { yi = 65535; }


    *x = (uint16_t)xi;
    *y = (uint16_t)yi;
}

static void SetTradfriColor(uint8_t r, uint8_t g, uint8_t b, uint8_t t)
{
	const char* deviceId = "65574";
	uint16_t x,y;
	rgb_to_xy_uint16(r, g, b, &x, &y);
	enqueue_set_color(deviceId, x, y, t);
}
static void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t t)
{
	lv_obj_t* screen = lv_scr_act();
	
	lv_color_t color = lv_color_make(r, g, b);
	
    lv_obj_set_style_bg_color(screen, color, 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
	SetTradfriColor(r, g, b, t);
}
static void SetColorFromColor(lv_color_t col, uint8_t t)
{
	lv_obj_t* screen = lv_scr_act();
	
	uint32_t c32 = lv_color_to32(col);

    // Extract channels (AA RR GG BB)
    uint8_t r = (c32 >> 16) & 0xFF;
    uint8_t g = (c32 >> 8) & 0xFF;
    uint8_t b = (c32 >> 0) & 0xFF;
	
	SetColor(r,g,b, t);
}

//void colorWheelReleaseFunction(lv_event_t* e)
//{
//    lv_obj_t* cw = lv_event_get_target(e);
//
//    // RGB565 color from the wheel
//    lv_color_t color = lv_colorwheel_get_rgb(cw);
//    SetColorFromColor(color, transitionTime);
//}

void colorWheelFunction(lv_event_t* e)
{
    lv_obj_t* cw = lv_event_get_target(e);

    // RGB565 color from the wheel
    lv_color_t color = lv_colorwheel_get_rgb(cw);

	if (is_queue_empty())	
	{
		SetColorFromColor(color, transitionTime);  //1.5 roughly the duration that it takes for a call
	}
}

void sliderTimeFunction(lv_event_t* e)
{
    lv_obj_t* slider = lv_event_get_target(e);
    transitionTime = lv_slider_get_value(slider);
}

void sliderBrightnessFunction(lv_event_t* e)
{
    lv_obj_t* slider = lv_event_get_target(e);
    int brightness = lv_slider_get_value(slider);

	if (brightness < 1)
		brightness = 1;
	//otherwise the screen turns off and its hard to find the slider again lol
    fs_set_brightness(brightness);
}

void resetBackgroundButtonFunction(lv_event_t* e)
{
	SetColor(255, 255, 255, transitionTime);
}

void changeColorRed(lv_event_t* e)
{
	SetColor(255, 0, 0, transitionTime);
}
void changeColorGreen(lv_event_t* e)
{
	SetColor(0, 255, 0, transitionTime);
}
void changeColorBlue(lv_event_t* e)
{
	SetColor(0, 0, 255, transitionTime);
}



//static void controller_main(void *p)
//{
//	
//    ////fdata data = f_create();
//    ////f_set_str(data, "key", "value");
//    ////char* json = f_serialize(data);
//    //const char* deviceId = "65574";
//    ////char* info = GetTradfriDeviceInfo(deviceId);
//    ////printf("info :\n%s\n", info);
//	//
//    ////vTaskDelay(pdMS_TO_TICKS(1000));
//    //CycleColorsXY(deviceId);
//	while(true)
//	{
//		printf("hello world!");
//		vTaskDelay(pdMS_TO_TICKS(1000));
//	}
//
//    ESP_LOGI(TRADFRI_TAG, "Finished");
//    vTaskDelete(NULL);
//}

void app_main(void)
{
    fs_init();
	fs_set_brightness(25);
	
	//squareline studio ui init func
	ui_init();
	queue_init();
	//fs_new_button(50, 50, 120, 40, "Press Me", on_click);

    //while (1) {
    //    vTaskDelay(10 / portTICK_PERIOD_MS);
    //}

    // Create a button at (50,50) size 120x50
    //fried_screen_create_button(50, 50, 120, 50, "Press me", my_button_cb);

	
    //xTaskCreate(controller_main, "controller", 8 * 1024, NULL, 5, NULL);
}
