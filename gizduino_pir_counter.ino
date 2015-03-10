#include <Wire.h>
#define DS1307_I2C_ADDRESS 0x68
#include <LiquidCrystal.h>   

int ledPin = 30;
int inputPin = 11;
int pirState = LOW;
int val = 0;
int pinSpeaker = 31;
int vis = 0;
char lcd_buffer[16];
LiquidCrystal lcd(21, 20, 19, 18, 17, 16);

byte decToBcd(byte val){
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val){
  return ( (val/16*10) + (val%16) );
}

void setDateDs1307( byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year){
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(0);
    Wire.write(decToBcd(second));
    Wire.write(decToBcd(minute));
    Wire.write(decToBcd(hour));
    Wire.write(decToBcd(dayOfWeek));
    Wire.write(decToBcd(dayOfMonth));
    Wire.write(decToBcd(month));
    Wire.write(decToBcd(year));
    Wire.write(0x10);
    Wire.endTransmission();
}

void getDateDs1307(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year){
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}

void playTone(long duration, int freq, boolean state) {
  if(state){
    digitalWrite(ledPin, HIGH);
  }else{
    digitalWrite(ledPin, LOW);
  }
  duration *= 1000;
  int period = (.3 / freq) * 100000;
  long elapsed_time = 0;
  while (elapsed_time < duration) {
    digitalWrite(pinSpeaker,HIGH);
    delayMicroseconds(period / 2);
    digitalWrite(pinSpeaker, LOW);
    delayMicroseconds(period / 2);
    elapsed_time += (period);
  }
}

void displayTime(){
  lcd.clear();
  lcd.setCursor(0, 0);
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  
  sprintf(lcd_buffer, " %02d, 20%02d ", dayOfMonth, year );
  lcd.print(getName(month, 1) + lcd_buffer + getName(dayOfWeek, 0));
  lcd.setCursor(0, 1);
  sprintf(lcd_buffer, "%02d:%02d:%02d", hour, minute, second );
  lcd.print(lcd_buffer);
  delay(1000);
}

String getName(byte day, int type){  
  switch(day){
    case 1:      
      return (type < 1) ? ("SUN") : ("JAN");
      break;
    case 2:
      return (type < 1) ? ("MON") : ("FEB");
      break;
    case 3:
      return (type < 1) ? ("TUE") : ("MAR");
      break;
    case 4:
      return (type < 1) ? ("WED") : ("APR");
      break;
    case 5:
      return (type < 1) ? ("THU") : ("MAY");
      break;
    case 6:
      return (type < 1) ? ("FRI") : ("JUN");
      break;
    case 7:
      return (type < 1) ? ("SAT") : ("JUL");
      break;
    case 8:
      return (type < 1) ? ("") : ("AUG");
      break;
    case 9:
      return (type < 1) ? ("") : ("SEP");
      break;
    case 10:
      return (type < 1) ? ("") : ("OCT");
      break;
    case 11:
      return (type < 1) ? ("") : ("NOV");
      break;
    case 12:
      return (type < 1) ? ("") : ("DEC");
      break;
  }
}

void displaySensorActive(){
  vis++;
  lcd.clear();
  lcd.setCursor(0, 0);   
  sprintf(lcd_buffer, "Sensor Active:");
  lcd.print(lcd_buffer);
  lcd.setCursor(0,1);
  sprintf(lcd_buffer, "(%d) times",vis );
  lcd.print(lcd_buffer);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pinSpeaker, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Gizduino+");
  lcd.setCursor(0, 1);   
  lcd.print("Boot Up ...");
  Wire.begin();
  Serial.begin(9600);
  setDateDs1307(30, 49, 3, 3, 10, 3, 14);
}

void loop() {
  val = digitalRead(inputPin);  
  boolean state = (val == HIGH);
  if(state){
    playTone(150, 150, state);
    if (pirState == LOW) {
      displaySensorActive();
    }    
    pirState = HIGH;    
  }else{
    playTone(0, 0, state);
    displayTime();
    pirState = LOW;
  }
  //displayTime();
}
