#include "crsf.h"

CRSF crsf;

void setup() {

    Serial.begin(115200);

    crsf.init_crsf(&Serial1, 20, 21);
}

void loop() {
    crsf.read_packets(0);

    if (crsf.getDeviceCommandReplyPending())
    {
        // Rückmeldeflag zurücksetzen
        crsf.setDeviceCommandReplyPending(false);

        // Lese den CRSF-Befehl:
        // Byte 5 = Realm (z. B. 0xA1 für MultiSwitch)
        // Byte 6 = Command (z. B. 0x01, 0x07, 0x09, 0x02)
        uint8_t WMcode = crsf.get_crfs_buffer(5);
        uint8_t command = crsf.get_crfs_buffer(6);
        uint8_t adress = crsf.get_crfs_buffer(7);        
        uint8_t state_H = crsf.get_crfs_buffer(8);
        uint8_t state_L = crsf.get_crfs_buffer(9);

        Serial.print("Commad WM  Realm : ");
        Serial.print(WMcode, HEX);
        Serial.print(" Command: ");
        Serial.print(command, HEX);
        Serial.print(" Adresse: ");
        Serial.print(adress, HEX);        
        Serial.print(" State_H: ");
        Serial.print(state_H, HEX);
        Serial.print(" State_L: ");
        Serial.print(state_L, HEX);
        Serial.println();
    }
}