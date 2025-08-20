<?php

if(isset($_GET["uid"])) {
   $uid = $_GET["uid"]; // get uid value from HTTP GET
   $nama = $_GET["nama"]; // get nama value from HTTP GET
   $points = $_GET["points"]; // get points value from HTTP GET

   $servername = "localhost";
   $username = "Arduino";
   $password = "ArduinoPass";
   $dbname = "db_arduino";

   // Create connection
   $conn = new mysqli($servername, $username, $password, $dbname);
   // Check connection
   if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
   }

   $sql = "INSERT INTO db_rvm (uid_value, name, points_value) VALUES ($uid, '$name', $points)";

   if ($conn->query($sql) === TRUE) {
      echo "New record created successfully";
   } else {
      echo "Error: " . $sql . " => " . $conn->error;
   }

   $conn->close();
} else {
   echo "Data is not set";
}

?>
