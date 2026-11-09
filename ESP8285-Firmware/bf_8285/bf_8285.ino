#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h> 
#include <ArduinoJson.h>
#include "cred.h"
float eeg_value;
const char WIFI_SSID[] = "Home";
const char WIFI_PASSWORD[] = "Bhagode Bable";
const char THINGNAME[] = "EEG_LODA";
const char MQTT_HOST[] = "a14qnyirupq00n-ats.iot.ap-south-1.amazonaws.com";
const char AWS_IOT_PUBLISH_TOPIC[] = "eeg/input";
const char AWS_IOT_SUBSCRIBE_TOPIC[] = "eeg/sub";
const int8_t TIME_ZONE = -5;
unsigned long lastMillis = 0;
const long interval = 5000;
WiFiClientSecure espClient;
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
PubSubClient client(espClient);
void NTPConnect() {
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 1510592825) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
}
void connectAWS() {
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  NTPConnect();
  espClient.setTrustAnchors(&cert);
  espClient.setClientRSACert(&client_crt, &key);
  client.setServer(MQTT_HOST, 8883);
  Serial.println("Connecting to AWS IoT");
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(1000);
  }
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
}
void sendEEGData(float eeg_value){
    Serial.print("Publishing EEG data: ");
    Serial.println(eeg_value);
    StaticJsonDocument<200> doc;
    doc["eeg_value"] = eeg_value;
    char jsonBuffer[200];
    serializeJson(doc, jsonBuffer);
    client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer,1);
}
void setup(){
    Serial.begin(115200);
    connectAWS();
}
void loop(){
    if (Serial.available() > 0){
        eeg_value = Serial.read();
   }
   else{
    eeg_value = 69.420;
   }
  if (millis() - lastMillis > interval) {
    lastMillis = millis();
    if (client.connected()) {
      sendEEGData(eeg_value);
    // delay(5000);
    }
  }
  client.loop();
}