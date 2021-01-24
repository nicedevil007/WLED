#include "wled.h"

#include "DHT.h"

/*
 * This v1 usermod file allows you to add own functionality to WLED more easily
 * See: https://github.com/Aircoookie/WLED/wiki/Add-own-functionality
 * EEPROM bytes 2750+ are reserved for your custom use case. (if you extend #define EEPSIZE in const.h)
 * If you just need 8 bytes, use 2551-2559 (you do not need to increase EEPSIZE)
 * 
 * Consider the v2 usermod API if you need a more advanced feature set!
 */

//Use userVar0 and userVar1 (API calls &U0=,&U1=, uint16_t)

// PIR sensor pin
const int MOTION_PIN = 4; // D2
 // PIR MQTT topic
const char MQTT_TOPIC_PIR[] = "/s/pir";

int prevState = LOW;

// DHT sensor pin
const int DHT_PIN = 5; // D1
#define DHTtype DHT22
DHT dht(DHT_PIN, DHTtype);
 // DHT MQTT topic
const char MQTT_TOPIC_TEMP[] = "/s/temp";
const char MQTT_TOPIC_HUM[] = "/s/hum";


const long UPDATE_MS = 60000; // Upper threshold between mqtt messages

// variables
long lastTime = 0;
long timeDiff = 0;
long readTime = 0;

//gets called once at boot. Do all initialization that doesn't depend on network here
void userSetup()
{
  pinMode(MOTION_PIN, INPUT);
  dht.begin();
}

//gets called every time WiFi is (re-)connected. Initialize own network interfaces here
void userConnected()
{

}

void publishMqttPIR(String state)
{
  //Check if MQTT Connected, otherwise it will crash the 8266
  if (mqtt != nullptr && millis() >= 40000){
    char subuf[38];
    strcpy(subuf, mqttDeviceTopic);
    strcat(subuf, MQTT_TOPIC_PIR);
    mqtt->publish(subuf, 0, true, state.c_str());
  }
}

void publishMqttHUM(float state)
{
  //Check if MQTT Connected, otherwise it will crash the 8266
  if (mqtt != nullptr){
    char subuf[38];
    strcpy(subuf, mqttDeviceTopic);
    strcat(subuf, MQTT_TOPIC_HUM);
    mqtt->publish(subuf, 0, true, String(state,2).c_str());
  }
}

void publishMqttTEMP(float state)
{
  //Check if MQTT Connected, otherwise it will crash the 8266
  if (mqtt != nullptr){
    char subuf[38];
    strcpy(subuf, mqttDeviceTopic);
    strcat(subuf, MQTT_TOPIC_TEMP);
    mqtt->publish(subuf, 0, true, String(state,2).c_str());
  }
}

//loop. You can use "if (WLED_CONNECTED)" to check for successful connection
void userLoop()
{
  if (digitalRead(MOTION_PIN) == HIGH && prevState == LOW) { // Motion detected
    publishMqttPIR("ON");
    prevState = HIGH;
  } 
  if (digitalRead(MOTION_PIN) == LOW && prevState == HIGH) {  // Motion stopped
    publishMqttPIR("OFF");
    prevState = LOW;
  }

  if (millis() - readTime > 500)
  {
    readTime = millis();
    timeDiff = millis() - lastTime;
    
    // Read DHT Sensor Values
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    // Send MQTT message on significant change or after UPDATE_MS
    if (timeDiff > UPDATE_MS) 
    {
      publishMqttHUM(hum);
      publishMqttTEMP(temp);
      lastTime = millis();
    }
  }
}

