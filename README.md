# 🎓 Smart Attendance System
### RFID-Based Automated Attendance with Real-Time Google Sheets Integration
 
<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif" />
  <img src="https://img.shields.io/badge/Protocol-RFID%20RC522-green?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Cloud-Google%20Sheets-brightgreen?style=for-the-badge&logo=google-sheets" />
  <img src="https://img.shields.io/badge/IDE-Arduino-teal?style=for-the-badge&logo=arduino" />
  <img src="https://img.shields.io/badge/Version-1.0-orange?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Status-Active-success?style=for-the-badge" />
</p>
---
 
## 📌 Overview
 
The **Smart Attendance System** is an IoT-based solution designed to automate attendance recording using RFID technology.
 
Instead of traditional manual registration, each student uses a personal **RFID Card or Tag** to check in. The system identifies the student, logs their **Name, ID, and UID**, records a precise **timestamp**, and sends all data instantly to a **Google Sheet** via WiFi — all in under **1 second**.
 
> Built as a graduation project at **Al Ryada University for Science and Technology**
> Faculty of Computer Science — Academic Year **2025–2026**
 
---
 
## 🖼️ Project Photos
 
| Hardware Build | Circuit Simulation | Google Sheets Live Data |
|:-:|:-:|:-:|
| ![Hardware](assets/hardware.jpg) | ![Simulation](assets/simulation.png) | ![Sheets](assets/sheets.png) |
 
---
 
## ✨ Features
 
- ⚡ **Instant scan** — attendance logged in under 1 second
- 🔒 **Tamper-proof** — each card has a unique UID, cannot be forged
- ☁️ **Real-time cloud sync** — data pushed to Google Sheets via HTTP over WiFi
- 🟢 **Visual feedback** — Green LED (granted) / Red LED (denied)
- 🔔 **Audio feedback** — Buzzer beep on every scan
- 🕐 **Accurate timestamps** — synced via NTP (Egypt GMT+2)
- 📊 **Easy tracking** — professor monitors attendance from any device, anywhere
---
 
## 🔧 Hardware Components
 
| # | Component | Description | Price |
|---|-----------|-------------|-------|
| 1 | **ESP32 NodeMCU-32S** | Main controller — WiFi + Bluetooth | 340 EGP |
| 2 | **RFID MFRC522 + Card & Tag** | 13.56 MHz HF reader | 110 EGP |
| 3 | **OLED 1.3" I2C** | Display *(planned — Version 2)* | 325 EGP |
| 4 | **Breadboard 830pt (OSEPP)** | Prototyping platform | 125 EGP |
| 5 | **Active Buzzer 3V** | Audio feedback | 20 EGP |
| 6 | **LED Green & Red** | Visual feedback indicators | 10 EGP |
| 7 | **Jumper Wires** | Connections | 10 EGP |
| | **Total** | | **940 EGP** |
 
---
 
## 🔌 Wiring Diagram
 
### RFID RC522 → ESP32
| RC522 Pin | ESP32 Pin |
|-----------|-----------|
| SDA (SS) | D5 |
| SCK | D18 |
| MOSI | D23 |
| MISO | D19 |
| RST | D4 |
| IRQ | D27 |
| GND | GND |
| 3.3V | 3.3V |
 
### Other Components
| Component | ESP32 Pin |
|-----------|-----------|
| Green LED | D2 |
| Red LED | D15 |
| Buzzer (+) | D13 |
| Buzzer (−) | GND |
| OLED SDA | D21 |
| OLED SCL | D22 |
 
---
 
## ⚙️ How It Works
 
```
Student taps RFID Card
        │
        ▼
ESP32 reads UID via RC522
        │
        ▼
UID matched in cards[] database?
   ┌────┴────┐
  YES       NO
   │         │
   ▼         ▼
Green LED  Red LED
+ Beep     + Buzz
   │         │
   └────┬────┘
        │
        ▼
sendToGoogle()
→ HTTP GET to Google Apps Script
→ Name, ID, UID, Timestamp appended to Sheet
        │
        ▼
Professor sees attendance in real-time ✅
```
 
---
 
## 📋 Google Sheets Output
 
| Name | ID | UID | Time |
|------|----|-----|------|
| Ahmed Ali | 2026001 | 24 AF 1B 02 | 2026-04-20 20:52:11 |
| Elsayed Fayez | 2026002 | BE 34 92 F0 | 2026-04-20 20:53:05 |
 
---
 
## 💻 Source Code
 
### Key Libraries
```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MFRC522.h>
#include <time.h>
```
 
