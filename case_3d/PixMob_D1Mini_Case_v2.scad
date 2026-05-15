// =============================================================================
//  PixMob IR Controller — Gehäuse v2
//  Wemos D1 Mini + KY-005 IR-Sender + IR-Empfänger
//
//  Zweiteiler: Tray (Unterteil) + Lid (Deckel, Reibschluss)
//  Druckbar OHNE Stützen — beide Teile flach auf Druckbett
//
//  Vorderseite (Y=0) von oben nach unten:
//    [oben]  IR-LED Loch (5 mm, Sender)
//    [mitte] IR-Empfänger Loch (5 mm Sensor-Dom)
//    [unten] USB Type-C Buchse
//
//  Rückseite (Y=ol):
//    Kabelkanal für 6 Jumper-Kabel (3× Sender + 3× Empfänger)
//
//  Verkabelung im Inneren:
//    KY-005 (Sender)  → PCB liegt flach im Gehäuse, LED in Vorderwand
//    IR-Receiver      → PCB liegt flach, Sensor-Dom in Vorderwand
//    Jumper-Kabel     → führen von Pin-Headern zur Rückwand heraus
//
//  Druck-Empfehlung:
//    Material : PETG oder PLA
//    Schicht  : 0.2 mm
//    Füllung  : 20 %
//    Wände    : 3
// =============================================================================

/* [1 — D1 Mini Platinen-Maße] */
bw      = 26.7;   // Breite  [mm]
bl      = 34.2;   // Länge   [mm]
bt      = 1.6;    // PCB-Dicke
pin_h   = 7.5;    // Höhe Pin-Header unter PCB (2.54mm Header + Spiel)
comp_h  = 4.0;    // Bauteil-Höhe über PCB (ESP8266-Modul ~3mm + Spiel)

/* [2 — USB Type-C Ausschnitt] */
usb_w   = 9.5;    // Breite  (Type-C Standard 8.94 + 0.6 Spiel)
usb_h   = 4.0;    // Höhe    (Type-C Standard 3.26 + 0.7 Spiel)

/* [3 — IR-Sender LED Loch (Vorderwand, oben)] */
ir_d        = 5.5;    // Durchmesser (5 mm LED + 0.5 Presspassung)
ir_from_top = 5.0;    // Abstand LED-Mitte von Oberkante

/* [4 — IR-Empfänger Loch (Vorderwand, Mitte)] */
recv_d        = 5.5;  // Durchmesser (Sensor-Dom 4.8 mm + 0.7 Spiel)
recv_from_top = 14.0; // Abstand Empfänger-Mitte von Oberkante

/* [5 — KY-005 Modul (Sender-PCB, liegt flach im Gehäuse)] */
ky_w    = 15.5;   // Breite  KY-005 PCB
ky_l    = 18.0;   // Tiefe   KY-005 PCB
ky_t    = 1.6;    // PCB-Dicke

/* [6 — IR-Empfänger Modul (PCB, liegt flach im Gehäuse)] */
rx_w    = 10.5;   // Breite  Receiver-PCB
rx_l    = 14.0;   // Tiefe   Receiver-PCB
rx_t    = 1.6;    // PCB-Dicke

/* [7 — Kabelkanal (Rückwand, für 6 Jumper-Kabel)] */
cable_w = 18.0;   // Breiter Kanal: 6× 2.54mm Jumper nebeneinander
cable_h = 6.0;

/* [8 — Gehäuse] */
wall    = 2.0;    // Wandstärke
floor_t = 1.5;    // Bodenstärke
gap     = 0.5;    // Spiel Board ↔ Wand (etwas mehr für Jumper-Kabel)
box_r   = 3.0;    // Eckabrundungsradius

/* [9 — Deckel (Reibschluss)] */
lid_t   = 2.0;    // Deckelplatten-Stärke
rim_h   = 4.0;    // Krempen-Höhe (steckt in Tray)
rim_w   = 1.5;    // Krempen-Wandstärke
fit     = 0.20;   // Reibschluss-Spiel

