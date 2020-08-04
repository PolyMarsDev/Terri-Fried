#ifndef __DRAW_H__
#define __DRAW_H__

#include <stdint.h>

void xbox_draw_init();

void xbox_clear_screen(uint32_t clr);

uint32_t *xbox_load_texture(const uint8_t texture_rgba[], int texture_width, int texture_height);
void xbox_free_texture(uint32_t *alloc_texture);

void xbox_draw_sprite(int xpos, int ypos, int width, int height, uint32_t *texture, int texture_width, int texture_height);
void xbox_draw_trajectory(int playerX, int playerY, double velocityX, double velocityY);

void xbox_text_init(int size);
void xbox_draw_text(const char *text, int x, int y, float scale, uint8_t r, uint8_t g, uint8_t b);
void xbox_text_free();

void xbox_draw_free();

void xbox_target_back_buffer();
void xbox_swap_buffers();

#endif
