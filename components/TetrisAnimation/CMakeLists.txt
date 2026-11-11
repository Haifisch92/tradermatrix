idf_component_register(SRCS "src/TetrisMatrixDraw.cpp"
                       INCLUDE_DIRS "src"
                       REQUIRES esp_timer        # Dipendenza interna a ESP-IDF
                       PRIV_REQUIRES ESP32-HUB75-MatrixPanel-DMA) # Dipendenza da un altro tuo componente