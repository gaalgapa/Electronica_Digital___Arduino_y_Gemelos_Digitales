#include <OneWire.h>
#include <DallasTemperature.h>

const int pinDatos = 3;
OneWire oneWire(pinDatos);
DallasTemperature sensors(&oneWire);

DeviceAddress tempDeviceAddress;

void setup() {
  Serial.begin(9600);
  sensors.begin();

  Serial.print("Sensores encontrados: ");
  Serial.println(sensors.getDeviceCount());

  for (int i = 0; i < sensors.getDeviceCount(); i++) {
    if (sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(" Dirección: ");
      for (uint8_t j = 0; j < 8; j++) {
        if (tempDeviceAddress[j] < 16) Serial.print("0");
        Serial.print(tempDeviceAddress[j], HEX);
      }
      Serial.println();
    } else {
      Serial.print("No se pudo obtener dirección del sensor ");
      Serial.println(i);
    }
  }
}

void loop() {}