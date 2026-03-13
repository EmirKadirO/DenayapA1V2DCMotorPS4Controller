import pygame
import requests
import time
import sys

# --- AYARLAR ---
ROBOT_IP = "http://192.168.4.1"
COMMAND_INTERVAL = 0.05 

# Eğer test scriptinde farklı numaralar bulursan burayı güncelle:
BTN_UP = 11
BTN_DOWN = 12
BTN_LEFT = 13
BTN_RIGHT = 14
# ---------------

def send_command(path):
    try:
        requests.get(ROBOT_IP + path, timeout=0.1)
        return True
    except:
        return False

def main():
    pygame.init()
    pygame.joystick.init()

    if pygame.joystick.get_count() == 0:
        print("HATA: PS4 Kolu bulunamadı!")
        pygame.quit()
        sys.exit()

    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    
    print(f"\n--- vira.py: PS4 Bridge Aktif ---")
    print(f"Cihaz: {joystick.get_name()}")
    print("---------------------------------")

    last_cmd = "/dur"
    
    try:
        while True:
            pygame.event.pump()
            current_cmd = "/dur"

            # 1. Analog Çubuk Kontrolü (Sol Analog - Sol/Sağ ve İleri/Geri)
            # Genellikle Axis 0 (Yatay) ve Axis 1 (Dikey)
            ax_x = joystick.get_axis(0)
            ax_y = joystick.get_axis(1)

            if ax_y < -0.5: current_cmd = "/ileri"
            elif ax_y > 0.5: current_cmd = "/geri"
            elif ax_x < -0.5: current_cmd = "/sol"
            elif ax_x > 0.5: current_cmd = "/sag"

            # 2. Buton Kontrolü (D-Pad buton olarak tanımlıysa)
            if current_cmd == "/dur":
                if joystick.get_button(BTN_UP): current_cmd = "/ileri"
                elif joystick.get_button(BTN_DOWN): current_cmd = "/geri"
                elif joystick.get_button(BTN_LEFT): current_cmd = "/sol"
                elif joystick.get_button(BTN_RIGHT): current_cmd = "/sag"

            # Komut Değişimi
            if current_cmd != last_cmd:
                print(f"Aksiyon: {current_cmd}")
                send_command(current_cmd)
                last_cmd = current_cmd

            time.sleep(COMMAND_INTERVAL)

    except KeyboardInterrupt:
        send_command("/dur")
        print("\nDurduruldu.")
    finally:
        pygame.quit()

if __name__ == "__main__":
    main()
