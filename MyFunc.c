#include "main.h"
#include "myregmap.h"
#include "MyFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include "MyBMP.h"

unsigned char gExti[16] = {0,};
unsigned int gExNum = 0;
unsigned short gColor;

void boardSet()
{
  // right, left, up, down, select
  
  rRCC_APB2ENR = rRCC_APB2ENR | (1<<3) | (1<<5) | (1<<6);
  // port B, D, E를 enable 시킴
  
  *(unsigned int *) 0xE000E100 |= (0x1 << 6);
  // NVIC 인터럽트 및 enable register를 설정
  // 인터럽트 PE0(right 버튼)을 enable 시킴 (IRQ6)
  
  *(unsigned int *) 0xE000E100 |= (0x1 << 7);
  // 인터럽트 PE1(left 버튼)을 enable 시킴 (IRQ7)
  
  *(unsigned int *) 0xE000E100 |= (0x1 << 23);
  // 인터럽트 PD8(up 버튼) / PB9(key 버튼)을 enable 시킴 (IRQ23)
  
  *(unsigned int *) 0xE000E104 |= (0x1 << 8);
  // 인터럽트 PD12(down 버튼) / PD14(select 버튼)을 enable 시킴 (IRQ40)
  
  rEXTI_IMR = rEXTI_IMR | (1<<12) | (1<<14) | (1<<1) | (1<<0) | (1<<8) | (1<<9);
  // 조이스틱 및 Key 버튼의 인터럽트를 unmasking 해줌
  
  rEXTI_FTSR = rEXTI_FTSR | (1<<12) | (1<<14) | (1<<1) | (1<<0) | (1<<8) | (1<<9);
  // 조이스틱 및 Key 버튼이 눌리는 순간에 인터럽트가 발생하도록 설정 (falling edge)
  
  rAFIO_EXTICR4 = rAFIO_EXTICR4 & ~(0xf << 0);
  rAFIO_EXTICR4 = rAFIO_EXTICR4 | (0x3 << 0);
  // PD12(down 버튼)를 enable 시킴

  rAFIO_EXTICR4 = rAFIO_EXTICR4 & ~(0xf << 8);
  rAFIO_EXTICR4 = rAFIO_EXTICR4 | (0x3 << 8);
  // PD14(select 버튼)를 enable 시킴
  
  rAFIO_EXTICR1 = rAFIO_EXTICR1 & ~(0xf << 4);
  rAFIO_EXTICR1 = rAFIO_EXTICR1 | (0x4 << 4);
  // PE1(left 버튼)를 enable 시킴
  
  rAFIO_EXTICR1 = rAFIO_EXTICR1 & ~(0xf << 0);
  rAFIO_EXTICR1 = rAFIO_EXTICR1 | (0x4 << 0);
  // PE0(right 버튼)를 enable 시킴

  rAFIO_EXTICR3 = rAFIO_EXTICR3 & ~(0xf << 0);
  rAFIO_EXTICR3 = rAFIO_EXTICR3 | (0x3 << 0);
  // PD8(up 버튼)를 enable 시킴
  
  rAFIO_EXTICR3 = rAFIO_EXTICR3 & ~(0xf << 4);
  rAFIO_EXTICR3 = rAFIO_EXTICR3 | (0x1 << 4);
  // PD9(key 버튼)를 enable 시킴
  
  LCD_Setup();
  STM3210B_LCD_Init();
  // LCD를 사용하도록 설정
}

unsigned int victory = 0;

struct p {
  double x;
  double y;
};

typedef struct p Pos;

Pos walls[200];
int numOfWall;

Pos webs[50];
int numOfWeb;

Pos spiders[10];
int spiderDir[10];
int numOfSpider;
double spiderSpeed = 0.5;

double playerSpeed = 0.5;

double boyXPos = 289-(30*5);
double boyYPos = 200;

unsigned int beforeBoyDir = 0;
unsigned int boyDir = 0;

