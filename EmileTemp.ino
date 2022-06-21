/*********
  EMILE-Thermometer
  Version = v0.3
*********/

//
//// Include some Libraries
//
#include <OneWire.h>              // Access 1-wire temperature sensors, memory and other chips
#include <DallasTemperature.h>    // Arduino Library for Dallas Temperature ICs - Supports DS18B20, DS18S20, DS1822, DS1820
#include <DS18B20.h>              // Arduino library for the Maxim Integrated DS18B20 1-Wire temperature sensor
#include <Adafruit_GFX.h>         // Grafik-Library OLED Display
#include <Adafruit_SSD1306.h>     // Treiber-Library OLED Display
#include <ESP8266WiFi.h>          // WLAN Client
#include <PubSubClient.h>         // A client library for MQTT messaging

// 
//// SETTINGS
//
const char* ver = "0.3";                                            // Setze die Versionsnummer des Skriptes, damit es nur einmal gesetzt werden muss.
// WIFI SETTINGS
const char* ssid     = "SSID";                              // WIFI SSID 
const char* password = "PW";                      // WIFI Password
// Temp-Sensor Offset
float temp1offset = 0;                                              // Offset Sensor 1
float temp2offset = 2;                                              // Offset Sensor 2
// OLED SETTINGS
#define SCREEN_WIDTH 128                                            // OLED display width, in pixels
#define SCREEN_HEIGHT 64                                            // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);   // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) (SCL == D1 && SDA == D2)
// MQTT Server
const char* mqtt_server = "URL";                           // Adresse des MQTT-Servers
const int port = 1883;                                              // Port des MQTT-Servers
const char* thing_id = "mqtt";                                      // 
const char* thing_key = "2MalMQTT";                                 // 
const char* username = "mqtt";                                      // 
// Declaration for Temp-Sensors
#define ONE_WIRE_BUS 2                                              // Definiere den Daten-Bus der Thermostate (ds(2) == Pin D4)
// Initialisiere WiFi, MQTT, OneWire und Temperatursensor
WiFiClient espClient;                                               // Initialize the WiFi-client library
PubSubClient client(espClient);                                     // Creates a partially initialised MQTT-client instance
OneWire oneWire(ONE_WIRE_BUS);                                      // Setup a oneWire instance to communicate with any OneWire device
DallasTemperature sensors(&oneWire);                                // Pass oneWire reference to DallasTemperature library

//
//// Function to reconnect to the MQTT Broker
//
void reconnect() {
  while (!client.connected()) {
    if (!client.connect(thing_id, username, thing_key)) {
      delay(1000);
    }
  }
}


//
//// Initial setup und einmalig ausgeführte Aktionen
//
void setup() {
  Serial.begin(115200);                                             // Enable Serial Port as OutPut
  client.setServer(mqtt_server, port);                              // 
  sensors.begin();                                                  // Begin the DS18B20 initialization
// Initializing the OLED Display and clear it
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {                  // Versuche Adresse 0x3C für das Display (SSD1306, 128x64) zu setzen
    Serial.println(F("SSD1306 allocation failed"));                 // Falls das Display nicht gefunden wird löst das vorherige if eine Ausgabe am seriellen Port aus
    for(;;);                                                        // Bricht ggf. die Schleife ab
  }
  delay(500);                                                       // Warte eine halbe Sekunde
  display.clearDisplay();                                           // Leere das Display
// Just some informations on the OLED Display, without functionality. The start screen.
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 4);
  display.println((String)"WLAN-Thermometer v"+ver);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(18, 16);
  display.println("AK-IT");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(12, 42);
  display.println("Initialisiere...");
  display.display();  
//Just the same output on the serial port. Additional Info in case someone find the ESP8266 without knowledge of the content
  Serial.println((String)"WLAN-Thermometer v"+ver);
  Serial.println("AK-IT");
  Serial.println("Initialisiere...");
  Serial.println("(You can find additional informations here: https://github.com/SirFiChi/EmileTemp/)");
  Serial.println();
