#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "cred.h"

// WiFi credentials
const char WIFI_SSID[] = "Home";
const char WIFI_PASSWORD[] = "Bhagode Bable";

// Device name from AWS
const char THINGNAME[] = "EEG_LODA";
const char MQTT_HOST[] = "a14qnyirupq00n-ats.iot.ap-south-1.amazonaws.com";

// MQTT topics
const char AWS_IOT_PUBLISH_TOPIC[] = "eeg/input";
const char AWS_IOT_SUBSCRIBE_TOPIC[] = "eeg/sub";

// Publishing interval
const long interval = 5000;

// Timezone offset from UTC
const int8_t TIME_ZONE = -5;

// Last time message was sent
unsigned long lastMillis = 0;

// WiFiClientSecure object for secure communication
WiFiClientSecure net;
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
PubSubClient client(net);

// Function to connect to NTP server and set time
void NTPConnect() {
  // Set time using SNTP
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 1510592825) { // January 13, 2018
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
}

// Callback function for message reception
void messageReceived(char *topic, byte *payload, unsigned int length) {
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
void connectAWS() {
  delay(3000);EEG_LODA
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
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

// Function to publish message to AWS IoT Core
void publishMessage() {
  // Generate random humidity and temperature values
  float h = random(0, 100); 
  float t = random(0, 50); 

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.println("°C");

  // Create JSON document for message
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["humidity"] = h;
  doc["temperature"] = t;

  // Serialize JSON document to string
  char jsonBuffer[200];
  serializeJson(doc, jsonBuffer);

  // Publish message to MQTT topic
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Connect to AWS IoT Core
  connectAWS();
}

void loop() {
  // Check if it's time to publish a message
  if (millis() - lastMillis > interval) {
    lastMillis = millis();
    if (client.connected()) {
      // Publish message
      publishMessage();
    }
  }
  // Maintain MQTT connection
  client.loop();
}