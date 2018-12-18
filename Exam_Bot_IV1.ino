#include <BOT_I.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


int pos = 0;
int Max_Light[12] = {2200, 4100, 4100, 4100, 4100, 2200 , 4100, 4100, 4100, 3000, 3000, 3000};          //ใส่ค่าขณะที่วัดสี ขาว
int Min_Light[12] = {400 , 1500 , 1300 , 1000 , 1400 , 400 , 300 , 350 , 200  , 100 , 100 , 100};       //ใส่ค่าขณะที่วัดสี ดำ

// หุ่นยนต์จะต่อเซ็นเซอร์คือ 

//   ด้านหน้า      A2 A3 A4 A5 A6
//   ด้านหลัง      A1          A7

//และที่วัดระยะทางต่อที่    A8



int ref[12] = {((Max_Light[0]+Min_Light[0])/2)      //หาค่ากลางของ sensor หรือค่าเฉลี่ยนั้นเอง
              ,((Max_Light[1]+Min_Light[1])/2)
              ,((Max_Light[2]+Min_Light[2])/2)
              ,((Max_Light[3]+Min_Light[3])/2)
              ,((Max_Light[4]+Min_Light[4])/2)
              ,((Max_Light[5]+Min_Light[5])/2)
              ,((Max_Light[6]+Min_Light[6])/2)};

