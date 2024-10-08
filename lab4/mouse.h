#pragma once
#include <lcom/lcf.h>



int (get_KBD_stat)(uint8_t* status);

int (get_KBD_out)(uint8_t port, uint8_t *output, uint8_t mouse);

int (KBD_cmd)(uint8_t port, uint8_t commandByte);

int (mouse_subscribe)(uint8_t *bit_no);

int (mouse_unsubscribe)();

void (mouse_ih)();

void (readBytes)();

void (toPacket)();

int (mouse_write)(uint8_t command);
