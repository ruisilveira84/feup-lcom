// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include "video_gr.h"
#include "keyboard.h"

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

extern void *video_mem;
extern vbe_mode_info_t vmi;

extern int kbd_read;

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if(vg_init(mode) == NULL) {
    vg_exit();
  }

  sleep(delay);
  vg_exit();

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
                    
  if(vg_init(mode) == NULL) {
    vg_exit();
    return 1;
  }

  if(vg_draw_rectangle(x,y,width,height,color)) {
    vg_exit();
    return 1;
  }

  if(kbd_loop()) {
    vg_exit();
    return 1;
  }

  vg_exit();
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if(vg_init(mode) == NULL) {
    vg_exit();
    return 1;
  }

  int vertical = vmi.YResolution / no_rectangles;
  int horizontal = vmi.XResolution / no_rectangles;

  for (int i = 0 ; i < no_rectangles ; i++) {
    for (int j = 0 ; j < no_rectangles ; j++) {

      uint32_t color;

      if(mode == 0x105) {
        color = (first + (j * no_rectangles + i) * step) % (1 << vmi.BitsPerPixel);
      } else {
        uint32_t Red = (R(first) + j * step) % (1 << vmi.RedMaskSize);
        uint32_t Green = (G(first) + i * step) % (1 << vmi.GreenMaskSize);
        uint32_t Blue = (B(first) + (j + i) * step) % (1 << vmi.BlueMaskSize);
        
        color = (Red << vmi.RedFieldPosition) | (Green << vmi.GreenFieldPosition) | 
        (Blue << vmi.BlueFieldPosition);
      }

      if(vg_draw_rectangle(j * horizontal, i * vertical, horizontal, vertical, color)) return 1;
    }
  }

  if(kbd_loop()) {
    return 1;
  }

  vg_exit();
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if(vg_init((uint16_t) 0x105) == NULL) {
    vg_exit();
    return 1;
  }

  if(print_xpm(xpm, x, y) != 0) {
    return 1;
  }

  if(kbd_loop()) {
    return 1;
  }

  vg_exit();
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}

int(kbd_loop)() {
  int ipcstatus, driver;
  uint8_t coco = 2;
  uint32_t hookid = BIT(coco);
  message msg;

  kbd_subscribe(&coco);

  while(kbd_read != 0x90) {
    if((driver = driver_receive(ANY,&msg,&ipcstatus))!=0) {
      printf("Erro a ler");
    } else { //else
      if(is_ipc_notify(ipcstatus)) {
        switch(_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: 
            if(msg.m_notify.interrupts & hookid) {
              kbc_ih();
            }
            break;
          default:
            break;
        }
      }
    }
  }
  return kbd_unsubscribe();
}
