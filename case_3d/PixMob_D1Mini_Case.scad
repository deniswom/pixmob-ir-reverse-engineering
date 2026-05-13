// =============================================================================
//  PixMob IR Controller — Gehäuse
//  Wemos D1 Mini (USB Type-C) + KY-005 IR-Sender
//
//  Zweiteiler: Tray (Unterteil) + Lid (Deckel, Reibschluss)
//  Druckbar OHNE Stützen — beide Teile flach auf Druckbett
//
//  Vorderseite (Y=0):
//    [unten] USB Type-C Buchse
//    [oben]  IR-LED Loch (5 mm LED bündig einpressen)
//
//  Kabelkanal (Rückseite Y=ol):
//    Für KY-005 Anschlussleitungen (S / VCC / GND)
//
//  Druck-Empfehlung:
//    Material : PETG oder PLA
//    Schicht  : 0.2 mm
//    Füllung  : 20 %
//    Wände    : 3
// =============================================================================

/* [1 — Platinen-Maße  D1 Mini] */
bw      = 26.7;   // Breite  [mm]
bl      = 34.2;   // Länge   [mm]
bt      = 1.6;    // PCB-Dicke
pin_h   = 2.54;   // Pin-Header Höhe unter PCB
comp_h  = 3.5;    // Bauteil-Höhe über PCB (ESP8266-Modul ≈ 3 mm)

/* [2 — USB Type-C Ausschnitt] */
usb_w   = 9.5;    // Breite  (Type-C Standard 8.94 + 0.6 Spiel)
usb_h   = 4.0;    // Höhe    (Type-C Standard 3.26 + 0.7 Spiel)

/* [3 — IR-LED Loch (Vorderwand)] */
ir_d    = 5.5;    // Durchmesser (5 mm LED-Körper + 0.5 Presspassung)

/* [4 — KY-005 Kabelkanal (Rückwand)] */
cable_w = 9.0;
cable_h = 5.5;

/* [5 — Gehäuse] */
wall    = 2.0;    // Wandstärke
floor_t = 1.5;    // Bodenstärke
gap     = 0.4;    // Spiel Board ↔ Wand (rundum)
box_r   = 2.5;    // Eckabrundungsradius

/* [6 — Deckel (Reibschluss)] */
lid_t   = 2.0;    // Deckelplatten-Stärke
rim_h   = 4.0;    // Krempen-Höhe (steckt in Tray)
rim_w   = 1.5;    // Krempen-Wandstärke
fit     = 0.20;   // Reibschluss-Spiel (reduzieren → straffer, erhöhen → lockerer)

/* [7 — Render-Qualität] */
$fn     = 48;


// =============================================================================
// Berechnete Maße (nicht ändern)
// =============================================================================
iw   = bw + 2*gap;                          // Innenbreite
il   = bl + 2*gap;                          // Innenlänge
ih   = pin_h + bt + comp_h + 1.5;          // Innenhöhe
ow   = iw + 2*wall;                         // Außenbreite
ol   = il + 2*wall;                         // Außenlänge
oh   = floor_t + ih;                        // Tray Gesamthöhe

// USB-Loch: vertikal auf Mitte des PCB-Steckerkontakts
usb_z  = floor_t + pin_h + bt/2 - usb_h/2;

// IR-LED Loch: 3 mm unter Oberkante
ir_cz  = oh - 3.0;


// =============================================================================
// Hilfsfunktion: abgerundeter Quader
// =============================================================================
module rbox(x, y, z, r = box_r) {
    hull()
        for (xi = [r, x - r], yi = [r, y - r])
            translate([xi, yi, 0])
                cylinder(r = r, h = z);
}


// =============================================================================
// TRAY — Unterteil
// =============================================================================
module tray() {
    difference() {
        // Außenkörper
        rbox(ow, ol, oh);

        // Innenraum (oben offen)
        translate([wall, wall, floor_t])
            cube([iw, il, ih + 0.1]);

        // ── USB Type-C ── Vorderwand (y = 0)
        translate([ow/2 - usb_w/2,  -0.1,  usb_z])
            cube([usb_w,  wall + 0.2,  usb_h]);

        // ── IR-LED Loch ── Vorderwand, oben
        //    Zylinder-Achse liegt in Y-Richtung durch Vorderwand
        translate([ow/2,  wall + 0.1,  ir_cz])
            rotate([90, 0, 0])
                cylinder(d = ir_d,  h = wall + 0.2);

        // ── Kabelkanal ── Rückwand (y = ol)
        translate([ow/2 - cable_w/2,  ol - wall - 0.1,  floor_t])
            cube([cable_w,  wall + 0.2,  cable_h]);
    }