double girlXPos = 289-(30*3);
double girlYPos = 200;

unsigned int beforeGirlDir = 0;
unsigned int girlDir = 0;

unsigned int boyLive = 1;
unsigned int girlLive = 1;

unsigned int shootState = 0;
unsigned int superMode = 0;

unsigned int superItem = 0;
Pos sitem;

void setWalls(){
  
  int cnt = 0;
  
  for(int y=40; y<=220; y+=20){
    walls[cnt].x = 319;
    walls[cnt].y = y;
    
    cnt++;
  }
  
  for(int x=289; x>=19; x-=30){
    walls[cnt].x = x;
    walls[cnt].y = 220;
    
    cnt++;
  }
  
  for(int y=200; y>=40; y-=20){
    walls[cnt].x = 19;
    walls[cnt].y = y;
    
    cnt++;
  }
  
  for(int x=49; x<=289; x+=30){
    walls[cnt].x = x;
    walls[cnt].y = 40;
    
    cnt++;
  }
  
 for(int y=200; y>=80; y-=20){
   
    walls[cnt].x = 289 - (30*4);
    walls[cnt].y = y;
    
    cnt++;
  }
  
  for(int x=259; x>=79; x-=30){
    
    if(x == 169)
      continue;
    
    walls[cnt].x = x;
    walls[cnt].y = 180;
    
    cnt++;
  }
  
  for(int x=289; x>=49; x-=30){
    
    if(x == 199 || x == 169 || x == 139 || x == 79)
      continue;
    
    walls[cnt].x = x;
    walls[cnt].y = 140;
    
    cnt++;
  }
  
  for(int x=289; x>=49; x-=30){
    
    if(x == 259 || x == 199 || x == 169 || x == 139 || x == 79)
      continue;
    
    walls[cnt].x = x;
    walls[cnt].y = 100;
    
    cnt++;
  }
  
  for(int x=289; x>=49; x-=30){
    
    if(x == 259 || x == 169 || x == 79)
      continue;
    
    walls[cnt].x = x;
    walls[cnt].y = 80;
    
    cnt++;
  }
  
  numOfWall = cnt;
}

void setWebs(){
  
  int cnt = 0;
  
  webs[cnt].x = 289-(30*2);
  webs[cnt].y = 160;
  cnt++;
  
  webs[cnt].x = 289-(30*8);
  webs[cnt].y = 120;
  cnt++;
  
  webs[cnt].x = 289-(30*7);
  webs[cnt].y = 120;
  cnt++;
  
  webs[cnt].x = 289-(30*5);
  webs[cnt].y = 140;
  cnt++;
  
  webs[cnt].x = 289-(30*8);
  webs[cnt].y = 160;
  cnt++;
  
  webs[cnt].x = 289-(30*0);
  webs[cnt].y = 160;
  cnt++;
  
  numOfWeb = cnt;
}

void setSpiders(){
  
  int cnt = 0;
  
  spiders[cnt].x = 259;
  spiders[cnt].y = 80;
  cnt++;
  
  spiders[cnt].x = 79;
  spiders[cnt].y = 60;
  cnt++;
  
  numOfSpider = cnt;
}

void initialize(){
  
  LCD_Clear(Black);
  
  setWalls();
  setWebs();
  setSpiders();
  
  for(int i=0; i<numOfWall; i++)
    LCD_DrawArray(wall, walls[i].y, walls[i].x, 30, 20);
  
  for(int i=0; i<numOfWeb; i++)
    LCD_DrawArray(web, webs[i].y, webs[i].x, 30, 20);
  
  for(int i=0; i<numOfSpider; i++){
    LCD_DrawArray(spider1, spiders[i].y, spiders[i].x, 30, 20);
    spiderDir[i] = 1;
  }
  
  sitem.x = 289-(30*5);
  sitem.y = 100;
  
  LCD_DrawArray(boy, boyYPos, boyXPos, 28, 18);
  LCD_DrawArray(girl, girlYPos, girlXPos, 28, 18);
  
  LCD_DrawArray(heart, 60, 289-(30*4), 30, 20);
  
  boyDir = 0;
  girlDir = 0;
}
    
