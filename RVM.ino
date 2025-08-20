# include <Ethernet.h>
# include <LiquidCrystal_I2C.h>
# include <Wire.h>
# include <PN532_I2C.h>
# include <PN532.h>
# include <SPI.h>
# include <Servo.h>
# include <HX711_ADC.h>
# include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

EthernetClient client;

Servo servo1;
Servo servo2;

const int HX711_dout = 2;
const int HX711_sck = 3;

HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

byte mac[] = { 0xEC, 0xED, 0xBE, 0xEF, 0xFE, 0xDE };

int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET";
char   HOST_NAME[] = "172.10.20.228"; // change to your PC's IP address
String PATH_NAME   = "/insert_points.php";

String uidStr;
String nama;
int points;
boolean success;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Ethernet Setup
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to obtain an IP address using DHCP");
  }

  // LCD Setup
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Please insert trash below");


  // NFC Setup
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  nfc.SAMConfig();


  // servo1 setup
  servo1.attach(11);
  servo2.attach(10);
  servo1.write(0);
  servo2.write(0);


  // Load Cell Setup
  LoadCell.begin();
  float calibrationValue;
  EEPROM.get(calVal_eepromAdress, calibrationValue);

  unsigned long stabilizingtime = 5000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  LoadCell.setCalFactor(calibrationValue);

  Serial.println("Init success");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      int weight = LoadCell.getData();
      newDataReady = 0;
      t = millis();
      Serial.println(String(weight));

      if ( weight > 24) {
        servo1.write(180);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Proccessing trash...");
        lcd.setCursor(0, 1);
        lcd.print("Please wait...");
        delay(200);

        servo2.write(45);
        delay(500);
        servo1.write(0);
        servo2.write(0);

        while ( weight >= 25 ) {
          points += 10;
          weight -= 25;
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Please tap your card...");
        success = false;

        while (!success) {
          writeNFC();
        }

        sentData();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Please insert trash below");
      }
    }
  }
  
  delay (5000);
}

void writeNFC() {
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  String idStr;
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    for (uint8_t i=0; i < uidLength; i++) {
      idStr = String(uid[i], HEX);
      uidStr = uidStr + "0x" + idStr + " ";
    }

    uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);   
    if (success) {
      uint8_t data[16];
        
      success = nfc.mifareclassic_ReadDataBlock(4, data);
      String namaStr;

      for (uint8_t i=0; i < 16; i++) {
        if (data[i] != 0) {
          namaStr = char(data[i]);
        } else {
          namaStr = "";
        }
        nama = nama + namaStr;
      }


      success = nfc.mifareclassic_ReadDataBlock(5, data);

      if (success) {
        nfc.PrintHexChar(data, 16);

        uint8_t newData[16];
        newData[0] = int(data[0]) + points;

        for (int i = 1; i < 16; i++) {
          newData[i] = 0;
        }

        memcpy(data, newData, sizeof data);
        success = nfc.mifareclassic_WriteDataBlock (5, data);
        points = newData[0];
    
        if (success) {
          delay(5000);
        }
      }
    }
  }
}

void sentData() {

  String queryString = "?uid="+'"'+uidStr+'"'+"&nama="+'"'+nama+'"'+"&points="+ String(points);

  // connect to web server on port 80:
  if(client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println(HTTP_METHOD + " " + PATH_NAME + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header

    while(client.connected()) {
      if(client.available()){
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("Disconnected");
  } else {// if not connected:
    Serial.println("Connection failed");
  }

}
