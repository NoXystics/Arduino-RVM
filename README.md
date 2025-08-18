# **RVM**
An Arduino Project that utilizes the use of Ethernet Shield, Load Cell, NFC Module, Servo, LCD and mySQL database to rewards user with digital points that can be used as a currency. 

## Material
  - **Arduino UNO**
  - Ethernet Shield
  - Liquid Crystal LCD
  - PN532 NFC Module
  - Load Cell & HX711 Amplifier
  - Main Structure

## How it works
  RVM works by weighing the object put by user and calculate amount of points will be rewarded using that data. After getting the amount of point that will be rewarded RVM will wait until user tap an NFC Card and write the amount of points the user have then sent the newest data to the mySQL database through HTTP Request using the Ethernet Shield. 

## Setup
  ### Step 1: Install XAMPP
  Install and open XAMPP control panel to enable the mySQL and web server
  <img width="832" height="540" alt="image" src="https://github.com/user-attachments/assets/ca79b689-6a3b-40cb-bbc8-a2688efdc8c6" />

  ### Step 2: Place PHP File in HTDOCS Folder
  Download and place the PHP file inside C:\xampp\htdocs\

  ### Step 3: Create Database
  Create the database for storing the points data for each user

  > [!IMPORTANT]
  > It's very recommended to create a mySQL user account that can connect to your mySQL\
  > Database from localhost only!

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
  - VCC --> 3.3V
  - SIGNAL PIN --> 11
  [!TIP]
  You can configure your own pins for DT, SCK and Servo signal pin by configuring the code
  
  
  ### Step 5: Upload The Code
  Install required libraries and copy the code to Arduino IDE and upload it to your Arduino UNO
  <img width="1366" height="768" alt="image" src="https://github.com/user-attachments/assets/a44f6caa-edea-4df0-95a6-552b1ef08156" />

  ## License 

  ```
                      GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

  Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
  Everyone is permitted to copy and distribute verbatim copies
  of this license document, but changing it is not allowed.
  ```
  
