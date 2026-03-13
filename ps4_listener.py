# Proje: PS4 Kontrolcusu WiFi Kopru Yazilimi
# Project: PS4 Controller WiFi Bridge Software
# Aciklama: Pygame-ce kullanarak PS4 kolu verilerini okur ve WiFi uzerinden robota iletir.
# Description: Reads PS4 controller data using pygame-ce and forwards it to the robot via WiFi.

import pygame
import requests
import time
import sys

# --- AYARLAR / SETTINGS ---
# Robotun varsayilan IP adresi / Default IP address of the robot
ROBOT_IP = "http://192.168.4.1"
# Komut gonderim sıklıgı (saniye) / Command sending frequency (seconds)
COMMAND_INTERVAL = 0.05 
# --------------------------

def send_command(path):
    """
    Robotun web sunucusuna HTTP GET istegi gonderir.
    Sends an HTTP GET request to the robot's web server.
    """
    try:
        requests.get(ROBOT_IP + path, timeout=0.1)
        return True
    except Exception as e:
        # Baglanti hatalari sessizce gecilir / Connection errors are ignored silently
        return False

def main():
    # Pygame ve joystick modulleri baslatiliyor / Initialize pygame and joystick modules
    pygame.init()
    pygame.joystick.init()

    # Kontrolcu kontrolu / Controller check
    if pygame.joystick.get_count() == 0:
        print("HATA: PS4 Kolu bulunamadi! / ERROR: PS4 Controller not found!")
        pygame.quit()
        sys.exit()

    # Ilk kolu baslat / Initialize the first controller
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    
    # Kol yeteneklerini kontrol et / Check controller capabilities
    num_hats = joystick.get_numhats()
    num_buttons = joystick.get_numbuttons()
    
    print(f"\n--- PS4 Bridge Aktif / Active (pygame-ce) ---")
    print(f"Cihaz / Device: {joystick.get_name()}")
    print(f"Tespit Edilen / Detected: {num_hats} Hat/Hat, {num_buttons} Buton/Button")
    print("---------------------------------------------")

    if num_hats == 0:
        print("UYARI: D-Pad 'hat' olarak bulunamadi. / WARNING: D-Pad not found as hat.")

    last_cmd = "/dur"
    
    try:
        while True:
            # Olaylari isle / Process events
            pygame.event.pump()
            
            current_cmd = "/dur"
            is_analog_active = False

            # 1. YONTEM: Analog (Joystick) Kontrolu / METHOD 1: Analog (Joystick) Control
            if joystick.get_numaxes() >= 2:
                # Eksen 0: Sol-Sag (Left-Right), Eksen 1: Ileri-Geri (Forward-Backward)
                # Y ekseni genelde asagi dogru pozitiftir, bu yuzden ters ceviriyoruz
                # The Y-axis is usually positive downwards, so we invert it
                x_axis = joystick.get_axis(0)
                y_axis = -joystick.get_axis(1)

                # Deadzone (0.1) ekleyerek ufak titremeleri yoksayiyoruz
                if abs(x_axis) > 0.1 or abs(y_axis) > 0.1:
                    is_analog_active = True
                    # -1 ile 1 arasindaki degeri -100 ile 100 arasina olcekliyoruz
                    joy_x = int(x_axis * 100)
                    joy_y = int(y_axis * 100)
                    current_cmd = f"/joy?x={joy_x}&y={joy_y}"

            # 2. YONTEM: Hat (D-Pad) Kontrolu / METHOD 2: Hat (D-Pad) Control
            if not is_analog_active and num_hats > 0:
                try:
                    hat = joystick.get_hat(0)
                    if hat == (0, 1):    current_cmd = "/ileri"
                    elif hat == (0, -1): current_cmd = "/geri"
                    elif hat == (-1, 0): current_cmd = "/sol"
                    elif hat == (1, 0):  current_cmd = "/sag"
                    elif hat == (-1, 1): current_cmd = "/ilerisol"
                    elif hat == (1, 1):  current_cmd = "/ilerisag"
                    elif hat == (-1, -1): current_cmd = "/gerisol"
                    elif hat == (1, -1): current_cmd = "/gerisag"
                except pygame.error:
                    pass # Hat gecersizse butona gec / Fallback to buttons if hat fails
            
            # 3. YONTEM: Buton Kontrolu / METHOD 3: Button Control
            # D-Pad bazen buton olarak tanimlanabilir / D-Pad might be mapped as buttons
            if not is_analog_active and current_cmd == "/dur":
                if num_buttons > 14:
                    if joystick.get_button(11):   current_cmd = "/ileri"
                    elif joystick.get_button(12): current_cmd = "/geri"
                    elif joystick.get_button(13): current_cmd = "/sol"
                    elif joystick.get_button(14): current_cmd = "/sag"

            # Komut degisimi kontrolu / Command change control
            if current_cmd != last_cmd:
                print(f"[KOMUT / COMMAND] {current_cmd}")
                send_command(current_cmd)
                last_cmd = current_cmd

            time.sleep(COMMAND_INTERVAL)

    except KeyboardInterrupt:
        print("\nCikis yapiliyor... / Exiting...")
        send_command("/dur")
    finally:
        pygame.quit()

if __name__ == "__main__":
    main()
