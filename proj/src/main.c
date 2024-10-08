#include <lcom/lcf.h>

#include "mouse.h"
#include "timer.h"
#include "keyboard.h"
#include "utils.h"
#include "vg.h"
#include "i8254.h"
#include "xpm2.h"
#include "xpmbig.h"
#include "rtc.h"


#include <stdint.h>

#include "i8254.h"

extern int timer_hook_id;
extern int kbd_hook_id;
extern int timer_counter;
extern uint8_t kbd_read;
extern void *video_mem;
extern vbe_mode_info_t vmi;
extern struct packet pckt;
extern uint8_t bIndex;

struct pointer{
  int X;
  int Y;
};

struct player{
  int X;
  int Y;
  int XLen, YLen;
  xpm_row_t** img;
  int curAnim, maxAnim;
};

struct obstacle{
  int X;
  int Y;
  int XLen, YLen;
  int speed;
  xpm_row_t* img;
  bool active;
};

int maxTime = 50;
float speedMul = 1;
char clockStr[50];

int main(int argc, char *argv[]){

  lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

    return 0;
}



void (saveNewHighScore)(int maxScore){
  FILE *fp;

  fp = fopen("/home/lcom/labs/proj/highScore.txt", "w");
  fprintf(fp, "%d", maxScore);
  fclose(fp);
}

int (readHighScore)(){
  int out = 0;
  FILE *fp;
  
  fp = fopen("/home/lcom/labs/proj/highScore.txt", "r");
  if(fp == NULL) return 0;

  fscanf(fp, "%d", &out);
  fclose(fp);
  return out;
}

void(drawNumber)(int num, int x, int y){
  if(num == 0) print_xpm(num0,x,y);
  else if(num == 1) print_xpm(num1,x,y);
  else if(num == 2) print_xpm(num2,x,y);
  else if(num == 3) print_xpm(num3,x,y);
  else if(num == 4) print_xpm(num4,x,y);
  else if(num == 5) print_xpm(num5,x,y);
  else if(num == 6) print_xpm(num6,x,y);
  else if(num == 7) print_xpm(num7,x,y);
  else if(num == 8) print_xpm(num8,x,y);
  else if(num == 9) print_xpm(num9,x,y);
}

void (drawClock)(){
  for(int i = 0; i < 9; i++){
    if(clockStr[i]==':') print_xpm(symbolColon,10+(i*32),820);
    else drawNumber(clockStr[i]-'0',10+(i*32),820);
  }
}

void (drawPoints)(int x, int y,int points){
  char pointsStr[8] = "000000";
  sprintf(pointsStr,"%06d",points);
  int i = 0;
  while(pointsStr[i] != '\0'){
    drawNumber(pointsStr[i]-'0',x+(i*32),y);
    i++;
  }
}

int (max)(int a, int b){
  if(a>b) return a;
  return b;
}

int (min)(int a, int b){
  if(a<b) return a;
  return b;
}

int (clamp)(int a, int mn, int mx){
  if(a<mn) return mn;
  if(a>mx) return mx;
  return a;
}

bool (intersects)(struct player p, struct obstacle o){
  if((p.X+p.XLen-10) < o.X || (p.Y+p.YLen-10) < o.Y || p.Y > (o.Y+o.YLen-10) || p.X > (o.X+o.XLen-10)) return false;
  return true;
}

