// Controlling a servo position using a dual joystick 
// https://gsyan888.blogspot.com/2014/11/android-me-arm-bluetooth-app.html
#include <SoftwareSerial.h>
#include <Servo.h> 
const int SERVOS_TOTAL = 4;		// 總共有幾顆伺服馬達
const int joystickOffsetMin = 100;	// 搖桿至少扳動多少才開始動作
const int joystickIdleMax = 100;	// 搖桿未使用狀態計數器達多少次後, 將馬達斷線
const int delayMilliseconds = 20;	// 每隔多少 MS 讀取一次資料
const int ledPin = 13;			// 狀態顯示的 LED 接在哪一個 pin

//定義Arduino PIN10及PIN11分別為TX及RX腳
SoftwareSerial BT(10,11);

Servo myservo[SERVOS_TOTAL];	        // 宣告 Servo 物件陣件
int idleCounter[SERVOS_TOTAL];	        // 宣告記錄搖桿某方位多久沒動作的陣列

//
// 伺服馬達的設定, 一行設定一顆
//
// 參數依序為 : joystickPin, servoPin, min, max, initialAngle
//	joystickPin : 搖桿輸入給 Arduino 哪個腳位(用 0~3 代表 A0~A3)
//	servoPin : 伺服馬達(橙/黃線)接在 Arduino 哪個腳位
//	min : 伺服馬達最小的角度是多少
//	max : 伺服馬達最大的角度是多少
//	initialAngle : 伺服馬達初始的角度是多少
//
int servoConfig[SERVOS_TOTAL][5] = {
  // joystickPin,  servoPin,   min,    max,      initialAngle
    { 0,                  9,     0,    179,      90 },        //A0 = L.AX ,  Buttom
    { 1,                  6,   179,    0,      150 },        //A1 = L.AY ,  Left
    { 2,                  5,   179,    0,       90 },        //A2 = R.AY ,  Right
    { 3,                  3,   179,   100,      110 },        //A3 = R.AX ,  Front (claw)
};

//
// 初始化的程序
//
void setup() 
{ 
  int outputPin, angleMin, angleMax, angleInit;
  
  BT.begin(9600); //Arduino起始鮑率：38400
  Serial.begin(9600); 
  
  //status LED
  pinMode(ledPin, OUTPUT);	        // 設定顯示狀態用的 LED 接腳
  digitalWrite(ledPin, HIGH);	        // 點亮 LED
  
  for(int i=0; i<SERVOS_TOTAL; i++) {
    outputPin = servoConfig[i][1];	// 由設定取得馬達在 Arduino 的接腳參數
    angleInit = servoConfig[i][4];	// 由設定取得馬達初始的角度參數
    
    myservo[i].attach(outputPin);       // attaches the servo on  pin config to the servo object
    myservo[i].write(angleInit);	// 設定馬達的角度
    idleCounter[i] = 0;			// 將搖桿未使用記數器歸零
  }
} 

