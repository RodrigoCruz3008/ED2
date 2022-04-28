//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   IE3027: Electrónica Digital 2 - 2019
*/
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"
#include "pitches.h"
#include <SPI.h>
#include <SD.h>

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};

int seleccion;
File image;
//***************************************************************************************************************************************
// LCD and SD Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

int asc_conversion(int a);
void mapping_SD(char c[]);

//Blue Tank Sprites
extern uint8_t tank_blue_up[];
extern uint8_t tank_blue_right[];
extern uint8_t tank_blue_down[];
extern uint8_t tank_blue_left[];
//Red Tank Sprites
extern uint8_t tank_red_up[];
extern uint8_t tank_red_right[];
extern uint8_t tank_red_down[];
extern uint8_t tank_red_left[];

//***************************************************************************************************************************************
// Game Definitioins, Variables & Prototypes
//***************************************************************************************************************************************
#define TIVA_PUSH1 PF_4
#define TIVA_PUSH2 PF_0

int uart2_reciever;
int uart3_reciever;

int game_state = 0;
int menu_slct = 0;
int tiva_push1_antir = 0;
int tiva_push2_antir = 0;
int tiva_push1_r = 0;
int tiva_push2_r = 0;

int buzzerPin = PF_3;

int tank_speed = 1;
int tank_delay = 100;
int tank_x_collision = 0;
int tank_y_collision = 0;

int blue_tank_index = 0;
int blue_tank_state = 0;
int blue_tank_x_position = 0;
int blue_tank_y_position = 0;
int blue_tank_speed = 4;
int blue_tank_counter = 0;
int blue_tank_lives;
int blue_bullet_state = 0;
int blue_bullet_flag = 0;
int blue_bullet_counter;
int blue_bullet_x_position;
int blue_bullet_y_position;
int blue_bullet_collision = 0;
int blue_bullet_wall_collision = 0;

int red_tank_index = 0;
int red_tank_state = 0;
int red_tank_x_position = 0;
int red_tank_y_position = 0;
int red_tank_speed = 4;
int red_tank_counter = 0;
int red_tank_lives;
int red_bullet_state = 0;
int red_bullet_flag = 0;
int red_bullet_counter;
int red_bullet_x_position;
int red_bullet_y_position;
int red_bullet_collision = 0;
int red_bullet_wall_collision = 0;

extern uint8_t menu_btn1[];
extern uint8_t menu_btn2[];
extern uint8_t menu_btn3[];
extern uint8_t explosion[];

String valor_red;
String valor_blue;
String red_lives = "red lives: ";
String blue_lives = "blue lives: ";

int gameOnNoteQuantity = 116;
int gameOnNoteCount = 0;
int gameOnTimeCount = 0;
int gameOnMelody[] = {
  NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_DS5, NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_DS5, NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_DS5, NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_DS5,
  NOTE_FS4, NOTE_A4, NOTE_D5, NOTE_DS5, NOTE_FS4, NOTE_A4, NOTE_D5, NOTE_DS5, NOTE_FS4, NOTE_A4, NOTE_D5, NOTE_DS5, NOTE_FS4, NOTE_A4, NOTE_D5, NOTE_DS5,
  NOTE_F4, NOTE_GS4, NOTE_D5, NOTE_DS5, NOTE_F4, NOTE_GS4, NOTE_D5, NOTE_DS5, NOTE_F4, NOTE_GS4, NOTE_D5, NOTE_DS5, NOTE_F4, NOTE_GS4, NOTE_D5, NOTE_DS5,
  NOTE_E4, NOTE_G4, NOTE_D5, NOTE_DS5, NOTE_E4, NOTE_G4, NOTE_D5, NOTE_DS5, NOTE_E4, NOTE_G4, NOTE_D5, NOTE_DS5, NOTE_E4, NOTE_G4, NOTE_D5, NOTE_DS5,
  NOTE_DS4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_DS4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_DS4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_DS4, NOTE_G4, NOTE_C5, NOTE_D5,
  NOTE_D4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_D4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_D4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_D4, NOTE_G4, NOTE_C5, NOTE_D5,
  NOTE_C3, NOTE_FS3, NOTE_A3, NOTE_C5, NOTE_FS3, NOTE_A3, NOTE_C4, NOTE_DS4, NOTE_A3, NOTE_C4, NOTE_DS4, NOTE_C4, NOTE_DS4, NOTE_FS4, NOTE_DS4, NOTE_FS4, NOTE_A4, NOTE_FS4, NOTE_A4, NOTE_C5
};
int gameOnNoteDurations[] = {
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};


