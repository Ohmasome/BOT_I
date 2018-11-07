#include <Servo.h>
#define A1 36
#define A2 39
#define A3 34
#define A4 35
#define A5 32
#define A6 33
#define A7 25
#define A8 26
#define A9 27
#define A10 14
#define A11 12
#define A12 13
#define _servo1 18
#define _servo2 19
#define _servo3 23

Servo servo1;
Servo servo2;
Servo servo3;
#ifndef ALL
 #define ALL 100
#endif



void servo(uint8_t servo,int16_t angle)
{
  if (servo==ALL)
  {
    if (angle==-1)
	{
	  servo1.detach();
	  servo2.detach();
	  servo3.detach();
	}
	else
	{
      if (!(servo1.attached()))
	  {
		servo1.attach(_servo1);
	  }
	  servo1.write(angle);

      if (!(servo2.attached()))
	  {
		servo2.attach(_servo2);
	  }
	  servo2.write(angle);

      if (!(servo3.attached()))
	  {
		servo3.attach(_servo3);
	  }
	  servo3.write(angle);
	}
  }
  if (servo==1)
  {
    if (angle==-1)
	{
	  servo1.detach();
	}
	else
	{
      if (!(servo1.attached()))
	  {
		servo1.attach(_servo1);
	  }
	  servo1.write(angle);
	}
  }
  if (servo==2)
  {
    if (angle==-1)
	{
	  servo2.detach();
	}
	else
	{
      if (!(servo2.attached()))
	  {
		servo2.attach(_servo2);
	  }
	  servo2.write(angle);
	}
  }
  if (servo==3)
  {
    if (angle==-1)
	{
	  servo3.detach();
	}
	else
	{
      if (!(servo3.attached()))
	  {
		servo3.attach(_servo3);
	  }
	  servo3.write(angle);
	}
  }
}
void BOT_I_begin(){ 
  pinMode(2,INPUT_PULLUP);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  pinMode(A6,INPUT);
  pinMode(A7,INPUT);
  pinMode(A8,INPUT);
  pinMode(A9,INPUT);
  pinMode(A10,INPUT);
  pinMode(A11,INPUT);
  pinMode(A12,INPUT);
  pinMode(15,OUTPUT);
  pinMode(16,OUTPUT);
  pinMode(17,OUTPUT);
  pinMode(5,OUTPUT);

  ledcSetup(2, 5000, 8);
  ledcSetup(3, 5000, 8);
  ledcSetup(4, 5000, 8);
  ledcSetup(5, 5000, 8);
  ledcAttachPin(15, 2);
  ledcAttachPin(16, 3);
  ledcAttachPin(17, 4);
  ledcAttachPin(5, 5);
  
  
  
}
int SW1(){
 return digitalRead(2);
}
int analog(int port){
	if(port == 1)return (int)analogRead(A1);
	else if(port == 2)return (int)analogRead(A2);
	else if(port == 3)return (int)analogRead(A3);
	else if(port == 4)return (int)analogRead(A4);
	else if(port == 5)return (int)analogRead(A5);
	else if(port == 6)return (int)analogRead(A6);
	else if(port == 7)return (int)analogRead(A7);
	else if(port == 8)return (int)analogRead(A8);
	else if(port == 9)return (int)analogRead(A9);
	else if(port == 10)return (int)analogRead(A10);
	else if(port == 11)return (int)analogRead(A11);
	else if(port == 12)return (int)analogRead(A12);

  
}
void motor(int pin,int Speeds){
	int _SpeedsA;
	int _SpeedsB;
  if(pin == 1){
    _SpeedsA = abs(Speeds);
    if(Speeds > 100)_SpeedsA=100;
    _SpeedsA = map(abs(_SpeedsA),0,100,0,255);
    if(Speeds > 0){
      ledcWrite(2, abs(_SpeedsA));
      ledcWrite(3,0);
    }
    else if(Speeds <= 0){
      ledcWrite(2,0);
      ledcWrite(3,abs(_SpeedsA));
    }
  }
   if(pin == 2){
    _SpeedsB = abs(Speeds);
    if(Speeds > 100)_SpeedsB=100;
    _SpeedsB = map(abs(_SpeedsB),0,100,0,255);
    if(Speeds > 0){
      ledcWrite(4, abs(_SpeedsB));
      ledcWrite(5,0);
    }
    else if(Speeds <= 0){
      ledcWrite(4,0);
      ledcWrite(5,abs(_SpeedsB));
    }
  }
}
void motor_t(int speeda,int speedb){
	if(speeda > 0){
		if(speeda > 100 )speeda=100;
		ledcWrite(2, abs(speeda));
      	ledcWrite(3,0);
	}
	else {
		speeda= abs(speeda);
		if(speeda > 100 )speeda=100;
		ledcWrite(2, 0);
      	ledcWrite(3,abs(speeda));
	}
	if(speedb > 0){
		if(speedb > 100 )speedb=100;
		ledcWrite(4, speedb);
      	ledcWrite(5,0);
	}
	else {
		speedb = abs(speedb);
		if(speedb > 100 )speedb=100;
		ledcWrite(4, 0);
      	ledcWrite(5,abs(speedb));
	}

}