//
// 重覆執行的程序 
//
void loop() 
{ 
  int inputValue, currentAngle, joyStickOffset, angleOffset;
  int inputPin, outputPin, angleMin, angleMax, angleInit, temp;
  char inBytes;
  int blueToothCmd[SERVOS_TOTAL];
  int inSize;
  
  for(int i=0; i<SERVOS_TOTAL; i++) {
    blueToothCmd[i] = 0;
  }
  //讀取藍牙訊息
  if (BT.available()){ 
    //Serial.print('1'); 
    
      inBytes = BT.read(); 
      Serial.println(inBytes);
        
    Serial.print(char(inBytes));
    //讀取inBytes[0]字元，判斷字元控制機械手臂
    switch(char(inBytes)) {
      case 'B' :      //底部馬達增加
        blueToothCmd[0] = 1;
        break;
      case 'b' :      //底部馬達減少
        blueToothCmd[0] = -1;
        break;
      case 'L' :      //左側馬達增加
        blueToothCmd[1] = 1;
        break;
      case 'l' :      //左側馬達減少
        blueToothCmd[1] = -1;
        break;
      case 'R' :      //右側馬達增加
        blueToothCmd[2] = 1;
        break;
      case 'r' :      //右側馬達減少
        blueToothCmd[2] = -1;
        break;
      case 'C' :      //抓子馬達增加
        blueToothCmd[3] = 1;
        break;
      case 'c' :      //爪子馬達減少
        blueToothCmd[3] = -1;
        break;       
    }
  }
  
  for(int i=0; i<SERVOS_TOTAL; i++) {
    inputPin   = servoConfig[i][0];	// 由設定取得搖桿輸入給 Arduino 的接腳參數
    outputPin  = servoConfig[i][1];	// 由設定取得馬達在 Arduino 的接腳參數
    angleMin   = servoConfig[i][2];	// 由設定取得馬達最小角度的參數
    angleMax   = servoConfig[i][3];	// 由設定取得馬達最大角度的參數
    
    inputValue = analogRead(inputPin);  // 讀取搖桿的數值(0~1023) 
    currentAngle = myservo[i].read();   // 取得馬達目前的角度
    
    joyStickOffset = inputValue - 512;	// 計算搖桿和未使用時(中間值 512)的差動多少, 用以判斷是否要改變馬達的角度
    
    if( abs(blueToothCmd[i]) == 1 ) {
      joyStickOffset = blueToothCmd[i]*(joystickOffsetMin+1);
    }
	
    // 如果搖桿扳動的幅度大於設定的最小值就改變馬達的角度
    if(abs(joyStickOffset) > joystickOffsetMin) {
      // 如果最小角度 > 最大角度 ==> 搖桿動作相反 
      if(angleMin > angleMax) {  	
        joyStickOffset *= -1;		// 將搖桿動作設為相反 1 -> -1 , -1 -> 1
		
        // 將最小角度和最大角度的值交換
        temp = angleMin;
        angleMin = angleMax;
        angleMax = temp;
      }
      
      // 如果搖桿往比 512 大的方向扳動就讓馬達角度減少, 反之則增加
      if(joyStickOffset > 0) {
        angleOffset = -1;
      } else {
        angleOffset = 1;
      }
      currentAngle += angleOffset;    // 計算新的馬達角度
	  
      // 如果新的馬達角度在最小角度參數和最大角度參數之間, 就進行馬達改變角度的程序
      if(currentAngle > angleMin && currentAngle < angleMax) {
        idleCounter[i] = 0;	
		
        // 如果馬達未連線, 先行連線
        if(!myservo[i].attached()) {
          myservo[i].attach(outputPin);
        }
        
	// 如果馬達已連線, 將角度設為新的角度
        if(myservo[i].attached()) {
          myservo[i].write(currentAngle);     // sets the servo position according to the scaled value 
        }
      }
    } else {
       idleCounter[i]++;	    // 搖桿未扳動或未達搖桿動作的標準, 將計數器加一
    }
	
    // 如果搖桿未動的計數超過設定的最大值後, 切斷馬達的連線
    if(idleCounter[i] > joystickIdleMax) {
      idleCounter[i] = 0;	   // 計數器歸零
      myservo[i].detach();	   // 切斷馬達的連線
    }
  }
  
  if(analogRead(5) == 0) {
    digitalWrite(ledPin, LOW);	        // LED Off
    for(int i=0; i<SERVOS_TOTAL; i++) {
      outputPin = servoConfig[i][1];	// 由設定取得馬達在 Arduino 的接腳參數
      angleInit = servoConfig[i][4];	// 由設定取得馬達初始的角度參數
    
      myservo[i].attach(outputPin);       // attaches the servo on  pin config to the servo object
      myservo[i].write(angleInit);	// 設定馬達的角度
      idleCounter[i] = 0;			// 將搖桿未使用記數器歸零
    }
  } else {
    digitalWrite(ledPin, HIGH);	        // 點亮 LED
  }
    
  delay(delayMilliseconds);        // 暫停多少 MS 後繼續
} 
