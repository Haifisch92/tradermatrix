#ifndef HUB75_WRAPPER_H
#define HUB75_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Qui dichiari solo funzioni C "pure"
void matrix_init();
void matrix_set_pixel(int x, int y, int r, int g, int b);
void matrix_clear();
void matrix_update_text(const char* text);

void wallstreet_text(const char* text, const int x, int y);

void drawCustomText(int x, int y, const char* text, int r, int g, int b);
void drawCustomChar(int x, int y, char c, int r, int g, int b);

void tetris_init();
void tetris_timeset();
void tetris_textset();
bool tetris_text();
bool tetris_clock();

#ifdef __cplusplus
}
#endif

#endif