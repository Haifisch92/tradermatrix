#ifndef ESP_SERVER_H_
#define ESP_SERVER_H_

#include "esp_http_server.h"

typedef void (*wifi_credentials_callback_t)(const char *ssid, const char *pawwsord);

void set_wifi_callback(wifi_credentials_callback_t callback);
// Avvia il server HTTP
void start_webserver();

// Ferma il server HTTP
void stop_webserver();

// GLOBAL VARIABLE CLOCK
extern char *time_clock;
extern char *date_clock;

#endif

