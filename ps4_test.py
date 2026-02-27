# Proje: PS4 Kontrolcusu Kalibrasyon ve Test Araci
# Project: PS4 Controller Calibration and Test Tool
# Aciklama: Bagli olan oyun kolunun buton, eksen ve hat (D-pad) ID'lerini tespit eder.
# Description: Detects button, axis, and hat (D-pad) IDs of the connected controller.

import pygame
import sys

def main():
    # Pygame ve Joystick modulleri baslatiliyor / Initialize Pygame and Joystick modules
    pygame.init()
    pygame.joystick.init()

    # Bagli kontrolcu kontrolu / Connected controller check
    if pygame.joystick.get_count() == 0:
        print("\n[HATA] Bilgisayara bagli bir kol bulunamadi!")
        print("[ERROR] No controller found connected to the computer!")
        print("Lutfen USB veya Bluetooth baglantisini kontrol edin. / Please check USB or Bluetooth connection.")
        return

    # Ilk siradaki kolu al / Get the first controller
    joystick = pygame.joystick.Joystick(0)
    joystick.init()

    print(f"\n" + "="*40)
    print(f" PS4 KOL DETAYLI TEST / CONTROLLER TEST ")
    print(f"="*40)
    print(f"Cihaz Adi / Device Name: {joystick.get_name()}")
    print(f"Buton Sayisi / Button Count: {joystick.get_numbuttons()}")
    print(f"Eksen Sayisi / Axis Count: {joystick.get_numaxes()}")
    print(f"Hat Sayisi / Hat Count: {joystick.get_numhats()}")
    print("-" * 40)
    print("Yon tuslarina ve butonlara basin. / Press D-pad and buttons.")
    print("ID numaralarini not alin. / Note down the ID numbers.")
    print("Cikis icin / To Exit: Ctrl + C")
    print("-" * 40 + "\n")

    try:
        while True:
            # Olaylari (event) yakala / Capture events
            for event in pygame.event.get():
                
                # BUTON BASILMA OLAYI / BUTTON DOWN EVENT
                if event.type == pygame.JOYBUTTONDOWN:
                    print(f"[BUTON / BUTTON] ID: {event.button} - DOWN")
                
                # BUTON BIRAKILMA OLAYI / BUTTON UP EVENT
                if event.type == pygame.JOYBUTTONUP:
                    print(f"[BUTON / BUTTON] ID: {event.button} - UP")

                # HAT (D-PAD) OLAYI / HAT (D-PAD) EVENT
                if event.type == pygame.JOYHATMOTION:
                    print(f"[D-PAD / HAT] Value: {event.value}")

                # EKSEN (ANALOG) HAREKETI / AXIS (ANALOG) MOTION
                if event.type == pygame.JOYAXISMOTION:
                    # Titremeleri onlemek icin esik degeri / Threshold to prevent jitter
                    if abs(event.value) > 0.2:
                        # Eksen 0: Sol Analog X, Eksen 1: Sol Analog Y
                        # Axis 0: Left Analog X, Axis 1: Left Analog Y
                        print(f"[EKSEN / AXIS] ID: {event.axis} | Value: {event.value:+.2f}")

    except KeyboardInterrupt:
        print("\n[BILGI] Test sonlandirildi. / [INFO] Test terminated.")
    finally:
        pygame.quit()
        sys.exit()

if __name__ == "__main__":
    main()
