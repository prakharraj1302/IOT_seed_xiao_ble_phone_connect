#include <ArduinoBLE.h>
#include "LSM6DS3.h"
// #include "Wire.h"  

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");  // Bluetooth® Low Energy LED Service
BLEService tempService("19B10002-E8F2-537E-4F6C-D104768A1214");

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic tempCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A
char cstr[16];

const int ledPin = LED_BUILTIN;  // pin to use for the LED
const int temppin = TempOUTPUT;

  void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);
  if (myIMU.begin() != 0) {
        Serial.println("Device error");
    } else {
        Serial.println("Device OK!");
    }

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }
  


  // set advertised local name and service UUID:
  BLE.setLocalName("SEEED XIAO BLE");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);
  tempService.addCharacteristic(tempCharacteristic);

  // add service
  BLE.addService(ledService);
  BLE.addService(tempService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral and Temp");
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {
          Serial.println("LED on");
          digitalWrite(ledPin, LOW);  // changed from HIGH to LOW
        } else {
          Serial.println(F("LED off"));
          digitalWrite(ledPin, HIGH);  // changed from LOW to HIGH
        }
      }
      if(myIMU.readTempC()){
        
        // Serial.println(myIMU.readTempC());
        // tempCharacteristic.writeValue(itoa( ((int)myIMU.readTempC()), cstr, 10));
        tempCharacteristic.writeValue( (byte) myIMU.readTempC());
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