//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  
  SPI.setModule(0);
  pinMode(PA_3, OUTPUT);                              //Chip Select as OUTPUT
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);
  
  //SD Card Setup
  Serial.print("Initializing SD card...");
  if (!SD.begin(PA_3)) {                              //Inicialización
    Serial.println("initialization failed!"); 
    return;
  }
  Serial.println("initialization done.");
  //Se abre dentro de la ubicación principal
  image = SD.open("/");
  //se abre el directorio (el de el File image)
  printDirectory(image, 0);

  pinMode(TIVA_PUSH1, INPUT_PULLUP);
  pinMode(TIVA_PUSH2, INPUT_PULLUP);
  
}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  //Game State = 0 => Press any button screen
  //Game State = 1 => Menu screen
  //Game State = 2 => Game Active
  //Game State = 3 => Tutorial Screen
  //Game State = 4 => Winner Screen
  
  switch (game_state) {
    ////////////////////////////////////Main Screen State////////////////////////////////////
    case 0:
      delay(200);
      Serial2.write("0");
      mapping_SD("SCREEN~2.TXT");
      blue_tank_x_position = 50;                  //Starting position for animation
      blue_tank_y_position = 130;
      blue_tank_state = 0;
      red_tank_x_position = 247;                  //Starting position for animation
      red_tank_y_position = 165;
      red_tank_state = 2;
      Serial2.write("2");
      
      while (game_state == 0) {
        tiva_push1_r = digitalRead(TIVA_PUSH1);
        tiva_push2_r = digitalRead(TIVA_PUSH2);
        if (tiva_push1_r == 0 || tiva_push2_r == 0) {
          game_state = 1;
        }
        //Sprite Limits
        if (blue_tank_index == 3) {
          blue_tank_index = 0;
        }
        if (red_tank_index == 3) {
          red_tank_index = 0;
        }
        //Main Screen Animation
        switch (blue_tank_state) {
          case 0:
            Move_Blue_Tank(0, 0);
            if (blue_tank_x_position >= 247){blue_tank_state = 1;}
          break; 
          case 1:
            Move_Blue_Tank(1, 0);
            if (blue_tank_y_position >= 165){blue_tank_state = 2;}
          break;
          case 2:
            Move_Blue_Tank(0, 1);
            if (blue_tank_x_position <= 50){blue_tank_state = 3;}
          break;
          case 3:
            Move_Blue_Tank(1, 1);
            if (blue_tank_y_position <= 130){blue_tank_state = 0;}
          break;
        }
        switch (red_tank_state) {
          case 0:
            Move_Red_Tank(0, 0);
            if (red_tank_x_position >= 247){red_tank_state = 1;}
          break; 
          case 1:
            Move_Red_Tank(1, 0);
            if (red_tank_y_position >= 165){red_tank_state = 2;}
          break;
          case 2:
            Move_Red_Tank(0, 1);
            if (red_tank_x_position <= 50){red_tank_state = 3;}
          break;
          case 3:
            Move_Red_Tank(1, 1);
            if (red_tank_y_position <= 130){red_tank_state = 0;}
          break;
        }
        delay(tank_delay);
      }
    break;
    
    ////////////////////////////////////Game Menu State////////////////////////////////////
    case 1:
      Print_Menu();                                                 //Se imprime el menú
      delay(1000);
      menu_slct = 0;
      Menu_Select(menu_slct, 0xFFE0);                               //Imprimir la caja de selección
      while (game_state == 1) {
        tiva_push1_r = digitalRead(TIVA_PUSH1);                     //Leer los botones de la TIVA para controlar la selección
        tiva_push2_r = digitalRead(TIVA_PUSH2);
        if (tiva_push1_r == 0) {tiva_push1_antir = 1;}              //antirrebotes de la TIVA
        if (tiva_push2_r == 0) {tiva_push2_antir = 1;}        
        if (tiva_push1_r == 1 && tiva_push1_antir == 1) {           //Si se presiona el PUSH 1...
          Menu_Select(menu_slct, 0x104B);                           //Imprimir la caja de la nueva selección
          menu_slct++;                                              //
          tiva_push1_antir = 0;
        }
        if (tiva_push2_r == 1 && tiva_push2_antir == 1) {
          tiva_push2_antir = 0;
          switch (menu_slct) {
            case 0:
              Serial2.write("0");
              game_state = 2;
            break;
            case 1:
              game_state = 3;
            break;
            case 2:
              game_state = 0;
            break;
          }
        }
        if (menu_slct >= 3) {
          menu_slct = 0;
        }
      }
    break;

    ////////////////////////////////////Game On State////////////////////////////////////
    case 2:
    red_tank_lives = 3;
    blue_tank_lives = 3;
    Restart_Game();
    Serial2.write("1");
    Serial.print("Sending song: Zelda");
    while (game_state == 2) {
      uart2_reciever = 0;
      uart3_reciever = 0;
      if (blue_tank_index == 3) {
        blue_tank_index = 0;
      }
      if (red_tank_index == 3) {
        red_tank_index = 0;
      }
      //Tank_Collision_Check();
      if (Serial2.available()) {
        uart2_reciever = Serial2.read();
        Serial.println(uart2_reciever);
      }
      if (uart2_reciever == 51 && blue_tank_speed == blue_tank_counter && tank_x_collision == 0 && blue_tank_x_position < 292) {
        Move_Blue_Tank(0, 0);
        blue_tank_state = 0;
      } else if (uart2_reciever == 52 && blue_tank_speed == blue_tank_counter && tank_y_collision == 0 && blue_tank_y_position < 177) {
        Move_Blue_Tank(1, 0);
        blue_tank_state = 1;
      } else if (uart2_reciever == 49 && blue_tank_speed == blue_tank_counter && tank_x_collision == 0 && blue_tank_x_position > 0) {
        Move_Blue_Tank(0, 1);
        blue_tank_state = 2;
      } else if (uart2_reciever == 50 && blue_tank_speed == blue_tank_counter && tank_y_collision == 0 && blue_tank_y_position > 0) {
        Move_Blue_Tank(1, 1);
        blue_tank_state = 3;
      } 
      if (uart2_reciever == 54 && blue_bullet_flag == 0 && (blue_bullet_wall_collision == 0 || blue_bullet_collision == 0)) {
        blue_bullet_state = blue_tank_state;
        Shoot_Blue_Bullet ();
        blue_bullet_flag = 1;
      }
      if (blue_bullet_flag == 1 && (blue_bullet_wall_collision == 0 || blue_bullet_collision == 0)) {
        Move_Blue_Bullet();
        Blue_Bullet_Collision_Check ();
      }
      if (Serial3.available()) {
        uart3_reciever = Serial3.read();
        Serial.println(uart3_reciever);
      }
      if (uart3_reciever == 57 && red_tank_speed == red_tank_counter && tank_x_collision == 0 && red_tank_x_position < 292) {
        Move_Red_Tank(0, 0);
        red_tank_state = 0;
      } else if (uart3_reciever == 97 && red_tank_speed == red_tank_counter && tank_y_collision == 0 && red_tank_y_position < 177) {
        Move_Red_Tank(1, 0);
        red_tank_state = 1;
      } else if (uart3_reciever == 55 && red_tank_speed == red_tank_counter && tank_x_collision == 0 && red_tank_x_position > 0) {
        Move_Red_Tank(0, 1);
        red_tank_state = 2;
      } else if (uart3_reciever == 56 && red_tank_speed == red_tank_counter && tank_y_collision == 0 && red_tank_y_position > 0) {
        Move_Red_Tank(1, 1);
        red_tank_state = 3;
      } 
      if (uart3_reciever == 99 && red_bullet_flag == 0 && (red_bullet_wall_collision == 0 || red_bullet_collision == 0)) {
        red_bullet_state = red_tank_state;
        Shoot_Red_Bullet ();
        red_bullet_flag = 1;
      }
      if (red_bullet_flag == 1 && (red_bullet_wall_collision == 0 || red_bullet_collision == 0)) {
        Move_Red_Bullet();
        Red_Bullet_Collision_Check();
      }
      if (blue_bullet_collision == 1) {
        Erase_Blue_Bullet();
        Explosion_Animation(red_tank_x_position, red_tank_y_position);
        red_tank_lives--;
        if(red_tank_lives == 0){
          game_state = 4;
        } else {
          Restart_Game();
        }
        red_bullet_collision = 0;
      }
      if (blue_bullet_wall_collision == 1) {
        Erase_Blue_Bullet();
        blue_bullet_wall_collision = 0;
        blue_bullet_flag = 0;
      }
      if (red_bullet_collision == 1) {
        Erase_Red_Bullet();
        Explosion_Animation(blue_tank_x_position, blue_tank_y_position);
        blue_tank_lives--;
        if (blue_tank_lives == 0){
          game_state = 4;
        } else {
          Restart_Game();
        }
        blue_bullet_collision = 0;
      }
      if (red_bullet_wall_collision == 1) {
        Erase_Red_Bullet();
        red_bullet_wall_collision = 0;
        red_bullet_flag = 0;
      }
      if (blue_tank_counter == blue_tank_speed){blue_tank_counter = 0;}
      blue_tank_counter++;
      if (red_tank_counter == red_tank_speed){red_tank_counter = 0;}
      red_tank_counter++;
      delay(10);
    }
    break;

    ////////////////////////////////////Tutorial State////////////////////////////////////
    case 3:
      mapping_SD("SCREEN~1.txt");
      while (game_state == 3) {
        tiva_push1_r = digitalRead(TIVA_PUSH1);
        tiva_push2_r = digitalRead(TIVA_PUSH2);
        if (tiva_push1_r == 0 || tiva_push2_r == 0) {
          game_state = 1;
        }
      }
    break;

    case 4:
      Serial1.write("0");
      delay (3000);
      if (blue_bullet_collision == 1) {
        mapping_SD("WINNER~3.txt");
      } else if (red_bullet_collision == 1) {
        mapping_SD("WINNER~1.txt");
      }
      delay(5000);
      game_state = 0;
    break;
    
    default:
    game_state = 0;
    break;
  }
}
//***************************************************************************************************************************************
// Game Functions
//***************************************************************************************************************************************
///////////////Menu Functions///////////////
//int btn_x, btn_y, btn_w, btn_h, btn_gap, slct_bx_w;
unsigned int btn_x = 60; 
unsigned int btn_y = 70; 
unsigned int btn_w = 200; 
unsigned int btn_h = 40; 
int btn_gap = 10;
int slct_bx_w = 4;