// Deleting the "Initialisiere" message on the OLED and replace it with "Verbinde mit" and the name of the SSID
  delay(1000);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(12, 42);
  display.println("Initialisiere...");
  display.display();
  delay(2000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 48);
  display.println("Verbinde mit ");
  display.print(ssid);
  display.display();
// Print the same info on the serial port
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
//
//// Connecting to the WiFi network
//
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
// Clear the OLED Display and give some Infos about the WIFI connection
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 4);
  display.println("WLAN-Settings");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 16);
  display.println("IP-Adresse: ");
  display.setCursor(8, 26);
  display.println(WiFi.localIP());
  display.setCursor(0, 40);
  display.println("Verbunden mit:");
  display.setCursor(8, 50);
  display.println(ssid);
  display.display();
// Put the same info on the serial port
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
// Wait two seconds
  delay(2000); 
}

//
//// Aktionen die in der Schleife laufen, bis ein Abbruch ausgelöst wird
//
void loop() {
  sensors.requestTemperatures();                                           // Call all sensors on one wire to start calculating the temperature readings
  if (!client.connected()) {                                               // Wenn der MQTT getrennt wurde...
    reconnect();                                                           // ...verbinde neu.
  }
// Forme die Daten der Sensoren und übertrage sie per MQTT
  float myTemp1Float = sensors.getTempCByIndex(0) + temp1offset;           // Get temperature reading from sensor 0 in celsius scale and set offset
  String myTemp1String = String(myTemp1Float, 2);                          // 
  char myTemp1CharArray[6];                                                // 
  myTemp1String.toCharArray(myTemp1CharArray,6);                           // 
  String PacketJsonData1 = "{\"Temp1\":"+myTemp1String+"}";                // form the json-data als Wert "Temp1"
  client.publish("eMile/1OG/Temp_Innen", PacketJsonData1.c_str(), true);   // Sende die Daten per MQTT mit dem Topic "eMile/1OG/Temp_Innen"
  float myTemp2Float = sensors.getTempCByIndex(1) + temp2offset;           // Get temperature reading from sensor 1 in celsius scale and set offset
  String myTemp2String = String(myTemp2Float, 2);                          // 
  char myTemp2CharArray[6];                                                // 
  myTemp2String.toCharArray(myTemp2CharArray,6);                           // 
  String PacketJsonData2 = "{\"Temp2\":"+myTemp2String+"}";                // form the json-data als Wert "Temp2"
  client.publish("eMile/1OG/Temp_Aussen", PacketJsonData2.c_str(), true);  // Sende die Daten per MQTT mit dem Topic "eMile/1OG/Temp_Aussen"
// Gib die Temperaturen auch auf dem seriellen Port aus
  Serial.println();
  Serial.print("Temperature 1: ");
  Serial.print(myTemp1CharArray);
  Serial.print(" °C");
  Serial.println();
  Serial.print("Temperature 2: ");
  Serial.print(myTemp2CharArray);
  Serial.print(" °C");
  Serial.println();
// Eine Schleife um die Werte auf dem Display zu wechseln, aber die Temperatur nur einmal pro Minute zum MQTT-Broker zu übertragen
  for (uint8_t counter = 0; counter < 6; counter++) {                     // Start a loop, so the upload is paused but the display changes the sensors values.
   for (uint8_t i = 0; i < 2; i++) {                                      // Schleife um die beiden Werte zu wechseln
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 56);
    display.print("Upload alle 60 Sek.");
    display.setTextColor(WHITE);
    display.setCursor(0, 4);
    display.print("Temperatur ");
    if (i == 0){
      display.print("Innen");
    }
    if (i == 1){
      display.print("Aussen");
    }
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(0, 16);
    if (i == 0){
      display.print(myTemp1CharArray);
    }
    if (i == 1){
      display.print(myTemp2CharArray);
    }
    display.display();
    delay(5000);
   }
  }
}
