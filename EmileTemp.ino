/*********
  EMILE-Thermometer
  v0.1
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <DS18B20.h>
#include <PubSubClient.h>

// WIFI SETTINGS
#ifndef STASSID
#define STASSID "SSID"
#define STAPSK  "PASSWORD"
#endif
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) (SCL == D1 && SDA == D2)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Declaration for Temp-Sensors (ds(2) == D4)
DS18B20 ds(2);

const char* ssid     = STASSID;
const char* password = STAPSK;

// Temp-Sensoren
uint8_t address1[] = {40, 97, 100, 18, 46, 117, 84, 72};
uint8_t checksum1;
uint8_t address2[] = {40, 97, 100, 18, 41, 206, 246, 0};
uint8_t checksum2;
uint8_t aktuell;

// ting.io MQTT
const char* mqtt_server = "mqtt.tingg.io";
const int port = 1883;
const char* thing_id = "TINGG_ID";
const char* thing_key = "TINGG_KEY";
const char* username = "thing";


WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    if (!client.connect(thing_id, username, thing_key)) {
      delay(1000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  checksum1 = address1[0] + address1[1] + address1[2] + address1[3] + address1[4] + address1[5] + address1[6] + address1[7];
  checksum2 = address2[0] + address2[1] + address2[2] + address2[3] + address2[4] + address2[5] + address2[6] + address2[7];
  client.setServer(mqtt_server, port);
  //client.setCallback(callback);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 4);
  // Display static text
  display.println("WLAN-Thermometer v0.1");
  display.display(); 

  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(18, 16);
  // Display static text
  display.println("AK-IT");
  display.display();

  Serial.println();
  Serial.print("Checksum 1 ");
  Serial.println(checksum1);
  Serial.print("Checksum 2 ");
  Serial.println(checksum2);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(12, 42);
  // Display static text
  display.println("Initialisiere...");
  display.display();


  // We start by connecting to a WiFi network
  delay(2000);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(12, 42);
  // Display static text
  display.println("Initialisiere...");
  display.display();
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(2000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 48);
  // Display static text
  display.println("Verbinde mit ");
  display.print(ssid);
  display.display();
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 4);
  // Display static text
  display.println("WLAN-Settings");
  display.display(); 

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 16);
  display.println("IP-Adresse: ");
  display.setCursor(8, 26);
  display.println(WiFi.localIP());
  display.display();
  display.setCursor(0, 40);
  display.println("Verbunden mit:");
  display.setCursor(8, 50);
  display.println(ssid);
  display.display();
    delay(5000);

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 4);
  // Display static text
  display.println("Temperaturfuehler");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 16);
  display.println("Anzahl der Fuehler:");
  display.println(ds.getNumberOfDevices());  
  display.display();
  delay(5000);  
}

void loop() {
  while (ds.selectNext()) {
    uint8_t address[8];
    ds.getAddress(address);
    aktuell = address[0] + address[1] + address[2] + address[3] + address[4] + address[5] + address[6] + address[7];    
    Serial.print("Address:");
    for (uint8_t i = 0; i < 8; i++) {
      Serial.print(" ");
      Serial.print(address[i]);
    }
    Serial.println();
    Serial.print("Temperature: ");
    Serial.print(ds.getTempC());
    Serial.print(" C / ");
    Serial.print(ds.getTempF());
    Serial.println(" F");
    Serial.println();

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 4);
      // Display static text
      display.print("Temperatur ");
      if (aktuell == checksum1){
        display.print("Innen");
      }
      if (aktuell == checksum2){
        display.print("Aussen");
      }
      display.setTextSize(4);
      display.setTextColor(WHITE);
      display.setCursor(0, 16);
      display.print(ds.getTempC());
      display.display();
    delay(2000);
  }
  if (!client.connected()) {
    reconnect();
  }
  while (ds.selectNext()) {
    float myTemp1Float = ds.getTempC();
    String myTemp1String = String(myTemp1Float, 2);
    char myTemp1CharArray[6];
    myTemp1String.toCharArray(myTemp1CharArray,6);
    client.publish("Temp1", myTemp1CharArray);
  }
  while (ds.selectNext()) {
    float myTemp2Float = ds.getTempC();
    String myTemp2String = String(myTemp2Float, 2);
    char myTemp2CharArray[6];
    myTemp2String.toCharArray(myTemp2CharArray,6);
    client.publish("Temp2", myTemp2CharArray);
  }

  delay(11000);
}
