#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// BLYNK info
#define BLYNK_TEMPLATE_ID "TMPL6f5CChR4v"
#define BLYNK_TEMPLATE_NAME "ok"
#define BLYNK_AUTH_TOKEN "OZ1-hxMJ5AZ5hQ9MMj9DGBlZ3Z7bSfBU"

// Chân kết nối
#define SS_PIN D8
#define RST_PIN D0
#define RELAY_PIN D3
#define LED_PIN D1
#define FAN_PIN D2

// WiFi
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "a120.2";
char pass[] = "12345678";

// Blynk widget
WidgetLED led_connect(V0);

// RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

// UID của thẻ hợp lệ
const byte authorizedCard[4] = {0x23, 0x2C, 0xB7, 0xA7};

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();

  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  if (!mfrc522.PICC_ReadCardSerial())
    return;

  Serial.print("UID tag :");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  if (isAuthorized(mfrc522.uid.uidByte)) {
    Serial.println("Access granted");
    digitalWrite(RELAY_PIN, LOW);
    delay(5000);
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    Serial.println("Access denied");
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

bool isAuthorized(byte *uid) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != authorizedCard[i])
      return false;
  }
  return true;
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}

BLYNK_WRITE(V1) {
  int value = param.asInt();
  digitalWrite(LED_PIN, value == 1 ? HIGH : LOW);
}

BLYNK_WRITE(V2) {
  int value = param.asInt();
  digitalWrite(FAN_PIN, value == 1 ? HIGH : LOW);
}

BLYNK_WRITE(V3) {
  int value = param.asInt();
  digitalWrite(RELAY_PIN, value == 1 ? LOW : HIGH);
  delay(100);
}
