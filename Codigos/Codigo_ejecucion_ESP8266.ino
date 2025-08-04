#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// 📡 WiFi Credentials
const char* ssid = "moto g72_8732";        
const char* password = "UJNyedv7841";

// 🔗 Google Apps Script Webhook
const char* serverName = "https://script.google.com/macros/s/AKfycbyQSZIkZfSy1WOlet3MgD-S9BIi7W52rqh1Mr5o8eNv77CKXNSGlMArPZN8xsPCj__f/exec";

// 🧊 Pin de datos de los sensores DS18B20
#define ONE_WIRE_BUS D4  
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// 🧊 Direcciones HEX de los sensores
const char* hexAddresses[] = {
  "2861640B49BAFB7E",    //Sensor 1
  "28E11548F6973CBE",    //Sensor 2
  "2861640B4B38E7B1",    //Sensor 3
  "2861640B4E1367EA",    //Sensor 4
  "282BB748F65C3CC9",    //Sensor 5
  "2861640B4E0E8827",    //Sensor 6
  "286164098108F661",    //Sensor 7
  "286164098105011F",    //Sensor 8
  "2861640B4B372EFF",    //Sensor 9
  "28ABCB2833200121"     //Sensor 10
};

const int numSensores = sizeof(hexAddresses) / sizeof(hexAddresses[0]);
DeviceAddress sensorAddresses[numSensores];

// 🔧 Convierte string HEX a DeviceAddress
void hexStringToDeviceAddress(const char* hex, DeviceAddress addr) {
  for (int i = 0; i < 8; i++) {
    char byteStr[3] = { hex[i * 2], hex[i * 2 + 1], '\0' };
    addr[i] = strtoul(byteStr, NULL, 16);
  }
}

void setup() {
  Serial.begin(115200);
  sensors.begin();

  // Conexión WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Inicialización de sensores
  Serial.println("Iniciando sensores DS18B20...");
  for (int i = 0; i < numSensores; i++) {
    hexStringToDeviceAddress(hexAddresses[i], sensorAddresses[i]);
    if (!sensors.isConnected(sensorAddresses[i])) {
      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.println(" no conectado.");
    }
  }
  Serial.println("Inicialización completa.");
}

void loop() {
  sensors.requestTemperatures();
  float tiempo = millis() / 1000.0;

  Serial.print("-----------\tTiempo: ");
  Serial.print(tiempo, 2);
  Serial.println(" s");

  // Construir URL con los datos
  String url = String(serverName) + "?tiempo=" + String(tiempo, 2);
  
  for (int i = 0; i < numSensores; i++) {
    float temp = sensors.getTempC(sensorAddresses[i]);
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    
    if (temp != DEVICE_DISCONNECTED_C) {
      Serial.print(temp, 4);
      Serial.println(" °C");
      url += "&s" + String(i + 1) + "=" + String(temp, 4);
    } else {
      Serial.println("Desconectado");
      url += "&s" + String(i + 1) + "=NaN";
    }
  }

  // Enviar datos si WiFi está conectado
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;                // ✅ Cliente WiFi necesario para el nuevo formato
    client.setInsecure();  // ⚠️ Esta línea es CLAVE para conexiones HTTPS en ESP8266
    HTTPClient http;
    http.setTimeout(10000);

    http.begin(client, url);           // ✅ Nuevo formato de begin()

    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Respuesta: " + payload);
    } else {
      Serial.print("Error HTTP: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("WiFi desconectado");
  }

  delay(10000);  // Espera 10 segundos
}
