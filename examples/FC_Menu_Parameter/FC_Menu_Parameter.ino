#include "crsf.h"

/*
*  Example of code how to use the library
*
*/

uint8_t Variable1 = 2;
uint8_t Variable2 = 3;
uint8_t Variable3 = 4;
uint8_t Variable4 = 5;
uint8_t Variable5 = 6;
uint8_t Variable6 = 7;


CRSF crsf;


void read(void *pvParameters) {
    while (1) {
        crsf.read_packets(0);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}



void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT); // Setzt die Onboard-LED als Ausgang
    digitalWrite(LED_BUILTIN, LOW); // LED ausschalten

    // Initialize a Serial port for comunication with receiver (HardwareSerial *serialPort, rxPin, txPin)
#ifndef CONFIG_IDF_TARGET_ESP32C3
    crsf.init_crsf(&Serial1, 16, 17);
#else
    crsf.init_crsf(&Serial1, 20, 21);
#endif
    // Create a task to read CRSF packets without blocking the main loop

#ifndef CONFIG_IDF_TARGET_ESP32C3
    xTaskCreatePinnedToCore(
        read,
        "read",
        4096,
        NULL,
        1,
        NULL,
        1
    );
#endif
}

void loop() {
#ifdef CONFIG_IDF_TARGET_ESP32C3
    crsf.read_packets(0);
#endif
    // Use the channels array as needed (0-15)
    //Serial.print("Channel 0: ");
    //Serial.println(channels[0]);
    //delay(100);


    if (crsf.getDeviceInfoReplyPending())
    {
        crsf.send_device_info("ESP32-S3-TX", 11);
    }

    if (crsf.getDeviceReadReplyPending())
    {
        //deviceReadReplyPending = false;
        handle_param_request(crsf.get_crfs_buffer(5)); //handle_param_request(crfs_buffer[5]); 
    }   
   
    if (crsf.getDeviceWriteReplyPending())
    {
        //deviceWriteReplyPending = false;
        crsf.setDeviceWriteReplyPending(false);
        //handle_param_write(crfs_buffer[5], crfs_buffer[6]);
        handle_param_write(crsf.get_crfs_buffer(5), crsf.get_crfs_buffer(6));
    }

    if (crsf.getDeviceCommandReplyPending())
    {
        crsf.setDeviceCommandReplyPending(false);

        uint8_t count = crsf.get_crfs_buffer(6);
        uint8_t swAddress = crsf.get_crfs_buffer(7);
        int16_t sw = (crsf.get_crfs_buffer(8) << 8) + crsf.get_crfs_buffer(9);

            uint8_t sw8 = 0;
            for(uint8_t k = 0; k < 8; ++k) {
                const uint8_t s = (sw >> (2 * k)) & 0b11;
                if (s > 0) {
                sw8 |= (1 << k);
                }
            }
            Serial.print("Switch set4M Adr: ");
            Serial.print(swAddress);
            Serial.print(" SW8: ");
            Serial.println(sw8);
        
    }
        
}

void handle_param_write(uint8_t id, uint8_t value) {
        Serial.println(id);
        Serial.println(value);
    switch (id) {
        case 1: Variable2 = value; break;
        case 5: Variable1 = value; break;
        case 7: Variable3 = value; break;
        case 8: Variable4 = value; break;
        case 9: Variable5 = value; break;
        case 10: Variable6 = value; break;                        
    }
}

