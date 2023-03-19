#include <Blynk.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "EEPROM.h"
#include <Servo.h>
Servo myservo;  // create servo object to control a servo

int pos = 0;    // variable to store the servo position
const int trigPin = 25;
const int echoPin = 26;

#define BLYNK_TEMPLATE_ID           "TMPLT8JIOtbf"
#define BLYNK_TEMPLATE_NAME         "ESP32"
#define BLYNK_AUTH_TOKEN            "jzM-tZZ_RGE0L028r2o5nAB4qvouQsA_"
#define BLYNK_PRINT Serial

#define EEPROM_SIZE 40 //1024
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
#define BUZZZER_PIN  5
#define LED 2

String ssidName;
String ssidPass;
String adminName;
char buffSSID[20]; 
char buffPASS[20]; 

WidgetLCD lcd(V1);
WidgetLED led(V2);

long duration;
float distanceCm;
float distanceInch;

BLYNK_WRITE(V0)
{
  int pinValue = param.asInt();

  if (pinValue ==0){
    digitalWrite(LED, HIGH);
  }
  else
    digitalWrite(LED, LOW);
}

void setup() {
  Serial.begin(115200); // Starts the serial communication
  EEPROM.begin(EEPROM_SIZE);

  byte counter = 0;
  while (!Serial.available())
  {
      Serial.print('.');
      delay(1000);
      counter++;
      if (counter >= 20) break;
  }

  Serial.println("[1/4] Write data from EEPROM");
  String outMessage = "";
  while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '>') {
        Serial.println("Recieve serial data");
        Serial.println(outMessage);
        Serial.println("Write to EEPROM");
        writeString(0, outMessage);
        Serial.println("Writing data success");
        break;
      }
      else
        outMessage.concat(inChar);
  }

  Serial.println("[2/4] Read data from EEPROM");
  String readData = read_String(0);
  Serial.println(readData);

  ssidName = getValue(readData, '#', 1); Serial.print("SSID NAME     : "); Serial.println(ssidName);
  ssidPass = getValue(readData, '#', 2); Serial.print("SSID PASS     : "); Serial.println(ssidPass);
  adminName = getValue(readData, '#', 3); Serial.print("ADMIN NAME     : "); Serial.println(adminName);


  ssidName.toCharArray(buffSSID, (ssidName.length() + 1));
  ssidPass.toCharArray(buffPASS, (ssidPass.length() + 1));
  
  Serial.println("[3/4] Finish read data from EEPROM !");
  
  WiFi.begin(buffSSID, buffPASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN, buffSSID, buffPASS);

  pinMode(LED, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(18, OUTPUT);
  pinMode(BUZZZER_PIN, OUTPUT);
  myservo.attach(19);  // attaches the servo on pin 13 to the servo object

}

void loop() {
  Blynk.run();

  Blynk.virtualWrite(V5, "User: ", adminName);
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculate the distance
    distanceCm = duration * SOUND_SPEED/2;
    
    // Convert to inches
    distanceInch = distanceCm * CM_TO_INCH;
    
    // Prints the distance in the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
    Serial.print("Distance (inch): ");
    Serial.println(distanceInch);
    Blynk.virtualWrite(V4, "Jarak: ", distanceCm, "cm");
    
    if (distanceCm <10){
      digitalWrite(18, HIGH);
      digitalWrite(BUZZZER_PIN, HIGH);
      led.on();
      lcd.print(4, 0, "Object");
      delay(500);
      lcd.print(4, 1, "Detected");
      delay(1500);
  
    }
    else {
      lcd.clear();
      led.off();
      digitalWrite(18, LOW);
      digitalWrite(BUZZZER_PIN, LOW);
    } 
      delay(30);
    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculate the distance
    distanceCm = duration * SOUND_SPEED/2;
    
    // Convert to inches
    distanceInch = distanceCm * CM_TO_INCH;
    
    // Prints the distance in the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
    Serial.print("Distance (inch): ");
    Serial.println(distanceInch);
    Blynk.virtualWrite(V4, "Jarak: ", distanceCm, "cm");
    
    if (distanceCm <10){
      digitalWrite(18, HIGH);
      digitalWrite(BUZZZER_PIN, HIGH);
      led.on();
      lcd.print(4, 0, "Object");
      delay(500);
      lcd.print(4, 1, "Detected");
      delay(1500);
    }
    else {
      lcd.clear();
      led.off();
      digitalWrite(18, LOW);
      digitalWrite(BUZZZER_PIN, LOW);
    } 
      delay(30);                    
  }

}
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void writeString(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
}

String read_String(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}