void isVictory(){
  if(boyLive == 0 && girlLive == 0){
    LCD_DrawArray(gameover, 100, 200, 120, 40);
    victory = -1;
  }
  else if( boyLive == 1 && girlLive == 1
    && (boyXPos >= 289-(30*5) && boyXPos <= 289-(30*3))
    && (girlXPos >= 289-(30*5) && girlXPos <= 289-(30*3))
    && (boyYPos >= 60 && boyYPos <= 60+10)
    && (girlYPos >= 60 && girlYPos <= 60+10) ){
    LCD_DrawArray(youwin, 100, 200, 120, 40);
    LCD_DrawArray(heart2, 60, 289-(30*4), 30, 20);
    
    victory = 1;
  }
}

void enemyMove(){
  if(!(spiders[0].y >= 60 && spiders[0].y <= 100))
    spiderDir[0] *= -1;// 1 or -1
  
  if(!(spiders[1].x >= 49 && spiders[1].x <= 109))
    spiderDir[1] *= -1;// 1 or -1
  
  spiders[0].y += (spiderDir[0] * spiderSpeed);
  spiders[1].x += (spiderDir[1] * spiderSpeed);
  
  for(int i=0; i<numOfSpider; i++)
    LCD_DrawArray(spider1, spiders[i].y, spiders[i].x, 30, 20);
}

void setSuperState(){
  if( superItem == 1
        && (((boyXPos-28 <= sitem.x && boyXPos >= sitem.x-30) && (boyYPos < sitem.y+20 && boyYPos+18 > sitem.y))
         || ((boyYPos+18 >= sitem.y && boyYPos <= sitem.y+20) && (boyXPos > sitem.x-30 && boyXPos-28 < sitem.x)))){
           superMode = 1;
           playerSpeed = 1;
           LCD_DrawArray(clearbox, sitem.y, sitem.x, 30, 20);
           
           if(boyLive == 0){
             LCD_DrawArray(boy, boyYPos, boyXPos, 28, 18);
             boyLive = 1;
           }
           else if(girlLive == 0){
             LCD_DrawArray(girl, girlYPos, girlXPos, 28, 18);
             girlLive = 1;
           }
  }
}

void boyCollision(){
  
  unsigned int catched = 0;  // 0: 잡히지 않음
                                           // 1: boy 하나만 잡힘 상태로 바꿀 경우
                                           // 2: boy, girl 모두 잡힘 상태로 바꿀 경우
  int catchedX = 0;
  int catchedY = 0;
  
  for(int i=0; i<numOfWeb; i++){
    if( ((boyXPos-28 <= webs[i].x && boyXPos >= webs[i].x-30) && (boyYPos < webs[i].y+20 && boyYPos+18 > webs[i].y))
         || ((boyYPos+18 >= webs[i].y && boyYPos <= webs[i].y+20) && (boyXPos > webs[i].x-30 && boyXPos-28 < webs[i].x)) ){
           catched = 1;
           catchedX = webs[i].x;
           catchedY = webs[i].y;
           webs[i].x = 0;
           webs[i].y = 0;
           break;
    }
  }
  
  for(int i=0; i<numOfSpider; i++){
    if( ((boyXPos-28 <= spiders[i].x && boyXPos >= spiders[i].x-30) && (boyYPos < spiders[i].y+20 && boyYPos+18 > spiders[i].y))
         || ((boyYPos+18 >= spiders[i].y && boyYPos <= spiders[i].y+20) && (boyXPos > spiders[i].x-30 && boyXPos-28 < spiders[i].x)) ){
           if(superMode == 1){
             LCD_DrawArray(clearbox, spiders[i].y, spiders[i].x, 30, 20);
             spiders[i].x = 0;
             spiders[i].y = 0;
           }
           else{
             catched = 2;
             catchedX = spiders[i].x;
             catchedY = spiders[i].y;
           }
           
           break;
    }
  }
  
  if(catched >= 1){
    LCD_DrawArray(blank, boyYPos, boyXPos, 30, 20);
    LCD_DrawArray(boycatched, catchedY, catchedX, 30, 20);
    
    boyXPos = catchedX;
    boyYPos = catchedY;
    
    beforeBoyDir = 0;
    boyDir = 0;
    
    boyLive = 0;
  }
  if(catched >= 2){
    LCD_DrawArray(blank, girlYPos, girlXPos, 30, 20);
    LCD_DrawArray(girlcatched, girlYPos, girlXPos, 30, 20);
    
    beforeGirlDir = 0;
    girlDir = 0;
    
    girlLive = 0;
  }
  
  if(superMode == 0)
    setSuperState();
}

