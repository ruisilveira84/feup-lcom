#pragma once

#include <lcom/lcf.h>
#include <stdint.h>
 
/**
 * @brief Initializes video mode
 * @param uint16_t* The mode to initialize
 * @return Pointer to the beggining of the screen memory
 */
void *(vg_init)(uint16_t mode);

/**
 * @brief Maps vram
 * @param uint16_t* The mode to initialize
 * @return 0 if successful, 1 if not
 */
int (map_vram)(uint16_t mode);

/**
 * @brief Draws a pixel in the buffer
 * @param uint16_t x coordinates
 * @param uint16_t y coordinates
 * @param uint32_t colour
 * @return 0 if successful, 1 if not
 */
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief Draws a pixel in the background buffer
 * @param uint16_t x coordinates
 * @param uint16_t y coordinates
 * @param uint32_t colour
 * @return 0 if successful, 1 if not
 */
int (vg_draw_pixel_to_background)(uint16_t x, uint16_t y, uint32_t color);
 
/**
 * @brief Draws a line in the buffer
 * @param uint16_t x coordinates
 * @param uint16_t y coordinates
 * @param uint16_t length
 * @param uint32_t colour
 * @return 0 if successful, 1 if not
 */
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
 
/**
 * @brief Draws a pixel
 * @param uint16_t x coordinates
 * @param uint16_t y coordinates
 * @param uint16_t width
 * @param uint16_t height
 * @param uint32_t colour
 * @return 0 if successful, 1 if not
 */
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

/**
 * @brief Prints a XPM to the buffer
 * @param xpm_map_t The XPM to be drawn
 * @param uint16_t x coordinates
 * @param uint16_t y coordinates
 * @return 0 if successful, 1 if not
 */
int (print_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

/**
 * @brief Prints a XPM to the background buffer
 * @param xpm_map_t The XPM to be drawn
 * @return 0 if successful, 1 if not
 */
int (loadBackground)(xpm_map_t xpm);

/**
 * @brief Allocates space in the memory for the buffer and the background buffer
 */
void (allocateBuffer)();

/**
 * @brief Copies the buffer to the vram
 */
void (showBuffer)();

/**
 * @brief Fills the buffer with 0 (black)
 */
void (clearBuffer)();

/**
 * @brief Copies the background buffer to the buffer
 */
void (drawBackground)();

/**
 * @brief Frees the allocated memory for the buffer and background buffer
 */
void (freeBuffer)();
