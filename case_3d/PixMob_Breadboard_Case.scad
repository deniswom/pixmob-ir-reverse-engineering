// =============================================================================
//  PixMob IR Controller — Breadboard-Gehäuse
//  Wemos D1 Mini + KY-005 IR-Sender + IR-Empfänger auf Breadboard
//
//  Zweiteiler: Tray (Unterteil) + Lid (Deckel, Reibschluss)
//  Druckbar OHNE Stützen — beide Teile flach auf Druckbett
//
//  Ausrichtung im Gehäuse:
//    Vorderseite (Y=0) → KY-005 Seite (IR-LED zeigt nach vorne)
//    Rückseite (Y=ol)  → D1 Mini USB-C Seite
//    Linke Wand (X=0)  → IR-Empfänger Seite
//
//  Vorderseite hat:
//    • IR-Sender LED Loch (5 mm, Presspassung KY-005 LED)
//
//  Linke Wand hat:
//    • IR-Empfänger Loch (5 mm, Sensor zeigt nach links)
//
//  Rückseite hat:
//    • USB-C Kabelkanal (für Powerkabel vom D1 Mini)
//    • Breiter Jumper-Kabelkanal (falls externe Kabel nötig)
//
//  Druck-Empfehlung:
//    Material : PETG oder PLA
//    Schicht  : 0.2 mm
//    Füllung  : 20 %
//    Wände    : 3
// =============================================================================

/* [1 — Breadboard-Maße] */
bb_w    = 85.0;   // Länge Breadboard (die lange Seite, 8.5 cm)
bb_l    = 55.0;   // Breite Breadboard (die kurze Seite, 5.5 cm)
bb_h    = 9.5;    // Dicke Breadboard (inkl. Gummi-Unterseite)

/* [2 — Komponenten-Höhe über Breadboard] */
comp_h  = 22.0;   // D1 Mini + Pin-Header + Module ≈ 20 mm, + 2 mm Spiel

/* [3 — IR-Sender LED Loch (Vorderwand, KY-005 Seite)] */
ir_d       = 5.5;   // Durchmesser (5 mm LED + 0.5 Presspassung)
// Höhe: Breadboard + ca. 10 mm (KY-005 LED sitzt ca. 10 mm über Breadboard)
ir_z_above_bb = 10.0;

/* [4 — IR-Empfänger Loch (linke Seitenwand)] */
recv_d        = 5.5;  // Durchmesser Sensor-Dom
// Position entlang Y (von Vorderseite): Empfänger ist ca. 2/3 nach hinten
recv_y_pos    = 20.0; // Abstand Loch-Mitte von Vorderwand-Innen
// Höhe: ähnlich wie Sender
recv_z_above_bb = 10.0;

/* [5 — USB-C Kabelkanal (Rückwand, oben)] */
usbc_w  = 14.0;   // Breite (USB-C Kabel inkl. Stecker ≈ 12 mm)
usbc_h  = 10.0;   // Höhe

/* [6 — Jumper-Kabelkanal (Rückwand, unten)] */
jmp_w   = 30.0;   // Breite für mehrere Jumper-Kabel

/* [7 — Gehäuse-Wand] */
wall    = 2.5;    // Wandstärke (etwas dicker für Stabilität)
floor_t = 2.0;    // Bodenstärke
gap     = 0.8;    // Spiel Breadboard ↔ Innenwand (rundum)
box_r   = 4.0;    // Eckabrundungsradius

/* [8 — Deckel (Reibschluss)] */
lid_t   = 2.5;    // Deckelplatten-Stärke
rim_h   = 5.0;    // Krempen-Höhe
rim_w   = 2.0;    // Krempen-Wandstärke
fit     = 0.25;   // Reibschluss-Spiel

/* [9 — Render-Qualität] */
$fn     = 64;


// =============================================================================
// Berechnete Maße
// =============================================================================
iw   = bb_w + 2*gap;          // Innenbreite  (Breadboard-Länge + Spiel)
il   = bb_l + 2*gap;          // Innenlänge   (Breadboard-Breite + Spiel)
ih   = bb_h + comp_h;         // Innenhöhe
ow   = iw + 2*wall;           // Außenbreite
ol   = il + 2*wall;           // Außenlänge
oh   = floor_t + ih;          // Tray-Höhe

// IR-Sender Loch: Vorderwand, Höhe = Boden + Breadboard + Versatz
ir_z     = floor_t + bb_h + ir_z_above_bb;
// IR-Sender Loch X-Position: links von Mitte (KY-005 ist links auf Breadboard)
ir_x     = ow / 2 - 8;

// IR-Empfänger Loch: linke Wand
recv_z   = floor_t + bb_h + recv_z_above_bb;
recv_y   = wall + recv_y_pos;

// USB-C Kabelkanal: Rückwand, oben mittig (D1 Mini ist ca. mittig)
usbc_z   = oh - usbc_h - 1;
usbc_x   = ow/2 - usbc_w/2;

