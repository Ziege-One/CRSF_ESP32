// Einbinden der CRSF-ESP32 Bibliothek
#include "crsf_esp32.h"

// Erzeugen eines CRSF-Objekts für die Protokollverarbeitung
CRSF crsf;

void setup() {
    // Startet die serielle Schnittstelle für Debug-Ausgaben (USB, Baudrate 115200)
    Serial.begin(115200);

    // Initialisiert CRSF mit einer zweiten seriellen Schnittstelle (Serial1)
    // Pin 20 = RX, Pin 21 = TX (ESP32-spezifisch)
    crsf.init_crsf(&Serial1, 20, 21);
}

void loop() {
    // Liest eingehende CRSF-Pakete und verarbeitet sie
    crsf.read_packets(0);

    // Ausgabe der Kanalwerte auf der Debug-Schnittstelle
    Serial.print("Channels: ");
    for (int i = 0; i < CRSF_MAX_CHANNELS; i++) {
        // Holt den Wert des i-ten Kanals aus dem CRSF-Objekt
        Serial.print(crsf.get_crfs_channels(i));
        Serial.print(" ");
    }
    // Zeilenumbruch nach der Ausgabe aller Kanäle
    Serial.println();
}