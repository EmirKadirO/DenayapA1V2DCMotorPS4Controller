<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=00b4d8&height=200&section=header&text=Deneyap%20Kart%20A1%20V2&fontSize=50" width="100%" />

  ![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
  ![Python](https://img.shields.io/badge/-Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
  ![ESP32-S3](https://img.shields.io/badge/-ESP32--S3-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
  ![WiFi](https://img.shields.io/badge/-WiFi-00514F?style=for-the-badge&logo=wi-fi&logoColor=white)
</div>

---

## Deneyap Kart A1 V2 WiFi Robot Control
In this project, you can control your robot with Deneyap Kart A1 V2 (ESP32-S3) both via a web interface over WiFi and with a PS4 controller. So, you can move your robot from your phone, computer, or even with a PS4 controller.

### 🚀 Highlighted Features
* **WiFi AP Mode:** The robot creates its own wireless network. You don’t need to look for an extra modem or router.
* **Web Interface:** There is a modern, fast, and fully responsive HTML5-based control panel. You can use it easily from your phone, tablet, or computer.
* **Diagonal Movement:** The robot moves in eight different directions at full speed and optimized performance. You get full performance from the motors.
* **Python Bridge:** You can instantly control the robot by connecting the PS4 controller to your computer. No lag issues.
* **CORS & Security:** The necessary security headers for desktop browsers and external scripts are included. We didn’t leave any vulnerability to bother you.

### 🛠 Required Hardware
* Deneyap Kart A1 V2 (ESP32-S3)
* Deneyap Dual Channel Motor Driver (I2C)
* 2 DC motors and a robot chassis

### ⚙ Setup and Usage
#### Arduino IDE Settings
First, install the ESP32 package and Deneyap Dual Channel Motor Driver library to the Arduino IDE.
Upload the `ps4_motor_kontrol.ino` file to the board.

#### WiFi Connection
Connect your computer or phone to the `Deneyap_Robot` network.
Password: `deneyap-robot`

#### Control via Web
Enter `192.168.4.1` from any browser and start controlling your robot instantly.

#### Control with PS4 Controller (Optional)
Python must be installed on your computer.
Install the necessary libraries: `pip install pygame-ce requests`
Run the `ps4_listener.py` file, connect your PS4 controller, and manage the robot in real time.

### 📁 Project File Structure
* `ps4_motor_kontrol.ino`: Main control code for the robot (Arduino side).
* `ps4_listener.py`: Python bridge code that connects the PS4 controller to the robot.
* `ps4_test.py`: Auxiliary tool for calibration and button tests of the PS4 controller.
