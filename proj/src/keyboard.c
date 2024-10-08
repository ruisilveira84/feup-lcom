#include <lcom/lcf.h>
#include "keyboard.h"

#define KBD_IRQ 1
#define KBD_OUT_BUF 0x60
#define KBD_STAT_REG 0x64



#define DELAY_US    20000

int kbd_hook_id = 3;
uint8_t kbd_read = 0x00;



void (kbc_ih)(){
  uint8_t b;
  if(kbd_output_buffer_reader(&b)) return;
  kbd_read = b;
}

int (kbd_subscribe)(uint8_t *bit_no){
  
  if(bit_no == NULL) return 1;

  *bit_no = BIT(kbd_hook_id);
  
  return sys_irqsetpolicy(KBD_IRQ,IRQ_REENABLE | IRQ_EXCLUSIVE ,&kbd_hook_id);
}

int (kbd_unsubscribe)(){
  return sys_irqrmpolicy(&kbd_hook_id);
}

int (kbd_status_reg_reader)(uint8_t *b){ //VERIFICAR SE POSSO LER OU NAO
  if(b==NULL) return 1;
  return util_sys_inb(KBD_STAT_REG,b);
}

int (kbd_output_buffer_reader)(uint8_t *b){ //LER
  if(b==NULL) return 1;
  if(kbd_status_reg_reader(b)) return 1;
  if(*b & (BIT(7)|BIT(6))) return 1; //PARITY (7) E TIMEOUT (6) ERROR
  if(*b & (BIT(5))) return 1; //SE TIVER RATO SAIR
  if(*b & (BIT(0))) return util_sys_inb(KBD_OUT_BUF,b); //SE O OUTPUT BUFFER ESTIVER CHEIO, LER
  
  return 1;
}

int (keyboard_restore)() {
  uint8_t commandByte;
  if(sys_outb(0x64, 0x20)) return 1; // vou ler
  if(util_sys_inb(0x60,&commandByte)) return 1; // ler
  commandByte = commandByte | BIT(0); // ligar o teclado
  if(sys_outb(0x64,0x60)) return 1; //vou escrever
  return sys_outb(0x60,commandByte);
}
