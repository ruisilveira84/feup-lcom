/**
 * @brief Gets the least significant byte of a number with 2 bytes
 * @param uint16_t The number with two bytes
 * @param uint8_t The variable it will be written into
 * @return 0 if successful, 1 otherwise
 */

int(util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief Gets the most significant byte of a number with 2 bytes
 * @param uint16_t The number with two bytes
 * @param uint8_t The variable it will be written into
 * @return 0 if successful, 1 otherwise
 */

int(util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief sys_inb wrapper for uint8_t values
 * @param int The port to write the command into
 * @param uint8_t The command
 * @return 0 if successful, 1 otherwise
 */

int (util_sys_inb)(int port, uint8_t *value);
