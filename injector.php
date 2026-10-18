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