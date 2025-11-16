#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

//#define FRIED_ESP_CYD_SCREEN_DEMO
//#define FRIED_ESP_CYD_SCREEN_DEBUG
#include "fried_esp_cyd_screen.h"
#include "ui.h"


#include "esp_log.h"
//#include "fried_tradfri_client.h"
//#include "fdata.h"

static const char* TRADFRI_TAG = "FRIED_DISPLAY_TRADFRI_CONTROLLER";


//static void CycleColorsXY(const char* deviceId)
//{
//    struct { uint16_t x, y; } colors[] = {
//        { 45860, 19660 }, // red
//        { 19660, 39321 }, // green
//        { 9830,  6554  }, // blue
//        { 32768, 32768 }  // white
//    };
//
//    const int color_count = sizeof(colors) / sizeof(colors[0]);
//
//    while (1)
//    {
//        for (int i = 0; i < color_count; i++)
//        {
//            SetColorTradfriLamp(deviceId, colors[i].x, colors[i].y);
//            printf("Set XY to %u %u\n", colors[i].x, colors[i].y);
//            vTaskDelay(pdMS_TO_TICKS(3000));
//        }
//    }
//}

//void my_button_cb(lv_event_t* e) {
//    printf("Button clicked!\n");
//}

static void on_click(lv_event_t* e)
{
    printf("You clicked me :3\n");
	fs_set_brightness(75);
}

void ballsFunction(lv_event_t* e)
{
    printf("balls function called! :3\n");
    fs_set_brightness(80);
}


static void controller_main(void *p)
{
    ////fdata data = f_create();
    ////f_set_str(data, "key", "value");
    ////char* json = f_serialize(data);
    //const char* deviceId = "65574";
    ////char* info = GetTradfriDeviceInfo(deviceId);
    ////printf("info :\n%s\n", info);
	//
    ////vTaskDelay(pdMS_TO_TICKS(1000));
    //CycleColorsXY(deviceId);
	while(true)
	{
		printf("hello world!");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

    ESP_LOGI(TRADFRI_TAG, "Finished");
    vTaskDelete(NULL);
}

void app_main(void)
{
    fs_init();
	fs_set_brightness(5);
	
	//squareline studio ui init func
	ui_init();
	//fs_new_button(50, 50, 120, 40, "Press Me", on_click);

    while (1) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    // Create a button at (50,50) size 120x50
    //fried_screen_create_button(50, 50, 120, 50, "Press me", my_button_cb);

    //tradfri_init("192.168.2.9", "tradfri_12345", "IaY5AQRXw1awfqEt");

    //xTaskCreate(controller_main, "controller", 8 * 1024, NULL, 5, NULL);
}
