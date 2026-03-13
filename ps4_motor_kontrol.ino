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
        .controls-container { display: flex; flex-direction: column; align-items: center; gap: 20px; margin-top: 20px; }
        #joystick-zone { width: 200px; height: 200px; background: #1a1a1a; border-radius: 50%; position: relative; border: 2px solid #333; margin: 0 auto; touch-action: none; }
        @media (min-width: 768px) {
            .controls-container { flex-direction: row; justify-content: center; gap: 50px; align-items: flex-start; }
            .grid { margin: 0; }
        }
    </style>
    <!-- NippleJS for Virtual Joystick -->
    <script src="https://cdnjs.cloudflare.com/ajax/libs/nipplejs/0.10.1/nipplejs.min.js"></script>
</head>
<body>
    <h2>Robot Kontrol / Control Panel</h2>
    <div class="controls-container">
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

    <div id="joystick-zone"></div>
    </div>

    <div class="status">Sinyal / Signal: <span id="log">Ready</span></div>
    
    <script>
        var lastSent = "";
        var joyTimer = null;
        var joyX = 0;
        var joyY = 0;
        var isJoystickActive = false;
        
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
            if (lastSent !== "/dur" && !isJoystickActive) {
                cmd("/dur");
            }
        };

        // Joystick Setup
        var manager = nipplejs.create({
            zone: document.getElementById('joystick-zone'),
            mode: 'static',
            position: { left: '50%', top: '50%' },
            color: '#3498db',
            size: 150
        });

        manager.on('start', function(evt, data) {
            isJoystickActive = true;
            joyTimer = setInterval(sendJoystickData, 100); // 100ms interval
        });

        manager.on('move', function(evt, data) {
            // data.vector.x and data.vector.y are unit vectors (direction only)
            // We scale them by the force or distance to get proportional speed
            // Force is usually between 0 and 2-3 depending on how far it's dragged.
            // We cap the force multiplier at 1 to represent max 100% speed.
            var forceMultiplier = Math.min(data.force || 1, 1);
            joyX = Math.round(data.vector.x * forceMultiplier * 100);
            joyY = Math.round(data.vector.y * forceMultiplier * 100);
        });

        manager.on('end', function(evt, data) {
            isJoystickActive = false;
            clearInterval(joyTimer);
            joyX = 0;
            joyY = 0;
            cmd("/dur");
        });

        function sendJoystickData() {
            if(joyX === 0 && joyY === 0) return;
            var path = "/joy?x=" + joyX + "&y=" + joyY;
            if (path !== lastSent) {
                cmd(path);
            }
        }
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

    // Joystick (Analog) Hareketi / Joystick (Analog) Movement
    server.on("/joy", []() {
        if (server.hasArg("x") && server.hasArg("y")) {
            int x = server.arg("x").toInt(); // -100 to 100
            int y = server.arg("y").toInt(); // -100 to 100

            // Diferansiyel surus algoritmasi / Differential drive algorithm
            int v = y;
            int w = x;

            int right = v - w;
            int left = v + w;

            // Sinirlama / Constrain (-100 to 100)
            if (left > 100) left = 100;
            if (left < -100) left = -100;
            if (right > 100) right = 100;
            if (right < -100) right = -100;

            // Motor yonleri / Motor directions (1: Ileri/Forward, 0: Geri/Backward)
            int dirLeft = (left >= 0) ? 1 : 0;
            int dirRight = (right >= 0) ? 1 : 0;

            // Hizlari mutlak degere cevir / Convert speeds to absolute values
            int speedLeft = abs(left);
            int speedRight = abs(right);

            MotorSurucu.MotorDrive(MOTOR1, speedLeft, dirLeft);
            MotorSurucu.MotorDrive(MOTOR2, speedRight, dirRight);

            sendSuccessResponse();
        } else {
            server.send(400, "text/plain", "Bad Request: Missing x or y");
        }
    });

    server.begin();
    Serial.println("Sistem Hazir / System Ready");
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
}

void loop() {
    server.handleClient();
}