//Print Menu
void Print_Menu (void){ 
  //Print Background
  mapping_SD("SCREEN~3.txt");
  //Print Buttons
  LCD_Bitmap(btn_x, btn_y + 0 * (btn_h + btn_gap), btn_w, btn_h, menu_btn1);
  LCD_Bitmap(btn_x, btn_y + 1 * (btn_h + btn_gap), btn_w, btn_h, menu_btn2);
  LCD_Bitmap(btn_x, btn_y + 2 * (btn_h + btn_gap), btn_w, btn_h, menu_btn3);
}
//Menu Select Indicator
void Menu_Select (int btn_slct, unsigned int color) {
  if (btn_slct == 0) {
    for (int i = 0; i < slct_bx_w; i++){
      H_line(btn_x - slct_bx_w, btn_y - slct_bx_w + i, btn_w + (slct_bx_w * 2), color);
    }
    for (int i = 0; i < slct_bx_w; i++) {
      V_line(btn_x - slct_bx_w + i, btn_y, btn_h, color);
    }
    for (int i = 0; i <= slct_bx_w; i++) {
      V_line(btn_w + btn_x + i, btn_y, btn_h, color);
    }
    for (int i = 0; i < slct_bx_w; i++){
      H_line(btn_x - slct_bx_w, btn_y + btn_h + i, btn_w + (slct_bx_w * 2), color);
    }
  } else {
    for (int i = 0; i < slct_bx_w; i++){
      H_line(btn_x - slct_bx_w, btn_y - slct_bx_w + (btn_slct * (btn_h + btn_gap)) + i, btn_w + (slct_bx_w * 2), color);
    }
    for (int i = 0; i < slct_bx_w; i++) {
      V_line(btn_x - slct_bx_w + i, btn_y + (btn_slct * (btn_h + btn_gap)), btn_h, color);
    }
    for (int i = 0; i <= slct_bx_w; i++) {
      V_line(btn_w + btn_x + i, btn_y + (btn_slct * (btn_h + btn_gap)), btn_h, color);
    }
    for (int i = 0; i < slct_bx_w; i++){
      H_line(btn_x - slct_bx_w, btn_y + btn_h + i + (btn_slct * (btn_h + btn_gap)), btn_w + (slct_bx_w * 2), color);
    }
  }
}

