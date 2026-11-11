#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_manager.h"
#include "esp_server.h"
#include "esp_log.h"
#include "hub75_wrapper.h"
#include "stockprice.h"


bool ap_mode = false;
bool tetris_animation_done = false;


static void wifi_credential_handler(const char *ssid, const char *password){

    ESP_LOGI("MAIN", "CALLBACK ATTIVATA NEL MAIN");
    ESP_LOGI("MAIN", "SSID aggiornato %s", ssid);
    ESP_LOGI("MAIN", "PASSWORD aggiornata %s", password);

    //wifi_stop();
    vTaskDelay(pdMS_TO_TICKS(5000));
    //wifi_init_sta(ssid, password);     
    //ap_mode = false;
}


void main_task(){
    matrix_init();
    tetris_init();
    tetris_textset();

    //tetris_timeset();
    //draw_triangle(5, 30, 10 , 255, 0, 0, false);

    int total_data = (sizeof(eurusd) / 4) / 4;

    int i;
    float max = eurusd[0][0], min = eurusd[0][0];

    for(i = 0; i < total_data; i++){
        float hight = eurusd[i][1];
        float low = eurusd[i][2];

        if(hight > max)
            max = hight;

        if(low < min)
            min = low;
    }

    float range = max - min;

    for(i = 0; i < total_data; i++){
        float close = eurusd[i][3];
        float normalized_value = (close - min) / range;
        //printf("normalized_value: %f\n", normalized_value);
        int y = normalized_value * 15;
        //printf("y: %d\n",y);
        //for(j = y; j >= 0; j--)
        matrix_set_pixel(i, y, 0, 255, 0);
    }

    
    while(1){
        
        //matrix_clear();
        //pulizia solo di una specifica area
        for(int x = 0; x < 64; x++)
        {
            for(int y = 22; y < 32; y ++)
            {
                matrix_set_pixel(x, y, 0, 0, 0);
            }

        }
        //i simboli > < sono stati rimappati per fare le frecce in stile wall street
        //accetta solo caratteri maiuscoli
        //deve avere uno spazio finale 
        char testo[] = "NASDAQ 3%> EURUSD 2%< XAUUSD 1%> ";

        //wallstreet_text(testo, 64, 2);
        wallstreet_text(testo, 32, 24);
        //wallstreet_text(testo, 0, 22);

        for(i = 0; i < total_data; i++){
            float close = eurusd[i][3];
            float normalized_value = (close - min) / range;
            //printf("normalized_value: %f\n", normalized_value);
            int y = normalized_value * 15;
            //printf("y: %d\n",y);
            //for(j = y; j >= 0; j--)
            matrix_set_pixel(i, y, 0, 255, 0);
        }

        //if(!tetris_animation_done){
        //    matrix_clear();
        //    tetris_animation_done = tetris_text(); //tetris_clock();
        //}
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void app_main(void) {

    wifi_prepare();
    wifi_init_softap();
    vTaskDelay(pdMS_TO_TICKS(1000));
    ap_mode = true;

    start_webserver();
    set_wifi_callback(wifi_credential_handler);
    TaskHandle_t maintask_handle = NULL;
    xTaskCreate(main_task, "main_task", 4092, NULL, 5, &maintask_handle);

}