/* [10 — Render-Qualität] */
$fn     = 64;


// =============================================================================
// Berechnete Maße
// =============================================================================
// Innenraum muss KY-005 + Receiver-PCB + Jumper-Kabel-Routing beherbergen
// Breite: D1 Mini + etwas Spiel für Kabel auf beiden Seiten
// Länge: D1 Mini + etwas extra vorne für die beiden Module übereinander

iw   = bw + 2*gap;
il   = bl + 2*gap;

// Innenhöhe: genug für Pin-Header + PCB + Komponenten + Kabel drüber
ih   = pin_h + bt + comp_h + 2.0;

ow   = iw + 2*wall;
ol   = il + 2*wall;
oh   = floor_t + ih;

// USB-Loch: vertikal zentriert auf PCB-Steckeranschluss
usb_z  = floor_t + pin_h + bt/2 - usb_h/2;

// IR-LED Loch: ir_from_top mm von Oberkante des Tray
ir_cz  = oh - ir_from_top;

// IR-Empfänger Loch: recv_from_top mm von Oberkante
recv_cz = oh - recv_from_top;

// Trennlinie der beiden Lagen im Innenraum
// Unterlage für KY-005 PCB (direkt auf Boden)
ky_z = floor_t + 0.5;    // KY-005 liegt auf dem Boden (leicht angehoben)

// Receiver PCB liegt etwas höher (damit Sensor auf richtiger Höhe)
rx_z = floor_t + ky_t + 1.5;  // über dem KY-005


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

        // ── USB Type-C ── Vorderwand (y = 0), unten
        translate([ow/2 - usb_w/2, -0.1, usb_z])
            cube([usb_w, wall + 0.2, usb_h]);

        // ── IR-LED (Sender) ── Vorderwand, oben
        translate([ow/2, wall + 0.1, ir_cz])
            rotate([90, 0, 0])
                cylinder(d = ir_d, h = wall + 0.2);

        // ── IR-Empfänger ── Vorderwand, darunter
        translate([ow/2, wall + 0.1, recv_cz])
            rotate([90, 0, 0])
                cylinder(d = recv_d, h = wall + 0.2);

        // ── Label-Vertiefung vorne (Gravur zwischen den zwei Löchern)
        translate([ow/2 - 7, -0.01, recv_cz + recv_d/2 + 1.5])
            cube([14, 0.5, max(0.1, ir_cz - recv_cz - recv_d/2 - ir_d/2 - 3)]);

        // ── Kabelkanal ── Rückwand (y = ol), breiter für 6 Jumper
        translate([ow/2 - cable_w/2, ol - wall - 0.1, floor_t + 0.5])
            cube([cable_w, wall + 0.2, cable_h]);

        // ── Extra-Schlitz Rückwand oben ── für USB-Kabel wenn nötig
        // (auskommentiert — USB-C ist vorne)
    }

    // ── Board-Auflagepfosten ──
    // Heben D1 Mini an, sodass Pin-Header frei hängen
    pr = 1.8;
    ph = pin_h - 0.3;
    for (xi = [wall + gap + 3, wall + gap + iw - 3])
        for (yi = [wall + gap + 5, wall + gap + il - 5])
            translate([xi, yi, floor_t])
                cylinder(r = pr, h = ph);

    // ── Führungsrippen für KY-005 PCB ──
    // Hinweis: Slicer (Cura/PrusaSlicer) repariert koplanare Flächen automatisch
    // Hält das Sender-Modul seitlich an der Vorderwand
    rib_h = ky_t + 1.0;
    // Linke Rippe
    translate([wall + gap, wall, floor_t])
        cube([1.2, ky_l + 1, rib_h]);
    // Rechte Rippe
    translate([wall + gap + ky_w - 1.2, wall, floor_t])
        cube([1.2, ky_l + 1, rib_h]);

    // ── Führungsrippen für IR-Receiver PCB ── (liegt über KY-005)
    rx_offset_x = wall + gap + (iw - rx_w) / 2;
    rx_offset_y = wall + gap;
    // Rippen rechts und links des Receiver-PCB
    translate([rx_offset_x - 1.2, rx_offset_y, floor_t + ky_t + 1.0])
        cube([1.2, rx_l + 1, rx_t + 1.0]);
    translate([rx_offset_x + rx_w, rx_offset_y, floor_t + ky_t + 1.0])
        cube([1.2, rx_l + 1, rx_t + 1.0]);
}


