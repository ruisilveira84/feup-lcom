#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id = 0;
int acc = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
    if (timer<0 || timer>2) return 1;
    if (freq>TIMER_FREQ || freq<19) return 1;
    uint16_t counter = TIMER_FREQ/freq;
    uint8_t lsb, msb;
    util_get_LSB(counter, &lsb);
    util_get_MSB(counter, &msb);
    uint8_t cfg;
    if (timer_get_conf(timer, &cfg) != 0) return 1;
    cfg = cfg & 0x0F;
    cfg = cfg | TIMER_LSB_MSB;

    switch (timer) {
        case 0:
            cfg = cfg | TIMER_SEL0;
            break;
        case 1:
            cfg = cfg | TIMER_SEL1;
            break;
        case 2:
            cfg = cfg | TIMER_SEL2;
            break;
        default:
            return 1;
    }

    if (sys_outb(TIMER_CTRL, cfg) != 0) return 1;

    switch (timer) {
        case 0:
            if (sys_outb(TIMER_0, lsb) != 0) return 1;
            if (sys_outb(TIMER_0, msb) != 0) return 1;
        case 1:
            if (sys_outb(TIMER_1, lsb) != 0) return 1;
            if (sys_outb(TIMER_1, msb) != 0) return 1;
        case 2:
            if (sys_outb(TIMER_2, lsb) != 0) return 1;
            if (sys_outb(TIMER_2, msb) != 0) return 1;
    }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    if (bit_no == NULL) return 1;
    *bit_no = BIT(hook_id);
    if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id)) return 1;
  return 0;
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void (timer_int_handler)() {
    acc++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
    if (!(timer==0 || timer==1 || timer==2)) return 1;
    if (st == NULL) return 1;
    uint8_t cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
    if (sys_outb(TIMER_CTRL, cmd)) return 1;
    return util_sys_inb(0x40+timer, st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
    if (st == 0 || timer<0 || timer>2) return 1;
    union timer_status_field_val dat;
    switch (field) {
        case tsf_all:
            dat.byte = st;
            break;

        case tsf_initial:
            st = st>>4 & 0x03;
            if (st == 1) dat.in_mode = LSB_only;
            else if (st == 2) dat.in_mode = MSB_only;
            else if (st == 3) dat.in_mode = MSB_after_LSB;
            else dat.in_mode = INVAL_val;
            break;

        case tsf_mode:
            st = st>>1 & 0x07;
            if (st == 6) dat.count_mode = 2;
            else if (st == 7) dat.count_mode = 3;
            else dat.count_mode = st;
            break;

        case tsf_base:
            dat.bcd = st & 0x01;
            break;

        default:
            return 1;
    }
    if (timer_print_config(timer, field, dat) != 0) return 1;
  return 0;
}
