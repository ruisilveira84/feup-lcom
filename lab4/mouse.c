#include <lcom/lcf.h>

#include "mouse.h"
#include "utils.h"

#define IRQ_MOUSE 12

int mouse_hook_id = 2;        
struct packet pckt;  
uint8_t bIndex = 0;       
uint8_t bytes[3];       
uint8_t curByte;         

int (mouse_subscribe)(uint8_t *bit_no){
  if (bit_no == NULL) return 1;
  *bit_no = BIT(mouse_hook_id);
  return sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id);
}


int (mouse_unsubscribe)(){
  return sys_irqrmpolicy(&mouse_hook_id);
}


void (mouse_ih)(){
  get_KBD_out(0x60, &curByte, 1);
}


void (readBytes)() {
  if (bIndex != 0 || (curByte & BIT(3))) {
    bytes[bIndex]= curByte;
    bIndex++;
  }
}

void (toPacket)(){

  pckt.bytes[0] = bytes[0];
  pckt.bytes[1] = bytes[1];
  pckt.bytes[2] = bytes[2];


  pckt.lb = bytes[0] & BIT(0);
  pckt.rb = bytes[0] & BIT(1);
  pckt.mb = bytes[0] & BIT(2);
  
  pckt.delta_x = bytes[1];
  if(bytes[0] & BIT(4)) pckt.delta_x |= 0xFF00;
  pckt.delta_y = bytes[2];
  if(bytes[0] & BIT(5)) pckt.delta_y |= 0xFF00;

  pckt.x_ov = bytes[0] & BIT(6);
  pckt.y_ov = bytes[0] & BIT(7);
  
}

int (mouse_write)(uint8_t cmd) {

  uint8_t out;
  uint8_t attempts = 20;
  

  do {
    attempts--;
    if (KBD_cmd(0x64, 0xd4)) return 1;
    if (KBD_cmd(0x60, cmd)) return 1;
    tickdelay(micros_to_ticks(20000));
    if (util_sys_inb(0x60, &out)) return 1;
    if (out == 0xfa) return 0;
  } while (out != 0xfa && attempts);

  return 1;
}

int (get_KBD_stat)(uint8_t* stat) {
  return util_sys_inb(0x64, stat);
}

int (get_KBD_out)(uint8_t port, uint8_t *out, uint8_t mouse) {
  uint8_t stat;
  uint8_t attempts = 20;
  while (attempts) {
    if (get_KBD_stat(&stat) != 0) return 1;
    if ((stat & BIT(0)) != 0) {                       
      if(util_sys_inb(port, out) != 0) return 1;
      if((stat & BIT(7)) != 0) return 1;
      if((stat & BIT(6)) != 0) return 1;
      if (mouse && !(stat & BIT(5))) return 1;
      return 0;
    }
    tickdelay(micros_to_ticks(20000));
    attempts--;
  }
  return 1;
}

int (KBD_cmd)(uint8_t port, uint8_t cmd) {

  uint8_t stat;
  uint8_t attempts = 20;
  while (attempts) {
    if (get_KBD_stat(&stat) != 0) return 1;
    if ((stat & BIT(1)) == 0){
        if(sys_outb(port, cmd) != 0) return 1;
        return 0;
    }
    tickdelay(micros_to_ticks(20000));
    attempts--;
  }
  return 1;
}
