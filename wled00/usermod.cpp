#include "wled.h"

//Use userVar0 and userVar1 (API calls &U0=,&U1=, uint16_t)

// PIR sensor pin
const int MOTION_PIN = 4; // D2
 // PIR MQTT topic
const char MQTT_TOPIC_PIR[] = "/s/pir";

int prevState = LOW;

//gets called once at boot. Do all initialization that doesn't depend on network here
void userSetup()
{
  pinMode(MOTION_PIN, INPUT);
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
}