### Core Functions
 
**`getUID()`** — Reads card UID as uppercase hex string
```cpp
String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();
  return uid; // e.g. "24 AF 1B 02"
}
```
 
**`sendToGoogle()`** — Sends data via HTTP GET to Google Apps Script
```cpp
void sendToGoogle(String name, String id, String uid) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  String url = serverName +
               "?name=" + name +
               "&id="   + id   +
               "&uid="  + uid  +
               "&time=" + getTime();
  http.begin(client, url);
  http.GET();
  http.end();
}
```
 
**`loop()`** — Main scan cycle (800ms debounce)
```cpp
void loop() {
  if (millis() - lastScan < 800) return;
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;
  lastScan = millis();
  String uid = getUID();
  bool found = false;
  for (int i = 0; i < 2; i++) {
    if (uid == cards[i]) {
      granted(i, uid);
      found = true;
      break;
    }
  }
  if (!found) denied(uid);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
```
 
---
 
## 🚀 Installation & Setup
 
### 1. Clone the Repository
```bash
git clone https://github.com/YOUR_USERNAME/smart-attendance-system.git
cd smart-attendance-system
```
 
### 2. Install Arduino Libraries
Open **Arduino IDE** → Library Manager → Install:
- `MFRC522` by GithubCommunity
- `ESP32` board package (Espressif)
### 3. Configure Your Settings
Open `main.ino` and update:
```cpp
// WiFi credentials
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
 
// Google Apps Script URL
String serverName = "https://script.google.com/macros/s/YOUR_SCRIPT_ID/exec";
 
// Add your RFID cards
String cards[] = { "XX XX XX XX", "YY YY YY YY" };
String names[] = { "Student Name 1", "Student Name 2" };
String ids[]   = { "ID001", "ID002" };
```
 
### 4. Upload to ESP32
- Select board: **ESP32 Dev Module**
- Select correct **COM port**
- Click **Upload** ✅
### 5. Setup Google Apps Script
1. Open [Google Sheets](https://sheets.google.com) → create a new sheet
2. Go to **Extensions → Apps Script**
3. Paste the Apps Script code from `/google-script/Code.gs`
4. Deploy as **Web App** → Anyone can access
5. Copy the deployment URL → paste into `serverName` in the Arduino code
---
 
## 📡 RFID Range — Current vs Future
 
| Technology | Current Range | After Upgrade |
|------------|--------------|---------------|
| **HF — RC522** *(current)* | 1 – 10 cm | up to 30 cm |
| **UHF Reader + High-Gain Antenna** | 3 – 6 m | 12 – 15 m |
| **Active RFID Tags** | 30 – 100 m | 500+ m |
 
---
 
## 🗺️ Roadmap
 
### ✅ Version 1 — Completed
- [x] RFID card scanning with RC522
- [x] Student identification by UID
- [x] Real-time Google Sheets logging via WiFi
- [x] Visual feedback (Green/Red LED)
- [x] Audio feedback (Buzzer)
- [x] NTP timestamp sync (Egypt GMT+2)
### 🔄 Version 2 — In Progress
- [ ] **OLED Display** — show student name and status on screen
- [ ] **PCB Design** — replace breadboard with compact Printed Circuit Board
- [ ] **Rechargeable Battery** — independent power source
- [ ] **WiFi Configuration Portal** — Captive Portal, no code editing needed
- [ ] **UHF RFID Upgrade** — extend range to 10–15 meters
- [ ] **Active RFID Tags** — for 100+ meter range
---
 
## 👥 Team
 
| Name | Role |
|------|------|
| **Omar Shouman** | Project Lead & Developer |
| **Elsayed Fayez** | Hardware & Circuit Design |
| **Ahmed Amjad** | Software & Integration |
| **Ali** | Testing & Documentation |
| **Walid** | Research & Development |
 
---
 
## 🏛️ Academic Acknowledgements
 
This project was developed at:
 
**Al Ryada University for Science and Technology (RST)**
Faculty of Computer Science — IoT & Embedded Systems
 
| Role | Name |
|------|------|
| 🎓 Dean of Faculty | **Prof. Yahia El-Helwagi** |
| 👨‍🏫 Supervisor | **Dr. Anas Saber** |
| 👩‍🏫 Teaching Assistant | **Salma** |
| 👩‍🏫 Teaching Assistant | **Yasmine** |
 
---
 
## 📄 License
 
This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.
 
---
 
<p align="center">
  Made with ❤️ by the Smart Attendance Team — RST University 2025–2026
</p>
 
