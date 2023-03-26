//
// Created by rui on 22-03-2023.
//

#include "kbc.h"
#include "i8042.h"

int (read_KBC_status)(uint8_t* status) {
    return util_sys_inb(0x64, status);
}

int (read_KBC_output)(uint8_t port, uint8_t *output) {
    uint8_t attemp = 10;
    uint8_t status;

    while (attemp != 0) {
        if (read_KBC_status(&status) != 0) return 1;

        if ((status & BIT(0)) != 0) {
            if (util_sys_inb(port, output) != 0) return 1;

            if ((status & BIT(6)) != 0) return 1;

            if ((status & BIT(7)) != 0) return 1;

            return 0;
        }
        tickdelay(micros_to_ticks(20000));
        attemp--;
    }
    return 1;
}


