///#include <SoftwareSerial.h>
#include <WiFiClient.h>
#include <EspMQTTClient.h>
#include "I2Cdev.h"
#include "MPU6050.h"

char AX[10];
char AY[10];
char AZ[10];
char GX[10];
char GY[10];
char GZ[10];

int16_t ax, ay, az;
int16_t gx, gy, gz;
MPU6050 accelgyro;

#define OUTPUT_READABLE_ACCELGYRO
#define LED_PIN 13
bool blinkState = false;

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// WiFi
const char* ssid = "Winwin";
const char* password = "Second2019";

// MQTT
const char* mqtt_server = "192.168.88.11"; 
const char* mqtt_username = "Winwin"; // MQTT username
const char* mqtt_password = "Windy23062002"; // MQTT password
const char* mqtt_topic = "sensor/mpu6050";
const char* clientID = "Weather_Reporter"; // MQTT client ID
const int mqtt_port = 1883;
unsigned int count = 0;


EspMQTTClient client("Winwin",
                     "Second2019",
                     "192.168.88.11",  // MQTT Broker server ip
                     "Winwin",   // Can be omitted if not needed
                     "Windy23062002",   // Can be omitted if not needed
                     "TestClient",     // Client name that uniquely identify your device
                     1883              // The MQTT port, default to 1883. this line can be omitted);
                    );


void setup(){
  Serial.begin(9600);
   //WiFi.begin(ssid, pass);
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  client.enableDebuggingMessages(); 
  client.enableHTTPWebUpdater(); 
  client.enableOTA(); 
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline"); 
  
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  // configure Arduino LED pin for output
  pinMode(LED_PIN, OUTPUT);
  
  // blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);
  
  connect();
}
void onConnectionEstablished()
{
  // Publish nilai IMU
  client.publish("MPU6050/ax", AX); // Kirim nilai ax sebagai string di topik MPU6050/ax
  client.publish("MPU6050/ay", AY); // Kirim nilai ay sebagai string di topik MPU6050/ay
  client.publish("MPU6050/ax", AZ); // Kirim nilai az sebagai string di topik MPU6050/az
  client.publish("MPU6050/gx", GX); // Kirim nilai gx sebagai string di topik MPU6050/gx
  client.publish("MPU6050/gy", GY); // Kirim nilai gy sebagai string di topik MPU6050/gy
  client.publish("MPU6050/gx", GZ); // Kirim nilai gz sebagai string di topik MPU6050/gz
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(1000);
  }
  
  Serial.print("\nconnecting...");
  while (!client.isConnected()) {
   Serial.print(".");
   delay(1000);
  }

  Serial.println("\nconnected!");
}

void loop() {
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //dapetin nilai gyro
  client.loop();                                      //communicate to MQTT broker
  
  sprintf(AX, "%d", ax);
  sprintf(AY, "%d", ay);
  sprintf(AZ, "%d", az);
  sprintf(GX, "%d", gx);
  sprintf(GY, "%d", gy);
  sprintf(GZ, "%d", gz);
  //Serial monitor nilai gyro
  
  #ifdef OUTPUT_READABLE_ACCELGYRO
  Serial.print("a/g:\t");
  Serial.print(ax);
  Serial.print("\t");
  Serial.print(ay);
  Serial.print("\t");
  Serial.print(az);
  Serial.print("\t");
  Serial.print(gx);
  Serial.print("\t");
  Serial.print(gy);
  Serial.print("\t");
  Serial.println(gz);
  #endif
  
  #ifdef OUTPUT_BINARY_ACCELGYRO
  Serial.write((uint8_t)(ax >> 8));
  Serial.write((uint8_t)(ax & 0xFF));
  Serial.write((uint8_t)(ay >> 8));
  Serial.write((uint8_t)(ay & 0xFF));
  Serial.write((uint8_t)(az >> 8));
  Serial.write((uint8_t)(az & 0xFF));
  Serial.write((uint8_t)(gx >> 8));
  Serial.write((uint8_t)(gx & 0xFF));
  Serial.write((uint8_t)(gy >> 8));
  Serial.write((uint8_t)(gy & 0xFF));
  Serial.write((uint8_t)(gz >> 8));
  Serial.write((uint8_t)(gz & 0xFF));
  #endif
  
  // blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);
}