// Jumper-Kanal: Rückwand, unten
jmp_z    = floor_t + 1;
jmp_x    = ow/2 - jmp_w/2;


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

        // ── IR-Sender LED ── Vorderwand (y = 0)
        translate([ir_x, -0.1, ir_z])
            rotate([90, 0, 0])
                cylinder(d = ir_d, h = wall + 0.2);

        // ── IR-Empfänger ── Linke Wand (x = 0)
        translate([-0.1, recv_y, recv_z])
            rotate([0, 90, 0])
                cylinder(d = recv_d, h = wall + 0.2);

        // ── USB-C Kabelkanal ── Rückwand (y = ol), oben
        translate([usbc_x, ol - wall - 0.1, usbc_z])
            cube([usbc_w, wall + 0.2, usbc_h + 1]);

        // ── Jumper-Kabelkanal ── Rückwand, unten
        translate([jmp_x, ol - wall - 0.1, jmp_z])
            cube([jmp_w, wall + 0.2, 8.0]);

        // ── Daumen-Kerbe Vorderseite (zum Herausnehmen Breadboard)
        translate([ow/2 - 12, -0.1, floor_t + bb_h/2 - 5])
            cube([24, wall + 0.2, 10]);
    }

    // ── Breadboard-Auflagerippen ──
    // Heben Breadboard 0.5mm an (Luft unter Gummistreifen)
    // und halten es formschlüssig
    rib_h  = 1.0;
    rib_t  = 1.5;

    // 2 Längsrippen (parallel zur langen Seite)
    for (yi = [wall + gap/2, wall + gap + bb_l - gap/2 - rib_t])
        translate([wall + gap, yi, floor_t])
            cube([iw - gap, rib_t, rib_h]);

    // Clips: kleine Nasen die Breadboard leicht klemmen
    // (gedruckt flexibel genug zum Einrasten)
    clip_h = bb_h - 0.5;
    clip_d = 1.2;
    for (xi = [wall + gap + 10, wall + gap + bb_w - 10])
        for (yi = [wall + gap, wall + gap + bb_l])
            translate([xi - clip_d/2, yi - clip_d/2, floor_t + rib_h])
                cylinder(d = clip_d * 2, h = clip_h - rib_h);
}


// =============================================================================
// LID — Deckel
// =============================================================================
module lid() {
    difference() {
        union() {
            rbox(ow, ol, lid_t);

            // Innenkrempe
            translate([wall + fit, wall + fit, lid_t])
                difference() {
                    cube([iw - 2*fit, il - 2*fit, rim_h]);
                    translate([rim_w, rim_w, -0.1])
                        cube([iw - 2*fit - 2*rim_w,
                              il - 2*fit - 2*rim_w,
                              rim_h + 0.2]);
                }
        }

        // Lüftungs-Gitter (3×5 Rechteck-Slots)
        slot_w  = 3.0;
        slot_l  = 14.0;
        slot_gap = 4.0;
        start_x = ow/2 - (3 * slot_w + 2 * slot_gap)/2;
        start_y = ol/2 - (5 * slot_l + 4 * 3)/2;  // 4 gaps of 3mm
        for (xi = [0:2])
            for (yi = [0:4])
                translate([start_x + xi*(slot_w + slot_gap),
                           start_y + yi*(slot_l + 3),
                           -0.1])
                    cube([slot_w, slot_l, lid_t + 0.2]);

        // Daumen-Kerbe rückseitig öffnen
        translate([ow/2 - 15, ol - 1, -0.1])
            cube([30, 2, lid_t + 0.2]);

        // Gravur-Vertiefung vorne
        translate([wall + 4, wall + 4, lid_t - 0.5])
            cube([ow - 2*wall - 8, 20, 0.5 + 0.01]);
    }
}


// =============================================================================
// AUSGABE — beide Teile nebeneinander
// =============================================================================
color("SteelBlue", 0.95)
    tray();

color("DodgerBlue", 0.95)
    translate([ow + 16, 0, 0])
        lid();


// =============================================================================
// ZUSAMMENGESETZT (Kommentarzeichen entfernen zum Prüfen)
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
// 1. Tray und Deckel drucken (keine Stützen nötig)
//
// 2. Breadboard einsetzen:
//    → Breadboard mit KY-005-Seite zur Vorderwand (mit IR-LED Loch) ausrichten
//    → Breadboard auf die Längsrippen legen und einrasten
//
// 3. KY-005 IR-Sender:
//    → LED-Körper durch das 5-mm-Loch in der Vorderwand nach außen führen
//    → Ggf. mit etwas Heißkleber sichern
//
// 4. IR-Empfänger:
//    → Sensor durch das 5-mm-Loch in der linken Seitenwand führen
//    → Module so auf Breadboard positionieren dass Sensor bündig ist
//
// 5. Kabel:
//    → USB-C Powerkabel durch den oberen Rückkanal führen
//    → Übrige Jumper ggf. durch unteren Kabelkanal
//
// 6. Deckel aufsetzen (Krempe eindrücken)
//
echo("Außenbreite  ow =", ow, "mm");
echo("Außenlänge   ol =", ol, "mm");
echo("Tray-Höhe    oh =", oh, "mm");
echo("Gesamt-Höhe     =", oh + lid_t, "mm");
