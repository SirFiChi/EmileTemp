# EmileTemp
TempSensors with one ESP8266, multiple DS18B20 and MQTT transfer to tingg.io.

# Version v0.1
Scipt is running an the output is shown on tingg.io

# Instructions TINGG.io
1. Create Account and/or login
2. Create a Tingg ("Things" --> "Connect a Thing")
3. Click on the new Thing --> "Configure Thing Type Resources" --> "New Resource"
4. Enter the topic "Temp1", choose Method "Publish data" and choose the type "Number". Leave the dialog with "configure".
5. Repeat step 3 and 4 for the topic "Temp2"
6. Navigate back to "Things" and the name of your new "Tingg" and see the transmitted data.
7. You can see "Live data" and "Charts" by pressing the links...

# Instructions (ESP8266)
1. Change the settings for WIFI and TINGG.io
2. Upload the script to an ESP8266
3. Assemble the Display (SDA - D2 | SCL - D1 | VCC - 3V | GND - G)
4. Assemble the temp-sensors (Data - D4 | VCC - 3V | GND - G | 4.7kOhm resistor between VCC and Data)
5. Start the messurment buy powering the ESP8266

# ToDo
1. Addressing of the Temp-Sensors to minimizie side effects (same temperature for booth sensors)
2. Better documentation in the .ino file

# Sample
![Picture of the device](https://repository-images.githubusercontent.com/410052560/ea7028f5-4d61-4c8c-aa26-e4e332b2d1cc)
