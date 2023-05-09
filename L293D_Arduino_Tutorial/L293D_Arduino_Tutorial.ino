//https://www.instructables.com/How-to-use-the-L293D-Motor-Driver-Arduino-Tutorial/
#include <SoftwareSerial.h>
#include <Wire.h>
//Motor L
const int motorPin1  = 2;  // Pin 15 of L293 
const int motorPin2  = 4;  // Pin 10 of L293 
const int enablePin1  = 3;  // Pin 1 of L293

//Motor R
const int motorPin3  = 7; // Pin  7 of L293  
const int motorPin4  = 8;  // Pin 2 of L293
const int enablePin2  = 6;  // Pin 9 of L293

//定義Arduino PIN10及PIN11分別為TX及RX腳
SoftwareSerial BT(10,11);

//讀取手機傳輸數值的變數
char  Car_status[2] ;

//定義轉速的變數
int runspeed;

void setup(){
 
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
    pinMode(enablePin1, OUTPUT);
    pinMode(enablePin2, OUTPUT);
    
    //設定初始值
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    analogWrite(enablePin1, 0);
    analogWrite(enablePin2, 0);
    
    // 設定和 HC-05 通訊的速度 (預設 9600)  
    BT.begin(9600); 
    Serial.begin(9600);
}

void loop(){
  
  //讀取藍芽模組訊息，並傳送到電腦
  if (BT.available()){
    Serial.println("Car_status : ");
    
    //透過for迴圈收集手機傳輸的暸筆資料
    for(int j = 0; j <= 1; j++){          
      Car_status[j] = char(BT.read());    
      Serial.println(Car_status[j]);
      }
      
    //判斷輸入字母去選擇馬達轉速
    switch(int(Car_status[1])){
        case '0':           //0
          runspeed = 0;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
        case 'A':            //A
          runspeed = 30;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
        case 'B':            //B
          runspeed = 60;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
        case 'C':            //C
          runspeed = 90;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
        case 'D':            //D
          runspeed = 120;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
        case 'E':            //E
          runspeed = 150;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
        case 'F':            //F
          runspeed = 180;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
        case 'G':            //G
          runspeed = 210;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
        case 'H':            //H
          runspeed = 240;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
        case 'I':            //I
          runspeed = 254;
          Serial.println("runspeed : ");
          Serial.println(runspeed);
        break;
    }
   }
   
  //馬達轉速
  analogWrite(enablePin1, runspeed);        
  analogWrite(enablePin2, runspeed);        

  //當讀取到資料第一筆，判斷該前後左右停
  switch (Car_status[0]){                   
  case '1':                    //前進
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
  break;
  case '2':                   //左轉
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
  break;
  case '3':                    //右轉
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
  break;
  case '4':                  //停止
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
  break;
  case '5':                  //後退
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
  break;
  }  
}
