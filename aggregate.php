<?php
require_once "config.php";

// Hitung rata-rata 15 menit terakhir
$stmt = $db->prepare("
    SELECT AVG(temperature) AS avg_temp, AVG(humidity) AS avg_hum
    FROM dht_raw_logs
    WHERE timestamp >= (NOW() - INTERVAL 15 MINUTE)
");
$stmt->execute();
$row = $stmt->fetch(PDO::FETCH_ASSOC);

if ($row && $row['avg_temp'] !== null) {
    $stmtInsert = $db->prepare("INSERT INTO dht_avg_logs (avg_temperature, avg_humidity) VALUES (?, ?)");
    $stmtInsert->execute([$row['avg_temp'], $row['avg_hum']]);
    echo "[AGGREGASI] Data rata-rata tersimpan.\n";
} else {
    echo "[AGGREGASI] Tidak ada data untuk dihitung.\n";
}

// Hapus data mentah lebih dari 15 menit
$db->exec("DELETE FROM dht_raw_logs WHERE timestamp < (NOW() - INTERVAL 15 MINUTE)");

// Hapus data rata-rata lebih dari 30 hari
$db->exec("DELETE FROM dht_avg_logs WHERE timestamp < (NOW() - INTERVAL 30 DAY)");
?>