void boyMove(){
  
    unsigned int canMove = 1;
  
    if(boyDir == 1){  // UP
    
    for(int i=0; i<numOfWall; i++){
      if((boyYPos <= walls[i].y+20 && boyYPos > walls[i].y+19)
         && (boyXPos <= walls[i].x && boyXPos > walls[i].x-30)){
        canMove = 0;
        break;
      }
    }
    
    if(canMove == 1){
      boyYPos -= playerSpeed;
      LCD_DrawArray(boyup1, boyYPos, boyXPos, 28, 18);
    }
  }
  else if(boyDir == 2){  // DOWN
    
    for(int i=0; i<numOfWall; i++){
      if((boyYPos+20 >= walls[i].y && boyYPos+19 < walls[i].y)
         && (boyXPos <= walls[i].x && boyXPos > walls[i].x-30)){
        canMove = 0;
        break;
      }
    }
    
    if(canMove == 1){
      boyYPos += playerSpeed;
      LCD_DrawArray(boydown1, boyYPos, boyXPos, 28, 18);
    }
  }
  else if(boyDir == 3){  // LEFT
    
    for(int i=0; i<numOfWall; i++){
      if((boyXPos >= walls[i].x-30 && boyXPos < walls[i].x-29)
         && (boyYPos >= walls[i].y && boyYPos < walls[i].y+20)){
        canMove = 0;
        break;
      }
    }
    
    if(canMove == 1){
      boyXPos += playerSpeed;
      LCD_DrawArray(boyleft1, boyYPos, boyXPos, 28, 18);
    }
  }
  else if(boyDir == 4){  // RIGHT
    
    for(int i=0; i<numOfWall; i++){
      if((boyXPos-30 <= walls[i].x && boyXPos-29 > walls[i].x)
         && (boyYPos >= walls[i].y && boyYPos < walls[i].y+20)){
        canMove = 0;
        break;
      }
    }
    
    if(canMove == 1){
      boyXPos -= playerSpeed;
      LCD_DrawArray(boyright1, boyYPos, boyXPos, 28, 18);
    }
  }
}

