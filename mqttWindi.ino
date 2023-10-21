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
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "192.168.88.11";

char AX[10];
char AY[10];
char AZ[10];
char GX[10];
char GY[10];
char GZ[10];
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
  setup_wifi();
  
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  client.setServer(mqtt_server, 1883);
//  client.setCallback(callback);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
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
    
    // Convert the value to a char array
    sprintf(AX, "%d", ax);
    sprintf(AY, "%d", ay);
    sprintf(AZ, "%d", az);
    sprintf(GX, "%d", gx);
    sprintf(GY, "%d", gy);
    sprintf(GZ, "%d", gz);
    
    // Publish nilai IMU
    client.publish("MPU6050/ax", AX); // Kirim nilai ax sebagai string di topik MPU6050/ax
    client.publish("MPU6050/ay", AY); // Kirim nilai ay sebagai string di topik MPU6050/ay
    client.publish("MPU6050/ax", AZ); // Kirim nilai az sebagai string di topik MPU6050/az
    client.publish("MPU6050/gx", GX); // Kirim nilai gx sebagai string di topik MPU6050/gx
    client.publish("MPU6050/gy", GY); // Kirim nilai gy sebagai string di topik MPU6050/gy
    client.publish("MPU6050/gx", GZ); // Kirim nilai gz sebagai string di topik MPU6050/gz
  }
}
