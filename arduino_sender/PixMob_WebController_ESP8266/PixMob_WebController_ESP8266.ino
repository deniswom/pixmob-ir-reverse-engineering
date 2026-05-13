// =============================================================================
//  PixMob IR Web-Controller
//  Zielplattform : ESP8266 (NodeMCU v3 / Wemos D1 Mini)
//  IR-Sender     : KY-005 an D2 / GPIO4
//
//  Quelle der Signalcodes:
//    github.com/danielweidman/pixmob-ir-reverse-engineering
//    python_tools/effect_definitions.py
//
//  Bibliotheken (Arduino IDE):
//    - IRremoteESP8266  (crankyoldgit) >= 2.8
//    - ESPAsyncWebServer (me-no-dev)   + ESPAsyncTCP (me-no-dev) als .ZIP
//
//  Netzwerk:
//    SSID     : PixMob_Party
//    Passwort : partytime
//    IP       : 192.168.4.1
// =============================================================================

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// ---------------------------------------------------------------------------
// Konfiguration
// ---------------------------------------------------------------------------
const char*    AP_SSID     = "PixMob_Party";
const char*    AP_PASSWORD = "partytime";
const uint16_t IR_PIN      = 4;    // D2 = GPIO4
const uint16_t PULSE_US    = 700;  // 694,44 µs laut Patent US-10863607-B2
const uint8_t  IR_KHZ      = 38;   // Trägerfrequenz 38 kHz
const uint8_t  TX_REPEAT   = 3;    // Sendewiederholungen für Zuverlässigkeit

IRsend         irsend(IR_PIN);
AsyncWebServer server(80);

// ---------------------------------------------------------------------------
// PixMob-Signale als Binär-Arrays
// Protokoll: 1 = Puls (Mark), 0 = Pause (Space), je PULSE_US Mikrosekunden
// Quelle: effect_definitions.py / effects_definitions.js im Repo
// ---------------------------------------------------------------------------

// --- Farben ---
const uint8_t SIG_RED[]      = {1,1,0,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_GREEN[]    = {1,1,0,0,1,0,1,0,1,1,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_BLUE[]     = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,1,0,1,0,0,0,1};
const uint8_t SIG_WHITE[]    = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0,1,0,0,1};
const uint8_t SIG_YELLOW[]   = {1,1,0,0,1,0,1,0,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_ORANGE[]   = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_PINK[]     = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,1,0,0,1,0,0,0,1,0,1,0,0,0,1};
const uint8_t SIG_MAGENTA[]  = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,1,0,1,1,0,0,0,1,0,1,0,0,0,1};
const uint8_t SIG_TURQUOISE[]= {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,1,0,1,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,0,0,1};

// --- Effekte: Basisfarbe (39 Bit) + Tail-Code (24 Bit) = 63 Bit ---
// Fade In+Out auf Rot (RED + FADE_2)
const uint8_t SIG_FADE_RED[] = {
  1,1,0,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,  // RED
  0,0,1,0,0,1,1,0,1,0,1,0,1,0,0,0,0,1,1,0,0,0,0,1                                  // FADE_2
};

// Slow Fade Out auf Blau (BLUE + FADE_4)
const uint8_t SIG_FADE_BLUE[] = {
  1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,1,0,1,0,0,0,1,  // BLUE
  0,0,0,0,1,1,0,0,1,0,0,0,1,0,1,0,0,1,1,0,0,0,0,1                                  // FADE_4
};

// Probabilistischer Blink Grün (GREEN + SHARP_PROBABILISTIC_1) — nur manche Armbänder leuchten
const uint8_t SIG_BLINK_GREEN[] = {
  1,1,0,0,1,0,1,0,1,1,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,  // GREEN
  0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1                                  // SHARP_PROBABILISTIC_1
};

// Twinkling Weiß — probabilistisch (WHITE + FADE_PROBABILISTIC_1)
const uint8_t SIG_TWINKLE[] = {
  1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0,1,0,0,1,  // WHITE
  0,0,0,1,0,0,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1                                  // FADE_PROBABILISTIC_1
};

// ---------------------------------------------------------------------------
// Binär-Array → Raw-IR-Timing Konverter
// Aufeinanderfolgende gleiche Bits werden per RLE zu µs-Dauern zusammengefasst
// ---------------------------------------------------------------------------
uint16_t rawBuf[160];
uint16_t rawLen;

void buildRaw(const uint8_t* sig, uint16_t len) {
  rawLen = 0;
  uint16_t i = 0;
  while (i < len && rawLen < 158) {
    uint8_t  bit   = sig[i];
    uint16_t count = 0;
    while (i < len && sig[i] == bit) { count++; i++; }
    rawBuf[rawLen++] = (uint16_t)(count * PULSE_US);
  }
}

// Signal TX_REPEAT-mal senden (Wiederholung erhöht die Empfangswahrscheinlichkeit)
void sendSignal(const uint8_t* sig, uint16_t len) {
  buildRaw(sig, len);
  for (uint8_t r = 0; r < TX_REPEAT; r++) {
    irsend.sendRaw(rawBuf, rawLen, IR_KHZ);
    if (r < TX_REPEAT - 1) delay(10);
  }
}

