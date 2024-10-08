#include <lcom/lcf.h>

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

/**
 * @brief Writes a command to the RTC
 * @param uint8_t The port to write to
 * @param uint8_t The command
 * @return 0 if successful, 1 otherwise
 */
int (writeRtc)(uint8_t loc, uint8_t cmd);

/**
 * @brief Reads from the RTC
 * @param uint8_t The port to read from
 * @param uint8_t The variable where the output is stored
 * @return 0 if successful, 1 otherwise
 */
int (readRtc)(uint8_t loc, uint8_t* out);

/**
 * @brief Checks if RTC updating
 * @return 0 if it isn't, 1 if it is
 */
bool (rtcUpdating)();

/**
 * @brief Checks if RTC is in binary mode
 * @return 0 if it isn't, 1 if it is
 */
bool (rtcBinary)();

/**
 * @brief Converts a number to binary
 * @param uint8_t The number to convert to binary
 * @return The converted number
 */
uint8_t (toBinary)(uint8_t n);

/**
 * @brief Reads hours
 * @param uint8_t* The variable where the output will be stored
 * @return 0 if successful, 1 otherwise
 */
int (rtcReadHours)(uint8_t* out);

/**
 * @brief Reads minutes
 * @param uint8_t* The variable where the output will be stored
 * @return 0 if successful, 1 otherwise
 */
int (rtcReadMinutes)(uint8_t* out);

/**
 * @brief Reads seconds
 * @param uint8_t* The variable where the output will be stored
 * @return 0 if successful, 1 otherwise
 */
int (rtcReadSeconds)(uint8_t* out);
