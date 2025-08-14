<?php
header("Content-Type: application/json");
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: GET, POST");
header("Access-Control-Allow-Headers: Content-Type");

require_once "config.php";

$method = $_SERVER['REQUEST_METHOD'];
$path = isset($_GET['endpoint']) ? $_GET['endpoint'] : '';

// --- Fungsi untuk mendapatkan status relay terakhir ---
function getRelayStatus($db) {
    $stmt = $db->query("SELECT state FROM relay_logs ORDER BY timestamp DESC LIMIT 1");
    $row = $stmt->fetch(PDO::FETCH_ASSOC);
    return $row ? $row['state'] : 'off';
}

// --- GET: Data real-time dan status relay ---
if ($method === 'GET' && $path === 'realtime-data') {
    $stmt = $db->query("SELECT temperature, humidity, timestamp FROM dht_raw_logs ORDER BY timestamp DESC LIMIT 1");
    $data = $stmt->fetch(PDO::FETCH_ASSOC);

    if ($data) {
        $timestamp = new DateTime($data['timestamp']);
        $relayState = getRelayStatus($db); // Ambil status relay terbaru

        $response = [
            "temperature" => (float)$data['temperature'],
            "humidity" => (float)$data['humidity'],
            "lastUpdated" => $timestamp->format('d-m-Y H:i:s'),
            "connectionStatus" => "Connected",
            "statusMessage" => "OK",
            "relayState" => $relayState
        ];
    } else {
        $response = [
            "temperature" => null,
            "humidity" => null,
            "lastUpdated" => "N/A",
            "connectionStatus" => "Disconnected",
            "statusMessage" => "NO DATA YET",
            "relayState" => "off"
        ];
    }

    echo json_encode($response);
}

// --- GET: Data historis (tidak berubah) ---
elseif ($method === 'GET' && $path === 'historical-data') {
    $stmt = $db->query("SELECT timestamp, avg_temperature, avg_humidity FROM dht_avg_logs ORDER BY timestamp ASC");
    echo json_encode($stmt->fetchAll(PDO::FETCH_ASSOC));
}

// --- POST: Update data sensor (tidak berubah) ---
elseif ($method === 'POST' && $path === 'sensor-update') {
    $input = json_decode(file_get_contents("php://input"), true);
    if (isset($input['temperature']) && isset($input['humidity'])) {
        $stmt = $db->prepare("INSERT INTO dht_raw_logs (temperature, humidity) VALUES (?, ?)");
        $stmt->execute([$input['temperature'], $input['humidity']]);
        echo json_encode(["message" => "Data sensor berhasil diperbarui."]);
    } else {
        http_response_code(400);
        echo json_encode(["error" => "Data tidak lengkap."]);
    }
}

// --- POST: Kontrol relay (tidak berubah) ---
elseif ($method === 'POST' && $path === 'relay-control') {
    $input = json_decode(file_get_contents("php://input"), true);
    if (isset($input['state']) && in_array($input['state'], ['on', 'off'])) {
        $stmt = $db->prepare("INSERT INTO relay_logs (state) VALUES (?)");
        $stmt->execute([$input['state']]);
        echo json_encode(["message" => "Relay set to {$input['state']}"]);
    } else {
        http_response_code(400);
        echo json_encode(["error" => "Invalid relay state."]);
    }
}

// --- GET: Status relay (tidak berubah) ---
elseif ($method === 'GET' && $path === 'relay-status') {
    $state = getRelayStatus($db); // Menggunakan fungsi yang sudah ada
    echo json_encode(["relayState" => $state]);
}

else {
    http_response_code(404);
    echo json_encode(["error" => "Endpoint tidak ditemukan."]);
}
?>
