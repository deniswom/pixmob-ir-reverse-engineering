// =============================================================================
//  PixMob IR Web-Controller v2
//  Wemos D1 Mini | IR-Sender D2/GPIO4 | IR-Empfänger D5/GPIO14
//  74 Farben · 14 Effekte · Party-Sequenzen · Fernbedienung · EEPROM
// =============================================================================

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <EEPROM.h>

// ---------------------------------------------------------------------------
// Konfiguration
// ---------------------------------------------------------------------------
const char*    AP_SSID          = "PixMob_Party";
const char*    AP_PASSWORD      = "partytime";
const uint16_t IR_SEND_PIN      = 4;
const uint16_t IR_RECV_PIN      = 14;
const uint16_t PULSE_US         = 700;
const uint8_t  IR_KHZ           = 38;
const uint8_t  TX_REPEAT        = 5;
const uint32_t LEARN_TIMEOUT_MS = 10000;

IRsend         irsend(IR_SEND_PIN);
IRrecv         irrecv(IR_RECV_PIN);
decode_results irData;
AsyncWebServer server(80);

// ---------------------------------------------------------------------------
// Signal-Arrays — Farben (39 Bit)
// ---------------------------------------------------------------------------
const uint8_t SIG_RED[]          = {1,1,0,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_DIM_RED[]      = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,1,1,0,0,0,0,1};
const uint8_t SIG_REDORANGE[]    = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,0,1,1,0,0,1,0,1,0,0,1,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_GREEN[]        = {1,1,0,0,1,0,1,0,1,1,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_GREEN_DIM[]    = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_LIGHT_GREEN[]  = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,1};
const uint8_t SIG_YELLOWGREEN[]  = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,0,1,0,1,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_BLUE[]         = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,1,0,1,0,0,0,1};
const uint8_t SIG_LIGHT_BLUE[]   = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,1,0,1,1,0,0,1,0,0,0,1};
const uint8_t SIG_DIM_BLUE[]     = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,1,0,0,1,0,0,1};
const uint8_t SIG_WHITISH[]      = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0,1,0,0,1};
const uint8_t SIG_WHITISH_2[]    = {1,0,1,0,0,0,1,1,0,0,1,0,0,0,1,0,0,1,0,1,0,0,0,0,1,1,0,0,1,0,0,0,1,0,1,0,0,0,1};
const uint8_t SIG_WHITISH_3[]    = {1,1,0,0,1,0,1,0,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,0,0,1};
const uint8_t SIG_YELLOW[]       = {1,1,0,0,1,0,1,0,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_YELLOW_3[]     = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_ORANGE[]       = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_ORANGE_2[]     = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,0,1,1,0,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_YELLOWORANGE[] = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,1,0,1,0,0,0,1,0,1,0,1,1,0,0,0,1,1,0,0,0,0,1};
const uint8_t SIG_PINK[]         = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,1,0,0,1,0,0,0,1,0,1,0,0,0,1};
const uint8_t SIG_PINK_2[]       = {1,0,1,0,0,0,1,1,0,1,0,0,0,0,1,0,0,0,1,1,0,0,0,0,1,1,0,0,1,0,0,0,1,0,1,0,0,0,1};
const uint8_t SIG_MAGENTA[]      = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,1,0,1,1,0,0,0,1,0,1,0,0,0,1};
const uint8_t SIG_MAGENTA_2[]    = {1,0,1,0,0,0,1,1,0,0,1,1,0,1,1,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,0,0,1,0,0,1,1,0,1};
const uint8_t SIG_TURQUOISE[]    = {1,0,1,0,1,1,0,0,1,1,0,0,0,0,1,0,0,1,0,1,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,0,0,1};
const uint8_t SIG_TURQUOISE_2[]  = {1,0,1,0,0,0,1,1,0,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,1,0,1};