///////////////Tutorial Functions///////////////
void Print_Tutorial (void) {
  FillRect(0, 0, 320, 240, 0x104B);
  String tutorial_txt = "Tutorial";
  LCD_Print(tutorial_txt, 50, 50, 2, 0xFCC0, 0x104B);
}

///////////////Game Functions///////////////
//Print Game
void Restart_Game(void){
  FillRect(0,0,320,240, 0xAF40);
  blue_tank_state = 0;
  red_tank_state = 2;
  blue_tank_x_position = 0;
  blue_tank_y_position = 0;
  red_tank_x_position = 292;
  red_tank_y_position = 177;
  FillRect(0, 205, 320, 35, 0x2817);

  LCD_Sprite(5, 240 - 28, 23, 23, tank_blue_up, 3, red_tank_index, 0, 0);
  LCD_Sprite(320 - 28, 240 - 28, 23, 23, tank_red_up, 3, blue_tank_index, 0, 0);

  LCD_Sprite(blue_tank_x_position, blue_tank_y_position, 23, 23, tank_blue_right, 3, blue_tank_index, 0, 0);
  LCD_Sprite(red_tank_x_position, red_tank_y_position, 23, 23, tank_red_left, 3, red_tank_index, 0, 0);
  Print_Tank_Lives(blue_tank_lives, 1);
  Print_Tank_Lives(red_tank_lives, 2);
}

//Blue Tank Movement
void Move_Blue_Tank (int mov_xy, int mov_posneg) {
  if (mov_xy == 0){
    if (mov_posneg == 0){
      blue_tank_x_position = blue_tank_x_position + tank_speed;
      LCD_Sprite(blue_tank_x_position, blue_tank_y_position, 23, 23, tank_blue_right, 3, blue_tank_index, 0, 0);
      for (int i = 1; i <= tank_speed; i++) {
        V_line(blue_tank_x_position - i, blue_tank_y_position, 23, 0xFFA8);
      }
    } else if (mov_posneg == 1) {
      blue_tank_x_position = blue_tank_x_position - tank_speed;
      LCD_Sprite(blue_tank_x_position, blue_tank_y_position, 23, 23, tank_blue_left, 3, blue_tank_index, 0, 0);
      for (int i = 1; i <= tank_speed; i++) {
        V_line(blue_tank_x_position + 23 + i, blue_tank_y_position, 23, 0xFFA8);
      }
    }
  } else if (mov_xy == 1){
    if (mov_posneg == 0){
      blue_tank_y_position = blue_tank_y_position + tank_speed;
      LCD_Sprite(blue_tank_x_position, blue_tank_y_position, 23, 23, tank_blue_down, 3, blue_tank_index, 0, 0);
      for (int i = 1; i <= tank_speed; i++) {
        H_line(blue_tank_x_position, blue_tank_y_position - i, 23, 0xFFA8);
      }
    } else if (mov_posneg == 1) {
      blue_tank_y_position = blue_tank_y_position - tank_speed;
      LCD_Sprite(blue_tank_x_position, blue_tank_y_position, 23, 23, tank_blue_up, 3, blue_tank_index, 0, 0);
      for (int i = 1; i <= tank_speed; i++) {
        H_line(blue_tank_x_position, blue_tank_y_position + 23 + i, 23, 0xFFA8);
      }
    }
  }
  blue_tank_index++;
}

