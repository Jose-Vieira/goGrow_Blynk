/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "ec64c583e791417291f526b12622ca53";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Blynk";
char pass[] = "teste123";

#define DHTPIN 2          // Pino conectado D4
#define DHTTYPE DHT11     // DHT 11

#define releAgua 14   //d5
#define releIlum 12   //d6
#define releAque 13   //d7
#define releVent 15   //d8
#define powerMoist 0  //d1

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
WidgetRTC rtc;
Adafruit_ADS1115 ads;
///////////////////////////////
bool primeiraConexao = true;
int tempMin = 15;
int tempMax = 28;
int manual = 0;
//////////////////////////////
boolean flagTIW1 = false;
boolean flagTIW2 = false;
boolean flagTIW3 = false;
boolean flagTIW4 = false;
//////////////////////////////

BLYNK_CONNECTED(){
  if(primeiraConexao){    
    Blynk.syncVirtual(V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11);
    Blynk.notify("Go Grow Conectado");
    primeiraConexao = false;
  }
}
BLYNK_APP_CONNECTED(){
  Blynk.virtualWrite(V9, tempMin);
  Blynk.virtualWrite(V10, tempMax);
  Blynk.virtualWrite(V11, manual);
}


BLYNK_WRITE(V1){ //TimeInput IRRIGAÇÃO
  long segundosAgora = 0;
  long segundosStart = 0;
  long segundosStop = 0;
  if(manual == 0){ 
     TimeInputParam t(param);
     int ajusteDia = -1;                                  
     if(weekday() == 1) ajusteDia = 6;                    
     if(t.isWeekdaySelected(weekday() + ajusteDia)){                                                               
      if(t.hasStartTime()){ 
        segundosAgora = (hour() * 3600) + (minute() * 60 + second());
        Serial.println(String("Segundos Agora: ") + segundosAgora);
        segundosStart = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
        segundosStop = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
        Serial.println(String("Segundos Start: ") + segundosStart);
        if(segundosAgora >= segundosStart && segundosAgora <= segundosStop){
          digitalWrite(releAgua, HIGH); 
          flagTIW1 = true;  
          Blynk.virtualWrite(V12, 1);
          Serial.println("liga TIW 1");
        }
      }
      else{ /*nao faz nada*/}
      if(t.hasStopTime()){ 
        Serial.println(String("Segundos Stop: ") + segundosStop);
        if(segundosAgora >= segundosStop){
          digitalWrite(releAgua, LOW); 
          flagTIW1 = true;  
          Blynk.virtualWrite(V12, 0);
          Serial.println("desliga TIW 1");
        }
      }
      else{/*nao faz nada*/} 
     }
     else{/*nao faz nada*/}   
  }
}

BLYNK_WRITE(V2){  //TimpeInput ILUMINAÇÃO
  long segundosAgora = 0;
  long segundosStart = 0;
  long segundosStop = 0;
  if(manual == 0){
     TimeInputParam t(param);
     int ajusteDia = -1;
     if(weekday() == 1) ajusteDia = 6;
     if(t.isWeekdaySelected(weekday() + ajusteDia)){
      if(t.hasStartTime()){
        segundosAgora = (hour() * 3600) + (minute() * 60 + second());
        segundosStart = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
        segundosStop = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
        if(segundosAgora >= segundosStart && segundosAgora <= segundosStop){
          digitalWrite(releIlum, HIGH);
          flagTIW2 = true;
          Blynk.virtualWrite(V13, 1);
          Serial.println("liga TIW 2");
        }
      }
      else{/*nao faz nada*/}
      if(t.hasStopTime()){
        if(segundosAgora >= segundosStop){
          digitalWrite(releIlum, LOW);
          flagTIW2 = true;
          Blynk.virtualWrite(V13, 0);
          Serial.println("desliga TIW 2");
        }
      }
      else{/*nao faz nada*/} 
     }
     else{/*nao faz nada*/}    
  }
}

BLYNK_WRITE(V3){ //TimeInput AQUECIMENTO
  long segundosAgora = 0;
  long segundosStart = 0;
  long segundosStop = 0; 
  if(manual == 0){
     TimeInputParam t(param);
     int ajusteDia = -1;
     if(weekday() == 1) ajusteDia = 6;
     if(t.isWeekdaySelected(weekday() + ajusteDia)){
      if(t.hasStartTime()){
        segundosAgora = (hour() * 3600) + (minute() * 60 + second());
        segundosStart = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
        segundosStop = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
        if(segundosAgora >= segundosStart && segundosAgora <= segundosStop){
          int i = aquecMaster();
          Serial.println(String("Resultado Aquec Master: ") + i);
          digitalWrite(releAque, i);
          flagTIW3 = true;
          Blynk.virtualWrite(V14, 1);
          Serial.println("liga TIW 3");
        }
      }
      else{/*nao faz nada*/}
      if(t.hasStopTime()){
        if(segundosAgora >= segundosStop){
          digitalWrite(releAque, LOW);
          flagTIW3 = true;
          Blynk.virtualWrite(V14, 0);
          Serial.println("desliga TIW 3");
        }
      }
      else{/*nao faz nada*/} 
     }
     else{/*nao faz nada*/}
  }
}

