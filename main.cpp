#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <WiFiClientSecure.h>

// --- Konfigurasi Bot Telegram ---
// Ganti dengan token bot Telegram Anda
const char* telegramBotToken = "7640360156:AAGpsIK_ngPCyaZHVRvUVNJgD-zUBugQdSM";
// Ganti dengan ID chat Telegram Anda
const char* chatId = "6031752986"; 
String telegramApiUrl = "https://api.telegram.org/bot" + String(telegramBotToken);
long lastUpdateId = 0;

// --- Konfigurasi URL Backend Hosting ---
// Ganti dengan URL backend Anda
const char* backendUrl = "https://wildanhusna.my.id/api.php"; 
const char* sensorUpdateEndpoint = "?endpoint=sensor-update";
const char* relayControlEndpoint = "?endpoint=relay-control";
const char* relayStatusEndpoint = "?endpoint=relay-status";

// --- Konfigurasi DHT11 ---
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Konfigurasi OLED Display (I2C) ---
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

// --- DATA BITMAP LOGO (TETAP SAMA) ---
const unsigned char gImage_lo[] PROGMEM = {
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X40,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X60,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X7C,0X3C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X7C,0X3C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X7C,0X7C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X7C,0X7C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X1C,0X7C,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X1E,0X7C,0XF8,0X00,0X00,0X01,0X40,0X40,0X00,0X02,0X01,0X80,0X00,0X00,0X00,0X00,
    0X1E,0X7D,0XF0,0X00,0X00,0X01,0X00,0X40,0X00,0X00,0X01,0X00,0X00,0X00,0X00,0X00,
    0X0F,0X7F,0XF0,0X00,0X00,0X36,0X52,0X92,0X4D,0X92,0XC9,0X34,0X00,0X00,0X00,0X00,
    0X0F,0X7B,0XE0,0X00,0X00,0X22,0X50,0XA4,0X49,0X12,0X89,0X62,0X00,0X00,0X00,0X00,
    0X0F,0X77,0XE0,0X00,0X0F,0X22,0X46,0XE4,0X51,0X14,0X89,0X42,0XF0,0X00,0X00,0XF0,
    0X07,0XF7,0XC0,0X00,0X0F,0X26,0X82,0X96,0XD1,0X14,0X8A,0X24,0XF0,0X00,0X00,0XF0,
    0X07,0XEF,0X80,0X00,0X0F,0X1A,0X9C,0X93,0X11,0X24,0X92,0X18,0XE0,0X00,0X00,0XF0,
    0X07,0XFE,0X00,0X00,0X1E,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XE0,0X00,0X01,0XE0,
    0X07,0XC7,0XE0,0X00,0X1E,0X30,0X00,0XC0,0X00,0X08,0X00,0X21,0XE0,0X30,0X01,0XE0,
    0X03,0XDF,0XF8,0X00,0X1F,0XFC,0XA7,0XF7,0X87,0X7F,0X01,0XFD,0XE0,0XFE,0XF1,0XE0,
    0X03,0X7F,0XFE,0X00,0X1F,0XFE,0X0F,0XFF,0X8F,0XFF,0X83,0XFF,0XE3,0XFF,0XE1,0XE0,
    0X03,0XFF,0X1E,0X00,0X1F,0XFE,0X1F,0XFF,0X8F,0XFF,0X87,0XFF,0XC7,0XFF,0XE1,0XE0,
    0X02,0XFC,0XFF,0X80,0X3E,0X1F,0X3E,0X0F,0X8F,0X87,0X8F,0X87,0XC7,0X83,0XE3,0XC0,
    0X01,0XF3,0XFF,0X80,0X3C,0X0F,0X3C,0X0F,0X0F,0X07,0X8F,0X03,0XCF,0X81,0XE3,0XC0,
    0X01,0XE7,0XFF,0XC0,0X3C,0X0E,0X3C,0X0F,0X0F,0X07,0X9E,0X03,0XCF,0X01,0XE3,0XC0,
    0X03,0XCF,0XFF,0XC0,0X3C,0X1E,0X3C,0X0F,0X1E,0X07,0X9E,0X03,0XCF,0X01,0XC3,0XC0,
    0X03,0XDF,0XE1,0XC0,0X38,0X1E,0X3C,0X0F,0X1E,0X07,0X9E,0X03,0X8F,0X03,0XC3,0XC0,
    0X03,0XBF,0X3F,0X00,0X78,0X1E,0X3C,0X1F,0X1E,0X07,0X1F,0X07,0X8F,0X03,0XC7,0X80,
    0X03,0XBC,0XFF,0XC0,0X78,0X1E,0X3E,0X3E,0X1E,0X0F,0X0F,0X9F,0X87,0XCF,0XC7,0X80,
    0X03,0XFB,0XFF,0XC0,0X78,0X1C,0X1F,0XFE,0X1C,0X0F,0X0F,0XFF,0X87,0XFF,0XC7,0X80,
    0X03,0XF7,0XFF,0XC0,0X78,0X3C,0X0F,0XFE,0X3C,0X0F,0X07,0XFF,0X83,0XFF,0X87,0X80,
    0X01,0XF7,0X03,0X80,0X70,0X3C,0X07,0XDE,0X3C,0X0F,0X01,0XE7,0X00,0XF3,0X87,0X80,
    0X01,0XFE,0X01,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0XBC,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X75,0XF0,0X00,0X03,0XFC,0XA8,0X1D,0XD7,0XCF,0X94,0X84,0X98,0XD7,0XE9,0X00,
    0X00,0X3B,0XF0,0X00,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X1F,0XF0,0X00,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

// --- Konfigurasi Buzzer dan Relay ---
#define BUZZER_PIN D7
#define RELAY_PIN D6
bool isRelayOn = false;

// --- Konfigurasi Waktu ---
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

// Klien Wi-Fi Aman untuk Telegram dan Backend Hosting
WiFiClientSecure client;

// Status notifikasi untuk menghindari spam
bool tempHighNotificationSent = false;
bool tempNormalNotificationSent = false;
static unsigned long lastSensorRead = 0;
static unsigned long lastTelegramCheck = 0;
static unsigned long lastRelayStatusCheck = 0;

// --- VARIABEL UNTUK CONFIGURATION PORTAL ---
const char* apSsid = "DC Monitor"; // SSID AP untuk konfigurasi
const char* configFilePath = "/wifi_config.json"; // File untuk menyimpan kredensial
ESP8266WebServer server(80);
DNSServer dnsServer;

// --- STRUKTUR UNTUK KREDENSIAL WI-FI ---
struct WifiConfig {
    char ssid[32];
    char password[64];
};
WifiConfig wifiConfig;

// Fungsi untuk menyimpan kredensial Wi-Fi ke LittleFS
void saveConfig() {
    File configFile = LittleFS.open(configFilePath, "w");
    if (!configFile) {
        Serial.println("[ERROR] Gagal membuka file konfigurasi untuk ditulis");
        return;
    }
    StaticJsonDocument<128> doc;
    doc["ssid"] = wifiConfig.ssid;
    doc["password"] = wifiConfig.password;
    if (serializeJson(doc, configFile) == 0) {
        Serial.println("[ERROR] Gagal menulis ke file");
    }
    configFile.close();
    Serial.println("[INFO] Kredensial WiFi disimpan.");
}

// Fungsi untuk memuat kredensial Wi-Fi dari LittleFS
bool loadConfig() {
    if (LittleFS.exists(configFilePath)) {
        File configFile = LittleFS.open(configFilePath, "r");
        if (configFile) {
            StaticJsonDocument<128> doc;
            DeserializationError error = deserializeJson(doc, configFile);
            if (!error) {
                strlcpy(wifiConfig.ssid, doc["ssid"], sizeof(wifiConfig.ssid));
                strlcpy(wifiConfig.password, doc["password"], sizeof(wifiConfig.password));
                configFile.close();
                Serial.println("[INFO] Kredensial WiFi dimuat dari file.");
                return true;
            } else {
                Serial.println("[ERROR] Gagal mengurai file konfigurasi");
            }
        }
    }
    Serial.println("[INFO] Tidak ada kredensial WiFi yang ditemukan.");
    return false;
}

// Fungsi handler untuk halaman root web server (HTML Form)
void handleRoot() {
    String html = "<html><body>"
                  "<h1>Konfigurasi WiFi ESP8266</h1>"
                  "<form action='/save' method='post'>"
                  "SSID: <input type='text' name='ssid' value=''><br>"
                  "Password: <input type='password' name='password' value=''><br>"
                  "<input type='submit' value='Hubungkan'>"
                  "</form></body></html>";
    server.send(200, "text/html", html);
}

// Fungsi handler untuk menyimpan kredensial Wi-Fi dari form HTML
void handleSave() {
    // Memastikan argumen form tersedia sebelum digunakan
    if (server.hasArg("ssid") && server.hasArg("password")) {
        String ssid = server.arg("ssid");
        String password = server.arg("password");
        ssid.toCharArray(wifiConfig.ssid, sizeof(wifiConfig.ssid));
        password.toCharArray(wifiConfig.password, sizeof(wifiConfig.password));
        saveConfig();
        server.send(200, "text/plain", "Kredensial disimpan. Memulai ulang...");
        Serial.println("[INFO] Kredensial disimpan melalui form. Memulai ulang...");
        ESP.restart();
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

// Fungsi handler BARU untuk menerima kredensial dari aplikasi mobile via JSON
void handleJsonConfig() {
    // Periksa apakah metode permintaan adalah POST
    if (server.method() == HTTP_POST) {
        // Periksa apakah tipe konten adalah JSON
        if (server.hasHeader("Content-Type") && server.header("Content-Type") == "application/json") {
            String body = server.arg("plain");
            StaticJsonDocument<128> doc;
            DeserializationError error = deserializeJson(doc, body);

            if (!error) {
                const char* ssid = doc["ssid"];
                const char* password = doc["password"];

                if (ssid && password) {
                    strlcpy(wifiConfig.ssid, ssid, sizeof(wifiConfig.ssid));
                    strlcpy(wifiConfig.password, password, sizeof(wifiConfig.password));
                    saveConfig();
                    server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Kredensial disimpan. Memulai ulang...\"}");
                    Serial.println("[INFO] Kredensial disimpan melalui JSON. Memulai ulang...");
                    ESP.restart();
                } else {
                    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"SSID atau password tidak ditemukan di JSON.\"}");
                    Serial.println("[ERROR] SSID atau password tidak ditemukan di JSON.");
                }
            } else {
                server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Gagal mengurai JSON.\"}");
                // BARIS INI DIPERBAIKI
                Serial.println(String("[ERROR] Gagal mengurai JSON: ") + error.c_str());
            }
        } else {
            server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Tipe konten harus application/json.\"}");
        }
    } else {
        server.send(405, "application/json", "{\"status\":\"error\",\"message\":\"Metode tidak diizinkan. Gunakan POST.\"}");
    }
}