// ---------------------------------------------------------------------------
// Tail-Codes (24 Bit) — anhängen an Basisfarbe für Effekte
// ---------------------------------------------------------------------------
const uint8_t TAIL_FADE_1[]              = {0,0,0,1,0,1,1,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_FADE_2[]              = {0,0,1,0,0,1,1,0,1,0,1,0,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_FADE_3[]              = {0,0,0,1,0,1,1,0,1,0,1,0,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_FADE_4[]              = {0,0,0,0,1,1,0,0,1,0,0,0,1,0,1,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_FADE_5[]              = {0,0,0,1,0,1,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_FADE_6[]              = {0,0,1,0,0,1,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_SHARP_PROB_1[]        = {0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_SHARP_PROB_2[]        = {0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_SHARP_PROB_3[]        = {0,0,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_SHARP_PROB_4[]        = {0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_FADE_PROB_1[]         = {0,0,0,1,0,0,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_FADE_PROB_2[]         = {0,0,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_FADE_PROB_3[]         = {0,0,1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1};
const uint8_t TAIL_FADE_PROB_4[]         = {0,0,0,1,0,1,0,0,1,0,0,0,1,0,1,0,0,1,1,0,0,0,0,1};

// ---------------------------------------------------------------------------
// Lookup-Tabellen
// ---------------------------------------------------------------------------
struct ColorDef { const char* name; const uint8_t* sig; uint16_t len; };
struct TailDef  { const char* name; const uint8_t* sig; uint16_t len; };

const ColorDef COLORS[] = {
  {"RED",         SIG_RED,         sizeof(SIG_RED)},
  {"DIM_RED",     SIG_DIM_RED,     sizeof(SIG_DIM_RED)},
  {"REDORANGE",   SIG_REDORANGE,   sizeof(SIG_REDORANGE)},
  {"GREEN",       SIG_GREEN,       sizeof(SIG_GREEN)},
  {"GREEN_DIM",   SIG_GREEN_DIM,   sizeof(SIG_GREEN_DIM)},
  {"LIGHT_GREEN", SIG_LIGHT_GREEN, sizeof(SIG_LIGHT_GREEN)},
  {"YELLOWGREEN", SIG_YELLOWGREEN, sizeof(SIG_YELLOWGREEN)},
  {"BLUE",        SIG_BLUE,        sizeof(SIG_BLUE)},
  {"LIGHT_BLUE",  SIG_LIGHT_BLUE,  sizeof(SIG_LIGHT_BLUE)},
  {"DIM_BLUE",    SIG_DIM_BLUE,    sizeof(SIG_DIM_BLUE)},
  {"WHITISH",     SIG_WHITISH,     sizeof(SIG_WHITISH)},
  {"WHITISH_2",   SIG_WHITISH_2,   sizeof(SIG_WHITISH_2)},
  {"WHITISH_3",   SIG_WHITISH_3,   sizeof(SIG_WHITISH_3)},
  {"YELLOW",      SIG_YELLOW,      sizeof(SIG_YELLOW)},
  {"YELLOW_3",    SIG_YELLOW_3,    sizeof(SIG_YELLOW_3)},
  {"ORANGE",      SIG_ORANGE,      sizeof(SIG_ORANGE)},
  {"ORANGE_2",    SIG_ORANGE_2,    sizeof(SIG_ORANGE_2)},
  {"YELLOWORANGE",SIG_YELLOWORANGE,sizeof(SIG_YELLOWORANGE)},
  {"PINK",        SIG_PINK,        sizeof(SIG_PINK)},
  {"PINK_2",      SIG_PINK_2,      sizeof(SIG_PINK_2)},
  {"MAGENTA",     SIG_MAGENTA,     sizeof(SIG_MAGENTA)},
  {"MAGENTA_2",   SIG_MAGENTA_2,   sizeof(SIG_MAGENTA_2)},
  {"TURQUOISE",   SIG_TURQUOISE,   sizeof(SIG_TURQUOISE)},
  {"TURQUOISE_2", SIG_TURQUOISE_2, sizeof(SIG_TURQUOISE_2)},
};
const uint8_t COLOR_COUNT = sizeof(COLORS) / sizeof(COLORS[0]);

const TailDef TAILS[] = {
  {"NONE",         nullptr, 0},
  {"FADE_1",       TAIL_FADE_1,       sizeof(TAIL_FADE_1)},
  {"FADE_2",       TAIL_FADE_2,       sizeof(TAIL_FADE_2)},
  {"FADE_3",       TAIL_FADE_3,       sizeof(TAIL_FADE_3)},
  {"FADE_4",       TAIL_FADE_4,       sizeof(TAIL_FADE_4)},
  {"FADE_5",       TAIL_FADE_5,       sizeof(TAIL_FADE_5)},
  {"FADE_6",       TAIL_FADE_6,       sizeof(TAIL_FADE_6)},
  {"BLINK_1",      TAIL_SHARP_PROB_1, sizeof(TAIL_SHARP_PROB_1)},
  {"BLINK_2",      TAIL_SHARP_PROB_2, sizeof(TAIL_SHARP_PROB_2)},
  {"BLINK_3",      TAIL_SHARP_PROB_3, sizeof(TAIL_SHARP_PROB_3)},
  {"BLINK_4",      TAIL_SHARP_PROB_4, sizeof(TAIL_SHARP_PROB_4)},
  {"TWINKLE_1",    TAIL_FADE_PROB_1,  sizeof(TAIL_FADE_PROB_1)},
  {"TWINKLE_2",    TAIL_FADE_PROB_2,  sizeof(TAIL_FADE_PROB_2)},
  {"TWINKLE_3",    TAIL_FADE_PROB_3,  sizeof(TAIL_FADE_PROB_3)},
  {"TWINKLE_4",    TAIL_FADE_PROB_4,  sizeof(TAIL_FADE_PROB_4)},
};
const uint8_t TAIL_COUNT = sizeof(TAILS) / sizeof(TAILS[0]);

// ---------------------------------------------------------------------------
// IR-Senden
// ---------------------------------------------------------------------------
uint16_t rawBuf[160];
uint16_t rawLen;
uint8_t  combBuf[64];

void buildRaw(const uint8_t* sig, uint16_t len) {
  rawLen = 0;
  uint16_t i = 0;
  while (i < len && rawLen < 158) {
    uint8_t bit = sig[i]; uint16_t count = 0;
    while (i < len && sig[i] == bit) { count++; i++; }
    rawBuf[rawLen++] = (uint16_t)(count * PULSE_US);
  }
}

void sendSignal(const uint8_t* sig, uint16_t len) {
  irrecv.pause();
  buildRaw(sig, len);
  for (uint8_t r = 0; r < TX_REPEAT; r++) {
    irsend.sendRaw(rawBuf, rawLen, IR_KHZ);
    if (r < TX_REPEAT - 1) delay(10);
  }
  irrecv.resume();
}

bool lookupAndSend(const String& colorName, const String& tailName) {
  const uint8_t* colorSig = nullptr;
  uint16_t colorLen = 0;
  for (uint8_t i = 0; i < COLOR_COUNT; i++) {
    if (colorName == COLORS[i].name) { colorSig = COLORS[i].sig; colorLen = COLORS[i].len; break; }
  }
  if (!colorSig) return false;

  if (tailName.length() == 0 || tailName == "NONE") {
    sendSignal(colorSig, colorLen);
  } else {
    for (uint8_t i = 0; i < TAIL_COUNT; i++) {
      if (tailName == TAILS[i].name && TAILS[i].sig) {
        memcpy(combBuf, colorSig, colorLen);
        memcpy(combBuf + colorLen, TAILS[i].sig, TAILS[i].len);
        sendSignal(combBuf, colorLen + TAILS[i].len);
        break;
      }
    }
  }
  Serial.printf("[IR] %s + %s\n", colorName.c_str(), tailName.c_str());
  return true;
}

// ---------------------------------------------------------------------------
// Party-Sequenz
// ---------------------------------------------------------------------------
const uint8_t MAX_SEQ = 20;
struct SeqEntry { char color[20]; char tail[16]; };
SeqEntry   seq[MAX_SEQ];
uint8_t    seqLen      = 0;
uint8_t    seqIdx      = 0;
uint32_t   seqInterval = 1000;
uint32_t   seqLastFire = 0;
bool       seqRunning  = false;

// Vordefinierte Presets
void loadPreset(const String& name, uint32_t interval) {
  seqLen = 0; seqIdx = 0; seqInterval = interval; seqRunning = false;

  auto add = [](const char* c, const char* t = "FADE_2") {
    if (seqLen >= MAX_SEQ) return;
    strncpy(seq[seqLen].color, c, 19);
    strncpy(seq[seqLen].tail,  t, 15);
    seqLen++;
  };

  if (name == "rainbow") {
    add("RED"); add("REDORANGE"); add("ORANGE"); add("YELLOWORANGE");
    add("YELLOW"); add("YELLOWGREEN"); add("GREEN"); add("TURQUOISE");
    add("LIGHT_BLUE"); add("BLUE"); add("MAGENTA"); add("PINK");
  } else if (name == "warm") {
    add("RED"); add("REDORANGE"); add("ORANGE"); add("YELLOWORANGE"); add("YELLOW");
  } else if (name == "cool") {
    add("BLUE"); add("LIGHT_BLUE"); add("TURQUOISE"); add("LIGHT_GREEN"); add("GREEN");
  } else if (name == "disco") {
    add("RED","BLINK_1"); add("GREEN","BLINK_1"); add("BLUE","BLINK_1");
    add("YELLOW","BLINK_2"); add("MAGENTA","BLINK_2"); add("TURQUOISE","BLINK_2");
  } else if (name == "white") {
    add("WHITISH","TWINKLE_1"); add("WHITISH_2","TWINKLE_2"); add("WHITISH_3","TWINKLE_3");
  }
}

// ---------------------------------------------------------------------------
// EEPROM — Fernbedienungs-Mappings
// ---------------------------------------------------------------------------
struct Mapping { uint64_t code; char color[20]; char tail[16]; bool learned; };
const uint8_t MAP_COUNT = 13;
Mapping mappings[MAP_COUNT] = {
  {0,"RED",      "NONE",false},{0,"GREEN",    "NONE",false},{0,"BLUE",       "NONE",false},
  {0,"WHITISH",  "NONE",false},{0,"YELLOW",   "NONE",false},{0,"ORANGE",     "NONE",false},
  {0,"PINK",     "NONE",false},{0,"MAGENTA",  "NONE",false},{0,"TURQUOISE",  "NONE",false},
  {0,"RED",      "FADE_2",false},{0,"BLUE",   "FADE_4",false},
  {0,"GREEN",    "BLINK_1",false},{0,"WHITISH","TWINKLE_1",false},
};

const uint16_t EEPROM_MAGIC = 0x504D;
const uint16_t EEPROM_ENTRY = sizeof(uint64_t) + 20 + 16 + 1;
const uint16_t EEPROM_SIZE  = 2 + MAP_COUNT * EEPROM_ENTRY;

void eepromSave() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(0, EEPROM_MAGIC);
  for (uint8_t i = 0; i < MAP_COUNT; i++) {
    uint16_t a = 2 + i * EEPROM_ENTRY;
    EEPROM.put(a, mappings[i].code);
    for (uint8_t j = 0; j < 20; j++) EEPROM.write(a+8+j,  mappings[i].color[j]);
    for (uint8_t j = 0; j < 16; j++) EEPROM.write(a+28+j, mappings[i].tail[j]);
    EEPROM.write(a+44, mappings[i].learned ? 1 : 0);
  }
  EEPROM.commit(); EEPROM.end();
}

void eepromLoad() {
  EEPROM.begin(EEPROM_SIZE);
  uint16_t magic; EEPROM.get(0, magic);
  if (magic == EEPROM_MAGIC) {
    for (uint8_t i = 0; i < MAP_COUNT; i++) {
      uint16_t a = 2 + i * EEPROM_ENTRY;
      EEPROM.get(a, mappings[i].code);
      for (uint8_t j = 0; j < 20; j++) mappings[i].color[j] = EEPROM.read(a+8+j);
      for (uint8_t j = 0; j < 16; j++) mappings[i].tail[j]  = EEPROM.read(a+28+j);
      mappings[i].learned = EEPROM.read(a+44) == 1;
    }
    Serial.println("[EEPROM] Mappings geladen");
  }
  EEPROM.end();
}

void eepromClear() {
  for (uint8_t i = 0; i < MAP_COUNT; i++) { mappings[i].code = 0; mappings[i].learned = false; }
  eepromSave();
}

// Lernzustand
volatile bool learnActive  = false;
String        learnTarget  = "";
uint32_t      learnStart   = 0;
volatile bool learnSuccess = false;

// ---------------------------------------------------------------------------
// Web-UI
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
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;
  background:#080812;color:#f0f0f0;min-height:100vh;
  display:flex;flex-direction:column;align-items:center;padding:28px 14px 48px;}

/* Header */
.logo{font-size:2rem;letter-spacing:4px;text-transform:uppercase;font-weight:900;
  background:linear-gradient(135deg,#ff6ec7 0%,#7b61ff 50%,#00d4ff 100%);
  -webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;
  margin-bottom:2px;}
.sub{font-size:.65rem;color:#444;letter-spacing:3px;margin-bottom:28px;text-transform:uppercase;}

/* Cards */
.card{width:100%;max-width:390px;
  background:rgba(255,255,255,.04);
  border:1px solid rgba(255,255,255,.08);
  border-radius:20px;padding:18px;margin-bottom:12px;
  backdrop-filter:blur(8px);}
.card-title{font-size:.6rem;letter-spacing:3px;text-transform:uppercase;
  color:#555;margin-bottom:14px;display:flex;align-items:center;gap:8px;}
.card-title::after{content:'';flex:1;height:1px;background:rgba(255,255,255,.06);}

/* Farb-Tabs */
.tabs{display:flex;gap:6px;overflow-x:auto;padding-bottom:2px;
  scrollbar-width:none;margin-bottom:12px;}
.tabs::-webkit-scrollbar{display:none}
.tab{border:1px solid rgba(255,255,255,.1);border-radius:20px;
  background:rgba(255,255,255,.05);color:#666;
  font-size:.72rem;padding:7px 14px;cursor:pointer;white-space:nowrap;flex-shrink:0;
  transition:all .2s;}
.tab.active{background:#7b61ff;border-color:#7b61ff;color:#fff;
  box-shadow:0 0 16px #7b61ff66;}

/* Farb-Grid */
.cgrid{display:grid;grid-template-columns:repeat(3,1fr);gap:8px;}
.cbtn{position:relative;overflow:hidden;border:none;border-radius:16px;
  padding:18px 6px 14px;font-size:.78rem;font-weight:700;cursor:pointer;
  display:flex;flex-direction:column;align-items:center;gap:6px;
  color:#fff;text-shadow:0 1px 4px rgba(0,0,0,.6);
  transition:transform .15s,box-shadow .15s;
  -webkit-tap-highlight-color:transparent;outline:none;user-select:none;}
.cbtn:active{transform:scale(.88);}
.cbtn.sending{transform:scale(.92);filter:brightness(1.3);}
.cbtn.sent::after{content:'✓';position:absolute;inset:0;
  display:flex;align-items:center;justify-content:center;
  font-size:1.8rem;background:rgba(0,0,0,.4);border-radius:16px;
  animation:fadeCheck .6s ease forwards;}
@keyframes fadeCheck{0%{opacity:0;transform:scale(.5)}40%{opacity:1;transform:scale(1.1)}100%{opacity:0;transform:scale(1)}}
.dot{width:20px;height:20px;border-radius:50%;background:rgba(255,255,255,.22);flex-shrink:0;}

/* Ripple */
.ripple{position:absolute;border-radius:50%;background:rgba(255,255,255,.35);
  transform:scale(0);animation:ripple .5s linear;}
@keyframes ripple{to{transform:scale(4);opacity:0}}

/* Effekt-Row */
.efx-row{display:flex;gap:8px;align-items:stretch;}
.efx-row select{flex:1;background:rgba(255,255,255,.06);
  border:1px solid rgba(255,255,255,.1);border-radius:12px;
  color:#ddd;font-size:.8rem;padding:11px 14px;
  appearance:none;outline:none;cursor:pointer;}
.efx-row button{border:none;border-radius:12px;
  background:linear-gradient(135deg,#7b61ff,#5540dd);
  color:#fff;font-size:.82rem;font-weight:700;padding:11px 18px;
  cursor:pointer;transition:opacity .15s,transform .15s;white-space:nowrap;}
.efx-row button:active{opacity:.75;transform:scale(.95);}

/* Sequenz */
.presets{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:12px;}
.pbtn{border:1px solid rgba(255,255,255,.1);border-radius:14px;
  background:rgba(255,255,255,.05);color:#aaa;
  font-size:.8rem;padding:13px 8px;cursor:pointer;text-align:center;
  transition:all .2s;}
.pbtn:active{transform:scale(.95);}
.pbtn.active{border-color:#7b61ff;color:#fff;background:rgba(123,97,255,.15);
  box-shadow:0 0 14px #7b61ff44;}
.slider-row{display:flex;align-items:center;gap:12px;margin-bottom:12px;}
.slider-row label{font-size:.72rem;color:#555;white-space:nowrap;}
.slider-row input[type=range]{flex:1;accent-color:#7b61ff;height:4px;}
#spd-lbl{font-size:.75rem;color:#7b61ff;min-width:34px;text-align:right;font-weight:700;}
.seq-btns{display:flex;gap:8px;}
.seq-btns button{flex:1;border:none;border-radius:14px;padding:14px;
  font-size:.88rem;font-weight:800;cursor:pointer;transition:all .2s;}
#btn-play{background:linear-gradient(135deg,#00cc66,#009944);color:#fff;
  box-shadow:0 4px 20px #00cc6644;}
#btn-play:active{transform:scale(.95);}
#btn-stop{background:rgba(255,255,255,.07);border:1px solid rgba(255,255,255,.1);color:#555;}
#btn-stop.active{background:linear-gradient(135deg,#ff4444,#cc0000);
  color:#fff;border-color:transparent;box-shadow:0 4px 20px #ff444444;}

/* Toast */
#toast{position:fixed;bottom:24px;left:50%;
  transform:translateX(-50%) translateY(80px);
  background:#1a1a2e;border:1px solid #7b61ff;
  color:#fff;padding:10px 22px;border-radius:24px;
  font-size:.8rem;font-weight:600;letter-spacing:.5px;
  transition:transform .3s cubic-bezier(.34,1.56,.64,1),opacity .3s;
  opacity:0;pointer-events:none;z-index:999;white-space:nowrap;
  box-shadow:0 8px 32px rgba(123,97,255,.3);}
#toast.show{transform:translateX(-50%) translateY(0);opacity:1;}
#toast.ok{border-color:#00cc66;box-shadow:0 8px 32px rgba(0,204,102,.25);}
#toast.err{border-color:#ff5555;box-shadow:0 8px 32px rgba(255,85,85,.25);}
#toast.seq{border-color:#ff6ec7;box-shadow:0 8px 32px rgba(255,110,199,.25);}

/* Fernbedienung */
.remote-toggle{width:100%;background:rgba(123,97,255,.08);
  border:1px solid rgba(123,97,255,.25);border-radius:16px;
  color:#9980ff;font-size:.78rem;font-weight:700;letter-spacing:1px;
  padding:14px 18px;cursor:pointer;
  display:flex;justify-content:space-between;align-items:center;
  transition:background .2s;}
.remote-toggle:active{background:rgba(123,97,255,.15);}
.remote-body{display:none;margin-top:10px;}
.remote-body.open{display:block}
.rhint{font-size:.65rem;color:#444;margin-bottom:12px;line-height:1.6;}
.rmap{display:flex;align-items:center;gap:10px;
  padding:8px 0;border-bottom:1px solid rgba(255,255,255,.04);}
.rmap:last-of-type{border-bottom:none;}
.rmap-label{flex:1;font-size:.78rem;color:#ccc;}
.rmap-sub{font-size:.62rem;color:#555;display:block;}
.rdot{width:9px;height:9px;border-radius:50%;background:#222;flex-shrink:0;
  transition:background .3s;}
.rdot.ok{background:#00cc66;box-shadow:0 0 6px #00cc66;}
.rdot.learning{background:#ffaa00;animation:pulse .9s infinite;}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.2}}
.lbtn{border:1px solid rgba(123,97,255,.3);border-radius:8px;
  background:rgba(123,97,255,.1);color:#9980ff;
  font-size:.7rem;padding:6px 10px;cursor:pointer;transition:all .2s;}
.lbtn:active{transform:scale(.92);}
.lbtn.active{border-color:#ffaa00;color:#ffaa00;background:rgba(255,170,0,.1);}
#lst{font-size:.7rem;text-align:center;padding:10px;border-radius:10px;
  background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.06);
  margin-top:10px;color:#444;}
#lst.waiting{color:#ffaa00;border-color:rgba(255,170,0,.2);}
#lst.ok{color:#00cc66;border-color:rgba(0,204,102,.2);}
#lst.err{color:#ff5555;border-color:rgba(255,85,85,.2);}
.clr-btn{width:100%;margin-top:10px;border:1px solid rgba(255,85,85,.2);
  border-radius:10px;background:rgba(255,85,85,.05);
  color:#ff5555;font-size:.72rem;padding:9px;cursor:pointer;transition:all .2s;}
.clr-btn:active{background:rgba(255,85,85,.12);}
</style>
</head>
<body>

<p class="logo">&#9733; PixMob</p>
<p class="sub">IR Party Controller</p>

<!-- FARBEN -->
<div class="card">
  <div class="card-title">Farben</div>
  <div class="tabs" id="tabs"></div>
  <div class="cgrid" id="cgrid"></div>
</div>

<!-- EFFEKT -->
<div class="card">
  <div class="card-title">Effekt kombinieren</div>
  <div class="efx-row">
    <select id="tail-sel"></select>
    <button onclick="sendCurrent()">&#9654; Senden</button>
  </div>
</div>

<!-- SEQUENZ -->
<div class="card">
  <div class="card-title">Party Sequenz</div>
  <div class="presets" id="presets"></div>
  <div class="slider-row">
    <label>Tempo</label>
    <input type="range" id="speed" min="200" max="3000" step="100" value="800"
      oninput="document.getElementById('spd-lbl').textContent=(this.value/1000).toFixed(1)+'s'">
    <span id="spd-lbl">0.8s</span>
  </div>
  <div class="seq-btns">
    <button id="btn-play" onclick="seqStart()">&#9654; Start</button>
    <button id="btn-stop" onclick="seqStop()">&#9632; Stop</button>
  </div>
</div>

<!-- FERNBEDIENUNG -->
<div style="width:100%;max-width:390px;">
  <button class="remote-toggle" onclick="toggleRemote()">
    <span>&#128246;&nbsp; Fernbedienung einlernen</span>
    <span id="rarrow">&#9660;</span>
  </button>
  <div class="remote-body" id="rbody">
    <p class="rhint">Tippe &laquo;Lernen&raquo; neben einer Farbe und dr&uuml;cke dann die entsprechende Taste auf deiner Fernbedienung. Du hast 10 Sekunden Zeit.</p>
    <div id="rlist"></div>
    <div id="lst">Kein Lernvorgang aktiv</div>
    <button class="clr-btn" onclick="clearMappings()">Alle Mappings l&ouml;schen</button>
  </div>
</div>

<!-- Toast -->
<div id="toast"></div>

<script>
const colorGroups = {
  'Rot':    [{id:'RED',l:'Rot',bg:'#cc0000'},{id:'DIM_RED',l:'Dim',bg:'#770000'},{id:'REDORANGE',l:'Rot-Or.',bg:'#bb3300'}],
  'Grün':   [{id:'GREEN',l:'Grün',bg:'#006622'},{id:'GREEN_DIM',l:'Dim',bg:'#003311'},{id:'LIGHT_GREEN',l:'Hell',bg:'#008844'},{id:'YELLOWGREEN',l:'Gelbgr.',bg:'#446600'}],
  'Blau':   [{id:'BLUE',l:'Blau',bg:'#0022bb'},{id:'LIGHT_BLUE',l:'Hell',bg:'#0055bb'},{id:'DIM_BLUE',l:'Dim',bg:'#001166'}],
  'Weiß':   [{id:'WHITISH',l:'Weiß',bg:'#777777'},{id:'WHITISH_2',l:'Weiß 2',bg:'#666666'},{id:'WHITISH_3',l:'Weiß 3',bg:'#555555'}],
  'Gelb':   [{id:'YELLOW',l:'Gelb',bg:'#aa7700'},{id:'YELLOW_3',l:'Warm',bg:'#995500'}],
  'Orange': [{id:'ORANGE',l:'Orange',bg:'#bb4400'},{id:'ORANGE_2',l:'Or. 2',bg:'#aa3300'},{id:'YELLOWORANGE',l:'Gelb-Or.',bg:'#aa6600'}],
  'Pink':   [{id:'PINK',l:'Pink',bg:'#aa0066'},{id:'PINK_2',l:'Pink 2',bg:'#880055'}],
  'Magenta':[{id:'MAGENTA',l:'Magenta',bg:'#770077'},{id:'MAGENTA_2',l:'Mag. 2',bg:'#660055'}],
  'Türkis': [{id:'TURQUOISE',l:'Türkis',bg:'#006655'},{id:'TURQUOISE_2',l:'Türk. 2',bg:'#005544'}],
};
const tailOptions=[
  {id:'NONE',l:'Kein Effekt'},{id:'FADE_1',l:'Fade 1 – Langsam'},
  {id:'FADE_2',l:'Fade 2'},{id:'FADE_3',l:'Fade 3'},{id:'FADE_4',l:'Fade 4 – Schnell'},
  {id:'FADE_5',l:'Fade 5'},{id:'FADE_6',l:'Fade 6'},
  {id:'BLINK_1',l:'Blink 1 – Selten'},{id:'BLINK_2',l:'Blink 2'},
  {id:'BLINK_3',l:'Blink 3'},{id:'BLINK_4',l:'Blink 4 – Häufig'},
  {id:'TWINKLE_1',l:'Twinkle 1'},{id:'TWINKLE_2',l:'Twinkle 2'},
  {id:'TWINKLE_3',l:'Twinkle 3'},{id:'TWINKLE_4',l:'Twinkle 4'},
];
const presetDefs=[
  {id:'rainbow',l:'🌈 Regenbogen'},{id:'warm',l:'🔥 Warm'},
  {id:'cool',l:'❄️ Kühl'},{id:'disco',l:'💃 Disco'},
  {id:'white',l:'⚪ Weiß'},
];
const remapDefs=[
  {slot:0,l:'Rot'},{slot:1,l:'Grün'},{slot:2,l:'Blau'},
  {slot:3,l:'Weiß'},{slot:4,l:'Gelb'},{slot:5,l:'Orange'},
  {slot:6,l:'Pink'},{slot:7,l:'Magenta'},{slot:8,l:'Türkis'},
  {slot:9,l:'Fade Rot'},{slot:10,l:'Fade Blau'},
  {slot:11,l:'Blink'},{slot:12,l:'Twinkle'},
];

let activeGroup=Object.keys(colorGroups)[0];
let activeColor=colorGroups[activeGroup][0].id;
let activePreset=null,learnedSlots=new Set(),learningSlot=null,pollTimer=null;
let toastTimer=null;

// Toast
function toast(msg,type='ok'){
  const t=document.getElementById('toast');
  clearTimeout(toastTimer);
  t.textContent=msg; t.className='show '+type;
  toastTimer=setTimeout(()=>t.className='',2400);
}

// Ripple
function ripple(btn,e){
  const r=document.createElement('span');
  r.className='ripple';
  const rect=btn.getBoundingClientRect();
  const size=Math.max(rect.width,rect.height);
  r.style.cssText=`width:${size}px;height:${size}px;left:${e.clientX-rect.left-size/2}px;top:${e.clientY-rect.top-size/2}px`;
  btn.appendChild(r);
  setTimeout(()=>r.remove(),600);
}

function init(){
  // Tabs
  const te=document.getElementById('tabs');
  Object.keys(colorGroups).forEach(g=>{
    const t=document.createElement('button');
    t.className='tab'+(g===activeGroup?' active':'');
    t.textContent=g; t.onclick=()=>switchGroup(g);
    te.appendChild(t);
  });
  // Tail
  const sel=document.getElementById('tail-sel');
  tailOptions.forEach(t=>{
    const o=document.createElement('option');
    o.value=t.id; o.textContent=t.l; sel.appendChild(o);
  });
  // Presets
  const pe=document.getElementById('presets');
  presetDefs.forEach(p=>{
    const b=document.createElement('button');
    b.className='pbtn'; b.id='p-'+p.id; b.textContent=p.l;
    b.onclick=()=>selectPreset(p.id); pe.appendChild(b);
  });
  renderGrid();
}

function switchGroup(g){
  activeGroup=g;
  document.querySelectorAll('.tab').forEach((t,i)=>
    t.classList.toggle('active',Object.keys(colorGroups)[i]===g));
  activeColor=colorGroups[g][0].id;
  renderGrid();
}

function renderGrid(){
  const grid=document.getElementById('cgrid');
  grid.innerHTML='';
  colorGroups[activeGroup].forEach(c=>{
    const b=document.createElement('button');
    b.className='cbtn';
    b.style.background=`linear-gradient(145deg,${c.bg}cc,${c.bg}88)`;
    b.style.boxShadow=`0 6px 20px ${c.bg}55`;
    b.innerHTML=`<span class="dot"></span>${c.l}`;
    b.addEventListener('click',e=>{
      ripple(b,e);
      activeColor=c.id;
      sendColor(c.id,b);
    });
    grid.appendChild(b);
  });
}

function sendColor(colorId, btn){
  const tail=document.getElementById('tail-sel').value;
  if(btn){btn.classList.add('sending');}
  fetch('/cmd?c='+colorId+'&t='+tail)
    .then(r=>r.ok?r.text():Promise.reject(r.status))
    .then(()=>{
      if(btn){btn.classList.remove('sending');btn.classList.add('sent');setTimeout(()=>btn.classList.remove('sent'),700);}
      toast('✓ '+colorId+(tail!=='NONE'?' + '+tail:''),'ok');
    })
    .catch(e=>{
      if(btn)btn.classList.remove('sending');
      toast('Fehler ('+e+')','err');
    });
}

function sendCurrent(){
  sendColor(activeColor,null);
}

function selectPreset(id){
  activePreset=id;
  document.querySelectorAll('.pbtn').forEach(b=>b.classList.remove('active'));
  document.getElementById('p-'+id).classList.add('active');
}

function seqStart(){
  if(!activePreset){toast('Bitte zuerst ein Preset wählen','err');return;}
  const interval=document.getElementById('speed').value;
  fetch('/seq/start?preset='+activePreset+'&interval='+interval)
    .then(()=>{
      document.getElementById('btn-stop').classList.add('active');
      toast('▶ Sequenz: '+activePreset,'seq');
    });
}

function seqStop(){
  fetch('/seq/stop').then(()=>{
    document.getElementById('btn-stop').classList.remove('active');
    toast('■ Gestoppt','ok');
  });
}

function toggleRemote(){
  const body=document.getElementById('rbody');
  const arrow=document.getElementById('rarrow');
  const open=body.classList.toggle('open');
  arrow.textContent=open?'▲':'▼';
  if(open)loadLearnStatus();
}

function renderRemote(){
  document.getElementById('rlist').innerHTML=remapDefs.map(r=>`
    <div class="rmap">
      <span class="rdot ${learnedSlots.has(r.slot)?'ok':(learningSlot===r.slot?'learning':'')}"></span>
      <span class="rmap-label">${r.l}</span>
      <button class="lbtn ${learningSlot===r.slot?'active':''}" onclick="startLearn(${r.slot})">
        ${learnedSlots.has(r.slot)?'Neu lernen':'Lernen'}
      </button>
    </div>`).join('');
}

function loadLearnStatus(){
  fetch('/learnstatus').then(r=>r.json()).then(d=>{
    learnedSlots=new Set(d.learned); renderRemote();
  }).catch(()=>renderRemote());
}

function startLearn(slot){
  learningSlot=slot; renderRemote();
  const lst=document.getElementById('lst');
  lst.className='waiting';
  lst.textContent='Fernbedienungstaste drücken … (10 s)';
  fetch('/learn?slot='+slot).catch(()=>{});
  let rem=10;
  clearInterval(pollTimer);
  pollTimer=setInterval(()=>{
    rem--;
    fetch('/learnstatus').then(r=>r.json()).then(d=>{
      learnedSlots=new Set(d.learned);
      if(!d.active){
        clearInterval(pollTimer); learningSlot=null; renderRemote();
        if(learnedSlots.has(slot)){lst.className='ok';lst.textContent='✓ Gelernt!';}
        else{lst.className='err';lst.textContent='Zeitüberschreitung.';}
      } else if(rem<=0){
        clearInterval(pollTimer); learningSlot=null; renderRemote();
        lst.className='err'; lst.textContent='Zeitüberschreitung.';
      } else {
        lst.textContent='Fernbedienungstaste drücken … ('+rem+' s)';
      }
    });
  },1000);
}

function clearMappings(){
  if(!confirm('Alle gelernten Mappings wirklich löschen?'))return;
  fetch('/clearmappings').then(()=>{
    learnedSlots=new Set(); renderRemote();
    document.getElementById('lst').className='ok';
    document.getElementById('lst').textContent='Alle Mappings gelöscht.';
    toast('Mappings gelöscht','ok');
  });
}

init();
</script>
</body>
</html>
)rawliteral";

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  eepromLoad();
  irsend.begin();
  irrecv.enableIRIn();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.printf("\n[PixMob] AP: %s | IP: %s\n", AP_SSID, WiFi.softAPIP().toString().c_str());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", HTML_PAGE);
  });

  // Farbe [+ Tail] senden
  server.on("/cmd", HTTP_GET, [](AsyncWebServerRequest* req) {
    if (!req->hasParam("c")) { req->send(400, "text/plain", "missing c"); return; }
    String c = req->getParam("c")->value();
    String t = req->hasParam("t") ? req->getParam("t")->value() : "NONE";
    c.toUpperCase(); t.toUpperCase();
    if (lookupAndSend(c, t)) req->send(200, "text/plain", "ok");
    else req->send(400, "text/plain", "unknown: " + c);
  });

  // Sequenz starten
  server.on("/seq/start", HTTP_GET, [](AsyncWebServerRequest* req) {
    String preset   = req->hasParam("preset")   ? req->getParam("preset")->value()   : "rainbow";
    uint32_t interval = req->hasParam("interval") ? req->getParam("interval")->value().toInt() : 1000;
    interval = constrain(interval, 200, 10000);
    loadPreset(preset, interval);
    seqRunning  = true;
    seqLastFire = millis() - interval; // sofort beim nächsten loop feuern
    req->send(200, "text/plain", "ok");
    Serial.printf("[SEQ] Start: %s @ %dms\n", preset.c_str(), interval);
  });

  // Sequenz stoppen
  server.on("/seq/stop", HTTP_GET, [](AsyncWebServerRequest* req) {
    seqRunning = false;
    req->send(200, "text/plain", "ok");
  });

  // Lernmodus starten (per Slot-Nummer)
  server.on("/learn", HTTP_GET, [](AsyncWebServerRequest* req) {
    if (!req->hasParam("slot")) { req->send(400, "text/plain", "missing slot"); return; }
    uint8_t slot = req->getParam("slot")->value().toInt();
    if (slot >= MAP_COUNT) { req->send(400, "text/plain", "invalid slot"); return; }
    learnTarget  = String(slot);
    learnStart   = millis();
    learnSuccess = false;
    learnActive  = true;
    req->send(200, "text/plain", "waiting");
  });

  // Lernstatus
  server.on("/learnstatus", HTTP_GET, [](AsyncWebServerRequest* req) {
    String json = "{\"active\":";
    json += learnActive ? "true" : "false";
    json += ",\"learned\":[";
    bool first = true;
    for (uint8_t i = 0; i < MAP_COUNT; i++) {
      if (mappings[i].learned) {
        if (!first) json += ",";
        json += i;
        first = false;
      }
    }
    json += "]}";
    req->send(200, "application/json", json);
  });

  // Mappings löschen
  server.on("/clearmappings", HTTP_GET, [](AsyncWebServerRequest* req) {
    eepromClear(); req->send(200, "text/plain", "ok");
  });

  server.onNotFound([](AsyncWebServerRequest* req) {
    req->send(404, "text/plain", "not found");
  });

  server.begin();
  MDNS.begin("pixmob");
  Serial.println("[PixMob] Webserver läuft — http://pixmob.local  |  http://192.168.4.1");
}

// ---------------------------------------------------------------------------
// Loop — Sequenz-Timer + IR-Empfänger
// ---------------------------------------------------------------------------
void loop() {
  MDNS.update();

  // Sequenz-Tick
  if (seqRunning && seqLen > 0 && millis() - seqLastFire >= seqInterval) {
    lookupAndSend(String(seq[seqIdx].color), String(seq[seqIdx].tail));
    seqIdx = (seqIdx + 1) % seqLen;
    seqLastFire = millis();
  }

  // Lernmodus-Timeout
  if (learnActive && millis() - learnStart > LEARN_TIMEOUT_MS) {
    learnActive = false; learnTarget = "";
    Serial.println("[LEARN] Timeout");
  }

  if (!irrecv.decode(&irData)) return;
  uint64_t code = irData.value;
  irrecv.resume();
  if (code == 0xFFFFFFFF || code == 0) return;

  Serial.printf("[RECV] 0x%llX (%s)\n", code, typeToString(irData.decode_type).c_str());

  if (learnActive) {
    uint8_t slot = learnTarget.toInt();
    mappings[slot].code    = code;
    mappings[slot].learned = true;
    learnSuccess = true;
    learnActive  = false;
    learnTarget  = "";
    eepromSave();
    Serial.printf("[LEARN] Slot %d = 0x%llX\n", slot, code);
  } else {
    for (uint8_t i = 0; i < MAP_COUNT; i++) {
      if (mappings[i].learned && mappings[i].code == code) {
        seqRunning = false; // Fernbedienung übernimmt
        lookupAndSend(String(mappings[i].color), String(mappings[i].tail));
        break;
      }
    }
  }
}