//Red Tank Movement
void Move_Red_Tank (int mov_xy, int mov_posneg) {
  if (mov_xy == 0){
    if (mov_posneg == 0){
      red_tank_x_position = red_tank_x_position + tank_speed;
      LCD_Sprite(red_tank_x_position, red_tank_y_position, 23, 23, tank_red_right, 3, red_tank_index, 0, 0);
      for (int i = 1; i <= tank_speed; i++) {
        V_line(red_tank_x_position - i, red_tank_y_position, 23, 0xFFA8);
      }
    } else if (mov_posneg == 1) {
      red_tank_x_position = red_tank_x_position - tank_speed;
      LCD_Sprite(red_tank_x_position, red_tank_y_position, 23, 23, tank_red_left, 3, red_tank_index, 0, 0);
      for (int i = 1; i <= tank_speed; i++) {
        V_line(red_tank_x_position + 23 + i, red_tank_y_position, 23, 0xFFA8);
      }
    }
  } else if (mov_xy == 1){
    if (mov_posneg == 0){
      red_tank_y_position = red_tank_y_position + tank_speed;
      LCD_Sprite(red_tank_x_position, red_tank_y_position, 23, 23, tank_red_down, 3, red_tank_index, 0, 0);
      for (int i = 1; i <= tank_speed; i++) {
        H_line(red_tank_x_position, red_tank_y_position - i, 23, 0xFFA8);
      }
    } else if (mov_posneg == 1) {
      red_tank_y_position = red_tank_y_position - tank_speed;
      LCD_Sprite(red_tank_x_position, red_tank_y_position, 23, 23, tank_red_up, 3, red_tank_index, 0, 0);
      for (int i = 1; i <= tank_speed; i++) {
        H_line(red_tank_x_position, red_tank_y_position + 23 + i, 23, 0xFFA8);
      }
    }
  }
  red_tank_index++;
}

void Shoot_Blue_Bullet () {
  blue_bullet_counter = 0;
  if (blue_bullet_state == 0) {
    blue_bullet_x_position = blue_tank_x_position + 26;
    blue_bullet_y_position = blue_tank_y_position + 10;
  } else if (blue_bullet_state == 1) {
    blue_bullet_x_position = blue_tank_x_position + 10;
    blue_bullet_y_position = blue_tank_y_position + 26;
  } else if (blue_bullet_state == 2) {
    blue_bullet_x_position = blue_tank_x_position - 3;
    blue_bullet_y_position = blue_tank_y_position + 10;
  } else if (blue_bullet_state == 3) {
    blue_bullet_x_position = blue_tank_x_position + 10;
    blue_bullet_y_position = blue_tank_y_position - 3;
  }
}

void Shoot_Red_Bullet () {
  red_bullet_counter = 0;
  if (red_bullet_state == 0) {
    red_bullet_x_position = red_tank_x_position + 26;
    red_bullet_y_position = red_tank_y_position + 10;
  } else if (red_bullet_state == 1) {
    red_bullet_x_position = red_tank_x_position + 10;
    red_bullet_y_position = red_tank_y_position + 26;
  } else if (red_bullet_state == 2) {
    red_bullet_x_position = red_tank_x_position - 3;
    red_bullet_y_position = red_tank_y_position + 10;
  } else if (red_bullet_state == 3) {
    red_bullet_x_position = red_tank_x_position + 10;
    red_bullet_y_position = red_tank_y_position - 3;
  }
}

void Move_Blue_Bullet () {
  int bullet_width = 3;
  int bullet_height = 6;
  if (blue_bullet_state == 0) {
    if (blue_bullet_collision == 0) {
      FillRect(blue_bullet_x_position + 1, blue_bullet_y_position, bullet_height, bullet_width, 0x0000);
      V_line(blue_bullet_x_position - 1, blue_bullet_y_position, bullet_width, 0xFC20);
      V_line(blue_bullet_x_position - 2, blue_bullet_y_position, bullet_width, 0xFC20);
      V_line(blue_bullet_x_position - 3, blue_bullet_y_position, bullet_width, 0xFC20);
      V_line(blue_bullet_x_position - 4, blue_bullet_y_position, bullet_width, 0xFFA8);
      blue_bullet_x_position++;
    } else {blue_bullet_flag = 0;}
    blue_bullet_counter++;
  } 
  if (blue_bullet_state == 1) {
    if (blue_bullet_collision == 0) {
      FillRect(blue_bullet_x_position, blue_bullet_y_position + 1, bullet_width, bullet_height, 0x0000);
      H_line(blue_bullet_x_position, blue_bullet_y_position - 1, bullet_width, 0xFC20);
      H_line(blue_bullet_x_position, blue_bullet_y_position - 2, bullet_width, 0xFC20);
      H_line(blue_bullet_x_position, blue_bullet_y_position - 3, bullet_width, 0xFC20);
      H_line(blue_bullet_x_position, blue_bullet_y_position - 4, bullet_width, 0xFFA8);
      blue_bullet_y_position++;
    } else {blue_bullet_flag = 0;}
    blue_bullet_counter++;
  }
  if (blue_bullet_state == 2) {
    if (blue_bullet_collision == 0) {
      FillRect(blue_bullet_x_position - bullet_height - 4, blue_bullet_y_position, bullet_height, bullet_width, 0x0000);
      V_line(blue_bullet_x_position - 4, blue_bullet_y_position, bullet_width, 0xFC20);
      V_line(blue_bullet_x_position - 3, blue_bullet_y_position, bullet_width, 0xFC20);
      V_line(blue_bullet_x_position - 2, blue_bullet_y_position, bullet_width, 0xFC20);
      V_line(blue_bullet_x_position - 1, blue_bullet_y_position, bullet_width, 0xFFA8);
      blue_bullet_x_position--;
    } else {blue_bullet_flag = 0;}
    blue_bullet_counter++;
  }
  if (blue_bullet_state == 3) {
    if (blue_bullet_collision == 0) {
      FillRect(blue_bullet_x_position, blue_bullet_y_position  - bullet_height - 4, bullet_width, bullet_height, 0x0000);
      H_line(blue_bullet_x_position, blue_bullet_y_position - 4, bullet_width, 0xFC20);
      H_line(blue_bullet_x_position, blue_bullet_y_position - 3, bullet_width, 0xFC20);
      H_line(blue_bullet_x_position, blue_bullet_y_position - 2, bullet_width, 0xFC20);
      H_line(blue_bullet_x_position, blue_bullet_y_position - 1, bullet_width, 0xFFA8);
      blue_bullet_y_position--;
    } else {blue_bullet_flag = 0;}
  }
}