void girlCollision(){

  unsigned int catched = 0;  // 0: 잡히지 않음
                                           // 1: girl 하나만 잡힘 상태로 바꿀 경우
                                           // 2: girl, girl 모두 잡힘 상태로 바꿀 경우
  int catchedX = 0;
  int catchedY = 0;
  
  for(int i=0; i<numOfWeb; i++){
    if( ((girlXPos-28 <= webs[i].x && girlXPos >= webs[i].x-30) && (girlYPos < webs[i].y+20 && girlYPos+18 > webs[i].y))
         || ((girlYPos+18 >= webs[i].y && girlYPos <= webs[i].y+20) && (girlXPos > webs[i].x-30 && girlXPos-28 < webs[i].x)) ){
           catched = 1;
           catchedX = webs[i].x;
           catchedY = webs[i].y;
           webs[i].x = 0;
           webs[i].y = 0;
           break;
    }
  }
  
  for(int i=0; i<numOfSpider; i++){
    if( ((girlXPos-28 <= spiders[i].x && girlXPos >= spiders[i].x-30) && (girlYPos < spiders[i].y+20 && girlYPos+18 > spiders[i].y))
         || ((girlYPos+18 >= spiders[i].y && girlYPos <= spiders[i].y+20) && (girlXPos > spiders[i].x-30 && girlXPos-28 < spiders[i].x)) ){
           if(superMode == 1){
             LCD_DrawArray(clearbox, spiders[i].y, spiders[i].x, 30, 20);
             spiders[i].x = 0;
             spiders[i].y = 0;
           }
           else{
             catched = 2;
             catchedX = spiders[i].x;
             catchedY = spiders[i].y;
           }
           
           break;
    }
  }
  
  if(catched >= 1){
    LCD_DrawArray(blank, girlYPos, girlXPos, 30, 20);
    LCD_DrawArray(girlcatched, catchedY, catchedX, 30, 20);
    
    girlXPos = catchedX;
    girlYPos = catchedY;
    
    beforeGirlDir = 0;
    girlDir = 0;
    
    girlLive = 0;
  }
  if(catched >= 2){
    LCD_DrawArray(blank, girlYPos, girlXPos, 30, 20);
    LCD_DrawArray(girlcatched, girlYPos, girlXPos, 30, 20);
    
    beforeGirlDir = 0;
    girlDir = 0;
    
    girlLive = 0;
  }
  
  if(superMode == 0)
    setSuperState();
}

void girlMove(){
  
    unsigned int canMove = 1;
  
    if(girlDir == 1){  // UP
    
    for(int i=0; i<numOfWall; i++){
      if((girlYPos <= walls[i].y+20 && girlYPos > walls[i].y+19)
         && (girlXPos <= walls[i].x && girlXPos > walls[i].x-30)){
        canMove = 0;
        break;
      }
    }
    
    if(canMove == 1){
      girlYPos -= playerSpeed;
      LCD_DrawArray(girlup1, girlYPos, girlXPos, 28, 18);
    }
  }
  else if(girlDir == 2){  // DOWN
    
    for(int i=0; i<numOfWall; i++){
      if((girlYPos+20 >= walls[i].y && girlYPos+19 < walls[i].y)
         && (girlXPos <= walls[i].x && girlXPos > walls[i].x-30)){
        canMove = 0;
        break;
      }
    }
    
    if(canMove == 1){
      girlYPos += playerSpeed;
      LCD_DrawArray(girldown1, girlYPos, girlXPos, 28, 18);
    }
  }
  else if(girlDir == 3){  // LEFT
    
    for(int i=0; i<numOfWall; i++){
      if((girlXPos >= walls[i].x-30 && girlXPos < walls[i].x-29)
         && (girlYPos >= walls[i].y && girlYPos < walls[i].y+20)){
        canMove = 0;
        break;
      }
    }
    
    if(canMove == 1){
      girlXPos += playerSpeed;
      LCD_DrawArray(girlleft1, girlYPos, girlXPos, 28, 18);
    }
  }
  else if(girlDir == 4){  // RIGHT
    
    for(int i=0; i<numOfWall; i++){
      if((girlXPos-30 <= walls[i].x && girlXPos-29 > walls[i].x)
         && (girlYPos >= walls[i].y && girlYPos < walls[i].y+20)){
        canMove = 0;
        break;
      }
    }
    
    if(canMove == 1){
      girlXPos -= playerSpeed;
      LCD_DrawArray(girlright1, girlYPos, girlXPos, 28, 18);
    }
  }
}

