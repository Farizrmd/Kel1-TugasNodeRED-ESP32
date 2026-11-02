#include <EspMQTTClient.h>
#include <DHT.h>
#include <math.h>

#if __has_include("secrets.h")
#include "secrets.h"
#else
// Tetap bisa dikompilasi sebelum Wi-Fi lokal dikonfigurasi.
#include "secrets.example.h"
#endif

#define KELOMPOK "klp1"
#define ROOM "ruang1"  // Hanya ini yang berbeda untuk empat board.
#define BROKER "broker.mqttdashboard.com"
#define PREFIX "pens/" KELOMPOK "/" ROOM

const char* T_TEMP = PREFIX "/temperature";
const char* T_HUM = PREFIX "/humidity";
const char* T_L1 = PREFIX "/lamp1";
const char* T_L2 = PREFIX "/lamp2";
const char* T_STATUS = PREFIX "/status";
const int DHT_PIN = 16;
const int L1_PIN = 25;
const int L2_PIN = 26;
const unsigned long INTERVAL_MS = 5000UL;
unsigned long lastPublishTime = 0;

DHT dht(DHT_PIN, DHT11);
EspMQTTClient client(WIFI_SSID, WIFI_PASS, BROKER, "", "",
                     "esp32-" KELOMPOK "-" ROOM, 1883);

void setLamp(int pin, const String& payload) {
  if (payload == "1") {
    digitalWrite(pin, HIGH);
  } else if (payload == "0") {
    digitalWrite(pin, LOW);
  } else {
    Serial.println("Payload lampu diabaikan: harus tepat 1 atau 0.");
    return;
  }
  Serial.printf("GPIO %d = %s\n", pin, payload.c_str());
}

void setup() {
  Serial.begin(115200);
  pinMode(L1_PIN, OUTPUT);
  pinMode(L2_PIN, OUTPUT);
  digitalWrite(L1_PIN, LOW);
  digitalWrite(L2_PIN, LOW);
  dht.begin();
  client.enableDebuggingMessages();
  client.setKeepAlive(15);
  client.enableLastWillMessage(T_STATUS, "offline", true);
  Serial.printf("Kelompok %s, %s, sensor DHT11 GPIO %d\n",
                KELOMPOK, ROOM, DHT_PIN);
}

// Library memanggil callback ini lagi setiap reconnect MQTT.
void onConnectionEstablished() {
  Serial.println("MQTT Connection established.");
  client.subscribe(T_L1, [](const String& payload) {
    setLamp(L1_PIN, payload);
  }, 1);
  client.subscribe(T_L2, [](const String& payload) {
    setLamp(L2_PIN, payload);
  }, 1);
  client.publish(T_STATUS, "online", true);
}

void loop() {
  client.loop();  // Tetap dipanggil sering untuk Wi-Fi, MQTT dan reconnect.
  const unsigned long now = millis();
  // Pengurangan unsigned tetap benar ketika millis() rollover.
  if (now - lastPublishTime < INTERVAL_MS) return;
  lastPublishTime = now;
  if (!client.isConnected()) return;

  const float temperature = dht.readTemperature();
  const float humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT11 gagal dibaca; data tidak dipublish.");
    return;
  }
  const bool tempSent = client.publish(T_TEMP, String(temperature, 1), false);
  const bool humSent = client.publish(T_HUM, String(humidity, 1), false);
  Serial.printf("%s: %.1f C | %.1f %% | publish %s\n", ROOM,
                temperature, humidity, tempSent && humSent ? "OK" : "gagal");
}
