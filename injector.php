<?php

if(isset($_GET["uid"])) {
   $temperature = $_GET["uid"]; // get uid value from HTTP GET

   $servername = "localhost";
   $username = "Arduino";
   $password = "ArduinoGetStarted.com";
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