// Fungsi untuk memulai configuration portal
void startConfigPortal() {
    Serial.println("[SETUP] Memulai portal konfigurasi...");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Portal Konfigurasi");
    display.print("Hubungkan ke: ");
    display.println(apSsid);
    display.display();

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid);

    dnsServer.start(53, "*", WiFi.softAPIP());
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.on("/config-api", handleJsonConfig); // --- DAFTARKAN HANDLER BARU UNTUK APLIKASI MOBILE ---
    server.begin();
    Serial.print("[INFO] Alamat IP AP: ");
    Serial.println(WiFi.softAPIP());
    
    // Loop untuk menjalankan server
    while (true) {
        dnsServer.processNextRequest();
        server.handleClient();
        delay(10);
    }
}

// ---------- Fungsi Tambahan (Buzzer, Telegram, API Backend) ----------

void playEmergencyAlarm() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(75);
        digitalWrite(BUZZER_PIN, LOW);
        delay(75);
    }
}

void sendMessage(String text) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = telegramApiUrl + "/sendMessage?chat_id=" + String(chatId) + "&text=" + text;
        client.setInsecure();
        http.begin(client, url);
        int httpCode = http.GET();
        if (httpCode > 0) {
            Serial.printf("[INFO] Pesan Telegram terkirim, kode: %d\n", httpCode);
        } else {
            Serial.printf("[ERROR] Gagal mengirim pesan Telegram, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }
}

void checkNewMessages() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = telegramApiUrl + "/getUpdates?offset=" + String(lastUpdateId + 1);
        client.setInsecure();
        http.begin(client, url);
        int httpCode = http.GET();
        
        if (httpCode > 0) {
            String payload = http.getString();
            
            DynamicJsonDocument doc(2048);
            DeserializationError error = deserializeJson(doc, payload);

            if (error) {
                Serial.print(F("[ERROR] Deserialisasi gagal: "));
                Serial.println(error.c_str());
                return;
            }
            
            JsonArray results = doc["result"];
            
            for (JsonObject result : results) {
                long update_id = result["update_id"];
                String messageText = result["message"]["text"].as<String>();
                lastUpdateId = update_id;
                
                Serial.print("[INFO] Pesan Telegram diterima: ");
                Serial.println(messageText);
                
                if (messageText.equalsIgnoreCase("suhu") || messageText.equalsIgnoreCase("/suhu")) {
                    float t = dht.readTemperature();
                    if (isnan(t)) {
                        sendMessage("Gagal membaca sensor suhu.");
                    } else {
                        sendMessage("Suhu saat ini adalah " + String(t) + " C");
                    }
                } else if (messageText.equalsIgnoreCase("kelembaban") || messageText.equalsIgnoreCase("/kelembaban")) {
                    float h = dht.readHumidity();
                    if (isnan(h)) {
                        sendMessage("Gagal membaca sensor kelembaban.");
                    } else {
                        sendMessage("Kelembaban saat ini adalah " + String(h) + " %");
                    }
                } else if (messageText.equalsIgnoreCase("nyalakan relay") || messageText.equalsIgnoreCase("/nyalakan")) {
                    String relayApiUrl = String(backendUrl) + String(relayControlEndpoint);
                    String requestBody = "{\"state\": \"on\"}";
                    HTTPClient relayHttp;
                    client.setInsecure();
                    relayHttp.begin(client, relayApiUrl);
                    relayHttp.addHeader("Content-Type", "application/json");
                    int httpCode = relayHttp.POST(requestBody);
                    if (httpCode > 0) {
                        sendMessage("Memerintahkan relay untuk menyala.");
                    } else {
                        sendMessage("Gagal mengirim perintah ke server.");
                    }
                    relayHttp.end();
                } else if (messageText.equalsIgnoreCase("matikan relay") || messageText.equalsIgnoreCase("/matikan")) {
                    String relayApiUrl = String(backendUrl) + String(relayControlEndpoint);
                    String requestBody = "{\"state\": \"off\"}";
                    HTTPClient relayHttp;
                    client.setInsecure();
                    relayHttp.begin(client, relayApiUrl);
                    relayHttp.addHeader("Content-Type", "application/json");
                    int httpCode = relayHttp.POST(requestBody);
                    if (httpCode > 0) {
                        sendMessage("Memerintahkan relay untuk mati.");
                    } else {
                        sendMessage("Gagal mengirim perintah ke server.");
                    }
                    relayHttp.end();
                } else {
                    sendMessage("Maaf, perintah tidak dikenali. Coba 'suhu', 'kelembaban', 'nyalakan relay', atau 'matikan relay'.");
                }
            }
        }
        http.end();
    }
}

