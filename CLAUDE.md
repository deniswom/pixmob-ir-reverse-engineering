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
| Mikrocontroller | Wemos D1 Mini (ESP8266-12F, FT232, USB Type-C, 4MB Flash) |
| IR-Sender | IR Transmitter Modul (DAT/VCC/GND) |
| IR-Sender Pin | **D2 / GPIO4** |
| IR-Empfänger | IR Receiver Modul (S/VCC/GND) |
| IR-Empfänger Pin | **D5 / GPIO14** |
| Stromversorgung | USB-Powerbank |

**Verdrahtung IR-Sender:**
- `DAT` → D2 (GPIO4)
- `VCC` → **5V** (nicht 3V3 — für maximale LED-Helligkeit)
- `GND` → G

**Verdrahtung IR-Empfänger:**
- `S` / `OUT` → D5 (GPIO14)
- `VCC` → 3V3
- `GND` → G

---

## Netzwerk

| Parameter | Wert |
|---|---|
| Modus | Access Point (kein Router) |
| SSID | `PixMob_Party` |
| Passwort | `partytime` |
| IP | `192.168.4.1` |
| mDNS | `http://pixmob.local` |

---

## Sketch-Datei

```
arduino_sender/PixMob_WebController_ESP8266/PixMob_WebController_ESP8266.ino
```

### Was der Sketch macht
- Startet einen **WLAN Access Point** (kein Router nötig)
- Stellt eine **mobile-optimierte Web-UI v2** über ESPAsyncWebServer bereit
- Erreichbar über `http://pixmob.local` (mDNS) oder `http://192.168.4.1`
- Sendet PixMob-Signale mit `irsend.sendRaw()` — **5× wiederholt** für Zuverlässigkeit
- **IR-Empfänger** liest Fernbedienungs-Codes, mappt sie auf PixMob-Effekte
- Mappings werden per **EEPROM** dauerhaft gespeichert (überleben Neustart)
- **Party-Sequenz-Engine** mit 5 Presets und konfigurierbarem Tempo

### IR-Protokoll
- Trägerfrequenz: **38 kHz**
- Pulsbreite: **700 µs** pro Bit (Patent US-10863607-B2)
- Format: Binär-Array → RLE → `irsend.sendRaw()`

### Implementierte Farben & Effekte

**24 Basisfarben** in 9 Gruppen (Tabs in der Web-UI):
Rot, Dim-Rot, Rot-Orange | Grün, Dim-Grün, Hellgrün, Gelbgrün |
Blau, Hellblau, Dim-Blau | Weiß ×3 | Gelb ×2 | Orange ×3 | Pink ×2 | Magenta ×2 | Türkis ×2

**14 Tail-Codes** (kombinierbar mit jeder Farbe):
FADE_1–6 | BLINK_1–4 | TWINKLE_1–4

**5 Party-Sequenz-Presets:**
Regenbogen · Warm · Kühl · Disco · Weiß

**13 Fernbedienungs-Slots** (per Web-UI einlernbar, EEPROM-persistent)

---

## Benötigte Arduino-Bibliotheken

| Bibliothek | Quelle | Installation |
|---|---|---|
| `IRremoteESP8266` (≥ 2.8) | crankyoldgit/IRremoteESP8266 | Arduino Bibliotheksmanager |
| `ESPAsyncWebServer` | me-no-dev/ESPAsyncWebServer | Als .ZIP |
| `ESPAsyncTCP` | me-no-dev/ESPAsyncTCP | Als .ZIP (Abhängigkeit) |
| `ESP8266WiFi` | (Teil des ESP8266-Cores) | Automatisch |
| `ESP8266mDNS` | (Teil des ESP8266-Cores) | Automatisch |

**Board Manager URL für ESP8266:**
```
https://arduino.esp8266.com/stable/package_esp8266com_index.json
```

---

## Flash-Einstellungen (arduino-cli)

```bash
# Kompilieren
arduino-cli compile --fqbn esp8266:esp8266:d1_mini \
  --libraries "$HOME/Documents/Arduino/libraries" \
  arduino_sender/PixMob_WebController_ESP8266/

# Flashen (Port ggf. anpassen)
arduino-cli upload --fqbn esp8266:esp8266:d1_mini \
  --port /dev/cu.usbserial-A5069RR4 \
  arduino_sender/PixMob_WebController_ESP8266/
```

**Hinweis Mac/Apple Silicon:** ESP8266-Toolchain ist x86 → Rosetta 2 erforderlich.
Python-Wrapper nötig unter:
`~/Library/Arduino15/packages/esp8266/tools/python3/3.7.2-post1/python3`

---

## Web-UI Endpoints

| Endpoint | Funktion |
|---|---|
| `GET /` | Web-UI |
| `GET /cmd?c=RED&t=FADE_2` | Farbe + optionaler Tail senden |
| `GET /seq/start?preset=rainbow&interval=800` | Sequenz starten |
| `GET /seq/stop` | Sequenz stoppen |
| `GET /learn?slot=0` | Lernmodus für Slot starten |
| `GET /learnstatus` | Lernstatus abfragen (JSON) |
| `GET /clearmappings` | Alle Fernbedienungs-Mappings löschen |

---

## Referenz-Dateien im Repo

| Datei | Inhalt |
|---|---|
| `python_tools/effect_definitions.py` | Alle ~74 Farb- und Tail-Codes als Python-Dict |
| `www/js/effects_definitions.js` | Dieselben Codes als JavaScript |
| `media/IR_Sender_KY005_Module.jpg` | Foto des IR-Sender-Moduls |
| `case_3d/PixMob_D1Mini_Case.scad` | Parametrisches 3D-Gehäuse (OpenSCAD) |
| `raw_wild_ir_captures/` | Echte IR-Mitschnitte von Konzerten (Coldplay, etc.) |

---

## Nächste mögliche Schritte

- [ ] 3D-Gehäuse drucken (SCAD-Datei vorhanden, Empfänger-Loch noch ergänzen)
- [ ] OTA-Update-Support (`ArduinoOTA`)
- [ ] RF-Protokoll (868/915 MHz) für neuere Armbänder evaluieren
- [ ] Eigene Custom-Sequenz per Web-UI definierbar machen
