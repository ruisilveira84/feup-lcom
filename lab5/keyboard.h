#pragma once

void (kbc_ih)();

int (kbd_subscribe)(uint8_t *bit_no);

int (kbd_unsubscribe)();

int (kbd_status_reg_reader)(uint8_t *b);

int (kbd_output_buffer_reader)(uint8_t *b);
