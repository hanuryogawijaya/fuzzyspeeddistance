#include "TinyGPS++.h"
#include "NewPing.h"
TinyGPSPlus gps;
#define gpsSerial Serial3

#define TRIGGER_PIN 22
#define ECHO_PIN 23
#define LED_PIN 34

#define TRIGGER_PIN2 24
#define ECHO_PIN2 25
#define LED_PIN2 35

#define TRIGGER_PIN3 26
#define ECHO_PIN3 27
#define LED_PIN3 36

#define TRIGGER_PIN4 28
#define ECHO_PIN4 29
#define LED_PIN4 37

#define TRIGGER_PIN5 30
#define ECHO_PIN5 31
#define LED_PIN5 38

#define TRIGGER_PIN6 32
#define ECHO_PIN6 33
#define LED_PIN6 39

#define MAX_DISTANCE 1000
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE);
NewPing sonar3(TRIGGER_PIN3, ECHO_PIN3, MAX_DISTANCE);
NewPing sonar4(TRIGGER_PIN4, ECHO_PIN4, MAX_DISTANCE);
NewPing sonar5(TRIGGER_PIN5, ECHO_PIN5, MAX_DISTANCE);
NewPing sonar6(TRIGGER_PIN6, ECHO_PIN6, MAX_DISTANCE);
unsigned long lastDisplayTime = 0;

int lama;
float Jarak, Speed;
float A, B;
float hasilfuzzy,kecepatangps;

float JarakDekat[] = {0.0, 50.0, 75.0};
float JarakNormal[] = {50.0, 75.0, 80.0};
float JarakJauh[] = {75.0, 100.0, 100.0};

float SpeedLambat[] = {0.0, 30.0, 45.0};
float SpeedSedang[] = {30.0, 45.0, 60.0};
float SpeedTinggi[] = {45.0, 60.0, 100.0};

float tidakb = 1;
float bahaya = 0.5;

float minr[10];
float Rule[10];

float fJarakDekat()
{
    if (Jarak < JarakDekat[1])
    {
        return 1.0;
    }
    else if (Jarak >= JarakDekat[1] && Jarak <= JarakDekat[2])
    {
        return (JarakDekat[2] - Jarak) / (JarakDekat[2] - JarakDekat[1]);
    }
    else{
      return 0.0;
    }
}

float fJarakNormal()
{
    if (Jarak < JarakNormal[0] || Jarak > JarakNormal[2]){
      return 0.0;
    }
    else if (Jarak >= JarakNormal[0] && Jarak <= JarakNormal[1])
    {
        return (Jarak - JarakNormal[0]) / (JarakNormal[1] - JarakNormal[0]);
    }
    else if (Jarak > JarakNormal[1] && Jarak <= JarakNormal[2])
    {
        return (JarakNormal[2] - Jarak) / (JarakNormal[2] - JarakNormal[1]);
    }
    else{
      return 0.0;
    }
}

float fJarakJauh()
{
    if (Jarak < JarakJauh[0])
    {
        return 0.0;
    }
    else if (Jarak >= JarakJauh[0] && Jarak <= JarakJauh[1])
    {
        return (Jarak - JarakJauh[0]) / (JarakJauh[1] - JarakJauh[0]);
    }
    else if (Jarak > JarakJauh[1])
    {
        return 1.0;
    }else{
      return 0.0;
    }
}

float fSpeedLambat()
{
    if (Speed < SpeedLambat[1])
    {
        return 1.0;
    }
    else if (Speed >= SpeedLambat[1] && Speed <= SpeedLambat[2])
    {
        return (SpeedLambat[2] - Speed) / (SpeedLambat[2] - SpeedLambat[1]);
    }
    else{
      return 0.0;
    }
}

float fSpeedSedang()
{
    if (Speed < SpeedSedang[0] || Speed > SpeedSedang[2])
    {
        return 0.0;
    }
    else if (Speed >= SpeedSedang[0] && Speed <= SpeedSedang[1])
    {
        return (Speed - SpeedSedang[0]) / (SpeedSedang[1] - SpeedSedang[0]);
    }
    else if (Speed > SpeedSedang[1] && Speed <= SpeedSedang[2])
    {
        return (SpeedSedang[2] - Speed) / (SpeedSedang[2] - SpeedSedang[1]);
    }
   else{
      return 0.0;
    }
}

float fSpeedTinggi()
{
    if (Speed < SpeedTinggi[0])
    {
        return 0.0;
    }
    else if (Speed >= SpeedTinggi[0] && Speed <= SpeedTinggi[1])
    {
        return (Speed - SpeedTinggi[0]) / (SpeedTinggi[1] - SpeedTinggi[0]);
    }
    else if (Speed > SpeedTinggi[1])
    {
        return 1.0;
    }
    else{
      return 0.0;
    }
}

float Min(float a, float b)
{
    if (a < b)
    {
        return a;
    }
    else if (b < a)
    {
        return b;
    }
    else
    {
        return a;
    }
}