BLYNK_WRITE(V4){ //TimeInput VENTILACAO
  long segundosAgora = 0;
  long segundosStart = 0;
  long segundosStop = 0; 
  if(manual == 0){
     TimeInputParam t(param);
     int ajusteDia = -1;
     if(weekday() == 1) ajusteDia = 6;
     if(t.isWeekdaySelected(weekday() + ajusteDia)){
      if(t.hasStartTime()){
        segundosAgora = (hour() * 3600) + (minute() * 60 + second());
        segundosStart = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
        segundosStop = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
        if(segundosAgora >= segundosStart && segundosAgora <= segundosStop){
          digitalWrite(releVent, HIGH);
          flagTIW4 = true;
          Blynk.virtualWrite(V15, 1);
          Serial.println("liga TIW 4");
        }
      }
      else{/*nao faz nada*/}
      if(t.hasStopTime()){
        if(segundosAgora >= segundosStop){
          digitalWrite(releVent, LOW);
          flagTIW4 = true;
          Blynk.virtualWrite(V15, 0);
          Serial.println("desliga TIW 4");
        }
      }
      else{/*nao faz nada*/} 
     }
     else{/*nao faz nada*/}
  }
}
BLYNK_WRITE(V9){
  tempMin = param.asInt();
  if(tempMin > tempMax){
    tempMin = tempMax -1;
    Blynk.virtualWrite(V9, tempMin);
  }
}

BLYNK_WRITE(V10){
  tempMax = param.asInt();
  if(tempMax < tempMin){
    tempMax = tempMin +1;
    Blynk.virtualWrite(V10, tempMax);
  }
}
BLYNK_WRITE(V11){
  manual = param.asInt();
  if(manual == 0){
    Blynk.syncVirtual(V1, V2, V3, V4);
  }
}
BLYNK_WRITE(V12){
  if(flagTIW1 == 1){
    digitalWrite(releAgua, param.asInt());
  }
  else{
    digitalWrite(releAgua, param.asInt());
    manual = 1;
    Blynk.virtualWrite(V11, manual);
  }
  flagTIW1 = 0;
}
BLYNK_WRITE(V13){
  if(flagTIW2 == 1){
    digitalWrite(releIlum, param.asInt());
  }
  else{
    digitalWrite(releIlum, param.asInt());
    manual = 1; // liga manual
    Blynk.virtualWrite(V11, manual);
  }
  flagTIW2 = 0;
}
BLYNK_WRITE(V14){
  if(param.asInt() == 1){ // recebe 1 pelo VirtualPin
    int i = aquecMaster();
   if(flagTIW3 == 1){ // verifica se o valor veio pelo TIW3
    digitalWrite(releAque, i);
   }
   else{  // ou pelo Usuário
    digitalWrite(releAque, i);
    manual = 1; // Liga Manual
    Blynk.virtualWrite(V11, manual);
   }
  }
  else{ // recebe 0 pelo VirtualPin
    if(flagTIW3 == 1){  // verifica se o valor veio pelo TIW3
      digitalWrite(releAque, param.asInt());
    }
    else{ // ou pelo Usuário
      digitalWrite(releAque, param.asInt());
      manual = 1; //Liga Manual
      Blynk.virtualWrite(V11, manual);
    }
  }
  flagTIW3 = 0;
}
BLYNK_WRITE(V15){
  if(flagTIW4 == 1){
    digitalWrite(releVent, param.asInt());
  }
  else{
    digitalWrite(releVent, param.asInt());
    manual = 1; // liga manual
    Blynk.virtualWrite(V11, manual);
  }
  flagTIW4 = 0;
}

void sendDHT()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Serial.println(h); Serial.println(t);
  Blynk.virtualWrite(V6, t);
}
void sendSoilMoist(){
  int16_t m = 0;
  digitalWrite(powerMoist, HIGH);
  for(int i = 0; i < 3; i ++){
    m = ads.readADC_SingleEnded(0) + m;
  }
  digitalWrite(powerMoist, LOW);
  m = m/3;
  Blynk.virtualWrite(V8, m);
}
void sendLux(){
  int16_t l = 0;
  l = ads.readADC_SingleEnded(1);
  Blynk.virtualWrite(V7, l);
  Serial.println(l);
}
int aquecMaster(){
  int flag = 0;
  if(V14 == 1){
    if(V6 < tempMin && digitalRead(releAque) == LOW){
      flag = 1;
    }
    if(V6 > tempMax && digitalRead(releAque) == HIGH){
      flag = 0;
    }
  }
  return flag;
}
void aquecManual(){
  if(manual == 1){
    if(V14 == 1){
      if(V6 < tempMin && digitalRead(releAque) == LOW){
        digitalWrite(releAque, HIGH);
      }
      if(V16 > tempMax && digitalRead(releAque) == HIGH){
        digitalWrite(releAque, LOW);
      }
    }
  }
}
void verificaRotinas(){
  Serial.println(manual);
  if(manual == 0){
    Blynk.syncVirtual(V1, V2, V3, V4);
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);

  dht.begin();
  setSyncInterval(1);
  rtc.begin();
  ads.begin();
  Blynk.syncAll();

  pinMode(releAgua, OUTPUT);
  pinMode(releIlum, OUTPUT);
  pinMode(releAque, OUTPUT);
  pinMode(releVent, OUTPUT);
  pinMode(powerMoist, OUTPUT);

  timer.setInterval(2000L, sendDHT);
  timer.setInterval(30000L, verificaRotinas);
  timer.setInterval(60000L, aquecManual);
  timer.setInterval(30000L, sendSoilMoist);
  timer.setInterval(10000L, sendLux);
}

void loop()
{
  Blynk.run();
  timer.run();
}

