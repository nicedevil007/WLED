#include "wled.h"

#include "DHT.h"

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
  if (mqtt != nullptr && millis() >= 60000){
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
    publishMqttPIR("true");
    prevState = HIGH;
  } 
  if (digitalRead(MOTION_PIN) == LOW && prevState == HIGH) {  // Motion stopped
    publishMqttPIR("false");
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

