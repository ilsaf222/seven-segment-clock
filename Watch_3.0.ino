//------Настройки-------
#define DHTPIN 7          //пин к которому подключён датчик темепературы
#define DHTTYPE DHT11     //тип датчика температуры(DHT 11 и DHT 22)
#define latch_Pin 8
#define clock_Pin 12
#define data_Pin 11
#define led_Pin 10        //мигающие точки
#define mosfetpin 9
#define datchikpin 17
//------Библиотеки------
#include <virtuabotixRTC.h>
#include "DHT.h"

//------Объекты---------
DHT dht (DHTPIN, DHTTYPE);
virtuabotixRTC myRTC(2, 3, 4);

//------Переменные------
int XX, YY, x, y, z, a, val, led, yw, xw, second;
boolean ledflag = false;
boolean dataflag = false;
boolean count;

//------Символы и анимации-------
byte segments[10] = {0b11111011, 0b10010001, 0b11101101, 0b11110101, 0b10010111, 0b01110110, 0b01111110, 0b11010000, 0b11111111, 0b11110111};
byte Temp_Hum[4] = {0b11000111, 0b01101011, 0b00111100, 0b11000111};

void setup() {
  Serial.begin(9600);
  pinMode(latch_Pin, OUTPUT);
  pinMode(clock_Pin, OUTPUT);
  pinMode(data_Pin, OUTPUT);
  pinMode(led_Pin, OUTPUT);
  pinMode(datchikpin, INPUT);
  analogWrite(mosfetpin, 160);
  //myRTC.setDS1302Time(30, 49, 23, 7, 7, 8, 2021); // секунды, минуты, часы, день недели, день месяца, месяц, год

}
void OutputWatch(int XX, int YY)
{
  if (round(millis() / 1000) % 2 == 0)
  {
    xw = XX / 10 % 10;
    yw = XX % 10;
    z = YY / 10 % 10;
    a = YY % 10;

    digitalWrite(led_Pin, HIGH);
    ledflag = true;
    digitalWrite(latch_Pin, LOW);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[a]);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[z]);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[yw]);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[xw]);
    digitalWrite(latch_Pin, HIGH);
  }
  else {
    digitalWrite(led_Pin, LOW);
    ledflag = false;
  }
}
void OutputData(int XX, int YY)
{
  if (round(millis() / 1000) % 2 == 0)
  {
    x = XX / 10 % 10;
    y = XX % 10;
    z = YY / 10 % 10;
    a = YY % 10;
    digitalWrite(latch_Pin, LOW);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[a]);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[z]);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[y]);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[x]);
    digitalWrite(latch_Pin, HIGH);
  }
}
void OutputHumTemp(int XX, int YY) {
  if (!ledflag) {
    digitalWrite(led_Pin, HIGH);
  }
  if (round(millis() / 1000) % 2 == 0)
  {
    x = XX / 10 % 10;
    y = XX % 10;
    z = YY / 10 % 10;
    a = YY % 10;
    digitalWrite(latch_Pin, LOW);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, Temp_Hum[z]);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, Temp_Hum[a]);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[y]);
    shiftOut(data_Pin, clock_Pin, MSBFIRST, segments[x]);
    digitalWrite(latch_Pin, HIGH);
  }
}
void Time()
{
  myRTC.updateTime();
  XX = myRTC.hours;
  YY = myRTC.minutes;
  second = myRTC.seconds;
  OutputWatch(XX, YY);
}
void Data()
{
  myRTC.updateTime();
  XX = myRTC.dayofmonth;
  YY = myRTC.month;
  if (!ledflag) {
    digitalWrite(led_Pin, HIGH);
  }
  OutputData(XX, YY);
}
void Temp()
{
  myRTC.updateTime();
  XX = dht.readTemperature();
  YY = 10;
  OutputHumTemp(XX, YY);
}
void Hum()
{
  myRTC.updateTime();
  XX = dht.readHumidity();
  YY = 23;
  OutputHumTemp(XX, YY);
}
void Minus()
{
  if (yw == 1)
  {
    count = true;
  }
  if (count == true && yw == 2 && second == 59)
  {
    count = false;
    myRTC.updateTime();
    XX = myRTC.hours;
    YY = myRTC.minutes;
    int MM = myRTC.dayofmonth;
    int MMM = myRTC.month;
    int m = myRTC.seconds - 10;
    myRTC.setDS1302Time(m, YY, XX, 7, MM, MMM, 2021);
  }
}
void Cycle()
{
  uint8_t cycle = (millis() / 500) % 36;
  if (cycle < 30) {
    Time();
  }
  else if (cycle < 34) {
    Data();
  }
//  else if (cycle < 38) {
//    Temp();
//  }
//  else if (cycle < 42) {
//    Hum();
//  }
}
void Datchik()
{
  if (round(millis() / 500) % 2 == 0)
  {
    val = digitalRead(datchikpin);
    if (val == 1)
    {
      analogWrite(mosfetpin, 160);
    }
    else
    {
      analogWrite(mosfetpin, 25);
    }
  }
}
void loop() {
  Cycle();
  Minus();
  Datchik();
}
