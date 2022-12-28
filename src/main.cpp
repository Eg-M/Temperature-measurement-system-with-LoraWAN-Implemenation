/*
  Lora Send And Receive
  This sketch demonstrates how to send and receive data with the MKR WAN 1300 LoRa module.
  This example code is in the public domain.
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <MKRWAN.h>
#include <CayenneLPP.h>
#include <Adafruit_SleepyDog.h>

// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4
volatile int sleepMS;


LoRaModem modem;
CayenneLPP lpp(51);
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer, outsideThermometer;

// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);

// Prototypes of the functions
void printData(DeviceAddress deviceAddress);
void printAddress(DeviceAddress deviceAddress);
void deep_sleep ();

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode())
    Serial.println("ON");
  else
    Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0))
    Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(outsideThermometer, 1))
    Serial.println("Unable to find address for Device 1");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(outsideThermometer);
  Serial.println();
  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 12);
  sensors.setResolution(outsideThermometer, 12);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(outsideThermometer), DEC);
  Serial.println();

  pinMode(LED_BUILTIN, OUTPUT); // sign of live
  for (int i = 1; i <= 10; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(1000);                     // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    delay(1000);
    // wait for a second
    Serial.println(10 - i);
  }

  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868))
  {
    Serial.println("Failed to start module");
    while (1)
    {
    }
  };

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected)
  {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1)
    {
    }
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);

  // NOTE: independently by this setting the modem will
  // not allow to send more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.
  
}

void loop()
{

  // if (!(millis() % 60000))
  //{
  //Serial.print("wdt period");
  //Serial.println(wdt_period);
  


  sensors.requestTemperatures(); // Send the command to get temperatures

  lpp.reset();
  lpp.addTemperature(1, sensors.getTempC(insideThermometer));
  lpp.addTemperature(2, sensors.getTempC(outsideThermometer));
  lpp.addAnalogInput(3, 3.31f);
  modem.beginPacket();
  modem.write(lpp.getBuffer(), lpp.getSize());
  if (modem.endPacket(true) > 0)
  {
    Serial.println("Message sent correctly!");
  }
  else
  {
    Serial.println("Error sending message :(");
    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  }
  // delay(1000);
  if (!modem.available())
  {
    Serial.println("No downlink message received at this time.");
  }
  else
  {
    String rcv;
    rcv.reserve(64);
    while (modem.available())
    {
      rcv += (char)modem.read();
    }
    Serial.print("Received: " + rcv + " - ");
    for (unsigned int i = 0; i < rcv.length(); i++)
    {
      Serial.print(rcv[i] >> 4, HEX);
      Serial.print(rcv[i] & 0xF, HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println("waiting 60 seconds");

 

  deep_sleep();



  //}
}

 
void deep_sleep ()
{ 

  for (int i = 1; i <= 4; i++)

  {
     Watchdog.sleep();
    
  }

}

void printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  // Serial.print("Temp C: ");
  // Serial.print(sensors.getTempC(deviceAddress));
  // Serial.print(" Temp F: ");
  // Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  // Serial.print("Temp C: ");
  Serial.print(tempC);
  // Serial.print(" Temp F: ");
  // Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}