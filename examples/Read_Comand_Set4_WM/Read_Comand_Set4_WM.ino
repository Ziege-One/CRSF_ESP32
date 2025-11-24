#include "crsf_esp32.h"   // Einbinden der CRSF-Bibliothek für das Crossfire-Protokoll

// Erzeugen eines CRSF-Objekts zur Verarbeitung eingehender Pakete
CRSF crsf;

void setup() {
    Serial.begin(115200);               // Startet die serielle Schnittstelle für Debug-Ausgaben

    // Initialisiert CRSF mit einer Hardware-Serial-Schnittstelle
    // Übergabe: &Serial1 = zweite UART, RX=20, TX=21 (ESP32-C3 spezifisch)
    crsf.init_crsf(&Serial1, 20, 21);
}

void loop() {
    // Liest eingehende CRSF-Pakete und aktualisiert interne Buffer
    crsf.read_packets(0);

    // Prüft, ob ein Gerätekommando zur Verarbeitung ansteht
    if (crsf.getDeviceCommandReplyPending())
    {
        // Rückmeldeflag zurücksetzen, damit es nicht mehrfach verarbeitet wird
        crsf.setDeviceCommandReplyPending(false);

        // Auslesen der relevanten Bytes aus dem CRSF-Puffer:
        // Byte 5 = Realm (z. B. 0xA1 für MultiSwitch)
        // Byte 6 = Command (z. B. 0x01, 0x07, 0x09, 0x02)
        // Byte 7 = Adresse (z. B. Switch-Adresse)
        // Byte 8 = High-Byte des Zustands
        // Byte 9 = Low-Byte des Zustands
        uint8_t WMcode   = crsf.get_crfs_buffer(5);
        uint8_t command  = crsf.get_crfs_buffer(6);
        uint8_t adress   = crsf.get_crfs_buffer(7);
        uint8_t state_H  = crsf.get_crfs_buffer(8);
        uint8_t state_L  = crsf.get_crfs_buffer(9);

        // Ausgabe der eingelesenen Werte im Serial Monitor
        Serial.print("Command WM Realm : ");
        Serial.print(WMcode, HEX);       // Realm-Code in Hex
        Serial.print(" Command: ");
        Serial.print(command, HEX);      // Kommando in Hex
        Serial.print(" Adresse: ");
        Serial.print(adress, HEX);       // Adresse in Hex
        Serial.print(" State_H: ");
        Serial.print(state_H, HEX);      // High-Byte des Zustands
        Serial.print(" State_L: ");
        Serial.print(state_L, HEX);      // Low-Byte des Zustands
        Serial.println();                // Zeilenumbruch
    }
}