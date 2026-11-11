idf_component_register(SRCS "wifi_manager.c"
                       REQUIRES spi_flash esp_event nvs_flash esp_netif esp_wifi
                       INCLUDE_DIRS ".")