void checkRelayStatusFromServer() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String apiUrl = String(backendUrl) + String(relayStatusEndpoint);
        client.setInsecure();
        http.begin(client, apiUrl);
        int httpCode = http.GET();
        
        if (httpCode > 0) {
            String payload = http.getString();
            DynamicJsonDocument doc(256);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                const char* state = doc["relayState"];
                if (strcmp(state, "on") == 0) {
                    digitalWrite(RELAY_PIN, LOW); // Logika terbalik: LOW = ON
                    isRelayOn = true;
                } else {
                    digitalWrite(RELAY_PIN, HIGH); // Logika terbalik: HIGH = OFF
                    isRelayOn = false;
                }
                Serial.printf("[INFO] Status relay dari server: %s\n", state);
            } else {
                Serial.println("[ERROR] Gagal mengurai JSON status relay.");
            }
        } else {
            Serial.printf("[ERROR] Gagal mendapatkan status relay dari server. Kode HTTP: %d\n", httpCode);
        }
        http.end();
    }
}

// ----------------------------------------------------------------------
// ---------------------------- FUNGSI UTAMA ----------------------------
// ----------------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    Serial.println("\n[SETUP] Memulai Data Center Monitor...");

    Wire.begin();
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("[ERROR] Alokasi SSD1306 gagal!"));
        for (;;) { delay(1000); }
    }

    display.clearDisplay();
    display.drawBitmap(0, 0, gImage_lo, OLED_WIDTH, OLED_HEIGHT, 1);
    display.display();
    delay(3000);

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    dht.begin();
    Serial.println("[SETUP] Sensor DHT dimulai.");

    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    digitalWrite(RELAY_PIN, HIGH); // Logika terbalik: HIGH = OFF
    digitalWrite(BUZZER_PIN, LOW);
    isRelayOn = false;
    Serial.println("[SETUP] Pin Relay dan Buzzer diinisialisasi (MATI).");

    // Inisialisasi LittleFS
    if (!LittleFS.begin()) {
        Serial.println("[ERROR] Gagal menginisialisasi LittleFS");
        for (;;) { delay(1000); }
    }

    // Periksa apakah ada kredensial Wi-Fi yang tersimpan
    if (loadConfig()) {
        Serial.println("[INFO] Mencoba terhubung ke WiFi yang disimpan...");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Menghubungkan ke");
        display.println(wifiConfig.ssid);
        display.display();

        WiFi.mode(WIFI_STA);
        WiFi.begin(wifiConfig.ssid, wifiConfig.password);
        int attempts = 0;
        // Coba terhubung selama 15 detik
        while (WiFi.status() != WL_CONNECTED && attempts < 30) { 
            delay(500);
            Serial.print(".");
            display.print(".");
            display.display();
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n[INFO] WiFi terhubung!");
            Serial.print("[INFO] Alamat IP: ");
            Serial.println(WiFi.localIP());

            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
            Serial.println("[INFO] Waktu dikonfigurasi melalui NTP.");

            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("WiFi Terhubung!");
            display.println(WiFi.localIP());
            display.display();
            delay(2000);
            
            client.setInsecure();
            sendMessage("Bot ESP8266 sudah online dan terhubung ke WiFi!");
        } else {
            Serial.println("\n[ERROR] Gagal terhubung ke WiFi. Memulai ulang dengan portal konfigurasi.");
            // Hapus file konfigurasi agar bisa dikonfigurasi ulang
            LittleFS.remove(configFilePath);
            startConfigPortal();
        }
    } else {
        // Jika tidak ada kredensial yang tersimpan, langsung mulai portal konfigurasi
        startConfigPortal();
    }
}

