#pragma once

int mouse_test_packet(uint32_t cnt);

int mouse_test_async(uint8_t  idle_time);

int mouse_test_remote(uint16_t period, uint8_t cnt);

int mouse_test_gesture(uint8_t x_len, uint8_t tolerance);