// =============================================================================
// LID — Deckel
// Druckposition: Außenfläche unten → glatte Oberfläche oben
// =============================================================================
module lid() {
    difference() {
        union() {
            // Deckelfläche
            rbox(ow, ol, lid_t);

            // Innenkrempe (Reibschluss)
            translate([wall + fit, wall + fit, lid_t])
                difference() {
                    cube([iw - 2*fit, il - 2*fit, rim_h]);
                    translate([rim_w, rim_w, -0.1])
                        cube([iw - 2*fit - 2*rim_w,
                              il - 2*fit - 2*rim_w,
                              rim_h + 0.2]);
                }
        }

        // Lüftungsschlitze (5 Stück, Mitte Deckel)
        for (xi = [-8, -4, 0, 4, 8])
            translate([ow/2 + xi - 0.8, ol/2 - 9, -0.1])
                cube([1.6, 18, lid_t + 0.2]);

        // Daumen-Kerbe zum Öffnen (Rückseite)
        translate([ow/2 - 10, ol - 0.5, -0.1])
            cube([20, 2, lid_t + 0.2]);

        // Versenktes Label-Feld (Gravur)
        translate([wall + 3, wall + 3, lid_t - 0.4])
            cube([ow - 2*wall - 6, ol - 2*wall - 6, 0.5]);

        // PixMob-Text als Gravur (optional — auskommentiert wenn kein Font geladen)
        // translate([ow/2, ol/2, lid_t - 0.3])
        //     linear_extrude(0.4)
        //         text("PixMob", size=5, halign="center", valign="center");
    }
}


// =============================================================================
// AUSGABE — beide Teile nebeneinander für Slicer
// =============================================================================
color("SteelBlue", 0.95)
    tray();

color("DodgerBlue", 0.95)
    translate([ow + 14, 0, 0])
        lid();


// =============================================================================
// ZUSAMMENGESETZT (zum Prüfen — Kommentarzeichen entfernen)
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
//
// 2. KY-005 IR-Sender einbauen:
//    a) IR-LED von innen in das obere 5-mm-Loch der Vorderwand drücken (Presspassung)
//    b) PCB in die unteren Führungsrippen einlegen (liegt flach)
//    c) 3 Jumper-Kabel (DAT→D2/GPIO4, VCC→5V, GND) nach hinten führen
//
// 3. IR-Empfänger einbauen:
//    a) Sensor-Dom von innen in das mittlere 5-mm-Loch drücken oder kleben
//    b) PCB liegt auf den oberen Führungsrippen
//    c) 3 Jumper-Kabel (S→D5/GPIO14, VCC→3V3, GND) nach hinten führen
//
// 4. Wemos D1 Mini auf die 4 Auflagepfosten setzen:
//    → USB-C Buchse zeigt zur Vorderwand (y=0)
//    → Pin-Header hängen frei (passen zwischen Pfosten)
//    → Jumper-Kabel von KY-005 und Receiver auf passende Pins stecken
//
// 5. Alle 6 Kabel durch den hinteren Kabelkanal nach außen führen
//    (Reihenfolge: GND/GND, VCC/VCC, DAT, S)
//
// 6. Deckel umdrehen und aufdrücken (Krempe rastet ein)
//    → Öffnen: Daumen in Rückseiten-Kerbe, Deckel abziehen
//
// Maße Fertig-Gehäuse:
echo("Außenbreite  ow =", ow, "mm");
echo("Außenlänge   ol =", ol, "mm");
echo("Tray-Höhe    oh =", oh, "mm");
echo("Deckel-Dicke    =", lid_t, "mm");
echo("Gesamt-Höhe     =", oh + lid_t, "mm");