void rule()
{
    // if jarak dekat and speed lambat then tidak berbahaya
    minr[1] = Min(fJarakDekat(), fSpeedLambat());
    Rule[1] = tidakb;
    // if jarak dekat and speed sedang then berbahaya
    minr[2] = Min(fJarakDekat(), fSpeedSedang());
    Rule[2] = bahaya;
    // if jarak dekat and speed tinggi then berbahaya
    minr[3] = Min(fJarakDekat(), fSpeedTinggi());
    Rule[3] = bahaya;
    // if jarak normal and speed lambat then kurang tidak berbahaya
    minr[4] = Min(fJarakNormal(), fSpeedLambat());
    Rule[4] = tidakb;
    // if jarak normal and speed sedang then tidak berbahaya
    minr[5] = Min(fJarakNormal(), fSpeedSedang());
    Rule[5] = tidakb;
    // if jarak normal and speed cepat then berbahaya
    minr[6] = Min(fJarakNormal(), fSpeedTinggi());
    Rule[6] = bahaya;
    // if jarak jauh and speed lambat then tidak berbahaya
    minr[7] = Min(fJarakJauh(), fSpeedLambat());
    Rule[7] = tidakb;
    // if jarak jauh and speed sedang then tidak berbahaya
    minr[8] = Min(fJarakJauh(), fSpeedSedang());
    Rule[8] = tidakb;
    // if jarak jauh and speed cepat then tidak berbahaya
    minr[9] = Min(fJarakJauh(), fSpeedTinggi());
    Rule[9] = tidakb;
}
float defuzzyfikasi()
{
    rule();
    A = 0;
    B = 0;

    for (int i = 1; i <= 9; i++)
    {
        //Serial.print("Rule ke ");
        //Serial.print(i);
        //Serial.print(" ");
        //Serial.println(Rule[i]);
        //Serial.print("Min ke ");
        //Serial.print(i);
        //Serial.print(" ");
        //Serial.println(minr[i]);
        A += Rule[i] * minr[i];
        B += minr[i];
    }
    Serial.print("Hasil A :");
    Serial.println(A);
    Serial.print("Hasil B :");
    Serial.println(B);
    hasilfuzzy = A/B;
    return hasilfuzzy;
}

void waktukedip(int lampu, int waktu){
    //Serial.print("Delay = ");
    //Serial.print(waktu);
    //Serial.println();
    digitalWrite(lampu, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(waktu);                       // wait for a second
    digitalWrite(lampu, LOW);    // turn the LED off by making the voltage LOW
    delay(waktu);                       // wait for a second
    digitalWrite(lampu, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(waktu);                       // wait for a second
    digitalWrite(lampu, LOW);    // turn the LED off by making the voltage LOW
    delay(waktu*2);                       // wait for a second
}

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  pinMode(LED_PIN,OUTPUT);
  pinMode(LED_PIN2,OUTPUT);
  pinMode(LED_PIN3,OUTPUT);
  pinMode(LED_PIN4,OUTPUT);
  pinMode(LED_PIN5,OUTPUT);
  pinMode(LED_PIN6,OUTPUT);
}

void loop() {
  while (gpsSerial.available() > 0)gps.encode(gpsSerial.read());
  if (gps.speed.isValid() && (millis() - lastDisplayTime >= 1000)) {
    Serial.print("SPD=");  Serial.println(gps.speed.kmph());
    kecepatangps = gps.speed.kmph();
    unsigned distance = sonar.ping();
    Serial.print("Jarak Sensor 1 =");
    Serial.print(distance/ US_ROUNDTRIP_CM);
    Serial.println(" cm");
    
    unsigned distance2 = sonar2.ping();
    Serial.print("Jarak Sensor 2 =");
    Serial.print(distance2/ US_ROUNDTRIP_CM);
    Serial.println(" cm");
    
    unsigned distance3 = sonar3.ping();
    Serial.print("Jarak Sensor 3 =");
    Serial.print(distance3/ US_ROUNDTRIP_CM);
    Serial.println(" cm");
    
    unsigned distance4 = sonar4.ping();
    Serial.print("Jarak Sensor 4 =");
    Serial.print(distance4/ US_ROUNDTRIP_CM);
    Serial.println(" cm");
    
    unsigned distance5 = sonar5.ping();
    Serial.print("Jarak Sensor 5 =");
    Serial.print(distance5/ US_ROUNDTRIP_CM);
    Serial.println(" cm");
    
    unsigned distance6 = sonar6.ping();
    Serial.print("Jarak Sensor 6 =");
    Serial.print(distance6/ US_ROUNDTRIP_CM);
    Serial.println(" cm");

    Jarak = distance;
    Speed = kecepatangps;
    Serial.print("Hasil Fuzzy 1:");
    Serial.println();
    Serial.println(defuzzyfikasi());
    Serial.println();
    lama = hasilfuzzy*100;
    waktukedip(LED_PIN,lama);
  
    Jarak = distance2;
    Speed = kecepatangps;
    Serial.print("Hasil Fuzzy 2 :");
    Serial.println();
    Serial.println(defuzzyfikasi());
    Serial.println();
    lama = hasilfuzzy*100;
    waktukedip(LED_PIN2,lama);
    
    Jarak = distance3;
    Speed = kecepatangps;
    Serial.print("Hasil Fuzzy 3:");
    Serial.println();
    Serial.println(defuzzyfikasi());
    Serial.println();
    lama = hasilfuzzy*100;
    waktukedip(LED_PIN3,lama);
    
    Jarak = distance4;
    Speed = kecepatangps;
    Serial.print("Hasil Fuzzy 4 :");
    Serial.println();
    Serial.println(defuzzyfikasi());
    Serial.println();
    lama = hasilfuzzy*100;
    waktukedip(LED_PIN4,lama);
    
    Jarak = distance5;
    Speed = kecepatangps;
    Serial.print("Hasil Fuzzy 5 :");
    Serial.println();
    Serial.println(defuzzyfikasi());
    Serial.println();
    lama = hasilfuzzy*100;
    waktukedip(LED_PIN5,lama);
    
    Jarak = distance6;
    Speed = kecepatangps;
    Serial.print("Hasil Fuzzy 6 :");
    Serial.println();
    Serial.println(defuzzyfikasi());
    Serial.println();
    lama = hasilfuzzy*100;
    waktukedip(LED_PIN6,lama);
    
    lastDisplayTime = millis();
  }
}
