#pragma once
 
#include <stdint.h>
 
void *(vg_init)(uint16_t mode);

int (map_vram)(uint16_t mode);

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
 
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
 
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
 
int (vg_display_vbe_contr_info)(struct vg_vbe_contr_info *info_p);

int (kbd_loop)();

uint32_t (R)(uint32_t first);

uint32_t (G)(uint32_t first);

uint32_t (B)(uint32_t first);

int (print_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);