void Move_Red_Bullet () {
  int bullet_width = 3;
  int bullet_height = 6;
  if (red_bullet_state == 0) {
    if (red_bullet_collision == 0) {
      FillRect(red_bullet_x_position + 1, red_bullet_y_position, bullet_height, bullet_width, 0x0000);
      V_line(red_bullet_x_position - 1, red_bullet_y_position, bullet_width, 0xFC20);
      V_line(red_bullet_x_position - 2, red_bullet_y_position, bullet_width, 0xFC20);
      V_line(red_bullet_x_position - 3, red_bullet_y_position, bullet_width, 0xFC20);
      V_line(red_bullet_x_position - 4, red_bullet_y_position, bullet_width, 0xFFA8);
      red_bullet_x_position++;
    } else {red_bullet_flag = 0;}
    red_bullet_counter++;
  } 
  if (red_bullet_state == 1) {
    if (red_bullet_collision == 0) {
      FillRect(red_bullet_x_position, red_bullet_y_position + 1, bullet_width, bullet_height, 0x0000);
      H_line(red_bullet_x_position, red_bullet_y_position - 1, bullet_width, 0xFC20);
      H_line(red_bullet_x_position, red_bullet_y_position - 2, bullet_width, 0xFC20);
      H_line(red_bullet_x_position, red_bullet_y_position - 3, bullet_width, 0xFC20);
      H_line(red_bullet_x_position, red_bullet_y_position - 4, bullet_width, 0xFFA8);
      red_bullet_y_position++;
    } else {red_bullet_flag = 0;}
    red_bullet_counter++;
  }
  if (red_bullet_state == 2) {
    if (red_bullet_collision == 0) {
      FillRect(red_bullet_x_position - bullet_height - 4, red_bullet_y_position, bullet_height, bullet_width, 0x0000);
      V_line(red_bullet_x_position - 4, red_bullet_y_position, bullet_width, 0xFC20);
      V_line(red_bullet_x_position - 3, red_bullet_y_position , bullet_width, 0xFC20);
      V_line(red_bullet_x_position - 2, red_bullet_y_position, bullet_width, 0xFC20);
      V_line(red_bullet_x_position - 1, red_bullet_y_position, bullet_width, 0xFFA8);
      red_bullet_x_position--;
    } else {red_bullet_flag = 0;}
    red_bullet_counter++;
  }
  if (red_bullet_state == 3) {
    if (red_bullet_collision == 0) {
      FillRect(red_bullet_x_position, red_bullet_y_position - bullet_height - 4, bullet_width, bullet_height, 0x0000);
      H_line(red_bullet_x_position, red_bullet_y_position - 4, bullet_width, 0xFC20);
      H_line(red_bullet_x_position, red_bullet_y_position - 3, bullet_width, 0xFC20);
      H_line(red_bullet_x_position, red_bullet_y_position - 2, bullet_width, 0xFC20);
      H_line(red_bullet_x_position, red_bullet_y_position - 1, bullet_width, 0xFFA8);
      red_bullet_y_position--;
    } else {red_bullet_flag = 0;}
    red_bullet_counter++;
  }
}

