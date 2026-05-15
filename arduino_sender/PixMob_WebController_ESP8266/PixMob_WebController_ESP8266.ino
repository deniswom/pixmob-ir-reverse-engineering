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
  } else if (name == "strobe") {
    add("RED","BLINK_4"); add("BLUE","BLINK_4"); add("GREEN","BLINK_4");
    add("YELLOW","BLINK_4"); add("MAGENTA","BLINK_4"); add("TURQUOISE","BLINK_4");
  } else if (name == "heartbeat") {
    add("RED","FADE_1"); add("DIM_RED","FADE_3"); add("RED","FADE_1"); add("DIM_RED","FADE_5");
  } else if (name == "wave") {
    add("BLUE","FADE_2"); add("LIGHT_BLUE","FADE_2"); add("TURQUOISE","FADE_2");
    add("LIGHT_GREEN","FADE_2"); add("TURQUOISE","FADE_2"); add("LIGHT_BLUE","FADE_2");
  } else if (name == "fire") {
    add("RED","FADE_3"); add("REDORANGE","FADE_2"); add("ORANGE","FADE_2");
    add("YELLOWORANGE","FADE_3"); add("ORANGE","FADE_2"); add("REDORANGE","FADE_3");
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
// Web-UI v3
// ---------------------------------------------------------------------------
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">
<title>PixMob</title>
<style>
:root{--acc:#7b61ff;--acc2:#ff61c7;--bg:#08080f;--card:rgba(255,255,255,.05);--bdr:rgba(255,255,255,.08);--tx:#e8e8f0;--sub:#555;--nh:68px}
*{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent}
body{font-family:-apple-system,'SF Pro Display',system-ui,sans-serif;background:var(--bg);color:var(--tx);min-height:100dvh;overflow-x:hidden}
.topbar{position:sticky;top:0;z-index:100;background:rgba(8,8,15,.9);backdrop-filter:blur(20px);border-bottom:1px solid var(--bdr);padding:12px 16px;display:flex;align-items:center;gap:10px}
.tmark{width:32px;height:32px;border-radius:10px;background:linear-gradient(135deg,#7b61ff,#ff61c7);display:flex;align-items:center;justify-content:center;font-size:14px;flex-shrink:0;box-shadow:0 4px 12px rgba(123,97,255,.4)}
.tinfo{flex:1;font-size:.62rem;font-weight:700;letter-spacing:.08em;color:var(--sub);text-transform:uppercase}
.apill{display:inline-flex;align-items:center;gap:6px;background:rgba(255,255,255,.06);border:1px solid var(--bdr);border-radius:20px;padding:4px 10px;font-size:.7rem;color:#bbb;transition:all .3s}
.adot{width:8px;height:8px;border-radius:50%;background:#cc0000;box-shadow:0 0 8px #cc0000;transition:background .3s,box-shadow .3s;flex-shrink:0}
.offbtn{border:1px solid rgba(255,255,255,.1);border-radius:10px;background:transparent;color:#666;font-size:.7rem;padding:6px 12px;cursor:pointer;transition:all .2s}
.offbtn:active{background:rgba(255,85,85,.15);color:#ff5555;border-color:rgba(255,85,85,.3)}
.page{display:none;padding:14px 14px calc(var(--nh) + 20px)}
.page.on{display:block}
.sh{font-size:.63rem;font-weight:700;letter-spacing:.1em;color:var(--sub);text-transform:uppercase;margin:18px 0 10px 2px}
.sh:first-child{margin-top:4px}
.gtabs{display:flex;gap:6px;overflow-x:auto;padding-bottom:4px;margin-bottom:14px;scrollbar-width:none}
.gtabs::-webkit-scrollbar{display:none}
.gtab{flex-shrink:0;border:1px solid var(--bdr);border-radius:20px;background:transparent;color:#666;font-size:.73rem;padding:6px 14px;cursor:pointer;transition:all .2s;white-space:nowrap}
.gtab.on{background:var(--acc);border-color:var(--acc);color:#fff}
.cgrid{display:grid;grid-template-columns:repeat(3,1fr);gap:10px}
.ctile{aspect-ratio:1;border-radius:16px;border:2px solid transparent;cursor:pointer;position:relative;overflow:hidden;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:5px;transition:transform .15s,border-color .15s}
.ctile:active{transform:scale(.91)}
.ctile.sel{border-color:rgba(255,255,255,.75)}
.ctile .tn{font-size:.68rem;font-weight:700;color:rgba(255,255,255,.92);text-shadow:0 1px 4px rgba(0,0,0,.8);text-align:center;padding:0 4px;line-height:1.2}
.ctile .ti{font-size:1.35rem;filter:drop-shadow(0 2px 6px rgba(0,0,0,.5))}
.ctile.flash::after{content:'';position:absolute;inset:0;background:rgba(255,255,255,.3);border-radius:14px;animation:fl .4s ease forwards}
@keyframes fl{to{opacity:0}}
.favbar{display:flex;gap:8px;overflow-x:auto;padding-bottom:4px;scrollbar-width:none;margin-bottom:2px}
.favbar::-webkit-scrollbar{display:none}
.fchip{flex-shrink:0;border-radius:20px;border:none;cursor:pointer;padding:7px 14px;font-size:.7rem;font-weight:600;color:rgba(255,255,255,.9);transition:transform .15s}
.fchip:active{transform:scale(.9)}
.efxbar{margin-top:14px;background:var(--card);border:1px solid var(--bdr);border-radius:16px;padding:14px;display:flex;align-items:center;gap:10px}
.efxbar label{font-size:.72rem;color:#777;flex-shrink:0}
.efxbar select{flex:1;background:rgba(255,255,255,.07);border:1px solid var(--bdr);border-radius:10px;color:var(--tx);font-size:.78rem;padding:8px 12px;appearance:none;-webkit-appearance:none}
.sndbtn{background:linear-gradient(135deg,var(--acc),var(--acc2));border:none;border-radius:12px;color:#fff;font-size:.82rem;font-weight:700;padding:10px 18px;cursor:pointer;box-shadow:0 4px 14px rgba(123,97,255,.4);transition:transform .15s;flex-shrink:0}
.sndbtn:active{transform:scale(.92)}
.pgrid{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:4px}
.ptile{border-radius:16px;border:1px solid var(--bdr);background:var(--card);padding:16px 14px;cursor:pointer;transition:all .2s;text-align:left;position:relative;overflow:hidden}
.ptile:active{transform:scale(.95)}
.ptile.on{border-color:rgba(123,97,255,.6);background:rgba(123,97,255,.12)}
.ptile.wide{grid-column:1/-1}
.pe{font-size:1.5rem;margin-bottom:5px}
.pn{font-size:.8rem;font-weight:700}
.pd{font-size:.63rem;color:var(--sub);margin-top:2px}
.bcard{background:var(--card);border:1px solid var(--bdr);border-radius:16px;padding:16px;margin-top:14px}
.brow{display:flex;align-items:center;gap:12px;margin-bottom:12px}
.brow label{font-size:.72rem;color:#777;flex-shrink:0}
.brow input[type=range]{flex:1;accent-color:var(--acc)}
.bval{font-size:.88rem;font-weight:700;color:var(--acc);min-width:38px;text-align:right}
.tapbtn{width:100%;padding:13px;border-radius:14px;border:2px dashed rgba(123,97,255,.4);background:rgba(123,97,255,.06);color:var(--acc);font-size:.82rem;font-weight:700;cursor:pointer;transition:all .15s;margin-bottom:12px}
.tapbtn:active{background:rgba(123,97,255,.16);transform:scale(.97)}
.sctrl{display:flex;gap:10px}
.playbtn{flex:1;border:none;border-radius:14px;background:linear-gradient(135deg,#22cc66,#11aa44);color:#fff;font-size:.88rem;font-weight:700;padding:14px;cursor:pointer;box-shadow:0 4px 14px rgba(34,204,102,.3);transition:transform .15s}
.playbtn:active{transform:scale(.95)}
.stopbtn{flex:1;border:1px solid rgba(255,85,85,.3);border-radius:14px;background:rgba(255,85,85,.08);color:#ff5555;font-size:.88rem;font-weight:700;padding:14px;cursor:pointer;transition:all .2s}
.stopbtn:active{background:rgba(255,85,85,.2)}
.spgrid{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-top:4px}
.sptile{border-radius:14px;border:1px solid var(--bdr);background:var(--card);padding:14px;cursor:pointer;transition:all .2s;text-align:center;position:relative;overflow:hidden}
.sptile:active{transform:scale(.94)}
.sptile .si{font-size:1.4rem}
.sptile .sn{font-size:.73rem;font-weight:600;margin-top:4px}
.sptile .ss{font-size:.6rem;color:var(--sub)}
.rmgrid{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.rmslot{background:var(--card);border:1px solid var(--bdr);border-radius:14px;padding:12px;display:flex;flex-direction:column;gap:8px;transition:border-color .3s,background .3s}
.rmslot.ok{border-color:rgba(0,204,102,.3);background:rgba(0,204,102,.04)}
.rmslot.lrn{border-color:rgba(255,170,0,.5);background:rgba(255,170,0,.06);animation:pb .9s infinite}
@keyframes pb{0%,100%{border-color:rgba(255,170,0,.5)}50%{border-color:rgba(255,170,0,.15)}}
.rmhdr{display:flex;align-items:center;justify-content:space-between}
.rmnm{font-size:.76rem;font-weight:600}
.rmsb{font-size:.6rem;color:var(--sub)}
.rmdot{width:8px;height:8px;border-radius:50%;background:#222;transition:background .3s}
.rmdot.ok{background:#00cc66;box-shadow:0 0 6px #00cc66}
.rmdot.lrn{background:#ffaa00;animation:bp .9s infinite}
@keyframes bp{0%,100%{opacity:1}50%{opacity:.2}}
.lbtn{width:100%;border:1px solid rgba(123,97,255,.3);border-radius:8px;background:rgba(123,97,255,.08);color:#9980ff;font-size:.68rem;padding:7px;cursor:pointer;transition:all .2s}
.lbtn:active{background:rgba(123,97,255,.2)}
.lbtn.on{border-color:#ffaa00;color:#ffaa00;background:rgba(255,170,0,.1)}
.lst{font-size:.73rem;text-align:center;padding:12px;border-radius:12px;border:1px solid var(--bdr);background:rgba(255,255,255,.03);color:var(--sub);margin-top:12px;transition:all .3s}
.lst.w{color:#ffaa00;border-color:rgba(255,170,0,.25)}
.lst.ok{color:#00cc66;border-color:rgba(0,204,102,.25)}
.lst.er{color:#ff5555;border-color:rgba(255,85,85,.25)}
.dbtn{width:100%;margin-top:14px;padding:12px;border:1px solid rgba(255,85,85,.2);border-radius:12px;background:rgba(255,85,85,.04);color:#ff5555;font-size:.73rem;cursor:pointer;transition:all .2s}
.dbtn:active{background:rgba(255,85,85,.14)}
.bnav{position:fixed;bottom:0;left:0;right:0;z-index:200;height:var(--nh);background:rgba(10,10,18,.93);backdrop-filter:blur(24px);border-top:1px solid var(--bdr);display:flex;align-items:flex-start;padding-top:8px}
.ni{flex:1;display:flex;flex-direction:column;align-items:center;gap:4px;cursor:pointer;padding:6px 4px;transition:color .2s;color:var(--sub);user-select:none}
.ni.on{color:var(--acc)}
.nicon{font-size:1.25rem}
.nlbl{font-size:.56rem;font-weight:700;letter-spacing:.05em;text-transform:uppercase}
.ndot{width:4px;height:4px;border-radius:50%;background:var(--acc);opacity:0;transition:opacity .2s;margin-top:-2px}
.ni.on .ndot{opacity:1}
.rpl{position:absolute;border-radius:50%;background:rgba(255,255,255,.22);transform:scale(0);animation:rp .5s linear;pointer-events:none}
@keyframes rp{to{transform:scale(2.8);opacity:0}}
#toast{position:fixed;bottom:calc(var(--nh) + 12px);left:50%;transform:translateX(-50%) translateY(14px);background:rgba(20,20,40,.97);border:1px solid var(--bdr);backdrop-filter:blur(16px);color:var(--tx);border-radius:20px;padding:10px 20px;font-size:.78rem;font-weight:600;opacity:0;transition:all .25s;pointer-events:none;white-space:nowrap;z-index:999}
#toast.show{opacity:1;transform:translateX(-50%) translateY(0)}
#toast.ok{border-color:rgba(0,204,102,.4);color:#00cc66}
#toast.er{border-color:rgba(255,85,85,.4);color:#ff5555}
#toast.sq{border-color:rgba(123,97,255,.4);color:#9980ff}
</style>
</head>
<body>
<div class="topbar">
  <div class="tmark">&#10022;</div>
  <div class="tinfo">PixMob Controller</div>
  <div class="apill"><div class="adot" id="adot"></div><span id="albl">Rot</span></div>
  <button class="offbtn" onclick="stopSeq()">&#9632; Stop</button>
</div>

<div class="page on" id="pg-colors">
  <div class="sh">Favoriten</div>
  <div class="favbar" id="favbar"></div>
  <div class="sh">Alle Farben</div>
  <div class="gtabs" id="gtabs"></div>
  <div class="cgrid" id="cgrid"></div>
  <div class="efxbar">
    <label>Effekt</label>
    <select id="tsel"></select>
    <button class="sndbtn" onclick="sendCurrent(this)">&#9654; Senden</button>
  </div>
</div>

<div class="page" id="pg-party">
  <div class="sh">Preset</div>
  <div class="pgrid" id="pgrid"></div>
  <div class="bcard">
    <div class="sh" style="margin-top:0">Geschwindigkeit</div>
    <div class="brow">
      <label>Interval</label>
      <input type="range" id="spd" min="200" max="3000" step="100" value="800" oninput="updSpd(this.value)">
      <span class="bval" id="spdlbl">0.8s</span>
    </div>
    <button class="tapbtn" onclick="tap()">&#128076; Tap Tempo</button>
    <div class="sctrl">
      <button class="playbtn" onclick="seqStart()">&#9654; Start</button>
      <button class="stopbtn" onclick="stopSeq()">&#9632; Stop</button>
    </div>
  </div>
  <div class="sh">Sonderfunktionen</div>
  <div class="spgrid">
    <div class="sptile" onclick="special('strobe',200)"><div class="si">&#9889;</div><div class="sn">Strobe</div><div class="ss">Alle Farben schnell</div></div>
    <div class="sptile" onclick="special('heartbeat',600)"><div class="si">&#128151;</div><div class="sn">Herzschlag</div><div class="ss">Rot pulsierend</div></div>
    <div class="sptile" onclick="special('wave',500)"><div class="si">&#127754;</div><div class="sn">Welle</div><div class="ss">Blau &#8594; T&#252;rkis</div></div>
    <div class="sptile" onclick="special('fire',400)"><div class="si">&#128293;</div><div class="sn">Feuer</div><div class="ss">Rot &#8594; Orange &#8594; Gelb</div></div>
  </div>
</div>

<div class="page" id="pg-remote">
  <div class="sh">Tasten einlernen</div>
  <p style="font-size:.7rem;color:var(--sub);margin-bottom:12px;line-height:1.6">Tippe <b style="color:#bbb">Lernen</b> und dr&#252;cke dann innerhalb von 10&thinsp;s die gew&#252;nschte Taste auf deiner Fernbedienung.</p>
  <div class="rmgrid" id="rmgrid"></div>
  <div class="lst" id="lst">Kein Lernvorgang aktiv</div>
  <button class="dbtn" onclick="clearMaps()">&#128465; Alle Mappings l&#246;schen</button>
</div>

<nav class="bnav">
  <div class="ni on" id="n-colors" onclick="nav('colors')"><div class="nicon">&#127912;</div><div class="nlbl">Farben</div><div class="ndot"></div></div>
  <div class="ni" id="n-party" onclick="nav('party')"><div class="nicon">&#127881;</div><div class="nlbl">Party</div><div class="ndot"></div></div>
  <div class="ni" id="n-remote" onclick="nav('remote')"><div class="nicon">&#128225;</div><div class="nlbl">Fernbed.</div><div class="ndot"></div></div>
</nav>
<div id="toast"></div>

<script>
const cg={
  'Rot':    [{id:'RED',l:'Rot',bg:'#cc0000',i:'&#128308;'},{id:'DIM_RED',l:'Dim Rot',bg:'#770000',i:'&#128308;'},{id:'REDORANGE',l:'Rot-Or.',bg:'#bb3300',i:'&#127805;'}],
  'Grün':   [{id:'GREEN',l:'Grün',bg:'#006622',i:'&#128994;'},{id:'GREEN_DIM',l:'Dim',bg:'#003311',i:'&#128994;'},{id:'LIGHT_GREEN',l:'Hell',bg:'#009944',i:'&#128154;'},{id:'YELLOWGREEN',l:'Gelbgr.',bg:'#446600',i:'&#127807;'}],
  'Blau':   [{id:'BLUE',l:'Blau',bg:'#0022bb',i:'&#128309;'},{id:'LIGHT_BLUE',l:'Hell',bg:'#0055bb',i:'&#129689;'},{id:'DIM_BLUE',l:'Dim',bg:'#001166',i:'&#128309;'}],
  'Weiß':   [{id:'WHITISH',l:'Weiß',bg:'#555555',i:'&#9898;'},{id:'WHITISH_2',l:'Weiß 2',bg:'#444444',i:'&#9898;'},{id:'WHITISH_3',l:'Weiß 3',bg:'#383838',i:'&#9898;'}],
  'Gelb':   [{id:'YELLOW',l:'Gelb',bg:'#aa7700',i:'&#128993;'},{id:'YELLOW_3',l:'Warm',bg:'#995500',i:'&#127765;'}],
  'Orange': [{id:'ORANGE',l:'Orange',bg:'#bb4400',i:'&#127818;'},{id:'ORANGE_2',l:'Or. 2',bg:'#aa3300',i:'&#127818;'},{id:'YELLOWORANGE',l:'Gelb-Or.',bg:'#aa6600',i:'&#128993;'}],
  'Pink':   [{id:'PINK',l:'Pink',bg:'#aa0066',i:'&#129321;'},{id:'PINK_2',l:'Pink 2',bg:'#880055',i:'&#128151;'}],
  'Magenta':[{id:'MAGENTA',l:'Magenta',bg:'#770077',i:'&#128156;'},{id:'MAGENTA_2',l:'Mag. 2',bg:'#660055',i:'&#128156;'}],
  'Türkis': [{id:'TURQUOISE',l:'Türkis',bg:'#006655',i:'&#129689;'},{id:'TURQUOISE_2',l:'Türk. 2',bg:'#005544',i:'&#129689;'}]
};
const tails=[
  {id:'NONE',l:'&#8212; Kein Effekt'},
  {id:'FADE_1',l:'Fade 1 &#8211; Sanft'},{id:'FADE_2',l:'Fade 2'},{id:'FADE_3',l:'Fade 3'},
  {id:'FADE_4',l:'Fade 4 &#8211; Schnell'},{id:'FADE_5',l:'Fade 5'},{id:'FADE_6',l:'Fade 6'},
  {id:'BLINK_1',l:'Blink 1 &#8211; Selten'},{id:'BLINK_2',l:'Blink 2'},{id:'BLINK_3',l:'Blink 3'},{id:'BLINK_4',l:'Blink 4 &#8211; Häufig'},
  {id:'TWINKLE_1',l:'Twinkle 1'},{id:'TWINKLE_2',l:'Twinkle 2'},{id:'TWINKLE_3',l:'Twinkle 3'},{id:'TWINKLE_4',l:'Twinkle 4'}
];
const presets=[
  {id:'rainbow',e:'&#127752;',n:'Regenbogen',d:'Alle Farben rotieren'},
  {id:'warm',   e:'&#128293;',n:'Warm',       d:'Rot · Orange · Gelb'},
  {id:'cool',   e:'&#10052;', n:'Kühl',  d:'Blau · Türkis · Weiß'},
  {id:'disco',  e:'&#128131;',n:'Disco',      d:'Schneller Farbwechsel'},
  {id:'white',  e:'&#9898;',  n:'Weiß',  d:'Alle Weiß-Töne',w:1}
];
const rmDefs=[
  {slot:0,l:'Rot',s:'Basis'},{slot:1,l:'Grün',s:'Basis'},{slot:2,l:'Blau',s:'Basis'},
  {slot:3,l:'Weiß',s:'Basis'},{slot:4,l:'Gelb',s:'Basis'},{slot:5,l:'Orange',s:'Basis'},
  {slot:6,l:'Pink',s:'Basis'},{slot:7,l:'Magenta',s:'Basis'},{slot:8,l:'Türkis',s:'Basis'},
  {slot:9,l:'Fade Rot',s:'Effekt'},{slot:10,l:'Fade Blau',s:'Effekt'},
  {slot:11,l:'Blink',s:'Effekt'},{slot:12,l:'Twinkle',s:'Effekt'}
];
const favIds=['RED','GREEN','BLUE','WHITISH','YELLOW','PINK'];
let actGrp=Object.keys(cg)[0], actCol=cg[Object.keys(cg)[0]][0];
let actPreset=null, learned=new Set(), lrnSlot=null, pollTm=null, toastTm=null;
let tapTs=[], lastTap=0;

function nav(p){
  document.querySelectorAll('.page').forEach(e=>e.classList.remove('on'));
  document.querySelectorAll('.ni').forEach(e=>e.classList.remove('on'));
  document.getElementById('pg-'+p).classList.add('on');
  document.getElementById('n-'+p).classList.add('on');
  if(p==='remote') loadLearnStatus();
}
function toast(msg,t='ok'){
  const el=document.getElementById('toast');
  clearTimeout(toastTm);
  el.textContent=msg; el.className='show '+t;
  toastTm=setTimeout(()=>el.className='',2400);
}
function ripple(el,e){
  const r=document.createElement('span'); r.className='rpl';
  const rc=el.getBoundingClientRect(), sz=Math.max(rc.width,rc.height);
  r.style.cssText='width:'+sz+'px;height:'+sz+'px;left:'+(e.clientX-rc.left-sz/2)+'px;top:'+(e.clientY-rc.top-sz/2)+'px';
  el.style.position='relative'; el.style.overflow='hidden';
  el.appendChild(r); setTimeout(()=>r.remove(),500);
}
document.addEventListener('click',e=>{
  const h=e.target.closest('.sndbtn,.playbtn,.stopbtn,.tapbtn,.sptile,.ctile,.fchip,.ptile,.lbtn,.dbtn,.offbtn');
  if(h) ripple(h,e);
});
function updStatus(){
  const dot=document.getElementById('adot');
  dot.style.background=actCol.bg; dot.style.boxShadow='0 0 8px '+actCol.bg;
  const t=document.getElementById('tsel')?.value;
  document.getElementById('albl').textContent=actCol.l+(t&&t!=='NONE'?' · '+t.replace('_',' '):'');
}
function init(){
  const gt=document.getElementById('gtabs');
  Object.keys(cg).forEach(g=>{
    const b=document.createElement('button'); b.className='gtab'+(g===actGrp?' on':'');
    b.textContent=g; b.onclick=()=>swGrp(g); gt.appendChild(b);
  });
  const fb=document.getElementById('favbar');
  favIds.forEach(id=>{
    const col=Object.values(cg).flat().find(c=>c.id===id); if(!col) return;
    const b=document.createElement('button'); b.className='fchip';
    b.style.background='linear-gradient(135deg,'+col.bg+'ee,'+col.bg+'99)';
    b.innerHTML=col.i+' '+col.l; b.onclick=()=>tapCol(col); fb.appendChild(b);
  });
  const sel=document.getElementById('tsel');
  tails.forEach(t=>{const o=document.createElement('option');o.value=t.id;o.innerHTML=t.l;sel.appendChild(o);});
  sel.addEventListener('change',updStatus);
  const pg=document.getElementById('pgrid');
  presets.forEach(p=>{
    const d=document.createElement('div'); d.className='ptile'+(p.w?' wide':''); d.id='p-'+p.id;
    d.innerHTML='<div class="pe">'+p.e+'</div><div class="pn">'+p.n+'</div><div class="pd">'+p.d+'</div>';
    d.onclick=()=>selPreset(p.id); pg.appendChild(d);
  });
  renderGrid(); updStatus();
}
function swGrp(g){
  actGrp=g;
  document.querySelectorAll('.gtab').forEach((t,i)=>t.classList.toggle('on',Object.keys(cg)[i]===g));
  renderGrid();
}
function renderGrid(){
  const grid=document.getElementById('cgrid'); grid.innerHTML='';
  cg[actGrp].forEach(c=>{
    const b=document.createElement('div');
    b.className='ctile'+(c.id===actCol.id?' sel':'');
    b.style.background='linear-gradient(145deg,'+c.bg+'ee,'+c.bg+'88)';
    b.style.boxShadow='0 6px 20px '+c.bg+'55';
    b.innerHTML='<div class="ti">'+c.i+'</div><div class="tn">'+c.l+'</div>';
    b.onclick=()=>tapCol(c); grid.appendChild(b);
  });
}
function tapCol(c){
  actCol=c;
  const grp=Object.keys(cg).find(g=>cg[g].some(col=>col.id===c.id));
  if(grp) swGrp(grp); else renderGrid();
  sendColor(c);
}
function sendColor(c){
  const t=document.getElementById('tsel').value; updStatus();
  fetch('/cmd?c='+c.id+'&t='+t)
    .then(r=>r.ok?r.text():Promise.reject(r.status))
    .then(()=>toast('✓ '+c.l+(t!=='NONE'?' + '+t:''),'ok'))
    .catch(err=>toast('Fehler ('+err+')','er'));
}
function sendCurrent(){sendColor(actCol);}
function selPreset(id){
  actPreset=id;
  document.querySelectorAll('.ptile').forEach(b=>b.classList.remove('on'));
  document.getElementById('p-'+id).classList.add('on');
}
function updSpd(v){document.getElementById('spdlbl').textContent=(v/1000).toFixed(1)+'s'; tapTs=[];}
function tap(){
  const now=Date.now();
  if(lastTap&&now-lastTap<3000){
    tapTs.push(now-lastTap); if(tapTs.length>4) tapTs.shift();
    const avg=tapTs.reduce((a,b)=>a+b,0)/tapTs.length;
    const v=Math.max(200,Math.min(3000,Math.round(avg/100)*100));
    document.getElementById('spd').value=v; updSpd(v);
    toast('⏱ '+(v/1000).toFixed(1)+'s','ok');
  } else {tapTs=[]; toast('👆 Weiter tippen…','ok');}
  lastTap=now;
}
function seqStart(){
  if(!actPreset){toast('Erst Preset wählen','er');return;}
  const iv=document.getElementById('spd').value;
  fetch('/seq/start?preset='+actPreset+'&interval='+iv)
    .then(()=>toast('▶ '+presets.find(p=>p.id===actPreset).n+' läuft','sq'))
    .catch(err=>toast('Fehler ('+err+')','er'));
}
function stopSeq(){
  fetch('/seq/stop').then(()=>toast('■ Gestoppt','ok')).catch(()=>{});
}
function special(id,iv){
  const names={strobe:'⚡ Strobe',heartbeat:'💗 Herzschlag',wave:'🌊 Welle',fire:'🔥 Feuer'};
  fetch('/seq/start?preset='+id+'&interval='+iv)
    .then(()=>toast(names[id]+' aktiv','sq'))
    .catch(err=>toast('Fehler ('+err+')','er'));
}
function renderRemote(){
  const g=document.getElementById('rmgrid'); g.innerHTML='';
  rmDefs.forEach(r=>{
    const isOk=learned.has(r.slot), isLrn=lrnSlot===r.slot;
    const d=document.createElement('div');
    d.className='rmslot'+(isOk?' ok':'')+(isLrn?' lrn':'');
    d.innerHTML='<div class="rmhdr"><div><div class="rmnm">'+r.l+'</div><div class="rmsb">'+r.s+'</div></div>'
      +'<div class="rmdot '+(isOk?'ok':isLrn?'lrn':'')+'"></div></div>'
      +'<button class="lbtn'+(isLrn?' on':'') +'" onclick="startLearn('+r.slot+')">'
      +(isLrn?'⏳ Warten…':isOk?'↺ Neu lernen':'Lernen')+'</button>';
    g.appendChild(d);
  });
}
function loadLearnStatus(){
  fetch('/learnstatus').then(r=>r.json()).then(d=>{learned=new Set(d.learned);renderRemote();}).catch(()=>renderRemote());
}
function startLearn(slot){
  lrnSlot=slot; renderRemote();
  const st=document.getElementById('lst');
  st.className='lst w'; st.textContent='Fernbedienungstaste drücken … (10 s)';
  fetch('/learn?slot='+slot).catch(()=>{});
  let rem=10; clearInterval(pollTm);
  pollTm=setInterval(()=>{
    rem--;
    fetch('/learnstatus').then(r=>r.json()).then(d=>{
      learned=new Set(d.learned);
      if(!d.active){
        clearInterval(pollTm); lrnSlot=null; renderRemote();
        if(learned.has(slot)){st.className='lst ok';st.textContent='✓ Gelernt!';}
        else{st.className='lst er';st.textContent='Überschreitung.';}
      } else if(rem<=0){
        clearInterval(pollTm); lrnSlot=null; renderRemote();
        st.className='lst er'; st.textContent='Überschreitung.';
      } else {
        st.textContent='Fernbedienungstaste drücken … ('+rem+' s)';
      }
    });
  },1000);
}
function clearMaps(){
  fetch('/clearmappings').then(()=>{
    learned=new Set(); lrnSlot=null; renderRemote();
    const st=document.getElementById('lst');
    st.className='lst ok'; st.textContent='Alle Mappings gelöscht.';
    toast('Mappings gelöscht','ok');
  }).catch(err=>toast('Fehler ('+err+')','er'));
}
init();
</script>
</body>
</html>
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