void removeObject(int x, int y, int direction){
  
  int Xpos = x;
  int Ypos = y;
  
  if(direction == 1)
    Ypos = y - 20;
  else if(direction == 2)
    Ypos = y + 20;
  else if(direction == 3)
    Xpos = x + 20;
  else if(direction == 4)
    Xpos = x - 30;
  
  if((boyXPos >= Xpos-10 && boyXPos <= Xpos+10)
       && (boyYPos >= Ypos-10 && boyYPos <= Ypos+10)
       && boyLive == 0){
         LCD_DrawArray(clearbox, boyYPos, boyXPos, 30, 20);
         LCD_DrawArray(boy, boyYPos, boyXPos, 28, 18);
         boyLive = 1;
  }
  else if((girlXPos >= Xpos-10 && girlXPos <= Xpos+10)
       && (girlYPos >= Ypos-10 && girlYPos <= Ypos+10)
       && girlLive == 0){
         LCD_DrawArray(clearbox, girlYPos, girlXPos, 30, 20);
         LCD_DrawArray(girl, girlYPos, girlXPos, 28, 18);
         girlLive = 1;
  }
  
  unsigned int webRemoved = 0;
  
  for(int i=0; i<numOfWeb; i++){
    if((webs[i].x >= Xpos-10 && webs[i].x <= Xpos+10)
       && (webs[i].y >= Ypos-10 && webs[i].y <= Ypos+10)){
         LCD_DrawArray(clearbox, webs[i].y, webs[i].x, 30, 20);
         webs[i].x = 0;
         webs[i].y = 0;
         webRemoved = 1;
         break;
    }
  }
  
  if(webRemoved == 1){
    unsigned int allRemoved = 1;
    
    for(int i=0; i<numOfWeb; i++){
      if(webs[i].x != 0 && webs[i].y != 0){
        allRemoved = 0;
        break;
      }
    }
    
    if(allRemoved == 1){
      LCD_DrawArray(fastitem, sitem.y, sitem.x, 30, 20);
      superItem = 1;
    }
  }
  
  for(int i=0; i<numOfSpider; i++){
    if((spiders[i].x >= Xpos-10 && spiders[i].x <= Xpos+10)
       && (spiders[i].y >= Ypos-10 && spiders[i].y <= Ypos+10)){
         LCD_DrawArray(clearbox, spiders[i].y, spiders[i].x, 30, 20);
         spiders[i].x = 0;
         spiders[i].y = 0;
         break;
    }
  }
}

void setShootState(){
  shootState = 1 - shootState;
        
  if(shootState == 1){  // 사격을 시작한다.
    
    if(boyLive == 1){
      if(beforeBoyDir == 1)
        LCD_DrawArray(boyshootup, boyYPos-(28-18), boyXPos, 28, 28);
      else if(beforeBoyDir == 2)
        LCD_DrawArray(boyshootdown, boyYPos, boyXPos, 28, 25);
      else if(beforeBoyDir == 3)
        LCD_DrawArray(boyshootleft, boyYPos, boyXPos+(45-28), 45, 20);
      else if(beforeBoyDir == 4)
        LCD_DrawArray(boyshootright, boyYPos, boyXPos, 45, 20);
      
      removeObject(boyXPos, boyYPos, beforeBoyDir);
    }
          
    if(girlLive == 1){
      if(beforeGirlDir == 1)
        LCD_DrawArray(girlshootup, girlYPos-(28-18), girlXPos, 28, 28);
      else if(beforeGirlDir == 2)
        LCD_DrawArray(girlshootdown, girlYPos, girlXPos, 28, 26);
      else if(beforeGirlDir == 3)
        LCD_DrawArray(girlshootleft, girlYPos, girlXPos+(45-28), 45, 20);
      else if(beforeGirlDir == 4)
        LCD_DrawArray(girlshootright, girlYPos, girlXPos, 45, 20);
      
      removeObject(girlXPos, girlYPos, beforeGirlDir);
    }
  }
  else{  // 사격을 중지한다.
    if(boyLive == 1){
      if(beforeBoyDir == 1)
        LCD_DrawArray(clearupdown, boyYPos-(28-18), boyXPos, 28, 28);
      else if(beforeBoyDir == 2)
        LCD_DrawArray(clearupdown, boyYPos, boyXPos, 28, 28);
      else if(beforeBoyDir == 3)
        LCD_DrawArray(clearleftright, boyYPos, boyXPos+(45-28), 45, 20);
      else if(beforeBoyDir == 4)
        LCD_DrawArray(clearleftright, boyYPos, boyXPos, 45, 20);
      
      LCD_DrawArray(boy, boyYPos, boyXPos, 28, 18);
    }
          
    if(girlLive == 1){
      if(beforeGirlDir == 1){
        LCD_DrawArray(clearupdown, girlYPos-(28-18), girlXPos, 28, 28);
      }
      else if(beforeGirlDir == 2){
        LCD_DrawArray(clearupdown, girlYPos, girlXPos, 28, 28);
      }
      else if(beforeGirlDir == 3){
        LCD_DrawArray(clearleftright, girlYPos, girlXPos+(45-28), 45, 20);
      }
      else if(beforeGirlDir == 4){
        LCD_DrawArray(clearleftright, girlYPos, girlXPos, 45, 20);
      }
      
      LCD_DrawArray(girl, girlYPos, girlXPos, 28, 18);
    }
  }
}

