#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MFRC522.h>
#include <time.h>

#define SS_PIN 5
#define RST_PIN 4
MFRC522 rfid(SS_PIN, RST_PIN);

#define GREEN_LED 2
#define RED_LED 15
#define BUZZER 13

const char* ssid = "hg";
const char* password = "omaromar";

String serverName = "https://script.google.com/macros/s/AKfycbyI11qb9ivc8jDbTaNK5H5qy0BCN-9CWzOcBTUOYN3Px4gwMttp8eJ6dFhfQjvKlHJ5PA/exec";

String cards[] = {
  "24 AF 1B 02",
  "BE 34 92 F0",
  "A0 0A 4A 09",
  "CE 7B 7A F0"
};

String names[] = {
  "Ahmed Ali",
  "Elsayed Fayez",
  "Ahmed Amgad",
  "Omer Shoman"
};

String ids[] = {
  "2026001",
  "2026002",
  "2501599",
  "2501350"
};

unsigned long lastScan = 0;

String getUID() {
  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }

  uid.toUpperCase();
  return uid;
}

String getTime() {

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "NO_TIME";
  }

  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);

  return String(buffer);
}

void sendToGoogle(String name, String id, String uid) {

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  uid.replace(" ", "%20");
  name.replace(" ", "%20");

  String timeValue = getTime();
  timeValue.replace(" ", "%20");

  String url = serverName +
               "?name=" + name +
               "&id=" + id +
               "&uid=" + uid +
               "&time=" + timeValue;

  Serial.println(url);

  http.begin(client, url);
  int code = http.GET();

  Serial.println("HTTP Code: " + String(code));

  http.end();
}

void granted(int index, String uid) {

  digitalWrite(GREEN_LED, HIGH);
  tone(BUZZER, 1000, 150);

  sendToGoogle(names[index], ids[index], uid);

  delay(150);
  digitalWrite(GREEN_LED, LOW);
}

void denied(String uid) {

  digitalWrite(RED_LED, HIGH);
  tone(BUZZER, 200, 300);

  sendToGoogle("Unknown", "0000", uid);

  delay(150);
  digitalWrite(RED_LED, LOW);
}

void setup() {

  Serial.begin(115200);

  SPI.begin();
  rfid.PCD_Init();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.println("Time Syncing...");
}

void loop() {

  if (millis() - lastScan < 800) return;

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  lastScan = millis();

  String uid = getUID();
  Serial.println("UID: " + uid);

  bool found = false;
  int index = -1;

  for (int i = 0; i < 4; i++) {
    if (uid == cards[i]) {
      found = true;
      index = i;
      break;
    }
  }

  if (found) {
    granted(index, uid);
  } else {
    denied(uid);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}