void loop() {
    // Jalankan program utama hanya jika terhubung ke Wi-Fi
    if (WiFi.status() == WL_CONNECTED) {
        // Periksa pesan Telegram setiap 5 detik
        if (millis() - lastTelegramCheck >= 5000) {
            checkNewMessages();
            lastTelegramCheck = millis();
        }

        // Periksa status relay dari server setiap 10 detik
        if (millis() - lastRelayStatusCheck >= 10000) {
            checkRelayStatusFromServer();
            lastRelayStatusCheck = millis();
        }

        // Baca sensor dan kirim data setiap 5 detik
        if (millis() - lastSensorRead >= 5000) {
            float h = dht.readHumidity();
            float t = dht.readTemperature();
            
            lastSensorRead = millis();

            struct tm timeinfo;
            if (!getLocalTime(&timeinfo)) {
                Serial.println("[ERROR] Gagal mendapatkan waktu");
                timeinfo.tm_hour = 0;
                timeinfo.tm_min = 0;
                timeinfo.tm_sec = 0;
            }

            if (isnan(h) || isnan(t)) {
                Serial.println("[ERROR] Gagal membaca dari sensor DHT!");
                h = 0.0;
                t = 0.0;
            }
            
            // --- LOGIKA ALARM DAN NOTIFIKASI ---
            if (t > 25.0) {
                playEmergencyAlarm();
                
                if (!tempHighNotificationSent) {
                    sendMessage("!!! PERINGATAN !!! Suhu di atas 25 C, saat ini: " + String(t) + " C. Buzzer aktif.");
                    tempHighNotificationSent = true;
                    tempNormalNotificationSent = false;
                }
                
                Serial.println("[ALARM] Suhu di atas 25 C! Buzzer ON.");
                display.clearDisplay();
                display.setCursor(0, 0);
                display.println("!!! SUHU TINGGI !!!");
                display.print("Suhu > 25 C");
                display.setCursor(0, 24);
                display.printf("Suhu: %.1f C", t);
                display.display();
            } else {
                digitalWrite(BUZZER_PIN, LOW);
                
                if (!tempNormalNotificationSent && tempHighNotificationSent) {
                    sendMessage("Suhu telah kembali normal. Saat ini: " + String(t) + " C.");
                    tempNormalNotificationSent = true;
                    tempHighNotificationSent = false;
                }
                
                Serial.println("\n--- Pembacaan Sensor ---");
                Serial.print("Suhu: "); Serial.print(t); Serial.println(" *C");
                Serial.print("Kelembaban: "); Serial.print(h); Serial.println(" %");
                Serial.println("-----------------------");

                display.clearDisplay();
                display.setCursor(0, 0);
                display.println("DATA CENTER MONITOR");
                display.println("-------------------");
                display.print("Suhu: "); display.print(t); display.println(" C");
                display.print("Kelembaban: "); display.print(h); display.println(" %");
                display.println("-------------------");
                display.printf("%02d:%02d:%02d WIB", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
                display.display();
            }

            // Kirim data sensor ke backend
            HTTPClient http;
            String serverPath = String(backendUrl) + String(sensorUpdateEndpoint);
            client.setInsecure();
            http.begin(client, serverPath);
            http.addHeader("Content-Type", "application/json");
            StaticJsonDocument<256> doc;
            doc["temperature"] = t;
            doc["humidity"] = h;
            String requestBody;
            serializeJson(doc, requestBody);
            Serial.println("[INFO] Mengirim data JSON ke backend: " + requestBody);
            int httpResponseCode = http.POST(requestBody);
            if (httpResponseCode > 0) {
                Serial.printf("[INFO] Kode Respons HTTP: %d\n", httpResponseCode);
                String responsePayload = http.getString();
                Serial.println("[INFO] Respons Server: " + responsePayload);
            } else {
                Serial.printf("[ERROR] Gagal mengirim permintaan POST. Kode HTTP: %d\n", httpResponseCode);
                Serial.println(http.errorToString(httpResponseCode));
            }
            http.end();
        }
    } else {
        // Jika koneksi terputus saat runtime, coba sambungkan kembali
        Serial.println("[INFO] WiFi Terputus, mencoba menyambungkan kembali...");
        WiFi.begin(wifiConfig.ssid, wifiConfig.password);
        delay(5000);
    }
}
