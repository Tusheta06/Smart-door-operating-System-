#define BLYNK_TEMPLATE_ID "YourTemplateID"
#define BLYNK_TEMPLATE_NAME "SmartDoor"
#define BLYNK_AUTH_TOKEN "YourAuthToken"  // Paste from Blynk email

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char ssid[] = "YourWiFiSSID";
char pass[] = "YourWiFiPassword";

// Pin Definitions
#define RELAY_PIN D1
#define MOTION_SENSOR_PIN D2
#define DOOR_SENSOR_PIN D5
#define DHT_PIN D6
#define DHT_TYPE DHT11 // or DHT22

DHT dht(DHT_PIN, DHT_TYPE);
BlynkTimer timer;

void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // Reed switch

  digitalWrite(RELAY_PIN, LOW); // Keep door locked initially

  timer.setInterval(2000L, readSensors); // Every 2 seconds
}

// Virtual pin V1 controls door lock
BLYNK_WRITE(V1) {
  int pinValue = param.asInt(); // 1 = unlock, 0 = lock
  digitalWrite(RELAY_PIN, pinValue);
}

// Read and send sensor data
void readSensors() {
  // Motion Detection
  bool motionDetected = digitalRead(MOTION_SENSOR_PIN);
  Blynk.virtualWrite(V3, motionDetected);
  if (motionDetected) {
    Serial.println("Motion Detected!");
  }

  // Door Status
  bool doorOpen = digitalRead(DOOR_SENSOR_PIN) == LOW; // Assuming reed switch closes circuit
  Blynk.virtualWrite(V2, doorOpen ? "Open" : "Closed");
  Serial.println(doorOpen ? "Door is OPEN" : "Door is CLOSED");

  // DHT Sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    Blynk.virtualWrite(V4, t);
    Blynk.virtualWrite(V5, h);
    Serial.printf("Temp: %.1f°C  Humidity: %.1f%%\n", t, h);
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
