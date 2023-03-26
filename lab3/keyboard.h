//
// Created by rui on 26-03-2023.
//

#ifndef SHARED_KEYBOARD_H
#define SHARED_KEYBOARD_H

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "i8042.h"
#include "KBC.h"

int (keyboard_subscribe_interrupts)(uint8_t *bit_no);

int (keyboard_unsubscribe_interrupts)();

void (kbc_ih)();

int (keyboard_restore)();

#endif //SHARED_KEYBOARD_H
