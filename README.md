# **RVM**
An Arduino Project that utilizes the use of Ethernet Shield, Load Cell, NFC Module, Servo, LCD and mySQL database to rewards user with digital points that can be used as a currency.

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
  - [LiquidCrystal_I2C](https://github.com/NoXystics/Arduino-RVM/edit/main/README.md)

## How it works
RVM works by weighing the object put by user and calculate amount of points will be rewarded using that data. After getting the amount of point that will be rewarded RVM will wait until user tap an NFC Card and write the amount of points the user have then sent the newest data to the mySQL database through HTTP Request using the Ethernet Shield. 

## Setup
  ### Step 1: Install XAMPP
  Install XAMPP and open XAMPP control panel to enable mySQL and web server
  <img width="832" height="540" alt="image" src="https://github.com/user-attachments/assets/ca79b689-6a3b-40cb-bbc8-a2688efdc8c6" />

  ### Step 2: Create Database
  Create a database named db_arduino for storing the points data for each user then create a table called rvm_table and then create a mySQL account      that can be accessed from localhost only so the table can be accessed by localhost only. 

  ### Step 3: Place PHP File in HTDOCS Folder
  Download or copy and place the injector.php file inside C:\XAMPP\htdocs\
  
  ```
  <?php
  
  if(isset($_GET["uid"])) {
     $temperature = $_GET["uid"]; // get uid value from HTTP GET
  
     $servername = "localhost";
     $username = "Arduino";                       // Change this to match your mySQL account
     $password = "ArduinoGetStarted.com";         // This too
     $dbname = "db_arduino";
  
     // Create connection
     $conn = new mysqli($servername, $username, $password, $dbname);
     // Check connection
     if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
     }
  
     $sql = "INSERT INTO rvm_table (uid_value, name, points) VALUES ($uid, name, points)";
  
     if ($conn->query($sql) === TRUE) {
        echo "New record created successfully";
     } else {
        echo "Error: " . $sql . " => " . $conn->error;
     }
  
     $conn->close();
  } else {
     echo "Points is not set";
  }
  
  ?>
  ```


  > [!IMPORTANT]
  > It's very recommended to create a mySQL user account that can connect to your mySQL database from localhost only for security reasons

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
  
  
  ### Step 5: Upload The Code
  Install required libraries and copy the code to Arduino IDE and upload it to your Arduino UNO
  <img width="1366" height="768" alt="image" src="https://github.com/user-attachments/assets/57b6d31c-b713-46e8-b04c-c0bfe888e7e3" />


  ## License 

  ```
  MIT License

  Copyright (c) 2025 SanC
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  ```
  