    // Board-Auflagepfosten: heben PCB an, sodass Pins frei hängen
    pr = 1.8;
    ph = pin_h - 0.3;   // etwas kürzer → PCB liegt satt auf
    for (xi = [wall + gap + 3,   wall + gap + iw - 3])
        for (yi = [wall + gap + 3,   wall + gap + il - 3])
            translate([xi, yi, floor_t])
                cylinder(r = pr, h = ph);
}


// =============================================================================
// LID — Deckel
// Druckposition: Außenfläche unten (glatte Oberfläche), Krempe zeigt nach oben
// Zum Aufsetzen: umdrehen und in Tray eindrücken
// =============================================================================
module lid() {
    difference() {
        union() {
            // Deckelfläche
            rbox(ow, ol, lid_t);

            // Innenkrempe (Reibschluss — ragt in Tray-Öffnung)
            translate([wall + fit,  wall + fit,  lid_t])
                difference() {
                    cube([iw - 2*fit,  il - 2*fit,  rim_h]);
                    translate([rim_w,  rim_w,  -0.1])
                        cube([iw - 2*fit - 2*rim_w,
                              il - 2*fit - 2*rim_w,
                              rim_h + 0.2]);
                }
        }

        // Lüftungsschlitze (3 Stück, Mitte Deckel)
        for (xi = [-6, 0, 6])
            translate([ow/2 + xi - 1,  ol/2 - 8,  -0.1])
                cube([2, 16, lid_t + 0.2]);

        // Daumen-Kerbe zum Öffnen (Rückseite)
        translate([ow/2 - 9,  ol - 0.5,  -0.1])
            cube([18, 2, lid_t + 0.2]);

        // Versenktes Label-Feld (Gravur / Beschriftung)
        translate([wall + 4,  wall + 4,  lid_t - 0.4])
            cube([ow - 2*wall - 8,  ol - 2*wall - 8,  0.5]);
    }
}


// =============================================================================
// AUSGABE
// Beide Teile nebeneinander — direkt für Slicer exportieren
// =============================================================================

// ── Tray ──
color("SteelBlue", 0.95)
    tray();

// ── Deckel  (versetzt, Krempe zeigt nach oben — so drucken) ──
color("DodgerBlue", 0.95)
    translate([ow + 12,  0,  0])
        lid();


// =============================================================================
// Zusammengesetzt  (zum Prüfen — Kommentarzeichen entfernen)
// =============================================================================
// color("SteelBlue")
//     tray();
// color("DodgerBlue", 0.5)
//     translate([0, 0, oh + lid_t + rim_h])
//         rotate([180, 0, 0])
//             translate([0, -ol, 0])
//                 lid();


// =============================================================================
// MONTAGEANLEITUNG
// =============================================================================
// 1. Beide Teile drucken (keine Stützen nötig)
// 2. Wemos D1 Mini auf die Auflagepfosten setzen
//    → USB-C Buchse zeigt zur Vorderwand (y=0)
// 3. KY-005 IR-Sender:
//    a) LED-Körper von innen in das 5-mm-Loch der Vorderwand drücken
//       (Presspassung — ggf. leicht nachfeilen oder Heißkleber)
//    b) 3 Leitungen (S→D2, VCC→3V3, GND→GND) an D1 Mini anlöten
//    c) KY-005 PCB (klein, ~15×18 mm) kann im Innenraum liegen
// 4. Deckel umdrehen und eindrücken (Krempe rastet im Tray ein)
//    → Zum Öffnen: Daumen in Kerbe (Rückseite), Deckel abziehen
// 5. Optional: Micro-Kreuzschlitz-Schraube M2×8 durch Deckelmitte sichern
