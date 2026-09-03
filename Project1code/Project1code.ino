#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Screen
#define SDA 7
#define SCL 6
LiquidCrystal_I2C lcd(0x27,16,2);

//Sensor
#define trigPin 4
#define echoPin 5
#define MAX_DISTANCE 700
float timeOut = MAX_DISTANCE * 60;
int soundVelocity = 340;

//Buzzer
#define BUZZER_PIN 15
const float WARN_THRESHOLD = 10.0;

//LED bar
int latchPin = 13;
int clockPin = 14;
int dataPin = 12;

float filterBuffer[5];
int filterIndex = 0;
float lastValidDistance = 0.0f;
int badReadingCounter = 0;
const int MAX_BAD_SAMPLE = 8;

float filterDistance(float newDist){
  // skip NAN, do not add to buffer
  if(isnan(newDist)){
    return NAN;
  }
  filterBuffer[filterIndex] = newDist;
  filterIndex = (filterIndex +1) %5;
  float sum=0;
  for(int i=0;i<5;i++) sum += filterBuffer[i];
  return sum /5.0f;
}
// Test I2C LCD address
bool i2CAddrTest(uint8_t addr) {
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}

void writeTo595(int order, byte _data) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, order, _data);
  digitalWrite(latchPin, HIGH);
}

float getSonar(){
  unsigned long pingTime;
  float distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pingTime = pulseIn(echoPin, HIGH, (unsigned long)timeOut);
  if(pingTime == 0){
    return NAN;
  }
  distance = (float)pingTime * soundVelocity / 2 / 10000;
  return distance;
}

byte distanceToLed(float dist) {
  byte pattern = 0x00;
  if(isnan(dist) || dist < 0) return 0x00; // fix NAN case
  if(dist < 100) pattern |= (1 << 7);
  if(dist < 85)  pattern |= (1 << 6);
  if(dist < 70)  pattern |= (1 << 5);
  if(dist < 55)  pattern |= (1 << 4);
  if(dist < 40)  pattern |= (1 << 3);
  if(dist < 25)  pattern |= (1 << 2);
  if(dist < 15)  pattern |= (1 << 1);
  if(dist < 8)   pattern |= (1 << 0);
  return pattern;
}
void setup() {
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pinMode(BUZZER_PIN, OUTPUT);
 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(dataPin, OUTPUT);
 
 Serial.begin(115200);
 Wire.begin(SDA,SCL);
 
 if (!i2CAddrTest(0x27)){
    lcd = LiquidCrystal_I2C(0x3F, 16,2);
 }
 lcd.init();
 lcd.backlight();
 lcd.setCursor(0, 0);
 lcd.print("ready");
 delay(1000);
 lcd.clear();
}

void loop() {
float distance = getSonar();
  delay(200);

  Serial.printf("Distance: %.1f cm | ", distance);
  byte ledPattern = distanceToLed(distance);
  Serial.print("ledPattern: ");
  Serial.println(ledPattern, BIN);

  lcd.setCursor(0,0);
  lcd.print("dist:");
  lcd.print(distance,1);
  lcd.print("cm        ");
  lcd.setCursor(0,1);
  lcd.print("                ");

  writeTo595(LSBFIRST, ledPattern);

 if(!isnan(distance) && distance > 0 && distance < 10.0 ){
    digitalWrite(BUZZER_PIN, HIGH);
  }else{
    digitalWrite(BUZZER_PIN, LOW);
  }
}