int number_Sensor = 7;
int setpoint = 0;
int state_on_Line = 0;
float Kp = 3;
float Ki = 0;
float Kd = 12;
float present_position;
float errors = 0;
float output = 0;
float integral ;
float  derivative ;
float previous_error ;
int Percent_Light[12];
long Position() {
  long sum_value_total = 0;
  long divide_value_total = 0;
  Percent_Light[1] = map(analog(2), Min_Light[1], Max_Light[1], 100, 0);      //เปลี่ยนจากค่าแสงที่อ่านได้เป็น %   ถ้าเป็นสี ดำ = 100 % ขาว = 0 %;
  Percent_Light[2] = map(analog(3), Min_Light[2], Max_Light[2], 100, 0);
  Percent_Light[3] = map(analog(4), Min_Light[3], Max_Light[3], 100, 0);
  Percent_Light[4] = map(analog(5), Min_Light[4], Max_Light[4], 100, 0);
  Percent_Light[5] = map(analog(6), Min_Light[5], Max_Light[5], 100, 0);

  
  sum_value_total =  0 * Percent_Light[1]     +  20 * Percent_Light[2] + 40 * Percent_Light[3]
                     + 60 * Percent_Light[4] + 80 * Percent_Light[5] ;
  divide_value_total = Percent_Light[1] + Percent_Light[2] + Percent_Light[3] + Percent_Light[4] + Percent_Light[5] ;

  if (divide_value_total != 0) {
    return (sum_value_total / divide_value_total) ;
  }
}
void s() {                                      //เดินจับเส้นไปจนถึงจุดตัด
  Run();
  Run_LL();
  motor(1,50);motor(2,50);delay(70);
  motor(1,0);motor(2,0);delay(50);
}
void ss() {                                       //เดินจับเส้นไปจนถึงจุดตัดจุดที่ 2 
  Run();
  Run_LL();
  Run();
  Run_LL();
  motor(1,50);motor(2,50);delay(70);
  motor(1,0);motor(2,0);delay(50);
}
void Run() {                                    //เดินจับเส้นไปจนกว่าเซ็นเซอร์แผงด้านหน้าเจอเส้นดำ
  do  {
    if (analog(2) < ref[1] )state_on_Line = 1;
    if (state_on_Line == 1 && analog(3) < ref[2])state_on_Line = 0;
    if (analog(6) < ref[5] )state_on_Line = 2;
    if (state_on_Line == 2 && analog(5) < ref[4])state_on_Line = 0;
    switch (state_on_Line) {
      case 0: {
          present_position = Position();
        } break;
      case 1: {
          present_position = 5.0;
        } break;
      case 2: {
          present_position = 75.0;
        } break;
    }
    setpoint = 40.0;
    errors = setpoint - present_position;
    integral = integral + errors ;
    derivative = (errors - previous_error) ;
    output = Kp * errors + Ki * integral + Kd * derivative;
    previous_error = errors;
    int speed_max = 60;                                                                 //ตั้งค่าความเร็ว   100 คือมากสุด
    int speed_motor_forword = speed_max + output;
    int speed_motor_backward = speed_max - output;
    if (speed_motor_forword > 100)speed_motor_forword = 100;
    if (speed_motor_forword < 0 )speed_motor_forword = 0;
    if (speed_motor_backward > 100)speed_motor_backward = 100;
    if (speed_motor_backward < 0 )speed_motor_backward = 0;
    if (Position() > 0) {
      motor(2, speed_motor_forword);
      motor(1, speed_motor_backward);
    }
    else {
      motor(2, speed_motor_backward);
      motor(1, speed_motor_forword);
    }
  } while ((analog(2) > ref[1] || analog(3) > ref[2]) && (analog(4) > ref[3] || analog(5) > ref[6]) );
  ST(1);
}
void Run_s() {                                                    //เดินไปจนกว่าจะเจอกระป๋อง
  do  {
    if (analog(2) < ref[1] )state_on_Line = 1;
    if (state_on_Line == 1 && analog(3) < ref[2])state_on_Line = 0;
    if (analog(5) < ref[4] )state_on_Line = 2;
    if (state_on_Line == 2 && analog(4) < ref[3])state_on_Line = 0;
    switch (state_on_Line) {
      case 0: {
          present_position = Position();
        } break;
      case 1: {
          present_position = 5.0;
        } break;
      case 2: {
          present_position = 75.0;
        } break;
    }
    setpoint = 45.0;
    errors = setpoint - present_position;
    integral = integral + errors ;
    derivative = (errors - previous_error) ;
    output = Kp * errors + Ki * integral + Kd * derivative;
    previous_error = errors;
    int speed_max = 60;
    int speed_motor_forword = speed_max + output;
    int speed_motor_backward = speed_max - output;
    if (speed_motor_forword > 100)speed_motor_forword = 100;
    if (speed_motor_forword < 0 )speed_motor_forword = 0;
    if (speed_motor_backward > 100)speed_motor_backward = 100;
    if (speed_motor_backward < 0 )speed_motor_backward = 0;
    if (Position() > 0) {
      motor(2, speed_motor_forword);
      motor(1, speed_motor_backward);
    }
    else {
      motor(2, speed_motor_backward);
      motor(1, speed_motor_forword);
    }
  } while (analog(8) < 1800);
  ST(1);
}
void Run_LL()                                         //เดินหน้าโดยไม่จับเส้นไปจนกว่า เซ็นเซอร์ด้านหลังเจอเส้นดำ
{
  do{
    motor(1,50);
    motor(2,50);
  }while ((analog(1) > ref[0] && analog(6) > ref[6]));
}
void Run_L() {
  do  {
    if (analog(2) < ref[1] )state_on_Line = 1;
    if (state_on_Line == 1 && analog(3) < ref[2])state_on_Line = 0;
    if (analog(5) < ref[4] )state_on_Line = 2;
    if (state_on_Line == 2 && analog(4) < ref[3])state_on_Line = 0;
    switch (state_on_Line) {
      case 0: {
          present_position = Position();
        } break;
      case 1: {
          present_position = 5.0;
        } break;
      case 2: {
          present_position = 75.0;
        } break;
    }
    setpoint = 45.0;
    errors = setpoint - present_position;
    integral = integral + errors ;
    derivative = (errors - previous_error) ;
    output = Kp * errors + Ki * integral + Kd * derivative;
    previous_error = errors;
    int speed_max = 40;
    int speed_motor_forword = speed_max + output;
    int speed_motor_backward = speed_max - output;
    if (speed_motor_forword > 100)speed_motor_forword = 100;
    if (speed_motor_forword < 0 )speed_motor_forword = 0;
    if (speed_motor_backward > 100)speed_motor_backward = 100;
    if (speed_motor_backward < 0 )speed_motor_backward = 0;
    if (Position() > 0) {
      motor(2, speed_motor_forword);
      motor(1, speed_motor_backward);
    }
    else {
      motor(2, speed_motor_backward);
      motor(1, speed_motor_forword);
    }
  } while ((analog(1) > ref[0] && analog(6) > ref[6]));

}
void hand_down() {                                      // เอามือลง
  ST(1);
  for (int i = 180; i >80; i-=1) {
    servo(1, i);
    delay(5);
  }
  delay(500);
}
void hand_up() {                                       //เอามือขึ้น                           
  ST(1);
  for (int i = 80; i < 180; i+=1) {
    servo(1, i);
    delay(5);
  }
  delay(500);
}
void hand_close() {                                   //หนีบกระป๋อง
  ST(1);
  servo(2, 157);
  delay(500);
}
void hand_open() {                                    // ปล่อยกระป๋อง
  ST(1);
  servo(2, 70);
  delay(500);
}
void L() {                                          //เลี่ยวซ้าย
  motor(1, -60);
  motor(2, 60);
  delay(200);
  while (analog(3) > ref[2])motor(1, -50); motor(2, 50);
  motor(1, 0); motor(2, 0);
  delay(20);
  motor(1, 60);
  motor(2, 60);
  delay(50);
}
void R() {                                    //เลี้ยวขวา
  motor(1, 50);
  motor(2, -60);
  delay(200);
  while (analog(5) > ref[4])motor(1, 50); motor(2, -60);
  motor(1, 0); motor(2, 0);
  delay(20);
  motor(1, 60);
  motor(2, 60);
  delay(50);
}
void TR(){                                  //เดินหน้าจนกว่าจะเจอเส้นตัดแล้วเลี้ยว ขวา
  s();
  R();
}
void TL(){                                  //เดินหน้าจนกว่าจะเจอเส้นตัดแล้วเลี้ยว ซ้าย
  s();
  L();
}
void ST(int a){                             //หยุดเป็นเวลา    ยกตัวอย่าง   เช่น   ST(1000);
  motor(1,0);                               //มอเตอร์หยุดหมุน เป็นเวลา 1 วินาที
  motor(2,0);
  delay(a);
}
void MT(int speeda, int speedb,int time_speed){       //เดินหน้าเป็นเวลา ตัวอย่างเช่น  MT(100,100,1000);
  motor(1,speeda);                                    // มอเตอร์ซ้าย หมุน  100 % มอเตอร์ ขวา หมุน 100 % เป็นความเร็ว 1 วินาที 
  motor(2,speedb);                                    
  delay(time_speed);
  ST(1);
}
void setup() {
  BOT_I_begin();                              //เรียกใช้บอร์ด
  ST(10);                                       //หยุดเป็นเวลา 0.01 วินาที
  hand_open();         
  hand_up();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);


  while (SW1() == 1) {                            //รอจนกว่าจะกด switch
    display.clearDisplay();
    display.setCursor(0, 9);
    display.println(String(analog(1)) + " " + String(analog(2)) + " " + String(analog(3)));
    display.println(String(analog(4)) + " " + String(analog(5)) + " " + String(analog(6)));
    display.println(String(analog(7)) + " " + String(analog(8)) +" " +  String(Position()));


    display.display();
    delay(200);
  }
  display.clearDisplay();                         // เคลียจอ lcd 



}

void loop() {
s();                                        //เดินไปถึงจุดต้ด 
L();                                        //เลี้ยวซ้าย
s();                                        //เดินหน้าไปจุดตัด
hand_down();                                //เมื่อถึงจุดตัดแล้วจะให้เอามือลง
delay(500);
hand_close();
delay(500);
hand_up();
delay(500);
R();                                        //เลี้ยวซ้าย
R();                                        //เลี้ยวซ้ายอีกรอบ เสหมือนกลับตัว 
s();                                        //เดินตามเส้นไปจุดตัด
R();                                        //เมื่อถึงจุดตัดแล้วก็จะให้หุ่นยนต์เลี้ยวขวา
s();                                         //เดินไปจุดตัด
ST(1000);                                    //หยุกเป็นเวลา 1 วินาที   
while(1);                                   //จบโปรแกรม

}
