#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

/**
 * @brief Sets the frequency of a timer
 * @param uint8_t The timer
 * @param uint32_t The frequency
 * @return 0 if successful, 1 otherwise
 */
int (timer_set_frequency)(uint8_t timer, uint32_t freq);

/**
 * @brief Subscribes to the interrupts of timer 0
 * @param uint8_t The bit for the bitmask
 * @return 0 if successful, 1 otherwise
 */
int (timer_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes to the interrupts of timer 0
 * @return 0 if successful, 1 otherwise
 */
int (timer_unsubscribe_int)();

/**
 * @brief Handles interrupts of the timer
 * Adds +1 to variable timer_counter
 */
void (timer_int_handler)();

/**
 * @brief Gets the configuration of the timer
 * @param uint8_t The timer
 * @param uint8_t* The variable the output will be saved on
 * @return 0 if successful, 1 otherwise
 */
int (timer_get_conf)(uint8_t timer, uint8_t *st);

/**
 * @brief Displays the configuration of a timer
 * @param uint8_t The timer
 * @param uint8_t The configuration
 * @param enum timer_status_field Whether it will display all the information or only part of it
 * @return 0 if successful, 1 otherwise
 */
int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field);
