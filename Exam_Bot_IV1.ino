#include <BOT_I.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "PrinceBot";            //ใส่ชื่อ WIFI
const char* password = "aaaaaaaa";          // รหัส WIFI

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


int pos = 0;
int Max_Light[12] = {4100, 4100, 4100, 4100, 4100, 4100 , 4100, 4100, 4100, 3000, 3000, 3000};     //ใส่ค่าแสงที่ได้ค่ามากจาก  Sensor (เจอสีขาว)
int Min_Light[12] = {200 , 300 , 400 , 200 , 200 , 200 , 300 , 350 , 200  , 100 , 100 , 100};      //ใส่ค่าแสงที่ได้ค่ามากจาก  Sensor (เจอสีดำ)
int ref[12] = {((Max_Light[0]+Min_Light[0])/2)
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
  Percent_Light[1] = map(analog(2), Min_Light[1], Max_Light[1], 100, 0);
  Percent_Light[2] = map(analog(3), Min_Light[2], Max_Light[2], 100, 0);
  Percent_Light[3] = map(analog(4), Min_Light[3], Max_Light[3], 100, 0);
  Percent_Light[4] = map(analog(5), Min_Light[4], Max_Light[4], 100, 0);
  Percent_Light[5] = map(analog(6), Min_Light[5], Max_Light[5], 100, 0);

  
  sum_value_total =  0 * Percent_Light[1]     +  20 * Percent_Light[2] + 40 * Percent_Light[3]
                     + 60 * Percent_Light[4] +  80 * Percent_Light[5] ;
  divide_value_total = Percent_Light[1] + Percent_Light[2] + Percent_Light[3] + Percent_Light[4] + Percent_Light[5];

  if (divide_value_total != 0) {
    return (sum_value_total / divide_value_total) ;
  }
}
void s() {                                //เดินไปที่จุดตัด แล้วหยุดกลางตัวหุ่นยนต์
  Run();
  Run_LL();
  motor(1,50);motor(2,50);delay(70);
  motor(1,0);motor(2,0);delay(50);
} 
void ss() {                               //เดินข้ามเส้น 1 เส้น แล้วไปหยุดที่ เส้นที่ 2 
  Run();
  Run_LL();
  Run();
  Run_LL();
  motor(1,50);motor(2,50);delay(70);
  motor(1,0);motor(2,0);delay(50);
}
void Run() {                              //เดินตามเส้นจนกว่า sensor ด้านหน้าจะเจอเส้นตัด
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
  } while ((analog(2) > ref[1] || analog(3) > ref[2]) && (analog(5) > ref[4] || analog(6) > ref[5]) );
}
void Run_s() {                                                      //เดินตามเส้นจนกว่า sensor วัดระยะทางไปเจอกระป๋อง
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
  } while (analog(8) < 2700);
}
void Run_LL()                                   //เดินตรงจนกว่าจะเจอเส้นตัด
{
  do{
    motor(1,50);
    motor(2,50);
  }while ((analog(1) > ref[0] && analog(7) > ref[6]));
}
void Run_L() {
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
  } while ((analog(1) > ref[0] && analog(7) > ref[6]));

}

void hand_up() {                                                    //เอามือขึ้น
  
  for (int i = 10; i <100; i+=1) {
    servo(1, i);
    delay(5);
  }
  delay(500);
}
void hand_down() {                                                //วางมือลง
  for (int i = 100; i >10; i-=1) {
    servo(1, i);
    delay(5);
  }
  delay(500);
}
void hand_close() {                                               //หนีบกระป๋อง
  servo(2, 157);
  delay(500);
}
void hand_open() {                                                //ปล่อยกระป๋อง
  servo(2, 70);
  delay(500);
}
void L() {                                                        //เลี้ยวซ้าย
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
void R() {                                                      //เลี้ยวขวา
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
void TR(){                                              //เจอไปเจอจุดตัดแล้วเลี้ยวขวา
  s();
  R();
}
void TL(){                                              //เจอไปเจอจุดตัดแล้วเลี้ยวซ้าย
  s();
  L();
}
void ST(int a){                                         //หยุดเป็นเวลา
  motor(1,0);
  motor(2,0);
  delay(a);
}
void MT(int speeda, int speedb,int time_speed){                     //เดินหน้ามอเตอร์ด้วยความเร็ว .... ....  เป็นระยะเวลา ....
  motor(1,speeda);
  motor(2,speedb);
  delay(time_speed);
}
void WIFI_SETUP(){                                  //ตั้งค่าการโหลดโปรแกรมไร้สาย
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
void setup() {
  BOT_I_begin();                                      //เรียกให้ฟังก์ชั้นภายในบอร์ด
  ST(10);                                               //หยุดเป็นเวลา 0.01 วินาที
  hand_open();                                         
  hand_up();
 // hand_off();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);


  while (SW1() == 1) {                                    //รอจนกว่าจะกด sw 1 
    ArduinoOTA.handle();
    display.clearDisplay();
    display.setCursor(0, 9);
    display.println(String(analog(1)) + " " + String(analog(2)) + " " + String(analog(3)));
    display.println(String(analog(4)) + " " + String(analog(5)) + " " + String(analog(6)));
    display.println(String(analog(7)) + " " + String(analog(8)) +" " +  String(Position()));


    display.display();
    delay(200);
  }
  display.clearDisplay();



}

void loop() {
s();                        //เดินหน้าไปเจอจุดตัด
s();                                
R();                        //เลี้ยวขวา
s();                        //เดินหน้าไปเจอจุดตัด
Run_s();                    //เดินไปกว่าจะเจอกระป๋อง
ST(500);                    //หยุดเป็นเวลา 0.5 วินาที
hand_down();                //วางมือลง
delay(500);                 
hand_close();               //หนีบกระป๋อง
delay(500);
hand_up();                  //เอามือขึ้น
delay(500);                 //หน่วงเวลา 0.5 วินาที
MT(-50,-50,700);            //ถอยหลัง ด้วยความเร็ว 50 เป็นเวลา 0.7 วินาที
ST(700);                     //หยุดเป็นเวลา 0.7 วินาที
while(1);                   //จบโปรแกรม
 
}
