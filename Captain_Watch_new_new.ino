
#include <DHT11.h>
#include <SoftwareSerial.h>

#define BT_RXD 7
#define BT_TXD 8

unsigned long resData = 0;
unsigned long one = 1;

int MOTION = 2;
int BUTTON = 5;

int SOUND = A0;
int LIGHT = A1;
int HUMMID = A2;
int POSITION=12;
int LED=13;


String myString;

DHT11 dht11(HUMMID);
int cur = 0;
int button = 0;
int sound = 0;
int light = 0;
int motion = 0;
int cnt = 0;
int buttoncnt = 0;
int soundcnt = 0;
int soundtemp = 0;
unsigned long motiontemp = 0;
int dontsleep = 0;
float temperature = 0 ;
float humidity = 0;
int state=0;
int pbutton=0;


SoftwareSerial bluetooth(BT_RXD,BT_TXD); //블루투스의 Tx, Rx핀을 7번 8번핀으로 설정

void setup() {
  // 시리얼 통신의 속도를 9600으로 설정
  Serial.begin(9600);
  bluetooth.begin(9600);
  pinMode(BUTTON,INPUT);
  pinMode(SOUND,INPUT);
  pinMode(MOTION,INPUT);
  pinMode(POSITION,INPUT);
  pinMode(LED,OUTPUT);
}

void loop() { //코드를 무한반복합니다.
  
  resData = resData | (one<<31);
  one = 1;
  //Serial.println(resData);

  
  // Pressed button detect function
  button = digitalRead(BUTTON); 
  if(button!=cur){ // Drive bounce phenomenon
   delay(100);
  }
  if(button==HIGH){
    buttoncnt = 10;
  }
  if(buttoncnt>0){
    buttoncnt--;
    Serial.println("push");
    resData = resData|(one<<30);
    one = 1;
  }
  cur = button;
  //Serial.println(resData);
 
  
  // Found sleeping state with motion sensor
  motion = digitalRead(MOTION);
  //Serial.println(motion);
  cnt++;
  if(motion == HIGH){
    dontsleep++;
    Serial.println("dontsleep");
  }
  if(cnt>10){
    if(dontsleep>2){ //Cant sleep well
      motiontemp = 1;
      Serial.println("cant sleep");
    }
    else{ // sleep well
      motiontemp = 0;
      Serial.println("sleep well");
    }
    cnt = 0;
    dontsleep = 0;
  }
  resData = resData | (motiontemp<<28);

  //Serial.println(resData);


  // Handle humidity sensor
  dht11.read(humidity,temperature);
  //Serial.print("Humidity = ");
  //Serial.println(humidity);
  //Serial.print("temperature = ");
  //Serial.println(temperature);
  //Serial.println(resData);
  temperature = (temperature - 10)*4;
  resData = resData | ((unsigned long)temperature<<21);
  resData = resData | ((unsigned long)humidity<<14);
   


  //Serial.println(resData);

   
  
  // Handle cds sensor
  light = analogRead(LIGHT);
  light = light / 8;
  resData = resData | (light << 7);


  //Serial.println(resData);


  // Handle sound sensor
  sound = analogRead(SOUND);
  if(sound>110){
    soundtemp = sound;
    soundcnt = 10;
  }
  if(soundcnt>0){
    sound = soundtemp ;
    soundcnt--;
  }
  //Serial.println(sound);
  sound = sound / 8;
  resData = resData | sound;

  if( pbutton ==1 && digitalRead(POSITION)==0)
  {
    state =!state;
    digitalWrite(LED,state);
    
  }
  if(state ==1)
  {
    one=1;
    resData = resData | (one <<29);
    one=1;
    resData = resData & ~(one <<28);
    one=1;
  }

  pbutton=digitalRead(POSITION);
  

  // Bluetooth detect function
  myString = String(resData);
  bluetooth.print(myString);
  //Serial.println(myString);
  delay(1500);
  resData = 0;
}
