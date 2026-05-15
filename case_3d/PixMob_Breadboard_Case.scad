// =============================================================================
//  PixMob IR Controller -- Breadboard-Gehaeuse
//  Wemos D1 Mini + KY-005 IR-Sender + IR-Empfaenger auf Breadboard
//
//  Zweiteiler: Tray (Unterteil) + Lid (Deckel, Reibschluss)
//  Druckbar OHNE Stuetzen -- beide Teile flach auf Druckbett
//
//  Ausrichtung:
//    Vorderseite (Y=0)  -- KY-005 IR-Sender zeigt nach vorne
//    Rueckseite  (Y=ol) -- USB-C Kabel + IR-Empfaenger
//
//  Vorderseite:
//    [mitte] IR-Sender LED Loch (5 mm, KY-005)
//
//  Rueckseite:
//    [links]  USB-C Kabelkanal (Powerkabel D1 Mini)
//    [rechts] IR-Empfaenger Loch (5 mm, Sensor)
//    [unten]  Jumper-Kabelkanal
//
//  Druck-Empfehlung:
//    Material : PETG oder PLA
//    Schicht  : 0.2 mm
//    Fuellung : 20 %
//    Waende   : 3
// =============================================================================

/* [1 -- Breadboard-Masse] */
bb_w    = 85.0;   // Laenge Breadboard (8.5 cm, die lange Seite)
bb_l    = 55.0;   // Breite Breadboard (5.5 cm, die kurze Seite)
bb_h    = 9.5;    // Dicke Breadboard (inkl. Gummi-Unterseite)

/* [2 -- Komponenten-Hoehe ueber Breadboard] */
comp_h  = 22.0;   // D1 Mini + Pin-Header + Module ca. 20 mm + 2 mm Spiel

/* [3 -- IR-Sender LED Loch (Vorderwand)] */
ir_d          = 5.5;    // Durchmesser (5 mm LED + 0.5 Presspassung)
ir_z_above_bb = 10.0;   // Hoehe LED-Mitte ueber Breadboard-Oberflaeche
ir_x_offset   = 0.0;    // X-Versatz von Mitte (+ = rechts, - = links)

/* [4 -- IR-Empfaenger Loch (Rueckwand, rechts neben USB-C)] */
recv_d          = 5.5;  // Durchmesser Sensor-Dom
recv_z_above_bb = 10.0; // Hoehe Sensor-Mitte ueber Breadboard
recv_x_offset   = 18.0; // Abstand Empfaenger-Mitte von Gehaeuse-Mitte (rechts)

/* [5 -- USB-C Kabelkanal (Rueckwand, links von Mitte)] */
usbc_w        = 14.0;   // Breite (USB-C Kabel + Stecker ca. 12 mm)
usbc_h        = 12.0;   // Hoehe
usbc_x_offset = -18.0;  // Versatz von Gehaeuse-Mitte (negativ = links)

/* [6 -- Jumper-Kabelkanal (Rueckwand, unten)] */
jmp_w   = 40.0;   // Breite fuer mehrere Jumper-Kabel
jmp_h   = 8.0;    // Hoehe

/* [7 -- Gehaeuse] */
wall    = 2.5;    // Wandstaerke
floor_t = 2.0;    // Bodenstaerke
gap     = 0.8;    // Spiel Breadboard <-> Innenwand (rundum)
box_r   = 4.0;    // Eckabrundungsradius

/* [8 -- Deckel (Reibschluss)] */
lid_t   = 2.5;    // Deckelplatten-Staerke
rim_h   = 5.0;    // Krempen-Hoehe (steckt in Tray)
rim_w   = 2.0;    // Krempen-Wandstaerke
fit     = 0.25;   // Reibschluss-Spiel

/* [9 -- Render-Qualitaet] */
$fn     = 64;


// =============================================================================
// Berechnete Masse
// =============================================================================
iw   = bb_w + 2*gap;
il   = bb_l + 2*gap;
ih   = bb_h + comp_h;
ow   = iw + 2*wall;
ol   = il + 2*wall;
oh   = floor_t + ih;

// IR-Sender: Vorderwand
ir_z = floor_t + bb_h + ir_z_above_bb;
ir_x = ow/2 + ir_x_offset;

// IR-Empfaenger: Rueckwand, rechts
recv_z = floor_t + bb_h + recv_z_above_bb;
recv_x = ow/2 + recv_x_offset;

// USB-C: Rueckwand, links
usbc_z = oh - usbc_h - 1.0;
usbc_x = ow/2 + usbc_x_offset - usbc_w/2;

