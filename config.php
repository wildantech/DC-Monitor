<?php 
// Atur zona waktu default ke Asia/Jakarta
date_default_timezone_set('Asia/Jakarta');

// ====== KONFIGURASI DATABASE MYSQL ====== 
$host = "127.0.0.1";       // Pastikan ini adalah '127.0.0.1'
$user = "wildanhu_admin";  // user MySQL dari cPanel
$pass = "wildaN7.";        // password MySQL
$dbname = "wildanhu_dc";   // nama database

try { 
    // Buat koneksi PDO
    $db = new PDO("mysql:host=$host;dbname=$dbname;charset=utf8", $user, $pass); 
    // Atur atribut error mode untuk menampilkan exception
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION); 
} catch (PDOException $e) { 
    // Jika koneksi gagal, tampilkan pesan error yang lebih spesifik
    die(json_encode(["error" => "Koneksi DB gagal: " . $e->getMessage()])); 
} 
?>