void Blue_Bullet_Collision_Check () {
  //return (x1 < x2 + w2) && (x1 + w1 > x2) && (y1 < y2 + h2) && (y1 + h1  > y2);
  int bullet_height = 6;
  int bullet_width = 3;
  int blue_bullet_x_position_b;
  int blue_bullet_y_position_b;
  int red_tank_x_position_b = red_tank_x_position + 23;
  int red_tank_y_position_b = red_tank_y_position+ 23;
  switch(blue_bullet_state){
    case 0:
      blue_bullet_x_position_b = blue_bullet_x_position + bullet_height;
      blue_bullet_y_position_b = blue_bullet_y_position + bullet_width;

      if (blue_bullet_x_position_b == red_tank_x_position && blue_bullet_y_position >= red_tank_y_position && blue_bullet_y_position_b <= red_tank_y_position_b){
        blue_bullet_collision = 1;
      } else {blue_bullet_collision = 0;}
      if (blue_bullet_x_position >= 320) {
        blue_bullet_wall_collision = 1;
      }
    break;

    case 1:
      blue_bullet_x_position_b = blue_bullet_x_position + bullet_width;
      blue_bullet_y_position_b = blue_bullet_y_position + bullet_height;

      if (blue_bullet_y_position_b == red_tank_y_position && blue_bullet_x_position >= red_tank_x_position && blue_bullet_x_position_b <= red_tank_x_position_b) {
        blue_bullet_collision = 1;
      } else {blue_bullet_collision = 0;}
      if (blue_bullet_y_position >= 194) {
        blue_bullet_wall_collision = 1;
      }
    break;
    
    case 2:
      blue_bullet_x_position_b = blue_bullet_x_position + bullet_height;
      blue_bullet_y_position_b = blue_bullet_y_position + bullet_width;

      if (blue_bullet_x_position == red_tank_x_position_b && blue_bullet_y_position >= red_tank_y_position && blue_bullet_y_position_b <= red_tank_y_position_b){
        blue_bullet_collision = 1;
      } else {blue_bullet_collision = 0;}
      if (blue_bullet_x_position <= 0) {
        blue_bullet_wall_collision = 1;
      }
    break;

    case 3:
      blue_bullet_x_position_b = blue_bullet_x_position + bullet_width;
      blue_bullet_y_position_b = blue_bullet_y_position + bullet_height;

      if (blue_bullet_y_position == red_tank_y_position_b && blue_bullet_x_position >= red_tank_x_position && blue_bullet_x_position_b <= red_tank_x_position_b) {
        blue_bullet_collision = 1;
      } else {blue_bullet_collision = 0;}
      if (blue_bullet_y_position <= 0) {
        blue_bullet_wall_collision = 1;
      }
    break;
    
  }
}

void Red_Bullet_Collision_Check () {
  int bullet_height = 6;
  int bullet_width = 3;
  int red_bullet_x_position_b;
  int red_bullet_y_position_b;
  int blue_tank_x_position_b = blue_tank_x_position + 23;
  int blue_tank_y_position_b = blue_tank_y_position+ 23;
  switch(red_bullet_state){
    case 0:
      red_bullet_x_position_b = red_bullet_x_position + bullet_height;
      red_bullet_y_position_b = red_bullet_y_position + bullet_width;

      if (red_bullet_x_position_b == blue_tank_x_position && red_bullet_y_position >= blue_tank_y_position && red_bullet_y_position_b <= blue_tank_y_position_b){
        red_bullet_collision = 1;
      } else {red_bullet_collision = 0;}
      if (red_bullet_x_position >= 320) {
        red_bullet_wall_collision = 1;
      }
    break;

    case 1:
      red_bullet_x_position_b = red_bullet_x_position + bullet_width;
      red_bullet_y_position_b = red_bullet_y_position + bullet_height;

      if (red_bullet_y_position_b == blue_tank_y_position && red_bullet_x_position >= blue_tank_x_position && red_bullet_x_position_b <= blue_tank_x_position_b) {
        red_bullet_collision = 1;
      } else {red_bullet_collision = 0;}
      if (red_bullet_y_position >= 194) {
        red_bullet_wall_collision = 1;
      }
    break;
    
    case 2:
      red_bullet_x_position_b = red_bullet_x_position + bullet_height;
      red_bullet_y_position_b = red_bullet_y_position + bullet_width;

      if (red_bullet_x_position == blue_tank_x_position_b && red_bullet_y_position >= blue_tank_y_position && red_bullet_y_position_b <= blue_tank_y_position_b){
        red_bullet_collision = 1;
      } else {red_bullet_collision = 0;}
      if (red_bullet_x_position <= 0) {
        red_bullet_wall_collision = 1;
      }
    break;

    case 3:
      red_bullet_x_position_b = red_bullet_x_position + bullet_width;
      red_bullet_y_position_b = red_bullet_y_position + bullet_height;

      if (red_bullet_y_position == blue_tank_y_position_b && red_bullet_x_position >= blue_tank_x_position && red_bullet_x_position_b <= blue_tank_x_position_b) {
        red_bullet_collision = 1;
      } else {red_bullet_collision = 0;}
      if (red_bullet_y_position <= 0) {
        red_bullet_wall_collision = 1;
      }
    break;
  }
}

void Erase_Blue_Bullet(void) {
  switch (blue_bullet_state) {
    case 0:
      for (int i = 0; i <= 9; i++) {
        V_line(blue_bullet_x_position - 3 + i, blue_bullet_y_position, 3, 0xFFA8);
      }
    break;
    case 1:
      for (int i = 0; i <= 9; i++) {
        H_line(blue_bullet_x_position, blue_bullet_y_position - 3 + i, 3, 0xFFA8);
      }
    break;
    case 2:
      for (int i = 0; i <= 9; i++) {
        V_line(blue_bullet_x_position + i, blue_bullet_y_position, 3, 0xFFA8);
      }
    break;
    case 3:
      for (int i = 0; i <= 9; i++) {
        H_line(blue_bullet_x_position, blue_bullet_y_position + i, 3, 0xFFA8);
      }
    break;
  }
}

