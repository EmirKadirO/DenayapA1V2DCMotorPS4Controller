/*
 * Proje: Deneyap Kart Wifi Tabanli Robot Kontrolu
 * Project: Deneyap Board Wifi Based Robot Control
 * * Aciklama: Bu kod, Deneyap Kart'in (ESP32-S3) bir erisim noktasi (AP) olusturarak 
 * web tarayicisi veya Python scripti uzerinden robotu kontrol etmesini saglar.
 * Description: This code allows the Deneyap Board (ESP32-S3) to create an Access Point (AP)
 * and control the robot via a web browser or a Python script.
 * * Donanim: Deneyap Kart, Deneyap Cift Kanalli Motor Surucu (I2C)
 * Hardware: Deneyap Board, Deneyap Dual Channel Motor Driver (I2C)
 */

#include <WiFi.h>
#include <WebServer.h>
#include <Deneyap_CiftKanalliMotorSurucu.h>

// Wi-Fi Ayarlari / Wi-Fi Configurations
const char* ssid = "Deneyap_Robot";
const char* password = "deneyap-robot"; 

WebServer server(80);
DualMotorDriver MotorSurucu;

// HTML Arayuzu / HTML User Interface
String htmlSayfasi = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Deneyap Kumanda / Remote Control</title>
    <style>
        body { font-family: sans-serif; text-align: center; background: #0f0f0f; color: #ffffff; margin: 0; padding: 10px; user-select: none; }
        .grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; max-width: 350px; margin: 20px auto; }
        .btn { padding: 25px 10px; background: #2c3e50; color: white; border-radius: 15px; border: 1px solid #34495e; font-size: 22px; cursor: pointer; touch-action: none; }
        .btn:active { background: #27ae60; border-color: #2ecc71; transform: scale(0.95); }
        .stop { background: #c0392b; border-color: #e74c3c; }
        .status { margin-top: 20px; font-size: 14px; color: #888; border-top: 1px solid #222; padding-top: 10px; }
        #log { color: #2ecc71; font-weight: bold; }
    </style>
</head>
<body>
    <h2>Robot Kontrol / Control Panel</h2>
    <div class="grid">
        <button class="btn" onmousedown="handle(event, '/ilerisol')" onmouseup="handle(event, '/dur')" ontouchstart="handle(event, '/ilerisol')" ontouchend="handle(event, '/dur')">↖</button>
        <button class="btn" onmousedown="handle(event, '/ileri')" onmouseup="handle(event, '/dur')" ontouchstart="handle(event, '/ileri')" ontouchend="handle(event, '/dur')">▲</button>
        <button class="btn" onmousedown="handle(event, '/ilerisag')" onmouseup="handle(event, '/dur')" ontouchstart="handle(event, '/ilerisag')" ontouchend="handle(event, '/dur')">↗</button>
        
        <button class="btn" onmousedown="handle(event, '/sol')" onmouseup="handle(event, '/dur')" ontouchstart="handle(event, '/sol')" ontouchend="handle(event, '/dur')">◀</button>
        <button class="btn stop" onmousedown="handle(event, '/dur')" ontouchstart="handle(event, '/dur')">■</button>
        <button class="btn" onmousedown="handle(event, '/sag')" onmouseup="handle(event, '/dur')" ontouchstart="handle(event, '/sag')" ontouchend="handle(event, '/dur')">▶</button>
        
        <button class="btn" onmousedown="handle(event, '/gerisol')" onmouseup="handle(event, '/dur')" ontouchstart="handle(event, '/gerisol')" ontouchend="handle(event, '/dur')">↙</button>
        <button class="btn" onmousedown="handle(event, '/geri')" onmouseup="handle(event, '/dur')" ontouchstart="handle(event, '/geri')" ontouchend="handle(event, '/dur')">▼</button>
        <button class="btn" onmousedown="handle(event, '/gerisag')" onmouseup="handle(event, '/dur')" ontouchstart="handle(event, '/gerisag')" ontouchend="handle(event, '/dur')">↘</button>
    </div>
    <div class="status">Sinyal / Signal: <span id="log">Ready</span></div>
    
    <script>
        var lastSent = "";
        
        function handle(e, path) {
            if (e.cancelable) e.preventDefault(); 
            if (path === lastSent && path !== "/dur") return; 
            cmd(path);
        }

        function cmd(path) {
            lastSent = path;
            document.getElementById('log').innerText = "Sending: " + path;
            var xhr = new XMLHttpRequest();
            xhr.open("GET", path, true);
            xhr.timeout = 400; 
            xhr.onreadystatechange = function() {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    document.getElementById('log').innerText = "Success: " + path;
                    document.getElementById('log').style.color = "#2ecc71";
                }
            };
            xhr.send();
        }

        // Guvenlik: Fare birakildiginda robotu durdur / Security: Stop when mouse is released
        window.onmouseup = function() {
            if (lastSent !== "/dur") {
                cmd("/dur");
            }
        };
    </script>
</body>
</html>
)rawliteral";

// Basarili yanıt yardımcı fonksiyonu / Success response helper
void sendSuccessResponse() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", "OK");
}

void setup() {
    Serial.begin(115200);
    delay(500);

    // Motor surucu baslatma / Initialize motor driver
    if (!MotorSurucu.begin(0x16)) {
        Serial.println("HATA: Motor surucuye ulasilamadi! / ERROR: Driver not found!");
    }

    // Wifi Yapilandirmasi / Wifi Configuration
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    WiFi.setSleep(false); // Stabilitie icin / For stability

    // Sunucu Rotalari / Server Routes
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", htmlSayfasi);
    });

    // Hareketler / Movements
    server.on("/ileri", []() { MotorSurucu.MotorDrive(MOTOR1, 100, 1); MotorSurucu.MotorDrive(MOTOR2, 100, 1); sendSuccessResponse(); });
    server.on("/geri",  []() { MotorSurucu.MotorDrive(MOTOR1, 100, 0); MotorSurucu.MotorDrive(MOTOR2, 100, 0); sendSuccessResponse(); });
    server.on("/sol",   []() { MotorSurucu.MotorDrive(MOTOR1, 100, 1); MotorSurucu.MotorDrive(MOTOR2, 100, 0); sendSuccessResponse(); });
    server.on("/sag",   []() { MotorSurucu.MotorDrive(MOTOR1, 100, 0); MotorSurucu.MotorDrive(MOTOR2, 100, 1); sendSuccessResponse(); });
    server.on("/dur",   []() { MotorSurucu.MotorDrive(MOTOR1, 0, 1);   MotorSurucu.MotorDrive(MOTOR2, 0, 1);   sendSuccessResponse(); });

    // Capraz Hareketler / Diagonal Movements
    server.on("/ilerisol", []() { MotorSurucu.MotorDrive(MOTOR1, 100, 1); MotorSurucu.MotorDrive(MOTOR2, 40, 1);  sendSuccessResponse(); });
    server.on("/ilerisag", []() { MotorSurucu.MotorDrive(MOTOR1, 40, 1);  MotorSurucu.MotorDrive(MOTOR2, 100, 1); sendSuccessResponse(); });
    server.on("/gerisol",  []() { MotorSurucu.MotorDrive(MOTOR1, 100, 0); MotorSurucu.MotorDrive(MOTOR2, 40, 0);  sendSuccessResponse(); });
    server.on("/gerisag",  []() { MotorSurucu.MotorDrive(MOTOR1, 40, 0);  MotorSurucu.MotorDrive(MOTOR2, 100, 0); sendSuccessResponse(); });

    server.begin();
    Serial.println("Sistem Hazir / System Ready");
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
}

void loop() {
    server.handleClient();
}
