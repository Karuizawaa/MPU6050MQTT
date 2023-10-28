/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#define OUTPUT_READABLE_ACCELGYRO
#define LED_PIN 13
bool blinkState = false;

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif


// Replace the next variables with your SSID/Password combination
const char* ssid = "Winwin";
const char* password = "Second2019";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.88.11";

int16_t ax, ay, az;
int16_t gx, gy, gz;
MPU6050 accelgyro;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  
  setup_wifi();
  
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  client.setServer(mqtt_server, 1883);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //dapetin nilai gyro
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //dapetin nilai gyro
    
    // Publish nilai IMU
    client.publish("MPU/AX", String(ax).c_str(), true);
    client.publish("MPU/AY", String(ay).c_str(), true);
    client.publish("MPU/AZ", String(az).c_str(), true);
    client.publish("MPU/GX", String(gx).c_str(), true);
    client.publish("MPU/GY", String(gy).c_str(), true);
    client.publish("MPU/GZ", String(gz).c_str(), true);

    // Nampilin nilai IMU
    Serial.print("ax : "); Serial.print(ax); Serial.print(" ");
    Serial.print("ay : "); Serial.print(ay); Serial.print(" ");
    Serial.print("az : "); Serial.print(az); Serial.print(" ");
    Serial.print("gx : "); Serial.print(gx); Serial.print(" ");
    Serial.print("gy : "); Serial.print(gy); Serial.print(" ");
    Serial.print("gz : "); Serial.println(gz);
  }
}
