#include <virtuabotixRTC.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN 9
#define SS_PIN  10

virtuabotixRTC myRTC(6, 7, 8);
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte knownTags[2][4] = {
  {0x12, 0xAC, 0xFE, 0x1E},
  {0x40, 0xF3, 0xA8, 0x89}
};

String tagNames[2] = { // 
  "Person 1",
  "Person 2"
};

bool loggedIn[2] = {false, false}; // Initialize login status for each tag

void setup() {
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Put your card");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Scanned UID");

      String uidString = "";
      for (uint8_t i = 0; i < 4; i++) {
        uidString += String(mfrc522.uid.uidByte[i], HEX) + " ";
      }

      bool tagFound = false;
      for (int i = 0; i < 2; i++) {
        bool match = true;
        for (int j = 0; j < 4; j++) {
          if (mfrc522.uid.uidByte[j] != knownTags[i][j]) {
            match = false;
            break;
          }
        }

        if (match) {
          lcd.clear();
          lcd.setCursor(0, 0);
          if (loggedIn[i]) {
              lcd.print("Goodbye, " + tagNames[i]);
              lcd.setCursor(0, 1);
              lcd.print("You logged out!");
              printDateTime(tagNames[i], loggedIn[i]); // Pass loggedIn status
          } else {
              lcd.print("Hi, " + tagNames[i]);
              lcd.setCursor(0, 1);
              lcd.print("You logged in!");
              printDateTime(tagNames[i], loggedIn[i]); // Pass loggedIn status
          }

          loggedIn[i] = !loggedIn[i]; // Toggle login/logout state
          tagFound = true;
          break;
        }
      }

      if (!tagFound) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Unknown tag");
      }

      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      delay(1000);
    }
  }
}

void printDateTime(String name, bool loggedIn) {
  myRTC.updateTime();

  Serial.print("Name: "); //space
  Serial.print(name);
  Serial.print(" "); //space
  if (loggedIn) {
    Serial.print("OUT");
  } else {
    Serial.print("IN");
  }
  Serial.print(" "); //space
  Serial.print("Date: "); //space
  Serial.print(myRTC.dayofmonth);
  Serial.print("/");
  Serial.print(myRTC.month);  
  Serial.print("/");
  Serial.print(myRTC.year);
  Serial.print("  "); //space
  Serial.print("Time: "); //space
  Serial.print(myRTC.hours);
  Serial.print(":");
  Serial.print(myRTC.minutes);
  Serial.print(":");
  Serial.println(myRTC.seconds);
}
