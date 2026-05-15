// =============================================================================
//  PixMob IR Controller -- Breadboard-Gehaeuse
//  Wemos D1 Mini + KY-005 IR-Sender + IR-Empfaenger auf Breadboard
//
//  Zweiteiler: Tray (Unterteil) + Lid (Deckel, Reibschluss)
//  Druckbar OHNE Stuetzen -- beide Teile flach auf Druckbett
//
//  Ausrichtung (Breadboard lange Seite = X-Achse, 85 mm):
//
//    Linke kurze Wand  (x=0):   USB-C Kabelkanal + Jumper-Kabelkanal
//    Rechte kurze Wand (x=ow):  IR-Sender LED Loch (5 mm, KY-005)
//    Lange Vorderwand  (y=0):   Daumen-Kerbe (Breadboard herausnehmen)
//    Lange Rueckwand   (y=ol):  nichts
//    Deckel (oben):             IR-Empfaenger Loch (Sensor-Dom zeigt nach oben)
//
//  Druck-Empfehlung:
//    Material : PETG oder PLA
//    Schicht  : 0.2 mm
//    Fuellung : 20 %
//    Waende   : 3
// =============================================================================

/* [1 -- Breadboard-Masse] */
bb_w    = 85.0;   // Laenge Breadboard (lange Seite, entlang X)
bb_l    = 55.0;   // Breite Breadboard (kurze Seite, entlang Y)
bb_h    = 9.5;    // Dicke Breadboard (inkl. Gummi-Unterseite)

/* [2 -- Komponenten-Hoehe ueber Breadboard] */
comp_h  = 50.0;   // Kabel-Routing-Hoehe: mind. 60 mm Gesamt (oh = 61.5 mm)

/* [3 -- IR-Sender LED Loch (rechte kurze Wand, x=ow)] */
ir_d          = 5.5;    // Durchmesser (5 mm LED + 0.5 Presspassung)
ir_z_above_bb = 10.0;   // Hoehe LED-Mitte ueber Breadboard-Oberflaeche
ir_y_offset   = 0.0;    // Y-Versatz von Mitte (+ = hinten, - = vorne)

/* [4 -- IR-Empfaenger Loch (Deckel, Sensor schaut nach oben)] */
recv_d        = 5.5;    // Durchmesser Sensor-Dom
recv_x_frac   = 0.65;   // X-Position als Anteil von ow (0=links, 1=rechts)
recv_y_frac   = 0.5;    // Y-Position als Anteil von ol (0=vorne, 1=hinten)

/* [5 -- USB-C Kabelkanal (linke kurze Wand, x=0)] */
usbc_w        = 14.0;   // Breite (USB-C Kabel + Stecker ca. 12 mm)
usbc_h        = 12.0;   // Hoehe
usbc_y_offset = 0.0;    // Y-Versatz von Mitte

/* [6 -- Jumper-Kabelkanal (linke kurze Wand, x=0, unter USB-C)] */
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

// IR-Sender: rechte kurze Wand (x=ow)
ir_z = floor_t + bb_h + ir_z_above_bb;
ir_y = ol/2 + ir_y_offset;

// USB-C: linke kurze Wand (x=0)
usbc_z = oh - usbc_h - 1.0;
usbc_y = ol/2 + usbc_y_offset - usbc_w/2;

// Jumper-Kanal: linke kurze Wand (x=0), mittig in Y
jmp_y = ol/2 - jmp_w/2;
jmp_z = floor_t + 0.5;

// IR-Empfaenger: Deckel
recv_x = recv_x_frac * ow;
recv_y = recv_y_frac * ol;


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

        // ---- RECHTE KURZE WAND (x=ow) -- IR-Sender LED Loch ----
        translate([ow - wall - 0.1, ir_y, ir_z])
            rotate([0, 90, 0])
                cylinder(d = ir_d, h = wall + 0.2);

        // ---- LINKE KURZE WAND (x=0) -- USB-C Kabelkanal ----
        translate([-0.1, usbc_y, usbc_z])
            cube([wall + 0.2, usbc_w, usbc_h + 1]);

        // ---- LINKE KURZE WAND (x=0) -- Jumper-Kabelkanal ----
        translate([-0.1, jmp_y, jmp_z])
            cube([wall + 0.2, jmp_w, jmp_h]);

        // ---- VORDERE LANGE WAND (y=0) -- Daumen-Kerbe ----
        translate([ow/2 - 12, -0.1, floor_t + 1])
            cube([24, wall + 0.2, 10]);
    }

    // ---- Breadboard-Halterung ----
    rib_h = 1.0;
    rib_t = 1.5;

    // 2 Laengsrippen (heben Breadboard leicht an, laufen entlang X)
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
// LID -- Deckel (IR-Empfaenger schaut nach oben durch das Loch)
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

        // IR-Empfaenger Loch (Sensor-Dom zeigt nach oben)
        translate([recv_x, recv_y, -0.1])
            cylinder(d = recv_d, h = lid_t + 0.2);

        // Lueftungs-Gitter (3x4 Schlitze, nicht ueber Empfaenger-Loch)
        slot_w   = 3.0;
        slot_l   = 14.0;
        slot_gap = 5.0;
        for (xi = [0:2])
            for (yi = [0:3])
                translate([ow/2 - (3*slot_w + 2*slot_gap)/2 + xi*(slot_w + slot_gap),
                           ol/2 - (4*slot_l + 3*4)/2 + yi*(slot_l + 4),
                           -0.1])
                    cube([slot_w, slot_l, lid_t + 0.2]);

        // Daumen-Kerbe (Deckel abziehen, vorne)
        translate([ow/2 - 15, -0.1, -0.1])
            cube([30, 2, lid_t + 0.2]);

        // Gravur-Vertiefung
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
//    -- lange Seite entlang X-Achse
//    -- Breadboard auf Laengsrippen legen, Clips rasten ein
//
// 3. KY-005 IR-Sender:
//    -- LED von innen durch das Loch in der rechten kurzen Wand stecken
//    -- Ggf. mit Heisskleber sichern
//
// 4. IR-Empfaenger:
//    -- Sensor-Dom von unten durch das Loch im Deckel stecken
//    -- Modul auf Breadboard positionieren und Deckel aufsetzen
//
// 5. Kabel:
//    -- USB-C durch den linken Kabelkanal (oben)
//    -- Jumper-Kabel durch den linken Kabelkanal (unten)
//
// 6. Deckel aufsetzen und eindr cken
//
echo(str("Aussenbreite ow = ", ow, " mm"));
echo(str("Aussenlaenge ol = ", ol, " mm"));
echo(str("Tray-Hoehe   oh = ", oh, " mm"));
echo(str("Gesamt-Hoehe    = ", oh + lid_t, " mm"));
