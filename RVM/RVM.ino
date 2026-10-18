# include <Arduino.h>
# include <Ethernet.h>
# include <LiquidCrystal_I2C.h>
# include <Wire.h>
# include <PN532_I2C.h>
# include <NfcAdapter.h>
# include <Servo.h>
# include <HX711_ADC.h>
# include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

EthernetClient client;

Servo servo1;
Servo servo2;

const int HX711_dout = 2;
const int HX711_sck = 3;

HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

byte mac[] = { 0xEC, 0xED, 0xBE, 0xEF, 0xFE, 0xDE };

int    HTTP_PORT   = 80; // change to your server's port number
String PATH_NAME   = "/api/insert_points.php"; // change to your server's path to
char   HOST_NAME[] = "xxx.xxx.x.x"; // change to your server's IP address
const char* API_KEY = "YOUR_API_KEY"; // change to your API key

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // LCD Setup
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Please insert");
  lcd.setCursor(0, 1);
  lcd.print("trash below");


  // NFC Setup
  nfc.begin();


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
}

void loop() {
  // put your main code here, to run repeatedly:
  static bool newDataReady = 0;

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    int weight = LoadCell.getData();
    newDataReady = 0;

    if (weight > 24) {
      int points = 0;
      
      servo1.write(180);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Proccessing trash...");
      lcd.setCursor(0, 1);
      lcd.print("Please wait...");

      for ( int i = 0; i < 50; i++) {
        weight = LoadCell.getData();
        delay(15);
      }

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

      bool success = false;
      String uid;
      String name;
      while (!success) {
        readNFC(uid, name, success);
      }

      sendData(uid, name, points);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Please insert trash below");
    }
  }
  
  delay (1000);
}

void readNFC(String &uid, String &name, bool &success) {  
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    uid = tag.getUidString();
    if (tag.hasNdefMessage()) {
      NdefMessage message = tag.getNdefMessage();
      NdefRecord record = message.getRecord(0);
      int payloadLength = record.getPayloadLength();
      byte payload [payloadLength];
      record.getPayload(payload);
      for (int c = 3; c < payloadLength; c++) {
        name += (char)payload[c];
      }
    }
    tag.print();
    success = true;
    return;
  }
  delay(1000);
}

void sendData(const String &uid, const String &nama, int &points) {
  // 1. Format the POST body payload
  String body = "uid=" + uid + "&nama=" + nama + "&points=" + String(points);

  // 2. Connect to web server
  if (client.connect(HOST_NAME, HTTP_PORT)) {
    // 3. Send HTTP Request Line & Headers
    client.println("POST " + PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println(F("Content-Type: application/x-www-form-urlencoded"));
    client.println(F("X-API-Key: YOUR_API_KEY")); // Must match API_SECRET_KEY in PHP
    client.println("Content-Length: " + String(body.length()));
    client.println(F("Connection: close"));
    client.println(); // Mandatory empty line separating headers from body

    // 4. Send POST Payload
    client.print(body);

    // 5. Read Response from PHP Server (Serial Monitor Debugging)
    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
      }
    }

    // 6. Close Connection
    client.stop();
  }
}