void Erase_Red_Bullet(void){
  switch (red_bullet_state) {
    case 0:
      for (int i = 0; i <= 9; i++) {
        V_line(red_bullet_x_position - 3 + i, red_bullet_y_position, 3, 0xFFA8);
      }
    break;
    case 1:
      for (int i = 0; i <= 9; i++) {
        H_line(red_bullet_x_position, red_bullet_y_position - 3 + i, 3, 0xFFA8);
      }
    break;
    case 2:
      for (int i = 0; i <= 9; i++) {
        V_line(red_bullet_x_position + i, red_bullet_y_position, 3, 0xFFA8);
      }
    break;
    case 3:
      for (int i = 0; i <= 9; i++) {
        H_line(red_bullet_x_position, red_bullet_y_position + i, 3, 0xFFA8);
      }
    break;
  } 
}

void Tank_Collision_Check (void) {
  int blue_tank_x_position_b = blue_tank_x_position + 24;
  int blue_tank_y_position_b = blue_tank_y_position + 24;
  int red_tank_x_position_b = red_tank_x_position + 24;
  int red_tank_y_position_b = red_tank_y_position + 24;
  if (blue_tank_x_position_b == red_tank_x_position && ((blue_tank_y_position < red_tank_y_position && red_tank_y_position < blue_tank_y_position_b) || (red_tank_y_position < blue_tank_y_position && blue_tank_y_position < red_tank_y_position_b))) {
    tank_x_collision = 1;
  } else if (blue_tank_x_position == red_tank_x_position && (blue_tank_y_position < red_tank_y_position < blue_tank_y_position_b || red_tank_y_position < blue_tank_y_position < red_tank_y_position_b)) {
    tank_x_collision = 1;
  } else if (blue_tank_y_position_b == red_tank_y_position && (blue_tank_x_position < red_tank_x_position < blue_tank_y_position_b || red_tank_x_position < blue_tank_x_position < red_tank_x_position_b)) {
    tank_y_collision = 1;
  } else if (blue_tank_y_position == red_tank_y_position_b && (blue_tank_x_position < red_tank_x_position_b < blue_tank_x_position_b || red_tank_x_position < blue_tank_x_position_b < red_tank_x_position_b)) {
    tank_y_collision = 1;
  } 
}

void Print_Tank_Lives(int lives, int choose_tank) {
  String lives_value;
  int message_x_position;
  int message_y_position = 216;
  if (choose_tank == 1) {
    message_x_position = 35;
  } else if (choose_tank == 2) {
    message_x_position = 270;
  }
  switch (lives){
      case 0: 
        lives_value = "0";
        LCD_Print(lives_value, message_x_position, message_y_position, 2, 0xFFFF, 0x2817);
      break;

      case 1: 
        lives_value = "1";
        LCD_Print(lives_value, message_x_position, message_y_position, 2, 0xFFFF, 0x2817);
      break;

      case 2: 
        lives_value = "2";
        LCD_Print(lives_value, message_x_position, message_y_position, 2, 0xFFFF, 0x2817);
      break;
      
      case 3:
        lives_value = "3";
        LCD_Print(lives_value, message_x_position, message_y_position, 2, 0xFFFF, 0x2817);
      break;
      }
}

void Explosion_Animation (int x, int y) {
  for (int i = 0; i < 8; i++) {
    LCD_Sprite(x, y, 23, 23, explosion, 8, i, 0, 0);
    delay(50);
  }
}


//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
  }
*/

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + w;
  y2 = y + h;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = w * h * 2 - 1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);

      //LCD_DATA(bitmap[k]);
      k = k - 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}

int asc_conversion(int a){
  switch(a){
    case 48:
      return 0;
    case 49:
      return 1;
    case 50:
      return 2;
    case 51:
      return 3;
    case 52:
      return 4;
    case 53:
      return 5;
    case 54:
      return 6;
    case 55:
      return 7;
    case 56:
      return 8;
    case 57:
      return 9;
    case 97:
      return 10;
    case 98:
      return 11;
    case 99:
      return 12;
    case 100:
      return 13;
    case 101:
      return 14;
    case 102:
      return 15;
  }
}

void mapping_SD(char c[]){
  image = SD.open(c, FILE_READ);  //se toma el archivo de la imagen
  int hex = 0;
  int valor_1 = 0;
  int valor_2 = 0;
  int mapping = 0;
  int vertical = 0;
  unsigned char mapp[640];  //array para el mapeo

  if (image){
    while(image.available()){
      mapping = 0;
      while (mapping < 640){
        hex = image.read();
        if (hex == 120){
          valor_1 = image.read();
          valor_2 = image.read();
          valor_1 = asc_conversion(valor_1);
          valor_2 = asc_conversion(valor_2);
          mapp[mapping] = valor_1 * 16 + valor_2;
          mapping++;
        }
      }
      LCD_Bitmap(0, vertical, 320, 1, mapp);
      vertical++;
    }
    image.close();
    Serial.println("se cerró");
  }
  else {
    Serial.println("no se puede abrir el archivo");
    image.close();
  } 
}

//Función que nos muestra el contenido de la tarjeta SD 
void printDirectory(File dir, int numTabs) {
   while(true) {
     //Se abre archivo por archivo
     File entry =  dir.openNextFile();  //se crea otro objeto de la clase File y se abre el siguiente archivo (entra a la ubicación principal del directorio)
     if (! entry) {
       // no more files
       break;
     }
     //imprime tabulaciones
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     //imprime el nombre del archivo
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);  //ir imprimiendo los archivos
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC); //imprime tamaño del archivo
     }
     entry.close();
   }
}