int (proj_main_loop)(){

  bool menu = true;
  

  if(vg_init((uint16_t) 0x14c) == NULL) {
    vg_exit();
    return 1;
  }

  

  struct pointer p2;

  p2.X = 500;
  p2.Y = 100;

  int ignoreFirstMousePackets = 5; // sem isto dá problemas

  memset(&pckt,0,sizeof(pckt));

  bool day = true;
  uint8_t hour = 0, minute = 0, second = 0;
  while(rtcReadHours(&hour));
  while(rtcReadMinutes(&minute));
  while(rtcReadSeconds(&second));
  day = (hour>=6 && hour<=19);
  int score = 0, maxScore = 0;
  maxScore = readHighScore();

  bool lost = false;
  bool tempIgnoreLeftMouse = false, tempIgnoreRightMouse = false, tempIgnoreMiddleMouse = false;

  struct player p;
  struct obstacle o[5];
  int nObs = 5;

  for(int i = 0; i < nObs; i++) o[i].active = false;

  //p.img = gajo0;

  p.X = 100;
  p.Y = 600;
  p.XLen = 64;
  p.YLen = 64;
  p.img = malloc(sizeof(xpm_row_t*)*2);
  p.img[0] = gajo1;
  p.img[1] = gajo2;
  p.maxAnim = 2;
  p.curAnim = 0;


  uint8_t kbdbitno = 0, timerbitno, mousebitno;
  uint8_t bytes[2] = {0,0};
  if(kbd_subscribe(&kbdbitno)) return 1;
  if(timer_subscribe_int(&timerbitno)) return 1;
  if (mouse_subscribe(&mousebitno) != 0) return 1;
  if (mouse_write(0xf4) != 0) return 1;
  kbd_read = 0x00;

  int ipc_status, driver;
  message msg;

  srand(time(NULL));
  allocateBuffer();
  loadBackground(minixSurfersXpm);

  bool left = false, right = false, up = false, down = false;
  bool endgame = false;

  clearBuffer();
  //print_xpm(titleXpm,150,200);

  while(kbd_read != 0x81 || !endgame) { 
    endgame = !menu;
    if((driver = driver_receive(ANY,&msg,&ipc_status))!=0){ //se houver algum erro a ler
    printf("Erro a ler");
    }
    else{ //else
      if(is_ipc_notify(ipc_status)){ //se tiver recebido um interrupt da merda q quero
        switch(_ENDPOINT_P(msg.m_source)){
          case HARDWARE: //SE FOR HARDWARE INTERRUPT
          if(msg.m_notify.interrupts & kbdbitno){ //SE TIVER VINDO DO SITIO QUE EU QUERO
            kbc_ih();
            if(kbd_read == 0x81){ //ESC
              while(mouse_write(0xf5));
              endgame = true;
              break;
            }
            else if(kbd_read == 0xe0){
                bytes[0] = 0xe0;
            }
            else if(kbd_read == 0x13 && lost){ //R
              score = 0;
              for(int i = 0; i < nObs; i++) o[i].active = false;
              p2.X = 500;
              p2.Y = 100;
              p.X = 100;
              p.Y = 600;
              speedMul = 1;
              lost = false;
            }
            else{
                //bool make = !(kbd_read & BIT(7));
                if(!lost){
                  if(bytes[0] == 0xe0){
                    bytes[1] = kbd_read;
                  }
                  else {
                    bytes[0] = kbd_read;
                    if(bytes[0] == 0x1e) left = true;
                    else if(bytes[0] == 0x9e) left = false;
                    else if(bytes[0] == 0x20) right = true;
                    else if(bytes[0] == 0xa0) right = false;
                    else if(bytes[0] == 0x11) up = true;
                    else if(bytes[0] == 0x91) up = false;
                    else if(bytes[0] == 0x1f) down = true;
                    else if(bytes[0] == 0x9f) down = false;
                  }
                }
                else{

                }
                bytes[0] = 0;
                bytes[1] = 0;
            }
          }
          if(msg.m_notify.interrupts & timerbitno){
            timer_int_handler();
            
            
            if(timer_counter>=5){
              clearBuffer();
              while(rtcReadHours(&hour));
              while(rtcReadMinutes(&minute));
              while(rtcReadSeconds(&second));
              sprintf(clockStr,"%02d:%02d:%02d",hour,minute,second);
              if(!menu){
                p.curAnim++;
                if(p.curAnim>=p.maxAnim) p.curAnim = 0;
                clearBuffer();
                drawBackground();
                
                //day = (hour>=6 && hour<=19);
                //if(day) print_xpm(sun,50,50);
                //else print_xpm(moon,50,50);
                if(!lost){ //numero arbitrario tbh
                  
                  score++;
                  if(score % maxTime == 0){
                    speedMul += 0.1;
                  }
                  
                  
                  if(left && !right && p.X>100){ //esquerda
                    p.X-=20;
                  }
                  else if(right && !left && p.X<970){ //direita
                    p.X+=20;
                  }
                  if(up && !down && p.Y > 400){ //cima
                    p.Y-=20;
                  }
                  else if(down && !up && p.Y < 700){ //cima
                    p.Y+=20;
                  }


                  timer_counter = 0;

                  for(int i = 0; i < nObs; i++){
                    if(o[i].active){
                      o[i].Y += o[i].speed*speedMul;
                      if((o[i].Y)>=vmi.YResolution) o[i].active = false;
                      if(o[i].active) print_xpm(o[i].img,o[i].X,o[i].Y);
                      if(intersects(p,o[i])){
                        lost = true;
                        left = false;
                        right = false;
                        up = false;
                        down = false;
                        tempIgnoreLeftMouse = false;
                        tempIgnoreRightMouse = false;
                        tempIgnoreMiddleMouse = false;
                        if(score > maxScore) 
                        {
                          maxScore = score;
                          saveNewHighScore(maxScore);
                        }
                      }
                    }
                  }
                
                print_xpm(p.img[p.curAnim],p.X,p.Y);
                if(lost) print_xpm(game_over,400,400);
                print_xpm(cursor,p2.X,p2.Y);
                drawPoints(710,0,maxScore);
                drawPoints(945,0,score);
                drawClock();
                showBuffer();
                }
                else{
                  timer_counter = 0;
                  print_xpm(cursor,p2.X,p2.Y);
                  print_xpm(morte,p.X,p.Y);
                  for(int i = 0; i < nObs; i++){
                    if(o[i].active) print_xpm(o[i].img,o[i].X,o[i].Y);
                  }
                  print_xpm(game_over,400,400);
                  drawPoints(710,0,maxScore);
                  drawPoints(945,0,score);
                  drawClock();
                  showBuffer();
                }
              }
              else{ //if (menu)
                drawBackground();
                drawPoints(710,0,maxScore);
                drawClock();
                timer_counter = 0;
                
                if(!(p2.X<474 || p2.X>675 || p2.Y<647 || p2.Y>720))
                {
                  print_xpm(play2,474,648);
                  if(pckt.lb){
                    menu = false;
                    if(day) loadBackground(bgXpmDAY);
                    else loadBackground(bgXpmNIGHT);
                  }
                }
                print_xpm(cursor,p2.X,p2.Y);
                showBuffer();
              }
            }
          }
          if (msg.m_notify.interrupts & mousebitno){
            endgame = false;
            mouse_ih();
            readBytes();
            if (bIndex == 3 && ignoreFirstMousePackets==0) {
              toPacket();
              p2.X+=(pckt.delta_x)/1;
              p2.Y-=(pckt.delta_y)/1;
              p2.X = clamp(p2.X,0,1152);
              p2.Y = clamp(p2.Y,0,864);
              if(!menu && !lost){
                p2.X = clamp(p2.X,120,980);
                p2.Y = clamp(p2.Y, 75, 300);
                if(pckt.lb && !tempIgnoreLeftMouse){
                  int n = 0;
                  for(int i = 0; i < nObs; i++){
                    if(!o[i].active && n < 3){
                      o[i].X = clamp(p2.X,100,780);
                      o[i].Y = p2.Y;
                      o[i].XLen = 200;
                      o[i].YLen = 100;
                      o[i].active = true;
                      o[i].img = obstacleWide;
                      o[i].speed = 5;
                      tempIgnoreLeftMouse = true;
                      break;
                    }
                    else if(o[i].XLen == 200) n++; //para nao deixar ter 3 compridos ao mesmo tempo (senao nao da para evitar)
                  }
                }
                else if(pckt.rb && !tempIgnoreRightMouse){
                  for(int i = 0; i < nObs; i++)  if(!o[i].active){
                    o[i].X = clamp(p2.X,100,880);
                    o[i].Y = p2.Y;
                    o[i].XLen = 100;
                    o[i].YLen = 200;
                    o[i].active = true;
                    o[i].img = ComboioCP;
                    o[i].speed = 20;
                    tempIgnoreRightMouse = true;
                    break;
                  }
                }
                else if(pckt.mb && !tempIgnoreMiddleMouse){
                  for(int i = 0; i < nObs; i++)  if(!o[i].active){
                    o[i].X = clamp(p2.X,100,958);
                    o[i].Y = p2.Y;
                    o[i].XLen = 22;
                    o[i].YLen = 58;
                    o[i].active = true;
                    o[i].img = missil;
                    o[i].speed = 60;
                    tempIgnoreMiddleMouse = true;
                    break;
                  }
                }
                if(!pckt.lb) tempIgnoreLeftMouse = false; //evitar repetidos
                if(!pckt.rb) tempIgnoreRightMouse = false;
                if(!pckt.mb) tempIgnoreMiddleMouse = false;
              }
              bIndex = 0;
            }
            else if(bIndex==3){
              toPacket();
              bIndex = 0;
              ignoreFirstMousePackets--;
            }
          }

          break;
          default:
          break;
        }
      }
    }
  }


  free(p.img);
  timer_unsubscribe_int();
  kbd_unsubscribe();
  mouse_unsubscribe();
  mouse_write(0xf5);
  freeBuffer();
  vg_exit();
  //while(mouse_unsubscribe());
  
  return 0;
}