// Jumper-Kanal: Rueckwand, unten
jmp_x = ow/2 - jmp_w/2;
jmp_z = floor_t + 0.5;


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
// TRAY -- Unterteil
// =============================================================================
module tray() {
    difference() {
        // Aussenkoerper
        rbox(ow, ol, oh);

        // Innenraum (oben offen)
        translate([wall, wall, floor_t])
            cube([iw, il, ih + 0.1]);

        // ---- VORDERSEITE (y = 0) ----

        // IR-Sender LED Loch
        translate([ir_x, -0.1, ir_z])
            rotate([90, 0, 0])
                cylinder(d = ir_d, h = wall + 0.2);

        // Daumen-Kerbe Vorderseite (Breadboard herausnehmen)
        translate([ow/2 - 12, -0.1, floor_t + 1])
            cube([24, wall + 0.2, 10]);

        // ---- RUECKSEITE (y = ol) ----

        // USB-C Kabelkanal (links von Mitte)
        translate([usbc_x, ol - wall - 0.1, usbc_z])
            cube([usbc_w, wall + 0.2, usbc_h + 1]);

        // IR-Empfaenger Loch (rechts neben USB-C)
        translate([recv_x, ol - wall - 0.1, recv_z])
            rotate([90, 0, 0])
                cylinder(d = recv_d, h = wall + 0.2);

        // Jumper-Kabelkanal (unten, volle Breite fuer alle Kabel)
        translate([jmp_x, ol - wall - 0.1, jmp_z])
            cube([jmp_w, wall + 0.2, jmp_h]);
    }

    // ---- Breadboard-Halterung ----
    rib_h = 1.0;
    rib_t = 1.5;

    // 2 Laengsrippen (heben Breadboard leicht an)
    for (yi = [wall + gap/2, wall + gap + bb_l - rib_t])
        translate([wall + gap, yi, floor_t])
            cube([iw, rib_t, rib_h]);

    // Eck-Clips (halten Breadboard seitlich)
    clip_r = 1.5;
    clip_h = bb_h * 0.6;
    for (xi = [wall + gap + 8, wall + gap + bb_w - 8])
        for (yi = [wall + gap + 3, wall + gap + bb_l - 3])
            translate([xi, yi, floor_t + rib_h])
                cylinder(r = clip_r, h = clip_h);
}


// =============================================================================
// LID -- Deckel
// =============================================================================
module lid() {
    difference() {
        union() {
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

        // Lueftungs-Gitter (3x5 Schlitze)
        slot_w   = 3.0;
        slot_l   = 16.0;
        slot_gap = 5.0;
        for (xi = [0:2])
            for (yi = [0:4])
                translate([ow/2 - (3*slot_w + 2*slot_gap)/2 + xi*(slot_w + slot_gap),
                           ol/2 - (5*slot_l + 4*4)/2 + yi*(slot_l + 4),
                           -0.1])
                    cube([slot_w, slot_l, lid_t + 0.2]);

        // Daumen-Kerbe rueckseitig (Deckel abziehen)
        translate([ow/2 - 15, ol - 1.0, -0.1])
            cube([30, 2, lid_t + 0.2]);

        // Gravur-Vertiefung vorne
        translate([wall + 4, wall + 4, lid_t - 0.4])
            cube([ow - 2*wall - 8, 18, 0.4 + 0.01]);
    }
}


// =============================================================================
// AUSGABE -- Tray + Deckel nebeneinander fuer Slicer
// =============================================================================
color("SteelBlue", 0.95)
    tray();

color("DodgerBlue", 0.95)
    translate([ow + 16, 0, 0])
        lid();


// =============================================================================
// ZUSAMMENGESETZT (Kommentarzeichen entfernen zum Pruefen)
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
// 1. Tray und Deckel drucken (keine Stuetzen noetig)
//
// 2. Breadboard einsetzen:
//    -- KY-005 Seite zeigt zur Vorderwand (mit IR-LED Loch)
//    -- Breadboard auf Laengsrippen legen, Clips rasten ein
//
// 3. KY-005 IR-Sender:
//    -- LED von innen durch das Vorderwand-Loch stecken (Presspassung)
//    -- Ggf. mit Heisskleber sichern
//
// 4. IR-Empfaenger:
//    -- Sensor-Dom durch das Rueckwand-Loch (rechts) stecken
//    -- Modul so auf Breadboard positionieren dass Sensor passt
//
// 5. Kabel:
//    -- USB-C Powerkabel durch den linken Rueckwand-Kanal
//    -- Restliche Jumper durch unteren Kabelkanal
//
// 6. Deckel aufsetzen und eindrücken
//
echo(str("Aussenbreite ow = ", ow, " mm"));
echo(str("Aussenlaenge ol = ", ol, " mm"));
echo(str("Tray-Hoehe   oh = ", oh, " mm"));
echo(str("Gesamt-Hoehe    = ", oh + lid_t, " mm"));
