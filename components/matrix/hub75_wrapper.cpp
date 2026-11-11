#include <stdio.h>
#include "hub75_wrapper.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "driver/gpio.h"
#include "TetrisMatrixDraw.h"
#include "fontmatrix.h"


// Definizione Pin 
#define R1_PIN 42
#define G1_PIN 40
#define B1_PIN 41
#define R2_PIN 38
#define G2_PIN 37
#define B2_PIN 39

#define A_PIN  45
#define B_PIN  36
#define C_PIN  48
#define D_PIN  35
#define E_PIN  21 

#define CLK_PIN   2
#define LAT_PIN   47
#define OE_PIN    14

#define BUTTON_A (gpio_num_t)6
#define BUTTON_B (gpio_num_t)7

// Istanza della matrice (globale o statica in questo file)
MatrixPanel_I2S_DMA *display = nullptr;
TetrisMatrixDraw *tetris = nullptr;

extern "C" void matrix_init() {
    // 1. Configurazione Matrix DMA
    HUB75_I2S_CFG mxconfig(64, 32, 1);

    mxconfig.clkphase = false; 
    
    // Mappatura manuale dei pin
    mxconfig.gpio.r1 = R1_PIN; mxconfig.gpio.g1 = G1_PIN; mxconfig.gpio.b1 = B1_PIN;
    mxconfig.gpio.r2 = R2_PIN; mxconfig.gpio.g2 = G2_PIN; mxconfig.gpio.b2 = B2_PIN;
    mxconfig.gpio.a = A_PIN; mxconfig.gpio.b = B_PIN; mxconfig.gpio.c = C_PIN; mxconfig.gpio.d = D_PIN;
    mxconfig.gpio.e = E_PIN;
    mxconfig.gpio.clk = CLK_PIN; mxconfig.gpio.lat = LAT_PIN; mxconfig.gpio.oe = OE_PIN;

    // 2. Inizializzazione
    display = new MatrixPanel_I2S_DMA(mxconfig);
    if (display->begin()) {
        printf("Matrice inizializzata correttamente!\n");
    }

}

extern "C" void drawCustomChar(int x, int y, char c, int r, int g, int b) {
    if (c < 32 || c > 126) return; // Fuori range ASCII gestito

    uint8_t brightness = 125;
    uint8_t sr = (r * brightness) >> 8;
    uint8_t sg = (g * brightness) >> 8;
    uint8_t sb = (b * brightness) >> 8;

    int index = c - 32; // Calcola l'indice partendo dallo spazio

    for (int col = 0; col < 5; col++) {
        uint8_t colData = font5x7[index][col]; // Prendo l'intera colonna (8 bit)
        
        for (int row = 0; row < 8; row++) {
            // Se il bit alla posizione 'row' è 1, accendi il pixel
            if (colData & (1 << row)) {
                display->drawPixel(x + col, y + row, display->color565(sr, sg, sb));
            }
        }
    }
}

// Funzione per stampare una stringa intera
extern "C" void drawCustomText(int x, int y, const char* text, int r, int g, int b) {
    int currentX = x;
    while (*text) {
        drawCustomChar(currentX, y, *text, r, g, b);
        currentX += 6; // 5 pixel del carattere + 1 pixel di spazio
        text++;
    }
}

extern "C" void matrix_clear() {
    if(display) display->fillScreen(0);
}

extern "C" void matrix_set_pixel(int x, int y, int r, int g, int b) {
    if(display) display->drawPixel(x, y, display->color565(r, g, b));
}

extern "C" void wallstreet_text(const char* text, const int x, int y) {

    //da sistemare il multiline la luminosità del testo
    static int scroll_x;
    static int first_call = 1;

    if(first_call){
        scroll_x = x;
        first_call = 0;
    }
    
    int len = strlen(text);
    int char_space = len - 1;
    int text_len = (len * 5) + char_space;

    drawCustomText(scroll_x, y, text, 0, 255, 0);

    drawCustomText(scroll_x + text_len, y, text, 0, 255, 0);

    scroll_x--;

    if(scroll_x <= -(text_len)) scroll_x = 0;

}

extern "C" void tetris_init() {
    if(display){
        tetris = new TetrisMatrixDraw(*display);
        tetris->scale = 1;
    }
    else{
        printf("Errore nell init tetris, panel non inizializzato!\n");
    }
}

extern "C" void tetris_timeset() {
    char tm[] = "20:39";
    tetris->setTime(tm);
}

extern "C" void tetris_textset() {
    char testo[] = "NASDAQ30%";
    tetris->setText(testo, false);
}

extern "C" bool tetris_text() {
    bool animazione_finita = tetris->drawText(0,25);  
    return animazione_finita;
}

extern "C" bool tetris_clock() {
    // draw number quando finisce l animazione ritorna true 
    bool animazione_finita = tetris->drawNumbers(0, 25, false); 
    printf("fine call tetris! %d\n", animazione_finita);
    return animazione_finita;
}

