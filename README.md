# EmileTemp
TempSensors with one ESP8266, multiple DS18B20 and MQTT transfer to own MQTT Broker.

# Version v0.3
Scipt is running an the output is sent to MQTT Broker

# Instructions HomeAssistant
1. [ToDo]

# Instructions (ESP8266)
1. Change the settings for WIFI and your MQTT Broker
2. Upload the script to an ESP8266
3. Assemble the Display (SDA - D2 | SCL - D1 | VCC - 3V | GND - G)
4. Assemble the temp-sensors (Data - D4 | VCC - 3V | GND - G | 4.7kOhm resistor between VCC and Data)
5. Start the messurment by powering the ESP8266

# ToDo
1. Better documentation in the .ino file

# Sample
![Picture of the device](https://repository-images.githubusercontent.com/410052560/ea7028f5-4d61-4c8c-aa26-e4e332b2d1cc)