void handle_param_request(uint8_t id) {
        switch (id) { //Parameter ?
            case 0: crsf.send_param_response_CRSF_FOLDER(id, 0, "ROOT",{1,2,3,4,5,9}); break;                       // Standart für TBS Angent dmit die Ordnerstruktur erkannt wird  mit den Parameter IDs 1,2,3,4,5 und 9                  
            case 1: crsf.send_param_response_CRSF_UINT8(id, 0, "Liter", Variable2, 0, 10, "  ml"); break;           // Variable in Ordner Root   
            case 2: crsf.send_param_response_CRSF_FOLDER(id, 0, "Ordner (ID2)", {8,9,6}); break;                    // Ordner in Ordner Root mit den Parameter IDs 8 und 9
            case 3: crsf.send_param_response_CRSF_FOLDER(id, 0, "Ordner (ID3)", {4}); break;                        // Ordner in Ordner Root mit den Parameter IDs 4
            case 4: crsf.send_param_response_CRSF_TEXT_SELECTION(id, 3, "BT Teleme 3", "Off;On", 0, 0, 1); break;   // Select in Ordner (ID3)
            case 5: crsf.send_param_response_CRSF_UINT8(id, 0, "Test Wert", Variable1, 0, 5, " kg123"); break;      // Variable in Ordner Root 
            case 6: crsf.send_param_response_CRSF_INFO(id, 2, "Hallo", "Welt"); break;                              // Infotext in Ordner Root
            case 7: crsf.send_param_response_CRSF_UINT8(id, 0, "Test 2", Variable3, 0, 11, "kg"); break;            // Variable in Ordner Root
            case 8: crsf.send_param_response_CRSF_UINT8(id, 2, "Test 3", Variable4, 0, 12, "kg"); break;            // Variable in Ordner (ID2)
            case 9: crsf.send_param_response_CRSF_UINT8(id, 2, "Test 4", Variable5, 0, 13, "kg"); break;            // Variable in Ordner (ID2)
            case 10: crsf.send_param_response_CRSF_UINT8(id, 0, "Test 5", Variable6, 0, 14, "kg"); break;           // Variable in Ordner Root
            case 11: crsf.send_param_response_CRSF_TEXT_SELECTION(id, 0, "BT Teleme 6", "Off;On", 0, 0, 1); break;  // Select in Ordner Root

            default: Serial.printf("❔ Unknown param ID %d\n", id); break;
        }
        
        //send_param_response_CRSF_UINT8(id, "Test Wert", 5, 0, 5, "kg"); 
        //send_param_response_CRSF_FLOAT(id, "Ziel", 1234, 0, 5000, 0, 2, 0.2, "°C");   
        //send_param_response_CRSF_INFO(id, "Hallo", "Welt");
        //send_param_response_CRSF_FOLDER(id, "ordner", 0); 
}


/*


23:25:09.585 -> 📤 CRSF_FRAMETYPE_COMMAND
23:25:10.674 -> RX-Buffer: C8 9 32 C8 EA A1 7 1 0 0 33

Out1 = 1
Out2 = 4
Out3 = 10
Out4 = 40

1 0 0


3.2.4. Module Commands (e.g. switch states: on/off)
Paket type: CRSF_FRAMETYPE_COMMAND, 0x32

Command realm: Module, 0xa1, (user defined realm)

Command: 0x01 (Set)

Address: 0x00 …​ 0xff

Data: variable length, 1 up to 8 bytes

Overall packet: [0xc8] [len] [0x32] [ [dst] [src] [0xa1] [0x01] <address> <byte0> …​ [byte7] ] [crc8]


    static inline void command(const auto payload, const uint8_t ) {
        const uint8_t destAddress = payload[3];
        const uint8_t srcAddress = payload[4];
        const uint8_t realm = payload[5];
        const uint8_t cmd = payload[6];
        const uint8_t address = (uint8_t)payload[7];
        if ((srcAddress == (uint8_t)RC::Protokoll::Crsf::V4::Address::Handset) && (destAddress >= 0xc0) && (destAddress <= 0xcf)) {
            if (realm == (uint8_t)RC::Protokoll::Crsf::V4::CommandType::Switch) {
                if (cmd == (uint8_t)RC::Protokoll::Crsf::V4::SwitchCommand::Set4M) {
                    uint8_t count = payload[7];
                    for(uint8_t i = 0; i < count; ++i) {
                        uint8_t swAddress = payload[8 + 3 * i];
                        uint16_t sw = (payload[9 + 3 * i] << 8) + payload[10 + 3 * i];
                        IO::outl<debug>("# Switch set4M: ", i, " adr: ", swAddress);
                        if (eeprom.address == swAddress) {
                            IO::outl<debug>("# Switch set4M adr: ", swAddress, " v: ", sw);
                            uint8_t sw8 = 0;
                            for(uint8_t k = 0; k < 8; ++k) {
                                const uint8_t s = (sw >> (2 * k)) & 0b11;
                                if (s > 0) {
                                    sw8 |= (1 << k);
                                }
                            }
                            SwitchCallback::set(sw8);
                        }
                    }
                }


*/
