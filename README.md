# **RVM**
An Arduino Project that utilizes the use of Ethernet Shield, Load Cell, NFC Module, Servos, LCD and mySQL database to rewards user with digital points that can be used as a currency.

## Material
  - Arduino UNO
  - Ethernet Shield
  - Liquid Crystal LCD
  - PN532 NFC Module
  - Load Cell & HX711 Amplifier
  - Breadboard
  - Machine Main Structure/Body

## Libraries
  - [Elechouse PN532](https://github.com/elechouse/PN532)
  - [HX711_ADC](https://github.com/olkal/HX711_ADC)
  - [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C/tree/master)

## How it works
RVM works by weighing the object put by user and calculate amount of points will be rewarded using that data. After getting the amount of point that will be rewarded RVM will wait until user tap an NFC Card and write the amount of points the user have then sent the newest data to the mySQL database through HTTP POST using the Ethernet Shield. 

## Setup
  ### Step 1: Install XAMPP/MySQL
  Install XAMPP and open XAMPP control panel to enable mySQL and web server or just use standalone MySQL install. Your choice!
  <img width="666" height="402" alt="XAMPP Control Panel v3 3 0     Compiled_ Apr 6th 2021   8_20_2025 6_53_11 PM" src="https://github.com/user-attachments/assets/aa389da0-c4d9-452e-b9c9-7b577c30e2d4" />


  ### Step 2: Create Database
  Create a database named db_arduino for storing the points data for each user then create a table called db_arduino and then create a mySQL account that can be accessed from localhost only so the table can be accessed by localhost only. 
  <img width="1904" height="920" alt="image" src="https://github.com/user-attachments/assets/25ecd8c2-7da1-4e56-8fc1-1c416972fed2" />


  ### Step 3: Place PHP File in HTDOCS Folder
  Download or simply copy and place the insert_points.php file inside C:\XAMPP\htdocs\api\
  
  ```
  <?php
   // Set JSON response header
   header('Content-Type: application/json');

   // Secret API Key (must match API_SECRET in your Arduino code)
   define('API_SECRET_KEY', 'YOUR_API_KEY');

   // 1. Enforce HTTP POST Method
   if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
      http_response_code(405); // 405 Method Not Allowed
      echo json_encode(["status" => "error", "message" => "Only POST requests are allowed"]);
      exit;
   }

   // 2. Validate API Key Header
   $headers = getallheaders();
   $apiKey = isset($headers['X-API-Key']) ? $headers['X-API-Key'] : (isset($_SERVER['HTTP_X_API_KEY']) ? $_SERVER['HTTP_X_API_KEY'] : '');

   if ($apiKey !== API_SECRET_KEY) {
      http_response_code(401); // 401 Unauthorized
      echo json_encode(["status" => "error", "message" => "Unauthorized: Invalid API Key"]);
      exit;
   }

   // 3. Extract and Sanitize POST Data
   $uid    = isset($_POST['uid']) ? trim($_POST['uid']) : '';
   $name   = isset($_POST['name']) ? trim($_POST['name']) : '';
   $points = isset($_POST['points']) ? filter_var($_POST['points'], FILTER_VALIDATE_INT) : false;

   if (empty($uid) || $points === false) {
      http_response_code(400); // 400 Bad Request
      echo json_encode(["status" => "error", "message" => "Missing or invalid parameters"]);
      exit;
   }

   // 4. Database Credentials
   $servername = "127.0.0.1"; // Use '127.0.0.1' or your MySQL Docker container name
   $username   = "Arduino";
   $password   = "ArduinoPass";
   $dbname     = "db_arduino";

   // 5. Connect to MySQL Database
   $conn = new mysqli($servername, $username, $password, $dbname);

   if ($conn->connect_error) {
      http_response_code(500); // 500 Internal Server Error
      echo json_encode(["status" => "error", "message" => "Database connection failed"]);
      exit;
   }

   // 6. Use Prepared Statements (Prevents SQL Injection & Fixes Variable Typo)
   $stmt = $conn->prepare("INSERT INTO db_arduino (uid, name, points) VALUES (?, ?, ?)");
   $stmt->bind_param("ssi", $uid, $name, $points); // 's' = string, 'i' = integer

   if ($stmt->execute()) {
      http_response_code(200);
      echo json_encode(["status" => "success", "message" => "Record created successfully"]);
   } else {
      http_response_code(500);
      echo json_encode(["status" => "error", "message" => "Execute error: " . $stmt->error]);
   }

   // Clean up
   $stmt->close();
   $conn->close();
   ?>

  ```

  ### Step 4: Configure your endpoint
   ##### API KEY
   Change 'YOUR_API_KEY' inside insert_points.php and RVM.ino to your own API KEY
   > [!TIP]
   > You can simply run the command "openssl rand -hex 32" for this
   #### Hostname
   Change "xxx.xxx.x.x" after char HOST_NAME[]= to your machine that is running MySQL

  ### Step 4: Connect Pins
  Connect the pins: 
  **LCD**
  - GND --> GND
  - VCC --> 5V
  - SDA --> A4
  - SCL --> A5

  **PN532**
  - GND --> GND
  - VCC --> 5V
  - SDA --> SDA
  - SCL --> SCL

  **HX711**
  - GND --> GND
  - DT  --> 2
  - SCK --> 3
  - VCC --> 5V

  **Servo**
  - GND --> GND
  - VCC --> 5V
  - SIGNAL PIN --> 11

  **Servo**
  - GND --> GND
  - VCC --> 5V
  - SIGNAL PIN --> 10
    
  > [!TIP]
  > You can configure your own pins for DT, SCK and Servo signal pin by configuring the code

  ### Step 5: Calibrate Load Cell
  - Open the example code from HX711_ADC library

    <img width="1919" height="1032" alt="image" src="https://github.com/user-attachments/assets/55ab7276-f3f7-4159-bf69-3648bb6c88aa" />
  
  - Change the pins to match your pins configuration
  - Upload the code to your Arduino
  - Open serial monitor and follow the given instructions
  - Save the calibration value to EEPROM
  
  ### Step 6: Upload The Code
  Install required libraries and copy the code to Arduino IDE and upload it to your Arduino UNO
  
  <img width="1366" height="768" alt="image" src="https://github.com/user-attachments/assets/57b6d31c-b713-46e8-b04c-c0bfe888e7e3" />
  

  > [!IMPORTANT]
  > If fetching calibration value from EEPROM doesn't work on the main code you can set it yourself by changing line 81 & line 82 of the .INO code
  > ```
  > float calibration value = <int>;
  > // EEPROM.get(calVal_eepromAdress, calibrationValue);
  > ```


  ## License 

  ```
  MIT License

  Copyright (c) 2026 SanC
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  ```
  
