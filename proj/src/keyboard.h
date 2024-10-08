
/**
 * @brief Handles interrupts for keyboard
 * Writes to uint8_t kbd_read;
 */

void (kbc_ih)();

/**
 * @brief Subscribes to keyboard interrupts
 * @param uint8_t bit number for bitmask
 * @return 0 if successful, 1 otherwise
 */

int (kbd_subscribe)(uint8_t *bit_no);

/**
 * @brief Unsubscribes from keyboard interrupts
 * @return 0 if successful, 1 otherwise
 */

int (kbd_unsubscribe)();

/**
 * @brief Reads from the keyboard status register
 * @param uint8_t* Variable for the output to be written into
 * @return 0 if successful, 1 otherwise
 */

int (kbd_status_reg_reader)(uint8_t *b);

/**
 * @brief Reads from the keyboard output buffer
 * @param uint8_t* Variable for the output to be written into
 * @return 0 if successful, 1 otherwise
 */

int (kbd_output_buffer_reader)(uint8_t *b);

/**
 * @brief Reastores keyboard usage after polling
 * @return 0 if successful, 1 otherwise
 */

int (keyboard_restore)();
