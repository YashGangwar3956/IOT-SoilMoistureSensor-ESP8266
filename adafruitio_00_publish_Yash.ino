// Include necessary libraries
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <LiquidCrystal.h> // Include the LiquidCrystal library instead of LiquidCrystal_I2C
#include "config.h" // Include your WiFi and MQTT credentials

// Define pins
#define SOIL_MOISTURE_PIN A0
#define RS_PIN D3
#define EN_PIN D4
#define D4_PIN D5
#define D5_PIN D6
#define D6_PIN D7
#define D7_PIN D8

// Set up MQTT client
WiFiClient espClient;
Adafruit_MQTT_Client mqtt(&espClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish soilMoisture = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil-moisture");

// Set up LCD
LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear(); // Clear the LCD screen
  lcd.print("Soil Moisture:"); // Print initial message on LCD

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Ensure MQTT connection
  MQTT_connect();

  // Read soil moisture sensor value
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

  // Publish soil moisture value to MQTT
  if (!soilMoisture.publish(soilMoistureValue)) {
    Serial.println(F("Failed to publish soil moisture data"));
  } else {
    Serial.println(F("Soil moisture data published"));
  }

  // Display soil moisture value on LCD
  lcd.setCursor(0, 1); // Set cursor to first column of second row
  lcd.print("Value: ");
  lcd.print(soilMoistureValue);

  Serial.println(1024 - soilMoistureValue);

  delay(2000); // Wait for 2 seconds before publishing again
}

// Function to connect or reconnect to MQTT broker
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // Connect to MQTT broker
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(2000); // Wait 5 seconds before retrying
  }

  Serial.println("MQTT connected!");
}