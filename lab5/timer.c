#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"
#include "timer.h"

int timer_hook_id = 4;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (freq > TIMER_FREQ || freq < 19) return 1;

  uint8_t conf = 0;
  if(timer_get_conf(timer,&conf)) return 1;
  conf = (conf & 0x0f) | TIMER_LSB_MSB; //OS BITS 1-3 TEM DE SE MANTER, E O BIT 0 É SEMPRE 0, TB TENHO DE SELECIONAR LSB+MSB

  if(timer==0) conf = conf | TIMER_SEL0;
  else if(timer==1) conf = conf | TIMER_SEL1;
  else if(timer==2) conf = conf | TIMER_SEL2;
  else return 1; //LER DO TIMER QUE QUERO

  uint16_t tick = TIMER_FREQ / freq;
  uint8_t LSB, MSB;
  util_get_LSB(tick,&LSB);
  util_get_MSB(tick,&MSB); //FREQ

  if(sys_outb(TIMER_CTRL, conf)) return 1; //DIZER AO CONTROLLER Q VOU ESCREVER
  if(sys_outb(TIMER(timer),LSB)) return 1; //ESCREVER O LSB NOVO DA FREQ
  return sys_outb(TIMER(timer),MSB); //"          MSB "

}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL) return 1;
  *bit_no = BIT(timer_hook_id);
  return sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&timer_hook_id);
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&timer_hook_id);
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if(st==NULL || timer > 2 || timer < 0) return 1;
  uint8_t rbCmd = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;
  if(sys_outb(TIMER_CTRL, rbCmd)) return 1;
  return util_sys_inb(TIMER(timer),st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val out;
  switch(field){
    case tsf_all:
      out.byte = st;
      break;
    

    case tsf_initial:
      st = st & (BIT(4)|BIT(5)); // st & 00110000 
      st>>=4; //00000011
      if(st==0) out.in_mode = INVAL_val; //00
      else if(st==1) out.in_mode = LSB_only; //01
      else if(st==2) out.in_mode = MSB_only; //10
      else out.in_mode = MSB_after_LSB; //11
      break;
    

    case tsf_mode:
      st = (st & (BIT(1)|BIT(2)|BIT(3))) >> 1;
      if(st == 6) out.count_mode = 2; // 110 == 010
      else if(st == 7) out.count_mode = 3; // 111 == 011
      else out.count_mode = st;
      break;
    

    case tsf_base:
      st = st & BIT(0);
      out.bcd = st;
      break;
    
    default:
      return 1;
  }

  return timer_print_config(timer, field, out);
}
