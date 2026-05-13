# CLAUDE.md — PixMob IR Web-Controller (ESP8266)

Dieses Dokument beschreibt den aktuellen Projektstand und dient als Kontext-Übergabe
zwischen Entwicklungs-Sessions (auch auf anderen Rechnern). Einfach das Repo klonen
und Claude Code öffnen — dieser Kontext wird automatisch geladen.

---

## Projektziel

Steuerung von **PixMob IR-Armbändern** (Party-Wristbands) über eine mobile Web-UI,
die von einem **ESP8266** als WLAN Access Point bereitgestellt wird.
Kein Router, kein Internet nötig — Smartphone direkt mit dem ESP verbinden, fertig.

---

## Hardware

| Komponente | Detail |
|---|---|
| Mikrocontroller | ESP8266 — NodeMCU v3 oder Wemos D1 Mini |
| IR-Sender | KY-005 Infrarot-Modul |
| IR-Pin | **D2 / GPIO4** |
| Stromversorgung | USB-Powerbank |

**Verdrahtung KY-005:**
- `S` (Signal) → D2 (GPIO4)
- `+` (VCC)    → 3V3
- `-` (GND)    → GND

---

## Netzwerk

| Parameter | Wert |
|---|---|
| Modus | Access Point (kein Router) |
| SSID | `PixMob_Party` |
| Passwort | `partytime` |
| IP | `192.168.4.1` |

---

## Sketch-Datei

```
arduino_sender/PixMob_WebController_ESP8266/PixMob_WebController_ESP8266.ino
```

### Was der Sketch macht
- Startet einen **WLAN Access Point** (kein STA-Modus, kein Router nötig)
- Stellt eine **mobile-optimierte Web-UI** über ESPAsyncWebServer bereit
- Konvertiert PixMob-Binärarrays per **RLE → µs** und sendet sie mit `irsend.sendRaw()`
- Sendet jedes Signal **3× wiederholt** (10 ms Pause) für bessere Empfangsrate

### IR-Protokoll
- Trägerfrequenz: **38 kHz**
- Pulsbreite: **700 µs** pro Bit (patent-exakt: 694,44 µs)
- Format: Binär-Array `{0,1,1,...}` → RLE → `irsend.sendRaw(rawBuf, len, 38)`

### Implementierte Farben & Effekte

| Button | Typ | Quelle in effect_definitions.py |
|---|---|---|
| Rot | Farbe | `RED` |
| Grün | Farbe | `GREEN` |
| Blau | Farbe | `BLUE` |
| Weiß | Farbe | `WHITISH` |
| Gelb | Farbe | `YELLOW` |
| Orange | Farbe | `ORANGE` |
| Pink | Farbe | `PINK` |
| Magenta | Farbe | `MAGENTA` |
| Türkis | Farbe | `TURQUOISE` |
| Fade Rot | Effekt | `RED` + `FADE_2` (63 Bit) |
| Fade Blau | Effekt | `BLUE` + `FADE_4` (63 Bit) |
| Blink | Effekt | `GREEN` + `SHARP_PROBABILISTIC_1` (63 Bit) |
| Twinkle | Effekt | `WHITISH` + `FADE_PROBABILISTIC_1` (63 Bit) |

---

## Benötigte Arduino-Bibliotheken

| Bibliothek | Quelle | Installation |
|---|---|---|
| `IRremoteESP8266` (≥ 2.8) | crankyoldgit/IRremoteESP8266 | Arduino Bibliotheksmanager |
| `ESPAsyncWebServer` | me-no-dev/ESPAsyncWebServer | Als .ZIP |
| `ESPAsyncTCP` | me-no-dev/ESPAsyncTCP | Als .ZIP (Abhängigkeit) |
| `ESP8266WiFi` | (Teil des ESP8266-Cores) | Automatisch |

**Board Manager URL für ESP8266:**
```
https://arduino.esp8266.com/stable/package_esp8266com_index.json
```

---

## Flash-Einstellungen (Arduino IDE)

| Parameter | Wert |
|---|---|
| Board | NodeMCU 1.0 (ESP-12E) oder Wemos D1 Mini |
| Upload Speed | 115200 |
| Flash Size | 4MB (FS: 2MB) |

---

## Referenz-Dateien im Repo

| Datei | Inhalt |
|---|---|
| `python_tools/effect_definitions.py` | Alle ~60 Farb- und Tail-Codes als Python-Dict |
| `www/js/effects_definitions.js` | Dieselben Codes als JavaScript |
| `arduino_sender/PixMob_Transmitter_ESP32/` | Original ESP32-Sketch (Serial/BT) als Referenz |
| `raw_wild_ir_captures/` | Echte IR-Mitschnitte von Konzerten (Coldplay, etc.) |

---

## Nächste mögliche Schritte

- [ ] Weitere Effekte aus `tail_codes{}` ergänzen (z. B. `FADE_1`, `FADE_5`, `FADE_6`)
- [ ] Alle ~60 Farben per Dropdown statt Buttons anbieten (Speicherersparnis)
- [ ] OTA-Update-Support hinzufügen (`ArduinoOTA`)
- [ ] mDNS einrichten → `http://pixmob.local` statt IP
- [ ] RF-Protokoll (868/915 MHz) evaluieren für neuere Armbänder

---

## Testanleitung

1. ESP8266 flashen (Sketch oben)
2. Seriellen Monitor öffnen (115200 Baud) → IP-Ausgabe abwarten
3. Smartphone: WLAN `PixMob_Party` / `partytime` beitreten
4. Browser: `http://192.168.4.1`
5. Button drücken → Armband leuchtet auf
6. Optimaler Abstand: ≤ 5 m, freie Sichtlinie zur IR-LED
