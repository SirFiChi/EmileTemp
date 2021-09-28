/*********
  EMILE-Thermometer
  v0.2
*********/

// Include some Libraries
///// #include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>          // WLAN Client
#include <Adafruit_GFX.h>         // Grafik-Library OLED Display
#include <Adafruit_SSD1306.h>     // Treiber-Library OLED Display
#include <DS18B20.h>
#include <PubSubClient.h>

// 
//// SETTINGS
//
// WIFI SETTINGS
const char* ssid     = "";                                     // WIFI SSID 
const char* password = "";                             // WIFI Password
// Temp-Sensor Offset
float temp1offset = 0;                                              // Offset Sensor 1
float temp2offset = 0.18;                                           // Offset Sensor 2
// OLED SETTINGS
#define SCREEN_WIDTH 128                                            // OLED display width, in pixels
#define SCREEN_HEIGHT 64                                            // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);   // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) (SCL == D1 && SDA == D2)
// tingg.io MQTT
const char* mqtt_server = "mqtt.tingg.io";
const int port = 1883;
const char* thing_id = "";      // Can be found in the settings of a Thing
const char* thing_key = "";         // Can be found in the settings of a Thing
const char* username = "thing";
// Declaration for Temp-Sensors
#define ONE_WIRE_BUS 2                                              // (ds(2) == D4)

WiFiClient espClient;
PubSubClient client(espClient);


// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);


// Function to reconnect to the MQTT Broker
void reconnect() {
  while (!client.connected()) {
    if (!client.connect(thing_id, username, thing_key)) {
      delay(1000);
    }
  }
}


void setup() {
  // Initial setup
  Serial.begin(115200);                     // Enable Serial Port as OutPut
  client.setServer(mqtt_server, port);
  
  sensors.begin();                        // Begin the DS18B20 initialization

  // Initializing the OLED Display and clear it
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(500);
  display.clearDisplay();

  // Just some informations on the OLED Display, without functionality. The start screen.
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 4);
  display.println("WLAN-Thermometer v0.2");
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
  Serial.println("WLAN-Thermometer v0.2");
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
  
  // We start by connecting to a WiFi network
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
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

void loop() {
  sensors.requestTemperatures(); //Call all sensors on one wire to start calculating the temperature readings

  if (!client.connected()) {
    reconnect();
  }
  float myTemp1Float = sensors.getTempCByIndex(0) + temp1offset;   // Get temperature reading from sensor 0 in celsius scale and set offset
  String myTemp1String = String(myTemp1Float, 2);
  char myTemp1CharArray[6];
  myTemp1String.toCharArray(myTemp1CharArray,6);
  client.publish("Temp1", myTemp1CharArray);
  float myTemp2Float = sensors.getTempCByIndex(1) + temp2offset;   // Get temperature reading from sensor 1 in celsius scale and set offset
  String myTemp2String = String(myTemp2Float, 2);
  char myTemp2CharArray[6];
  myTemp2String.toCharArray(myTemp2CharArray,6);
  client.publish("Temp2", myTemp2CharArray);

  Serial.println();
  Serial.print("Temperature 1: ");
  Serial.print(myTemp1CharArray);
  Serial.print(" °C");
  Serial.println();
  Serial.print("Temperature 2: ");
  Serial.print(myTemp2CharArray);
  Serial.print(" °C");
  Serial.println();

  for (uint8_t counter = 0; counter < 6; counter++) {             // Start a loop, so the upload is paused but the display changes the sensors values.
   for (uint8_t i = 0; i < 2; i++) {
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
