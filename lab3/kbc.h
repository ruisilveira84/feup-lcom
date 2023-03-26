//
// Created by rui on 26-03-2023.
//

#ifndef SHARED_KBC_H
#define SHARED_KBC_H

#include <minix/sysutil.h>
#include "i8042.h"
#include <lcom/lcf.h>

int (read_KBC_status)(uint8_t* status);

int (read_KBC_output)(uint8_t port, uint8_t *output);

int (write_KBC_command)(uint8_t port, uint8_t commandByte);


#endif //SHARED_KBC_H
