#include <lcom/lcf.h>

#include "vg.h"

static void *video_mem;
uint8_t* buffer;
uint8_t* background;
vbe_mode_info_t vmi;

void *(vg_init)(uint16_t mode) {
  reg86_t reg86;

  if(map_vram(mode)) {
    return NULL;
  }

  memset(&reg86, 0, sizeof(reg86));

  reg86.ax = 0x4f02;
  reg86.bx = (1 << 14) | mode;
  reg86.intno = 0x10;

  if(sys_int86(&reg86) != 0) {
    printf("\tvg_init(): sys_int86() failed \n");
    return NULL;
  }
  return video_mem;
}

int (map_vram)(uint16_t mode) {
  struct minix_mem_range mr;

  unsigned int vram_base;  /* VRAM's physical addresss */

  unsigned int vram_size;
  /* VRAM's size, but you can use
              the frame-buffer size, instead */

  unsigned int bytes_per_pixel;

  int r;

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  memset(&vmi, 0, sizeof(vmi));
  if(vbe_get_mode_info(mode, &vmi)!=0) {
    return 1;
  }

  vram_base = vmi.PhysBasePtr;
  bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;
  vram_size = (vmi.XResolution * vmi.YResolution) * bytes_per_pixel;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if( 0 != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return 1;
  }

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return 1;
  }

  return 0;
}

int(vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

    uint8_t bytes = (vmi.BitsPerPixel + 7) / 8;
    uint8_t* base = buffer + (y * vmi. XResolution + x) * bytes;

    if(x >= vmi.XResolution || y >= vmi.YResolution) return 0;
    for(uint8_t i = 0; i < bytes; i++){
        *base = color >> (i * 8);
        base++;
    }

    return 0;
}

int (vg_draw_pixel_to_background)(uint16_t x, uint16_t y, uint32_t color){
    uint8_t bytes = (vmi.BitsPerPixel + 7) / 8;
    uint8_t* base = background + (y * vmi. XResolution + x) * bytes;

    if(x >= vmi.XResolution || y >= vmi.YResolution) return 0;
    for(uint8_t i = 0; i < bytes; i++){
        *base = color >> (i * 8);
        base++;
    }

    return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t width, uint32_t color) {
  for(uint16_t i = 0; i < width; i++) {
    if (vg_draw_pixel(x + i, y, color) != 0) {
      return 1;
    }
  }

  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for(uint16_t i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color) != 0) {
      vg_exit();
      return 1;
    }
  }

  return 0;
}

uint32_t (R)(uint32_t first){
  return ((1 << vmi.RedMaskSize) - 1) & (first >> vmi.RedFieldPosition);
}

uint32_t (G)(uint32_t first){
  return ((1 << vmi.GreenMaskSize) - 1) & (first >> vmi.GreenFieldPosition);
}

uint32_t (B)(uint32_t first){
  return ((1 << vmi.BlueMaskSize) - 1) & (first >> vmi.BlueFieldPosition);
}

int (print_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    xpm_image_t img;
    uint32_t* colours = (uint32_t *) xpm_load(xpm, XPM_8_8_8_8, &img);
    int i = 0;
    for (int h = 0 ; h < img.height ; h++) {
        for (int w = 0 ; w < img.width ; w++) {
            if (colours[i]!=0 && (x+w)<vmi.XResolution && (y+h)<vmi.YResolution) {
                vg_draw_pixel(x+w,y+h,colours[i]);
            }
            i+=1;
        }
    }
    return 0;
}

int (loadBackground)(xpm_map_t xpm){
    xpm_image_t img;
    uint32_t* colours = (uint32_t *) xpm_load(xpm, XPM_8_8_8_8, &img);
    memset(background,0,vmi.XResolution*vmi.YResolution*4);
    //if(img.width!=vmi.XResolution || img.height!=vmi.YResolution) return 1;
    
    int i = 0;
    for (int h = 0 ; h < img.height ; h++) {
        for (int w = 0 ; w < img.width ; w++) {

            vg_draw_pixel_to_background(w,h,colours[i]);
            i+=1;
        }
    }
    return 0;
}

void (allocateBuffer)(){
    buffer = (uint8_t *)malloc(vmi.XResolution*vmi.YResolution*((vmi.BitsPerPixel + 7) / 8)); 
    background = (uint8_t *)malloc(vmi.XResolution*vmi.YResolution*((vmi.BitsPerPixel + 7) / 8)); 
}

void (showBuffer)(){
    memcpy((uint8_t*)video_mem, buffer, vmi.XResolution*vmi.YResolution*((vmi.BitsPerPixel + 7) / 8)); 
}

void (clearBuffer)(){
    memset(buffer,0,vmi.XResolution*vmi.YResolution*((vmi.BitsPerPixel + 7) / 8));
}

void (drawBackground)(){
    memcpy(buffer, background, vmi.XResolution*vmi.YResolution*((vmi.BitsPerPixel + 7) / 8)); 
}

void (freeBuffer)(){
    free(buffer); 
    free(background);
}
