Code:
//define baro prs sensor
#include <Adafruit_BMP085.h> 
Adafruit_BMP085 bmp;
int systolic, diastolic,alert; 
long temperature,pressure;
#define alarm D3 
#define trig D7 
#define echo D8 
long time1;
long distance;
long val1,val2,val3,val4,val5,val6;
#include<Wire.h> 
#include"MAX30100_PulseOximeter.h" 
#define REPORTING_PERIOD_MS 1000
PulseOximeter pox; 
uint32_t tsLastReport = 0;
void onBeatDetected() // a loop to Print "beat" when a beat is detected
{
Serial.println("Beat!"); // print statement
}
#include <ESP8266WiFi.h> //include the necessary libraries to the program (ESP 
library)
#include <WiFiClient.h> // wifi communication library 
#include <ThingSpeak.h> // thingspeak library
const char* ssid = "OnePlus7"; // Your Network SSID (Your Hotspot name) to 
connect to wifi
const char* password = "00000000"; // Your Network Password (Your Hotspot 
password)
WiFiClient client; // Initialization for wifi communication to specific IP 
address(Your Thingspeak account)
unsigned long myChannelNumber = 234117; // Your Thingspeak Channel Number 
(Without Brackets)
const char * myWriteAPIKey = "H65RHJGZ95B6T89H"; // Your Thingspeak Write 
API Key
void setup()
{
Serial.begin(9600); 
pinMode(trig, OUTPUT);
pinMode(echo, INPUT); //DEFINE ECHO PIN AS INPUT
pinMode(alarm, OUTPUT); //DEFINE alarm AS OUTPUT 
delay(10);
WiFi.begin(ssid, password); //Connect to WiFi network 
ThingSpeak.begin(client); // begin communication through the network to 
thingspeak
// ~~~~~~~~~~~~~~~~~~~~~~~INITIALIZE PRESSURE 
SENSOR~~~~~~~~~~~~~~~~~~~~~~~~~~~
if (!bmp.begin())
{
Serial.println("Could not find a valid BMP085 sensor, check wiring!"); 
while (1) {}
}
// ~~~~~~~~~~~~~~~~~~~~~~~INITIALIZE PULSE OXIMETER 
SENSOR~~~~~~~~~~~~~~~~~~~~~~~~~~~
Serial.println("Initializing pulse oximeter..");
if (!pox.begin())
{
Serial.println("FAILED"); 
for(;;);
}
else
{
Serial.println("SUCCESS");
}
pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
pox.setOnBeatDetectedCallback(onBeatDetected);
}
void loop() {
// ~~~~~~~~~~~~~~~~~~~~~~~HEART RATE AND OXYGEN LEVEL 
MEASUREMENT~~~~~~~~~~~~~~~~~~~~~~~~~~~
pox.update();
if (millis() - tsLastReport > REPORTING_PERIOD_MS)
{
val4 = pox.getHeartRate();
val5 = pox.getSpO2();
val5 = constrain(val5,0,100); 
tsLastReport = millis();
// Make sure to call update as fast as possible
// ~~~~~~~~~~~~~~~~~~~~~~~TEMPERATURE 
MEASUREMENT~~~~~~~~~~~~~~~~~~~~~~~~~~~
temperature = bmp.readTemperature(); 
val1 = ((temperature*1.8)+46);
// ~~~~~~~~~~~~~~~~~~~~~~~PRESSURE 
MEASUREMENT~~~~~~~~~~~~~~~~~~~~~~~~~~~
pressure = bmp.readPressure();
val2 =map(pressure,100943,101200,90,120); //systolic 
val3 = map(pressure,100943,101200,60,89); //diastolic
// ~~~~~~~~~~~~~~~~~~~~~~~DISTANCE 
MEASUREMENT~~~~~~~~~~~~~~~~~~~~~~~~~~~
digitalWrite(trig, LOW); 
delayMicroseconds(2); 
digitalWrite(trig, HIGH); 
delayMicroseconds(10); 
digitalWrite(trig, LOW);
time1 = pulseIn(echo, HIGH); 
distance = time1 / 58;
val6 = map(distance,3,20,0,20); 
val6 = constrain(val6,0,20);
// ~~~~~~~~~~~~~~~~~~~~~~~SEND TO CLOUD LOGIC - IOT 
DISPLAY~~~~~~~~~~~~~~~~~~~~~~~~~~~
ThingSpeak.writeField(myChannelNumber, 1,val1, myWriteAPIKey); //write the 
temperature value to ThingSpeak in channel 1
ThingSpeak.writeField(myChannelNumber, 2,val2, myWriteAPIKey); //write the
temperature value to ThingSpeak in channel 2 
ThingSpeak.writeField(myChannelNumber, 3,val3, myWriteAPIKey); //write the
temperature value to ThingSpeak in channel 3 
ThingSpeak.writeField(myChannelNumber, 4,val4, myWriteAPIKey); //write the
temperature value to ThingSpeak in channel 4 
ThingSpeak.writeField(myChannelNumber, 5,val5, myWriteAPIKey); //write the
temperature value to ThingSpeak in channel 5 
ThingSpeak.writeField(myChannelNumber, 6,val6, myWriteAPIKey); //write the
temperature value to ThingSpeak in channel 6 
ThingSpeak.writeField(myChannelNumber, 7,alert, myWriteAPIKey); //write the
temperature value to ThingSpeak in channel 7
// ~~~~~~~~~~~~~~~~~~~~~~~SERIAL PRINT LOGIC - LOCAL 
DISPLAY~~~~~~~~~~~~~~~~~~~~~~~~~~~
Serial.print(" Temperature = "); 
Serial.print(val1);
Serial.print(" F"); 
Serial.print(" Systolic = "); 
Serial.print(val2); 
Serial.print(" Diastolic = "); 
Serial.print(val3);
Serial.print(" Heart beat (bpm):"); 
Serial.print(val4);
Serial.print(" Oxygen (%):"); 
Serial.print(val5);
Serial.print(" Distance (Cm):");
Serial.print(val6); 
Serial.print(" Alert:"); 
Serial.print(alert);
Serial.println();
}// end of pulse loop
// ~~~~~~~~~~~~~~~~~~~~~~~ALERT LOGIC~~~~~~~~~~~~~~~~~~~~~~~~~~~
// if temp greater than 100
// if systolic pressure less than 90
// if systolic pressure greater than 120
// if diastolic pressure less than 60
// if diastolic pressure greater than 89
// if heart rate less than 50
// if heart rate greater than 95
// if oxygen level less than 95
if((val1>=100)||(val2<90)||(val2>120)||(val3<60)||(val3>89)||(val4<50)||(val4>95)||(va 
l5<95))
{
digitalWrite(alarm,HIGH); 
alert = 1;
}
else
{
digitalWrite(alarm,LOW);
alert = 0;
}
} // end of void loop