void termProject(){
   
  boardSet();
  
  initialize();
  
  while(1)
  {
    if(victory == 0)
      isVictory();
     
    if(victory == 0){
      if(boyLive == 1)
        boyCollision();
      
      if(boyLive == 1)
        boyMove();
      
      if(girlLive == 1)
        girlCollision();
      
      if(girlLive == 1)
        girlMove();
      
      enemyMove();
      
      if(gExti[0])  // right key
      {
        if(boyLive == 1){
          boyDir = 4;
          beforeBoyDir = 4;
        }
        
        if(girlLive == 1){
          girlDir = 3;
          beforeGirlDir = 3;
        }
        
        gExti[0] = 0;
      }
      else if(gExti[1])  // left key
      {
        if(boyLive == 1){
          boyDir = 3;
          beforeBoyDir = 3;
        }
        
        if(girlLive == 1){
          girlDir = 4;
          beforeGirlDir = 4;
        }
        
        gExti[1] = 0;
      }
      else if(gExti[8])  // up key
      {
        if(boyLive == 1){
          boyDir = 1;
          beforeBoyDir = 1;
        }
        
        if(girlLive == 1){
          girlDir = 1;
          beforeGirlDir = 1;
        }
          
        gExti[8] = 0;
      }
      else if(gExti[14])  // down key
      {
        if(boyLive == 1){
          boyDir = 2;
          beforeBoyDir = 2;
        }
        
        if(girlLive == 1){
          girlDir = 2;
          beforeGirlDir = 2;
        }
        
        gExti[14] = 0;
      }
      else if(gExti[12])  // select key
      {
        if(boyLive == 1){
          boyDir = 0;
          LCD_DrawArray(boy, boyYPos, boyXPos, 28, 18);
        }
        
        if(girlLive == 1){
          girlDir = 0;
          LCD_DrawArray(girl, girlYPos, girlXPos, 28, 18);
        }
        
        gExti[12] = 0;
      }
      else if(gExti[9]){  // key
        if(boyDir == 0 && girlDir == 0){
          setShootState();
        }
        
        gExti[9] = 0;
      }
      
      if(Uart_GetKey())
        break;
    }
  }
}

// Test_Practice
void BoardTest()
{
  termProject();
}

/*******************************************************************************
*
*   My Test
*
*******************************************************************************/

void *function[][2]=
{
	(void *)MyGpioPollTest,		"GPIO Polling Test ",

	0,0
};

void MyTest()
{
  // To Add your Function
  BoardTest();
}
