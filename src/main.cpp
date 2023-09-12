#include <MKRWAN.h>
#include <CayenneLPP.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoLowPower.h>
#include "arduino_secrets.h"

// OneWire settings
const int oneWirePin = 4;
OneWire oneWire(oneWirePin);
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer, outsideThermometer;

// Analog pin settings
const int analogPin = A1;
float analogValue;

// LoRaWAN and CayenneLPP
LoRaModem modem;
CayenneLPP lpp(51);
unsigned long lastSend = 0;

// Sleep settings
const unsigned long sleepTime = 60000; // Sleep for 1 minute

// Connection flag
bool isConnected = false;

// Functions declarations
void sendData();
void goToSleep();
void alarmEvent();
void reboot();

void setup() {



  Serial.begin(115200);
  analogReference(AR_DEFAULT);

  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, alarmEvent, CHANGE);



  if (!sensors.getAddress(insideThermometer, 0))
  Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(outsideThermometer, 1))
  Serial.println("Unable to find address for Device 1");


  // Initialize sensors
  sensors.begin();
  // set the resolution to 12 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 12);
  sensors.setResolution(outsideThermometer, 12);
  
  // Initialize LoRaWAN
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    delay(1000);
    reboot();
  }
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  if (!isConnected) {
    int connected = modem.joinOTAA(appEui, appKey);
    if (!connected)
    {
      Serial.println("Something went wrong; are you indoor? Move near a window and retry");
      reboot();
    }
    else {
      Serial.println("Connected to the network");
      isConnected = true;
    }
  }

  modem.minPollInterval(60);
  modem.setADR(true);
  Serial.end();
  USBDevice.detach();
}

void loop() {

   
  sendData();
  goToSleep();
}

void sendData() {
  sensors.requestTemperatures();
  float temp1 = sensors.getTempC(insideThermometer);
  float temp2 = sensors.getTempC(outsideThermometer);
  analogValue = analogRead(analogPin) * 3.3f / 1023.0f / 1.03f * (1.03f+0.325f);; 



  lpp.reset();
  lpp.addTemperature(1, temp1);
  lpp.addTemperature(2, temp2);
  lpp.addAnalogInput(3, analogValue); 


//Debug buffer 

  // Serial.print("Sending: ");
  // for (int i = 0; i < lpp.getSize(); i++) {
  //   Serial.print(lpp.getBuffer()[i], HEX);
  //   Serial.print(" ");
  // }
  // Serial.println();

  modem.beginPacket();
  modem.write(lpp.getBuffer(), lpp.getSize());
  int err = modem.endPacket(true);
  if (err > 0) {
    //Debug serial
    //Serial.println("Message sent correctly!");
  } else {
    //Serial.println("Error sending message :(");
  }
}

void goToSleep() {
  LowPower.sleep(sleepTime);
}

void alarmEvent() {
// RTC alarm wake interrupt callback
// do nothing
}

void reboot() {
  NVIC_SystemReset();
  while(1) ;
}

