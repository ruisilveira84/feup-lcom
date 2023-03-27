//
// Created by rui on 26-03-2023.
//

#include "keyboard.h"
#include "i8042.h"

uint8_t scancode;
int kb_id = 2;

int (keyboard_subscribe_interrupts)(uint8_t *bit_no) {
    if (bit_no == NULL) return 1;
    *bit_no = BIT(kb_id);
    if (sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &kb_id)) return 1;
    return 0;
}

int (keyboard_unsubscribe_interrupts)() {
    return sys_irqrmpolicy(&kb_id);
}

void (kbc_ih)() {
    read_KBC_output(0x60, &scancode);
}

int (keyboard_restore)() {
    uint8_t cmd;
    if (write_KBC_command(KBC_IN_CMD, KBC_READ_CMD) != 0) return 1;
    if (read_KBC_output(KBC_OUT_CMD, &cmd) != 0) return 1;
    cmd = cmd | ENABLE_INT;

    if (write_KBC_command(KBC_IN_CMD, KBC_WRITE_CMD) != 0) return 1;
    if (write_KBC_command(KBC_WRITE_CMD, cmd) != 0) return 1;
    return 0;
}

