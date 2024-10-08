#pragma once
#include <lcom/lcf.h>


/**
 * @brief Gets KBD status
 * @param uint8_t* The variable the result will be saved on
 * @return 0 if successful, 1 otherwise
 */
int (get_KBD_stat)(uint8_t* status);

/**
 * @brief Gets KBD output
 * @param uint8_t* The port to read
 * @param uint8_t* The variable the result will be saved on
 * @param uint8_t* Whether it's a mouse input or not
 * @return 0 if successful, 1 otherwise
 */
int (get_KBD_out)(uint8_t port, uint8_t *output);

/**
 * @brief Sends a command to the KBD
 * @param uint8_t* The port to write to
 * @param uint8_t The command
 * @return 0 if successful, 1 otherwise
 */
int (KBD_cmd)(uint8_t port, uint8_t commandByte);

/**
 * @brief Subscribes to mouse interrupts
 * @param uint8_t* The bit to be used for the bitmask
 * @return 0 if successful, 1 otherwise
 */
int (mouse_subscribe)(uint8_t *bit_no);

/**
 * @brief Unsubscribes from mouse interrupts
 * @return 0 if successful, 1 otherwise
 */
int (mouse_unsubscribe)();

/**
 * @brief Mouse interrupt handler
 */
void (mouse_ih)();

/**
 * @brief Reads a byte from the mouse
 * @return 0 if successful, 1 otherwise
 */
void (readBytes)();

/**
 * @brief Transforms the bytes read into struct packet for ease of use
 * @return 0 if successful, 1 otherwise
 */
void (toPacket)();

/**
 * @brief Writes a command to the mouse
 * @param uint8_t The command to be written
 * @return 0 if successful, 1 otherwise
 */
int (mouse_write)(uint8_t command);