// ---------------------------------------------------------------------------
// Web-UI  (in PROGMEM — spart kostbares DRAM des ESP8266)
// ---------------------------------------------------------------------------
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">
  <title>PixMob Controller</title>
  <style>
    *,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
    body{
      font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;
      background:#0d0d1a;color:#f0f0f0;min-height:100vh;
      display:flex;flex-direction:column;align-items:center;padding:28px 16px 40px;
    }
    h1{
      font-size:1.75rem;letter-spacing:3px;text-transform:uppercase;margin-bottom:4px;
      background:linear-gradient(135deg,#ff6ec7,#7b61ff,#00d4ff);
      -webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;
    }
    .sub{font-size:.72rem;color:#555;letter-spacing:2px;margin-bottom:30px}
    .label{
      font-size:.65rem;letter-spacing:3px;text-transform:uppercase;
      color:#444;margin-bottom:10px;width:100%;max-width:360px;
      padding-left:2px;border-left:2px solid #333;padding-left:8px;
    }
    .grid{
      display:grid;grid-template-columns:1fr 1fr;gap:11px;
      width:100%;max-width:360px;margin-bottom:22px;
    }
    .btn{
      border:none;border-radius:18px;padding:20px 8px 18px;
      font-size:.9rem;font-weight:700;letter-spacing:.5px;
      cursor:pointer;display:flex;flex-direction:column;
      align-items:center;gap:6px;color:#fff;
      text-shadow:0 1px 3px rgba(0,0,0,.4);
      transition:transform .12s,opacity .12s;
      -webkit-tap-highlight-color:transparent;user-select:none;
    }
    .btn:active{transform:scale(.91);opacity:.75}
    .ic{font-size:1.65rem;line-height:1}
    /* Farb-Buttons */
    .c-red    {background:linear-gradient(145deg,#ff2828,#aa0000);box-shadow:0 4px 20px rgba(255,40,40,.4)}
    .c-green  {background:linear-gradient(145deg,#1ddd60,#007730);box-shadow:0 4px 20px rgba(30,220,80,.35);color:#003}
    .c-blue   {background:linear-gradient(145deg,#2266ff,#0030cc);box-shadow:0 4px 20px rgba(30,100,255,.45)}
    .c-white  {background:linear-gradient(145deg,#ffffff,#c0c0c0);box-shadow:0 4px 20px rgba(255,255,255,.2);color:#222;text-shadow:none}
    .c-yellow {background:linear-gradient(145deg,#ffe600,#cc9900);box-shadow:0 4px 20px rgba(255,210,0,.4);color:#1a0}
    .c-orange {background:linear-gradient(145deg,#ff8800,#cc4400);box-shadow:0 4px 20px rgba(255,120,0,.4)}
    .c-pink   {background:linear-gradient(145deg,#ff55bb,#cc0077);box-shadow:0 4px 20px rgba(255,80,180,.4)}
    .c-magenta{background:linear-gradient(145deg,#ee00cc,#880088);box-shadow:0 4px 20px rgba(220,0,200,.4)}
    .c-turq   {background:linear-gradient(145deg,#00ddcc,#007766);box-shadow:0 4px 20px rgba(0,200,180,.4);color:#002}
    /* Effekt-Buttons — volle Breite im 1-Spalten-Grid */
    .efx-grid{
      display:grid;grid-template-columns:1fr 1fr;gap:11px;
      width:100%;max-width:360px;margin-bottom:22px;
    }
    .c-fade-r {background:linear-gradient(145deg,#ff6ec7,#cc0000);box-shadow:0 4px 20px rgba(200,50,150,.5)}
    .c-fade-b {background:linear-gradient(145deg,#4488ff,#7b61ff);box-shadow:0 4px 20px rgba(100,100,255,.5)}
    .c-blink  {background:linear-gradient(145deg,#44ff88,#007730);box-shadow:0 4px 20px rgba(30,220,100,.4);color:#002}
    .c-twinkle{background:linear-gradient(145deg,#ffffff,#8888ff);box-shadow:0 4px 20px rgba(200,200,255,.3);color:#222;text-shadow:none}
    /* Status */
    #st{font-size:.72rem;color:#444;margin-top:6px;min-height:20px;
        transition:color .3s;text-align:center}
    #st.ok {color:#00cc66}
    #st.err{color:#ff5555}
  </style>
</head>
<body>
  <h1>&#9733; PixMob</h1>
  <p class="sub">IR PARTY CONTROLLER</p>

  <p class="label">Farben</p>
  <div class="grid">
    <button class="btn c-red"     onclick="s('red')">    <span class="ic">&#11044;</span>Rot</button>
    <button class="btn c-green"   onclick="s('green')">  <span class="ic">&#11044;</span>Gr&uuml;n</button>
    <button class="btn c-blue"    onclick="s('blue')">   <span class="ic">&#11044;</span>Blau</button>
    <button class="btn c-white"   onclick="s('white')">  <span class="ic">&#11044;</span>Wei&szlig;</button>
    <button class="btn c-yellow"  onclick="s('yellow')"> <span class="ic">&#11044;</span>Gelb</button>
    <button class="btn c-orange"  onclick="s('orange')"> <span class="ic">&#11044;</span>Orange</button>
    <button class="btn c-pink"    onclick="s('pink')">   <span class="ic">&#11044;</span>Pink</button>
    <button class="btn c-magenta" onclick="s('magenta')"><span class="ic">&#11044;</span>Magenta</button>
    <button class="btn c-turq"    onclick="s('turq')">   <span class="ic">&#11044;</span>Türkis</button>
  </div>

  <p class="label">Effekte</p>
  <div class="efx-grid">
    <button class="btn c-fade-r"  onclick="s('fade_red')">  <span class="ic">&#10024;</span>Fade Rot</button>
    <button class="btn c-fade-b"  onclick="s('fade_blue')"> <span class="ic">&#10024;</span>Fade Blau</button>
    <button class="btn c-blink"   onclick="s('blink')">     <span class="ic">&#9889;</span>Blink</button>
    <button class="btn c-twinkle" onclick="s('twinkle')">   <span class="ic">&#10022;</span>Twinkle</button>
  </div>

  <p id="st">Bereit &#128308;</p>

  <script>
    const st = document.getElementById('st');
    const labels = {
      red:'Rot', green:'Grün', blue:'Blau', white:'Weiß',
      yellow:'Gelb', orange:'Orange', pink:'Pink',
      magenta:'Magenta', turq:'Türkis',
      fade_red:'Fade Rot', fade_blue:'Fade Blau',
      blink:'Blink', twinkle:'Twinkle'
    };
    function s(c) {
      st.className = '';
      st.textContent = 'Sende ' + (labels[c] || c) + ' …';
      fetch('/cmd?c=' + c)
        .then(r => r.ok ? r.text() : Promise.reject(r.status))
        .then(t => { st.textContent = '✓ ' + t; st.className = 'ok'; })
        .catch(e => { st.textContent = 'Fehler (' + e + ')'; st.className = 'err'; });
    }
  </script>
</body>
</html>
)rawliteral";

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  irsend.begin();

  // Access Point starten (kein Router nötig)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.printf("\n[PixMob] AP gestartet\n  SSID    : %s\n  Passwort: %s\n  IP      : %s\n",
                AP_SSID, AP_PASSWORD, WiFi.softAPIP().toString().c_str());

  // --- Routen ---
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", HTML_PAGE);
  });

  server.on("/cmd", HTTP_GET, [](AsyncWebServerRequest* req) {
    if (!req->hasParam("c")) {
      req->send(400, "text/plain", "Fehlender Parameter 'c'");
      return;
    }
    String cmd = req->getParam("c")->value();
    cmd.toLowerCase();

    // Farben
    if      (cmd == "red")     sendSignal(SIG_RED,         sizeof(SIG_RED));
    else if (cmd == "green")   sendSignal(SIG_GREEN,       sizeof(SIG_GREEN));
    else if (cmd == "blue")    sendSignal(SIG_BLUE,        sizeof(SIG_BLUE));
    else if (cmd == "white")   sendSignal(SIG_WHITE,       sizeof(SIG_WHITE));
    else if (cmd == "yellow")  sendSignal(SIG_YELLOW,      sizeof(SIG_YELLOW));
    else if (cmd == "orange")  sendSignal(SIG_ORANGE,      sizeof(SIG_ORANGE));
    else if (cmd == "pink")    sendSignal(SIG_PINK,        sizeof(SIG_PINK));
    else if (cmd == "magenta") sendSignal(SIG_MAGENTA,     sizeof(SIG_MAGENTA));
    else if (cmd == "turq")    sendSignal(SIG_TURQUOISE,   sizeof(SIG_TURQUOISE));
    // Effekte
    else if (cmd == "fade_red")  sendSignal(SIG_FADE_RED,    sizeof(SIG_FADE_RED));
    else if (cmd == "fade_blue") sendSignal(SIG_FADE_BLUE,   sizeof(SIG_FADE_BLUE));
    else if (cmd == "blink")     sendSignal(SIG_BLINK_GREEN, sizeof(SIG_BLINK_GREEN));
    else if (cmd == "twinkle")   sendSignal(SIG_TWINKLE,     sizeof(SIG_TWINKLE));
    else {
      req->send(400, "text/plain", "Unbekannter Befehl: " + cmd);
      return;
    }
    req->send(200, "text/plain", cmd);
    Serial.printf("[IR] Gesendet: %s\n", cmd.c_str());
  });

  server.onNotFound([](AsyncWebServerRequest* req) {
    req->send(404, "text/plain", "Nicht gefunden");
  });

  server.begin();
  Serial.println("[PixMob] Webserver laeuft — http://192.168.4.1");
}

// ---------------------------------------------------------------------------
// Loop — ESPAsyncWebServer ist vollständig interrupt-gesteuert, kein Polling
// ---------------------------------------------------------------------------
void loop() {}
