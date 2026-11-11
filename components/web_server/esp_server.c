#include "esp_server.h"
#include "esp_log.h"
#include "html_page.h"
#include "cJSON.h"
#include <string.h>

static const char *TAG = "HTTP_SERVER";
httpd_handle_t http_server = NULL;

static wifi_credentials_callback_t wifi_credential = NULL;

// funzione per registrare la callback della wifi
void set_wifi_callback(wifi_credentials_callback_t callback)
{
    wifi_credential = callback;
}


// GLOBAL VARIABLE CLOCK
char *time_clock = NULL;
char *date_clock = NULL;


/* HANDLER ROOT "/" */
static esp_err_t root_handler(httpd_req_t *req) {
  
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    httpd_resp_send(req, (const char *)index_html_gz, index_html_gz_len);
      
    return ESP_OK;
}


int jsondata_parser(char *buf){
    ESP_LOGI(TAG, "Json:  %s", buf);
    cJSON *json = cJSON_Parse(buf);
    // Converte in stringa JSON
    char *json_string = cJSON_PrintUnformatted(json);

    if (json) {

        // WIFI
        cJSON *wifi = cJSON_GetObjectItem(json, "wifi");
        if(!cJSON_IsObject(wifi)) {
            ESP_LOGI(TAG, "Json parse: wifi non trovato");
            return 0;
        }

        cJSON *wifi_enbl = cJSON_GetObjectItem(wifi, "enabled");
        cJSON *wifi_ssid = cJSON_GetObjectItem(wifi, "ssid");
        cJSON *wifi_ipv4 = cJSON_GetObjectItem(wifi, "ipv4");
        cJSON *wifi_password = cJSON_GetObjectItem(wifi, "password");

        //if(wifi_enbl && cJSON_IsBool(wifi_enbl)){
        //    wifi_enabled = cJSON_IsTrue(wifi_enbl);
        //}

        bool valid_ssid = wifi_ssid && cJSON_IsString(wifi_ssid) && strcmp(wifi_ssid->valuestring, "") != 0;
        bool valid_pswd = wifi_password && cJSON_IsString(wifi_password) && strcmp(wifi_password->valuestring, "") != 0;

        if(valid_ssid && valid_pswd){
            if(wifi_credential != NULL){
                wifi_credential(wifi_ssid->valuestring, wifi_password->valuestring);
            }else{
                ESP_LOGI(TAG, "Nessuna callback registrata per le credenziali wifi!!"); 
            }
        }

        //if(wifi_ipv4 && cJSON_IsString(wifi_ipv4) && strcmp(wifi_ipv4->valuestring, "") != 0) {
        //    ESP_LOGI(TAG, "ipv4 aggiornato %s", ipv4);
        //}


        // CLOCK
        cJSON *clock = cJSON_GetObjectItem(json, "clock");
        if(!cJSON_IsObject(clock)) {
            ESP_LOGI(TAG, "Json parse: clock non trovato");
            return 0;
        }

        cJSON *clock_time = cJSON_GetObjectItem(clock, "time");
        cJSON *clock_date = cJSON_GetObjectItem(clock, "date");

        if(clock_time && cJSON_IsString(clock_time) && strcmp(clock_time->valuestring, "") != 0) {
            if(time_clock != NULL){
                free(time_clock);
                time_clock = NULL;
            }
            time_clock = malloc(strlen(clock_time->valuestring) + 1);
            strcpy(time_clock, clock_time->valuestring);
            ESP_LOGI(TAG, "time aggiornato %s", time_clock);
        }

        if(clock_date && cJSON_IsString(clock_date) && strcmp(clock_date->valuestring, "") != 0) {
            if(date_clock != NULL){
                free(date_clock);
                date_clock = NULL;
            }
            date_clock = malloc(strlen(clock_date->valuestring) + 1);
            strcpy(date_clock, clock_date->valuestring);
            ESP_LOGI(TAG, "date aggiornato %s", date_clock);
        }

        cJSON_Delete(json);
    }

    return 1;
}

/*SUBMIT HANDLER WITH POST METHOD*/
esp_err_t submit_handler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    int received = 0;

    // Alloca buffer per il body (+1 per il terminatore)
    char *buf = malloc(total_len + 1);
    if (!buf) {
        ESP_LOGE("HTTP", "Malloc fallita per %d bytes", total_len);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memoria insufficiente");
        return ESP_FAIL;
    }

    // Ricezione a blocchi
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len - cur_len);
        if (received <= 0) {
            free(buf);
            ESP_LOGE(TAG, "Errore nella ricezione del body");
            return ESP_FAIL;
        }
        cur_len += received;
    }

    buf[total_len] = '\0';   // Termina la stringa

    jsondata_parser(buf);
    
    // Risposta
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"status\":\"ok\"}");

    free(buf);
    return ESP_OK;
}




static const httpd_uri_t uri_root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_handler
};

static const httpd_uri_t uri_submit = {
    .uri       = "/submit",
    .method    = HTTP_POST,
    .handler   = submit_handler,
    .user_ctx  = NULL
};




void start_webserver() {

    // Server principale
    httpd_config_t config_main = HTTPD_DEFAULT_CONFIG();
    ESP_LOGI(TAG, "Avvio del server HTTP sulla porta: '%d'", config_main.server_port);

    if (httpd_start(&http_server, &config_main) == ESP_OK) {
        httpd_register_uri_handler(http_server, &uri_root);
        httpd_register_uri_handler(http_server, &uri_submit);
        ESP_LOGI(TAG, "Server principale avviato");
    } else {
        ESP_LOGE(TAG, "Errore nell'avvio del server principale!");
        return;
    }
}

/* STOP SERVER */
void stop_webserver()
{
    if (http_server) {
        httpd_stop(http_server);
        ESP_LOGI(TAG, "Server fermato");
    }
}
